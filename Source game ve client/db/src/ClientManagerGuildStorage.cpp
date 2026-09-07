#include "stdafx.h"
#include "../../common/CommonDefines.h"

#ifdef __WIN32__
#include <mysql/mysql.h>
#endif

#include "../../common/billing.h"
#include "../../common/building.h"
#include "../../common/VnumHelper.h"
#include "../../libgame/include/grid.h"
#include "Main.h"
#include "Config.h"
#include "DBManager.h"
#include "QID.h"
#include "Peer.h"
#include "ClientManager.h"
#include "Cache.h"

#ifndef UINT16_MAX
#define UINT16_MAX  ((uint16_t)-1)
#endif

bool CreateItemTableFromRes(MYSQL_RES * res, std::vector<TPlayerItem> * pVec, DWORD dwPID);

template <class T>
const char* Decode(T*& pObj, const char* data){
	pObj = (T*) data;
	return data + sizeof(T);
}

void CClientManager::RecvGuildStoragePacket(CPeer* peer, const char* data)
{
	TPacketGDGuildStorage* pack;
	data = Decode(pack, data);
	
	switch (pack->bSubHeader)
	{
		case SUBHEADER_GD_REQUEST_GS:
			RecvGuildStorageRequestStorage(peer, data);
			break;
		case SUBHEADER_GD_REQUEST_ITEM_REMOVE:
			RecvGuildStorageRequestItemRemove(peer, data);
			break;
		case SUBHEADER_GD_REQUEST_ITEM_MOVE:
			RecvGuildStorageRequestItemMove(peer, data);
			break;
		case SUBHEADER_GD_REQUEST_GOLD_ADD:
			RecvGuildStorageRequestGoldAdd(peer, data);
			break;
		case SUBHEADER_GD_REQUEST_GOLD_REMOVE:
			RecvGuildStorageRequestGoldRemove(peer, data);
			break;
		case SUBHEADER_GD_ADD_ITEM_ADD_LOG:
			RecvGuildStorageAddItemAddLog(peer, data);
			break;
		default:
			sys_err("Unkown subheader %d", pack->bSubHeader);
			break;
	}
}


bool CClientManager::RecvGuildStorageRequestStorage(CPeer* peer, const char* data)
{
	TSubPacketRequestGS* subpack;
	data = Decode(subpack, data);
	
	const auto gs = m_guildStorages.find(subpack->guildId);
	if (gs == m_guildStorages.end())
	{
		char szQuery[512];
		snprintf(szQuery, sizeof(szQuery),
#ifdef __CHANGE_LOOK_SYSTEM__
			"SELECT id, window+0, pos, count, vnum, dwVnum, socket0, socket1, socket2, "
#else
			"SELECT id, window+0, pos, count, vnum, socket0, socket1, socket2, "
#endif
			"attrtype0, attrvalue0, "
			"attrtype1, attrvalue1, "
			"attrtype2, attrvalue2, "
			"attrtype3, attrvalue3, "
			"attrtype4, attrvalue4, "
			"attrtype5, attrvalue5, "
			"attrtype6, attrvalue6 "
			"FROM item%s WHERE owner_id=%d AND window='%s' LIMIT 1200",
			GetTablePostfix(), subpack->guildId, "GUILDSTORAGE");

		TSubPacketRequestGS* sent = new TSubPacketRequestGS;
		sent->guildId = subpack->guildId;

		CDBManager::instance().ReturnQuery(szQuery, QID_GUILDSTORAGE_LOAD, peer->GetHandle(), sent);
		return true;
	}

	TPacketDGGuildStorage pack;
	pack.bSubHeader = SUBHEADER_DG_SEND_GS;

	TSubPacketSendGS sub_pack;
	sub_pack.guildId = subpack->guildId;
	sub_pack.itemCount = gs->second->m_items.size();

	peer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketSendGS) + sizeof(TPlayerItem)*gs->second->m_items.size());
	peer->Encode(&pack, sizeof(TPacketDGGuildStorage));
	peer->Encode(&sub_pack, sizeof(TSubPacketSendGS));

	for (const auto& item : gs->second->m_items)
	{
		peer->Encode(item.second.get(), sizeof(TPlayerItem));
	}

	

	TPacketDGGuildStorage pack2;
	pack2.bSubHeader = SUBHEADER_DG_SEND_GOLD_UPDATE;
	TSubPacketGSRequestGoldUpdate sub_packet2;
	sub_packet2.guildId = subpack->guildId;
	sub_packet2.gold = gs->second->gold;
	sub_packet2.player_gold = 0;
	sub_packet2.player_id = 0;

	peer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketGSRequestGoldUpdate));
	peer->Encode(&pack2, sizeof(TPacketDGGuildStorage));
	peer->Encode(&sub_packet2, sizeof(TSubPacketGSRequestGoldUpdate));


	TPacketDGGuildStorage pack3;
	pack3.bSubHeader = SUBHEADER_DG_SEND_LOGS;

	TSubPacketDGGSLog sub_pack3;
	sub_pack3.log_count = gs->second->m_logs.size();
	sub_pack3.guildId = subpack->guildId;

	peer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketDGGSLog) + sizeof(TGSLogEntry)*sub_pack3.log_count);
	peer->Encode(&pack3, sizeof(TPacketDGGuildStorage));
	peer->Encode(&sub_pack3, sizeof(TSubPacketDGGSLog));

	for (const auto& item : gs->second->m_logs)
	{
		peer->Encode(item.get(), sizeof(TGSLogEntry));
	}

	return true;
}


void CClientManager::QueryResultGuildStorage(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo)
{
	TSubPacketRequestGS* packet = (TSubPacketRequestGS*)(pQueryInfo->pvData);

	const auto gs = m_guildStorages.find(packet->guildId);
	if (gs != m_guildStorages.end())
	{
		delete packet;
		return;
	}

	auto guildStorage = std::make_shared<S_guildStorage>();

	static std::vector<TPlayerItem> s_items;
	CreateItemTableFromRes(msg->Get()->pSQLResult, &s_items, packet->guildId);

	for (size_t i = 0; i < s_items.size(); ++i)
	{
		TPlayerItem item = s_items.at(i);

		auto itm = std::make_shared<TPlayerItem>();
		memcpy(itm.get(), &item, sizeof(TPlayerItem));

		guildStorage->m_items.insert(std::make_pair(itm->pos, itm));
	}

	m_guildStorages.insert(std::make_pair(packet->guildId, guildStorage));

	TPacketDGGuildStorage pack;
	pack.bSubHeader = SUBHEADER_DG_SEND_GS;

	TSubPacketSendGS sub_pack;
	sub_pack.guildId = packet->guildId;
	sub_pack.itemCount = guildStorage->m_items.size();

	peer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketSendGS) + sizeof(TPlayerItem)*guildStorage->m_items.size());
	peer->Encode(&pack, sizeof(TPacketDGGuildStorage));
	peer->Encode(&sub_pack, sizeof(TSubPacketSendGS));

	for (const auto& item : guildStorage->m_items)
	{
		peer->Encode(item.second.get(), sizeof(TPlayerItem));
	}

	char szQuery[512];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT gold FROM guild_gold%s WHERE id=%d",
		GetTablePostfix(), packet->guildId);

	TSubPacketRequestGS* sent = new TSubPacketRequestGS;
	sent->guildId = packet->guildId;

	CDBManager::instance().ReturnQuery(szQuery, QID_GUILDSTORAGE_LOAD_GOLD, peer->GetHandle(), sent);

	delete packet;
}


void CClientManager::QueryResultGuildStorageAddItem(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo)
{
	TPlayerItem* packet = (TPlayerItem*)(pQueryInfo->pvData);

	const auto gs = m_guildStorages.find(packet->owner);
	if (gs == m_guildStorages.end())
	{
		delete packet;
		return;
	}		

	if (gs->second->m_items.find(packet->pos) != gs->second->m_items.end())
	{
		sys_err("GUILDSTORAGE: slot already in use ._.");
		int new_slot = -1;
		for (int i = 0; i < GUILD_STORAGE_SLOTS; i++)
		{
			if (gs->second->m_items.find(i) == gs->second->m_items.end())
			{
				new_slot = i;
				break;
			}
		}
		if (new_slot == -1)
		{
			sys_err("GUILDSTORAGE: SLOT REUSAGE! item will be available after reboot & clearing of the slot id: %d", packet->id);
			delete packet;
			return;
		}
		sys_err("GUILDSTORAGE: Found new slot, moving item");

		packet->pos = new_slot;
		QUERY_ITEM_SAVE(peer, reinterpret_cast<const char*>(packet));
		return;
	}

	auto itm = std::make_shared<TPlayerItem>();
	memcpy(itm.get(), packet, sizeof(TPlayerItem));

	gs->second->m_items.insert(std::make_pair(itm->pos, itm));

	TPacketDGGuildStorage pack;
	pack.bSubHeader = SUBHEADER_DG_SEND_ITEM_ADD;
	TSubPacketDGGSCheckin sub_pack;
	sub_pack.guildId = itm->owner;
	memcpy(&sub_pack.item, packet, sizeof(TPlayerItem));

	for (const auto cpeer : m_peerList)
	{
		cpeer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketDGGSCheckin));
		cpeer->Encode(&pack, sizeof(TPacketDGGuildStorage));
		cpeer->Encode(&sub_pack, sizeof(TSubPacketDGGSCheckin));
	}
	
	delete packet;
}

bool CClientManager::RecvGuildStorageRequestItemRemove(CPeer* peer, const char* data)
{
	TSubPacketGDGSCheckout* subpack;
	data = Decode(subpack, data);
	
	const auto gs = m_guildStorages.find(subpack->guildId);
	if (gs == m_guildStorages.end())
		return false;

	const auto item = gs->second->m_items.find(subpack->slot);
	if (item == gs->second->m_items.end())
		return false;

	AddGuildStorageLogEntry(peer, subpack->guildId, subpack->player_name, item->second->vnum, subpack->slot, UINT16_MAX, 0);
	gs->second->m_items.erase(subpack->slot);

	TPacketDGGuildStorage packDGR;
	packDGR.bSubHeader = SUBHEADER_DGR_SEND_ITEM_REMOVE;
	peer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketGDGSCheckout));
	peer->Encode(&packDGR, sizeof(TPacketDGGuildStorage));
	peer->Encode(subpack, sizeof(TSubPacketGDGSCheckout));

	
	TPacketDGGuildStorage pack;
	pack.bSubHeader = SUBHEADER_DG_SEND_ITEM_REMOVE;
	TSubPacketDGGSCheckout sub_pack;
	sub_pack.guildId = subpack->guildId;
	sub_pack.slot = subpack->slot;

	for (const auto cpeer : m_peerList)
	{
		if (cpeer->GetP2PPort() == peer->GetP2PPort())
			continue;

		cpeer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketDGGSCheckout));
		cpeer->Encode(&pack, sizeof(TPacketDGGuildStorage));
		cpeer->Encode(&sub_pack, sizeof(TSubPacketDGGSCheckout));
	}

	return true;
}

bool CClientManager::RecvGuildStorageRequestItemMove(CPeer* peer, const char* data)
{
	TSubPacketGDGSMove* subpack;
	data = Decode(subpack, data);
	
	const auto gs = m_guildStorages.find(subpack->guildId);
	if (gs == m_guildStorages.end())
		return false;

	auto item = gs->second->m_items.find(subpack->slot);
	if (item == gs->second->m_items.end())
		return false;

	if (gs->second->m_items.find(subpack->new_slot) != gs->second->m_items.end())
		return false;

	AddGuildStorageLogEntry(peer, subpack->guildId, subpack->player_name, item->second->vnum, subpack->slot, subpack->new_slot, 0);
	
	std::shared_ptr<TPlayerItem> itm = item->second;
	gs->second->m_items.erase(subpack->slot);

	itm->pos = subpack->new_slot;

	TPacketDGGuildStorage pack;
	pack.bSubHeader = SUBHEADER_DG_SEND_ITEM_REMOVE;
	TSubPacketDGGSCheckout sub_pack;
	sub_pack.guildId = subpack->guildId;
	sub_pack.slot = subpack->slot;

	for (const auto cpeer : m_peerList)
	{
		cpeer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketDGGSCheckout));
		cpeer->Encode(&pack, sizeof(TPacketDGGuildStorage));
		cpeer->Encode(&sub_pack, sizeof(TSubPacketDGGSCheckout));
	}

	QUERY_ITEM_SAVE(peer, reinterpret_cast<const char*>(itm.get()));
	return true;
}


bool CClientManager::RecvGuildStorageRequestGoldAdd(CPeer* peer, const char* data)
{
	TSubPacketGSRequestGoldUpdate* subpack;
	data = Decode(subpack, data);
	
	const auto gs = m_guildStorages.find(subpack->guildId);
	if (gs == m_guildStorages.end())
		return false;

	gs->second->gold += subpack->gold;

	AddGuildStorageLogEntry(peer, subpack->guildId, subpack->player_name, 0, 0, 0, subpack->gold);

	char szQuery[512];

	snprintf(szQuery, sizeof(szQuery),
		"REPLACE INTO guild_gold%s (id, gold) "
		"VALUES(%u, %llu)",
		GetTablePostfix(),
		subpack->guildId, gs->second->gold);

	subpack->player_id = 0;

	TSubPacketGSRequestGoldUpdate* pack = new TSubPacketGSRequestGoldUpdate;
	memcpy(pack, subpack, sizeof(TSubPacketGSRequestGoldUpdate));
	pack->gold = gs->second->gold;
	CDBManager::instance().ReturnQuery(szQuery, QID_GUILDSTORAGE_GOLDUPDATE, peer->GetHandle(), pack);
	return true;
}

bool CClientManager::RecvGuildStorageRequestGoldRemove(CPeer* peer, const char* data)
{
	TSubPacketGSRequestGoldUpdate* subpack;
	data = Decode(subpack, data);

	const auto gs = m_guildStorages.find(subpack->guildId);
	if (gs == m_guildStorages.end())
		return false;

	if (gs->second->gold < subpack->gold)
		return false;
	
	gs->second->gold -= subpack->gold;

	AddGuildStorageLogEntry(peer, subpack->guildId, subpack->player_name, 0, 0, 0, -1*subpack->gold);

	char szQuery[512];

	snprintf(szQuery, sizeof(szQuery),
		"REPLACE INTO guild_gold%s (id, gold) "
		"VALUES(%u, %llu)",
		GetTablePostfix(),
		subpack->guildId, gs->second->gold);

	TSubPacketGSRequestGoldUpdate* pack = new TSubPacketGSRequestGoldUpdate;
	memcpy(pack, subpack, sizeof(TSubPacketGSRequestGoldUpdate));
	pack->gold = gs->second->gold;
	pack->player_gold = subpack->gold;
	CDBManager::instance().ReturnQuery(szQuery, QID_GUILDSTORAGE_GOLDUPDATE, peer->GetHandle(), pack);
	return true;
}

void CClientManager::QueryResultGuildStorageGoldUpdate(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo)
{
	TSubPacketGSRequestGoldUpdate* packet = (TSubPacketGSRequestGoldUpdate*)(pQueryInfo->pvData);

	TPacketDGGuildStorage pack;
	pack.bSubHeader = SUBHEADER_DG_SEND_GOLD_UPDATE;

	peer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketGSRequestGoldUpdate));
	peer->Encode(&pack, sizeof(TPacketDGGuildStorage));
	peer->Encode(packet, sizeof(TSubPacketGSRequestGoldUpdate));
	
	packet->player_id = 0;
	
	for (const auto cpeer : m_peerList)
	{
		if (cpeer->GetP2PPort() == peer->GetP2PPort())
			continue;
		cpeer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketGSRequestGoldUpdate));
		cpeer->Encode(&pack, sizeof(TPacketDGGuildStorage));
		cpeer->Encode(packet, sizeof(TSubPacketGSRequestGoldUpdate));
	}

	delete packet;
}

void CClientManager::QueryResultGuildStorageGoldLoad(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo)
{
	TSubPacketRequestGS* packet = (TSubPacketRequestGS*)(pQueryInfo->pvData);

	long long gold = 0;

	if (msg->uiSQLErrno == 0)
	{
		while (MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult))
		{
			str_to_number(gold, row[0]);
			break;
		}
	}

	const auto gs = m_guildStorages.find(packet->guildId);
	if (gs != m_guildStorages.end())
	{
		gs->second->gold = gold;
	}

	if (gold != 0)
	{
		TPacketDGGuildStorage pack;
		pack.bSubHeader = SUBHEADER_DG_SEND_GOLD_UPDATE;
		TSubPacketGSRequestGoldUpdate subpack;
		subpack.gold = gold;
		subpack.player_id = 0;
		subpack.guildId = packet->guildId;

		for (const auto cpeer : m_peerList)
		{
			cpeer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketGSRequestGoldUpdate));
			cpeer->Encode(&pack, sizeof(TPacketDGGuildStorage));
			cpeer->Encode(&subpack, sizeof(TSubPacketGSRequestGoldUpdate));
		}
	}

	char szQuery[512];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT id, guild_id, name, UNIX_TIMESTAMP(time) as time, vnum, slot, slot_new, gold FROM guild_log%s WHERE guild_id=%d",
		GetTablePostfix(), packet->guildId);

	TSubPacketRequestGS* sent = new TSubPacketRequestGS;
	sent->guildId = packet->guildId;

	CDBManager::instance().ReturnQuery(szQuery, QID_GUILDSTORAGE_LOAD_LOG, peer->GetHandle(), sent);

	delete packet;
}

void CClientManager::QueryResultGuildStorageLogLoad(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo)
{
	TSubPacketRequestGS* packet = (TSubPacketRequestGS*)(pQueryInfo->pvData);

	const auto gs = m_guildStorages.find(packet->guildId);
	if (gs == m_guildStorages.end())
		return;

	auto res = msg->Get()->pSQLResult;
	if (!res)
		return;

	int rows;

	if ((rows = mysql_num_rows(res)) <= 0)
		return;

	gs->second->m_logs.clear();

	for (int i = 0; i < rows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		auto log = std::make_shared<TGSLogEntry>();

		int cur = 0;

		str_to_number(log->id, row[cur++]);
		str_to_number(log->guild_id, row[cur++]);
		memcpy(log->name, row[cur++], sizeof(log->name));
		str_to_number(log->time, row[cur++]);
		str_to_number(log->vnum, row[cur++]);
		str_to_number(log->slot, row[cur++]);
		str_to_number(log->slot_new, row[cur++]);
		str_to_number(log->gold, row[cur++]);
		gs->second->m_logs.push_back(std::move(log));
	}

	TPacketDGGuildStorage pack;
	pack.bSubHeader = SUBHEADER_DG_SEND_LOGS;

	TSubPacketDGGSLog sub_pack;
	sub_pack.guildId = packet->guildId;
	sub_pack.log_count = gs->second->m_logs.size();

	peer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TSubPacketDGGSLog) + sizeof(TGSLogEntry)*sub_pack.log_count);
	peer->Encode(&pack, sizeof(TPacketDGGuildStorage));
	peer->Encode(&sub_pack, sizeof(TSubPacketDGGSLog));

	for (const auto& item : gs->second->m_logs)
	{
		peer->Encode(item.get(), sizeof(TGSLogEntry));
	}
	
	delete packet;
}


bool CClientManager::RecvGuildStorageAddItemAddLog(CPeer* peer, const char* data)
{
	TSubPacketGDGSItemAddLog* subpack;
	data = Decode(subpack, data);

	AddGuildStorageLogEntry(peer, subpack->guildId, subpack->player_name, subpack->vnum, UINT16_MAX, subpack->slot, 0);
	return true;
}


void CClientManager::AddGuildStorageLogEntry(CPeer* peer, uint32_t guildId, const char* name, uint32_t vnum, uint16_t slot, uint16_t slot_new, long long gold)
{
	const auto gs = m_guildStorages.find(guildId);
	if (gs == m_guildStorages.end())
		return;
	
		auto entry = std::make_shared<TGSLogEntry>();

	entry->guild_id = guildId;
	memcpy(entry->name, name, sizeof(entry->name));
	entry->vnum = vnum;
	entry->slot = slot;
	entry->slot_new = slot_new;
	entry->gold = gold;
	entry->time = static_cast<long>(GetCurrentTime());
	
	gs->second->m_logs.push_back(entry);

	char szQuery[1024];

	snprintf(szQuery, sizeof(szQuery),
		"INSERT INTO guild_log%s (guild_id, name, time, vnum, slot, slot_new, gold) "
		"VALUES(%u, '%s', NOW(), %u, %u, %u, %lld)",
		GetTablePostfix(),
		entry->guild_id, entry->name, entry->vnum, entry->slot, entry->slot_new, entry->gold);

	CDBManager::instance().AsyncQuery(szQuery, SQL_PLAYER); // dont use direct query, flush when ever db core flushes

	TPacketDGGuildStorage pack;
	pack.bSubHeader = SUBHEADER_DG_SEND_LOGENTRY;

	for (const auto cpeer : m_peerList)
	{
		cpeer->EncodeHeader(HEADER_DG_GUILDSTORAGE, 0, sizeof(TPacketDGGuildStorage) + sizeof(TGSLogEntry));
		cpeer->Encode(&pack, sizeof(TPacketDGGuildStorage));
		cpeer->Encode(entry.get(), sizeof(TGSLogEntry));
	}
}