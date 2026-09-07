#include "stdafx.h"

#ifdef ENABLE_SWITCHBOT_SYSTEM
#include "new_switchbot.h"
#include "desc.h"
#include "item.h"
#include "item_manager.h"
#include "char_manager.h"
#include "buffer_manager.h"
#include "char.h"
#include "config.h"
#include "p2p.h"
#ifdef __RANKING_SYSTEM__
	#include "RankPlayer.h"
#endif

bool ValidPosition(DWORD wCell)
{
	return wCell < SWITCHBOT_SLOT_COUNT;
}

const float c_fSpeed = 0.16f;

bool SwitchbotHelper::IsValidItem(LPITEM pkItem)
{
	if (!pkItem)
	{
		return false;
	}

	switch (pkItem->GetType())
	{
	case ITEM_WEAPON:
		return true;

	case ITEM_ARMOR:
		switch (pkItem->GetSubType())
		{
		case ARMOR_BODY:
		case ARMOR_HEAD:
		case ARMOR_SHIELD:
		case ARMOR_WRIST:
		case ARMOR_FOOTS:
		case ARMOR_NECK:
		case ARMOR_EAR:
#ifdef ENABLE_PENDANT_SYSTEM
		case ARMOR_PENDANT:
#endif
#ifdef ENABLE_GLOVE_SYSTEM
		case ARMOR_GLOVE:
#endif
			return true;
		}

	/*case ITEM_COSTUME:
		return  true;*/

	default:
		return false;
	}
}

CSwitchbot::CSwitchbot()
{
	m_pkSwitchEvent = NULL;
	m_table = {};
	m_isWarping = false;
}

CSwitchbot::~CSwitchbot()
{
	if (m_pkSwitchEvent)
	{
		event_cancel(&m_pkSwitchEvent);
		m_pkSwitchEvent = NULL;
	}

	m_table = {};
	m_isWarping = false;
}

void CSwitchbot::SetTable(TSwitchbotTable table)
{
	m_table = table;
}

TSwitchbotTable CSwitchbot::GetTable()
{
	return m_table;
}

void CSwitchbot::SetPlayerId(DWORD player_id)
{
	m_table.player_id = player_id;
}

DWORD CSwitchbot::GetPlayerId(DWORD player_id)
{
	return m_table.player_id;
}

void CSwitchbot::RegisterItem(WORD wCell, DWORD item_id)
{
	if (!ValidPosition(wCell))
	{
		return;
	}

	m_table.items[wCell] = item_id;
}

void CSwitchbot::UnregisterItem(WORD wCell)
{
	if (!ValidPosition(wCell))
	{
		return;
	}

	m_table.items[wCell] = 0;
	m_table.active[wCell] = false;
	m_table.finished[wCell] = false;
	memset(&m_table.alternatives[wCell], 0, sizeof(m_table.alternatives[wCell]));
}

void CSwitchbot::SetAttributes(BYTE slot, std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives)
{
	if (!ValidPosition(slot))
	{
		return;
	}

	for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
	{
		for (BYTE attrIdx = 0; attrIdx < MAX_NORM_ATTR_NUM + MAX_RARE_ATTR_NUM; ++attrIdx)
		{
			m_table.alternatives[slot][alternative].attributes[attrIdx].bType = vec_alternatives[alternative].attributes[attrIdx].bType;
			m_table.alternatives[slot][alternative].attributes[attrIdx].sValue = vec_alternatives[alternative].attributes[attrIdx].sValue;
		}
	}
}

void CSwitchbot::SetActive(BYTE slot, bool active)
{
	if (!ValidPosition(slot))
	{
		return;
	}

	m_table.active[slot] = active;
	m_table.finished[slot] = false;
}

EVENTINFO(TSwitchbotEventInfo)
{
	CSwitchbot *pkSwitchbot;

	TSwitchbotEventInfo() : pkSwitchbot(NULL)
	{
	}
};

EVENTFUNC(switchbot_event)
{
	TSwitchbotEventInfo *info = dynamic_cast<TSwitchbotEventInfo *>(event->info);

	if (info == NULL)
	{
		sys_err("switchbot_event> <Factor> Info Null pointer");
		return 0;
	}

	if (!info->pkSwitchbot)
	{
		sys_err("switchbot_event> <Factor> Switchbot Null pointer");
		return 0;
	}

	info->pkSwitchbot->SwitchItems();

	return PASSES_PER_SEC(c_fSpeed);
}

void CSwitchbot::Start()
{
	TSwitchbotEventInfo *info = AllocEventInfo<TSwitchbotEventInfo>();
	info->pkSwitchbot = this;

	m_pkSwitchEvent = event_create(switchbot_event, info, c_fSpeed);

	CSwitchbotManager::Instance().SendSwitchbotUpdate(m_table.player_id);
}

void CSwitchbot::Stop()
{
	if (m_pkSwitchEvent)
	{
		event_cancel(&m_pkSwitchEvent);
		m_pkSwitchEvent = NULL;
	}

	memset(&m_table.active, 0, sizeof(m_table.active));

	CSwitchbotManager::Instance().SendSwitchbotUpdate(m_table.player_id);
}

void CSwitchbot::Pause()
{
	if (m_pkSwitchEvent)
	{
		event_cancel(&m_pkSwitchEvent);
		m_pkSwitchEvent = NULL;
	}
}

bool CSwitchbot::IsActive(BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return false;
	}

	return m_table.active[slot];
}

bool CSwitchbot::IsConfigFor67Bonus(BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return false;
	}

	for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
	{
		for (BYTE attrIdx = 0; attrIdx < MAX_NORM_ATTR_NUM + MAX_RARE_ATTR_NUM; ++attrIdx)
		{
			if (m_table.alternatives[slot][alternative].attributes[attrIdx].bType || m_table.alternatives[slot][alternative].attributes[attrIdx].sValue)
			{
				// is 6/7
				if (attrIdx >= MAX_NORM_ATTR_NUM)
					return true;
			}
		}
	}
	
	return false;
}

bool CSwitchbot::HasActiveSlots()
{
	for (const auto &it : m_table.active)
	{
		if (it)
		{
			return true;
		}
	}

	return false;
}

bool CSwitchbot::IsSwitching()
{
	return m_pkSwitchEvent != NULL;
}

bool CSwitchbot::IsWarping()
{
	return m_isWarping;
}

void CSwitchbot::SetIsWarping(bool warping)
{
	m_isWarping = warping;
}

void CSwitchbot::SwitchItems()
{
	for (BYTE bSlot = 0; bSlot < SWITCHBOT_SLOT_COUNT; ++bSlot)
	{
		if (!m_table.active[bSlot])
		{
			continue;
		}

		bool IsPendant = false;
		m_table.finished[bSlot] = false;

		const DWORD item_id = m_table.items[bSlot];

		LPITEM pkItem = ITEM_MANAGER::Instance().Find(item_id);
		if (!pkItem)
		{
			continue;
		}


#ifdef ENABLE_SKINBONUS
					static const DWORD s_adwKAllowedVnum[] =
					{
						172650,
						172651,
						172652,
						172653,
						172654,
						172655,
						172656,
						172657,
						172658,
						172659,
						172660,
						172661,
						172662,
						172663,
						172664,
						172665,
						172665,
						172666,
						172667,
						172668,
						172669,
						172670,
						172671,
						172672,
						172673,
						172674,
						172675,
						172676,
						172677,
						172678,
						172679,
						172680,
						172681,
						172682,
						172683,
						172684,
						172685,
						172686,
						172687,
						172688,
						172689,
						172690,
						172691,
						172692,
						172693,
						172694,
						172695,
						172696,
						172697,
						172698,
						172699,
						172700,
						172701,
						172702,
						172703,
						172704,
						172705,
						172706,
						172707,
						172708,
						172709,

						79000,
						79001,
						79002,
						79003,
						79004,
						79005,
						79006,
						79007,
						79008,
						79009,
						79010,
						79011,
						79012,
						79013,
						79014,
						79015,
						79016,
						79017,
						79018,
						79019,
						79020,
						79021,
						79022,
						79023,
						79024,
						79025,
						79026,
						79027,
						79028,
						79029,
						79030,
						79031,
						79032,
						79033,
						79034,
						79035,
						79036,
						79037,
						79038,
						79039,
						79040,
						79041,
						79042,
						79043,
						79044,
						79045,
						79046,
						79047,
						79048,
						79049,
						79050,
						79051,
						79052,
						79053,
						79054,
						79055,
						79056,
						79057,
						79058,
						79059,
						79060,
						79061,
						79062,
						79063,
						79064,
						79065,
						79066,
						79067,
						79068,
						79069,
						79070,
						79071,
						79072,
						79073,
						79074,
						79075,
						79076,
						79077,
						79078,
						79079,
						79080,
						79081,
						79082,
						79083,
						79084,
						79085,
						79086,
						79087,
						79088,
						79089
					};

					bool bIsKTarget = false;
					{
						const DWORD dwTargetVnum = pkItem->GetVnum();
						for (int i = 0; i < (int)(sizeof(s_adwKAllowedVnum) / sizeof(s_adwKAllowedVnum[0])); ++i)
						{
							if (s_adwKAllowedVnum[i] == dwTargetVnum)
							{
								bIsKTarget = true;
								break;
							}
						}
					}
#endif

		LPCHARACTER pkOwner = pkItem->GetOwner();
		if (!pkOwner)
		{
			return;
		}

		if (pkItem->GetType() == ITEM_ARMOR && pkItem->GetSubType() == ARMOR_PENDANT)
		{
			IsPendant = true;
		}

		if (CheckItem(pkItem, bSlot))
		{
			LPDESC desc = pkOwner->GetDesc();
			if (desc)
			{
				char buf[255];
				int len = snprintf(buf, sizeof(buf), LC_TEXT("Bonuschange of %s (Slot: %d) successfully finished."), pkItem->GetName(), bSlot + 1);

				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SYSTEM;
				pack.wSize = sizeof(TPacketGCWhisper) + len;
				strlcpy(pack.szNameFrom, "[SWITCHBOT]", sizeof(pack.szNameFrom));
#if defined(__BL_CLIENT_LOCALE_STRING__)
				pack.bCanFormat = true;
#endif
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
				strlcpy(pack.szCountry, "", sizeof(pack.szCountry));
				pack.bEmpire = 0;
#endif
				pkOwner->GetDesc()->BufferedPacket(&pack, sizeof(pack));
				pkOwner->GetDesc()->Packet(buf, len);
			}

			SetActive(bSlot, false);

			m_table.finished[bSlot] = true;

			if (!HasActiveSlots())
			{
				Stop();
			}
			else
			{
				CSwitchbotManager::Instance().SendSwitchbotUpdate(m_table.player_id);
			}
		}
		else
		{
			bool stop = true;
			bool isLowLevelItem = false;
			DWORD switchItemUsed = 0;
			
			bool IsConfiguredFor67 = IsConfigFor67Bonus(bSlot);
			bool bIsUsedGreenFor67 = false;
			
			if (SWITCHBOT_PRICE_TYPE == 1)
			{
				if (IsConfiguredFor67)
				{
				#ifdef ENABLE_SKINBONUS
					if (bIsKTarget)
					{
						if (pkOwner->CountSpecifyItem(173004) >= 1)
						{
							stop = false;
							switchItemUsed = 173004;
						}
					}
					else
				#endif
					{
						if (pkOwner->CountSpecifyItem(71052) >= 1)
						{
							stop = false;
							switchItemUsed = 71052;
						}
					}
				}
				else if (IsPendant)
				{
					if (pkOwner->CountSpecifyItem(65794) >= SWITCHBOT_PRICE_AMOUNT)
					{
						switchItemUsed = 65794;
						stop = false;
					}
				}
				else if (pkItem->GetType() == ITEM_COSTUME)
				{
					if (pkOwner->CountSpecifyItem(70064) >= SWITCHBOT_PRICE_AMOUNT)
					{
						switchItemUsed = 70064;
						stop = false;
					}
				}
				else
				{
					for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
					{
						if (pkItem->GetLimitType(i) == LIMIT_LEVEL && pkItem->GetLimitValue(i) <= 40)
						{
							isLowLevelItem = true;
							break;
						}
					}
					
						if (!isLowLevelItem)
						{
							for (const auto& itemVnum : c_arSwitchingItems)
							{
#ifdef ENABLE_SKINBONUS
								if (bIsKTarget)
								{
									if (itemVnum != 173002)
										continue;

									if (pkOwner->CountSpecifyItem(173002) >= 1)
									{
										switchItemUsed = 173002;
										stop = false;
										break;
									}
								}
								else
								{
									if (itemVnum == 173002 || itemVnum == 173004)
										continue;

									if (pkOwner->CountSpecifyItem(itemVnum) >= SWITCHBOT_PRICE_AMOUNT)
									{
										switchItemUsed = itemVnum;
										stop = false;
										break;
									}
								}
#else
								if (pkOwner->CountSpecifyItem(itemVnum) >= SWITCHBOT_PRICE_AMOUNT)
								{
									switchItemUsed = itemVnum;
									stop = false;
									break;
								}
#endif
							}
						}
					else
					{
						for (const auto& itemLowVnum : c_arSwitchingLowItems)
						{
#ifdef ENABLE_SKINBONUS
							if (bIsKTarget)
								continue;

							if (itemLowVnum == 173002 || itemLowVnum == 173004)
								continue;
#endif
							if (pkOwner->CountSpecifyItem(itemLowVnum) >= SWITCHBOT_PRICE_AMOUNT)
							{
								switchItemUsed = itemLowVnum;
								stop = false;
								break;
							}
						}

						if (!switchItemUsed)
						{
							for (const auto& itemHightVnum : c_arSwitchingItems)
							{
#ifdef ENABLE_SKINBONUS
								if (bIsKTarget)
								{
									if (itemHightVnum != 173002)
										continue;

									if (pkOwner->CountSpecifyItem(173002) >= 1)
									{
										switchItemUsed = 173002;
										stop = false;
										break;
									}
								}
								else
								{
									if (itemHightVnum == 173002 || itemHightVnum == 173004)
										continue;

									if (pkOwner->CountSpecifyItem(itemHightVnum) >= SWITCHBOT_PRICE_AMOUNT)
									{
										switchItemUsed = itemHightVnum;
										stop = false;
										break;
									}
								}
#else
								if (pkOwner->CountSpecifyItem(itemHightVnum) >= SWITCHBOT_PRICE_AMOUNT)
								{
									switchItemUsed = itemHightVnum;
									stop = false;
									break;
								}
#endif
							}
						}
					}
				}
			}
			else if (SWITCHBOT_PRICE_TYPE == 2)
			{
				if (pkOwner->GetGold() >= SWITCHBOT_PRICE_AMOUNT)
				{
					stop = false;
				}
			}

			if (stop || (!switchItemUsed && SWITCHBOT_PRICE_TYPE != 2))
			{
				SetActive(bSlot, false);
				m_table.finished[bSlot] = false;
				if (!HasActiveSlots())
				{
					Stop();
				}
				else
				{
					CSwitchbotManager::Instance().SendSwitchbotUpdate(m_table.player_id);
				}
				// Stop();

				if (SWITCHBOT_PRICE_TYPE == 1)
					pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Switchbot stopped. Out of switchers."));
				else
					pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Switchbot stopped. Not enough money."));

				return;
			}

			if (SWITCHBOT_PRICE_TYPE == 1)
			{
				int iRemoveCount = SWITCHBOT_PRICE_AMOUNT;

			#ifdef ENABLE_SKINBONUS
				if (switchItemUsed == 173002 || switchItemUsed == 173004)
					iRemoveCount = 1;
			#endif

				LPITEM pkItemChanger = pkOwner->FindSpecifyItem(switchItemUsed);
				if (pkItemChanger)
					pkItemChanger->SetCount(pkItemChanger->GetCount() - iRemoveCount);
				else
					Stop();

				if (IsPendant)
				{
					LPITEM pkItemChanger2 = pkOwner->FindSpecifyItem(65794);
					if (pkItemChanger2)
						pkItemChanger2->SetCount(pkItemChanger2->GetCount() - SWITCHBOT_PRICE_AMOUNT);
					else
						Stop();
				}
			}
			else if (SWITCHBOT_PRICE_TYPE == 2)
			{
#ifdef ENABLE_GOLD_MAX_EXTENDED
				pkOwner->ChangeGold(-SWITCHBOT_PRICE_AMOUNT);
#else
				pkOwner->PointChange(POINT_GOLD, -SWITCHBOT_PRICE_AMOUNT, false);
#endif
			}
			
			if (IsConfiguredFor67)
			{
				if (bIsUsedGreenFor67)
				{
					if (number(1, 100) <= 10) // 10% chance to add
						pkItem->ChangeRareAttribute();
				}
				else
					pkItem->ChangeRareAttribute();
			}
			else
			{
				pkItem->ChangeAttribute();
			}
			
			SendItemUpdate(pkOwner, bSlot, pkItem);
#ifdef __RANKING_SYSTEM__
			RankPlayer::instance().SendInfoPlayer(pkOwner, RANK_BY_BONUS_CHANGED, RankPlayer::instance().GetProgressByPID(pkOwner->GetPlayerID(), RANK_BY_BONUS_CHANGED) + 1);
#endif
		}
	}
}

/*
bool CSwitchbot::CheckItem(LPITEM pkItem, BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return false;
	}

	if (!pkItem)
	{
		return false;
	}

	bool checked = 0;

	for (const auto &alternative : m_table.alternatives[slot])
	{
		if (!alternative.IsConfigured())
		{
			continue;
		}

		BYTE configuredAttrCount = 0;
		BYTE correctAttrCount = 0;

		for (const auto &destAttr : alternative.attributes)
		{
			if (!destAttr.bType || !destAttr.sValue)
			{
				continue;
			}

			++configuredAttrCount;

			for (BYTE attrIdx = 0; attrIdx < ITEM_ATTRIBUTE_NORM_NUM; ++attrIdx)
			{
				const TPlayerItemAttribute &curAttr = pkItem->GetAttribute(attrIdx);

				if (curAttr.bType != destAttr.bType || curAttr.sValue < destAttr.sValue)
				{
					continue;
				}

				++correctAttrCount;
				break;
			}
		}

		checked = true;

		if (configuredAttrCount == correctAttrCount)
		{
			return true;
		}
	}

	if (!checked)
	{
		return true;
	}

	return false;
}
*/

bool CSwitchbot::CheckItem(LPITEM pkItem, BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return false;
	}

	if (!pkItem)
	{
		return false;
	}

	bool checked = 0;

	for (int i = 0; i < SWITCHBOT_ALTERNATIVE_COUNT; ++i)
	{		
		const TSwitchbotAttributeAlternativeTable& alternative = m_table.alternatives[slot][i];
		if (!alternative.IsConfigured())
		{
			continue;
		}

		BYTE configuredAttrCount = 0;
		BYTE correctAttrCount = 0;
		
		// START_CHECK_67
		int startAttrNum = 0;
		int maxAttrNum = MAX_NORM_ATTR_NUM;
		for (BYTE attrIdx = 0; attrIdx < MAX_NORM_ATTR_NUM + MAX_RARE_ATTR_NUM; ++attrIdx)
		{
			if (m_table.alternatives[slot][i].attributes[attrIdx].bType || m_table.alternatives[slot][i].attributes[attrIdx].sValue)
			{
				// is 6/7
				if (attrIdx >= MAX_NORM_ATTR_NUM)
				{
					startAttrNum = MAX_NORM_ATTR_NUM;
					maxAttrNum = MAX_NORM_ATTR_NUM + MAX_RARE_ATTR_NUM;
					break;
				}
			}
		}
		// END_OF_CHECK_67
		
		for (int j = startAttrNum; j < maxAttrNum; ++j)
		{
			const TPlayerItemAttribute& destAttr = alternative.attributes[j];
			if (!destAttr.bType || !destAttr.sValue)
			{
				continue;
			}

			++configuredAttrCount;

			for (BYTE attrIdx = startAttrNum; attrIdx < maxAttrNum; ++attrIdx)
			{
				const TPlayerItemAttribute& curAttr = pkItem->GetAttribute(attrIdx);

				if (curAttr.bType != destAttr.bType || curAttr.sValue < destAttr.sValue)
				{
					continue;
				}

				++correctAttrCount;
				break;
			}
		}

		checked = true;

		if (configuredAttrCount == correctAttrCount)
		{
			return true;
		}
	}

	if (!checked)
	{
		return true;
	}

	return false;
}

void CSwitchbot::SendItemUpdate(LPCHARACTER ch, BYTE slot, LPITEM item)
{
	LPDESC desc = ch->GetDesc();
	if (!desc)
	{
		return;
	}

	TPacketGCSwitchbot pack;
	pack.header = HEADER_GC_SWITCHBOT;
	pack.subheader = SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM;
	pack.size = sizeof(TPacketGCSwitchbot) + sizeof(TSwitchbotUpdateItem);

	TSwitchbotUpdateItem update = {};
	update.slot = slot;
	update.vnum = item->GetVnum();
	update.count = item->GetCount();

	thecore_memcpy(update.alSockets, item->GetSockets(), sizeof(update.alSockets));
	thecore_memcpy(update.aAttr, item->GetAttributes(), sizeof(update.aAttr));

	desc->BufferedPacket(&pack, sizeof(pack));
	desc->Packet(&update, sizeof(TSwitchbotUpdateItem));
}

CSwitchbotManager::CSwitchbotManager()
{
	Initialize();
}

CSwitchbotManager::~CSwitchbotManager()
{
	Initialize();
}

void CSwitchbotManager::Initialize()
{
	// TODO : check this -> Delete maybe ?
	for (decltype(m_map_Switchbots)::const_iterator iter = m_map_Switchbots.begin(); iter != m_map_Switchbots.end(); ++iter)
	{
		delete iter->second;
	}
	m_map_Switchbots.clear();
}

void CSwitchbotManager::RegisterItem(DWORD player_id, DWORD item_id, WORD wCell)
{
	CSwitchbot *pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		pkSwitchbot = new CSwitchbot();
		pkSwitchbot->SetPlayerId(player_id);
		m_map_Switchbots.insert(std::make_pair(player_id, pkSwitchbot));
	}

	if (pkSwitchbot->IsWarping())
	{
		return;
	}

	pkSwitchbot->RegisterItem(wCell, item_id);
	SendSwitchbotUpdate(player_id);
}

void CSwitchbotManager::UnregisterItem(DWORD player_id, WORD wCell)
{
	CSwitchbot *pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return;
	}

	if (pkSwitchbot->IsWarping())
	{
		return;
	}

	pkSwitchbot->UnregisterItem(wCell);
	SendSwitchbotUpdate(player_id);
}

void CSwitchbotManager::Start(DWORD player_id, BYTE slot, std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives)
{
	if (!ValidPosition(slot))
	{
		return;
	}

	CSwitchbot *pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		sys_err("No Switchbot found for player_id %d slot %d", player_id, slot);
		return;
	}

	if (pkSwitchbot->IsActive(slot))
	{
		sys_err("Switchbot slot %d already running for player_id %d", slot, player_id);
		return;
	}

	// Restriction 6/7
	bool bHas1x5Bns = false;
	bool bHas6x7Bns = false;

	for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
	{
		for (BYTE attrIdx = 0; attrIdx < MAX_NORM_ATTR_NUM + MAX_RARE_ATTR_NUM; ++attrIdx)
		{
			if (vec_alternatives[alternative].attributes[attrIdx].bType || vec_alternatives[alternative].attributes[attrIdx].sValue)
			{
				if (attrIdx <= MAX_NORM_ATTR_NUM - 1)
					bHas1x5Bns = true;
				else
					bHas6x7Bns = true;
			}
		}
	}
	
	if (bHas6x7Bns && bHas1x5Bns)
	{
		LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindByPID(player_id);
		if (ch)
			ch->ChatPacket(CHAT_TYPE_INFO, "You cant switch 1/5 bonus and 6/7 in same time!");
		
		SendSwitchbotUpdate(player_id);
		return;
	}
	// End

	pkSwitchbot->SetActive(slot, true);
	pkSwitchbot->SetAttributes(slot, vec_alternatives);

	if (pkSwitchbot->HasActiveSlots() && !pkSwitchbot->IsSwitching())
	{
		pkSwitchbot->Start();
	}
	else
	{
		SendSwitchbotUpdate(player_id);
	}
}

void CSwitchbotManager::Stop(DWORD player_id, BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return;
	}

	CSwitchbot *pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		sys_err("No Switchbot found for player_id %d slot %d", player_id, slot);
		return;
	}

	if (!pkSwitchbot->IsActive(slot))
	{
		sys_err("Switchbot slot %d is not running for player_id %d", slot, player_id);
		return;
	}

	pkSwitchbot->SetActive(slot, false);

	if (!pkSwitchbot->HasActiveSlots() && pkSwitchbot->IsSwitching())
	{
		pkSwitchbot->Stop();
	}
	else
	{
		SendSwitchbotUpdate(player_id);
	}
}

bool CSwitchbotManager::IsActive(DWORD player_id, BYTE slot)
{
	if (!ValidPosition(slot))
	{
		return false;
	}

	CSwitchbot *pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return false;
	}

	return pkSwitchbot->IsActive(slot);
}

bool CSwitchbotManager::IsWarping(DWORD player_id)
{
	CSwitchbot *pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return false;
	}

	return pkSwitchbot->IsWarping();
}

void CSwitchbotManager::SetIsWarping(DWORD player_id, bool warping)
{
	CSwitchbot *pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return;
	}

	pkSwitchbot->SetIsWarping(warping);
}

CSwitchbot *CSwitchbotManager::FindSwitchbot(DWORD player_id)
{
	const auto &it = m_map_Switchbots.find(player_id);
	if (it == m_map_Switchbots.end())
	{
		return NULL;
	}

	return it->second;
}

void CSwitchbotManager::P2PSendSwitchbot(DWORD player_id, WORD wTargetPort)
{
	CSwitchbot *pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		sys_log(0, "No switchbot found to transfer. (pid %d source_port %d target_port %d)", player_id, mother_port, wTargetPort);
		return;
	}

	pkSwitchbot->Pause();
	m_map_Switchbots.erase(player_id);

	TPacketGGSwitchbot pack;
	pack.wPort = wTargetPort;
	pack.table = pkSwitchbot->GetTable();

	delete pkSwitchbot;
	pkSwitchbot = nullptr;

	P2P_MANAGER::Instance().Send(&pack, sizeof(pack));
}

void CSwitchbotManager::P2PReceiveSwitchbot(TSwitchbotTable table)
{
	CSwitchbot *pkSwitchbot = FindSwitchbot(table.player_id);
	if (!pkSwitchbot)
	{
		pkSwitchbot = new CSwitchbot();
		m_map_Switchbots.insert(std::make_pair(table.player_id, pkSwitchbot));
	}

	pkSwitchbot->SetTable(table);
}

void CSwitchbotManager::SendItemAttributeInformations(LPCHARACTER ch)
{
	if (!ch)
	{
		return;
	}

	LPDESC desc = ch->GetDesc();
	if (!desc)
	{
		return;
	}

	TPacketGCSwitchbot pack;
	pack.header = HEADER_GC_SWITCHBOT;
	pack.subheader = SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION;
	pack.size = sizeof(TPacketGCSwitchbot);

	TEMP_BUFFER buf;
	for (BYTE bAttributeSet = 0; bAttributeSet < ATTRIBUTE_SET_MAX_NUM; ++bAttributeSet)
	{
		for (int iApplyNum = 0; iApplyNum < MAX_APPLY_NUM; ++iApplyNum)
		{
			const TItemAttrTable &r = g_map_itemAttr[iApplyNum];

			BYTE max = r.bMaxLevelBySet[bAttributeSet];
			if (max > 0)
			{
				TSwitchbottAttributeTable table = {};
				table.attribute_set = bAttributeSet;
				table.apply_num = iApplyNum;
				table.max_value = r.lValues[max - 1];

				buf.write(&table, sizeof(table));
			}
			
			// RARE_ATTR
			const TItemAttrTable& rare = g_map_itemRare[iApplyNum];
			max = rare.bMaxLevelBySet[bAttributeSet];
			if (max > 0)
			{
				TSwitchbottAttributeTable table = {};
				table.attribute_set = bAttributeSet;
				table.apply_num = iApplyNum;
				table.max_value = rare.lValues[max-1];
				table.bIsRareAttr = true;
				buf.write(&table, sizeof(table));
			}
			// END
		}
	}

	if (buf.size())
	{
		pack.size += buf.size();
		desc->BufferedPacket(&pack, sizeof(pack));
		desc->Packet(buf.read_peek(), buf.size());
	}
	else
	{
		desc->Packet(&pack, sizeof(pack));
	}
}

void CSwitchbotManager::SendSwitchbotUpdate(DWORD player_id)
{
	CSwitchbot *pkSwitchbot = FindSwitchbot(player_id);
	if (!pkSwitchbot)
	{
		return;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::Instance().FindByPID(player_id);
	if (!ch)
	{
		return;
	}

	LPDESC desc = ch->GetDesc();
	if (!desc)
	{
		return;
	}

	TSwitchbotTable table = pkSwitchbot->GetTable();

	TPacketGCSwitchbot pack;
	pack.header = HEADER_GC_SWITCHBOT;
	pack.subheader = SUBHEADER_GC_SWITCHBOT_UPDATE;
	pack.size = sizeof(TPacketGCSwitchbot) + sizeof(TSwitchbotTable);

	desc->BufferedPacket(&pack, sizeof(pack));
	desc->Packet(&table, sizeof(table));
}

void CSwitchbotManager::EnterGame(LPCHARACTER ch)
{
	SendItemAttributeInformations(ch);
	SetIsWarping(ch->GetPlayerID(), false);
	SendSwitchbotUpdate(ch->GetPlayerID());

	CSwitchbot *pkSwitchbot = FindSwitchbot(ch->GetPlayerID());
	if (pkSwitchbot && pkSwitchbot->HasActiveSlots() && !pkSwitchbot->IsSwitching())
	{
		pkSwitchbot->Start();
	}
}

#endif
