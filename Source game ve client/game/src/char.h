#ifndef __INC_METIN_II_CHAR_H__
#define __INC_METIN_II_CHAR_H__

#include <boost/unordered_map.hpp>
#include "../../common/stl.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "vid.h"
#include "constants.h"
#include "affect.h"
#include "affect_flag.h"
#include "cube.h"
#include "mining.h"
#include "../../common/CommonDefines.h"
#include <array>
#include <utility>
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	#include <vector>
#endif
#if defined(ENABLE_QUICK_OPEN) && defined(_WIN32)
	#include <unordered_set>
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	#include "BiologSystemManager.hpp"
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	#include "packet.h"
#endif

#define ENABLE_ANTI_CMD_FLOOD
#define ENABLE_OPEN_SHOP_WITH_ARMOR
// #define ENABLE_SKILL_COOLDOWN_CHECK

enum eMountType {MOUNT_TYPE_NONE=0, MOUNT_TYPE_NORMAL=1, MOUNT_TYPE_COMBAT=2, MOUNT_TYPE_MILITARY=3};
eMountType GetMountLevelByVnum(DWORD dwMountVnum, bool IsNew);
const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);

class CBuffOnAttributes;
class CPetSystem;
#ifdef __PREMIUM_PRIVATE_SHOP__
	class CPrivateShop;
#endif

#define INSTANT_FLAG_DEATH_PENALTY		(1 << 0)
#define INSTANT_FLAG_SHOP			(1 << 1)
#define INSTANT_FLAG_EXCHANGE			(1 << 2)
#define INSTANT_FLAG_STUN			(1 << 3)
#define INSTANT_FLAG_NO_REWARD			(1 << 4)

#define AI_FLAG_NPC				(1 << 0)
#define AI_FLAG_AGGRESSIVE			(1 << 1)
#define AI_FLAG_HELPER				(1 << 2)
#define AI_FLAG_STAYZONE			(1 << 3)

#define SET_OVER_TIME(ch, time)	(ch)->SetOverTime(time)

extern int g_nPortalLimitTime;

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
#ifdef ENABLE_WOLFMAN_CHARACTER
	MAIN_RACE_WOLFMAN_M,
#endif
	MAIN_RACE_MAX_NUM,
};

enum
{
	POISON_LENGTH = 30,
#ifdef ENABLE_WOLFMAN_CHARACTER
	BLEEDING_LENGTH = 30,
#endif
	STAMINA_PER_STEP = 1,
	SAFEBOX_PAGE_SIZE = 9,
	AI_CHANGE_ATTACK_POISITION_TIME_NEAR = 10000,
	AI_CHANGE_ATTACK_POISITION_TIME_FAR = 1000,
	AI_CHANGE_ATTACK_POISITION_DISTANCE = 100,
	SUMMON_MONSTER_COUNT = 3,
};

enum
{
	FLY_NONE,
	FLY_EXP,
	FLY_HP_MEDIUM,
	FLY_HP_BIG,
	FLY_SP_SMALL,
	FLY_SP_MEDIUM,
	FLY_SP_BIG,
	FLY_FIREWORK1,
	FLY_FIREWORK2,
	FLY_FIREWORK3,
	FLY_FIREWORK4,
	FLY_FIREWORK5,
	FLY_FIREWORK6,
	FLY_FIREWORK_CHRISTMAS,
	FLY_CHAIN_LIGHTNING,
	FLY_HP_SMALL,
	FLY_SKILL_MUYEONG,
#ifdef ENABLE_QUIVER_SYSTEM
	FLY_QUIVER_ATTACK_NORMAL,
#endif
};

enum EDamageType
{
	DAMAGE_TYPE_NONE,
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_NORMAL_RANGE,
	DAMAGE_TYPE_MELEE,
	DAMAGE_TYPE_RANGE,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_ELEC,
	DAMAGE_TYPE_MAGIC,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_SPECIAL,
#ifdef ENABLE_WOLFMAN_CHARACTER
	DAMAGE_TYPE_BLEEDING,
#endif
};

enum DamageFlag
{
	DAMAGE_NORMAL	= (1 << 0),
	DAMAGE_POISON	= (1 << 1),
	DAMAGE_DODGE	= (1 << 2),
	DAMAGE_BLOCK	= (1 << 3),
	DAMAGE_PENETRATE= (1 << 4),
	DAMAGE_CRITICAL = (1 << 5),
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_BLEEDING_AS_POISON)
	DAMAGE_BLEEDING	= (1 << 6),
#endif
};

enum EPointTypes
{
	POINT_NONE,                 // 0
	POINT_LEVEL,                // 1
	POINT_VOICE,                // 2
	POINT_EXP,                  // 3
	POINT_NEXT_EXP,             // 4
	POINT_HP,                   // 5
	POINT_MAX_HP,               // 6
	POINT_SP,                   // 7
	POINT_MAX_SP,               // 8
	POINT_STAMINA,              // 9
	POINT_MAX_STAMINA,          // 10

	POINT_GOLD,                 // 11
	POINT_ST,                   // 12
	POINT_HT,                   // 13
	POINT_DX,                   // 14
	POINT_IQ,                   // 15
	POINT_DEF_GRADE,		// 16 ...
	POINT_ATT_SPEED,            // 17
	POINT_ATT_GRADE,		// 18
	POINT_MOV_SPEED,            // 19
	POINT_CLIENT_DEF_GRADE,	// 20
	POINT_CASTING_SPEED,        // 21
	POINT_MAGIC_ATT_GRADE,      // 22
	POINT_MAGIC_DEF_GRADE,      // 23
	POINT_EMPIRE_POINT,         // 24
	POINT_LEVEL_STEP,           // 25
	POINT_STAT,                 // 26
	POINT_SUB_SKILL,		// 27
	POINT_SKILL,		// 28
	POINT_WEAPON_MIN,		// 29
	POINT_WEAPON_MAX,		// 30
	POINT_PLAYTIME,             // 31
	POINT_HP_REGEN,             // 32
	POINT_SP_REGEN,             // 33

	POINT_BOW_DISTANCE,         // 34

	POINT_HP_RECOVERY,          // 35
	POINT_SP_RECOVERY,          // 36

	POINT_POISON_PCT,           // 37
	POINT_STUN_PCT,             // 38
	POINT_SLOW_PCT,             // 39
	POINT_CRITICAL_PCT,         // 40
	POINT_PENETRATE_PCT,        // 41
	POINT_CURSE_PCT,            // 42

	POINT_ATTBONUS_HUMAN,       // 43
	POINT_ATTBONUS_ANIMAL,      // 44
	POINT_ATTBONUS_ORC,         // 45
	POINT_ATTBONUS_MILGYO,      // 46
	POINT_ATTBONUS_UNDEAD,      // 47
	POINT_ATTBONUS_DEVIL,       // 48
	POINT_ATTBONUS_INSECT,      // 49
	POINT_ATTBONUS_FIRE,        // 50
	POINT_ATTBONUS_ICE,         // 51
	POINT_ATTBONUS_DESERT,      // 52
	POINT_ATTBONUS_MONSTER,     // 53
	POINT_ATTBONUS_WARRIOR,     // 54
	POINT_ATTBONUS_ASSASSIN,	// 55
	POINT_ATTBONUS_SURA,		// 56
	POINT_ATTBONUS_SHAMAN,		// 57
	POINT_ATTBONUS_TREE,     	// 58

	POINT_RESIST_WARRIOR,		// 59
	POINT_RESIST_ASSASSIN,		// 60
	POINT_RESIST_SURA,			// 61
	POINT_RESIST_SHAMAN,		// 62

	POINT_STEAL_HP,             // 63
	POINT_STEAL_SP,             // 64

	POINT_MANA_BURN_PCT,        // 65
	POINT_DAMAGE_SP_RECOVER,    // 66

	POINT_BLOCK,                // 67
	POINT_DODGE,                // 68

	POINT_RESIST_SWORD,         // 69
	POINT_RESIST_TWOHAND,       // 70
	POINT_RESIST_DAGGER,        // 71
	POINT_RESIST_BELL,          // 72
	POINT_RESIST_FAN,           // 73
	POINT_RESIST_BOW,           // 74
	POINT_RESIST_FIRE,          // 75
	POINT_RESIST_ELEC,          // 76
	POINT_RESIST_MAGIC,         // 77
	POINT_RESIST_WIND,          // 78

	POINT_REFLECT_MELEE,        // 79

	POINT_REFLECT_CURSE,		// 80
	POINT_POISON_REDUCE,		// 81

	POINT_KILL_SP_RECOVER,		// 82
	POINT_EXP_DOUBLE_BONUS,		// 83
	POINT_GOLD_DOUBLE_BONUS,	// 84
	POINT_ITEM_DROP_BONUS,		// 85

	POINT_POTION_BONUS,			// 86
	POINT_KILL_HP_RECOVERY,		// 87

	POINT_IMMUNE_STUN,			// 88
	POINT_IMMUNE_SLOW,			// 89
	POINT_IMMUNE_FALL,			// 90
	//////////////////

	POINT_PARTY_ATTACKER_BONUS,		// 91
	POINT_PARTY_TANKER_BONUS,		// 92

	POINT_ATT_BONUS,			// 93
	POINT_DEF_BONUS,			// 94

	POINT_ATT_GRADE_BONUS,		// 95
	POINT_DEF_GRADE_BONUS,		// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98

	POINT_RESIST_NORMAL_DAMAGE,		// 99

	POINT_HIT_HP_RECOVERY,		// 100
	POINT_HIT_SP_RECOVERY, 		// 101
	POINT_MANASHIELD,			// 102

	POINT_PARTY_BUFFER_BONUS,		// 103
	POINT_PARTY_SKILL_MASTER_BONUS,	// 104

	POINT_HP_RECOVER_CONTINUE,		// 105
	POINT_SP_RECOVER_CONTINUE,		// 106

	POINT_STEAL_GOLD,			// 107
	POINT_POLYMORPH,			// 108
	POINT_MOUNT,			// 109

	POINT_PARTY_HASTE_BONUS,		// 110
	POINT_PARTY_DEFENDER_BONUS,		// 111
	POINT_STAT_RESET_COUNT,		// 112

	POINT_HORSE_SKILL,			// 113

	POINT_MALL_ATTBONUS,		// 114
	POINT_MALL_DEFBONUS,		// 115
	POINT_MALL_EXPBONUS,		// 116
	POINT_MALL_ITEMBONUS,		// 117
	POINT_MALL_GOLDBONUS,		// 118

	POINT_MAX_HP_PCT,			// 119
	POINT_MAX_SP_PCT,			// 120

	POINT_SKILL_DAMAGE_BONUS,		// 121
	POINT_NORMAL_HIT_DAMAGE_BONUS,	// 122

	// DEFEND_BONUS_ATTRIBUTES
	POINT_SKILL_DEFEND_BONUS,		// 123
	POINT_NORMAL_HIT_DEFEND_BONUS,	// 124
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	POINT_PC_BANG_EXP_BONUS,		// 125
	POINT_PC_BANG_DROP_BONUS,		// 126

	POINT_RAMADAN_CANDY_BONUS_EXP,		// 127

	POINT_ENERGY = 128,

	POINT_ENERGY_END_TIME = 129,

	POINT_COSTUME_ATTR_BONUS = 130,
	POINT_MAGIC_ATT_BONUS_PER = 131,
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,

	POINT_RESIST_ICE = 133,
	POINT_RESIST_EARTH = 134,
	POINT_RESIST_DARK = 135,

	POINT_RESIST_CRITICAL = 136,
	POINT_RESIST_PENETRATE = 137,

#ifdef ENABLE_WOLFMAN_CHARACTER
	POINT_BLEEDING_REDUCE = 138,
	POINT_BLEEDING_PCT = 139,

	POINT_ATTBONUS_WOLFMAN = 140,
	POINT_RESIST_WOLFMAN = 141,
	POINT_RESIST_CLAW = 142,
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	POINT_ACCEDRAIN_RATE = 143,
#endif
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	POINT_RESIST_MAGIC_REDUCTION = 144,
#endif

	POINT_RESIST_MOUNT_FALL		= 146, //unk
	POINT_RESIST_HUMAN			= 147,

	POINT_ENCHANT_ELECT			= 148,
	POINT_ENCHANT_FIRE			= 149,
	POINT_ENCHANT_ICE			= 150,
	POINT_ENCHANT_WIND			= 151,
	POINT_ENCHANT_EARTH			= 152,
	POINT_ENCHANT_DARK			= 153,

	POINT_ATTBONUS_CZ			= 154,
	POINT_ATTBONUS_SWORD		= 155, //unk
	POINT_ATTBONUS_TWOHAND		= 156, //unk
	POINT_ATTBONUS_DAGGER		= 157, //unk
	POINT_ATTBONUS_BELL			= 158, //unk
	POINT_ATTBONUS_FAN			= 159, //unk
	POINT_ATTBONUS_BOW			= 160, //unk
#ifdef ENABLE_WOLFMAN_CHARACTER
	POINT_ATTBONUS_CLAW			= 161, //unk
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	POINT_PRIVATE_SHOP_UNLOCKED_SLOT = 162,
#endif

#ifdef ENABLE_GEM_SYSTEM
	POINT_GEM = 180,
#endif
#ifdef ENABLE_EXTENDED_ATTRS
	POINT_ATTBONUS_STONE = 181,
	POINT_ATTBONUS_BOSS = 182,
	POINT_ATTBONUS_ELEMENTS = 183,
	POINT_ENCHANT_ELEMENTS = 184,
	POINT_ATTBONUS_CHARACTERS = 185,
	POINT_ENCHANT_CHARACTERS = 186,
#endif
#ifdef ENABLE_BATTLE_PASS
	POINT_BATTLE_PASS_ID = 187,
#endif

	POINT_MAX,
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM
};

enum EPositions
{
	POS_DEAD,
	POS_SLEEPING,
	POS_RESTING,
	POS_SITTING,
	POS_FISHING,
	POS_FIGHTING,
	POS_MOUNTING,
	POS_STANDING
};

enum EBlockAction
{
	BLOCK_EXCHANGE		= (1 << 0),
	BLOCK_PARTY_INVITE		= (1 << 1),
	BLOCK_GUILD_INVITE		= (1 << 2),
	BLOCK_WHISPER		= (1 << 3),
	BLOCK_MESSENGER_INVITE	= (1 << 4),
	BLOCK_PARTY_REQUEST		= (1 << 5),
	BLOCK_EQUIPMENT_VIEW = (1 << 6),
};

// <Factor> Dynamically evaluated CHARACTER* equivalent.
// Referring to SCharDeadEventInfo.
struct DynamicCharacterPtr {
	DynamicCharacterPtr() : is_pc(false), id(0) {}
	DynamicCharacterPtr(const DynamicCharacterPtr& o)
		: is_pc(o.is_pc), id(o.id) {}

	// Returns the LPCHARACTER found in CHARACTER_MANAGER.
	LPCHARACTER Get() const;
	// Clears the current settings.
	void Reset() {
		is_pc = false;
		id = 0;
	}

	// Basic assignment operator.
	DynamicCharacterPtr& operator=(const DynamicCharacterPtr& rhs) {
		is_pc = rhs.is_pc;
		id = rhs.id;
		return *this;
	}
	// Supports assignment with LPCHARACTER type.
	DynamicCharacterPtr& operator=(LPCHARACTER character);
	// Supports type casting to LPCHARACTER.
	operator LPCHARACTER() const {
		return Get();
	}

	bool is_pc;
	uint32_t id;
};

typedef struct character_point
{
	long			points[POINT_MAX_NUM];

	BYTE			job;
	BYTE			voice;

	int			level;
	DWORD			exp;
#ifdef ENABLE_GOLD_MAX_EXTENDED
	unsigned long long gold;
#else
	long gold;
#endif
#ifdef ENABLE_GEM_SYSTEM
	long			gem;
#endif

	int				hp;
	int				sp;

	int				iRandomHP;
	int				iRandomSP;

	int				stamina;

	BYTE			skill_group;
} CHARACTER_POINT;

typedef struct character_point_instant
{
	long			points[POINT_MAX_NUM];

	float			fRot;

	int				iMaxHP;
	int				iMaxSP;

	long			position;

	long			instant_flag;
	DWORD			dwAIFlag;
	DWORD			dwImmuneFlag;
	DWORD			dwLastShoutPulse;

	DWORD			parts[PART_MAX_NUM]; // @fixme502

	LPCHARACTER		pCubeNpc;
	LPCHARACTER		battle_victim;

	BYTE			gm_level;

	BYTE			bBasePart;

	int				iMaxStamina;

	BYTE			bBlockMode;

	int				iDragonSoulActiveDeck;
	LPENTITY		m_pDragonSoulRefineWindowOpener;

#ifdef ENABLE_ANTI_EXP
	bool			anti_exp;
#endif
} CHARACTER_POINT_INSTANT;

// @fixme199 BEGIN
struct PlayerSlotT {
	LPITEM			pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
#ifndef ENABLE_SPECIAL_INVENTORY
	BYTE			bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];
#else
	WORD			bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];
#endif
	LPITEM			pDSItems[DRAGON_SOUL_INVENTORY_MAX_NUM];
	WORD			wDSItemGrid[DRAGON_SOUL_INVENTORY_MAX_NUM];
	LPITEM			pCubeItems[CUBE_MAX_NUM];
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	TItemPosEx		pAcceMaterials[ACCE_WINDOW_MAX_MATERIALS];
#endif
	TQuickslot		pQuickslot[QUICKSLOT_MAX_NUM];
#ifdef ENABLE_SWITCHBOT_SYSTEM
	LPITEM			pSwitchbotItems[SWITCHBOT_SLOT_COUNT];
#endif
#ifdef FAST_EQUIP_WORLDARD
	LPITEM 			pChangeEquipItem[CHANGE_EQUIP_SLOT_COUNT];
#endif
};
// @fixme199 END

#define TRIGGERPARAM		LPCHARACTER ch, LPCHARACTER causer

typedef struct trigger
{
	BYTE	type;
	int		(*func) (TRIGGERPARAM);
	long	value;
} TRIGGER;

class CTrigger
{
	public:
		CTrigger() : bType(0), pFunc(NULL)
		{
		}

		BYTE	bType;
		int	(*pFunc) (TRIGGERPARAM);
};

EVENTINFO(char_event_info)
{
	DynamicCharacterPtr ch;
};

typedef std::map<VID, size_t> target_map;
struct TSkillUseInfo
{
	int	    iHitCount;
	int	    iMaxHitCount;
	int	    iSplashCount;
	DWORD   dwNextSkillUsableTime;
	int	    iRange;
	bool    bUsed;
	bool	bSkillCD;
	DWORD	dwHitCount;
	DWORD   dwVID;
	bool    isGrandMaster;
	#ifdef ENABLE_SKILL_COOLDOWN_CHECK
	bool	cooldown{false};
	int		skillCount{0};
	#endif

	target_map TargetVIDMap;

	TSkillUseInfo()
		: iHitCount(0), iMaxHitCount(0), iSplashCount(0), dwNextSkillUsableTime(0), iRange(0), bUsed(false),bSkillCD(false), dwHitCount(0),
		dwVID(0), isGrandMaster(false)
   	{}

	bool    HitOnce(DWORD dwVnum = 0);
	bool	IsSkillCooldown(DWORD dwVnum, float fSkillPower);

	bool    UseSkill(bool isGrandMaster, DWORD vid, DWORD dwCooltime, int splashcount = 1, int hitcount = -1, int range = -1);
	DWORD   GetMainTargetVID() const	{ return dwVID; }
	void    SetMainTargetVID(DWORD vid) { dwVID=vid; }
	void    ResetHitCount() { if (iSplashCount) { iHitCount = iMaxHitCount; iSplashCount--; } }
	#ifdef ENABLE_SKILL_COOLDOWN_CHECK
	bool	IsOnCooldown(int vnum);
	#endif
};

typedef struct packet_party_update TPacketGCPartyUpdate;
class CExchange;
class CSkillProto;
class CParty;
class CDungeon;
class CWarMap;
class CAffect;
class CGuild;
class CSafebox;
class CArena;

class CShop;
typedef class CShop * LPSHOP;

class CMob;
class CMobInstance;
typedef struct SMobSkillInfo TMobSkillInfo;

//SKILL_POWER_BY_LEVEL
extern int GetSkillPowerByLevelFromType(int job, int skillgroup, int skilllevel);
//END_SKILL_POWER_BY_LEVEL

namespace marriage
{
	class WeddingMap;
}
enum e_overtime
{
	OT_NONE,
	OT_3HOUR,
	OT_5HOUR,
};

#define NEW_ICEDAMAGE_SYSTEM
class CHARACTER : public CEntity, public CFSM, public CHorseRider
{
	protected:
		//////////////////////////////////////////////////////////////////////////////////
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);
		//////////////////////////////////////////////////////////////////////////////////

	public:
		LPCHARACTER			FindCharacterInView(const char * name, bool bFindPCOnly);
		void				UpdatePacket();

		//////////////////////////////////////////////////////////////////////////////////
	protected:
		CStateTemplate<CHARACTER>	m_stateMove;
		CStateTemplate<CHARACTER>	m_stateBattle;
		CStateTemplate<CHARACTER>	m_stateIdle;

	public:
		virtual void		StateMove();
		virtual void		StateBattle();
		virtual void		StateIdle();
		virtual void		StateFlag();
		virtual void		StateFlagBase();
		void				StateHorse();

	protected:
		// STATE_IDLE_REFACTORING
		void				__StateIdle_Monster();
		void				__StateIdle_Stone();
		void				__StateIdle_NPC();
		// END_OF_STATE_IDLE_REFACTORING

	public:
		DWORD GetAIFlag() const	{ return m_pointsInstant.dwAIFlag; }

		void				SetAggressive();
		bool				IsAggressive() const;

		void				SetCoward();
		bool				IsCoward() const;
		void				CowardEscape();

		void				SetNoAttackShinsu();
		bool				IsNoAttackShinsu() const;

		void				SetNoAttackChunjo();
		bool				IsNoAttackChunjo() const;

		void				SetNoAttackJinno();
		bool				IsNoAttackJinno() const;

		void				SetAttackMob();
		bool				IsAttackMob() const;

		virtual void			BeginStateEmpty();
		virtual void			EndStateEmpty() {}

		void				RestartAtSamePos();

	protected:
		DWORD				m_dwStateDuration;
		//////////////////////////////////////////////////////////////////////////////////

	public:
		CHARACTER();
		virtual ~CHARACTER();

		void			Create(const char * c_pszName, DWORD vid, bool isPC);
		void			Destroy();

		void			Disconnect(const char * c_pszReason);

	protected:
		void			Initialize();

		//////////////////////////////////////////////////////////////////////////////////
		// Basic Points
	public:
		DWORD			GetPlayerID() const	{ return m_dwPlayerID; }

		void			SetPlayerProto(const TPlayerTable * table);
		void			CreatePlayerProto(TPlayerTable & tab);

		void			SetProto(const CMob * c_pkMob);
		DWORD			GetRaceNum() const; // @fixme501
		WORD			GetPlayerRace() const;

		void			Save();		// DelayedSave
		void			SaveReal();
		void			FlushDelayedSaveItem();

		const char *	GetName() const;
		const VID &		GetVID() const		{ return m_vid;		}

		void			SetName(const std::string& name) { m_stName = name; }

		void			SetRace(BYTE race);
		bool			ChangeSex();

		DWORD			GetAID() const;
		int				GetChangeEmpireCount() const;
		void			SetChangeEmpireCount();
		int				ChangeEmpire(BYTE empire);

		BYTE			GetJob() const;
		BYTE			GetCharType() const;

		bool			IsPC() const		{ return GetDesc() ? true : false; }
		bool			IsNPC()	const		{ return m_bCharType != CHAR_TYPE_PC; }
		bool			IsMonster()	const	{ return m_bCharType == CHAR_TYPE_MONSTER; }
		bool			IsStone() const		{ return m_bCharType == CHAR_TYPE_STONE; }
		bool			IsDoor() const		{ return m_bCharType == CHAR_TYPE_DOOR; }
		bool			IsBuilding() const	{ return m_bCharType == CHAR_TYPE_BUILDING;  }
		bool			IsWarp() const		{ return m_bCharType == CHAR_TYPE_WARP; }
		bool			IsGoto() const		{ return m_bCharType == CHAR_TYPE_GOTO; }
		bool 			IsBoss();
		// bool			IsPet() const		{ return m_bCharType == CHAR_TYPE_PET; }
		bool IsMount() const { return m_bCharType == CHAR_TYPE_MOUNT; }

		DWORD			GetLastShoutPulse() const	{ return m_pointsInstant.dwLastShoutPulse; }
		void			SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
		int				GetLevel() const		{ return m_points.level;	}
		void			SetLevel(int level);

		BYTE			GetGMLevel() const;
		BOOL 			IsGM() const;
		void			SetGMLevel();

		DWORD			GetExp() const		{ return m_points.exp;	}
		void			SetExp(DWORD exp)	{ m_points.exp = exp;	}
		DWORD			GetNextExp() const;
		LPCHARACTER		DistributeExp();
		void			DistributeHP(LPCHARACTER pkKiller);
		void			DistributeSP(LPCHARACTER pkKiller, int iMethod=0);

#ifdef ENABLE_KILL_EVENT_FIX
		LPCHARACTER		GetMostAttacked();
#endif

		void			SetPosition(int pos);
		bool			IsPosition(int pos) const	{ return m_pointsInstant.position == pos ? true : false; }
		int				GetPosition() const		{ return m_pointsInstant.position; }

		void			SetPart(BYTE bPartPos, DWORD wVal); // @fixme502
		DWORD			GetPart(BYTE bPartPos) const; // @fixme502
		DWORD			GetOriginalPart(BYTE bPartPos) const; // @fixme502

		void			SetHP(int hp)		{ m_points.hp = hp; }
		int				GetHP() const		{ return m_points.hp; }

		void			SetSP(int sp)		{ m_points.sp = sp; }
		int				GetSP() const		{ return m_points.sp; }

		void			SetStamina(int stamina)	{ m_points.stamina = stamina; }
		int				GetStamina() const		{ return m_points.stamina; }

		void			SetMaxHP(int iVal)	{ m_pointsInstant.iMaxHP = iVal; }
		int				GetMaxHP() const	{ return m_pointsInstant.iMaxHP; }

		void			SetMaxSP(int iVal)	{ m_pointsInstant.iMaxSP = iVal; }
		int				GetMaxSP() const	{ return m_pointsInstant.iMaxSP; }

		void			SetMaxStamina(int iVal)	{ m_pointsInstant.iMaxStamina = iVal; }
		int				GetMaxStamina() const	{ return m_pointsInstant.iMaxStamina; }

		void			SetRandomHP(int v)	{ m_points.iRandomHP = v; }
		void			SetRandomSP(int v)	{ m_points.iRandomSP = v; }

		int				GetRandomHP() const	{ return m_points.iRandomHP; }
		int				GetRandomSP() const	{ return m_points.iRandomSP; }

		int				GetHPPct() const;

		void			SetRealPoint(BYTE idx, int val);
		int				GetRealPoint(BYTE idx) const;

		void			SetPoint(BYTE idx, int val);
		int				GetPoint(BYTE idx) const;
		int				GetLimitPoint(BYTE idx) const;
		int				GetPolymorphPoint(BYTE idx) const;

		const TMobTable &	GetMobTable() const;
		BYTE				GetMobRank() const;
		BYTE				GetMobBattleType() const;
		BYTE				GetMobSize() const;
		DWORD				GetMobDamageMin() const;
		DWORD				GetMobDamageMax() const;
		WORD				GetMobAttackRange() const;
		DWORD				GetMobDropItemVnum() const;
		float				GetMobDamageMultiply() const;

		// NEWAI
		bool			IsBerserker() const;
		bool			IsBerserk() const;
		void			SetBerserk(bool mode);

		bool			IsStoneSkinner() const;

		bool			IsGodSpeeder() const;
		bool			IsGodSpeed() const;
		void			SetGodSpeed(bool mode);

		bool			IsDeathBlower() const;
		bool			IsDeathBlow() const;

		bool			IsReviver() const;
		bool			HasReviverInParty() const;
		bool			IsRevive() const;
		void			SetRevive(bool mode);
		// NEWAI END

		bool			IsRaceFlag(DWORD dwBit) const;
		bool			IsSummonMonster() const;
		DWORD			GetSummonVnum() const;

		DWORD			GetPolymorphItemVnum() const;
		DWORD			GetMonsterDrainSPPoint() const;

		void			MainCharacterPacket();

		void			ComputePoints();
		void			ComputeBattlePoints();
		void CheckMapLevelLimit();
		void			PointChange(BYTE type, int amount, bool bAmount = false, bool bBroadcast = false);
		void			PointsPacket();
		void			ApplyPoint(BYTE bApplyType, int iVal);
		void			CheckMaximumPoints();

		bool			Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false);

		void			Sitdown(int is_ground);
		void			Standup();

		void			SetRotation(float fRot);
		void			SetRotationToXY(long x, long y);
		float			GetRotation() const	{ return m_pointsInstant.fRot; }

		void			MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet);
		void			Motion(BYTE motion, LPCHARACTER victim = NULL);

		void			ChatPacket(BYTE type, const char *format, ...);
		void			MonsterChat(BYTE bMonsterChatType);

		void			ResetPoint(int iLv);

		void			SetBlockMode(BYTE bFlag);
		void			SetBlockModeForce(BYTE bFlag);
		void			LoadBlockMode();
		bool			IsBlockMode(BYTE bFlag) const	{ return (m_pointsInstant.bBlockMode & bFlag)?true:false; }

		bool			IsPolymorphed() const		{ return m_dwPolymorphRace>0; }
		bool			IsPolyMaintainStat() const	{ return m_bPolyMaintainStat; }
		void			SetPolymorph(DWORD dwRaceNum, bool bMaintainStat = false);
		DWORD			GetPolymorphVnum() const	{ return m_dwPolymorphRace; }
		int				GetPolymorphPower() const;

		// FISING
		void			fishing();
		void			fishing_take();
		// END_OF_FISHING

		// MINING
		void			mining(LPCHARACTER chLoad);
		void			mining_cancel();
		void			mining_take();
		// END_OF_MINING

		void			ResetPlayTime(DWORD dwTimeRemain = 0);

		void			CreateFly(BYTE bType, LPCHARACTER pkVictim);

		void			ResetChatCounter();
		BYTE			IncreaseChatCounter();
		BYTE			GetChatCounter() const;

	protected:
		DWORD			m_dwPolymorphRace;
		bool			m_bPolyMaintainStat;
		DWORD			m_dwLoginPlayTime;
		DWORD			m_dwPlayerID;
		VID				m_vid;
		std::string		m_stName;
		BYTE			m_bCharType;

		CHARACTER_POINT		m_points;
		CHARACTER_POINT_INSTANT	m_pointsInstant;
		std::unique_ptr<PlayerSlotT> m_PlayerSlots; // @fixme199

		int				m_iMoveCount;
		DWORD			m_dwPlayStartTime;
		BYTE			m_bAddChrState;
		bool			m_bSkipSave;
		BYTE			m_bChatCounter;

		// End of Basic Points

		//////////////////////////////////////////////////////////////////////////////////
		// Move & Synchronize Positions
		//////////////////////////////////////////////////////////////////////////////////
	public:
#ifdef __RANKING_SYSTEM__
		DWORD			GetPlayStartTime() { return m_dwPlayStartTime; }
#endif
		bool			IsStateMove() const			{ return IsState((CState&)m_stateMove); }
		bool			IsStateIdle() const			{ return IsState((CState&)m_stateIdle); }
		bool			IsWalking() const			{ return m_bNowWalking || GetStamina()<=0; }
		void			SetWalking(bool bWalkFlag)	{ m_bWalking=bWalkFlag; }
		void			SetNowWalking(bool bWalkFlag);
		void			ResetWalking()			{ SetNowWalking(m_bWalking); }

		bool			Goto(long x, long y);
		void			Stop();
		// START OF DevFix 129
        bool			IsPCAbleToAttack() const;
		// END OF DevFix 129
		bool			CanMove() const;

		void			SyncPacket();
		bool			Sync(long x, long y);
		bool			Move(long x, long y);
		void			OnMove(bool bIsAttack = false);
		DWORD			GetMotionMode() const;
		float			GetMoveMotionSpeed() const;
		float			GetMoveSpeed() const;
		void			CalculateMoveDuration();
		void			SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime=0, int iRot=-1);
		DWORD			GetCurrentMoveDuration() const	{ return m_dwMoveDuration; }
		DWORD			GetWalkStartTime() const	{ return m_dwWalkStartTime; }
		DWORD			GetLastMoveTime() const		{ return m_dwLastMoveTime; }
		DWORD			GetLastAttackTime() const	{ return m_dwLastAttackTime; }

		void			SetLastAttacked(DWORD time);

		bool			SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList = true);
		bool			IsSyncOwner(LPCHARACTER ch) const;

#ifdef ENABLE_TELEPORT_GUILD
		bool			WarpSetNew(long x, long y, long lMapIndex, WORD wPort);
#endif

		bool			WarpSet(long x, long y, long lRealMapIndex = 0, long lCustomAddr = 0, WORD wCustomPort = 0);
		void			SetWarpLocation(long lMapIndex, long x, long y);
		void			WarpEnd();
		const PIXEL_POSITION & GetWarpPosition() const { return m_posWarp; }
		bool			WarpToPID(DWORD dwPID);

		void			SaveExitLocation();
		void			ExitToSavedLocation();

		void			StartStaminaConsume();
		void			StopStaminaConsume();
		bool			IsStaminaConsume() const;
		bool			IsStaminaHalfConsume() const;

		void			ResetStopTime();
		DWORD			GetStopTime() const;

	protected:
		void			ClearSync();

		DWORD 			m_fSyncTime;
		LPCHARACTER		m_pkChrSyncOwner;
		CHARACTER_LIST	m_kLst_pkChrSyncOwned;

		PIXEL_POSITION	m_posDest;
		PIXEL_POSITION	m_posStart;
		PIXEL_POSITION	m_posWarp;
		long			m_lWarpMapIndex;

		PIXEL_POSITION	m_posExit;
		long			m_lExitMapIndex;

		DWORD			m_dwMoveStartTime;
		DWORD			m_dwMoveDuration;

		DWORD			m_dwLastMoveTime;
		DWORD			m_dwLastAttackTime;
		DWORD			m_dwWalkStartTime;
		DWORD			m_dwStopTime;

		bool			m_bWalking;
		bool			m_bNowWalking;
		bool			m_bStaminaConsume;
		// End

	public:
		void			SyncQuickslot(BYTE bType, BYTE bOldPos, BYTE bNewPos);
		bool			GetQuickslot(BYTE pos, TQuickslot ** ppSlot);
		bool			SetQuickslot(BYTE pos, TQuickslot & rSlot);
		bool			DelQuickslot(BYTE pos);
		bool			SwapQuickslot(BYTE a, BYTE b);
		void			ChainQuickslotItem(LPITEM pItem, BYTE bType, BYTE bOldPos);

		////////////////////////////////////////////////////////////////////////////////////////
		// Affect
	public:
		void			StartAffectEvent();
		void			ClearAffect(bool bSave=false
#ifdef ENABLE_NOT_BUFF_CLEAR
			, bool bSkipAffect = false
#endif
		);
		void			ComputeAffect(CAffect * pkAff, bool bAdd);
		bool			AddAffect(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube = false
#ifdef ENABLE_AFFECT_RENEWAL
				   ,
				   DWORD dwExpireTIme = 0
#endif
		);
		void			RefreshAffect();
		bool			RemoveAffect(DWORD dwType);
		bool			IsAffectFlag(DWORD dwAff) const;

		bool			UpdateAffect();	// called from EVENT
		int				ProcessAffect();

		void			LoadAffect(DWORD dwCount, TPacketAffectElement * pElements);
		void			SaveAffect();

		bool			IsLoadedAffect() const	{ return m_bIsLoadedAffect; }

		bool			IsGoodAffect(BYTE bAffectType) const;

		void			RemoveGoodAffect();
		void			RemoveBadAffect();

		CAffect *		FindAffect(DWORD dwType, BYTE bApply=APPLY_NONE) const;
#ifdef ENABLE_PLAYER_AFFECT_REMOVE
		CAffect *		FindAffectByFlag(DWORD dwFlag) const;
#endif
		const std::list<CAffect *> & GetAffectContainer() const	{ return m_list_pkAffect; }
		bool			RemoveAffect(CAffect * pkAff);

	protected:
		bool			m_bIsLoadedAffect;
		TAffectFlag		m_afAffectFlag;
		std::list<CAffect *>	m_list_pkAffect;

	public:
		// PARTY_JOIN_BUG_FIX
		void			SetParty(LPPARTY pkParty);
		LPPARTY			GetParty() const	{ return m_pkParty; }

		bool			RequestToParty(LPCHARACTER leader);
		void			DenyToParty(LPCHARACTER member);
		void			AcceptToParty(LPCHARACTER member);

		void			PartyInvite(LPCHARACTER pchInvitee);

		void			PartyInviteAccept(LPCHARACTER pchInvitee);

		void			PartyInviteDeny(DWORD dwPID);

		bool			BuildUpdatePartyPacket(TPacketGCPartyUpdate & out);
		int				GetLeadershipSkillLevel() const;

		bool			CanSummon(int iLeaderShip);

		void			SetPartyRequestEvent(LPEVENT pkEvent) { m_pkPartyRequestEvent = pkEvent; }

	protected:

		void			PartyJoin(LPCHARACTER pkLeader);

		enum PartyJoinErrCode {
			PERR_NONE		= 0,
			PERR_SERVER,
			PERR_DUNGEON,
			PERR_OBSERVER,
			PERR_LVBOUNDARY,
			PERR_LOWLEVEL,
			PERR_HILEVEL,
			PERR_ALREADYJOIN,
			PERR_PARTYISFULL,
			PERR_SEPARATOR,			///< Error type separator.
			PERR_DIFFEMPIRE,
			PERR_MAX
		};

		static PartyJoinErrCode	IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		static PartyJoinErrCode	IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		LPPARTY			m_pkParty;
		DWORD			m_dwLastDeadTime;
		LPEVENT			m_pkPartyRequestEvent;

		typedef std::map< DWORD, LPEVENT >	EventMap;
		EventMap		m_PartyInviteEventMap;

		// END_OF_PARTY_JOIN_BUG_FIX

		////////////////////////////////////////////////////////////////////////////////////////
		// Dungeon
	public:
		void			SetDungeon(LPDUNGEON pkDungeon);
		LPDUNGEON		GetDungeon() const	{ return m_pkDungeon; }
		LPDUNGEON		GetDungeonForce() const;
	protected:
		LPDUNGEON	m_pkDungeon;
		int			m_iEventAttr;

		////////////////////////////////////////////////////////////////////////////////////////
		// Guild
	public:
		void			SetGuild(CGuild * pGuild);
		CGuild*			GetGuild() const	{ return m_pGuild; }

		void			SetWarMap(CWarMap* pWarMap);
		CWarMap*		GetWarMap() const	{ return m_pWarMap; }

#ifdef __LEADER_ON_GUILD__
		void			CheckGuildAffect();
#endif

	protected:
		CGuild *		m_pGuild;
		DWORD			m_dwUnderGuildWarInfoMessageTime;
		CWarMap *		m_pWarMap;

		////////////////////////////////////////////////////////////////////////////////////////
		// Item related
	public:
		bool			CanHandleItem(bool bSkipRefineCheck = false, bool bSkipObserver = false);

		bool			IsItemLoaded() const	{ return m_bItemLoaded; }
		void			SetItemLoaded()	{ m_bItemLoaded = true; }

		void			ClearItem();
#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM
		void			SetItem(TItemPos Cell, LPITEM item, bool bWereMine = false);
#else
		void			SetItem(TItemPos Cell, LPITEM item);
#endif
		LPITEM			GetItem(TItemPos Cell) const;
		LPITEM			GetInventoryItem(WORD wCell) const;
		bool			IsEmptyItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;
#ifdef ENABLE_SPECIAL_INVENTORY
		bool			IsEmptySpecialItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;
#endif

		void			SetWear(BYTE bCell, LPITEM item);
		LPITEM			GetWear(BYTE bCell) const;

		// MYSHOP_PRICE_LIST
		void			UseSilkBotary(void);

		void			UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p);
		// END_OF_MYSHOP_PRICE_LIST

		bool			UseItemEx(LPITEM item, TItemPos DestCell);
		bool			UseItem(TItemPos Cell, TItemPos DestCell = NPOS);

		// ADD_REFINE_BUILDING
		bool			IsRefineThroughGuild() const;
		CGuild *		GetRefineGuild() const;
		int				ComputeRefineFee(int iCost, int iMultiply = 5) const;
		void			PayRefineFee(int iTotalMoney);
		void			SetRefineNPC(LPCHARACTER ch);
		// END_OF_ADD_REFINE_BUILDING

		bool			RefineItem(LPITEM pkItem, LPITEM pkTarget);
#ifdef ENABLE_EXTENDED_ITEM_COUNT
		bool			DropItem(TItemPos Cell, short bCount = 0);
#else
		bool			DropItem(TItemPos Cell, BYTE bCount = 0);
#endif
#ifdef ENABLE_ITEM_MANAGER
		bool			DestroyItem(TItemPos Cell);
		bool			SellItem(TItemPos Cell);
#endif
		bool			GiveRecallItem(LPITEM item);
		void			ProcessRecallItem(LPITEM item);

		//	void			PotionPacket(int iPotionType);
		void			EffectPacket(int enumEffectType);
		void			SpecificEffectPacket(const char filename[128]);

		// ADD_MONSTER_REFINE
		bool			DoRefine(LPITEM item, bool bMoneyOnly = false);
		// END_OF_ADD_MONSTER_REFINE

		bool			DoRefineWithScroll(LPITEM item);
		bool			RefineInformation(WORD wCell, BYTE bType, int iAdditionalCell = -1);

		void			SetRefineMode(int iAdditionalCell = -1);
		void			ClearRefineMode();

		bool			GiveItem(LPCHARACTER victim, TItemPos Cell);
		bool			CanReceiveItem(LPCHARACTER from, LPITEM item) const;
		void			ReceiveItem(LPCHARACTER from, LPITEM item);
		bool			GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector <DWORD> &dwItemVnums,
							std::vector <DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count);
#ifdef ENABLE_CHEST_OPEN_RENEWAL
		bool			OpenChestItem(TItemPos pos, WORD bOpenCount = 1);
		bool			OpenChestItem(LPITEM item, WORD bOpenCount = 1);
#endif

#ifdef ENABLE_EXTENDED_ITEM_COUNT
		bool			MoveItem(TItemPos pos, TItemPos change_pos, short num);
#else
		bool			MoveItem(TItemPos pos, TItemPos change_pos, BYTE num);
#endif
		bool			PickupItem(DWORD vid);
		bool			EquipItem(LPITEM item, int iCandidateCell = -1);
		bool			UnequipItem(LPITEM item);

		bool			CanEquipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		bool			CanUnequipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		bool			SwapItem(WORD wCell, WORD wDestCell);
#ifdef ENABLE_EXTENDED_ITEM_COUNT
		LPITEM			AutoGiveItem(DWORD dwItemVnum, short bCount = 1, int iRarePct = -1, bool bMsg = true);
		void			AutoGiveItem(LPITEM item, bool longOwnerShip = false);
#else
		LPITEM			AutoGiveItem(DWORD dwItemVnum, BYTE bCount=1, int iRarePct = -1, bool bMsg = true);
		void			AutoGiveItem(LPITEM item, bool longOwnerShip = false);
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
		int				GetEmptyInventory(LPITEM item) const;
		int				GetEmptyInventory(DWORD dwVnum, const TItemTable* pTable) const;
#else
		int				GetEmptyInventory(BYTE size) const;
#endif
		bool			GetEmptyInEachDsRoughCategory() const;
		int				GetEmptyDragonSoulInventory(LPITEM pItem) const;
		int				GetEmptyDragonSoulInventoryType() const;
		void			CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const;

		int				CountEmptyInventory() const;

		int				CountSpecifyItem(DWORD vnum) const;
		void			RemoveSpecifyItem(DWORD vnum, DWORD count = 1);
		LPITEM			FindSpecifyItem(DWORD vnum) const;
		LPITEM			FindItemByID(DWORD id) const;

		int				CountSpecifyTypeItem(BYTE type) const;
		void			RemoveSpecifyTypeItem(BYTE type, DWORD count = 1);

		bool			IsEquipUniqueItem(DWORD dwItemVnum) const;

		// CHECK_UNIQUE_GROUP
		bool			IsEquipUniqueGroup(DWORD dwGroupVnum) const;
		// END_OF_CHECK_UNIQUE_GROUP

		void			SendEquipment(LPCHARACTER ch);
		// End of Item

	protected:

		void			SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice);

		bool			m_bNoOpenedShop;

		bool			m_bItemLoaded;
		int				m_iRefineAdditionalCell;
		bool			m_bUnderRefine;
		DWORD			m_dwRefineNPCVID;

	public:
		////////////////////////////////////////////////////////////////////////////////////////
		// Money related
#ifdef ENABLE_GOLD_MAX_EXTENDED
		unsigned long long GetGold() const
		{
			return m_points.gold;
		}
		void SetGold(unsigned long long gold) { m_points.gold = gold; }
		bool DropGold(INT gold);
		unsigned long long GetAllowedGold() const;
		void GiveGold(INT iAmount);
		void ChangeGold(long long amount);
#else
		INT				GetGold() const		{ return m_points.gold;	}
		void			SetGold(INT gold)	{ m_points.gold = gold;	}
		bool			DropGold(INT gold);
		INT				GetAllowedGold() const;
		void			GiveGold(INT iAmount);
#endif
		// End of Money

		////////////////////////////////////////////////////////////////////////////////////////
		// Shop related
	public:
		void			SetShop(LPSHOP pkShop);
		LPSHOP			GetShop() const { return m_pkShop; }
		void			ShopPacket(BYTE bSubHeader);

		void			SetShopOwner(LPCHARACTER ch) { m_pkChrShopOwner = ch; }
		LPCHARACTER		GetShopOwner() const { return m_pkChrShopOwner;}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
		void			OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, short bItemCount);
#else
		void			OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, BYTE bItemCount);
#endif
		LPSHOP			GetMyShop() const { return m_pkMyShop; }
		void			CloseMyShop();

	protected:

		LPSHOP			m_pkShop;
		LPSHOP			m_pkMyShop;
		std::string		m_stShopSign;
		LPCHARACTER		m_pkChrShopOwner;
		// End of shop

		////////////////////////////////////////////////////////////////////////////////////////
		// Exchange related
	public:
		bool			ExchangeStart(LPCHARACTER victim);
		void			SetExchange(CExchange * pkExchange);
		CExchange *		GetExchange() const	{ return m_pkExchange;	}

	protected:
		CExchange *		m_pkExchange;
		// End of Exchange

		////////////////////////////////////////////////////////////////////////////////////////
		// Battle
	public:
		struct TBattleInfo
		{
			//fixme@grimm
			long long iTotalDamage;
			long long iAggro;

			TBattleInfo(long long iTot, long long iAggr)
				: iTotalDamage(iTot), iAggro(iAggr)
				{}
		};
		typedef std::map<VID, TBattleInfo>	TDamageMap;

		typedef struct SAttackLog
		{
			DWORD	dwVID;
			DWORD	dwTime;
		} AttackLog;

		bool				Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		bool				__Profile__Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		void				DeathPenalty(BYTE bExpLossPercent);
		void				ReviveInvisible(int iDur);

		bool				Attack(LPCHARACTER pkVictim, BYTE bType = 0);
		bool				IsAlive() const		{ return m_pointsInstant.position == POS_DEAD ? false : true; }
		bool				CanFight() const;

		bool				CanBeginFight() const;
		void				BeginFight(LPCHARACTER pkVictim);

		bool				CounterAttack(LPCHARACTER pkChr);

		bool				IsStun() const;
		void				Stun();
		bool				IsDead() const;
		void				Dead(LPCHARACTER pkKiller = NULL, bool bImmediateDead=true); // @fixme188 instant death false to true

		void				Reward(bool bItemDrop);
		void				RewardGold(LPCHARACTER pkAttacker);

		bool				Shoot(BYTE bType);
		void				FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader);

		void				ForgetMyAttacker();
		void				AggregateMonster();
		void				AttractRanger();
		void				PullMonster();

		int					GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount = 1);
		void				UseArrow(LPITEM pkArrow, DWORD dwArrowCount);

		void				AttackedByPoison(LPCHARACTER pkAttacker);
		void				RemovePoison();
#ifdef ENABLE_WOLFMAN_CHARACTER
		void				AttackedByBleeding(LPCHARACTER pkAttacker);
		void				RemoveBleeding();
#endif
		void				AttackedByFire(LPCHARACTER pkAttacker, int amount, int count);
		void				RemoveFire();

		void				UpdateAlignment(int iAmount);
		int					GetAlignment() const;

		int					GetRealAlignment() const;
		void				ShowAlignment(bool bShow);

		void				SetKillerMode(bool bOn);
		bool				IsKillerMode() const;
		void				UpdateKillerMode();

		BYTE				GetPKMode() const;
		void				SetPKMode(BYTE bPKMode);

		void				ItemDropPenalty(LPCHARACTER pkKiller);

		void				UpdateAggrPoint(LPCHARACTER ch, EDamageType type, int dam);

		// HACK

	public:
		void SetComboSequence(BYTE seq);
		BYTE GetComboSequence() const;

		void SetLastComboTime(DWORD time);
		DWORD GetLastComboTime() const;

		int GetValidComboInterval() const;
		void SetValidComboInterval(int interval);

		BYTE GetComboIndex() const;

		void IncreaseComboHackCount(int k = 1);
		void ResetComboHackCount();
		void SkipComboAttackByTime(int interval);
		DWORD GetSkipComboAttackByTime() const;

		int m_RealCastingSpeed = 0;
		int m_RealMoveSpeed = 0;
		int m_RealAttackSpeed = 0;

	protected:
		BYTE m_bComboSequence;
		DWORD m_dwLastComboTime;
		int m_iValidComboInterval;
		BYTE m_bComboIndex;
		int m_iComboHackCount;
		DWORD m_dwSkipComboAttackByTime;

	protected:
		void				UpdateAggrPointEx(LPCHARACTER ch, EDamageType type, int dam, TBattleInfo & info);
		void				ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim);

		DWORD				m_dwFlyTargetID;
		std::vector<DWORD>	m_vec_dwFlyTargets;
		TDamageMap			m_map_kDamage;
		DWORD				m_dwKillerPID;

		int					m_iAlignment;		// Lawful/Chaotic value -200000 ~ 200000
		int					m_iRealAlignment;
		int					m_iKillerModePulse;
		BYTE				m_bPKMode;

		// Aggro
		DWORD				m_dwLastVictimSetTime;
		int					m_iMaxAggro;
		// End of Battle

		// Stone
	public:
		void				SetStone(LPCHARACTER pkChrStone);
		void				ClearStone();
		void				DetermineDropMetinStone();
		DWORD				GetDropMetinStoneVnum() const { return m_dwDropMetinStone; }
		BYTE				GetDropMetinStonePct() const { return m_bDropMetinStonePct; }

	protected:
		LPCHARACTER			m_pkChrStone;
		CHARACTER_SET		m_set_pkChrSpawnedBy;
		DWORD				m_dwDropMetinStone;
		BYTE				m_bDropMetinStonePct;
		// End of Stone

	public:
		enum
		{
			SKILL_UP_BY_POINT,
			SKILL_UP_BY_BOOK,
			SKILL_UP_BY_TRAIN,

			// ADD_GRANDMASTER_SKILL
			SKILL_UP_BY_QUEST,
			// END_OF_ADD_GRANDMASTER_SKILL
		};

		void				SkillLevelPacket();
		void				SkillLevelUp(DWORD dwVnum, BYTE bMethod = SKILL_UP_BY_POINT);
		bool				SkillLevelDown(DWORD dwVnum);
		// ADD_GRANDMASTER_SKILL
		bool				UseSkill(DWORD dwVnum, LPCHARACTER pkVictim, bool bUseGrandMaster = true);
		void				ResetSkill();
        bool				IsSkillCooldown(DWORD dwVnum, float fSkillPower) { return m_SkillUseInfo[dwVnum].IsSkillCooldown(dwVnum, fSkillPower) ? true : false; }
#ifdef ENABLE_SKILL_COOLTIME_UPDATE
		void				ResetSkillCoolTimes();
#endif
		void				SetSkillLevel(DWORD dwVnum, BYTE bLev);
		int					GetUsedSkillMasterType(DWORD dwVnum);

		bool				IsLearnableSkill(DWORD dwSkillVnum) const;
		// END_OF_ADD_GRANDMASTER_SKILL

		bool				CheckSkillHitCount(const BYTE SkillID, const VID dwTargetVID);
		bool				CanUseSkill(DWORD dwSkillVnum) const;
		bool				IsUsableSkillMotion(DWORD dwMotionIndex) const;
		int					GetSkillLevel(DWORD dwVnum) const;
#ifdef __AUTO_SKILL_READER__
		BYTE 				GetSkillGradeByLevel(BYTE bSkillLevel) const;
#endif
		int					GetSkillMasterType(DWORD dwVnum) const;
		int					GetSkillPower(DWORD dwVnum, BYTE bLevel = 0) const;

		time_t				GetSkillNextReadTime(DWORD dwVnum) const;
		void				SetSkillNextReadTime(DWORD dwVnum, time_t time);
		void				SkillLearnWaitMoreTimeMessage(DWORD dwVnum);

		void				ComputePassiveSkill(DWORD dwVnum);
		int					ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#ifdef ENABLE_SKILL_FLAG_PARTY
		int					ComputeSkillParty(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif

#ifdef __SKILL_COSTUME__
		void 				ChangeBuffParty(DWORD dwVnum, LPCHARACTER pkAttacker);
#endif

		bool				IsBuffSkill(DWORD dwSkillVnum) const;
		int					ComputeSkillAtPosition(DWORD dwVnum, const PIXEL_POSITION& posTarget, BYTE bSkillLevel = 0);
		void				ComputeSkillPoints();

		void				SetSkillGroup(BYTE bSkillGroup);
		BYTE				GetSkillGroup() const		{ return m_points.skill_group; }

		int					ComputeCooltime(int time);

		void				GiveRandomSkillBook();

		void				DisableCooltime();
		bool				LearnSkillByBook(DWORD dwSkillVnum, BYTE bProb = 0);
		bool				LearnGrandMasterSkill(DWORD dwSkillVnum);

		#ifdef ENABLE_SKILL_COOLDOWN_CHECK
		bool				SkillIsOnCooldown(int vnum) { return m_SkillUseInfo[vnum].IsOnCooldown(vnum); }
		#endif
	private:
		bool				m_bDisableCooltime;
		DWORD				m_dwLastSkillTime;
		// End of Skill

		// MOB_SKILL
	public:
		bool				HasMobSkill() const;
		size_t				CountMobSkill() const;
		const TMobSkillInfo * GetMobSkill(unsigned int idx) const;
		bool				CanUseMobSkill(unsigned int idx) const;
		bool				UseMobSkill(unsigned int idx);
		void				ResetMobSkillCooltime();
	protected:
		DWORD				m_adwMobSkillCooltime[MOB_SKILL_MAX_NUM];
		// END_OF_MOB_SKILL

		// for SKILL_MUYEONG
	public:
		void				StartMuyeongEvent();
		void				StopMuyeongEvent();

	private:
		LPEVENT				m_pkMuyeongEvent;

		// for SKILL_CHAIN lighting
	public:
		int					GetChainLightningIndex() const { return m_iChainLightingIndex; }
		void				IncChainLightningIndex() { ++m_iChainLightingIndex; }
		void				AddChainLightningExcept(LPCHARACTER ch) { m_setExceptChainLighting.emplace(ch); }
		void				ResetChainLightningIndex() { m_iChainLightingIndex = 0; m_setExceptChainLighting.clear(); }
		int					GetChainLightningMaxCount() const;
		const CHARACTER_SET& GetChainLightingExcept() const { return m_setExceptChainLighting; }

	private:
		int					m_iChainLightingIndex;
		CHARACTER_SET m_setExceptChainLighting;

		// for SKILL_EUNHYUNG
	public:
		void				SetAffectedEunhyung();
		void				ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
		bool				GetAffectedEunhyung() const { return m_dwAffectedEunhyungLevel; }

	private:
		DWORD				m_dwAffectedEunhyungLevel;

		// Skill levels

	protected:
		TPlayerSkill*					m_pSkillLevels;
		boost::unordered_map<BYTE, int>		m_SkillDamageBonus;
		std::map<int, TSkillUseInfo>	m_SkillUseInfo;

		////////////////////////////////////////////////////////////////////////////////////////
		// AI related
	public:
		void			AssignTriggers(const TMobTable * table);
		LPCHARACTER		GetVictim() const;
		void			SetVictim(LPCHARACTER pkVictim);
		LPCHARACTER		GetNearestVictim(LPCHARACTER pkChr);
		LPCHARACTER		GetProtege() const;

		bool			Follow(LPCHARACTER pkChr, float fMinimumDistance = 150.0f);
		bool			Return();
		bool			IsGuardNPC() const;
		bool			IsChangeAttackPosition(LPCHARACTER target) const;
		void			ResetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time() - AI_CHANGE_ATTACK_POISITION_TIME_NEAR;}
		void			SetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time();}

		bool			OnIdle();

		void			OnAttack(LPCHARACTER pkChrAttacker);
		void			OnClick(LPCHARACTER pkChrCauser);

		VID				m_kVIDVictim;

	protected:
		DWORD			m_dwLastChangeAttackPositionTime;
		CTrigger		m_triggerOnClick;
		// End of AI

		////////////////////////////////////////////////////////////////////////////////////////
		// Target
	protected:
		LPCHARACTER				m_pkChrTarget;
		CHARACTER_SET	m_set_pkChrTargetedBy;

	public:
		void				SetTarget(LPCHARACTER pkChrTarget);
		void				BroadcastTargetPacket();
		void				ClearTarget();
		void				CheckTarget();
		LPCHARACTER			GetTarget() const { return m_pkChrTarget; }

		////////////////////////////////////////////////////////////////////////////////////////
		// Safebox
	public:
		int					GetSafeboxSize() const;
		void				QuerySafeboxSize();
		void				SetSafeboxSize(int size);

		CSafebox *			GetSafebox() const;
		void				LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems);
		void				ChangeSafeboxSize(BYTE bSize);
		void				CloseSafebox();

		void				ReqSafeboxLoad(const char* pszPassword);

		void				CancelSafeboxLoad( void ) { m_bOpeningSafebox = false; }

		void				SetMallLoadTime(int t) { m_iMallLoadTime = t; }
		int					GetMallLoadTime() const { return m_iMallLoadTime; }

		CSafebox *			GetMall() const;
		void				LoadMall(int iItemCount, TPlayerItem * pItems);
		void				CloseMall();

		void				SetSafeboxOpenPosition();
		float				GetDistanceFromSafeboxOpen() const;

	protected:
		CSafebox *			m_pkSafebox;
		int					m_iSafeboxSize;
		int					m_iSafeboxLoadTime;
		bool				m_bOpeningSafebox;

		CSafebox *			m_pkMall;
		int					m_iMallLoadTime;

		PIXEL_POSITION		m_posSafeboxOpen;

		////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////
		// Mounting
	public:
		void				EnterMount();
		void				MountVnum(DWORD vnum);
		DWORD				GetMountVnum() const { return m_dwMountVnum; }
		DWORD				GetLastMountTime() const { return m_dwMountTime; }

		bool				CanUseHorseSkill();

		// Horse
		virtual	void		SetHorseLevel(int iLevel);

		virtual	bool		StartRiding();
		virtual	bool		StopRiding();

		virtual	DWORD		GetMyHorseVnum() const;

		virtual	void		HorseDie();
		virtual bool		ReviveHorse();

		virtual void		SendHorseInfo();
		virtual	void		ClearHorseInfo();

		void				HorseSummon(bool bSummon, bool bFromFar = false, DWORD dwVnum = 0
#ifdef ENABLE_MOUNT_SYSTEM
		, const std::string & name = "", LPITEM mountItem = NULL
#else
		, const char *name = 0
#endif
		);

		LPCHARACTER			GetHorse() const			{ return m_chHorse; }
		LPCHARACTER			GetRider() const; // rider on horse
		void				SetRider(LPCHARACTER ch);

		bool				IsRiding() const;

#ifdef __PET_SYSTEM__
	public:
		CPetSystem*			GetPetSystem()				{ return m_petSystem; }

	protected:
		CPetSystem*			m_petSystem;

	public:
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	private:
		LPPRIVATE_SHOP		m_pPrivateShop;
		LPPRIVATE_SHOP		m_pMyPrivateShop;
		DWORD				m_dwPrivateShopOwner;
		TPrivateShop		m_privateShopTable;
		bool				m_bIsEditingPrivateShop;
		BYTE				m_bShopSearchMode;

		time_t				m_tLastPrivateShopModify;
		time_t				m_tLastPrivateShopWithdraw;
		time_t				m_tLastPrivateShopClose;
		time_t				m_tLastPrivateShopBuy;
		time_t				m_tLastPrivateShopSearch;
		time_t				m_tLastPrivateShopStateChange;
		time_t				m_tLastPrivateShopBuild;

		std::vector<TPlayerPrivateShopItem>		m_vec_privateShopItem;
		std::vector<TPrivateShopSale>			m_vec_privateShopSale;

	public:
		bool				BuildPrivateShop(const char* c_szTitle, DWORD dwPolyVnum, BYTE bTitleType, BYTE bPageCount, WORD wItemCount, TPrivateShopItem* pShopItemTable);
		void				ClosePrivateShop();

		void				SetViewingPrivateShop(LPPRIVATE_SHOP pShop) { m_pPrivateShop = pShop; }
		LPPRIVATE_SHOP		GetViewingPrivateShop() const { return m_pPrivateShop; }

		void				SetMyPrivateShop(LPPRIVATE_SHOP pShop) { m_pMyPrivateShop = pShop; }
		LPPRIVATE_SHOP		GetMyPrivateShop() const { return m_pMyPrivateShop; }

		void				SetPrivateShopOwner(DWORD dwPID) { m_dwPrivateShopOwner = dwPID; }
		DWORD				GetPrivateShopOwner() { return m_dwPrivateShopOwner; }

		void				SetPrivateShopTable(const TPrivateShop& rPrivateShopTable);
		TPrivateShop* GetPrivateShopTable() { return &m_privateShopTable; }
		bool				IsPrivateShopOwner() { return m_privateShopTable.dwOwner != 0; }
		bool				CanModifyPrivateShop() { return m_privateShopTable.bState == STATE_MODIFY; }

		void				SetEditingPrivateShop(bool bEditingPrivateShop) { m_bIsEditingPrivateShop = bEditingPrivateShop; }
		bool				IsEditingPrivateShop() const { return m_bIsEditingPrivateShop; }
		void				OpenPrivateShopPanel();
		void				ClosePrivateShopPanel(bool bSendClient = false);

		void				OpenShopSearch(BYTE bMode);
		void				CloseShopSearch();
		bool				IsShopSearch() const { return m_bShopSearchMode != MODE_NONE; }
		BYTE				GetShopSearchMode() { return m_bShopSearchMode; }

		long long						GetPrivateShopTotalGold();
		DWORD							GetPrivateShopTotalCheque();

		void							SetPrivateShopItem(const TPlayerPrivateShopItem& c_rPrivateShopItem);
		bool							RemovePrivateShopItem(WORD wPos);
		const TPlayerPrivateShopItem* GetPrivateShopItem(WORD wPos);
		WORD							GetPrivateShopItemCount() { return m_vec_privateShopItem.size(); }

		void							SetPrivateShopSale(const TPrivateShopSale& c_rPrivateShopSale);
		WORD							GetPrivateShopSaleCount() { return m_vec_privateShopSale.size(); }

		void							ChangePrivateShopItemPrice(WORD wPos, long long llGold, DWORD dwCheque);
		void							ChangePrivateShopItemPos(WORD wPos, WORD wChangePos);
		void							ChangePrivateShopTitle(const char* c_szTitle);
		void							SaleUpdate(const TPrivateShopSale* c_pShopSale);
		void							ItemExpireUpdate(WORD wPos);
		void							SetPrivateShopState(BYTE bState, bool bIsMainPlayerPrivateShop);
		void							WithdrawPrivateShop(long long llGold, DWORD dwCheque);
		void							WarpToPrivateShop(long lAddr, WORD wPort);

		bool							SetPremiumPrivateShopBonus(time_t tDuration);

		int								GetLastPrivateShopModifyTime() const { return m_tLastPrivateShopModify; }
		void							SetLastPrivateShopModifyTime() { m_tLastPrivateShopModify = thecore_pulse(); }

		int								GetLastPrivateShopWithdrawTime() const { return m_tLastPrivateShopWithdraw; }
		void							SetLastPrivateShopWithdrawTime() { m_tLastPrivateShopWithdraw = thecore_pulse(); }

		int								GetLastPrivateShopCloseTime() const { return m_tLastPrivateShopClose; }
		void							SetLastPrivateShopCloseTime() { m_tLastPrivateShopClose = thecore_pulse(); }

		int								GetLastPrivateShopBuildTime() const { return m_tLastPrivateShopBuild; }
		void							SetLastPrivateShopBuildTime() { m_tLastPrivateShopBuild = thecore_pulse(); }

		int								GetLastPrivateShopBuyTime() const { return m_tLastPrivateShopBuy; }
		void							SetLastPrivateShopBuyTime() { m_tLastPrivateShopBuy = thecore_pulse(); }

		int								GetLastPrivateShopSearchTime() const { return m_tLastPrivateShopSearch; }
		void							SetLastPrivateShopSearchTime() { m_tLastPrivateShopSearch = thecore_pulse(); }

		int								GetLastPrivateShopStateChangeTime() const { return m_tLastPrivateShopStateChange; }
		void							SetLastPrivateShopStateChangeTime() { m_tLastPrivateShopStateChange = thecore_pulse(); }
#endif

	protected:
		LPCHARACTER			m_chHorse;
		LPCHARACTER			m_chRider;

		DWORD				m_dwMountVnum;
		DWORD				m_dwMountTime;

		BYTE				m_bSendHorseLevel;
		BYTE				m_bSendHorseHealthGrade;
		BYTE				m_bSendHorseStaminaGrade;

		////////////////////////////////////////////////////////////////////////////////////////
		// Detailed Log
	public:
		void				DetailLog() { m_bDetailLog = !m_bDetailLog; }
		void				ToggleMonsterLog();
		void				MonsterLog(const char* format, ...);
	private:
		bool				m_bDetailLog;
		bool				m_bMonsterLog;

		////////////////////////////////////////////////////////////////////////////////////////
		// Empire

	public:
		void 				SetEmpire(BYTE bEmpire);
		BYTE				GetEmpire() const { return m_bEmpire; }

	protected:
		BYTE				m_bEmpire;

		////////////////////////////////////////////////////////////////////////////////////////
		// Regen
	public:
		void				SetRegen(LPREGEN pkRegen);

	protected:
		PIXEL_POSITION			m_posRegen;
		float				m_fRegenAngle;
		LPREGEN				m_pkRegen;
		size_t				regen_id_; // to help dungeon regen identification
		// End of Regen

		////////////////////////////////////////////////////////////////////////////////////////
		// Resists & Proofs
	public:
		bool				CannotMoveByAffect() const;
		bool				IsImmune(DWORD dwImmuneFlag);
		void				SetImmuneFlag(DWORD dw) { m_pointsInstant.dwImmuneFlag = dw; }

	protected:
		void				ApplyMobAttribute(const TMobTable* table);
		// End of Resists & Proofs

		////////////////////////////////////////////////////////////////////////////////////////
		// QUEST

	public:
		void				SetQuestNPCID(DWORD vid);
		DWORD				GetQuestNPCID() const { return m_dwQuestNPCVID; }
		LPCHARACTER			GetQuestNPC() const;

		void				SetQuestItemPtr(LPITEM item);
		void				ClearQuestItemPtr();
		LPITEM				GetQuestItemPtr() const;

#ifdef ENABLE_QUEST_DND_EVENT
		void				SetQuestDNDItemPtr(LPITEM item);
		void				ClearQuestDNDItemPtr();
		LPITEM				GetQuestDNDItemPtr() const;
#endif

		void				SetQuestBy(DWORD dwQuestVnum)	{ m_dwQuestByVnum = dwQuestVnum; }
		DWORD				GetQuestBy() const			{ return m_dwQuestByVnum; }

		int					GetQuestFlag(const std::string& flag) const;
		void				SetQuestFlag(const std::string& flag, int value);

		void				ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID);

	private:
		DWORD				m_dwQuestNPCVID;
		DWORD				m_dwQuestByVnum;
		LPITEM				m_pQuestItem;
#ifdef ENABLE_QUEST_DND_EVENT
		LPITEM				m_pQuestDNDItem{nullptr};
#endif

		// Events
	public:
		bool				StartStateMachine(int iPulse = 1);
		void				StopStateMachine();
		void				UpdateStateMachine(DWORD dwPulse);
		void				SetNextStatePulse(int iPulseNext);

		void				UpdateCharacter(DWORD dwPulse);

	protected:
		DWORD				m_dwNextStatePulse;

		// Marriage
	public:
		LPCHARACTER			GetMarryPartner() const;
		void				SetMarryPartner(LPCHARACTER ch);
		int					GetMarriageBonus(DWORD dwItemVnum, bool bSum = true);

		void				SetWeddingMap(marriage::WeddingMap* pMap);
		marriage::WeddingMap* GetWeddingMap() const { return m_pWeddingMap; }

	private:
		marriage::WeddingMap* m_pWeddingMap;
		LPCHARACTER			m_pkChrMarried;

		// Warp Character
	public:
		void				StartWarpNPCEvent();

	public:
		void				StartSaveEvent();
		void				StartRecoveryEvent();
		void				StartCheckSpeedHackEvent();
		void				StartDestroyWhenIdleEvent();

		LPEVENT				m_pkDeadEvent;
		LPEVENT				m_pkStunEvent;
		LPEVENT				m_pkSaveEvent;
		LPEVENT				m_pkRecoveryEvent;
		LPEVENT				m_pkTimedEvent;
		LPEVENT				m_pkFishingEvent;
		LPEVENT				m_pkAffectEvent;
		LPEVENT				m_pkPoisonEvent;
#ifdef ENABLE_WOLFMAN_CHARACTER
		LPEVENT				m_pkBleedingEvent;
#endif
		LPEVENT				m_pkFireEvent;
		LPEVENT				m_pkWarpNPCEvent;
		//DELAYED_WARP
		//END_DELAYED_WARP

		// MINING
		LPEVENT				m_pkMiningEvent;
		// END_OF_MINING
		LPEVENT				m_pkWarpEvent;
		LPEVENT				m_pkCheckSpeedHackEvent;
		LPEVENT				m_pkDestroyWhenIdleEvent;
		LPEVENT				m_pkPetSystemUpdateEvent;

		bool IsWarping() const { return m_pkWarpEvent ? true : false; }

		bool				m_bHasPoisoned;
#ifdef ENABLE_WOLFMAN_CHARACTER
		bool				m_bHasBled;
#endif

		const CMob *		m_pkMobData;
		CMobInstance *		m_pkMobInst;

		std::map<int, LPEVENT> m_mapMobSkillEvent;

		friend struct FuncSplashDamage;
		friend struct FuncSplashAffect;
		friend class CFuncShoot;

	public:
		int				GetPremiumRemainSeconds(BYTE bType) const;

	private:
		int				m_aiPremiumTimes[PREMIUM_MAX_NUM];

		// CHANGE_ITEM_ATTRIBUTES
		static const char		msc_szLastChangeItemAttrFlag[];
		// END_OF_CHANGE_ITEM_ATTRIBUTES

		// NEW_HAIR_STYLE_ADD
	public :
		bool ItemProcess_Hair(LPITEM item, int iDestCell);
		// END_NEW_HAIR_STYLE_ADD

	public :
		void ClearSkill();
		void ClearSubSkill();

		// RESET_ONE_SKILL
		bool ResetOneSkill(DWORD dwVnum);
		// END_RESET_ONE_SKILL

	private :
		void SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag);

	// ARENA
	private :
		CArena *m_pArena;
		bool m_ArenaObserver;
		int m_nPotionLimit;

	public :
		void 	SetArena(CArena* pArena) { m_pArena = pArena; }
		void	SetArenaObserverMode(bool flag) { m_ArenaObserver = flag; }

		CArena* GetArena() const { return m_pArena; }
		bool	GetArenaObserverMode() const { return m_ArenaObserver; }

		void	SetPotionLimit(int count) { m_nPotionLimit = count; }
		int		GetPotionLimit() const { return m_nPotionLimit; }
	// END_ARENA

		bool	IsOpenGuildStorage() const { return m_isOpenGuildStorage ? true : false; }
		void 	SetOpenGuildStorage(bool b) { m_isOpenGuildStorage = b; }

		int		GetGuildStorageLoadTime() const { return m_iGuildStorageLoadTime; }
		void	SetGuildStorageLoadTime() { m_iGuildStorageLoadTime = thecore_pulse(); }
		//END_PREVENT_TRADE_WINDOW
	private:
		bool	m_isOpenGuildStorage;
		int		m_iGuildStorageLoadTime;

		//PREVENT_TRADE_WINDOW
	public:
#ifdef __GUILD_SAFEBOX__
		bool	IsOpenSafebox() { return m_isOpenSafebox || IsOpenGuildSafebox() ? true : false; }
#else
		bool	IsOpenSafebox() const { return m_isOpenSafebox ? true : false; }
#endif
		void 	SetOpenSafebox(bool b) { m_isOpenSafebox = b; }

		int		GetSafeboxLoadTime() const { return m_iSafeboxLoadTime; }
		void	SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }
		//END_PREVENT_TRADE_WINDOW
	private:
		bool	m_isOpenSafebox;

#ifdef __GUILD_SAFEBOX__
	public:
		bool	IsOpenGuildSafebox();
#endif

	public:
		int		GetSkillPowerByLevel(int level, bool bMob = false) const;
#ifdef ENABLE_QUEST_RECEIVE_ITEM
		void SenderRecvItem(unsigned int pc, LPITEM item);
#endif

		//PREVENT_REFINE_HACK
		int		GetRefineTime() const { return m_iRefineTime; }
		void	SetRefineTime() { m_iRefineTime = thecore_pulse(); }
		int		m_iRefineTime;
		//END_PREVENT_REFINE_HACK

		//RESTRICT_USE_SEED_OR_MOONBOTTLE
		int 	GetUseSeedOrMoonBottleTime() const { return m_iSeedTime; }
		void  	SetUseSeedOrMoonBottleTime() { m_iSeedTime = thecore_pulse(); }
		int 	m_iSeedTime;
		//END_RESTRICT_USE_SEED_OR_MOONBOTTLE

		//PREVENT_PORTAL_AFTER_EXCHANGE
		int		GetExchangeTime() const { return m_iExchangeTime; }
		void	SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }
		int		m_iExchangeTime;
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

		int 	m_iMyShopTime;
		int		GetMyShopTime() const	{ return m_iMyShopTime; }
		void	SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }

		bool	IsHack(bool bSendMsg = true, bool bCheckShopOwner = true, int limittime = g_nPortalLimitTime);

		// MONARCH
		BOOL	IsMonarch() const;
		// END_MONARCH
		void Say(const std::string & s);

		enum MONARCH_COOLTIME
		{
			MC_HEAL = 10,
			MC_WARP	= 60,
			MC_TRANSFER = 60,
			MC_TAX = (60 * 60 * 24 * 7),
			MC_SUMMON = (60 * 60),
		};

		enum MONARCH_INDEX
		{
			MI_HEAL = 0,
			MI_WARP,
			MI_TRANSFER,
			MI_TAX,
			MI_SUMMON,
			MI_MAX
		};

		DWORD m_dwMonarchCooltime[MI_MAX]; //todo move inside PlayerSlotT
		DWORD m_dwMonarchCooltimelimit[MI_MAX]; //todo move inside PlayerSlotT

		void  InitMC();
		DWORD GetMC(enum MONARCH_INDEX e) const;
		void SetMC(enum MONARCH_INDEX e);
		bool IsMCOK(enum MONARCH_INDEX e) const;
		DWORD GetMCL(enum MONARCH_INDEX e) const;
		DWORD GetMCLTime(enum MONARCH_INDEX e) const;

	public:
		bool ItemProcess_Polymorph(LPITEM item);

		LPITEM*	GetCubeItem() { return (m_PlayerSlots) ? m_PlayerSlots->pCubeItems : nullptr; }
		bool IsCubeOpen () const	{ return (m_pointsInstant.pCubeNpc); }
		void SetCubeNpc(LPCHARACTER npc)	{ m_pointsInstant.pCubeNpc = npc; }
		bool CanDoCube() const;

	public:
		bool IsSiegeNPC() const;

	private:

		e_overtime m_eOverTime;

	public:
		bool IsOverTime(e_overtime e) const { return (e == m_eOverTime); }
		void SetOverTime(e_overtime e) { m_eOverTime = e; }

	private:
		int		m_deposit_pulse;

	public:
		void	UpdateDepositPulse();
		bool	CanDeposit() const;

	private:
		void	__OpenPrivateShop();

	public:
		struct AttackedLog
		{
			DWORD 	dwPID;
			DWORD	dwAttackedTime;

			AttackedLog() : dwPID(0), dwAttackedTime(0)
			{
			}
		};

		AttackLog	m_kAttackLog;
		AttackedLog m_AttackedLog;
		int			m_speed_hack_count;

	private :
		std::string m_strNewName;

	public :
		const std::string GetNewName() const { return this->m_strNewName; }
		void SetNewName(const std::string name) { this->m_strNewName = name; }

	public :
		void GoHome();

	private :
		std::set<DWORD>	m_known_guild;

	public :
		void SendGuildName(CGuild* pGuild);
		void SendGuildName(DWORD dwGuildID);

	private :
		DWORD m_dwLogOffInterval;

	public :
		DWORD GetLogOffInterval() const { return m_dwLogOffInterval; }

	public:
		bool UnEquipSpecialRideUniqueItem ();

#ifdef __GUILD_SAFEBOX__
		bool CanWarp ();
#else
		bool CanWarp () const;
#endif

#ifdef ENABLE_FRATELLO_FIX
		bool			CheckWarpEquip() const;
#endif

	public:
		void AutoRecoveryItemProcess (const EAffectTypes);

	public:
		void BuffOnAttr_AddBuffsFromItem(LPITEM pItem);
		void BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem);

	private:
		void BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue);
		void BuffOnAttr_ClearAll();

		typedef std::map <BYTE, CBuffOnAttributes*> TMapBuffOnAttrs;
		TMapBuffOnAttrs m_map_buff_on_attrs;

	public:
		void SetArmada() { cannot_dead = true; }
		void ResetArmada() { cannot_dead = false; }
	private:
		bool cannot_dead;

#ifdef __PET_SYSTEM__
	private:
		bool m_bIsPet;
	public:
		void SetPet() { m_bIsPet = true; }
		bool IsPet() { return m_bIsPet; }
#endif
#ifdef NEW_ICEDAMAGE_SYSTEM
	private:
		DWORD m_dwNDRFlag;
		std::set<DWORD> m_setNDAFlag;
	public:
		const DWORD GetNoDamageRaceFlag();
		void SetNoDamageRaceFlag(DWORD dwRaceFlag);
		void UnsetNoDamageRaceFlag(DWORD dwRaceFlag);
		void ResetNoDamageRaceFlag();
		const std::set<DWORD> & GetNoDamageAffectFlag();
		void SetNoDamageAffectFlag(DWORD dwAffectFlag);
		void UnsetNoDamageAffectFlag(DWORD dwAffectFlag);
		void ResetNoDamageAffectFlag();
#endif

	private:
		float m_fAttMul;
		float m_fDamMul;
	public:
		float GetAttMul() { return this->m_fAttMul; }
		void SetAttMul(float newAttMul) {this->m_fAttMul = newAttMul; }
		float GetDamMul() { return this->m_fDamMul; }
		void SetDamMul(float newDamMul) {this->m_fDamMul = newDamMul; }

	public:
		bool IsValidItemPosition(TItemPos Pos) const;

	public:

		void	DragonSoul_Initialize();

		bool	DragonSoul_IsQualified() const;
		void	DragonSoul_GiveQualification();

		int		DragonSoul_GetActiveDeck() const;
		bool	DragonSoul_IsDeckActivated() const;
		bool	DragonSoul_ActivateDeck(int deck_idx);

		void	DragonSoul_DeactivateAll();

		void	DragonSoul_CleanUp();

	public:
		bool		DragonSoul_RefineWindow_Open(LPENTITY pEntity);
		bool		DragonSoul_RefineWindow_Close();
		LPENTITY	DragonSoul_RefineWindow_GetOpener() { return  m_pointsInstant.m_pDragonSoulRefineWindowOpener; }
		bool		DragonSoul_RefineWindow_CanRefine();

	private:
		unsigned int itemAward_vnum;
		char		 itemAward_cmd[20];
	public:
		unsigned int GetItemAward_vnum() { return itemAward_vnum; }
		char*		 GetItemAward_cmd() { return itemAward_cmd;	  }
		void		 SetItemAward_vnum(unsigned int vnum) { itemAward_vnum = vnum; }
		void		 SetItemAward_cmd(char* cmd) { strcpy(itemAward_cmd,cmd); }

	private:
		timeval		m_tvLastSyncTime;
		int			m_iSyncHackCount;
	public:
		void			SetLastSyncTime(const timeval &tv) { memcpy(&m_tvLastSyncTime, &tv, sizeof(timeval)); }
		const timeval&	GetLastSyncTime() { return m_tvLastSyncTime; }
		void			SetSyncHackCount(int iCount) { m_iSyncHackCount = iCount;}
		int				GetSyncHackCount() { return m_iSyncHackCount; }

	// @fixme188 BEGIN
	public:
		void ResetRewardInfo() {
			m_map_kDamage.clear();
			if (!IsPC())
				SetExp(0);
		}
		void DeadNoReward() {
			if (!IsDead())
				ResetRewardInfo();
			Dead();
		}
	// @fixme188 END

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	protected:
		bool	m_bAcceCombination, m_bAcceAbsorption;

	public:
		bool	IsAcceOpened(bool bCombination) {return bCombination ? m_bAcceCombination : m_bAcceAbsorption;}
		bool	IsAcceOpened() const { return (m_bAcceCombination || m_bAcceAbsorption); }
		void	OpenAcce(bool bCombination);
		void	CloseAcce();
		void	ClearAcceMaterials();
		bool	CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget);
		std::vector<LPITEM>	GetAcceMaterials();
		const TItemPosEx*	GetAcceMaterialsInfo();
		void	SetAcceMaterial(int pos, LPITEM ptr);
		bool	AcceIsSameGrade(long lGrade);
		DWORD	GetAcceCombinePrice(long lGrade);
		void	GetAcceCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs);
		BYTE	CheckEmptyMaterialSlot();
		void	AddAcceMaterial(TItemPos tPos, BYTE bPos);
		void	RemoveAcceMaterial(BYTE bPos);
		BYTE	CanRefineAcceMaterials();
		void	RefineAcceMaterials();
#endif

#ifdef ENABLE_MOVE_CHANNEL
	public:
		bool ChangeChannel(BYTE newChannel);
#endif

#ifdef ENABLE_GEM_SYSTEM
	public:
		long GetGem() const { return m_points.gem; }
		void SetGem(long gem) { m_points.gem = gem; }
		void GiveGem(INT iAmount);

		void OpenGemShop();
		void GemShopBuy(BYTE bPos);
		bool CreateGaya(int glimmerstone_count, LPITEM metinstone_item, int cost, int pct);
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	protected:
		BYTE m_BiologActualMission;
		WORD m_BiologCollectedItems;
		BYTE m_BiologCooldownReminder;
		long m_BiologCooldown;

	public:
		CBiologSystem* GetBiologManager() const { return m_pkBiologManager; }

		BYTE GetBiologMissions() const { return m_BiologActualMission; }
		WORD GetBiologCollectedItems() const { return m_BiologCollectedItems; }
		BYTE GetBiologCooldownReminder() const { return m_BiologCooldownReminder; }
		long GetBiologCooldown() const { return m_BiologCooldown; }

		void SetBiologMissions(BYTE value) { m_BiologActualMission = value; }
		void SetBiologCollectedItems(WORD value) { m_BiologCollectedItems = value; }
		void SetBiologCooldownReminder(BYTE value) { m_BiologCooldownReminder = value; }
		void SetBiologCooldown(long value) { m_BiologCooldown = value; }

	private:
		CBiologSystem* m_pkBiologManager;
#endif

#ifdef ENABLE_SKILL_GROUP_GUI
	public:
		void MasterAllSkills(LPCHARACTER ch);
#endif

#ifdef __AUTO_SKILL_READER__
	public:
		void GetAutoSkill(BYTE skillIdx, bool status, bool isFromEvent = false);
		bool ReadSkill(BYTE skillIdx);
		bool SkillToBook(BYTE skillIdx, DWORD& itemIdx, DWORD& socket0, DWORD& exorcismIdx, DWORD& concentratedIdx);
		void SetAutoSkillEvent(LPEVENT _event) { m_pkAutoSkill = _event; }
		BYTE GetSelectedSkillIndex() { return m_bSelectedSkillIdx; }
	protected:
		enum EAutoSkillRead
		{
			// Constant values don't change
			SKILL_GRADE_MASTER = 0,
			SKILL_GRADE_GRAND_MASTER = 1,
			SKILL_GRADE_PERFECT_MASTER = 2,
			SKILL_GRADE_L_MASTER = 3,
			// Constant values don't change

			USE_YMIR_50300_SKILLBOOK = 0, // if the 0 system will search the manuel books example: 50401
			USE_COOLTIME_ON_BOOKS = 1,
			USE_COOLTIME_ON_SOULSTONE = 1,

			USE_DECREASE_ALIGNMENT_SOULSTONE = 1,
			USE_ZEN_BEAN_SOULSTONE = 1,
		};

		bool m_bAutoSkillStatus;
		BYTE m_bSelectedSkillIdx;
		LPEVENT m_pkAutoSkill;
#endif

#ifdef ENABLE_BUFF_ITEMS_SYSTEM
	public:
		void GetBuffs(long lDuration);
#endif

#if defined(ENABLE_REMOTE_SHOP)
	public:
		DWORD GetLastRemoteTime() const { return dwLastRemoteTime; }
		void SetLastRemoteTime(DWORD dwTime) { dwLastRemoteTime = dwTime; }
	private:
		DWORD dwLastRemoteTime;
#endif

#ifdef ENABLE_EXTENDED_BLEND
	public:
		int GetAffectType(LPITEM item);
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	public:
		int GetEmptyInventory(BYTE bType, DWORD dwVnum, BYTE bSize) const;
#endif

#if defined(ENABLE_OFFLINE_MESSAGE)
	protected:
		DWORD				dwLastOfflinePMTime;
	public:
		DWORD				GetLastOfflinePMTime() const { return dwLastOfflinePMTime; }
		void				SetLastOfflinePMTime() { dwLastOfflinePMTime = get_dword_time(); }
		void				SendOfflineMessage(const char* To, const char* Message);
		void				ReadOfflineMessages();
#endif

#ifdef ENABLE_RENEWAL_DEAD_PACKET
	public:
		DWORD CalculateDeadTime(BYTE type);
#endif

#if defined(ENABLE_COSTUME_PET)
	public:
		void CheckWears();
#endif

#ifdef RENEWAL_PICKUP_AFFECT
	public:
		bool CanPickupDirectly();
#endif

#ifdef ENABLE_ANTI_EXP
	public:
		bool GetAntiExp() { return m_pointsInstant.anti_exp; }
		void SetAntiExp(bool flag) { m_pointsInstant.anti_exp = flag; }
#endif

#ifdef ENABLE_HUNTING_SYSTEM
	public:
		void CheckHunting(bool isLevelUp = 0);
		void OpenHuntingWindowMain();
		void OpenHuntingWindowSelect();
		void OpenHuntingWindowReward();
		
		void UpdateHuntingMission(DWORD dwMonsterVnum);
		void ReciveHuntingRewards();
		void SetCachedRewards();

		void SetRewardRandomItemFromTable();
		void SendRandomItemPacket(bool IsSelectWindow);
		
		int GetRaceTable();
		int GetMinMaxMoney(DWORD missionLevel, bool AskMax);
		int GetRandomMoney(DWORD missionLevel);
		int GetMinMaxExp(DWORD missionLevel, bool AskMax);
		int GetRandomExp(DWORD missionLevel);
#endif
	public:
		// bool OpenChest(LPITEM item, WORD openCount);
		bool GiveItemFromSpecialItemGroupNew(DWORD dwGroupNum, WORD& loopCount);
		bool AutoGiveItemChest(LPITEM item, DWORD& itemCount, std::map<LPITEM, WORD>& vecUpdateItems);
#ifdef ENABLE_MOUNT_SYSTEM
	public:
		bool IsNormalHorse(DWORD dwHorseID) const;
		std::string GenerateMountName(DWORD dwMountVnum);
		void SaveMountItem(LPITEM item);
		LPITEM GetMountItem();
		void ClearMountItem();
		void GetSpecialMount(DWORD & dwVnum, bool bTakeOut = false);
		void RefreshMount(bool bTakeOut = false);
		void SummonMount();
		void ApplyMountBonus();
#endif
#if defined(__BL_67_ATTR__)
	public:
		void Open67Attr();
		void Set67Attr(bool b) { b67Attr = b; }
		bool Is67AttrOpen() const { return b67Attr; }
	private:
		bool b67Attr;
#endif
	public:
		void SetProtectTime(const std::string& flagname, int value);
		int GetProtectTime(const std::string& flagname) const;
	protected:
		std::map<std::string, int>  m_protection_Time;
#ifdef ENABLE_DUNGEON_INFO_SYSTEM
	public:
		void SetDungeonInfoOpen(bool bOpen = true) { m_bDungeonInfoOpen = bOpen; };
		bool IsDungeonInfoOpen() { return m_bDungeonInfoOpen; }

		void StartDungeonInfoReloadEvent();
		void StopDungeonInfoReloadEvent();

		bool UpdateDungeonRanking(const std::string c_strQuestName);

		void SetLastDamage(int iLastDamage) { m_iLastDamage = iLastDamage; }
		int GetLastDamage() { return m_iLastDamage; }

	protected:
		bool m_bDungeonInfoOpen;
		LPEVENT m_pkDungeonInfoReloadEvent;
		int m_iLastDamage;
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	public:
		void			SetSkillColor(DWORD* dwSkillColor);
		DWORD* GetSkillColor() { return m_dwSkillColor[0]; }
	protected:
		DWORD			m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
// #ifdef ENABLE_GLOBAL_REWARD
// 	public:
// 		DWORD GetRewardData(const BYTE bType);
// 		void SetRewardData(const BYTE bType, const DWORD value);

// 		void LoadRewardData();
// 		void SaveRewardData();
// 	protected:
// 		std::map<BYTE, DWORD> m_mapRewardData;
// 		bool m_bRewardLoaded;
// #endif
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	public:
		void UpdateCostumeVisibilityStatus();
#endif
#ifdef ENABLE_D_DECREASE_HP_BY_PCT
	public:
		void DecreaseHPByPercentage(int iPercent);
#endif
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	public:
		bool HasBlockedDrops();
		void UpdateCharacterWarpCheck(bool state) { bAFisWarping = state; }
		bool IsSetWarp() { return bAFisWarping; }
	protected:
		bool bAFisWarping;
#endif
#ifdef ENABLE_SPLIT_BY_COUNT
	public:
		void SendSplitItemByCount(const TItemPos &Cell, short sCount);
#endif
#ifdef ENABLE_RANK_PLAYER
	public:
		void SaveRankingInfo();
#endif
#ifdef __RANKING_SYSTEM__
	private:
		BYTE	m_bLastRankCategory;

	public:
		BYTE	GetLastRankCategory() { return m_bLastRankCategory; }
		void	SetLastRankCategory(BYTE bCat) { m_bLastRankCategory = bCat; }
		int 	m_iRequestRankTimer;
		int		GetRequestRankTimer() const	{ return m_iRequestRankTimer; }
		void	SetRequestRankTimer() { m_iRequestRankTimer = thecore_pulse(); }	
		void	SaveRankingInfo();		
#endif
#ifdef ENABLE_BATTLE_PASS
	public:
		void LoadBattlePass(DWORD dwCount, TPlayerBattlePassMission * data);
		DWORD GetMissionProgress(DWORD dwMissionID, DWORD dwBattlePassID);
		void UpdateMissionProgress(DWORD dwMissionID, DWORD dwBattlePassID, DWORD dwUpdateValue, DWORD dwTotalValue, bool isOverride = false, bool isSkip = false);
		bool IsCompletedMission(BYTE bMissionType);
		bool IsLoadedBattlePass() const	{ return m_bIsLoadedBattlePass; }
		BYTE GetBattlePassId();
		void SkipBattlePassMission(BYTE missionID);
	private:
		bool m_bIsLoadedBattlePass;
		std::list<TPlayerBattlePassMission *> m_listBattlePass;
#endif
#ifdef ENABLE_DAILY_BOSS
	protected:
		std::map<DWORD, DWORD> m_CharacterDamageInfo;
	public:
		int GetDamageByVid(DWORD dwTargetVID);
		void RegisterDamage(DWORD dwTargetVID, int iDamage);
#endif
#ifdef ENABLE_ITEMSHOP
	public:
		void SendItemshopItemsPacket(const std::unordered_map <BYTE, std::vector<TItemshopItemTable>> items, const std::unordered_map <BYTE, TItemshopCategoryInfo> categories);
		void SendPromotionRewardPacket(BYTE byAnswer, std::vector<TPromotionItemTable>);
		void SendItemshopSingleItemRefreshPacket(TItemshopItemTable* item);
		void SendItemshopSingleItemRemovePacket(TItemshopItemTable* item);
		void SendItemshopSingleItemAddPacket(TItemshopItemTable* item, TItemshopCategoryInfo category);
		void SendItemshopCoinPacket();
		bool CanUseItemshop() const { return  m_iItemshopCooldownTime < thecore_pulse(); }
		void SetItemshopCooldown(int pulse) { m_iItemshopCooldownTime = pulse; }
		bool CanUsePromotionCode() const { return  m_iPromotionCodeCooldownTime < thecore_pulse(); }
		void SetPromotionCodedown(int pulse) { m_iPromotionCodeCooldownTime = pulse; }
	protected:
		int m_iItemshopCooldownTime;
		int m_iPromotionCodeCooldownTime;
#endif
#ifdef ENABLE_QUICK_OPEN
public:
	void SetInventoryBuffer(bool bMode) { bInvBuff = bMode; }
	bool IsInvBuffOn() { return bInvBuff; }
	void AddItemToInvBuff(LPITEM item) { us_buffered_items.insert(item); }
	void RemoveItemFromInvBuff(LPITEM item) { us_buffered_items.erase(item); }
	void SendBufferedInventoryPacket();
	void QuickOpenStack(LPITEM item);

private:
	std::unordered_set<LPITEM> us_buffered_items;
	bool bInvBuff;
#endif
#ifdef ENABLE_FAST_STACK
public:
	bool FastStack(TItemPos cell);
#endif
public:
	DWORD GetCash();
	void SetCash(DWORD dwCash);

	DWORD GetCoins();
	void SetCoins(DWORD dwCoins);

#ifdef ENABLE_VOTE_4_BUFF
public:
	long long	GetVoteCoin();
	void SetVoteCoin(long long amount);
#endif

#ifdef __HIT_LIMITER_ENABLE__
public:
	const DWORD & GetHitCount() { return dw_hit_count; }
	const DWORD & GetHitNextUpdate() { return dw_hit_next_update; }
	void SetHitCount(const DWORD & dwHitCount) { dw_hit_count = dwHitCount; }
	void SetHitNextUpdate(const DWORD & dwHitNextUpdate) { dw_hit_next_update = dwHitNextUpdate; }
	bool FindEnemyVID(const DWORD & dwVID) { return (um_enemy_vids.find(dwVID) != um_enemy_vids.end()); }
	void AppendEnemyVID(const DWORD & dwVID) { um_enemy_vids.insert(std::make_pair(dwVID, 0)); }
	void UpdateEnemyHitCount(const DWORD & dwVID, const WORD & wCount) { um_enemy_vids[dwVID] = wCount; }
	const WORD & GetEnemyVIDHitCount(const DWORD & dwVID) { return um_enemy_vids[dwVID]; }
	const WORD GetEnemyVIDCount() { return um_enemy_vids.size(); }
	void ClearEnemyVids() { return um_enemy_vids.clear(); }
private:
	DWORD dw_hit_count;
	DWORD dw_hit_next_update;
	std::unordered_map<DWORD, WORD> um_enemy_vids;
#endif

#ifdef ENABLE_CSHIELD
	private:
		spCShield m_cshield;
	public:
		spCShield GetCShield() const { return m_cshield; }
#endif

	public:
		bool			IsCanRespondRequestWarp(std::string stName);
		void			SetCanRespondRequestWarp(std::string stName);
		void			RemoveCanRespondRequestWarp(std::string stName);

		std::map<std::string, int> m_map_warp_request;

#ifdef FAST_EQUIP_WORLDARD
	public:
		bool 			IsValidItemChangeEquip(int cell, LPITEM item);
		LPITEM			GetChangeEquipItem(WORD wCell) const;
		void 			SetTimeChangeEquip(int time ) {time_change_equip = time;}
		int 			GetTimeChangeEquip(){return time_change_equip;}
		DWORD 			GetLastSkillTime() const {return m_dwLastSkillTime;}
		bool 			ChechPositionAvailable(int iWearCell);
		int 			IsWearUniqueChangeEquip(BYTE page_index_ce,LPITEM item);

	private:
		int 	time_change_equip;

#endif

#ifdef __SKILL_COSTUME__
	public:
		void 			UseSkillCostumeItem(LPITEM item, bool bEquip);
		void 			SetSkillMotion(BYTE * bSkillMotion);
		BYTE* 			GetSkillMotion() { return m_bSkillMotion; }
	protected:
		BYTE			m_bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT]; 		
#endif

#ifdef BLOCK_TELEPORT_HACK
	public:
		void		StopAntiTPInit();
		bool		AntiTeleportCheck(long x, long y);
	private:
		long  m_lLastX;
		long  m_lLastY;
		long  m_lLastIdx;
		DWORD m_dwLastSyncTime;
		bool  m_bAntiTPInit;
		int   m_iTpStrike;
#endif

};

ESex GET_SEX(LPCHARACTER ch);

#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
