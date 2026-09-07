#include "stdafx.h"
#ifdef __ENABLE_BIOLOG_SYSTEM__
#include "BiologSystemManager.hpp"
#include "questmanager.h"
#include "char.h"
#include "packet.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "char_manager.h"
#include "p2p.h"
#include "item_manager.h"
#include "item.h"
#include "utils.h"

#include "mob_manager.h"

#include "EventFunctionHandler.hpp"
#include <cstdarg>

//#define __DEBUG_PRINT
#ifdef __DEBUG_PRINT
	#include <iostream>
	using std::cerr;
	using std::endl;
#endif

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CBiologSystem - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

CBiologSystem::CBiologSystem(LPCHARACTER m_ch)
	: m_pkChar(m_ch)
{
	char buf[64];
	snprintf(buf, sizeof(buf), "BIOLOG_MANAGER_ALERT_%u", m_ch->GetPlayerID());
	s_current_biolog_reminder = buf;

	if (m_pkChar->GetBiologCooldownReminder())
	{
		this->ActiveAlert(true);
	}
}

CBiologSystem::~CBiologSystem()
{
}

void CBiologSystem::OnDestroy()
{
	CEventFunctionHandler::Instance().RemoveEvent(s_current_biolog_reminder);
	m_pkChar = nullptr;
}

/*******************************************************************\
| [PUBLIC] Update Functions
\*******************************************************************/

void CBiologSystem::ResetMission()
{
	m_pkChar->SetBiologCollectedItems(0);
	m_pkChar->SetBiologCooldown(0);
	ActiveAlert(false);
}

/*******************************************************************\
| [PUBLIC] General Functions
\*******************************************************************/

void CBiologSystem::SendBiologInformation(bool bUpdate)
{
	if (!m_pkChar)
	{
		return;
	}

	auto bMission = m_pkChar->GetBiologMissions();
	auto iCount = m_pkChar->GetBiologCollectedItems();
	auto tWait = m_pkChar->GetBiologCooldown();
	auto tReminder = m_pkChar->GetBiologCooldownReminder();

	TBiologMissionsProto* pMission = CBiologSystemManager::instance().GetMission(bMission);
	if (!pMission)
	{
#ifdef __DEBUG_PRINT
		sys_err("SendBiologInformation -> cannot get mission %u by %s", bMission, m_pkChar->GetName());
#endif
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, "You have completed all missions!");
		return;
	}

	TBiologRewardsProto* pReward = CBiologSystemManager::instance().GetReward(bMission);
	if (!pReward)
	{
#ifdef __DEBUG_PRINT
		sys_err("SendBiologInformation -> cannot get reward %u by %s", bMission, m_pkChar->GetName());
#endif
		return;
	}

	TPacketGCBiologManagerInfo kInfo;
	memset(&kInfo, 0, sizeof(kInfo));

	//copy data
	kInfo.bUpdate = bUpdate;
	kInfo.bRequiredLevel = pMission->bRequiredLevel;
	kInfo.iRequiredItem = pMission->iRequiredItem;
	kInfo.wGivenItems = iCount;
	kInfo.wRequiredItemCount = pMission->wRequiredItemCount;
	kInfo.iGlobalCooldown = pMission->iCooldown;
	kInfo.iCooldown = (tWait - get_global_time());
	kInfo.iCooldownReminder = tReminder;
	kInfo.bChance = pMission->bChance;
	for (size_t i = 0; i < MAX_BONUSES_LENGTH; i++)
	{
		kInfo.bApplyType[i] = pReward->bApplyType[i];
		kInfo.lApplyValue[i] = pReward->lApplyValue[i];
	}

	kInfo.dRewardItem = pReward->dRewardItem;
	kInfo.wRewardItemCount = pReward->wRewardItemCount;

	SendClientPacket(m_pkChar->GetDesc(), GC_BIOLOG_OPEN, &kInfo, sizeof(kInfo));
}

void CBiologSystem::SendBiologItem(bool bAdditionalChance, bool bAdditionalTime)
{
	if (!m_pkChar)
	{
		return;
	}

	auto bMission = m_pkChar->GetBiologMissions();
	auto tWait = m_pkChar->GetBiologCooldown();
	auto iCount = m_pkChar->GetBiologCollectedItems();

	// Getting actual biolog mission
	TBiologMissionsProto* pMission = CBiologSystemManager::instance().GetMission(bMission);
	if (!pMission || bMission == CBiologSystemManager::instance().m_mapMission_Proto.size())
	{
		//We have to inform players they've done every mission!
#ifdef __DEBUG_PRINT
		cerr << "SendBiologItem -> cannot get mission of level " << bMission << " by " << m_pkChar->GetName() << endl;
#endif
		return;
	}

	// Checking required level
	if (m_pkChar->GetLevel() < pMission->bRequiredLevel)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have a sufficient level: %d!"), pMission->bRequiredLevel);
		return;
	}

	// Counting required item
	if (m_pkChar->CountSpecifyItem(pMission->iRequiredItem) < 1)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enogh items."));
		return;
	}

	// Checking time
	if (get_global_time() < tWait && bAdditionalTime)
	{
		if (m_pkChar->CountSpecifyItem(BIOLOG_TIME_ITEM) < 1)
		{
			m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have required item, disable time skip."));
			return;
		}
	}

	if (get_global_time() < tWait && !bAdditionalTime)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Try again in: %s!"), GetFullDateFromTime(tWait).c_str());
		return;
	}

	// Define variables
	BYTE chance = pMission->bChance;

	if (bAdditionalChance)
	{
		LPITEM biologChanceItem = m_pkChar->FindSpecifyItem(BIOLOG_CHANCE_ITEM);
		if (biologChanceItem)
		{
			const int BIOLOG_CHANCE = biologChanceItem->GetValue(0);

			if (BIOLOG_CHANCE > 0)
			{
				if (biologChanceItem->GetCount() > 0)
				{
					chance += BIOLOG_CHANCE;
				}
				else
				{
					m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have the required chance boost item."));
					return;
				}
			}
			else
			{
				return;
			}
		}
		else
		{
			m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have the required chance boost item."));
			return;
		}
	}

	// Checking count
	if (iCount < pMission->wRequiredItemCount) // it means you haven't give all items
	{
		m_pkChar->RemoveSpecifyItem(pMission->iRequiredItem, 1);
		
		if (bAdditionalTime)
			m_pkChar->RemoveSpecifyItem(BIOLOG_TIME_ITEM, 1);
		if (bAdditionalChance)
			m_pkChar->RemoveSpecifyItem(BIOLOG_CHANCE_ITEM, 1);
		
		if (chance >= number(1, 100))
		{
			// Update count
			m_pkChar->SetBiologCollectedItems(iCount + 1);

			// Update cooldown
			m_pkChar->SetBiologCooldown(get_global_time() + pMission->iCooldown);

			m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Item successfully returned!"));
		}
		else
		{
			m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item was too damaged, please try again!"));
		}

		// Active Alert
		if (m_pkChar->GetBiologCooldownReminder())
		{
			ActiveAlert(true);
		}

		bool bSendInformation = true;
		// Compare current count with required count
		if (m_pkChar->GetBiologCollectedItems() == pMission->wRequiredItemCount)
		{
			// We have to give reward
			GiveRewardByMission(bMission);

#ifdef ENABLE_GLOBAL_REWARD
			if (bMission == 9)
				CHARACTER_MANAGER::Instance().DoReward(m_pkChar, REWARD_MISSION_BIOLOG, 120, 1);
#endif
			// We have to reset mission
			ResetMission();

			BYTE nextMission = bMission + 1;
			m_pkChar->SetBiologMissions(nextMission);

			TBiologMissionsProto* pNextMission = CBiologSystemManager::instance().GetMission(nextMission);
			if (!pNextMission)
			{
				SendClientPacket(m_pkChar->GetDesc(), GC_BIOLOG_CLOSE, NULL, 0);
				bSendInformation = false;
			}
		}

		// Update client!
		if (bSendInformation)
		{
			SendBiologInformation(true);
		}
	}
}

void CBiologSystem::GiveRewardByMission(BYTE bMission)
{
	if (!m_pkChar)
	{
		return;
	}

	TBiologRewardsProto* pReward = CBiologSystemManager::instance().GetReward(bMission);
	if (!pReward)
	{
		sys_err("GiveRewardByMission -> cannot get reward %u by %s", bMission, m_pkChar->GetName());
		return;
	}

	for (size_t i = 0; i < MAX_BONUSES_LENGTH; i++)
	{
		if (pReward->bApplyType[i] != 0 && pReward->lApplyValue[i] != 0)
		{
#ifdef __DEBUG_PRINT
			cerr << "GiveRewardByMission -> Reward for mission: " << bMission << " by " << m_pkChar->GetName()
				 << " type: " << pReward->bApplyType[i] << " value: " << pReward->lApplyValue[i] << endl;
#endif
			m_pkChar->AddAffect(AFFECT_BIOLOG, aApplyInfo[pReward->bApplyType[i]].bPointType, pReward->lApplyValue[i], 0, BIOLOG_AFF_TIME, 0, false);
		}
	}

	if (pReward->dRewardItem != 0 && pReward->wRewardItemCount != 0)
	{
		m_pkChar->AutoGiveItem(pReward->dRewardItem, pReward->wRewardItemCount);
	}
}

bool CBiologSystem::GetBiologItemByMobVnum(LPCHARACTER pkKiller, WORD MonsterVnum, DWORD& ItemVnum, BYTE& bChance)
{
	if (!pkKiller || !MonsterVnum)
	{
		return false;
	}

	const CMob* pkMob = CMobManager::instance().Get(MonsterVnum);
	if (!pkMob)
	{
		return false;
	}

	auto bMission = pkKiller->GetBiologMissions();

	TBiologMissionsProto* pMission = CBiologSystemManager::instance().GetMission(bMission);
	if (!pMission)
	{
		return false;
	}

	if (pkKiller->GetLevel() < pMission->bRequiredLevel)
	{
		return false;
	}

	std::vector<TBiologMonstersProto*> monsters;
	if (!CBiologSystemManager::instance().GetMonsters(bMission, monsters))
	{
		return false;
	}

	auto monster = std::find_if(monsters.begin(), monsters.end(), [&MonsterVnum](TBiologMonstersProto * m)
	{
		return m->dwMonsterVnum == MonsterVnum;
	});
	if (monster == monsters.end())
	{
		return false;
	}

	ItemVnum = pMission->iRequiredItem;
	bChance = static_cast<TBiologMonstersProto*>(*monster)->bChance;

	return true;
}

/*******************************************************************\
| [PUBLIC] Alert Functions
\*******************************************************************/

void CBiologSystem::ActiveAlert(bool bReminder)
{
#ifdef __DEBUG_PRINT
	cerr << "ActiveAlert -> Beign of activation: " << bReminder << endl;
#endif

	if (!m_pkChar)
	{
		return;
	}

#ifdef __DEBUG_PRINT
	cerr << "ActiveAlert -> We have m_pkChar " << endl;
#endif

	if (bReminder)
	{
		m_pkChar->SetBiologCooldownReminder(1);

		if (m_pkChar->GetBiologCooldown() < get_global_time())
		{
#ifdef __DEBUG_PRINT
			cerr << "ActiveAlert -> Cooldown is smaller than actuall time " << endl;
#endif
			return;
		}

		if (!CEventFunctionHandler::Instance().FindEvent(s_current_biolog_reminder))
		{
#ifdef __DEBUG_PRINT
			cerr << "ActiveAlert -> Add event! " << endl;
#endif
			CEventFunctionHandler::instance().AddEvent([this](SArgumentSupportImpl*)
			{
				this->BroadcastAlert();
			},
			(s_current_biolog_reminder), m_pkChar->GetBiologCooldown() - get_global_time());
		}
		else
		{
			CEventFunctionHandler::instance().DelayEvent((s_current_biolog_reminder), m_pkChar->GetBiologCooldown() - get_global_time());
		}

		return;
	}

	m_pkChar->SetBiologCooldownReminder(0);
	if (CEventFunctionHandler::Instance().FindEvent(s_current_biolog_reminder))
	{
		CEventFunctionHandler::Instance().RemoveEvent((s_current_biolog_reminder));
	}
}

void CBiologSystem::BroadcastAlert()
{
	if (m_pkChar == nullptr)
	{
		return;
	}

	LPDESC desc = m_pkChar->GetDesc();
	if (desc)
	{
		char buf[255];
		int len = snprintf(buf, sizeof(buf), LC_TEXT("The cooldown has finished."));

		TPacketGCWhisper pack;
		pack.bHeader = HEADER_GC_WHISPER;
		pack.bType = WHISPER_TYPE_SYSTEM;
		pack.wSize = sizeof(TPacketGCWhisper) + len;
		strlcpy(pack.szNameFrom, "[BIOLOG]", sizeof(pack.szNameFrom));
#if defined(__BL_CLIENT_LOCALE_STRING__)
		pack.bCanFormat = true;
#endif
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
		strlcpy(pack.szCountry, "", sizeof(pack.szCountry));
		pack.bEmpire = 0;
#endif
		m_pkChar->GetDesc()->BufferedPacket(&pack, sizeof(pack));
		m_pkChar->GetDesc()->Packet(buf, len);
	}

	if (m_pkChar && m_pkChar->GetDesc() && m_pkChar->GetDesc()->IsPhase(PHASE_GAME))
	{
		SendClientPacket(m_pkChar->GetDesc(), GC_BIOLOG_ALERT, NULL, 0);
	}
}

/*******************************************************************\
| [PUBLIC] Incoming Packet Functions
\*******************************************************************/
// @checkMe -> Consider refactor; you shouldn't use that odd-looking bytes computation; otherwise it might lead to unexpected outcome
int CBiologSystem::RecvClientPacket(BYTE bSubHeader, const char* c_pData, size_t uiBytes)
{
	switch (bSubHeader)
	{
	case CG_BIOLOG_OPEN:
	{
		SendBiologInformation();
		return 0;
	}
	break;

	case CG_BIOLOG_SEND:
	{
		if (uiBytes < sizeof(bool) + sizeof(bool))
		{
			return -1;
		}

		bool bAdditionalChance = *(bool*)c_pData;
		c_pData += sizeof(bool);
		uiBytes -= sizeof(bool);

		bool bAdditionalTime = *(bool*)c_pData;
		c_pData += sizeof(bool);
		uiBytes -= sizeof(bool);

		SendBiologItem(bAdditionalChance, bAdditionalTime);
		return sizeof(bool) + sizeof(bool);
	}
	break;

	case CG_BIOLOG_TIMER:
	{
		if (uiBytes < sizeof(bool))
		{
			return -1;
		}

		bool bReminder = *(bool*)c_pData;
		c_pData += sizeof(bool);
		uiBytes -= sizeof(bool);

		ActiveAlert(bReminder);
		return sizeof(bool);
	}
	break;
	}

	sys_err("invalid subheader %u", bSubHeader);
	return -1;
}

/*******************************************************************\
| [PUBLIC] Outgoing Packet Functions
\*******************************************************************/

void CBiologSystem::SendClientPacket(LPDESC pkDesc, BYTE bSubHeader, const void* c_pvData, size_t iSize)
{
	TPacketGCBiologManager packet;
	packet.bHeader = HEADER_GC_BIOLOG_MANAGER;
	packet.wSize = sizeof(packet) + iSize;
	packet.bSubHeader = bSubHeader;

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(packet));
	if (iSize)
	{
		buf.write(c_pvData, iSize);
	}

	pkDesc->Packet(buf.read_peek(), buf.size());
}

void CBiologSystem::SendClientPacket(DWORD dwPID, BYTE bSubHeader, const void* c_pvData, size_t iSize)
{
	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(dwPID);

	if (pkChr)
	{
		SendClientPacket(pkChr->GetDesc(), bSubHeader, c_pvData, iSize);
	}
	else
	{
		CCI* pkCCI = P2P_MANAGER::instance().FindByPID(dwPID);
		if (pkCCI)
		{
			pkCCI->pkDesc->SetRelay(pkCCI->szName);
			SendClientPacket(pkCCI->pkDesc, bSubHeader, c_pvData, iSize);
		}
		else
		{
			sys_err("cannot send client packet to pid %u subheader %hu [cannot find player]", dwPID, bSubHeader);
		}
	}
}

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CBiologSystemManager - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

CBiologSystemManager::CBiologSystemManager()
{
}

CBiologSystemManager::~CBiologSystemManager()
{
}

void CBiologSystemManager::InitializeMissions(TBiologMissionsProto* pData, size_t size)
{
	for (size_t i{ 0 }; i < size; ++i, ++pData)
	{
		std::memcpy(&m_mapMission_Proto[pData->bMission], pData, sizeof(TBiologMissionsProto));
	}
}

TBiologMissionsProto* CBiologSystemManager::GetMission(BYTE bMission)
{
	TMissionProtoMap::iterator it = m_mapMission_Proto.find(bMission);
	if (it == m_mapMission_Proto.end())
	{
		return NULL;
	}

	return &it->second;
}

void CBiologSystemManager::InitializeRewards(TBiologRewardsProto* pData, size_t size)
{
	for (size_t i{ 0 }; i < size; ++i, ++pData)
	{
		std::memcpy(&m_mapReward_Proto[pData->bMission], pData, sizeof(TBiologRewardsProto));
	}
}

TBiologRewardsProto* CBiologSystemManager::GetReward(BYTE bMission)
{
	TRewardProtoMap::iterator it = m_mapReward_Proto.find(bMission);
	if (it == m_mapReward_Proto.end())
	{
		return NULL;
	}

	return &it->second;
}

void CBiologSystemManager::InitializeMonsters(TBiologMonstersProto* pData, size_t size)
{
	for (size_t i{ 0 }; i < size; ++i, ++pData)
	{
		m_mapMonsters_Proto[pData->bMission].push_back(pData);
	}
}

bool CBiologSystemManager::GetBiologWikipedia(std::vector<TBiologWikipedia>& wikiInfo)
{
	for (const auto& rkMission : m_mapMission_Proto)
	{
		auto it = m_mapMonsters_Proto.find(rkMission.first);
		if (it != m_mapMonsters_Proto.end())
		{
			for (auto pkMonster : it->second)
			{
				wikiInfo.push_back(TBiologWikipedia { pkMonster->dwMonsterVnum, rkMission.second.iRequiredItem, pkMonster->bChance});
				sys_log(0, "GetBiologWikipedia: Adding Monster: %u Adding Item: %u", pkMonster->dwMonsterVnum, rkMission.second.iRequiredItem);
			}
		}
	}
	
	return wikiInfo.size();
}

bool CBiologSystemManager::GetMonsters(BYTE bMission, std::vector<TBiologMonstersProto*>& monsters)
{
	auto it = m_mapMonsters_Proto.find(bMission);
	if (it == m_mapMonsters_Proto.end())
	{
		return false;
	}

	monsters = it->second;

	return true;
}
#endif
