#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "protocol.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "cmd.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "battle.h"
#include "exchange.h"
#include "questmanager.h"
#include "profiler.h"
#include "messenger_manager.h"
#include "party.h"
#include "p2p.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "log.h"
#include "banword.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "building.h"
#include "locale_service.h"
#include "gm.h"
#include "spam.h"
#include "ani.h"
#include "motion.h"
#include "OXEvent.h"
#include "locale_service.h"
#include "DragonSoul.h"
#include "belt_inventory_helper.h" // @fixme119
#include "../../common/CommonDefines.h"
#ifdef ENABLE_NEWSTUFF
	#include "../../common/PulseManager.h"
#endif

#include "input.h"
#include "GuildStorageManager.h"
#ifdef ENABLE_SWITCHBOT_SYSTEM
	#include "new_switchbot.h"
#endif
#ifdef ENABLE_EVENT_MANAGER
	#include "event_manager.h"
#endif
#ifdef ENABLE_INGAME_WIKI
	#include "mob_manager.h"
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	#include "BiologSystemManager.hpp"
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	#include "private_shop_manager.h"
	#include "private_shop.h"
	#include "private_shop_util.h"
#endif
#ifdef ENABLE_DUNGEON_INFO_SYSTEM
	#include "DungeonInfo.h"
#endif
#ifdef ENABLE_ANTI_MULTIPLE_FARM
	#include "HAntiMultipleFarm.h"
#endif
#ifdef ENABLE_BATTLE_PASS
	#include "battle_pass.h"
#endif
#ifdef ENABLE_ITEMSHOP
	#include "itemshop.h"
#endif
#ifdef __GUILD_SAFEBOX__
	#include "guild_safebox.h"
#endif

#define ENABLE_CHAT_COLOR_SYSTEM
#define ENABLE_CHAT_LOGGING
#define ENABLE_CHAT_SPAMLIMIT
#define ENABLE_WHISPER_CHAT_SPAMLIMIT
#define ENABLE_CHECK_GHOSTMODE

#ifdef ENABLE_CHAT_LOGGING
static char	__escape_string[1024];
static char	__escape_string2[1024];
#endif

#ifdef ENABLE_GOLD_MAX_EXTENDED
	static unsigned long long __deposit_limit()
#else
	static int __deposit_limit()
#endif
{
	return (1000*10000);
}

void SendBlockChatInfo(LPCHARACTER ch, int sec)
{
	if (sec <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("채팅 금지 상태입니다."));
		return;
	}

	long hour = sec / 3600;
	sec -= hour * 3600;

	long min = (sec / 60);
	sec -= min * 60;

	char buf[128+1];

	if (hour > 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d 시간 %d 분 %d 초 동안 채팅금지 상태입니다"), hour, min, sec);
	else if (hour > 0 && min == 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d 시간 %d 초 동안 채팅금지 상태입니다"), hour, sec);
	else if (hour == 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d 분 %d 초 동안 채팅금지 상태입니다"), min, sec);
	else
		snprintf(buf, sizeof(buf), LC_TEXT("%d 초 동안 채팅금지 상태입니다"), sec);

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

EVENTINFO(spam_event_info)
{
	char host[MAX_HOST_LENGTH+1];

	spam_event_info()
	{
		::memset( host, 0, MAX_HOST_LENGTH+1 );
	}
};

typedef boost::unordered_map<std::string, std::pair<unsigned int, LPEVENT> > spam_score_of_ip_t;
spam_score_of_ip_t spam_score_of_ip;

EVENTFUNC(block_chat_by_ip_event)
{
	spam_event_info* info = dynamic_cast<spam_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "block_chat_by_ip_event> <Factor> Null pointer" );
		return 0;
	}

	const char * host = info->host;

	spam_score_of_ip_t::iterator it = spam_score_of_ip.find(host);

	if (it != spam_score_of_ip.end())
	{
		it->second.first = 0;
		it->second.second = NULL;
	}

	return 0;
}

bool SpamBlockCheck(LPCHARACTER ch, const char* const buf, const size_t buflen)
{
	if (ch->GetLevel() < g_iSpamBlockMaxLevel)
	{
		spam_score_of_ip_t::iterator it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());

		if (it == spam_score_of_ip.end())
		{
			spam_score_of_ip.emplace(ch->GetDesc()->GetHostName(), std::make_pair(0, (LPEVENT) NULL));
			it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());
		}

		if (it->second.second)
		{
			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);
			return true;
		}

		unsigned int score;
		const char * word = SpamManager::instance().GetSpamScore(buf, buflen, score);

		it->second.first += score;

		if (word)
			sys_log(0, "SPAM_SCORE: %s text: %s score: %u total: %u word: %s", ch->GetName(), buf, score, it->second.first, word);

		if (it->second.first >= g_uiSpamBlockScore)
		{
			spam_event_info* info = AllocEventInfo<spam_event_info>();
			strlcpy(info->host, ch->GetDesc()->GetHostName(), sizeof(info->host));

			it->second.second = event_create(block_chat_by_ip_event, info, PASSES_PER_SEC(g_uiSpamBlockDuration));
			sys_log(0, "SPAM_IP: %s for %u seconds", info->host, g_uiSpamBlockDuration);

			LogManager::instance().CharLog(ch, 0, "SPAM", word);

			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);

			return true;
		}
	}

	return false;
}

enum
{
	TEXT_TAG_PLAIN,
	TEXT_TAG_TAG, // ||
	TEXT_TAG_COLOR, // |cffffffff
	TEXT_TAG_HYPERLINK_START, // |H
	TEXT_TAG_HYPERLINK_END, // |h ex) |Hitem:1234:1:1:1|h
	TEXT_TAG_RESTORE_COLOR,
};

int GetTextTag(const char * src, int maxLen, int & tagLen, std::string & extraInfo)
{
	tagLen = 1;

	if (maxLen < 2 || *src != '|')
		return TEXT_TAG_PLAIN;

	const char * cur = ++src;

	if (*cur == '|')
	{
		tagLen = 2;
		return TEXT_TAG_TAG;
	}
	else if (*cur == 'c') // color |cffffffffblahblah|r
	{
		tagLen = 2;
		return TEXT_TAG_COLOR;
	}
	else if (*cur == 'H')
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_START;
	}
	else if (*cur == 'h') // end of hyperlink
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_END;
	}

	return TEXT_TAG_PLAIN;
}

#if defined(__BL_CLIENT_LOCALE_STRING__)
static int htoi(const char* s)
{
	const char* t = s;
	int x = 0, y = 1;
	s += strlen(s);

	while (t <= --s)
	{
		if ('0' <= *s && *s <= '9')
			x += y * (*s - '0');
		else if ('a' <= *s && *s <= 'f')
			x += y * (*s - 'a' + 10);
		else if ('A' <= *s && *s <= 'F')
			x += y * (10 + *s - 'A');
		else
			return -1; /* invalid input! */
		y <<= 4;
	}

	return x;
}

static std::string __FormatHyperLinkItemString(std::string input)
{
	size_t startPos = input.find("|Hitem:");
	while (startPos != std::string::npos)
	{
		const size_t endPos = input.find(":", startPos + 7);
		if (endPos == std::string::npos)
			break;

		const size_t contentStartPos = input.find("|h[", endPos);
		const size_t contentEndPos = input.find("]|h|", contentStartPos);
		if (contentStartPos == std::string::npos || contentEndPos == std::string::npos)
			break;

		std::string itemVnum = input.substr(startPos + 7, endPos - (startPos + 7));
		itemVnum = "[IN;" + std::to_string(htoi(itemVnum.c_str())) + "]";

		input.replace(contentStartPos + 3, contentEndPos - (contentStartPos + 3), itemVnum);
		startPos = input.find("|Hitem:", contentStartPos + itemVnum.length());
	}

	return input;
}
#endif

void GetTextTagInfo(const char * src, int src_len, int & hyperlinks, bool & colored)
{
	colored = false;
	hyperlinks = 0;

	int len;
	std::string extraInfo;

	for (int i = 0; i < src_len;)
	{
		int tag = GetTextTag(&src[i], src_len - i, len, extraInfo);

		if (tag == TEXT_TAG_HYPERLINK_START)
			++hyperlinks;

		if (tag == TEXT_TAG_COLOR)
			colored = true;

		i += len;
	}
}

int ProcessTextTag(LPCHARACTER ch, const char * c_pszText, size_t len)
{
	int hyperlinks;
	bool colored;

	GetTextTagInfo(c_pszText, len, hyperlinks, colored);

	if (colored == true && hyperlinks == 0)
		return 4;

#ifdef ENABLE_NEWSTUFF
	if (g_bDisablePrismNeed)
		return 0;
#endif
	int nPrismCount = ch->CountSpecifyItem(ITEM_PRISM);

	if (nPrismCount < hyperlinks)
		return 1;

	if (!ch->GetMyShop())
	{
		ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
		return 0;
	} else
	{
		int sellingNumber = ch->GetMyShop()->GetNumberByVnum(ITEM_PRISM);
		if(nPrismCount - sellingNumber < hyperlinks)
		{
			return 2;
		} else
		{
			ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
			return 0;
		}
	}

	return 4;
}

int CInputMain::Whisper(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGWhisper* pinfo = reinterpret_cast<const TPacketCGWhisper*>(data);

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGWhisper);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

#ifdef ENABLE_WHISPER_CHAT_SPAMLIMIT
	if (ch->IncreaseChatCounter() >= 10)
	{
		ch->GetDesc()->DelayedDisconnect(0);
		return (iExtraLen);
	}
#endif

	if (ch->FindAffect(AFFECT_BLOCK_CHAT))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("채팅 금지 상태입니다."));
		return (iExtraLen);
	}

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(pinfo->szNameTo);

	if (pkChr == ch)
		return (iExtraLen);

	LPDESC pkDesc = NULL;

	BYTE bOpponentEmpire = 0;

	if (test_server)
	{
		if (!pkChr)
			sys_log(0, "Whisper to %s(%s) from %s", "Null", pinfo->szNameTo, ch->GetName());
		else
			sys_log(0, "Whisper to %s(%s) from %s", pkChr->GetName(), pinfo->szNameTo, ch->GetName());
	}

	if (ch->IsBlockMode(BLOCK_WHISPER))
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		return iExtraLen;
	}

	if (!pkChr)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			pkDesc->SetRelay(pinfo->szNameTo);
			bOpponentEmpire = pkCCI->bEmpire;

			if (test_server)
				sys_log(0, "Whisper to %s from %s (Channel %d Mapindex %d)", "Null", ch->GetName(), pkCCI->bChannel, pkCCI->lMapIndex);
		}
	}
	else
	{
		pkDesc = pkChr->GetDesc();
		bOpponentEmpire = pkChr->GetEmpire();
	}

	if (!pkDesc)
	{
		if (ch->GetDesc())
		{
#if defined(ENABLE_OFFLINE_MESSAGE)
			const BYTE bDelay = 10;
			char msg[64];
			if (get_dword_time() - ch->GetLastOfflinePMTime() > bDelay * 1000)
			{
				char buf[CHAT_MAX_LEN + 1];
				strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
#if defined(__BL_CLIENT_LOCALE_STRING__)
				int hyperlinks;
				bool colored;
				GetTextTagInfo(buf, strlen(buf), hyperlinks, colored);
				if (hyperlinks)
				{
					const std::string formatStr = __FormatHyperLinkItemString(buf);
					strlcpy(buf, formatStr.c_str(), sizeof(buf));
				}
#endif
				const size_t buflen = strlen(buf);

				CBanwordManager::instance().ConvertString(buf, buflen);

				int processReturn = ProcessTextTag(ch, buf, buflen);
				if (0 != processReturn)
				{
					TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable) {
						char buffer[128];
						int len;
						if (3==processReturn)
							len = snprintf(buf, sizeof(buf), LC_TEXT("사용할수 없습니다."), pTable->szLocaleName);
						else
							len = snprintf(buf, sizeof(buf), LC_TEXT("%s이 필요합니다."), pTable->szLocaleName);

						if (len < 0 || len >= (int)sizeof(buffer))
							len = sizeof(buffer) - 1;

						++len;

						TPacketGCWhisper pack;

						pack.bHeader = HEADER_GC_WHISPER;
						pack.bType = WHISPER_TYPE_ERROR;
						pack.wSize = static_cast<WORD>(sizeof(TPacketGCWhisper) + len);
						strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

						ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
						ch->GetDesc()->Packet(buffer, len);

						sys_log(0, "WHISPER: not enough %s: char: %s", pTable->szLocaleName, ch->GetName());
					}
					return (iExtraLen);
				}

				if (buflen > 0) 
				{
					ch->SendOfflineMessage(pinfo->szNameTo, buf);
					snprintf(msg, sizeof(msg), "An offline message has been sent.");
				}
				else
					return (iExtraLen);
			}
			else
			{
				snprintf(msg, sizeof(msg), "You have to wait %d seconds for send offline message.", bDelay);
			}

			/*Info*/
			TPacketGCWhisper pack;
			int len = MIN(CHAT_MAX_LEN, strlen(msg) + 1);
			pack.bHeader = HEADER_GC_WHISPER;
			pack.wSize = static_cast<WORD>(sizeof(TPacketGCWhisper) + len);
			pack.bType = WHISPER_TYPE_SYSTEM;
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

			TEMP_BUFFER buf;
			buf.write(&pack, sizeof(TPacketGCWhisper));
			buf.write(msg, len);
			ch->GetDesc()->Packet(buf.read_peek(), buf.size());

#else
			TPacketGCWhisper pack;

			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_NOT_EXIST;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWhisper));
#endif
			sys_log(0, "WHISPER: no player");
		}
	}
	else
	{
		if (ch->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else if (pkChr && pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_TARGET_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else
		{
			BYTE bType = WHISPER_TYPE_NORMAL;

			char buf[CHAT_MAX_LEN + 1];
			strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
#if defined(__BL_CLIENT_LOCALE_STRING__)
			int hyperlinks;
			bool colored;
			GetTextTagInfo(buf, strlen(buf), hyperlinks, colored);
			if (hyperlinks)
			{
				const std::string formatStr = __FormatHyperLinkItemString(buf);
				strlcpy(buf, formatStr.c_str(), sizeof(buf));
			}
#endif
			const size_t buflen = strlen(buf);

			if (true == SpamBlockCheck(ch, buf, buflen))
			{
				if (!pkChr)
				{
					CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

					if (pkCCI)
					{
						pkDesc->SetRelay("");
					}
				}
				return iExtraLen;
			}

			CBanwordManager::instance().ConvertString(buf, buflen);

			if (g_bEmpireWhisper)
				if (!ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
					if (!(pkChr && pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)))
						if (bOpponentEmpire != ch->GetEmpire() && ch->GetEmpire() && bOpponentEmpire
								&& ch->GetGMLevel() == GM_PLAYER && gm_get_level(pinfo->szNameTo) == GM_PLAYER)

						{
							if (!pkChr)
							{
								bType = ch->GetEmpire() << 4;
							}
							else
							{
								ConvertEmpireText(ch->GetEmpire(), buf, buflen, 10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + ch->GetEmpire() - 1));
							}
						}

			int processReturn = ProcessTextTag(ch, buf, buflen);
			if (0!=processReturn)
			{
				if (ch->GetDesc())
				{
					TItemTable * pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable)
					{
						char buf[128];
						int len;
						if (3==processReturn)
							len = snprintf(buf, sizeof(buf), LC_TEXT("사용할수 없습니다."), pTable->szLocaleName);
						else
							len = snprintf(buf, sizeof(buf), LC_TEXT("%s이 필요합니다."), pTable->szLocaleName);

						if (len < 0 || len >= (int) sizeof(buf))
							len = sizeof(buf) - 1;

						++len;

						TPacketGCWhisper pack;

						pack.bHeader = HEADER_GC_WHISPER;
						pack.bType = WHISPER_TYPE_ERROR;
						pack.wSize = sizeof(TPacketGCWhisper) + len;
						strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

						ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
						ch->GetDesc()->Packet(buf, len);

						sys_log(0, "WHISPER: not enough %s: char: %s", pTable->szLocaleName, ch->GetName());
					}
				}

				pkDesc->SetRelay("");
				return (iExtraLen);
			}

			if (ch->IsGM())
				bType = (bType & 0xF0) | WHISPER_TYPE_GM;

			if (buflen > 0)
			{
				TPacketGCWhisper pack;

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + buflen;
				pack.bType = bType;
				strlcpy(pack.szNameFrom, ch->GetName(), sizeof(pack.szNameFrom));
#if defined(__BL_CLIENT_LOCALE_STRING__)
				pack.bCanFormat = (hyperlinks > 0);
#endif
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
				strlcpy(pack.szCountry, ch->GetDesc()->GetCountryName().c_str(), sizeof(pack.szCountry));
				pack.bEmpire = ch->GetEmpire();
#endif
				TEMP_BUFFER tmpbuf;

				tmpbuf.write(&pack, sizeof(pack));
				tmpbuf.write(buf, buflen);

				pkDesc->Packet(tmpbuf.read_peek(), tmpbuf.size());

				// @warme006
				// sys_log(0, "WHISPER: %s -> %s : %s", ch->GetName(), pinfo->szNameTo, buf);
#ifdef ENABLE_CHAT_LOGGING
				if (ch->IsGM())
				{
					LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), buf, buflen);
					LogManager::instance().EscapeString(__escape_string2, sizeof(__escape_string2), pinfo->szNameTo, sizeof(pack.szNameFrom));
					LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, __escape_string2, "WHISPER", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
				}
#endif
			}
		}
	}
	if(pkDesc)
		pkDesc->SetRelay("");

	return (iExtraLen);
}

struct RawPacketToCharacterFunc
{
	const void * m_buf;
	int	m_buf_len;

	RawPacketToCharacterFunc(const void * buf, int buf_len) : m_buf(buf), m_buf_len(buf_len)
	{
	}

	void operator () (LPCHARACTER c)
	{
		if (!c->GetDesc())
			return;

		c->GetDesc()->Packet(m_buf, m_buf_len);
	}
};

struct FEmpireChatPacket
{
	packet_chat& p;
	const char* orig_msg;
	int orig_len;
	char converted_msg[CHAT_MAX_LEN+1];

	BYTE bEmpire;
	int iMapIndex;
	int namelen;

	FEmpireChatPacket(packet_chat& p, const char* chat_msg, int len, BYTE bEmpire, int iMapIndex, int iNameLen)
		: p(p), orig_msg(chat_msg), orig_len(len), bEmpire(bEmpire), iMapIndex(iMapIndex), namelen(iNameLen)
	{
		memset( converted_msg, 0, sizeof(converted_msg) );
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != iMapIndex)
			return;

#ifdef ENABLE_EMPIRE_ON_CHAT
		d->BufferedPacket(&p, sizeof(packet_chat));
		d->Packet(orig_msg, orig_len);
#else
		d->BufferedPacket(&p, sizeof(packet_chat));
		if (d->GetEmpire() == bEmpire ||
			bEmpire == 0 ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			d->Packet(orig_msg, orig_len);
		}
		else
		{
			// ???? ????? ??? ?? ?????
			size_t len = strlcpy(converted_msg, orig_msg, sizeof(converted_msg));

			if (len >= sizeof(converted_msg))
				len = sizeof(converted_msg) - 1;

			ConvertEmpireText(bEmpire, converted_msg + namelen, len - namelen, 10 + 2 * d->GetCharacter()->GetSkillPower(SKILL_LANGUAGE1 + bEmpire - 1));
			d->Packet(converted_msg, orig_len);
		}
#endif
	}
};

struct FYmirChatPacket
{
	packet_chat& packet;
	const char* m_szChat;
	size_t m_lenChat;
	const char* m_szName;

	int m_iMapIndex;
	BYTE m_bEmpire;
	bool m_ring;

	char m_orig_msg[CHAT_MAX_LEN+1];
	int m_len_orig_msg;
	char m_conv_msg[CHAT_MAX_LEN+1];
	int m_len_conv_msg;

	FYmirChatPacket(packet_chat& p, const char* chat, size_t len_chat, const char* name, size_t len_name, int iMapIndex, BYTE empire, bool ring)
		: packet(p),
		m_szChat(chat), m_lenChat(len_chat),
		m_szName(name),
		m_iMapIndex(iMapIndex), m_bEmpire(empire),
		m_ring(ring)
	{
		m_len_orig_msg = snprintf(m_orig_msg, sizeof(m_orig_msg), "%s : %s", m_szName, m_szChat) + 1;

		if (m_len_orig_msg < 0 || m_len_orig_msg >= (int) sizeof(m_orig_msg))
			m_len_orig_msg = sizeof(m_orig_msg) - 1;

		m_len_conv_msg = snprintf(m_conv_msg, sizeof(m_conv_msg), "??? : %s", m_szChat) + 1;

		if (m_len_conv_msg < 0 || m_len_conv_msg >= (int) sizeof(m_conv_msg))
			m_len_conv_msg = sizeof(m_conv_msg) - 1;

		ConvertEmpireText(m_bEmpire, m_conv_msg + 6, m_len_conv_msg - 6, 10);
	}

	void operator() (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != m_iMapIndex)
			return;

		if (m_ring ||
			d->GetEmpire() == m_bEmpire ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			packet.size = m_len_orig_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_orig_msg, m_len_orig_msg);
		}
		else
		{
			packet.size = m_len_conv_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_conv_msg, m_len_conv_msg);
		}
	}
};

int CInputMain::Chat(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGChat* pinfo = reinterpret_cast<const TPacketCGChat*>(data);

	if (uiBytes < pinfo->size)
		return -1;

	const int iExtraLen = pinfo->size - sizeof(TPacketCGChat);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->size, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char buf[CHAT_MAX_LEN - (CHARACTER_NAME_MAX_LEN + 3) + 1];
	strlcpy(buf, data + sizeof(TPacketCGChat), MIN(iExtraLen + 1, sizeof(buf)));
	const size_t buflen = strlen(buf);

	if (buflen > 1 && *buf == '/')
	{
		interpret_command(ch, buf + 1, buflen - 1);
		return iExtraLen;
	}
#ifdef ENABLE_CHAT_SPAMLIMIT
	if (ch->IncreaseChatCounter() >= 4)
	{
		if (ch->GetChatCounter() == 10)
			ch->GetDesc()->DelayedDisconnect(0);
		return iExtraLen;
	}
#else
	if (ch->IncreaseChatCounter() >= 10)
	{
		if (ch->GetChatCounter() == 10)
		{
			sys_log(0, "CHAT_HACK: %s", ch->GetName());
			ch->GetDesc()->DelayedDisconnect(5);
		}

		return iExtraLen;
	}
#endif

	const CAffect* pAffect = ch->FindAffect(AFFECT_BLOCK_CHAT);

	if (pAffect != NULL)
	{
		SendBlockChatInfo(ch, pAffect->lDuration);
		return iExtraLen;
	}

	if (true == SpamBlockCheck(ch, buf, buflen))
	{
		return iExtraLen;
	}

	// @fixme133 begin
	CBanwordManager::instance().ConvertString(buf, buflen);

	int processReturn = ProcessTextTag(ch, buf, buflen);
	if (0!=processReturn)
	{
		const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

		if (NULL != pTable)
		{
			if (3==processReturn)
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용할수 없습니다."), pTable->szLocaleName);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s이 필요합니다."), pTable->szLocaleName);

		}

		return iExtraLen;
	}
	// @fixme133 end

	char chatbuf[CHAT_MAX_LEN + 1];
// #ifdef ENABLE_CHAT_COLOR_SYSTEM
// 	static const char* colorbuf[] = {"|cFFffa200|H|h[Staff]|h|r", "|cFFff0000|H|h[Shinsoo]|h|r", "|cFFffc700|H|h[Chunjo]|h|r", "|cFF000bff|H|h[Jinno]|h|r"};
// 	int len = snprintf(chatbuf, sizeof(chatbuf), "%s %s : %s", (ch->IsGM()?colorbuf[0]:colorbuf[MINMAX(0, ch->GetEmpire(), 3)]), ch->GetName(), buf);
// #else
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s : %s", ch->GetName(), buf);
// #endif
#if defined(__BL_CLIENT_LOCALE_STRING__)
	int hyperlinks;
	bool colored;
	GetTextTagInfo(chatbuf, len, hyperlinks, colored);
	if (hyperlinks)
	{
		const std::string formatStr = __FormatHyperLinkItemString(chatbuf);
		strlcpy(chatbuf, formatStr.c_str(), sizeof(chatbuf));
		len = strlen(chatbuf);
	}
#endif

	if (CHAT_TYPE_SHOUT == pinfo->type)
	{
		LogManager::instance().ShoutLog(g_bChannel, ch->GetEmpire(), chatbuf);
	}

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	if (pinfo->type == CHAT_TYPE_SHOUT)
	{
		if (ch->GetLevel() < g_iShoutLimitLevel)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("외치기는 레벨 %d 이상만 사용 가능 합니다."), g_iShoutLimitLevel);
			return (iExtraLen);
		}

		if (thecore_heart->pulse - (int) ch->GetLastShoutPulse() < passes_per_sec * 15)
			return (iExtraLen);

		ch->SetLastShoutPulse(thecore_heart->pulse);

		TPacketGGShout p;

		p.bHeader = HEADER_GG_SHOUT;
		p.bEmpire = ch->GetEmpire();
		strlcpy(p.szText, chatbuf, sizeof(p.szText));
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
		strlcpy(p.szCountry, ch->GetDesc()->GetCountryName().c_str(), sizeof(p.szCountry));
		p.hyperlinks = hyperlinks;
#endif

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShout));

		SendShout(chatbuf, ch->GetEmpire()
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
			, p.szCountry, p.hyperlinks
#endif
		);

		return (iExtraLen);
	}

	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = pinfo->type;
	pack_chat.id = ch->GetVID();
#if defined(__BL_CLIENT_LOCALE_STRING__)
	pack_chat.bCanFormat = (hyperlinks > 0);
#endif
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
	strlcpy(pack_chat.szCountry, ch->GetDesc()->GetCountryName().c_str(), sizeof(pack_chat.szCountry));
	pack_chat.bEmpire = ch->GetEmpire();
#endif

	switch (pinfo->type)
	{
		case CHAT_TYPE_TALKING:
			{
				const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

				if (false)
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FYmirChatPacket(pack_chat,
								buf,
								strlen(buf),
								ch->GetName(),
								strlen(ch->GetName()),
								ch->GetMapIndex(),
								ch->GetEmpire(),
								ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)));
				}
				else
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FEmpireChatPacket(pack_chat,
								chatbuf,
								len,
								(ch->GetGMLevel() > GM_PLAYER ||
								 ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)) ? 0 : ch->GetEmpire(),
								ch->GetMapIndex(), strlen(ch->GetName())));
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, "", "NORMAL", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		case CHAT_TYPE_PARTY:
			{
				if (!ch->GetParty())
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("파티 중이 아닙니다."));
				else
				{
					TEMP_BUFFER tbuf;

					tbuf.write(&pack_chat, sizeof(pack_chat));
					tbuf.write(chatbuf, len);

					RawPacketToCharacterFunc f(tbuf.read_peek(), tbuf.size());
					ch->GetParty()->ForEachOnlineMember(f);
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetParty()->GetLeaderPID(), "", "PARTY", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		case CHAT_TYPE_GUILD:
			{
				if (!ch->GetGuild())
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("길드에 가입하지 않았습니다."));
				else
				{
					ch->GetGuild()->Chat(chatbuf);
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetGuild()->GetID(), ch->GetGuild()->GetName(), "GUILD", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		default:
			sys_err("Unknown chat type %d", pinfo->type);
			break;
	}

	return (iExtraLen);
}

void CInputMain::ItemUse(LPCHARACTER ch, const char * data)
{
#ifdef ENABLE_GLOBAL_REWARD
	LPITEM item = ch->GetItem(((struct command_item_use*)data)->Cell);
	const DWORD itemVnum = item ? item->GetVnum() : 0;
	const bool isSuccess = ch->UseItem(((struct command_item_use*)data)->Cell);
	if (isSuccess)
		CHARACTER_MANAGER::Instance().DoReward(ch, REWARD_MISSION_USE_ITEM, itemVnum, 1);
#else
	ch->UseItem(((struct command_item_use *) data)->Cell);
#endif
}

void CInputMain::ItemToItem(LPCHARACTER ch, const char * pcData)
{
	TPacketCGItemUseToItem * p = (TPacketCGItemUseToItem *) pcData;
#ifdef ENABLE_GLOBAL_REWARD
	if (ch)
	{
		LPITEM item = ch->GetItem(p->Cell);
		const DWORD itemVnum = item ? item->GetVnum() : 0;
		const bool isSuccess = ch->UseItem(p->Cell, p->TargetCell);
		if (isSuccess)
			CHARACTER_MANAGER::Instance().DoReward(ch, REWARD_MISSION_USE_ITEM, itemVnum, 1);
	}
#else
	if (ch)
		ch->UseItem(p->Cell, p->TargetCell);
#endif
}

void CInputMain::ItemDrop(LPCHARACTER ch, const char * data)
{
	struct command_item_drop * pinfo = (struct command_item_drop *) data;

	//MONARCH_LIMIT
	//if (ch->IsMonarch())
	//	return;
	//END_MONARCH_LIMIT
	if (!ch)
		return;

	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell);
}

void CInputMain::ItemDrop2(LPCHARACTER ch, const char * data)
{
	//MONARCH_LIMIT
	//if (ch->IsMonarch())
	//	return;
	//END_MONARCH_LIMIT

	TPacketCGItemDrop2 * pinfo = (TPacketCGItemDrop2 *) data;

	if (!ch)
		return;
	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell, pinfo->count);
}

#ifdef ENABLE_ITEM_MANAGER
void CInputMain::ItemDestroy(LPCHARACTER ch, const char *data)
{
	struct command_item_destroy *pinfo = (struct command_item_destroy *)data;
	if (ch)
	{
		ch->DestroyItem(pinfo->Cell);
	}
}

void CInputMain::ItemSell(LPCHARACTER ch, const char *data)
{
	TPacketCGItemSell *pinfo = (TPacketCGItemSell *)data;

	if (!ch)
	{
		return;
	}

	if (pinfo->gold > 0)
	{
		ch->DropGold(pinfo->gold);
	}
	else
	{
		ch->SellItem(pinfo->Cell);
	}
}
#endif

void CInputMain::ItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (ch)
		ch->MoveItem(pinfo->Cell, pinfo->CellTo, pinfo->count);
}

void CInputMain::ItemPickup(LPCHARACTER ch, const char * data)
{
	struct command_item_pickup * pinfo = (struct command_item_pickup*) data;
	if (ch)
		ch->PickupItem(pinfo->vid);
}

void CInputMain::QuickslotAdd(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_add * pinfo = (struct command_quickslot_add *) data;
	ch->SetQuickslot(pinfo->pos, pinfo->slot);
}

void CInputMain::QuickslotDelete(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_del * pinfo = (struct command_quickslot_del *) data;
	ch->DelQuickslot(pinfo->pos);
}

void CInputMain::QuickslotSwap(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_swap * pinfo = (struct command_quickslot_swap *) data;
	ch->SwapQuickslot(pinfo->pos, pinfo->change_pos);
}

int CInputMain::Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGMessenger* p = (TPacketCGMessenger*) c_pData;

	if (uiBytes < sizeof(TPacketCGMessenger))
		return -1;

	c_pData += sizeof(TPacketCGMessenger);
	uiBytes -= sizeof(TPacketCGMessenger);

	switch (p->subheader)
	{
		case MESSENGER_SUBHEADER_CG_ADD_BY_VID:
			{
				if (uiBytes < sizeof(TPacketCGMessengerAddByVID))
					return -1;

				TPacketCGMessengerAddByVID * p2 = (TPacketCGMessengerAddByVID *) c_pData;
				LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

				if (!ch_companion)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->IsObserverMode())
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch_companion->IsBlockMode(BLOCK_MESSENGER_INVITE))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 메신져 추가 거부 상태입니다."));
					return sizeof(TPacketCGMessengerAddByVID);
				}

				LPDESC d = ch_companion->GetDesc();

				if (!d)
					return sizeof(TPacketCGMessengerAddByVID);

#ifdef ENABLE_GM_BLOCK
				if (ch->IsGM() == true && ch_companion->IsGM() == false)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_FRIEND_TEAM"));
					return sizeof(TPacketCGMessengerAddByVID);
				}
				
				if (ch->IsGM() == false && ch_companion->IsGM() == true)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_FRIEND"));
					return sizeof(TPacketCGMessengerAddByVID);
				}
#endif

				if (ch->GetDesc() == d)
					return sizeof(TPacketCGMessengerAddByVID);

				MessengerManager::instance().RequestToAdd(ch, ch_companion);
				//MessengerManager::instance().AddToList(ch->GetName(), ch_companion->GetName());
			}
			return sizeof(TPacketCGMessengerAddByVID);

		case MESSENGER_SUBHEADER_CG_ADD_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));


				LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

				if (!tch)
#ifdef CROSS_CHANNEL_FRIEND_REQUEST
					MessengerManager::instance().P2PRequestToAdd_Stage1(ch, name);
#else
#ifdef ENABLE_GM_BLOCK
					if (tch->IsGM() == true && ch->IsGM() == false)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_FRIEND"));
					}
					else if (tch->IsGM() == false && ch->IsGM() == true)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_FRIEND_TEAM"));
					}
#endif
#endif
				else
				{
					if (tch == ch)
						return CHARACTER_NAME_MAX_LEN;

					if (tch->IsBlockMode(BLOCK_MESSENGER_INVITE) == true)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 메신져 추가 거부 상태입니다."));
					}
					else
					{
						MessengerManager::instance().RequestToAdd(ch, tch);
						//MessengerManager::instance().AddToList(ch->GetName(), tch->GetName());
					}
				}
			}
			return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_REMOVE:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char char_name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(char_name, c_pData, sizeof(char_name));
				MessengerManager::instance().RemoveFromList(ch->GetName(), char_name);
				MessengerManager::instance().RemoveFromList(char_name, ch->GetName()); // @fixme183
			}
			return CHARACTER_NAME_MAX_LEN;

		default:
			sys_err("CInputMain::Messenger : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

#ifdef ENABLE_SPECIAL_INVENTORY
int CInputMain::Shop(LPCHARACTER ch, const char* data)
#else
int CInputMain::Shop(LPCHARACTER ch, const char* data, size_t uiBytes)
#endif
{
	TPacketCGShop * p = (TPacketCGShop *) data;

#ifndef ENABLE_SPECIAL_INVENTORY
	if (uiBytes < sizeof(TPacketCGShop))
		return -1;
#endif

	if (test_server)
		sys_log(0, "CInputMain::Shop() ==> SubHeader %d", p->subheader);

#ifndef ENABLE_SPECIAL_INVENTORY
	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);
#endif

	switch (p->subheader)
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		case SHOP_SUBHEADER_CG_END:
		{
			sys_log(1, "INPUT: %s SHOP: END", ch->GetName());
			CShopManager::Instance().StopShopping(ch);
		}
		break;

		case SHOP_SUBHEADER_CG_BUY:
		{
			sys_log(1, "INPUT: %s SHOP: BUY %d", ch->GetName(), static_cast<uint8_t>(p->wPos));
			CShopManager::Instance().Buy(ch, static_cast<uint8_t>(p->wPos));
		}
		break;

		case SHOP_SUBHEADER_CG_SELL:
		{
			sys_log(0, "INPUT: %s pos %d SHOP: SELL", ch->GetName(), p->wPos);
			CShopManager::Instance().Sell(ch, p->wPos);
		}
		break;

		case SHOP_SUBHEADER_CG_SELL2:
		{
			sys_log(0, "INPUT: %s pos %d count %d SHOP: SELL2", ch->GetName(), p->wPos, p->bCount);
			CShopManager::Instance().Sell(ch, p->wPos, p->bCount);
		}
		break;
#else
		case SHOP_SUBHEADER_CG_END:
			sys_log(1, "INPUT: %s SHOP: END", ch->GetName());
			CShopManager::instance().StopShopping(ch);
			return 0;

		case SHOP_SUBHEADER_CG_BUY:
			{
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
					return -1;

				BYTE bPos = *(c_pData + 1);
				sys_log(1, "INPUT: %s SHOP: BUY %d", ch->GetName(), bPos);
				CShopManager::instance().Buy(ch, bPos);
				return (sizeof(BYTE) + sizeof(BYTE));
			}

		case SHOP_SUBHEADER_CG_SELL:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE pos = *c_pData;

				sys_log(0, "INPUT: %s SHOP: SELL", ch->GetName());
				CShopManager::instance().Sell(ch, pos);
				return sizeof(BYTE);
			}

		case SHOP_SUBHEADER_CG_SELL2:
			{
#ifdef ENABLE_EXTENDED_ITEM_COUNT
				if (uiBytes < sizeof(BYTE) + sizeof(short))
					return -1;

				BYTE pos = *reinterpret_cast<const BYTE*>(c_pData);
				short sCount = *(c_pData + sizeof(short));

				sys_log(0, "INPUT: %s SHOP: SELL2", ch->GetName());

				CShopManager::instance().Sell(ch, pos, sCount);
					return sizeof(BYTE) + sizeof(short);
#else
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
					return -1;

				BYTE pos = *(c_pData++);
				BYTE count = *(c_pData);

				sys_log(0, "INPUT: %s SHOP: SELL2", ch->GetName());
				CShopManager::instance().Sell(ch, pos, count);
				return sizeof(BYTE) + sizeof(BYTE);
#endif
			}
#endif

		default:
			sys_err("CInputMain::Shop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

void CInputMain::OnClick(LPCHARACTER ch, const char * data)
{
	struct command_on_click *	pinfo = (struct command_on_click *) data;
	LPCHARACTER			victim;

	if ((victim = CHARACTER_MANAGER::instance().Find(pinfo->vid)))
		victim->OnClick(ch);
	else if (test_server)
	{
		sys_err("CInputMain::OnClick %s.Click.NOT_EXIST_VID[%d]", ch->GetName(), pinfo->vid);
	}
}

void CInputMain::Exchange(LPCHARACTER ch, const char * data)
{
	struct command_exchange * pinfo = (struct command_exchange *) data;
	LPCHARACTER	to_ch = NULL;

	if (!ch->CanHandleItem())
		return;

	int iPulse = thecore_pulse();

	if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
	{
		if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래 후 %d초 이내에 창고를 열수 없습니다."), g_nPortalLimitTime);
			return;
		}

		if (iPulse - to_ch->GetGuildStorageLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°A·¡ EA %dAE AI³≫¿¡ A￠°i¸| ¿­¼o ¾ø½A´I´U."), g_nPortalLimitTime);
			return;
		}

		if( true == to_ch->IsDead() )
		{
			return;
		}
	}

	sys_log(0, "CInputMain()::Exchange()  SubHeader %d ", pinfo->sub_header);

	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래 후 %d초 이내에 창고를 열수 없습니다."), g_nPortalLimitTime);
		return;
	}

	if (iPulse - ch->GetGuildStorageLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°A·¡ EA %dAE AI³≫¿¡ A￠°i¸| ¿­¼o ¾ø½A´I´U."), g_nPortalLimitTime);
		return;
	}

	switch (pinfo->sub_header)
	{
		case EXCHANGE_SUBHEADER_CG_START:	// arg1 == vid of target character
			if (!ch->GetExchange())
			{
				if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
				{
					//MONARCH_LIMIT

					if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("창고를 연후 %d초 이내에는 거래를 할수 없습니다."), g_nPortalLimitTime);

						if (test_server)
							ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("창고를 연후 %d초 이내에는 거래를 할수 없습니다."), g_nPortalLimitTime);

						if (test_server)
							to_ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, to_ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (iPulse - ch->GetGuildStorageLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A￠°i¸| ¿￢EA %dAE AI³≫¿¡´A °A·¡¸| CO¼o ¾ø½A´I´U."), g_nPortalLimitTime);

						if (test_server)
							ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, ch->GetGuildStorageLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (iPulse - to_ch->GetGuildStorageLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A￠°i¸| ¿￢EA %dAE AI³≫¿¡´A °A·¡¸| CO¼o ¾ø½A´I´U."), g_nPortalLimitTime);


						if (test_server)
							to_ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, to_ch->GetGuildStorageLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

#ifdef ENABLE_GOLD_MAX_EXTENDED
					if (ch->GetGold() >= GOLD_MAX_MAX)
#else
					if (ch->GetGold() >= GOLD_MAX)
#endif
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("액수가 20억 냥을 초과하여 거래를 할수가 없습니다.."));

						sys_err("[OVERFLOG_GOLD] START (%u) id %u name %s ", ch->GetGold(), ch->GetPlayerID(), ch->GetName());
						return;
					}

					if (to_ch->IsPC())
					{
						if (quest::CQuestManager::instance().GiveItemToPC(ch->GetPlayerID(), to_ch))
						{
							sys_log(0, "Exchange canceled by quest %s %s", ch->GetName(), to_ch->GetName());
							return;
						}
					}

					if (ch->GetMyShop() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen()
#ifdef __PREMIUM_PRIVATE_SHOP__
						|| ch->IsEditingPrivateShop() || ch->IsShopSearch() || ch->GetViewingPrivateShop() 
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
						|| ch->IsAcceOpened()
#endif
#if defined(__BL_67_ATTR__)
						|| ch->Is67AttrOpen()
#endif
						|| ch->IsOpenGuildStorage()
					)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 거래중일경우 개인상점을 열수가 없습니다."));
						return;
					}

					ch->ExchangeStart(to_ch);
				}
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_ADD:	// arg1 == position of item, arg2 == position in exchange window
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2
#ifdef ENABLE_CHECKINOUT_UPDATE
										   ,
										   pinfo->bAutoItemPos
#endif
					);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_DEL:	// arg1 == position of item
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->RemoveItem(pinfo->arg1);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ELK_ADD:	// arg1 == amount of gold
			if (ch->GetExchange())
			{
#ifdef ENABLE_GOLD_MAX_EXTENDED
				const unsigned long long nTotalGold = static_cast<unsigned long long>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<unsigned long long>(pinfo->arg1);
				if (GOLD_MAX_MAX <= nTotalGold)
#else
				const int64_t nTotalGold = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<int64_t>(pinfo->arg1);
				if (GOLD_MAX <= nTotalGold)
#endif
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방의 총금액이 20억 냥을 초과하여 거래를 할수가 없습니다.."));

#ifdef ENABLE_GOLD_MAX_EXTENDED
					sys_err("[OVERFLOW_GOLD] ELK_ADD (%llu) id %u name %s ",
#else
					sys_err("[OVERFLOW_GOLD] ELK_ADD (%u) id %u name %s ",
#endif
							ch->GetExchange()->GetCompany()->GetOwner()->GetGold(),
							ch->GetExchange()->GetCompany()->GetOwner()->GetPlayerID(),
						   	ch->GetExchange()->GetCompany()->GetOwner()->GetName());

					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddGold(pinfo->arg1);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ACCEPT:	// arg1 == not used
			if (ch->GetExchange())
			{
				sys_log(0, "CInputMain()::Exchange() ==> ACCEPT ");
				ch->GetExchange()->Accept(true);
			}

			break;

		case EXCHANGE_SUBHEADER_CG_CANCEL:	// arg1 == not used
			if (ch->GetExchange())
				ch->GetExchange()->Cancel();
			break;
	}
}

void CInputMain::Position(LPCHARACTER ch, const char * data)
{
	struct command_position * pinfo = (struct command_position *) data;

	switch (pinfo->position)
	{
		case POSITION_GENERAL:
			ch->Standup();
			break;

		case POSITION_SITTING_CHAIR:
			ch->Sitdown(0);
			break;

		case POSITION_SITTING_GROUND:
			ch->Sitdown(1);
			break;
	}
}

static const int ComboSequenceBySkillLevel[3][8] =
{
	// 0   1   2   3   4   5   6   7
	{ 14, 15, 16, 17,  0,  0,  0,  0 },
	{ 14, 15, 16, 18, 20,  0,  0,  0 },
	{ 14, 15, 16, 18, 19, 17,  0,  0 },
};

#define COMBO_HACK_ALLOWABLE_MS	100

bool CheckComboHack(LPCHARACTER ch, BYTE bArg, DWORD dwTime, bool CheckSpeedHack)
{
	if(!gHackCheckEnable) return false;

	//	if (IsStun() || IsDead()) return false;

	if (ch->IsStun() || ch->IsDead())
		return false;
	int ComboInterval = dwTime - ch->GetLastComboTime();
	int HackScalar = 0;
#if 0
	sys_log(0, "COMBO: %s arg:%u seq:%u delta:%d checkspeedhack:%d",
			ch->GetName(), bArg, ch->GetComboSequence(), ComboInterval - ch->GetValidComboInterval(), CheckSpeedHack);
#endif

	if (bArg == 14)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{
			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 300;

			//sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
			//		ch->GetName(),
			//		bArg,
			//		ComboInterval,
			//		ch->GetValidComboInterval(),
			//		ch->GetPoint(POINT_ATT_SPEED),
			//	    ch->IsRiding() ? "yes" : "no");
		}

		ch->SetComboSequence(1);
		ch->SetValidComboInterval((int) (ani_combo_speed(ch, 1) / (ch->GetPoint(POINT_ATT_SPEED) / 100.f)));
		ch->SetLastComboTime(dwTime);
	}
	else if (bArg > 14 && bArg < 22)
	{
		int idx = MIN(2, ch->GetComboIndex());

		if (ch->GetComboSequence() > 5)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);
			sys_log(0, "COMBO_HACK: 5 %s combo_seq:%d", ch->GetName(), ch->GetComboSequence());
		}

		else if (bArg == 21 &&
				 idx == 2 &&
				 ch->GetComboSequence() == 5 &&
				 ch->GetJob() == JOB_ASSASSIN &&
				 ch->GetWear(WEAR_WEAPON) &&
				 ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_DAGGER)
			ch->SetValidComboInterval(300);
#ifdef ENABLE_WOLFMAN_CHARACTER
		else if (bArg == 21 && idx == 2 && ch->GetComboSequence() == 5 && ch->GetJob() == JOB_WOLFMAN && ch->GetWear(WEAR_WEAPON) && ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_CLAW)
			ch->SetValidComboInterval(300);
#endif
		else if (ComboSequenceBySkillLevel[idx][ch->GetComboSequence()] != bArg)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);

			sys_log(0, "COMBO_HACK: 3 %s arg:%u valid:%u combo_idx:%d combo_seq:%d",
					ch->GetName(),
					bArg,
					ComboSequenceBySkillLevel[idx][ch->GetComboSequence()],
					idx,
					ch->GetComboSequence());
		}
		else
		{
			if (CheckSpeedHack && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
			{
				HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

				sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
						ch->GetName(),
						bArg,
						ComboInterval,
						ch->GetValidComboInterval(),
						ch->GetPoint(POINT_ATT_SPEED),
						ch->IsRiding() ? "yes" : "no");
			}

			//if (ch->IsHorseRiding())
			if (ch->IsRiding())
				ch->SetComboSequence(ch->GetComboSequence() == 1 ? 2 : 1);
			else
				ch->SetComboSequence(ch->GetComboSequence() + 1);

			ch->SetValidComboInterval((int) (ani_combo_speed(ch, bArg - 13) / (ch->GetPoint(POINT_ATT_SPEED) / 100.f)));
			ch->SetLastComboTime(dwTime);
		}
	}
	else if (bArg == 13)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{
			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

			//sys_log(0, "COMBO_HACK: 6 %s arg:%u interval:%d valid:%u atkspd:%u",
			//		ch->GetName(),
			//		bArg,
			//		ComboInterval,
			//		ch->GetValidComboInterval(),
			//		ch->GetPoint(POINT_ATT_SPEED));
		}

		if (ch->GetRaceNum() >= MAIN_RACE_MAX_NUM)
		{
			// POLYMORPH_BUG_FIX

			// DELETEME

			float normalAttackDuration = CMotionManager::instance().GetNormalAttackDuration(ch->GetRaceNum());
			int k = (int) (normalAttackDuration / ((float) ch->GetPoint(POINT_ATT_SPEED) / 100.f) * 900.f);
			ch->SetValidComboInterval(k);
			ch->SetLastComboTime(dwTime);
			// END_OF_POLYMORPH_BUG_FIX
		}
		else
		{
			//if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
			//{
			//	LogManager::instance().HackLog("Hacker", ch);
			//	sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
			//}

			// by rtsummit
		}
	}
	else
	{
		if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
		{
			LogManager::instance().HackLog("Hacker", ch);
			sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
		}

		HackScalar = 10;
		ch->SetValidComboInterval(300);
	}

	if (HackScalar)
	{
		if (get_dword_time() - ch->GetLastMountTime() > 1500)
			ch->IncreaseComboHackCount(1 + HackScalar);

		ch->SkipComboAttackByTime(ch->GetValidComboInterval());
	}

	return HackScalar;
}

void CInputMain::Move(LPCHARACTER ch, const char * data)
{
	if (!ch->CanMove())
		return;

	struct command_move * pinfo = (struct command_move *) data;

	if (pinfo->bFunc >= FUNC_MAX_NUM && !(pinfo->bFunc & 0x80))
	{
		sys_err("invalid move type: %s", ch->GetName());
		return;
	}

	//enum EMoveFuncType
	//{
	//	FUNC_WAIT,
	//	FUNC_MOVE,
	//	FUNC_ATTACK,
	//	FUNC_COMBO,
	//	FUNC_MOB_SKILL,
	//	_FUNC_SKILL,
	//	FUNC_MAX_NUM,
	//	FUNC_SKILL = 0x80,
	//};

	{
		// #define ENABLE_TP_SPEED_CHECK
		#ifdef ENABLE_TP_SPEED_CHECK
		const float fDist = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100);
		if (((false == ch->IsRiding() && fDist > 25) || fDist > 60) && OXEVENT_MAP_INDEX != ch->GetMapIndex()) // @fixme106 (changed 40 to 60)
		{
			#ifdef ENABLE_HACK_TELEPORT_LOG // @warme006
			{
				const PIXEL_POSITION & warpPos = ch->GetWarpPosition();

				if (warpPos.x == 0 && warpPos.y == 0)
					LogManager::instance().HackLog("Teleport", ch);
			}
			#endif
			sys_log(0, "MOVE: %s trying to move too far (dist: %.1fm) Riding(%d)", ch->GetName(), fDist, ch->IsRiding());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
		#endif
		#ifdef ENABLE_CHECK_GHOSTMODE
		if (ch->IsPC() && ch->IsDead())
		{
			sys_log(0, "MOVE: %s trying to move as dead", ch->GetName());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
		#endif

		DWORD dwCurTime = get_dword_time();

		bool CheckSpeedHack = (false == ch->GetDesc()->IsHandshaking() && dwCurTime - ch->GetDesc()->GetClientTime() > 7000);

		if (CheckSpeedHack)
		{
			int iDelta = (int) (pinfo->dwTime - ch->GetDesc()->GetClientTime());
			int iServerDelta = (int) (dwCurTime - ch->GetDesc()->GetClientTime());

			iDelta = (int) (dwCurTime - pinfo->dwTime);

			if (iDelta >= 30000)
			{
				sys_log(0, "SPEEDHACK: slow timer name %s delta %d", ch->GetName(), iDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}

			else if (iDelta < -(iServerDelta / 50))
			{
				sys_log(0, "SPEEDHACK: DETECTED! %s (delta %d %d)", ch->GetName(), iDelta, iServerDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}
		}

		if (pinfo->bFunc == FUNC_COMBO && g_bCheckMultiHack)
		{
			CheckComboHack(ch, pinfo->bArg, pinfo->dwTime, CheckSpeedHack);
		}
	}

	if (pinfo->bFunc == FUNC_MOVE)
	{
		if (ch->GetLimitPoint(POINT_MOV_SPEED) == 0)
			return;

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();				// ""

		ch->Goto(pinfo->lX, pinfo->lY);
	}
	else
	{
		if (pinfo->bFunc == FUNC_ATTACK || pinfo->bFunc == FUNC_COMBO)
			ch->OnMove(true);
		else if (pinfo->bFunc & FUNC_SKILL)
		{
			const int MASK_SKILL_MOTION = 0x7F;
			unsigned int motion = pinfo->bFunc & MASK_SKILL_MOTION;

			if (!ch->IsUsableSkillMotion(motion))
			{
				const char* name = ch->GetName();
				unsigned int job = ch->GetJob();
				unsigned int group = ch->GetSkillGroup();

				char szBuf[256];
				snprintf(szBuf, sizeof(szBuf), "SKILL_HACK: name=%s, job=%d, group=%d, motion=%d", name, job, group, motion);
				LogManager::instance().HackLog(szBuf, ch->GetDesc()->GetAccountTable().login, ch->GetName(), ch->GetDesc()->GetHostName());
				sys_log(0, "%s", szBuf);

				if (test_server)
				{
					ch->GetDesc()->DelayedDisconnect(number(2, 8));
					ch->ChatPacket(CHAT_TYPE_INFO, szBuf);
				}
				else
				{
					ch->GetDesc()->DelayedDisconnect(number(150, 500));
				}
			}

			ch->OnMove();
		}

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();

		ch->Move(pinfo->lX, pinfo->lY);
		ch->Stop();
		ch->StopStaminaConsume();
	}

	TPacketGCMove pack;

	pack.bHeader      = HEADER_GC_MOVE;
	pack.bFunc        = pinfo->bFunc;
	pack.bArg         = pinfo->bArg;
	pack.bRot         = pinfo->bRot;
	pack.dwVID        = ch->GetVID();
	pack.lX           = pinfo->lX;
	pack.lY           = pinfo->lY;
	pack.dwTime       = pinfo->dwTime;
	pack.dwDuration   = (pinfo->bFunc == FUNC_MOVE) ? ch->GetCurrentMoveDuration() : 0;

	ch->PacketAround(&pack, sizeof(TPacketGCMove), ch);

	/*
	sys_log(0,
			"MOVE: %s Func:%u Arg:%u Pos:%dx%d Time:%u Dist:%.1f",
			ch->GetName(),
			pinfo->bFunc,
			pinfo->bArg,
			pinfo->lX / 100,
			pinfo->lY / 100,
			pinfo->dwTime,
			fDist);
	*/
}

void CInputMain::Attack(LPCHARACTER ch, const BYTE header, const char* data)
{
	if (NULL == ch)
		return;

	struct type_identifier
	{
		BYTE header;
		BYTE type;
	};

	const struct type_identifier* const type = reinterpret_cast<const struct type_identifier*>(data);

	if (type->type > 0)
	{
		if (false == ch->CanUseSkill(type->type))
		{
			return;
		}

		switch (type->type)
		{
			case SKILL_GEOMPUNG:
			case SKILL_SANGONG:
			case SKILL_YEONSA:
			case SKILL_KWANKYEOK:
			case SKILL_HWAJO:
			case SKILL_GIGUNG:
			case SKILL_PABEOB:
			case SKILL_MARYUNG:
			case SKILL_TUSOK:
			case SKILL_MAHWAN:
			case SKILL_BIPABU:
			case SKILL_NOEJEON:
			case SKILL_CHAIN:
			case SKILL_HORSE_WILDATTACK_RANGE:
				if (HEADER_CG_SHOOT != type->header)
				{
					if (test_server)
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Attack :name[%s] Vnum[%d] can't use skill by attack(warning)"), type->type);
					return;
				}
				break;
		}
	}

	switch (header)
	{
		case HEADER_CG_ATTACK:
			{
				if (NULL == ch->GetDesc())
					return;

				const TPacketCGAttack* const packMelee = reinterpret_cast<const TPacketCGAttack*>(data);

				ch->GetDesc()->AssembleCRCMagicCube(packMelee->bCRCMagicCubeProcPiece, packMelee->bCRCMagicCubeFilePiece);

				LPCHARACTER	victim = CHARACTER_MANAGER::instance().Find(packMelee->dwVID);

				if (NULL == victim || ch == victim)
					return;

				switch (victim->GetCharType())
				{
					case CHAR_TYPE_NPC:
					case CHAR_TYPE_WARP:
					case CHAR_TYPE_GOTO:
					case CHAR_TYPE_MOUNT:	  // ENABLE_MOUNT_SYSTEM_EX
						return;
				}

				if (packMelee->bType > 0)
				{
					if (false == ch->CheckSkillHitCount(packMelee->bType, victim->GetVID()))
					{
						return;
					}
				}

				ch->Attack(victim, packMelee->bType);
			}
			break;

		case HEADER_CG_SHOOT:
			{
				const TPacketCGShoot* const packShoot = reinterpret_cast<const TPacketCGShoot*>(data);

				ch->Shoot(packShoot->bType);
			}
			break;
	}
}

int CInputMain::SyncPosition(LPCHARACTER ch, const char * c_pcData, size_t uiBytes)
{
	const TPacketCGSyncPosition* pinfo = reinterpret_cast<const TPacketCGSyncPosition*>( c_pcData );

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGSyncPosition);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (0 != (iExtraLen % sizeof(TPacketCGSyncPositionElement)))
	{
		sys_err("invalid packet length %d (name: %s)", pinfo->wSize, ch->GetName());
		return iExtraLen;
	}

	int iCount = iExtraLen / sizeof(TPacketCGSyncPositionElement);

	if (iCount <= 0)
		return iExtraLen;

	static const int nCountLimit = 16;

	if( iCount > nCountLimit )
	{
		//LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );
		sys_err( "Too many SyncPosition Count(%d) from Name(%s)", iCount, ch->GetName() );
		//ch->GetDesc()->SetPhase(PHASE_CLOSE);
		//return -1;
		iCount = nCountLimit;
	}

	TEMP_BUFFER tbuf;
	LPBUFFER lpBuf = tbuf.getptr();

	TPacketGCSyncPosition * pHeader = (TPacketGCSyncPosition *) buffer_write_peek(lpBuf);
	buffer_write_proceed(lpBuf, sizeof(TPacketGCSyncPosition));

	const TPacketCGSyncPositionElement* e =
		reinterpret_cast<const TPacketCGSyncPositionElement*>(c_pcData + sizeof(TPacketCGSyncPosition));

	timeval tvCurTime;
	gettimeofday(&tvCurTime, NULL);

	for (int i = 0; i < iCount; ++i, ++e)
	{
		LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(e->dwVID);

		if (!victim)
			continue;

		switch (victim->GetCharType())
		{
			case CHAR_TYPE_NPC:
			case CHAR_TYPE_WARP:
			case CHAR_TYPE_GOTO:
			case CHAR_TYPE_MOUNT:	  // ENABLE_MOUNT_SYSTEM_EX
				continue;
		}

		if (!victim->SetSyncOwner(ch))
			continue;

		const float fDistWithSyncOwner = DISTANCE_SQRT( (victim->GetX() - ch->GetX()) / 100, (victim->GetY() - ch->GetY()) / 100 );
		static const float fLimitDistWithSyncOwner = 2500.f + 1000.f;

		if (fDistWithSyncOwner > fLimitDistWithSyncOwner)
		{
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too far SyncPosition DistanceWithSyncOwner(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
					fDistWithSyncOwner, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}

		const float fDist = DISTANCE_SQRT( (victim->GetX() - e->lX) / 100, (victim->GetY() - e->lY) / 100 );
		static const long g_lValidSyncInterval = 100 * 1000; // 100ms
		const timeval &tvLastSyncTime = victim->GetLastSyncTime();
		timeval *tvDiff = timediff(&tvCurTime, &tvLastSyncTime);

		if (tvDiff->tv_sec == 0 && tvDiff->tv_usec < g_lValidSyncInterval)
		{
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too often SyncPosition Interval(%ldms)(%s) from Name(%s) VICTIM(%d,%d) SYNC(%d,%d)",
					tvDiff->tv_sec * 1000 + tvDiff->tv_usec / 1000, victim->GetName(), ch->GetName(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}
		else if( fDist > 25.0f )
		{
			LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

			sys_err( "Too far SyncPosition Distance(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
				   	fDist, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
				  e->lX, e->lY );

			ch->GetDesc()->SetPhase(PHASE_CLOSE);

			return -1;
		}
		else
		{
			victim->SetLastSyncTime(tvCurTime);
			victim->Sync(e->lX, e->lY);
			buffer_write(lpBuf, e, sizeof(TPacketCGSyncPositionElement));
		}
	}

	if (buffer_size(lpBuf) != sizeof(TPacketGCSyncPosition))
	{
		pHeader->bHeader = HEADER_GC_SYNC_POSITION;
		pHeader->wSize = buffer_size(lpBuf);

		ch->PacketAround(buffer_read_peek(lpBuf), buffer_size(lpBuf), ch);
	}

	return iExtraLen;
}

void CInputMain::FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader)
{
	TPacketCGFlyTargeting * p = (TPacketCGFlyTargeting *) pcData;
	ch->FlyTarget(p->dwTargetVID, p->x, p->y, bHeader);
}

void CInputMain::UseSkill(LPCHARACTER ch, const char * pcData)
{
	TPacketCGUseSkill * p = (TPacketCGUseSkill *) pcData;
	ch->UseSkill(p->dwVnum, CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::ScriptButton(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptButton * p = (TPacketCGScriptButton *) c_pData;
	sys_log(0, "QUEST ScriptButton pid %d idx %u", ch->GetPlayerID(), p->idx);

	quest::PC* pc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (pc && pc->IsConfirmWait())
	{
		quest::CQuestManager::instance().Confirm(ch->GetPlayerID(), quest::CONFIRM_TIMEOUT);
	}
	else if (p->idx & 0x80000000)
	{
		quest::CQuestManager::Instance().QuestInfo(ch->GetPlayerID(), p->idx & 0x7fffffff);
	}
	else
	{
		quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), p->idx);
	}
}

void CInputMain::ScriptAnswer(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptAnswer * p = (TPacketCGScriptAnswer *) c_pData;
	sys_log(0, "QUEST ScriptAnswer pid %d answer %d", ch->GetPlayerID(), p->answer);

	if (p->answer > 250)
	{
		quest::CQuestManager::Instance().Resume(ch->GetPlayerID());
	}
	else
	{
		quest::CQuestManager::Instance().Select(ch->GetPlayerID(),  p->answer);
	}
}

// SCRIPT_SELECT_ITEM
void CInputMain::ScriptSelectItem(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptSelectItem* p = (TPacketCGScriptSelectItem*) c_pData;
	sys_log(0, "QUEST ScriptSelectItem pid %d answer %d", ch->GetPlayerID(), p->selection);
	quest::CQuestManager::Instance().SelectItem(ch->GetPlayerID(), p->selection);
}
// END_OF_SCRIPT_SELECT_ITEM

void CInputMain::QuestInputString(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestInputString * p = (TPacketCGQuestInputString*) c_pData;

	char msg[65];
	strlcpy(msg, p->msg, sizeof(msg));
	sys_log(0, "QUEST InputString pid %u msg %s", ch->GetPlayerID(), msg);

	quest::CQuestManager::Instance().Input(ch->GetPlayerID(), msg);
}

void CInputMain::QuestConfirm(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestConfirm* p = (TPacketCGQuestConfirm*) c_pData;
	LPCHARACTER ch_wait = CHARACTER_MANAGER::instance().FindByPID(p->requestPID);
	if (p->answer)
		p->answer = quest::CONFIRM_YES;
	sys_log(0, "QuestConfirm from %s pid %u name %s answer %d", ch->GetName(), p->requestPID, (ch_wait)?ch_wait->GetName():"", p->answer);
	if (ch_wait)
	{
		quest::CQuestManager::Instance().Confirm(ch_wait->GetPlayerID(), (quest::EQuestConfirmType) p->answer, ch->GetPlayerID());
	}
}

void CInputMain::Target(LPCHARACTER ch, const char * pcData)
{
	TPacketCGTarget * p = (TPacketCGTarget *) pcData;

	building::LPOBJECT pkObj = building::CManager::instance().FindObjectByVID(p->dwVID);

	if (pkObj)
	{
		TPacketGCTarget pckTarget;
		pckTarget.header = HEADER_GC_TARGET;
		pckTarget.dwVID = p->dwVID;
		ch->GetDesc()->Packet(&pckTarget, sizeof(TPacketGCTarget));
	}
	else
		ch->SetTarget(CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::Warp(LPCHARACTER ch, const char * pcData)
{
	ch->WarpEnd();
}

void CInputMain::SafeboxCheckin(LPCHARACTER ch, const char * c_pData)
{
	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return;

	TPacketCGSafeboxCheckin * p = (TPacketCGSafeboxCheckin *) c_pData;

	if (!ch->CanHandleItem())
		return;

	CSafebox * pkSafebox = ch->GetSafebox();
	LPITEM pkItem = ch->GetItem(p->ItemPos);

	if (!pkSafebox || !pkItem)
		return;

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("HAS_BLOCK_STATUS"));
		return;
	}
	// END_OF_BLOCK_PLAYER	

#ifdef ENABLE_GM_BLOCK
	if (ch->IsGM() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_SAFEBOX"));
		return;
	}
#endif

#ifdef ENABLE_CHECKINOUT_UPDATE
	if (p->bAutoItemPos)
	{
		int AutoPos = pkSafebox->GetEmptySafebox(pkItem->GetSize());
		if (AutoPos == -1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in your safebox"));
			return;
		}
		p->bSafePos = AutoPos;
	}
#endif

	if (pkItem->GetCell() >= INVENTORY_MAX_NUM && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
	    ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 창고로 옮길 수 없는 아이템 입니다."));
	    return;
	}

	if (!pkSafebox->IsEmpty(p->bSafePos, pkItem->GetSize()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 옮길 수 없는 위치입니다."));
		return;
	}

	if (pkItem->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 이 아이템은 넣을 수 없습니다."));
		return;
	}

	if( IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX) )
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 이 아이템은 넣을 수 없습니다."));
		return;
	}

	if (true == pkItem->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 이 아이템은 넣을 수 없습니다."));
		return;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (pkItem->IsEquipped())
	{
		int iWearCell = pkItem->FindEquipCell(ch);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !ch->UnequipItem(costumeWeapon))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return;
			}
		}
	}
#endif

	// @fixme140 BEGIN
	if (ITEM_BELT == pkItem->GetType() && CBeltInventoryHelper::IsExistItemInBeltInventory(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("벨트 인벤토리에 아이템이 존재하면 해제할 수 없습니다."));
		return;
	}
	// @fixme140 END

	pkItem->RemoveFromCharacter();
	if (!pkItem->IsDragonSoul())
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 255);
	pkSafebox->Add(p->bSafePos, pkItem);

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX PUT", szHint);
}

void CInputMain::SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall)
{
	TPacketCGSafeboxCheckout * p = (TPacketCGSafeboxCheckout *) c_pData;

	if (!ch->CanHandleItem())
		return;

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("HAS_BLOCK_STATUS"));
		return;
	}
	// END_OF_BLOCK_PLAYER	

	CSafebox * pkSafebox;

	if (bMall)
		pkSafebox = ch->GetMall();
	else
		pkSafebox = ch->GetSafebox();

	if (!pkSafebox)
		return;

	LPITEM pkItem = pkSafebox->Get(p->bSafePos);

	if (!pkItem)
		return;

#ifdef ENABLE_GM_BLOCK
	if (ch->IsGM() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_SAFEBOX"));
		return;
	}
#endif

#ifdef ENABLE_CHECKINOUT_UPDATE
	if (p->bAutoItemPos)
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		int AutoPos = pkItem->IsDragonSoul() ? ch->GetEmptyDragonSoulInventory(pkItem) : ch->GetEmptyInventory(pkItem);
#else
		int AutoPos = pkItem->IsDragonSoul() ? ch->GetEmptyDragonSoulInventory(pkItem) : ch->GetEmptyInventory(pkItem->GetSize());
#endif
		if (AutoPos == -1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough space."));
			return;
		}
		p->ItemPos = TItemPos(pkItem->IsDragonSoul() ? DRAGON_SOUL_INVENTORY : INVENTORY, AutoPos);
	}
#endif

	if (!ch->IsEmptyItemGrid(p->ItemPos, pkItem->GetSize()))
		return;

	if (pkItem->IsDragonSoul())
	{
		if (bMall)
		{
			DSManager::instance().DragonSoulItemInitialize(pkItem);
		}

		if (DRAGON_SOUL_INVENTORY != p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 옮길 수 없는 위치입니다."));
			return;
		}

		TItemPos DestPos = p->ItemPos;
		if (!DSManager::instance().IsValidCellForThisItem(pkItem, DestPos))
		{
			int iCell = ch->GetEmptyDragonSoulInventory(pkItem);
			if (iCell < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 옮길 수 없는 위치입니다."));
				return ;
			}
			DestPos = TItemPos (DRAGON_SOUL_INVENTORY, iCell);
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, DestPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 옮길 수 없는 위치입니다."));
			return;
		}
		// @fixme119
		if (p->ItemPos.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(pkItem))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 벨트 인벤토리로 옮길 수 없습니다."));
			return;
		}
#ifdef ENABLE_SPECIAL_INVENTORY
		if (pkItem->GetSpecialInventoryType() != p->ItemPos.GetSpecialInventoryType())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item here."));
			return;
		}
#endif

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}

	DWORD dwID = pkItem->GetID();
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	if (bMall)
		LogManager::instance().ItemLog(ch, pkItem, "MALL GET", szHint);
	else
		LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX GET", szHint);
}

void CInputMain::SafeboxItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (!ch->CanHandleItem())
		return;

	if (!ch->GetSafebox())
		return;

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("HAS_BLOCK_STATUS"));
		return;
	}
	// END_OF_BLOCK_PLAYER	

	ch->GetSafebox()->MoveItem(pinfo->Cell.cell, pinfo->CellTo.cell, pinfo->count);
}

// PARTY_JOIN_BUG_FIX
void CInputMain::PartyInvite(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	TPacketCGPartyInvite * p = (TPacketCGPartyInvite*) c_pData;

	LPCHARACTER pInvitee = CHARACTER_MANAGER::instance().Find(p->vid);

	if (!pInvitee || !ch->GetDesc() || !pInvitee->GetDesc())
	{
		sys_err("PARTY Cannot find invited character");
		return;
	}

#ifdef ENABLE_GM_BLOCK
	if (ch->IsGM() == true && pInvitee->IsGM() == false)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_GROUP"));
		return;
	}
	
	else if (ch->IsGM() == false && pInvitee->IsGM() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_GROUP_TEAM"));
		return;
	}
#endif

	ch->PartyInvite(pInvitee);
}

void CInputMain::PartyInviteAnswer(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	TPacketCGPartyInviteAnswer * p = (TPacketCGPartyInviteAnswer*) c_pData;

	LPCHARACTER pInviter = CHARACTER_MANAGER::instance().Find(p->leader_vid);

	if (!pInviter)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티요청을 한 캐릭터를 찾을수 없습니다."));
	// @fixme301 BEGIN
	else if (!pInviter->GetParty() && pInviter->GetMapIndex() >= 10000)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Party> The party leader is inside a dungeon."));
	else if (pInviter->GetParty() && pInviter->GetParty()->IsPartyInAnyDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Party> The party can't invite people if members are inside a dungeon."));
		pInviter->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Party> The party can't invite people if members are inside a dungeon."));
	}
	// @fixme301 END
	else if (!p->accept)
		pInviter->PartyInviteDeny(ch->GetPlayerID());
	else
		pInviter->PartyInviteAccept(ch);
}
// END_OF_PARTY_JOIN_BUG_FIX

void CInputMain::PartySetState(LPCHARACTER ch, const char* c_pData)
{
	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
		return;
	}

	TPacketCGPartySetState* p = (TPacketCGPartySetState*) c_pData;

	if (!ch->GetParty())
		return;

	if (ch->GetParty()->GetLeaderPID() != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 리더만 변경할 수 있습니다."));
		return;
	}

	if (!ch->GetParty()->IsMember(p->pid))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 상태를 변경하려는 사람이 파티원이 아닙니다."));
		return;
	}

	DWORD pid = p->pid;
	sys_log(0, "PARTY SetRole pid %d to role %d state %s", pid, p->byRole, p->flag ? "on" : "off");

	switch (p->byRole)
	{
		case PARTY_ROLE_NORMAL:
			break;

		case PARTY_ROLE_ATTACKER:
		case PARTY_ROLE_TANKER:
		case PARTY_ROLE_BUFFER:
		case PARTY_ROLE_SKILL_MASTER:
		case PARTY_ROLE_HASTE:
		case PARTY_ROLE_DEFENDER:
			if (ch->GetParty()->SetRole(pid, p->byRole, p->flag))
			{
				TPacketPartyStateChange pack;
				pack.dwLeaderPID = ch->GetPlayerID();
				pack.dwPID = p->pid;
				pack.bRole = p->byRole;
				pack.bFlag = p->flag;
				db_clientdesc->DBPacket(HEADER_GD_PARTY_STATE_CHANGE, 0, &pack, sizeof(pack));
			}

			break;

		default:
			sys_err("wrong byRole in PartySetState Packet name %s state %d", ch->GetName(), p->byRole);
			break;
	}
}

void CInputMain::PartyRemove(LPCHARACTER ch, const char* c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
		return;
	}

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 서버 문제로 파티 관련 처리를 할 수 없습니다."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 던전 안에서는 파티에서 추방할 수 없습니다."));
		return;
	}

	TPacketCGPartyRemove* p = (TPacketCGPartyRemove*) c_pData;

	if (!ch->GetParty())
		return;

	LPPARTY pParty = ch->GetParty();
	if (pParty->GetLeaderPID() == ch->GetPlayerID())
	{
		if (ch->GetDungeon())
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 던젼내에서는 파티원을 추방할 수 없습니다."));
		else
		{
			if (pParty->IsPartyInAnyDungeon()) // @fixme301
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티>던전 안에 파티원이 있어 파티를 해산 할 수 없습니다."));
				return;
			}

			// leader can remove any member
			if (p->pid == ch->GetPlayerID() || pParty->GetMemberCount() == 2)
			{
				// party disband
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				LPCHARACTER B = CHARACTER_MANAGER::instance().FindByPID(p->pid);
				if (B)
				{
					//pParty->SendPartyRemoveOneToAll(B);
					B->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티에서 추방당하셨습니다."));
					//pParty->Unlink(B);
					//CPartyManager::instance().SetPartyMember(B->GetPlayerID(), NULL);
				}
				pParty->Quit(p->pid);
			}
		}
	}
	else
	{
		// otherwise, only remove itself
		if (p->pid == ch->GetPlayerID())
		{
			if (ch->GetDungeon())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 던젼내에서는 파티를 나갈 수 없습니다."));
			}
			else
			{
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
					//CPartyManager::instance().SetPartyMember(ch->GetPlayerID(), NULL);
				}
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 다른 파티원을 탈퇴시킬 수 없습니다."));
		}
	}
}

void CInputMain::AnswerMakeGuild(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGAnswerMakeGuild* p = (TPacketCGAnswerMakeGuild*) c_pData;

	if (ch->GetGold() < 200000)
		return;

/* 	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_disband_time") <
			CGuildManager::instance().GetDisbandDelay())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 해산한 후 %d일 이내에는 길드를 만들 수 없습니다."),
				quest::CQuestManager::instance().GetEventFlag("guild_disband_delay"));
		return;
	}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_withdraw_time") <
			CGuildManager::instance().GetWithdrawDelay())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 탈퇴한 후 %d일 이내에는 길드를 만들 수 없습니다."),
				quest::CQuestManager::instance().GetEventFlag("guild_withdraw_delay"));
		return;
	} */

	if (ch->GetGuild())
		return;

	CGuildManager& gm = CGuildManager::instance();

	TGuildCreateParameter cp;
	memset(&cp, 0, sizeof(cp));

	cp.master = ch;
	strlcpy(cp.name, p->guild_name, sizeof(cp.name));

	if (cp.name[0] == 0 || !check_name(cp.name))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("적합하지 않은 길드 이름 입니다."));
		return;
	}

	DWORD dwGuildID = gm.CreateGuild(cp);

	if (dwGuildID)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> [%s] 길드가 생성되었습니다."), cp.name);

		int GuildCreateFee = 200000;

#ifdef ENABLE_GOLD_MAX_EXTENDED
		ch->ChangeGold(-GuildCreateFee);
#else
		ch->PointChange(POINT_GOLD, -GuildCreateFee);
#endif
		DBManager::instance().SendMoneyLog(MONEY_LOG_GUILD, ch->GetPlayerID(), -GuildCreateFee);

		char Log[128];
		snprintf(Log, sizeof(Log), "GUILD_NAME %s MASTER %s", cp.name, ch->GetName());
		LogManager::instance().CharLog(ch, 0, "MAKE_GUILD", Log);

		ch->RemoveSpecifyItem(GUILD_CREATE_ITEM_VNUM, 1);
		//ch->SendGuildName(dwGuildID);
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드 생성에 실패하였습니다."));
}

void CInputMain::PartyUseSkill(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGPartyUseSkill* p = (TPacketCGPartyUseSkill*) c_pData;
	if (!ch->GetParty())
		return;

	if (ch->GetPlayerID() != ch->GetParty()->GetLeaderPID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 파티 기술은 파티장만 사용할 수 있습니다."));
		return;
	}

	switch (p->bySkillIndex)
	{
		case PARTY_SKILL_HEAL:
			ch->GetParty()->HealParty();
			break;
		case PARTY_SKILL_WARP:
			{
				LPCHARACTER pch = CHARACTER_MANAGER::instance().Find(p->vid);
				if (pch)
					ch->GetParty()->SummonToLeader(pch->GetPlayerID());
				else
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티> 소환하려는 대상을 찾을 수 없습니다."));
			}
			break;
	}
}

void CInputMain::PartyParameter(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGPartyParameter * p = (TPacketCGPartyParameter *) c_pData;

	if (ch->GetParty())
		ch->GetParty()->SetParameter(p->bDistributeMode);
}

size_t GetSubPacketSize(const GUILD_SUBHEADER_CG& header)
{
	switch (header)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_ADD_MEMBER:					return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:			return 10;
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:		return sizeof(BYTE) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_OFFER:						return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHARGE_GSP:					return sizeof(int);
		case GUILD_SUBHEADER_CG_POST_COMMENT:				return 1;
		case GUILD_SUBHEADER_CG_DELETE_COMMENT:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:			return 0;
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_USE_SKILL:					return sizeof(TPacketCGGuildUseSkill);
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:		return sizeof(DWORD) + sizeof(BYTE);
	}

	return 0;
}

int CInputMain::Guild(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGGuild))
		return -1;

	const TPacketCGGuild* p = reinterpret_cast<const TPacketCGGuild*>(data);
	const char* c_pData = data + sizeof(TPacketCGGuild);

	uiBytes -= sizeof(TPacketCGGuild);

	const GUILD_SUBHEADER_CG SubHeader = static_cast<GUILD_SUBHEADER_CG>(p->subheader);
	const size_t SubPacketLen = GetSubPacketSize(SubHeader);

	if (uiBytes < SubPacketLen)
	{
		return -1;
	}

	CGuild* pGuild = ch->GetGuild();

	if (NULL == pGuild)
	{
		if (SubHeader != GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드에 속해있지 않습니다."));
			return SubPacketLen;
		}
	}

	switch (SubHeader)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:
			{
				return SubPacketLen;

#ifdef ENABLE_GOLD_MAX_EXTENDED
				const int gold = std::min<unsigned long long>(*reinterpret_cast<const int *>(c_pData), __deposit_limit());
#else
				const int gold = MIN(*reinterpret_cast<const int *>(c_pData), __deposit_limit());
#endif

				if (gold < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 잘못된 금액입니다."));
					return SubPacketLen;
				}

#ifdef ENABLE_GOLD_MAX_EXTENDED
				if (ch->GetGold() < static_cast<unsigned long long>(gold))
#else
				if (ch->GetGold() < gold)
#endif
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 가지고 있는 돈이 부족합니다."));
					return SubPacketLen;
				}

				pGuild->RequestDepositMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:
			{
				return SubPacketLen;

#ifdef ENABLE_GOLD_MAX_EXTENDED
				const long gold = std::min<long>(*reinterpret_cast<const long *>(c_pData), 500000);
#else
				const int gold = MIN(*reinterpret_cast<const int *>(c_pData), 500000);
#endif

				if (gold < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 잘못된 금액입니다."));
					return SubPacketLen;
				}

				pGuild->RequestWithdrawMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_ADD_MEMBER:
			{
				const DWORD vid = *reinterpret_cast<const DWORD*>(c_pData);
				LPCHARACTER newmember = CHARACTER_MANAGER::instance().Find(vid);

				if (!newmember)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 그러한 사람을 찾을 수 없습니다."));
					return SubPacketLen;
				}

				// @fixme145 BEGIN (+newmember ispc check)
				if (!ch->IsPC() || !newmember->IsPC())
					return SubPacketLen;
#ifdef ENABLE_GM_BLOCK
				if (ch->IsPC() == true && newmember->IsGM() == true)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_GUILD"));
					return SubPacketLen;
				}
				
				if (ch->IsGM() == true && newmember->IsPC() == true)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_GUILD_TEAM"));
					return SubPacketLen;
				}
#endif
				// @fixme145 END

				pGuild->Invite(ch, newmember);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:
			{
				if (pGuild->UnderAnyWar() != 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전 중에는 길드원을 탈퇴시킬 수 없습니다."));
					return SubPacketLen;
				}

				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				LPCHARACTER member = CHARACTER_MANAGER::instance().FindByPID(pid);

				if (member)
				{
					if (member->GetGuild() != pGuild)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 상대방이 같은 길드가 아닙니다."));
						return SubPacketLen;
					}

					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드원을 강제 탈퇴 시킬 권한이 없습니다."));
						return SubPacketLen;
					}

					member->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
					pGuild->RequestRemoveMember(member->GetPlayerID());

					if (g_bGuildInviteLimit)
					{
						DBManager::instance().Query("REPLACE INTO guild_invite_limit VALUES(%d, %d)", pGuild->GetID(), get_global_time());
					}
				}
				else
				{
					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드원을 강제 탈퇴 시킬 권한이 없습니다."));
						return SubPacketLen;
					}

					if (pGuild->RequestRemoveMember(pid))
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드원을 강제 탈퇴 시켰습니다."));
					else
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 그러한 사람을 찾을 수 없습니다."));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:
			{
				char gradename[GUILD_GRADE_NAME_MAX_LEN + 1];
				strlcpy(gradename, c_pData + 1, sizeof(gradename));

				const TGuildMember * m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 직위 이름을 변경할 권한이 없습니다."));
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드장의 직위 이름은 변경할 수 없습니다."));
				}
				else if (!check_name(gradename))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 적합하지 않은 직위 이름 입니다."));
				}
				else
				{
					pGuild->ChangeGradeName(*c_pData, gradename);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:
			{
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 직위 권한을 변경할 권한이 없습니다."));
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드장의 권한은 변경할 수 없습니다."));
				}
				else
				{
					pGuild->ChangeGradeAuth(*c_pData, *(c_pData + 1));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_OFFER:
			{
				DWORD offer = *reinterpret_cast<const DWORD*>(c_pData);

				if (pGuild->GetLevel() >= GUILD_MAX_LEVEL)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드가 이미 최고 레벨입니다."));
				}
				else
				{
					offer /= 100;
					offer *= 100;

					if (pGuild->OfferExp(ch, offer))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> %u의 경험치를 투자하였습니다."), offer);
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 경험치 투자에 실패하였습니다."));
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHARGE_GSP:
			{
				const int offer = *reinterpret_cast<const int*>(c_pData);
				const int gold = offer * 100;

				if (offer < 0 || gold < offer || gold < 0 || ch->GetGold() < gold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 돈이 부족합니다."));
					return SubPacketLen;
				}

				if (!pGuild->ChargeSP(ch, offer))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 용신력 회복에 실패하였습니다."));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_POST_COMMENT:
			{
				const size_t length = *c_pData;

				if (length > GUILD_COMMENT_MAX_LEN)
				{
					sys_err("POST_COMMENT: %s comment too long (length: %u)", ch->GetName(), length);
					ch->GetDesc()->SetPhase(PHASE_CLOSE);
					return -1;
				}

				if (uiBytes < 1 + length)
					return -1;

				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (length && !pGuild->HasGradeAuth(m->grade, GUILD_AUTH_NOTICE) && *(c_pData + 1) == '!')
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 공지글을 작성할 권한이 없습니다."));
				}
				else
				{
					std::string str(c_pData + 1, length);
					pGuild->AddComment(ch, str);
				}

				return (1 + length);
			}

		case GUILD_SUBHEADER_CG_DELETE_COMMENT:
			{
				const DWORD comment_id = *reinterpret_cast<const DWORD*>(c_pData);

				pGuild->DeleteComment(ch, comment_id);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:
			pGuild->RefreshComment(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE grade = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 직위를 변경할 권한이 없습니다."));
				else if (ch->GetPlayerID() == pid)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드장의 직위는 변경할 수 없습니다."));
				else if (grade == 1)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드장으로 직위를 변경할 수 없습니다."));
				else
					pGuild->ChangeMemberGrade(pid, grade);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_USE_SKILL:
			{
				const TPacketCGGuildUseSkill* p = reinterpret_cast<const TPacketCGGuildUseSkill*>(c_pData);

				pGuild->UseSkill(p->dwVnum, ch, p->dwPID);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE is_general = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 장군을 지정할 권한이 없습니다."));
				}
				else
				{
					if (!pGuild->ChangeMemberGeneral(pid, is_general))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 더이상 장수를 지정할 수 없습니다."));
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:
			{
				const DWORD guild_id = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE accept = *(c_pData + sizeof(DWORD));

				CGuild * g = CGuildManager::instance().FindGuild(guild_id);

				if (g)
				{
					if (accept)
						g->InviteAccept(ch);
					else
						g->InviteDeny(ch->GetPlayerID());
				}
			}
			return SubPacketLen;

	}

	return 0;
}

void CInputMain::Fishing(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFishing* p = (TPacketCGFishing*)c_pData;
	ch->SetRotation(p->dir * 5);
	ch->fishing();
	return;
}

void CInputMain::ItemGive(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGGiveItem* p = (TPacketCGGiveItem*) c_pData;
	LPCHARACTER to_ch = CHARACTER_MANAGER::instance().Find(p->dwTargetVID);

	if (to_ch)
		ch->GiveItem(to_ch, p->ItemPos);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템을 건네줄 수 없습니다."));
}

void CInputMain::Hack(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGHack* p = (TPacketCGHack*)c_pData;

	char buf[sizeof(p->szBuf)];
	strlcpy(buf, p->szBuf, sizeof(buf));

	sys_err("HACK_DETECT: %s %s", ch->GetName(), buf);

	ch->GetDesc()->SetPhase(PHASE_CLOSE);
}

int CInputMain::MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyShop * p = (TPacketCGMyShop *) c_pData;
	int iExtraLen = p->bCount * sizeof(TShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyShop) + iExtraLen)
		return -1;

#ifdef ENABLE_GOLD_MAX_EXTENDED
	if (ch->GetGold() >= GOLD_MAX_MAX)
#else
	if (ch->GetGold() >= GOLD_MAX)
#endif
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소유 돈이 20억냥을 넘어 거래를 핼수가 없습니다."));
		sys_log(0, "MyShop ==> OverFlow Gold id %u name %s ", ch->GetPlayerID(), ch->GetName());
		return (iExtraLen);
	}

	if (ch->IsStun() || ch->IsDead())
		return (iExtraLen);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen()
#ifdef __PREMIUM_PRIVATE_SHOP__
		|| ch->IsEditingPrivateShop() || ch->IsShopSearch() || ch->GetViewingPrivateShop() 
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		|| ch->IsAcceOpened()
#endif
#if defined(__BL_67_ATTR__)
		|| ch->Is67AttrOpen()
#endif
		|| ch->IsOpenGuildStorage()
	)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 거래중일경우 개인상점을 열수가 없습니다."));
		return (iExtraLen);
	}

	sys_log(0, "MyShop count %d", p->bCount);
	ch->OpenMyShop(p->szSign, (TShopItemTable *) (c_pData + sizeof(TPacketCGMyShop)), p->bCount);
	return (iExtraLen);
}

void CInputMain::Refine(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGRefine* p = reinterpret_cast<const TPacketCGRefine*>(c_pData);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen()
#ifdef __PREMIUM_PRIVATE_SHOP__
						|| ch->IsEditingPrivateShop() || ch->IsShopSearch() || ch->GetViewingPrivateShop()
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
						|| ch->IsAcceOpened()
#endif
#if defined(__BL_67_ATTR__)
						|| ch->Is67AttrOpen()
#endif
						|| ch->IsOpenGuildStorage()
	)
	{
		ch->ChatPacket(CHAT_TYPE_INFO,  LC_TEXT("창고,거래창등이 열린 상태에서는 개량을 할수가 없습니다"));
		ch->ClearRefineMode();
		return;
	}

	if (p->type == 255)
	{
		// DoRefine Cancel
		ch->ClearRefineMode();
		return;
	}

	if (p->pos >= INVENTORY_MAX_NUM)
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		if (!(p->pos >= SPECIAL_INVENTORY_SLOT_START && p->pos < SPECIAL_INVENTORY_SLOT_END))
		{
			ch->ClearRefineMode();
			return;
		}
#else
		ch->ClearRefineMode();
		return;
#endif
	}

	LPITEM item = ch->GetInventoryItem(p->pos);

	if (!item)
	{
		ch->ClearRefineMode();
		return;
	}

	ch->SetRefineTime();

	if (p->type == REFINE_TYPE_NORMAL)
	{
		sys_log (0, "refine_type_noraml");
		ch->DoRefine(item);
	}
	else if (p->type == REFINE_TYPE_SCROLL || p->type == REFINE_TYPE_HYUNIRON || p->type == REFINE_TYPE_MUSIN || p->type == REFINE_TYPE_BDRAGON)
	{
		sys_log (0, "refine_type_scroll, ...");
		ch->DoRefineWithScroll(item);
	}
	else if (p->type == REFINE_TYPE_MONEY_ONLY)
	{
		const LPITEM item = ch->GetInventoryItem(p->pos);

		if (NULL != item)
		{
			if (500 <= item->GetRefineSet())
			{
				LogManager::instance().HackLog("DEVIL_TOWER_REFINE_HACK", ch);
			}
			else
			{
				if (ch->GetQuestFlag("deviltower_zone.can_refine") > 0) // @fixme158 (allow multiple refine attempts)
				{
					if (ch->DoRefine(item, true))
						ch->SetQuestFlag("deviltower_zone.can_refine", ch->GetQuestFlag("deviltower_zone.can_refine") - 1);
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "사귀 타워 완료 보상은 한번까지 사용가능합니다.");
				}
			}
		}
	}

	ch->ClearRefineMode();
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CInputMain::Acce(LPCHARACTER pkChar, const char* c_pData)
{
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning())
		return;

	TPacketAcce * sPacket = (TPacketAcce*) c_pData;
	switch (sPacket->subheader)
	{
	case ACCE_SUBHEADER_CG_CLOSE:
	{
		pkChar->CloseAcce();
	}
	break;
	case ACCE_SUBHEADER_CG_ADD:
	{
		pkChar->AddAcceMaterial(sPacket->tPos, sPacket->bPos);
	}
	break;
	case ACCE_SUBHEADER_CG_REMOVE:
	{
		pkChar->RemoveAcceMaterial(sPacket->bPos);
	}
	break;
	case ACCE_SUBHEADER_CG_REFINE:
	{
		pkChar->RefineAcceMaterials();
	}
	break;
	default:
		break;
	}
}
#endif

#ifdef ENABLE_SWITCHBOT_SYSTEM
int CInputMain::Switchbot(LPCHARACTER ch, const char *data, size_t uiBytes)
{
	const TPacketCGSwitchbot *p = reinterpret_cast<const TPacketCGSwitchbot *>(data);
	if (uiBytes < sizeof(TPacketCGSwitchbot))
	{
		return -1;
	}

	const char *c_pData = data + sizeof(TPacketCGSwitchbot);
	uiBytes -= sizeof(TPacketCGSwitchbot);

	switch (p->subheader)
	{
	case SUBHEADER_CG_SWITCHBOT_START:
	{
		size_t extraLen = sizeof(TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
		if (uiBytes < extraLen)
		{
			return -1;
		}

		std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives;

		for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
		{
			const TSwitchbotAttributeAlternativeTable *pAttr = reinterpret_cast<const TSwitchbotAttributeAlternativeTable *>(c_pData);
			c_pData += sizeof(TSwitchbotAttributeAlternativeTable);

			vec_alternatives.emplace_back(*pAttr);
		}

		CSwitchbotManager::Instance().Start(ch->GetPlayerID(), p->slot, vec_alternatives);
		return extraLen;
	}

	case SUBHEADER_CG_SWITCHBOT_STOP:
	{
		CSwitchbotManager::Instance().Stop(ch->GetPlayerID(), p->slot);
		return 0;
	}
	}

	return 0;
}
#endif

#ifdef ENABLE_DROP_INFO
void CInputMain::DropInfo(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
	{
		return;
	}

	const auto pkTarget = ch->GetTarget();

	if (!pkTarget)
	{
		return;
	}

	if (pkTarget->IsMonster() || pkTarget->IsStone())
	{
		std::vector<TDropInfoItem> vecDropInfo;
		const auto bHasDropInfo = ITEM_MANAGER::Instance().CreateDropItemInfo(pkTarget, ch, vecDropInfo);

		if (bHasDropInfo)
		{
			TEMP_BUFFER buf;

			for (const auto &it : vecDropInfo)
			{
				TPacketDropInfoItem item;
				item.dwVnum = it.dwVnum;
				item.byMinCount = it.byMinCount;
				item.byMaxCount = it.byMaxCount;
				buf.write(&it, sizeof(TPacketDropInfoItem));
			}

			TPacketDropInfo packet;
			packet.header = HEADER_GC_DROP_INFO;
			packet.size = sizeof(TPacketDropInfo) + buf.size();
			packet.item_count = vecDropInfo.size();

			const auto &tTable = pkTarget->GetMobTable();
			packet.gold_min = tTable.dwGoldMin;
			packet.gold_max = tTable.dwGoldMax;

			packet.mob_vnum = pkTarget->GetRaceNum();

			ch->GetDesc()->BufferedPacket(&packet, sizeof(TPacketDropInfo));
			ch->GetDesc()->Packet(buf.read_peek(), buf.size());
		}
	}
}
#endif

#ifdef ENABLE_EVENT_MANAGER
void CInputMain::RequestEventQuest(LPCHARACTER ch, const void *c_pData)
{
	TPacketCGRequestEventQuest *p = (TPacketCGRequestEventQuest *)c_pData;

	if (ch && ch->GetDesc())
	{
		quest::CQuestManager::instance().RequestEventQuest(p->szName, ch->GetPlayerID());
	}
}

void CInputMain::RequestEventData(LPCHARACTER ch, const char *c_pData)
{
	if (ch && ch->GetDesc())
	{
		const TPacketCGRequestEventData *p = reinterpret_cast<const TPacketCGRequestEventData *>(c_pData);
		CEventManager::Instance().SendEventInfo(ch, p->bMonth);
	}
}
#endif


#ifdef ENABLE_INGAME_WIKI
void CInputMain::RecvWikiPacket(LPCHARACTER ch, const char *c_pData)
{
	if (!ch || (ch && !ch->GetDesc()))
	{
		return;
	}

	if (!c_pData)
	{
		return;
	}

	InGameWiki::TCGWikiPacket *p = nullptr;
	if (!(p = (InGameWiki::TCGWikiPacket *)c_pData))
	{
		return;
	}

	InGameWiki::TGCWikiPacket pack;
	pack.set_data_type(!p->is_mob ? InGameWiki::LOAD_WIKI_ITEM : InGameWiki::LOAD_WIKI_MOB);
	pack.increment_data_size(WORD(sizeof(InGameWiki::TGCWikiPacket)));

	if (pack.is_data_type(InGameWiki::LOAD_WIKI_ITEM))
	{
		const std::vector<CommonWikiData::TWikiItemOriginInfo> &originVec = ITEM_MANAGER::Instance().GetItemOrigin(p->vnum);
		const std::vector<CSpecialItemGroup::CSpecialItemInfo> _gV = ITEM_MANAGER::instance().GetWikiChestInfo(p->vnum);
		const std::vector<CommonWikiData::TWikiRefineInfo> _rV = ITEM_MANAGER::instance().GetWikiRefineInfo(p->vnum);
		const CommonWikiData::TWikiInfoTable *_wif = ITEM_MANAGER::instance().GetItemWikiInfo(p->vnum);

		if (!_wif)
		{
			return;
		}

		const size_t origin_size = originVec.size();
		const size_t chest_info_count = _wif->chest_info_count;
		const size_t refine_infos_count = _wif->refine_infos_count;
		const size_t buf_data_dize = sizeof(InGameWiki::TGCItemWikiPacket) +
									 (origin_size * sizeof(CommonWikiData::TWikiItemOriginInfo)) +
									 (chest_info_count * sizeof(CommonWikiData::TWikiChestInfo)) +
									 (refine_infos_count * sizeof(CommonWikiData::TWikiRefineInfo));

		if (chest_info_count != _gV.size())
		{
			// sys_err("Item Vnum : %d || ERROR TYPE -> 1", p->vnum);
			return;
		}

		if (refine_infos_count != _rV.size())
		{
			// sys_err("Item Vnum : %d || ERROR TYPE -> 2", p->vnum);
			return;
		}

		pack.increment_data_size(WORD(buf_data_dize));

		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(InGameWiki::TGCWikiPacket));

		InGameWiki::TGCItemWikiPacket data_packet;
		data_packet.mutable_wiki_info(*_wif);
		data_packet.set_origin_infos_count(origin_size);
		data_packet.set_vnum(p->vnum);
		data_packet.set_ret_id(p->ret_id);
		buf.write(&data_packet, sizeof(data_packet));

		{
			if (origin_size)
				for (int idx = 0; idx < (int)origin_size; ++idx)
				{
					buf.write(&(originVec[idx]), sizeof(CommonWikiData::TWikiItemOriginInfo));
				}

			if (chest_info_count > 0)
			{
				for (int idx = 0; idx < (int)chest_info_count; ++idx)
				{
					CommonWikiData::TWikiChestInfo write_struct(_gV[idx].vnum, _gV[idx].count);
					buf.write(&write_struct, sizeof(CommonWikiData::TWikiChestInfo));
				}
			}

			if (refine_infos_count > 0)
				for (int idx = 0; idx < (int)refine_infos_count; ++idx)
				{
					buf.write(&(_rV[idx]), sizeof(CommonWikiData::TWikiRefineInfo));
				}
		}

		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
	else
	{
		CMobManager::TMobWikiInfoVector &mobVec = CMobManager::instance().GetMobWikiInfo(p->vnum);
		const size_t _mobVec_size = mobVec.size();

		if (!_mobVec_size)
		{
			if (test_server)
			{
				sys_log(0, "Mob Vnum: %d : || LOG TYPE -> 1", p->vnum);
			}

			return;
		}

		const size_t buf_data_dize = (sizeof(InGameWiki::TGCMobWikiPacket) + (_mobVec_size * sizeof(CommonWikiData::TWikiMobDropInfo)));
		pack.increment_data_size(WORD(buf_data_dize));

		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(InGameWiki::TGCWikiPacket));

		InGameWiki::TGCMobWikiPacket data_packet;
		data_packet.set_drop_info_count(_mobVec_size);
		data_packet.set_vnum(p->vnum);
		data_packet.set_ret_id(p->ret_id);
		buf.write(&data_packet, sizeof(InGameWiki::TGCMobWikiPacket));

		{
			if (_mobVec_size)
			{
				for (int idx = 0; idx < (int)_mobVec_size; ++idx)
				{
					CommonWikiData::TWikiMobDropInfo write_struct(mobVec[idx].vnum, mobVec[idx].count);
					buf.write(&write_struct, sizeof(CommonWikiData::TWikiMobDropInfo));
				}
			}
		}

		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}
#endif

#ifdef ENABLE_GEM_SYSTEM
int CInputMain::GemShop(LPCHARACTER ch, const char *data, size_t uiBytes)
{
	TPacketCGGemShop *p = (TPacketCGGemShop *)data;

	if (uiBytes < sizeof(TPacketCGGemShop))
	{
		return -1;
	}

	const char *c_pData = data + sizeof(TPacketCGGemShop);
	uiBytes -= sizeof(TPacketCGGemShop);

	switch (p->subheader)
	{
	case GEM_SHOP_SUBHEADER_CG_BUY:
	{
		if (uiBytes < sizeof(BYTE))
		{
			return -1;
		}

		BYTE bPos = *c_pData;

		sys_log(1, "INPUT: %s GEM_SHOP: BUY %d", ch->GetName(), bPos);
		ch->GemShopBuy(bPos);
		return sizeof(BYTE);
	}
	default:
		sys_err("CInputMain::GemShop() : Unknown subheader %d : %s", p->subheader, ch->GetName());
		break;
	}

	return 0;
}
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
int CInputMain::BiologManager(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	if (!ch)
	{
		return -1;
	}

	TPacketCGBiologManagerAction* p = (TPacketCGBiologManagerAction*)c_pData;
	c_pData += sizeof(TPacketCGBiologManagerAction);

	CBiologSystem* pkBiologManager = ch->GetBiologManager();
	if (!pkBiologManager)
	{
		return -1;
	}

	return pkBiologManager->RecvClientPacket(p->bSubHeader, c_pData, uiBytes);
}
#endif

#ifdef ENABLE_SKILL_GROUP_GUI
void CInputMain::SkillGroup(LPCHARACTER ch, const char *c_pData)
{
	TPacketCGSkillGroup *packet = (TPacketCGSkillGroup *)c_pData;
	ch->SetSkillGroup(packet->bGroup);
	ch->ClearSkill();
#ifdef ENABLE_MASTER_ALL_SKILLS
	ch->MasterAllSkills(ch);
#endif
}
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
int CInputMain::PrivateShopBuild(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopBuild* p = (TPacketCGPrivateShopBuild*)c_pData;
	c_pData += sizeof(TPacketCGPrivateShopBuild);

	// Size of the remaining packet
	int iExtraLen = sizeof(TPacketCGPrivateShopBuild) + p->wItemCount * sizeof(TPrivateShopItem);

	// No items have been passed
	if (uiBytes < (sizeof(TPacketCGPrivateShop) + iExtraLen))
		return -1;

	if (!ch || !ch->GetDesc())
		return iExtraLen;

	if (ch && ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!check_allow(ch->GetGMLevel(), GM_ALLOW_CREATE_PRIVATE_SHOP))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this with this gamemaster rank."));
		return iExtraLen;
	}

#ifdef ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS
	if (!CanBuildPrivateShop(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a personal shop while another window is open."));
		return iExtraLen;
	}
#endif

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU HAVE BLOCK STATUS, OPEN A SUPPORT TICKET"));
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopBuildTime() < PASSES_PER_SEC(10))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a few moments before building your personal shop again."));
		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a personal shop while another window is open."));
		return iExtraLen;
	}

	ch->BuildPrivateShop(p->szTitle, p->dwPolyVnum, p->bTitleType, p->bPageCount, p->wItemCount, (TPrivateShopItem*)c_pData);
	ch->SetLastPrivateShopBuildTime();

	return iExtraLen;
}

void CInputMain::PrivateShopClose(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return;

	if (ch && ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return;
	}

	if (ch->IsStun() || ch->IsDead())
		return;

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot close a personal shop while another window is open."));
		return;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have an open personal shop."));
		return;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopCloseTime() < PASSES_PER_SEC(10))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a few moments before closing your personal shop again."));
		return;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_CLOSE;
	DWORD dwPID = ch->GetPlayerID();

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(DWORD));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&dwPID, sizeof(DWORD));

	ch->SetLastPrivateShopCloseTime();
}

void CInputMain::PrivateShopPanelOpen(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return;

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a personal shop while another window is open."));
		return;
	}

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsEditingPrivateShop())
		return;

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU HAVE BLOCK STATUS, OPEN A SUPPORT TICKET"));
		return;
	}

	ch->OpenPrivateShopPanel();
}

void CInputMain::PrivateShopPanelClose(LPCHARACTER ch)
{
	if (!ch)
		return;

	if (ch->IsStun() || ch->IsDead())
		return;

	ch->ClosePrivateShopPanel();

	// Set shop's state back to opened if it was left out in the modify state
	if (ch->CanModifyPrivateShop())
	{
		BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_MODIFY_REQUEST;
		DWORD dwPID = ch->GetPlayerID();
		db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(DWORD));
		db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
		db_clientdesc->Packet(&dwPID, sizeof(DWORD));
	}
}

int CInputMain::PrivateShopStart(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	const DWORD dwVID = *reinterpret_cast<const DWORD*>(c_pData);
	int iExtraLen = sizeof(DWORD);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch || !ch->GetDesc())
		return iExtraLen;

	if (ch && ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));

		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot view a personal shop while having other trading windows open."));
		return iExtraLen;
	}

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU HAVE BLOCK STATUS, OPEN A SUPPORT TICKET"));
		return iExtraLen;
	}

	LPPRIVATE_SHOP pPrivateShop = CPrivateShopManager::Instance().GetPrivateShopByVID(dwVID);
	if (!pPrivateShop)
		return iExtraLen;

	if (pPrivateShop->GetID() == ch->GetPlayerID())
	{
		if (!ch->IsEditingPrivateShop())
			ch->OpenPrivateShopPanel();

		return iExtraLen;
	}

	if (pPrivateShop == ch->GetViewingPrivateShop())
		return iExtraLen;

	if (ch->IsEditingPrivateShop())
		ch->ClosePrivateShopPanel();

	if (ch->GetViewingPrivateShop())
		ch->GetViewingPrivateShop()->RemoveShopViewer(ch);

	pPrivateShop->AddShopViewer(ch);

	return iExtraLen;
}

void CInputMain::PrivateShopEnd(LPCHARACTER ch)
{
	if (!ch)
		return;

	CPrivateShopManager::Instance().StopShopping(ch);
}

int CInputMain::PrivateShopBuy(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	const WORD wPos = *reinterpret_cast<const WORD*>(c_pData);
	int iExtraLen = sizeof(WORD);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch || !ch->GetDesc())
		return iExtraLen;

	if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		return iExtraLen;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopBuyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a moment before buying from a personal shop again."));
		return iExtraLen;
	}

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU HAVE BLOCK STATUS, OPEN A SUPPORT TICKET"));
		return iExtraLen;
	}

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->GetViewingPrivateShop())
		return iExtraLen;

	if (!ch->GetViewingPrivateShop()->GetItem(wPos))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot buy an item from your own personal shop."));
		return iExtraLen;
	}

	if (!check_allow(ch->GetGMLevel(), GM_ALLOW_BUY_PRIVATE_ITEM))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this with this gamemaster rank."));
		return iExtraLen;
	}
	
	// Packet to db
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_BUY_REQUEST;

	TPacketGDPrivateShopBuyRequest subPacket{};
	subPacket.dwCustomerPID = ch->GetPlayerID();
	subPacket.llGoldBalance = ch->GetGold();
#ifdef ENABLE_PRIVATE_SHOP_CHEQUE
	subPacket.dwChequeBalance = ch->GetCheque();
#else
	subPacket.dwChequeBalance = 0;
#endif
	subPacket.aSelectedItems[0].dwShopID = ch->GetViewingPrivateShop()->GetID();
	subPacket.aSelectedItems[0].wPos = wPos;
	subPacket.aSelectedItems[0].TPrice.llGold = ch->GetViewingPrivateShop()->GetItem(wPos)->GetGoldPrice();
	subPacket.aSelectedItems[0].TPrice.dwCheque = ch->GetViewingPrivateShop()->GetItem(wPos)->GetChequePrice();

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopBuyRequest));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopBuyRequest));

	ch->SetLastPrivateShopBuyTime();

	return iExtraLen;
}

void CInputMain::PrivateShopWithdraw(LPCHARACTER ch)
{
	if (!ch)
		return;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return;
	}

	if (ch->IsStun() || ch->IsDead())
		return;

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have an open personal shop."));
		return;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopWithdrawTime() < PASSES_PER_SEC(10))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a few moments before withdrawing your personal shop again."));
		return;
	}

#ifdef ENABLE_GOLD_MAX_EXTENDED
	if ((ch->GetPrivateShopTable()->llGold + ch->GetGold()) >= GOLD_MAX_MAX)
#else
	if ((ch->GetPrivateShopTable()->llGold + ch->GetGold()) >= GOLD_MAX)
#endif
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot exchange as you would exceed the maximum amount of Yang."));
		return;
	}

#ifdef ENABLE_PRIVATE_SHOP_CHEQUE
	if ((ch->GetPrivateShopTable()->dwCheque + ch->GetCheque()) > CHEQUE_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot exchange as you would exceed the maximum amount of Won."));
		return;
	}
#endif

	if (!ch->GetPrivateShopTable()->llGold && !ch->GetPrivateShopTable()->dwCheque)
		return;

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_WITHDRAW_REQUEST;
	DWORD dwPID = ch->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(DWORD));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&dwPID, sizeof(DWORD));

	ch->SetLastPrivateShopWithdrawTime();
}

void CInputMain::PrivateShopModify(LPCHARACTER ch)
{
	if (!ch)
		return;

	if (ch->IsStun() || ch->IsDead())
		return;

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You do not have an open private shop.");
		return;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopStateChangeTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a moment before changing state of your personal shop again."));
		return;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_MODIFY_REQUEST;
	DWORD dwPID = ch->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(DWORD));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&dwPID, sizeof(DWORD));

	ch->SetLastPrivateShopStateChangeTime();
}

int CInputMain::PrivateShopItemPriceChange(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopItemPriceChange* p = (TPacketCGPrivateShopItemPriceChange*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopItemPriceChange);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot modify a personal shop while having other trading windows open."));
		return iExtraLen;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have an open personal shop."));
		return iExtraLen;
	}

    if ((ch->GetMapIndex() >= 1000 || ch->GetDungeon()))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot edit a personal shop in a dungeon."));
        return iExtraLen;
    }

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot manage personal shop's content while it is not in a modifying state."));
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a moment before editing your personal shop's content again."));
		return iExtraLen;
	}

	const TPlayerPrivateShopItem* pPrivateShopItem = ch->GetPrivateShopItem(p->wPos);
	if (!pPrivateShopItem)
		return iExtraLen;

#ifdef ENABLE_GOLD_MAX_EXTENDED
	if ((ch->GetPrivateShopTotalGold() + ch->GetGold() - pPrivateShopItem->TPrice.llGold + p->TPrice.llGold) >= GOLD_MAX_MAX)
#else
	if ((ch->GetPrivateShopTotalGold() + ch->GetGold() - pPrivateShopItem->TPrice.llGold + p->TPrice.llGold) >= GOLD_MAX)
#endif
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The items you put up for sale must not exceed the permitted total value."));
		return iExtraLen;
	}

#ifdef ENABLE_PRIVATE_SHOP_CHEQUE
	if ((ch->GetPrivateShopTotalCheque() + ch->GetCheque() - pPrivateShopItem->TPrice.dwCheque + p->TPrice.dwCheque) > CHEQUE_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The items you put up for sale must not exceed the permitted total value."));
		return iExtraLen;
	}
#endif

	if (p->TPrice.llGold < 0)
	{
		sys_err("Player %u is trying to negatively manipulate price of the item", ch->GetPlayerID());
		return iExtraLen;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_ITEM_PRICE_CHANGE_REQUEST;

	TPacketPrivateShopItemPriceChange subPacket{};
	subPacket.dwShopID = ch->GetPlayerID();
	subPacket.wPos = p->wPos;
	subPacket.TPrice.llGold = p->TPrice.llGold;
	subPacket.TPrice.dwCheque = p->TPrice.dwCheque;

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketPrivateShopItemPriceChange));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketPrivateShopItemPriceChange));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

int CInputMain::PrivateShopItemMove(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopItemMove* p = (TPacketCGPrivateShopItemMove*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopItemMove);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->CanHandleItem())
		return iExtraLen;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot modify a personal shop while having other trading windows open."));
		return iExtraLen;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have an open personal shop."));
		return iExtraLen;
	}

    if ((ch->GetMapIndex() >= 1000 || ch->GetDungeon()))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot edit a personal shop in a dungeon."));
        return iExtraLen;
    }

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot manage personal shop's content while it is not in a modifying state."));
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a moment before editing your personal shop's content again."));
		return iExtraLen;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_ITEM_MOVE_REQUEST;

	TPacketPrivateShopItemMove subPacket{};
	subPacket.dwShopID = ch->GetPlayerID();
	subPacket.wPos = p->wPos;
	subPacket.wChangePos = p->wChangePos;

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketPrivateShopItemMove));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketPrivateShopItemMove));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

int CInputMain::PrivateShopItemCheckin(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopItemCheckin* p = (TPacketCGPrivateShopItemCheckin*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopItemCheckin);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->CanHandleItem())
		return iExtraLen;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot modify a personal shop while having other trading windows open."));
		return iExtraLen;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have an open personal shop."));
		return iExtraLen;
	}

    if ((ch->GetMapIndex() >= 1000 || ch->GetDungeon()))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot edit a personal shop in a dungeon."));
        return iExtraLen;
    }

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot manage personal shop's content while it is not in a modifying state."));
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a moment before editing your personal shop's content again."));
		return iExtraLen;
	}

#ifdef ENABLE_GOLD_MAX_EXTENDED
	if ((ch->GetPrivateShopTotalGold() + ch->GetGold() + p->llGold) >= GOLD_MAX_MAX)
#else
	if ((ch->GetPrivateShopTotalGold() + ch->GetGold() + p->llGold) >= GOLD_MAX)
#endif
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The items you put up for sale must not exceed the permitted total value."));
		return iExtraLen;
	}

#ifdef ENABLE_PRIVATE_SHOP_CHEQUE
	if ((ch->GetPrivateShopTotalCheque() + ch->GetCheque() + p->dwCheque) > CHEQUE_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The items you put up for sale must not exceed the permitted total value."));
		return iExtraLen;
	}
#endif

	if (p->llGold < 0)
	{
		sys_err("Player %u is trying to add an item with negative price", ch->GetPlayerID());
		return iExtraLen;
	}

	LPITEM pItem = ch->GetItem(p->TSrcPos);
	if (!pItem)
		return iExtraLen;

	if (!pItem->GetOwner() || ch != pItem->GetOwner())
	{
		sys_err("Player %u tried to add item %u that is not bound to him", ch->GetPlayerID(), pItem->GetID());
		return iExtraLen;
	}

	const TItemTable* pItemTable = pItem->GetProto();
	if (!pItemTable)
	{
		sys_err("Could not find an item table for an item at position: %d vnum: %d", p->TSrcPos, pItem->GetVnum());
		return false;
	}

	if (pItemTable && (IS_SET(pItemTable->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell Item-Shop items in a personal shop."));
		return false;
	}

	if (pItem->IsEquipped())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell equipped items in a personal shop."));
		return false;
	}

	if (pItem->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell locked items in a personal shop."));
		return false;
	}

	// Flush delayed item data as we don't want it flushed later on after (if) its saved in private shop
	ITEM_MANAGER::Instance().FlushDelayedSave(pItem);

	TPlayerPrivateShopItem t;
	t.dwID = pItem->GetID();
	t.wPos = 0;
	t.dwCount = pItem->GetCount();
	t.dwVnum = pItem->GetOriginalVnum();
	thecore_memcpy(t.alSockets, pItem->GetSockets(), sizeof(t.alSockets));
	thecore_memcpy(t.aAttr, pItem->GetAttributes(), sizeof(t.aAttr));
	t.TPrice.llGold = p->llGold;
	t.TPrice.dwCheque = p->dwCheque;
	t.dwOwner = pItem->GetOwner()->GetPlayerID();
	t.tCheckin = time(0);
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	t.dwTransmutationVnum = pItem->GetTransmutationVnum();
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	t.dwRefineElement = pItem->GetRefineElement();
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	thecore_memcpy(t.aApplyRandom, pItem->GetRandomApplies(), sizeof(t.aApplyRandom));
#endif

	TPacketGDPrivateShopItemCheckin subPacket{};
	subPacket.dwShopID = ch->GetPlayerID();
	subPacket.TItem = t;
	subPacket.iPos = p->iDstPos;

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKIN_REQUEST;
	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopItemCheckin));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopItemCheckin));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

int CInputMain::PrivateShopItemCheckout(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopItemCheckout* p = (TPacketCGPrivateShopItemCheckout*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopItemCheckout);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->CanHandleItem())
		return iExtraLen;

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot modify a personal shop while having other trading windows open."));
		return iExtraLen;
	}

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have an open personal shop."));
		return iExtraLen;
	}

    if ((ch->GetMapIndex() >= 1000 || ch->GetDungeon()))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot edit a personal shop in a dungeon."));
        return iExtraLen;
    }

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot manage personal shop's content while it is not in a modifying state."));
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a moment before editing your personal shop's content again."));
		return iExtraLen;
	}

	const TPlayerPrivateShopItem* c_pShopItem = ch->GetPrivateShopItem(p->wSrcPos);
	if (!c_pShopItem)
	{
		sys_err("Cannot find item on position %d pid %u", p->wSrcPos, ch->GetPlayerID());
		return iExtraLen;
	}

	const TItemTable* pItemTable = ITEM_MANAGER::Instance().GetTable(c_pShopItem->dwVnum);
	if (!pItemTable)
	{
		sys_err("Cannot find item table for item vnum %d", c_pShopItem->dwVnum);
		return iExtraLen;
	}

	BYTE bWindow = RESERVED_WINDOW;
	LPITEM pFakeItem = ITEM_MANAGER::Instance().CreateItem(c_pShopItem->dwVnum);
	int iPos = GetEmptyInventory(ch, pFakeItem);

	// Find a position if none was specified or the specified one is invalid
	if (pItemTable->bType == ITEM_DS)
	{
		// if (p->iDstPos < 0 || !ch->IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, p->iDstPos), pItemTable->bSize))
		if (p->iDstPos >= 0 || !ch->IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, p->iDstPos), pItemTable->bSize))
		{
			if (iPos < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough space in your inventory."));
				M2_DESTROY_ITEM(pFakeItem);
				return iExtraLen;
			}

			p->iDstPos = iPos;
		}

		bWindow = DRAGON_SOUL_INVENTORY;
	}
	else
	{
		// if (p->iDstPos >= 0)
		// {
		// 	if (pFakeItem->GetSpecialInventoryType() != TItemPos(INVENTORY, p->iDstPos).GetSpecialInventoryType())
		// 	{
		// 		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item here."));
		// 		M2_DESTROY_ITEM(pFakeItem);
		// 		return iExtraLen;
		// 	}
		// }

		if (p->iDstPos >= 0 || !ch->IsEmptySpecialItemGrid(TItemPos(INVENTORY, p->iDstPos), pItemTable->bSize))
		// if (p->iDstPos < 0 || !ch->IsEmptyItemGrid(TItemPos(INVENTORY, p->iDstPos), pItemTable->bSize))
		{
			if (iPos < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough space in your inventory."));
				M2_DESTROY_ITEM(pFakeItem);
				return iExtraLen;
			}

			p->iDstPos = iPos;
		}

		bWindow = INVENTORY;
	}

	M2_DESTROY_ITEM(pFakeItem);

	// Packet to db
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKOUT_REQUEST;

	TPacketGDPrivateShopItemCheckout subPacket{};
	subPacket.dwPID = ch->GetPlayerID();
	subPacket.wSrcPos = p->wSrcPos;
	subPacket.TDstPos.cell = p->iDstPos;
	subPacket.TDstPos.window_type = bWindow;
	subPacket.TItem = *c_pShopItem;

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopItemCheckout));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopItemCheckout));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

int CInputMain::PrivateShopTitleChange(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	const char* c_szTitle = c_pData;
	int iExtraLen = TITLE_MAX_LEN + 1;

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->IsPrivateShopOwner() || !ch->IsEditingPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have an open personal shop."));
		return iExtraLen;
	}

    if ((ch->GetMapIndex() >= 1000 || ch->GetDungeon()))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot edit a personal shop in a dungeon."));
        return iExtraLen;
    }

	if (!ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot manage personal shop's content while it is not in a modifying state."));
		return iExtraLen;
	}

	if (strlen(c_szTitle) < TITLE_MIN_LEN)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The entered name is too short."));
		return iExtraLen;
	}

	if (thecore_pulse() - ch->GetLastPrivateShopModifyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a moment before editing your personal shop's content again."));
		return iExtraLen;
	}

	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_TITLE_CHANGE_REQUEST;

	TPacketPrivateShopTitleChange subPacket{};
	subPacket.dwPID = ch->GetPlayerID();
	strncpy(subPacket.szTitle, c_szTitle, TITLE_MAX_LEN);

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketPrivateShopTitleChange));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketPrivateShopTitleChange));

	ch->SetLastPrivateShopModifyTime();

	return iExtraLen;
}

void CInputMain::PrivateShopWarpRequest(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return;

	if (ch->IsStun() || ch->IsDead())
		return;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return;
	}

	if (!ch->IsPrivateShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have an open personal shop."));
		return;
	}

	TPrivateShop* pTable = ch->GetPrivateShopTable();
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_WARP_REQUEST;

	TPacketGDPrivateShopWarpReq subPacket{};
	subPacket.dwPID = ch->GetPlayerID();
	subPacket.dwMapIndex = pTable->lMapIndex;
	subPacket.wListenPort = pTable->wPort;
	subPacket.bChannel = pTable->bChannel;

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopWarpReq));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopWarpReq));
}

int CInputMain::PrivateShopSlotUnlockRequest(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	const WORD wPos = *reinterpret_cast<const WORD*>(c_pData);

	int iExtraLen = sizeof(WORD);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch || !ch->GetDesc())
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (!ch->CanHandleItem())
		return iExtraLen;

	if (!CheckTradeWindows(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot modify a personal shop while having other trading windows open."));
		return iExtraLen;
	}

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&ch->m_pkTimedEvent);

		return iExtraLen;
	}

    if ((ch->GetMapIndex() >= 1000 || ch->GetDungeon()))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot edit a personal shop in a dungeon."));
        return iExtraLen;
    }

	if (ch->IsEditingPrivateShop() && !ch->CanModifyPrivateShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot manage personal shop's content while it is not in a modifying state."));
		return iExtraLen;
	}

	LPITEM pUnlockItem = ch->GetItem(TItemPos(INVENTORY, wPos));

	if (!pUnlockItem || pUnlockItem->GetVnum() != PRIVATE_SHOP_SLOT_UNLOCK_ITEM)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unlock a slot on your personal shop without Personal Shop Key."));
		return iExtraLen;
	}

	WORD wUnlockedSlots = ch->GetPoint(POINT_PRIVATE_SHOP_UNLOCKED_SLOT);
	int iAvailableSlots = PRIVATE_SHOP_LOCKED_SLOT_MAX_NUM - wUnlockedSlots;

	if (!iAvailableSlots)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unlock any more slots."));
		return iExtraLen;
	}

	// If player has an open shop, await a reply from the db
	if (ch->IsPrivateShopOwner())
	{
		BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_SLOT_UNLOCK_REQUEST;
		TPacketGDPrivateShopSlotUnlockReq subPacket{};
		subPacket.dwPID = ch->GetPlayerID();
		subPacket.wCount = pUnlockItem->GetValue(0);

		db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopSlotUnlockReq));
		db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
		db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopSlotUnlockReq));
	}
	else
	{
		// Prevent overflow
		WORD wIncreasedSlotCount = std::min<WORD>(pUnlockItem->GetValue(0), iAvailableSlots);
		ch->PointChange(POINT_PRIVATE_SHOP_UNLOCKED_SLOT, wIncreasedSlotCount, true);

		pUnlockItem->SetCount(pUnlockItem->GetCount() - 1);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have increased available space on your personal shop."));
	}

	return iExtraLen;
}

void CInputMain::PrivateShopSearchClose(LPCHARACTER ch)
{
	if (!ch)
		return;

	ch->CloseShopSearch();
}

int CInputMain::PrivateShopSearch(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopSearch* p = (TPacketCGPrivateShopSearch*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopSearch);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	if (thecore_pulse() - ch->GetLastPrivateShopSearchTime() < PASSES_PER_SEC(2))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a moment before searching other personal shops again."));
		return iExtraLen;
	}

	CPrivateShopManager::Instance().SearchItem(ch->GetDesc(), p->Filter, p->bUseFilter);

	TPacketGGPrivateShopItemSearch packet{};
	packet.bHeader = HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH;
	packet.dwCustomerID = ch->GetPlayerID();
	packet.dwCustomerPort = p2p_port;
	packet.bUseFilter = p->bUseFilter;
	memcpy(&packet.Filter, &p->Filter, sizeof(packet.Filter));

	P2P_MANAGER::Instance().Send(&packet, sizeof(TPacketGGPrivateShopItemSearch));

	ch->SetLastPrivateShopSearchTime();
	return iExtraLen;
}

int CInputMain::PrivateShopSearchBuy(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGPrivateShopSearchBuy* p = (TPacketCGPrivateShopSearchBuy*)c_pData;

	int iExtraLen = sizeof(TPacketCGPrivateShopSearchBuy);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		return iExtraLen;

	if (ch->IsStun() || ch->IsDead())
		return iExtraLen;

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU HAVE BLOCK STATUS, OPEN A SUPPORT TICKET"));
		return iExtraLen;
	}

	if (!ch->IsShopSearch())
	{
		sys_err("Player %u is tryint to buy an item with no window opened", ch->GetPlayerID());
		return iExtraLen;
	}

	for (BYTE i = 0; i < SELECTED_ITEM_MAX_NUM; ++i)
	{
		if (p->aSelectedItems[i].dwShopID == ch->GetPlayerID())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot buy an item from your own personal shop."));
			return iExtraLen;
		}
	}

	if (thecore_pulse() - ch->GetLastPrivateShopBuyTime() < PASSES_PER_SEC(1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait a moment before buying from a personal shop again."));
		return iExtraLen;
	}

	// Packet to db
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_BUY_REQUEST;

	TPacketGDPrivateShopBuyRequest subPacket{};
	subPacket.dwCustomerPID = ch->GetPlayerID();
	subPacket.llGoldBalance = ch->GetGold();
#ifdef ENABLE_PRIVATE_SHOP_CHEQUE
	subPacket.dwChequeBalance = ch->GetCheque();
#else
	subPacket.dwChequeBalance = 0;
#endif

	for (BYTE i = 0; i < SELECTED_ITEM_MAX_NUM; ++i)
	{
		if (!p->aSelectedItems[i].dwShopID)
			break;

		subPacket.aSelectedItems[i].dwShopID = p->aSelectedItems[i].dwShopID;
		subPacket.aSelectedItems[i].wPos = p->aSelectedItems[i].wPos;
		subPacket.aSelectedItems[i].TPrice.llGold = p->aSelectedItems[i].TPrice.llGold;
		subPacket.aSelectedItems[i].TPrice.dwCheque = p->aSelectedItems[i].TPrice.dwCheque;
	}

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE) + sizeof(TPacketGDPrivateShopBuyRequest));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&subPacket, sizeof(TPacketGDPrivateShopBuyRequest));

	ch->SetLastPrivateShopBuyTime();
	return iExtraLen;
}
void CInputMain::PrivateShopMarketItemPriceDataRequest(LPCHARACTER ch)
{
	if (!ch)
		return;

	CPrivateShopManager::Instance().SendMarketItemPriceData(ch);
}

int CInputMain::PrivateShopMarketItemPriceRequest(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	DWORD dwVnum = *(DWORD*)c_pData;

	int iExtraLen = sizeof(DWORD);

	// No data has been passed
	if (uiBytes < iExtraLen)
		return -1;

	if (!ch)
		return iExtraLen;

	CPrivateShopManager::Instance().SendMarketItemPrice(ch, dwVnum);
	return iExtraLen;
}
#endif

#if defined(ENABLE_REMOTE_SHOP)
static bool RemoteShopRestrictedMap(long lMapIndex)
{
	switch (lMapIndex)
	{
	case 113:
		return false;
	}
	return true;
}
void CInputMain::RemoteShop(LPCHARACTER ch, const char* data)
{
	const TPacketCGRemoteShop* p = reinterpret_cast<const TPacketCGRemoteShop*>(data);

	if (RemoteShopRestrictedMap(ch->GetMapIndex()) == false)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "<RemoteShop> You can not open in this map.");
		return;
	}

	//PREVENT_TRADE_WINDOW
	if (ch->IsOpenSafebox() || ch->GetExchange() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen()
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
						|| ch->IsAcceOpened()
#endif
#if defined(__BL_67_ATTR__)
						|| ch->Is67AttrOpen()
#endif
	)
		return;
	//END_PREVENT_TRADE_WINDOW

	const DWORD dwCurrentTime = get_dword_time(), dwLimitTime = ch->GetLastRemoteTime() + 3000; // 3 seconds.
	if (dwCurrentTime < dwLimitTime) {
		ch->ChatPacket(CHAT_TYPE_INFO, "<RemoteShop> You have to wait %u sec.", MAX(1, (dwLimitTime - dwCurrentTime) / 1000));
		return;
	}

	static const/*expr*/ DWORD _arrShopVnum[] = { 80, 410, 90, 409, 64 };
	if (p->shop_index >= _countof(_arrShopVnum)) {
		sys_err("RemoteShop player(%s) unknown index(%d)", ch->GetName(), p->shop_index);
		return;
	}

	const DWORD dwShopVnum = _arrShopVnum[p->shop_index];
	LPSHOP pkShop = CShopManager::instance().Get(dwShopVnum);
	if (pkShop == NULL)
		return;

	pkShop->AddGuest(ch, 0, false);
	ch->SetShopOwner(ch);
	ch->SetLastRemoteTime(get_dword_time());
	sys_log(0, "SHOP: START: %s", ch->GetName());
}
#endif

#ifdef ENABLE_HWID_BAN
void CInputMain::HWIDManager(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGHwidBan* p = (TPacketCGHwidBan*)c_pData;

	if (!ch)
		return;

	if (!c_pData)
		return;

	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	if (!ch->IsGM())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "<HWID-Manager> You are not a gamemaster.");
		return;
	}

	if (!check_allow(ch->GetGMLevel(), GM_ALLOW_BAN_PLAYER))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this with this gamemaster rank."));
		return;
	}

	if (!p->bMode)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "<HWID-Manager> select an action");
		return;
	}
	if (strlen(p->szPlayer) == 0) 
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "<HWID-Manager> Player Name can not be empty");
		return;
	}
	if (strlen(p->szReason) == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "<HWID-Manager> Reason can not be empty");
		return;
	}

	//UNBAN
	int account_id = 0;
	if (p->bMode == 1)
	{
		// if (ch->GetGMLevel() <= GM_UNBAN_RIGHT)
		// {
		// 	ch->ChatPacket(CHAT_TYPE_INFO, "<HWID-Manager> You do not have permission!");
		// 	return;
		// }

		char szEscapeName[CHARACTER_NAME_MAX_LEN * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->szPlayer, strlen(p->szPlayer));

		char szEscapeReason[512 * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeReason, sizeof(szEscapeReason), p->szReason, strlen(p->szReason));
		DBManager::Instance().DirectQuery("INSERT INTO log.hwid_ban_log (hwid, player, gm, reason, mode) VALUES ('NO_HWID_NEEDED', '%s', '%s', '%s', 'UNBAN')", szEscapeName, ch->GetName(), szEscapeReason);
		auto pMsg(DBManager::Instance().DirectQuery("SELECT player.account_id FROM player WHERE LOWER(name) like LOWER('%s') LIMIT 1", szEscapeName));
		if (pMsg->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s doesnt exist, search for playername", szEscapeName);
			return;
		}
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(account_id, row[0]);
		DBManager::Instance().DirectQuery("UPDATE account.account set status='OK', availDt='0000-00-00 00:00:00' where id='%d'",account_id);
		ch->ChatPacket(CHAT_TYPE_INFO,"%s unbanned", szEscapeName);
		return;
	}

	// BAN
	if (p->bMode == 2)
	{
		// if (ch->GetGMLevel() <= GM_BAN_RIGHT)
		// {
		// 	ch->ChatPacket(CHAT_TYPE_INFO, "<HWID-Manager> You do not have permission!");
		// 	return;
		// }

		char szEscapeName[CHARACTER_NAME_MAX_LEN * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->szPlayer, strlen(p->szPlayer));
		auto pMsg(DBManager::Instance().DirectQuery("SELECT player.account_id FROM player WHERE LOWER(name) like LOWER('%s') LIMIT 1", szEscapeName));
		if (pMsg->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s doesnt exist, search for playername", szEscapeName);
			return;
		}
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(account_id, row[0]);
		DBManager::Instance().DirectQuery("UPDATE account.account set status='BLOCK' where id='%d'", account_id);
		char szEscapeReason[512 * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeReason, sizeof(szEscapeReason), p->szReason, strlen(p->szReason));
		DBManager::Instance().DirectQuery("INSERT INTO log.hwid_ban_log (hwid, player, gm, reason, mode) VALUES ('NO_HWID_NEEDED', '%s', '%s', '%s', 'BAN')", szEscapeName, ch->GetName(), szEscapeReason);
		ch->ChatPacket(CHAT_TYPE_INFO,"%s banned", szEscapeName);
		LPDESC d = DESC_MANAGER::Instance().FindByCharacterName(szEscapeName);
		LPCHARACTER	tch = d ? d->GetCharacter() : nullptr;
		if (!tch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s: no such a player.", szEscapeName);
			return;
		}

		if (tch == ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "cannot disconnect myself");
			return;
		}

		DESC_MANAGER::Instance().DestroyDesc(d);
		return;
	}

	//HWID BAN
	if (p->bMode == 3)
	{
		// if (ch->GetGMLevel() <= GM_HWID_BAN_RIGHT)
		// {
		// 	ch->ChatPacket(CHAT_TYPE_INFO, "<HWID-Manager> You do not have permission!");
		// 	return;
		// }

		char szEscapeName[CHARACTER_NAME_MAX_LEN * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->szPlayer, strlen(p->szPlayer));	
		auto pMsg(DBManager::Instance().DirectQuery("SELECT player.account_id FROM player WHERE LOWER(name) like LOWER('%s') LIMIT 1", szEscapeName));
		if (pMsg->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s doesnt exist, search for playername", szEscapeName);
			return;
		}
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(account_id, row[0]);
		auto pMsg2(DBManager::Instance().DirectQuery("SELECT hwid FROM account.account WHERE id = '%d' LIMIT 1", account_id));
		if (pMsg2->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "cant get hwid from %s", szEscapeName);
			return;
		}
		MYSQL_ROW row2 = mysql_fetch_row(pMsg2->Get()->pSQLResult);
		if (strcmp(row2[0], "") == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s has no hwid", szEscapeName);
			return;
		}
		DBManager::Instance().DirectQuery("INSERT INTO account.hwid_ban (hwid, player, gm) VALUES ('%s', '%s', '%s')", row2[0], szEscapeName, ch->GetName());
		char szEscapeReason[512 * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeReason, sizeof(szEscapeReason), p->szReason, strlen(p->szReason));
		DBManager::Instance().DirectQuery("INSERT INTO log.hwid_ban_log (hwid, player, gm, reason, mode) VALUES ('%s', '%s', '%s', '%s', 'HWID_BAN')", row2[0], szEscapeName, ch->GetName(), szEscapeReason);
		ch->ChatPacket(CHAT_TYPE_INFO,"%s HWID banned (%s)", szEscapeName, row2[0]);
		LPDESC d = DESC_MANAGER::Instance().FindByCharacterName(szEscapeName);
		LPCHARACTER	tch = d ? d->GetCharacter() : nullptr;
		if (!tch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s: no such a player.", szEscapeName);
			return;
		}

		if (tch == ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "cannot disconnect myself");
			return;
		}

		DESC_MANAGER::Instance().DestroyDesc(d);
		return;
	}

	//DELETE HWID BAN
	if (p->bMode == 4)
	{
		// if (ch->GetGMLevel() <= GM_DELETE_HWID_BAN_RIGHT)
		// {
		// 	ch->ChatPacket(CHAT_TYPE_INFO, "<HWID-Manager> You do not have permission!");
		// 	return;
		// }

		char szEscapeName[CHARACTER_NAME_MAX_LEN * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->szPlayer, strlen(p->szPlayer));
		auto pMsg(DBManager::Instance().DirectQuery("SELECT player.account_id FROM player WHERE LOWER(name) like LOWER('%s') LIMIT 1", szEscapeName));
		if (pMsg->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s doesnt exist, search for playername", szEscapeName);
			return;
		}
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(account_id, row[0]);
		auto pMsg2(DBManager::Instance().DirectQuery("SELECT hwid FROM account.account WHERE id = '%d' LIMIT 1", account_id));
		if (pMsg2->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "cant get hwid from %s", szEscapeName);
			return;
		}
		MYSQL_ROW row2 = mysql_fetch_row(pMsg2->Get()->pSQLResult);
		if (strcmp(row2[0], "") == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s has no hwid", szEscapeName);
			return;
		}
		DBManager::Instance().DirectQuery("DELETE FROM account.hwid_ban WHERE hwid = '%s'", row2[0]);

		char szEscapeReason[512 * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeReason, sizeof(szEscapeReason), p->szReason, strlen(p->szReason));
		DBManager::Instance().DirectQuery("INSERT INTO log.hwid_ban_log (hwid, player, gm, reason, mode) VALUES ('%s', '%s', '%s', '%s', 'DELETE_HWID_BAN')", row2[0], szEscapeName, ch->GetName(), szEscapeReason);
		ch->ChatPacket(CHAT_TYPE_INFO,"%s HWID unbanned (%s)", szEscapeName, row2[0]);
		return;
	}

	//TOTAL HWID BAN
	if (p->bMode == 5)
	{
		// if (ch->GetGMLevel() <= GM_TOTAL_HWID_BAN_RIGHT)
		// {
		// 	ch->ChatPacket(CHAT_TYPE_INFO, "<HWID-Manager> You do not have permission!");
		// 	return;
		// }

		char szEscapeName[CHARACTER_NAME_MAX_LEN * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->szPlayer, strlen(p->szPlayer));
		auto pMsg(DBManager::Instance().DirectQuery("SELECT player.account_id FROM player WHERE LOWER(name) like LOWER('%s') LIMIT 1", szEscapeName));
		if (pMsg->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s doesnt exist, search for playername", szEscapeName);
			return;
		}
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(account_id, row[0]);
		auto pMsg2(DBManager::Instance().DirectQuery("SELECT hwid FROM account.account WHERE id = '%d' LIMIT 1", account_id));
		if (pMsg2->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "cant get hwid from %s", szEscapeName);
			return;
		}
		MYSQL_ROW row2 = mysql_fetch_row(pMsg2->Get()->pSQLResult);
		if (strcmp(row2[0], "") == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s has no hwid", szEscapeName);
			return;
		}
		DBManager::Instance().DirectQuery("UPDATE account.account set status = 'BLOCK' WHERE hwid = '%s'", row2[0]);
		DBManager::Instance().DirectQuery("INSERT INTO account.hwid_ban (hwid, player, gm) VALUES ('%s', '%s', '%s')", row2[0], szEscapeName, ch->GetName());

		char szEscapeReason[512 * 2 + 1];
		DBManager::Instance().EscapeString(szEscapeReason, sizeof(szEscapeReason), p->szReason, strlen(p->szReason));
		DBManager::Instance().DirectQuery("INSERT INTO log.hwid_ban_log (hwid, player, gm, reason, mode) VALUES ('%s', '%s', '%s', '%s', 'TOTAL_HWID_BAN')", row2[0], szEscapeName, ch->GetName(), szEscapeReason);
		ch->ChatPacket(CHAT_TYPE_INFO,"%s HWID and all associated accounts banned (%s)", szEscapeName, row2[0]);
		LPDESC d = DESC_MANAGER::Instance().FindByCharacterName(szEscapeName);
		LPCHARACTER	tch = d ? d->GetCharacter() : nullptr;
		if (!tch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s: no such a player.", szEscapeName);
			return;
		}

		if (tch == ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "cannot disconnect myself");
			return;
		}

		DESC_MANAGER::Instance().DestroyDesc(d);
		return;
	}
}
#endif

#ifdef ENABLE_HUNTING_SYSTEM
int CInputMain::ReciveHuntingAction(LPCHARACTER ch, const char* c_pData)
{
	TPacketGCHuntingAction* p = (TPacketGCHuntingAction*)c_pData;

	switch (p->bAction)
	{
		// Open Window
		case 1:
			
			ch->OpenHuntingWindowMain();
			break;

		// Select Type
		case 2:
			if (ch->GetQuestFlag("hunting_system.is_active") == -1)
			{
				
				if (ch->GetLevel() < ch->GetQuestFlag("hunting_system.level"))
					return 0;
				
				ch->SetQuestFlag("hunting_system.is_active", 1);
				ch->SetQuestFlag("hunting_system.type", p->dValue);
				ch->SetQuestFlag("hunting_system.count", 0);
				
				ch->OpenHuntingWindowMain();

			} 
			else 
				ch->ChatPacket(CHAT_TYPE_INFO, "Es ist noch eine Jagdmission offen.");
			break;
		
		// Open Reward Window
		case 3:
			ch->OpenHuntingWindowReward();
			break;
			
		// Recive Reward
		case 4:
			ch->ReciveHuntingRewards();
			break;

		default:
			break;
	}

	return 0;
}
#endif

#if defined(__BL_67_ATTR__)
#include "../../common/VnumHelper.h"

void CInputMain::Attr67(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCG67Attr* p = reinterpret_cast<const TPacketCG67Attr*>(c_pData);
	
	if (ch->IsDead())
		return;
	
	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsCubeOpen()
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		|| ch->IsAcceOpened()
#endif
	)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You have to close other windows.");
		return;
	}
	
	const LPITEM item = ch->GetInventoryItem(p->sItemPos);
	if (!item)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "There is no item.");
		return;
	}

	switch (item->GetType())
	{
	case ITEM_WEAPON:
	case ITEM_ARMOR:
	case ITEM_COSTUME:
		break;
	default:
		ch->ChatPacket(CHAT_TYPE_INFO, "The item type is not suitable for 6-7 attr.");
		return;
	}

	if (item->IsEquipped())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot add a bonus to a equipped item.");
		return;
	}

	if (item->IsExchanging())
		return;

	const int norm_attr_count = item->GetAttributeCount();
	const int rare_attr_count = item->GetRareAttrCount();
	const int attr_set_index = item->GetAttributeSetIndex();

	if (attr_set_index == -1 || norm_attr_count < 5 || rare_attr_count >= 2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "This item is not suitable for 6-7 attr.");
		return;
	}

	enum
	{
		SUCCESS_PER_MATERIAL = 2,
		MATERIAL_MAX_COUNT = 10,
		SUPPORT_MAX_COUNT = 5,
	};

	if (p->bMaterialCount > MATERIAL_MAX_COUNT || p->bSupportCount > SUPPORT_MAX_COUNT)
		return;

	const DWORD dwMaterialVnum = CItemVnumHelper::Get67MaterialVnum(item->GetLevelLimit());
	if (dwMaterialVnum == 0 || p->bMaterialCount < 1 
		|| ch->CountSpecifyItem(dwMaterialVnum) < p->bMaterialCount)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enough material item.");
		return;
	}
	
	BYTE success = SUCCESS_PER_MATERIAL * p->bMaterialCount;
	
	if (p->sSupportPos != -1)
	{
		const LPITEM support_item = ch->GetInventoryItem(p->sSupportPos);
		if (support_item)
		{
			if (support_item->GetCount() < p->bSupportCount)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You don't have enough support item.");
				return;
			}

			BYTE uSupportSuccess = 0;
			switch (support_item->GetVnum())
			{
			case 72064:
				uSupportSuccess = 1;
				break;
			case 72065:
				uSupportSuccess = 2;
				break;
			case 72066:
				uSupportSuccess = 4;
				break;
			case 72067:
				uSupportSuccess = 10;
				break;
			default:
				ch->ChatPacket(CHAT_TYPE_INFO, "The support item is inappropriate.");
				return;
			}

			success += uSupportSuccess * p->bSupportCount;
			support_item->SetCount(support_item->GetCount() - p->bSupportCount);
		}
	}

	ch->RemoveSpecifyItem(dwMaterialVnum, p->bMaterialCount);
	
	const bool bAdded = (number(1, 100) <= success && item->AddRareAttribute());
	ch->ChatPacket(CHAT_TYPE_INFO, "%s!", bAdded ? "Success" : "Failed");
}

void CInputMain::Attr67Close(LPCHARACTER ch, const char* c_pData)
{
	ch->Set67Attr(false);
}
#endif


#ifdef ENABLE_DUNGEON_INFO_SYSTEM
void CInputMain::DungeonInfo(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGDungeonInfo* p = (TPacketCGDungeonInfo*)c_pData;

	switch (p->bySubHeader)
	{
		case EAction::CLOSE:
		{
			ch->SetDungeonInfoOpen(false);
			ch->StopDungeonInfoReloadEvent();
		}
		break;

		case EAction::OPEN:
			if (!ch->IsDungeonInfoOpen())
				CDungeonInfoManager::Instance().SendInfo(ch);
			break;

		case EAction::WARP:
			if (ch->IsDungeonInfoOpen())
				CDungeonInfoManager::Instance().Warp(ch, p->byIndex);
			break;

		case EAction::RANK:
			if (ch->IsDungeonInfoOpen())
				CDungeonInfoManager::Instance().Ranking(ch, p->byIndex, p->byRankType);
			break;

		default:
			return;
	}
}
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
void CInputMain::SetSkillColor(LPCHARACTER ch, const char* pcData)
{
	TPacketCGSkillColor * p = (TPacketCGSkillColor*)pcData;

	if (p->skill >= ESkillColorLength::MAX_SKILL_COUNT)
		return;

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::SkillColor, std::chrono::milliseconds(2000)))
		return ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d s! Try again later."), 2);
#endif

	DWORD data[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	thecore_memcpy(data, ch->GetSkillColor(), sizeof(data));

	data[p->skill][0] = p->col1;
	data[p->skill][1] = p->col2;
	data[p->skill][2] = p->col3;
	data[p->skill][3] = p->col4;
	data[p->skill][4] = p->col5;

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have changed the color of your skill."));

	ch->SetSkillColor(data[0]);

	TSkillColor db_pack;
	thecore_memcpy(db_pack.dwSkillColor, data, sizeof(data));
	db_pack.player_id = ch->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_SKILL_COLOR_SAVE, 0, sizeof(TSkillColor));
	db_clientdesc->Packet(&db_pack, sizeof(TSkillColor));
}
#endif


#ifdef ENABLE_HIDE_COSTUME_SYSTEM
void CInputMain::ChangeCostumeVisibilityStatus(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGChangeCostumeVisibilityStatus* p = (TPacketCGChangeCostumeVisibilityStatus*) c_pData;

	if (!ch)
	{
		return;
	}

	switch (p->type)
	{
	case ECostumeHideParts::HIDE_COSTUME_BODY:
	{
		if (p->status)
		{
			ch->SetQuestFlag("hide.costume_body", 0);
		}
		else
		{
			ch->SetQuestFlag("hide.costume_body", 1);
		}
	}
	break;

	case ECostumeHideParts::HIDE_COSTUME_HAIR:
	{
		if (p->status)
		{
			ch->SetQuestFlag("hide.costume_hair", 0);
		}
		else
		{
			ch->SetQuestFlag("hide.costume_hair", 1);
		}
	}
	break;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	case ECostumeHideParts::HIDE_COSTUME_WEAPON:
	{
		if (p->status)
		{
			ch->SetQuestFlag("hide.costume_weapon", 0);
		}
		else
		{
			ch->SetQuestFlag("hide.costume_weapon", 1);
		}
	}
	break;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	case ECostumeHideParts::HIDE_COSTUME_ACCE:
	{
		if (p->status)
		{
			ch->SetQuestFlag("hide.costume_acce", 0);
		}
		else
		{
			ch->SetQuestFlag("hide.costume_acce", 1);
		}
	}
	break;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	case ECostumeHideParts::HIDE_COSTUME_AURA:
	{
		if (p->status)
		{
			ch->SetQuestFlag("hide.costume_aura", 0);
		}
		else
		{
			ch->SetQuestFlag("hide.costume_aura", 1);
		}
	}
	break;
#endif
	}

	ch->BroadcastTargetPacket();
	ch->UpdatePacket();
}
#endif

#ifdef ENABLE_ANTI_MULTIPLE_FARM
int CInputMain::RecvAntiFarmUpdateStatus(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	const TSendAntiFarmInfo* p = reinterpret_cast<const TSendAntiFarmInfo*>(data);

	if (uiBytes < sizeof(TSendAntiFarmInfo))
		return -1;
	
	LPDESC d = nullptr;
	if (!ch || !(d = ch->GetDesc()))
		return -1;

	const char* c_pData = data + sizeof(TSendAntiFarmInfo);
	uiBytes -= sizeof(TSendAntiFarmInfo);

	switch (p->subheader)
	{
		case AF_SH_SEND_STATUS_UPDATE:
		{
			size_t extraLen = (sizeof(DWORD) * MULTIPLE_FARM_MAX_ACCOUNT);
			if (uiBytes < extraLen)
				return -1;
			
			std::vector<DWORD> v_dwPIDS;
			for (uint8_t i = 0; i < MULTIPLE_FARM_MAX_ACCOUNT; ++i)
				v_dwPIDS.emplace_back(*reinterpret_cast<const DWORD*>(c_pData + (sizeof(DWORD) * i)));
			
			std::string sMAIf = d->GetLoginMacAdress();
			CAntiMultipleFarm::instance().SendBlockDropStatusChange(sMAIf, v_dwPIDS);
			
			{
				//send p2p update
				CAntiMultipleFarm::TP2PChangeDropStatus dataPacket(HEADER_GG_ANTI_FARM);
				strlcpy(dataPacket.cMAIf, sMAIf.c_str(), sizeof(dataPacket.cMAIf));
				for (uint8_t i = 0; i < v_dwPIDS.size(); ++i)
					dataPacket.dwPIDs[i] = v_dwPIDS[i];

				P2P_MANAGER::instance().Send(&dataPacket, sizeof(CAntiMultipleFarm::TP2PChangeDropStatus));
			}
			
			return extraLen;
		}
	}

	return 0;
}
#endif

#ifdef ENABLE_SPLIT_BY_COUNT
void CInputMain::SendSplitItemByCount(LPCHARACTER ch, const char* c_pData)
{
	if (!ch || !c_pData)
		return;
	
	TSplitItemByCount* dataPacket = (TSplitItemByCount*)c_pData;
	ch->SendSplitItemByCount(dataPacket->Cell, dataPacket->sCount);
}
#endif

#ifdef ENABLE_BATTLE_PASS
int CInputMain::BattlePass(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	TPacketCGBattlePassAction * p = (TPacketCGBattlePassAction *) data;

	if (uiBytes < sizeof(TPacketCGBattlePassAction))
	{
		return -1;
	}

	const char * c_pData = data + sizeof(TPacketCGBattlePassAction);
	uiBytes -= sizeof(TPacketCGBattlePassAction);

	switch (p->bAction)
	{
	case 1:
		CBattlePass::instance().BattlePassRequestOpen(ch);
		break;

	case 2:
		CBattlePass::instance().BattlePassRequestReward(ch);
		break;

	case 3:
	{
		DWORD dwPlayerId = ch->GetPlayerID();
		BYTE bIsGlobal = 0;

		db_clientdesc->DBPacketHeader(HEADER_GD_BATTLE_PASS_RANKING, ch->GetDesc()->GetHandle(), sizeof(DWORD) + sizeof(BYTE));
		db_clientdesc->Packet(&dwPlayerId, sizeof(DWORD));
		db_clientdesc->Packet(&bIsGlobal, sizeof(BYTE));
	}
	break;

	default:
		break;
	}

	return 0;
}
#endif

#ifdef ENABLE_ITEMSHOP
void CInputMain::BuyItemshopItem(LPCHARACTER ch, const char* c_pData)
{
	if (!ch || !ch->GetDesc())
		return;

	// Wenn der Char kein GM ist, prufen wir, ob sein Account trotzdem in der GMListe steht
	if (!ch->IsGM())
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT count(*) FROM common.gmlist WHERE mAccount='%s'", ch->GetDesc()->GetAccountTable().login));
		if (pMsg->Get()->uiNumRows > 0)
		{
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			if (row && atoi(row[0]) > 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Admin-Account erkannt: Kaufe nur mit dem GM-Charakter erlaubt.");
				return;
			}
		}
	}

	const TPacketCGBuyItemshopItem* p = reinterpret_cast<const TPacketCGBuyItemshopItem*>(c_pData);
	if (CItemshopManager::instance().CanBuyItem(ch, p->hash, p->wCount))
	{
		TItemshopCheckBuy p_gd;
		strlcpy(p_gd.hash, p->hash, sizeof(p_gd.hash));
		p_gd.wCount = p->wCount;
		db_clientdesc->DBPacket(HEADER_GD_BUY_ITEMSHOP, ch->GetDesc()->GetHandle(), &p_gd, sizeof(p_gd));
	}
}

void CInputMain::RedeemPromotionCode(LPCHARACTER ch, const char* c_pData)
{
	// Sperre auch fur Promotion-Codes:
	if (ch->GetGMLevel() > GM_PLAYER && !ch->IsGM())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Codes koennen auf Admin-Accounts nur mit dem GM-Charakter eingeloest werden.");
		return;
	}

	if (!ch->CanUsePromotionCode())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait 10 seconds after using promotion codes."));
		return;
	}

	ch->SetPromotionCodedown(thecore_pulse() + PASSES_PER_SEC(10));
	const TPacketCGRedeemPromotionCode* p = reinterpret_cast<const TPacketCGRedeemPromotionCode*>(c_pData);
	
	if (!strcmp(p->promotion_code, ""))
		return;

	TPromotionRedeem gd_p;
	strlcpy(gd_p.code, p->promotion_code, sizeof(gd_p.code));
	gd_p.accID = ch->GetAID();
	db_clientdesc->DBPacket(HEADER_GD_PROMOTION_REDEEM, ch->GetDesc()->GetHandle(), &gd_p, sizeof(gd_p));
}
#endif

#ifdef ENABLE_FAST_STACK
void CInputMain::FastStack(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFastStack* p = (TPacketCGFastStack*)c_pData;
	if (ch)
		ch->FastStack(p->pos);
}
#endif

#ifdef __GUILD_SAFEBOX__
void CInputMain::GuildSafebox(LPCHARACTER ch, BYTE header, const char* c_pData)
{
	if (!ch->GetGuild())
	{
		sys_err("no guild for player %d %s header %d", ch->GetPlayerID(), ch->GetName(), header);
		return;
	}

	TGuildMember* pkMember = ch->GetGuild()->GetMember(ch->GetPlayerID());
	if (!pkMember)
	{
		sys_err("no guild member in own guild %d %s", ch->GetPlayerID(), ch->GetName());
		return;
	}

	CGuildSafeBox& rkGuildSafeBox = ch->GetGuild()->GetSafeBox();
	if (!rkGuildSafeBox.HasSafebox())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your guild has no safebox."));
		return;
	}

#ifdef ENABLE_ALLOW_GUILD_SAFEBOX
	if (!ch->GetGuild()->HasGradeAuth(pkMember->grade, GUILD_AUTH_SAFEBOX))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT ("<Guild> You dont have permission to use the guild safebox."));
		return;
	}
#endif

	if (header == HEADER_CG_GUILD_SAFEBOX_OPEN)
	{
		rkGuildSafeBox.OpenSafebox(ch);
	}
	else if (header == HEADER_CG_GUILD_SAFEBOX_CHECKIN)
	{
		TPacketCGGuildSafeboxCheckin* p = (TPacketCGGuildSafeboxCheckin*)c_pData;
		LPITEM pkItem = ch->GetItem(p->ItemPos);
		if (pkItem)
		{
			if (pkItem->IsEquipped() || pkItem->isLocked())
				return;

			rkGuildSafeBox.CheckInItem(ch, pkItem, p->bSafePos);
		}
		else
			sys_err("GUILD_SAFEBOX_CHECKIN: cannot get item window %d pos %d", p->ItemPos.window_type, p->ItemPos.cell);

	}
	else if (header == HEADER_CG_GUILD_SAFEBOX_CHECKOUT)
	{
		TPacketCGGuildSafeboxCheckout* p = (TPacketCGGuildSafeboxCheckout*)c_pData;
		rkGuildSafeBox.CheckOutItem(ch, p->bSafePos, p->ItemPos.window_type, p->ItemPos.cell);
	}
	else if (header == HEADER_CG_GUILD_SAFEBOX_ITEM_MOVE)
	{
		TPacketCGItemMove* p = (TPacketCGItemMove*)c_pData;
		rkGuildSafeBox.MoveItem(ch, p->Cell.cell, p->CellTo.cell, p->count);
	}
	else if (header == HEADER_CG_GUILD_SAFEBOX_GIVE_GOLD)
	{
		TPacketCGGuildSafeboxGold* p = (TPacketCGGuildSafeboxGold*)c_pData;
		rkGuildSafeBox.GiveGold(ch, p->gold);
	}
	else if (header == HEADER_CG_GUILD_SAFEBOX_GET_GOLD)
	{
		TPacketCGGuildSafeboxGold* p = (TPacketCGGuildSafeboxGold*)c_pData;
		rkGuildSafeBox.TakeGold(ch, p->gold);
	}
}
#endif

#ifdef ENABLE_TRASH_SYSTEM
int CInputMain::RecvTrashPacket(LPCHARACTER ch, const char* data)
{
	TTrashPacketCG* packet_data = (TTrashPacketCG*) data;
	data += sizeof(TTrashPacketCG);

	std::vector<uint8_t> tmp_vec_RemovedItems;
	// tmp_vec_RemovedItems.reserve(packet_data->items_count);

	sys_log(0, "CInputMain::RecvTrashPacket >> packet_data->items_count = %d", packet_data->items_count);

	for (uint8_t i = 0; i < packet_data->items_count; i++)
	{
		TTrashItemInfo* trash_item = (TTrashItemInfo*) data;
		data += sizeof(TTrashItemInfo);

		LPITEM item = ch->GetItem(trash_item->item_pos);
		if (!item)
			continue;

		if (item->IsExchanging() || item->IsEquipped() || true == item->isLocked())
			continue;

		if (g_setTrashDisablesItems.contains(item->GetVnum()))
			continue;

		/*char reason[256];
		memset(reason, 0, sizeof(reason));
		snprintf(reason, sizeof(reason), "[TRASH] Removed item %d (vnum %d, count %d)",
				item->GetID(), item->GetVnum(), item->GetCount());*/

		ITEM_MANAGER::Instance().RemoveItem(item/*, reason*/);
		tmp_vec_RemovedItems.push_back(trash_item->slot);
	}

	if (tmp_vec_RemovedItems.size() != packet_data->items_count)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Some items were not removed"));

	TEMP_BUFFER buf;

	TTrashPacketGC packet;
	packet.header = HEADER_GC_TRASH;
	packet.size = sizeof(packet) + sizeof(uint8_t) * tmp_vec_RemovedItems.size();

	buf.write(&packet, sizeof(packet));
	for (const auto& slot : tmp_vec_RemovedItems)
		buf.write(&slot, sizeof(slot));

	ch->GetDesc()->Packet(buf.read_peek(), buf.size());

	return sizeof(TTrashItemInfo) * packet_data->items_count;
}
#endif

#ifdef ENABLE_CHEST_OPEN_RENEWAL
void CInputMain::ChestOpen(LPCHARACTER ch, const char * data)
{
	struct OpenChestPacket * pinfo = (struct OpenChestPacket *) data;
	if (ch)
		ch->OpenChestItem(pinfo->pos, pinfo->wOpenCount);
}
#endif

#ifndef __ENABLE_NEW_OFFLINESHOP__
template <class T>
bool CanDecode(T* p, int buffleft) {
	return buffleft >= (int)sizeof(T);
}

template <class T>
const char* Decode(T*& pObj, const char* data, int* pbufferLeng = nullptr, int* piBufferLeft = nullptr)
{
	pObj = (T*)data;

	if (pbufferLeng)
		*pbufferLeng += sizeof(T);

	if (piBufferLeft)
		*piBufferLeft -= sizeof(T);

	return data + sizeof(T);
}
#endif


int GuildStorageRequestCheckout(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	TPacketCGGuildStorageCheckout* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);


	GuildStorageManager::Instance().RemoveItem(ch, pack->slot, pack->inventoryPos);
	return iExtra;
}

int GuildStorageRequestCheckin(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	TPacketCGGuildStorageCheckin* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);


	GuildStorageManager::Instance().AddItem(ch, pack->inventoryPos, pack->slot);
	return iExtra;
}

int GuildStorageRequestMove(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	TPacketCGGuildStorageMove* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);


	GuildStorageManager::Instance().MoveItem(ch, pack->slot, pack->new_slot);
	return iExtra;
}

int GuildStorageRequestClose(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	GuildStorageManager::Instance().CloseGuildStorage(ch);
	return iBufferLeft;
}


int GuildStorageRequestCheckinGold(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	TPacketGuildStorageGoldUpdate* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);


	GuildStorageManager::Instance().AddGold(ch, pack->gold);
	return iExtra;
}


int GuildStorageRequestCheckoutGold(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	TPacketGuildStorageGoldUpdate* pack = nullptr;
	if (!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra = 0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);


	GuildStorageManager::Instance().RemoveGold(ch, pack->gold);
	return iExtra;
}

int GuildStoragePacket(const char* data, LPCHARACTER ch, long iBufferLeft)
{
	if (iBufferLeft < sizeof(TPacketGuildStorage))
		return -1;

	TPacketGuildStorage* pPack = nullptr;
	iBufferLeft -= sizeof(TPacketGuildStorage);
	data = Decode(pPack, data);

	switch (pPack->subHeader)
	{

	case GUILDSTORAGE_CG_REQUEST_CHECKOUT:
		return GuildStorageRequestCheckout(ch, data, iBufferLeft);
	case GUILDSTORAGE_CG_REQUEST_CHECKIN:
		return GuildStorageRequestCheckin(ch, data, iBufferLeft);
	case GUILDSTORAGE_CG_REQUEST_MOVE:
		return GuildStorageRequestMove(ch, data, iBufferLeft);
	case GUILDSTORAGE_CG_REQUEST_CLOSE:
		return GuildStorageRequestClose(ch, data, iBufferLeft);
	case GUILDSTORAGE_CG_REQUEST_GOLDIN:
		return GuildStorageRequestCheckinGold(ch, data, iBufferLeft);
	case GUILDSTORAGE_CG_REQUEST_GOLDOUT:
		return GuildStorageRequestCheckoutGold(ch, data, iBufferLeft);

	default:
		sys_err("UNKNOWN SUBHEADER %d ", pPack->subHeader);
		return -1;
	}

}

int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		d->SetPhase(PHASE_CLOSE);
		return (0);
	}

	int iExtraLen = 0;

	if (test_server && bHeader != HEADER_CG_MOVE)
		sys_log(0, "CInputMain::Analyze() ==> Header [%d] ", bHeader);

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if (test_server)
			{
				char* pBuf = (char*)c_pData;
				sys_log(0, "%s", pBuf + sizeof(TPacketCGChat));
			}

			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MOVE:
			Move(ch, c_pData);
			// @fixme103 (removed CheckClientVersion since useless in here)
			break;

		case HEADER_CG_CHARACTER_POSITION:
			Position(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE:
			if (!ch->IsObserverMode())
				ItemUse(ch, c_pData);
			break;

		case HEADER_CG_ITEM_DROP:
			if (!ch->IsObserverMode())
			{
				ItemDrop(ch, c_pData);
			}
			break;

#ifdef ENABLE_ITEM_MANAGER
		case HEADER_CG_ITEM_DESTROY:
			if (!ch->IsObserverMode())
			{
				ItemDestroy(ch, c_pData);
			}
			break;

		case HEADER_CG_ITEM_SELL:
			if (!ch->IsObserverMode())
			{
				ItemSell(ch, c_pData);
			}
			break;
#endif

		case HEADER_CG_ITEM_DROP2:
			if (!ch->IsObserverMode())
				ItemDrop2(ch, c_pData);
			break;

		case HEADER_CG_ITEM_MOVE:
			if (!ch->IsObserverMode())
				ItemMove(ch, c_pData);
			break;

		case HEADER_CG_ITEM_PICKUP:
			if (!ch->IsObserverMode())
				ItemPickup(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE_TO_ITEM:
			if (!ch->IsObserverMode())
				ItemToItem(ch, c_pData);
			break;

		case HEADER_CG_ITEM_GIVE:
			if (!ch->IsObserverMode())
				ItemGive(ch, c_pData);
			break;

		case HEADER_CG_EXCHANGE:
			if (!ch->IsObserverMode())
				Exchange(ch, c_pData);
			break;

		case HEADER_CG_ATTACK:
		case HEADER_CG_SHOOT:
			if (!ch->IsObserverMode())
			{
				Attack(ch, bHeader, c_pData);
			}
			break;

		case HEADER_CG_USE_SKILL:
			if (!ch->IsObserverMode())
				UseSkill(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_ADD:
			QuickslotAdd(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_DEL:
			QuickslotDelete(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_SWAP:
			QuickslotSwap(ch, c_pData);
			break;

#ifdef ENABLE_SPECIAL_INVENTORY
		case HEADER_CG_SHOP:
			Shop(ch, c_pData);
			break;
#else
		case HEADER_CG_SHOP:
			if ((iExtraLen = Shop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

		case HEADER_CG_MESSENGER:
			if ((iExtraLen = Messenger(ch, c_pData, m_iBufferLeft))<0)
				return -1;
			break;

		case HEADER_CG_ON_CLICK:
			OnClick(ch, c_pData);
			break;

		case HEADER_CG_SYNC_POSITION:
			if ((iExtraLen = SyncPosition(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_ADD_FLY_TARGETING:
		case HEADER_CG_FLY_TARGETING:
			FlyTarget(ch, c_pData, bHeader);
			break;

		case HEADER_CG_SCRIPT_BUTTON:
			ScriptButton(ch, c_pData);
			break;

			// SCRIPT_SELECT_ITEM
		case HEADER_CG_SCRIPT_SELECT_ITEM:
			ScriptSelectItem(ch, c_pData);
			break;
			// END_OF_SCRIPT_SELECT_ITEM

		case HEADER_CG_SCRIPT_ANSWER:
			ScriptAnswer(ch, c_pData);
			break;

		case HEADER_CG_QUEST_INPUT_STRING:
			QuestInputString(ch, c_pData);
			break;

		case HEADER_CG_QUEST_CONFIRM:
			QuestConfirm(ch, c_pData);
			break;

		case HEADER_CG_TARGET:
			Target(ch, c_pData);
			break;

		case HEADER_CG_WARP:
			Warp(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKIN:
			SafeboxCheckin(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKOUT:
			SafeboxCheckout(ch, c_pData, false);
			break;

		case HEADER_CG_SAFEBOX_ITEM_MOVE:
			SafeboxItemMove(ch, c_pData);
			break;

		case HEADER_CG_MALL_CHECKOUT:
			SafeboxCheckout(ch, c_pData, true);
			break;

		case HEADER_CG_PARTY_INVITE:
			PartyInvite(ch, c_pData);
			break;

		case HEADER_CG_PARTY_REMOVE:
			PartyRemove(ch, c_pData);
			break;

		case HEADER_CG_PARTY_INVITE_ANSWER:
			PartyInviteAnswer(ch, c_pData);
			break;

		case HEADER_CG_PARTY_SET_STATE:
			PartySetState(ch, c_pData);
			break;

		case HEADER_CG_PARTY_USE_SKILL:
			PartyUseSkill(ch, c_pData);
			break;

		case HEADER_CG_PARTY_PARAMETER:
			PartyParameter(ch, c_pData);
			break;

		case HEADER_CG_ANSWER_MAKE_GUILD:
#ifdef ENABLE_NEWGUILDMAKE
			ch->ChatPacket(CHAT_TYPE_INFO, "<%s> AnswerMakeGuild disabled", __FUNCTION__);
#else
			AnswerMakeGuild(ch, c_pData);
#endif
			break;

		case HEADER_CG_GUILD:
			if ((iExtraLen = Guild(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_FISHING:
			Fishing(ch, c_pData);
			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		case HEADER_CG_MYSHOP:
			if ((iExtraLen = MyShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case HEADER_CG_ACCE:
			Acce(ch, c_pData);
			break;
#endif

		case HEADER_CG_REFINE:
			Refine(ch, c_pData);
			break;

		case HEADER_CG_GUILDSTORAGE:
			if ((iExtraLen = GuildStoragePacket(c_pData, ch, m_iBufferLeft))< 0)
				return -1;
			break;

		case HEADER_CG_CLIENT_VERSION:
			Version(ch, c_pData);
			break;

		case HEADER_CG_DRAGON_SOUL_REFINE:
			{
				TPacketCGDragonSoulRefine* p = reinterpret_cast <TPacketCGDragonSoulRefine*>((void*)c_pData);
				switch(p->bSubType)
				{
				case DS_SUB_HEADER_CLOSE:
					ch->DragonSoul_RefineWindow_Close();
					break;
				case DS_SUB_HEADER_DO_REFINE_GRADE:
					{
						DSManager::instance().DoRefineGrade(ch, p->ItemGrid);
					}
					break;
#ifdef __ENABLE_DS_REFINE_ALL__
				case DS_SUB_HEADER_DO_REFINE_GRADE_ALL:
					{
						DSManager::instance().DoAllRefineGrade(ch, p->ItemGrid[0].cell, p->ItemGrid[1].cell);
					}
					break;
#endif
				case DS_SUB_HEADER_DO_REFINE_STEP:
					{
						DSManager::instance().DoRefineStep(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STRENGTH:
					{
						DSManager::instance().DoRefineStrength(ch, p->ItemGrid);
					}
					break;
				}
			}

			break;

#ifdef ENABLE_SWITCHBOT_SYSTEM
		case HEADER_CG_SWITCHBOT:
			if ((iExtraLen = Switchbot(ch, c_pData, m_iBufferLeft)) < 0)
			{
				return -1;
			}
			break;
#endif

#ifdef ENABLE_DROP_INFO
		case HEADER_CG_DROP_INFO:
			DropInfo(ch);
			break;
#endif

#ifdef ENABLE_EVENT_MANAGER
		case HEADER_CG_REQUEST_EVENT_QUEST:
			RequestEventQuest(ch, c_pData);
			break;

		case HEADER_CG_REQUEST_EVENT_DATA:
			RequestEventData(ch, c_pData);
			break;
#endif

#ifdef ENABLE_INGAME_WIKI
		case InGameWiki::HEADER_CG_WIKI:
			RecvWikiPacket(ch, c_pData);
			break;
#endif

#ifdef ENABLE_GEM_SYSTEM
		case HEADER_CG_GEM_SHOP:
			if ((iExtraLen = GemShop(ch, c_pData, m_iBufferLeft)) < 0)
			{
				return -1;
			}
			break;
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
		case HEADER_CG_BIOLOG_MANAGER:
			if ((iExtraLen = BiologManager(ch, c_pData, m_iBufferLeft)) < 0)
			{
				return -1;
			}
			break;
#endif

#ifdef ENABLE_SKILL_GROUP_GUI
		case HEADER_CG_SKILL_GROUP_SELECT:
			SkillGroup(ch, c_pData);
			break;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
		case HEADER_CG_PRIVATE_SHOP:
		{
			TPacketCGPrivateShop* p = (TPacketCGPrivateShop*)c_pData;
			c_pData += sizeof(TPacketCGPrivateShop);

			switch (p->bSubHeader)
			{
			case SUBHEADER_CG_PRIVATE_SHOP_BUILD:
			{
				if ((iExtraLen = PrivateShopBuild(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_CLOSE:
			{
				PrivateShopClose(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN:
			{
				PrivateShopPanelOpen(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_PANEL_CLOSE:
			{
				PrivateShopPanelClose(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_START:
			{
				if ((iExtraLen = PrivateShopStart(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_END:
			{
				PrivateShopEnd(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_BUY:
			{
				if ((iExtraLen = PrivateShopBuy(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_WITHDRAW:
			{
				PrivateShopWithdraw(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_MODIFY:
			{
				PrivateShopModify(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_ITEM_PRICE_CHANGE:
			{
				if ((iExtraLen = PrivateShopItemPriceChange(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_ITEM_MOVE:
			{
				if ((iExtraLen = PrivateShopItemMove(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKIN:
			{
				if ((iExtraLen = PrivateShopItemCheckin(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKOUT:
			{
				if ((iExtraLen = PrivateShopItemCheckout(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_TITLE_CHANGE:
			{
				if ((iExtraLen = PrivateShopTitleChange(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_WARP_REQUEST:
			{
				PrivateShopWarpRequest(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_SLOT_UNLOCK_REQUEST:
			{
				if ((iExtraLen = PrivateShopSlotUnlockRequest(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_SEARCH_CLOSE:
			{
				PrivateShopSearchClose(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_SEARCH:
			{
				if ((iExtraLen = PrivateShopSearch(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_SEARCH_BUY:
			{
				if ((iExtraLen = PrivateShopSearchBuy(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_REQUEST:
			{
				PrivateShopMarketItemPriceDataRequest(ch);
			} break;

			case SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_REQUEST:
			{
				if ((iExtraLen = PrivateShopMarketItemPriceRequest(ch, c_pData, m_iBufferLeft)) < 0)
					return -1;
			} break;

			default:
				sys_err("Unknown private shop subheader %u", p->bSubHeader);
				break;
			}
		} break;
#endif

#if defined(ENABLE_REMOTE_SHOP)
		case HEADER_CG_REMOTE_SHOP:
			RemoteShop(ch, c_pData);
			break;
#endif

#ifdef ENABLE_HWID_BAN
		case HEADER_CG_HWID_SYSTEM:
			HWIDManager(ch, c_pData);
			break;
#endif

#ifdef ENABLE_HUNTING_SYSTEM
		case HEADER_CG_SEND_HUNTING_ACTION:
			ReciveHuntingAction(ch, c_pData);
			break;
#endif

#if defined(__BL_67_ATTR__)
		case HEADER_CG_67_ATTR:
			Attr67(ch, c_pData);
			break;
			
		case HEADER_CG_CLOSE_67_ATTR:
			Attr67Close(ch, c_pData);
			break;
#endif

#ifdef ENABLE_DUNGEON_INFO_SYSTEM
		case HEADER_CG_DUNGEON_INFO:
			DungeonInfo(ch, c_pData);
			break;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		case HEADER_CG_SKILL_COLOR:
			SetSkillColor(ch, c_pData);
			break;
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
		case HEADER_CG_CHANGE_COSTUME_VISIBILITY_STATUS:
			ChangeCostumeVisibilityStatus(ch, c_pData);
			break;
#endif

#ifdef ENABLE_ANTI_MULTIPLE_FARM
		case HEADER_CG_ANTI_FARM:
			if ((iExtraLen = RecvAntiFarmUpdateStatus(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef ENABLE_SPLIT_BY_COUNT
	case HEADER_CG_SPLIT_ITEM:
		if (!ch->IsObserverMode()) SendSplitItemByCount(ch, c_pData);
		break;
#endif

#ifdef ENABLE_BATTLE_PASS
	case HEADER_CG_BATTLE_PASS:
		if ((iExtraLen = BattlePass(ch, c_pData, m_iBufferLeft)) < 0)
			return -1;
		break;
#endif

#ifdef ENABLE_ITEMSHOP
	case HEADER_CG_BUY_ITEMSHOP_ITEM:
		BuyItemshopItem(ch, c_pData);
		break;

	case HEADER_CG_PROMOTION:
		RedeemPromotionCode(ch, c_pData);
		break;
#endif

#ifdef ENABLE_FAST_STACK
	case HEADER_CG_FAST_STACK:
		if (!ch->IsObserverMode()) FastStack(ch, c_pData);
		break;
#endif

#ifdef __GUILD_SAFEBOX__
	case HEADER_CG_GUILD_SAFEBOX_OPEN:
	case HEADER_CG_GUILD_SAFEBOX_CHECKIN:
	case HEADER_CG_GUILD_SAFEBOX_CHECKOUT:
	case HEADER_CG_GUILD_SAFEBOX_ITEM_MOVE:
	case HEADER_CG_GUILD_SAFEBOX_GIVE_GOLD:
	case HEADER_CG_GUILD_SAFEBOX_GET_GOLD:
		GuildSafebox(ch, bHeader, c_pData);
		break;
#endif

#ifdef ENABLE_TRASH_SYSTEM
	case HEADER_CG_TRASH:
		if ((iExtraLen = RecvTrashPacket(ch, c_pData)) < 0)
			return -1;
		break;
#endif

#ifdef ENABLE_CHEST_OPEN_RENEWAL
	case HEADER_CG_CHEST_OPEN:
		if (!ch->IsObserverMode())
			ChestOpen(ch, c_pData);
		break;
#endif
	}
	return (iExtraLen);
}

int CInputDead::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		return 0;
	}

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		default:
			return (0);
	}

	return (iExtraLen);
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
