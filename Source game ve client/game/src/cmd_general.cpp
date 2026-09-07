#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../../libthecore/include/xmd5.h"
#endif

#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "threeway_war.h"
#include "log.h"
#include "../../common/VnumHelper.h"
#ifdef ENABLE_NEWSTUFF
#include "../../common/PulseManager.h"
#endif
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	#include "HAntiMultipleFarm.h"
#endif
#ifdef ENABLE_RANK_PLAYER
	#include "RankPlayer.h"
#endif
#ifdef __RANKING_SYSTEM__
	#include "RankPlayer.h"
#endif
#ifdef ENABLE_DAILY_BOSS
	#include "daily_boss.h"
#endif
#ifdef __GUILD_SAFEBOX__
	#include "guild_safebox.h"
#endif
#ifdef __DAILY_QUESTS__
#include "dailyquest_manager.h"
#endif
#include "gm.h"

#include "GuildStorageManager.h"
ACMD(do_open_gs)
{
	if (ch == nullptr || ch->GetGuild() == nullptr)
		return;
	
	if (GuildStorageManager::Instance().HasViewersGuildID(ch->GetGuild()->GetID()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GUILD STORAGE: SOMEONE IS OPENING ALREADY"));
		return;
	}
	
#ifdef __PREMIUM_PRIVATE_SHOP__
	if (ch->GetViewingPrivateShop() || ch->IsEditingPrivateShop() || ch->IsShopSearch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GUILD STORAGE: OFFLINE SHOP IS OPENED"));
		return;
	}
#endif
	
	GuildStorageManager::Instance().OpenGuildStorage(ch);
}

#ifdef __DAILY_QUESTS__
ACMD(do_claim_daily)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
	{
		return;
	}

	int32_t rewardId = 0;
	str_to_number(rewardId, arg1);

	if (rewardId >= 1 && rewardId <= 3)
		CDailyQuestManager::instance().ClaimReward(rewardId, ch);
}
#endif

ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 탈것을 이용중입니다."));
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 먼저 소환해주세요."));
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

ACMD(do_user_horse_back)
{
	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
#ifdef ENABLE_MOUNT_SYSTEM
		ch->ClearMountItem();
#endif
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 돌려보냈습니다."));
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말에서 먼저 내려야 합니다."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 먼저 소환해주세요."));
	}
}

ACMD(do_user_horse_feed)
{
	if (ch->GetMyShop())
		return;

	if (ch->GetHorse() == NULL)
	{
		if (ch->IsHorseRiding() == false)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 먼저 소환해주세요."));
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 탄 상태에서는 먹이를 줄 수 없습니다."));
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말에게 %s%s 주었습니다."),
				ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName,
				"");
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 아이템이 필요합니다"), ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName);
	}
}

#define MAX_REASON_LEN		128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo()
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
#ifdef ENABLE_FLUSH_AT_SHUTDOWN
			d->GetCharacter()->SaveReal();
			DWORD pid = d->GetCharacter()->GetPlayerID();
			db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
			db_clientdesc->Packet(&pid, sizeof(DWORD));
#endif

			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
	: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		char buf[64];
		snprintf(buf, sizeof(buf), LC_TEXT("셧다운이 %d초 남았습니다."), *pSec);
		SendNotice(buf);

		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	char buf[64];
	snprintf(buf, sizeof(buf), LC_TEXT("%d초 후 게임이 셧다운 됩니다."), iSec);

	SendNotice(buf);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	sys_err("Accept shutdown command from %s.", (ch) ? ch->GetName() : "NONAME");

	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
			case SCMD_QUIT:
			case SCMD_PHASE_SELECT:
#if defined(__BL_MULTI_LANGUAGE__)
			case SCMD_LANGUAGE_CHANGE:
#endif
#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
			case SCMD_LANGUAGE_CHANGE_ANONYMOUS:
#endif
				{
					TPacketNeedLoginLogInfo acc_info;
					acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;

					db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

					LogManager::instance().DetailLoginLog( false, ch );
				}
				break;
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				if (d) // @fixme197
					d->DelayedDisconnect(1);
				break;

			case SCMD_PHASE_SELECT:
				ch->Disconnect("timed_event - SCMD_PHASE_SELECT");
				if (d)
					d->SetPhase(PHASE_SELECT);
				break;
#if defined(__BL_MULTI_LANGUAGE__)
			case SCMD_LANGUAGE_CHANGE:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "language_change");
				ch->Disconnect("timed_event - SCMD_LANGUAGE_CHANGE");
				if (d)
					d->SetPhase(PHASE_SELECT);
				break;
#endif
#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
			case SCMD_LANGUAGE_CHANGE_ANONYMOUS:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "language_anonymous");
				ch->Disconnect("timed_event - SCMD_LANGUAGE_CHANGE_ANONYMOUS");
				if (d)
					d->SetPhase(PHASE_SELECT);
				break;
#endif
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d초 남았습니다."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{
	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("취소 되었습니다."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("로그인 화면으로 돌아 갑니다. 잠시만 기다리세요."));
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("게임을 종료 합니다. 잠시만 기다리세요."));
			break;

		case SCMD_PHASE_SELECT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("캐릭터를 전환 합니다. 잠시만 기다리세요."));
			break;

#if defined(__BL_MULTI_LANGUAGE__)
		case SCMD_LANGUAGE_CHANGE:
#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
		case SCMD_LANGUAGE_CHANGE_ANONYMOUS:
#endif
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;515]");
			break;
#endif
	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) &&
		false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()) &&
	   	(!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
#if defined(__BL_MULTI_LANGUAGE__)
		case SCMD_LANGUAGE_CHANGE:
#endif
#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
		case SCMD_LANGUAGE_CHANGE_ANONYMOUS:
#endif
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				info->left_second = 3;

				info->ch		= ch;
				info->subcmd		= subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
	}
}

ACMD(do_mount)
{
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

	if (NULL == ch->m_pkDeadEvent)
		return;

	int iTimeToDead = (event_time(ch->m_pkDeadEvent) / passes_per_sec);

	if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{
				if (false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 재시작 할 수 없습니다. (%d초 남음)"), iTimeToDead - (180 - g_nPortalLimitTime));
					return;
				}
			}
#define eFRS_HERESEC	177
			if (iTimeToDead > eFRS_HERESEC)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 재시작 할 수 없습니다. (%d초 남음)"), iTimeToDead - eFRS_HERESEC);
				return;
			}

// #ifdef ENABLE_NOT_BUFF_CLEAR
// 			ch->UpdatePacket();
// #endif
		}
	}

	//PREVENT_HACK

	if (subcmd == SCMD_RESTART_TOWN)
	{
		if (ch->IsHack())
		{
			if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) ||
			   	false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 재시작 할 수 없습니다. (%d초 남음)"), iTimeToDead - (180 - g_nPortalLimitTime));
				return;
			}
		}

#define eFRS_TOWNSEC	173
		if (iTimeToDead > eFRS_TOWNSEC)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 마을에서 재시작 할 수 없습니다. (%d 초 남음)"), iTimeToDead - eFRS_TOWNSEC);
			return;
		}
	}
	//END_PREVENT_HACK

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();
// #ifdef ENABLE_NOT_BUFF_CLEAR
// 	ch->UpdatePacket();
// #endif

	//FORKED_LOAD

	if (1 == quest::CQuestManager::instance().GetEventFlag("threeway_war"))
	{
		if (subcmd == SCMD_RESTART_TOWN || subcmd == SCMD_RESTART_HERE)
		{
			if (true == CThreeWayWar::instance().IsThreeWayWarMapIndex(ch->GetMapIndex()) &&
					false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

				ch->ReviveInvisible(5);
				ch->SetQuestFlag("duel_type", 0);
#if defined(ENABLE_COSTUME_PET)
				ch->CheckWears();
#endif
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

				return;
			}

			if (true == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				if (CThreeWayWar::instance().GetReviveTokenForPlayer(ch->GetPlayerID()) <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성지에서 부활 기회를 모두 잃었습니다! 마을로 이동합니다!"));
					ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
				}
				else
				{
					ch->Show(ch->GetMapIndex(), GetSungziStartX(ch->GetEmpire()), GetSungziStartY(ch->GetEmpire()));
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);
				ch->SetQuestFlag("duel_type", 0);
#if defined(ENABLE_COSTUME_PET)
				ch->CheckWears();
#endif

				return;
			}
		}
	}
	//END_FORKED_LOAD

	if (ch->GetDungeon())
		ch->GetDungeon()->UseRevive(ch);

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;

		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					sys_log(0, "do_restart: restart town");
					PIXEL_POSITION pos;

					if (CWarMapManager::instance().GetStartPosition(ch->GetMapIndex(), ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						ch->Show(ch->GetMapIndex(), pos.x, pos.y);
					else
						ch->ExitToSavedLocation();

					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
					ch->SetQuestFlag("duel_type", 0);
#if defined(ENABLE_COSTUME_PET)
					ch->CheckWears();
#endif
					break;

				case SCMD_RESTART_HERE:
					sys_log(0, "do_restart: restart here");
					ch->RestartAtSamePos();
					//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
					ch->SetQuestFlag("duel_type", 0);
#if defined(ENABLE_COSTUME_PET)
					ch->CheckWears();
#endif
					break;
			}

			return;
		}
	}
	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			sys_log(0, "do_restart: restart town");
			PIXEL_POSITION pos;

			if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
				ch->WarpSet(pos.x, pos.y);
			else
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

			if (ch->GetQuestFlag("player.slain_by_monster") >= 1)
			{
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

				ch->SetQuestFlag("player.slain_by_monster", 0);
			}
			else
				ch->PointChange(POINT_HP, 50 - ch->GetHP());
			ch->DeathPenalty(1);
			break;

		case SCMD_RESTART_HERE:
			sys_log(0, "do_restart: restart here");
			ch->RestartAtSamePos();
			//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
			if (ch->GetQuestFlag("player.slain_by_monster") >= 1 || ch->GetQuestFlag("slain_by_duel_partner"))
			{
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

				ch->SetQuestFlag("player.slain_by_monster", 0);
				ch->SetQuestFlag("slain_by_duel_partner", 0);
			}
			else
				ch->PointChange(POINT_HP, 50 - ch->GetHP());
			ch->DeathPenalty(0);
			ch->ReviveInvisible(5);
			ch->SetQuestFlag("duel_type", 0);
#if defined(ENABLE_COSTUME_PET)
			ch->CheckWears();
#endif
			break;
	}
}

#define MAX_STAT g_iStatusPointSetMaxValue

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 능력을 올릴 수 없습니다."));
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

#ifdef ENABLE_RENEWAL_ADD_STATS
	if (CPVPManager::Instance().IsFighting(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't do this during a duel."));
		return;
	}
#endif

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 능력을 올릴 수 없습니다."));
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	// ch->ChatPacket(CHAT_TYPE_INFO, "%s GRP(%d) idx(%u), MAX_STAT(%d), expr(%d)", __FUNCTION__, ch->GetRealPoint(idx), idx, MAX_STAT, ch->GetRealPoint(idx) >= MAX_STAT);
	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -1);
	ch->ComputePoints();
}

ACMD(do_pvp)
{
#ifdef ENABLE_DAILY_BOSS
	if (CDailyBoss::Instance().IsBossMap(ch->GetMapIndex()))
	{
		return;
	}
#endif

	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	switch (ch->GetMapIndex())
	{
	case 91:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot duel in this map."));
		return;
	}

#ifdef ENABLE_DUEL_OPTIONS
	char arg1[256], arg2[256], arg3[256];
	three_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

	DWORD vid = 0;
	BYTE bIsSpecial = 0;
	int DuelType = 0;
	str_to_number(vid, arg1);
	str_to_number(bIsSpecial, arg2);
	str_to_number(DuelType, arg3);
#else
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
#endif
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

	if (pkVictim->GetArena() != NULL)
	{
		pkVictim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 대련중입니다."));
		return;
	}

#ifdef ENABLE_DUEL_OPTIONS
	CPVPManager::instance().Insert(ch, pkVictim, bIsSpecial ? true : false, DuelType);
#else
	CPVPManager::instance().Insert(ch, pkVictim);
#endif
}

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드에 속해있지 않습니다."));
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드 스킬 레벨을 변경할 권한이 없습니다."));
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:
				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

//
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 창고가 이미 열려있습니다."));
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 창고를 닫은지 10초 안에는 열 수 없습니다."));
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 던전 안에서는 파티에서 나갈 수 없습니다."));
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티에서 나가셨습니다."));
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_war)
{
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 이미 다른 전쟁에 참전 중 입니다."));
		return;
	}

	char arg1[256], arg2[256];
	DWORD type = GUILD_WAR_TYPE_FIELD; //fixme102 base int modded uint
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	if (*arg2)
	{
		str_to_number(type, arg2);

		if (type >= GUILD_WAR_TYPE_MAX_NUM)
			type = GUILD_WAR_TYPE_FIELD;
	}

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전에 대한 권한이 없습니다."));
		return;
	}

	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 그런 길드가 없습니다."));
		return;
	}

	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드가 이미 전쟁 중 입니다."));
					return;
				}

				int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

				if (g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 전비가 부족하여 길드전을 할 수 없습니다."));
					return;
				}

				if (opp_g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 전비가 부족하여 길드전을 할 수 없습니다."));
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 선전포고 중인 길드입니다."));
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드가 이미 전쟁 중 입니다."));
					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 이미 전쟁이 예약된 길드 입니다."));
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 이미 전쟁 중인 길드입니다."));
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 레더 점수가 모자라서 길드전을 할 수 없습니다."));
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전을 하기 위해선 최소한 %d명이 있어야 합니다."), GUILD_WAR_MIN_MEMBER_COUNT);
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 레더 점수가 모자라서 길드전을 할 수 없습니다."));
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 길드원 수가 부족하여 길드전을 할 수 없습니다."));
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 길드장이 접속중이 아닙니다."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방 길드의 길드장이 접속중이 아닙니다."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전에 대한 권한이 없습니다."));
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 그런 길드가 없습니다."));
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

#ifdef ENABLE_DAILY_BOSS
	if (CDailyBoss::Instance().IsBossMap(ch->GetMapIndex()))
	{
		return;
	}
#endif

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);
	// @fixme130 AuthToAdd void -> bool
	bool bIsDenied = answer != 'y';
	bool bIsAdded = MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, bIsDenied); // DENY
	if (bIsAdded && bIsDenied)
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님으로 부터 친구 등록을 거부 당했습니다."), ch->GetName());
#ifdef CROSS_CHANNEL_FRIEND_REQUEST
		else
		{
			CCI* pkCCI = P2P_MANAGER::Instance().Find(arg2);
			if (pkCCI)
			{
				LPDESC pkDesc = pkCCI->pkDesc;
				pkDesc->SetRelay(arg2);
				pkDesc->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님으로 부터 친구 등록을 거부 당했습니다."), ch->GetName());
				pkDesc->SetRelay("");
			}
		}
#endif
	}
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
	if (true == ch->UnEquipSpecialRideUniqueItem())
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsHorseRiding())
		{
			ch->StopRiding();
		}
	}
	else
	{
		ch->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("인벤토리가 꽉 차서 내릴 수 없습니다."));
	}
}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{
    // if (ch->GetGMLevel() <= GM_PLAYER)
    // 	return;

    char arg1[256];
    one_argument(argument, arg1, sizeof(arg1));

    if (*arg1)
    {
        DWORD vid = 0;
        str_to_number(vid, arg1);
        LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

        if (!tch)
            return;

        if (!tch->IsPC())
            return;

        // Uberprufen, ob der Spieler den Blockmodus fur die Ausrustungsansicht aktiviert hat,
        // aber es dem GM dennoch erlaubt, die Ausrustung zu sehen.
        if (tch->IsBlockMode(BLOCK_EQUIPMENT_VIEW) && ch->GetGMLevel() <= GM_PLAYER)
        {
            ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s disabled his equipment view."), tch->GetName());
            return;
        }

#ifdef ENABLE_NEWSTUFF
        if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::RequestViewEuipment, std::chrono::milliseconds(1000)))
        {
            ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait 1 second."));
            return;
        }
#endif

        tch->SendEquipment(ch);
        tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s is viewing your equipment."), ch->GetName());
    }
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 파티에 속해 있으므로 가입신청을 할 수 없습니다."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

ACMD(do_monarch_warpto)
{
	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주만이 사용 가능한 기능입니다"));
		return;
	}

	if (!ch->IsMCOK(CHARACTER::MI_WARP))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 초간 쿨타임이 적용중입니다."), ch->GetMCLTime(CHARACTER::MI_WARP));
		return;
	}

	const int WarpPrice = 10000;

	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("국고에 돈이 부족합니다. 현재 : %u 필요금액 : %u"), NationMoney, WarpPrice);
		return;
	}

	int x = 0, y = 0;
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용법: warpto <character name>"));
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("타제국 유저에게는 이동할수 없습니다"));
				return;
			}

			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("해당 유저는 %d 채널에 있습니다. (현재 채널 %d)"), pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 이동할 수 없습니다."));
				return;
			}

			PIXEL_POSITION pos;

			if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map (index %d)", pkCCI->lMapIndex);
			else
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, "You warp to (%d, %d)", pos.x, pos.y);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 에게로 이동합니다"), arg1);
				ch->WarpSet(pos.x, pos.y);

				CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

				ch->SetMC(CHARACTER::MI_WARP);
			}
		}
		else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no one by that name");
		}

		return;
	}
	else
	{
		if (tch->GetEmpire() != ch->GetEmpire())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("타제국 유저에게는 이동할수 없습니다"));
			return;
		}
		if (!IsMonarchWarpZone(tch->GetMapIndex()))
		{
			ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 이동할 수 없습니다."));
			return;
		}
		x = tch->GetX();
		y = tch->GetY();
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 에게로 이동합니다"), arg1);
	ch->WarpSet(x, y);
	ch->Stop();

	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	ch->SetMC(CHARACTER::MI_WARP);
}

ACMD(do_monarch_transfer)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용법: transfer <name>"));
		return;
	}

	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주만이 사용 가능한 기능입니다"));
		return;
	}

	if (!ch->IsMCOK(CHARACTER::MI_TRANSFER))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 초간 쿨타임이 적용중입니다."), ch->GetMCLTime(CHARACTER::MI_TRANSFER));
		return;
	}

	const int WarpPrice = 10000;

	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("국고에 돈이 부족합니다. 현재 : %u 필요금액 : %u"), NationMoney, WarpPrice);
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 제국 유저는 소환할 수 없습니다."));
				return;
			}
			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님은 %d 채널에 접속 중 입니다. (현재 채널: %d)"), arg1, pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 이동할 수 없습니다."));
				return;
			}
			if (!IsMonarchWarpZone(ch->GetMapIndex()))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 소환할 수 없습니다."));
				return;
			}

			TPacketGGTransfer pgg;

			pgg.bHeader = HEADER_GG_TRANSFER;
			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			pgg.lX = ch->GetX();
			pgg.lY = ch->GetY();

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGTransfer));
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님을 소환하였습니다."), arg1);

			CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

			ch->SetMC(CHARACTER::MI_TRANSFER);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("입력하신 이름을 가진 사용자가 없습니다."));
		}

		return;
	}

	if (ch == tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("자신을 소환할 수 없습니다."));
		return;
	}

	if (tch->GetEmpire() != ch->GetEmpire())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 제국 유저는 소환할 수 없습니다."));
		return;
	}
	if (!IsMonarchWarpZone(tch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 이동할 수 없습니다."));
		return;
	}
	if (!IsMonarchWarpZone(ch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("해당 지역으로 소환할 수 없습니다."));
		return;
	}

	//tch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
	tch->WarpSet(ch->GetX(), ch->GetY(), ch->GetMapIndex());

	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	ch->SetMC(CHARACTER::MI_TRANSFER);
}

ACMD(do_monarch_info)
{
	if (CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("나의 군주 정보"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			if (n == ch->GetEmpire())
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s군주] : %s  보유금액 %lld "), EMPIRE_NAME(n), p->name[n], p->money[n]);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s군주] : %s  "), EMPIRE_NAME(n), p->name[n]);

		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주 정보"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s군주] : %s  "), EMPIRE_NAME(n), p->name[n]);

		}
	}
}

ACMD(do_elect)
{
	db_clientdesc->DBPacketHeader(HEADER_GD_COME_TO_VOTE, ch->GetDesc()->GetHandle(), 0);
}

// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

ACMD(do_monarch_tax)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: monarch_tax <1-50>");
		return;
	}

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주만이 사용할수 있는 기능입니다"));
		return;
	}

	int tax = 0;
	str_to_number(tax,  arg1);

	if (tax < 1 || tax > 50)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1-50 사이의 수치를 선택해주세요"));

	quest::CQuestManager::instance().SetEventFlag("trade_tax", tax);

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("세금이 %d %로 설정되었습니다"));

	char szMsg[1024];

	snprintf(szMsg, sizeof(szMsg), "군주의 명으로 세금이 %d %% 로 변경되었습니다", tax);
	BroadcastNotice(szMsg);

	snprintf(szMsg, sizeof(szMsg), "앞으로는 거래 금액의 %d %% 가 국고로 들어가게됩니다.", tax);
	BroadcastNotice(szMsg);

	ch->SetMC(CHARACTER::MI_TAX);
}

static const DWORD cs_dwMonarchMobVnums[] =
{
	191,
	192,
	193,
	194,
	391,
	392,
	393,
	394,
	491,
	492,
	493,
	494,
	591,
	691,
	791,
	1304,
	1901,
	2091,
	2191,
	2206,
	0,
};

ACMD(do_monarch_mob)
{
	char arg1[256];
	LPCHARACTER	tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주만이 사용할수 있는 기능입니다"));
		return;
	}

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mmob <mob name>");
		return;
	}

#ifdef ENABLE_MONARCH_MOB_CMD_MAP_CHECK // @warme006
	BYTE pcEmpire = ch->GetEmpire();
	BYTE mapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(ch->GetMapIndex());
	if (mapEmpire != pcEmpire && mapEmpire != 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("자국 영토에서만 사용할 수 있는 기능입니다"));
		return;
	}
#endif

	const int SummonPrice = 5000000;

	if (!ch->IsMCOK(CHARACTER::MI_SUMMON))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 초간 쿨타임이 적용중입니다."), ch->GetMCLTime(CHARACTER::MI_SUMMON));
		return;
	}

	if (!CMonarch::instance().IsMoneyOk(SummonPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("국고에 돈이 부족합니다. 현재 : %u 필요금액 : %u"), NationMoney, SummonPrice);
		return;
	}

	const CMob * pkMob;
	DWORD vnum = 0;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	DWORD count;

	for (count = 0; cs_dwMonarchMobVnums[count] != 0; ++count)
		if (cs_dwMonarchMobVnums[count] == vnum)
			break;

	if (0 == cs_dwMonarchMobVnums[count])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소환할수 없는 몬스터 입니다. 소환가능한 몬스터는 홈페이지를 참조하세요"));
		return;
	}

	tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750),
			ch->GetY() - number(200, 750),
			ch->GetX() + number(200, 750),
			ch->GetY() + number(200, 750),
			true,
			pkMob->m_table.bType == CHAR_TYPE_STONE,
			true);

	if (tch)
	{
		CMonarch::instance().SendtoDBDecMoney(SummonPrice, ch->GetEmpire(), ch);

		ch->SetMC(CHARACTER::MI_SUMMON);
	}
}

static const char* FN_point_string(int apply_number)
{
	switch (apply_number)
	{
		case POINT_MAX_HP:	return LC_TEXT("최대 생명력 +%d");
		case POINT_MAX_SP:	return LC_TEXT("최대 정신력 +%d");
		case POINT_HT:		return LC_TEXT("체력 +%d");
		case POINT_IQ:		return LC_TEXT("지능 +%d");
		case POINT_ST:		return LC_TEXT("근력 +%d");
		case POINT_DX:		return LC_TEXT("민첩 +%d");
		case POINT_ATT_SPEED:	return LC_TEXT("공격속도 +%d");
		case POINT_MOV_SPEED:	return LC_TEXT("이동속도 %d");
		case POINT_CASTING_SPEED:	return LC_TEXT("쿨타임 -%d");
		case POINT_HP_REGEN:	return LC_TEXT("생명력 회복 +%d");
		case POINT_SP_REGEN:	return LC_TEXT("정신력 회복 +%d");
		case POINT_POISON_PCT:	return LC_TEXT("독공격 %d");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_PCT:	return LC_TEXT("독공격 %d");
#endif
		case POINT_STUN_PCT:	return LC_TEXT("스턴 +%d");
		case POINT_SLOW_PCT:	return LC_TEXT("슬로우 +%d");
		case POINT_CRITICAL_PCT:	return LC_TEXT("%d%% 확률로 치명타 공격");
		case POINT_RESIST_CRITICAL:	return LC_TEXT("상대의 치명타 확률 %d%% 감소");
		case POINT_PENETRATE_PCT:	return LC_TEXT("%d%% 확률로 관통 공격");
		case POINT_RESIST_PENETRATE: return LC_TEXT("상대의 관통 공격 확률 %d%% 감소");
		case POINT_ATTBONUS_HUMAN:	return LC_TEXT("인간류 몬스터 타격치 +%d%%");
		case POINT_ATTBONUS_ANIMAL:	return LC_TEXT("동물류 몬스터 타격치 +%d%%");
		case POINT_ATTBONUS_ORC:	return LC_TEXT("웅귀족 타격치 +%d%%");
		case POINT_ATTBONUS_MILGYO:	return LC_TEXT("밀교류 타격치 +%d%%");
		case POINT_ATTBONUS_UNDEAD:	return LC_TEXT("시체류 타격치 +%d%%");
		case POINT_ATTBONUS_DEVIL:	return LC_TEXT("악마류 타격치 +%d%%");
		case POINT_STEAL_HP:		return LC_TEXT("타격치 %d%% 를 생명력으로 흡수");
		case POINT_STEAL_SP:		return LC_TEXT("타력치 %d%% 를 정신력으로 흡수");
		case POINT_MANA_BURN_PCT:	return LC_TEXT("%d%% 확률로 타격시 상대 전신력 소모");
		case POINT_DAMAGE_SP_RECOVER:	return LC_TEXT("%d%% 확률로 피해시 정신력 회복");
		case POINT_BLOCK:			return LC_TEXT("물리타격시 블럭 확률 %d%%");
		case POINT_DODGE:			return LC_TEXT("활 공격 회피 확률 %d%%");
		case POINT_RESIST_SWORD:	return LC_TEXT("한손검 방어 %d%%");
		case POINT_RESIST_TWOHAND:	return LC_TEXT("양손검 방어 %d%%");
		case POINT_RESIST_DAGGER:	return LC_TEXT("두손검 방어 %d%%");
		case POINT_RESIST_BELL:		return LC_TEXT("방울 방어 %d%%");
		case POINT_RESIST_FAN:		return LC_TEXT("부채 방어 %d%%");
		case POINT_RESIST_BOW:		return LC_TEXT("활공격 저항 %d%%");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_CLAW:		return LC_TEXT("두손검 방어 %d%%");
#endif
		case POINT_RESIST_FIRE:		return LC_TEXT("화염 저항 %d%%");
		case POINT_RESIST_ELEC:		return LC_TEXT("전기 저항 %d%%");
		case POINT_RESIST_MAGIC:	return LC_TEXT("마법 저항 %d%%");
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case POINT_RESIST_MAGIC_REDUCTION:	return LC_TEXT("마법 저항 %d%%");
#endif
		case POINT_RESIST_WIND:		return LC_TEXT("바람 저항 %d%%");
		case POINT_RESIST_ICE:		return LC_TEXT("냉기 저항 %d%%");
		case POINT_RESIST_EARTH:	return LC_TEXT("대지 저항 %d%%");
		case POINT_RESIST_DARK:		return LC_TEXT("어둠 저항 %d%%");
		case POINT_REFLECT_MELEE:	return LC_TEXT("직접 타격치 반사 확률 : %d%%");
		case POINT_REFLECT_CURSE:	return LC_TEXT("저주 되돌리기 확률 %d%%");
		case POINT_POISON_REDUCE:	return LC_TEXT("독 저항 %d%%");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_REDUCE:	return LC_TEXT("독 저항 %d%%");
#endif
		case POINT_KILL_SP_RECOVER:	return LC_TEXT("%d%% 확률로 적퇴치시 정신력 회복");
		case POINT_EXP_DOUBLE_BONUS:	return LC_TEXT("%d%% 확률로 적퇴치시 경험치 추가 상승");
		case POINT_GOLD_DOUBLE_BONUS:	return LC_TEXT("%d%% 확률로 적퇴치시 돈 2배 드롭");
		case POINT_ITEM_DROP_BONUS:	return LC_TEXT("%d%% 확률로 적퇴치시 아이템 2배 드롭");
		case POINT_POTION_BONUS:	return LC_TEXT("물약 사용시 %d%% 성능 증가");
		case POINT_KILL_HP_RECOVERY:	return LC_TEXT("%d%% 확률로 적퇴치시 생명력 회복");
		case POINT_ATT_GRADE_BONUS:	return LC_TEXT("공격력 +%d");
		case POINT_DEF_GRADE_BONUS:	return LC_TEXT("방어력 +%d");
		case POINT_MAGIC_ATT_GRADE:	return LC_TEXT("마법 공격력 +%d");
		case POINT_MAGIC_DEF_GRADE:	return LC_TEXT("마법 방어력 +%d");
		case POINT_MAX_STAMINA:	return LC_TEXT("최대 지구력 +%d");
		case POINT_ATTBONUS_WARRIOR:	return LC_TEXT("무사에게 강함 +%d%%");
		case POINT_ATTBONUS_ASSASSIN:	return LC_TEXT("자객에게 강함 +%d%%");
		case POINT_ATTBONUS_SURA:		return LC_TEXT("수라에게 강함 +%d%%");
		case POINT_ATTBONUS_SHAMAN:		return LC_TEXT("무당에게 강함 +%d%%");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_ATTBONUS_WOLFMAN:	return LC_TEXT("무당에게 강함 +%d%%");
#endif
		case POINT_ATTBONUS_MONSTER:	return LC_TEXT("몬스터에게 강함 +%d%%");
		case POINT_MALL_ATTBONUS:		return LC_TEXT("공격력 +%d%%");
		case POINT_MALL_DEFBONUS:		return LC_TEXT("방어력 +%d%%");
		case POINT_MALL_EXPBONUS:		return LC_TEXT("경험치 %d%%");
		case POINT_MALL_ITEMBONUS:		return LC_TEXT("아이템 드롭율 %d배"); // @fixme180 float to int
		case POINT_MALL_GOLDBONUS:		return LC_TEXT("돈 드롭율 %d배"); // @fixme180 float to int
		case POINT_MAX_HP_PCT:			return LC_TEXT("최대 생명력 +%d%%");
		case POINT_MAX_SP_PCT:			return LC_TEXT("최대 정신력 +%d%%");
		case POINT_SKILL_DAMAGE_BONUS:	return LC_TEXT("스킬 데미지 %d%%");
		case POINT_NORMAL_HIT_DAMAGE_BONUS:	return LC_TEXT("평타 데미지 %d%%");
		case POINT_SKILL_DEFEND_BONUS:		return LC_TEXT("스킬 데미지 저항 %d%%");
		case POINT_NORMAL_HIT_DEFEND_BONUS:	return LC_TEXT("평타 데미지 저항 %d%%");
		case POINT_RESIST_WARRIOR:	return LC_TEXT("무사공격에 %d%% 저항");
		case POINT_RESIST_ASSASSIN:	return LC_TEXT("자객공격에 %d%% 저항");
		case POINT_RESIST_SURA:		return LC_TEXT("수라공격에 %d%% 저항");
		case POINT_RESIST_SHAMAN:	return LC_TEXT("무당공격에 %d%% 저항");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_WOLFMAN:	return LC_TEXT("무당공격에 %d%% 저항");
#endif
		default:					return "UNK_ID %d%%"; // @fixme180
	}
}

static bool FN_hair_affect_string(LPCHARACTER ch, char *buf, size_t bufsiz)
{
	if (NULL == ch || NULL == buf)
		return false;

	CAffect* aff = NULL;
	time_t expire = 0;
	struct tm ltm;
	int	year, mon, day;
	int	offset = 0;

	aff = ch->FindAffect(AFFECT_HAIR);

	if (NULL == aff)
		return false;

	expire = ch->GetQuestFlag("hair.limit_time");

	if (expire < get_global_time())
		return false;

	// set apply string
	offset = snprintf(buf, bufsiz, FN_point_string(aff->bApplyOn), aff->lApplyValue);

	if (offset < 0 || offset >= (int) bufsiz)
		offset = bufsiz - 1;

	localtime_r(&expire, &ltm);

	year	= ltm.tm_year + 1900;
	mon		= ltm.tm_mon + 1;
	day		= ltm.tm_mday;

	snprintf(buf + offset, bufsiz - offset, LC_TEXT(" (만료일 : %d년 %d월 %d일)"), year, mon, day);

	return true;
}

ACMD(do_costume)
{
	char buf[1024]; // @warme015
	const size_t bufferSize = sizeof(buf);

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	CItem* pBody = ch->GetWear(WEAR_COSTUME_BODY);
	CItem* pHair = ch->GetWear(WEAR_COSTUME_HAIR);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	CItem* pMount = ch->GetWear(WEAR_COSTUME_MOUNT);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CItem* pAcce = ch->GetWear(WEAR_COSTUME_ACCE);
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	CItem* pWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "COSTUME status:");

	if (pHair)
	{
		const char* itemName = pHair->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  HAIR : %s", itemName);

		for (int i = 0; i < pHair->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pHair->GetAttribute(i);
			if (0 < attr.bType)
			{
				snprintf(buf, bufferSize, FN_point_string(attr.bType), attr.sValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pHair->IsEquipped() && arg1[0] == 'h')
			ch->UnequipItem(pHair);
	}

	if (pBody)
	{
		const char* itemName = pBody->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  BODY : %s", itemName);

		if (pBody->IsEquipped() && arg1[0] == 'b')
			ch->UnequipItem(pBody);
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (pMount)
	{
		const char* itemName = pMount->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  MOUNT : %s", itemName);

		if (pMount->IsEquipped() && arg1[0] == 'm')
			ch->UnequipItem(pMount);
	}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (pAcce)
	{
		const char* itemName = pAcce->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  ACCE : %s", itemName);

		if (pAcce->IsEquipped() && arg1[0] == 'a')
			ch->UnequipItem(pAcce);
	}
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (pWeapon)
	{
		const char* itemName = pWeapon->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  WEAPON : %s", itemName);

		if (pWeapon->IsEquipped() && arg1[0] == 'w')
			ch->UnequipItem(pWeapon);
	}
#endif
}

ACMD(do_hair)
{
	char buf[256];

	if (false == FN_hair_affect_string(ch, buf, sizeof(buf)))
		return;

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s",
						index, item ? item->GetName() : "<NONE>");
		++index;
	}
}

//gift notify quest command
ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

ACMD(do_cube)
{
	if (!ch->CanDoCube())
		return;

	sys_log(1, "CUBE COMMAND <%s>: %s", ch->GetName(), argument);
	int cube_index = 0, inven_index = 0;
	const char *line;

	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		// print usage
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: cube open");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube close");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube add <inveltory_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube delete <cube_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube list");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube cancel");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube make [all]");
		return;
	}

	const std::string& strArg1 = std::string(arg1);

	// r_info (request information)

	//					    (Server -> Client) /cube r_list npcVNUM resultCOUNT 123,1/125,1/128,1/130,5

	//					   (Server -> Client) /cube m_info startIndex count 125,1|126,2|127,2|123,5&555,5&555,4/120000@125,1|126,2|127,2|123,5&555,5&555,4/120000

	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		case 'c':	// close
			Cube_close(ch);
			break;

		case 'l':	// list
			Cube_show_list(ch);
			break;

		case 'a':	// add cue_index inven_index
			{
				if (0 == arg2[0] || !isdigit(*arg2) ||
					0 == arg3[0] || !isdigit(*arg3))
					return;

				str_to_number(cube_index, arg2);
				str_to_number(inven_index, arg3);
				Cube_add_item (ch, cube_index, inven_index);
			}
			break;

		case 'd':	// delete
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;

				str_to_number(cube_index, arg2);
				Cube_delete_item (ch, cube_index);
			}
			break;

		case 'm':	// make
			if (0 != arg2[0])
			{
				while (true == Cube_make(ch))
					sys_log(1, "cube make success");
			}
			else
				Cube_make(ch);
			break;

		default:
			return;
	}
}

ACMD(do_in_game_mall)
{
	if (LC_IsEurope() == true)
	{
		char country_code[3];

		switch (LC_GetLocalType())
		{
			case LC_GERMANY:	country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_FRANCE:		country_code[0] = 'f'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_ITALY:		country_code[0] = 'i'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_SPAIN:		country_code[0] = 'e'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_UK:			country_code[0] = 'e'; country_code[1] = 'n'; country_code[2] = '\0'; break;
			case LC_TURKEY:		country_code[0] = 't'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_POLAND:		country_code[0] = 'p'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_PORTUGAL:	country_code[0] = 'p'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_GREEK:		country_code[0] = 'g'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_RUSSIA:		country_code[0] = 'r'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_DENMARK:	country_code[0] = 'd'; country_code[1] = 'k'; country_code[2] = '\0'; break;
			case LC_BULGARIA:	country_code[0] = 'b'; country_code[1] = 'g'; country_code[2] = '\0'; break;
			case LC_CROATIA:	country_code[0] = 'h'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_MEXICO:		country_code[0] = 'm'; country_code[1] = 'x'; country_code[2] = '\0'; break;
			case LC_ARABIA:		country_code[0] = 'a'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_CZECH:		country_code[0] = 'c'; country_code[1] = 'z'; country_code[2] = '\0'; break;
			case LC_ROMANIA:	country_code[0] = 'r'; country_code[1] = 'o'; country_code[2] = '\0'; break;
			case LC_HUNGARY:	country_code[0] = 'h'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_NETHERLANDS: country_code[0] = 'n'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_USA:		country_code[0] = 'u'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_CANADA:	country_code[0] = 'c'; country_code[1] = 'a'; country_code[2] = '\0'; break;
			default:
				if (test_server == true)
				{
					country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0';
				}
				break;
		}

		char buf[512+1];
		char sas[33];
		MD5_CTX ctx;
		const char sas_key[] = "GF9001";

		snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

		MD5Init(&ctx);
		MD5Update(&ctx, (const unsigned char *) buf, strlen(buf));
#ifdef __FreeBSD__
		MD5End(&ctx, sas);
#else
		static const char hex[] = "0123456789abcdef";
		unsigned char digest[16];
		MD5Final(digest, &ctx);
		int i;
		for (i = 0; i < 16; ++i) {
			sas[i+i] = hex[digest[i] >> 4];
			sas[i+i+1] = hex[digest[i] & 0x0f];
		}
		sas[i+i] = '\0';
#endif

		snprintf(buf, sizeof(buf), "mall http://%s/ishop?pid=%u&c=%s&sid=%d&sas=%s",
				g_strWebMallURL.c_str(), ch->GetPlayerID(), country_code, g_server_id, sas);

		ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
	}
}

ACMD(do_dice)
{
	char arg1[256], arg2[256];
	int start = 1, end = 100;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		start = atoi(arg1);
		end = atoi(arg2);
	}
	else if (*arg1 && !*arg2)
	{
		start = 1;
		end = atoi(arg1);
	}

	end = MAX(start, end);
	start = MIN(start, end);

	int n = number(start, end);

#ifdef ENABLE_DICE_SYSTEM
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_DICE_INFO, LC_TEXT("%s님이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_DICE_INFO, LC_TEXT("당신이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), n, start, end);
#else
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_INFO, LC_TEXT("%s님이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("당신이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), n, start, end);
#endif
}

#ifdef ENABLE_NEWSTUFF
ACMD(do_click_safebox)
{
	if ((ch->GetGMLevel() <= GM_PLAYER) && (ch->GetDungeon() || ch->GetWarMap()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open the safebox in dungeon or at war."));
		return;
	}

	if (!check_allow(ch->GetGMLevel(), GM_ALLOW_USE_SAFEBOX))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this with this gamemaster rank."));
		return;
	}
	
	ch->SetSafeboxOpenPosition();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
}
ACMD(do_force_logout)
{
	LPDESC pDesc=DESC_MANAGER::instance().FindByCharacterName(ch->GetName());
	if (!pDesc)
		return;
	pDesc->DelayedDisconnect(0);
}
#endif

ACMD(do_click_mall)
{
	return;
	// ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

ACMD(do_ride)
{
    sys_log(1, "[DO_RIDE] start");
    if (ch->IsDead() || ch->IsStun())
	return;

    {
	if (ch->IsHorseRiding())
	{
	    sys_log(1, "[DO_RIDE] stop riding");
	    ch->StopRiding();
	    return;
	}

	if (ch->GetMountVnum())
	{
	    sys_log(1, "[DO_RIDE] unmount");
	    do_unmount(ch, NULL, 0, 0);
	    return;
	}
    }

    {
	if (ch->GetHorse() != NULL)
	{
	    sys_log(1, "[DO_RIDE] start riding");
	    ch->StartRiding();
	    return;
	}

	for (BYTE i=0; i<INVENTORY_MAX_NUM; ++i)
	{
	    LPITEM item = ch->GetInventoryItem(i);
	    if (NULL == item)
			continue;

		if (item->IsRideItem())
		{
			if (
				NULL==ch->GetWear(WEAR_UNIQUE1)
				|| NULL==ch->GetWear(WEAR_UNIQUE2)
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
				|| NULL==ch->GetWear(WEAR_COSTUME_MOUNT)
#endif
			)
			{
				sys_log(1, "[DO_RIDE] USE UNIQUE ITEM");
				//ch->EquipItem(item);
				ch->UseItem(TItemPos (INVENTORY, i));
				return;
			}
		}

	    switch (item->GetVnum())
	    {
		case 71114:
		case 71116:
		case 71118:
		case 71120:
		    sys_log(1, "[DO_RIDE] USE QUEST ITEM");
		    ch->UseItem(TItemPos (INVENTORY, i));
		    return;
	    }

		if( (item->GetVnum() > 52000) && (item->GetVnum() < 52091) )	{
			sys_log(1, "[DO_RIDE] USE QUEST ITEM");
			ch->UseItem(TItemPos (INVENTORY, i));
		    return;
		}
	}
    }

    ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 먼저 소환해주세요."));
}

#ifdef ENABLE_MOVE_CHANNEL
ACMD(DoChangeChannel)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	WORD channel = 0;
	str_to_number(channel, arg1);
	if (!channel)
		return;

	ch->ChangeChannel(channel);
}
#endif

#ifdef __AUTO_SKILL_READER__
ACMD(do_auto_skill_reader)
{
	// TODO : add pulse manager
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2)
	{
		return;
	}
	else if (vecArgs[1] == "status")
	{
		if (vecArgs.size() < 4)
		{
			return;
		}

		BYTE skillIdx, status;
		if (!str_to_number(skillIdx, vecArgs[2].c_str()) || !str_to_number(status, vecArgs[3].c_str()))
			return;

		ch->GetAutoSkill(skillIdx, status ? true : false);
	}
}
#endif


#ifdef ENABLE_PLAYER_AFFECT_REMOVE
ACMD(do_remove_polymorph)
{
	if (!ch)
	{
		return;
	}

	if (!ch->IsPolymorphed())
	{
		return;
	}

	ch->SetPolymorph(0);
	ch->RemoveAffect(AFFECT_POLYMORPH);
}

ACMD(do_player_remove_affect)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	DWORD affectvnum = 0;
	str_to_number(affectvnum, arg1);

	if (!ch->IsAffectFlag(affectvnum))
	{
		return;
	}

	if (affectvnum != AFF_HOSIN && affectvnum != AFF_BOHO && affectvnum != AFF_GICHEON && affectvnum != AFF_KWAESOK && affectvnum != AFF_JEUNGRYEOK
#ifdef ENABLE_WOLFMAN_CHARACTER
			&& affectvnum != AFF_BLUE_POSSESSION
#endif
#ifdef ENABLE_BUFF_ITEMS_SYSTEM
			&& affectvnum != AFF_RESIST_BUFF && affectvnum != AFF_CRITICAL_BUFF && affectvnum != AFF_REFLECT_BUFF
			&& affectvnum != AFF_MOV_SPEED_BUFF && affectvnum != AFF_ATT_GRADE_BUFF
#endif
	   )
	{
		return;
	}

	CAffect *pAffect = ch->FindAffectByFlag(affectvnum);
	if (!pAffect)
	{
		return;
	}

	ch->RemoveAffect(pAffect);
}
#endif


#ifdef ENABLE_RENEWAL_ADD_STATS
ACMD(do_stat_val)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	int val = 0;
	str_to_number(val, arg2);

	if (!*arg1 || val <= 0)
	{
		return;
	}

	if (CPVPManager::Instance().IsFighting(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't do this during a duel."));
		return;
	}

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your state as long as you are transformed."));
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
	{
		return;
	}

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
	{
		idx = POINT_ST;
	}
	else if (!strcmp(arg1, "dx"))
	{
		idx = POINT_DX;
	}
	else if (!strcmp(arg1, "ht"))
	{
		idx = POINT_HT;
	}
	else if (!strcmp(arg1, "iq"))
	{
		idx = POINT_IQ;
	}
	else
	{
		return;
	}

	if (ch->GetRealPoint(idx) >= MAX_STAT)
	{
		return;
	}

	if (val > ch->GetPoint(POINT_STAT))
	{
		val = ch->GetPoint(POINT_STAT);
	}

	if (ch->GetRealPoint(idx) + val > MAX_STAT)
	{
		val = MAX_STAT - ch->GetRealPoint(idx);
	}

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + val);
	ch->SetPoint(idx, ch->GetPoint(idx) + val);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -val);
	ch->ComputePoints();
}
#endif

#ifdef ENABLE_SAVE_BLOCK_ATTR
ACMD(do_save_block_attr)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::Unstuck, std::chrono::milliseconds(3000)))
		return ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d s! Try again later."), 3);
#endif

	// TODO : Add check for open windows

	LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());
	if (pkSectreeMap)
	{
		LPSECTREE tree = pkSectreeMap->Find(ch->GetX(), ch->GetY());
		if (tree)
		{
			const DWORD dwAttr = tree->GetAttribute(ch->GetX(), ch->GetY());
			if (!IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT | ATTR_WATER))
				return;
		}
	}

	const long lastX = ch->GetXLast(), lastY = ch->GetYLast(),lastZ = ch->GetZLast();
	if (lastX != 0 && lastX != 0)
		ch->Show(ch->GetMapIndex(), lastX, lastY, lastZ);
}
#endif

#ifdef RENEWAL_PICKUP_AFFECT
ACMD(do_pickup_affect)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::PickupAffect, std::chrono::milliseconds(3000)))
		return ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d s! Try again later."), 3);
#endif

	CAffect* affect = ch->FindAffect(AFFECT_PICKUP_ENABLE);
	if (!affect)
	{
		affect = ch->FindAffect(AFFECT_PICKUP_DEACTIVE);
		if (!affect)
			return;
	}

	const int64_t duration = affect->dwExpireTime;
	const bool isActive = affect->dwType == AFFECT_PICKUP_ENABLE;
	ch->RemoveAffect(affect);
	if (duration <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Affect is removed.");
		return;
	}
	ch->AddAffect(isActive ? AFFECT_PICKUP_DEACTIVE : AFFECT_PICKUP_ENABLE, POINT_NONE, 0, AFF_NONE, duration, 0, 0, false, duration);
}
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
ACMD(do_open_search_shop)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::OpenSearchShop, std::chrono::milliseconds(1000)))
		return ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d s! Try again later."), 1);
#endif
	if (!ch)
	{
		return;
	}

	ch->OpenShopSearch(MODE_TRADING);
}
#endif

#ifdef ENABLE_ANTI_EXP
ACMD(do_anti_exp)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::AntiExpButton, std::chrono::milliseconds(3000)))
		return ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d s! Try again later."), 3);
#endif
	ch->SetAntiExp(!ch->GetAntiExp());
	ch->ChatPacket(CHAT_TYPE_COMMAND, "SetAntiExp %d", ch->GetAntiExp()?1:0);
}
#endif

// ACMD(do_quick_open)
// {
// 	std::vector<std::string> vecArgs;
// 	split_argument(argument, vecArgs);

// 	if (vecArgs.size() < 5) { return; }

// #ifdef ENABLE_NEWSTUFF
// 	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::QuickOpen, std::chrono::milliseconds(2000)))
// 		return ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d s! Try again later."), 2);
// #endif

// 	BYTE invType;
// 	str_to_number(invType, vecArgs[2].c_str());

// 	WORD invPos;
// 	str_to_number(invPos, vecArgs[3].c_str());

// 	WORD clickCount;
// 	str_to_number(clickCount, vecArgs[4].c_str());

// 	const LPITEM item = ch->GetItem(TItemPos(invType, invPos));
// 	if (!item)
// 		return;

// 	if (item->isLocked() || item->IsExchanging())
// 		return;

// 	if (!ch->CanHandleItem())
// 		return;

// 	ch->OpenChest(item, clickCount);
// }

#ifdef ENABLE_QUICK_OPEN
ACMD(do_quick_open)
{
	if (!ch || !ch->CanWarp())
	{
		return;
	}

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::QuickOpen, std::chrono::milliseconds(1000)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Pulse Manager] You need to wait 1 second."));
		return;
	}
#endif

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || !isnhdigit(*arg1))
	{
		return;
	}

	LPITEM item = ch->GetInventoryItem(atoi(arg1));
	if (!item)
	{
		return;
	}

	ch->QuickOpenStack(item);
}
#endif

#ifdef __SPIN_WHEEL__
ACMD(do_spin_wheel)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "spin")
	{
		if (ch->IsHack() || !ch->CanHandleItem() || ch->GetQuestFlag("spin_wheel.count") < 200 || quest::CQuestManager::instance().GetEventFlag("spin_wheel") != 1)
			return;

#ifdef ENABLE_NEWSTUFF
		if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::WheelSpin, std::chrono::milliseconds(3000)))
			return ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d s! Try again later."), 3);
#endif

		ch->SetQuestFlag("spin_wheel.count", 0);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SetSpinWheel 0 1");

		struct spinItem
		{
			BYTE rareType;
			DWORD itemIdx;
			WORD itemCount;
			spinItem(const BYTE _rareType, const DWORD _itemIdx, const WORD _itemCount) : rareType(_rareType), itemIdx(_itemIdx), itemCount(_itemCount) {}
			spinItem() {}
		};
		struct spinStruct
		{
			WORD wStartLevel, wEndLevel;
			std::vector<spinItem> vecItems;
			spinItem jackPotItem;
			spinStruct(const WORD _wStartLevel, const WORD _wEndLevel, const std::vector<spinItem>& _vecItems, const spinItem& _jackPotItem) : wStartLevel(_wStartLevel), wEndLevel(_wEndLevel), vecItems(_vecItems), jackPotItem(){
				thecore_memcpy(&jackPotItem, &_jackPotItem, sizeof(jackPotItem));
			}
		};

		const static std::vector<spinStruct> vecSpinData = {
			spinStruct(
				1, 
				175, 
				{
					spinItem(0, 95778, 5), spinItem(0, 95778, 10), spinItem(0, 95778, 15), spinItem(0, 95778, 20), spinItem(0, 95778, 25),
					spinItem(0, 39070, 5), spinItem(0, 71646, 5), spinItem(0, 71646, 10), spinItem(0, 19070, 3), spinItem(0, 19070, 5),
					spinItem(1, 50515, 3), spinItem(1, 50515, 5), spinItem(1, 65792, 5), spinItem(1, 65793, 5), spinItem(1, 65794, 5),
					spinItem(1, 72064, 10), spinItem(1, 72065, 5), spinItem(1, 72066, 5), spinItem(1, 72067, 5), spinItem(1, 31000, 10),
					spinItem(2, 77940, 2), spinItem(2, 77940, 3), spinItem(2, 77940, 5), spinItem(2, 71524, 2), spinItem(2, 77404, 3)
				},
				spinItem(3, 80015, 1)
			),
		};

		for (auto it = vecSpinData.begin(); it != vecSpinData.end(); ++it)
		{
			const spinStruct& spinData = *it;
			if (ch->GetLevel() >= spinData.wStartLevel && ch->GetLevel() <= spinData.wEndLevel)
			{
				DWORD selectedItemIdx = 0, selectedItemCount = 0;

				if (number(1, 50) == 1)
				{
					selectedItemIdx = spinData.jackPotItem.itemIdx;
					selectedItemCount = spinData.jackPotItem.itemCount;
				}
				else
				{
					const WORD idx = spinData.vecItems.size() <= 1 ? 0 : number(0, spinData.vecItems.size() - 1);
					selectedItemIdx = spinData.vecItems[idx].itemIdx;
					selectedItemCount = spinData.vecItems[idx].itemCount;

				}

				std::string cmd("");
				for (BYTE j = 0; j < spinData.vecItems.size(); ++j)
				{
					cmd += std::to_string(spinData.vecItems[j].rareType);
					cmd += "|";
					cmd += std::to_string(spinData.vecItems[j].itemIdx);
					cmd += "|";
					cmd += std::to_string(spinData.vecItems[j].itemCount);
					cmd += "?";
				}

				cmd += std::to_string(spinData.jackPotItem.rareType);
				cmd += "|";
				cmd += std::to_string(spinData.jackPotItem.itemIdx);
				cmd += "|";
				cmd += std::to_string(spinData.jackPotItem.itemCount);

				if (cmd == "")
					cmd = "-";
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SetSpinReward %u %u %s", selectedItemIdx, selectedItemCount, cmd.c_str());
				ch->SetProtectTime("spint_itemidx", selectedItemIdx);
				ch->SetProtectTime("spint_itemcount", selectedItemCount);
				return;
			}
		}
	}
	else if (vecArgs[1] == "ani_done")
	{
		if (!ch->GetProtectTime("spint_itemidx") || !ch->GetProtectTime("spint_itemcount"))
			return;
		ch->AutoGiveItem(ch->GetProtectTime("spint_itemidx"), ch->GetProtectTime("spint_itemcount"));
		ch->SetProtectTime("spint_itemidx", 0);
		ch->SetProtectTime("spint_itemcount", 0);
		ch->ChatPacket(CHAT_TYPE_INFO, "Successfuly gived spin reward.");
	}
}
#endif

#ifdef __SPIN_WHEEL_BOSS__
ACMD(do_spin_wheel_boss)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "spin")
	{
		if (ch->IsHack() || !ch->CanHandleItem() || ch->GetQuestFlag("spin_wheel_boss.count") < 50 || quest::CQuestManager::instance().GetEventFlag("spin_wheel_boss") != 1)
			return;

		ch->SetQuestFlag("spin_wheel_boss.count", 0);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SetSpinWheelBoss 0 1");

		struct spinItem
		{
			BYTE rareType;
			DWORD itemIdx;
			WORD itemCount;
			spinItem(const BYTE _rareType, const DWORD _itemIdx, const WORD _itemCount) : rareType(_rareType), itemIdx(_itemIdx), itemCount(_itemCount) {}
			spinItem() {}
		};
		struct spinStruct
		{
			WORD wStartLevel, wEndLevel;
			std::vector<spinItem> vecItems;
			spinItem jackPotItem;
			spinStruct(const WORD _wStartLevel, const WORD _wEndLevel, const std::vector<spinItem>& _vecItems, const spinItem& _jackPotItem) : wStartLevel(_wStartLevel), wEndLevel(_wEndLevel), vecItems(_vecItems), jackPotItem(){
				thecore_memcpy(&jackPotItem, &_jackPotItem, sizeof(jackPotItem));
			}
		};

		const static std::vector<spinStruct> vecSpinData = {
			spinStruct(
				1, 
				175, 
				{
					spinItem(0, 95778, 5), spinItem(0, 95778, 10), spinItem(0, 95778, 15), spinItem(0, 95778, 20), spinItem(0, 95778, 25),
					spinItem(0, 71647, 5), spinItem(0, 71647, 10), spinItem(0, 71647, 15), spinItem(0, 71647, 20), spinItem(0, 71647, 25),
					spinItem(1, 19070, 5), spinItem(1, 19070, 10), spinItem(1, 19070, 15), spinItem(1, 19070, 20), spinItem(1, 19070, 25),
					spinItem(1, 50515, 10), spinItem(1, 50515, 15), spinItem(1, 50515, 20), spinItem(1, 50515, 25), spinItem(1, 50515, 30),
					spinItem(2, 77940, 2), spinItem(2, 77940, 3), spinItem(2, 77940, 5), spinItem(2, 71524, 2), spinItem(2, 77404, 3)
				},
				spinItem(3, 80016, 1)
			),
		};

		for (auto it = vecSpinData.begin(); it != vecSpinData.end(); ++it)
		{
			const spinStruct& spinData = *it;
			if (ch->GetLevel() >= spinData.wStartLevel && ch->GetLevel() <= spinData.wEndLevel)
			{
				DWORD selectedItemIdx = 0, selectedItemCount = 0;

				if (number(1, 50) == 1)
				{
					selectedItemIdx = spinData.jackPotItem.itemIdx;
					selectedItemCount = spinData.jackPotItem.itemCount;
				}
				else
				{
					const WORD idx = spinData.vecItems.size() <= 1 ? 0 : number(0, spinData.vecItems.size() - 1);
					selectedItemIdx = spinData.vecItems[idx].itemIdx;
					selectedItemCount = spinData.vecItems[idx].itemCount;

				}

				std::string cmd("");
				for (BYTE j = 0; j < spinData.vecItems.size(); ++j)
				{
					cmd += std::to_string(spinData.vecItems[j].rareType);
					cmd += "|";
					cmd += std::to_string(spinData.vecItems[j].itemIdx);
					cmd += "|";
					cmd += std::to_string(spinData.vecItems[j].itemCount);
					cmd += "?";
				}

				cmd += std::to_string(spinData.jackPotItem.rareType);
				cmd += "|";
				cmd += std::to_string(spinData.jackPotItem.itemIdx);
				cmd += "|";
				cmd += std::to_string(spinData.jackPotItem.itemCount);

				if (cmd == "")
					cmd = "-";
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SetSpinRewardBoss %u %u %s", selectedItemIdx, selectedItemCount, cmd.c_str());
				ch->SetProtectTime("spintboss_itemidx", selectedItemIdx);
				ch->SetProtectTime("spintboss_itemcount", selectedItemCount);
				return;
			}
		}
	}
	else if (vecArgs[1] == "ani_done")
	{
		if (!ch->GetProtectTime("spintboss_itemidx") || !ch->GetProtectTime("spintboss_itemcount"))
			return;
		ch->AutoGiveItem(ch->GetProtectTime("spintboss_itemidx"), ch->GetProtectTime("spintboss_itemcount"));
		ch->SetProtectTime("spintboss_itemidx", 0);
		ch->SetProtectTime("spintboss_itemcount", 0);
		ch->ChatPacket(CHAT_TYPE_INFO, "Successfuly gived Boss spin reward.");
	}
}
#endif

#ifdef ENABLE_GLOBAL_REWARD
ACMD(do_reward_system)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "update")
	{
		CHARACTER_MANAGER::Instance().SendRewardInfo(255, true, false, ch);
		// CHARACTER_MANAGER::Instance().SendRewardInfo(255, false, false, ch);
	}
}
#endif

#ifdef ENABLE_ANTI_MULTIPLE_FARM
ACMD(do_debug_anti_multiple_farm)
{
	LPDESC d = nullptr;
	if (!ch || (ch && !(d = ch->GetDesc())))
		return;
	
	CAntiMultipleFarm::instance().PrintPlayerDropState(d->GetLoginMacAdress(), ch);
}
#endif

#ifdef ENABLE_VOTE_4_BUFF
ACMD(do_vote4buff)
{
	// split args
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 1)
	{
		return;
	}

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::Vote4Buff, std::chrono::milliseconds(10000)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait 10 seconds."));
		return;
	}
#endif

	// if (ch->GetProtectTime("vote4buff") > get_global_time())
	// {
	// 	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d s! Try again later."), ch->GetProtectTime("vote4buff") - get_global_time());
	// 	return;
	// }
	// ch->SetProtectTime("vote4buff", get_global_time() + 10);

	// TODO: Add check for windows

	// // check for bonus
	DWORD bonusAffect;
	str_to_number(bonusAffect, vecArgs[1].c_str());

	if (bonusAffect < 1)
		return;

	// check for affect
	bool isChangeBonus = false;
	
	auto pAffect = ch->FindAffect(AFFECT_VOTE_4_BUFF);
	long iDurAffect = 86400;
#ifdef ENABLE_GEM_SYSTEM
	if (pAffect)
	{
		if (ch->GetGem() < 5)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You need 5 Gaya Points to change the bonus! Du benotigst 5 Gaya, um den Bonus zu andern!");
			return;
		}
		
		iDurAffect = pAffect->lDuration;
		isChangeBonus = true;
		ch->RemoveAffect(AFFECT_VOTE_4_BUFF);
	}
#endif
	// check for vote
	const int iNeedCashValue = 1; // edit me value
	
	if (!isChangeBonus && ch->GetCash() < iNeedCashValue)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You dont have enough vote coins. Du hast keinen Vote Coin."));
        return;
	}

	//CQuestManager& q = quest::CQuestManager::instance();
	DWORD dwPlayerId = ch->GetDesc()->GetAccountTable().id;
	std::string voteGameFlag = std::to_string(dwPlayerId) + "_vote4buff_affect";
	std::string voteGameFlagApply = std::to_string(dwPlayerId) + "_vote4buff_apply";
	std::string voteGameFlagValue = std::to_string(dwPlayerId) + "_vote4buff_value";
	
	auto leftTime = quest::CQuestManager::instance().GetEventFlag(voteGameFlag) - get_global_time();
	if (!isChangeBonus && leftTime > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already voted in the last 24 hours. Du hast bereits in den letzten 24 Stunden gevotet."));
        return;
	}

	// auto dwBonusExpireTime = duration - dbVoteTime;
	// give bonus
	if (bonusAffect == 1)
	{
		ch->AddAffect(AFFECT_VOTE_4_BUFF, POINT_ATTBONUS_HUMAN, 15, 0, iDurAffect, 0, false, false, (get_global_time() + iDurAffect));
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Succesfully added affect. Bonus erfolgreich hinzugefugt."));
	
		quest::CQuestManager::instance().RequestSetEventFlag(voteGameFlagApply, POINT_ATTBONUS_HUMAN);
		quest::CQuestManager::instance().RequestSetEventFlag(voteGameFlagValue, 15);
	}
	else if (bonusAffect == 2)
	{
		ch->AddAffect(AFFECT_VOTE_4_BUFF, POINT_ATTBONUS_MONSTER, 20, 0, iDurAffect, 0, false, (get_global_time() + iDurAffect));
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Succesfully added affect. Bonus erfolgreich hinzugefugt."));
		
		quest::CQuestManager::instance().RequestSetEventFlag(voteGameFlagApply, POINT_ATTBONUS_MONSTER);
		quest::CQuestManager::instance().RequestSetEventFlag(voteGameFlagValue, 20);
	}
	else if (bonusAffect == 3)
	{
		ch->AddAffect(AFFECT_VOTE_4_BUFF, POINT_ITEM_DROP_BONUS, 20, 0, iDurAffect, 0, false, (get_global_time() + iDurAffect));
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Succesfully added affect. Bonus erfolgreich hinzugefugt."));
		
		quest::CQuestManager::instance().RequestSetEventFlag(voteGameFlagApply, POINT_ITEM_DROP_BONUS);
		quest::CQuestManager::instance().RequestSetEventFlag(voteGameFlagValue, 20);
	}

	if (!isChangeBonus)
		ch->SetCash(ch->GetCash()-iNeedCashValue);
#ifdef ENABLE_GEM_SYSTEM
	else
		ch->PointChange(POINT_GEM, -5, false);
#endif

	quest::CQuestManager::instance().RequestSetEventFlag(voteGameFlag, (get_global_time() + iDurAffect));
}
#endif

#ifdef ENABLE_RANK_PLAYER
ACMD(do_request_rank_info)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::RequestRankInfo, std::chrono::milliseconds(1000)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait 1 second."));
		return;
	}
#endif

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;
	
	BYTE i = 0;
	str_to_number(i, arg1);

	if (i >= 0 && i < RANK_MAX)
		RankPlayer::instance().RequestInfoRank(ch, i);
}
#endif

#ifdef __RANKING_SYSTEM__
ACMD(do_request_rank_info)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::RequestRankInfo, std::chrono::milliseconds(1000)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait 1 second."));
		return;
	}
#else
	int iPulse = thecore_pulse();
	
	if (iPulse - ch->GetRequestRankTimer() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait %d seconds and try again."), 1);
		return;
	}
#endif

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;
	
	BYTE i = 0;
	str_to_number(i, arg1);

	if (i >= 0 && i < RANK_MAX)
		RankPlayer::instance().RequestInfoRank(ch, i);
	
	ch->SetRequestRankTimer();
}
#endif

#ifdef ENABLE_SORT_INVENTORY
bool sortByType(CItem* a, CItem* b)
{
	return (a->GetType() < b->GetType());
}

bool sortBySubType(CItem* a, CItem* b)
{
	return (a->GetSubType() < b->GetSubType());
}

bool sortByVnum(CItem* a, CItem* b)
{
	return (a->GetVnum() < b->GetVnum());
}

bool sortBySocket(CItem* a, CItem* b)
{
	return (a->GetSocket(0) < b->GetSocket(0));
}

ACMD(do_sort_inventory)
{
	if (!ch)
		return;

	if (!ch->CanHandleItem())
	{
		return;
	}

	if (quest::CQuestManager::instance().GetEventFlag("disable_sort_feature") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Feature disabled is for now."));
		return;
	}

	if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef __PREMIUM_PRIVATE_SHOP__
			|| ch->IsEditingPrivateShop() || ch->IsShopSearch() || ch->GetViewingPrivateShop() 
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			|| ch->IsAcceOpened(true) || ch->IsAcceOpened(false)
#endif
#if defined(__BL_67_ATTR__)
			|| ch->Is67AttrOpen()
#endif
	)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sort inventory while another window is open."));
		return;
	}

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been cancelled."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	if (!ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait 10 seconds after opening any window!");
		return;
	}

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::InventorySort, std::chrono::milliseconds(15000)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait 15 seconds."));
		return;
	}
#endif

	std::vector<CItem*> collectItems;
	int totalSize = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);

		if (item)
		{
			totalSize += item->GetSize();
			collectItems.push_back(item);
		}
	}

	if (totalSize - 3 >= INVENTORY_MAX_NUM)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("INVENTORY_FULL_CANNOT_SORT"));
		return;
	}

	for (std::vector<CItem*>::iterator it = collectItems.begin() ; it != collectItems.end(); ++it)
	{
		LPITEM item = *it;
		item->RemoveFromCharacter();
	}

	std::sort(collectItems.begin(), collectItems.end(), sortByType);
	std::sort(collectItems.begin(), collectItems.end(), sortBySubType);
	std::sort(collectItems.begin(), collectItems.end(), sortByVnum);
	std::sort(collectItems.begin(), collectItems.end(), sortBySocket);

#ifdef ENABLE_QUICK_OPEN
	ch->SetInventoryBuffer(true);
#endif
	for (std::vector<CItem*>::iterator iit = collectItems.begin(); iit < collectItems.end(); ++iit)
	{
		LPITEM sortedItem = *iit;
		if (sortedItem)
		{
			DWORD dwCount = sortedItem->GetCount();

			if (sortedItem->IsStackable() && !IS_SET(sortedItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
			{
				for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
				{
					LPITEM item2 = ch->GetInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

						DWORD dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}
			}

			if (dwCount > 0)
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				int cell = ch->GetEmptyInventory(sortedItem);
#else
				int cell = ch->GetEmptyInventory(sortedItem->GetSize());
#endif
				sortedItem->AddToCharacter(ch, TItemPos(INVENTORY, cell));
			}
		}
	}
#ifdef ENABLE_QUICK_OPEN
	ch->SendBufferedInventoryPacket();
#endif
}

#ifdef ENABLE_SPECIAL_INVENTORY
ACMD(do_sort_special_inventory)
{
	if (!ch)
		return;

	if (!ch->CanHandleItem())
	{
		return;
	}

	if (quest::CQuestManager::instance().GetEventFlag("disable_sort_feature") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Feature disabled is for now."));
		return;
	}

	if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef __PREMIUM_PRIVATE_SHOP__
			|| ch->IsEditingPrivateShop() || ch->IsShopSearch() || ch->GetViewingPrivateShop() 
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			|| ch->IsAcceOpened(true) || ch->IsAcceOpened(false)
#endif
#if defined(__BL_67_ATTR__)
			|| ch->Is67AttrOpen()
#endif
	)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sort inventory while another window is open."));
		return;
	}

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been cancelled."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	if (!ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Please wait 10 seconds after opening any window!");
		return;
	}

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::InventorySort, std::chrono::milliseconds(1000)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait 15 seconds."));
		return;
	}
#endif

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	BYTE specialInventoryType = atoi(arg1);

	std::vector<CItem*> collectItems;

	int startIndex = SPECIAL_INVENTORY_SLOT_START + (SPECIAL_INVENTORY_PAGE_SIZE * INVENTORY_PAGE_COUNT) * specialInventoryType;
	int endIndex = SPECIAL_INVENTORY_SLOT_START + (SPECIAL_INVENTORY_PAGE_SIZE * INVENTORY_PAGE_COUNT) * (specialInventoryType + 1);

	for (int i = startIndex; i < endIndex; ++i)
	{
		if (!ch)
			continue;		
		LPITEM item = ch->GetInventoryItem(i);

		if (item)
		{
			collectItems.push_back(item);
		}
	}

	for (std::vector<CItem*>::iterator it = collectItems.begin() ; it != collectItems.end(); ++it)
	{
		if (!ch)
			continue;			
		LPITEM item = *it;
		item->RemoveFromCharacter();
	}

	std::sort(collectItems.begin(), collectItems.end(), sortByType);
	std::sort(collectItems.begin(), collectItems.end(), sortBySubType);
	std::sort(collectItems.begin(), collectItems.end(), sortByVnum);
	std::sort(collectItems.begin(), collectItems.end(), sortBySocket);

#ifdef ENABLE_QUICK_OPEN
	ch->SetInventoryBuffer(true);
#endif
	for (std::vector<CItem*>::iterator iit = collectItems.begin(); iit < collectItems.end(); ++iit)
	{
		if (!ch)
			continue;		
		LPITEM sortedItem = *iit;
		if (sortedItem)
		{
			DWORD dwCount = sortedItem->GetCount();

			if (sortedItem->IsStackable() && !IS_SET(sortedItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
			{
				for (int i = startIndex; i < endIndex; ++i)
				{
					LPITEM item2 = ch->GetInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

						DWORD dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}
			}

			if (dwCount > 0)
			{
				int cell = ch->GetEmptyInventory(sortedItem);
				sortedItem->AddToCharacter(ch, TItemPos(INVENTORY, cell));
			}
		}
	}
#ifdef ENABLE_QUICK_OPEN
	ch->SendBufferedInventoryPacket();
#endif
}
#endif
#endif

#ifdef ENABLE_DAILY_BOSS
EVENTINFO(RequestTimerEventInfo)
{
	DynamicCharacterPtr ch;
	int x;
	int y;
	int left_second;

	RequestTimerEventInfo()
		: ch()
		, x(0)
		, y(0)
		, left_second(0)
	{
	}
};

EVENTFUNC(timer_teleport_event)
{
	RequestTimerEventInfo * info = dynamic_cast<RequestTimerEventInfo *> (event->info);

	if (info == NULL)
	{
		sys_err("timed_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL)
	{
		return 0;
	}

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		if (ch->CanWarp())
		{
			ch->WarpSet(info->x, info->y, 0);
		}

		return 0;
	}
	else
	{
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_timer_warp)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::RequestTimerWarp, std::chrono::milliseconds(3000)))
		return ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d s! Try again later."), 3);
#endif

	int iIndex = atoi(arg1);

	if ((iIndex >= 0 && iIndex <= 1))
	{
		std::map<int, std::map<std::string, long>> Cords;

		Cords[0]["x"] = 2149000;
		Cords[0]["y"] = 1061900;

		Cords[1]["x"] = 2149000;
		Cords[1]["y"] = 1061900;

		RequestTimerEventInfo* info = AllocEventInfo<RequestTimerEventInfo>();
		{
			info->left_second = 0;
		}

		info->ch = ch;
		info->x = Cords[iIndex]["x"];
		info->y = Cords[iIndex]["y"];

		ch->m_pkTimedEvent = event_create(timer_teleport_event, info, 1);

	}
}
#endif

#ifdef __GUILD_SAFEBOX__
ACMD(do_guild_safebox_close)
{
	if (ch->GetGuild())
	{
		ch->GetGuild()->GetSafeBox().CloseSafebox(ch);
	}
}
#endif

#ifdef ENABLE_EVENT_CALENDAR
ACMD(do_event_manager)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);

	if (vecArgs.size() < 2)
	{
		return;
	}
	else if (vecArgs[1] == "info")
	{
		CHARACTER_MANAGER::Instance().SendDataPlayer(ch);
	}
	else if (vecArgs[1] == "remove")
	{
		if (!ch->IsGM())
			return;

		if (vecArgs.size() < 3)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Put the event index!!");
			return;
		}

		uint8_t removeIndex;
		str_to_number(removeIndex, vecArgs[2].c_str());

		if (CHARACTER_MANAGER::Instance().CloseEventManuel(removeIndex))
			ch->ChatPacket(CHAT_TYPE_INFO, "Successfuly removed!");
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "Dont have any event!");
	}
	else if (vecArgs[1] == "update")
	{
		if (!ch->IsGM())
			return;

		const uint8_t subHeader = EVENT_MANAGER_UPDATE;
		//db_clientdesc->DBPacketHeader(HEADER_GD_EVENT_MANAGER, 0, sizeof(uint8_t));
		//db_clientdesc->Packet(&subHeader, sizeof(uint8_t));
		db_clientdesc->DBPacket(HEADER_GD_EVENT_MANAGER, 0, &subHeader, sizeof(uint8_t));

		ch->ChatPacket(CHAT_TYPE_INFO, "Successfully updated!");
	}
}
#endif

ACMD(do_skip_battlepass_mission)
{
	// split args
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 1)
	{
		return;
	}

	BYTE bMissionID;
	str_to_number(bMissionID, vecArgs[1].c_str());

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::BattlePassSkipMission, std::chrono::milliseconds(3000)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait 3 seconds."));
		return;
	}
#endif

	CAffect* affect = ch->FindAffect(AFFECT_BATTLE_PASS);
	if (!affect)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to buy Battle Pass from ItemShop to get reward for missions."));
		return;
	}

	ch->SkipBattlePassMission(bMissionID);
}

#ifdef ENABLE_REWARD_SYSTEM
ACMD(do_update_reward_data)
{
	CHARACTER_MANAGER::Instance().SendRewardInfo(ch);
}
#endif

#ifdef ENABLE_GUILD_MEETING
ACMD(do_guild_meeting)
{
	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드에 속해있지 않습니다."));
		return;
	}

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::RequestGuildMeeting, std::chrono::milliseconds(10000)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait 10 seconds."));
		return;
	}
#endif

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember (ch->GetPlayerID());
	if (gm->grade != GUILD_LEADER_GRADE)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You are not the leader of the guild.");
		return;
	}

	if (ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot hold guild meetings on this map."));
		return;
	}

	switch (ch->GetMapIndex())
	{
		case 72:
		case 73:
		case 352:
		case 351:
		case 216:
		case 79:
		case 71:
		case 211:
		case 113:
		case 66:
		case 28:
		case 104:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot hold guild meetings on this map."));
			return;
	}

	quest::CQuestManager::instance().RequestSetEventFlagEx(g->GetName(), "guild_meeting_x", ch->GetX());
	quest::CQuestManager::instance().RequestSetEventFlagEx(g->GetName(), "guild_meeting_y", ch->GetY());
	quest::CQuestManager::instance().RequestSetEventFlagEx(g->GetName(), "guild_meeting_mapindex", ch->GetMapIndex());

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Meeting requests sent to guild members."));
	g->RequestGuildMeeting(ch);
}

ACMD(do_guild_meeting_yes)
{
	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드에 속해있지 않습니다."));
		return;
	}

	char buf[256];
	char buf2[256];
	char buf3[256];

	snprintf (buf, sizeof (buf), "%sguild_meeting_x", ch->GetGuild()->GetName());
	snprintf (buf2, sizeof (buf2), "%sguild_meeting_y", ch->GetGuild()->GetName());
	snprintf (buf3, sizeof (buf3), "%sguild_meeting_mapindex", ch->GetGuild()->GetName());

	int x = quest::CQuestManager::instance().GetEventFlag(buf);
	int y = quest::CQuestManager::instance().GetEventFlag(buf2);
	int mapindex = quest::CQuestManager::instance().GetEventFlag(buf3);

	ch->WarpSet(x, y, mapindex);
}
#endif

#ifdef ENABLE_TELEPORT_GUILD
ACMD(do_teleport_guild)
{
	if (!ch->GetGuild())
		return;
	
	auto pGuild = ch->GetGuild();
	if (!pGuild || ch->GetDungeon() || ch->GetMapIndex() >= 10000)
		return;
	
	if (pGuild->GetMasterPID() != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "you are not leader");
		return;
	}
	
	if (ch->GetQuestFlag("warp.guild") > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You have to wait 30s before to do this!");
		return;
	}
	
	ch->SetQuestFlag("warp.guild", get_global_time() + 30);
	
	pGuild->SendTeleportGuild(ch->GetPlayerID(), ch->GetX(), ch->GetY(), ch->GetMapIndex(), mother_port);


	TPacketGGGuildTeleport p;
	p.bHeader = HEADER_GG_GUILD_TELEPORT;
	p.dwGuild = pGuild->GetID();
	p.dwLeaderPID = pGuild->GetMasterPID();
	p.lX = ch->GetX();
	p.lY = ch->GetY();
	p.lMapIndex = ch->GetMapIndex();
	p.wPort = mother_port;
	
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGGuildTeleport));
}

ACMD(do_teleport_guild_answer)
{
	if (!ch->GetGuild())
		return;

	auto pGuild = ch->GetGuild();
	if (!pGuild || ch->GetDungeon() || ch->GetMapIndex() >= 10000)
		return;
	
	auto lX = ch->GetQuestFlag("guild_warp_x");
	auto lY = ch->GetQuestFlag("guild_warp_y");

	auto lMapIndex = ch->GetQuestFlag("guild_warp_map");
	auto wPort = ch->GetQuestFlag("guild_warp_port");
	
	if (lMapIndex == 0)
		return;
	
	if (!ch->CanWarp())
		return;
	
	ch->WarpSetNew(lX, lY, lMapIndex, wPort);
}
#endif


ACMD(do_block)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	if (!*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 잘못된 암호를 입력하셨습니다."));
		return;
	}

	int iDurationBlock = 0;
	str_to_number(iDurationBlock, arg2);
	
	if (iDurationBlock > 0)
		quest::CQuestManager::instance().RequestSetEventFlag((std::string("block_") + arg1).c_str(), get_global_time() + (iDurationBlock * 60));
	else
		quest::CQuestManager::instance().RequestSetEventFlag((std::string("block_") + arg1).c_str(), 0);

	ch->ChatPacket(CHAT_TYPE_INFO, "Player %s is blocked for %d minutes", arg1, iDurationBlock);
}
#ifdef ENABLE_TELEPORT_PM
ACMD(do_answer_warp_at_character)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;
	
	bool answer = 0;
	str_to_number(answer, arg1);
	
	if (ch->GetDungeon())
		return;
	
	if (!ch->IsCanRespondRequestWarp(arg2))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You can't answer to this");
		return;
	}

	ch->RemoveCanRespondRequestWarp(arg2);
	
	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);
	if (tch)
	{
		if (answer == false)
		{
			tch->ChatPacket(CHAT_TYPE_INFO, "Player %s denied warp.", ch->GetName());
			return;
		}
		
		// If is alreading teleporting request, stop..
		if (tch->GetQuestFlag("warp.time_request") >= get_global_time())
			return;
		tch->SetQuestFlag("warp.time_request", get_global_time() + 10);

		if (quest::CQuestManager::instance().GetPCForce(tch->GetPlayerID())->IsRunning())
			return;
		
		tch->WarpToPID(ch->GetPlayerID());
	}
	else
	{
		CCI* pkCCI = P2P_MANAGER::Instance().Find(arg2);
		if (pkCCI)
		{
			if (pkCCI->dwPID == ch->GetPlayerID())
				return;

			if (answer == false)
			{
				LPDESC pkDesc = pkCCI->pkDesc;
				pkDesc->SetRelay(arg2);
				pkDesc->ChatPacket(CHAT_TYPE_INFO, "Player %s denied warp.", ch->GetName());
				pkDesc->SetRelay("");
				return;
			}

			TPacketGGRequestWarp pgg;

			pgg.header = HEADER_GG_REQUEST_WARP;
			pgg.bRequestAccepted = true;
			pgg.dwTargetPID = ch->GetPlayerID();
			
			strlcpy(pgg.szName, arg2, sizeof(pgg.szName));

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGRequestWarp));
		}
	}
}
#endif
#ifdef ENABLE_TELEPORT_PM
ACMD(do_request_warp_at_character)
{
	// Send Request dialog to player..
	if (ch->GetDungeon())
		return;
	
	// timer timer..
	if (ch->GetQuestFlag("warp_player.timer") > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You have to wait 30s before to do this!");
		return;
	}
	
	ch->SetQuestFlag("warp_player.timer", get_global_time() + 5);
	// end of timer..

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1); // want to teleport to him..
	if (tch)
	{
		if (tch->GetPlayerID() == ch->GetPlayerID())
			return;

		tch->SetCanRespondRequestWarp(ch->GetName()); // Set Waiting respond from transfer-char
		tch->ChatPacket(CHAT_TYPE_COMMAND, "requestWarpCharacter %s", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_INFO, "Warp request sended to %s", arg1);
	}
	else
	{
		CCI* pkCCI = P2P_MANAGER::Instance().Find(arg1);
		if (pkCCI)
		{
			if (pkCCI->dwPID == ch->GetPlayerID())
				return;

			ch->ChatPacket(CHAT_TYPE_INFO, "Warp request sended to %s", arg1);

			TPacketGGRequestWarp pgg;
			pgg.header = HEADER_GG_REQUEST_WARP;
			pgg.bRequestAccepted = false;
			pgg.dwTargetPID = 0;

			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			strlcpy(pgg.szNameRequest, ch->GetName(), sizeof(pgg.szNameRequest));

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGRequestWarp));
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "Player %s not online.", arg1);
	}
}
#endif

#ifdef FAST_EQUIP_WORLDARD
ACMD(do_open_change_equip)
{

	char arg1[256];
	one_argument (argument, arg1, sizeof(arg1));
	
	if (0 == arg1[0])
		return;

	int page_index = atoi(arg1);

	if(page_index <= 0 || page_index > CHANGE_EQUIP_PAGE_EXTRA){
		return;
	}

	if(!ch->CanHandleItem()){
		return;
	}

	if (ch->IsDead()){
		return;
	}

	if (ch->IsStun()){
		return;
	}

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
		return;
	}

    int get_time_change_equip = ch->GetTimeChangeEquip();
    int currentPulse = thecore_pulse();
   
    if (get_time_change_equip > currentPulse) {
        int deltaInSeconds = ((get_time_change_equip / PASSES_PER_SEC(1)) - (currentPulse / PASSES_PER_SEC(1)));
        int minutes = deltaInSeconds / 60;
        int seconds = (deltaInSeconds - (minutes * 60));
		
		ch->ChatPacket(CHAT_TYPE_INFO, "you have to wait %02d seconds to change equip .", seconds);
        return;
    }

	DWORD dwCurTime = get_dword_time();

	if (dwCurTime - ch->GetLastAttackTime() <= 1500 || dwCurTime - ch->GetLastSkillTime() <= 1500)
	{
		return;
	}

	DWORD index_old = CHANGE_EQUIP_SLOT_COUNT-(CHANGE_EQUIP_SLOT_COUNT/page_index);

	if(page_index > 1){
		index_old = CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA*(page_index-1);
	}

	// Fix Weapon Costume

	LPITEM item_change_equip;
	LPITEM item_inv;
	LPITEM item_extra;

	item_inv = ch->GetWear(WEAR_COSTUME_WEAPON);
	if(item_inv){
		item_change_equip = ch->GetChangeEquipItem(index_old+WEAR_WEAPON);
		if(item_change_equip)
		{
			if (item_change_equip->GetType() != ITEM_WEAPON || item_inv->GetValue(3) != item_change_equip->GetSubType())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Dieser Ausrustungswechsel kann nicht durchgefuhrt werden, da deine aktuelle Waffen-Skin nicht kompatibel ist.");
				return;
			}
		}
	}


	item_inv = ch->GetWear(WEAR_WEAPON);
	if(item_inv){
		item_change_equip = ch->GetChangeEquipItem(index_old+WEAR_COSTUME_WEAPON);
		item_extra = ch->GetChangeEquipItem(index_old+WEAR_WEAPON);

		if(item_change_equip && !item_extra)
		{
			if (item_change_equip->GetSubType() != COSTUME_WEAPON || item_change_equip->GetValue(3) != item_inv->GetSubType())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Dieser Ausrustungswechsel kann nicht durchgefuhrt werden, da deine aktuelle Waffen-Skin nicht kompatibel ist.");
				return;
			}
		}
	}else{
		item_change_equip = ch->GetChangeEquipItem(index_old+WEAR_COSTUME_WEAPON);
		if(item_change_equip){
			ch->ChatPacket(CHAT_TYPE_INFO, "Dieser Ausrustungswechsel kann nicht durchgefuhrt werden, da deine aktuelle Waffen-Skin nicht kompatibel ist.");
			return;	
		}
	}

	// Fix Weapon Costume

	for (int i = index_old; i < CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA*page_index; ++i)
	{
		item_change_equip = ch->GetChangeEquipItem(i);

		int cell = i;

		if(page_index > 1){
			if(cell >= CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA*(page_index-1) && cell < (CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA)*(page_index))
			{
				cell = cell - ((CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA)*(page_index-1));
			}
		}

		item_inv = ch->GetWear(cell);

		if(item_change_equip && item_inv == NULL)
		{
			item_change_equip->EquipTo(ch, item_change_equip->FindEquipCell(ch));
		}

		if(item_change_equip && item_inv)
		{
			item_inv->RemoveFromCharacter();
			if(item_change_equip->EquipTo(ch, item_change_equip->FindEquipCell(ch)))
				item_inv->AddToCharacter(ch, TItemPos(CHANGE_EQUIP, i));
			
		}
	}

	ch->SetTimeChangeEquip(thecore_pulse() + PASSES_PER_SEC(7));

}
#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
