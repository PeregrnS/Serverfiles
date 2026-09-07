#include <boost/algorithm/string.hpp>
#include "stdafx.h"
#include "constants.h"
#include "packet.h"
#include "desc.h"
#include "char.h"
#include "p2p.h"
#include "config.h"
#ifdef __RANKING_SYSTEM__
#include "RankPlayer.h"
#include "db.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char_manager.h"
#include "buffer_manager.h"
#include "utils.h"

/* RankPlayer::RankPlayer()
{
}

RankPlayer::~RankPlayer()
{
	// clear map while closing server
	m_info_player.clear();
} */

void RankPlayer::Initialize() // Initalize (start server load)
{

	if (g_bAuthServer)
		return;

	m_info_player.clear();
	
	// Syntax :: rank|name_table|where_table|select_tables|order_by_tables
	LoadPlayerValue(RANK_BY_LEVEL, "player", "id >= 0", "id|level", "level");
	LoadPlayerValue(RANK_BY_STONE, "player_ranking", "`rank` = 1", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_MAX_DMG_STONE, "player_ranking", "`rank` = 2", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_DUNGEON_COMPLETE, "player_ranking", "`rank` = 3", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_BOSS, "player_ranking", "`rank` = 4", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_MAX_DMG_BOSS, "player_ranking", "`rank` = 5", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_MONSTER, "player_ranking", "`rank` = 6", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_ITEM_IMPROVED, "player_ranking", "`rank` = 7", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_PLAYING_TIME, "player", "id >= 0", "id|playtime", "playtime");
	LoadPlayerValue(RANK_BY_MISSION_BOOK_COMPLETE, "player_ranking", "`rank` = 9", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_BOX_OPENED, "player_ranking", "`rank` = 10", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_BONUS_CHANGED, "player_ranking", "`rank` = 11", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_FISH_CAUGHT, "player_ranking", "`rank` = 12", "dwPID|lValue", "lValue");
	LoadPlayerValue(RANK_BY_EXTRACTION, "player_ranking", "`rank` = 13", "dwPID|lValue", "lValue");
}

void RankPlayer::Destroy() // Destroy (server stopped)
{
	if (m_pkCheckRankEvent)
		event_cancel(&m_pkCheckRankEvent);
	m_pkCheckRankEvent = NULL;
	m_bCanUseForceSave = false;
}

bool RankPlayer::BlackList(std::string name)
{
	std::map<std::string,DWORD> n_blackList = {
		{"0000000000000000000000000",0}
	};
	auto it = n_blackList.find(name);
	if(it != n_blackList.end())
		return true;
	return false;
}


void RankPlayer::LoadPlayerValue(int iMode, const char* szNameTable, const char* szWhereExcept, const char* szSelect, const char* szNameColumn)
{
	std::vector<std::string> stArgs;
	boost::split(stArgs, szSelect, boost::is_any_of("|"));
	
	char query[QUERY_MAX_LEN];
	sprintf(query, "SELECT %s", stArgs[0].c_str());
	
	for (int i = 1; i < stArgs.size(); ++i)
		sprintf(query, "%s, %s", query, stArgs[i].c_str());

	sprintf(query, "%s FROM %s WHERE %s ORDER BY %s DESC", query, szNameTable, szWhereExcept, szNameColumn);

	// unique_ptr vs unique_ptr
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(query));
	
	if (pMsg->Get()->uiNumRows == 0)
		return;

	MYSQL_ROW row = NULL;
	while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		DWORD dwPid = 0;
		int iVal = 0;
		str_to_number(dwPid, row[0]);

		char szName[CHARACTER_NAME_MAX_LEN + 1];
		long long lYang = 0;

		switch(iMode)
		{
			case RANK_BY_LEVEL:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].bLevel = iVal;
				break;
				
			case RANK_BY_STONE:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iStone = iVal;
				break;
			
			case RANK_BY_MAX_DMG_STONE:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iDmgStone = iVal;
				break;			
			
			case RANK_BY_DUNGEON_COMPLETE:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iDungeon = iVal;
				break;			
			
			case RANK_BY_BOSS:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iBoss = iVal;
				break;
			
			case RANK_BY_MAX_DMG_BOSS:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iDmgBoss = iVal;
				break;		
			
			case RANK_BY_MONSTER:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iMonster = iVal;
				break;
				
			case RANK_BY_ITEM_IMPROVED:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iImprove = iVal;
				break;

			case RANK_BY_PLAYING_TIME:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iPlayingTime = iVal;
				break;

			case RANK_BY_MISSION_BOOK_COMPLETE:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iMissionBook = iVal;
				break;

			case RANK_BY_BOX_OPENED:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iBox = iVal;
				break;

			case RANK_BY_BONUS_CHANGED:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iBonus = iVal;
				break;

			case RANK_BY_FISH_CAUGHT:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iFish = iVal;
				break;
				
			case RANK_BY_EXTRACTION:
				str_to_number(iVal, row[1]);
				m_info_player[dwPid].iExtraction = iVal;
				break;
		}
		
		// Try Load Info About Player
		LoadInfoPlayer(dwPid);
	}
}

void RankPlayer::LoadInfoPlayer(DWORD dwPID)
{
	std::map<DWORD, SInfoPlayer>::iterator it = m_info_player.find(dwPID);
	if (it != m_info_player.end()) // load for 1 time, bcs loading categories info are not load in same line
	{
		if (it->second.bLoaded == true)
			return;
	}
	
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT name, account_id, account.login FROM player LEFT JOIN account.account ON account.id = player.account_id WHERE player.id = %d", dwPID));
	
	if (pMsg->Get()->uiNumRows == 0)
		return;

	MYSQL_ROW row = NULL;
	while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];
		char szAccount[LOGIN_MAX_LEN + 1];
		DWORD dwAcc = 0;
	
		if (!strncmp(row[0], "[", 1)) // GM
			continue;

		strlcpy(szName, row[0], sizeof(szName));
		str_to_number(dwAcc, row[1]);
		strlcpy(szAccount, row[2], sizeof(szAccount));
		
		// Get Empire
		int iEmpire = GetEmpire(dwAcc);
		
		if (iEmpire == -1) // account id not found, continue loop
			continue;

		strlcpy(m_info_player[dwPID].szName, szName, sizeof(m_info_player[dwPID].szName));
		strlcpy(m_info_player[dwPID].szAccount, szAccount, sizeof(m_info_player[dwPID].szAccount));
		m_info_player[dwPID].bEmpire = iEmpire;
		m_info_player[dwPID].bLoaded = true;
	}		
}

int RankPlayer::GetEmpire(DWORD dwAcc) const
{
	std::unique_ptr<SQLMsg> pMsgEmpire(DBManager::instance().DirectQuery("SELECT empire FROM player.player_index WHERE id = %u", dwAcc));
	
	if (pMsgEmpire->Get()->uiNumRows == 0)
		return -1;
	
	MYSQL_ROW row = NULL;
	int iEmpire = 0;
	while ((row = mysql_fetch_row(pMsgEmpire->Get()->pSQLResult)))
	{
		str_to_number(iEmpire, row[0]);
	}
	
	return iEmpire;
}

long long RankPlayer::GetProgressByPID(DWORD dwPid, int iMode)
{
	itertype(m_info_player) it = m_info_player.find(dwPid);
	
	if (it == m_info_player.end())
		return 0;
	
	switch(iMode)
	{
		case RANK_BY_STONE:
			return it->second.iStone;
			
		case RANK_BY_MAX_DMG_STONE:
			return it->second.iDmgStone;	
				
		case RANK_BY_DUNGEON_COMPLETE:
			return it->second.iDungeon;
		
		case RANK_BY_BOSS:
			return it->second.iBoss;
		
		case RANK_BY_MAX_DMG_BOSS:
			return it->second.iDmgBoss;
		
		case RANK_BY_MONSTER:
			return it->second.iMonster;		
			
		case RANK_BY_ITEM_IMPROVED:
			return it->second.iImprove;			

		case RANK_BY_MISSION_BOOK_COMPLETE:
			return it->second.iMissionBook;	
	
		case RANK_BY_BOX_OPENED:
			return it->second.iBox;
	
		case RANK_BY_BONUS_CHANGED:
			return it->second.iBonus;	
	
		case RANK_BY_FISH_CAUGHT:
			return it->second.iFish;
			
		case RANK_BY_EXTRACTION:
			return it->second.iExtraction;

		default:
			return -1; // SKIP_RANK_BECAUSE_ITS_NON_IN_LIST
	}

	return -1; // SKIP_RANK_BECAUSE_ITS_NON_IN_LIST
}

void RankPlayer::ReceiveP2P(int iMode, DWORD dwPid, const char* szAccount, const char* szName, BYTE bEmpire, long long lValue)
{
	m_info_player[dwPid].bEmpire = bEmpire;
	
	switch(iMode)
	{
		case RANK_BY_LEVEL:
			m_info_player[dwPid].bLevel = lValue;
			break;
			
		case RANK_BY_STONE:
			m_info_player[dwPid].iStone = lValue;
			break;
		
		case RANK_BY_MAX_DMG_STONE:
			m_info_player[dwPid].iDmgStone = lValue;
			break;
		
		case RANK_BY_DUNGEON_COMPLETE:
			m_info_player[dwPid].iDungeon = lValue;
			break;		
		
		case RANK_BY_BOSS:
			m_info_player[dwPid].iBoss = lValue;
			break;
		
		case RANK_BY_MAX_DMG_BOSS:
			m_info_player[dwPid].iDmgBoss = lValue;
			break;
		
		case RANK_BY_MONSTER:
			m_info_player[dwPid].iMonster = lValue;
			break;
			
		case RANK_BY_ITEM_IMPROVED:
			m_info_player[dwPid].iImprove = lValue;
			break;
	
		case RANK_BY_PLAYING_TIME:
			m_info_player[dwPid].iPlayingTime = lValue;
			break;
	
		case RANK_BY_MISSION_BOOK_COMPLETE:
			m_info_player[dwPid].iMissionBook = lValue;
			break;	
	
		case RANK_BY_BOX_OPENED:
			m_info_player[dwPid].iBox = lValue;
			break;	
	
		case RANK_BY_BONUS_CHANGED:
			m_info_player[dwPid].iBonus = lValue;
			break;	
	
		case RANK_BY_FISH_CAUGHT:
			m_info_player[dwPid].iFish = lValue;
			break;
			
		case RANK_BY_EXTRACTION:
			m_info_player[dwPid].iExtraction = lValue;
			break;
	}
	
	strlcpy(m_info_player[dwPid].szName, szName, sizeof(m_info_player[dwPid].szName));
	strlcpy(m_info_player[dwPid].szAccount, szAccount, sizeof(m_info_player[dwPid].szAccount));
}

void RankPlayer::SendInfoPlayer(LPCHARACTER ch, int iMode, long long lValue, bool bCanP2P)
{
	if (!ch)
		return;

	if (ch->IsGM())
		return;

	// Check for DMG
	switch (iMode)
	{
		case RANK_BY_MAX_DMG_STONE:
		case RANK_BY_MAX_DMG_BOSS:
		{
			if (lValue <= GetProgressByPID(ch->GetPlayerID(), iMode))
				return;
		} break;
		
		default: 
			break;
	}
	// Check for DMG
	
	ReceiveP2P(iMode, ch->GetPlayerID(), ch->GetDesc()->GetAccountTable().login, ch->GetName(), ch->GetEmpire(), lValue);
	
	if (!bCanP2P)
		return;
	
	TPacketGGPlayerRank p2;

	p2.bHeader = HEADER_GG_PLAYER_RANK;
	strlcpy(p2.szName, ch->GetName(), sizeof(p2.szName));
	strlcpy(p2.szAccount, ch->GetDesc()->GetAccountTable().login, sizeof(p2.szAccount));
	p2.bEmpire = ch->GetEmpire();
	p2.dwPid = ch->GetPlayerID();
	p2.lValue = lValue;
	p2.iMode = iMode;
	p2.lSize = 0;

	P2P_MANAGER::instance().Send(&p2, sizeof(TPacketGGPlayerRank));
}

void RankPlayer::SendInfoPlayerMulti(LPCHARACTER ch, std::vector<TPacketGDAddRanking> s_table)
{
	if (!ch)
		return;

	if (ch->IsGM())
		return;

	if (s_table.empty())
		return;
	
	std::vector<TPacketGGUpdateRanking> s_P2PTable;
	s_P2PTable.clear();
	int count = 0;
	long long realValue = 0;
	
	for (TPacketGDAddRanking i : s_table)
	{
		// ALWAYS USING P2P - we can skip them
		if (IsInstantP2P(i.bTypeRank))
			continue;
		// ALWAYS USING P2P - we can skip them
		
		realValue = i.lValue;

		ReceiveP2P(i.bTypeRank, ch->GetPlayerID(), ch->GetDesc()->GetAccountTable().login, ch->GetName(), ch->GetEmpire(), realValue);
		
		TPacketGGUpdateRanking pRank;
		pRank.bEmpire = ch->GetEmpire();
		pRank.dwPid = ch->GetPlayerID();
		pRank.lValue = realValue;
		pRank.iMode = i.bTypeRank;
		strlcpy(pRank.szName, ch->GetName(), sizeof(pRank.szName));
		strlcpy(pRank.szAccount, ch->GetDesc()->GetAccountTable().login, sizeof(pRank.szAccount));
		
		s_P2PTable.push_back(pRank);
		count++;
	}
	
	if (count)
	{
		TPacketGGPlayerRank p;
		p.bHeader = HEADER_GG_PLAYER_RANK;
		p.lSize = sizeof(TPacketGGUpdateRanking) * count;
		
		TEMP_BUFFER buf;
		buf.write(&p, sizeof(p));
		buf.write(&s_P2PTable[0], sizeof(TPacketGGUpdateRanking) * count);

		P2P_MANAGER::instance().Send(buf.read_peek(), buf.size());
	}
}

#include <boost/unordered_map.hpp>

bool CompareRankPlayerByLevel(SInfoPlayer i, SInfoPlayer j)
{
	return i.bLevel > j.bLevel;
}

bool CompareRankPlayerByStone(SInfoPlayer i, SInfoPlayer j)
{
	return i.iStone > j.iStone;
}

bool CompareRankPlayerByDmgStone(SInfoPlayer i, SInfoPlayer j)
{
	return i.iDmgStone > j.iDmgStone;
}

bool CompareRankPlayerByDungeon(SInfoPlayer i, SInfoPlayer j)
{
	return i.iDungeon > j.iDungeon;
}

bool CompareRankPlayerByBoss(SInfoPlayer i, SInfoPlayer j)
{
	return i.iBoss > j.iBoss;
}

bool CompareRankPlayerByDmgBoss(SInfoPlayer i, SInfoPlayer j)
{
	return i.iDmgBoss > j.iDmgBoss;
}

bool CompareRankPlayerByMonster(SInfoPlayer i, SInfoPlayer j)
{
	return i.iMonster > j.iMonster;
}

bool CompareRankPlayerByImprove(SInfoPlayer i, SInfoPlayer j)
{
	return i.iImprove > j.iImprove;
}

bool CompareRankPlayerByPlayingTime(SInfoPlayer i, SInfoPlayer j)
{
	return i.iPlayingTime > j.iPlayingTime;
}

bool CompareRankPlayerByMissionBookComplete(SInfoPlayer i, SInfoPlayer j)
{
	return i.iMissionBook > j.iMissionBook;
}

bool CompareRankPlayerByBox(SInfoPlayer i, SInfoPlayer j)
{
	return i.iBox > j.iBox;
}

bool CompareRankPlayerByBonus(SInfoPlayer i, SInfoPlayer j)
{
	return i.iBonus > j.iBonus;
}

bool CompareRankPlayerByFish(SInfoPlayer i, SInfoPlayer j)
{
	return i.iFish > j.iFish;
}

bool CompareRankPlayerByExtraction(SInfoPlayer i, SInfoPlayer j)
{
	return i.iExtraction > j.iExtraction;
}

void RankPlayer::RequestInfoRank(LPCHARACTER ch, int iMode)
{
	if (!ch || !ch->GetDesc())
		return;

	int iSize = 0;
	// int iMyPos = -1;
	
	std::vector<SInfoPlayer> vec_cache;
	
	for (std::map<DWORD, SInfoPlayer>::iterator it = m_info_player.begin(); it != m_info_player.end(); ++it)
	{
		if (it->second.szName != NULL && strlen(it->second.szName) > 1)
			vec_cache.push_back(it->second);
	}

	iSize = vec_cache.size();
	
	if (iSize > SHOW_MAX_LIMIT + 1)
		iSize = SHOW_MAX_LIMIT + 1;
	
	TPacketGCRankInfo pack;
	pack.bHeader = HEADER_GC_RANK_INFO;
	pack.iMode = iMode;
	
	switch(iMode)
	{
		case RANK_BY_LEVEL:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByLevel);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].bLevel;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].bLevel;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
			
		case RANK_BY_STONE:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByStone);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iStone;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iStone;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
			
		case RANK_BY_MAX_DMG_STONE:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByDmgStone);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iDmgStone;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iDmgStone;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;			
		
		case RANK_BY_DUNGEON_COMPLETE:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByDungeon);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iDungeon;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iDungeon;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
		
		case RANK_BY_BOSS:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByBoss);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iBoss;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iBoss;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
			
		case RANK_BY_MAX_DMG_BOSS:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByDmgBoss);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iDmgBoss;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iDmgBoss;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;			
			
		case RANK_BY_MONSTER:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByMonster);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iMonster;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iMonster;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
			
		case RANK_BY_ITEM_IMPROVED:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByImprove);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iImprove;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iImprove;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
			
		case RANK_BY_PLAYING_TIME:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByPlayingTime);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iPlayingTime;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iPlayingTime;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
			
		case RANK_BY_MISSION_BOOK_COMPLETE:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByMissionBookComplete);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iMissionBook;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iMissionBook;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;

		case RANK_BY_BOX_OPENED:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByBox);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iBox;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iBox;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
			
		case RANK_BY_BONUS_CHANGED:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByBonus);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iBonus;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iBonus;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
			
		case RANK_BY_FISH_CAUGHT:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByFish);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iFish;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iFish;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
			
		case RANK_BY_EXTRACTION:
			std::stable_sort(vec_cache.begin(), vec_cache.end(), CompareRankPlayerByExtraction);

			for (int iPos = 0; iPos < iSize; ++iPos)
			{	
				pack.lValue = vec_cache[iPos].iExtraction;
				strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
				pack.iMyPos = 0;
				pack.iPos = iPos + 1;
				pack.bEmpire = vec_cache[iPos].bEmpire;

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
			
			for (int iPos = 0; iPos < vec_cache.size(); ++iPos)
			{
				if (!strcmp(vec_cache[iPos].szName, ch->GetName()))
				{
					pack.lValue = vec_cache[iPos].iExtraction;
					strlcpy(pack.szName, vec_cache[iPos].szName, sizeof(pack.szName));
					pack.iPos = iPos + 1;
					pack.iMyPos = 1;
					pack.bEmpire = vec_cache[iPos].bEmpire;

					ch->GetDesc()->Packet(&pack, sizeof(pack));
					break;
				}
			}
			break;
	}
}

bool RankPlayer::IsInstantP2P(int iMode) const
{
	switch (iMode)
	{
		case RANK_BY_MISSION_BOOK_COMPLETE:
		case RANK_BY_DUNGEON_COMPLETE:
		case RANK_BY_FISH_CAUGHT:
		case RANK_BY_EXTRACTION:
			return true;
			break;

		default:
			return false;
	}
	
	return false;
}
#endif
