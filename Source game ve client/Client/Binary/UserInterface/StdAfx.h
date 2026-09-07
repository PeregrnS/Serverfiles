#pragma once

#pragma warning(disable:4702)
#pragma warning(disable:4100)
#pragma warning(disable:4201)
#pragma warning(disable:4511)
#pragma warning(disable:4663)
#pragma warning(disable:4018)
#pragma warning(disable:4245)

#if _MSC_VER >= 1400
//if don't use below, time_t is 64bit
#define _USE_32BIT_TIME_T
#endif
#include <iterator>
#include "../eterLib/StdAfx.h"
#include "../eterPythonLib/StdAfx.h"
#include "../gameLib/StdAfx.h"
#include "../scriptLib/StdAfx.h"
#include "../milesLib/StdAfx.h"
#include "../EffectLib/StdAfx.h"
#include "../PRTerrainLib/StdAfx.h"
#include "../SpeedTreeLib/StdAfx.h"

#ifndef __D3DRM_H__
#define __D3DRM_H__
#endif

#include <dshow.h>
#include <qedit.h>

#include "Locale.h"

#include "GameType.h"
extern DWORD __DEFAULT_CODE_PAGE__;

#define APP_NAME	"Metin 2"
#ifdef ENABLE_ANTI_DEBUG_METHOD
#define APP_PROCESS "Hardcore.exe"
#endif

enum
{
	POINT_MAX_NUM = 255,
#ifdef ENABLE_LONGER_MONSTER_NAME
	CHARACTER_NAME_MAX_LEN = 60,
#else
	CHARACTER_NAME_MAX_LEN = 24,
#endif
#if defined(LOCALE_SERVICE_JAPAN)
	PLAYER_NAME_MAX_LEN = 16,
#else
	PLAYER_NAME_MAX_LEN = 12,
#endif
#ifdef ENABLE_GEM_SYSTEM
	GEM_SLOTS_MAX_NUM = 9 * 5, // same value @ common/length.h
#endif
#ifdef ENABLE_HWID_BAN
	HWID_BAN_REASON_MAX_LEN = 512,
#endif
};

#ifdef ENABLE_ANTI_DEBUG_METHOD
void AntiDebugScanProcess();
bool vKillProcess(const char* filename);
void initThreadAntiDebug();
void vScan_AntiDebug_thread();
#endif

#ifdef ENABLE_ANTI_CHEAT_METHOD
BOOL bListProcModules(DWORD dwPID); // AntiCheat
void vScan_thread();
bool bKillProc(const char* filename);
void vAntiCheat();
void initCheat(); //AntiCheat
void initThreadPack();
#endif

void initudp();
void initapp();
void initime();
void initsystemSetting();
void initchr();
void initchrmgr();
void initChat();
void initTextTail();
void initime();
void initItem();
void initNonPlayer();
void initnet();
void initPlayer();
void initSectionDisplayer();
void initServerStateChecker();
void initTrade();
void initMiniMap();
void initProfiler();
void initEvent();
void initeffect();
void initsnd();
void initeventmgr();
void initBackground();
void initwndMgr();
void initshop();
void initpack();
void initskill();
void initfly();
void initquest();
void initsafebox();
void initguild();
void initguildstorage();
void initMessenger();
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	void initAcce();
#endif
#ifdef ENABLE_RENDER_TARGET
	void initRenderTarget();
#endif
#if defined(ENABLE_LOADING_PERFORMANCE)
	void initLoading();
#endif
#ifdef ENABLE_SWITCHBOT_SYSTEM
	void initSwitchbot();
#endif
#ifdef ENABLE_INGAME_WIKI
	void initWiki();
#endif
#ifdef ENABLE_BIOLOG_SYSTEM
	void initBiologManager();
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	void initPrivateShop();
#endif
#ifdef ENABLE_DUNGEON_INFO_SYSTEM
	void initDungeonInfo();
#endif
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	void initAntiMultipleFarmMethods();
#endif
#ifdef ENABLE_TRASH_SYSTEM
	void initTrash();
#endif

extern const std::string& ApplicationStringTable_GetString(DWORD dwID);
extern const std::string& ApplicationStringTable_GetString(DWORD dwID, LPCSTR szKey);

extern const char* ApplicationStringTable_GetStringz(DWORD dwID);
extern const char* ApplicationStringTable_GetStringz(DWORD dwID, LPCSTR szKey);

extern void ApplicationSetErrorString(const char* szErrorString);

#ifdef ENABLE_VOTE_4_BUFF
extern std::string httpGet(const std::string& url);
#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
