

#include "../../common/length.h"

#include <boost/unordered_map.hpp>

class CHARACTER;

enum ERankByValue
{
	RANK_BY_LEVEL,
	RANK_BY_YANG,
	RANK_BY_STONE,
	RANK_BY_BOSS,
	RANK_BY_MONSTER,
	RANK_BY_DUNGEON_COMPLETE,
	RANK_BY_FISH_CAUGHT,
	RANK_BY_EXTRACTION,
	RANK_BY_BONUS_CHANGED,
	RANK_BY_BOX_OPENED,
	RANK_BY_ITEM_IMPROVED,
	RANK_BY_DEATH,

	RANK_MAX,
};

enum EConfigRankPlayer
{
	MAX_LIMIT_GET_RANK_PLAYER = 50, // limit maximum load players from mysql
	SHOW_MAX_LIMIT = 10, // show maximum 10 players in top. (2 pages)
};

struct SInfoPlayer
{
	bool bLoaded;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE bEmpire, bLevel;
	long long lYang;
	int iStone, iBoss, iMonster, iDungeon, iFish, iExtraction, iBonus, iBox, iImprove, iDeath;
	
	SInfoPlayer()
	{
		bLoaded = false;
		memset(szName, 0, sizeof(szName));
		bEmpire = 0;
		bLevel = 0; lYang = 0;
		iStone = 0; iBoss = 0; iMonster = 0;
		iDungeon = 0; iFish = 0; iExtraction = 0;
		iBonus = 0; iBox = 0; iImprove = 0;
		iDeath = 0;
	}
};

class RankPlayer : public singleton<RankPlayer>
{
	public:
		RankPlayer();
		~RankPlayer();

		void	Initialize();
		void	LoadPlayerValue(int iMode, const char* szNameTable, const char* szWhereExcept, const char* szSelect, const char* szNameColumn);
		void	LoadInfoPlayer(DWORD dwPID);
		int		GetEmpire(DWORD dwAcc) const;
		void	ReceiveP2P(int iMode, DWORD dwPid, const char* szName, BYTE bEmpire, long long lValue);
		void	SendInfoPlayer(LPCHARACTER ch, int iMode, long long lValue, bool bCanP2P = true);
		void	RequestInfoRank(LPCHARACTER ch, int iMode);
		
		long long GetProgressByPID(DWORD dwPid, int iMode);
	
	protected:	
		std::map<DWORD, SInfoPlayer> m_info_player;
};
