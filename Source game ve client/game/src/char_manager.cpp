#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "desc.h"
#include "char.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "party.h"
#include "regen.h"
#include "p2p.h"
#include "dungeon.h"
#include "db.h"
#include "config.h"
#include "xmas_event.h"
#include "questmanager.h"
#include "questlua.h"
#include "locale_service.h"
#include "shutdown_manager.h"
#include "../../common/CommonDefines.h"
#if defined(ENABLE_GLOBAL_REWARD) || defined(ENABLE_EVENT_CALENDAR)
	#include "desc_manager.h"
#endif
#ifdef ENABLE_ITEMSHOP
	#include "itemshop.h"
#endif
#ifdef ENABLE_EVENT_CALENDAR
	#include "item_manager.h"
	#include "item.h"
	#include "desc_client.h"
#endif

#if (!defined(__GNUC__) || defined(__clang__)) && !defined(CXX11_ENABLED)
#	include <boost/bind.hpp>

#elif defined(CXX11_ENABLED)
#	include <functional>
	template <typename T>
	decltype(std::bind(&T::second, std::placeholders::_1)) select2nd()
	{
		return std::bind(&T::second, std::placeholders::_1);
	}
#endif

CHARACTER_MANAGER::CHARACTER_MANAGER() :
	m_iVIDCount(0),
	m_pkChrSelectedStone(NULL),
	m_bUsePendingDestroy(false)
{
	RegisterRaceNum(xmas::MOB_XMAS_FIRWORK_SELLER_VNUM);
	RegisterRaceNum(xmas::MOB_SANTA_VNUM);
	RegisterRaceNum(xmas::MOB_XMAS_TREE_VNUM);

	m_iMobItemRate = 100;
	m_iMobDamageRate = 100;
	m_iMobGoldAmountRate = 100;
	m_iMobGoldDropRate = 100;
	m_iMobExpRate = 100;

	m_iMobItemRatePremium = 100;
	m_iMobGoldAmountRatePremium = 100;
	m_iMobGoldDropRatePremium = 100;
	m_iMobExpRatePremium = 100;

	m_iUserDamageRate = 100;
	m_iUserDamageRatePremium = 100;

#ifdef ENABLE_REWARD_SYSTEM
	m_rewardData.clear();
#endif
}

CHARACTER_MANAGER::~CHARACTER_MANAGER()
{
	Destroy();
}

void CHARACTER_MANAGER::Destroy()
{
#ifdef ENABLE_REWARD_SYSTEM
	m_rewardData.clear();
#endif
	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.begin();
	while (it != m_map_pkChrByVID.end()) {
		LPCHARACTER ch = it->second;
		M2_DESTROY_CHARACTER(ch); // m_map_pkChrByVID is changed here
		it = m_map_pkChrByVID.begin();
	}
}

void CHARACTER_MANAGER::GracefulShutdown()
{
	NAME_MAP::iterator it = m_map_pkPCChr.begin();

	while (it != m_map_pkPCChr.end())
		(it++)->second->Disconnect("GracefulShutdown");
}

DWORD CHARACTER_MANAGER::AllocVID()
{
	++m_iVIDCount;
	return m_iVIDCount;
}

LPCHARACTER CHARACTER_MANAGER::CreateCharacter(const char * name, DWORD dwPID)
{
	DWORD dwVID = AllocVID();

#ifdef M2_USE_POOL
	LPCHARACTER ch = pool_.Construct();
#else
	LPCHARACTER ch = M2_NEW CHARACTER;
#endif
	ch->Create(name, dwVID, dwPID ? true : false);

	m_map_pkChrByVID.emplace(dwVID, ch);

	if (dwPID)
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];
		str_lower(name, szName, sizeof(szName));

		m_map_pkPCChr.emplace(szName, ch);
		m_map_pkChrByPID.emplace(dwPID, ch);
	}

	return (ch);
}

#ifndef DEBUG_ALLOC
void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER ch)
#else
void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER ch, const char* file, size_t line)
#endif
{
	if (!ch)
		return;

	// <Factor> Check whether it has been already deleted or not.
	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.find(ch->GetVID());
	if (it == m_map_pkChrByVID.end()) {
		sys_err("[CHARACTER_MANAGER::DestroyCharacter] <Factor> %d not found", (long)(ch->GetVID()));
		return; // prevent duplicated destrunction
	}

	if (ch->IsNPC() && !ch->IsPet() && ch->GetRider() == NULL
#ifdef ENABLE_MOUNT_SYSTEM
			&& !ch->IsMount()
#endif
	)
	{
		if (ch->GetDungeon())
		{
			ch->GetDungeon()->DeadCharacter(ch);
		}
	}

	if (m_bUsePendingDestroy)
	{
		m_set_pkChrPendingDestroy.emplace(ch);
		return;
	}

	m_map_pkChrByVID.erase(it);

	if (true == ch->IsPC())
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];

		str_lower(ch->GetName(), szName, sizeof(szName));

		NAME_MAP::iterator it = m_map_pkPCChr.find(szName);

		if (m_map_pkPCChr.end() != it)
			m_map_pkPCChr.erase(it);
	}

	if (0 != ch->GetPlayerID())
	{
		itertype(m_map_pkChrByPID) it = m_map_pkChrByPID.find(ch->GetPlayerID());

		if (m_map_pkChrByPID.end() != it)
		{
			m_map_pkChrByPID.erase(it);
		}
	}

	UnregisterRaceNumMap(ch);

	RemoveFromStateList(ch);

#ifdef M2_USE_POOL
	pool_.Destroy(ch);
#else
#ifndef DEBUG_ALLOC
	M2_DELETE(ch);
#else
	M2_DELETE_EX(ch, file, line);
#endif
#endif
}

LPCHARACTER CHARACTER_MANAGER::Find(DWORD dwVID)
{
	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.find(dwVID);

	if (m_map_pkChrByVID.end() == it)
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && dwVID != (DWORD)found->GetVID()) {
		sys_err("[CHARACTER_MANAGER::Find] <Factor> %u != %u", dwVID, (DWORD)found->GetVID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::Find(const VID & vid)
{
	LPCHARACTER tch = Find((DWORD) vid);

	if (!tch || tch->GetVID() != vid)
		return NULL;

	return tch;
}

LPCHARACTER CHARACTER_MANAGER::FindByPID(DWORD dwPID)
{
	itertype(m_map_pkChrByPID) it = m_map_pkChrByPID.find(dwPID);

	if (m_map_pkChrByPID.end() == it)
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && dwPID != found->GetPlayerID()) {
		sys_err("[CHARACTER_MANAGER::FindByPID] <Factor> %u != %u", dwPID, found->GetPlayerID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::FindPC(const char * name)
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	str_lower(name, szName, sizeof(szName));
	NAME_MAP::iterator it = m_map_pkPCChr.find(szName);

	if (it == m_map_pkPCChr.end())
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && strncasecmp(szName, found->GetName(), CHARACTER_NAME_MAX_LEN) != 0) {
		sys_err("[CHARACTER_MANAGER::FindPC] <Factor> %s != %s", name, found->GetName());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRandomPosition(DWORD dwVnum, long lMapIndex, bool is_aggressive)
{
	{
		if (dwVnum == 5001 && !quest::CQuestManager::instance().GetEventFlag("japan_regen"))
		{
			sys_log(1, "WAEGU[5001] regen disabled.");
			return NULL;
		}
	}

	{
		if (dwVnum == 5002 && !quest::CQuestManager::instance().GetEventFlag("newyear_mob"))
		{
			sys_log(1, "HAETAE (new-year-mob) [5002] regen disabled.");
			return NULL;
		}
	}

	{
		if (dwVnum == 5004 && !quest::CQuestManager::instance().GetEventFlag("independence_day"))
		{
			sys_log(1, "INDEPENDECE DAY [5004] regen disabled.");
			return NULL;
		}
	}

	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
	{
		sys_err("no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!map_allow_find(lMapIndex))
	{
		sys_err("not allowed map %u", lMapIndex);
		return NULL;
	}

	LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
	if (pkSectreeMap == NULL) {
		return NULL;
	}

	int i;
	long x, y;
	for (i=0; i<2000; i++)
	{
		x = number(1, (pkSectreeMap->m_setting.iWidth / 100)  - 1) * 100 + pkSectreeMap->m_setting.iBaseX;
		y = number(1, (pkSectreeMap->m_setting.iHeight / 100) - 1) * 100 + pkSectreeMap->m_setting.iBaseY;
		//LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);
		LPSECTREE tree = pkSectreeMap->Find(x, y);

		if (!tree)
			continue;

		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT))
			continue;

		if (IS_SET(dwAttr, ATTR_BANPK))
			continue;

		break;
	}

	if (i == 2000)
	{
		sys_err("cannot find valid location");
		return NULL;
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);

	if (!ch)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create new character");
		return NULL;
	}

	ch->SetProto(pkMob);

	// if mob is npc with no empire assigned, assign to empire of map
	if (pkMob->m_table.bType == CHAR_TYPE_NPC)
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));

	ch->SetRotation(number(0, 360));
	if (is_aggressive) //@fixme195
		ch->SetAggressive();

	if (!ch->Show(lMapIndex, x, y, 0, false))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_err(0, "SpawnMobRandomPosition: cannot show monster");
		return NULL;
	}

	char buf[512+1];
	long local_x = x - pkSectreeMap->m_setting.iBaseX;
	long local_y = y - pkSectreeMap->m_setting.iBaseY;
	snprintf(buf, sizeof(buf), "spawn %s[%d] random position at %ld %ld %ld %ld (time: %d)", ch->GetName(), dwVnum, x, y, local_x, local_y, get_global_time());

	if (test_server)
		SendNotice(buf);

	sys_log(0, buf);
	return (ch);
}

LPCHARACTER CHARACTER_MANAGER::SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion, int iRot, bool bShow)
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);
	if (!pkMob)
	{
		sys_err("SpawnMob: no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!(pkMob->m_table.bType == CHAR_TYPE_NPC || pkMob->m_table.bType == CHAR_TYPE_WARP || pkMob->m_table.bType == CHAR_TYPE_GOTO
 		|| pkMob->m_table.bType == CHAR_TYPE_MOUNT
		) || mining::IsVeinOfOre (dwVnum)
		)
	{
		LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

		if (!tree)
		{
			sys_log(0, "no sectree for spawn at %d %d mobvnum %d mapindex %d", x, y, dwVnum, lMapIndex);
			return NULL;
		}

		DWORD dwAttr = tree->GetAttribute(x, y);

		bool is_set = false;

		if ( mining::IsVeinOfOre (dwVnum) ) is_set = IS_SET(dwAttr, ATTR_BLOCK);
		else is_set = IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT);

		if ( is_set )
		{
			// SPAWN_BLOCK_LOG
			static bool s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");
			static DWORD s_nextTime=get_global_time()+10000;

			DWORD curTime=get_global_time();

			if (curTime>s_nextTime)
			{
				s_nextTime=curTime;
				s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");

			}

			if (s_isLog)
				sys_log(0, "SpawnMob: BLOCKED position for spawn %s %u at %d %d (attr %u)", pkMob->m_table.szName, dwVnum, x, y, dwAttr);
			// END_OF_SPAWN_BLOCK_LOG
			return NULL;
		}

		if (IS_SET(dwAttr, ATTR_BANPK))
		{
			sys_log(0, "SpawnMob: BAN_PK position for mob spawn %s %u at %d %d", pkMob->m_table.szName, dwVnum, x, y);
			return NULL;
		}
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "SpawnMob: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);

	if (!ch)
	{
		sys_log(0, "SpawnMob: cannot create new character");
		return NULL;
	}

	if (iRot == -1)
		iRot = number(0, 360);

	ch->SetProto(pkMob);

	// if mob is npc with no empire assigned, assign to empire of map
	if (pkMob->m_table.bType == CHAR_TYPE_NPC)
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));

	ch->SetRotation(iRot);

	if (bShow && !ch->Show(lMapIndex, x, y, z, bSpawnMotion))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_log(0, "SpawnMob: cannot show monster");
		return NULL;
	}

	return (ch);
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRange(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, bool bIsException, bool bSpawnMotion, bool bAggressive )
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
		return NULL;

	if (pkMob->m_table.bType == CHAR_TYPE_STONE)
		bSpawnMotion = true;

	int i = 16;

	while (i--)
	{
		int x = number(sx, ex);
		int y = number(sy, ey);
		/*
		   if (bIsException)
		   if (is_regen_exception(x, y))
		   continue;
		 */
		LPCHARACTER ch = SpawnMob(dwVnum, lMapIndex, x, y, 0, bSpawnMotion);

		if (ch)
		{
			sys_log(1, "MOB_SPAWN: %s(%d) %dx%d", ch->GetName(), (DWORD) ch->GetVID(), ch->GetX(), ch->GetY());
			if (bAggressive)
				ch->SetAggressive();
			return (ch);
		}
	}

	return NULL;
}

void CHARACTER_MANAGER::SelectStone(LPCHARACTER pkChr)
{
	m_pkChrSelectedStone = pkChr;
}

bool CHARACTER_MANAGER::SpawnMoveGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, int tx, int ty, LPREGEN pkRegen, bool bAggressive_)
{
	if (!dwVnum)
		return false;

	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return false;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;
		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)
				return false;

			continue;
		}

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}
		if (bAggressive)
			tch->SetAggressive();

		if (tch->Goto(tx, ty))
			tch->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	}

	return true;
}

bool CHARACTER_MANAGER::SpawnGroupGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	const DWORD dwGroupID = CMobManager::Instance().GetGroupFromGroupGroup(dwVnum);

	if( dwGroupID != 0 )
	{
		return SpawnGroup(dwGroupID, lMapIndex, sx, sy, ex, ey, pkRegen, bAggressive_, pDungeon);
	}
	else
	{
		sys_err( "NOT_EXIST_GROUP_GROUP_VNUM(%u) MAP(%ld)", dwVnum, lMapIndex );
		return false;
	}
}

LPCHARACTER CHARACTER_MANAGER::SpawnGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	if (!dwVnum)
		return NULL;

	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return NULL;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;

		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	LPCHARACTER chLeader = NULL;

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)
				return NULL;

			continue;
		}

		if (i == 0)
			chLeader = tch;

		tch->SetDungeon(pDungeon);

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}

		if (bAggressive)
			tch->SetAggressive();
	}

	return chLeader;
}

struct FuncUpdateAndResetChatCounter
{
	void operator () (LPCHARACTER ch)
	{
		ch->ResetChatCounter();
		ch->CFSM::Update();
	}
};

void CHARACTER_MANAGER::Update(int iPulse)
{
	using namespace std;
#if defined(__GNUC__) && !defined(__clang__) && !defined(CXX11_ENABLED)
	using namespace __gnu_cxx;
#endif

	BeginPendingDestroy();

	{
		if (!m_map_pkPCChr.empty())
		{
			CHARACTER_VECTOR v;
			v.reserve(m_map_pkPCChr.size());
#if (defined(__GNUC__) && !defined(__clang__)) || defined(CXX11_ENABLED)
			transform(m_map_pkPCChr.begin(), m_map_pkPCChr.end(), back_inserter(v), select2nd<NAME_MAP::value_type>());
#else
			transform(m_map_pkPCChr.begin(), m_map_pkPCChr.end(), back_inserter(v), boost::bind(&NAME_MAP::value_type::second, _1));
#endif

			if (0 == (iPulse % PASSES_PER_SEC(5)))
			{
				FuncUpdateAndResetChatCounter f;
				for_each(v.begin(), v.end(), f);
			}
			else
			{
				for_each(v.begin(), v.end(), msl::bind2nd(std::mem_fn(&CHARACTER::UpdateCharacter), iPulse));
			}
		}

	}

	{
		if (!m_set_pkChrState.empty())
		{
			CHARACTER_VECTOR v;
			v.reserve(m_set_pkChrState.size());
#if defined(__GNUC__) && !defined(__clang__) && !defined(CXX11_ENABLED)
			transform(m_set_pkChrState.begin(), m_set_pkChrState.end(), back_inserter(v), identity<CHARACTER_SET::value_type>());
#else
			v.insert(v.end(), m_set_pkChrState.begin(), m_set_pkChrState.end());
#endif
			for_each(v.begin(), v.end(), msl::bind2nd(std::mem_fn(&CHARACTER::UpdateStateMachine), iPulse));
		}
	}

	{
		CharacterVectorInteractor i;

		if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(xmas::MOB_SANTA_VNUM, i))
		{
			for_each(i.begin(), i.end(),
					msl::bind2nd(std::mem_fn(&CHARACTER::UpdateStateMachine), iPulse));
		}
	}

	if (0 == (iPulse % PASSES_PER_SEC(3600)))
	{
		for (itertype(m_map_dwMobKillCount) it = m_map_dwMobKillCount.begin(); it != m_map_dwMobKillCount.end(); ++it)
			DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER_KILL, it->first, it->second);

		m_map_dwMobKillCount.clear();
	}

	if (test_server && 0 == (iPulse % PASSES_PER_SEC(60)))
		sys_log(0, "CHARACTER COUNT vid %zu pid %zu", m_map_pkChrByVID.size(), m_map_pkChrByPID.size());

#ifdef ENABLE_WORLD_BOSS
	if (0 == (iPulse % PASSES_PER_SEC(10))) // EVERY 10S
	{
		CheckWorldBossSpawn();
	}
#endif

	FlushPendingDestroy();

	// ShutdownManager Update
	CShutdownManager::Instance().Update();
}

void CHARACTER_MANAGER::ProcessDelayedSave()
{
	CHARACTER_SET::iterator it = m_set_pkChrForDelayedSave.begin();

	while (it != m_set_pkChrForDelayedSave.end())
	{
		LPCHARACTER pkChr = *it++;
		pkChr->SaveReal();
	}

	m_set_pkChrForDelayedSave.clear();
}

bool CHARACTER_MANAGER::AddToStateList(LPCHARACTER ch)
{
	assert(ch != NULL);

	CHARACTER_SET::iterator it = m_set_pkChrState.find(ch);

	if (it == m_set_pkChrState.end())
	{
		m_set_pkChrState.emplace(ch);
		return true;
	}

	return false;
}

void CHARACTER_MANAGER::RemoveFromStateList(LPCHARACTER ch)
{
	CHARACTER_SET::iterator it = m_set_pkChrState.find(ch);

	if (it != m_set_pkChrState.end())
	{
		//sys_log(0, "RemoveFromStateList %p", ch);
		m_set_pkChrState.erase(it);
	}
}

void CHARACTER_MANAGER::DelayedSave(LPCHARACTER ch)
{
	m_set_pkChrForDelayedSave.emplace(ch);
}

bool CHARACTER_MANAGER::FlushDelayedSave(LPCHARACTER ch)
{
	CHARACTER_SET::iterator it = m_set_pkChrForDelayedSave.find(ch);

	if (it == m_set_pkChrForDelayedSave.end())
		return false;

	m_set_pkChrForDelayedSave.erase(it);
	ch->SaveReal();
	return true;
}

void CHARACTER_MANAGER::RegisterForMonsterLog(LPCHARACTER ch)
{
	m_set_pkChrMonsterLog.emplace(ch);
}

void CHARACTER_MANAGER::UnregisterForMonsterLog(LPCHARACTER ch)
{
	m_set_pkChrMonsterLog.erase(ch);
}

void CHARACTER_MANAGER::PacketMonsterLog(LPCHARACTER ch, const void* buf, int size)
{
	itertype(m_set_pkChrMonsterLog) it;

	for (it = m_set_pkChrMonsterLog.begin(); it!=m_set_pkChrMonsterLog.end();++it)
	{
		LPCHARACTER c = *it;

		if (ch && DISTANCE_APPROX(c->GetX()-ch->GetX(), c->GetY()-ch->GetY())>6000)
			continue;

		LPDESC d = c->GetDesc();

		if (d)
			d->Packet(buf, size);
	}
}

void CHARACTER_MANAGER::KillLog(DWORD dwVnum)
{
	const DWORD SEND_LIMIT = 10000;

	itertype(m_map_dwMobKillCount) it = m_map_dwMobKillCount.find(dwVnum);

	if (it == m_map_dwMobKillCount.end())
		m_map_dwMobKillCount.emplace(dwVnum, 1);
	else
	{
		++it->second;

		if (it->second > SEND_LIMIT)
		{
			DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER_KILL, it->first, it->second);
			m_map_dwMobKillCount.erase(it);
		}
	}
}

void CHARACTER_MANAGER::RegisterRaceNum(DWORD dwVnum)
{
	m_set_dwRegisteredRaceNum.emplace(dwVnum);
}

void CHARACTER_MANAGER::RegisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	if (m_set_dwRegisteredRaceNum.find(dwVnum) != m_set_dwRegisteredRaceNum.end())
	{
		sys_log(0, "RegisterRaceNumMap %s %u", ch->GetName(), dwVnum);
		m_map_pkChrByRaceNum[dwVnum].emplace(ch);
	}
}

void CHARACTER_MANAGER::UnregisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	itertype(m_map_pkChrByRaceNum) it = m_map_pkChrByRaceNum.find(dwVnum);

	if (it != m_map_pkChrByRaceNum.end())
		it->second.erase(ch);
}

bool CHARACTER_MANAGER::GetCharactersByRaceNum(DWORD dwRaceNum, CharacterVectorInteractor & i)
{
	std::map<DWORD, CHARACTER_SET>::iterator it = m_map_pkChrByRaceNum.find(dwRaceNum);

	if (it == m_map_pkChrByRaceNum.end())
		return false;

	i = it->second;
	return true;
}

#define FIND_JOB_WARRIOR_0	(1 << 3)
#define FIND_JOB_WARRIOR_1	(1 << 4)
#define FIND_JOB_WARRIOR_2	(1 << 5)
#define FIND_JOB_WARRIOR	(FIND_JOB_WARRIOR_0 | FIND_JOB_WARRIOR_1 | FIND_JOB_WARRIOR_2)
#define FIND_JOB_ASSASSIN_0	(1 << 6)
#define FIND_JOB_ASSASSIN_1	(1 << 7)
#define FIND_JOB_ASSASSIN_2	(1 << 8)
#define FIND_JOB_ASSASSIN	(FIND_JOB_ASSASSIN_0 | FIND_JOB_ASSASSIN_1 | FIND_JOB_ASSASSIN_2)
#define FIND_JOB_SURA_0		(1 << 9)
#define FIND_JOB_SURA_1		(1 << 10)
#define FIND_JOB_SURA_2		(1 << 11)
#define FIND_JOB_SURA		(FIND_JOB_SURA_0 | FIND_JOB_SURA_1 | FIND_JOB_SURA_2)
#define FIND_JOB_SHAMAN_0	(1 << 12)
#define FIND_JOB_SHAMAN_1	(1 << 13)
#define FIND_JOB_SHAMAN_2	(1 << 14)
#define FIND_JOB_SHAMAN		(FIND_JOB_SHAMAN_0 | FIND_JOB_SHAMAN_1 | FIND_JOB_SHAMAN_2)
#ifdef ENABLE_WOLFMAN_CHARACTER
#define FIND_JOB_WOLFMAN_0	(1 << 15)
#define FIND_JOB_WOLFMAN_1	(1 << 16)
#define FIND_JOB_WOLFMAN_2	(1 << 17)
#define FIND_JOB_WOLFMAN		(FIND_JOB_WOLFMAN_0 | FIND_JOB_WOLFMAN_1 | FIND_JOB_WOLFMAN_2)
#endif

//
// (job+1)*3+(skill_group)
//
LPCHARACTER CHARACTER_MANAGER::FindSpecifyPC(unsigned int uiJobFlag, long lMapIndex, LPCHARACTER except, int iMinLevel, int iMaxLevel)
{
	LPCHARACTER chFind = NULL;
	itertype(m_map_pkChrByPID) it;
	int n = 0;

	for (it = m_map_pkChrByPID.begin(); it != m_map_pkChrByPID.end(); ++it)
	{
		LPCHARACTER ch = it->second;

		if (ch == except)
			continue;

		if (ch->GetLevel() < iMinLevel)
			continue;

		if (ch->GetLevel() > iMaxLevel)
			continue;

		if (ch->GetMapIndex() != lMapIndex)
			continue;

		if (uiJobFlag)
		{
			unsigned int uiChrJob = (1 << ((ch->GetJob() + 1) * 3 + ch->GetSkillGroup()));

			if (!IS_SET(uiJobFlag, uiChrJob))
				continue;
		}

		if (!chFind || number(1, ++n) == 1)
			chFind = ch;
	}

	return chFind;
}

int CHARACTER_MANAGER::GetMobItemRate(LPCHARACTER ch)
{
	//PREVENT_TOXICATION_FOR_CHINA
	if (g_bChinaIntoxicationCheck)
	{
		if ( ch->IsOverTime( OT_3HOUR ) )
		{
			if (ch && ch->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0)
				return m_iMobItemRatePremium/2;
			return m_iMobItemRate/2;
		}
		else if ( ch->IsOverTime( OT_5HOUR ) )
		{
			return 0;
		}
	}
	//END_PREVENT_TOXICATION_FOR_CHINA
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0)
		return m_iMobItemRatePremium;
	return m_iMobItemRate;
}

int CHARACTER_MANAGER::GetMobDamageRate(LPCHARACTER ch)
{
	return m_iMobDamageRate;
}

int CHARACTER_MANAGER::GetMobGoldAmountRate(LPCHARACTER ch)
{
	if ( !ch )
		return m_iMobGoldAmountRate;

	//PREVENT_TOXICATION_FOR_CHINA
	if (g_bChinaIntoxicationCheck)
	{
		if ( ch->IsOverTime( OT_3HOUR ) )
		{
			if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
				return m_iMobGoldAmountRatePremium/2;
			return m_iMobGoldAmountRate/2;
		}
		else if ( ch->IsOverTime( OT_5HOUR ) )
		{
			return 0;
		}
	}
	//END_PREVENT_TOXICATION_FOR_CHINA
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
		return m_iMobGoldAmountRatePremium;
	return m_iMobGoldAmountRate;
}

int CHARACTER_MANAGER::GetMobGoldDropRate(LPCHARACTER ch)
{
	if ( !ch )
		return m_iMobGoldDropRate;

	//PREVENT_TOXICATION_FOR_CHINA
	if (g_bChinaIntoxicationCheck)
	{
		if ( ch->IsOverTime( OT_3HOUR ) )
		{
			if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
				return m_iMobGoldDropRatePremium/2;
			return m_iMobGoldDropRate/2;
		}
		else if ( ch->IsOverTime( OT_5HOUR ) )
		{
			return 0;
		}
	}
	//END_PREVENT_TOXICATION_FOR_CHINA
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0)
		return m_iMobGoldDropRatePremium;
	return m_iMobGoldDropRate;
}

int CHARACTER_MANAGER::GetMobExpRate(LPCHARACTER ch)
{
	if ( !ch )
		return m_iMobExpRate;

	//PREVENT_TOXICATION_FOR_CHINA
	if (g_bChinaIntoxicationCheck)
	{
		if ( ch->IsOverTime( OT_3HOUR ) )
		{
			if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
				return m_iMobExpRatePremium/2;
			return m_iMobExpRate/2;
		}
		else if ( ch->IsOverTime( OT_5HOUR ) )
		{
			return 0;
		}
	}
	//END_PREVENT_TOXICATION_FOR_CHINA
	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		return m_iMobExpRatePremium;
	return m_iMobExpRate;
}

int	CHARACTER_MANAGER::GetUserDamageRate(LPCHARACTER ch)
{
	if (!ch)
		return m_iUserDamageRate;

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		return m_iUserDamageRatePremium;

	return m_iUserDamageRate;
}

void CHARACTER_MANAGER::SendScriptToMap(long lMapIndex, const std::string & s)
{
	LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);

	if (NULL == pSecMap)
		return;

	struct packet_script p;

	p.header = HEADER_GC_SCRIPT;
	p.skin = 1;
	p.src_size = s.size();

	quest::FSendPacket f;
	p.size = p.src_size + sizeof(struct packet_script);
	f.buf.write(&p, sizeof(struct packet_script));
	f.buf.write(&s[0], s.size());

	pSecMap->for_each(f);
}

bool CHARACTER_MANAGER::BeginPendingDestroy()
{
	if (m_bUsePendingDestroy)
		return false;

	m_bUsePendingDestroy = true;
	return true;
}

void CHARACTER_MANAGER::FlushPendingDestroy()
{
	using namespace std;

	m_bUsePendingDestroy = false;

	if (!m_set_pkChrPendingDestroy.empty())
	{
		sys_log(0, "FlushPendingDestroy size %d", m_set_pkChrPendingDestroy.size());

		CHARACTER_SET::iterator it = m_set_pkChrPendingDestroy.begin(),
			end = m_set_pkChrPendingDestroy.end();
		for ( ; it != end; ++it) {
			M2_DESTROY_CHARACTER(*it);
		}

		m_set_pkChrPendingDestroy.clear();
	}
}

CharacterVectorInteractor::CharacterVectorInteractor(const CHARACTER_SET & r)
{
	using namespace std;
#if defined(__GNUC__) && !defined(__clang__) && !defined(CXX11_ENABLED)
	using namespace __gnu_cxx;
#endif

	reserve(r.size());
#if defined(__GNUC__) && !defined(__clang__) && !defined(CXX11_ENABLED)
	transform(r.begin(), r.end(), back_inserter(*this), identity<CHARACTER_SET::value_type>());
#else
	insert(end(), r.begin(), r.end());
#endif

	if (CHARACTER_MANAGER::instance().BeginPendingDestroy())
		m_bMyBegin = true;
}

CharacterVectorInteractor::~CharacterVectorInteractor()
{
	if (m_bMyBegin)
		CHARACTER_MANAGER::instance().FlushPendingDestroy();
}

#ifdef EXPANDED_RELOAD_COMMANDS
void CHARACTER_MANAGER::DestroyCharacterInMap(long lMapIndex)
{
	std::vector<LPCHARACTER> tempVec;
	for (itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.begin(); it != m_map_pkChrByVID.end(); it++)
	{
		LPCHARACTER pkChr = it->second;
		if (pkChr && pkChr->GetMapIndex() == lMapIndex && pkChr->IsNPC() && !pkChr->IsPet() && pkChr->GetRider() == NULL)
		{
			tempVec.push_back(pkChr);
		}
	}
	for (std::vector<LPCHARACTER>::iterator it = tempVec.begin(); it != tempVec.end(); it++)
	{
		DestroyCharacter(*it);
	}
}
#endif


#ifdef ENABLE_GLOBAL_REWARD
typedef struct SRewardGlobal
{
	BYTE	bType;
	DWORD	dwSubValue;
	std::vector<std::pair<DWORD, WORD>> mapRewardList;
	SRewardGlobal(const BYTE _bType, const DWORD _dwSubValue, const std::vector<std::pair<DWORD, WORD>> _mapRewardList) : bType(_bType), dwSubValue(_dwSubValue), mapRewardList(_mapRewardList){}
}TRewardGlobal;

const std::map<BYTE, TRewardGlobal> m_mapRewardGlobal = {
	{
		0, TRewardGlobal(
			REWARD_MISSION_LEVEL_UP,
			175,
			{
				{80018, 1},
				{77404, 25},
			}
		)
	},
	{
		1, TRewardGlobal(
			REWARD_MISSION_SKILL_UPGRADE,
			40,
			{
				{80017, 1},
			}
		)
	},
	{
		2, TRewardGlobal(
			REWARD_MISSION_FIRST_ITEM,
			166079,
			{
				{80018, 1},
				{77404, 50},
			}
		)
	},
	{
		3, TRewardGlobal(
			REWARD_MISSION_FIRST_ITEM,
			166119,
			{
				{80018, 1},
				{77404, 50},
			}
		)
	},
	{
		4, TRewardGlobal(
			REWARD_MISSION_FIRST_ITEM,
			166099,
			{
				{80018, 1},
				{77404, 50},
			}
		)
	},
	{
		5, TRewardGlobal(
			REWARD_MISSION_FIRST_ITEM,
			77943,
			{
				{80018, 1},
				{77942, 1},
			}
		)
	},
	{
		6, TRewardGlobal(
			REWARD_MISSION_FIRST_ITEM,
			77912,
			{
				{80018, 1},
				{77914, 1},
			}
		)
	},
	{
		7, TRewardGlobal(
			REWARD_MISSION_FIRST_ITEM,
			51003,
			{
				{80018, 1},
			}
		)
	},
	{
		8, TRewardGlobal(
			REWARD_MISSION_BATTLEPASS,
			0,
			{
				{80018, 1},
			}
		)
	},
	{
		9, TRewardGlobal(
			REWARD_MISSION_BIOLOG,
			120,
			{
				{80018, 1},
			}
		)
	},
	{
		10, TRewardGlobal(
			REWARD_MISSION_OFFLINESHOP_SLOT,
			40,
			{
				{80018, 1},
			}
		)
	},
};
/*
typedef struct SRewardSolo
{
	BYTE	bType;
	DWORD	dwSubValue;
	DWORD	dwMaxValue;
	BYTE	bLevelDifference;
	std::vector<std::pair<DWORD, WORD>> mapRewardList;
	SRewardSolo(const BYTE _bType, const DWORD _dwSubValue, const DWORD _dwMaxValue, const BYTE _bLevelDifference, const std::vector<std::pair<DWORD, WORD>> _mapRewardList) : bType(_bType), dwSubValue(_dwSubValue), dwMaxValue(_dwMaxValue), bLevelDifference(_bLevelDifference), mapRewardList(_mapRewardList) {}
}TRewardSolo;
const std::map<BYTE, TRewardSolo> m_mapRewardSolo = {
	{
		0, TRewardSolo(
			REWARD_MISSION_LEVEL_UP,
			120,
			1,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		1, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			6500,
			50,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		2, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			6501,
			200,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		3, TRewardSolo(
			REWARD_MISSION_USE_ITEM,
			67997,
			10000,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		4, TRewardSolo(
			REWARD_MISSION_USE_ITEM,
			67998,
			50,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		5, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			2862,
			250,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		6, TRewardSolo(
			REWARD_MISSION_KILL_STONE,
			0,
			100000,
			15,
			{
				{65703, 1},
			}
		)
	},
	{
		7, TRewardSolo(
			REWARD_MISSION_DUNGEON,
			0,
			1000,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		8, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			2493,
			100,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		9, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			2598,
			100,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		10, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			2092,
			100,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		11, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			6091,
			400,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		12, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			6191,
			400,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		13, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			20435,
			100,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		14, TRewardSolo(
			REWARD_MISSION_COMPLETE_SKILL,
			0,
			1,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		15, TRewardSolo(
			REWARD_MISSION_KILL_BOSS,
			0,
			2500,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		16, TRewardSolo(
			REWARD_MISSION_PLAYTIME,
			0,
			43200,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		17, TRewardSolo(
			REWARD_MISSION_OFFLINESHOP_SLOT,
			40,
			1,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		18, TRewardSolo(
			REWARD_MISSION_INVENTORY_SLOT,
			40,
			1,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		19, TRewardSolo(
			REWARD_MISSION_BUY_ITEM,
			51501,
			1,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		20, TRewardSolo(
			REWARD_MISSION_USE_ITEM,
			71084,
			100000,
			0,
			{
				{65703, 1},
			}
		)
	},
	{
		20, TRewardSolo(
			REWARD_MISSION_BATTLEPASS,
			0,
			1,
			0,
			{
				{65703, 1},
			}
		)
	},
};
*/

bool CHARACTER_MANAGER::GetGlobalReward(std::string& szPlayerName, const char* szFilename, ...)
{
	szPlayerName = "";
	char newFileName[254];
	va_list args;
	va_start(args, szFilename);
	vsnprintf(newFileName, sizeof(newFileName), szFilename, args);
	va_end(args);
	FILE* fp;
	if ((fp = fopen(newFileName, "r")) != NULL)
	{
		char	one_line[256];
		while (fgets(one_line, 256, fp))
			break;

		if (strlen(one_line))
			szPlayerName = one_line;

		fclose(fp);
	}

	return szPlayerName.length() ? true : false;
}

void CHARACTER_MANAGER::SetGlobalReward(const char* szPlayerName, const char* szFilename, ...)
{
	char newFileName[254];
	va_list args;
	va_start(args, szFilename);
	vsnprintf(newFileName, sizeof(newFileName), szFilename, args);
	va_end(args);
	FILE* fp;
	if ((fp = fopen(newFileName, "w+")) != NULL)
	{
		fprintf(fp, szPlayerName);
		fclose(fp);
	}
}

void CHARACTER_MANAGER::SendRewardInfo(const BYTE bType, const bool isGlobal, bool isP2P, LPCHARACTER ch)
{
	std::string szCommand("");
	bool clean = true;

	if (ch != NULL && bType == 255)
		ch->SetProtectTime("reward_opened", 1);

	// if (isGlobal)
	// {
	if (isP2P)
	{
		TPacketGGRewardInfo p;
		p.bHeader = HEADER_GG_REWARD_INFO;
		p.bType = bType;
		P2P_MANAGER::Instance().Send(&p, sizeof(TPacketGGRewardInfo));
	}

	if (bType == 255)
	{
		clean = true;

		for (itertype(m_mapRewardGlobal) it = m_mapRewardGlobal.begin(); it != m_mapRewardGlobal.end(); ++it)
		{
			std::string szPlayerName("");
			if (GetGlobalReward(szPlayerName, "%s/reward_global/%u", LocaleService_GetBasePath().c_str(), it->first))
			{
				szCommand += std::to_string(it->first);
				szCommand += "?";
				szCommand += szPlayerName.c_str();
				szCommand += "#";
			}
		}
	}
	else
	{
		if (ch && ch->GetProtectTime("reward_opened") == 0)
			return;

		clean = false;
		
		itertype(m_mapRewardGlobal) it = m_mapRewardGlobal.find(bType);
		if (it != m_mapRewardGlobal.end())
		{
			std::string szPlayerName("");
			if (GetGlobalReward(szPlayerName, "%s/reward_global/%u", LocaleService_GetBasePath().c_str(), it->first))
			{
				szCommand += std::to_string(it->first);
				szCommand += "?";
				szCommand += szPlayerName.c_str();
				szCommand += "#";
			}
		}
	}
	// }
	// else
	// {
	// 	if (!ch)
	// 		return;

	// 	if (bType == 255)
	// 	{
	// 		clean = true;

	// 		for (itertype(m_mapRewardSolo) it = m_mapRewardSolo.begin(); it != m_mapRewardSolo.end(); ++it)
	// 		{
	// 			szCommand += std::to_string(it->first);
	// 			szCommand += "?";
	// 			szCommand += std::to_string(ch->GetRewardData(it->first));
	// 			szCommand += "#";
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (ch->GetProtectTime("reward_opened") == 0)
	// 			return;

	// 		clean = false;

	// 		itertype(m_mapRewardSolo) it = m_mapRewardSolo.find(bType);
	// 		if (it != m_mapRewardSolo.end())
	// 		{
	// 			szCommand += std::to_string(it->first);
	// 			szCommand += "?";
	// 			szCommand += std::to_string(ch->GetRewardData(it->first));
	// 			szCommand += "#";
	// 		}
	// 	}
	// }

	if (!szCommand.length())
		szCommand = "empty";

	if (ch)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RewardData %d %d %s", isGlobal, clean, szCommand.c_str());
	}
	else
	{
		const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
		if (c_ref_set.size())
		{
			for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
			{
				auto desc = *it;
				if (desc)
				{
					LPCHARACTER tch = desc->GetCharacter();
					if (tch && tch->GetProtectTime("reward_opened") == 1)
						tch->ChatPacket(CHAT_TYPE_COMMAND, "RewardData %d %d %s", isGlobal, clean, szCommand.c_str());
				}
			}
		}
	}
}


bool level_check_difference(int iLevel, int yLevel, int iDifLev)
{
	return ((iLevel - iDifLev <= yLevel) && (iLevel + iDifLev >= yLevel));
}

void CHARACTER_MANAGER::DoReward(LPCHARACTER ch, const BYTE bType, const DWORD dwSubValue, DWORD value, const BYTE levelDifference)
{
	if (!ch)
		return;

	if (ch->IsGM())
		return;

	for (auto it = m_mapRewardGlobal.begin(); it != m_mapRewardGlobal.end(); ++it)
	{
		const TRewardGlobal& missionData = it->second;
		if (missionData.bType == bType)
		{
			if (missionData.dwSubValue != 0 && missionData.dwSubValue != dwSubValue)
				continue;
	
			std::string szPlayerName;
			if (!GetGlobalReward(szPlayerName, "%s/reward_global/%u", LocaleService_GetBasePath().c_str(), it->first))
			{
				SetGlobalReward(ch->GetName(), "%s/reward_global/%u", LocaleService_GetBasePath().c_str(), it->first);
				SendRewardInfo(it->first, true, true);

				//reward_part
				for (BYTE i = 0; i < missionData.mapRewardList.size(); ++i)
					ch->AutoGiveItem(missionData.mapRewardList[i].first, missionData.mapRewardList[i].second);

			}
		}
	}

	/*for (auto it = m_mapRewardSolo.begin(); it != m_mapRewardSolo.end(); ++it)
	{
		const TRewardSolo& missionData = it->second;
		if (missionData.bType == bType)
		{
			if (missionData.dwSubValue != 0 && missionData.dwSubValue != dwSubValue)
				continue;
			else if (missionData.bLevelDifference != 0 && !level_check_difference(ch->GetLevel(), levelDifference, missionData.bLevelDifference))
				continue;
			DWORD currentVal = ch->GetRewardData(it->first);
			if (currentVal >= missionData.dwMaxValue)
				continue;
			currentVal += value;
			if (currentVal > missionData.dwMaxValue)
				currentVal = missionData.dwMaxValue;
			ch->SetRewardData(it->first, currentVal);
			SendRewardInfo(it->first, false, false, ch);

			//reward_part
			if (currentVal == missionData.dwMaxValue)
			{
				for (BYTE i = 0; i < missionData.mapRewardList.size(); ++i)
					ch->AutoGiveItem(missionData.mapRewardList[i].first, missionData.mapRewardList[i].second);
			}
		}
	}*/
}
#endif

#ifdef ENABLE_ITEMSHOP
void CHARACTER_MANAGER::SendItemshopSingleRemoveItem(TItemshopItemTable* item)
{
	for (const auto& [player_id, ch] : m_map_pkChrByPID)
	{
		if (!ch)
			continue;
		ch->SendItemshopSingleItemRemovePacket(item);
	}
}

void CHARACTER_MANAGER::SendItemshopSingleAddItem(TItemshopItemTable* item)
{
	const auto& categories = CItemshopManager::instance().GetItemshopCategories();
	const auto& category_info = categories.find(item->byCategory);
	if (category_info == categories.end())
	{
		// This could NEVER happen but it seems like it does
		return;
	}
	for (const auto& [player_id, ch] : m_map_pkChrByPID)
	{
		if (!ch)
			continue;
		ch->SendItemshopSingleItemAddPacket(item, category_info->second);
	}
}

void CHARACTER_MANAGER::SendItemshopSingleItemRefresh(TItemshopItemTable* item)
{
	for (const auto& [player_id, ch] : m_map_pkChrByPID)
	{
		if (!ch)
			continue;
		ch->SendItemshopSingleItemRefreshPacket(item);
	}
}

void CHARACTER_MANAGER::SendItemshopItems()
{
	const auto& items = CItemshopManager::instance().GetItemshopItems();
	const auto& categories = CItemshopManager::instance().GetItemshopCategories();
	for (const auto& [player_id, ch] : m_map_pkChrByPID)
	{
		if (!ch)
			continue;
		ch->SendItemshopItemsPacket(items, categories);
	}
}
#endif

#ifdef ENABLE_EVENT_CALENDAR
void CHARACTER_MANAGER::ClearEventData()
{
	m_eventData.clear();
}

void CHARACTER_MANAGER::CheckBonusEvent(LPCHARACTER ch)
{
	const TEventManagerData* eventPtr = CheckEventIsActive(BONUS_EVENT, ch->GetEmpire());
	if (eventPtr)
		ch->ApplyPoint(eventPtr->value[0], eventPtr->value[1]);
}

const TEventManagerData* CHARACTER_MANAGER::CheckEventIsActive(uint8_t eventIndex, uint8_t empireIndex)
{
	const time_t cur_Time = time(NULL);
	const struct tm vKey = *localtime(&cur_Time);

	for (auto it = m_eventData.begin(); it != m_eventData.end(); ++it)
	{
		const auto& dayIndex = it->first;
		const auto& dayVector = it->second;
		for (uint32_t j = 0; j < dayVector.size(); ++j)
		{
			const auto& eventData = dayVector[j];
			if (eventData.eventIndex == eventIndex)
			{
				if (eventData.channelFlag != 0)
					if (eventData.channelFlag != g_bChannel)
						continue;

				if (eventData.empireFlag != 0 && empireIndex != 0)
					if (eventData.empireFlag != empireIndex)
						continue;

				if (eventData.eventStatus == true)
					return &eventData;

				//if (cur_Time >= eventData.startTime && cur_Time <= eventData.endTime)
				//	return &eventData;
			}
		}
	}
	return NULL;
}

void CHARACTER_MANAGER::CheckEventForDrop(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item)
{
	const uint8_t killerEmpire = pkKiller->GetEmpire();
	const TEventManagerData* eventPtr = NULL;
	LPITEM rewardItem = NULL;
	int iLevelRange = 10;
	// DOUBLE_METIN_LOOT_EVENT
	if (pkChr->IsStone())
	{
		eventPtr = CheckEventIsActive(DOUBLE_METIN_LOOT_EVENT, killerEmpire);
		if (eventPtr)
		{
			const int prob = number(1, 1000);
			const int success_prob = eventPtr->value[3];
			if (prob < success_prob)
			{
				std::vector<LPITEM> m_cache;
				for (uint32_t j = 0; j < vec_item.size(); ++j)
				{
					const auto vItem = vec_item[j];
					rewardItem = ITEM_MANAGER::Instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
					if (rewardItem) m_cache.emplace_back(rewardItem);
				}
				for (uint32_t j = 0; j < m_cache.size(); ++j)
					vec_item.emplace_back(m_cache[j]);
			}
		}

		// MOONLIGHT_EVENT
		eventPtr = CheckEventIsActive(MOONLIGHT_EVENT, killerEmpire);
		if (eventPtr)
		{
			const int prob = number(1, 1000);
			const int success_prob = eventPtr->value[3];
			if (prob < success_prob)
			{
				LPITEM item = ITEM_MANAGER::Instance().CreateItem(50011, 1, 0, true);
				if (item) vec_item.emplace_back(item);
			}
		}
		// MOONLIGHT_EVENT_END
	}
	// DOUBLE_METIN_LOOT_EVENT_END
	// DOUBLE_BOSS_LOOT_EVENT
	else if (pkChr->GetMobRank() >= MOB_RANK_BOSS)
	{
		eventPtr = CheckEventIsActive(DOUBLE_BOSS_LOOT_EVENT, killerEmpire);
		if (eventPtr)
		{
			const int prob = number(1, 1000);
			const int success_prob = eventPtr->value[3];
			if (prob < success_prob)
			{
				std::vector<LPITEM> m_cache;
				for (uint32_t j = 0; j < vec_item.size(); ++j)
				{
					const auto& vItem = vec_item[j];
					
					// skip pet | mount from x2 at double boss drop
					if (vItem->GetType() == ITEM_PET)
						continue;
					
					if (vItem->GetType() == ITEM_MOUNT)
						continue;
					
					// if (vItem->GetType() == ITEM_COSTUME && vItem->GetSubType() == COSTUME_MOUNT)
						// continue;
					// end
					
					rewardItem = ITEM_MANAGER::Instance().CreateItem(vItem->GetVnum(), vItem->GetCount(), 0, true);
					if (rewardItem) m_cache.emplace_back(rewardItem);
				}
				for (uint32_t j = 0; j < m_cache.size(); ++j)
					vec_item.emplace_back(m_cache[j]);
			}
		}
	}
	// DOUBLE_BOSS_LOOT_EVENT_END
	
	eventPtr = CheckEventIsActive(DOUBLE_MISSION_BOOK_EVENT, killerEmpire);
	if (eventPtr)
	{
		const int prob = number(1, 1000);
		const int success_prob = eventPtr->value[3];
		if (pkChr->GetLevel() >= (pkKiller->GetLevel() - iLevelRange))
		{
			if (prob < success_prob)
			{
				// If you have different book index put here!
				constexpr DWORD m_lbookItems[] = { 50300, 50301, 50302 };
				std::vector<LPITEM> m_cache;
				for (const auto& vItem : vec_item)
				{
					const DWORD itemVnum = vItem->GetVnum();
					for (const auto& missionBook : m_lbookItems)
					{
						if (missionBook == itemVnum)
						{
							rewardItem = ITEM_MANAGER::Instance().CreateItem(itemVnum, vItem->GetCount(), 0, true);
							if (rewardItem) m_cache.emplace_back(rewardItem);

							break;
						}
					}
					for (const auto& rItem : m_cache)
						vec_item.emplace_back(rItem);
				}
			}
		}
	}

	eventPtr = CheckEventIsActive(DUNGEON_TICKET_LOOT_EVENT, killerEmpire);
	if (eventPtr)
	{
		const int prob = number(1, 1000);
		const int success_prob = eventPtr->value[3];
		if (pkChr->GetLevel() >= (pkKiller->GetLevel() - iLevelRange))
		{
			if (prob < success_prob)
			{
				// If you have different book index put here!
				constexpr DWORD m_lticketItems[] = { 71201 };
				std::vector<LPITEM> m_cache;
				for (const auto& vItem : vec_item)
				{
					const DWORD itemVnum = vItem->GetVnum();
					for (const auto& ticketItem : m_lticketItems)
					{
						if (ticketItem == itemVnum)
						{
							rewardItem = ITEM_MANAGER::Instance().CreateItem(itemVnum, vItem->GetCount(), 0, true);
							if (rewardItem) m_cache.emplace_back(rewardItem);

							break;
						}
					}
					for (const auto& rItem : m_cache)
						vec_item.emplace_back(rItem);
				}
			}
		}
	}

	// MOONLIGHT_EVENT
	// eventPtr = CheckEventIsActive(MOONLIGHT_EVENT, killerEmpire);
	// if (eventPtr)
	// {
		// const int prob = number(1, 1000);
		// const int success_prob = eventPtr->value[3];
		// if (pkChr->GetLevel() >= (pkKiller->GetLevel() - iLevelRange))
		// {
			// if (prob < success_prob)
			// {
				// LPITEM item = ITEM_MANAGER::Instance().CreateItem(50011, 1, 0, true);
				// if (item) vec_item.emplace_back(item);
			// }
		// }
	// }
	// MOONLIGHT_EVENT_END
}

void CHARACTER_MANAGER::CompareEventSendData(TEMP_BUFFER* buf)
{
	const uint8_t dayCount = m_eventData.size();
	const uint8_t subIndex = EVENT_MANAGER_LOAD;
	const int cur_Time = time(NULL);
	TPacketGCEventManager p;
	p.header = HEADER_GC_EVENT_MANAGER;
	p.size = sizeof(TPacketGCEventManager) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(int);
	for (auto it = m_eventData.begin(); it != m_eventData.end(); ++it)
	{
		const auto& dayIndex = it->first;
		const auto& dayData = it->second;
		const uint8_t dayEventCount = dayData.size();
		p.size += sizeof(uint8_t) + sizeof(uint8_t) + (dayEventCount * sizeof(TEventManagerData));
	}
	buf->write(&p, sizeof(TPacketGCEventManager));
	buf->write(&subIndex, sizeof(uint8_t));
	buf->write(&dayCount, sizeof(uint8_t));
	buf->write(&cur_Time, sizeof(int));
	for (auto it = m_eventData.begin(); it != m_eventData.end(); ++it)
	{
		const auto& dayIndex = it->first;
		const auto& dayData = it->second;
		const uint8_t dayEventCount = dayData.size();
		buf->write(&dayIndex, sizeof(uint8_t));
		buf->write(&dayEventCount, sizeof(uint8_t));
		if (dayEventCount > 0)
			buf->write(dayData.data(), dayEventCount * sizeof(TEventManagerData));
	}
}

void CHARACTER_MANAGER::UpdateAllPlayerEventData()
{
	TEMP_BUFFER buf;
	CompareEventSendData(&buf);
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
	{
		LPCHARACTER ch = (*it)->GetCharacter();
		if (!ch)
			continue;
		(*it)->Packet(buf.read_peek(), buf.size());
	}
}

void CHARACTER_MANAGER::SendDataPlayer(LPCHARACTER ch)
{
	auto desc = ch->GetDesc();
	if (!desc)
		return;

	TEMP_BUFFER buf;
	CompareEventSendData(&buf);
	desc->Packet(buf.read_peek(), buf.size());
}

bool CHARACTER_MANAGER::CloseEventManuel(uint8_t eventIndex)
{
	auto eventPtr = CheckEventIsActive(eventIndex);
	if (eventPtr != NULL)
	{
		const uint8_t subHeader = EVENT_MANAGER_REMOVE_EVENT;
		db_clientdesc->DBPacketHeader(HEADER_GD_EVENT_MANAGER, 0, sizeof(uint8_t) + sizeof(uint16_t));
		db_clientdesc->Packet(&subHeader, sizeof(uint8_t));
		db_clientdesc->Packet(&eventPtr->eventID, sizeof(uint16_t));
		return true;
	}

	return false;
}

void CHARACTER_MANAGER::SetEventStatus(const uint16_t eventID, const bool eventStatus, const int endTime, const char* endTimeText)
{
	// Event status : 0 OFF  // 1 ON

	TEventManagerData* eventData = NULL;
	for (auto it = m_eventData.begin(); it != m_eventData.end(); ++it)
	{
		if (it->second.size())
		{
			for (uint32_t j = 0; j < it->second.size(); ++j)
			{
				TEventManagerData& pData = it->second[j];
				if (pData.eventID == eventID)
				{
					eventData = &pData;
					break;
				}
			}
		}
	}

	if (eventData == NULL)
		return;

	eventData->eventStatus = eventStatus;
	eventData->endTime = endTime;
	strlcpy(eventData->endTimeText, endTimeText, sizeof(eventData->endTimeText));

	// Auto open & close notice
	const std::map<uint8_t, std::pair<std::string, std::string>> m_eventText = {
		{BONUS_EVENT,{"761","762"}},
		{DOUBLE_BOSS_LOOT_EVENT,{"763","764"}},
		{DOUBLE_METIN_LOOT_EVENT,{"765","766"}},
		{DOUBLE_MISSION_BOOK_EVENT,{"767","768"}},
		{DUNGEON_COOLDOWN_EVENT,{"769","770"}},
		{DUNGEON_TICKET_LOOT_EVENT,{"771","772"}},
		{MOONLIGHT_EVENT,{"773","774"}},
	};

	const auto it = m_eventText.find(eventData->eventIndex);
	if (it != m_eventText.end())
	{
		if (eventStatus)
			SendNotice(LC_TEXT(it->second.first.c_str()));
		else
			SendNotice(LC_TEXT(it->second.second.c_str()));
	}

	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();

	// Bonus event update status
	if (eventData->eventIndex == BONUS_EVENT)
	{
		for (auto itDesc = c_ref_set.begin(); itDesc != c_ref_set.end(); ++itDesc)
		{
			LPCHARACTER ch = (*itDesc)->GetCharacter();
			if (!ch)
				continue;

			if (eventData->empireFlag != 0)
			{
				if (eventData->empireFlag != ch->GetEmpire())
					continue;
			}
			if (eventData->channelFlag != 0)
			{
				if (eventData->channelFlag != g_bChannel)
					return;
			}
			if (!eventStatus)
			{
				const int32_t value = eventData->value[1];
				ch->ApplyPoint(eventData->value[0], -value);
			}

			ch->ComputePoints();
		}
	}

	// if (eventData->eventIndex == EVENT_HIDE && eventStatus)
	// {
	// 	// EventManagerAddons::Instance().OnTriggerEvent();
	// 	quest::CQuestManager::instance().RequestSetEventFlag("enable_hide_seek_event", eventStatus);
	// }

	// if (eventData->eventIndex == EVENT_PVM)
	// {
	// 	// EventManagerAddons::Instance().OnTriggerBossEvent(eventStatus);
	// 	quest::CQuestManager::instance().RequestSetEventFlag("enable_pvm_event", eventStatus);
	// }

	// if (eventData->eventIndex == EVENT_FISHING)
	// 	quest::CQuestManager::instance().RequestSetEventFlag("enable_fish_event", eventStatus);

	const int now = time(0);
	const uint8_t subIndex = EVENT_MANAGER_EVENT_STATUS;

	TPacketGCEventManager p;
	p.header = HEADER_GC_EVENT_MANAGER;
	p.size = sizeof(TPacketGCEventManager) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(bool) + sizeof(int) + sizeof(int);
	p.size = sizeof(TPacketGCEventManager) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(bool) + sizeof(int) + sizeof(int) + sizeof(eventData->endTimeText);

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(TPacketGCEventManager));
	buf.write(&subIndex, sizeof(uint8_t));
	buf.write(&eventData->eventID, sizeof(uint16_t));
	buf.write(&eventData->eventStatus, sizeof(bool));
	buf.write(&eventData->endTime, sizeof(int));
	buf.write(&eventData->endTimeText, sizeof(eventData->endTimeText));

	buf.write(&now, sizeof(int));

	for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
	{
		LPCHARACTER ch = (*it)->GetCharacter();
		if (!ch)
			continue;

		(*it)->Packet(buf.read_peek(), buf.size());
	}

}
void CHARACTER_MANAGER::SetEventData(uint8_t dayIndex, const std::vector<TEventManagerData>& m_data)
{
	const auto it = m_eventData.find(dayIndex);
	if (it == m_eventData.end())
		m_eventData.emplace(dayIndex, m_data);
	else
	{
		it->second.clear();
		for (uint32_t j = 0; j < m_data.size(); ++j)
			it->second.emplace_back(m_data[j]);
	}
}
#endif

#ifdef ENABLE_REWARD_SYSTEM
const char* GetRewardIndexToString(BYTE rewardIndex)
{
	std::map<BYTE, std::string> rewardNames = {
		{REWARD_120,"REWARD_120"},
		{REWARD_PET_115,"REWARD_PET_115"},
		{REWARD_LEGENDARY_SKILL,"REWARD_LEGENDARY_SKILL"},
		{REWARD_LEGENDARY_SKILL_SET,"REWARD_LEGENDARY_SKILL_SET"},
		{REWARD_HYDRA,"REWARD_HYDRA"},
		{REWARD_MELEY,"REWARD_MELEY"},
		{REWARD_AKZADUR,"REWARD_AKZADUR"},
		{REWARD_EKZEKIEL,"REWARD_EKZEKIEL"},
		{REWARD_AVERAGE,"REWARD_AVERAGE"},
		{REWARD_INVEN_SLOT,"REWARD_INVEN_SLOT"},
		{REWARD_OFFLINE_SLOT,"REWARD_OFFLINE_SLOT"},
		{REWARD_BATTLEPASS,"REWARD_BATTLEPASS"},
		{REWARD_ACCE,"REWARD_ACCE"},
		{REWARD_LEADERSHIP,"REWARD_LEADERSHIP"},
		{REWARD_CRYSTAL,"REWARD_CRYSTAL"},
		{REWARD_BUFFI,"REWARD_BUFFI"},
		{REWARD_BIOLOG,"REWARD_BIOLOG"},
		{REWARD_ELEMENT_PAGE,"REWARD_ELEMENT_PAGE"},
		{REWARD_FIRST_WEAPON_ZODIAC_CRAFT,"REWARD_FIRST_WEAPON_ZODIAC_CRAFT"},
		{FIRST_AVERAGE_ZODIAC_50,"FIRST_AVERAGE_ZODIAC_50"},

	};

	auto it = rewardNames.find(rewardIndex);
	if (it != rewardNames.end())
		return it->second.c_str();

	return 0;
}

BYTE GetRewardIndex(const char* szRewardName)
{
	std::map<std::string, BYTE> rewardNames = {
		{"REWARD_120",REWARD_120},
		{"REWARD_PET_115",REWARD_PET_115},
		{"REWARD_LEGENDARY_SKILL",REWARD_LEGENDARY_SKILL},
		{"REWARD_LEGENDARY_SKILL_SET",REWARD_LEGENDARY_SKILL_SET},
		{"REWARD_HYDRA",REWARD_HYDRA},
		{"REWARD_MELEY",REWARD_MELEY},
		{"REWARD_AKZADUR",REWARD_AKZADUR},
		{"REWARD_EKZEKIEL",REWARD_EKZEKIEL},
		{"REWARD_AVERAGE",REWARD_AVERAGE},
		{"REWARD_INVEN_SLOT",REWARD_INVEN_SLOT},
		{"REWARD_OFFLINE_SLOT",REWARD_OFFLINE_SLOT},
		{"REWARD_BATTLEPASS",REWARD_BATTLEPASS},
		{"REWARD_ACCE",REWARD_ACCE},
		{"REWARD_LEADERSHIP",REWARD_LEADERSHIP},
		{"REWARD_CRYSTAL",REWARD_CRYSTAL},
		{"REWARD_BUFFI",REWARD_BUFFI},
		{"REWARD_BIOLOG",REWARD_BIOLOG},
		{"REWARD_ELEMENT_PAGE",REWARD_ELEMENT_PAGE},
		{"REWARD_FIRST_WEAPON_ZODIAC_CRAFT",REWARD_FIRST_WEAPON_ZODIAC_CRAFT},
		{"FIRST_AVERAGE_ZODIAC_50",FIRST_AVERAGE_ZODIAC_50},
	};
	
	auto it = rewardNames.find(szRewardName);
	if (it != rewardNames.end())
		return it->second;

	return 0;
}
void CHARACTER_MANAGER::LoadRewardData()
{
	m_rewardData.clear();

	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery), "SELECT rewardIndex, lc_text, playerName, itemVnum0, itemCount0, itemVnum1, itemCount1, itemVnum2, itemCount2 FROM player.reward_table");
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));

	if (pMsg->Get()->uiNumRows != 0)
	{
		MYSQL_ROW row = NULL;
		for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
		{
			int col = 0;
			TRewardInfo p;
			p.m_rewardItems.clear();

			char rewardName[50];
			DWORD rewardIndex;
			strlcpy(rewardName, row[col++], sizeof(rewardName));
			rewardIndex = GetRewardIndex(rewardName);

			strlcpy(p.lc_text, row[col++], sizeof(p.lc_text));
			strlcpy(p.playerName, row[col++], sizeof(p.playerName));
			for (BYTE j = 0; j < 3; ++j)
			{
				DWORD itemVnum, itemCount;
				str_to_number(itemVnum, row[col++]);
				str_to_number(itemCount, row[col++]);
				p.m_rewardItems.emplace_back(itemVnum, itemCount);
			}
			m_rewardData.emplace(rewardIndex, p);
		}
	}
}

bool CHARACTER_MANAGER::IsRewardEmpty(BYTE rewardIndex)
{
	auto it = m_rewardData.find(rewardIndex);
	if (it != m_rewardData.end())
	{
		if (strcmp(it->second.playerName, "") == 0)
			return true;
	}

	return false;
}

void CHARACTER_MANAGER::SendRewardInfo(LPCHARACTER ch)
{
	ch->SetProtectTime("RewardInfo", 1);
	std::string cmd="";//12
	if (m_rewardData.size())
	{
		for (auto it = m_rewardData.begin(); it != m_rewardData.end(); ++it)
		{
			if (strlen(it->second.playerName) > 1)
			{
				char text[60];
				snprintf(text, sizeof(text), "%d|%s#", it->first, it->second.playerName);
				cmd += text;
			}
			if (strlen(cmd.c_str()) + 12 > CHAT_MAX_LEN - 30)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "RewardInfo %s", cmd.c_str());
				cmd = "";
			}
		}
		if (strlen(cmd.c_str()) > 1)
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RewardInfo %s", cmd.c_str());
	}
}

struct RewardForEach
{
	void operator() (LPDESC d)
	{
		LPCHARACTER ch = d->GetCharacter();
		if (ch == NULL)
			return;
		else if (ch->GetProtectTime("RewardInfo") != 1)
			return;

		CHARACTER_MANAGER::Instance().SendRewardInfo(ch);
	}
};

void CHARACTER_MANAGER::SetRewardData(BYTE rewardIndex, const char* playerName, bool isP2P)
{
	if (!IsRewardEmpty(rewardIndex))
		return;

	auto it = m_rewardData.find(rewardIndex);
	if (it == m_rewardData.end())
		return;

	TRewardInfo& rewardInfo = it->second;
	LPCHARACTER ch = FindPC(playerName);

	if (ch && ch->IsGM() && isP2P)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Sorry, but staff can't take reward >.<");
		return;
	}

	strlcpy(rewardInfo.playerName, playerName, sizeof(rewardInfo.playerName));

	if (isP2P)
	{		
		if (ch)
		{
			for (DWORD j = 0; j < rewardInfo.m_rewardItems.size(); ++j)
				ch->AutoGiveItem(rewardInfo.m_rewardItems[j].first, rewardInfo.m_rewardItems[j].second);
		}

		TPacketGGRewardInfo p;
		p.bHeader = HEADER_GG_REWARD_INFO;
		p.rewardIndex = rewardIndex;
		strlcpy(p.playerName, playerName, sizeof(p.playerName));
		P2P_MANAGER::Instance().Send(&p, sizeof(p));

		char szQuery[4096];
		snprintf(szQuery, sizeof(szQuery), "UPDATE player.reward_table SET playerName = '%s' WHERE lc_text = '%s'", playerName, rewardInfo.lc_text);
		auto pMsg(DBManager::instance().DirectQuery(szQuery));
	}

	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), RewardForEach());
}
#endif

#ifdef ENABLE_WORLD_BOSS
#include "text_file_loader.h"
#include "group_text_parse_tree.h"

void CHARACTER_MANAGER::LoadConfigWorldBoss()
{
	if (g_bAuthServer)
		return;
	
	m_vec_worldBoss.clear();
	
	CTextFileLoader loader;

	char c_pszFileName[156];
	snprintf(c_pszFileName, sizeof(c_pszFileName), "%s/config_systems/world_boss.txt", LocaleService_GetBasePath().c_str());

	if (!loader.Load(c_pszFileName))
		return;
	
	std::string stName;
	
	for (DWORD i = 0; i < loader.GetChildNodeCount(); ++i)
	{
		loader.SetChildNode(i);
		loader.GetCurrentNodeName(&stName);

		int vnumMob = 0;
		if (!loader.GetTokenInteger("vnum", &vnumMob))
		{
			sys_err("LoadConfigWorldBoss : Syntax error %s : no vnum, node %s", c_pszFileName, stName.c_str());
			loader.SetParentNode();
			return;
		}
	
		int channel = 0;
		if (!loader.GetTokenInteger("channel", &channel))
		{
			sys_err("LoadConfigWorldBoss : Syntax error %s : no channel, node %s", c_pszFileName, stName.c_str());
			loader.SetParentNode();
			return;
		}
	
		int mapIndex = 0;
		if (!loader.GetTokenInteger("map_index", &mapIndex))
		{
			sys_err("LoadConfigWorldBoss : Syntax error %s : no map_index, node %s", c_pszFileName, stName.c_str());
			loader.SetParentNode();
			return;
		}
	
		int posX = 0;
		if (!loader.GetTokenInteger("x", &posX))
		{
			sys_err("LoadConfigWorldBoss : Syntax error %s : no x, node %s", c_pszFileName, stName.c_str());
			loader.SetParentNode();
			return;
		}
	
		int posY = 0;
		if (!loader.GetTokenInteger("y", &posY))
		{
			sys_err("LoadConfigWorldBoss : Syntax error %s : no y, node %s", c_pszFileName, stName.c_str());
			loader.SetParentNode();
			return;
		}
		
		int iSpawnCycle = -1;
		if (!loader.GetTokenInteger("spawn_cycle_min", &iSpawnCycle))
			iSpawnCycle = -1;

		SConfigWorldBoss newWorldBoss = {};
		newWorldBoss.iLastTimeSpawn = 0;
		newWorldBoss.dwVirtualID = 0;
		newWorldBoss.dwMobVnum = vnumMob;
		newWorldBoss.mapIndex = mapIndex;
		newWorldBoss.posX = posX;
		newWorldBoss.posY = posY;
		newWorldBoss.channelIndex = channel;
		newWorldBoss.iSpawnCycle = iSpawnCycle;
	
		if (newWorldBoss.channelIndex != -1 && !(newWorldBoss.channelIndex == g_bChannel))
		{
			loader.SetParentNode();
			continue; // is not for this channel the spawn
		}

		auto mapIndexBoss = newWorldBoss.mapIndex;
		if (!map_allow_find(mapIndexBoss))
		{
			loader.SetParentNode();
			continue; // is not for this channel the spawn
		}
	
		// Spawn Times Config
		TTokenVector * pTok;
		char buf[32 + 1];
		for (int k = 1; k < 30; ++k)
		{
			snprintf(buf, sizeof(buf), "spawn_%d", k);
			if (loader.GetTokenVector(buf, &pTok))
			{
				SConfigWorldBossSpawn newSpawn = {};
				
				str_to_number(newSpawn.bDaySpawn, pTok->at(0).c_str());
				str_to_number(newSpawn.bHourSpawn, pTok->at(1).c_str());
				str_to_number(newSpawn.bMinSpawn, pTok->at(2).c_str());
				
				newWorldBoss.vecSpawns.push_back(newSpawn);
			}
		}
		
		if (iSpawnCycle == -1)
			m_map_worldBoss[vnumMob] = true;
		
		m_vec_worldBoss.push_back(newWorldBoss);
		loader.SetParentNode();
	}
}

bool CHARACTER_MANAGER::CheckWorldBossSpawn()
{
	if (g_bAuthServer || m_vec_worldBoss.empty())
		return false;

	time_t cTime = get_global_time();
	struct tm vKey = *localtime(&cTime);

	auto day = vKey.tm_mday;
	auto hour = vKey.tm_hour;
	auto minute = vKey.tm_min;
	
	//debug
	// sys_err("CheckWorldBossSpawn: current day %d hour %d min %d", day, hour, minute);
	
	for (auto &it : m_vec_worldBoss)
	{
		if (it.iLastTimeSpawn != 0 && it.iLastTimeSpawn > get_global_time())
			continue;

		if (it.channelIndex != -1) // has ch config
		{
			if (!(it.channelIndex == g_bChannel))
				continue; // is not for this channel the spawn
		}
		
		auto mapIndexBoss = it.mapIndex;
		if (!map_allow_find(mapIndexBoss))
			continue; // is not for this core the spawn... or map can't be found
		
		auto dwVIDMob = it.dwVirtualID;
		if (dwVIDMob != 0 && CHARACTER_MANAGER::instance().Find(dwVIDMob))
			continue; // the mob is already spawned
		
		bool bIsTimeForSpawn = false;

		for (auto &itSpawn : it.vecSpawns)
		{
			if (!(itSpawn.bDaySpawn == -1 || day == itSpawn.bDaySpawn) || !(hour == itSpawn.bHourSpawn) || !(minute == itSpawn.bMinSpawn))
				continue; // is not the day or the hour or the minute to be spawned this mob
			
			bIsTimeForSpawn = true;
			break;
		}
		
		if (it.iSpawnCycle != -1 && minute % it.iSpawnCycle == 0)
		{
			bIsTimeForSpawn = true;
		}
		
		if (!bIsTimeForSpawn)
			continue;

		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(mapIndexBoss);
		if (pkSectreeMap == NULL) {
			sys_err("CheckWorldBossSpawn: SECTREE_MAP not found for #%ld", mapIndexBoss);
			continue;
		}

		LPCHARACTER pMob = CHARACTER_MANAGER::instance().SpawnMob(it.dwMobVnum, mapIndexBoss,
			pkSectreeMap->m_setting.iBaseX+it.posX*100,
			pkSectreeMap->m_setting.iBaseY+it.posY*100,
			0);

		if (pMob == NULL)
		{
			sys_err("CheckWorldBossSpawn: CANT SPAWN MOB not found for #%d", it.dwMobVnum);
			continue;
		}
		
		// Append Mob VID to map
		it.dwVirtualID = pMob->GetVID();
		it.iLastTimeSpawn = get_global_time() + 100;

		// char buf[256];
		// snprintf(buf, sizeof(buf), "%s is now Spawned!", pMob->GetName());
		// SendNotice(buf);
	}
	
	return true;
}

#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
