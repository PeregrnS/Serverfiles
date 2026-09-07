#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "shopEx.h"
#include "group_text_parse_tree.h"
#ifdef ENABLE_BATTLE_PASS
	#include "battle_pass.h"
#endif

bool CShopEx::Create(DWORD dwVnum, DWORD dwNPCVnum)
{
	m_dwVnum = dwVnum;
	m_dwNPCVnum = dwNPCVnum;
	return true;
}

bool CShopEx::AddShopTable(TShopTableEx& shopTable)
{
	for (itertype(m_vec_shopTabs) it = m_vec_shopTabs.begin(); it != m_vec_shopTabs.end(); it++)
	{
		const TShopTableEx& _shopTable = *it;
		if (0 != _shopTable.dwVnum && _shopTable.dwVnum == shopTable.dwVnum)
			return false;
		if (0 != _shopTable.dwNPCVnum && _shopTable.dwNPCVnum == shopTable.dwVnum)
			return false;
	}
	m_vec_shopTabs.emplace_back(shopTable);
	return true;
}

bool CShopEx::AddGuest(LPCHARACTER ch,DWORD owner_vid, bool bOtherEmpire)
{
	if (!ch)
		return false;

	if (ch->GetExchange())
		return false;

	if (ch->GetShop())
		return false;

	ch->SetShop(this);

	m_map_guest.emplace(ch, bOtherEmpire);

	TPacketGCShop pack;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_START_EX;

	TPacketGCShopStartEx pack2;

	memset(&pack2, 0, sizeof(pack2));

	pack2.owner_vid = owner_vid;
	pack2.shop_tab_count = m_vec_shopTabs.size();
#if defined(ENABLE_RENEWAL_SHOPEX)
	char temp[8096 * SHOP_TAB_COUNT_MAX];
#else
	char temp[8096];
#endif
	char* buf = &temp[0];
	size_t size = 0;
	for (itertype(m_vec_shopTabs) it = m_vec_shopTabs.begin(); it != m_vec_shopTabs.end(); it++)
	{
		const TShopTableEx& shop_tab = *it;
		TPacketGCShopStartEx::TSubPacketShopTab pack_tab;
		pack_tab.coin_type = shop_tab.coinType;
		memcpy(pack_tab.name, shop_tab.name.c_str(), SHOP_TAB_NAME_MAX);

		for (BYTE i = 0; i < SHOP_HOST_ITEM_MAX_NUM; i++)
		{
			pack_tab.items[i].vnum = shop_tab.items[i].vnum;
			pack_tab.items[i].count = shop_tab.items[i].count;
#if defined(ENABLE_RENEWAL_SHOPEX)
			pack_tab.items[i].price_type = shop_tab.items[i].price_type;
			//pack_tab.items[i].price_vnum = shop_tab.items[i].price_vnum;
			pack_tab.items[i].price = shop_tab.items[i].price;
			if ((bOtherEmpire  && !g_bEmpireShopPriceTripleDisable) && pack_tab.items[i].price_type == SHOPEX_GOLD)
				pack_tab.items[i].price *= 1;
			for (int j = 0; j < SHOP_MAX_ITEM_NUM; j++)
			{
				pack_tab.items[i].item_vnum[j] = shop_tab.items[i].item_vnum[j];
				pack_tab.items[i].item_price[j] = shop_tab.items[i].item_price[j];
			}
			thecore_memcpy(pack_tab.items[i].aAttr, shop_tab.items[i].aAttr, sizeof(pack_tab.items[i].aAttr));
			thecore_memcpy(pack_tab.items[i].alSockets, shop_tab.items[i].alSockets, sizeof(pack_tab.items[i].alSockets));
#else
			switch(shop_tab.coinType)
			{
			case SHOP_COIN_TYPE_GOLD:
#ifdef ENABLE_NEWSTUFF
				if (bOtherEmpire && !g_bEmpireShopPriceTripleDisable) // no empire price penalty for pc shop
#else
				if (bOtherEmpire) // no empire price penalty for pc shop
#endif
					pack_tab.items[i].price = shop_tab.items[i].price * 1;
				else
					pack_tab.items[i].price = shop_tab.items[i].price;
				break;
			case SHOP_COIN_TYPE_SECONDARY_COIN:
				pack_tab.items[i].price = shop_tab.items[i].price;
				break;
			}
			memset(pack_tab.items[i].aAttr, 0, sizeof(pack_tab.items[i].aAttr));
			memset(pack_tab.items[i].alSockets, 0, sizeof(pack_tab.items[i].alSockets));
#endif
		}

		memcpy(buf, &pack_tab, sizeof(pack_tab));
		buf += sizeof(pack_tab);
		size += sizeof(pack_tab);
	}

	pack.size = sizeof(pack) + sizeof(pack2) + size;

	ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
	ch->GetDesc()->BufferedPacket(&pack2, sizeof(TPacketGCShopStartEx));
	ch->GetDesc()->Packet(temp, size);

	return true;
}

int CShopEx::Buy(LPCHARACTER ch, BYTE pos)
{
	BYTE tabIdx = pos / SHOP_HOST_ITEM_MAX_NUM;
	BYTE slotPos = pos % SHOP_HOST_ITEM_MAX_NUM;
	if (tabIdx >= GetTabCount())
	{
		sys_log(0, "ShopEx::Buy : invalid position %d : %s", pos, ch->GetName());
		return SHOP_SUBHEADER_GC_INVALID_POS;
	}

	sys_log(0, "ShopEx::Buy : name %s pos %d", ch->GetName(), pos);

	GuestMapType::iterator it = m_map_guest.find(ch);

	if (it == m_map_guest.end())
		return SHOP_SUBHEADER_GC_END;

	TShopTableEx& shopTab = m_vec_shopTabs[tabIdx];
	TShopItemTable& r_item = shopTab.items[slotPos];

	if (r_item.price < 0)
	{
		LogManager::instance().HackLog("SHOP_BUY_GOLD_OVERFLOW", ch);
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}

	DWORD dwPrice = r_item.price;

#if defined(ENABLE_RENEWAL_SHOPEX)
	switch (r_item.price_type)
	{
	case SHOPEX_GOLD:
		if (it->second)
			dwPrice *= 1;
		if (ch->GetGold() < static_cast<int>(dwPrice))
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		break;
	case SHOPEX_SECONDARY:
		if (ch->CountSpecifyTypeItem(ITEM_SECONDARY_COIN) < static_cast<int>(dwPrice))
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX;
		break;
	case SHOPEX_ITEM:
	{
		for (int i = 0; i < SHOP_MAX_ITEM_NUM; i++)
		{
			if (ch->CountSpecifyItem(r_item.item_vnum[i]) < r_item.item_price[i])
			{
				if (r_item.item_vnum[i] > 0)
				{
					return SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM;
				}
			}
		}
		// if (ch->CountSpecifyItem(r_item.price_vnum) < static_cast<int>(dwPrice))
		// 	return SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM;
	}
		break;
	case SHOPEX_EXP:
		if (ch->GetExp() < dwPrice)
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_EXP;
#ifdef ENABLE_GEM_SYSTEM
	case SHOPEX_GEM:
		if (ch->GetGem() < static_cast<int>(dwPrice))
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_GEM;
		break;
#endif
	}
#else
	switch (shopTab.coinType)
	{
	case SHOP_COIN_TYPE_GOLD:
		if (it->second)	// if other empire, price is triple
			dwPrice *= 3;

		if (ch->GetGold() < (int) dwPrice)
		{
			sys_log(1, "ShopEx::Buy : Not enough money : %s has %d, price %d", ch->GetName(), ch->GetGold(), dwPrice);
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}
		break;
	case SHOP_COIN_TYPE_SECONDARY_COIN:
		{
			DWORD count = ch->CountSpecifyTypeItem(ITEM_SECONDARY_COIN);
			if (count < dwPrice)
			{
				sys_log(1, "ShopEx::Buy : Not enough myeongdojun : %s has %d, price %d", ch->GetName(), count, dwPrice);
				return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX;
			}
		}
		break;
	}
#endif

	LPITEM item;

	TItemTable* pTable = ITEM_MANAGER::instance().GetTable(r_item.vnum);
	item = ITEM_MANAGER::instance().CreateItem(r_item.vnum, r_item.count, 0, (pTable && pTable->sAddonType != 0));

	if (!item)
		return SHOP_SUBHEADER_GC_SOLD_OUT;

	int iEmptyPos;
	if (item->IsDragonSoul())
	{
		iEmptyPos = ch->GetEmptyDragonSoulInventory(item);
	}
	else
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		iEmptyPos = ch->GetEmptyInventory(item);
#else
		iEmptyPos = ch->GetEmptyInventory(item->GetSize());
#endif
	}

	if (iEmptyPos < 0)
	{
		sys_log(1, "ShopEx::Buy : Inventory full : %s size %d", ch->GetName(), item->GetSize());
		M2_DESTROY_ITEM(item);
		return SHOP_SUBHEADER_GC_INVENTORY_FULL;
	}

#if defined(ENABLE_RENEWAL_SHOPEX)
	switch (r_item.price_type)
	{
	case SHOPEX_GOLD:
#ifdef ENABLE_GOLD_MAX_EXTENDED
		ch->ChangeGold(-static_cast<int>(dwPrice));
#else
		ch->PointChange(POINT_GOLD, -static_cast<int>(dwPrice), false);
#endif
		break;
	case SHOPEX_SECONDARY:
		ch->RemoveSpecifyTypeItem(ITEM_SECONDARY_COIN, dwPrice);
		break;
	case SHOPEX_ITEM:
	{
		for (int i = 0; i < SHOP_MAX_ITEM_NUM; i++)
		{
			ch->RemoveSpecifyItem(r_item.item_vnum[i], r_item.item_price[i]);
		}
		//ch->RemoveSpecifyItem(r_item.price_vnum, dwPrice);
	}
	break;
	case SHOPEX_EXP:
		ch->PointChange(POINT_EXP, -static_cast<int>(dwPrice), false);
		break;
#ifdef ENABLE_GEM_SYSTEM
	case SHOPEX_GEM:
		ch->PointChange(POINT_GEM, -static_cast<int>(dwPrice), false);
		break;
#endif
	}
	{
		/*Attrs*/
		if (r_item.aAttr[0].bType != 0)
		{
			item->SetAttributes(r_item.aAttr);
		}

		/*Sockets*/
		for (BYTE i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			if (r_item.alSockets[i] != 0)
				item->SetSocket(i, r_item.alSockets[i]);
		}
	}
#else

	switch (shopTab.coinType)
	{
	case SHOP_COIN_TYPE_GOLD:
#ifdef ENABLE_GOLD_MAX_EXTENDED
		ch->ChangeGold(-dwPrice);
#else
		ch->PointChange(POINT_GOLD, -dwPrice, false);
#endif
		break;
	case SHOP_COIN_TYPE_SECONDARY_COIN:
		ch->RemoveSpecifyTypeItem(ITEM_SECONDARY_COIN, dwPrice);
		break;
	}
#endif

#ifdef ENABLE_BATTLE_PASS
	DWORD dwPriceCount = dwPrice;
	{
		BYTE bBattlePassId = ch->GetBattlePassId();
		if (bBattlePassId)
		{
			DWORD dwYangCount, dwNotUsed;
			if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, SPENT_YANG, &dwNotUsed, &dwYangCount))
			{
				if (r_item.price_type == SHOPEX_GOLD && ch->GetMissionProgress(SPENT_YANG, bBattlePassId) < dwYangCount)
				{
					ch->UpdateMissionProgress(SPENT_YANG, bBattlePassId, dwPriceCount, dwYangCount);
				}
			}
		}
	}
#endif

	if (item->IsDragonSoul())
		item->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
	else
	{
		short bCount = item->GetCount();
		
		if (IS_SET(item->GetFlag(), ITEM_FLAG_STACKABLE) && !item->IsEquipped())
		{
			for (WORD i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
			{
				LPITEM item2 = ch->GetInventoryItem(i);

				if (!item2)
					continue;

				if (item2->GetCount() == g_bItemCountLimit)
					continue;

				if (item2->GetVnum() == item->GetVnum())
				{
					int j;

					for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						if (item2->GetSocket(j) != item->GetSocket(j))
							break;

					if (j != ITEM_SOCKET_MAX_NUM)
						continue;

					short bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
					bCount -= bCount2;
					item2->SetCount(item2->GetCount() + bCount2);

					if (bCount == 0)
						break;
				}
			}

			item->SetCount(bCount);
		}
		
		if (bCount > 0)
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else
			M2_DESTROY_ITEM(item);
	}
	
	ITEM_MANAGER::instance().FlushDelayedSave(item);
	LogManager::instance().ItemLog(ch, item, "BUY", item->GetName());

	if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
	{
		LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), PERSONAL_SHOP_BUY, "");
	}

	DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), -dwPrice);

	if (item)
		sys_log(0, "ShopEx: BUY: name %s %s(x %d):%u price %u", ch->GetName(), item->GetName(), item->GetCount(), item->GetID(), dwPrice);

#ifdef ENABLE_QUEST_RECEIVE_ITEM
	ch->SenderRecvItem(ch->GetPlayerID(), item);
#endif
	ch->Save();

    return (SHOP_SUBHEADER_GC_OK);
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
