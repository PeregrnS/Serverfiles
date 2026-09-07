// --
// Author: Grimmjock
// Date: 01.09.2020
// Last Modif: 01.05.2021
// Version: 0.2.1
// --

#include "../../common/length.h"
#include <boost/unordered_map.hpp>

class CHARACTER;

enum ERankByValue
{
	RANK_BY_LEVEL,
	RANK_BY_STONE,
	RANK_BY_MAX_DMG_STONE,
	RANK_BY_DUNGEON_COMPLETE,
	RANK_BY_BOSS,
	RANK_BY_MAX_DMG_BOSS,
	RANK_BY_MONSTER,
	RANK_BY_ITEM_IMPROVED,
	RANK_BY_PLAYING_TIME,
	RANK_BY_MISSION_BOOK_COMPLETE,
	RANK_BY_BOX_OPENED,
	RANK_BY_BONUS_CHANGED,
	RANK_BY_FISH_CAUGHT,
	RANK_BY_EXTRACTION,
	RANK_MAX,                 
};

enum EConfigRankPlayer
{
	MAX_LIMIT_GET_RANK_PLAYER = 50, // limit maximum load players from mysql
	SHOW_MAX_LIMIT = 10, // show maximum 10 players in top. (2 pages)
};

enum ERankingSubHeaders
{
	RANKING_SUBHEADER_FORCE_SAVE,
	RANKING_SUBHEADER_NEXT_RESET,
};

struct SInfoPlayer
{
	bool bLoaded;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	char szAccount[LOGIN_MAX_LEN + 1];
	BYTE bEmpire, bLevel;
	long long lYang;
	int iStone, iDmgStone, iDungeon, iBoss, iDmgBoss, iMonster, iImprove, iPlayingTime, iMissionBook, iBox , iBonus, iFish, iExtraction;
	SInfoPlayer()
	{
		bLoaded = false;
		memset(szName, 0, sizeof(szName));
		memset(szAccount, 0, sizeof(szAccount));
		bEmpire = 0;
		bLevel = 0;
		iStone = 0;
		iDmgStone = 0;
		iDungeon = 0;
		iBoss = 0;
		iDmgBoss = 0;
		iMonster = 0;
		iImprove = 0;
		iPlayingTime = 0;
		iMissionBook = 0;
		iBox = 0;
		iBonus = 0;
		iFish = 0;
		iExtraction = 0;
	}
};

class RankPlayer : public singleton<RankPlayer>
{
	public:
		// RankPlayer();
		// ~RankPlayer();

		void	Initialize();
		void	Destroy();
		void	LoadPlayerValue(int iMode, const char* szNameTable, const char* szWhereExcept, const char* szSelect, const char* szNameColumn);
		void	LoadInfoPlayer(DWORD dwPID);
		int		GetEmpire(DWORD dwAcc) const;
		void	ReceiveP2P(int iMode, DWORD dwPid, const char* szAccount, const char* szName, BYTE bEmpire, long long lValue);
		void	SendInfoPlayer(LPCHARACTER ch, int iMode, long long lValue, bool bCanP2P = true);
		void	SendInfoPlayerMulti(LPCHARACTER ch, std::vector<TPacketGDAddRanking> s_table);
		void	RequestInfoRank(LPCHARACTER ch, int iMode);
		
		long long GetProgressByPID(DWORD dwPid, int iMode);
		
		bool	BlackList(std::string name);
		bool	IsInstantP2P(int iMode) const;

		void	ClearRanking(bool bDeleteFromMysql = false);
		
		void	RequestForceSave();
		void	ReceiveForceSave();

		bool	CanUseForceSave() { return m_bCanUseForceSave; }

	protected:	
		std::map<DWORD, SInfoPlayer> m_info_player;
		bool	m_bCanUseForceSave;
		LPEVENT m_pkCheckRankEvent;
};
