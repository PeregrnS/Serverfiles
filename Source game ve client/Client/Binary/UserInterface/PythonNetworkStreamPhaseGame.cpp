#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "FratelloSecurity.h"
#include "Packet.h"

#include "PythonGuild.h"
#include "PythonCharacterManager.h"
#include "PythonPlayer.h"
#include "PythonBackground.h"
#include "PythonMiniMap.h"
#include "PythonTextTail.h"
#include "PythonItem.h"
#include "PythonChat.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonQuest.h"
#include "PythonEventManager.h"
#include "PythonMessenger.h"
#include "PythonApplication.h"
#include "PythonGuildStorage.h"
#include "../EterPack/EterPackManager.h"
#include "../gamelib/ItemManager.h"
#ifdef ENABLE_INGAME_WIKI
	#include "../gamelib/inGameWiki.h"
#endif

#include "AbstractApplication.h"
#include "AbstractCharacterManager.h"
#include "InstanceBase.h"
#include "../EterLib/GrpDevice.h"

#include "ProcessCRC.h"

#ifdef ENABLE_DUNGEON_INFO_SYSTEM
	#include "PythonDungeonInfo.h"
#endif

#ifdef ENABLE_ANTI_MULTIPLE_FARM
	#include "HAntiMultipleFarm.h"
#endif
#ifdef ENABLE_TRASH_SYSTEM
	#include "CTrash.h"
#endif

BOOL gs_bEmpireLanuageEnable = TRUE;

void CPythonNetworkStream::__RefreshAlignmentWindow()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshAlignment", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshTargetBoardByVID(DWORD dwVID)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByVID", Py_BuildValue("(i)", dwVID));
}

void CPythonNetworkStream::__RefreshTargetBoardByName(const char * c_szName)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByName", Py_BuildValue("(s)", c_szName));
}

void CPythonNetworkStream::__RefreshTargetBoard()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoard", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshGuildWindowGradePage()
{
	m_isRefreshGuildWndGradePage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowSkillPage()
{
	m_isRefreshGuildWndSkillPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPageGradeComboBox()
{
	m_isRefreshGuildWndMemberPageGradeComboBox=true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPage()
{
	m_isRefreshGuildWndMemberPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowBoardPage()
{
	m_isRefreshGuildWndBoardPage=true;
}

void CPythonNetworkStream::__RefreshGuildWindowInfoPage()
{
	m_isRefreshGuildWndInfoPage=true;
}

void CPythonNetworkStream::__RefreshMessengerWindow()
{
	m_isRefreshMessengerWnd=true;
}

void CPythonNetworkStream::__RefreshSafeboxWindow()
{
	m_isRefreshSafeboxWnd=true;
}

void CPythonNetworkStream::__RefreshMallWindow()
{
	m_isRefreshMallWnd=true;
}

#ifdef ENABLE_GUILD_SAFEBOX
void CPythonNetworkStream::__RefreshGuildSafeboxWindow()
{
	m_isRefreshGuildSafeboxWnd=true;
}
#endif

void CPythonNetworkStream::__RefreshSkillWindow()
{
	m_isRefreshSkillWnd=true;
}

void CPythonNetworkStream::__RefreshExchangeWindow()
{
	m_isRefreshExchangeWnd=true;
}

void CPythonNetworkStream::__RefreshStatus()
{
	m_isRefreshStatus=true;
}

void CPythonNetworkStream::__RefreshCharacterWindow()
{
	m_isRefreshCharacterWnd=true;
}

void CPythonNetworkStream::__RefreshInventoryWindow()
{
	m_isRefreshInventoryWnd=true;
}

void CPythonNetworkStream::__RefreshEquipmentWindow()
{
	m_isRefreshEquipmentWnd=true;
}

void CPythonNetworkStream::__SetGuildID(DWORD id)
{
	if (m_dwGuildID != id)
	{
		m_dwGuildID = id;
		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

		for (int i = 0; i < PLAYER_PER_ACCOUNT4; ++i)
			if (!strncmp(m_akSimplePlayerInfo[i].szName, rkPlayer.GetName(), CHARACTER_NAME_MAX_LEN))
			{
				m_adwGuildID[i] = id;

				std::string  guildName;
				if (CPythonGuild::Instance().GetGuildName(id, &guildName))
				{
					m_astrGuildName[i] = guildName;
				}
				else
				{
					m_astrGuildName[i] = "";
				}
			}
	}
}

struct PERF_PacketInfo
{
	DWORD dwCount;
	DWORD dwTime;

	PERF_PacketInfo()
	{
		dwCount=0;
		dwTime=0;
	}
};

#ifdef __PERFORMANCE_CHECK__

class PERF_PacketTimeAnalyzer
{
	public:
		~PERF_PacketTimeAnalyzer()
		{
			FILE* fp=fopen("perf_dispatch_packet_result.txt", "w");

			for (std::map<DWORD, PERF_PacketInfo>::iterator i=m_kMap_kPacketInfo.begin(); i!=m_kMap_kPacketInfo.end(); ++i)
			{
				if (i->second.dwTime>0)
					fprintf(fp, "header %d: count %d, time %d, tpc %d\n", i->first, i->second.dwCount, i->second.dwTime, i->second.dwTime/i->second.dwCount);
			}
			fclose(fp);
		}

	public:
		std::map<DWORD, PERF_PacketInfo> m_kMap_kPacketInfo;
};

PERF_PacketTimeAnalyzer gs_kPacketTimeAnalyzer;

#endif

// Game Phase ---------------------------------------------------------------------------
void CPythonNetworkStream::GamePhase()
{
	if (!m_kQue_stHack.empty())
	{
		__SendHack(m_kQue_stHack.front().c_str());
		m_kQue_stHack.pop_front();
	}

	TPacketHeader header = 0;
	bool ret = true;

#ifdef __PERFORMANCE_CHECK__
	DWORD timeBeginDispatch=timeGetTime();

	static std::map<DWORD, PERF_PacketInfo> kMap_kPacketInfo;
	kMap_kPacketInfo.clear();
#endif

	const DWORD MAX_RECV_COUNT = 16*2; // @warme669
	const DWORD SAFE_RECV_BUFSIZE = 8192*2;
	DWORD dwRecvCount = 0;

    while (ret)
	{
		if(dwRecvCount++ >= MAX_RECV_COUNT-1 && GetRecvBufferSize() < SAFE_RECV_BUFSIZE
			&& m_strPhase == "Game")
			break;

		if (!CheckPacket(&header))
			break;

#ifdef __PERFORMANCE_CHECK__
		DWORD timeBeginPacket=timeGetTime();
#endif

#if defined(_DEBUG) && defined(ENABLE_PRINT_RECV_PACKET_DEBUG)
		Tracenf("RECV HEADER : %u , phase %s ", header, m_strPhase.c_str());
#endif

		switch (header)
		{
			case HEADER_GC_OBSERVER_ADD:
				ret = RecvObserverAddPacket();
				break;
			case HEADER_GC_OBSERVER_REMOVE:
				ret = RecvObserverRemovePacket();
				break;
			case HEADER_GC_OBSERVER_MOVE:
				ret = RecvObserverMovePacket();
				break;
			case HEADER_GC_WARP:
				ret = RecvWarpPacket();
				break;

			case HEADER_GC_PHASE:
				ret = RecvPhasePacket();
				return;
				break;

			case HEADER_GC_PVP:
				ret = RecvPVPPacket();
				break;

			case HEADER_GC_DUEL_START:
				ret = RecvDuelStartPacket();
				break;

			case HEADER_GC_CHARACTER_ADD:
 				ret = RecvCharacterAppendPacket();
				break;

			case HEADER_GC_CHAR_ADDITIONAL_INFO:
				ret = RecvCharacterAdditionalInfo();
				break;

			case HEADER_GC_CHARACTER_ADD2:
				ret = RecvCharacterAppendPacketNew();
				break;

			case HEADER_GC_CHARACTER_UPDATE:
				ret = RecvCharacterUpdatePacket();
				break;

			case HEADER_GC_CHARACTER_DEL:
				ret = RecvCharacterDeletePacket();
				break;

			case HEADER_GC_CHAT:
				ret = RecvChatPacket();
				break;

			case HEADER_GC_SYNC_POSITION:
				ret = RecvSyncPositionPacket();
				break;

			case HEADER_GC_OWNERSHIP:
				ret = RecvOwnerShipPacket();
				break;

			case HEADER_GC_WHISPER:
				ret = RecvWhisperPacket();
				break;

			case HEADER_GC_CHARACTER_MOVE:
				ret = RecvCharacterMovePacket();
				break;

			// Position
			case HEADER_GC_CHARACTER_POSITION:
				ret = RecvCharacterPositionPacket();
				break;

			// Battle Packet
			case HEADER_GC_STUN:
				ret = RecvStunPacket();
				break;

			case HEADER_GC_DEAD:
				ret = RecvDeadPacket();
				break;

			case HEADER_GC_PLAYER_POINT_CHANGE:
				ret = RecvPointChange();
				break;

			case HEADER_GC_CHARACTER_DRAGON_POINTS_CHANGE:
				if (RecvDragonPointsChange())
				{
					return;
				}
				break;

			// item packet.
			case HEADER_GC_ITEM_SET:
				ret = RecvItemSetPacket();
				break;

			case HEADER_GC_ITEM_SET2:
				ret = RecvItemSetPacket2();
				break;

			case HEADER_GC_ITEM_USE:
				ret = RecvItemUsePacket();
				break;

			case HEADER_GC_ITEM_UPDATE:
				ret = RecvItemUpdatePacket();
				break;

			case HEADER_GC_ITEM_GROUND_ADD:
				ret = RecvItemGroundAddPacket();
				break;

			case HEADER_GC_ITEM_GROUND_DEL:
				ret = RecvItemGroundDelPacket();
				break;

			case HEADER_GC_ITEM_OWNERSHIP:
				ret = RecvItemOwnership();
				break;

			case HEADER_GC_QUICKSLOT_ADD:
				ret = RecvQuickSlotAddPacket();
				break;

			case HEADER_GC_QUICKSLOT_DEL:
				ret = RecvQuickSlotDelPacket();
				break;

			case HEADER_GC_QUICKSLOT_SWAP:
				ret = RecvQuickSlotMovePacket();
				break;

			case HEADER_GC_MOTION:
				ret = RecvMotionPacket();
				break;

			case HEADER_GC_SHOP:
				ret = RecvShopPacket();
				break;

			case HEADER_GC_SHOP_SIGN:
				ret = RecvShopSignPacket();
				break;

			case HEADER_GC_EXCHANGE:
				ret = RecvExchangePacket();
				break;

			case HEADER_GC_QUEST_INFO:
				ret = RecvQuestInfoPacket();
				break;

			case HEADER_GC_REQUEST_MAKE_GUILD:
				ret = RecvRequestMakeGuild();
				break;

			case HEADER_GC_PING:
				ret = RecvPingPacket();
				break;

			case HEADER_GC_SCRIPT:
				ret = RecvScriptPacket();
				break;

			case HEADER_GC_QUEST_CONFIRM:
				ret = RecvQuestConfirmPacket();
				break;

			case HEADER_GC_TARGET:
				ret = RecvTargetPacket();
				break;

			case HEADER_GC_DAMAGE_INFO:
				ret = RecvDamageInfoPacket();
				break;

			case HEADER_GC_MOUNT:
				ret = RecvMountPacket();
				break;

			case HEADER_GC_CHANGE_SPEED:
				ret = RecvChangeSpeedPacket();
				break;

			case HEADER_GC_PLAYER_POINTS:
				ret = __RecvPlayerPoints();
				break;

			case HEADER_GC_CHARACTER_DRAGON_POINTS:
				ret = __RecvPlayerDragonPoints();
				break;

			case HEADER_GC_CREATE_FLY:
				ret = RecvCreateFlyPacket();
				break;

			case HEADER_GC_FLY_TARGETING:
				ret = RecvFlyTargetingPacket();
				break;

			case HEADER_GC_ADD_FLY_TARGETING:
				ret = RecvAddFlyTargetingPacket();
				break;

			case HEADER_GC_SKILL_LEVEL:
				ret = RecvSkillLevel();
				break;

			case HEADER_GC_SKILL_LEVEL_NEW:
				ret = RecvSkillLevelNew();
				break;

			case HEADER_GC_MESSENGER:
				ret = RecvMessenger();
				break;

			case HEADER_GC_GUILD:
				ret = RecvGuild();
				break;

			case HEADER_GC_PARTY_INVITE:
				ret = RecvPartyInvite();
				break;

			case HEADER_GC_PARTY_ADD:
				ret = RecvPartyAdd();
				break;

			case HEADER_GC_PARTY_UPDATE:
				ret = RecvPartyUpdate();
				break;

			case HEADER_GC_PARTY_REMOVE:
				ret = RecvPartyRemove();
				break;

			case HEADER_GC_PARTY_LINK:
				ret = RecvPartyLink();
				break;

			case HEADER_GC_PARTY_UNLINK:
				ret = RecvPartyUnlink();
				break;

			case HEADER_GC_PARTY_PARAMETER:
				ret = RecvPartyParameter();
				break;

			case HEADER_GC_SAFEBOX_SET:
				ret = RecvSafeBoxSetPacket();
				break;

			case HEADER_GC_SAFEBOX_DEL:
				ret = RecvSafeBoxDelPacket();
				break;

			case HEADER_GC_SAFEBOX_WRONG_PASSWORD:
				ret = RecvSafeBoxWrongPasswordPacket();
				break;

			case HEADER_GC_SAFEBOX_SIZE:
				ret = RecvSafeBoxSizePacket();
				break;

			case HEADER_GC_SAFEBOX_MONEY_CHANGE:
				ret = RecvSafeBoxMoneyChangePacket();
				break;

			case HEADER_GC_FISHING:
				ret = RecvFishing();
				break;

			case HEADER_GC_DUNGEON:
				ret = RecvDungeon();
				break;

			case HEADER_GC_TIME:
				ret = RecvTimePacket();
				break;

			case HEADER_GC_WALK_MODE:
				ret = RecvWalkModePacket();
				break;

			case HEADER_GC_CHANGE_SKILL_GROUP:
				ret = RecvChangeSkillGroupPacket();
				break;

			case HEADER_GC_REFINE_INFORMATION:
				ret = RecvRefineInformationPacket();
				break;

			case HEADER_GC_REFINE_INFORMATION_NEW:
				ret = RecvRefineInformationPacketNew();
				break;

			case HEADER_GC_SEPCIAL_EFFECT:
				ret = RecvSpecialEffect();
				break;

			case HEADER_GC_NPC_POSITION:
				ret = RecvNPCList();
				break;

			case HEADER_GC_CHANNEL:
				ret = RecvChannelPacket();
				break;

			case HEADER_GC_VIEW_EQUIP:
				ret = RecvViewEquipPacket();
				break;

			case HEADER_GC_LAND_LIST:
				ret = RecvLandPacket();
				break;

			//case HEADER_GC_TARGET_CREATE:
			//	ret = RecvTargetCreatePacket();
			//	break;

			case HEADER_GC_TARGET_CREATE_NEW:
				ret = RecvTargetCreatePacketNew();
				break;

			case HEADER_GC_TARGET_UPDATE:
				ret = RecvTargetUpdatePacket();
				break;

			case HEADER_GC_TARGET_DELETE:
				ret = RecvTargetDeletePacket();
				break;

			case HEADER_GC_AFFECT_ADD:
				ret = RecvAffectAddPacket();
				break;

			case HEADER_GC_AFFECT_REMOVE:
				ret = RecvAffectRemovePacket();
				break;

			case HEADER_GC_MALL_OPEN:
				ret = RecvMallOpenPacket();
				break;

			case HEADER_GC_MALL_SET:
				ret = RecvMallItemSetPacket();
				break;

			case HEADER_GC_MALL_DEL:
				ret = RecvMallItemDelPacket();
				break;

			case HEADER_GC_LOVER_INFO:
				ret = RecvLoverInfoPacket();
				break;

			case HEADER_GC_LOVE_POINT_UPDATE:
				ret = RecvLovePointUpdatePacket();
				break;

			case HEADER_GC_DIG_MOTION:
				ret = RecvDigMotionPacket();
				break;

			case HEADER_GC_HANDSHAKE:
				RecvHandshakePacket();
				return;
				break;

			case HEADER_GC_HANDSHAKE_OK:
				RecvHandshakeOKPacket();
				return;
				break;

			case HEADER_GC_HYBRIDCRYPT_KEYS:
				RecvHybridCryptKeyPacket();
				return;
				break;

			case HEADER_GC_HYBRIDCRYPT_SDB:
				RecvHybridCryptSDBPacket();
				return;
				break;

#ifdef _IMPROVED_PACKET_ENCRYPTION_
			case HEADER_GC_KEY_AGREEMENT:
				RecvKeyAgreementPacket();
				return;
				break;

			case HEADER_GC_KEY_AGREEMENT_COMPLETED:
				RecvKeyAgreementCompletedPacket();
				return;
				break;
#endif

			case HEADER_GC_SPECIFIC_EFFECT:
				ret = RecvSpecificEffect();
				break;

			case HEADER_GC_DRAGON_SOUL_REFINE:
				ret = RecvDragonSoulRefine();
				break;

#ifdef __DAILY_QUESTS__
			case HEADER_GC_SEND_DAILY_QUESTS:
				ret = RecvDailyQuests();
				break;
			case HEADER_GC_SEND_DAILY_REWARDS:
				ret = RecvDailyRewards();
				break;
			case HEADER_GC_UPDATE_DAILY_QUESTS:
				ret = RecvUpdateDailyQuests();
				break;
			case HEADER_GC_UPDATE_DAILY_REWARDS:
				ret = RecvUpdateDailyRewards();
				break;
#endif

			case HEADER_GC_GUILDSTORAGE:
				CPythonGuildStorage::Instance()->RecvGuildStoragePacket();
				ret = true;
				break;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			case HEADER_GC_ACCE:
				ret = RecvAccePacket();
				break;
#endif

#ifdef ENABLE_SWITCHBOT_SYSTEM
			case HEADER_GC_SWITCHBOT:
				ret = RecvSwitchbotPacket();
				break;
#endif

#ifdef ENABLE_DROP_INFO
			case HEADER_GC_DROP_INFO:
				ret = RecvDropInfoPacket();
				break;
#endif

#ifdef ENABLE_EVENT_MANAGER
			case HEADER_GC_EVENT_INFO:
				ret = RecvEventInformation();
				break;

			case HEADER_GC_EVENT_RELOAD:
				ret = RecvEventReload();
				break;

			case HEADER_GC_EVENT_KW_SCORE:
				ret = RecvEventKWScore();
				break;
#endif

#ifdef ENABLE_GEM_SYSTEM
			case HEADER_GC_GEM_SHOP_OPEN:
				ret = RecvGemShopOpen();
				break;
#endif

#ifdef ENABLE_INGAME_WIKI
			case InGameWiki::HEADER_GC_WIKI:
				ret = RecvWikiPacket();
				break;
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
			case HEADER_GC_BIOLOG_MANAGER:
				ret = RecvBiologManager();
				break;
#endif

#ifdef ENABLE_SKILL_GROUP_GUI
			case HEADER_GC_SKILL_GROUP_SELECT:
				ret = RecvSkillGroup();
				break;
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
			case HEADER_GC_PRIVATE_SHOP:
				ret = RecvPrivateShop();
				break;
#endif

#ifdef ENABLE_GOLD_MAX_EXTENDED
			case HEADER_GC_CHARACTER_GOLD_CHANGE:
				if (RecvGoldChange())
				{
					return;
				}
				break;

			case HEADER_GC_CHARACTER_GOLD:
				ret = __RecvPlayerGold();
				break;
#endif

#ifdef ENABLE_ATLASS_EXTENDED
			case HEADER_GC_BOSS_POSITION:
				ret = RecvBossList();
				break;
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
			case HEADER_GC_MAINTENANCE_INFO:
				ret = RecvMaintenanceInfo();
				break;
#endif

#ifdef ENABLE_HUNTING_SYSTEM
			case HEADER_GC_HUNTING_OPEN_MAIN:
				ret = RecvHuntingOpenWindowMain();
				break;

			case HEADER_GC_HUNTING_OPEN_SELECT:
				ret = RecvHuntingOpenWindowSelect();
				break;

			case HEADER_GC_HUNTING_OPEN_REWARD:
				ret = RecvHuntingOpenWindowReward();
				break;

			case HEADER_GC_HUNTING_UPDATE:
				ret = RecvHuntingUpdate();
				break;

			case HEADER_GC_HUNTING_RECIVE_RAND_ITEMS:
				ret = RecvHuntingRandomItems();
				break;
#endif

#if defined(__BL_67_ATTR__)
			case HEADER_GC_OPEN_67_ATTR:
				ret = Recv67AttrOpenPacket();
				break;
#endif

#ifdef ENABLE_DUNGEON_INFO_SYSTEM
			case HEADER_GC_DUNGEON_INFO:
				ret = RecvDungeonInfo();
				break;

			case HEADER_GC_DUNGEON_RANKING:
				ret = RecvDungeonRanking();
				break;
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			case HEADER_GC_UPDATE_COSTUME_VISIBILITY_STATUS:
				ret = UpdateCostumeVisibleSettings();
				break;
#endif

#ifdef ENABLE_ANTI_MULTIPLE_FARM
			case HEADER_GC_ANTI_FARM:
				ret = RecvAntiFarmInformation();
				break;
#endif

#ifdef ENABLE_PLAYER_RANKING
			case HEADER_GC_RANK_INFO:
				ret = RecvRankInfo();
				break;
#endif

#ifdef ENABLE_BATTLE_PASS
			case HEADER_GC_BATTLE_PASS_OPEN:
				ret = RecvBattlePassPacket();
				break;

			case HEADER_GC_BATTLE_PASS_UPDATE:
				ret = RecvBattlePassUpdatePacket();
				break;

			case HEADER_GC_BATTLE_PASS_RANKING:
				ret = RecvBattlePassRankingPacket();
				break;
#endif

#ifdef ENABLE_ITEMSHOP
			case HEADER_GC_ITEMSHOP:
				ret = RecvItemshopPacket();
				break;
#endif

#ifdef ENABLE_QUICK_OPEN
			case HEADER_GC_ITEM_BUFFERED:
				ret = RecvBufferedInventoryPacket();
				break;
#endif

#ifdef ENABLE_GUILD_SAFEBOX
			case HEADER_GC_GUILD_SAFEBOX:
				ret = RecvGuildSafeboxPacket();
				break;
#endif

#ifdef ENABLE_EVENT_CALENDAR
			case HEADER_GC_EVENT_MANAGER:
				ret = RecvEventManager();
				break;
#endif

#ifdef ENABLE_TRASH_SYSTEM
			case HEADER_GC_TRASH:
				ret = RecvTrashPacket();
				break;
#endif

			default:
				ret = RecvDefaultPacket(header);
				break;

		}
#ifdef __PERFORMANCE_CHECK__
		DWORD timeEndPacket=timeGetTime();

		{
			PERF_PacketInfo& rkPacketInfo=kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime+=timeEndPacket-timeBeginPacket;
		}

		{
			PERF_PacketInfo& rkPacketInfo=gs_kPacketTimeAnalyzer.m_kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime+=timeEndPacket-timeBeginPacket;
		}
#endif
	}

#ifdef __PERFORMANCE_CHECK__
	DWORD timeEndDispatch=timeGetTime();

	if (timeEndDispatch-timeBeginDispatch>2)
	{
		static FILE* fp=fopen("perf_dispatch_packet.txt", "w");

		fprintf(fp, "delay %d\n", timeEndDispatch-timeBeginDispatch);
		for (std::map<DWORD, PERF_PacketInfo>::iterator i=kMap_kPacketInfo.begin(); i!=kMap_kPacketInfo.end(); ++i)
		{
			if (i->second.dwTime>0)
				fprintf(fp, "header %d: count %d, time %d\n", i->first, i->second.dwCount, i->second.dwTime);
		}
		fputs("=====================================================\n", fp);
		fflush(fp);
	}
#endif

	if (!ret)
		RecvErrorPacket(header);

	static DWORD s_nextRefreshTime = ELTimer_GetMSec();

	DWORD curTime = ELTimer_GetMSec();
	if (s_nextRefreshTime > curTime)
		return;

	if (m_isRefreshCharacterWnd)
	{
		m_isRefreshCharacterWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshEquipmentWnd)
	{
		m_isRefreshEquipmentWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEquipment", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshInventoryWnd)
	{
		m_isRefreshInventoryWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshInventory", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshExchangeWnd)
	{
		m_isRefreshExchangeWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshExchange", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSkillWnd)
	{
		m_isRefreshSkillWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSkill", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSafeboxWnd)
	{
		m_isRefreshSafeboxWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafebox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMallWnd)
	{
		m_isRefreshMallWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMall", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

#ifdef ENABLE_GUILD_SAFEBOX
	if (m_isRefreshGuildSafeboxWnd)
	{
		m_isRefreshGuildSafeboxWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildSafebox", Py_BuildValue("()"));
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildSafeboxMoney", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
#endif

	if (m_isRefreshStatus)
	{
		m_isRefreshStatus=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMessengerWnd)
	{
		m_isRefreshMessengerWnd=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMessenger", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndInfoPage)
	{
		m_isRefreshGuildWndInfoPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildInfoPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndBoardPage)
	{
		m_isRefreshGuildWndBoardPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBoardPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPage)
	{
		m_isRefreshGuildWndMemberPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPageGradeComboBox)
	{
		m_isRefreshGuildWndMemberPageGradeComboBox=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPageGradeComboBox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndSkillPage)
	{
		m_isRefreshGuildWndSkillPage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildSkillPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndGradePage)
	{
		m_isRefreshGuildWndGradePage=false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGradePage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	if (m_isRefreshPrivateShopWindow)
	{
		m_isRefreshPrivateShopWindow = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshPrivateShopWindow", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
#endif
}

void CPythonNetworkStream::__InitializeGamePhase()
{
	__ServerTimeSync_Initialize();

	m_isRefreshStatus=false;
	m_isRefreshCharacterWnd=false;
	m_isRefreshEquipmentWnd=false;
	m_isRefreshInventoryWnd=false;
	m_isRefreshExchangeWnd=false;
	m_isRefreshSkillWnd=false;
	m_isRefreshSafeboxWnd=false;
	m_isRefreshMallWnd=false;
#ifdef ENABLE_GUILD_SAFEBOX
	m_isRefreshGuildSafeboxWnd=false;
#endif
	m_isRefreshMessengerWnd=false;
	m_isRefreshGuildWndInfoPage=false;
	m_isRefreshGuildWndBoardPage=false;
	m_isRefreshGuildWndMemberPage=false;
	m_isRefreshGuildWndMemberPageGradeComboBox=false;
	m_isRefreshGuildWndSkillPage=false;
	m_isRefreshGuildWndGradePage=false;

	m_EmoticonStringVector.clear();

	m_pInstTarget = NULL;
}

void CPythonNetworkStream::Warp(LONG lGlobalX, LONG lGlobalY)
{
	CPythonBackground& rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.Destroy();
	rkBgMgr.Create();
	rkBgMgr.Warp(lGlobalX, lGlobalY);
	//rkBgMgr.SetShadowLevel(CPythonBackground::SHADOW_ALL);
	rkBgMgr.RefreshShadowLevel();

	CGraphicDevice::ReleaseAccumulatedDriverMemory();

	LONG lLocalX = lGlobalX;
	LONG lLocalY = lGlobalY;
	__GlobalPositionToLocalPosition(lLocalX, lLocalY);
	float fHeight = CPythonBackground::Instance().GetHeight(float(lLocalX), float(lLocalY));

	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();
	rkApp.SetCenterPosition(float(lLocalX), float(lLocalY), fHeight);

	__ShowMapName(lLocalX, lLocalY);
}

void CPythonNetworkStream::__ShowMapName(LONG lLocalX, LONG lLocalY)
{
	const std::string & c_rstrMapFileName = CPythonBackground::Instance().GetWarpMapName();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ShowMapName", Py_BuildValue("(sii)", c_rstrMapFileName.c_str(), lLocalX, lLocalY));
}

void CPythonNetworkStream::__LeaveGamePhase()
{
	CInstanceBase::ClearPVPKeySystem();

	__ClearNetworkActorManager();

	m_bComboSkillFlag = FALSE;

	IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();
	rkChrMgr.Destroy();

	CPythonItem& rkItemMgr=CPythonItem::Instance();
	rkItemMgr.Destroy();
}

void CPythonNetworkStream::SetGamePhase()
{
#ifdef ENABLE_FRATELLO_SECURITY
	CFratelloSecurity::Instance().RunFastScan();
	CFratelloSecurity::Instance().RunFullScan();
#endif
	if ("Game"!=m_strPhase)
		m_phaseLeaveFunc.Run();

	Tracen("");
	Tracen("## Network - Game Phase ##");
	Tracen("");

	m_strPhase = "Game";

	m_dwChangingPhaseTime = ELTimer_GetMSec();
	m_phaseProcessFunc.Set(this, &CPythonNetworkStream::GamePhase);
	m_phaseLeaveFunc.Set(this, &CPythonNetworkStream::__LeaveGamePhase);

	IAbstractPlayer & rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

	__RefreshStatus();
}

bool CPythonNetworkStream::RecvObserverAddPacket()
{
	TPacketGCObserverAdd kObserverAddPacket;
	if (!Recv(sizeof(kObserverAddPacket), &kObserverAddPacket))
		return false;

	CPythonMiniMap::Instance().AddObserver(
		kObserverAddPacket.vid,
		kObserverAddPacket.x*100.0f,
		kObserverAddPacket.y*100.0f);

	return true;
}

bool CPythonNetworkStream::RecvObserverRemovePacket()
{
	TPacketGCObserverAdd kObserverRemovePacket;
	if (!Recv(sizeof(kObserverRemovePacket), &kObserverRemovePacket))
		return false;

	CPythonMiniMap::Instance().RemoveObserver(
		kObserverRemovePacket.vid
	);

	return true;
}

bool CPythonNetworkStream::RecvObserverMovePacket()
{
	TPacketGCObserverMove kObserverMovePacket;
	if (!Recv(sizeof(kObserverMovePacket), &kObserverMovePacket))
		return false;

	CPythonMiniMap::Instance().MoveObserver(
		kObserverMovePacket.vid,
		kObserverMovePacket.x*100.0f,
		kObserverMovePacket.y*100.0f);

	return true;
}

bool CPythonNetworkStream::RecvWarpPacket()
{
	TPacketGCWarp kWarpPacket;

	if (!Recv(sizeof(kWarpPacket), &kWarpPacket))
		return false;

#if defined(ENABLE_LOADING_PERFORMANCE) && defined(__WARP_SHOWER__)
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenWarpShowerWindow", Py_BuildValue("()"));
#endif
	__DirectEnterMode_Set(m_dwSelectedCharacterIndex);

	CNetworkStream::Connect((DWORD)kWarpPacket.lAddr, kWarpPacket.wPort);

	return true;
}

bool CPythonNetworkStream::RecvDuelStartPacket()
{
	TPacketGCDuelStart kDuelStartPacket;
	if (!Recv(sizeof(kDuelStartPacket), &kDuelStartPacket))
		return false;

	DWORD count = (kDuelStartPacket.wSize - sizeof(kDuelStartPacket))/sizeof(DWORD);

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
	{
		TraceError("CPythonNetworkStream::RecvDuelStartPacket - MainCharacter is NULL");
		return false;
	}
	DWORD dwVIDSrc = pkInstMain->GetVirtualID();
	DWORD dwVIDDest;

	for ( DWORD i = 0; i < count; i++)
	{
		Recv(sizeof(dwVIDDest),&dwVIDDest);
		CInstanceBase::InsertDUELKey(dwVIDSrc,dwVIDDest);
	}

	if(count == 0)
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_CANNOTATTACK);
	else
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_START);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));

	rkChrMgr.RefreshAllPCTextTail();

	return true;
}

bool CPythonNetworkStream::RecvPVPPacket()
{
	TPacketGCPVP kPVPPacket;
	if (!Recv(sizeof(kPVPPacket), &kPVPPacket))
		return false;

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();
	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();

	switch (kPVPPacket.bMode)
	{
		case PVP_MODE_AGREE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDDst))
				rkPlayer.RememberChallengeInstance(kPVPPacket.dwVIDSrc);

			if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDSrc))
				rkPlayer.RememberCantFightInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_REVENGE:
		{
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

			DWORD dwKiller = kPVPPacket.dwVIDSrc;
			DWORD dwVictim = kPVPPacket.dwVIDDst;

			if (rkPlayer.IsMainCharacterIndex(dwVictim))
				rkPlayer.RememberRevengeInstance(dwKiller);

			if (rkPlayer.IsMainCharacterIndex(dwKiller))
				rkPlayer.RememberCantFightInstance(dwVictim);
			break;
		}

		case PVP_MODE_FIGHT:
			rkChrMgr.InsertPVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
		case PVP_MODE_NONE:
			rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
			rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
			break;
	}

	__RefreshTargetBoardByVID(kPVPPacket.dwVIDSrc);
	__RefreshTargetBoardByVID(kPVPPacket.dwVIDDst);

	return true;
}

// DELETEME
/*
void CPythonNetworkStream::__SendWarpPacket()
{
	TPacketCGWarp kWarpPacket;
	kWarpPacket.bHeader=HEADER_GC_WARP;
	if (!Send(sizeof(kWarpPacket), &kWarpPacket))
	{
		return;
	}
}
*/
void CPythonNetworkStream::NotifyHack(const char* c_szMsg)
{
	if (!m_kQue_stHack.empty())
		if (c_szMsg==m_kQue_stHack.back())
			return;

	m_kQue_stHack.push_back(c_szMsg);
}

bool CPythonNetworkStream::__SendHack(const char* c_szMsg)
{
	Tracen(c_szMsg);

	TPacketCGHack kPacketHack;
	kPacketHack.bHeader=HEADER_CG_HACK;
	strncpy(kPacketHack.szBuf, c_szMsg, sizeof(kPacketHack.szBuf)-1);

	if (!Send(sizeof(kPacketHack), &kPacketHack))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddByVIDPacket(DWORD vid)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_VID;
	if (!Send(sizeof(packet), &packet))
		return false;
	if (!Send(sizeof(vid), &vid))
		return false;
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddByNamePacket(const char * c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_NAME;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN-1);
	szName[CHARACTER_NAME_MAX_LEN-1] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;
	Tracef(" SendMessengerAddByNamePacket : %s\n", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerRemovePacket(const char * c_szKey, const char * c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_REMOVE;
	if (!Send(sizeof(packet), &packet))
		return false;
	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN-1);
	if (!Send(sizeof(szKey), &szKey))
		return false;
	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
	if (!__CanActMainInstance())
		return true;

	if (fDstRot < 0.0f)
		fDstRot = 360 + fDstRot;
	else if (fDstRot > 360.0f)
		fDstRot = fmodf(fDstRot, 360.0f);

	TPacketCGMove kStatePacket;
	kStatePacket.bHeader = HEADER_CG_CHARACTER_MOVE;
	kStatePacket.bFunc = eFunc;
	kStatePacket.bArg = uArg;
	kStatePacket.bRot = fDstRot/5.0f;
	kStatePacket.lX = long(c_rkPPosDst.x);
	
    kStatePacket.lY = long(c_rkPPosDst.y);
	{
		CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (eFunc == 3 && uArg > 21){ return true; }
		 
		static UINT c_packet = 0;
		static DWORD t_packet = 0;
		DWORD t = timeGetTime();
		if (t_packet + 500 < t) {
			c_packet = 0;
			t_packet = t;
		}
		c_packet++;
		if (c_packet > 8){ return true; }
		 
		static UINT l_func = 0;
		static DWORD t_f = 0;
		 
		static float f_x = 0.0f;
		static float f_y = 0.0f;
		const D3DXVECTOR3 & cpos = pkInst->GetGraphicThingInstancePtr()->GetPosition();
		if (f_x == cpos.x && f_y == cpos.y && t - t_f < 1000 && l_func == eFunc && (eFunc < 3)) {
			return true;
		}
		f_x = cpos.x;
		f_y = cpos.y;
		 
		if (l_func == 0 && eFunc == 0 && t_f + 10 > t){ return true; }
		l_func = eFunc;
		t_f = t;
		if (cpos.x == 0.0f && cpos.y == -0.0f) {
			kStatePacket.lX = long(c_rkPPosDst.x);
			kStatePacket.lY = long(c_rkPPosDst.y);
		}else{
			kStatePacket.lX = long(cpos.x);
			kStatePacket.lY = long(cpos.y < 0.0f ? cpos.y * -1.0f : cpos.y);
		}
	}

	kStatePacket.dwTime = ELTimer_GetServerMSec();

	assert(kStatePacket.lX >= 0 && kStatePacket.lX < 204800);

	__LocalPositionToGlobalPosition(kStatePacket.lX, kStatePacket.lY);

	if (!Send(sizeof(kStatePacket), &kStatePacket))
	{
		Tracenf("CPythonNetworkStream::SendCharacterStatePacket(dwCmdTime=%u, fDstPos=(%f, %f), fDstRot=%f, eFunc=%d uArg=%d) - PACKET SEND ERROR",
			kStatePacket.dwTime,
			float(kStatePacket.lX),
			float(kStatePacket.lY),
			fDstRot,
			kStatePacket.bFunc,
			kStatePacket.bArg);
		return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID)
{
	TPacketCGUseSkill UseSkillPacket;
	UseSkillPacket.bHeader = HEADER_CG_USE_SKILL;
	UseSkillPacket.dwVnum = dwSkillIndex;
	UseSkillPacket.dwTargetVID = dwTargetVID;
	if (!Send(sizeof(TPacketCGUseSkill), &UseSkillPacket))
	{
		Tracen("CPythonNetworkStream::SendUseSkillPacket - SEND PACKET ERROR");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChatPacket(const char * c_szChat, BYTE byType)
{
	if (strlen(c_szChat) == 0)
		return true;

	if (strlen(c_szChat) >= 512)
		return true;

	if (c_szChat[0] == '/')
	{
		if (1 == strlen(c_szChat))
		{
			if (!m_strLastCommand.empty())
				c_szChat = m_strLastCommand.c_str();
		}
		else
		{
			m_strLastCommand = c_szChat;
		}
	}

	if (ClientCommand(c_szChat))
		return true;

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGChat ChatPacket;
	ChatPacket.header = HEADER_CG_CHAT;
	ChatPacket.length = sizeof(ChatPacket) + iTextLen;
	ChatPacket.type = byType;

	if (!Send(sizeof(ChatPacket), &ChatPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

//////////////////////////////////////////////////////////////////////////
// Emoticon
void CPythonNetworkStream::RegisterEmoticonString(const char * pcEmoticonString)
{
	if (m_EmoticonStringVector.size() >= CInstanceBase::EMOTICON_NUM)
	{
		TraceError("Can't register emoticon string... vector is full (size:%d)", m_EmoticonStringVector.size() );
		return;
	}
	m_EmoticonStringVector.push_back(pcEmoticonString);
}

bool CPythonNetworkStream::ParseEmoticon(const char * pChatMsg, DWORD * pdwEmoticon)
{
	for (DWORD dwEmoticonIndex = 0; dwEmoticonIndex < m_EmoticonStringVector.size() ; ++dwEmoticonIndex)
	{
		if (strlen(pChatMsg) > m_EmoticonStringVector[dwEmoticonIndex].size())
			continue;

		const char * pcFind = strstr(pChatMsg, m_EmoticonStringVector[dwEmoticonIndex].c_str());

		if (pcFind != pChatMsg)
			continue;

		*pdwEmoticon = dwEmoticonIndex;

		return true;
	}

	return false;
}
// Emoticon
//////////////////////////////////////////////////////////////////////////

void CPythonNetworkStream::__ConvertEmpireText(DWORD dwEmpireID, char* szText)
{
	if (dwEmpireID<1 || dwEmpireID>3)
		return;

	UINT uHanPos;

	STextConvertTable& rkTextConvTable=m_aTextConvTable[dwEmpireID-1];

	BYTE* pbText=(BYTE*)szText;
	while (*pbText)
	{
		if (*pbText & 0x80)
		{
			if (pbText[0]>=0xb0 && pbText[0]<=0xc8 && pbText[1]>=0xa1 && pbText[1]<=0xfe)
			{
				uHanPos=(pbText[0]-0xb0)*(0xfe-0xa1+1)+(pbText[1]-0xa1);
				pbText[0]=rkTextConvTable.aacHan[uHanPos][0];
				pbText[1]=rkTextConvTable.aacHan[uHanPos][1];
			}
			pbText+=2;
		}
		else
		{
			if (*pbText>='a' && *pbText<='z')
			{
				*pbText=rkTextConvTable.acLower[*pbText-'a'];
			}
			else if (*pbText>='A' && *pbText<='Z')
			{
				*pbText=rkTextConvTable.acUpper[*pbText-'A'];
			}
			pbText++;
		}
	}
}

std::string GetCharNameFromStr(const std::string& s) {
	const std::string start = "[Dungeon] ";
	const std::string end = " has completed";

	size_t p1 = s.find(start);
	if (p1 == std::string::npos) return "";

	p1 += start.length(); // move after "[Dungeon] "

	size_t p2 = s.find(end, p1);
	if (p2 == std::string::npos) return "";

	return s.substr(p1, p2 - p1);
}

bool CPythonNetworkStream::RecvChatPacket()
{
	TPacketGCChat kChat;
    char buf[1024 + 1];
	char line[1024 + 1];

	if (!Recv(sizeof(kChat), &kChat))
		return false;

	UINT uChatSize=kChat.size - sizeof(kChat);

	if (!Recv(uChatSize, buf))
		return false;

	buf[uChatSize]='\0';

#if defined(__BL_CLIENT_LOCALE_STRING__)
	TTokenVector vecMultiLine;

	if (kChat.bCanFormat) {
		std::string sFormat{ buf };
		CPythonLocale::Instance().FormatString(sFormat);
		strncpy(buf, sFormat.c_str(), sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		CPythonLocale::Instance().MultiLineSplit(sFormat, vecMultiLine);
	}
#endif

	if (LocaleService_IsEUROPE() && GetDefaultCodePage() == 1256)
	{
		char * p = strchr(buf, ':');
		if (p && p[1] == ' ')
			p[1] = 0x08;
	}

	if (kChat.type >= CHAT_TYPE_MAX_NUM)
		return true;

	if (CHAT_TYPE_COMMAND == kChat.type)
	{
		ServerCommand(buf);
		return true;
	}

	// Stop dungeon notice for others @GRIMM
	/*
	if (CHAT_TYPE_NOTICE == kChat.type)
	{
		CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
		CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();

		if (pkInstMain && !pkInstMain->IsGameMaster())
		{
			std::string stNotice = buf;
			if (stNotice.find("[Dungeon]") != std::string::npos && stNotice.find("has completed") != std::string::npos)
			{
				std::string stCharName = GetCharNameFromStr(stNotice);
				std::string stMyName = pkInstMain->GetNameString();
				if (stCharName.length() != 0 && stMyName != stCharName)
				{
					return true;
				}
			}
		}
	}
	//END
	*/

	if (kChat.dwVID != 0)
	{
		CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
		CInstanceBase * pkInstChatter = rkChrMgr.GetInstancePtr(kChat.dwVID);
		if (NULL == pkInstChatter)
			return true;

		switch (kChat.type)
		{
		case CHAT_TYPE_TALKING:
		case CHAT_TYPE_PARTY:
		case CHAT_TYPE_GUILD:
		case CHAT_TYPE_SHOUT:
		case CHAT_TYPE_WHISPER:
			{
				char * p = strchr(buf, ':');

				if (p)
					p += 2;
				else
					p = buf;

				DWORD dwEmoticon;

				if (ParseEmoticon(p, &dwEmoticon))
				{
					pkInstChatter->SetEmoticon(dwEmoticon);
					return true;
				}
				else
				{
					if (gs_bEmpireLanuageEnable)
					{
						CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
						if (pkInstMain)
							if (!pkInstMain->IsSameEmpire(*pkInstChatter))
								__ConvertEmpireText(pkInstChatter->GetEmpireID(), p);
					}

					if (m_isEnableChatInsultFilter)
					{
						if (false == pkInstChatter->IsNPC() && false == pkInstChatter->IsEnemy())
						{
							__FilterInsult(p, strlen(p));
						}
					}

					_snprintf(line, sizeof(line), "%s", p);
				}
			}
			break;
		case CHAT_TYPE_COMMAND:
		case CHAT_TYPE_INFO:
		case CHAT_TYPE_NOTICE:
		case CHAT_TYPE_BIG_NOTICE:
		// case CHAT_TYPE_UNK_10:
#ifdef ENABLE_DICE_SYSTEM
		case CHAT_TYPE_DICE_INFO:
#endif
#if defined(ENABLE_CHATTING_WINDOW_RENEWAL)
		case CHAT_TYPE_EXP_INFO:
		case CHAT_TYPE_ITEM_INFO:
		case CHAT_TYPE_MONEY_INFO:
#endif
		case CHAT_TYPE_MAX_NUM:
		default:
			_snprintf(line, sizeof(line), "%s", buf);
			break;
		}

		if (CHAT_TYPE_SHOUT != kChat.type)
		{
			CPythonTextTail::Instance().RegisterChatTail(kChat.dwVID, line);
		}

		if (pkInstChatter->IsPC())
		{
			CPythonChat::Instance().AppendChat(kChat.type, buf
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
			, IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_NORMAL, kChat.bEmpire, kChat.szCountry
#endif
			);
		}
	}
	else
	{
		if (CHAT_TYPE_NOTICE == kChat.type)
		{
#if defined(__BL_CLIENT_LOCALE_STRING__)
			if (!vecMultiLine.empty())
			{
				for (const std::string& sMessage : vecMultiLine)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", sMessage.c_str()));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", buf));
			}
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", buf));
#endif
		}
		else if (CHAT_TYPE_BIG_NOTICE == kChat.type)
		{
#if defined(__BL_CLIENT_LOCALE_STRING__)
			if (!vecMultiLine.empty())
			{
				for (const std::string& sMessage : vecMultiLine)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", sMessage.c_str()));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", buf));
			}
#else
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", buf));
#endif
		}
		else if (CHAT_TYPE_SHOUT == kChat.type)
		{
			char * p = strchr(buf, ':');

			if (p)
			{
				if (m_isEnableChatInsultFilter)
					__FilterInsult(p, strlen(p));
			}
		}

#if defined(__BL_CLIENT_LOCALE_STRING__)
		if (!vecMultiLine.empty())
		{
			for (const std::string& sMessage : vecMultiLine)
				CPythonChat::Instance().AppendChat(kChat.type, sMessage.c_str());
		}
		else
		{
			if (buf[0] == '@')
				CPythonChat::Instance().AppendChat(kChat.type, &buf[1], IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_0);
			else if (buf[0] == '$')
				CPythonChat::Instance().AppendChat(kChat.type, &buf[1], IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_DICE_1);
			else
			{
	#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
				CPythonChat::Instance().AppendChat(kChat.type, buf, IAbstractChat::ESpecialColorType::CHAT_SPECIAL_COLOR_NORMAL, kChat.bEmpire, kChat.szCountry);
	#else
				CPythonChat::Instance().AppendChat(kChat.type, buf);
	#endif
			}
		}
#else
		CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif

	}
	return true;
}

bool CPythonNetworkStream::RecvWhisperPacket()
{
	TPacketGCWhisper whisperPacket;
    char buf[512 + 1];

	if (!Recv(sizeof(whisperPacket), &whisperPacket))
		return false;

	assert(whisperPacket.wSize - sizeof(whisperPacket) < 512);

	if (!Recv(whisperPacket.wSize - sizeof(whisperPacket), &buf))
		return false;

	buf[whisperPacket.wSize - sizeof(whisperPacket)] = '\0';
#if defined(__BL_CLIENT_LOCALE_STRING__)
	if (whisperPacket.bCanFormat)
		CPythonLocale::Instance().FormatString(buf, sizeof(buf));
#endif

	static char line[256];
	if (CPythonChat::WHISPER_TYPE_CHAT == whisperPacket.bType || CPythonChat::WHISPER_TYPE_GM == whisperPacket.bType
#if defined(ENABLE_OFFLINE_MESSAGE)
		|| CPythonChat::WHISPER_TYPE_OFFLINE == whisperPacket.bType
#endif
	)
	{
		_snprintf(line, sizeof(line), "%s : %s", whisperPacket.szNameFrom, buf);
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(issis)", (int) whisperPacket.bType, whisperPacket.szNameFrom, line, whisperPacket.bEmpire, whisperPacket.szCountry));
#else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, line));
#endif
	}
	else if (CPythonChat::WHISPER_TYPE_SYSTEM == whisperPacket.bType || CPythonChat::WHISPER_TYPE_ERROR == whisperPacket.bType)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperSystemMessage", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperError", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}

	return true;
}

bool CPythonNetworkStream::SendWhisperPacket(const char * name, const char * c_szChat)
{
	if (strlen(c_szChat) >= 255)
		return true;

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGWhisper WhisperPacket;
	WhisperPacket.bHeader = HEADER_CG_WHISPER;
	WhisperPacket.wSize = sizeof(WhisperPacket) + iTextLen;

	strncpy(WhisperPacket.szNameTo, name, sizeof(WhisperPacket.szNameTo) - 1);

	if (!Send(sizeof(WhisperPacket), &WhisperPacket))
		return false;

	if (!Send(iTextLen, c_szChat))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvPointChange()
{
	TPacketGCPointChange PointChange;

	if (!Recv(sizeof(TPacketGCPointChange), &PointChange))
	{
		Tracen("Recv Point Change Packet Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	rkChrMgr.ShowPointEffect(PointChange.Type, PointChange.dwVID);

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pInstance && PointChange.dwVID == pInstance->GetVirtualID())
	{
		CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
		rkPlayer.SetStatus(PointChange.Type, PointChange.value);

		switch (PointChange.Type)
		{
			case POINT_STAT_RESET_COUNT:
				__RefreshStatus();
				break;
			case POINT_LEVEL:
			case POINT_ST:
			case POINT_DX:
			case POINT_HT:
			case POINT_IQ:
				__RefreshStatus();
				__RefreshSkillWindow();
				break;
			case POINT_SKILL:
			case POINT_SUB_SKILL:
			case POINT_HORSE_SKILL:
				__RefreshSkillWindow();
				break;
			case POINT_ENERGY:
				if (PointChange.value == 0)
				{
					rkPlayer.SetStatus(POINT_ENERGY_END_TIME, 0);
				}
				__RefreshStatus();
				break;
			default:
				__RefreshStatus();
				break;
		}

		if (POINT_GOLD == PointChange.Type)
		{
			if (PointChange.amount > 0)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickMoney", Py_BuildValue("(i)", PointChange.amount));
			}
		}
	}
#ifdef ENABLE_TEXT_LEVEL_REFRESH
	else
	{
		// the /advance command will provide no global refresh! it sends the pointchange only to the specific player and not all
		pInstance = CPythonCharacterManager::Instance().GetInstancePtr(PointChange.dwVID);
		if (pInstance && PointChange.Type == POINT_LEVEL)
		{
			pInstance->SetLevel(PointChange.value);
			pInstance->UpdateTextTailLevel(PointChange.value);
		}
	}
#endif

	return true;
}

bool CPythonNetworkStream::RecvStunPacket()
{
	TPacketGCStun StunPacket;

	if (!Recv(sizeof(StunPacket), &StunPacket))
	{
		Tracen("CPythonNetworkStream::RecvStunPacket Error");
		return false;
	}

	//Tracef("RecvStunPacket %d\n", StunPacket.vid);

	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase * pkInstSel = rkChrMgr.GetInstancePtr(StunPacket.vid);

	if (pkInstSel)
	{
		if (CPythonCharacterManager::Instance().GetMainInstancePtr()==pkInstSel)
			pkInstSel->Die();
		else
			pkInstSel->Stun();
	}

	return true;
}

bool CPythonNetworkStream::RecvDeadPacket()
{
	TPacketGCDead DeadPacket;
	if (!Recv(sizeof(DeadPacket), &DeadPacket))
	{
		Tracen("CPythonNetworkStream::RecvDeadPacket Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase * pkChrInstSel = rkChrMgr.GetInstancePtr(DeadPacket.vid);
	if (pkChrInstSel)
	{
		CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
		if (pkInstMain==pkChrInstSel)
		{
			Tracenf("On MainActor");
			if (false == pkInstMain->GetDuelMode())
			{
#ifndef ENABLE_RENEWAL_DEAD_PACKET
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("()"));
#else
				PyObject *times = PyTuple_New(REVIVE_TYPE_MAX);
				for (int i = REVIVE_TYPE_HERE; i < REVIVE_TYPE_MAX; i++)
				{
					PyTuple_SetItem(times, i, PyInt_FromLong(DeadPacket.t_d[i]));
				}
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("(O)", times));
#endif
			}
			CPythonPlayer::Instance().NotifyDeadMainCharacter();
#ifdef ENABLE_SKILL_COOLTIME_UPDATE
			CPythonPlayer::Instance().ResetSkillCoolTimes();
#endif
#ifdef ENABLE_METIN_STONE_QUEUE
			CPythonPlayer::Instance().SetMetinstonesQueue(false, 0);
#endif
		}

		pkChrInstSel->Die();
	}

	return true;
}

bool CPythonNetworkStream::SendCharacterPositionPacket(BYTE iPosition)
{
	TPacketCGPosition PositionPacket;

	PositionPacket.header = HEADER_CG_CHARACTER_POSITION;
	PositionPacket.position = iPosition;

	if (!Send(sizeof(TPacketCGPosition), &PositionPacket))
	{
		Tracen("Send Character Position Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOnClickPacket(DWORD vid)
{
	TPacketCGOnClick OnClickPacket;
	OnClickPacket.header	= HEADER_CG_ON_CLICK;
	OnClickPacket.vid		= vid;

	if (!Send(sizeof(OnClickPacket), &OnClickPacket))
	{
		Tracen("Send On_Click Packet Error");
		return false;
	}

	Tracef("SendOnClickPacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::RecvCharacterPositionPacket()
{
	TPacketGCPosition PositionPacket;

	if (!Recv(sizeof(TPacketGCPosition), &PositionPacket))
		return false;

	CInstanceBase * pChrInstance = CPythonCharacterManager::Instance().GetInstancePtr(PositionPacket.vid);

	if (!pChrInstance)
		return true;

	//pChrInstance->UpdatePosition(PositionPacket.position);

	return true;
}

bool CPythonNetworkStream::RecvMotionPacket()
{
	TPacketGCMotion MotionPacket;

	if (!Recv(sizeof(TPacketGCMotion), &MotionPacket))
		return false;

	CInstanceBase * pMainInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.vid);
	CInstanceBase * pVictimInstance = NULL;

	if (0 != MotionPacket.victim_vid)
		pVictimInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.victim_vid);

	if (!pMainInstance)
		return false;

	return true;
}

bool CPythonNetworkStream::RecvShopPacket()
{
	std::vector<char> vecBuffer;
	vecBuffer.clear();

    TPacketGCShop  packet_shop;
	if (!Recv(sizeof(packet_shop), &packet_shop))
		return false;

	int iSize = packet_shop.size - sizeof(packet_shop);
	if (iSize > 0)
	{
		vecBuffer.resize(iSize);
		if (!Recv(iSize, &vecBuffer[0]))
			return false;
	}

	switch (packet_shop.subheader)
	{
		case SHOP_SUBHEADER_GC_START:
			{
				CPythonShop::Instance().Clear();

				DWORD dwVID = *(DWORD *)&vecBuffer[0];

				TPacketGCShopStart * pShopStartPacket = (TPacketGCShopStart *)&vecBuffer[4];
				for (BYTE iItemIndex = 0; iItemIndex < SHOP_HOST_ITEM_MAX_NUM; ++iItemIndex)
				{
					CPythonShop::Instance().SetItemData(iItemIndex, pShopStartPacket->items[iItemIndex]);
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
			}
			break;

		case SHOP_SUBHEADER_GC_START_EX:
			{
				CPythonShop::Instance().Clear();

				TPacketGCShopStartEx * pShopStartPacket = (TPacketGCShopStartEx *)&vecBuffer[0];
				size_t read_point = sizeof(TPacketGCShopStartEx);

				DWORD dwVID = pShopStartPacket->owner_vid;
				BYTE shop_tab_count = pShopStartPacket->shop_tab_count;
				
				// TraceError("shop_tab_count : %d", shop_tab_count);

				CPythonShop::instance().SetTabCount(shop_tab_count);

				for (size_t i = 0; i < shop_tab_count; i++)
				{
					TPacketGCShopStartEx::TSubPacketShopTab* pPackTab = (TPacketGCShopStartEx::TSubPacketShopTab*)&vecBuffer[read_point];
					read_point += sizeof(TPacketGCShopStartEx::TSubPacketShopTab);

					CPythonShop::instance().SetTabCoinType(i, pPackTab->coin_type);
					CPythonShop::instance().SetTabName(i, pPackTab->name);

					struct packet_shop_item* item = &pPackTab->items[0];

					for (BYTE j = 0; j < SHOP_HOST_ITEM_MAX_NUM; j++)
					{
						TShopItemData* itemData = (item + j);
						CPythonShop::Instance().SetItemData(i, j, *itemData);
					}
				}

				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(ii)", dwVID, shop_tab_count));
			}
			break;

		case SHOP_SUBHEADER_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndShop", Py_BuildValue("()"));
			break;

		case SHOP_SUBHEADER_GC_UPDATE_ITEM:
			{
				TPacketGCShopUpdateItem * pShopUpdateItemPacket = (TPacketGCShopUpdateItem *)&vecBuffer[0];
				CPythonShop::Instance().SetItemData(pShopUpdateItemPacket->pos, pShopUpdateItemPacket->item);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshShop", Py_BuildValue("()"));
			}
			break;

		case SHOP_SUBHEADER_GC_UPDATE_PRICE:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetShopSellingPrice", Py_BuildValue("(i)", *(int *)&vecBuffer[0]));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY_EX"));
			break;

		case SHOP_SUBHEADER_GC_SOLDOUT:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "SOLDOUT"));
			break;

		case SHOP_SUBHEADER_GC_INVENTORY_FULL:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVENTORY_FULL"));
			break;

		case SHOP_SUBHEADER_GC_INVALID_POS:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVALID_POS"));
			break;

#if defined(ENABLE_RENEWAL_SHOPEX)
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_ITEM"));
			break;

		case SHOP_SUBHEADER_GC_NOT_ENOUGH_EXP:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_EXP"));
			break;

#ifdef ENABLE_GEM_SYSTEM
		case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_GEM:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY_GEM"));
			break;
#endif
#endif

		default:
			TraceError("CPythonNetworkStream::RecvShopPacket: Unknown subheader\n");
			break;
	}

	return true;
}

bool CPythonNetworkStream::RecvExchangePacket()
{
	TPacketGCExchange exchange_packet;

	if (!Recv(sizeof(exchange_packet), &exchange_packet))
		return false;

	switch (exchange_packet.subheader)
	{
		case EXCHANGE_SUBHEADER_GC_START:
			CPythonExchange::Instance().Clear();
			CPythonExchange::Instance().Start();
			CPythonExchange::Instance().SetSelfName(CPythonPlayer::Instance().GetName());
#if defined(ENABLE_LEVEL_IN_TRADE) || defined(ENABLE_EXCHANGE_WINDOW_RENEWAL)
			CPythonExchange::Instance().SetSelfLevel(CPythonPlayer::Instance().GetStatus(POINT_LEVEL));
#endif
#ifdef ENABLE_EXCHANGE_WINDOW_RENEWAL
			CPythonExchange::Instance().SetSelfRace(CPythonPlayer::Instance().GetRace());
#endif

			{
				CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(exchange_packet.arg1);

				if (pCharacterInstance)
				{
					CPythonExchange::Instance().SetTargetName(pCharacterInstance->GetNameString());
#if defined(ENABLE_LEVEL_IN_TRADE) || defined(ENABLE_EXCHANGE_WINDOW_RENEWAL)
					CPythonExchange::Instance().SetTargetLevel(pCharacterInstance->GetLevel());
#endif
#ifdef ENABLE_EXCHANGE_WINDOW_RENEWAL
					CPythonExchange::Instance().SetTargetRace(pCharacterInstance->GetRace());
#endif
				}
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartExchange", Py_BuildValue("()"));
			break;

		case EXCHANGE_SUBHEADER_GC_ITEM_ADD:
			if (exchange_packet.is_me)
			{
				int iSlotIndex = exchange_packet.arg2.cell;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
				CPythonExchange::Instance().SetItemToSelf(iSlotIndex, exchange_packet.arg1, (short)exchange_packet.arg3);
#else
				CPythonExchange::Instance().SetItemToSelf(iSlotIndex, exchange_packet.arg1, (BYTE) exchange_packet.arg3);
#endif
				for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
					CPythonExchange::Instance().SetItemMetinSocketToSelf(iSlotIndex, i, exchange_packet.alValues[i]);
				for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
					CPythonExchange::Instance().SetItemAttributeToSelf(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);

#ifdef WJ_ENABLE_TRADABLE_ICON
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddExchangeItemSlotIndex", Py_BuildValue("(i)", exchange_packet.arg4.cell));
#endif
			}
			else
			{
				int iSlotIndex = exchange_packet.arg2.cell;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
				CPythonExchange::Instance().SetItemToTarget(iSlotIndex, exchange_packet.arg1, (short)exchange_packet.arg3);
#else
				CPythonExchange::Instance().SetItemToTarget(iSlotIndex, exchange_packet.arg1, (BYTE) exchange_packet.arg3);
#endif
				for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
					CPythonExchange::Instance().SetItemMetinSocketToTarget(iSlotIndex, i, exchange_packet.alValues[i]);
				for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
					CPythonExchange::Instance().SetItemAttributeToTarget(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);
			}

			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ITEM_DEL:
			if (exchange_packet.is_me)
			{
#ifdef ENABLE_EXTENDED_ITEM_COUNT
				CPythonExchange::Instance().DelItemOfSelf((short)exchange_packet.arg1);
#else
				CPythonExchange::Instance().DelItemOfSelf((BYTE) exchange_packet.arg1);
#endif
			}
			else
			{
#ifdef ENABLE_EXTENDED_ITEM_COUNT
				CPythonExchange::Instance().DelItemOfTarget((short)exchange_packet.arg1);
#else
				CPythonExchange::Instance().DelItemOfTarget((BYTE) exchange_packet.arg1);
#endif
			}
			__RefreshExchangeWindow();
			__RefreshInventoryWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ELK_ADD:
			if (exchange_packet.is_me)
				CPythonExchange::Instance().SetElkToSelf(exchange_packet.arg1);
			else
				CPythonExchange::Instance().SetElkToTarget(exchange_packet.arg1);

			__RefreshExchangeWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_ACCEPT:
			if (exchange_packet.is_me)
			{
				CPythonExchange::Instance().SetAcceptToSelf((BYTE) exchange_packet.arg1);
			}
			else
			{
				CPythonExchange::Instance().SetAcceptToTarget((BYTE) exchange_packet.arg1);
			}
			__RefreshExchangeWindow();
			break;

		case EXCHANGE_SUBHEADER_GC_END:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndExchange", Py_BuildValue("()"));
			__RefreshInventoryWindow();
			CPythonExchange::Instance().End();
			break;

		case EXCHANGE_SUBHEADER_GC_ALREADY:
			Tracef("trade_already");
			break;

		case EXCHANGE_SUBHEADER_GC_LESS_ELK:
			Tracef("trade_less_elk");
			break;
	};

	return true;
}

bool CPythonNetworkStream::RecvQuestInfoPacket()
{
	TPacketGCQuestInfo QuestInfo;

	if (!Peek(sizeof(TPacketGCQuestInfo), &QuestInfo))
	{
		Tracen("Recv Quest Info Packet Error #1");
		return false;
	}

	if (!PeekNoFetch(QuestInfo.size))
	{
		Tracen("Recv Quest Info Packet Error #2");
		return false;
	}

	RecvNoFetch(sizeof(TPacketGCQuestInfo));

	const BYTE & c_rFlag = QuestInfo.flag;

	enum
	{
		QUEST_PACKET_TYPE_NONE,
		QUEST_PACKET_TYPE_BEGIN,
		QUEST_PACKET_TYPE_UPDATE,
		QUEST_PACKET_TYPE_END,
	};

	BYTE byQuestPacketType = QUEST_PACKET_TYPE_NONE;

	if (0 != (c_rFlag & QUEST_SEND_IS_BEGIN))
	{
		BYTE isBegin;
		if (!Recv(sizeof(isBegin), &isBegin))
			return false;

		if (isBegin)
			byQuestPacketType = QUEST_PACKET_TYPE_BEGIN;
		else
			byQuestPacketType = QUEST_PACKET_TYPE_END;
	}
	else
	{
		byQuestPacketType = QUEST_PACKET_TYPE_UPDATE;
	}

	// Recv Data Start
	char szTitle[30 + 1] = "";
	char szClockName[16 + 1] = "";
	int iClockValue = 0;
	char szCounterName[16 + 1] = "";
	int iCounterValue = 0;
	char szIconFileName[24 + 1] = "";

	if (0 != (c_rFlag & QUEST_SEND_TITLE))
	{
		if (!Recv(sizeof(szTitle), &szTitle))
			return false;

		szTitle[30]='\0';
#if defined(__BL_CLIENT_LOCALE_STRING__)
		CPythonLocale::Instance().FormatString(szTitle, sizeof(szTitle));
#endif
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_NAME))
	{
		if (!Recv(sizeof(szClockName), &szClockName))
			return false;

		szClockName[16]='\0';
#if defined(__BL_CLIENT_LOCALE_STRING__)
		CPythonLocale::Instance().FormatString(szClockName, sizeof(szClockName));
#endif
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_VALUE))
	{
		if (!Recv(sizeof(iClockValue), &iClockValue))
			return false;
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_NAME))
	{
		if (!Recv(sizeof(szCounterName), &szCounterName))
			return false;

		szCounterName[16]='\0';
#if defined(__BL_CLIENT_LOCALE_STRING__)
		CPythonLocale::Instance().FormatString(szCounterName, sizeof(szCounterName));
#endif
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_VALUE))
	{
		if (!Recv(sizeof(iCounterValue), &iCounterValue))
			return false;
	}
	if (0 != (c_rFlag & QUEST_SEND_ICON_FILE))
	{
		if (!Recv(sizeof(szIconFileName), &szIconFileName))
			return false;

		szIconFileName[24]='\0';
	}
	// Recv Data End

	CPythonQuest& rkQuest=CPythonQuest::Instance();

	// Process Start
	if (QUEST_PACKET_TYPE_END == byQuestPacketType)
	{
		rkQuest.DeleteQuestInstance(QuestInfo.index);
	}
	else if (QUEST_PACKET_TYPE_UPDATE == byQuestPacketType)
	{
		if (!rkQuest.IsQuest(QuestInfo.index))
		{
			rkQuest.MakeQuest(QuestInfo.index);
		}

		if (strlen(szTitle) > 0)
			rkQuest.SetQuestTitle(QuestInfo.index, szTitle);
		if (strlen(szClockName) > 0)
			rkQuest.SetQuestClockName(QuestInfo.index, szClockName);
		if (strlen(szCounterName) > 0)
			rkQuest.SetQuestCounterName(QuestInfo.index, szCounterName);
		if (strlen(szIconFileName) > 0)
			rkQuest.SetQuestIconFileName(QuestInfo.index, szIconFileName);

		if (c_rFlag & QUEST_SEND_CLOCK_VALUE)
			rkQuest.SetQuestClockValue(QuestInfo.index, iClockValue);
		if (c_rFlag & QUEST_SEND_COUNTER_VALUE)
			rkQuest.SetQuestCounterValue(QuestInfo.index, iCounterValue);
	}
	else if (QUEST_PACKET_TYPE_BEGIN == byQuestPacketType)
	{
		CPythonQuest::SQuestInstance QuestInstance;
		QuestInstance.dwIndex = QuestInfo.index;
		QuestInstance.strTitle = szTitle;
		QuestInstance.strClockName = szClockName;
		QuestInstance.iClockValue = iClockValue;
		QuestInstance.strCounterName = szCounterName;
		QuestInstance.iCounterValue = iCounterValue;
		QuestInstance.strIconFileName = szIconFileName;
		CPythonQuest::Instance().RegisterQuestInstance(QuestInstance);
	}
	// Process Start End

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshQuest", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvQuestConfirmPacket()
{
	TPacketGCQuestConfirm kQuestConfirmPacket;
	if (!Recv(sizeof(kQuestConfirmPacket), &kQuestConfirmPacket))
	{
		Tracen("RecvQuestConfirmPacket Error");
		return false;
	}

#if defined(__BL_CLIENT_LOCALE_STRING__)
	CPythonLocale::Instance().FormatString(kQuestConfirmPacket.msg, sizeof(kQuestConfirmPacket.msg));
#endif
	PyObject * poArg = Py_BuildValue("(sii)", kQuestConfirmPacket.msg, kQuestConfirmPacket.timeout, kQuestConfirmPacket.requestPID);
 	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OnQuestConfirm", poArg);
	return true;
}

bool CPythonNetworkStream::RecvRequestMakeGuild()
{
	TPacketGCBlank blank;
	if (!Recv(sizeof(blank), &blank))
	{
		Tracen("RecvRequestMakeGuild Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AskGuildName", Py_BuildValue("()"));

	return true;
}

void CPythonNetworkStream::ToggleGameDebugInfo()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ToggleDebugInfo", Py_BuildValue("()"));
}

#if defined(ENABLE_PICK_FILTER)
void CPythonNetworkStream::OpenPickUpWindow()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPickUpWindow", Py_BuildValue("()"));
}
#endif

bool CPythonNetworkStream::SendExchangeStartPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_START;
	packet.arg1			= vid;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_start_packet Error\n");
		return false;
	}

	Tracef("send_trade_start_packet   vid %d \n", vid);
	return SendSequence();
}

#ifdef ENABLE_GOLD_MAX_EXTENDED
	bool CPythonNetworkStream::SendExchangeElkAddPacket(unsigned long long elk)
#else
	bool CPythonNetworkStream::SendExchangeElkAddPacket(DWORD elk)
#endif
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ELK_ADD;
	packet.arg1			= elk;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_elk_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos
#ifdef ENABLE_CHECKINOUT_UPDATE
	, bool bAutoItemPos
#endif
)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_ADD;
	packet.Pos			= ItemPos;
	packet.arg2			= byDisplayPos;
#ifdef ENABLE_CHECKINOUT_UPDATE
	packet.bAutoItemPos = bAutoItemPos;
#endif

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeItemDelPacket(BYTE pos)
{
	assert(!"Can't be called function - CPythonNetworkStream::SendExchangeItemDelPacket");
	return true;

	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_DEL;
	packet.arg1			= pos;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_del_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeAcceptPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ACCEPT;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_accept_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeExitPacket()
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_CANCEL;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_exit_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendPointResetPacket()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartPointReset", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::__IsPlayerAttacking()
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
		return false;

	if (!pkInstMain->IsAttacking())
		return false;

	return true;
}

bool CPythonNetworkStream::RecvScriptPacket()
{
	TPacketGCScript ScriptPacket;

	if (!Recv(sizeof(TPacketGCScript), &ScriptPacket))
	{
		TraceError("RecvScriptPacket_RecvError");
		return false;
	}

	if (ScriptPacket.size < sizeof(TPacketGCScript))
	{
		TraceError("RecvScriptPacket_SizeError");
		return false;
	}

	ScriptPacket.size -= sizeof(TPacketGCScript);

	static string str;
	str = "";
	str.resize(ScriptPacket.size+1);

	if (!Recv(ScriptPacket.size, &str[0]))
		return false;

	str[str.size()-1] = '\0';

#if defined(__BL_CLIENT_LOCALE_STRING__)
	CPythonLocale::Instance().FormatString(str);
#endif

	int iIndex = CPythonEventManager::Instance().RegisterEventSetFromString(str);

	if (-1 != iIndex)
	{
		CPythonEventManager::Instance().SetVisibleLineCount(iIndex, 30);
		CPythonNetworkStream::Instance().OnScriptEventStart(ScriptPacket.skin,iIndex);
	}

	return true;
}

bool CPythonNetworkStream::SendScriptAnswerPacket(int iAnswer)
{
	TPacketCGScriptAnswer ScriptAnswer;

	ScriptAnswer.header = HEADER_CG_SCRIPT_ANSWER;
	ScriptAnswer.answer = (BYTE) iAnswer;
	if (!Send(sizeof(TPacketCGScriptAnswer), &ScriptAnswer))
	{
		Tracen("Send Script Answer Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendScriptButtonPacket(unsigned int iIndex)
{
	TPacketCGScriptButton ScriptButton;

	ScriptButton.header = HEADER_CG_SCRIPT_BUTTON;
	ScriptButton.idx = iIndex;
	if (!Send(sizeof(TPacketCGScriptButton), &ScriptButton))
	{
		Tracen("Send Script Button Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAnswerMakeGuildPacket(const char * c_szName)
{
	TPacketCGAnswerMakeGuild Packet;

	Packet.header = HEADER_CG_ANSWER_MAKE_GUILD;
	strncpy(Packet.guild_name, c_szName, GUILD_NAME_MAX_LEN);
	Packet.guild_name[GUILD_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendAnswerMakeGuild Packet Error");
		return false;
	}

// 	Tracef(" SendAnswerMakeGuildPacket : %s", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendQuestInputStringPacket(const char * c_szString)
{
	TPacketCGQuestInputString Packet;
	Packet.bHeader = HEADER_CG_QUEST_INPUT_STRING;
	strncpy(Packet.szString, c_szString, QUEST_INPUT_STRING_MAX_NUM);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendQuestInputStringPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID)
{
	TPacketCGQuestConfirm kPacket;
	kPacket.header = HEADER_CG_QUEST_CONFIRM;
	kPacket.answer = byAnswer;
	kPacket.requestPID = dwPID;

	if (!Send(sizeof(kPacket), &kPacket))
	{
		Tracen("SendQuestConfirmPacket Error");
		return false;
	}

	Tracenf(" SendQuestConfirmPacket : %d, %d", byAnswer, dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::RecvSkillCoolTimeEnd()
{
	TPacketGCSkillCoolTimeEnd kPacketSkillCoolTimeEnd;
	if (!Recv(sizeof(kPacketSkillCoolTimeEnd), &kPacketSkillCoolTimeEnd))
	{
		Tracen("CPythonNetworkStream::RecvSkillCoolTimeEnd - RecvError");
		return false;
	}

	CPythonPlayer::Instance().EndSkillCoolTime(kPacketSkillCoolTimeEnd.bSkill);

	return true;
}

bool CPythonNetworkStream::RecvSkillLevel()
{
	assert(!"CPythonNetworkStream::RecvSkillLevel - Don't use this function");
	TPacketGCSkillLevel packet;
	if (!Recv(sizeof(TPacketGCSkillLevel), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevel - RecvError");
		return false;
	}

	DWORD dwSlotIndex;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		if (rkPlayer.GetSkillSlotIndex(i, &dwSlotIndex))
			rkPlayer.SetSkillLevel(dwSlotIndex, packet.abSkillLevels[i]);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	Tracef(" >> RecvSkillLevel\n");
	return true;
}

bool CPythonNetworkStream::RecvSkillLevelNew()
{
	TPacketGCSkillLevelNew packet;

	if (!Recv(sizeof(TPacketGCSkillLevelNew), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevelNew - RecvError");
		return false;
	}

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();

	rkPlayer.SetSkill(7, 0);
	rkPlayer.SetSkill(8, 0);

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		TPlayerSkill & rPlayerSkill = packet.skills[i];

#ifndef ENABLE_SKILL_DESCRIPTION_RENEWAL
		if (i >= 112 && i <= 115 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(7, i);

		if (i >= 116 && i <= 119 && rPlayerSkill.bLevel)
			rkPlayer.SetSkill(8, i);
#endif

#ifdef ENABLE_SKILL_DESCRIPTION_RENEWAL
		rkPlayer.SetSkillLevel_(i, rPlayerSkill.bMasterType, rPlayerSkill.bLevel, rPlayerSkill.bBooks);
#else
		rkPlayer.SetSkillLevel_(i, rPlayerSkill.bMasterType, rPlayerSkill.bLevel);
#endif
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	//Tracef(" >> RecvSkillLevelNew\n");
	return true;
}

bool CPythonNetworkStream::RecvDamageInfoPacket()
{
	TPacketGCDamageInfo DamageInfoPacket;

	if (!Recv(sizeof(TPacketGCDamageInfo), &DamageInfoPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase * pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(DamageInfoPacket.dwVID);
	bool bSelf = (pInstTarget == CPythonCharacterManager::Instance().GetMainInstancePtr());
	bool bTarget = (pInstTarget==m_pInstTarget);
	if (pInstTarget)
	{
		if(DamageInfoPacket.damage >= 0)
			pInstTarget->AddDamageEffect(DamageInfoPacket.damage,DamageInfoPacket.flag,bSelf,bTarget);
#ifdef __DEBUG
		else
			TraceError("Damage is equal or below 0.");
#endif
	}

	return true;
}
bool CPythonNetworkStream::RecvTargetPacket()
{
	TPacketGCTarget TargetPacket;

	if (!Recv(sizeof(TPacketGCTarget), &TargetPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase * pInstPlayer = CPythonCharacterManager::Instance().GetMainInstancePtr();
	CInstanceBase * pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(TargetPacket.dwVID);
	if (pInstPlayer && pInstTarget)
	{
		if (!pInstTarget->IsDead())
		{
#if !defined(ENABLE_PLAYER_HP)
			if (pInstTarget->IsBuilding())
#else
			if (pInstTarget->IsPC() || pInstTarget->IsBuilding())
#endif
			{
#ifdef ENABLE_PLAYER_HP
				bool bIsPC;

				if (pInstTarget->IsPoly())
				{
					bIsPC = false;
				}
				else
				{
					bIsPC = true;
				}

					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoardEx", Py_BuildValue("(iisiii)", bIsPC, TargetPacket.dwVID, pInstTarget->GetNameString(), TargetPacket.bHPPercent, TargetPacket.dwActualHP, TargetPacket.dwMaxHP));
#else
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetPCTargetBoard", Py_BuildValue("(isiii)", TargetPacket.dwVID, pInstTarget->GetNameString(), TargetPacket.bHPPercent, TargetPacket.dwActualHP, TargetPacket.dwMaxHP));
#endif
			}
			else if (pInstPlayer->CanViewTargetHP(*pInstTarget))
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue("(iiii)", TargetPacket.dwVID, TargetPacket.bHPPercent, TargetPacket.dwActualHP, TargetPacket.dwMaxHP));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
			}

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (pInstTarget->IsPC())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHideCostumeTargetBoard", Py_BuildValue("(ibbb"
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
									  "b"
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
									  "b"
#endif
									  ")",
									  TargetPacket.dwVID,
									  TargetPacket.bCostumeHide[ECostumeHideParts::HIDE_COSTUME_HAIR],
									  TargetPacket.bCostumeHide[ECostumeHideParts::HIDE_COSTUME_BODY],
									  TargetPacket.bCostumeHide[ECostumeHideParts::HIDE_COSTUME_WEAPON]
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
									  , TargetPacket.bCostumeHide[ECostumeHideParts::HIDE_COSTUME_ACCE]
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
									  , TargetPacket.bCostumeHide[ECostumeHideParts::HIDE_COSTUME_AURA]
#endif
																												  ));
			}
#endif

			m_pInstTarget = pInstTarget;
		}
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
	}

	return true;
}

bool CPythonNetworkStream::RecvMountPacket()
{
	TPacketGCMount MountPacket;

	if (!Recv(sizeof(TPacketGCMount), &MountPacket))
	{
		Tracen("Recv Mount Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(MountPacket.vid);

	if (pInstance)
	{
		// Mount
		if (0 != MountPacket.mount_vid)
		{
//			pInstance->Ride(MountPacket.pos, MountPacket.mount_vid);
		}
		// Unmount
		else
		{
//			pInstance->Unride(MountPacket.pos, MountPacket.x, MountPacket.y);
		}
	}

	if (CPythonPlayer::Instance().IsMainCharacterIndex(MountPacket.vid))
	{
//		CPythonPlayer::Instance().SetRidingVehicleIndex(MountPacket.mount_vid);
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSpeedPacket()
{
	TPacketGCChangeSpeed SpeedPacket;

	if (!Recv(sizeof(TPacketGCChangeSpeed), &SpeedPacket))
	{
		Tracen("Recv Speed Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(SpeedPacket.vid);

	if (!pInstance)
		return true;

//	pInstance->SetWalkSpeed(SpeedPacket.walking_speed);
//	pInstance->SetRunSpeed(SpeedPacket.running_speed);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Recv

bool CPythonNetworkStream::SendAttackPacket(UINT uMotAttack, DWORD dwVIDVictim)
{
	if (!__CanActMainInstance())
		return true;

#ifdef ATTACK_TIME_LOG
	static DWORD prevTime = timeGetTime();
	DWORD curTime = timeGetTime();
	TraceError("TIME: %.4f(%.4f) ATTACK_PACKET: %d TARGET: %d", curTime/1000.0f, (curTime-prevTime)/1000.0f, uMotAttack, dwVIDVictim);
	prevTime = curTime;
#endif

	TPacketCGAttack kPacketAtk;

	kPacketAtk.header = HEADER_CG_ATTACK;
	kPacketAtk.bType = uMotAttack;
	kPacketAtk.dwVictimVID = dwVIDVictim;

	if (!SendSpecial(sizeof(kPacketAtk), &kPacketAtk))
	{
		Tracen("Send Battle Attack Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSpecial(int nLen, void * pvBuf)
{
	BYTE bHeader = *(BYTE *) pvBuf;

	switch (bHeader)
	{
		case HEADER_CG_ATTACK:
			{
				TPacketCGAttack * pkPacketAtk = (TPacketCGAttack *) pvBuf;
				pkPacketAtk->bCRCMagicCubeProcPiece = GetProcessCRCMagicCubePiece();
				pkPacketAtk->bCRCMagicCubeFilePiece = GetProcessCRCMagicCubePiece();
				return Send(nLen, pvBuf);
			}
			break;
	}

	return Send(nLen, pvBuf);
}

bool CPythonNetworkStream::RecvAddFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	Tracef("VID [%d] Added to target settings\n",kPacket.dwShooterVID);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifndef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX,kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(D3DXVECTOR3(kPacket.lX,kPacket.lY,h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::RecvFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
		return false;

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	//Tracef("CPythonNetworkStream::RecvFlyTargetingPacket - VID [%d]\n",kPacket.dwShooterVID);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifdef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX, kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(D3DXVECTOR3(kPacket.lX,kPacket.lY,h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::SendShootPacket(UINT uSkill)
{
	TPacketCGShoot kPacketShoot;
	kPacketShoot.bHeader=HEADER_CG_SHOOT;
	kPacketShoot.bType=uSkill;

	if (!Send(sizeof(kPacketShoot), &kPacketShoot))
	{
		Tracen("SendShootPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader	= HEADER_CG_ADD_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader	= HEADER_CG_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvCreateFlyPacket()
{
	TPacketGCCreateFly kPacket;
	if (!Recv(sizeof(TPacketGCCreateFly), &kPacket))
		return false;

	CFlyingManager& rkFlyMgr = CFlyingManager::Instance();
	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase * pkStartInst = rkChrMgr.GetInstancePtr(kPacket.dwStartVID);
	CInstanceBase * pkEndInst = rkChrMgr.GetInstancePtr(kPacket.dwEndVID);
	if (!pkStartInst || !pkEndInst)
		return true;

	if (CFlyingManager::FLY_SKILL_MUYEONG == kPacket.bType)
	{
		const uint8_t bEffectIndex = pkStartInst->GetSkillMotion(pkStartInst->GetSkillIndexByID(CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_MUYEONG));
		if (bEffectIndex != 0)
		{
			CFlyingManager::Instance().CreateIndexedFly(kPacket.bType + 100 + (bEffectIndex - 1),
				pkStartInst->GetGraphicThingInstancePtr(),
				pkEndInst->GetGraphicThingInstancePtr(),
				kPacket.bType);
			return true;
		}
	}

	rkFlyMgr.CreateIndexedFly(kPacket.bType, pkStartInst->GetGraphicThingInstancePtr(), pkEndInst->GetGraphicThingInstancePtr(), kPacket.bType);

	return true;
}

bool CPythonNetworkStream::SendTargetPacket(DWORD dwVID)
{
	TPacketCGTarget packet;
	packet.header = HEADER_CG_TARGET;
	packet.dwVID = dwVID;

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send Target Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY)
{
	TPacketCGSyncPositionElement kSyncPos;
	kSyncPos.dwVID=dwVictimVID;
	kSyncPos.lX=dwVictimX;
	kSyncPos.lY=dwVictimY;

	__LocalPositionToGlobalPosition(kSyncPos.lX, kSyncPos.lY);

	if (!Send(sizeof(kSyncPos), &kSyncPos))
	{
		Tracen("CPythonNetworkStream::SendSyncPositionElementPacket - ERROR");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvMessenger()
{
    TPacketGCMessenger p;
	if (!Recv(sizeof(p), &p))
		return false;

	int iSize = p.size - sizeof(p);
	char char_name[24+1];

	switch (p.subheader)
	{
		case MESSENGER_SUBHEADER_GC_LIST:
		{
			TPacketGCMessengerListOnline on;
			while(iSize)
			{
				if (!Recv(sizeof(TPacketGCMessengerListOffline),&on))
					return false;

				if (!Recv(on.length, char_name))
					return false;

				char_name[on.length] = 0;

				if (on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnFriendLogin(char_name);
				else
					CPythonMessenger::Instance().OnFriendLogout(char_name);

				iSize -= sizeof(TPacketGCMessengerListOffline);
				iSize -= on.length;
			}
			break;
		}

#ifdef ENABLE_MESSENGER_GM
		case MESSENGER_SUBHEADER_GC_GM_LIST:
		{
			TPacketGCMessengerGMListOnline gm_on;
			while (iSize)
			{
				if (!Recv(sizeof(TPacketGCMessengerGMListOffline), &gm_on))
					return false;

				if (!Recv(gm_on.length, char_name))
					return false;

				char_name[gm_on.length] = 0;

				if (gm_on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
					CPythonMessenger::Instance().OnGMLogin(char_name);
				else
					CPythonMessenger::Instance().OnGMLogout(char_name);

				iSize -= sizeof(TPacketGCMessengerGMListOffline);
				iSize -= gm_on.length;
			}
			break;
		}

		case MESSENGER_SUBHEADER_GC_GM_LOGIN:
		{
			TPacketGCMessengerLogin gm_p;
			if (!Recv(sizeof(gm_p), &gm_p))
				return false;
			if (!Recv(gm_p.length, char_name))
				return false;
			char_name[gm_p.length] = 0;
			CPythonMessenger::Instance().OnGMLogin(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_GM_LOGOUT:
		{
			TPacketGCMessengerLogout gm_logout;
			if (!Recv(sizeof(gm_logout), &gm_logout))
				return false;
			if (!Recv(gm_logout.length, char_name))
				return false;
			char_name[gm_logout.length] = 0;
			CPythonMessenger::Instance().OnGMLogout(char_name);
			break;
		}
#endif

		case MESSENGER_SUBHEADER_GC_LOGIN:
		{
			TPacketGCMessengerLogin p;
			if (!Recv(sizeof(p),&p))
				return false;
			if (!Recv(p.length, char_name))
				return false;
			char_name[p.length] = 0;
			CPythonMessenger::Instance().OnFriendLogin(char_name);
			__RefreshTargetBoardByName(char_name);
			break;
		}

		case MESSENGER_SUBHEADER_GC_LOGOUT:
		{
			TPacketGCMessengerLogout logout;
			if (!Recv(sizeof(logout),&logout))
				return false;
			if (!Recv(logout.length, char_name))
				return false;
			char_name[logout.length] = 0;
			CPythonMessenger::Instance().OnFriendLogout(char_name);
			break;
		}
#ifdef ENABLE_FRIENDS_LIST_FIX
		case MESSENGER_SUBHEADER_GC_REMOVE_FRIEND:
		{
			BYTE bLength;
			if (!Recv(sizeof(bLength), &bLength))
				return false;

			if (!Recv(bLength, char_name))
				return false;

			char_name[bLength] = 0;
			CPythonMessenger::Instance().RemoveFriend(char_name);
			break;
		}
#endif
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Party

bool CPythonNetworkStream::SendPartyInvitePacket(DWORD dwVID)
{
	TPacketCGPartyInvite kPartyInvitePacket;
	kPartyInvitePacket.header = HEADER_CG_PARTY_INVITE;
	kPartyInvitePacket.vid = dwVID;

	if (!Send(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInvitePacket [%ud] - PACKET SEND ERROR", dwVID);
		return false;
	}

	Tracef(" << SendPartyInvitePacket : %d\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAnswer)
{
	TPacketCGPartyInviteAnswer kPartyInviteAnswerPacket;
	kPartyInviteAnswerPacket.header = HEADER_CG_PARTY_INVITE_ANSWER;
	kPartyInviteAnswerPacket.leader_pid = dwLeaderVID;
	kPartyInviteAnswerPacket.accept = byAnswer;

	if (!Send(sizeof(kPartyInviteAnswerPacket), &kPartyInviteAnswerPacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInviteAnswerPacket [%ud %ud] - PACKET SEND ERROR", dwLeaderVID, byAnswer);
		return false;
	}

	Tracef(" << SendPartyInviteAnswerPacket : %d, %d\n", dwLeaderVID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyRemovePacket(DWORD dwPID)
{
	TPacketCGPartyRemove kPartyInviteRemove;
	kPartyInviteRemove.header = HEADER_CG_PARTY_REMOVE;
	kPartyInviteRemove.pid = dwPID;

	if (!Send(sizeof(kPartyInviteRemove), &kPartyInviteRemove))
	{
		Tracenf("CPythonNetworkStream::SendPartyRemovePacket [%ud] - PACKET SEND ERROR", dwPID);
		return false;
	}

	Tracef(" << SendPartyRemovePacket : %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag)
{
	TPacketCGPartySetState kPartySetState;
	kPartySetState.byHeader = HEADER_CG_PARTY_SET_STATE;
	kPartySetState.dwVID = dwVID;
	kPartySetState.byState = byState;
	kPartySetState.byFlag = byFlag;

	if (!Send(sizeof(kPartySetState), &kPartySetState))
	{
		Tracenf("CPythonNetworkStream::SendPartySetStatePacket(%ud, %ud) - PACKET SEND ERROR", dwVID, byState);
		return false;
	}

	Tracef(" << SendPartySetStatePacket : %d, %d, %d\n", dwVID, byState, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID)
{
	TPacketCGPartyUseSkill kPartyUseSkill;
	kPartyUseSkill.byHeader = HEADER_CG_PARTY_USE_SKILL;
	kPartyUseSkill.bySkillIndex = bySkillIndex;
	kPartyUseSkill.dwTargetVID = dwVID;

	if (!Send(sizeof(kPartyUseSkill), &kPartyUseSkill))
	{
		Tracenf("CPythonNetworkStream::SendPartyUseSkillPacket(%ud, %ud) - PACKET SEND ERROR", bySkillIndex, dwVID);
		return false;
	}

	Tracef(" << SendPartyUseSkillPacket : %d, %d\n", bySkillIndex, dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyParameterPacket(BYTE byDistributeMode)
{
	TPacketCGPartyParameter kPartyParameter;
	kPartyParameter.bHeader = HEADER_CG_PARTY_PARAMETER;
	kPartyParameter.bDistributeMode = byDistributeMode;

	if (!Send(sizeof(kPartyParameter), &kPartyParameter))
	{
		Tracenf("CPythonNetworkStream::SendPartyParameterPacket(%d) - PACKET SEND ERROR", byDistributeMode);
		return false;
	}

	Tracef(" << SendPartyParameterPacket : %d\n", byDistributeMode);
	return SendSequence();
}

bool CPythonNetworkStream::RecvPartyInvite()
{
	TPacketGCPartyInvite kPartyInvitePacket;
	if (!Recv(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kPartyInvitePacket.leader_pid);
	if (!pInstance)
	{
		TraceError(" CPythonNetworkStream::RecvPartyInvite - Failed to find leader instance [%d]\n", kPartyInvitePacket.leader_pid);
		return true;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvPartyInviteQuestion", Py_BuildValue("(is)", kPartyInvitePacket.leader_pid, pInstance->GetNameString()));
	Tracef(" >> RecvPartyInvite : %d, %s\n", kPartyInvitePacket.leader_pid, pInstance->GetNameString());

	return true;
}

bool CPythonNetworkStream::RecvPartyAdd()
{
	TPacketGCPartyAdd kPartyAddPacket;
	if (!Recv(sizeof(kPartyAddPacket), &kPartyAddPacket))
		return false;

	CPythonPlayer::Instance().AppendPartyMember(kPartyAddPacket.pid, kPartyAddPacket.name);
#ifdef BL_PARTY_UPDATE
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddPartyMember", Py_BuildValue("(isii)", kPartyAddPacket.pid, kPartyAddPacket.name, kPartyAddPacket.mapidx, kPartyAddPacket.channel));
#else
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddPartyMember", Py_BuildValue("(is)", kPartyAddPacket.pid, kPartyAddPacket.name));
#endif
	Tracef(" >> RecvPartyAdd : %d, %s\n", kPartyAddPacket.pid, kPartyAddPacket.name);

	return true;
}

bool CPythonNetworkStream::RecvPartyUpdate()
{
	TPacketGCPartyUpdate kPartyUpdatePacket;
	if (!Recv(sizeof(kPartyUpdatePacket), &kPartyUpdatePacket))
		return false;

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(kPartyUpdatePacket.pid, &pPartyMemberInfo))
		return true;

	BYTE byOldState = pPartyMemberInfo->byState;

	CPythonPlayer::Instance().UpdatePartyMemberInfo(kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);
	for (int i = 0; i < PARTY_AFFECT_SLOT_MAX_NUM; ++i)
	{
		CPythonPlayer::Instance().UpdatePartyMemberAffect(kPartyUpdatePacket.pid, i, kPartyUpdatePacket.affects[i]);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UpdatePartyMemberInfo", Py_BuildValue("(i)", kPartyUpdatePacket.pid));

	DWORD dwVID;
	if (CPythonPlayer::Instance().PartyMemberPIDToVID(kPartyUpdatePacket.pid, &dwVID))
	if (byOldState != kPartyUpdatePacket.state)
	{
		__RefreshTargetBoardByVID(dwVID);
	}

// 	Tracef(" >> RecvPartyUpdate : %d, %d, %d\n", kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);

	return true;
}

bool CPythonNetworkStream::RecvPartyRemove()
{
	TPacketGCPartyRemove kPartyRemovePacket;
	if (!Recv(sizeof(kPartyRemovePacket), &kPartyRemovePacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RemovePartyMember", Py_BuildValue("(i)", kPartyRemovePacket.pid));
	Tracef(" >> RecvPartyRemove : %d\n", kPartyRemovePacket.pid);

	return true;
}

bool CPythonNetworkStream::RecvPartyLink()
{
	TPacketGCPartyLink kPartyLinkPacket;
	if (!Recv(sizeof(kPartyLinkPacket), &kPartyLinkPacket))
		return false;

	CPythonPlayer::Instance().LinkPartyMember(kPartyLinkPacket.pid, kPartyLinkPacket.vid);
#ifdef BL_PARTY_UPDATE
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "LinkPartyMember", Py_BuildValue("(iiii)", kPartyLinkPacket.pid, kPartyLinkPacket.vid, kPartyLinkPacket.mapidx, kPartyLinkPacket.channel));
#else
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "LinkPartyMember", Py_BuildValue("(ii)", kPartyLinkPacket.pid, kPartyLinkPacket.vid));
#endif
	Tracef(" >> RecvPartyLink : %d, %d\n", kPartyLinkPacket.pid, kPartyLinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyUnlink()
{
	TPacketGCPartyUnlink kPartyUnlinkPacket;
	if (!Recv(sizeof(kPartyUnlinkPacket), &kPartyUnlinkPacket))
		return false;

	CPythonPlayer::Instance().UnlinkPartyMember(kPartyUnlinkPacket.pid);

	if (CPythonPlayer::Instance().IsMainCharacterIndex(kPartyUnlinkPacket.vid))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkAllPartyMember", Py_BuildValue("()"));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkPartyMember", Py_BuildValue("(i)", kPartyUnlinkPacket.pid));
	}

	Tracef(" >> RecvPartyUnlink : %d, %d\n", kPartyUnlinkPacket.pid, kPartyUnlinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyParameter()
{
	TPacketGCPartyParameter kPartyParameterPacket;
	if (!Recv(sizeof(kPartyParameterPacket), &kPartyParameterPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ChangePartyParameter", Py_BuildValue("(i)", kPartyParameterPacket.bDistributeMode));
	Tracef(" >> RecvPartyParameter : %d\n", kPartyParameterPacket.bDistributeMode);

	return true;
}

// Party
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Guild

bool CPythonNetworkStream::SendGuildAddMemberPacket(DWORD dwVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_ADD_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwVID), &dwVID))
		return false;

	Tracef(" SendGuildAddMemberPacket\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRemoveMemberPacket(DWORD dwPID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REMOVE_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwPID), &dwPID))
		return false;

	Tracef(" SendGuildRemoveMemberPacket %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char * c_szName)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;

	char szName[GUILD_GRADE_NAME_MAX_LEN+1];
	strncpy(szName, c_szName, GUILD_GRADE_NAME_MAX_LEN);
	szName[GUILD_GRADE_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(szName), &szName))
		return false;

	Tracef(" SendGuildChangeGradeNamePacket %d, %s\n", byGradeNumber, c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
		return false;
	if (!Send(sizeof(byAuthority), &byAuthority))
		return false;

	Tracef(" SendGuildChangeGradeAuthorityPacket %d, %d\n", byGradeNumber, byAuthority);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildOfferPacket(DWORD dwExperience)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_OFFER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwExperience), &dwExperience))
		return false;

	Tracef(" SendGuildOfferPacket %d\n", dwExperience);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildPostCommentPacket(const char * c_szMessage)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_POST_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	BYTE bySize = BYTE(strlen(c_szMessage)) + 1;
	if (!Send(sizeof(bySize), &bySize))
		return false;
	if (!Send(bySize, c_szMessage))
		return false;

	Tracef(" SendGuildPostCommentPacket %d, %s\n", bySize, c_szMessage);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDeleteCommentPacket(DWORD dwIndex)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DELETE_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwIndex), &dwIndex))
		return false;

	Tracef(" SendGuildDeleteCommentPacket %d\n", dwIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRefreshCommentsPacket(DWORD dwHighestIndex)
{
	static DWORD s_LastTime = timeGetTime() - 1001;

	if (timeGetTime() - s_LastTime < 1000)
		return true;
	s_LastTime = timeGetTime();

	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	Tracef(" SendGuildRefreshCommentPacket %d\n", dwHighestIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byGrade), &byGrade))
		return false;

	Tracef(" SendGuildChangeMemberGradePacket %d, %d\n", dwPID, byGrade);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_USE_SKILL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwSkillID), &dwSkillID))
		return false;
	if (!Send(sizeof(dwTargetVID), &dwTargetVID))
		return false;

	Tracef(" SendGuildUseSkillPacket %d, %d\n", dwSkillID, dwTargetVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwPID), &dwPID))
		return false;
	if (!Send(sizeof(byFlag), &byFlag))
		return false;

	Tracef(" SendGuildChangeMemberGeneralFlagPacket %d, %d\n", dwPID, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwGuildID), &dwGuildID))
		return false;
	if (!Send(sizeof(byAnswer), &byAnswer))
		return false;

	Tracef(" SendGuildInviteAnswerPacket %d, %d\n", dwGuildID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChargeGSPPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHARGE_GSP;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;

	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildChargeGSPPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDepositMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DEPOSIT_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildDepositMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildWithdrawMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_WITHDRAW_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
		return false;
	if (!Send(sizeof(dwMoney), &dwMoney))
		return false;

	Tracef(" SendGuildWithdrawMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::RecvGuild()
{
    TPacketGCGuild GuildPacket;
	if (!Recv(sizeof(GuildPacket), &GuildPacket))
		return false;

	switch(GuildPacket.subheader)
	{
		case GUILD_SUBHEADER_GC_LOGIN:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			// Messenger
			CPythonGuild::TGuildMemberData * pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LoginGuildMember(pGuildMemberData->strName.c_str());

			//Tracef(" <Login> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_LOGOUT:
		{
			DWORD dwPID;
			if (!Recv(sizeof(DWORD), &dwPID))
				return false;

			// Messenger
			CPythonGuild::TGuildMemberData * pGuildMemberData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
				if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().LogoutGuildMember(pGuildMemberData->strName.c_str());

			//Tracef(" <Logout> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_REMOVE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;

			if (CPythonGuild::Instance().IsMainPlayer(dwPID))
			{
				CPythonGuild::Instance().Destroy();
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "DeleteGuild", Py_BuildValue("()"));
				CPythonMessenger::Instance().RemoveAllGuildMember();
				__SetGuildID(0);
				__RefreshMessengerWindow();
				__RefreshTargetBoard();
				__RefreshCharacterWindow();
			}
			else
			{
				// Get Member Name
				std::string strMemberName = "";
				CPythonGuild::TGuildMemberData * pData;
				if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pData))
				{
					strMemberName = pData->strName;
					CPythonMessenger::Instance().RemoveGuildMember(pData->strName.c_str());
				}

				CPythonGuild::Instance().RemoveMember(dwPID);

				// Refresh
				__RefreshTargetBoardByName(strMemberName.c_str());
				__RefreshGuildWindowMemberPage();
			}

			Tracef(" <Remove> %d\n", dwPID);
			break;
		}
		case GUILD_SUBHEADER_GC_LIST:
		{
			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			for (; iPacketSize > 0;)
			{
				TPacketGCGuildSubMember memberPacket;
				if (!Recv(sizeof(memberPacket), &memberPacket))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN+1] = "";
				if (memberPacket.byNameFlag)
				{
					if (!Recv(sizeof(szName), &szName))
						return false;

					iPacketSize -= CHARACTER_NAME_MAX_LEN+1;
				}
				else
				{
					CPythonGuild::TGuildMemberData * pMemberData;
					if (CPythonGuild::Instance().GetMemberDataPtrByPID(memberPacket.pid, &pMemberData))
					{
						strncpy(szName, pMemberData->strName.c_str(), CHARACTER_NAME_MAX_LEN);
					}
				}

				//Tracef(" <List> %d : %s, %d (%d, %d, %d)\n", memberPacket.pid, szName, memberPacket.byGrade, memberPacket.byJob, memberPacket.byLevel, memberPacket.dwOffer);

				CPythonGuild::SGuildMemberData GuildMemberData;
				GuildMemberData.dwPID = memberPacket.pid;
				GuildMemberData.byGrade = memberPacket.byGrade;
				GuildMemberData.strName = szName;
				GuildMemberData.byJob = memberPacket.byJob;
				GuildMemberData.byLevel = memberPacket.byLevel;
				GuildMemberData.dwOffer = memberPacket.dwOffer;
				GuildMemberData.byGeneralFlag = memberPacket.byIsGeneral;
				CPythonGuild::Instance().RegisterMember(GuildMemberData);

				// Messenger
				if (strcmp(szName, CPythonPlayer::Instance().GetName()))
					CPythonMessenger::Instance().AppendGuildMember(szName);

				__RefreshTargetBoardByName(szName);

				iPacketSize -= sizeof(memberPacket);
			}

			__RefreshGuildWindowInfoPage();
			__RefreshGuildWindowMemberPage();
			__RefreshMessengerWindow();
			__RefreshCharacterWindow();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			for (BYTE i = 0; i < byCount; ++ i)
			{
				BYTE byIndex;
				if (!Recv(sizeof(byCount), &byIndex))
					return false;
				TPacketGCGuildSubGrade GradePacket;
				if (!Recv(sizeof(GradePacket), &GradePacket))
					return false;

#if defined(__BL_CLIENT_LOCALE_STRING__)
				CPythonLocale::Instance().FormatString(GradePacket.grade_name, sizeof(GradePacket.grade_name));
#endif
				CPythonGuild::Instance().SetGradeData(byIndex, CPythonGuild::SGuildGradeData(GradePacket.auth_flag, GradePacket.grade_name));
				//Tracef(" <Grade> [%d/%d] : %s, %d\n", byIndex, byCount, GradePacket.grade_name, GradePacket.auth_flag);
			}
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_NAME:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;

			char szGradeName[GUILD_GRADE_NAME_MAX_LEN+1] = "";
			if (!Recv(sizeof(szGradeName), &szGradeName))
				return false;

#if defined(__BL_CLIENT_LOCALE_STRING__)
			CPythonLocale::Instance().FormatString(szGradeName, sizeof(szGradeName));
#endif
			CPythonGuild::Instance().SetGradeName(byGradeNumber, szGradeName);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Name> %d, %s\n", byGradeNumber, szGradeName);
			__RefreshGuildWindowGradePage();
			__RefreshGuildWindowMemberPageGradeComboBox();
			break;
		}
		case GUILD_SUBHEADER_GC_GRADE_AUTH:
		{
			BYTE byGradeNumber;
			if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
				return false;
			BYTE byAuthorityFlag;
			if (!Recv(sizeof(byAuthorityFlag), &byAuthorityFlag))
				return false;

			CPythonGuild::Instance().SetGradeAuthority(byGradeNumber, byAuthorityFlag);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

			Tracef(" <Change Grade Authority> %d, %d\n", byGradeNumber, byAuthorityFlag);
			__RefreshGuildWindowGradePage();
			break;
		}
		case GUILD_SUBHEADER_GC_INFO:
		{
			TPacketGCGuildInfo GuildInfo;
			if (!Recv(sizeof(GuildInfo), &GuildInfo))
				return false;

			CPythonGuild::Instance().EnableGuild();
			CPythonGuild::TGuildInfo & rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();
			strncpy(rGuildInfo.szGuildName, GuildInfo.name, GUILD_NAME_MAX_LEN);
			rGuildInfo.szGuildName[GUILD_NAME_MAX_LEN] = '\0';

			rGuildInfo.dwGuildID = GuildInfo.guild_id;
			rGuildInfo.dwMasterPID = GuildInfo.master_pid;
			rGuildInfo.dwGuildLevel = GuildInfo.level;
			rGuildInfo.dwCurrentExperience = GuildInfo.exp;
			rGuildInfo.dwCurrentMemberCount = GuildInfo.member_count;
			rGuildInfo.dwMaxMemberCount = GuildInfo.max_member_count;
			rGuildInfo.dwGuildMoney = GuildInfo.gold;
			rGuildInfo.bHasLand = GuildInfo.hasLand;

			//Tracef(" <Info> %s, %d, %d : %d\n", GuildInfo.name, GuildInfo.master_pid, GuildInfo.level, rGuildInfo.bHasLand);
			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_COMMENTS:
		{
			BYTE byCount;
			if (!Recv(sizeof(byCount), &byCount))
				return false;

			CPythonGuild::Instance().ClearComment();
			//Tracef(" >>> Comments Count : %d\n", byCount);

			for (BYTE i = 0; i < byCount; ++i)
			{
				DWORD dwCommentID;
				if (!Recv(sizeof(dwCommentID), &dwCommentID))
					return false;

				char szName[CHARACTER_NAME_MAX_LEN+1] = "";
				if (!Recv(sizeof(szName), &szName))
					return false;

				char szComment[GULID_COMMENT_MAX_LEN+1] = "";
				if (!Recv(sizeof(szComment), &szComment))
					return false;

				//Tracef(" [Comment-%d] : %s, %s\n", dwCommentID, szName, szComment);
				CPythonGuild::Instance().RegisterComment(dwCommentID, szName, szComment);
			}

			__RefreshGuildWindowBoardPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_EXP:
		{
			BYTE byLevel;
			if (!Recv(sizeof(byLevel), &byLevel))
				return false;
			DWORD dwEXP;
			if (!Recv(sizeof(dwEXP), &dwEXP))
				return false;
			CPythonGuild::Instance().SetGuildEXP(byLevel, dwEXP);
			Tracef(" <ChangeEXP> %d, %d\n", byLevel, dwEXP);
			__RefreshGuildWindowInfoPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byGrade;
			if (!Recv(sizeof(byGrade), &byGrade))
				return false;
			CPythonGuild::Instance().ChangeGuildMemberGrade(dwPID, byGrade);
			Tracef(" <ChangeMemberGrade> %d, %d\n", dwPID, byGrade);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_SKILL_INFO:
		{
			CPythonGuild::TGuildSkillData & rSkillData = CPythonGuild::Instance().GetGuildSkillDataRef();
			if (!Recv(sizeof(rSkillData.bySkillPoint), &rSkillData.bySkillPoint))
				return false;
			if (!Recv(sizeof(rSkillData.bySkillLevel), rSkillData.bySkillLevel))
				return false;
			if (!Recv(sizeof(rSkillData.wGuildPoint), &rSkillData.wGuildPoint))
				return false;
			if (!Recv(sizeof(rSkillData.wMaxGuildPoint), &rSkillData.wMaxGuildPoint))
				return false;

			Tracef(" <SkillInfo> %d / %d, %d\n", rSkillData.bySkillPoint, rSkillData.wGuildPoint, rSkillData.wMaxGuildPoint);
			__RefreshGuildWindowSkillPage();
			break;
		}
		case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL:
		{
			DWORD dwPID;
			if (!Recv(sizeof(dwPID), &dwPID))
				return false;
			BYTE byFlag;
			if (!Recv(sizeof(byFlag), &byFlag))
				return false;

			CPythonGuild::Instance().ChangeGuildMemberGeneralFlag(dwPID, byFlag);
			Tracef(" <ChangeMemberGeneralFlag> %d, %d\n", dwPID, byFlag);
			__RefreshGuildWindowMemberPage();
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_INVITE:
		{
			DWORD dwGuildID;
			if (!Recv(sizeof(dwGuildID), &dwGuildID))
				return false;
			char szGuildName[GUILD_NAME_MAX_LEN+1];
			if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
				return false;

			szGuildName[GUILD_NAME_MAX_LEN] = 0;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvGuildInviteQuestion", Py_BuildValue("(is)", dwGuildID, szGuildName));
			Tracef(" <Guild Invite> %d, %s\n", dwGuildID, szGuildName);
			break;
		}
		case GUILD_SUBHEADER_GC_WAR:
		{
			TPacketGCGuildWar kGuildWar;
			if (!Recv(sizeof(kGuildWar), &kGuildWar))
				return false;

			switch (kGuildWar.bWarState)
			{
				case GUILD_WAR_SEND_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_SEND_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnSendDeclare",
						Py_BuildValue("(i)", kGuildWar.dwGuildOpp)
					);
					break;
				case GUILD_WAR_RECV_DECLARE:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_RECV_DECLARE\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnRecvDeclare",
						Py_BuildValue("(ii)", kGuildWar.dwGuildOpp, kGuildWar.bType)
					);
					break;
				case GUILD_WAR_ON_WAR:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_ON_WAR : %d, %d\n", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp);
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnStart",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().StartGuildWar(kGuildWar.dwGuildOpp);
					break;
				case GUILD_WAR_END:
					Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_END\n");
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						"BINARY_GuildWar_OnEnd",
						Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
					);
					CPythonGuild::Instance().EndGuildWar(kGuildWar.dwGuildOpp);
					break;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_NAME:
		{
			DWORD dwID;
			char szGuildName[GUILD_NAME_MAX_LEN+1];

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

			int nItemSize = sizeof(dwID) + GUILD_NAME_MAX_LEN;

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_NAME");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwID), &dwID))
					return false;

				if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
					return false;

				szGuildName[GUILD_NAME_MAX_LEN] = 0;

				//Tracef(" >> GulidName [%d : %s]\n", dwID, szGuildName);
				CPythonGuild::Instance().RegisterGuildName(dwID, szGuildName);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_WAR_LIST");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::InsertGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST:
		{
			DWORD dwSrcGuildID;
			DWORD dwDstGuildID;

			int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
			int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

			assert(iPacketSize%nItemSize==0 && "GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST");

			for (; iPacketSize > 0;)
			{
				if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
					return false;

				if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
					return false;

				Tracef(" >> GulidWarEndList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
				CInstanceBase::RemoveGVGKey(dwSrcGuildID, dwDstGuildID);
				CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
				iPacketSize -= nItemSize;
			}
			break;
		}
		case GUILD_SUBHEADER_GC_WAR_POINT:
		{
			TPacketGuildWarPoint GuildWarPoint;
			if (!Recv(sizeof(GuildWarPoint), &GuildWarPoint))
				return false;

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"BINARY_GuildWar_OnRecvPoint",
				Py_BuildValue("(iii)", GuildWarPoint.dwGainGuildID, GuildWarPoint.dwOpponentGuildID, GuildWarPoint.lPoint)
			);
			break;
		}
		case GUILD_SUBHEADER_GC_MONEY_CHANGE:
		{
			DWORD dwMoney;
			if (!Recv(sizeof(dwMoney), &dwMoney))
				return false;

			CPythonGuild::Instance().SetGuildMoney(dwMoney);

			__RefreshGuildWindowInfoPage();
			Tracef(" >> Guild Money Change : %d\n", dwMoney);
			break;
		}
	}

	return true;
}

// Guild
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Fishing

bool CPythonNetworkStream::SendFishingPacket(int iRotation)
{
	BYTE byHeader = HEADER_CG_FISHING;
	if (!Send(sizeof(byHeader), &byHeader))
		return false;
	BYTE byPacketRotation = iRotation / 5;
	if (!Send(sizeof(BYTE), &byPacketRotation))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount)
{
	TPacketCGGiveItem GiveItemPacket;
	GiveItemPacket.byHeader = HEADER_CG_GIVE_ITEM;
	GiveItemPacket.dwTargetVID = dwTargetVID;
	GiveItemPacket.ItemPos = ItemPos;
	GiveItemPacket.byItemCount = iItemCount;

	if (!Send(sizeof(GiveItemPacket), &GiveItemPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvFishing()
{
	TPacketGCFishing FishingPacket;
	if (!Recv(sizeof(FishingPacket), &FishingPacket))
		return false;

	CInstanceBase * pFishingInstance = NULL;
	if (FISHING_SUBHEADER_GC_FISH != FishingPacket.subheader)
	{
		pFishingInstance = CPythonCharacterManager::Instance().GetInstancePtr(FishingPacket.info);
		if (!pFishingInstance)
			return true;
	}

	switch (FishingPacket.subheader)
	{
		case FISHING_SUBHEADER_GC_START:
			pFishingInstance->StartFishing(float(FishingPacket.dir) * 5.0f);
			break;
		case FISHING_SUBHEADER_GC_STOP:
			if (pFishingInstance->IsFishing())
				pFishingInstance->StopFishing();
			break;
		case FISHING_SUBHEADER_GC_REACT:
			if (pFishingInstance->IsFishing())
			{
				pFishingInstance->SetFishEmoticon(); // Fish Emoticon
				pFishingInstance->ReactFishing();
			}
			break;
		case FISHING_SUBHEADER_GC_SUCCESS:
			pFishingInstance->CatchSuccess();
			break;
		case FISHING_SUBHEADER_GC_FAIL:
			pFishingInstance->CatchFail();
			if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingFailure", Py_BuildValue("()"));
			}
			break;
		case FISHING_SUBHEADER_GC_FISH:
		{
			DWORD dwFishID = FishingPacket.info;

			if (0 == FishingPacket.info)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotifyUnknown", Py_BuildValue("()"));
				return true;
			}

			CItemData * pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(dwFishID, &pItemData))
				return true;

			CInstanceBase * pMainInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
			if (!pMainInstance)
				return true;

			if (pMainInstance->IsFishing())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotify", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingSuccess", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
			}
			break;
		}
	}

	return true;
}
// Fishing
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Dungeon
bool CPythonNetworkStream::RecvDungeon()
{
	TPacketGCDungeon DungeonPacket;
	if (!Recv(sizeof(DungeonPacket), &DungeonPacket))
		return false;

	switch (DungeonPacket.subheader)
	{
		case DUNGEON_SUBHEADER_GC_TIME_ATTACK_START:
		{
			break;
		}
		case DUNGEON_SUBHEADER_GC_DESTINATION_POSITION:
		{
			unsigned long ulx, uly;
			if (!Recv(sizeof(ulx), &ulx))
				return false;
			if (!Recv(sizeof(uly), &uly))
				return false;

			CPythonPlayer::Instance().SetDungeonDestinationPosition(ulx, uly);
			break;
		}
	}

	return true;
}
// Dungeon
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// MyShop
bool CPythonNetworkStream::SendBuildPrivateShopPacket(const char * c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock)
{
	TPacketCGMyShop packet;
	packet.bHeader = HEADER_CG_MYSHOP;
	strncpy(packet.szSign, c_szName, SHOP_SIGN_MAX_LEN);
	packet.bCount = c_rSellingItemStock.size();
	if (!Send(sizeof(packet), &packet))
		return false;

	for (std::vector<TShopItemTable>::const_iterator itor = c_rSellingItemStock.begin(); itor < c_rSellingItemStock.end(); ++itor)
	{
		const TShopItemTable & c_rItem = *itor;
		if (!Send(sizeof(c_rItem), &c_rItem))
			return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvShopSignPacket()
{
	TPacketGCShopSign p;
	if (!Recv(sizeof(TPacketGCShopSign), &p))
		return false;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();

#if defined(__BL_GRAPHIC_ON_OFF__)
	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(p.dwVID);
	if (!pInstance)
		return true;
#endif

	if (0 == strlen(p.szSign))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_PrivateShop_Disappear",
			Py_BuildValue("(i)", p.dwVID)
		);

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.ClosePrivateShop();

#if defined(__BL_GRAPHIC_ON_OFF__)
		pInstance->GetGraphicThingInstancePtr()->RemovePrivateShopSign();
#endif
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_PrivateShop_Appear",
			Py_BuildValue("(is)", p.dwVID, p.szSign)
		);

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
			rkPlayer.OpenPrivateShop();

#if defined(__BL_GRAPHIC_ON_OFF__)
		pInstance->GetGraphicThingInstancePtr()->AddPrivateShopSign();
#endif
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////

bool CPythonNetworkStream::RecvTimePacket()
{
	TPacketGCTime TimePacket;
	if (!Recv(sizeof(TimePacket), &TimePacket))
		return false;

	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();
	rkApp.SetServerTime(TimePacket.time);

	return true;
}

bool CPythonNetworkStream::RecvWalkModePacket()
{
	TPacketGCWalkMode WalkModePacket;
	if (!Recv(sizeof(WalkModePacket), &WalkModePacket))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(WalkModePacket.vid);
	if (pInstance)
	{
		if (WALKMODE_RUN == WalkModePacket.mode)
		{
			pInstance->SetRunMode();
		}
		else
		{
			pInstance->SetWalkMode();
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSkillGroupPacket()
{
	TPacketGCChangeSkillGroup ChangeSkillGroup;
	if (!Recv(sizeof(ChangeSkillGroup), &ChangeSkillGroup))
		return false;

	m_dwMainActorSkillGroup = ChangeSkillGroup.skill_group;

	CPythonPlayer::Instance().NEW_ClearSkillData();
	__RefreshCharacterWindow();
	return true;
}

void CPythonNetworkStream::__TEST_SetSkillGroupFake(int iIndex)
{
	m_dwMainActorSkillGroup = DWORD(iIndex);

	CPythonPlayer::Instance().NEW_ClearSkillData();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
}

bool CPythonNetworkStream::SendRefinePacket(WORD wPos, BYTE byType)
{
	TPacketCGRefine kRefinePacket;
	kRefinePacket.header = HEADER_CG_REFINE;
	kRefinePacket.pos = wPos;
	kRefinePacket.type = byType;

	if (!Send(sizeof(kRefinePacket), &kRefinePacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSelectItemPacket(DWORD dwItemPos)
{
	TPacketCGScriptSelectItem kScriptSelectItem;
	kScriptSelectItem.header = HEADER_CG_SCRIPT_SELECT_ITEM;
	kScriptSelectItem.selection = dwItemPos;

	if (!Send(sizeof(kScriptSelectItem), &kScriptSelectItem))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvRefineInformationPacket()
{
	TPacketGCRefineInformation kRefineInfoPacket;
	if (!Recv(sizeof(kRefineInfoPacket), &kRefineInfoPacket))
		return false;

	TRefineTable & rkRefineTable = kRefineInfoPacket.refine_table;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"OpenRefineDialog",
		Py_BuildValue("(iiii)",
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.prob));

	for (int i = 0; i < rkRefineTable.material_count; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));
	}

#ifdef _DEBUG
	Tracef(" >> RecvRefineInformationPacket(pos=%d, result_vnum=%d, cost=%d, prob=%d)\n",
														kRefineInfoPacket.pos,
														kRefineInfoPacket.refine_table.result_vnum,
														rkRefineTable.cost,
														rkRefineTable.prob);
#endif

	return true;
}

bool CPythonNetworkStream::RecvRefineInformationPacketNew()
{
	TPacketGCRefineInformationNew kRefineInfoPacket;
	if (!Recv(sizeof(kRefineInfoPacket), &kRefineInfoPacket))
		return false;

	TRefineTable & rkRefineTable = kRefineInfoPacket.refine_table;
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"OpenRefineDialog",
		Py_BuildValue("(iiiii)",
			kRefineInfoPacket.pos,
			kRefineInfoPacket.refine_table.result_vnum,
			rkRefineTable.cost,
			rkRefineTable.prob,
			kRefineInfoPacket.type)
		);

	for (int i = 0; i < rkRefineTable.material_count; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));
	}

#ifdef _DEBUG
	Tracef(" >> RecvRefineInformationPacketNew(pos=%d, result_vnum=%d, cost=%d, prob=%d, type=%d)\n",
														kRefineInfoPacket.pos,
														kRefineInfoPacket.refine_table.result_vnum,
														rkRefineTable.cost,
														rkRefineTable.prob,
														kRefineInfoPacket.type);
#endif

	return true;
}

bool CPythonNetworkStream::RecvNPCList()
{
	TPacketGCNPCPosition kNPCPosition;
	if (!Recv(sizeof(kNPCPosition), &kNPCPosition))
		return false;

	assert(int(kNPCPosition.size)-sizeof(kNPCPosition) == kNPCPosition.count*sizeof(TNPCPosition) && "HEADER_GC_NPC_POSITION");

	CPythonMiniMap::Instance().ClearAtlasMarkInfo();

	for (int i = 0; i < kNPCPosition.count; ++i)
	{
		TNPCPosition NPCPosition;
		if (!Recv(sizeof(TNPCPosition), &NPCPosition))
			return false;

		CPythonMiniMap::Instance().RegisterAtlasMark(NPCPosition.bType, NPCPosition.name, NPCPosition.x, NPCPosition.y);
	}

	return true;
}

bool CPythonNetworkStream::__SendCRCReportPacket()
{
	/*
	DWORD dwProcessCRC = 0;
	DWORD dwFileCRC = 0;
	CFilename exeFileName;
	//LPCVOID c_pvBaseAddress = NULL;

	GetExeCRC(dwProcessCRC, dwFileCRC);

	CFilename strRootPackFileName = CEterPackManager::Instance().GetRootPacketFileName();
	strRootPackFileName.ChangeDosPath();

	TPacketCGCRCReport kReportPacket;

	kReportPacket.header = HEADER_CG_CRC_REPORT;
	kReportPacket.byPackMode = CEterPackManager::Instance().GetSearchMode();
	kReportPacket.dwBinaryCRC32 = dwFileCRC;
	kReportPacket.dwProcessCRC32 = dwProcessCRC;
	kReportPacket.dwRootPackCRC32 = GetFileCRC32(strRootPackFileName.c_str());

	if (!Send(sizeof(kReportPacket), &kReportPacket))
		Tracef("SendClientReportPacket Error");

	return SendSequence();
	*/
	return true;
}

bool CPythonNetworkStream::SendClientVersionPacket()
{
	std::string filename;

	GetExcutedFileName(filename);

	filename = CFileNameHelper::NoPath(filename);
	CFileNameHelper::ChangeDosPath(filename);

	if (LocaleService_IsEUROPE() && false == LocaleService_IsYMIR())
	{
		TPacketCGClientVersion2 kVersionPacket;
		kVersionPacket.header = HEADER_CG_CLIENT_VERSION2;
		strncpy(kVersionPacket.filename, filename.c_str(), sizeof(kVersionPacket.filename)-1);
		strncpy(kVersionPacket.timestamp, "1215955205", sizeof(kVersionPacket.timestamp)-1);
		//strncpy(kVersionPacket.timestamp, __TIMESTAMP__, sizeof(kVersionPacket.timestamp)-1); // old_string_ver
		//strncpy(kVersionPacket.timestamp, "1218055205", sizeof(kVersionPacket.timestamp)-1); // new_future
		//strncpy(kVersionPacket.timestamp, "1214055205", sizeof(kVersionPacket.timestamp)-1); // old_past

		if (!Send(sizeof(kVersionPacket), &kVersionPacket))
			Tracef("SendClientReportPacket Error");
	}
	else
	{
		TPacketCGClientVersion kVersionPacket;
		kVersionPacket.header = HEADER_CG_CLIENT_VERSION;
		strncpy(kVersionPacket.filename, filename.c_str(), sizeof(kVersionPacket.filename)-1);
		strncpy(kVersionPacket.timestamp, __TIMESTAMP__, sizeof(kVersionPacket.timestamp)-1);

		if (!Send(sizeof(kVersionPacket), &kVersionPacket))
			Tracef("SendClientReportPacket Error");
	}
	return SendSequence();
}

bool CPythonNetworkStream::RecvAffectAddPacket()
{
	TPacketGCAffectAdd kAffectAdd;
	if (!Recv(sizeof(kAffectAdd), &kAffectAdd))
		return false;

	TPacketAffectElement & rkElement = kAffectAdd.elem;
	if (rkElement.bPointIdxApplyOn == POINT_ENERGY)
	{
		CPythonPlayer::instance().SetStatus (POINT_ENERGY_END_TIME, CPythonApplication::Instance().GetServerTimeStamp() + rkElement.lDuration);
		__RefreshStatus();
	}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	if (rkElement.dwType == CInstanceBase::NEW_AFFECT_PREMIUM_PRIVATE_SHOP)
	{
		CPythonPrivateShop::Instance().SetPremiumTime(CPythonApplication::Instance().GetServerTimeStamp() + rkElement.lDuration);
		__RefreshPrivateShopWindow();
	}
#endif

#ifdef ENABLE_METIN_STONE_QUEUE
	if (rkElement.dwType == CInstanceBase::NEW_AFFECT_METIN_QUEUE)
	{
		CPythonPlayer::Instance().SetMetinstonesQueueCount(8);
	}
#endif

#ifdef ENABLE_BATTLE_PASS
	if (rkElement.dwType == CInstanceBase::NEW_AFFECT_BATTLE_PASS)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ShowBattlePassButton", Py_BuildValue("(ii)", rkElement.lApplyValue, rkElement.lDuration));
	}
#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_AddAffect", Py_BuildValue("(iiii)", rkElement.dwType, rkElement.bPointIdxApplyOn, rkElement.lApplyValue, rkElement.lDuration));
#ifdef ENABLE_PLAYER_MODULE_CHECK_AFFECT
	CPythonPlayer::instance().AddAffect(rkElement.dwType, kAffectAdd.elem);
#endif

	return true;
}

bool CPythonNetworkStream::RecvAffectRemovePacket()
{
	TPacketGCAffectRemove kAffectRemove;
	if (!Recv(sizeof(kAffectRemove), &kAffectRemove))
		return false;

#ifdef ENABLE_METIN_STONE_QUEUE
	if (kAffectRemove.dwType == CInstanceBase::NEW_AFFECT_METIN_QUEUE)
	{
		CPythonPlayer::Instance().SetMetinstonesQueueCount(3);
	}
#endif

#ifdef ENABLE_BATTLE_PASS
	if (kAffectRemove.dwType == CInstanceBase::NEW_AFFECT_BATTLE_PASS)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_HideBattlePassButton", Py_BuildValue("()"));
	}
#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_RemoveAffect", Py_BuildValue("(ii)", kAffectRemove.dwType, kAffectRemove.bApplyOn));
#ifdef ENABLE_PLAYER_MODULE_CHECK_AFFECT
	CPythonPlayer::instance().RemoveAffect(kAffectRemove.dwType, kAffectRemove.bApplyOn);
#endif

	return true;
}

bool CPythonNetworkStream::RecvChannelPacket()
{
	TPacketGCChannel kChannelPacket;
	if (!Recv(sizeof(kChannelPacket), &kChannelPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_CurrentChannel", Py_BuildValue("(i)", kChannelPacket.channel));
#ifdef ENABLE_ANTI_EXP
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetAntiExp", Py_BuildValue("(i)", kChannelPacket.bAntiExp));
#endif

	return true;
}

bool CPythonNetworkStream::RecvViewEquipPacket()
{
	TPacketGCViewEquip kViewEquipPacket;
	if (!Recv(sizeof(kViewEquipPacket), &kViewEquipPacket))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenEquipmentDialog", Py_BuildValue("(i)", kViewEquipPacket.dwVID));

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		TEquipmentItemSet & rItemSet = kViewEquipPacket.equips[i];
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogItem", Py_BuildValue("(iiii)", kViewEquipPacket.dwVID, i, rItemSet.vnum, rItemSet.count));

		for (int j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; ++j)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogSocket", Py_BuildValue("(iiii)", kViewEquipPacket.dwVID, i, j, rItemSet.alSockets[j]));

		for (int k = 0; k < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++k)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogAttr", Py_BuildValue("(iiiii)", kViewEquipPacket.dwVID, i, k, rItemSet.aAttr[k].bType, rItemSet.aAttr[k].sValue));
	}

	return true;
}

bool CPythonNetworkStream::RecvLandPacket()
{
	TPacketGCLandList kLandList;
	if (!Recv(sizeof(kLandList), &kLandList))
		return false;

	std::vector<DWORD> kVec_dwGuildID;

	CPythonMiniMap & rkMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkBG = CPythonBackground::Instance();
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();

	rkMiniMap.ClearGuildArea();
	rkBG.ClearGuildArea();

	int iPacketSize = (kLandList.size - sizeof(TPacketGCLandList));
	for (; iPacketSize > 0; iPacketSize-=sizeof(TLandPacketElement))
	{
		TLandPacketElement kElement;
		if (!Recv(sizeof(TLandPacketElement), &kElement))
			return false;

		rkMiniMap.RegisterGuildArea(kElement.dwID,
									kElement.dwGuildID,
									kElement.x,
									kElement.y,
									kElement.width,
									kElement.height);

		if (pMainInstance)
		if (kElement.dwGuildID == pMainInstance->GetGuildID())
		{
			rkBG.RegisterGuildArea(kElement.x,
								   kElement.y,
								   kElement.x+kElement.width,
								   kElement.y+kElement.height);
		}

		if (0 != kElement.dwGuildID)
			kVec_dwGuildID.push_back(kElement.dwGuildID);
	}
	// @fixme006
	if (kVec_dwGuildID.size()>0)
		__DownloadSymbol(kVec_dwGuildID);

	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacket()
{
	TPacketGCTargetCreate kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

#if defined(__BL_CLIENT_LOCALE_STRING__)
	CPythonLocale::Instance().FormatString(kTargetCreate.szTargetName, sizeof(kTargetCreate.szTargetName));
#endif

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);

//#ifdef _DEBUG
//	char szBuf[256+1];
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetCreatePacket %d : %s\n", kTargetCreate.lID, kTargetCreate.szTargetName);
//#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacketNew()
{
	TPacketGCTargetCreateNew kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
		return false;

#if defined(__BL_CLIENT_LOCALE_STRING__)
	CPythonLocale::Instance().FormatString(kTargetCreate.szTargetName, sizeof(kTargetCreate.szTargetName));
#endif

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	if (CREATE_TARGET_TYPE_LOCATION == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);
	}
	else
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreateTargetEffect(kTargetCreate.lID, kTargetCreate.dwVID);
	}

//#ifdef _DEBUG
//	char szBuf[256+1];
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetCreatePacketNew %d : %d/%d\n", kTargetCreate.lID, kTargetCreate.byType, kTargetCreate.dwVID);
//#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetUpdatePacket()
{
	TPacketGCTargetUpdate kTargetUpdate;
	if (!Recv(sizeof(kTargetUpdate), &kTargetUpdate))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.UpdateTarget(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.CreateTargetEffect(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

//#ifdef _DEBUG
//	char szBuf[256+1];
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetUpdatePacket %d : %d, %d\n", kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
//#endif

	return true;
}

bool CPythonNetworkStream::RecvTargetDeletePacket()
{
	TPacketGCTargetDelete kTargetDelete;
	if (!Recv(sizeof(kTargetDelete), &kTargetDelete))
		return false;

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.DeleteTarget(kTargetDelete.lID);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.DeleteTargetEffect(kTargetDelete.lID);

//#ifdef _DEBUG
//	Tracef(" >> RecvTargetDeletePacket %d\n", kTargetDelete.lID);
//#endif

	return true;
}

bool CPythonNetworkStream::RecvLoverInfoPacket()
{
	TPacketGCLoverInfo kLoverInfo;
	if (!Recv(sizeof(kLoverInfo), &kLoverInfo))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_LoverInfo", Py_BuildValue("(si)", kLoverInfo.szName, kLoverInfo.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVER INFO : %s, %d\n", kLoverInfo.szName, kLoverInfo.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvLovePointUpdatePacket()
{
	TPacketGCLovePointUpdate kLovePointUpdate;
	if (!Recv(sizeof(kLovePointUpdate), &kLovePointUpdate))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateLovePoint", Py_BuildValue("(i)", kLovePointUpdate.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVE POINT UPDATE : %d\n", kLovePointUpdate.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvDigMotionPacket()
{
	TPacketGCDigMotion kDigMotion;
	if (!Recv(sizeof(kDigMotion), &kDigMotion))
		return false;

#ifdef _DEBUG
	Tracef(" Dig Motion [%d/%d]\n", kDigMotion.vid, kDigMotion.count);
#endif

	IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();
	CInstanceBase * pkInstMain = rkChrMgr.GetInstancePtr(kDigMotion.vid);
	CInstanceBase * pkInstTarget = rkChrMgr.GetInstancePtr(kDigMotion.target_vid);
	if (NULL == pkInstMain)
		return true;

	if (pkInstTarget)
		pkInstMain->NEW_LookAtDestInstance(*pkInstTarget);

	for (int i = 0; i < kDigMotion.count; ++i)
		pkInstMain->PushOnceMotion(CRaceMotionData::NAME_DIG);

	return true;
}

bool CPythonNetworkStream::SendDragonSoulRefinePacket(BYTE bRefineType, TItemPos* pos)
{
	TPacketCGDragonSoulRefine pk;
	pk.header = HEADER_CG_DRAGON_SOUL_REFINE;
	pk.bSubType = bRefineType;
	memcpy (pk.ItemGrid, pos, sizeof (TItemPos) * DS_REFINE_WINDOW_MAX_NUM);
	if (!Send(sizeof (pk), &pk))
	{
		return false;
	}
	return true;
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
bool CPythonNetworkStream::RecvAccePacket(bool bReturn)
{
	TPacketAcce sPacket;
	if (!Recv(sizeof(sPacket), &sPacket))
		return bReturn;

	bReturn = true;
	switch (sPacket.subheader)
	{
		case ACCE_SUBHEADER_GC_OPEN:
			CPythonAcce::Instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 1, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_GC_CLOSE:
			CPythonAcce::Instance().Clear();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 2, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_GC_ADDED:
			CPythonAcce::Instance().AddMaterial(sPacket.dwPrice, sPacket.bPos, sPacket.tPos);
			if (sPacket.bPos == 1)
			{
				CPythonAcce::Instance().AddResult(sPacket.dwItemVnum, sPacket.dwMinAbs, sPacket.dwMaxAbs);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AlertAcce", Py_BuildValue("(b)", sPacket.bWindow));
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 3, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_GC_REMOVED:
			CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, sPacket.bPos);
			if (sPacket.bPos == 0)
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 1);

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 4, sPacket.bWindow));
			break;

		case ACCE_SUBHEADER_CG_REFINED:
			if (sPacket.dwMaxAbs == 0)
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 1);
			else
			{
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 0);
				CPythonAcce::Instance().RemoveMaterial(sPacket.dwPrice, 1);
			}

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ActAcce", Py_BuildValue("(ib)", 4, sPacket.bWindow));
			break;

		default:
			TraceError("CPythonNetworkStream::RecvAccePacket: unknown subheader %d\n.", sPacket.subheader);
			break;
	}

	return bReturn;
}

bool CPythonNetworkStream::SendAcceClosePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type	= INVENTORY;
	tPos.cell			= 0;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_CLOSE;
	sPacket.dwPrice		= 0;
	sPacket.bPos		= 0;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceAddPacket(TItemPos tPos, BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_ADD;
	sPacket.dwPrice		= 0;
	sPacket.bPos		= bPos;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRemovePacket(BYTE bPos)
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_REMOVE;
	sPacket.dwPrice		= 0;
	sPacket.bPos		= bPos;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendAcceRefinePacket()
{
	if (!__CanActMainInstance())
		return true;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header		= HEADER_CG_ACCE;
	sPacket.subheader	= ACCE_SUBHEADER_CG_REFINE;
	sPacket.dwPrice		= 0;
	sPacket.bPos		= 0;
	sPacket.tPos		= tPos;
	sPacket.dwItemVnum	= 0;
	sPacket.dwMinAbs	= 0;
	sPacket.dwMaxAbs	= 0;

	if (!Send(sizeof(sPacket), &sPacket))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_DISCORD_RPC
#include "Discord.h"
#include <discord_rpc.h>

#ifdef _DEBUG
#pragma comment(lib, "discord_rpc_d.lib")
#else
#pragma comment(lib, "discord_rpc_r.lib")
#endif

static int64_t DiscordStartTime{};

void CPythonNetworkStream::Discord_Start()
{
	DiscordStartTime = time(0);
	DiscordEventHandlers handlers{};
	Discord_Initialize(Discord::DiscordClientID, &handlers, 1, nullptr);
	Discord_Update(false);
}

void CPythonNetworkStream::Discord_Update(const bool ingame)
{
	DiscordRichPresence discordPresence{};
	discordPresence.startTimestamp = DiscordStartTime;

#ifdef ENABLE_DISCORD_JOIN_URL
	discordPresence.buttonLabel = "Register";
	discordPresence.buttonURL = "https://hardcoremt2.de/";
#endif

	if (!ingame)
	{
		Discord_UpdatePresence(&discordPresence);
		return;
	}

	/*Name*/
	auto NameData = Discord::GetNameData();
	discordPresence.state = std::get<0>(NameData).c_str();
	discordPresence.details = std::get<1>(NameData).c_str();

	/*Race*/
	auto RaceData = Discord::GetRaceData();
	discordPresence.largeImageKey = std::get<0>(RaceData).c_str();
	discordPresence.largeImageText = std::get<1>(RaceData).c_str();

	/*Empire*/
	auto EmpireData = Discord::GetEmpireData();
	discordPresence.smallImageKey = std::get<0>(EmpireData).c_str();
	discordPresence.smallImageText = std::get<1>(EmpireData).c_str();

	Discord_UpdatePresence(&discordPresence);
}

void CPythonNetworkStream::Discord_Close()
{
	Discord_Shutdown();
}
#endif


#ifdef ENABLE_SWITCHBOT_SYSTEM
bool CPythonNetworkStream::RecvSwitchbotPacket()
{
	TPacketGCSwitchbot pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	size_t packet_size = int(pack.size) - sizeof(TPacketGCSwitchbot);

	// size_t packet_size = static_cast<size_t>(pack.size) - sizeof(TPacketGCSwitchbot);
	if (pack.subheader == SUBHEADER_GC_SWITCHBOT_UPDATE)
	{
		if (packet_size != sizeof(CPythonSwitchbot::TSwitchbotTable))
		{
			return false;
		}

		CPythonSwitchbot::TSwitchbotTable table;
		if (!Recv(sizeof(table), &table))
		{
			return false;
		}

		CPythonSwitchbot::Instance().Update(table);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotWindow", Py_BuildValue("()"));
	}
	else if (pack.subheader == SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM)
	{
		if (packet_size != sizeof(TSwitchbotUpdateItem))
		{
			return false;
		}

		TSwitchbotUpdateItem update;
		if (!Recv(sizeof(update), &update))
		{
			return false;
		}

		TItemPos pos(SWITCHBOT, update.slot);

		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
		rkPlayer.SetItemCount(pos, update.count);

		for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		{
			rkPlayer.SetItemMetinSocket(pos, i, update.alSockets[i]);
		}

		for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		{
			rkPlayer.SetItemAttribute(pos, j, update.aAttr[j].bType, update.aAttr[j].sValue);
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotItem", Py_BuildValue("(i)", update.slot));
		return true;
	}
	else if (pack.subheader == SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION)
	{
		CPythonSwitchbot::Instance().ClearAttributeMap();

		size_t table_size = sizeof(CPythonSwitchbot::TSwitchbottAttributeTable);
		while (packet_size >= table_size)
		{
			const int test = sizeof(CPythonSwitchbot::TSwitchbottAttributeTable);

			CPythonSwitchbot::TSwitchbottAttributeTable table;
			if (!Recv(table_size, &table))
			{
				return false;
			}
			//# GRM Update 16112023
			if (table.bIsRareAttr)
				CPythonSwitchbot::Instance().AddAttributeRareToMap(table);
			else
				CPythonSwitchbot::Instance().AddAttributeToMap(table);
			packet_size -= table_size;
		}
	}

	return true;
}

bool CPythonNetworkStream::SendSwitchbotStartPacket(BYTE slot, std::vector<CPythonSwitchbot::TSwitchbotAttributeAlternativeTable> alternatives)
{
	TPacketCGSwitchbot pack;
	pack.header = HEADER_CG_SWITCHBOT;
	pack.subheader = SUBHEADER_CG_SWITCHBOT_START;
	pack.size = sizeof(TPacketCGSwitchbot) + sizeof(CPythonSwitchbot::TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
	pack.slot = slot;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	for (const auto& it : alternatives)
	{
		if (!Send(sizeof(it), &it))
		{
			return false;
		}
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSwitchbotStopPacket(BYTE slot)
{
	TPacketCGSwitchbot pack;
	pack.header = HEADER_CG_SWITCHBOT;
	pack.subheader = SUBHEADER_CG_SWITCHBOT_STOP;
	pack.size = sizeof(TPacketCGSwitchbot);
	pack.slot = slot;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	return SendSequence();
}
#endif


#ifdef ENABLE_DROP_INFO
bool CPythonNetworkStream::SendDropInfoPacket()
{
	if (!__CanActMainInstance())
	{
		return false;
	}

	BYTE header = HEADER_CG_DROP_INFO;
	if (!Send(sizeof(BYTE), &header))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvDropInfoPacket()
{
	TPacketDropInfo packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	if (packet.size - sizeof(TPacketDropInfo) < packet.item_count * sizeof(TPacketDropInfoItem))
	{
		TraceError("RecvDropInfoPacket: buffer is to low to receive %d items", packet.item_count);
		return false;
	}

	for (int i = 0; i < packet.item_count; i++)
	{
		TPacketDropInfoItem item;
		if (!Recv(sizeof(TPacketDropInfoItem), &item))
		{
			return false;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DropInfoAppendItem", Py_BuildValue("(iiii)",
							  packet.mob_vnum, item.dwVnum, item.byMinCount, item.byMaxCount));

	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DropInfoAppendItem", Py_BuildValue("(iiii)",
						  packet.mob_vnum, 1, packet.gold_min, packet.gold_max));

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DropInfoRefresh", Py_BuildValue("(i)", packet.mob_vnum));

	return true;
}
#endif


#ifdef ENABLE_EVENT_MANAGER
bool CPythonNetworkStream::SendRequestEventQuest(const char *c_szString)
{
	TPacketCGRequestEventQuest Packet;
	Packet.bHeader = HEADER_CG_REQUEST_EVENT_QUEST;
	strncpy(Packet.szName, c_szString, QUEST_NAME_MAX_NUM);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendRequestEventQuest Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendRequestEventData(int iMonth)
{
	// Return if data has been already requested
	if (InGameEventManager::Instance().GetRequestEventData())
	{
		return false;
	}

	SPacketCGRequestEventData p;
	p.bHeader = HEADER_CG_REQUEST_EVENT_DATA;
	p.bMonth = BYTE(iMonth);

	if (!Send(sizeof(p), &p))
	{
		Tracen("Send SendRequestEventData Packet Error");
		return false;
	}

	Tracef("SendRequestEventData\n");
	return SendSequence();
}

bool CPythonNetworkStream::RecvEventInformation()
{
	TPacketGCEventInfo p;

	if (!Recv(sizeof(TPacketGCEventInfo), &p))
	{
		TraceError("CPythonNetworkStream::RecvEventInformation TPacketGCEventInfo Error");
		return false;
	}

	time_t serverTimeStamp = CPythonApplication::Instance().GetServerTimeStamp();
	time_t clientTimeStamp = time(0);
	time_t deltaTime = clientTimeStamp - serverTimeStamp;

	std::vector<InGameEventManager::TEventTable> eventVec;

	int iPacketSize = (p.wSize - sizeof(TPacketGCEventInfo));
	for (; iPacketSize > 0; iPacketSize -= sizeof(TPacketEventData))
	{
		TPacketEventData kElement;
		if (!Recv(sizeof(TPacketEventData), &kElement))
		{
			TraceError("CPythonNetworkStream::RecvEventInformation TPacketEventData Error");
			return false;
		}

		InGameEventManager::TEventTable table;
		table.dwID = kElement.dwID;
		table.bType = kElement.bType;
		table.startTime = kElement.startTime + deltaTime;
		table.endTime = kElement.endTime + deltaTime;
		table.iValue0 = kElement.iValue0;
		table.iValue1 = kElement.iValue1;
		table.bCompleted = kElement.bCompleted;

		eventVec.push_back(table);

	}

	InGameEventManager::Instance().SetRequestEventData(true);
	InGameEventManager::Instance().AddEventData(eventVec);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenInGameEvent", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvEventReload()
{
	TPacketGCEventReload p;

	if (!Recv(sizeof(TPacketGCEventReload), &p))
	{
		TraceError("CPythonNetworkStream::RecvEventReload TPacketGCEventReload Error");
		return false;
	}

	InGameEventManager::Instance().SetRequestEventData(false);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_CloseInGameEvent", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvEventKWScore()
{
	TPacketGCEventKWScore p;

	if (!Recv(sizeof(TPacketGCEventKWScore), &p))
	{
		TraceError("CPythonNetworkStream::RecvEventKWScore TPacketGCEventKWScore Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_KingdomWarScore", Py_BuildValue("(iii)", p.wKingdomScores[0], p.wKingdomScores[1], p.wKingdomScores[2]));
	return true;
}
#endif

#ifdef ENABLE_GEM_SYSTEM
bool CPythonNetworkStream::RecvGemShopOpen()
{
	TPacketGCGemShopOpen kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
	{
		TraceError("CPythonNetworkStream::RecvGemShopOpen Error");
		return false;
	}

	CPythonPlayer::Instance().ClearGemShopItemVector();
	for (int i = 0; i < GEM_SLOTS_MAX_NUM; ++i)
	{
		TGemShopItem GemItem;
		GemItem.slotIndex = kPacket.shopItems[i].slotIndex;
		GemItem.dwVnum = kPacket.shopItems[i].dwVnum;
		GemItem.bCount = kPacket.shopItems[i].bCount;
		GemItem.dwPrice = kPacket.shopItems[i].dwPrice;

		CPythonPlayer::Instance().SetGemShopItemData(kPacket.shopItems[i].slotIndex, GemItem);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenGemShop", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::SendGemShopBuy(BYTE bPos)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGGemShop PacketGemShop;
	PacketGemShop.header = HEADER_CG_GEM_SHOP;
	PacketGemShop.subheader = GEM_SHOP_SUBHEADER_CG_BUY;

	if (!Send(sizeof(TPacketCGGemShop), &PacketGemShop))
	{
		Tracef("SendGemShopBuyPacket Error\n");
		return false;
	}

	if (!Send(sizeof(BYTE), &bPos))
	{
		Tracef("SendGemShopBuyPacket Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_INGAME_WIKI
extern PyObject* wikiModule;

bool CPythonNetworkStream::SendWikiRequestInfo(unsigned long long retID, DWORD vnum, bool isMob)
{
	InGameWiki::TCGWikiPacket pack;
	pack.vnum = vnum;
	pack.is_mob = isMob;
	pack.ret_id = retID;
	
	if (!Send(sizeof(InGameWiki::TCGWikiPacket), &pack))
		return false;
	
	return true;
}

bool CPythonNetworkStream::RecvWikiPacket()
{
	InGameWiki::TGCWikiPacket pack;
	if (!Recv(sizeof(InGameWiki::TGCWikiPacket), &pack))
		return false;
	
	WORD iPacketSize = pack.size - sizeof(InGameWiki::TGCWikiPacket);
	if (iPacketSize <= 0)
		return false;
	
	unsigned long long ret_id = 0;
	DWORD data_vnum = 0;
	
	if (pack.is_data_type(InGameWiki::LOAD_WIKI_ITEM))
	{
		const size_t recv_size = sizeof(InGameWiki::TGCItemWikiPacket);
		iPacketSize -= WORD(recv_size);
		
		InGameWiki::TGCItemWikiPacket item_data;
		if (!Recv(recv_size, &item_data))
			return false;
		
		ret_id = item_data.ret_id;
		data_vnum = item_data.vnum;
		
		CItemData* pData = nullptr;
		if (!CItemManager::instance().GetItemDataPointer(item_data.vnum, &pData)) {
			TraceError("Cant get pointer from item -> %d", item_data.vnum);
			return false;
		}
		
		auto& recv_wiki = item_data.wiki_info;
		auto wikiInfo = pData->GetWikiTable();
		
		const int origin_size = item_data.origin_infos_count;
		const int chest_info_count = recv_wiki.chest_info_count;
		const int refine_infos_count = recv_wiki.refine_infos_count;
		
		wikiInfo->isSet = true;
		wikiInfo->hasData = true;
		wikiInfo->bIsCommon = recv_wiki.is_common;
		wikiInfo->dwOrigin = recv_wiki.origin_vnum;
		wikiInfo->maxRefineLevel = refine_infos_count;
		
		{
			wikiInfo->pOriginInfo.clear();
			const size_t origin_info_recv_base_size = sizeof(CommonWikiData::TWikiItemOriginInfo);
			
			for (int idx = 0; idx < origin_size; ++idx) {
				CommonWikiData::TWikiItemOriginInfo origin_data;
				if (!Recv(origin_info_recv_base_size, &origin_data))
					return false;
				
				wikiInfo->pOriginInfo.emplace_back(origin_data);
				iPacketSize -= WORD(origin_info_recv_base_size);
			}
			
			
			wikiInfo->pChestInfo.clear();
			const size_t chest_info_recv_base_size = sizeof(CommonWikiData::TWikiChestInfo);
			
			for (int idx = 0; idx < chest_info_count; ++idx) {
				CommonWikiData::TWikiChestInfo chest_data;
				if (!Recv(chest_info_recv_base_size, &chest_data))
					return false;
				
				wikiInfo->pChestInfo.emplace_back(chest_data);
				iPacketSize -= WORD(chest_info_recv_base_size);
			}
			
			wikiInfo->pRefineData.clear();
			const size_t refine_info_recv_base_size = sizeof(CommonWikiData::TWikiRefineInfo);
			
			for (int idx = 0; idx < refine_infos_count; ++idx) {
				CommonWikiData::TWikiRefineInfo refine_info_data;
				if (!Recv(refine_info_recv_base_size, &refine_info_data))
					return false;
				
				wikiInfo->pRefineData.emplace_back(refine_info_data);
				iPacketSize -= WORD(refine_info_recv_base_size);
			}
		}
		
		if (iPacketSize != 0)
			return false;
	}
	else
	{
		const size_t recv_size = sizeof(InGameWiki::TGCMobWikiPacket);
		iPacketSize -= WORD(recv_size);
		
		InGameWiki::TGCMobWikiPacket mob_data;
		if (!Recv(recv_size, &mob_data))
			return false;
		
		ret_id = mob_data.ret_id;
		data_vnum = mob_data.vnum;
		const int drop_info_count = mob_data.drop_info_count;
		
		CPythonNonPlayer::TWikiInfoTable* mobData = nullptr;
		if (!(mobData = CPythonNonPlayer::instance().GetWikiTable(mob_data.vnum))) {
			TraceError("Cant get mob data from monster -> %d", mob_data.vnum);
			return false;
		}
		
		mobData->isSet = (drop_info_count > 0);
		
		{
			mobData->dropList.clear();
			const size_t mob_drop_info_recv_base_size = sizeof(CommonWikiData::TWikiMobDropInfo);
			
			for (int idx = 0; idx < drop_info_count; ++idx) {
				CommonWikiData::TWikiMobDropInfo drop_data;
				if (!Recv(mob_drop_info_recv_base_size, &drop_data))
					return false;
				
				mobData->dropList.push_back(drop_data);
				iPacketSize -= WORD(mob_drop_info_recv_base_size);
			}
		}
		
		if (iPacketSize != 0)
			return false;
	}
	
	if (wikiModule)
		PyCallClassMemberFunc(wikiModule, "BINARY_LoadInfo", Py_BuildValue("(Li)", (long long)ret_id, data_vnum));
	
	return true;
}
#endif


#ifdef ENABLE_BIOLOG_SYSTEM
bool CPythonNetworkStream::SendBiologManagerAction(BYTE bSubHeader)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGBiologManagerAction packet;
	packet.bHeader = HEADER_CG_BIOLOG_MANAGER;
	packet.bSubHeader = bSubHeader;

	if (!Send(sizeof(TPacketCGBiologManagerAction), &packet))
	{
		Tracef("SendBiologManagerAction Send Packet Error\n");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvBiologManager()
{
	TPacketGCBiologManager packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("RecvBiologManager Error");
		return false;
	}

	switch (packet.bSubHeader)
	{
	case GC_BIOLOG_MANAGER_OPEN:
	{
		TPacketGCBiologManagerInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		CPythonBiologManager::Instance()._LoadBiologInformation(&kInfo);
		if (kInfo.bUpdate)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BiologManagerUpdate", Py_BuildValue("()"));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BiologManagerOpen", Py_BuildValue("()"));
		}
	}
	break;
	case GC_BIOLOG_MANAGER_ALERT:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BiologManagerAlert", Py_BuildValue("()"));
	}
	break;
	case GC_BIOLOG_MANAGER_CLOSE:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BiologManagerClose", Py_BuildValue("()"));
	}
	break;
	}
	return true;
}
#endif


#ifdef ENABLE_SKILL_GROUP_GUI
bool CPythonNetworkStream::RecvSkillGroup()
{
	TPacketGCSkillGroup packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Skill_Group", Py_BuildValue("(i)", packet.bJob));
	return true;
}

bool CPythonNetworkStream::SendSkillGroupPacket(BYTE bGroup)
{
	TPacketCGSkillGroup packet;
	packet.bHeader = HEADER_CG_SKILL_GROUP_SELECT;
	packet.bGroup = bGroup;

	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
bool CPythonNetworkStream::SendBuildPrivateShopPacket(const char* c_szTitle, DWORD dwPolyVnum, BYTE bTitleType, BYTE bPageCount, const std::vector<TPrivateShopItem>& c_vec_itemStock)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_BUILD;

	TPacketCGPrivateShopBuild subPacket{};
	strncpy(subPacket.szTitle, c_szTitle, TITLE_MAX_LEN);
	subPacket.dwPolyVnum = dwPolyVnum;
	subPacket.bTitleType = bTitleType;
	subPacket.bPageCount = bPageCount;
	subPacket.wItemCount = static_cast<WORD>(c_vec_itemStock.size());

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	for (const auto& c_rShopItem : c_vec_itemStock)
	{
		if (!Send(sizeof(c_rShopItem), &c_rShopItem))
			return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendClosePrivateShopPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_CLOSE;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendOpenPrivateShopPanelPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendClosePrivateShopPanelPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_PANEL_CLOSE;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopStartPacket(DWORD dwVID)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_START;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(dwVID), &dwVID))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopEndPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_END;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopBuyPacket(WORD wPos)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_BUY;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(wPos), &wPos))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendWithdrawPrivateShopPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_WITHDRAW;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendModifyPrivateShopPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_MODIFY;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendItemPriceChangePrivateShopPacket(WORD wPos, long long llGold, DWORD dwCheque)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_ITEM_PRICE_CHANGE;

	TPacketCGPrivateShopItemPriceChange subPacket{};
	subPacket.wPos = wPos;
	subPacket.TPrice.llGold = llGold;
	subPacket.TPrice.dwCheque = dwCheque;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendItemMovePrivateShopPacket(WORD wPos, WORD wChangePos)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_ITEM_MOVE;

	TPacketCGPrivateShopItemMove subPacket{};
	subPacket.wPos = wPos;
	subPacket.wChangePos = wChangePos;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendItemCheckinPrivateShopPacket(WORD wSrcPos, WORD wSrcWindow, long long llGold, DWORD dwCheque, int iDstPos /* = -1 */)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKIN;

	TPacketCGPrivateShopItemCheckin subPacket{};
	subPacket.TSrcPos.cell = wSrcPos;
	subPacket.TSrcPos.window_type = wSrcWindow;
	subPacket.TPrice.llGold = llGold;
	subPacket.TPrice.dwCheque = dwCheque;
	subPacket.iDstPos = iDstPos;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendItemCheckoutPrivateShopPacket(WORD wSrcPos, int iDstPos/* = -1 */)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKOUT;

	TPacketCGPrivateShopItemCheckout subPacket{};
	subPacket.wSrcPos = wSrcPos;
	subPacket.iDstPos = iDstPos;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendTitleChangePrivateShopPacket(const char* c_szTitle)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_TITLE_CHANGE;

	char szTitle[TITLE_MAX_LEN + 1];
	strncpy(szTitle, c_szTitle, TITLE_MAX_LEN);

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(szTitle), &szTitle))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopWarpRequest()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_WARP_REQUEST;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendClosePrivateShopSearchPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_SEARCH_CLOSE;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopSearchPacket(TPacketCGPrivateShopSearch* pPacket)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_SEARCH;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(TPacketCGPrivateShopSearch), pPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopSearchBuyPacket(int* pSelectedItem)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_SEARCH_BUY;

	TPacketCGPrivateShopSearchBuy subPacket{};

	for (int i = 0; i < SELECTED_ITEM_MAX_NUM; ++i, ++pSelectedItem)
	{
		if (*pSelectedItem < 0)
			break;

		const TPrivateShopSearchData* c_pItemData;
		if (!CPythonPrivateShop::Instance().GetSearchItemData(*pSelectedItem, &c_pItemData))
			return true;

		subPacket.aSelectedItems[i].dwShopID = c_pItemData->dwShopID;
		subPacket.aSelectedItems[i].wPos = c_pItemData->wPos;
		subPacket.aSelectedItems[i].TPrice.llGold = c_pItemData->TPrice.llGold;
		subPacket.aSelectedItems[i].TPrice.dwCheque = c_pItemData->TPrice.dwCheque;
	}

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopMarketItemPriceDataReqPacket()
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_REQUEST;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopMarketItemPriceReqPacket(DWORD dwVnum)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_REQUEST;

	DWORD subPacket = dwVnum;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(subPacket), &subPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendPrivateShopSlotUnlockRequest(WORD wSrcPos)
{
	TPacketCGPrivateShop mainPacket{};
	mainPacket.bHeader = HEADER_CG_PRIVATE_SHOP;
	mainPacket.bSubHeader = SUBHEADER_CG_PRIVATE_SHOP_SLOT_UNLOCK_REQUEST;

	if (!Send(sizeof(mainPacket), &mainPacket))
		return false;

	if (!Send(sizeof(wSrcPos), &wSrcPos))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvPrivateShop()
{
	TPacketGCPrivateShop mainPacket{};
	if (!Recv(sizeof(mainPacket), &mainPacket))
		return false;

	switch (mainPacket.bSubHeader)
	{
	case SUBHEADER_GC_PRIVATE_SHOP_ADD_ENTITY:
	{
		TPacketGCPrivateShopAddEntity subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopAddEntity), &subPacket))
			return false;

		CPythonPrivateShop::TPrivateShopInstance* pPrivateShopInstance = CPythonPrivateShop::Instance().CreatePrivateShopInstance(
			subPacket.dwVID, subPacket.dwVnum,
			subPacket.szName, subPacket.lX, subPacket.lY, subPacket.lZ);

		if (!pPrivateShopInstance)
			return true;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"AddPrivateShopTitleBoard",
			Py_BuildValue("(isi)", subPacket.dwVID, subPacket.szTitle, subPacket.bTitleType)
		);

	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_DEL_ENTITY:
	{
		TPacketGCPrivateShopDelEntity subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopDelEntity), &subPacket))
			return false;

		CPythonPrivateShop::Instance().DeletePrivateShopInstance(subPacket.dwVID);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"RemovePrivateShopTitleBoard",
			Py_BuildValue("(i)", subPacket.dwVID)
		);
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_TITLE:
	{
		TPacketGCPrivateShopTitle subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopTitle), &subPacket))
			return false;

		CPythonPlayer& rkPlayer = CPythonPlayer::Instance();

		if (0 == strlen(subPacket.szTitle))
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"RemovePrivateShopTitleBoard",
				Py_BuildValue("(i)", subPacket.dwVID)
			);
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
				"AddPrivateShopTitleBoard",
				Py_BuildValue("(isi)", subPacket.dwVID, subPacket.szTitle, subPacket.bTitleType)
			);
		}
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_LOAD:
	{
		TPacketGCPrivateShopLoad subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopLoad), &subPacket))
			return false;

		// Clear the stock if we were building a private shop
		CPythonPrivateShop::Instance().ClearPrivateShopStock();

		CPythonPrivateShop::Instance().SetGold(subPacket.llGold);
		CPythonPrivateShop::Instance().SetCheque(subPacket.dwCheque);
		CPythonPrivateShop::Instance().SetLocation(subPacket.lX, subPacket.lY, subPacket.bChannel);
		CPythonPrivateShop::Instance().SetMyTitle(subPacket.szTitle);
		CPythonPrivateShop::Instance().SetMyState(subPacket.bState);
		CPythonPrivateShop::Instance().SetMyPageCount(subPacket.bPageCount);
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SET_ITEM:
	{
		TPrivateShopItemData subPacket{};
		if (!Recv(sizeof(TPrivateShopItemData), &subPacket))
			return false;

		CPythonPrivateShop::Instance().SetItemData(subPacket, true);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SET_SALE:
	{
		TPrivateShopSaleData subPacket{};
		if (!Recv(sizeof(TPrivateShopSaleData), &subPacket))
			return false;

		CPythonPrivateShop::Instance().SetSaleItemData(subPacket);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_BALANCE_UPDATE:
	{
		TPacketGCPrivateShopBalanceUpdate subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopBalanceUpdate), &subPacket))
			return false;

		CPythonPrivateShop::Instance().ChangeGold(subPacket.TPrice.llGold);
		CPythonPrivateShop::Instance().ChangeCheque(subPacket.TPrice.dwCheque);

		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_OPEN_PANEL:
	{
		CPythonPrivateShop::Instance().SetMainPlayerPrivateShop(true);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_CLOSE_PANEL:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClosePrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_CLOSE:
	{
		CPythonPrivateShop::Instance().ClearMyPrivateShop();
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClosePrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_START:
	{
		TPacketGCPrivateShopOpen subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopOpen), &subPacket))
			return false;

		CPythonPrivateShop::Instance().Clear();
		CPythonPrivateShop::Instance().SetTitle(subPacket.szTitle);
		CPythonPrivateShop::Instance().SetState(subPacket.bState);
		CPythonPrivateShop::Instance().SetPageCount(subPacket.bPageCount);
		CPythonPrivateShop::Instance().SetUnlockedSlots(subPacket.wUnlockedSlots);
		CPythonPrivateShop::Instance().SetMainPlayerPrivateShop(false);

		for (WORD i = 0; i < PRIVATE_SHOP_HOST_ITEM_MAX_NUM; ++i)
		{
			if (subPacket.aItems[i].dwVnum)
				CPythonPrivateShop::Instance().SetItemData(subPacket.aItems[i], false);
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_END:
	{
		CPythonPrivateShop::Instance().Clear();
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClosePrivateShopPanel", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_REMOVE_ITEM:
	{
		WORD wPos;
		if (!Recv(sizeof(WORD), &wPos))
			return false;

		CPythonPrivateShop::Instance().RemoveItemData(wPos, false);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_REMOVE_MY_ITEM:
	{
		WORD wPos;
		if (!Recv(sizeof(WORD), &wPos))
			return false;

		CPythonPrivateShop::Instance().RemoveItemData(wPos, true);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_STATE_UPDATE:
	{
		TPacketGCPrivateStateUpdate subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateStateUpdate), &subPacket))
			return false;

		if (subPacket.bIsMainPlayerPrivateShop)
		{
			CPythonPrivateShop::Instance().SetMyState(subPacket.bState);

			// Request market prices when owner is trying to edit the private shop
			if (subPacket.bState == STATE_MODIFY && !CPythonPrivateShop::Instance().IsMarketItemPriceDataLoaded())
				SendPrivateShopMarketItemPriceDataReqPacket();
		}
		else
			CPythonPrivateShop::Instance().SetState(subPacket.bState);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PrivateShopStateUpdate", Py_BuildValue("()"));
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_WITHDRAW:
	{
		CPythonPrivateShop::Instance().SetGold(0);
		CPythonPrivateShop::Instance().SetCheque(0);

		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_ITEM_PRICE_CHANGE:
	{
		TPacketGCPrivateShopItemPriceChange subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopItemPriceChange), &subPacket))
			return false;

		CPythonPrivateShop::Instance().ChangeItemPrice(subPacket.wPos, subPacket.TPrice.llGold, subPacket.TPrice.dwCheque);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_ITEM_MOVE:
	{
		TPacketGCPrivateShopItemMove subPacket{};
		if (!Recv(sizeof(TPacketGCPrivateShopItemMove), &subPacket))
			return false;

		CPythonPrivateShop::Instance().MoveItem(subPacket.wPos, subPacket.wChangePos);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_ADD_ITEM:
	{
		TPrivateShopItemData subPacket{};
		if (!Recv(sizeof(TPrivateShopItemData), &subPacket))
			return false;

		CPythonPrivateShop::Instance().SetItemData(subPacket, false);
		__RefreshPrivateShopWindow();
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_TITLE_CHANGE:
	{
		char szTitle[TITLE_MAX_LEN + 1] = { 0 };
		if (!Recv(sizeof(szTitle), &szTitle))
			return false;

		CPythonPrivateShop::Instance().SetMyTitle(szTitle);
		__RefreshPrivateShopWindow();
	}break;

	case SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_LOOK_MODE:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPrivateShopSearch", Py_BuildValue("(i)", MODE_LOOK));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_TRADE_MODE:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenPrivateShopSearch", Py_BuildValue("(i)", MODE_TRADE));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SEARCH_RESULT:
	{

		WORD wSize = mainPacket.wSize - sizeof(mainPacket);
		if (!wSize)
			break;

		DWORD dwCurrentResult = CPythonPrivateShop::Instance().GetResultMaxCount();
		while (wSize)
		{
			TPrivateShopSearchData subPacket{};
			if (!Recv(sizeof(subPacket), &subPacket))
				return false;

			CPythonPrivateShop::Instance().SetSearchItemData(subPacket);

			wSize -= sizeof(TPrivateShopSearchData);
		}

		if (!CPythonPrivateShop::Instance().GetResultPage())
			CPythonPrivateShop::Instance().SetResultPage(1);

		CPythonPrivateShop::Instance().SortSearchResult();

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PrivateShopSearchRefresh", Py_BuildValue("()"));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_SEARCH_UPDATE:
	{
		TPacketGCPrivateShopSearchUpdate subPacket;
		if (!Recv(sizeof(subPacket), &subPacket))
			return false;

		if (subPacket.iSpecificItemPos < 0)
		{
			std::vector<WORD> vec_itemPositions;
			CPythonPrivateShop::Instance().GetSearchItemDataPos(subPacket.dwShopID, vec_itemPositions);
			CPythonPrivateShop::Instance().SetSearchItemDataState(subPacket.dwShopID, subPacket.bState);

			for (const auto& wPos : vec_itemPositions)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PrivateShopSearchUpdate", Py_BuildValue("(ii)", wPos, subPacket.bState));
			}
		}
		else
		{
			int iPos = CPythonPrivateShop::Instance().GetSearchItemDataPos(subPacket.dwShopID, subPacket.iSpecificItemPos);

			if (iPos != -1)
			{
				CPythonPrivateShop::Instance().SetSearchItemDataState(subPacket.dwShopID, subPacket.bState, subPacket.iSpecificItemPos);
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "PrivateShopSearchUpdate", Py_BuildValue("(ii)", iPos, subPacket.bState));
			}
		}
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_RESULT:
	{
		WORD wSize = mainPacket.wSize - sizeof(mainPacket);
		while (wSize)
		{
			TMarketItemPrice subPacket{};
			if (!Recv(sizeof(subPacket), &subPacket))
				return false;

			CPythonPrivateShop::Instance().SetMarketItemPrice(subPacket);

			wSize -= sizeof(TMarketItemPrice);
		}

		CPythonPrivateShop::Instance().SetMarketItemPriceDataLoaded(true);
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_RESULT:
	{
		TMarketItemPrice subPacket{};
		if (!Recv(sizeof(subPacket), &subPacket))
			return false;

		CPythonPrivateShop::Instance().SetMarketItemPrice(subPacket);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMarketItemPrice", Py_BuildValue("(Li)", subPacket.TPrice.llGold, subPacket.TPrice.dwCheque));
	} break;

	case SUBHEADER_GC_PRIVATE_SHOP_UNLOCKED_SLOTS_CHANGE:
	{
		WORD wUnlockedSlots;
		if (!Recv(sizeof(wUnlockedSlots), &wUnlockedSlots))
			return false;

		CPythonPrivateShop::Instance().SetUnlockedSlots(wUnlockedSlots);
	} break;
	}

	return true;
}
#endif

#if defined(ENABLE_REMOTE_SHOP)
bool CPythonNetworkStream::SendRemoteShopPacket(BYTE bIndex)
{
	TPacketCGRemoteShop p;
	p.header = HEADER_CG_REMOTE_SHOP;
	p.shop_index = bIndex;

	if (!Send(sizeof(p), &p))
		return false;

	return SendSequence();
}
#endif


#ifdef ENABLE_GOLD_MAX_EXTENDED
bool CPythonNetworkStream::RecvGoldChange()
{
	TPacketGCGoldChange GoldChange;

	if (!Recv(sizeof(TPacketGCGoldChange), &GoldChange))
	{
		Tracen("Recv Gold Change Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pInstance)
	{
		if (GoldChange.dwVID == pInstance->GetVirtualID())
		{
			CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
			rkPlayer.SetGold(GoldChange.value);
			// TraceError("GoldChange.amount: %d", GoldChange.amount);
			// TraceError("GoldChange.value: %llu", GoldChange.value);

			if (GoldChange.amount > 0)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickMoney", Py_BuildValue("(K)", GoldChange.amount));
			}
		}
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));

	return true;
}
#endif

#ifdef ENABLE_ATLASS_EXTENDED
bool CPythonNetworkStream::RecvBossList()
{
	TPacketGCBossPosition kBossPosition;
	if (!Recv(sizeof(kBossPosition), &kBossPosition))
		return false;

	assert(int(kBossPosition.size) - sizeof(kBossPosition) == kBossPosition.count * sizeof(TBossPosition) && "HEADER_GC_BOSS_POSITION");

	CPythonMiniMap::Instance().ClearAtlasBossInfo();
	for (int i = 0; i < kBossPosition.count; ++i)
	{
		TBossPosition kBossPosition;
		if (!Recv(sizeof(TBossPosition), &kBossPosition))
			return false;
		
		const char* c_szName = "";
		//CPythonNonPlayer::Instance().GetName(kBossPosition.name, &c_szName);
		//CPythonMiniMap::Instance().RegisterAtlasBoss(c_szName, kBossPosition.x, kBossPosition.y, kBossPosition.dwTime);
		CPythonMiniMap::Instance().RegisterAtlasBoss(kBossPosition.name, kBossPosition.x, kBossPosition.y, kBossPosition.dwTime);
	}

	return true;
}
#endif

#ifdef ENABLE_HWID_BAN
bool CPythonNetworkStream::SendHwidBanPacket(BYTE bMode, const char* c_szPlayer, const char* c_szReason)
{
	TPacketCGHwidBan pack{};
	pack.header = HEADER_CG_HWID_SYSTEM;
	pack.bMode = bMode;
	strncpy(pack.szPlayer, c_szPlayer, sizeof(pack.szPlayer));
	strncpy(pack.szReason, c_szReason, sizeof(pack.szReason));

	if (!Send(sizeof(pack), &pack))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
bool CPythonNetworkStream::RecvMaintenanceInfo()
{
	TPacketGCMaintenanceInfo pack;
	if (!Recv(sizeof(pack), &pack))
		return false;

	std::vector<char> cause(pack.cause_len);
	if (!Recv(pack.cause_len, &cause[0]))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Maintenance", Py_BuildValue("(isi)", pack.remaining_time, &cause[0], pack.duration));

	return true;
}
#endif


#ifdef ENABLE_HUNTING_SYSTEM
bool CPythonNetworkStream::SendHuntingAction(BYTE bAction, DWORD dValue)
{
	if (!__CanActMainInstance())
		return true;

	TPacketGCHuntingAction packet;
	packet.bHeader = HEADER_CG_SEND_HUNTING_ACTION;
	packet.bAction = bAction;
	packet.dValue = dValue;

	if (!Send(sizeof(TPacketGCHuntingAction), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvHuntingOpenWindowMain()
{
	TPacketGCOpenWindowHuntingMain packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenHuntingWindowMain", Py_BuildValue("(iiiiiiiiii)", 
		packet.dLevel, packet.dMonster, packet.dCurCount, packet.dDestCount, packet.dMoneyMin, packet.dMoneyMax, packet.dExpMin, packet.dExpMax, packet.dRaceItem, packet.dRaceItemCount
	));

	return true;
}

bool CPythonNetworkStream::RecvHuntingOpenWindowSelect()
{
	TPacketGCOpenWindowHuntingSelect packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenHuntingWindowSelect", Py_BuildValue("(iiiiiiiiii)",
		packet.dLevel, packet.bType, packet.dMonster, packet.dCount, packet.dMoneyMin, packet.dMoneyMax, packet.dExpMin, packet.dExpMax, packet.dRaceItem, packet.dRaceItemCount
	));

	return true;
}

bool CPythonNetworkStream::RecvHuntingOpenWindowReward()
{
	TPacketGCOpenWindowReward packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenHuntingWindowReward", Py_BuildValue("(iiiiiii)", packet.dLevel, packet.dReward, packet.dRewardCount, packet.dRandomReward, packet.dRandomRewardCount, packet.dMoney, packet.bExp ));

	return true;
}

bool CPythonNetworkStream::RecvHuntingUpdate()
{
	TPacketGCUpdateHunting packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateHuntingMission", Py_BuildValue("(i)", packet.dCount));

	return true;
}

bool CPythonNetworkStream::RecvHuntingRandomItems()
{
	TPacketGCReciveRandomItems packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_HuntingReciveRandomItem", Py_BuildValue("(iii)", packet.bWindow, packet.dItemVnum, packet.dItemCount));

	return true;
}
#endif

#if defined(__BL_67_ATTR__)
bool CPythonNetworkStream::Send67AttrPacket(int iMaterialCount, int iSupportCount, int iSupportPos, int iItemPos)
{
	TPacketCG67Attr p =
	{
		static_cast<BYTE>(HEADER_CG_67_ATTR),
		static_cast<BYTE>(iMaterialCount),
		static_cast<BYTE>(iSupportCount),
		static_cast<short>(iSupportPos),
		static_cast<short>(iItemPos),
	};

	if (!Send(sizeof(p), &p))
	{
		Tracef("Send67AttrPacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::Send67AttrClosePacket()
{
	TPacket67AttrOpenClose p;
	p.bHeader = HEADER_CG_CLOSE_67_ATTR;

	if (!Send(sizeof(p), &p))
	{
		Tracef("Send67AttrClosePacket Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::Recv67AttrOpenPacket()
{
	TPacket67AttrOpenClose p;

	if (!Recv(sizeof(p), &p))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenAttr67AddDlg", Py_BuildValue("()"));

	return true;
}
#endif

#ifdef ENABLE_DUNGEON_INFO_SYSTEM
bool CPythonNetworkStream::RecvDungeonInfo()
{
	TPacketGCDungeonInfo packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	switch (packet.bySubHeader)
	{
		case SUBHEADER_DUNGEON_INFO_SEND:
		{
			CPythonDungeonInfo::Instance().AddDungeon(packet.byIndex, packet);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DungeonInfoReload", Py_BuildValue("(i)", packet.bReset));
		}
		break;

		case SUBHEADER_DUNGEON_INFO_OPEN:
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DungeonInfoOpen", Py_BuildValue("()"));
			break;
	}

	return true;
}

bool CPythonNetworkStream::SendDungeonInfo(uint8_t byAction, uint8_t byIndex, uint8_t byRankType)
{
	TPacketCGDungeonInfo packet;
	packet.byHeader = HEADER_CG_DUNGEON_INFO;
	packet.bySubHeader = byAction;
	packet.byIndex = byIndex;
	packet.byRankType = byRankType;

	if (!Send(sizeof(packet), &packet))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvDungeonRanking()
{
	TPacketGCDungeonInfoRank packet;
	if (!Recv(sizeof(packet), &packet))
	{
		Tracen("RecvDungeonRanking Error");
		return false;
	}

	CPythonDungeonInfo::Instance().AddRanking(packet.szName, packet.iLevel, packet.dwPoints);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DungeonRankingRefresh", Py_BuildValue("()"));

	return true;
}
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
bool CPythonNetworkStream::SendSkillColorPacket(BYTE bSkillSlot, DWORD dwColor1, DWORD dwColor2, DWORD dwColor3, DWORD dwColor4, DWORD dwColor5)
{
	TPacketCGSkillColor pack;
	pack.bheader = HEADER_CG_SKILL_COLOR;
	pack.skill = bSkillSlot;
	pack.col1 = dwColor1;
	pack.col2 = dwColor2;
	pack.col3 = dwColor3;
	pack.col4 = dwColor4;
	pack.col5 = dwColor5;

	if (!Send(sizeof(pack), &pack))
	{
		Tracen("Send Skill Color Packet Error");
		return false;
	}

	return SendSequence();
}
#endif


#ifdef ENABLE_HIDE_COSTUME_SYSTEM
bool CPythonNetworkStream::UpdateCostumeVisibleSettings()
{
	TPacketGCUpdateCostumeVisible pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						  "BINARY_UpdateCostumeVisible",
						  Py_BuildValue("(bbb"
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
										"b"
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
										"b"
#endif
										")",
										pack.type[ECostumeHideParts::HIDE_COSTUME_HAIR],
										pack.type[ECostumeHideParts::HIDE_COSTUME_BODY],
										pack.type[ECostumeHideParts::HIDE_COSTUME_WEAPON]
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
										, pack.type[ECostumeHideParts::HIDE_COSTUME_ACCE]
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
										, pack.type[ECostumeHideParts::HIDE_COSTUME_AURA]
#endif
									   )
						 );

	return true;
}
#endif

#ifdef ENABLE_ANTI_MULTIPLE_FARM
bool CPythonNetworkStream::RecvAntiFarmInformation()
{
	TSendAntiFarmInfo bufPacket;
	if (!Recv(sizeof(bufPacket), &bufPacket))
		return false;
	
	int packet_size = int(bufPacket.size) - sizeof(bufPacket);

	if (bufPacket.subheader == AF_SH_SENDING_DATA)
	{
		bool canClean;
		if (!Recv(sizeof(canClean), &canClean))
			return false;
		
		if (canClean) CAntiMultipleFarm::instance().ClearAntiFarmData();
		packet_size -= sizeof(canClean);
		
		size_t table_size = sizeof(TAntiFarmPlayerInfo);
		while (packet_size >= table_size)
		{
			TAntiFarmPlayerInfo table;
			if (!Recv(table_size, &table))
				return false;
			
			CAntiMultipleFarm::instance().AddNewPlayerData(table);
			packet_size -= table_size;
		}
		
		/*send refresh advise for main character*/
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecvAntiFarmReload", Py_BuildValue("()"));
	}

	return true;
}

bool CPythonNetworkStream::SendAntiFarmStatus(std::vector<DWORD> dwPIDs)
{
	TSendAntiFarmInfo pack;
	pack.header = HEADER_CG_ANTI_FARM;
	pack.subheader = AF_SH_SEND_STATUS_UPDATE;
	pack.size = sizeof(TSendAntiFarmInfo) + (sizeof(DWORD) * MULTIPLE_FARM_MAX_ACCOUNT);
	
	if (!Send(sizeof(pack), &pack))
		return false;
	
	for (uint8_t i = 0; i < MULTIPLE_FARM_MAX_ACCOUNT; ++i)
		if (!Send(sizeof(dwPIDs[i]), &dwPIDs[i]))
			return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_PLAYER_RANKING
bool CPythonNetworkStream::RecvRankInfo()
{
	TPacketGCRankInfo pack;
	if (!Recv(sizeof(pack), &pack))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RANK_APPEND", Py_BuildValue("(iiisLi)", pack.iMyPos, pack.iMode, pack.iPos, pack.szName, pack.lValue, pack.bEmpire));

	return true;
}
#endif

#ifdef ENABLE_BATTLE_PASS
bool CPythonNetworkStream::SendBattlePassAction(BYTE bAction)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGBattlePassAction packet;
	packet.bHeader = HEADER_CG_BATTLE_PASS;
	packet.bAction = bAction;

	if (!Send(sizeof(TPacketCGBattlePassAction), &packet))
	{
		Tracef("SendBattlePassAction Send Packet Error\n");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvBattlePassPacket()
{
	TPacketGCBattlePass packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	packet.wSize -= sizeof(packet);
	while (packet.wSize > 0)
	{
		TBattlePassMissionInfo missionInfo;
		if (!Recv(sizeof(missionInfo), &missionInfo))
		{
			return false;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassAddMission", Py_BuildValue("(iiii)",
							  missionInfo.bMissionType, missionInfo.dwMissionInfo[0], missionInfo.dwMissionInfo[1], missionInfo.dwMissionInfo[2]));

		for (int i = 0; i < 3; ++i)
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassAddMissionReward", Py_BuildValue("(iii)",
								  missionInfo.bMissionType, missionInfo.aRewardList[i].dwVnum, missionInfo.aRewardList[i].bCount));

		packet.wSize -= sizeof(missionInfo);
	}

	while (packet.wRewardSize > 0)
	{
		TBattlePassRewardItem rewardInfo;
		if (!Recv(sizeof(rewardInfo), &rewardInfo))
		{
			return false;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassAddReward", Py_BuildValue("(ii)", rewardInfo.dwVnum, rewardInfo.bCount));

		packet.wRewardSize -= sizeof(rewardInfo);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassOpen", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvBattlePassRankingPacket()
{
	TPacketGCBattlePassRanking packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	packet.wSize -= sizeof(packet);
	while (packet.wSize > 0)
	{
		TBattlePassRanking rankingInfo;
		if (!Recv(sizeof(rankingInfo), &rankingInfo))
		{
			return false;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassAddRanking", Py_BuildValue("(isi)",
							  rankingInfo.bPos, rankingInfo.playerName, rankingInfo.dwFinishTime));

		packet.wSize -= sizeof(rankingInfo);
	}

	if (packet.bIsGlobal)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassOpenRanking", Py_BuildValue("()"));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassRefreshRanking", Py_BuildValue("()"));
	}

	return true;
}

bool CPythonNetworkStream::RecvBattlePassUpdatePacket()
{
	TPacketGCBattlePassUpdate packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BattlePassUpdate", Py_BuildValue("(ii)", packet.bMissionType, packet.dwNewProgress));

	return true;
}
#endif

#ifdef ENABLE_ITEMSHOP
bool CPythonNetworkStream::SendBuyItemshopItemPacket(const char* hash, WORD wCount)
{
	TPacketCGBuyItemshopItem p;
	p.header = HEADER_CG_BUY_ITEMSHOP_ITEM;
	p.wCount = wCount;
	strncpy(p.hash, hash, sizeof(p.hash));

	if (!Send(sizeof(p), &p))
	{
		return false;
	}
	return true;
}

bool CPythonNetworkStream::SendRedeemPromotionCodePacket(const char* promotion_code)
{
	TPacketCGRedeemPromotionCode p;
	p.header = HEADER_CG_PROMOTION;
	strncpy(p.promotion_code, promotion_code, sizeof(p.promotion_code) - 1);
	if (!Send(sizeof(p), &p))
	{
		return false;
	}
	return true;
}

bool CPythonNetworkStream::RecvItemshopPacket()
{
	TPacketGCItemshopInfo packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	DWORD subHeader = packet.subheader;

	switch (subHeader)
	{
	case SUBHEADER_ITEMSHOP_REFRESH_ITEMS:
	{
		TPacketGCitemshopCategorySize itemshop_category_size;
		if (!Recv(sizeof(itemshop_category_size), &itemshop_category_size))
			return false;

		std::map<BYTE, TItemshopCategoryInfo> itemshop_categories;

		BYTE key = 0;
		TItemshopCategoryInfo category_info;

		for (int i = 0; i < itemshop_category_size.size; i++)
		{
			if (!Recv(sizeof(BYTE), &key))
				return false;

			if (!Recv(sizeof(category_info), &category_info))
				return false;

			itemshop_categories.insert(std::pair(key, category_info));
		}

		TPacketGCItemshopItemSize itemshop_item_size;
		if (!Recv(sizeof(itemshop_item_size), &itemshop_item_size))
			return false;

		std::map<BYTE, std::vector<TItemshopItemTable>> itemshop_items;
		size_t vec_size = 0;
		TItemshopItemTable table{};
		for (int i = 0; i < itemshop_item_size.size; i++) {
			if (!Recv(sizeof(BYTE), &key)) {
				return false;
			}
			if (!Recv(sizeof(size_t), &vec_size)) {
				return false;
			}
			std::vector<TItemshopItemTable> itemVec;
			itemVec.resize(vec_size);
			if (!Recv(sizeof(TItemshopItemTable) * vec_size, itemVec.data()))
			{
				return false;
			}
			itemshop_items[key] = itemVec;
		}

		// NOTE: Sort items in categories
		for (auto& vec_item : itemshop_items)
		{
			std::sort(vec_item.second.begin(), vec_item.second.end(), [](TItemshopItemTable const& f, TItemshopItemTable const& s) {
				return f.end_time > s.end_time || f.end_time == s.end_time &&
					(f.llLimitCount > s.llLimitCount || f.llLimitCount == s.llLimitCount &&
						(f.byDiscountPercent > s.byDiscountPercent || f.byDiscountPercent == s.byDiscountPercent &&
							(f.weight > s.weight || f.weight == s.weight &&
								(f.dwVnum > s.dwVnum))));
				}
			);
		}

		PyObject* py_itemshop_infos = PyList_New(0);

		for (const auto& [key, cat] : itemshop_items)
		{
			PyObject* py_itemshop_category = PyDict_New();

			PyDict_SetItem(py_itemshop_category, PyString_FromString("index"), PyInt_FromLong(key));
			PyDict_SetItem(py_itemshop_category, PyString_FromString("category"), PyString_FromString(itemshop_categories.at(key).category));
			PyDict_SetItem(py_itemshop_category, PyString_FromString("icon"), PyString_FromString(itemshop_categories.at(key).icon));

			PyObject* py_itemshop_items = PyList_New(0);

			for (const auto& item : cat)
			{
				PyObject* py_itemshop_item = PyDict_New();
				PyDict_SetItem(py_itemshop_item, PyString_FromString("vnum"), PyInt_FromLong(item.dwVnum));

				PyObject* sockets = PyList_New(0);
				for (int socket = 0; socket < ITEM_SOCKET_SLOT_MAX_NUM; socket++)
				{
					PyList_Append(sockets, PyInt_FromLong(item.alSockets[socket]));
				}
				PyDict_SetItem(py_itemshop_item, PyString_FromString("sockets"), sockets);

				PyObject* attrList = PyList_New(0);

				for (const auto& attr : item.aAttr)
				{
					PyList_Append(attrList, Py_BuildValue("ii", attr.bType, attr.sValue));
				}
				PyDict_SetItem(py_itemshop_item, PyString_FromString("attr"), attrList);
				PyDict_SetItem(py_itemshop_item, PyString_FromString("price"), PyInt_FromLong(item.ullPrice));
				PyDict_SetItem(py_itemshop_item, PyString_FromString("count"), PyInt_FromLong(item.wCount));
				PyDict_SetItem(py_itemshop_item, PyString_FromString("hash"), PyString_FromString(item.hash));
				PyDict_SetItem(py_itemshop_item, PyString_FromString("discount"), PyInt_FromLong(item.byDiscountPercent));
				PyDict_SetItem(py_itemshop_item, PyString_FromString("limitCount"), PyInt_FromLong(item.llLimitCount));
				PyDict_SetItem(py_itemshop_item, PyString_FromString("endTime"), PyInt_FromLong(item.end_time));

				PyList_Append(py_itemshop_items, py_itemshop_item);
			}
			PyDict_SetItem(py_itemshop_category, PyString_FromString("items"), py_itemshop_items);
			PyList_Append(py_itemshop_infos, py_itemshop_category);
		}
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UPDATE_ITEMSHOP_ITEMS", Py_BuildValue("(O)", py_itemshop_infos));
	}
	break;
	case SUBHEADER_ITEMSHOP_REFRESH_SINGLE_ITEM:
	{
		TItemshopItemTable item;
		if (!Recv(sizeof(item), &item))
			return false;

		PyObject* py_itemshop_item = PyDict_New();
		PyDict_SetItem(py_itemshop_item, PyString_FromString("vnum"), PyInt_FromLong(item.dwVnum));
		PyObject* sockets = PyList_New(0);
		for (int socket = 0; socket < ITEM_SOCKET_SLOT_MAX_NUM; socket++)
		{
			PyList_Append(sockets, PyInt_FromLong(item.alSockets[socket]));
		}
		PyDict_SetItem(py_itemshop_item, PyString_FromString("sockets"), sockets);

		PyObject* attrList = PyList_New(0);

		for (int attr = 0; attr < ITEM_ATTRIBUTE_SLOT_MAX_NUM; attr++)
		{
			PyList_Append(attrList, Py_BuildValue("ii", item.aAttr[attr].bType, item.aAttr[attr].sValue));
		}
		PyDict_SetItem(py_itemshop_item, PyString_FromString("attr"), attrList);
		PyDict_SetItem(py_itemshop_item, PyString_FromString("price"), PyInt_FromLong(item.ullPrice));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("count"), PyInt_FromLong(item.wCount));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("hash"), PyString_FromString(item.hash));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("discount"), PyInt_FromLong(item.byDiscountPercent));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("limitCount"), PyInt_FromLong(item.llLimitCount));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("endTime"), PyInt_FromLong(item.end_time));

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UPDATE_ITEMSHOP_ITEM", Py_BuildValue("(O)", py_itemshop_item));
	}
	break;
	case SUBHEADER_ITEMSHOP_REFRESH_COINS:
	{
		unsigned long long ullCoins;

		if (!Recv(sizeof(unsigned long long), &ullCoins))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UPDATE_ITEMSHOP_COINS", Py_BuildValue("(L)", ullCoins));
	}
	break;
	case SUBHEADER_ITEMSHOP_REMOVE_SINGLE_ITEM:
	{
		TItemshopItemTable item;
		if (!Recv(sizeof(item), &item))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_REMOVE_ITEMSHOP_ITEM", Py_BuildValue("(s)", item.hash));
	}
	break;

	case SUBHEADER_ITEMSHOP_ADD_SINGLE_ITEM:
	{
		TItemshopCategoryInfo category_info;
		if (!Recv(sizeof(category_info), &category_info))
			return false;

		TItemshopItemTable item;
		if (!Recv(sizeof(item), &item))
			return false;

		PyObject* py_itemshop_category = PyDict_New();

		PyDict_SetItem(py_itemshop_category, PyString_FromString("index"), PyInt_FromLong(item.byCategory));
		PyDict_SetItem(py_itemshop_category, PyString_FromString("category"), PyString_FromString(category_info.category));
		PyDict_SetItem(py_itemshop_category, PyString_FromString("icon"), PyString_FromString(category_info.icon));

		PyObject* py_itemshop_items = PyList_New(0);
		PyObject* py_itemshop_item = PyDict_New();
		PyDict_SetItem(py_itemshop_item, PyString_FromString("vnum"), PyInt_FromLong(item.dwVnum));

		PyObject* sockets = PyList_New(0);
		for (int socket = 0; socket < ITEM_SOCKET_SLOT_MAX_NUM; socket++)
		{
			PyList_Append(sockets, PyInt_FromLong(item.alSockets[socket]));
		}
		PyDict_SetItem(py_itemshop_item, PyString_FromString("sockets"), sockets);

		PyObject* attrList = PyList_New(0);

		for (const auto& attr : item.aAttr)
		{
			PyList_Append(attrList, Py_BuildValue("ii", attr.bType, attr.sValue));
		}
		PyDict_SetItem(py_itemshop_item, PyString_FromString("attr"), attrList);
		PyDict_SetItem(py_itemshop_item, PyString_FromString("price"), PyInt_FromLong(item.ullPrice));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("count"), PyInt_FromLong(item.wCount));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("hash"), PyString_FromString(item.hash));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("discount"), PyInt_FromLong(item.byDiscountPercent));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("limitCount"), PyInt_FromLong(item.llLimitCount));
		PyDict_SetItem(py_itemshop_item, PyString_FromString("endTime"), PyInt_FromLong(item.end_time));

		PyList_Append(py_itemshop_items, py_itemshop_item);
		PyDict_SetItem(py_itemshop_category, PyString_FromString("items"), py_itemshop_items);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ADD_ITEMSHOP_ITEM", Py_BuildValue("(O)", py_itemshop_category));
	}
	break;

	case SUBHEADER_PROMOTION_CODE_REWARDS:
	{
		BYTE answer;
		if (!Recv(sizeof(BYTE), &answer))
			return false;

		size_t size;
		if (!Recv(sizeof(size_t), &size))
			return false;
		std::vector <TPromotionItemTable> items;

		items.clear();
		items.resize(size);

		if (size && !Recv(sizeof(TPromotionItemTable) * size, &items[0]))
			return false;

		PyObject* py_itemshop_items = PyList_New(0);

		for (const auto& item : items)
		{
			PyObject* py_itemshop_item = PyDict_New();
			PyDict_SetItem(py_itemshop_item, PyString_FromString("vnum"), PyInt_FromLong(item.dwVnum));
			PyObject* sockets = PyList_New(0);
			for (int socket = 0; socket < ITEM_SOCKET_SLOT_MAX_NUM; socket++)
			{
				PyList_Append(sockets, PyInt_FromLong(item.alSockets[socket]));
			}
			PyDict_SetItem(py_itemshop_item, PyString_FromString("sockets"), sockets);

			PyObject* attrList = PyList_New(0);

			for (int attr = 0; attr < ITEM_ATTRIBUTE_SLOT_MAX_NUM; attr++)
			{
				PyList_Append(attrList, Py_BuildValue("ii", item.aAttr[attr].bType, item.aAttr[attr].sValue));
			}
			PyDict_SetItem(py_itemshop_item, PyString_FromString("attr"), attrList);
			PyDict_SetItem(py_itemshop_item, PyString_FromString("count"), PyInt_FromLong(item.wCount));

			PyList_Append(py_itemshop_items, py_itemshop_item);
		}
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_PROMOTION_CODE_REWARDS", Py_BuildValue("(iO)", answer, py_itemshop_items));
	}
	break;
	}
	return true;
}
#endif

#ifdef ENABLE_QUICK_OPEN
bool CPythonNetworkStream::RecvBufferedInventoryPacket()
{
	TPacketGCInventoryHeader packHdr;
	if (!Recv(sizeof(TPacketGCInventoryHeader), &packHdr))
	{
		return false;
	}

	packHdr.wSize -= sizeof(packHdr);

	// Keep on recv
	while (packHdr.wSize)
	{
		TPacketGCItemSet2 packItem;
		if (!Recv(sizeof(TPacketGCItemSet2), &packItem))
		{
			return false;
		}

		TItemData kItemData;
		kItemData.vnum = packItem.vnum;
		kItemData.count = packItem.count;

		kItemData.flags = 0;
		for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		{
			kItemData.alSockets[i] = packItem.alSockets[i];
		}
		for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		{
			kItemData.aAttr[j] = packItem.aAttr[j];
		}

		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

		rkPlayer.SetItemData(packItem.Cell, kItemData);
		packHdr.wSize -= sizeof(packItem);
	}

	// Now refresh
	__RefreshInventoryWindow();
	return true;
}
#endif

bool CPythonNetworkStream::RecvDragonPointsChange()
{
	TPacketGCDragonPointsChange DragonPointsChange;

	if (!Recv(sizeof(TPacketGCDragonPointsChange), &DragonPointsChange))
	{
		Tracen("Recv Dragon Points Change Packet Error");
		return false;
	}

	CInstanceBase *pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pInstance)
	{
		if (DragonPointsChange.dwVID == pInstance->GetVirtualID())
		{
			CPythonPlayer &rkPlayer = CPythonPlayer::Instance();
			rkPlayer.SetCash(DragonPointsChange.dwCash);
			rkPlayer.SetCoins(DragonPointsChange.dwCoins);
		}
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));
	return true;
}

#ifdef ENABLE_EVENT_CALENDAR
bool CPythonNetworkStream::RecvEventManager()
{
	TPacketGCEventManager p;
	if (!Recv(sizeof(TPacketGCEventManager), &p))
		return false;

	BYTE subIndex;
	if (!Recv(sizeof(BYTE), &subIndex))
		return false;

	if (subIndex == EVENT_MANAGER_LOAD)
	{
		BYTE dayCount;
		if (!Recv(sizeof(BYTE), &dayCount))
			return false;

		int serverTime;
		if (!Recv(sizeof(int), &serverTime))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ClearEventManager", Py_BuildValue("()"));

		for (BYTE j = 0; j < dayCount; ++j)
		{
			BYTE dayIndex;
			if (!Recv(sizeof(BYTE), &dayIndex))
				return false;

			BYTE dayEventCount;
			if (!Recv(sizeof(BYTE), &dayEventCount))
				return false;

			if (dayEventCount > 0)
			{
				std::vector<TEventManagerData> m_vec;
				m_vec.resize(dayEventCount);
				if (!Recv(dayEventCount * sizeof(TEventManagerData), &m_vec[0]))
					return false;
	
				for (const auto& eventPtr : m_vec)
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendEvent", Py_BuildValue("(iiissiiiiiiiii)", dayIndex, eventPtr.eventID, eventPtr.eventIndex, eventPtr.startTimeText, eventPtr.endTimeText, eventPtr.empireFlag, eventPtr.channelFlag, eventPtr.value[0], eventPtr.value[1], eventPtr.value[2], eventPtr.value[3], eventPtr.startTime - serverTime, eventPtr.endTime == 0 ? 0 : eventPtr.endTime - serverTime, eventPtr.eventStatus));
			}
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEventManager", Py_BuildValue("()"));
	}
	else if (subIndex == EVENT_MANAGER_EVENT_STATUS)
	{
		WORD eventID;
		if (!Recv(sizeof(WORD), &eventID))
			return false;

		bool eventStatus;
		if (!Recv(sizeof(bool), &eventStatus))
			return false;

		int endTime;
		if (!Recv(sizeof(int), &endTime))
			return false;

		char endTimeText[25];
		if (!Recv(sizeof(endTimeText), &endTimeText))
			return false;

		int serverTime;
		if (!Recv(sizeof(int), &serverTime))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEventStatus", Py_BuildValue("(iiis)", eventID, eventStatus, endTime == 0 ? 0 : endTime - serverTime, endTimeText));
	}
	return true;
}
#endif

#ifdef ENABLE_TRASH_SYSTEM
bool CPythonNetworkStream::RecvTrashPacket()
{
	TTrashPacketGC packet_data;
	if (!Recv(sizeof(packet_data), &packet_data))
		return false;

	uint16_t packet_left_size = packet_data.size - sizeof(packet_data);

	while(packet_left_size > 0)
	{
		uint8_t trash_slot;
		if (!Recv(sizeof(trash_slot), &trash_slot))
			return false;

		CTrash::RemoveItem(trash_slot);
		packet_left_size -= sizeof(uint8_t);
	}

	return true;
}
#endif

#ifdef __DAILY_QUESTS__
bool CPythonNetworkStream::RecvDailyQuests()
{
	TPacketGCSendDailyQuests pack{};
	if (!Recv(sizeof pack, &pack))
		return false;

	assert(static_cast<int32_t>(pack.size) - sizeof(pack) == pack.count * sizeof TPacketGCSendDailyQuest);

	TPacketGCSendDailyQuest DailyQuests{};

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ResetDailyRewards", Py_BuildValue("()"));

	for (uint16_t i = 0; i < pack.count; i++)
	{
		if (!Recv(sizeof(TPacketGCSendDailyQuest), &DailyQuests))
			return false;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecvDailyQuests", Py_BuildValue("iiiiii",
			DailyQuests.id, DailyQuests.type, DailyQuests.vnum, DailyQuests.value, DailyQuests.amount, DailyQuests.progress
		));
	}

	return true;
}

bool CPythonNetworkStream::RecvDailyRewards()
{
	TPacketGCSendDailyRewards pack{};
	if (!Recv(sizeof pack, &pack))
		return false;

	assert(static_cast<int32_t>(pack.size) - sizeof(pack) == pack.count * sizeof TPacketGCSendDailyReward);

	TPacketGCSendDailyReward DailyRewards{};

	for (uint16_t i = 0; i < pack.count; i++)
	{
		if (!Recv(sizeof(TPacketGCSendDailyReward), &DailyRewards))
			return false;

		PyObject* items = PyList_New(0);
		for (uint32_t x = 0; x < 6; x++)
		{
			PyObject* pyitem = PyList_New(2);

			PyList_SetItem(pyitem, 0, Py_BuildValue("i", DailyRewards.rewardVnum[x]));
			PyList_SetItem(pyitem, 1, Py_BuildValue("i", DailyRewards.rewardCount[x]));

			PyList_Append(items, pyitem);
		}


		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecvDailyRewards", Py_BuildValue("iOi",
			DailyRewards.id, items, DailyRewards.progress
		));
	}

	return true;
}

bool CPythonNetworkStream::RecvUpdateDailyQuests()
{
	TPacketGCUpdateDailyQuest packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecvUpdateDailyQuests", Py_BuildValue("(ii)", packet.id, packet.progress));

	return true;
}

bool CPythonNetworkStream::RecvUpdateDailyRewards()
{
	TPacketGCUpdateDailyReward packet;
	if (!Recv(sizeof(packet), &packet))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RecvUpdateDailyRewards", Py_BuildValue("(i)", packet.progress));

	return true;
}
#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
