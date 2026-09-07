#include "stdafx.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "packet.h"
#include "guild.h"
#include "vector.h"
#include "questmanager.h"
#include "item.h"
#include "horsename_manager.h"
#include "locale_service.h"
#include "arena.h"

#include "../../common/VnumHelper.h"

#ifdef ENABLE_MOUNT_SYSTEM
	#include "mob_manager.h"
	#include "item_manager.h"
#endif

bool CHARACTER::StartRiding()
{
#ifdef ENABLE_NEWSTUFF
	if (g_NoMountAtGuildWar && GetWarMap())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
		if (IsRiding())
			StopRiding();
		return false;
	}
#endif
	if (IsDead() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("쓰러진 상태에서는 말에 탈 수 없습니다."));
		return false;
	}

	if (IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신 상태에서는 말에 탈 수 없습니다."));
		return false;
	}

	LPITEM armor = GetWear(WEAR_BODY);

	if (armor && (armor->GetVnum() >= 11901 && armor->GetVnum() <= 11904))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("예복을 입은 상태에서 말을 탈 수 없습니다."));
		return false;
	}

	// @warme005
	if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
		return false;

	DWORD dwMountVnum = m_chHorse ? m_chHorse->GetRaceNum() : GetMyHorseVnum();
#ifdef ENABLE_MOUNT_SYSTEM
	GetSpecialMount(dwMountVnum);
#endif

	if (false == CHorseRider::StartRiding())
	{
		if (GetHorseLevel() <= 0)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 소유하고 있지 않습니다."));
		else if (GetHorseHealth() <= 0)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말이 죽어있는 상태 입니다."));
		else if (GetHorseStamina() <= 0)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말의 스테미너가 부족하여 말을 탈 수 없습니다."));

		return false;
	}

	HorseSummon(false);

	MountVnum(dwMountVnum);

	if(test_server)
		sys_log(0, "Ride Horse : %s ", GetName());

	return true;
}

bool CHARACTER::StopRiding()
{
	if (CHorseRider::StopRiding())
	{
		quest::CQuestManager::instance().Unmount(GetPlayerID());

		if (!IsDead() && !IsStun())
		{
			DWORD dwOldVnum = GetMountVnum();
			MountVnum(0);

#ifdef ENABLE_MOUNT_SYSTEM
			HorseSummon(true, false, dwOldVnum, !IsNormalHorse(dwOldVnum) ? GenerateMountName(dwOldVnum) : "");
#else
			HorseSummon(true, false, dwOldVnum);
#endif
		}
		else
		{
			m_dwMountVnum = 0;
			ComputePoints();
			UpdatePacket();
		}

		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_HT, 0);
		PointChange(POINT_IQ, 0);

		return true;
	}

	return false;
}

EVENTFUNC(horse_dead_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "horse_dead_event> <Factor> Null pointer" );
		return 0;
	}

	// <Factor>
	LPCHARACTER ch = info->ch;
	if (ch == NULL) {
		return 0;
	}
	ch->HorseSummon(false);
	return 0;
}

void CHARACTER::SetRider(LPCHARACTER ch)
{
	if (m_chRider)
		m_chRider->ClearHorseInfo();

	m_chRider = ch;

	if (m_chRider)
		m_chRider->SendHorseInfo();
}

LPCHARACTER CHARACTER::GetRider() const
{
	return m_chRider;
}

void CHARACTER::HorseSummon(bool bSummon, bool bFromFar, DWORD dwVnum
#ifdef ENABLE_MOUNT_SYSTEM
, const std::string & pPetName, LPITEM mountItem
#else
, const char *pPetName
#endif
)
{
	if ( bSummon )
	{
		if( m_chHorse != NULL )
		{
			return;
		}

		if (GetHorseLevel() <= 0)
			return;

		if (IsRiding())
			return;

		#ifdef ENABLE_MOUNT_COSTUME_EX_SYSTEM
		if (GetMountVnum())
			return;
		#endif

		sys_log(0, "HorseSummon : %s lv:%d bSummon:%d fromFar:%d", GetName(), GetLevel(), bSummon, bFromFar);

		long x = GetX();
		long y = GetY();

		if (GetHorseHealth() <= 0)
			bFromFar = false;

		if (bFromFar)
		{
			x += (number(0, 1) * 2 - 1) * number(2000, 2500);
			y += (number(0, 1) * 2 - 1) * number(2000, 2500);
		}
		else
		{
			x += number(-100, 100);
			y += number(-100, 100);
		}

		m_chHorse = CHARACTER_MANAGER::instance().SpawnMob(
				(0 == dwVnum) ? GetMyHorseVnum() : dwVnum,
				GetMapIndex(),
				x, y,
				GetZ(), false, (int)(GetRotation()+180), false);

		if (!m_chHorse)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말 소환에 실패하였습니다."));
			return;
		}

		if (GetHorseHealth() <= 0)
		{
			m_chHorse->SetPosition(POS_DEAD);

			char_event_info* info = AllocEventInfo<char_event_info>();
			info->ch = this;
			m_chHorse->m_pkDeadEvent = event_create(horse_dead_event, info, PASSES_PER_SEC(60));
		}

#ifdef ENABLE_MOUNT_SYSTEM
		m_chHorse->SetLevel((IsNormalHorse(m_chHorse->GetRaceNum())) ? GetHorseLevel() : 30);
		const char* pHorseName = pPetName.size() ? pPetName.c_str() : CHorseNameManager::instance().GetHorseName(GetPlayerID());
#else
		m_chHorse->SetLevel(GetHorseLevel());
		const char *pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());
#endif

		if ( pHorseName != NULL && strlen(pHorseName) != 0 )
		{
			m_chHorse->m_stName = pHorseName;
		}
		else
		{
			m_chHorse->m_stName = GetName();
#if defined(__BL_MULTI_LANGUAGE__)
			m_chHorse->m_stName += " - @NPCNAME@";
#else
			m_chHorse->m_stName += LC_TEXT("님의 말");
#endif
		}

		if (!m_chHorse->Show(GetMapIndex(), x, y, GetZ()))
		{
			M2_DESTROY_CHARACTER(m_chHorse);
			sys_err("cannot show monster");
			m_chHorse = NULL;
			return;
		}

		if ((GetHorseHealth() <= 0))
		{
			TPacketGCDead pack;
			pack.header	= HEADER_GC_DEAD;
			pack.vid    = m_chHorse->GetVID();
			PacketAround(&pack, sizeof(pack));
		}

		m_chHorse->SetRider(this);
#ifdef ENABLE_MOUNT_SYSTEM
		if (mountItem)
		{
			SaveMountItem(mountItem);
		}
#endif
	}
	else
	{
		if (!m_chHorse)
			return;

		LPCHARACTER chHorse = m_chHorse;

		chHorse->SetRider(NULL); // m_chHorse assign to NULL

		if ((GetHorseHealth() <= 0))
			bFromFar = false;

		if (!bFromFar)
		{
			M2_DESTROY_CHARACTER(chHorse);
		}
		else
		{
			chHorse->SetNowWalking(false);
			float fx, fy;
			chHorse->SetRotation(GetDegreeFromPositionXY(chHorse->GetX(), chHorse->GetY(), GetX(), GetY())+180);
			GetDeltaByDegree(chHorse->GetRotation(), 3500, &fx, &fy);
			chHorse->Goto((long)(chHorse->GetX()+fx), (long) (chHorse->GetY()+fy));
			chHorse->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
		}

		m_chHorse = NULL;

	}
}

DWORD CHARACTER::GetMyHorseVnum() const
{
	int delta = 0;

	if (GetGuild())
	{
		++delta;

		if (GetGuild()->GetMasterPID() == GetPlayerID())
			++delta;
	}

	return c_aHorseStat[GetHorseLevel()].iNPCRace + delta;
}

void CHARACTER::HorseDie()
{
	CHorseRider::HorseDie();
	HorseSummon(false);
}

bool CHARACTER::ReviveHorse()
{
	if (CHorseRider::ReviveHorse())
	{
		HorseSummon(false);
		HorseSummon(true);
		return true;
	}
	return false;
}

void CHARACTER::ClearHorseInfo()
{
	if (!IsHorseRiding())
	{
		ChatPacket(CHAT_TYPE_COMMAND, "hide_horse_state");

		m_bSendHorseLevel = 0;
		m_bSendHorseHealthGrade = 0;
		m_bSendHorseStaminaGrade = 0;
	}

	m_chHorse = NULL;
}

void CHARACTER::SendHorseInfo()
{
	if (m_chHorse || IsHorseRiding())
	{
		int iHealthGrade;
		int iStaminaGrade;

		if (GetHorseHealth() == 0)
			iHealthGrade = 0;
		else if (GetHorseHealth() * 10 <= GetHorseMaxHealth() * 3)
			iHealthGrade = 1;
		else if (GetHorseHealth() * 10 <= GetHorseMaxHealth() * 7)
			iHealthGrade = 2;
		else
			iHealthGrade = 3;

		if (GetHorseStamina() * 10 <= GetHorseMaxStamina())
			iStaminaGrade = 0;
		else if (GetHorseStamina() * 10 <= GetHorseMaxStamina() * 3)
			iStaminaGrade = 1;
		else if (GetHorseStamina() * 10 <= GetHorseMaxStamina() * 7)
			iStaminaGrade = 2;
		else
			iStaminaGrade = 3;

		if (m_bSendHorseLevel != GetHorseLevel() ||
				m_bSendHorseHealthGrade != iHealthGrade ||
				m_bSendHorseStaminaGrade != iStaminaGrade)
		{
			ChatPacket(CHAT_TYPE_COMMAND, "horse_state %d %d %d", GetHorseLevel(), iHealthGrade, iStaminaGrade);

			m_bSendHorseLevel = GetHorseLevel();
			m_bSendHorseHealthGrade = iHealthGrade;
			m_bSendHorseStaminaGrade = iStaminaGrade;
		}
	}
}

bool CHARACTER::CanUseHorseSkill()
{
	if(IsRiding())
	{
		if (GetHorseGrade() == 3)
			return true;
		else
			return false;

		if(GetMountVnum())
		{
			if (GetMountVnum() >= 20209 && GetMountVnum() <= 20212)
				return true;

			if (CMobVnumHelper::IsRamadanBlackHorse(GetMountVnum()))
				return true;
		}
		else
			return false;

	}

	return false;
}

void CHARACTER::SetHorseLevel(int iLevel)
{
	CHorseRider::SetHorseLevel(iLevel);
	SetSkillLevel(SKILL_HORSE, GetHorseLevel());
}


#ifdef ENABLE_MOUNT_SYSTEM
bool CHARACTER::IsNormalHorse(DWORD dwHorseID) const
{
	return (dwHorseID >= 20101 && dwHorseID <= 20109);
}

std::string CHARACTER::GenerateMountName(DWORD dwMountVnum)
{
	auto pMob = CMobManager::instance().Get(dwMountVnum);
	return (pMob) ? std::string(GetName()) + "'s " + pMob->m_table.szLocaleName : "UNKNOWN";
}

void CHARACTER::SaveMountItem(LPITEM item)
{
	// Check if IsPC() if you don't have a fixme at SetQuestFlag for this check
	SetQuestFlag("mount_system.mount_item", item->GetID());
	item->Lock(true);
	item->SetSocket(2, true);
}

LPITEM CHARACTER::GetMountItem()
{
	return (GetQuestFlag("mount_system.mount_item")) ? ITEM_MANAGER::instance().Find(GetQuestFlag("mount_system.mount_item")) : nullptr;
}

void CHARACTER::ClearMountItem()
{
	auto pItem = GetMountItem();
	if (pItem)
	{
		pItem->Lock(false);
		pItem->SetSocket(2, false);
	}

	SetQuestFlag("mount_system.mount_item", 0);
}

void CHARACTER::GetSpecialMount(DWORD & dwVnum, bool bTakeOut)
{
	// In case if no mount is binded, do not alter value
	if (!GetQuestFlag("mount_system.mount_item"))
	{
		return;
	}

	auto pItem = GetMountItem();

	// In case if item has not been found, clear cache and do not alter value
	if (!pItem)
	{
		ClearMountItem();
		return;
	}

	// If it is normal horse, return value corresponding to standard function
	// if (!CMobManager::instance().Get(pItem->GetValue(0)))
	if (!CMobManager::instance().Get(pItem->FindApplyValue(APPLY_MOUNT)))
	{
		dwVnum = GetMyHorseVnum();
		return;
	}

	// Check if mask is applied; skip it if argument is provided
	auto pWearItem = (!bTakeOut) ? GetWear(WEAR_MOUNT) : nullptr;

	// alter value corresponding to mask status
	// dwVnum = (pWearItem) ? pWearItem->GetValue(0) : pItem->GetValue(0);
	dwVnum = (pWearItem) ? pWearItem->FindApplyValue(APPLY_MOUNT) : pItem->FindApplyValue(APPLY_MOUNT);
}

void CHARACTER::RefreshMount(bool bTakeOut)
{
	DWORD dwVnum = GetMyHorseVnum();
	GetSpecialMount(dwVnum, bTakeOut);

	// If horse is only summoned, resummon new
	if (GetHorse())
	{
		HorseSummon(false, false, 0, "");
		HorseSummon(true, false, dwVnum, !IsNormalHorse(dwVnum) ? GenerateMountName(dwVnum) : "", GetMountItem());
	}
	// If pc's mounted, remount it
	else if (GetMountVnum())
	{
		MountVnum(dwVnum);
	}
}

void CHARACTER::SummonMount()
{
	LPITEM pItem = GetMountItem();
	if (!pItem)
	{
		return;
	}

	DWORD dwMountVnum = GetMyHorseVnum();
	// if (CMobManager::instance().Get(pItem->GetValue(0)))
	if (CMobManager::instance().Get(pItem->FindApplyValue(APPLY_MOUNT)))
		// Try to apply mask in case of custom item
	{
		GetSpecialMount(dwMountVnum);
	}

	HorseSummon(true, false, dwMountVnum, GenerateMountName(dwMountVnum), pItem);
}

void CHARACTER::ApplyMountBonus()
{
	// Skip for normal horses
	if (!GetMountVnum() || IsNormalHorse(GetMountVnum()))
	{
		return;
	}
	
	if (m_dwMountVnum == 0)
		return;
	
	// ChatPacket(CHAT_TYPE_INFO, "applied mount bonuses");

	LPITEM pMountItem = GetWear(WEAR_MOUNT);
	LPITEM pMountSeal = GetMountItem();

	if (!pMountSeal)
	{
		return;
	}

	// Applies
	for (const auto & pItem : {pMountItem, pMountSeal})
	{
		if (!pItem)
		{
			continue;
		}

		auto pItemTable = pItem->GetProto();
		for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		{
			if (pItemTable->aApplies[i].bType != APPLY_NONE)
			{
				ApplyPoint(pItemTable->aApplies[i].bType, pItemTable->aApplies[i].lValue);
			}
		}

		// Attrs
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pItem->GetAttributeType(i) != POINT_NONE)
			{
				ApplyPoint(pItem->GetAttributeType(i), pItem->GetAttributeValue(i));
			}
		}
	}
}
#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
