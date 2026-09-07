#pragma once
#include "../GameLib/ItemData.h"

struct SAffects
{
	enum
	{
		AFFECT_MAX_NUM = 32,
	};

	SAffects() : dwAffects(0) {}
	SAffects(const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}
	int operator = (const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}

	BOOL IsAffect(BYTE byIndex)
	{
		return dwAffects & (1 << byIndex);
	}

	void __SetAffects(const DWORD & c_rAffects)
	{
		dwAffects = c_rAffects;
	}

	DWORD dwAffects;
};

extern std::string g_strGuildSymbolPathName;

constexpr DWORD c_Name_Max_Length = 64;
constexpr DWORD c_FileName_Max_Length = 128;
constexpr DWORD c_Short_Name_Max_Length = 32;

constexpr DWORD c_Inventory_Page_Column = 5;
constexpr DWORD c_Inventory_Page_Row = 9;
constexpr DWORD c_Inventory_Page_Size = c_Inventory_Page_Column*c_Inventory_Page_Row; // x*y
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
constexpr DWORD c_Inventory_Page_Count = 4;
#else
constexpr DWORD c_Inventory_Page_Count = 2;
#endif
constexpr DWORD c_ItemSlot_Count = c_Inventory_Page_Size * c_Inventory_Page_Count;
constexpr DWORD c_Equipment_Count = 12;

constexpr DWORD c_Equipment_Start = c_ItemSlot_Count;

constexpr DWORD c_Equipment_Body	= c_Equipment_Start + CItemData::WEAR_BODY;
constexpr DWORD c_Equipment_Head	= c_Equipment_Start + CItemData::WEAR_HEAD;
constexpr DWORD c_Equipment_Shoes	= c_Equipment_Start + CItemData::WEAR_FOOTS;
constexpr DWORD c_Equipment_Wrist	= c_Equipment_Start + CItemData::WEAR_WRIST;
constexpr DWORD c_Equipment_Weapon	= c_Equipment_Start + CItemData::WEAR_WEAPON;
constexpr DWORD c_Equipment_Neck	= c_Equipment_Start + CItemData::WEAR_NECK;
constexpr DWORD c_Equipment_Ear		= c_Equipment_Start + CItemData::WEAR_EAR;
constexpr DWORD c_Equipment_Unique1	= c_Equipment_Start + CItemData::WEAR_UNIQUE1;
constexpr DWORD c_Equipment_Unique2	= c_Equipment_Start + CItemData::WEAR_UNIQUE2;
constexpr DWORD c_Equipment_Arrow	= c_Equipment_Start + CItemData::WEAR_ARROW;
constexpr DWORD c_Equipment_Shield	= c_Equipment_Start + CItemData::WEAR_SHIELD;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
constexpr DWORD c_New_Equipment_Start = c_Equipment_Start + CItemData::WEAR_BELT;
constexpr DWORD c_New_Equipment_Count = 1;
constexpr DWORD c_Equipment_Belt  = c_Equipment_Start + CItemData::WEAR_BELT;
#endif
#ifdef ENABLE_PENDANT_SYSTEM
constexpr DWORD c_Equipment_Pendant  = c_Equipment_Start + CItemData::WEAR_PENDANT;
#endif
#ifdef ENABLE_GLOVE_SYSTEM
constexpr DWORD c_Equipment_Glove  = c_Equipment_Start + CItemData::WEAR_GLOVE;
#endif
#ifdef ENABLE_COSTUME_PET
	const DWORD c_Equipment_Pet		= c_Equipment_Start + CItemData::WEAR_PET;
#endif

#ifdef WJ_ENABLE_TRADABLE_ICON
enum ETopWindowTypes
{
	ON_TOP_WND_NONE,
	ON_TOP_WND_SHOP,
	ON_TOP_WND_EXCHANGE,
	ON_TOP_WND_SAFEBOX,
	ON_TOP_WND_PRIVATE_SHOP,
#ifdef ENABLE_MOVE_COSTUME_ATTR
	ON_TOP_WND_ITEM_COMB,
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	ON_TOP_WND_COMBINE,
	ON_TOP_WND_ABSORB_PHASE1,
	ON_TOP_WND_ABSORB_PHASE2,
#endif
#ifdef ENABLE_ITEM_MANAGER
	ON_TOP_WND_MANAGER,
#endif
#ifdef ENABLE_MAILBOX_SYSTEM
	ON_TOP_WND_MAILBOX,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	ON_TOP_WND_PET_FEED,
	ON_TOP_WND_PET_ATTR_CHANGE,
	ON_TOP_WND_PET_PRIMIUM_FEEDSTUFF,
#endif
#ifdef ENABLE_FISH_EVENT
	ON_TOP_WND_FISH_EVENT,
#endif
#ifdef ENABLE_RENEWAL_CUBE
	ON_TOP_WND_CUBE,
#endif
#if defined(__BL_67_ATTR__)
	ON_TOP_WND_ATTR_67,
#endif
	ON_TOP_WND_MAX,
};
#endif

#ifdef ENABLE_SKILL_COSTUME_SYSTEM
namespace ESkillMotionLength
{
	enum ESkillMotionLength
	{
		MAX_SKILL_COUNT = 6,
		BUFF_BEGIN = MAX_SKILL_COUNT,
		MAX_BUFF_COUNT = 5,
	};
}
#endif

enum EDragonSoulDeckType
{
	DS_DECK_1,
	DS_DECK_2,
	DS_DECK_MAX_NUM = 2,
};

enum EDragonSoulGradeTypes
{
	DRAGON_SOUL_GRADE_NORMAL,
	DRAGON_SOUL_GRADE_BRILLIANT,
	DRAGON_SOUL_GRADE_RARE,
	DRAGON_SOUL_GRADE_ANCIENT,
	DRAGON_SOUL_GRADE_LEGENDARY,
	DRAGON_SOUL_GRADE_MAX,
};

enum EDragonSoulStepTypes
{
	DRAGON_SOUL_STEP_LOWEST,
	DRAGON_SOUL_STEP_LOW,
	DRAGON_SOUL_STEP_MID,
	DRAGON_SOUL_STEP_HIGH,
	DRAGON_SOUL_STEP_HIGHEST,
	DRAGON_SOUL_STEP_MAX,
};

#ifdef ENABLE_SPECIAL_INVENTORY
enum ESpecialInventoryTypes
{
	INVENTORY_TYPE_INVENTORY,
	INVENTORY_TYPE_SKILLBOOK,
	INVENTORY_TYPE_STONE,
	INVENTORY_TYPE_MATERIAL,
	INVENTORY_TYPE_COUNT,
};

enum ESpecialInventorySize
{
	SPECIAL_INVENTORY_WIDTH = 5,
	SPECIAL_INVENTORY_HEIGHT = 9,
	SPECIAL_INVENTORY_PAGE_SIZE = SPECIAL_INVENTORY_WIDTH * SPECIAL_INVENTORY_HEIGHT,
};
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
enum EBiologBonuses
{
	MAX_BONUSES_LENGTH = 3,
};
#endif

#ifdef ENABLE_COSTUME_SYSTEM
	const DWORD c_Costume_Slot_Start	= c_Equipment_Start + CItemData::WEAR_COSTUME_BODY;
	const DWORD	c_Costume_Slot_Body		= c_Costume_Slot_Start + CItemData::COSTUME_BODY;
	const DWORD	c_Costume_Slot_Hair		= c_Costume_Slot_Start + CItemData::COSTUME_HAIR;
//#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
//	const DWORD	c_Costume_Slot_Mount	= c_Costume_Slot_Start + CItemData::COSTUME_MOUNT;
//#endif
#ifdef ENABLE_MOUNT_SYSTEM
	const DWORD	c_Costume_Slot_Mount 	= c_Equipment_Start + CItemData::WEAR_MOUNT;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Acce		= c_Costume_Slot_Start + CItemData::COSTUME_ACCE;
#endif

#if defined(ENABLE_WEAPON_COSTUME_SYSTEM) || defined(ENABLE_ACCE_COSTUME_SYSTEM)
	const DWORD c_Costume_Slot_Count	= 4;
#elif defined(ENABLE_MOUNT_COSTUME_SYSTEM)
	const DWORD c_Costume_Slot_Count	= 3;
#else
	const DWORD c_Costume_Slot_Count	= 2;
#endif

	const DWORD c_Costume_Slot_End		= c_Costume_Slot_Start + c_Costume_Slot_Count;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Weapon	= c_Equipment_Start + CItemData::WEAR_COSTUME_WEAPON; // c_Costume_Slot_End + 1;
#endif

#endif

#ifdef ENABLE_SHINING_ITEM_SYSTEM
	const DWORD	c_Shining_Slot_Weapon1	= c_Equipment_Start + CItemData::WEAR_SHINING_WEAPON_1;
	const DWORD	c_Shining_Slot_Weapon2	= c_Equipment_Start + CItemData::WEAR_SHINING_WEAPON_2;
	const DWORD	c_Shining_Slot_Weapon3	= c_Equipment_Start + CItemData::WEAR_SHINING_WEAPON_3;
	const DWORD	c_Shining_Slot_Body1	= c_Equipment_Start + CItemData::WEAR_SHINING_ARMOR_1;
	const DWORD	c_Shining_Slot_Body2	= c_Equipment_Start + CItemData::WEAR_SHINING_ARMOR_2;
	const DWORD	c_Shining_Slot_Special	= c_Equipment_Start + CItemData::WEAR_SHINING_SPECIAL;
	const DWORD c_Shining_Slot_Count = 6;
	const DWORD c_Shining_Slot_Start = c_Shining_Slot_Weapon1;
	const DWORD c_Shining_Slot_End = c_Shining_Slot_Start + c_Shining_Slot_Count;
#endif

#ifdef ENABLE_BOOSTER_ITEMS
	const DWORD	c_Booster_Slot_Weapon = c_Equipment_Start + CItemData::WEAR_BOOSTER_WEAPON;
	const DWORD	c_Booster_Slot_Body	= c_Equipment_Start + CItemData::WEAR_BOOSTER_ARMOR;
	const DWORD	c_Booster_Slot_Hair	= c_Equipment_Start + CItemData::WEAR_BOOSTER_HAIR;
	const DWORD c_Booster_Slot_Count = 3;
	const DWORD c_Booster_Slot_Start = c_Booster_Slot_Weapon;
	const DWORD c_Booster_Slot_End = c_Booster_Slot_Start + c_Booster_Slot_Count;
#endif

#ifdef FAST_EQUIP_WORLDARD 
const DWORD c_change_equip_Slot_Start = c_New_Equipment_Start + c_New_Equipment_Count;
const DWORD c_change_equip_Slot_End = c_change_equip_Slot_Start + 30 * 10;
#endif		

const DWORD c_Wear_Max = CItemData::WEAR_MAX_NUM;
const DWORD c_DragonSoul_Equip_Start = c_ItemSlot_Count + c_Wear_Max;
const DWORD c_DragonSoul_Equip_Slot_Max = 6;
const DWORD c_DragonSoul_Equip_End = c_DragonSoul_Equip_Start + c_DragonSoul_Equip_Slot_Max * DS_DECK_MAX_NUM;

const DWORD c_DragonSoul_Equip_Reserved_Count = c_DragonSoul_Equip_Slot_Max * 3;

// Belt Inventory Window
const DWORD c_Belt_Inventory_Slot_Start = c_DragonSoul_Equip_End + c_DragonSoul_Equip_Reserved_Count;
const DWORD c_Belt_Inventory_Width = 4;
const DWORD c_Belt_Inventory_Height= 4;
const DWORD c_Belt_Inventory_Slot_Count = c_Belt_Inventory_Width * c_Belt_Inventory_Height;
const DWORD c_Belt_Inventory_Slot_End = c_Belt_Inventory_Slot_Start + c_Belt_Inventory_Slot_Count;

#ifdef ENABLE_SPECIAL_INVENTORY
	const DWORD c_Special_Inventory_Slot_Start = c_Belt_Inventory_Slot_End;
	const DWORD c_Special_Inventory_Skillbook_Slot_Start = c_Special_Inventory_Slot_Start;
	const DWORD c_Special_Inventory_Skillbook_Slot_End = c_Special_Inventory_Skillbook_Slot_Start + SPECIAL_INVENTORY_PAGE_SIZE * c_Inventory_Page_Count;
	const DWORD c_Special_Inventory_Stone_Slot_Start = c_Special_Inventory_Skillbook_Slot_End;
	const DWORD c_Special_Inventory_Stone_Slot_End = c_Special_Inventory_Stone_Slot_Start + SPECIAL_INVENTORY_PAGE_SIZE * c_Inventory_Page_Count;
	const DWORD c_Special_Inventory_Material_Slot_Start = c_Special_Inventory_Stone_Slot_End;
	const DWORD c_Special_Inventory_Material_Slot_End = c_Special_Inventory_Material_Slot_Start + SPECIAL_INVENTORY_PAGE_SIZE * c_Inventory_Page_Count;
	const DWORD c_Special_Inventory_Slot_End = c_Special_Inventory_Material_Slot_End;

	const DWORD c_Inventory_Count = c_Special_Inventory_Slot_End;
#else
	const DWORD c_Inventory_Count	= c_Belt_Inventory_Slot_End;
#endif

const DWORD c_DragonSoul_Inventory_Start = 0;
const DWORD c_DragonSoul_Inventory_Box_Size = 32;
#ifdef ENABLE_EXTENDED_DS_INVENTORY
	const DWORD c_DragonSoul_Inventory_Page_Count = 3;
	const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Page_Count * c_DragonSoul_Inventory_Box_Size;
#else
	const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Box_Size;
#endif
const DWORD c_DragonSoul_Inventory_End = c_DragonSoul_Inventory_Start + c_DragonSoul_Inventory_Count;

enum ESlotType
{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_SKILL,
	SLOT_TYPE_EMOTION,
	SLOT_TYPE_SHOP,
	SLOT_TYPE_EXCHANGE_OWNER,
	SLOT_TYPE_EXCHANGE_TARGET,
	SLOT_TYPE_QUICK_SLOT,
	SLOT_TYPE_SAFEBOX,
	SLOT_TYPE_PRIVATE_SHOP,
	SLOT_TYPE_MALL,
	SLOT_TYPE_DRAGON_SOUL_INVENTORY,
#ifdef ENABLE_SWITCHBOT_SYSTEM
	SLOT_TYPE_SWITCHBOT,
#endif
#ifdef ENABLE_GUILD_SAFEBOX
	SLOT_TYPE_GUILD_SAFEBOX,
#endif

	SLOT_TYPE_GUILDSTORAGE,

#ifdef FAST_EQUIP_WORLDARD
	SLOT_TYPE_CHANGE_EQUIP,
#endif

	SLOT_TYPE_MAX,
};

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
#ifdef ENABLE_SWITCHBOT_SYSTEM
	SWITCHBOT,
#endif
//#ifdef ENABLE_GUILD_SAFEBOX
	GUILD_SAFEBOX,
//#endif
	GUILDSTORAGE,
#ifdef FAST_EQUIP_WORLDARD
	CHANGE_EQUIP,
#endif
	GROUND,
	WINDOW_TYPE_MAX,
};

#ifdef FAST_EQUIP_WORLDARD 
enum ChangeEquipConfig
{
	CHANGE_EQUIP_PAGE_EXTRA = 10,
	CHANGE_EQUIP_SLOT_COUNT = 30 * CHANGE_EQUIP_PAGE_EXTRA,
};

#endif

enum EDSInventoryMaxNum
{
	DS_INVENTORY_MAX_NUM = c_DragonSoul_Inventory_Count,
	DS_REFINE_WINDOW_MAX_NUM = 15,
};

#ifdef ENABLE_SWITCHBOT_SYSTEM
enum ESwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 5,
	SWITCHBOT_ALTERNATIVE_COUNT = 2,
	MAX_NORM_ATTR_NUM = 5 + 2,// # GRM Update 16112023
};

enum EAttributeSet
{
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY,
	ATTRIBUTE_SET_WRIST,
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
	ATTRIBUTE_SET_MAX_NUM,
};
#endif

#pragma pack (push, 1)
#define WORD_MAX 0xffff

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
    SItemPos ()
    {
		window_type =     INVENTORY;
		cell = WORD_MAX;
    }
	SItemPos (BYTE _window_type, WORD _cell)
    {
        window_type = _window_type;
        cell = _cell;
    }

  //  int operator=(const int _cell)
  //  {
		//window_type = INVENTORY;
  //      cell = _cell;
  //      return cell;
  //  }
	bool IsValidCell()
	{
		switch (window_type)
		{
		case INVENTORY:
			return cell < c_Inventory_Count;
			break;
		case EQUIPMENT:
			return cell < c_DragonSoul_Equip_End;
			break;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DS_INVENTORY_MAX_NUM);
			break;

#ifdef FAST_EQUIP_WORLDARD
		case CHANGE_EQUIP:
			return cell < CHANGE_EQUIP_SLOT_COUNT;
			break;
#endif

#ifdef ENABLE_SWITCHBOT_SYSTEM
		case SWITCHBOT:
			return cell < SWITCHBOT_SLOT_COUNT;
			break;
#endif
		default:
			return false;
		}
	}
	bool IsEquipCell()
	{
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			return (c_Equipment_Start + c_Wear_Max > cell) && (c_Equipment_Start <= cell);
			break;

		case BELT_INVENTORY:
		case DRAGON_SOUL_INVENTORY:
			return false;
			break;

		default:
			return false;
		}
	}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	bool IsBeltInventoryCell()
	{
		bool bResult = c_Belt_Inventory_Slot_Start <= cell && c_Belt_Inventory_Slot_End > cell;
		return bResult;
	}
#endif

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}

	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;
#pragma pack(pop)

const DWORD c_QuickBar_Line_Count = 3;
const DWORD c_QuickBar_Slot_Count = 12;

const float c_Idle_WaitTime = 5.0f;

const int c_Monster_Race_Start_Number = 6;
const int c_Monster_Model_Start_Number = 20001;

const float c_fAttack_Delay_Time = 0.2f;
const float c_fHit_Delay_Time = 0.1f;
const float c_fCrash_Wave_Time = 0.2f;
const float c_fCrash_Wave_Distance = 3.0f;

const float c_fHeight_Step_Distance = 50.0f;

enum
{
	DISTANCE_TYPE_FOUR_WAY,
	DISTANCE_TYPE_EIGHT_WAY,
	DISTANCE_TYPE_ONE_WAY,
	DISTANCE_TYPE_MAX_NUM,
};

const float c_fMagic_Script_Version = 1.0f;
const float c_fSkill_Script_Version = 1.0f;
const float c_fMagicSoundInformation_Version = 1.0f;
const float c_fBattleCommand_Script_Version = 1.0f;
const float c_fEmotionCommand_Script_Version = 1.0f;
const float c_fActive_Script_Version = 1.0f;
const float c_fPassive_Script_Version = 1.0f;

// Used by PushMove
const float c_fWalkDistance = 175.0f;
const float c_fRunDistance = 310.0f;

#define FILE_MAX_LEN 128

enum
{
	ITEM_SOCKET_SLOT_MAX_NUM = 3,
	// refactored attribute slot begin
	ITEM_ATTRIBUTE_SLOT_NORM_NUM	= 5,
	ITEM_ATTRIBUTE_SLOT_RARE_NUM	= 2,

	ITEM_ATTRIBUTE_SLOT_NORM_START	= 0,
	ITEM_ATTRIBUTE_SLOT_NORM_END	= ITEM_ATTRIBUTE_SLOT_NORM_START + ITEM_ATTRIBUTE_SLOT_NORM_NUM,

	ITEM_ATTRIBUTE_SLOT_RARE_START	= ITEM_ATTRIBUTE_SLOT_NORM_END,
	ITEM_ATTRIBUTE_SLOT_RARE_END	= ITEM_ATTRIBUTE_SLOT_RARE_START + ITEM_ATTRIBUTE_SLOT_RARE_NUM,

	ITEM_ATTRIBUTE_SLOT_MAX_NUM		= ITEM_ATTRIBUTE_SLOT_RARE_END, // 7
	// refactored attribute slot end

#if defined(ENABLE_RENEWAL_SHOPEX)
	SHOP_MAX_ITEM_NUM = 5,
#endif
};

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
// Same with ECostumeHideParts @ common/length.h
enum ECostumeHideParts
{
	HIDE_COSTUME_HAIR,
	HIDE_COSTUME_BODY,
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	HIDE_COSTUME_WEAPON,
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	HIDE_COSTUME_ACCE,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	HIDE_COSTUME_AURA,
#endif
	HIDE_COSTUME_COUNT,
};
#endif

#pragma pack(push)
#pragma pack(1)

typedef struct SQuickSlot
{
	BYTE Type;
	BYTE Position;
} TQuickSlot;

typedef struct TPlayerItemAttribute
{
    BYTE        bType;
    short       sValue;
} TPlayerItemAttribute;

typedef struct packet_item
{
    DWORD       vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short		count;
#else
	BYTE		count;
#endif
	DWORD		flags;
	DWORD		anti_flags;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TItemData;

#if defined(ENABLE_RENEWAL_SHOPEX)
enum STableExTypes
{
	SHOPEX_GOLD = 1,
	SHOPEX_SECONDARY,
	SHOPEX_ITEM,
	SHOPEX_EXP,
#ifdef ENABLE_GEM_SYSTEM
	SHOPEX_GEM,
#endif
	SHOPEX_MAX,
};
#endif

typedef struct packet_shop_item
{
    DWORD       vnum;
#ifdef ENABLE_GOLD_MAX_EXTENDED
	unsigned long long       price;
#else
	DWORD       price;
#endif
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short		count;
#else
	BYTE		count;
#endif
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(ENABLE_RENEWAL_SHOPEX)
	BYTE 		price_type;
	DWORD		item_vnum[SHOP_MAX_ITEM_NUM];
	DWORD		item_price[SHOP_MAX_ITEM_NUM];
	packet_shop_item() : price_type(SHOPEX_GOLD)
	{
		memset(&item_vnum, 0, sizeof(item_vnum));
		memset(&item_price, 0, sizeof(item_price));
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
	}
#endif
} TShopItemData;

#ifdef ENABLE_GEM_SYSTEM
typedef struct SGemShopItem
{
	BYTE	slotIndex;
	DWORD	dwVnum;
	BYTE	bCount;
	DWORD	dwPrice;
} TGemShopItem;
#endif

#ifdef ENABLE_GLOBAL_REWARD
enum
{
	REWARD_MISSION_LEVEL_UP,
	REWARD_MISSION_LEVEL_UP_PET,
	REWARD_MISSION_SKILL_UPGRADE,
	REWARD_MISSION_KILL_MONSTER,
	REWARD_MISSION_KILL_STONE,
	REWARD_MISSION_KILL_BOSS,
	REWARD_MISSION_INVENTORY_SLOT,
	REWARD_MISSION_OFFLINESHOP_SLOT,
	REWARD_MISSION_AVERAGE_BONUS,
	REWARD_MISSION_BATTLEPASS,
	REWARD_MISSION_FIRST_ITEM,
	REWARD_MISSION_CUSTOM_SASH,
	REWARD_MISSION_BIOLOG,
	REWARD_MISSION_PASSIVE_SKILL_COMPLETE,
	REWARD_MISSION_DUNGEON,
	REWARD_MISSION_USE_ITEM,
	REWARD_MISSION_SELL_ITEM,
	REWARD_MISSION_BUY_ITEM,
	REWARD_MISSION_PLAYTIME,
	REWARD_MISSION_COMPLETE_SKILL,
};
#endif

#ifdef ENABLE_BATTLE_PASS
typedef struct SBattlePassRewardItem
{
	DWORD dwVnum;
	BYTE bCount;
} TBattlePassRewardItem;

typedef struct SBattlePassMissionInfo
{
	BYTE bMissionType;
	DWORD dwMissionInfo[3];
	TBattlePassRewardItem aRewardList[3];
} TBattlePassMissionInfo;

typedef struct SBattlePassRanking
{
	BYTE bPos;
	char playerName[60/*CHARACTER_NAME_MAX_LEN*/ + 1];
	DWORD dwFinishTime;
} TBattlePassRanking;
#endif

#pragma pack(pop)

inline float GetSqrtDistance(int ix1, int iy1, int ix2, int iy2) // By sqrt
{
	float dx, dy;

	dx = float(ix1 - ix2);
	dy = float(iy1 - iy2);

	return sqrtf(dx*dx + dy*dy);
}

// DEFAULT_FONT
void DefaultFont_Startup();
void DefaultFont_Cleanup();
void DefaultFont_SetName(const char * c_szFontName);
CResource* DefaultFont_GetResource();
CResource* DefaultItalicFont_GetResource();
// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char * c_szPathName);
const char * GetGuildSymbolFileName(DWORD dwGuildID);
BYTE SlotTypeToInvenType(BYTE bSlotType);
#ifdef ENABLE_DETAILS_UI
BYTE ApplyTypeToPointType(BYTE bApplyType);
#endif

#ifdef ENABLE_DUNGEON_INFO_SYSTEM
enum EDungeonInfo
{
	MAX_DUNGEONS = 100,
	MAX_REQUIRED_ITEMS = 3,
	MAX_BOSS_ITEM_SLOTS = 16,
	MAX_BOSS_DROP_ITEMS = 255,
	MAX_DUNGEON_SCROLL = 10,
};
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
enum ESkillColorLength
{
	MAX_SKILL_COUNT = 6,
	MAX_EFFECT_COUNT = 5,
	BUFF_BEGIN = MAX_SKILL_COUNT,
	MAX_BUFF_COUNT = 5,
};
#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
