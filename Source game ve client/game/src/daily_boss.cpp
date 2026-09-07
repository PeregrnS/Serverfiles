#include "stdafx.h"

#ifdef ENABLE_DAILY_BOSS
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "utils.h"
#include "config.h"
#include "constants.h"
#include "char.h"
#include "char_manager.h"
#include "sectree_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "db.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "auto_event_manager.h"
#include "daily_boss.h"

CDailyBoss::CDailyBoss() {}

CDailyBoss::~CDailyBoss() {}

bool CDailyBoss::Initialize()
{
    m_map_attender.clear();
    return true;
}

void CDailyBoss::Destroy()
{
    m_map_attender.clear();
}

struct notice_global_temporary_packet_func
{
	const char* m_str;
	const char* m_arg;

	notice_global_temporary_packet_func(const char* str, const char* arg) : m_str(str), m_arg(arg) {}

	void operator()(LPDESC d)
	{
		if (!d->GetCharacter())
		{
			return;
		}

		std::string nmsg = LC_TEXT(m_str);
		d->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, "%s|cffcd9bff%s|r", nmsg.c_str(), m_arg);
	}
};

void SendNoticeGlobalTemporaryFunc(const char* c_pszBuf, const char* arg)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_global_temporary_packet_func(c_pszBuf, arg));
}

void CDailyBoss::Check(int day, int hour, int minute, int second)
{
	if (ENABLE_DAILY == false && CHOOSEN_DAY != day)
	{
		return;
	}

	if (minute == 10 && second == 10)
	{
		SendNoticeGlobalTemporaryFunc("ANNOUCEMENT_LINE_1", GetName(GetNextBossVnum(GetNextBoss(hour, minute, second))));
		SendNoticeGlobalTemporaryFunc("ANNOUCEMENT_LINE_2", GetLeftTime());
		time_t cur_Time = time(NULL);
		struct tm *localTime = localtime(&cur_Time);
		SendNoticeGlobalTemporaryFunc("ANNOUCEMENT_LINE_3", asctime(localTime));
		// SendNotice("ANNOUCEMENT_LINE_4");
	}

	bool isBossHour = false;

	switch (hour)
	{
		case START_HOUR_NERIUS_1:
		// case START_HOUR_NERIUS_2:
		// case START_HOUR_NERIUS_3:
		case START_HOUR_KAOM_1:
		// case START_HOUR_KAOM_2:
		// case START_HOUR_KAOM_3:
		// case START_HOUR_AVARIUS_1:
		// case START_HOUR_AVARIUS_2:
		// case START_HOUR_AVARIUS_3:
		// case START_HOUR_ODEGON:
			isBossHour = true;
	}

	if (isBossHour && minute == START_MINUTE && second == 10)
	{
		if (CDailyBoss::Spawn(GetNextBossVnum(GetNextBoss(hour, minute, second))) == true)
		{
			SendNotice("BOSS_SPAWNED_LINE_1");
			SendNotice("BOSS_SPAWNED_LINE_2");
		}
		// else
		// {
		// 	SendNotice("SPAWN_ERROR");
		// }
	}
}

int CDailyBoss::GetNextBoss(int hour, int minute, int second)
{
	int mobIndex = 0;
	// if ((hour == START_HOUR_AVARIUS_3 && minute > 30) ||
	// 	(hour == START_HOUR_NERIUS_1 && minute <= 30) ||
	// 	(hour == START_HOUR_NERIUS_1 && minute == 30 && second == 10) ||
	// 	(hour > START_HOUR_AVARIUS_3 && hour < START_HOUR_NERIUS_1))
	// 	mobIndex = 0;

	if ((hour == START_HOUR_NERIUS_1 && minute <= 00) ||
		(hour == START_HOUR_NERIUS_1 && minute == 00 && second == 10) ||
		(hour < START_HOUR_NERIUS_1))
		mobIndex = 0;

	// else if ((hour == START_HOUR_AVARIUS_1 && minute > 30) ||
	// 	(hour == START_HOUR_NERIUS_2 && minute <= 30) ||
	// 	(hour == START_HOUR_NERIUS_2 && minute == 30 && second == 10) ||
	// 	(hour > START_HOUR_AVARIUS_1 && hour < START_HOUR_NERIUS_2))
	// 	mobIndex = 1;
	
	// else if ((hour == START_HOUR_ODEGON && minute > 30) ||
	// 	(hour == START_HOUR_NERIUS_3 && minute <= 30) ||
	// 	(hour == START_HOUR_NERIUS_3 && minute == 30 && second == 10) ||
	// 	(hour > START_HOUR_ODEGON && hour < START_HOUR_NERIUS_3))
	// 	mobIndex = 2;

	else if ((hour == START_HOUR_NERIUS_1 && minute > 00) ||
		(hour == START_HOUR_KAOM_1 && minute <= 00) ||
		(hour == START_HOUR_KAOM_1 && minute == 00 && second == 10) ||
		(hour > START_HOUR_NERIUS_1 && hour < START_HOUR_KAOM_1))
		mobIndex = 1;
	
	// else if ((hour == START_HOUR_NERIUS_2 && minute > 30) ||
	// 	(hour == START_HOUR_KAOM_2 && minute <= 30) ||
	// 	(hour == START_HOUR_KAOM_2 && minute == 30 && second == 10) ||
	// 	(hour > START_HOUR_NERIUS_2 && hour < START_HOUR_KAOM_2))
	// 	mobIndex = 4;

	// else if ((hour == START_HOUR_NERIUS_3 && minute > 30) ||
	// 	(hour == START_HOUR_KAOM_3 && minute <= 30) ||
	// 	(hour == START_HOUR_KAOM_3 && minute == 30 && second == 10) ||
	// 	(hour > START_HOUR_NERIUS_3 && hour < START_HOUR_KAOM_3))
	// 	mobIndex = 5;
	
	// else if ((hour == START_HOUR_KAOM_1 && minute > 30) ||
	// 	(hour == START_HOUR_AVARIUS_1 && minute <= 30) ||
	// 	(hour == START_HOUR_AVARIUS_1 && minute == 30 && second == 10) ||
	// 	(hour > START_HOUR_KAOM_1 && hour < START_HOUR_AVARIUS_1))
	// 	mobIndex = 6;
	
	// else if ((hour == START_HOUR_KAOM_2 && minute > 30) ||
	// 	(hour == START_HOUR_AVARIUS_2 && minute <= 30) ||
	// 	(hour == START_HOUR_AVARIUS_2 && minute == 30 && second == 10) ||
	// 	(hour > START_HOUR_KAOM_2 && hour < START_HOUR_AVARIUS_2))
	// 	mobIndex = 7;
	
	// else if ((hour == START_HOUR_KAOM_3 && minute > 30) ||
	// 	(hour == START_HOUR_AVARIUS_3 && minute <= 30) ||
	// 	(hour == START_HOUR_AVARIUS_3 && minute == 30 && second == 10) ||
	// 	(hour > START_HOUR_KAOM_3 && hour < START_HOUR_AVARIUS_3))
	// 	mobIndex = 8;
	else
		mobIndex = 1;

	return mobIndex;
}

DWORD CDailyBoss::GetNextBossVnum(int type)
{
	switch (type)
	{
		case 0:
		// case 1:
		// case 2:
			return NERIUS_VNUM;
		case 1:
		// case 4:
		// case 5:
			return KAOM_VNUM;
		// case 6:
		// case 7:
		// case 8:
		// 	return AVARIUS_VNUM;
		default:
			return 0;
	}
}

bool CDailyBoss::IsDailyBoss(DWORD mobVnum)
{
	switch (mobVnum)
	{
		case NERIUS_VNUM:
		case KAOM_VNUM:
		// case ODEGON_VNUM:
		// case AVARIUS_VNUM:
			return true;
		default:
			return false;
	}
}

char* CatchMessage(const char *message, ...)
{
    static char result[256];
    va_list ap;
    va_start(ap, message);
    vsnprintf(result, sizeof result, message, ap);
    va_end(ap);
    return result;
}

const char* CDailyBoss::GetLeftTime()
{
	time_t cur_Time;
	struct tm *timeNow;
	time(&cur_Time);
	timeNow = localtime(&cur_Time);
	int tempHour = 0;

	switch(GetNextBoss(timeNow->tm_hour, timeNow->tm_min, timeNow->tm_sec))
	{
		case 0:
			tempHour = START_HOUR_NERIUS_1;
			break;
		// case 1:
		// 	tempHour = START_HOUR_NERIUS_2;
		// 	break;
		// case 2:
		// 	tempHour = START_HOUR_NERIUS_3;
		// 	break;
		case 1:
			tempHour = START_HOUR_KAOM_1;
			break;
		// case 4:
		// 	tempHour = START_HOUR_KAOM_2;
		// 	break;
		// case 5:
		// 	tempHour = START_HOUR_KAOM_3;
		// 	break;
		// case 6:
		// 	tempHour = START_HOUR_AVARIUS_1;
		// 	break;
		// case 7:
		// 	tempHour = START_HOUR_AVARIUS_2;
		// 	break;
		// case 8:
		// 	tempHour = START_HOUR_AVARIUS_3;
		// 	break;
		default:
			tempHour = START_HOUR_NERIUS_1;
	}

	struct tm *spawnTime = localtime(&cur_Time);
	tempHour = tempHour - timeNow->tm_hour;
	if (tempHour < 0)
	{
		spawnTime->tm_hour = 0;
		spawnTime->tm_min = START_MINUTE + 59 - timeNow->tm_min;
	}
	else
	{
		spawnTime->tm_min = START_MINUTE - timeNow->tm_min;
		spawnTime->tm_hour = tempHour;
		if (spawnTime->tm_min < 0)
		{
			spawnTime->tm_min = 60 + spawnTime->tm_min;
			spawnTime->tm_hour = spawnTime->tm_hour - 1;
		}
	}
	spawnTime->tm_sec = 60 - timeNow->tm_sec;
	char* leftTime = CatchMessage("%02d:%02d:%02d", spawnTime->tm_hour, spawnTime->tm_min, spawnTime->tm_sec, tempHour);
	return leftTime;
}

const char* CDailyBoss::GetName(DWORD mobVnum) const
{
	std::string m_stName;
	const CMob * pkMob = CMobManager::instance().Get(mobVnum);
	return m_stName.empty() ? (pkMob ? pkMob->m_table.szLocaleName : "NONE") : m_stName.c_str();
}

bool CDailyBoss::Spawn(DWORD mobVnum)
{
	SECTREE_MANAGER::instance().PurgeMonstersInMap(MAP_INDEX);

	if (map_allow_find(MAP_INDEX))
	{
		PIXEL_POSITION posBase;
		if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(MAP_INDEX, posBase))
		{
			sys_err("cannot get map base position %d", MAP_INDEX);
			return false;
		}

		CHARACTER_MANAGER::instance().SpawnMob(mobVnum, MAP_INDEX, posBase.x + SPAWN_X * 100, posBase.y + SPAWN_Y * 100, 0, false, -1);

		// for (unsigned int i=0; i<25; i++)
		// {
			// CHARACTER_MANAGER::instance().SpawnMobRandomPosition(SUB_BOSS_VNUM, MAP_INDEX);
		// }
		// return true;
	}

	return false;
}

bool CDailyBoss::IsBossMap(int mapIndex)
{
	if (mapIndex == MAP_INDEX)
	{
		return true;
	}
	return false;
}

void CDailyBoss::OnKill(LPCHARACTER pkKiller, DWORD dwVID, DWORD mobVnum)
{
	// if (mobVnum == ODEGON_VNUM)
	// {
	// 	SendNotice("KILLED"); 
	// }
	// else
	// {
	SendNotice("KILLED2");
	// }
	CDailyBoss::GiveReward(dwVID, mobVnum);
}

struct FReward
{
	DWORD dwVID;
	DWORD dwMobVnum;

	void operator() (LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER)ent;

			if (ch && ch->GetDesc() && ch->IsPC())
			{
				// if (dwMobVnum == CDailyBoss::Instance().GetOdegonVnum())
				// {
				// 	ch->ChatPacket(CHAT_TYPE_INFO, "Your damage for %s is %d and the required is %d of 400.000.", CDailyBoss::Instance().GetName(dwMobVnum), ch->GetDamageByVid(dwVID)/1000, ch->GetDamageByVid(dwVID)%1000);
				// 	if (ch->GetDamageByVid(dwVID) > 400000)
				// 	{
				// 		ch->AutoGiveItem(CDailyBoss::Instance().GetChestVnum(dwMobVnum), 1);
				// 	}
				// 	// std::unique_ptr<SQLMsg>msg(DBManager::instance().DirectQuery("UPDATE account.account SET silver_expire = DATE_ADD( ( CASE WHEN silver_expire > NOW( ) THEN silver_expire ELSE NOW( ) END ), INTERVAL '%d' MINUTE ), money_drop_rate_expire = DATE_ADD( ( CASE WHEN money_drop_rate_expire > NOW( ) THEN money_drop_rate_expire ELSE NOW( ) END ), INTERVAL '%d' MINUTE ) WHERE id = '%d';", CDailyBoss::instance().GetBonusTime(), CDailyBoss::instance().GetBonusTime(), ch->GetDesc()->GetAccountTable().id));
				// 	ch->ChatPacket(CHAT_TYPE_INFO, "You received VIP for %d minutes for participe in this event. Please login again to activate bonus.", CDailyBoss::instance().GetBonusTime());
				// }
				// else
				// {
				ch->ChatPacket(CHAT_TYPE_INFO, "Your damage for %s is %d and the required is %d of 100.000.", CDailyBoss::Instance().GetName(dwMobVnum), ch->GetDamageByVid(dwVID)/1000, ch->GetDamageByVid(dwVID)%1000);
				if (ch->GetDamageByVid(dwVID) > 100000)
				{
					ch->AutoGiveItem(CDailyBoss::Instance().GetChestVnum(dwMobVnum), 1);
				}
				// }
			}
		}
	}
};

int CDailyBoss::GetChestVnum(DWORD mobVnum)
{
	switch (mobVnum)
	{
		case NERIUS_VNUM:
			return REWARD_CHEST_VNUM_2;
		case KAOM_VNUM:
			return REWARD_CHEST_VNUM_3;
		// case AVARIUS_VNUM:
		// 	return REWARD_CHEST_VNUM_4;
		// case ODEGON_VNUM:
		// 	return REWARD_CHEST_VNUM;
		default:
			return REWARD_CHEST_VNUM;
	}
}

void CDailyBoss::GiveReward(DWORD dwVID, DWORD mobVnum)
{
	LPSECTREE_MAP sectree = SECTREE_MANAGER::instance().GetMap(MAP_INDEX);

	if (sectree == NULL)
	{
		return;
	}

	if (ENABLE_BONUS_REWARD == true)
	{
		struct FReward f;
		f.dwVID = dwVID;
		f.dwMobVnum = mobVnum;
		sectree->for_each(f);
		SendNotice("NOTICE_ABOUT_VIP_REWARD");
	}
}

bool CDailyBoss::IsDay()
{
	time_t curTime = time(NULL);
	struct tm vKey = *localtime(&curTime);

	if (DailyBossSettings::ENABLE_DAILY == false && vKey.tm_wday != DailyBossSettings::CHOOSEN_DAY)
	{
		return false;
	}

	return true;
}

bool CDailyBoss::CheckIpAddress(LPCHARACTER ch)
{
    for (auto it = m_map_attender.begin(); it != m_map_attender.end(); ++it)
    {
		LPCHARACTER tch = CHARACTER_MANAGER::Instance().FindByPID(it->second);
		if (!tch || !tch->GetDesc())
		{
	    	continue;
		}

		if (!strcmp(ch->GetDesc()->GetHostName(), tch->GetDesc()->GetHostName()) && ch->GetMapIndex() == tch->GetMapIndex())
		{
			ch->GoHome();
			return false;
		}
    }
    
    return true;
}

bool CDailyBoss::EnterAttender(LPCHARACTER pkChar)
{
    DWORD pid = pkChar->GetPlayerID();
    if (CheckIpAddress(pkChar))
	{
		m_map_attender.insert(std::make_pair(pid, pid));
		return true;
    }
    return false;
}

void CDailyBoss::RemoveFromAttenderList(DWORD dwPID)
{
    m_map_attender.erase(dwPID);
}

namespace quest
{
	ALUA(dailyboss_is_map)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pkChar && CDailyBoss::instance().IsBossMap(pkChar->GetMapIndex()) == true)
		{
			lua_pushboolean(L, true);
		}
		else
		{
			lua_pushboolean(L, false);
		}
		return 1;
	}

	ALUA(dailyboss_is_day)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pkChar && CDailyBoss::instance().IsDay() == true)
		{
			lua_pushboolean(L, true);
		}
		else
		{
			lua_pushboolean(L, false);
		}
		return 1;
	}

	ALUA(dailyboss_get_left_time)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pkChar)
		{
			lua_pushstring(L, CDailyBoss::instance().GetLeftTime());
		}
		else
		{
			lua_pushstring(L, "00:00:00");
		}
		return 1;
	}

	ALUA(dailyboss_get_next_boss_index)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pkChar)
		{
			time_t cur_Time = time(NULL);
			struct tm *localTime = localtime(&cur_Time);
			lua_pushnumber(L, CDailyBoss::instance().GetNextBoss(localTime->tm_hour, localTime->tm_min, localTime->tm_sec));
		}
		return 1;
	}

	ALUA(dailyboss_get_next_boss_name)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pkChar)
		{
			time_t cur_Time = time(NULL);
			struct tm *localTime = localtime(&cur_Time);
			lua_pushstring(L, CDailyBoss::instance().GetName(CDailyBoss::instance().GetNextBossVnum(CDailyBoss::instance().GetNextBoss(localTime->tm_hour, localTime->tm_min, localTime->tm_sec))));
		}
		return 1;
	}

	void RegisterDailBossFunctionTable()
	{
		luaL_reg dailyboss_functions[] =
		{
			{ "is_dailyboss_map", dailyboss_is_map },
			{ "is_day", dailyboss_is_day },
			{ "get_left_time", dailyboss_get_left_time },
			{ "get_next_boss_index", dailyboss_get_next_boss_index },
			{ "get_next_boss_name", dailyboss_get_next_boss_name },
			{ NULL, NULL }
		};
		CQuestManager::instance().AddLuaFunctionTable("dailyboss", dailyboss_functions);
	}
}

#endif
