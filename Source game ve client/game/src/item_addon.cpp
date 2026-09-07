#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "item.h"
#include "item_addon.h"
#ifdef ENABLE_GLOBAL_REWARD
	#include "char_manager.h"
#endif
#ifdef ENABLE_REWARD_SYSTEM
	#include "char_manager.h"
	#include "char.h"
#endif

CItemAddonManager::CItemAddonManager()
{
}

CItemAddonManager::~CItemAddonManager()
{
}

#ifdef ENABLE_DEVILAY_LANGSAMERSWITCHEN

static bool KeepRoll(int keep, int total)
{
	return number(1, total) <= keep;
}

static bool IsHardWeaponVnum(DWORD vnum)
{
	switch (vnum)
	{
		case 172650:
		case 172651:
		case 172652:
		case 172653:
		case 172654:
		case 172655:
		case 172656:
		case 172657:
		case 172658:
		case 172659:

		case 172660:
		case 172661:
		case 172662:
		case 172663:
		case 172664:
		case 172665:
		case 172666:
		case 172667:
		case 172668:
		case 172669:

		case 172670:
		case 172671:
		case 172672:
		case 172673:
		case 172674:
		case 172675:
		case 172676:
		case 172677:
		case 172678:
		case 172679:

		case 172680:
		case 172681:
		case 172682:
		case 172683:
		case 172684:
		case 172685:
		case 172686:
		case 172687:
		case 172688:
		case 172689:

		case 172690:
		case 172691:
		case 172692:
		case 172693:
		case 172694:
		case 172695:
		case 172696:
		case 172697:
		case 172698:
		case 172699:

		case 172700:
		case 172701:
		case 172702:
		case 172703:
		case 172704:
		case 172705:
		case 172706:
		case 172707:
		case 172708:
		case 172709:
			return true;
	}

	return false;
}

void CItemAddonManager::ApplyAddonTo(int iAddonType, LPITEM pItem)
{
	if (!pItem)
	{
		sys_err("ITEM pointer null");
		return;
	}

	const bool bHardWeapon = IsHardWeaponVnum(pItem->GetVnum());

	int iSkillBonus = 0;
	int iNormalHitBonus = 0;

	while (true)
	{
		iSkillBonus = MINMAX(-30, (int)(gauss_random(0, 5) + 0.5f), 30);

		if (abs(iSkillBonus) <= 20)
			iNormalHitBonus = -2 * iSkillBonus + abs(number(-8, 8) + number(-8, 8)) + number(1, 4);
		else
			iNormalHitBonus = -2 * iSkillBonus + number(1, 5);

		iSkillBonus = MINMAX(-30, iSkillBonus, 30);
		iNormalHitBonus = MIN(iNormalHitBonus, 60);

		// =========================================================
		// FKS BALANCING
		// normale Waffen = maximal leicht
		// Platin-Waffen   = schwer
		// =========================================================
		if (iSkillBonus >= 28 && iSkillBonus <= 30)
		{
			if (!KeepRoll(bHardWeapon ? 25 : 100, 100))
				continue;
		}
		else if (iSkillBonus >= 25 && iSkillBonus <= 27)
		{
			if (!KeepRoll(bHardWeapon ? 5 : 100, 100))
				continue;
		}
		else if (iSkillBonus >= 22 && iSkillBonus <= 24)
		{
			if (!KeepRoll(1, bHardWeapon ? 120 : 1))
				continue;
		}
		else if (iSkillBonus >= 20 && iSkillBonus <= 21)
		{
			if (!KeepRoll(1, bHardWeapon ? 300 : 1))
				continue;
		}
		else if (iSkillBonus >= 17 && iSkillBonus <= 19)
		{
			if (!KeepRoll(1, bHardWeapon ? 70 : 1))
				continue;
		}
		else if (iSkillBonus >= 15 && iSkillBonus <= 16)
		{
			if (!KeepRoll(1, bHardWeapon ? 12 : 1))
				continue;
		}
		else if (iSkillBonus >= 10 && iSkillBonus <= 14)
		{
			if (!KeepRoll(1, bHardWeapon ? 35 : 1))
				continue;
		}

		// =========================================================
		// NEGATIVER SKILLBONUS - für DSS gespiegelt
		// =========================================================
		if (iSkillBonus <= -28 && iSkillBonus >= -30)
		{
			if (!KeepRoll(bHardWeapon ? 25 : 100, 100))
				continue;
		}
		else if (iSkillBonus <= -25 && iSkillBonus >= -27)
		{
			if (!KeepRoll(bHardWeapon ? 5 : 100, 100))
				continue;
		}
		else if (iSkillBonus <= -22 && iSkillBonus >= -24)
		{
			if (!KeepRoll(1, bHardWeapon ? 120 : 1))
				continue;
		}
		else if (iSkillBonus <= -20 && iSkillBonus >= -21)
		{
			if (!KeepRoll(1, bHardWeapon ? 300 : 1))
				continue;
		}
		else if (iSkillBonus <= -17 && iSkillBonus >= -19)
		{
			if (!KeepRoll(1, bHardWeapon ? 70 : 1))
				continue;
		}
		else if (iSkillBonus <= -15 && iSkillBonus >= -16)
		{
			if (!KeepRoll(1, bHardWeapon ? 12 : 1))
				continue;
		}
		else if (iSkillBonus <= -10 && iSkillBonus >= -14)
		{
			if (!KeepRoll(1, bHardWeapon ? 35 : 1))
				continue;
		}

		// =========================================================
		// DSS BALANCING
		// normale Waffen = maximal leicht
		// Platin-Waffen   = schwer
		// =========================================================
		if (iNormalHitBonus >= 58 && iNormalHitBonus <= 60)
		{
			if (!KeepRoll(bHardWeapon ? 20 : 100, 100))
				continue;
		}
		else if (iNormalHitBonus >= 55 && iNormalHitBonus <= 57)
		{
			if (!KeepRoll(bHardWeapon ? 5 : 100, 100))
				continue;
		}
		else if (iNormalHitBonus >= 53 && iNormalHitBonus <= 54)
		{
			if (!KeepRoll(bHardWeapon ? 4 : 100, 100))
				continue;
		}
		else if (iNormalHitBonus >= 50 && iNormalHitBonus <= 52)
		{
			if (!KeepRoll(bHardWeapon ? 10 : 100, 100))
				continue;
		}
		else if (iNormalHitBonus >= 45 && iNormalHitBonus <= 49)
		{
			if (!KeepRoll(bHardWeapon ? 3 : 100, 100))
				continue;
		}
		else if (iNormalHitBonus >= 40 && iNormalHitBonus <= 44)
		{
			if (!KeepRoll(bHardWeapon ? 10 : 100, 100))
				continue;
		}
		else if (iNormalHitBonus >= 30 && iNormalHitBonus <= 35)
		{
			if (!KeepRoll(1, bHardWeapon ? 50 : 1))
				continue;
		}

		break;
	}

	pItem->RemoveAttributeType(APPLY_SKILL_DAMAGE_BONUS);
	pItem->RemoveAttributeType(APPLY_NORMAL_HIT_DAMAGE_BONUS);
	pItem->AddAttribute(APPLY_NORMAL_HIT_DAMAGE_BONUS, iNormalHitBonus);
	pItem->AddAttribute(APPLY_SKILL_DAMAGE_BONUS, iSkillBonus);

#ifdef ENABLE_REWARD_SYSTEM
	if (iNormalHitBonus >= 60 && pItem->GetLevelLimit() == 75)
	{
		LPCHARACTER ch = pItem->GetOwner();
		if (ch)
			CHARACTER_MANAGER::Instance().SetRewardData(REWARD_AVERAGE, ch->GetName(), true);
	}

	if (iNormalHitBonus >= 50 && pItem->GetLevelLimit() >= 115)
	{
		LPCHARACTER ch = pItem->GetOwner();
		if (ch)
			CHARACTER_MANAGER::Instance().SetRewardData(FIRST_AVERAGE_ZODIAC_50, ch->GetName(), true);
	}
#endif

#ifdef ENABLE_GLOBAL_REWARD
	if (iNormalHitBonus >= 45)
	{
		LPCHARACTER ch = pItem->GetOwner();
		if (ch)
			CHARACTER_MANAGER::Instance().DoReward(ch, REWARD_MISSION_AVERAGE_BONUS, iNormalHitBonus, 1);
	}
#endif
}

#else

void CItemAddonManager::ApplyAddonTo(int iAddonType, LPITEM pItem)
{
	if (!pItem)
	{
		sys_err("ITEM pointer null");
		return;
	}

	int iSkillBonus = MINMAX(-30, (int)(gauss_random(0, 5) + 0.5f), 30);
	int iNormalHitBonus = 0;

	if (abs(iSkillBonus) <= 20)
		iNormalHitBonus = -2 * iSkillBonus + abs(number(-8, 8) + number(-8, 8)) + number(1, 4);
	else
		iNormalHitBonus = -2 * iSkillBonus + number(1, 5);

	pItem->RemoveAttributeType(APPLY_SKILL_DAMAGE_BONUS);
	pItem->RemoveAttributeType(APPLY_NORMAL_HIT_DAMAGE_BONUS);
	pItem->AddAttribute(APPLY_NORMAL_HIT_DAMAGE_BONUS, iNormalHitBonus);
	pItem->AddAttribute(APPLY_SKILL_DAMAGE_BONUS, iSkillBonus);

#ifdef ENABLE_REWARD_SYSTEM
	if (iNormalHitBonus >= 60 && pItem->GetLevelLimit() == 75)
	{
		LPCHARACTER ch = pItem->GetOwner();
		if (ch)
			CHARACTER_MANAGER::Instance().SetRewardData(REWARD_AVERAGE, ch->GetName(), true);
	}

	if (iNormalHitBonus >= 50 && pItem->GetLevelLimit() >= 115)
	{
		LPCHARACTER ch = pItem->GetOwner();
		if (ch)
			CHARACTER_MANAGER::Instance().SetRewardData(FIRST_AVERAGE_ZODIAC_50, ch->GetName(), true);
	}
#endif

#ifdef ENABLE_GLOBAL_REWARD
	if (iNormalHitBonus >= 45)
	{
		LPCHARACTER ch = pItem->GetOwner();
		if (ch)
			CHARACTER_MANAGER::Instance().DoReward(ch, REWARD_MISSION_AVERAGE_BONUS, iNormalHitBonus, 1);
	}
#endif
}

#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc