#include "stdafx.h"
#include "GuildStorageManager.h"
#ifdef __WIN32__
#include <mysql/mysql.h>
#endif

#include "char.h"
#include "desc.h"
#include "db.h"
#include "config.h"
#include "guild.h"
#include <vector>
#include "start_position.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "item.h"
#include "item_manager.h"
#include "questmanager.h"
#include "unique_item.h"



GuildStorageManager::GuildStorageManager()
{
	
}


GuildStorageManager::~GuildStorageManager()
{
	Initialize();
}

void GuildStorageManager::Initialize()
{
	sys_log(0, "<GuildStorageManager> Initialize");
	guildIdToStorage.clear();
}

bool GuildStorageManager::AddViewer(uint32_t guildId, LPCHARACTER ch)
{
	auto gs = GetGuildStorage(guildId);
	if (gs == nullptr)
		return false;

	if (gs->viewers.find(ch->GetPlayerID()) != gs->viewers.end())
		return false;

	ch->SetGuildStorageLoadTime();
	ch->SetOpenGuildStorage(true);

	gs->viewers.insert(std::make_pair(ch->GetPlayerID(), ch));

	{
		uint32_t leftItemcount = gs->m_items.size();


		TPacketGuildStorage pack;
		pack.header = HEADER_GC_GUILDSTORAGE;
		
		pack.subHeader = GUILDSTORAGE_GC_RECV_GSITEMS;

		TPacketGCGuildStorageOpenItems subpack;
		subpack.clear = true;
		subpack.item_count = leftItemcount > 50 ? 50 : leftItemcount; // Only send 50 items per packet

		pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageOpenItems) + sizeof(TPlayerItem) *subpack.item_count;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		uint32_t currentItemCount = 0;
		for (auto item : gs->m_items)
		{
			if (currentItemCount == 50)
			{
				if (leftItemcount > 0)
				{
					ch->GetDesc()->Packet(buff.read_peek(), buff.size());

					buff.reset();
					currentItemCount = 0;
					subpack.item_count = leftItemcount > 50 ? 50 : leftItemcount; // Only send 50 items per packet
					subpack.clear = false;
					pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageOpenItems) + sizeof(TPlayerItem) *subpack.item_count;

					buff.write(&pack, sizeof(pack));
					buff.write(&subpack, sizeof(subpack));
				}
			}
			buff.write(item.second.get(), sizeof(TPlayerItem));
			currentItemCount++;
			leftItemcount--;
		}

		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}

	{
		TPacketGuildStorage pack;
		pack.header = HEADER_GC_GUILDSTORAGE;
		pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageOpen);
		pack.subHeader = GUILDSTORAGE_GC_RECV_OPEN;

		TPacketGCGuildStorageOpen subpack;
		subpack.isAdmin = ch->GetGuild()->GetMember(ch->GetPlayerID())->grade == GUILD_LEADER_GRADE;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));
		ch->GetDesc()->Packet(buff.read_peek(), buff.size());

	}

	{
		TPacketGuildStorage pack;
		pack.header = HEADER_GC_GUILDSTORAGE;
		pack.size = sizeof(pack) + sizeof(TPacketGuildStorageGoldUpdate);
		pack.subHeader = GUILDSTORAGE_GC_GOLD_UPDATE;

		TPacketGuildStorageGoldUpdate subpack;
		subpack.gold = gs->gold;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}

	{
		uint32_t leftLogCount = gs->m_logs.size();


		TPacketGuildStorage pack;
		pack.header = HEADER_GC_GUILDSTORAGE;

		pack.subHeader = GUILDSTORAGE_GC_RECV_LOG;

		TPacketGCGuildStorageLog subpack;
		subpack.clear = true;
		subpack.log_count = leftLogCount > 50 ? 50 : leftLogCount; // Only send 50 items per packet

		pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageLog) + sizeof(TGSLogEntry) *subpack.log_count;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		uint32_t currentLogCount = 0;
		for (auto item : gs->m_logs)
		{
			if (currentLogCount == 50)
			{
				if (leftLogCount > 0)
				{
					ch->GetDesc()->Packet(buff.read_peek(), buff.size());

					buff.reset();
					currentLogCount = 0;
					subpack.log_count = leftLogCount > 50 ? 50 : leftLogCount; // Only send 50 logs per packet
					subpack.clear = false;
					pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageLog) + sizeof(TGSLogEntry) *subpack.log_count;

					buff.write(&pack, sizeof(pack));
					buff.write(&subpack, sizeof(subpack));
				}
			}
			buff.write(item.get(), sizeof(TGSLogEntry));
			currentLogCount++;
			leftLogCount--;
		}

		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}

	return true;
}

bool GuildStorageManager::RemoveViewer(uint32_t guildId, LPCHARACTER ch)
{
	ch->SetGuildStorageLoadTime();
	ch->SetOpenGuildStorage(false);

	{
		TPacketGuildStorage pack;
		pack.header = HEADER_GC_GUILDSTORAGE;
		pack.size = sizeof(pack);
		pack.subHeader = GUILDSTORAGE_GC_RECV_CLOSE;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	}
	
	auto gs = GetGuildStorage(guildId);
	if (gs == nullptr)
		return false;

	if (gs->viewers.find(ch->GetPlayerID()) == gs->viewers.end())
		return true;

	gs->viewers.erase(ch->GetPlayerID());
	return true;
}

bool GuildStorageManager::OpenGuildStorage(LPCHARACTER ch)
{
	if (ch == nullptr || ch->GetGuild() == nullptr)
		return false;

	if (!ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "Du kannst das Gildenlager momentan nicht öffnen!");
		return false;
	}

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU HAVE BLOCK STATUS, OPEN A SUPPORT TICKET"));
		return false;
	}
	// END_OF_BLOCK_PLAYER

	const auto grade = ch->GetGuild()->GetMember(ch->GetPlayerID())->grade;
	if (grade != GUILD_LEADER_GRADE &&
		ch->GetGuild()->HasGradeAuth(grade, GUILD_AUTH_GS_CHECKIN) == false &&
		ch->GetGuild()->HasGradeAuth(grade, GUILD_AUTH_GS_CHECKOUT) == false &&
		ch->GetGuild()->HasGradeAuth(grade, GUILD_AUTH_GS_GOLDIN) == false &&
		ch->GetGuild()->HasGradeAuth(grade, GUILD_AUTH_GS_GOLDOUT) == false)
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "Du hast keine Berechtigungen im Gildenlager und darfst es daher nicht oeffnen!");
		return false;
	}

	ch->SetGuildStorageLoadTime();
	
	const auto guildStorage = GetGuildStorage(ch->GetGuild()->GetID());
	if (guildStorage == nullptr)
	{
		waitingCharQueue.push_back(ch);
		
		TPacketGDGuildStorage pack;
		pack.bSubHeader = SUBHEADER_GD_REQUEST_GS;

		TSubPacketRequestGS subpack;
		subpack.guildId = ch->GetGuild()->GetID();

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		db_clientdesc->DBPacket(HEADER_GD_GUILDSTORAGE, 0, buff.read_peek(), buff.size());

		return true;
	}

	
	return AddViewer(ch->GetGuild()->GetID(), ch);
}

bool GuildStorageManager::CloseGuildStorage(LPCHARACTER ch)
{
	if (ch == nullptr || ch->GetGuild() == nullptr)
		return false;
	return RemoveViewer(ch->GetGuild()->GetID(), ch);
}

bool GuildStorageManager::IsLoadedGuildStorage(uint32_t guildId)
{
	if (GetGuildStorage(guildId) == nullptr)
		return false;
	return true;
}

bool GuildStorageManager::AddItem(LPCHARACTER ch, TItemPos slot, uint16_t new_slot)
{
	if (ch == nullptr || ch->GetGuild() == nullptr)
		return false;

	if (ch->GetGuild()->HasGradeAuth(ch->GetGuild()->GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_GS_CHECKIN) == false)
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "Du hast keine Berechtigungen um Items einzulagern!");
		return false;
	}

	if (new_slot < 0 || new_slot >= GUILD_STORAGE_SLOTS)
		return false;

	auto gs = GetGuildStorage(ch->GetGuild()->GetID());
	if (gs == nullptr)
		return false;

	if (gs->viewers.find(ch->GetPlayerID()) == gs->viewers.end())
		return false;

	const auto item = gs->m_items.find(new_slot);
	if (item != gs->m_items.end())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return false;
	
	if (ch->GetExchange() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen() || ch->IsOpenSafebox() || ch->IsWarping())
		return false;
	
	LPITEM pkItem = ch->GetItem(slot);

	if (!pkItem)
		return false;

	if (pkItem->IsEquipped())
		return false;

	if (pkItem->IsDragonSoul()) // maybe change later
		return false;

#ifdef ENABLE_SPECIAL_INVENTORY
	if (slot.IsSpecialInventoryPosition() == false && pkItem->GetCell() >= INVENTORY_MAX_NUM)
		return false;

	if (IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Ã¢°í·Î ¿Å±æ ¼ö ¾ø´Â ¾ÆÀÌÅÛ ÀÔ´Ï´Ù."));
		return false;
	}
#else
	if (pkItem->GetCell() >= INVENTORY_MAX_NUM || IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Ã¢°í·Î ¿Å±æ ¼ö ¾ø´Â ¾ÆÀÌÅÛ ÀÔ´Ï´Ù."));
		return false;
	}
#endif

	if (pkItem->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ÀÌ ¾ÆÀÌÅÛÀº ³ÖÀ» ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	if (IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ÀÌ ¾ÆÀÌÅÛÀº ³ÖÀ» ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	if (true == pkItem->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ÀÌ ¾ÆÀÌÅÛÀº ³ÖÀ» ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	
	pkItem->SetSkipSave(true); // Disable Saving
	pkItem->RemoveFromCharacter(); // Remove from Character
	ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, slot.cell, 255);

	
	// Set To GuildStorage & Save

	TPlayerItem t;

	t.id = pkItem->GetID();
	t.window = GUILDSTORAGE;
	t.pos = new_slot;
	t.count = pkItem->GetCount();
	t.vnum = pkItem->GetOriginalVnum();
	t.owner = ch->GetGuild()->GetID();
	thecore_memcpy(t.alSockets, pkItem->GetSockets(), sizeof(t.alSockets));
	thecore_memcpy(t.aAttr, pkItem->GetAttributes(), sizeof(t.aAttr));

	// Remove item from queues and normal item cache
	M2_DESTROY_ITEM(pkItem);

	// Send new item to DB Core
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_SAVE, 0, sizeof(TPlayerItem));
	db_clientdesc->Packet(&t, sizeof(TPlayerItem));

	//Flush item
	DWORD dwID = t.id;
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));

	//Send Log Entry
	TPacketGDGuildStorage pack;
	pack.bSubHeader = SUBHEADER_GD_ADD_ITEM_ADD_LOG;

	TSubPacketGDGSItemAddLog subpack;
	subpack.guildId = ch->GetGuild()->GetID();
	memcpy(subpack.player_name, ch->GetName(), sizeof(subpack.player_name));
	subpack.slot = new_slot;
	subpack.vnum = t.vnum;

	TEMP_BUFFER buff;
	buff.write(&pack, sizeof(pack));
	buff.write(&subpack, sizeof(subpack));

	db_clientdesc->DBPacket(HEADER_GD_GUILDSTORAGE, 0, buff.read_peek(), buff.size());
	return true;
}

bool GuildStorageManager::RemoveItem(LPCHARACTER ch, int16_t slot, TItemPos new_slot)
{
	if (ch == nullptr || ch->GetGuild() == nullptr)
		return false;

	if (ch->GetGuild()->HasGradeAuth(ch->GetGuild()->GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_GS_CHECKOUT) == false)
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "Du hast keine Berechtigungen um Items auszulagern!");
		return false;
	}

	if (slot < 0 || slot >= GUILD_STORAGE_SLOTS)
		return false;

	if (ch->GetExchange() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen() || ch->IsOpenSafebox() || ch->IsWarping())
		return false;
	
	auto gs = GetGuildStorage(ch->GetGuild()->GetID());
	if (gs == nullptr)
		return false;

	if (gs->viewers.find(ch->GetPlayerID()) == gs->viewers.end())
		return false;

	const auto item = gs->m_items.find(slot);
	
	if (item == gs->m_items.end())
		return false;
	
	if (ch->GetItem(new_slot) != nullptr)
		return false;

	const auto item_table = ITEM_MANAGER::Instance().GetTable(item->second->vnum);

	if (!item_table)
		return false;
	
#ifdef ENABLE_SPECIAL_INVENTORY
	if (CItem::GetSpecialInventoryTypeS(item->second->vnum, item_table->bType, item_table->bSubType) != new_slot.GetSpecialInventoryType())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Dieses Item ist nicht fuer dieses Inventar geeignet.");
		return false;
	}
#else
	if (!ch->IsValidItemPosition(new_slot))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Dieses Item ist nicht fuer dieses Inventar geeignet.");
		return false;
	}
#endif

	TPacketGDGuildStorage pack;
	pack.bSubHeader = SUBHEADER_GD_REQUEST_ITEM_REMOVE;

	TSubPacketGDGSCheckout subpack;
	subpack.guildId = ch->GetGuild()->GetID();
	subpack.player_id = ch->GetPlayerID();
	subpack.slot = slot;
	subpack.inventoryPos = new_slot;
	memcpy(subpack.player_name, ch->GetName(), sizeof(subpack.player_name));

	TEMP_BUFFER buff;
	buff.write(&pack, sizeof(pack));
	buff.write(&subpack, sizeof(subpack));

	db_clientdesc->DBPacket(HEADER_GD_GUILDSTORAGE, 0, buff.read_peek(), buff.size());
	return false;
}

bool GuildStorageManager::MoveItem(LPCHARACTER ch, uint16_t slot, uint16_t new_slot)
{
	if (ch == nullptr || ch->GetGuild() == nullptr)
		return false;

	if (ch->GetGuild()->HasGradeAuth(ch->GetGuild()->GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_GS_CHECKOUT) == false)
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "Du hast keine Berechtigungen um Items zu verschieben!");
		return false;
	}

	if (slot < 0 || slot >= GUILD_STORAGE_SLOTS)
		return false;
	
	if (new_slot < 0 || new_slot >= GUILD_STORAGE_SLOTS)
		return false;

	auto gs = GetGuildStorage(ch->GetGuild()->GetID());
	if (gs == nullptr)
		return false;

	if (gs->viewers.find(ch->GetPlayerID()) == gs->viewers.end())
		return false;

	const auto item = gs->m_items.find(slot);
	if (item == gs->m_items.end())
		return false;

	const auto item2 = gs->m_items.find(new_slot);
	if (item2 != gs->m_items.end())
		return false;

	TPacketGDGuildStorage pack;
	pack.bSubHeader = SUBHEADER_GD_REQUEST_ITEM_MOVE;

	TSubPacketGDGSMove subpack;
	subpack.guildId = ch->GetGuild()->GetID();
	subpack.slot = slot;
	subpack.new_slot = new_slot;
	memcpy(subpack.player_name, ch->GetName(), sizeof(subpack.player_name));

	TEMP_BUFFER buff;
	buff.write(&pack, sizeof(pack));
	buff.write(&subpack, sizeof(subpack));

	db_clientdesc->DBPacket(HEADER_GD_GUILDSTORAGE, 0, buff.read_peek(), buff.size());
	return false;
}

bool GuildStorageManager::AddGold(LPCHARACTER ch, long long gold)
{
	if (ch == nullptr || ch->GetGuild() == nullptr)
		return false;

	if (ch->GetGuild()->HasGradeAuth(ch->GetGuild()->GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_GS_GOLDIN) == false)
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "Du hast keine Berechtigungen um Geld einzuzahlen!");
		return false;
	}

	auto gs = GetGuildStorage(ch->GetGuild()->GetID());
	if (gs == nullptr)
		return false;

	if (gs->viewers.find(ch->GetPlayerID()) == gs->viewers.end())
		return false;

	if (gold <= 0)
		return false;
	
	// if (ch->GetGold() < gold)
	if (ch->GetGold() < static_cast<unsigned long long>(gold))
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "So viel Yang hast du nicht..");
		return false;
	}

	if (gs->gold + gold > GUILDSTORAGE_MAX_GOLD)
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "Mehr Yang kann die Gilde nicht lagern..");
		return false;
	}

	// ch->PointChange(POINT_GOLD, -gold, false);
	ch->ChangeGold(-(long long)gold);
	
	ch->ChatPacket(CHAT_TYPE_GUILD, "[debug]You want to add %lld yang", gold);

	TPacketGDGuildStorage pack;
	pack.bSubHeader = SUBHEADER_GD_REQUEST_GOLD_ADD;

	TSubPacketGSRequestGoldUpdate subpack;
	subpack.guildId = ch->GetGuild()->GetID();
	subpack.player_id = ch->GetPlayerID();
	subpack.gold = gold;
	memcpy(subpack.player_name, ch->GetName(), sizeof(subpack.player_name));

	TEMP_BUFFER buff;
	buff.write(&pack, sizeof(pack));
	buff.write(&subpack, sizeof(subpack));

	db_clientdesc->DBPacket(HEADER_GD_GUILDSTORAGE, 0, buff.read_peek(), buff.size());

	return false;
}

bool GuildStorageManager::RemoveGold(LPCHARACTER ch, long long gold)
{
	if (ch == nullptr || ch->GetGuild() == nullptr)
		return false;

	if (ch->GetGuild()->HasGradeAuth(ch->GetGuild()->GetMember(ch->GetPlayerID())->grade, GUILD_AUTH_GS_GOLDOUT) == false)
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "Du hast keine Berechtigungen um Geld auszuzahlen!");
		return false;
	}

	auto gs = GetGuildStorage(ch->GetGuild()->GetID());
	if (gs == nullptr)
		return false;

	if (gs->viewers.find(ch->GetPlayerID()) == gs->viewers.end())
		return false;

	if (gold <= 0)
		return false;

	if (static_cast<unsigned long long>(ch->GetGold() + gold) > ch->GetAllowedGold())//GOLD_MAX
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "So viel Yang kannst du nicht tragen..");
		return false;
	}

	if (gs->gold < gold)
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "So viel yang besitzt die gilde nicht..");
		return false;
	}
	
	// fixme @grm
	unsigned long long nTotalMoney = static_cast<unsigned long long>(ch->GetGold());
	nTotalMoney += static_cast<unsigned long long>(gold);
	
	if (ch->GetAllowedGold() <= nTotalMoney)
	{
		ch->ChatPacket(CHAT_TYPE_GUILD, "So viel Yang kannst du nicht tragen..");
		return false;
	}

	ch->ChatPacket(CHAT_TYPE_GUILD, "[debug]You want to remove %lld yang", gold);


	TPacketGDGuildStorage pack;
	pack.bSubHeader = SUBHEADER_GD_REQUEST_GOLD_REMOVE;

	TSubPacketGSRequestGoldUpdate subpack;
	subpack.guildId = ch->GetGuild()->GetID();
	subpack.player_id = ch->GetPlayerID();
	subpack.gold = gold;
	memcpy(subpack.player_name, ch->GetName(), sizeof(subpack.player_name));

	TEMP_BUFFER buff;
	buff.write(&pack, sizeof(pack));
	buff.write(&subpack, sizeof(subpack));

	db_clientdesc->DBPacket(HEADER_GD_GUILDSTORAGE, 0, buff.read_peek(), buff.size());

	return false;
}

void GuildStorageManager::RecvSlots(uint32_t guildId, std::vector<TPlayerItem>* items)
{
	auto guildStorage = GetGuildStorage(guildId);

	if (guildStorage != nullptr)
	{
		std::vector<LPCHARACTER> toRemove;
		for (auto ch : waitingCharQueue)
		{
			if (ch != nullptr && ch->GetGuild() != nullptr && ch->GetGuild()->GetID() == guildId)
			{
				AddViewer(ch->GetGuild()->GetID(), ch);
				toRemove.push_back(ch);
			}
		}

		for (LPCHARACTER ch : toRemove)
		{
			waitingCharQueue.erase(std::find(waitingCharQueue.begin(), waitingCharQueue.end(), ch));
		}
		return;
	}

	auto gs = std::make_shared<SGAME_guildStorage>();
	guildIdToStorage.insert(std::make_pair(guildId, gs));

	guildStorage = GetGuildStorage(guildId);

	for (size_t i = 0; i < items->size(); ++i)
	{
		TPlayerItem item = items->at(i);

		auto itm = std::make_shared<TPlayerItem>();
		memcpy(itm.get(), &item, sizeof(TPlayerItem));
		
		guildStorage->m_items.insert(std::make_pair(itm->pos, itm));
	}

	
	std::vector<LPCHARACTER> toRemove;
	for (auto ch : waitingCharQueue)
	{
		if (ch != nullptr && ch->GetGuild() != nullptr && ch->GetGuild()->GetID() == guildId)
		{
			AddViewer(ch->GetGuild()->GetID(), ch);
			toRemove.push_back(ch);
		}
	}

	for (LPCHARACTER ch : toRemove)
	{
		waitingCharQueue.erase(std::find(waitingCharQueue.begin(), waitingCharQueue.end(), ch));
	}
}

void GuildStorageManager::RecvSlotLock(uint32_t guildId, uint16_t slot)
{
}

void GuildStorageManager::RecvSlotUnlock(uint32_t guildId, uint16_t slot)
{
}

void GuildStorageManager::RecvItemAdd(uint32_t guildId, TPlayerItem item)
{
	auto gs = GetGuildStorage(guildId);
	if (gs != nullptr)
	{
		auto itm = std::make_shared<TPlayerItem>();
		memcpy(itm.get(), &item, sizeof(TPlayerItem));

		gs->m_items.insert(std::make_pair(itm->pos, itm));

		TPacketGuildStorage pack;
		pack.header = HEADER_GC_GUILDSTORAGE;
		pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageAddItem);
		pack.subHeader = GUILDSTORAGE_GC_ADD_ITEM;
		
		TPacketGCGuildStorageAddItem subpack;
		subpack.item = item;
		
		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		for (const auto viewer : gs->viewers)
		{
			if (viewer.second == nullptr || viewer.second->GetDesc() == nullptr)
				continue;

			viewer.second->GetDesc()->Packet(buff.read_peek(), buff.size());
		}
	}
}

void GuildStorageManager::RecvItemRemove(uint32_t guildId, uint16_t slot)
{
	auto gs = GetGuildStorage(guildId);
	if (gs != nullptr)
	{
		if (gs->m_items.find(slot) != gs->m_items.end())
		{
			gs->m_items.erase(slot);
			
			TPacketGuildStorage pack;
			pack.header = HEADER_GC_GUILDSTORAGE;
			pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageRemoveItem);
			pack.subHeader = GUILDSTORAGE_GC_REMOVE_ITEM;

			TPacketGCGuildStorageRemoveItem subpack;
			subpack.slot = slot;

			TEMP_BUFFER buff;
			buff.write(&pack, sizeof(pack));
			buff.write(&subpack, sizeof(subpack));
					
			for (const auto viewer : gs->viewers)
			{
				if (viewer.second == nullptr || viewer.second->GetDesc() == nullptr)
					continue;

				viewer.second->GetDesc()->Packet(buff.read_peek(), buff.size());
			}
		}
	}
}

void GuildStorageManager::RecvGoldUpdate(uint32_t guildId, long long gold, uint32_t player_id, long long player_gold)
{
	auto gs = GetGuildStorage(guildId);
	if (gs != nullptr)
	{
		gs->gold = gold;
		TPacketGuildStorage pack;
		pack.header = HEADER_GC_GUILDSTORAGE;
		pack.size = sizeof(pack) + sizeof(TPacketGuildStorageGoldUpdate);
		pack.subHeader = GUILDSTORAGE_GC_GOLD_UPDATE;

		TPacketGuildStorageGoldUpdate subpack;
		subpack.gold = gold;

		TEMP_BUFFER buff;
		buff.write(&pack, sizeof(pack));
		buff.write(&subpack, sizeof(subpack));

		for (const auto viewer : gs->viewers)
		{
			if (viewer.second == nullptr || viewer.second->GetDesc() == nullptr)
				continue;

			viewer.second->GetDesc()->Packet(buff.read_peek(), buff.size());

			if (player_id != 0)
			{
				if (viewer.second->GetPlayerID() == player_id)
				{					
					// viewer.second->PointChange(POINT_GOLD, player_gold, false);
					viewer.second->ChangeGold((long long)player_gold);
					viewer.second->ChatPacket(CHAT_TYPE_GUILD, "[debug]You received %lld yang", player_gold);
					
					player_id = 0;
				}
			}
		}
	}
}

void GuildStorageManager::RecvLogs(uint32_t guildId, std::vector<TGSLogEntry>* logs)
{
	auto guildStorage = GetGuildStorage(guildId);

	if (guildStorage == nullptr)
		return;
	
	for (size_t i = 0; i < logs->size(); ++i)
	{
		TGSLogEntry item = logs->at(i);

		auto itm = std::make_shared<TGSLogEntry>();
		memcpy(itm.get(), &item, sizeof(TGSLogEntry));

		guildStorage->m_logs.push_back(itm);
	}

	uint32_t leftLogCount = guildStorage->m_logs.size();


	TPacketGuildStorage pack;
	pack.header = HEADER_GC_GUILDSTORAGE;

	pack.subHeader = GUILDSTORAGE_GC_RECV_LOG;

	TPacketGCGuildStorageLog subpack;
	subpack.clear = true;
	subpack.log_count = leftLogCount > 50 ? 50 : leftLogCount; // Only send 50 items per packet

	pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageLog) + sizeof(TGSLogEntry) *subpack.log_count;

	TEMP_BUFFER buff;
	buff.write(&pack, sizeof(pack));
	buff.write(&subpack, sizeof(subpack));

	uint32_t currentLogCount = 0;
	for (auto item : guildStorage->m_logs)
	{
		if (currentLogCount == 50)
		{
			if (leftLogCount > 0)
			{
				for (const auto viewer : guildStorage->viewers)
				{
					if (viewer.second == nullptr || viewer.second->GetDesc() == nullptr)
						continue;

					viewer.second->GetDesc()->Packet(buff.read_peek(), buff.size());
				}

				buff.reset();
				currentLogCount = 0;
				subpack.log_count = leftLogCount > 50 ? 50 : leftLogCount; // Only send 50 logs per packet
				subpack.clear = false;
				pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageLog) + sizeof(TGSLogEntry) *subpack.log_count;

				buff.write(&pack, sizeof(pack));
				buff.write(&subpack, sizeof(subpack));
			}
		}
		buff.write(item.get(), sizeof(TGSLogEntry));
		currentLogCount++;
		leftLogCount--;
	}

	for (const auto viewer : guildStorage->viewers)
	{
		if (viewer.second == nullptr || viewer.second->GetDesc() == nullptr)
			continue;

		viewer.second->GetDesc()->Packet(buff.read_peek(), buff.size());
	}
}

void GuildStorageManager::RecvLogEntry(uint32_t guildId, TGSLogEntry* log)
{
	auto guildStorage = GetGuildStorage(guildId);

	if (guildStorage == nullptr)
		return;

	auto itm = std::make_shared<TGSLogEntry>();
	memcpy(itm.get(), log, sizeof(TGSLogEntry));

	guildStorage->m_logs.push_back(itm);

	TPacketGuildStorage pack;
	pack.header = HEADER_GC_GUILDSTORAGE;
	pack.subHeader = GUILDSTORAGE_GC_SEND_LOGENTRY;
	pack.size = sizeof(pack) + sizeof(TGSLogEntry);

	TEMP_BUFFER buff;
	buff.write(&pack, sizeof(pack));
	buff.write(log, sizeof(TGSLogEntry));
	
	for (const auto viewer : guildStorage->viewers)
	{
		if (viewer.second == nullptr || viewer.second->GetDesc() == nullptr)
			continue;

		viewer.second->GetDesc()->Packet(buff.read_peek(), buff.size());
	}
}

void GuildStorageManager::RecvRequestedItemRemove(uint32_t guildId, uint32_t player_id, uint16_t slot, TItemPos itemPos)
{
	auto gs = GetGuildStorage(guildId);
	if (gs == nullptr)
	{
		sys_log(0, "GuildStorageManager::RecvRequestedItemRemove How the fuck should I requested this?");
		return;
	}

	LPCHARACTER ch = gs->viewers.find(player_id)->second;
	
	auto pItem = gs->m_items.find(slot)->second;
	gs->m_items.erase(gs->m_items.find(slot));

	TPacketGuildStorage pack;
	pack.header = HEADER_GC_GUILDSTORAGE;
	pack.subHeader = GUILDSTORAGE_GC_REMOVE_ITEM;

	pack.size = sizeof(pack) + sizeof(TPacketGCGuildStorageRemoveItem);

	TPacketGCGuildStorageRemoveItem subpack;
	subpack.slot = slot;

	TEMP_BUFFER buff;
	buff.write(&pack, sizeof(pack));
	buff.write(&subpack, sizeof(subpack));
	//ch->GetDesc()->Packet(buff.read_peek(), buff.size());
	for (const auto viewer : gs->viewers)
	{
		if (viewer.second == nullptr || viewer.second->GetDesc() == nullptr)
			continue;

		viewer.second->GetDesc()->Packet(buff.read_peek(), buff.size());
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(pItem->vnum, pItem->count, pItem->id);

	if (!item)
	{
		sys_err("cannot create item vnum %d id %u", pItem->vnum, pItem->id);
		return;
	}

	item->SetSkipSave(true);
	item->SetSockets(pItem->alSockets);
	item->SetAttributes(pItem->aAttr);
	item->SetSkipSave(false);

	item->AddToCharacter(ch, itemPos); // Add catch...
	ITEM_MANAGER::instance().FlushDelayedSave(item);
	
	DWORD dwID = item->GetID();
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));
}

