#pragma once

class CDailyBoss : public singleton<CDailyBoss>
{
	enum DailyBossSettings
	{
		// if you set minutes lower than 10 ex. 09 then in quest and at notice will show ex. 18:5, so please set minutes higher than 9 
		// set here time of boss spawn
		ANNOUNCEMENT_1_H = 20,
		ANNOUNCEMENT_1_M = 00,
		ANNOUNCEMENT_2_H = 21,
		ANNOUNCEMENT_2_M = 15,
		ANNOUNCEMENT_3_H = 21,
		ANNOUNCEMENT_3_M = 40,
		ANNOUNCEMENT_4_H = 22,
		ANNOUNCEMENT_4_M = 25,

		START_HOUR_NERIUS_1 = 12,
		// START_HOUR_NERIUS_2 = 19,
		// START_HOUR_NERIUS_3 = 23,
		START_HOUR_KAOM_1 = 20,
		// START_HOUR_KAOM_2 = 20,
		// START_HOUR_KAOM_3 = 0,
		// START_HOUR_AVARIUS_1 = 18,
		// START_HOUR_AVARIUS_2 = 21,
		// START_HOUR_AVARIUS_3 = 01,
		// START_HOUR_ODEGON = 22,
		START_MINUTE = 00,

		// spawn info
		NERIUS_VNUM = 60004,
		KAOM_VNUM = 60005,
		// AVARIUS_VNUM = 2598,
		// ODEGON_VNUM = 692,

		MAP_INDEX = 51,
		SPAWN_X = 242,
		SPAWN_Y = 891,

		// system days settigns
		MONDAY = 1,
		TUESDAY = 2,
		WEDNESDAY = 3,
		THURSDAY = 4,
		FRIDAY = 5,
		SATURDAY = 6,
		SUNDAY = 0,

		// if you want boss every day then set here true
		ENABLE_DAILY = true,
		ENABLE_BONUS_REWARD = true,
		BONUS_REWARD_TIME = 120, // time in minutes

		REWARD_EMBLEM_VNUM = 65707, //Emblem ID
		REWARD_CHEST_VNUM = 65704, 	//Chest ID
		REWARD_CHEST_VNUM_2 = 65704, //Chest ID
		REWARD_CHEST_VNUM_3 = 65704, //Chest ID
		REWARD_CHEST_VNUM_4 = 65704, //Chest ID

		// if not, please choose day from "system days settings", default is sunday
		CHOOSEN_DAY = SUNDAY,
	};
public:
	CDailyBoss();
	virtual ~CDailyBoss();

	bool Initialize();
	void Destroy();

	void Check(int day, int hour, int minute, int second);
	int GetNextBoss(int hour, int minute, int second);
	bool IsDailyBoss(DWORD mobVnum);
	// int SubBossVnum() { return SUB_BOSS_VNUM; }
	DWORD GetNextBossVnum(int type);
	bool IsBossMap(int mapIndex);
	void OnKill(LPCHARACTER pkKiller, DWORD dwVID, DWORD mobVnum);

	// DWORD GetOdegonVnum() { return ODEGON_VNUM; }
	int GetBonusTime() { return BONUS_REWARD_TIME; }
	int GetEmblemVnum() { return REWARD_EMBLEM_VNUM; }
	int GetChestVnum(DWORD mobVnum);

	bool IsDay();
	const char * GetName(DWORD mobVnum) const;
	const char * GetLeftTime();

	bool EnterAttender(LPCHARACTER ch);
	bool CheckIpAddress(LPCHARACTER ch);
	void RemoveFromAttenderList(DWORD dwPID);

private:
	//void SendNoticeLine(const char * format, ...);
	bool Spawn(DWORD mobVnum);
	void GiveReward(DWORD dwVID, DWORD mobVnum);

	std::map<DWORD, DWORD> m_map_attender;
};
