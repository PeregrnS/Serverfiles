#ifndef __INC_PACKET_H__
#define __INC_PACKET_H__
#include "stdafx.h"
#ifdef ENABLE_REWARD_SYSTEM
	#include <vector>
#endif

#include <cstdint>

enum
{
	HEADER_CG_HANDSHAKE				= 0xff,
	HEADER_CG_PONG					= 0xfe,
	HEADER_CG_TIME_SYNC				= 0xfc,
	HEADER_CG_KEY_AGREEMENT			= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_

	HEADER_CG_LOGIN					= 1,
	HEADER_CG_ATTACK				= 2,
	HEADER_CG_CHAT					= 3,
	HEADER_CG_CHARACTER_CREATE		= 4,
	HEADER_CG_CHARACTER_DELETE		= 5,
	HEADER_CG_CHARACTER_SELECT		= 6,
	HEADER_CG_MOVE					= 7,
	HEADER_CG_SYNC_POSITION			= 8,
	HEADER_CG_ENTERGAME				= 10,

	HEADER_CG_ITEM_USE				= 11,
	HEADER_CG_ITEM_DROP				= 12,
	HEADER_CG_ITEM_MOVE				= 13,
	HEADER_CG_ITEM_PICKUP			= 15,

	HEADER_CG_QUICKSLOT_ADD			= 16,
	HEADER_CG_QUICKSLOT_DEL			= 17,
	HEADER_CG_QUICKSLOT_SWAP		= 18,
	HEADER_CG_WHISPER				= 19,
	HEADER_CG_ITEM_DROP2			= 20,
#ifdef ENABLE_ITEM_MANAGER
	HEADER_CG_ITEM_DESTROY 			= 21,
	HEADER_CG_ITEM_SELL 			= 22,
#endif
#if defined(ENABLE_REMOTE_SHOP)
	HEADER_CG_REMOTE_SHOP 			= 23,
#endif
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	HEADER_CG_ANTI_FARM 			= 24,
#endif
#ifdef ENABLE_SWITCHBOT_SYSTEM
	HEADER_CG_SWITCHBOT				= 25,
#endif
	HEADER_CG_ON_CLICK				= 26,
	HEADER_CG_EXCHANGE				= 27,
	HEADER_CG_CHARACTER_POSITION	= 28,
	HEADER_CG_SCRIPT_ANSWER			= 29,
	HEADER_CG_QUEST_INPUT_STRING	= 30,
	HEADER_CG_QUEST_CONFIRM			= 31,
#ifdef ENABLE_SPLIT_BY_COUNT
	HEADER_CG_SPLIT_ITEM			= 32,
#endif

	HEADER_CG_SHOP					= 50,
	HEADER_CG_FLY_TARGETING			= 51,
	HEADER_CG_USE_SKILL				= 52,
	HEADER_CG_ADD_FLY_TARGETING		= 53,
	HEADER_CG_SHOOT					= 54,
	HEADER_CG_MYSHOP				= 55,
#ifdef ENABLE_GEM_SYSTEM
	HEADER_CG_GEM_SHOP 				= 56,
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	HEADER_CG_BIOLOG_MANAGER		= 57,
#endif
#ifdef ENABLE_SKILL_GROUP_GUI
	HEADER_CG_SKILL_GROUP_SELECT 	= 58,
#endif

	HEADER_CG_ITEM_USE_TO_ITEM		= 60,
	HEADER_CG_TARGET			 	= 61,
#ifdef ENABLE_DROP_INFO
	HEADER_CG_DROP_INFO				= 62,
#endif
	// HEADER_CG_WIKI 				= 63,
	HEADER_CG_TEXT					= 64,
	HEADER_CG_WARP					= 65,
	HEADER_CG_SCRIPT_BUTTON			= 66,
	HEADER_CG_MESSENGER				= 67,

	HEADER_CG_MALL_CHECKOUT			= 69,
	HEADER_CG_SAFEBOX_CHECKIN		= 70,
	HEADER_CG_SAFEBOX_CHECKOUT		= 71,

	HEADER_CG_PARTY_INVITE			= 72,
	HEADER_CG_PARTY_INVITE_ANSWER	= 73,
	HEADER_CG_PARTY_REMOVE			= 74,
	HEADER_CG_PARTY_SET_STATE		= 75,
	HEADER_CG_PARTY_USE_SKILL		= 76,
	HEADER_CG_SAFEBOX_ITEM_MOVE		= 77,
	HEADER_CG_PARTY_PARAMETER		= 78,

	HEADER_CG_GUILD					= 80,
	HEADER_CG_ANSWER_MAKE_GUILD		= 81,

	HEADER_CG_FISHING				= 82,

	HEADER_CG_ITEM_GIVE				= 83,
#ifdef ENABLE_FAST_STACK
	HEADER_CG_FAST_STACK			= 84,
#endif

	HEADER_CG_EMPIRE				= 90,

#ifdef ENABLE_BATTLE_PASS
	HEADER_CG_BATTLE_PASS 			= 92,
#endif

	HEADER_CG_REFINE				= 96,

	HEADER_CG_MARK_LOGIN			= 100,
	HEADER_CG_MARK_CRCLIST			= 101,
	HEADER_CG_MARK_UPLOAD			= 102,
	HEADER_CG_MARK_IDXLIST			= 104,

	HEADER_CG_HACK					= 105,
	HEADER_CG_CHANGE_NAME			= 106,
	HEADER_CG_LOGIN2				= 109,
	HEADER_CG_DUNGEON				= 110,
	HEADER_CG_LOGIN3				= 111,

	HEADER_CG_GUILD_SYMBOL_UPLOAD	= 112,
	HEADER_CG_SYMBOL_CRC			= 113,

	// SCRIPT_SELECT_ITEM
	HEADER_CG_SCRIPT_SELECT_ITEM	= 114,
	// END_OF_SCRIPT_SELECT_ITEM
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	HEADER_CG_ACCE					= 116,
#endif
#ifdef ENABLE_EVENT_MANAGER
	HEADER_CG_REQUEST_EVENT_QUEST 	= 117,
	HEADER_CG_REQUEST_EVENT_DATA 	= 118,
#endif
#ifdef ENABLE_HWID_BAN
	HEADER_CG_HWID_SYSTEM			= 119,
#endif
#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_CG_SEND_HUNTING_ACTION	= 120,
#endif
#if defined(__BL_67_ATTR__)
	HEADER_CG_67_ATTR				= 121,
	HEADER_CG_CLOSE_67_ATTR			= 122,
#endif
#ifdef ENABLE_DUNGEON_INFO_SYSTEM	
	HEADER_CG_DUNGEON_INFO			= 123,
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	HEADER_CG_SKILL_COLOR 			= 124,
#endif
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	HEADER_CG_CHANGE_COSTUME_VISIBILITY_STATUS = 125,
#endif
#ifdef ENABLE_ITEMSHOP
	HEADER_CG_BUY_ITEMSHOP_ITEM = 126,
	HEADER_CG_PROMOTION = 127,
#endif
#ifdef __GUILD_SAFEBOX__
	HEADER_CG_GUILD_SAFEBOX_OPEN = 128,
	HEADER_CG_GUILD_SAFEBOX_CHECKIN = 129,
	HEADER_CG_GUILD_SAFEBOX_CHECKOUT = 130,
	HEADER_CG_GUILD_SAFEBOX_ITEM_MOVE = 131,
	HEADER_CG_GUILD_SAFEBOX_GIVE_GOLD = 132,
	HEADER_CG_GUILD_SAFEBOX_GET_GOLD = 133,
#endif
#ifdef ENABLE_TRASH_SYSTEM
	HEADER_CG_TRASH					= 134,
#endif
#ifdef ENABLE_CHEST_OPEN_RENEWAL
	HEADER_CG_CHEST_OPEN			= 135,
#endif

	HEADER_CG_DRAGON_SOUL_REFINE	= 205,
	HEADER_CG_STATE_CHECKER			= 206,

#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_CG_PRIVATE_SHOP			= 207,
#endif

	HEADER_CG_GUILDSTORAGE = 208,

	HEADER_CG_CLIENT_VERSION		= 0xfd,
	HEADER_CG_CLIENT_VERSION2		= 0xf1,

	/********************************************************/
	HEADER_GC_KEY_AGREEMENT_COMPLETED			= 0xfa, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_KEY_AGREEMENT						= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_TIME_SYNC							= 0xfc,
	HEADER_GC_PHASE								= 0xfd,
	HEADER_GC_BINDUDP							= 0xfe,
	HEADER_GC_HANDSHAKE							= 0xff,

	HEADER_GC_CHARACTER_ADD						= 1,
	HEADER_GC_CHARACTER_DEL						= 2,
	HEADER_GC_MOVE								= 3,
	HEADER_GC_CHAT								= 4,
	HEADER_GC_SYNC_POSITION						= 5,

	HEADER_GC_LOGIN_SUCCESS						= 6,
	HEADER_GC_LOGIN_SUCCESS_NEWSLOT				= 32,
	HEADER_GC_LOGIN_FAILURE						= 7,

	HEADER_GC_CHARACTER_CREATE_SUCCESS			= 8,
	HEADER_GC_CHARACTER_CREATE_FAILURE			= 9,
	HEADER_GC_CHARACTER_DELETE_SUCCESS			= 10,
	HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID	= 11,

	HEADER_GC_ATTACK							= 12,
	HEADER_GC_STUN								= 13,
	HEADER_GC_DEAD								= 14,

	HEADER_GC_MAIN_CHARACTER_OLD				= 15,
	HEADER_GC_CHARACTER_POINTS					= 16,
	HEADER_GC_CHARACTER_POINT_CHANGE			= 17,
	HEADER_GC_CHANGE_SPEED						= 18,
	HEADER_GC_CHARACTER_UPDATE					= 19,

	HEADER_GC_ITEM_DEL							= 20,
	HEADER_GC_ITEM_SET							= 21,
	HEADER_GC_ITEM_USE							= 22,
	HEADER_GC_ITEM_DROP							= 23,
#ifdef ENABLE_QUICK_OPEN
	HEADER_GC_ITEM_BUFFERED 					= 24,
#endif
	HEADER_GC_ITEM_UPDATE						= 25,

	HEADER_GC_ITEM_GROUND_ADD					= 26,
	HEADER_GC_ITEM_GROUND_DEL					= 27,

	HEADER_GC_QUICKSLOT_ADD						= 28,
	HEADER_GC_QUICKSLOT_DEL						= 29,
	HEADER_GC_QUICKSLOT_SWAP					= 30,

	HEADER_GC_ITEM_OWNERSHIP					= 31,

	HEADER_GC_WHISPER							= 34,

	HEADER_GC_MOTION							= 36,
	HEADER_GC_PARTS								= 37,

	HEADER_GC_SHOP								= 38,
	HEADER_GC_SHOP_SIGN							= 39,

	HEADER_GC_DUEL_START						= 40,
	HEADER_GC_PVP                               = 41,
	HEADER_GC_EXCHANGE							= 42,
	HEADER_GC_CHARACTER_POSITION				= 43,

	HEADER_GC_PING								= 44,
	HEADER_GC_SCRIPT							= 45,
	HEADER_GC_QUEST_CONFIRM						= 46,
#ifdef ENABLE_SKILL_GROUP_GUI
	HEADER_GC_SKILL_GROUP_SELECT 				= 47,
#endif
#ifdef ENABLE_GOLD_MAX_EXTENDED
	HEADER_GC_CHARACTER_GOLD 					= 48,
	HEADER_GC_CHARACTER_GOLD_CHANGE 			= 49,
#endif
#ifdef ENABLE_ATLASS_EXTENDED
	HEADER_GC_BOSS_POSITION						= 50,
#endif
#ifdef ENABLE_BATTLE_PASS
	HEADER_GC_BATTLE_PASS_OPEN 					= 54,
	HEADER_GC_BATTLE_PASS_UPDATE 				= 55,
	HEADER_GC_BATTLE_PASS_RANKING 				= 56,
#endif

	HEADER_GC_MOUNT								= 61,
	HEADER_GC_OWNERSHIP							= 62,
	HEADER_GC_TARGET			 				= 63,
	//HEADER_GC_WIKI							= 64,
	HEADER_GC_WARP								= 65,

	HEADER_GC_ADD_FLY_TARGETING					= 69,
	HEADER_GC_CREATE_FLY						= 70,
	HEADER_GC_FLY_TARGETING						= 71,
	HEADER_GC_SKILL_LEVEL_OLD					= 72,
	HEADER_GC_SKILL_LEVEL						= 76,

	HEADER_GC_MESSENGER							= 74,
	HEADER_GC_GUILD								= 75,

	HEADER_GC_PARTY_INVITE						= 77,
	HEADER_GC_PARTY_ADD							= 78,
	HEADER_GC_PARTY_UPDATE						= 79,
	HEADER_GC_PARTY_REMOVE						= 80,
	HEADER_GC_QUEST_INFO						= 81,
	HEADER_GC_REQUEST_MAKE_GUILD				= 82,
	HEADER_GC_PARTY_PARAMETER					= 83,

	HEADER_GC_SAFEBOX_SET						= 85,
	HEADER_GC_SAFEBOX_DEL						= 86,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD			= 87,
	HEADER_GC_SAFEBOX_SIZE						= 88,

	HEADER_GC_FISHING							= 89,

	HEADER_GC_EMPIRE							= 90,

	HEADER_GC_PARTY_LINK						= 91,
	HEADER_GC_PARTY_UNLINK						= 92,

	HEADER_GC_REFINE_INFORMATION_OLD			= 95,

	HEADER_GC_VIEW_EQUIP						= 99,

	HEADER_GC_MARK_BLOCK						= 100,
	HEADER_GC_MARK_IDXLIST						= 102,
#ifdef ENABLE_GEM_SYSTEM
	HEADER_GC_GEM_SHOP_OPEN 					= 103,
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	HEADER_GC_BIOLOG_MANAGER					= 104,
#endif

	HEADER_GC_TIME								= 106,
	HEADER_GC_CHANGE_NAME						= 107,
#ifdef ENABLE_SWITCHBOT_SYSTEM
	HEADER_GC_SWITCHBOT							= 108,
#endif
#if defined(__BL_67_ATTR__)
	HEADER_GC_OPEN_67_ATTR 						= 109,
#endif

	HEADER_GC_DUNGEON							= 110,

	HEADER_GC_WALK_MODE							= 111,
	HEADER_GC_SKILL_GROUP						= 112,
	HEADER_GC_MAIN_CHARACTER					= 113,

	HEADER_GC_SEPCIAL_EFFECT					= 114,

	HEADER_GC_NPC_POSITION						= 115,

	HEADER_GC_LOGIN_KEY							= 118,
	HEADER_GC_REFINE_INFORMATION				= 119,
	HEADER_GC_CHANNEL							= 121,

	HEADER_GC_TARGET_UPDATE						= 123,
	HEADER_GC_TARGET_DELETE						= 124,
	HEADER_GC_TARGET_CREATE						= 125,

	HEADER_GC_AFFECT_ADD						= 126,
	HEADER_GC_AFFECT_REMOVE						= 127,

	HEADER_GC_MALL_OPEN							= 122,
	HEADER_GC_MALL_SET							= 128,
	HEADER_GC_MALL_DEL							= 129,

	HEADER_GC_LAND_LIST							= 130,
	HEADER_GC_LOVER_INFO						= 131,
	HEADER_GC_LOVE_POINT_UPDATE					= 132,

	HEADER_GC_SYMBOL_DATA						= 133,

	// MINING
	HEADER_GC_DIG_MOTION						= 134,
	// END_OF_MINING

	HEADER_GC_DAMAGE_INFO						= 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO				= 136,

	// SUPPORT_BGM
	HEADER_GC_MAIN_CHARACTER3_BGM				= 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL			= 138,
	// END_OF_SUPPORT_BGM

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	HEADER_GC_ACCE								= 139,
#endif
#ifdef ENABLE_DROP_INFO
	HEADER_GC_DROP_INFO							= 140,
#endif
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GC_EVENT_INFO						= 141,
	HEADER_GC_EVENT_RELOAD						= 142,
	HEADER_GC_EVENT_KW_SCORE 					= 143,
#endif
#ifdef ENABLE_MAINTENANCE_SYSTEM
	HEADER_GC_MAINTENANCE_INFO 					= 144,
#endif
#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_GC_HUNTING_OPEN_MAIN					= 145,
	HEADER_GC_HUNTING_OPEN_SELECT				= 146,
	HEADER_GC_HUNTING_OPEN_REWARD				= 147,
	HEADER_GC_HUNTING_UPDATE					= 148,
	HEADER_GC_HUNTING_RECIVE_RAND_ITEMS			= 149,
#endif

	HEADER_GC_AUTH_SUCCESS						= 150,

	HEADER_GC_PANAMA_PACK						= 151,

	//HYBRID CRYPT
	HEADER_GC_HYBRIDCRYPT_KEYS					= 152,
	HEADER_GC_HYBRIDCRYPT_SDB					= 153, // SDB means Supplmentary Data Blocks
	//HYBRID CRYPT

#ifdef ENABLE_DUNGEON_INFO_SYSTEM	
	HEADER_GC_DUNGEON_INFO						= 154,
	HEADER_GC_DUNGEON_RANKING					= 155,
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	HEADER_GC_UPDATE_COSTUME_VISIBILITY_STATUS 	= 156,
#endif
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	HEADER_GC_ANTI_FARM 						= 157,
#endif
#ifdef ENABLE_RANK_PLAYER
	HEADER_GC_RANK_INFO 						= 158,
#endif
#ifdef __RANKING_SYSTEM__
	HEADER_GC_RANK_INFO 						= 158,
#endif
#ifdef ENABLE_ITEMSHOP
	HEADER_GC_ITEMSHOP 							= 159,
#endif
	HEADER_GC_CHARACTER_DRAGON_POINTS 			= 160,
	HEADER_GC_CHARACTER_DRAGON_POINTS_CHANGE 	= 161,
#ifdef __GUILD_SAFEBOX__
	HEADER_GC_GUILD_SAFEBOX						= 162,
#endif
#ifdef ENABLE_EVENT_CALENDAR
	HEADER_GC_EVENT_MANAGER						= 163,
#endif
#ifdef ENABLE_TRASH_SYSTEM
	HEADER_GC_TRASH								= 164,
#endif

	// ROULETTE
	HEADER_GC_ROULETTE							= 200,
	// END_ROULETTE

	HEADER_GC_SPECIFIC_EFFECT					= 208,

	HEADER_GC_DRAGON_SOUL_REFINE				= 209,
	HEADER_GC_RESPOND_CHANNELSTATUS				= 210,
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_GC_PRIVATE_SHOP						= 211,
#endif

	HEADER_GC_GUILDSTORAGE = 212,

#ifdef __DAILY_QUESTS__
	HEADER_GC_SEND_DAILY_QUESTS					= 218,
	HEADER_GC_SEND_DAILY_REWARDS				= 219,
	HEADER_GC_UPDATE_DAILY_QUESTS				= 220,
	HEADER_GC_UPDATE_DAILY_REWARDS				= 221,
#endif

	/////////////////////////////////////////////////////////////////////////////

	HEADER_GG_LOGIN								= 1,
	HEADER_GG_LOGOUT							= 2,
	HEADER_GG_RELAY								= 3,
	HEADER_GG_NOTICE							= 4,
	HEADER_GG_SHUTDOWN							= 5,
	HEADER_GG_GUILD								= 6,
	HEADER_GG_DISCONNECT						= 7,
	HEADER_GG_SHOUT								= 8,
	HEADER_GG_SETUP								= 9,
	HEADER_GG_MESSENGER_ADD          			= 10,
	HEADER_GG_MESSENGER_REMOVE       			= 11,
	HEADER_GG_FIND_POSITION						= 12,
	HEADER_GG_WARP_CHARACTER					= 13,
#ifdef CROSS_CHANNEL_FRIEND_REQUEST
	HEADER_GG_MESSENGER_REQUEST_ADD				= 14,
#endif
	HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX			= 15,
	HEADER_GG_TRANSFER							= 16,
	HEADER_GG_XMAS_WARP_SANTA					= 17,
	HEADER_GG_XMAS_WARP_SANTA_REPLY				= 18,
	HEADER_GG_RELOAD_CRC_LIST					= 19,
	HEADER_GG_LOGIN_PING						= 20,
	HEADER_GG_CHECK_CLIENT_VERSION				= 21,
	HEADER_GG_BLOCK_CHAT						= 22,
	HEADER_GG_SIEGE								= 25,
	HEADER_GG_MONARCH_NOTICE					= 26,
	HEADER_GG_MONARCH_TRANSFER					= 27,
#ifdef ENABLE_SWITCHBOT_SYSTEM
	HEADER_GG_SWITCHBOT							= 28,
#endif
	HEADER_GG_CHECK_AWAKENESS					= 29,
#ifdef ENABLE_FULL_NOTICE
	HEADER_GG_BIG_NOTICE						= 30,
#endif
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GG_EVENT_RELOAD 						= 31,
	HEADER_GG_EVENT 							= 32,
	HEADER_GG_EVENT_HIDE_AND_SEEK 				= 33,
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH_RESULT	= 34,
	HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH			= 35,
	HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH_UPDATE	= 36,
#endif
#ifdef ENABLE_MAINTENANCE_SYSTEM
	HEADER_GG_MAINTENANCE_INFO					= 37,
#endif
#ifdef ENABLE_GLOBAL_REWARD
	HEADER_GG_REWARD_INFO 						= 38,
#endif
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	HEADER_GG_ANTI_FARM 						= 39,
#endif
#ifdef ENABLE_RANK_PLAYER
	HEADER_GG_PLAYER_RANK						= 40,
#endif
#ifdef __RANKING_SYSTEM__
	HEADER_GG_PLAYER_RANK 						= 40,
#endif
#ifdef ENABLE_REWARD_SYSTEM
	HEADER_GG_REWARD_INFO						= 41,
#endif
#ifdef ENABLE_LOG_NOTIFICATIONS
	HEADER_GG_CSHIELD_LOG_NOTIFICATION	= 42,
#endif

#ifdef ENABLE_TELEPORT_GUILD
	HEADER_GG_GUILD_TELEPORT	= 43,
#endif
#ifdef ENABLE_TELEPORT_PM
	HEADER_GG_REQUEST_WARP = 44,
#endif
};

#pragma pack(1)
typedef struct SPacketGGSetup
{
	BYTE	bHeader;
	WORD	wPort;
	BYTE	bChannel;
} TPacketGGSetup;

typedef struct SPacketGGLogin
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD	dwPID;
	BYTE	bEmpire;
	long	lMapIndex;
	BYTE	bChannel;
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	char	cMAIf[MA_LENGTH + 1];
	int8_t	i8BlockState;
#endif
} TPacketGGLogin;

typedef struct SPacketGGLogout
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	bool	bAFisWarping;
#endif
} TPacketGGLogout;

typedef struct SPacketGGRelay
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lSize;
} TPacketGGRelay;

typedef struct SPacketGGNotice
{
	BYTE	bHeader;
	long	lSize;
} TPacketGGNotice;

typedef struct SPacketGGMonarchNotice
{
	BYTE	bHeader;
	BYTE	bEmpire;
	long	lSize;
} TPacketGGMonarchNotice;

//FORKED_ROAD
typedef struct SPacketGGForkedMapInfo
{
	BYTE	bHeader;
	BYTE	bPass;
	BYTE	bSungzi;
} TPacketGGForkedMapInfo;
//END_FORKED_ROAD
typedef struct SPacketGGShutdown
{
	BYTE	bHeader;
} TPacketGGShutdown;

typedef struct SPacketGGGuild
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
} TPacketGGGuild;

enum
{
	GUILD_SUBHEADER_GG_CHAT,
	GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS,
};

typedef struct SPacketGGGuildChat
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
	char	szText[CHAT_MAX_LEN + 1];
} TPacketGGGuildChat;

typedef struct SPacketGGParty
{
	BYTE	header;
	BYTE	subheader;
	DWORD	pid;
	DWORD	leaderpid;
} TPacketGGParty;

enum
{
	PARTY_SUBHEADER_GG_CREATE,
	PARTY_SUBHEADER_GG_DESTROY,
	PARTY_SUBHEADER_GG_JOIN,
	PARTY_SUBHEADER_GG_QUIT,
};

typedef struct SPacketGGDisconnect
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGDisconnect;

typedef struct SPacketGGShout
{
	BYTE	bHeader;
	BYTE	bEmpire;
	char	szText[CHAT_MAX_LEN + 1];
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
	char    szCountry[COUNTRY_NAME_MAX_LEN + 1]{};
	int     hyperlinks{};
#endif
} TPacketGGShout;

typedef struct SPacketGGXmasWarpSanta
{
	BYTE	bHeader;
	BYTE	bChannel;
	long	lMapIndex;
} TPacketGGXmasWarpSanta;

typedef struct SPacketGGXmasWarpSantaReply
{
	BYTE	bHeader;
	BYTE	bChannel;
} TPacketGGXmasWarpSantaReply;

typedef struct SPacketGGMessenger
{
	BYTE        bHeader;
	char        szAccount[CHARACTER_NAME_MAX_LEN + 1];
	char        szCompanion[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessenger;

#ifdef ENABLE_TELEPORT_PM
typedef struct SPacketGGRequestWarp
{
	BYTE	header;
	bool	bRequestAccepted;
	DWORD	dwTargetPID;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	char	szNameRequest[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGRequestWarp;
#endif

#ifdef CROSS_CHANNEL_FRIEND_REQUEST
typedef struct SPacketGGMessengerRequest
{
	uint8_t	header;
	char	account[CHARACTER_NAME_MAX_LEN + 1];
	char	target[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessengerRequest;
#endif

typedef struct SPacketGGFindPosition
{
	BYTE header;
	DWORD dwFromPID;
	DWORD dwTargetPID;
} TPacketGGFindPosition;

typedef struct SPacketGGWarpCharacter
{
	BYTE header;
	DWORD pid;
	long x;
	long y;
#ifdef ENABLE_CMD_WARP_IN_DUNGEON
	int mapIndex;
#endif
	WORD wPort;
} TPacketGGWarpCharacter;

//  HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX	    = 15,

typedef struct SPacketGGGuildWarMapIndex
{
	BYTE bHeader;
	DWORD dwGuildID1;
	DWORD dwGuildID2;
	long lMapIndex;
} TPacketGGGuildWarMapIndex;

typedef struct SPacketGGTransfer
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lX, lY;
} TPacketGGTransfer;

typedef struct SPacketGGLoginPing
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGLoginPing;

typedef struct SPacketGGBlockChat
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lBlockDuration;
} TPacketGGBlockChat;

typedef struct command_text
{
	BYTE	bHeader;
} TPacketCGText;

typedef struct command_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketCGHandshake;

typedef struct command_login
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	char	cMAIf[MA_LENGTH + 1];
#endif
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
#ifdef ENABLE_PIN_LOGIN
	char	pin[4 + 1];
#endif
#ifdef ENABLE_HWID_BAN
	char	hwid[HWID_MAX_LEN + 1];
#endif
	DWORD	adwClientKey[4];
} TPacketCGLogin3;

typedef struct packet_login_key
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
} TPacketGCLoginKey;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	index;
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
	char	country[COUNTRY_NAME_MAX_LEN + 1];
#endif
} TPacketCGPlayerSelect;

typedef struct command_player_delete
{
	BYTE	header;
	BYTE	index;
	char	private_code[8];
} TPacketCGPlayerDelete;

typedef struct command_player_create
{
	BYTE        header;
	BYTE        index;
	char        name[CHARACTER_NAME_MAX_LEN + 1];
	WORD        job;
	BYTE	shape;
	BYTE	Con;
	BYTE	Int;
	BYTE	Str;
	BYTE	Dex;
} TPacketCGPlayerCreate;

typedef struct command_player_create_success
{
	BYTE		header;
	BYTE		bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketGCPlayerCreateSuccess;

typedef struct command_attack
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwVID;
	BYTE	bCRCMagicCubeProcPiece;
	BYTE	bCRCMagicCubeFilePiece;
} TPacketCGAttack;

enum EMoveFuncType
{
	FUNC_WAIT,
	FUNC_MOVE,
	FUNC_ATTACK,
	FUNC_COMBO,
	FUNC_MOB_SKILL,
	_FUNC_SKILL,
	FUNC_MAX_NUM,
	FUNC_SKILL = 0x80,
};

typedef struct command_move
{
	BYTE	bHeader;
	BYTE	bFunc;
	BYTE	bArg;
	BYTE	bRot;
	long	lX;
	long	lY;
	DWORD	dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketCGSyncPositionElement;

typedef struct command_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketCGSyncPosition;

typedef struct command_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
} TPacketCGChat;

typedef struct command_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	char 	szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_entergame
{
	BYTE	header;
} TPacketCGEnterGame;

typedef struct command_item_use
{
	BYTE 	header;
	TItemPos 	Cell;
} TPacketCGItemUse;

typedef struct command_item_use_to_item
{
	BYTE	header;
	TItemPos	Cell;
	TItemPos	TargetCell;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short	count;
#else
	BYTE	count;
#endif
} TPacketCGItemDrop2;

#ifdef ENABLE_ITEM_MANAGER
typedef struct command_item_destroy
{
	BYTE header;
	TItemPos Cell;
} TPacketCGItemDestroy;

typedef struct command_item_sell
{
	BYTE header;
	TItemPos Cell;
	DWORD gold;
} TPacketCGItemSell;
#endif

typedef struct command_item_move
{
	BYTE 	header;
	TItemPos	Cell;
	TItemPos	CellTo;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short		count;
#else
	BYTE		count;
#endif
} TPacketCGItemMove;

typedef struct command_item_pickup
{
	BYTE 	header;
	DWORD	vid;
} TPacketCGItemPickup;

typedef struct command_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
} TPacketCGQuickslotAdd;

typedef struct command_quickslot_del
{
	BYTE	header;
	BYTE	pos;
} TPacketCGQuickslotDel;

typedef struct command_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	change_pos;
} TPacketCGQuickslotSwap;

enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2
};

typedef struct command_shop_buy
{
	BYTE	count;
} TPacketCGShopBuy;

typedef struct command_shop_sell
{
	BYTE	pos;
	BYTE	count;
} TPacketCGShopSell;

typedef struct command_shop
{
	BYTE	header;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short	subheader;
#else
	BYTE	subheader;
#endif
#ifdef ENABLE_SPECIAL_INVENTORY
	WORD	wPos;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short	bCount;
#else
	BYTE	bCount;
#endif
#endif
} TPacketCGShop;

typedef struct command_on_click
{
	BYTE	header;
	DWORD	vid;
} TPacketCGOnClick;

enum
{
	EXCHANGE_SUBHEADER_CG_START,	/* arg1 == vid of target character */
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ELK_ADD,	/* arg1 == amount of gold */
	EXCHANGE_SUBHEADER_CG_ACCEPT,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_CG_CANCEL,	/* arg1 == not used */
};

typedef struct command_exchange
{
	BYTE	header;
	BYTE	sub_header;
#ifdef ENABLE_GOLD_MAX_EXTENDED
	unsigned long long arg1;
#else
	DWORD arg1;
#endif
	BYTE	arg2;
	TItemPos	Pos;
#ifdef ENABLE_CHECKINOUT_UPDATE
	bool bAutoItemPos;
#endif
} TPacketCGExchange;

typedef struct command_position
{
	BYTE	header;
	BYTE	position;
} TPacketCGPosition;

typedef struct command_script_answer
{
	BYTE	header;
	BYTE	answer;
	//char	file[32 + 1];
	//BYTE	answer[16 + 1];
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
	BYTE        header;
	unsigned int	idx;
} TPacketCGScriptButton;

typedef struct command_quest_input_string
{
	BYTE header;
	char msg[64+1];
} TPacketCGQuestInputString;

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;

typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[64+1];
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketGCHandshake;

enum EPhase
{
	PHASE_CLOSE,
	PHASE_HANDSHAKE,
	PHASE_LOGIN,
	PHASE_SELECT,
	PHASE_LOADING,
	PHASE_GAME,
	PHASE_DEAD,

	PHASE_CLIENT_CONNECTING,
	PHASE_DBCLIENT,
	PHASE_P2P,
	PHASE_AUTH,
};

typedef struct packet_phase
{
	BYTE	header;
	BYTE	phase;
} TPacketGCPhase;

typedef struct packet_bindudp
{
	BYTE	header;
	DWORD	addr;
	WORD	port;
} TPacketGCBindUDP;

enum
{
	LOGIN_FAILURE_ALREADY	= 1,
	LOGIN_FAILURE_ID_NOT_EXIST	= 2,
	LOGIN_FAILURE_WRONG_PASS	= 3,
	LOGIN_FAILURE_FALSE		= 4,
	LOGIN_FAILURE_NOT_TESTOR	= 5,
	LOGIN_FAILURE_NOT_TEST_TIME	= 6,
	LOGIN_FAILURE_FULL		= 7
};

typedef struct packet_login_success
{
	BYTE		bHeader;
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
	DWORD		guild_id[PLAYER_PER_ACCOUNT];
	char		guild_name[PLAYER_PER_ACCOUNT][GUILD_NAME_MAX_LEN+1];

	DWORD		handle;
	DWORD		random_key;
} TPacketGCLoginSuccess;

typedef struct packet_auth_success
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
	BYTE	bResult;
} TPacketGCAuthSuccess;

typedef struct packet_login_failure
{
	BYTE	header;
	char	szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct packet_create_failure
{
	BYTE	header;
	BYTE	bType;
} TPacketGCCreateFailure;

enum
{
	ADD_CHARACTER_STATE_DEAD		= (1 << 0),
	ADD_CHARACTER_STATE_SPAWN		= (1 << 1),
	ADD_CHARACTER_STATE_GUNGON		= (1 << 2),
	ADD_CHARACTER_STATE_KILLER		= (1 << 3),
	ADD_CHARACTER_STATE_PARTY		= (1 << 4),
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CHR_EQUIPPART_ACCE,
#endif
	CHR_EQUIPPART_NUM,
};

#ifdef ENABLE_SHINING_ITEM_SYSTEM
enum EShiningParts
{
	CHR_SHINING_WEAPON_1,
	CHR_SHINING_WEAPON_2,
	CHR_SHINING_WEAPON_3,
	CHR_SHINING_ARMOR_1,
	CHR_SHINING_ARMOR_2,
	CHR_SHINING_SPECIAL,
	CHR_SHINING_NUM,
};
#endif

typedef struct packet_add_char
{
	BYTE	header;
	DWORD	dwVID;

	float	angle;
	long	x;
	long	y;
	long	z;

	BYTE	bType;
	DWORD	wRaceNum; // @fixme501
	DWORD	bMovingSpeed;
	BYTE	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];
} TPacketGCCharacterAdd;

typedef struct packet_char_additional_info
{
	BYTE	header;
	DWORD	dwVID;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
	char    country[COUNTRY_NAME_MAX_LEN + 1];
#endif
	DWORD	awPart[CHR_EQUIPPART_NUM]; // @fixme502
#ifdef ENABLE_SHINING_ITEM_SYSTEM
	DWORD	adwShining[CHR_SHINING_NUM];
#endif
	BYTE	bEmpire;
	DWORD	dwGuildID;
	DWORD	dwLevel;
	short	sAlignment;
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	DWORD	dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
	BYTE		byTitleIndex;
#ifdef __SKILL_COSTUME__
	BYTE	bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT];
#endif
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_update_char
{
	BYTE	header;
	DWORD	dwVID;

	DWORD	awPart[CHR_EQUIPPART_NUM]; // @fixme502
#ifdef ENABLE_SHINING_ITEM_SYSTEM
	DWORD	adwShining[CHR_SHINING_NUM];
#endif
	DWORD	bMovingSpeed;
	BYTE	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];

	DWORD	dwGuildID;
	short	sAlignment;
	DWORD	dwLevel;
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	DWORD	dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
	BYTE		byTitleIndex;
#ifdef __SKILL_COSTUME__
	BYTE	bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT];
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE	header;
	DWORD	id;
} TPacketGCCharacterDelete;

typedef struct packet_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
	DWORD	id;
	BYTE	bEmpire{};
#if defined(__BL_CLIENT_LOCALE_STRING__)
	bool	bCanFormat{true};
#endif
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
	char	szCountry[COUNTRY_NAME_MAX_LEN + 1]{};
#endif
} TPacketGCChat;

typedef struct packet_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bType;
	char	szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
#if defined(__BL_CLIENT_LOCALE_STRING__)
	bool	bCanFormat{true};
#endif
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
	BYTE	bEmpire{};
	char	szCountry[COUNTRY_NAME_MAX_LEN + 1]{};
#endif
} TPacketGCWhisper;

typedef struct packet_main_character
{
	BYTE        header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter;

// SUPPORT_BGM
typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	float	fBGMVol;
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter4_BGM_VOL;
// END_OF_SUPPORT_BGM

typedef struct packet_points
{
	BYTE	header;
	INT		points[POINT_MAX_NUM];
} TPacketGCPoints;

typedef struct packet_skill_level
{
	BYTE		bHeader;
	TPlayerSkill	skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct packet_point_change
{
	int		header;
	DWORD	dwVID;
	BYTE	type;
	long	amount;
	long	value;
} TPacketGCPointChange;

typedef struct packet_stun
{
	BYTE	header;
	DWORD	vid;
} TPacketGCStun;

#ifdef ENABLE_RENEWAL_DEAD_PACKET
enum EReviveTypes
{
	REVIVE_TYPE_HERE,
	REVIVE_TYPE_TOWN,
	REVIVE_TYPE_AUTO_TOWN,
	REVIVE_TYPE_MAX
};
#endif

typedef struct packet_dead
{
#ifdef ENABLE_RENEWAL_DEAD_PACKET
	packet_dead()
	{
		memset(&t_d, 0, sizeof(t_d));
	}
#endif
	BYTE	header;
	DWORD	vid;
#ifdef ENABLE_RENEWAL_DEAD_PACKET
	BYTE	t_d[REVIVE_TYPE_MAX];
#endif
} TPacketGCDead;

struct TPacketGCItemDelDeprecated
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short	count;
#else
	BYTE	count;
#endif
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

typedef struct packet_item_set
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short	count;
#else
	BYTE	count;
#endif
	DWORD	flags;
	DWORD	anti_flags;
	bool	highlight;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPacketGCItemSet;

typedef struct packet_item_del
{
	BYTE	header;
	BYTE	pos;
} TPacketGCItemDel;

struct packet_item_use
{
	BYTE	header;
	TItemPos Cell;
	DWORD	ch_vid;
	DWORD	victim_vid;
	DWORD	vnum;
};

struct packet_item_move
{
	BYTE	header;
	TItemPos Cell;
	TItemPos CellTo;
};

typedef struct packet_item_update
{
	BYTE	header;
	TItemPos Cell;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short	count;
#else
	BYTE	count;
#endif
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPacketGCItemUpdate;

typedef struct packet_item_ground_add
{
	BYTE	bHeader;
	long 	x, y, z;
	DWORD	dwVID;
	DWORD	dwVnum;
} TPacketGCItemGroundAdd;

typedef struct packet_item_ownership
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
#ifdef ENABLE_OWNERSHIP_DURATION_TIMER
	int		iSec;
	bool	bUpdate;
#endif
} TPacketGCItemOwnership;

typedef struct packet_item_ground_del
{
	BYTE	bHeader;
	DWORD	dwVID;
} TPacketGCItemGroundDel;

struct packet_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
};

struct packet_quickslot_del
{
	BYTE	header;
	BYTE	pos;
};

struct packet_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	pos_to;
};

struct packet_motion
{
	BYTE	header;
	DWORD	vid;
	DWORD	victim_vid;
	WORD	motion;
};

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_START,
	SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
	SHOP_SUBHEADER_GC_OK,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,
	SHOP_SUBHEADER_GC_SOLDOUT,
	SHOP_SUBHEADER_GC_INVENTORY_FULL,
	SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX,
#if defined(ENABLE_RENEWAL_SHOPEX)
	SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_EXP,
#ifdef ENABLE_GEM_SYSTEM
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_GEM,
#endif
#endif
};

struct packet_shop_item
{
	DWORD       vnum;
#ifdef ENABLE_GOLD_MAX_EXTENDED
	unsigned long long price;
#else
	DWORD		price;
#endif
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short		count;
#else
	BYTE		count;
#endif
	BYTE		display_pos;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#if defined(ENABLE_RENEWAL_SHOPEX)
	BYTE 	price_type;
	DWORD	item_vnum[SHOP_MAX_ITEM_NUM];
	DWORD	item_price[SHOP_MAX_ITEM_NUM];
	packet_shop_item() : price_type(SHOPEX_GOLD)
	{
		memset(&item_vnum, 0, sizeof(item_vnum));
		memset(&item_price, 0, sizeof(item_price));
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
	}
#endif
};

typedef struct packet_shop_start
{
	DWORD   owner_vid;
	struct packet_shop_item	items[SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCShopStart;

typedef struct packet_shop_start_ex
{
	typedef struct sub_packet_shop_tab
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;
	DWORD owner_vid;
	BYTE shop_tab_count;
} TPacketGCShopStartEx;

typedef struct packet_shop_update_item
{
	BYTE			pos;
	struct packet_shop_item	item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
#ifdef ENABLE_GOLD_MAX_EXTENDED
	unsigned long long iPrice;
#else
	int iPrice;
#endif
} TPacketGCShopUpdatePrice;

typedef struct packet_shop
{
	BYTE        header;
	WORD	size;
	BYTE        subheader;
} TPacketGCShop;

struct packet_exchange
{
	BYTE	header;
	BYTE	sub_header;
	BYTE	is_me;
#ifdef ENABLE_GOLD_MAX_EXTENDED
	unsigned long long arg1; // vnum
#else
	DWORD arg1; // vnum
#endif
	TItemPos	arg2;	// cell
	DWORD	arg3;	// count
#ifdef WJ_ENABLE_TRADABLE_ICON
	TItemPos	arg4;	// srccell
#endif
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

enum EPacketTradeSubHeaders
{
	EXCHANGE_SUBHEADER_GC_START,	/* arg1 == vid */
	EXCHANGE_SUBHEADER_GC_ITEM_ADD,	/* arg1 == vnum  arg2 == pos  arg3 == count */
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
	EXCHANGE_SUBHEADER_GC_GOLD_ADD,	/* arg1 == gold */
	EXCHANGE_SUBHEADER_GC_ACCEPT,	/* arg1 == accept */
	EXCHANGE_SUBHEADER_GC_END,		/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_ALREADY,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_LESS_GOLD,	/* arg1 == not used */
};

struct packet_position
{
	BYTE	header;
	DWORD	vid;
	BYTE	position;
};

typedef struct packet_ping
{
	BYTE	header;
} TPacketGCPing;

struct packet_script
{
	BYTE	header;
	WORD	size;
	BYTE	skin;
	WORD	src_size;
#ifdef ENABLE_QUEST_CATEGORY
	BYTE	quest_flag;
#endif
};

typedef struct packet_change_speed
{
	BYTE		header;
	DWORD		vid;
	WORD		moving_speed;
} TPacketGCChangeSpeed;

struct packet_mount
{
	BYTE	header;
	DWORD	vid;
	DWORD	mount_vid;
	BYTE	pos;
	DWORD	x, y;
};

typedef struct packet_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	DWORD		dwVID;
	long		lX;
	long		lY;
	DWORD		dwTime;
	DWORD		dwDuration;
} TPacketGCMove;

typedef struct packet_ownership
{
	BYTE		bHeader;
	DWORD		dwOwnerVID;
	DWORD		dwVictimVID;
} TPacketGCOwnership;

typedef struct packet_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketGCSyncPositionElement;

typedef struct packet_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketGCSyncPosition;

typedef struct packet_fly
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwStartVID;
	DWORD	dwEndVID;
} TPacketGCCreateFly;

typedef struct command_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwShooterVID;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE		bHeader;
	BYTE		bType;
} TPacketCGShoot;

typedef struct packet_duel_start
{
	BYTE	header;
	WORD	wSize;
} TPacketGCDuelStart;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE
};

typedef struct packet_pvp
{
	BYTE        bHeader;
	DWORD       dwVIDSrc;
	DWORD       dwVIDDst;
	BYTE        bMode;
} TPacketGCPVP;

typedef struct command_use_skill
{
	BYTE	bHeader;
	DWORD	dwVnum;
	DWORD	dwVID;
} TPacketCGUseSkill;

typedef struct command_target
{
	BYTE	header;
	DWORD	dwVID;
} TPacketCGTarget;

typedef struct packet_target
{
	BYTE	header;
	DWORD	dwVID;
	BYTE	bHPPercent;
	DWORD	dwActualHP;
	DWORD	dwMaxHP;
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	bool bCostumeHide[ECostumeHideParts::HIDE_COSTUME_COUNT];
#endif
} TPacketGCTarget;

typedef struct packet_warp
{
	BYTE	bHeader;
	long	lX;
	long	lY;
	long	lAddr;
	WORD	wPort;
} TPacketGCWarp;

typedef struct command_warp
{
	BYTE	bHeader;
} TPacketCGWarp;

struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#ifdef ENABLE_QUEST_CATEGORY
	WORD c_index;
#endif
	BYTE flag;
};

enum
{
#if defined(__MESSENGER_GM__)
	MESSENGER_SUBHEADER_GC_GM_LIST,
	MESSENGER_SUBHEADER_GC_GM_LOGIN,
	MESSENGER_SUBHEADER_GC_GM_LOGOUT,
#endif
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_INVITE,
#ifdef ENABLE_FRIENDS_LIST_FIX
	MESSENGER_SUBHEADER_GC_REMOVE_FRIEND,
#endif
};

typedef struct packet_messenger
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCMessenger;

typedef struct packet_messenger_guild_list
{
	BYTE connected;
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildList;

typedef struct packet_messenger_guild_login
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogin;

typedef struct packet_messenger_guild_logout
{
	BYTE length;

	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogout;

typedef struct packet_messenger_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerListOffline;

typedef struct packet_messenger_list_online
{
	BYTE connected; // always 1
	BYTE length;
} TPacketGCMessengerListOnline;

#if defined(__MESSENGER_GM__)
typedef struct packet_messenger_gm_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerGMListOffline;

typedef struct packet_messenger_gm_list_online
{
	BYTE connected; // always 1
	BYTE length;
} TPacketGCMessengerGMListOnline;
#endif

enum
{
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
	MESSENGER_SUBHEADER_CG_INVITE_ANSWER,
};

typedef struct command_messenger
{
	BYTE header;
	BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_add_by_vid
{
	DWORD vid;
} TPacketCGMessengerAddByVID;

typedef struct command_messenger_add_by_name
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketCGMessengerAddByName;

typedef struct command_messenger_remove
{
	char login[LOGIN_MAX_LEN+1];
	//DWORD account;
} TPacketCGMessengerRemove;

typedef struct command_safebox_checkout
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
#ifdef ENABLE_CHECKINOUT_UPDATE
	bool bAutoItemPos;
#endif
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
#ifdef ENABLE_CHECKINOUT_UPDATE
	bool bAutoItemPos;
#endif
} TPacketCGSafeboxCheckin;

///////////////////////////////////////////////////////////////////////////////////
// Party

typedef struct command_party_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketCGPartyParameter;

typedef struct paryt_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketGCPartyParameter;

typedef struct packet_party_add
{
	BYTE	header;
	DWORD	pid;
	char	name[CHARACTER_NAME_MAX_LEN+1];
#ifdef BL_PARTY_UPDATE
	DWORD mapidx;
	BYTE channel;
#endif
} TPacketGCPartyAdd;

typedef struct command_party_invite
{
	BYTE	header;
	DWORD	vid;
} TPacketCGPartyInvite;

typedef struct packet_party_invite
{
	BYTE	header;
	DWORD	leader_vid;
} TPacketGCPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE	header;
	DWORD	leader_vid;
	BYTE	accept;
} TPacketCGPartyInviteAnswer;

typedef struct packet_party_update
{
	BYTE	header;
	DWORD	pid;
	BYTE	role;
	BYTE	percent_hp;
	short	affects[7];
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

typedef struct packet_party_link
{
	BYTE header;
	DWORD pid;
	DWORD vid;
#ifdef BL_PARTY_UPDATE
	DWORD mapidx;
	BYTE channel;
#endif
} TPacketGCPartyLink;

typedef struct packet_party_unlink
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyUnlink;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE header;
	DWORD pid;
	BYTE byRole;
	BYTE flag;
} TPacketCGPartySetState;

enum
{
	PARTY_SKILL_HEAL = 1,
	PARTY_SKILL_WARP = 2
};

typedef struct command_party_use_skill
{
	BYTE header;
	BYTE bySkillIndex;
	DWORD vid;
} TPacketCGPartyUseSkill;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketCGSafeboxSize;

typedef struct packet_safebox_wrong_password
{
	BYTE	bHeader;
} TPacketCGSafeboxWrongPassword;

typedef struct command_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketGCEmpire;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE        bHeader;
	BYTE        bState;
	long	lMoney;
} TPacketCGSafeboxMoney;

typedef struct packet_safebox_money_change
{
	BYTE	bHeader;
	long	lMoney;
} TPacketGCSafeboxMoneyChange;

// Guild

enum
{
	GUILD_SUBHEADER_GC_LOGIN,
	GUILD_SUBHEADER_GC_LOGOUT,
	GUILD_SUBHEADER_GC_LIST,
	GUILD_SUBHEADER_GC_GRADE,
	GUILD_SUBHEADER_GC_ADD,
	GUILD_SUBHEADER_GC_REMOVE,
	GUILD_SUBHEADER_GC_GRADE_NAME,
	GUILD_SUBHEADER_GC_GRADE_AUTH,
	GUILD_SUBHEADER_GC_INFO,
	GUILD_SUBHEADER_GC_COMMENTS,
	GUILD_SUBHEADER_GC_CHANGE_EXP,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_GC_SKILL_INFO,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_GC_GUILD_INVITE,
	GUILD_SUBHEADER_GC_WAR,
	GUILD_SUBHEADER_GC_GUILD_NAME,
	GUILD_SUBHEADER_GC_GUILD_WAR_LIST,
	GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST,
	GUILD_SUBHEADER_GC_WAR_SCORE,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
};

enum GUILD_SUBHEADER_CG
{
	GUILD_SUBHEADER_CG_ADD_MEMBER,
	GUILD_SUBHEADER_CG_REMOVE_MEMBER,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY,
	GUILD_SUBHEADER_CG_OFFER,
	GUILD_SUBHEADER_CG_POST_COMMENT,
	GUILD_SUBHEADER_CG_DELETE_COMMENT,
	GUILD_SUBHEADER_CG_REFRESH_COMMENT,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_CG_USE_SKILL,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER,
	GUILD_SUBHEADER_CG_CHARGE_GSP,
	GUILD_SUBHEADER_CG_DEPOSIT_MONEY,
	GUILD_SUBHEADER_CG_WITHDRAW_MONEY,
};

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

typedef struct packet_guild_name_t
{
	BYTE header;
	WORD size;
	BYTE subheader;
	DWORD	guildID;
	char	guildName[GUILD_NAME_MAX_LEN];
} TPacketGCGuildName;

typedef struct packet_guild_war
{
	DWORD	dwGuildSelf;
	DWORD	dwGuildOpp;
	BYTE	bType;
	BYTE 	bWarState;
} TPacketGCGuildWar;

typedef struct command_guild
{
	BYTE header;
	BYTE subheader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN+1];
} TPacketCGAnswerMakeGuild;

typedef struct command_guild_use_skill
{
	DWORD	dwVnum;
	DWORD	dwPID;
} TPacketCGGuildUseSkill;

// Guild Mark
typedef struct command_mark_login
{
	BYTE    header;
	DWORD   handle;
	DWORD   random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
	BYTE	header;
	DWORD	gid;
	BYTE	image[16*12*4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE	header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE	header;
	BYTE	imgIdx;
	DWORD	crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE    header;
	DWORD	bufSize;
	WORD	count;
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE	header;
	DWORD	bufSize;
	BYTE	imgIdx;
	DWORD	count;
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	guild_id;
} TPacketCGGuildSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE header;
	DWORD guild_id;
	DWORD crc;
	DWORD size;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

// Fishing

typedef struct command_fishing
{
	BYTE header;
	BYTE dir;
} TPacketCGFishing;

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
} TPacketGCFishing;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short byItemCount;
#else
	BYTE byItemCount;
#endif
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE	bHeader;
	char	szBuf[255 + 1];
} TPacketCGHack;

// SubHeader - Dungeon
enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE bHeader;
	WORD size;
	BYTE subheader;
} TPacketGCDungeon;

typedef struct packet_dungeon_dest_position
{
	long x;
	long y;
} TPacketGCDungeonDestPosition;

typedef struct SPacketGCShopSign
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

typedef struct SPacketCGMyShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short	bCount;
#else
	BYTE	bCount;
#endif
} TPacketCGMyShop;

typedef struct SPacketGCTime
{
	BYTE	bHeader;
	time_t	time;
} TPacketGCTime;

enum
{
	WALKMODE_RUN,
	WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
	BYTE	header;
	DWORD	vid;
	BYTE	mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
	BYTE        header;
	BYTE        skill_group;
} TPacketGCChangeSkillGroup;

typedef struct SPacketCGRefine
{
	BYTE	header;
	WORD	pos;
	BYTE	type;
} TPacketCGRefine;

typedef struct SPacketCGRequestRefineInfo
{
	BYTE	header;
	WORD	pos;
} TPacketCGRequestRefineInfo;

typedef struct SPacketGCRefineInformaion
{
	BYTE	header;
	BYTE	type;
	WORD	pos;
	DWORD	src_vnum;
	DWORD	result_vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short	material_count;
#else
	BYTE	material_count;
#endif
	int		cost;
	int		prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TPacketGCRefineInformation;

struct TNPCPosition
{
	BYTE bType;
	char name[CHARACTER_NAME_MAX_LEN+1];
	long x;
	long y;
};

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;

	// array of TNPCPosition
} TPacketGCNPCPosition;

typedef struct SPacketGCSpecialEffect
{
	BYTE header;
	BYTE type;
	DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketCGChangeName;

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCChangeName;

typedef struct command_client_version
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion;

typedef struct command_client_version2
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion2;

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
#ifdef ENABLE_ANTI_EXP
	bool bAntiExp;
#endif
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	DWORD   vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short	count;
#else
	BYTE	count;
#endif
	long    alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TEquipmentItemSet;

typedef struct pakcet_view_equip
{
	BYTE  header;
	DWORD vid;
	TEquipmentItemSet equips[WEAR_MAX_NUM];
} TPacketViewEquip;

typedef struct SLandPacketElement
{
	DWORD	dwID;
	long	x, y;
	long	width, height;
	DWORD	dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
	BYTE	header;
	WORD	size;
} TPacketGCLandList;

typedef struct SPacketGCTargetCreate
{
	BYTE	bHeader;
	long	lID;
	char	szName[32+1];
	DWORD	dwVID;
	BYTE	bType;
} TPacketGCTargetCreate;

typedef struct SPacketGCTargetUpdate
{
	BYTE	bHeader;
	long	lID;
	long	lX, lY;
} TPacketGCTargetUpdate;

typedef struct SPacketGCTargetDelete
{
	BYTE	bHeader;
	long	lID;
} TPacketGCTargetDelete;

#ifdef ENABLE_AFFECT_RENEWAL
typedef struct SPacketGCAffectElement
{
	DWORD dwType;
	BYTE bApplyOn;
	long lApplyValue;
	DWORD dwFlag;
	long lDuration;
	long lSPCost;
} TPacketGCAffectElement;
#endif

typedef struct SPacketGCAffectAdd
{
	BYTE		bHeader;
#ifdef ENABLE_AFFECT_RENEWAL
	TPacketGCAffectElement elem;
#else
	TPacketAffectElement elem;
#endif
} TPacketGCAffectAdd;

typedef struct SPacketGCAffectRemove
{
	BYTE	bHeader;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_lover_info
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE love_point;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE header;
	BYTE love_point;
} TPacketGCLovePointUpdate;

// MINING
typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;
// END_OF_MINING

// SCRIPT_SELECT_ITEM
typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;
// END_OF_SCRIPT_SELECT_ITEM

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int damage;
} TPacketGCDamageInfo;

typedef struct tag_GGSiege
{
	BYTE	bHeader;
	BYTE	bEmpire;
	BYTE	bTowerCount;
} TPacketGGSiege;

typedef struct SPacketGGMonarchTransfer
{
	BYTE	bHeader;
	DWORD	dwTargetPID;
	long	x;
	long	y;
} TPacketMonarchGGTransfer;

typedef struct SPacketGGCheckAwakeness
{
	BYTE bHeader;
} TPacketGGCheckAwakeness;

typedef struct SPacketGCPanamaPack
{
	BYTE	bHeader;
	char	szPackName[256];
	BYTE	abIV[32];
} TPacketGCPanamaPack;

typedef struct SPacketGCHybridCryptKeys
{
	SPacketGCHybridCryptKeys() : m_pStream(NULL) {}
	~SPacketGCHybridCryptKeys()
	{
		if( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + KeyStreamLen;
	}

	BYTE* GetStreamData()
	{
		if( m_pStream )
			delete[] m_pStream;

		uDynamicPacketSize = (WORD)GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream+1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream+3, &KeyStreamLen, 4 );

		if( KeyStreamLen > 0 )
			memcpy( m_pStream+7, pDataKeyStream, KeyStreamLen );

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		KeyStreamLen;
	BYTE*   pDataKeyStream;

private:
	BYTE* m_pStream;
} TPacketGCHybridCryptKeys;

typedef struct SPacketGCPackageSDB
{
	SPacketGCPackageSDB() : m_pDataSDBStream(NULL), m_pStream(NULL) {}
	~SPacketGCPackageSDB()
	{
		if( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + iStreamLen;
	}

	BYTE* GetStreamData()
	{
		if( m_pStream )
			delete[] m_pStream;

		uDynamicPacketSize =  GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream+1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream+3, &iStreamLen, 4 );

		if( iStreamLen > 0 )
			memcpy( m_pStream+7, m_pDataSDBStream, iStreamLen );

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		iStreamLen;
	BYTE*   m_pDataSDBStream;

private:
	BYTE* m_pStream;
} TPacketGCPackageSDB;

#ifdef _IMPROVED_PACKET_ENCRYPTION_
struct TPacketKeyAgreement
{
	static const int MAX_DATA_LEN = 256;
	BYTE bHeader;
	WORD wAgreedLength;
	WORD wDataLength;
	BYTE data[MAX_DATA_LEN];
};

struct TPacketKeyAgreementCompleted
{
	BYTE bHeader;
	BYTE data[3]; // dummy (not used)
};

#endif // _IMPROVED_PACKET_ENCRYPTION_

#define MAX_EFFECT_FILE_NAME 128
typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[MAX_EFFECT_FILE_NAME];
} TPacketGCSpecificEffect;

enum EDragonSoulRefineWindowRefineType
{
	DragonSoulRefineWindow_UPGRADE,
	DragonSoulRefineWindow_IMPROVEMENT,
	DragonSoulRefineWindow_REFINE,
};

enum EPacketCGDragonSoulSubHeaderType
{
	DS_SUB_HEADER_OPEN,
	DS_SUB_HEADER_CLOSE,
	DS_SUB_HEADER_DO_REFINE_GRADE,
#ifdef __ENABLE_DS_REFINE_ALL__
	DS_SUB_HEADER_DO_REFINE_GRADE_ALL,
#endif
	DS_SUB_HEADER_DO_REFINE_STEP,
	DS_SUB_HEADER_DO_REFINE_STRENGTH,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
};
typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DRAGON_SOUL_REFINE_GRID_SIZE];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SPacketCGStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
} TPacketCGStateCheck;

typedef struct SPacketGCStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
	unsigned char state;
} TPacketGCStateCheck;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
enum
{
	ACCE_SUBHEADER_GC_OPEN = 0,
	ACCE_SUBHEADER_GC_CLOSE,
	ACCE_SUBHEADER_GC_ADDED,
	ACCE_SUBHEADER_GC_REMOVED,
	ACCE_SUBHEADER_CG_REFINED,
	ACCE_SUBHEADER_CG_CLOSE = 0,
	ACCE_SUBHEADER_CG_ADD,
	ACCE_SUBHEADER_CG_REMOVE,
	ACCE_SUBHEADER_CG_REFINE,
};

typedef struct SPacketAcce
{
	BYTE	header;
	BYTE	subheader;
	bool	bWindow;
	DWORD	dwPrice;
	BYTE	bPos;
	TItemPos	tPos;
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TPacketAcce;
#endif

#ifdef ENABLE_SWITCHBOT_SYSTEM
struct TPacketGGSwitchbot
{
	BYTE bHeader;
	WORD wPort;
	TSwitchbotTable table;

	TPacketGGSwitchbot() : bHeader(HEADER_GG_SWITCHBOT), wPort(0)
	{
		table = {};
	}
};

enum ECGSwitchbotSubheader
{
	SUBHEADER_CG_SWITCHBOT_START,
	SUBHEADER_CG_SWITCHBOT_STOP,
};

struct TPacketCGSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

enum EGCSwitchbotSubheader
{
	SUBHEADER_GC_SWITCHBOT_UPDATE,
	SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM,
	SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION,
};

struct TPacketGCSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

struct TSwitchbotUpdateItem
{
	BYTE slot;
	BYTE vnum;
	BYTE count;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};
#endif

#ifdef ENABLE_DROP_INFO
typedef struct SPacketDropInfoItem
{
	DWORD dwVnum;
	BYTE byMinCount;
	BYTE byMaxCount;
} TPacketDropInfoItem;

typedef struct SPacketDropInfo
{
	BYTE header;
	WORD size;
	WORD item_count;
	DWORD gold_min;
	DWORD gold_max;
	DWORD mob_vnum;
} TPacketDropInfo;
#endif

#ifdef ENABLE_EVENT_MANAGER
typedef struct command_request_event_quest
{
	BYTE bHeader;
	char szName[QUEST_NAME_MAX_NUM + 1];
} TPacketCGRequestEventQuest;

typedef struct SPacketGGReloadEvent
{
	BYTE bHeader;
} TPacketGGReloadEvent;

typedef struct SPacketGGEvent
{
	BYTE bHeader;
	TEventTable table;
	bool bState;
} TPacketGGEvent;

typedef struct SPacketGGEventHideAndSeek
{
	BYTE bHeader;
	int iPosition;
	int iRound;
} TPacketGGEventHideAndSeek;

typedef struct SPacketGCEventKWScore
{
	BYTE bHeader;
	WORD wKingdomScores[3];
} TPacketGCEventKWScore;

typedef struct SPacketCGRequestEventData
{
	BYTE bHeader;
	BYTE bMonth;
} TPacketCGRequestEventData;

typedef struct SPacketGCEventInfo
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCEventInfo;

typedef struct SPacketGCEventReload
{
	BYTE bHeader;
} TPacketGCEventReload;

typedef struct SPacketEventData
{
	DWORD dwID;
	BYTE bType;
	long startTime;
	long endTime;
	int iValue0;
	int iValue1;
	bool bCompleted;
} TPacketEventData;
#endif

#ifdef ENABLE_GEM_SYSTEM
enum
{
	GEM_SHOP_SUBHEADER_CG_BUY,
};

typedef struct command_gem_shop
{
	BYTE header;
	BYTE subheader;
} TPacketCGGemShop;

typedef struct command_gem_shop_open
{
	BYTE header;
	TGemShopItem shopItems[GEM_SLOTS_MAX_NUM];
} TPacketGCGemShopOpen;
#endif

#ifdef ENABLE_SKILL_GROUP_GUI
typedef struct SPacketCGSkillGroup
{
	BYTE bHeader;
	BYTE bGroup;
} TPacketCGSkillGroup;

typedef struct SPacketGCSkillGroup
{
	BYTE bHeader;
	BYTE bJob;
} TPacketGCSkillGroup;
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
typedef struct SPacketCGBiologManagerAction
{
	BYTE bHeader;
	BYTE bSubHeader;
} TPacketCGBiologManagerAction;

typedef struct SPacketGCBiologManager
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bSubHeader;
} TPacketGCBiologManager;

typedef struct SPacketGCBiologManagerInfo
{
	bool	bUpdate;
	BYTE	bRequiredLevel;
	DWORD	iRequiredItem;
	WORD	wGivenItems;
	WORD	wRequiredItemCount;
	time_t	iGlobalCooldown;
	time_t	iCooldown;
	bool	iCooldownReminder;
	BYTE	bChance;
	DWORD	bApplyType[MAX_BONUSES_LENGTH];
	long	lApplyValue[MAX_BONUSES_LENGTH];
	DWORD	dRewardItem;
	WORD	wRewardItemCount;
} TPacketGCBiologManagerInfo;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
enum EPrivateShopGCSubheader
{
	SUBHEADER_GC_PRIVATE_SHOP_ADD_ENTITY,
	SUBHEADER_GC_PRIVATE_SHOP_DEL_ENTITY,
	SUBHEADER_GC_PRIVATE_SHOP_TITLE,
	SUBHEADER_GC_PRIVATE_SHOP_LOAD,
	SUBHEADER_GC_PRIVATE_SHOP_SET_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_SET_SALE,
	SUBHEADER_GC_PRIVATE_SHOP_BALANCE_UPDATE,
	SUBHEADER_GC_PRIVATE_SHOP_OPEN_PANEL,
	SUBHEADER_GC_PRIVATE_SHOP_CLOSE_PANEL,
	SUBHEADER_GC_PRIVATE_SHOP_CLOSE,
	SUBHEADER_GC_PRIVATE_SHOP_START,
	SUBHEADER_GC_PRIVATE_SHOP_END,
	SUBHEADER_GC_PRIVATE_SHOP_REMOVE_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_REMOVE_MY_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_ADD_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_STATE_UPDATE,
	SUBHEADER_GC_PRIVATE_SHOP_WITHDRAW,
	SUBHEADER_GC_PRIVATE_SHOP_ITEM_PRICE_CHANGE,
	SUBHEADER_GC_PRIVATE_SHOP_ITEM_MOVE,
	SUBHEADER_GC_PRIVATE_SHOP_TITLE_CHANGE,
	SUBHEADER_GC_PRIVATE_SHOP_UNLOCKED_SLOTS_CHANGE,

	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_LOOK_MODE,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_TRADE_MODE,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_RESULT,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_UPDATE,

	SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_RESULT,
	SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_RESULT,
};

enum EPrivateShopCGSubheader
{
	SUBHEADER_CG_PRIVATE_SHOP_BUILD,
	SUBHEADER_CG_PRIVATE_SHOP_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_START,
	SUBHEADER_CG_PRIVATE_SHOP_END,
	SUBHEADER_CG_PRIVATE_SHOP_BUY,
	SUBHEADER_CG_PRIVATE_SHOP_WITHDRAW,
	SUBHEADER_CG_PRIVATE_SHOP_MODIFY,
	SUBHEADER_CG_PRIVATE_SHOP_STATE_UPDATE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_PRICE_CHANGE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_MOVE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKIN,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKOUT,
	SUBHEADER_CG_PRIVATE_SHOP_TITLE_CHANGE,
	SUBHEADER_CG_PRIVATE_SHOP_WARP_REQUEST,
	SUBHEADER_CG_PRIVATE_SHOP_SLOT_UNLOCK_REQUEST,

	SUBHEADER_CG_PRIVATE_SHOP_SEARCH_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_SEARCH,
	SUBHEADER_CG_PRIVATE_SHOP_SEARCH_BUY,

	SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_REQUEST,
	SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_REQUEST,
};

typedef struct SPrivateShopItem
{
	TItemPos	TPos;
	TItemPrice	TPrice;
	WORD		wDisplayPos;
} TPrivateShopItem;

typedef struct SPrivateShopItemData
{
	DWORD					dwVnum;
	TItemPrice				TPrice;
	time_t					tCheckin;
	DWORD					dwCount;
	WORD					wPos;
	long					alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPrivateShopItemData;

typedef struct SPrivateShopSaleData
{
	char					szCustomer[CHARACTER_NAME_MAX_LEN + 1];
	time_t					tTime;
	TPrivateShopItemData	TItem;
} TPrivateShopSaleData;

typedef struct SPrivateShopSearchData
{
	DWORD					dwShopID;
	char					szOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD					dwVnum;
	TItemPrice				TPrice;
	DWORD					dwCount;
	WORD					wPos;
	long					alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	time_t					tCheckin;
	BYTE					bState;
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPrivateShopSearchData;

typedef struct SPacketCGPrivateShop
{
	BYTE	bHeader;
	BYTE	bSubHeader;
} TPacketCGPrivateShop;

typedef struct SPacketGCPrivateShopAddEntity
{
	long		lX;
	long		lY;
	long		lZ;
	DWORD		dwVID;
	DWORD		dwVnum;
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE		bTitleType;
	char		szTitle[TITLE_MAX_LEN + 1];
} TPacketGCPrivateShopAddEntity;

typedef struct SPacketGCPrivateShopDelEntity
{
	DWORD		dwVID;
} TPacketGCPrivateShopDelEntity;

typedef struct SPacketGCPrivateShopTitle
{
	DWORD		dwVID;
	char		szTitle[TITLE_MAX_LEN + 1];
	BYTE		bTitleType;
} TPacketGCPrivateShopTitle;

typedef struct SPacketCGPrivateShopBuild
{
	char	szTitle[TITLE_MAX_LEN + 1];
	DWORD	dwPolyVnum;
	BYTE	bTitleType;
	BYTE	bPageCount;
	WORD	wItemCount;
} TPacketCGPrivateShopBuild;

typedef struct SPacketCGPrivateShopItemPriceChange
{
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketCGPrivateShopItemPriceChange;

typedef struct SPacketCGPrivateShopItemMove
{
	WORD		wPos;
	WORD		wChangePos;
} TPacketCGPrivateShopItemMove;

typedef struct SPrivateShopSearchFilter
{
	DWORD	dwVnum;
	char	szOwnerName[CHARACTER_NAME_MAX_LEN + 1];

	int		iItemType;
	int		iItemSubType;

	int		iJob;
	int		iGender;

	WORD	wMinStack;
	WORD	wMaxStack;

	BYTE	bMinRefine;
	BYTE	bMaxRefine;

	DWORD	dwMinLevel;
	DWORD	dwMaxLevel;

	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	WORD					wSashAbsorption;
	BYTE					bAlchemyLevel;
	BYTE					bAlchemyClarity;
} TPrivateShopSearchFilter;

typedef struct SPacketCGPrivateShopSearch
{
	TPrivateShopSearchFilter	Filter;
	bool						bUseFilter;
} TPacketCGPrivateShopSearch;

typedef struct SPrivateShopSearchSelectedItem
{
	DWORD		dwShopID;
	WORD		wPos;
	TItemPrice	TPrice;
} SPrivateShopSearchSelectedItem;

typedef struct SPacketCGPrivateShopSearchBuy
{
	SPrivateShopSearchSelectedItem aSelectedItems[SELECTED_ITEM_MAX_NUM];
} TPacketCGPrivateShopSearchBuy;

typedef struct SPacketGCPrivateShopSearchUpdate
{
	DWORD	dwShopID;
	int		iSpecificItemPos;
	BYTE	bState;
} TPacketGCPrivateShopSearchUpdate;

typedef struct SPacketGCPrivateShop
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bSubHeader;
} TPacketGCPrivateShop;

typedef struct SPacketGCPrivateShopLoad
{
	char		szTitle[SHOP_SIGN_MAX_LEN + 1];
	long long	llGold;
	DWORD		dwCheque;
	long		lX;
	long		lY;
	BYTE		bChannel;
	BYTE		bState;
	BYTE		bPageCount;
} TPacketGCPrivateShopLoad;

typedef struct SPacketGCPrivateShopOpen
{
	char					szTitle[SHOP_SIGN_MAX_LEN + 1];
	BYTE					bState;
	BYTE					bPageCount;
	WORD					wUnlockedSlots;
	DWORD					dwVID;
	TPrivateShopItemData	aItems[PRIVATE_SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCPrivateShopOpen;

typedef struct SPacketGCPrivateStateUpdate
{
	BYTE	bState;
	bool	bIsMainPlayerPrivateShop;
} TPacketGCPrivateStateUpdate;

typedef struct SPacketGCPrivateShopItemPriceChange
{
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketGCPrivateShopItemPriceChange;

typedef struct SPacketGCPrivateShopItemMove
{
	WORD		wPos;
	WORD		wChangePos;
} TPacketGCPrivateShopItemMove;

typedef struct SPacketGCPrivateShopBalanceUpdate
{
	TItemPrice	TPrice;
} TPacketGCPrivateShopBalanceUpdate;

typedef struct SPacketCGPrivateShopItemCheckin
{
	TItemPos	TSrcPos;
	long long	llGold;
	DWORD		dwCheque;
	int			iDstPos;
} TPacketCGPrivateShopItemCheckin;

typedef struct SPacketCGPrivateShopItemCheckout
{
	WORD		wSrcPos;
	int			iDstPos;
} TPacketCGPrivateShopItemCheckout;

typedef struct SPacketGGPrivateShopItemRemove
{
	BYTE					bHeader;
	DWORD					dwShopID;
	WORD					wPos;
} TPacketGGPrivateShopItemRemove;

typedef struct SPacketGGPrivateShopItemSearch
{
	BYTE						bHeader;
	DWORD						dwCustomerID;
	DWORD						dwCustomerPort;
	bool						bUseFilter;
	TPrivateShopSearchFilter	Filter;
} TPacketGGPrivateShopItemSearch;

typedef struct SPacketGGPrivateShopItemSearchUpdate
{
	BYTE					bHeader;
	DWORD					dwShopID;
	int						iSpecificItemPos;
	BYTE					bState;
} TPacketGGPrivateShopItemSearchUpdate;

typedef struct SPacketGGPrivateShopItemSearchResult
{
	BYTE					bHeader;
	WORD					wSize;
	DWORD					dwCustomerID;
} TPacketGGPrivateShopItemSearchResult;
#endif

#if defined(ENABLE_REMOTE_SHOP)
typedef struct command_remote_shop
{
	BYTE	header;
	BYTE	shop_index;
} TPacketCGRemoteShop;
#endif

#ifdef ENABLE_GOLD_MAX_EXTENDED
typedef struct packet_gold
{
	BYTE header;
	unsigned long long gold;
} TPacketGCGold;

typedef struct packet_gold_change
{
	int header;
	DWORD dwVID;
	long long amount;
	unsigned long long value;
} TPacketGCGoldChange;
#endif

#ifdef ENABLE_ATLASS_EXTENDED
struct TBossPosition
{
	DWORD dwTime;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	long x;
	long y;
};

typedef struct SPacketGCBossPosition
{
	BYTE header;
	WORD size;
	WORD count;
} TPacketGCBossPosition;
#endif

#ifdef ENABLE_HWID_BAN
typedef struct SPacketCGHwidBan
{
	BYTE header;
	BYTE bMode;
	char szPlayer[CHARACTER_NAME_MAX_LEN + 1];
	char szReason[HWID_BAN_REASON_MAX_LEN + 1];
} TPacketCGHwidBan;
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
typedef struct SPacketGCMaintenanceInfo
{
	BYTE	header;
	WORD	size;
	DWORD	remaining_time;
	BYTE	cause_len;
	DWORD	duration;
} TPacketGCMaintenanceInfo;

typedef struct SServerMaintenanceInfo
{
	SServerMaintenanceInfo()
	{
		shutdown_time = 0;
		cause[0] = '\0';
		duration = 0;
	}

	DWORD	shutdown_time;
	char	cause[100];
	DWORD	duration;
} TServerMaintenanceInfo;

typedef struct SPacketGGMaintenanceInfo
{
	BYTE	header;
	TServerMaintenanceInfo	info;
} TPacketGGMaintenanceInfo;
#endif


#ifdef ENABLE_HUNTING_SYSTEM
typedef struct SPacketCGHuntingAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue;
} TPacketGCHuntingAction;

typedef struct SPacketCGOpenWindowHuntingMain
{
	BYTE	bHeader;
	DWORD	dLevel;
	DWORD	dMonster;
	DWORD	dCurCount;
	DWORD	dDestCount;
	DWORD	dMoneyMin;
	DWORD	dMoneyMax;
	DWORD	dExpMin;
	DWORD	dExpMax;
	DWORD	dRaceItem;
	DWORD	dRaceItemCount;
} TPacketGCOpenWindowHuntingMain;

typedef struct SPacketCGOpenWindowHuntingSelect
{
	BYTE	bHeader;
	DWORD	dLevel;
	BYTE	bType;
	DWORD	dMonster;
	DWORD	dCount;
	DWORD	dMoneyMin;
	DWORD	dMoneyMax;
	DWORD	dExpMin;
	DWORD	dExpMax;
	DWORD	dRaceItem;
	DWORD	dRaceItemCount;
} TPacketGCOpenWindowHuntingSelect;

typedef struct SPacketGCOpenWindowReward
{
	BYTE	bHeader;
	DWORD	dLevel;
	DWORD	dReward;
	DWORD	dRewardCount;
	DWORD	dRandomReward;
	DWORD	dRandomRewardCount;
	DWORD	dMoney;
	BYTE	bExp;
} TPacketGCOpenWindowReward;

typedef struct SPacketGCUpdateHunting
{
	BYTE	bHeader;
	DWORD	dCount;
} TPacketGCUpdateHunting;

typedef struct SPacketGCReciveRandomItems
{
	BYTE	bHeader;
	BYTE	bWindow;
	DWORD	dItemVnum;
	DWORD	dItemCount;
} TPacketGCReciveRandomItems;
#endif

#if defined(__BL_67_ATTR__)
typedef struct command_67_attr
{
	BYTE			bHeader;
	BYTE			bMaterialCount;
	BYTE			bSupportCount;
	short			sSupportPos;
	short			sItemPos;
} TPacketCG67Attr;

typedef struct command_67_attr_open_close
{
	BYTE			bHeader;
} TPacket67AttrOpenClose;
#endif

#ifdef ENABLE_DUNGEON_INFO_SYSTEM
enum ESubHeader
{
	SUBHEADER_DUNGEON_INFO_SEND,
	SUBHEADER_DUNGEON_INFO_OPEN,
};

typedef struct SPacketCGDungeonInfo
{
	uint8_t byHeader;
	uint8_t bySubHeader;
	uint8_t byIndex;
	uint8_t byRankType;
} TPacketCGDungeonInfo;

struct LevelLimit { int iLevelMin, iLevelMax, iMemberMin, iMemberMax; };
struct Item { uint32_t dwVnum; uint16_t wCount; };
struct Bonus { uint16_t byAttBonus[POINT_MAX_NUM]; uint16_t byDefBonus[POINT_MAX_NUM]; };
struct Results { uint32_t dwFinish, dwFinishTime, dwFinishDamage; };

typedef struct SPacketGCDungeonInfo
{
	SPacketGCDungeonInfo() : byHeader(HEADER_GC_DUNGEON_INFO), bySubHeader(0), byIndex(0), byType(0),
		bReset(false),
		lMapIndex(0), lEntryMapIndex(0),
		dwBossVnum(0),
		dwDuration(0), dwCooldown(0), byElement(0)
	{
		memset(&sLevelLimit, 0, sizeof(sLevelLimit));
		memset(&sRequiredItem, 0, sizeof(sRequiredItem));
		memset(&sBonus.byAttBonus, 0, sizeof(sBonus.byAttBonus));
		memset(&sBonus.byDefBonus, 0, sizeof(sBonus.byDefBonus));
		memset(&sBossDropItem, 0, sizeof(sBossDropItem));
		memset(&sResults, 0, sizeof(sResults));
	}

	uint8_t byHeader;
	uint8_t bySubHeader;
	uint16_t byIndex;
	uint16_t byType;
	bool bReset;
	long lMapIndex;
	long lEntryMapIndex;
	uint32_t dwBossVnum;
	LevelLimit sLevelLimit;
	Item sRequiredItem[EDungeonInfoData::MAX_REQUIRED_ITEMS];
	uint32_t dwDuration;
	uint32_t dwCooldown;
	uint8_t byElement;
	Bonus sBonus;
	Item sBossDropItem[EDungeonInfoData::MAX_BOSS_ITEM_SLOTS];
	Results sResults;
} TPacketGCDungeonInfo;

typedef struct SPacketGCDungeonInfoRank
{
	SPacketGCDungeonInfoRank() { strncpy(szName, "", sizeof(szName)); }
	SPacketGCDungeonInfoRank(const char* c_szName, const int c_iLevel, const uint32_t c_dwPoints) :
		iLevel(c_iLevel), dwPoints(c_dwPoints) 
	{
		strncpy(szName, c_szName, sizeof(szName));
	}

	uint8_t byHeader = HEADER_GC_DUNGEON_RANKING;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	int iLevel = 0;
	uint32_t dwPoints = 0;
} TPacketGCDungeonInfoRank;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
typedef struct packet_skill_color
{
	BYTE		bheader;
	BYTE		skill;
	DWORD		col1;
	DWORD		col2;
	DWORD		col3;
	DWORD		col4;
	DWORD		col5;
} TPacketCGSkillColor;
#endif

#ifdef ENABLE_GLOBAL_REWARD
typedef struct SPacketGGRewardInfo
{
	BYTE	bHeader;
	BYTE	bType;
} TPacketGGRewardInfo;
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
typedef struct SPacketCGChangeCostumeVisibilityStatus
{
	BYTE bHeader;
	BYTE type;
	bool status;
} TPacketCGChangeCostumeVisibilityStatus;

typedef struct SPacketGCUpdateCostumeVisibilityStatus
{
	BYTE bHeader;
	bool type[ECostumeHideParts::HIDE_COSTUME_COUNT];
} TPacketGCUpdateCostumeVisibilityStatus;
#endif

#ifdef ENABLE_ANTI_MULTIPLE_FARM
enum EAntiFarmSubHeader
{
	AF_SH_SENDING_DATA,
	AF_SH_SEND_STATUS_UPDATE,
	AF_SH_SENDING_CONFIRM_DATA,
};

typedef struct SAntiFarmPlayerInfo
{
	SAntiFarmPlayerInfo(DWORD dwPID, bool bDropStatus) :
		dwPID(dwPID), bDropStatus(bDropStatus)
		{ memset(szName, 0, sizeof(szName)); }
	
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD	dwPID;
	bool	bDropStatus;
} TAntiFarmPlayerInfo;

typedef struct SSendAntiFarmInfo
{
	SSendAntiFarmInfo(BYTE header, int size, BYTE subheader) : 
		header(header), size(size), subheader(subheader)
		{}
	
	BYTE	header;
	int		size;
	BYTE	subheader;
} TSendAntiFarmInfo;
#endif

#ifdef ENABLE_SPLIT_BY_COUNT
typedef struct SSplitItemByCount
{
	SSplitItemByCount(BYTE bHeader, TItemPos Cell, short sCount) : 
		bHeader(bHeader), Cell(Cell), sCount(sCount)
		{}
	
	BYTE bHeader;
	TItemPos Cell;
	short sCount;
} TSplitItemByCount;
#endif

#ifdef ENABLE_RANK_PLAYER
typedef struct SPacketGGPlayerRank
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	bEmpire;
	long long lValue;
	int		iMode;
	DWORD	dwPid;
} TPacketGGPlayerRank;

typedef struct SGCRankInfo
{
	BYTE	bHeader;
	char 	szName[CHARACTER_NAME_MAX_LEN + 1];
	int		iMode, iPos, bEmpire, iMyPos;
	long long	lValue;
} TPacketGCRankInfo;
#endif

#ifdef __RANKING_SYSTEM__
typedef struct SPacketGGPlayerRank
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	bEmpire;
	long long lValue;
	int		iMode;
	DWORD	dwPid;
	char	szAccount[LOGIN_MAX_LEN + 1];
	long	lSize;
} TPacketGGPlayerRank;

typedef struct SGCRankInfo
{
	BYTE	bHeader;
	char 	szName[CHARACTER_NAME_MAX_LEN + 1];
	int		iMode, iPos, bEmpire, iMyPos;
	long long	lValue;
} TPacketGCRankInfo;
#endif

#ifdef ENABLE_BATTLE_PASS
typedef struct SPacketGCBattlePass
{
	BYTE	bHeader;
	WORD	wSize;
	WORD	wRewardSize;
} TPacketGCBattlePass;

typedef struct SPacketGCBattlePassUpdate
{
	BYTE	bHeader;
	BYTE	bMissionType;
	DWORD	dwNewProgress;
} TPacketGCBattlePassUpdate;

typedef struct SPacketCGBattlePassAction
{
	BYTE	bHeader;
	BYTE	bAction;
} TPacketCGBattlePassAction;

typedef struct SPacketGCBattlePassRanking
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bIsGlobal;
} TPacketGCBattlePassRanking;
#endif

#ifdef ENABLE_ITEMSHOP
typedef struct SPacketGCitemshopCategorySize
{
	DWORD size;
} TPacketGCitemshopCategorySize;

typedef struct SPacketGCItemshopItemSize
{
	DWORD size;
} TPacketGCItemshopItemSize;

typedef struct SPacketGCItemshopInfo
{
	BYTE header;
	WORD size;
	DWORD subheader;
} TPacketGCItemshopInfo;

enum
{
	SUBHEADER_ITEMSHOP_REFRESH_ITEMS,
	SUBHEADER_ITEMSHOP_REFRESH_COINS,
	SUBHEADER_ITEMSHOP_REFRESH_SINGLE_ITEM,
	SUBHEADER_ITEMSHOP_REMOVE_SINGLE_ITEM,
	SUBHEADER_ITEMSHOP_ADD_SINGLE_ITEM,
	SUBHEADER_PROMOTION_CODE_REWARDS,
};

typedef struct SPacketCGBuyItemshopItem
{
	BYTE	header;
	char	hash[ITEMSHOP_HASH_MAX_LEN + 1];
	WORD	wCount;
} TPacketCGBuyItemshopItem;

typedef struct SPacketCGRedeemPromotionCode
{
	BYTE	header;
	char	promotion_code[PROMOTION_CODE_MAX_LEN + 1];
} TPacketCGRedeemPromotionCode;
#endif

#ifdef ENABLE_QUICK_OPEN
typedef struct SPacketGCInventoryHeader
{
	BYTE bHeader;
#ifdef ENABLE_LARGE_DYNAMIC_PACKET
	int wSize;
#else
	WORD wSize;
#endif
} TPacketGCInventoryHeader;
#endif

#ifdef ENABLE_FAST_STACK
typedef struct packet_fast_stack
{
	BYTE header;
	TItemPos pos;
} TPacketCGFastStack;
#endif

typedef struct SPacketGCDragonPoints
{
	BYTE byHeader;
	DWORD dwCash;
	DWORD dwCoins;
} TPacketGCDragonPoints;

typedef struct SPacketGCDragonPointsChange
{
	BYTE byHeader;
	DWORD dwVID;
	DWORD dwCash;
	DWORD dwCoins;
} TPacketGCDragonPointsChange;

#ifdef __GUILD_SAFEBOX__
enum EGuildSafeboxSubHeader
{
	GUILD_SAFEBOX_SUBHEADER_SET_ITEM = 1,
	GUILD_SAFEBOX_SUBHEADER_REMOVE_ITEM = 2,
	GUILD_SAFEBOX_SUBHEADER_GOLD = 3,
	GUILD_SAFEBOX_SUBHEADER_OPEN = 4,
	GUILD_SAFEBOX_SUBHEADER_CLOSE = 5,
};

typedef struct guild_safebox_item
{
	DWORD vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short count;
#else
	BYTE count;
#endif
	BYTE pos;
	long sockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute attr[ITEM_ATTRIBUTE_MAX_NUM];
} TGuildSafeboxItem;

typedef struct packet_guild_safebox
{
	BYTE	header;
	WORD	size;
	BYTE	subheader;
} TPacketGCGuildSafebox;

typedef struct command_guild_safebox_checkout
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGGuildSafeboxCheckout;

typedef struct command_guild_safebox_checkin
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGGuildSafeboxCheckin;

typedef struct packet_cg_guild_safebox_gold
{
	BYTE	header;
	DWORD	gold;
} TPacketCGGuildSafeboxGold;
#endif

#ifdef ENABLE_EVENT_CALENDAR
typedef struct SPacketGCEventManager
{
	uint8_t	header;
	uint32_t	size;
} TPacketGCEventManager;
#endif

#ifdef ENABLE_REWARD_SYSTEM
typedef struct reward_infos
{
	char lc_text[255];
	char playerName[CHARACTER_NAME_MAX_LEN+1];
	std::vector<std::pair<DWORD, DWORD>> m_rewardItems;
}TRewardInfo;

enum
{
	REWARD_120 = 1,
	REWARD_PET_115,
	REWARD_LEGENDARY_SKILL,
	REWARD_LEGENDARY_SKILL_SET,
	REWARD_HYDRA,
	REWARD_MELEY,
	REWARD_AKZADUR,
	REWARD_EKZEKIEL,
	REWARD_AVERAGE,
	REWARD_INVEN_SLOT,
	REWARD_OFFLINE_SLOT,
	REWARD_BATTLEPASS,
	REWARD_ACCE,
	REWARD_LEADERSHIP,
	REWARD_CRYSTAL,
	REWARD_BUFFI,
	REWARD_BIOLOG,
	REWARD_ELEMENT_PAGE,
	REWARD_FIRST_WEAPON_ZODIAC_CRAFT,
	FIRST_AVERAGE_ZODIAC_50,
	REWARD_MAX,
};

typedef struct SPacketGGRewardInfo
{
	BYTE	bHeader;
	BYTE	rewardIndex;
	char	playerName[CHARACTER_NAME_MAX_LEN+1];
} TPacketGGRewardInfo;
#endif

#ifdef ENABLE_TRASH_SYSTEM
typedef struct STrashPacketCG
{
	BYTE	header;
	uint8_t	items_count;
} TTrashPacketCG;

typedef struct STrashPacketGC
{
	BYTE	header;
	WORD	size;
} TTrashPacketGC;

typedef struct STrashItemInfo
{
	uint8_t		slot;
	TItemPos	item_pos;
} TTrashItemInfo;
#endif

#ifdef ENABLE_CHEST_OPEN_RENEWAL
typedef struct OpenChestPacket
{
	BYTE header;
	TItemPos pos;
	WORD wOpenCount;
} TPacketCGOpenChest;
#endif

#ifdef ENABLE_TELEPORT_GUILD
typedef struct SPacketGGGuildTeleport
{
	BYTE	bHeader;
	DWORD	dwGuild;
	DWORD	dwLeaderPID;
	long	lX;
	long	lY;
	long	lMapIndex;
	long	wPort;
} TPacketGGGuildTeleport;
#endif

#ifdef __DAILY_QUESTS__
typedef struct SPacketGCSendDailyQuests
{
	uint8_t header;
	uint16_t size;
	uint16_t count;
} TPacketGCSendDailyQuests;

typedef struct SPacketGCSendDailyQuest
{
	uint32_t id;
	uint32_t type;
	uint32_t vnum;
	uint32_t value;
	uint32_t amount;
	uint32_t progress;
} TPacketGCSendDailyQuest;

typedef struct SPacketGCUpdateDailyQuest
{
	uint8_t header;
	uint32_t id;
	uint32_t progress;
} TPacketGCUpdateDailyQuest;

typedef struct SPacketGCSendDailyRewards
{
	uint8_t header;
	uint16_t size;
	uint16_t count;
} TPacketGCSendDailyRewards;

typedef struct SPacketGCSendDailyReward
{
	uint32_t id;
	uint32_t rewardVnum[6];
	uint32_t rewardCount[6];
	uint32_t progress;
} TPacketGCSendDailyReward;

typedef struct SPacketGCUpdateDailyReward
{
	uint8_t header;
	uint32_t progress;
} TPacketGCUpdateDailyReward;
#endif

#pragma pack()
#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
