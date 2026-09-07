#include "stdafx.h"
#ifdef __HIT_LIMITER_ENABLE__
#include "char.h"
#include "desc.h"
#include "utils.h"

namespace CHitLimiter
{
	std::map<DWORD, DWORD> m_hit_limits = {
											{MAIN_RACE_WARRIOR_M, 80},
											{MAIN_RACE_ASSASSIN_M, 80},
											{MAIN_RACE_SURA_M, 100},
											{MAIN_RACE_SHAMAN_M, 80},
											//
											{MAIN_RACE_WARRIOR_W, 80},
											{MAIN_RACE_ASSASSIN_W, 80},
											{MAIN_RACE_SURA_W, 100},
											{MAIN_RACE_SHAMAN_W, 80},
										};
	const DWORD HIT_UPDATE_CYCLE = 400;
	DWORD HIT_LIMIT_BUFF = 100; // default 50
	bool g_limiter_enable = true;

	const int iPolyLimit = 60;
	const int iLimitPerMonster = 15;

	DWORD GetVIDHitLimit()
	{
		return iLimitPerMonster*(static_cast<float>(HIT_UPDATE_CYCLE)/1000.0f);
	}

	DWORD GetHitLimit(LPCHARACTER ch)
	{
		if (ch->IsPolymorphed())
			return (iPolyLimit+HIT_LIMIT_BUFF)*(static_cast<float>(HIT_UPDATE_CYCLE)/1000.0f);

		auto fIt = m_hit_limits.find(ch->GetRaceNum());
		if (fIt != m_hit_limits.end())
			return (fIt->second+HIT_LIMIT_BUFF)*(static_cast<float>(HIT_UPDATE_CYCLE)/1000.0f);
		else
			return 0;
	}

	bool RegisterHit(LPCHARACTER ch, LPCHARACTER enemy)
	{
		if (!g_limiter_enable || !ch || !enemy || !ch->GetDesc())
			return true;

		if (!enemy->GetVID())
			return true;

		const DWORD & dwHitCount = ch->GetHitCount();
		const DWORD & dwNextUpdate = ch->GetHitNextUpdate();

		if (!ch->FindEnemyVID(enemy->GetVID()))
			ch->AppendEnemyVID(enemy->GetVID());

		if (!dwHitCount)
		{
			ch->SetHitCount(1);
			ch->UpdateEnemyHitCount(enemy->GetVID(), 1);

			ch->SetHitNextUpdate(get_dword_time()+HIT_UPDATE_CYCLE);
			return true;
		}

		ch->SetHitCount(dwHitCount+1);
		ch->UpdateEnemyHitCount(enemy->GetVID(), ch->GetEnemyVIDHitCount(enemy->GetVID())+1);

		if (get_dword_time() >= dwNextUpdate)
		{
			if (ch->GetHitCount() >= GetHitLimit(ch) || ch->GetEnemyVIDHitCount(enemy->GetVID()) >= GetVIDHitLimit())
			{
				sys_log(0, "Character %s reached hit limit! Score: %u", ch->GetName(), ch->GetHitCount());
				// ch->GetDesc()->SetPhase(PHASE_CLOSE);
				ch->ChatPacket(CHAT_TYPE_INFO, "%s hat Trefferlimit erreicht! Anazahl: %u", ch->GetName(), ch->GetHitCount());
				// reset
				ch->SetHitCount(0);
				ch->SetHitNextUpdate(0);
				return false;
			}
			else
			{
				ch->UpdateEnemyHitCount(enemy->GetVID(), 0);
				ch->SetHitCount(0);
				ch->ClearEnemyVids();
			}
		}

		return true;
	}
}
#endif

