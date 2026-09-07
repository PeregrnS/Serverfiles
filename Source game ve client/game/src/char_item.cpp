#include "stdafx.h"

#include <stack>

#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "affect.h"
#include "skill.h"
#include "start_position.h"
#include "mob_manager.h"
#include "db.h"
#include "log.h"
#include "vector.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "fishing.h"
#include "party.h"
#include "dungeon.h"
#include "refine.h"
#include "unique_item.h"
#include "war_map.h"
#include "xmas_event.h"
#include "marriage.h"
#include "monarch.h"
#include "polymorph.h"
#include "blend_item.h"
#include "castle.h"
#include "arena.h"
#include "threeway_war.h"

#include "safebox.h"
#include "shop.h"
#ifdef __DAILY_QUESTS__
#include "dailyquest_manager.h"
#endif
#ifdef ENABLE_NEWSTUFF
#include "pvp.h"
#include "../../common/PulseManager.h"
#endif

#include "../../common/VnumHelper.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/CommonDefines.h"
#include "PetSystem.h"

#ifdef ENABLE_SWITCHBOT_SYSTEM
	#include "new_switchbot.h"
#endif
#ifdef ENABLE_RANK_PLAYER
	#include "RankPlayer.h"
#endif
#ifdef __RANKING_SYSTEM__
	#include "RankPlayer.h"
#endif
#ifdef ENABLE_BATTLE_PASS
	#include "battle_pass.h"
#endif
#include "gm.h"

#ifdef ENABLE_OWN_STONE_CHANCE
extern int stone_chance;
#endif

#define ENABLE_EFFECT_EXTRAPOT
#define ENABLE_BOOKS_STACKFIX
#define ENABLE_ITEM_RARE_ATTR_LEVEL_PCT

enum {ITEM_BROKEN_METIN_VNUM = 28960};

// CHANGE_ITEM_ATTRIBUTES
const char CHARACTER::msc_szLastChangeItemAttrFlag[] = "Item.LastChangeItemAttr";
// END_OF_CHANGE_ITEM_ATTRIBUTES
const BYTE g_aBuffOnAttrPoints[] = { POINT_ENERGY, POINT_COSTUME_ATTR_BONUS };

struct FFindStone
{
	std::map<DWORD, LPCHARACTER> m_mapStone;

	void operator()(LPENTITY pEnt)
	{
		if (pEnt->IsType(ENTITY_CHARACTER) == true)
		{
			LPCHARACTER pChar = (LPCHARACTER)pEnt;

			if (pChar->IsStone() == true)
			{
				m_mapStone[(DWORD)pChar->GetVID()] = pChar;
			}
		}
	}
};

static bool IS_SUMMON_ITEM(int vnum)
{
	switch (vnum)
	{
		case 22000:
		case 22010:
		case 22011:
		case 22020:
		case ITEM_MARRIAGE_RING:
			return true;
	}

	return false;
}

static bool CAN_MOVE_LOCKED_ITEM(LPITEM item)
{
	if (item->GetType() == ITEM_BLEND || item->GetType() == ITEM_USE)
		return true;
	
	return false;
}

static bool IS_67_SKINBONUS_TARGET_VNUM(DWORD dwVnum)
{
	if (dwVnum >= 172650 && dwVnum <= 172709)
		return true;

	if (dwVnum >= 79000 && dwVnum <= 79098)
		return true;

	return false;
}

#ifdef BLOCK_ITEMS_ON_MAP
/*
static bool IS_ENABLE_ITEM(int vnum) ///39011,39012,39013 Item's Want To Block It.
{
	switch (vnum)
	{
		case 39011:
		case 39012:
		case 39013:
			return true;
	}

	return false;
}
*/

static bool IS_ENABLE_ITEM2(int type) ///Types Want To Block It.
{
	switch (type)
	{
		case ITEM_BELT:
		case ITEM_MOUNT:
		case ITEM_PET:
		case ITEM_UNIQUE:
			return true;
	}

	return false;
}

static bool IS_ENABLE_ITEM3(int subtype) ///SubTypes Want To Block It.
{
	switch (subtype)
	{
		case ARMOR_PENDANT:
		case COSTUME_ACCE:
			return true;
	}

	return false;
}
#endif

static bool IS_MONKEY_DUNGEON(int map_index)
{
	switch (map_index)
	{
		case 5:
		case 25:
		case 45:
		case 108:
		case 109:
			return true;;
	}

	return false;
}

bool IS_SUMMONABLE_ZONE(int map_index)
{
	if (IS_MONKEY_DUNGEON(map_index))
		return false;
	if (IS_CASTLE_MAP(map_index))
		return false;

	switch (map_index)
	{
		case 66 :
		case 71 :
		case 72 :
		case 73 :
		case 193 :
#if 0
		case 184 :
		case 185 :
		case 186 :
		case 187 :
		case 188 :
		case 189 :
#endif

		case 216 :
		case 217 :
		case 208 :

		case 113 :
			return false;
	}

	if (map_index > 10000) return false;

	return true;
}

#ifdef BLOCK_ITEMS_ON_MAP
bool IS_ENABLE_ITEM_ZONE(int map_index) ///41,110,111 Maps Index.
{
	// Original statische Maps
	if (map_index == 110 || map_index == 111)
		return false;

	// Dynamische Instanzen der Gildenkrieg-Maps
	if (map_index / 10000 == 110 || map_index / 10000 == 111)
		return false;

	return true;
}
#endif

bool IS_BOTARYABLE_ZONE(int nMapIndex)
{
	if (!g_bEnableBootaryCheck) return true;

	switch (nMapIndex)
	{
		case 1 :
		case 3 :
		case 21 :
		case 23 :
		case 41 :
		case 43 :
			return true;
	}

	return false;
}

static bool FN_check_item_socket(LPITEM item)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket(i) != item->GetProto()->alSockets[i])
			return false;
	}

	return true;
}

static void FN_copy_item_socket(LPITEM dest, LPITEM src)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket(i, src->GetSocket(i));
	}
}
static bool FN_check_item_sex(LPCHARACTER ch, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE==GET_SEX(ch))
			return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE))
	{
		if (SEX_FEMALE==GET_SEX(ch))
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// ITEM HANDLING
/////////////////////////////////////////////////////////////////////////////
bool CHARACTER::CanHandleItem(bool bSkipCheckRefine, bool bSkipObserver)
{
	if (!bSkipObserver)
		if (m_bIsObserver)
			return false;

	if (GetMyShop())
		return false;

	if (!bSkipCheckRefine)
		if (m_bUnderRefine)
			return false;

	if (IsCubeOpen() || NULL != DragonSoul_RefineWindow_GetOpener())
		return false;

	if (IsWarping())
		return false;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if ((m_bAcceCombination) || (m_bAcceAbsorption))
		return false;
#endif

#if defined(__BL_67_ATTR__)
	if (Is67AttrOpen())
		return false;
#endif

	return true;
}

#ifdef FAST_EQUIP_WORLDARD 
LPITEM CHARACTER::GetChangeEquipItem(WORD wCell) const
{
	return GetItem(TItemPos(CHANGE_EQUIP,wCell));
}
#endif

LPITEM CHARACTER::GetInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}
LPITEM CHARACTER::GetItem(TItemPos Cell) const
{
	if (!m_PlayerSlots)
		return nullptr;

	if (!IsValidItemPosition(Cell))
		return NULL;

	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pItems[wCell];
	case DRAGON_SOUL_INVENTORY:
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pDSItems[wCell];

#ifdef FAST_EQUIP_WORLDARD
	case CHANGE_EQUIP:
		if(wCell >= CHANGE_EQUIP_SLOT_COUNT)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid change_equip item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pChangeEquipItem[wCell];
#endif

#ifdef ENABLE_SWITCHBOT_SYSTEM
	case SWITCHBOT:
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid switchbot item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pSwitchbotItems[wCell];
#endif

	default:
		return NULL;
	}
	return NULL;
}

void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem
	#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM
	, bool bWereMine
	#endif
)
{
	if (!m_PlayerSlots)
		return;

	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	if ((unsigned long)((CItem*)pItem) == 0xff || (unsigned long)((CItem*)pItem) == 0xffffffff)
	{
		sys_err("!!! FATAL ERROR !!! item == 0xff (char: %s cell: %u)", GetName(), wCell);
		core_dump();
		return;
	}

	if (pItem && pItem->GetOwner())
	{
		assert(!"GetOwner exist");
		return;
	}

	switch(window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		{
			if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
			{
				sys_err("CHARACTER::SetItem: invalid item cell %d", wCell);
				return;
			}

			LPITEM pOld = m_PlayerSlots->pItems[wCell];

			if (pOld)
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				if (wCell < INVENTORY_MAX_NUM || Cell.IsSpecialInventoryPosition())
#else
				if (wCell < INVENTORY_MAX_NUM)
#endif
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
						{
#ifdef ENABLE_SPECIAL_INVENTORY
							if (!TItemPos(INVENTORY, p).IsSpecialInventoryPosition())
							{
								continue;
							}
#else
							continue;
#endif
						}

						if (m_PlayerSlots->pItems[p] && m_PlayerSlots->pItems[p] != pOld)
							continue;

						m_PlayerSlots->bItemGrid[p] = 0;
					}
				}
				else
					m_PlayerSlots->bItemGrid[wCell] = 0;
			}

			if (pItem)
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				if (wCell < INVENTORY_MAX_NUM || Cell.IsSpecialInventoryPosition())
#else
				if (wCell < INVENTORY_MAX_NUM)
#endif
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
						{
#ifdef ENABLE_SPECIAL_INVENTORY
							if (!TItemPos(INVENTORY, p).IsSpecialInventoryPosition())
							{
								continue;
							}
#else
							continue;
#endif
						}

						m_PlayerSlots->bItemGrid[p] = wCell + 1;
					}
				}
				else
					m_PlayerSlots->bItemGrid[wCell] = wCell + 1;
			}

			m_PlayerSlots->pItems[wCell] = pItem;
		}
		break;

	case DRAGON_SOUL_INVENTORY:
		{
			LPITEM pOld = m_PlayerSlots->pDSItems[wCell];

			if (pOld)
			{
				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						if (m_PlayerSlots->pDSItems[p] && m_PlayerSlots->pDSItems[p] != pOld)
							continue;

						m_PlayerSlots->wDSItemGrid[p] = 0;
					}
				}
				else
					m_PlayerSlots->wDSItemGrid[wCell] = 0;
			}

			if (pItem)
			{
				if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					sys_err("CHARACTER::SetItem: invalid DS item cell %d", wCell);
					return;
				}

				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						m_PlayerSlots->wDSItemGrid[p] = wCell + 1;
					}
				}
				else
					m_PlayerSlots->wDSItemGrid[wCell] = wCell + 1;
			}

			m_PlayerSlots->pDSItems[wCell] = pItem;
		}
		break;

#ifdef FAST_EQUIP_WORLDARD
	case CHANGE_EQUIP:
		{
			if (wCell >= CHANGE_EQUIP_SLOT_COUNT)
			{
				sys_err("CHARACTER::SetItem: invalid CHANGE_EQUIP item cell %d", wCell);
				return;
			}
			LPITEM pOld = m_PlayerSlots->pChangeEquipItem[wCell];

			if (pOld && pItem)
			{
				return;
			}

			m_PlayerSlots->pChangeEquipItem[wCell] = pItem;

		}
		break;	
#endif

#ifdef ENABLE_SWITCHBOT_SYSTEM
	case SWITCHBOT:
	{
		LPITEM pOld = m_PlayerSlots->pSwitchbotItems[wCell];
		if (pItem && pOld)
		{
			return;
		}

		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::SetItem: invalid switchbot item cell %d", wCell);
			return;
		}

		if (pItem)
		{
			CSwitchbotManager::Instance().RegisterItem(GetPlayerID(), pItem->GetID(), wCell);
		}
		else
		{
			CSwitchbotManager::Instance().UnregisterItem(GetPlayerID(), wCell);
		}

		m_PlayerSlots->pSwitchbotItems[wCell] = pItem;
	}
	break;
#endif
	default:
		sys_err ("Invalid Inventory type %d", window_type);
		return;
	}

	if (GetDesc())
	{
		if (pItem)
		{
			TPacketGCItemSet pack;
			pack.header = HEADER_GC_ITEM_SET;
			pack.Cell = Cell;

			pack.count = pItem->GetCount();
			pack.vnum = pItem->GetVnum();
			pack.flags = pItem->GetFlag();
			pack.anti_flags	= pItem->GetAntiFlag();
#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM
			pack.highlight = !bWereMine || (Cell.window_type == DRAGON_SOUL_INVENTORY);
#else
			pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#endif

			thecore_memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
			thecore_memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));

#ifndef ENABLE_QUICK_OPEN
			GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
#else
			if (!bInvBuff)
			{
				GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
			}
			else
			{
				AddItemToInvBuff(pItem);
			}
#endif
		}
		else
		{
			TPacketGCItemDelDeprecated pack;
			pack.header = HEADER_GC_ITEM_DEL;
			pack.Cell = Cell;
			pack.count = 0;
			pack.vnum = 0;
			memset(pack.alSockets, 0, sizeof(pack.alSockets));
			memset(pack.aAttr, 0, sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemDelDeprecated));
		}
	}

	if (pItem)
	{
		pItem->SetCell(this, wCell);
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell))
				pItem->SetWindow(INVENTORY);
#ifdef ENABLE_SPECIAL_INVENTORY
			else if (wCell >= SPECIAL_INVENTORY_SLOT_START && wCell < SPECIAL_INVENTORY_SLOT_END)
			{
				pItem->SetWindow(INVENTORY);
			}
#endif
			else
				pItem->SetWindow(EQUIPMENT);
			break;
		case DRAGON_SOUL_INVENTORY:
			pItem->SetWindow(DRAGON_SOUL_INVENTORY);
			break;
#ifdef ENABLE_SWITCHBOT_SYSTEM
		case SWITCHBOT:
			pItem->SetWindow(SWITCHBOT);
			break;
#endif
#ifdef FAST_EQUIP_WORLDARD
		case CHANGE_EQUIP:
			pItem->SetWindow(CHANGE_EQUIP);
			break;
#endif
		}
	}
}

LPITEM CHARACTER::GetWear(BYTE bCell) const
{
	if (!m_PlayerSlots)
		return nullptr;

	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::GetWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_PlayerSlots->pItems[INVENTORY_MAX_NUM + bCell];
}

void CHARACTER::SetWear(BYTE bCell, LPITEM item)
{
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::SetItem: invalid item cell %d", bCell);
		return;
	}

	SetItem(TItemPos (INVENTORY, INVENTORY_MAX_NUM + bCell), item);

#ifndef KEEP_AURA_AFTER_WARP
	if (!item && bCell == WEAR_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}
#endif
}

void CHARACTER::ClearItem()
{
	int		i;
	LPITEM	item;

	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = GetInventoryItem(i)))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);

			SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
	
#ifdef FAST_EQUIP_WORLDARD
	for (i = 0; i < CHANGE_EQUIP_SLOT_COUNT; ++i)
	{
		if ((item = GetItem(TItemPos(CHANGE_EQUIP, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

#endif
	
#ifdef ENABLE_SWITCHBOT_SYSTEM
	for (i = 0; i < SWITCHBOT_SLOT_COUNT; ++i)
	{
		if ((item = GetItem(TItemPos(SWITCHBOT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
}

bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
	if (!m_PlayerSlots)
		return false;

	switch (Cell.window_type)
	{
	case INVENTORY:
		{
			WORD bCell = Cell.cell;

			++iExceptionCell;

			if (Cell.IsBeltInventoryPosition())
			{
				LPITEM beltItem = GetWear(WEAR_BELT);

				if (NULL == beltItem)
					return false;

				if (false == CBeltInventoryHelper::IsAvailableCell(bCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
					return false;

				if (m_PlayerSlots->bItemGrid[bCell])
				{
					if (m_PlayerSlots->bItemGrid[bCell] == iExceptionCell)
						return true;

					return false;
				}

				if (bSize == 1)
					return true;

			}
#ifdef ENABLE_SPECIAL_INVENTORY
			else if (Cell.IsSpecialInventoryPosition())
			{
				return IsEmptySpecialItemGrid(Cell, bSize, iExceptionCell);
			}
#endif
			else if (bCell >= INVENTORY_MAX_NUM)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell])
			{
				if (m_PlayerSlots->bItemGrid[bCell] == iExceptionCell)
				{
					if (bSize == 1)
						return true;

					int j = 1;
					BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

					do
					{
						BYTE p = bCell + (5 * j);

						if (p >= INVENTORY_MAX_NUM)
							return false;

						if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
							return false;

						if (m_PlayerSlots->bItemGrid[p])
							if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
				else
					return false;
			}

			if (1 == bSize)
				return true;
			else
			{
				int j = 1;
				BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

				do
				{
					BYTE p = bCell + (5 * j);

					if (p >= INVENTORY_MAX_NUM)
						return false;

					if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				}
				while (++j < bSize);

				return true;
			}
		}
		break;
	case DRAGON_SOUL_INVENTORY:
		{
			WORD wCell = Cell.cell;
			if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				return false;

			iExceptionCell++;

			if (m_PlayerSlots->wDSItemGrid[wCell])
			{
				if (m_PlayerSlots->wDSItemGrid[wCell] == iExceptionCell)
				{
					if (bSize == 1)
						return true;

					int j = 1;

					do
					{
						int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							return false;

						if (m_PlayerSlots->wDSItemGrid[p])
							if (m_PlayerSlots->wDSItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
				else
					return false;
			}

			if (1 == bSize)
				return true;
			else
			{
				int j = 1;

				do
				{
					int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->wDSItemGrid[p] != iExceptionCell)
							return false;
				}
				while (++j < bSize);

				return true;
			}
		}
		break;

#ifdef FAST_EQUIP_WORLDARD
	case CHANGE_EQUIP:
		{
			WORD wCell = Cell.cell;
			if (wCell >= CHANGE_EQUIP_SLOT_COUNT)
			{
				return false;
			}

			if (m_PlayerSlots->pChangeEquipItem[wCell])
			{
				return false;
			}

			return true;
		}
		break;
#endif

#ifdef ENABLE_SWITCHBOT_SYSTEM
		case SWITCHBOT:
		{
			WORD wCell = Cell.cell;
			if (wCell >= SWITCHBOT_SLOT_COUNT)
			{
				return false;
			}

			if (m_PlayerSlots->pSwitchbotItems[wCell])
			{
				return false;
			}

			return true;
		}
		break;
#endif
	}
	return false;
}


#ifdef ENABLE_SPECIAL_INVENTORY
bool CHARACTER::IsEmptySpecialItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
	if (!m_PlayerSlots)
		return false;

	WORD wCell = Cell.cell;

	SpecialInventoryRange range = SpecialInventoryRange(Cell.GetSpecialInventoryType());
	if (wCell < range.start || wCell >= range.end)
	{
		sys_err("IsEmptySpecialItemGrid Range missmatch!");
		return false;
	}
	BYTE bPage = (wCell - SPECIAL_INVENTORY_SKILLBOOK_START) / SPECIAL_INVENTORY_PAGE_SIZE;
	if (m_PlayerSlots->bItemGrid[wCell])
	{
		//sys_err("m_PlayerSlots->bItemGrid[wCell] = %d", m_PlayerSlots->bItemGrid[wCell]);
		if (bSize == 1) {

			return m_PlayerSlots->bItemGrid[wCell] == iExceptionCell;
		}

		int j = 0;
		do
		{

			WORD p = wCell + (5 * j);

			if (p >= SPECIAL_INVENTORY_MATERIAL_END)
				return false;

			if ( (p - SPECIAL_INVENTORY_SKILLBOOK_START) / SPECIAL_INVENTORY_PAGE_SIZE != bPage)
				return false;

			if (test_server) sys_err("Checking Cell %d", p);
			if (m_PlayerSlots->bItemGrid[p])
			{
				if (test_server) sys_err("Cell %d has an Point %d", m_PlayerSlots->bItemGrid[p]);
				if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
					return false;
			}
		} while (++j < bSize);

		return true;
	}
	int j = 0;
	do
	{

		WORD p = wCell + (5 * j);

		if (p >= SPECIAL_INVENTORY_MATERIAL_END)
			return false;

		if ((p - SPECIAL_INVENTORY_SKILLBOOK_START) / SPECIAL_INVENTORY_PAGE_SIZE != bPage)
			return false;

		if (test_server) sys_err("Checking Cell %d", p);
		if (m_PlayerSlots->bItemGrid[p]) {
			if (test_server) sys_err("Cell %d has an Point %d", m_PlayerSlots->bItemGrid[p]);
			if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
				return false;
		}
			
	} while (++j < bSize);


	return true;
}

int CHARACTER::GetEmptyInventory(LPITEM item) const
{
	int start = 0;
	int end = INVENTORY_MAX_NUM;

	if (item->GetSpecialInventoryType() != -1)
	{
		SpecialInventoryRange range = SpecialInventoryRange(item->GetSpecialInventoryType());
		start = range.start;
		end = range.end;
	}

	for (int i = start; i < end; ++i)
	{
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), item->GetSize()))
		{
			return i;
		}
	}

	return -1;
}
#else
int CHARACTER::GetEmptyInventory(BYTE size) const
{
	for ( int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), size))
			return i;
	return -1;
}
#endif

bool CHARACTER::GetEmptyInEachDsRoughCategory() const
{
	bool bRet = true;

	LPITEM pItem = nullptr;

	uint32_t dwVnum;

	for (auto i = 0; i < 6; i++)
	{
		dwVnum = 110000 + (i * 10000);

		pItem = ITEM_MANAGER::instance().CreateItem(dwVnum, 1);
		if (!pItem)
		{
			sys_err("%u cannot be created!", dwVnum);

			bRet = false;
			break;
		}

		if (GetEmptyDragonSoulInventory(pItem) == -1)
		{
			M2_DESTROY_ITEM(pItem);

			bRet = false;
			break;
		}

		M2_DESTROY_ITEM(pItem);
	}

	return bRet;
}

int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem) const
{
	if (NULL == pItem || !pItem->IsDragonSoul())
		return -1;
	if (!DragonSoul_IsQualified())
	{
		return -1;
	}
	BYTE bSize = pItem->GetSize();
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItem);

	if (WORD_MAX == wBaseCell)
		return -1;

#ifdef ENABLE_EXTENDED_DS_INVENTORY
	for (int i = 0; i < (DRAGON_SOUL_BOX_SIZE * DRAGON_SOUL_INVENTORY_PAGE_COUNT); ++i)
#else
	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
#endif
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
			return i + wBaseCell;

	return -1;
}

int CHARACTER::GetEmptyDragonSoulInventoryType() const
{
    auto bSize = 1;

    bool foundEmptySlot = false;

    for (auto i = 0; i < DS_SLOT_MAX; ++i)
    {
        for (auto j = 0; j < DRAGON_SOUL_GRADE_MAX; ++j)
        {
#ifdef ENABLE_EXTENDED_DS_INVENTORY
			WORD wBaseCell = i * DRAGON_SOUL_GRADE_MAX * DRAGON_SOUL_BOX_SIZE * DRAGON_SOUL_INVENTORY_PAGE_COUNT + j * DRAGON_SOUL_BOX_SIZE * DRAGON_SOUL_INVENTORY_PAGE_COUNT;
#else
            WORD wBaseCell = i * DRAGON_SOUL_STEP_MAX * DRAGON_SOUL_BOX_SIZE + j * DRAGON_SOUL_BOX_SIZE;
#endif

            if (WORD_MAX == wBaseCell)
                return -1;

            foundEmptySlot = false;

#ifdef ENABLE_EXTENDED_DS_INVENTORY
			for (auto k = 0; k < DRAGON_SOUL_BOX_SIZE * DRAGON_SOUL_INVENTORY_PAGE_COUNT; ++k)
#else
            for (auto k = 0; k < DRAGON_SOUL_BOX_SIZE; ++k)
#endif
			{
				if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, k + wBaseCell), bSize))
				{
					foundEmptySlot = true;
					break;
				}
			}

            if (!foundEmptySlot)
            {
                return i * 10 + j;
            }
        }
    }

    return 0;
}

void CHARACTER::CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const
{
	vDragonSoulItemGrid.resize(DRAGON_SOUL_INVENTORY_MAX_NUM);

	std::copy(m_PlayerSlots->wDSItemGrid, m_PlayerSlots->wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
}

int CHARACTER::CountEmptyInventory() const
{
	int	count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem(i))
			count += GetInventoryItem(i)->GetSize();

	return (INVENTORY_MAX_NUM - count);
}

void TransformRefineItem(LPITEM pkOldItem, LPITEM pkNewItem)
{
	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
		//pkNewItem->StartAccessorySocketExpireEvent();
	}
	// END_OF_ACCESSORY_REFINE
	else
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
				break;
			else
				pkNewItem->SetSocket(i, 1);
		}

		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			long socket = pkOldItem->GetSocket(i);

			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				pkNewItem->SetSocket(slot++, socket);
		}

	}

	pkOldItem->CopyAttributeTo(pkNewItem);
}

void NotifyRefineSuccess(LPCHARACTER ch, LPITEM item, const char* way)
{
	if (NULL != ch && item != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), 1, way);
	}
}

void NotifyRefineFail(LPCHARACTER ch, LPITEM item, const char* way, int success = 0)
{
	if (NULL != ch && NULL != item)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailed");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), success, way);
	}
}

void CHARACTER::SetRefineNPC(LPCHARACTER ch)
{
	if ( ch != NULL )
	{
		m_dwRefineNPCVID = ch->GetVID();
	}
	else
	{
		m_dwRefineNPCVID = 0;
	}
}

bool CHARACTER::DoRefine(LPITEM item, bool bMoneyOnly)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	if (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_PENDANT)
	{
		if (item->GetCount() > 1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Seperate the Talisman first!"));
			return false;
		}
	}

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	DWORD result_vnum = item->GetRefinedVnum();

	// REFINE_COST
	int cost = ComputeRefineFee(prt->cost);

	int RefineChance = GetQuestFlag("main_quest_lv7.refine_chance");

	if (RefineChance > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("무료 개량 기회는 20 이하의 무기만 가능합니다"));
			return false;
		}

		cost = 0;
		SetQuestFlag("main_quest_lv7.refine_chance", RefineChance - 1);
	}
	// END_OF_REFINE_COST

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 개량할 수 없습니다."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
		return false;

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefine NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
		return false;
	}

	// REFINE_COST
#ifdef ENABLE_GOLD_MAX_EXTENDED
	if (GetGold() < static_cast<unsigned long long>(cost))
#else
	if (GetGold() < cost)
#endif
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개량을 하기 위한 돈이 부족합니다."));
		return false;
	}

	if (!bMoneyOnly && !RefineChance)
	{
		for (int i = 0; i < prt->material_count; ++i)
		{
			if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
			{
				if (test_server)
				{
					ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
				}
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개량을 하기 위한 재료가 부족합니다."));
				return false;
			}
		}

		for (int i = 0; i < prt->material_count; ++i)
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);
	}

	int prob = number(1, 100);

	if (IsRefineThroughGuild() || bMoneyOnly)
		prob -= 10;

	// END_OF_REFINE_COST

	if (prob <= prt->prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			WORD wCell = item->GetCell();

#ifdef ENABLE_BATTLE_PASS
			BYTE bBattlePassId = GetBattlePassId();
			if (bBattlePassId)
			{
				DWORD dwItemVnum, dwCount;
				if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, REFINE_ITEM, &dwItemVnum, &dwCount))
				{
					if (dwItemVnum == item->GetVnum() && GetMissionProgress(REFINE_ITEM, bBattlePassId) < dwCount)
					{
						UpdateMissionProgress(REFINE_ITEM, bBattlePassId, 1, dwCount);
					}
				}
			}
#endif

			// DETAIL_REFINE_LOG
			NotifyRefineSuccess(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			// END_OF_DETAIL_REFINE_LOG

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
#ifdef __RANKING_SYSTEM__
			RankPlayer::instance().SendInfoPlayer(this, RANK_BY_ITEM_IMPROVED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_ITEM_IMPROVED) + 1, false);
#endif
			sys_log(0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -cost);
			sys_log(0, "PayPee %d", cost);
			PayRefineFee(cost);
			sys_log(0, "PayPee End %d", cost);

#ifdef ENABLE_RANK_PLAYER
			RankPlayer::instance().SendInfoPlayer(this, RANK_BY_ITEM_IMPROVED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_ITEM_IMPROVED) + 1, false);
#endif
		}
		else
		{
			// DETAIL_REFINE_LOG

			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			// END_OF_DETAIL_REFINE_LOG
		}
	}
	else
	{
		DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
		NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		item->AttrLog();
		ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

		//PointChange(POINT_GOLD, -cost);
		PayRefineFee(cost);
	}

#ifdef __DAILY_QUESTS__
	CDailyQuestManager::instance().Progress(this, 0, DAILY_TYPE_UPGRADE);
#endif

	return true;
}

enum enum_RefineScrolls
{
	CHUKBOK_SCROLL = 0,
	HYUNIRON_CHN   = 1,
	YONGSIN_SCROLL = 2,
	MUSIN_SCROLL   = 3,
	YAGONG_SCROLL  = 4,
	MEMO_SCROLL	   = 5,
	BDRAGON_SCROLL	= 6,
};

bool CHARACTER::DoRefineWithScroll(LPITEM item)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	if (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_PENDANT)
	{
		if (item->GetCount() > 1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Talisman> Seperate the Talisman first! Bitte Talisman entstapeln!"));
			return false;
		}
	}

	ClearRefineMode();

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	LPITEM pkItemScroll;

	if (m_iRefineAdditionalCell < 0)
		return false;

	pkItemScroll = GetInventoryItem(m_iRefineAdditionalCell);

	if (!pkItemScroll)
		return false;

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
		return false;

	if (pkItemScroll->GetVnum() == item->GetVnum())
		return false;

	DWORD result_vnum = item->GetRefinedVnum();
	DWORD result_fail_vnum = item->GetRefineFromVnum();

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 개량할 수 없습니다."));
		return false;
	}

	// MUSIN_SCROLL
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		if (item->GetRefineLevel() >= 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 개량서로 더 이상 개량할 수 없습니다."));
			return false;
		}
	}
	// END_OF_MUSIC_SCROLL

	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		if (item->GetRefineLevel() != pkItemScroll->GetValue(1))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 개량서로 개량할 수 없습니다."));
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		if (item->GetType() != ITEM_METIN || item->GetRefineLevel() != 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템으로 개량할 수 없습니다."));
			return false;
		}
	}

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
		return false;
	}

	if (GetGold() < prt->cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개량을 하기 위한 돈이 부족합니다."));
		return false;
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
		if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
		{
			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
			}
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개량을 하기 위한 재료가 부족합니다."));
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);

	int prob = number(1, 100);
	int success_prob = prt->prob;
	bool bDestroyWhenFail = false;

	const char* szRefineType = "SCROLL";

	if (pkItemScroll->GetValue(0) == HYUNIRON_CHN ||
		pkItemScroll->GetValue(0) == YONGSIN_SCROLL ||
		pkItemScroll->GetValue(0) == YAGONG_SCROLL)
	{
		const char hyuniron_prob[9] = { 100, 75, 65, 55, 45, 40, 35, 25, 20 };
		const char yagong_prob[9] = { 100, 100, 90, 80, 70, 60, 50, 30, 20 };

		if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			success_prob = hyuniron_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			success_prob = yagong_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == HYUNIRON_CHN) {} // @fixme121
		else
		{
			sys_err("REFINE : Unknown refine scroll item. Value0: %d", pkItemScroll->GetValue(0));
		}

		if (test_server)
		{
			ChatPacket(CHAT_TYPE_INFO, "[Only Test] Success_Prob %d, RefineLevel %d ", success_prob, item->GetRefineLevel());
		}
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
			bDestroyWhenFail = true;

		// DETAIL_REFINE_LOG
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
		{
			szRefineType = "HYUNIRON";
		}
		else if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			szRefineType = "GOD_SCROLL";
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			szRefineType = "YAGONG_SCROLL";
		}
		// END_OF_DETAIL_REFINE_LOG
	}

	// DETAIL_REFINE_LOG
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		success_prob = 100;

		szRefineType = "MUSIN_SCROLL";
	}
	// END_OF_DETAIL_REFINE_LOG
	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		success_prob = 100;
		szRefineType = "MEMO_SCROLL";
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		success_prob = 80;
		szRefineType = "BDRAGON_SCROLL";
	}

	pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);

	if (prob <= success_prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			WORD wCell = item->GetCell();

#ifdef ENABLE_BATTLE_PASS
			BYTE bBattlePassId = GetBattlePassId();
			if (bBattlePassId)
			{
				DWORD dwItemVnum, dwCount;
				if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, REFINE_ITEM, &dwItemVnum, &dwCount))
				{
					if (dwItemVnum == item->GetVnum() && GetMissionProgress(REFINE_ITEM, bBattlePassId) < dwCount)
					{
						UpdateMissionProgress(REFINE_ITEM, bBattlePassId, 1, dwCount);
					}
				}
			}
#endif

			NotifyRefineSuccess(this, item, szRefineType);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -prt->cost);
			PayRefineFee(prt->cost);
#ifdef __RANKING_SYSTEM__
			RankPlayer::instance().SendInfoPlayer(this, RANK_BY_ITEM_IMPROVED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_ITEM_IMPROVED) + 1, false);
#endif
#ifdef ENABLE_RANK_PLAYER
			RankPlayer::instance().SendInfoPlayer(this, RANK_BY_ITEM_IMPROVED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_ITEM_IMPROVED) + 1, false);
#endif
		}
		else
		{
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else if (!bDestroyWhenFail && result_fail_vnum)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_fail_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

			WORD wCell = item->GetCell();

#ifdef ENABLE_BATTLE_PASS
			BYTE bBattlePassId = GetBattlePassId();
			if (bBattlePassId)
			{
				DWORD dwItemVnum, dwCount;
				if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, REFINE_ITEM, &dwItemVnum, &dwCount))
				{
					if (dwItemVnum == item->GetVnum() && GetMissionProgress(REFINE_ITEM, bBattlePassId) < dwCount)
					{
						UpdateMissionProgress(REFINE_ITEM, bBattlePassId, 1, dwCount);
					}
				}
			}
#endif

			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			NotifyRefineFail(this, item, szRefineType, -1);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			pkNewItem->AttrLog();

			//PointChange(POINT_GOLD, -prt->cost);
			PayRefineFee(prt->cost);
		}
		else
		{
			sys_err("cannot create item %u", result_fail_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else
	{
		NotifyRefineFail(this, item, szRefineType);

		PayRefineFee(prt->cost);
	}

#ifdef __DAILY_QUESTS__
	CDailyQuestManager::instance().Progress(this, 0, DAILY_TYPE_UPGRADE);
#endif

	return true;
}

bool CHARACTER::RefineInformation(WORD wCell, BYTE bType, int iAdditionalCell)
{
#ifdef ENABLE_SPECIAL_INVENTORY
	if (wCell > INVENTORY_MAX_NUM)
	{
		if (!(wCell >= SPECIAL_INVENTORY_STONE_START && wCell < SPECIAL_INVENTORY_SLOT_END))
		{
			return false;
		}
	}
#else
	if (wCell > INVENTORY_MAX_NUM)
	{
		return false;
	}
#endif

	LPITEM item = GetInventoryItem(wCell);

	if (!item)
		return false;

	// REFINE_COST
	if (bType == REFINE_TYPE_MONEY_ONLY && !GetQuestFlag("deviltower_zone.can_refine"))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사귀 타워 완료 보상은 한번까지 사용가능합니다."));
		return false;
	}
	// END_OF_REFINE_COST

	TPacketGCRefineInformation p;

	p.header = HEADER_GC_REFINE_INFORMATION;
	p.pos = wCell;
	p.src_vnum = item->GetVnum();
	p.result_vnum = item->GetRefinedVnum();
	p.type = bType;

	if (p.result_vnum == 0)
	{
		sys_err("RefineInformation p.result_vnum == 0");
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 이 방식으로는 개량할 수 없습니다."));
			return false;
		}
		else
		{
			LPITEM itemScroll = GetInventoryItem(iAdditionalCell);
			if (!itemScroll || item->GetVnum() == itemScroll->GetVnum())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("같은 개량서를 합칠 수는 없습니다."));
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("축복의 서와 현철을 합칠 수 있습니다."));
				return false;
			}
		}
	}

	CRefineManager & rm = CRefineManager::instance();

	const TRefineTable* prt = rm.GetRefineRecipe(item->GetRefineSet());

	if (!prt)
	{
		sys_err("RefineInformation NOT GET REFINE SET %d", item->GetRefineSet());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
		return false;
	}

	// REFINE_COST

	//MAIN_QUEST_LV7
	if (GetQuestFlag("main_quest_lv7.refine_chance") > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("무료 개량 기회는 20 이하의 무기만 가능합니다"));
			return false;
		}
		p.cost = 0;
	}
	else
		p.cost = ComputeRefineFee(prt->cost);

	//END_MAIN_QUEST_LV7
	p.prob = prt->prob;
	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));
	}
	else
	{
		p.material_count = prt->material_count;
		thecore_memcpy(&p.materials, prt->materials, sizeof(prt->materials));
	}
	// END_OF_REFINE_COST

	GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

	SetRefineMode(iAdditionalCell);
	return true;
}

bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;

	if (pkItem->GetSubType() == USE_TUNING)
	{
		// MUSIN_SCROLL
		if (pkItem->GetValue(0) == MUSIN_SCROLL)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_MUSIN, pkItem->GetCell());
		// END_OF_MUSIN_SCROLL
		else if (pkItem->GetValue(0) == HYUNIRON_CHN)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_HYUNIRON, pkItem->GetCell());
		else if (pkItem->GetValue(0) == BDRAGON_SCROLL)
		{
			if (pkTarget->GetRefineSet() != 702) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_BDRAGON, pkItem->GetCell());
		}
		else
		{
			if (pkTarget->GetRefineSet() == 501) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_SCROLL, pkItem->GetCell());
		}
	}
	else if (pkItem->GetSubType() == USE_DETACHMENT && IS_SET(pkTarget->GetFlag(), ITEM_FLAG_REFINEABLE))
	{
		LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT", pkTarget->GetName());

		bool bHasMetinStone = false;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			long socket = pkTarget->GetSocket(i);
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				bHasMetinStone = true;
				break;
			}
		}

		if (bHasMetinStone)
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				long socket = pkTarget->GetSocket(i);
				if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				{
					AutoGiveItem(socket);
					//TItemTable* pTable = ITEM_MANAGER::instance().GetTable(pkTarget->GetSocket(i));
					//pkTarget->SetSocket(i, pTable->alValues[2]);

					pkTarget->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
				}
			}
			pkItem->SetCount(pkItem->GetCount() - 1);
			return true;
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("빼낼 수 있는 메틴석이 없습니다."));
			return false;
		}
	}

	return false;
}

EVENTFUNC(kill_campfire_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "kill_campfire_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

bool CHARACTER::GiveRecallItem(LPITEM item)
{
	int idx = GetMapIndex();
	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("기억해 둘 수 없는 위치 입니다."));
		return false;
	}

	int pos;

	if (item->GetCount() == 1)
	{
		item->SetSocket(0, GetX());
		item->SetSocket(1, GetY());
	}
#ifdef ENABLE_SPECIAL_INVENTORY
	else if ((pos = GetEmptyInventory(item)) != -1)
#else
	else if ((pos = GetEmptyInventory(item->GetSize())) != -1)
#endif
	{
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);

		if (NULL != item2)
		{
			item2->SetSocket(0, GetX());
			item2->SetSocket(1, GetY());
			item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

			item->SetCount(item->GetCount() - 1);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지품에 빈 공간이 없습니다."));
		return false;
	}

	return true;
}

void CHARACTER::ProcessRecallItem(LPITEM item)
{
	int idx;

	if ((idx = SECTREE_MANAGER::instance().GetMapIndex(item->GetSocket(0), item->GetSocket(1))) == 0)
		return;

	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;

		case 301:
		case 302:
		case 303:
		case 304:
			if( GetLevel() < 90 )
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템의 레벨 제한보다 레벨이 낮습니다."));
				return;
			}
			else
				break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("기억된 위치가 타제국에 속해 있어서 귀환할 수 없습니다."));
		item->SetSocket(0, 0);
		item->SetSocket(1, 0);
	}
	else
	{
		sys_log(1, "Recall: %s %d %d -> %d %d", GetName(), GetX(), GetY(), item->GetSocket(0), item->GetSocket(1));
		WarpSet(item->GetSocket(0), item->GetSocket(1));
		item->SetCount(item->GetCount() - 1);
	}
}

void CHARACTER::__OpenPrivateShop()
{
#ifdef ENABLE_GM_BLOCK
	if (IsGM() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_CREATE"));
		return;
	}
#endif
#ifdef ENABLE_OPEN_SHOP_WITH_ARMOR
	ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
#else
	unsigned bodyPart = GetPart(PART_MAIN);
	switch (bodyPart)
	{
		case 0:
		case 1:
		case 2:
			ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
			break;
		default:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("갑옷을 벗어야 개인 상점을 열 수 있습니다."));
			break;
	}
#endif
}

// MYSHOP_PRICE_LIST
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice)
{
	char szLine[256];
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %u", dwItemVnum, dwItemPrice);
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}

//

//
void CHARACTER::UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p)
{
	const TItemPriceInfo* pInfo = (const TItemPriceInfo*)(p + 1);

	if (!p->byCount)

		SendMyShopPriceListCmd(1, 0);
	else {
		for (int idx = 0; idx < p->byCount; idx++)
			SendMyShopPriceListCmd(pInfo[ idx ].dwVnum, pInfo[ idx ].dwPrice);
	}

	__OpenPrivateShop();
}

//

//
void CHARACTER::UseSilkBotary(void)
{
	if (m_bNoOpenedShop) {
		DWORD dwPlayerID = GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_REQ, GetDesc()->GetHandle(), &dwPlayerID, sizeof(DWORD));
		m_bNoOpenedShop = false;
	} else {
		__OpenPrivateShop();
	}
}
// END_OF_MYSHOP_PRICE_LIST

int CalculateConsume(LPCHARACTER ch)
{
	static const int WARP_NEED_LIFE_PERCENT	= 30;
	static const int WARP_MIN_LIFE_PERCENT	= 10;
	// CONSUME_LIFE_WHEN_USE_WARP_ITEM
	int consumeLife = 0;
	{
		// CheckNeedLifeForWarp
		const int curLife		= ch->GetHP();
		const int needPercent	= WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;
		if (curLife < needLife)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("남은 생명력 양이 모자라 사용할 수 없습니다."));
			return -1;
		}

		consumeLife = needLife;

		const int minPercent	= WARP_MIN_LIFE_PERCENT;
		const int minLife	= ch->GetMaxHP() * minPercent / 100;
		if (curLife - needLife < minLife)
			consumeLife = curLife - minLife;

		if (consumeLife < 0)
			consumeLife = 0;
	}
	// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM
	return consumeLife;
}

int CalculateConsumeSP(LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
		lpChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("남은 정신력 양이 모자라 사용할 수 없습니다."));
		return -1;
	}

	return needSP;
}

// #define ENABLE_FIREWORK_STUN
#define ENABLE_ADDSTONE_FAILURE
bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell)
{
	int iLimitRealtimeStartFirstUseFlagIndex = -1;
	//int iLimitTimerBasedOnWearFlagIndex = -1;

	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limitValue)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템의 레벨 제한보다 레벨이 낮습니다."));
					return false;
				}
				break;

			case LIMIT_REAL_TIME_START_FIRST_USE:
				iLimitRealtimeStartFirstUseFlagIndex = i;
				break;

			case LIMIT_TIMER_BASED_ON_WEAR:
				//iLimitTimerBasedOnWearFlagIndex = i;
				break;
		}
	}

	if (test_server)
	{
		sys_log(0, "USE_ITEM %d %s, Inven %d, Cell %d, DestInven %d, DestCell %d, ItemType %d, SubType %d", item->GetVnum(), item->GetName(), item->GetWindow(), item->GetCell(), bDestInven, wDestCell, item->GetType(), item->GetSubType());
	}

	if ( CArenaManager::instance().IsLimitedItem( GetMapIndex(), item->GetVnum() ) == true )
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
		return false;
	}
#ifdef ENABLE_NEWSTUFF
	else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && IsLimitedPotionOnPVP(item->GetVnum()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
		return false;
	}
#endif

	// @fixme402 (IsLoadedAffect to block affect hacking)
	if (!IsLoadedAffect())
	{
		ChatPacket(CHAT_TYPE_INFO, "Affects are not loaded yet!");
		return false;
	}

	// @fixme141 BEGIN
	if (TItemPos(item->GetWindow(), item->GetCell()).IsBeltInventoryPosition())
	{
		LPITEM beltItem = GetWear(WEAR_BELT);

		if (NULL == beltItem)
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you have no equipped belt.");
			return false;
		}

		if (false == CBeltInventoryHelper::IsAvailableCell(item->GetCell() - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you don't upgrade your belt.");
			return false;
		}
	}
	// @fixme141 END

	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		if (0 == item->GetSocket(1))
		{
			long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[iLimitRealtimeStartFirstUseFlagIndex].lValue;

			if (0 == duration)
				duration = 60 * 60 * 24 * 7;

			item->SetSocket(0, time(0) + duration);
			item->StartRealTimeExpireEvent();
		}

		if (false == item->IsEquipped())
			item->SetSocket(1, item->GetSocket(1) + 1);
	}

#ifdef ENABLE_BATTLE_PASS
	BYTE bBattlePassId = GetBattlePassId();
	if (bBattlePassId)
	{
		DWORD dwItemVnum, dwUseCount;
		if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, USE_ITEM, &dwItemVnum, &dwUseCount))
		{
			if (dwItemVnum == item->GetVnum() && GetMissionProgress(USE_ITEM, bBattlePassId) < dwUseCount)
			{
				UpdateMissionProgress(USE_ITEM, bBattlePassId, 1, dwUseCount);
			}
		}
	}
#endif

#ifdef __SKILL_COSTUME__
	if (item->GetType() == ITEM_SKILL_COSTUME)
	{
		switch (GetJob())
		{
			case JOB_WARRIOR:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
					return false;
				break;

			case JOB_ASSASSIN:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
					return false;
				break;

			case JOB_SHAMAN:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
					return false;
				break;

			case JOB_SURA:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
					return false;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case JOB_WOLFMAN:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
					return false;
				break; // TODO: 수인족 아이템 착용가능여부 처리
#endif
		}
		
		if (item->GetSocket(1) != 0)
		{
			UseSkillCostumeItem(item, false);
			item->SetSocket(1, 0);
		}
		else
		{
			// deactive all
			for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
			{
				LPITEM item3 = GetInventoryItem(i);
				if (item3 && item3->GetSocket(1) != 0 && item3->GetType() == ITEM_SKILL_COSTUME && item3->GetSubType() == item->GetSubType())
				{
					UseSkillCostumeItem(item3, false);
					item3->SetSocket(1, 0);
				}
			}
			
			// ACTIVE
			UseSkillCostumeItem(item, true);
			item->SetSocket(1, 1);
		}
		
		return true;
	}
#endif

	switch (item->GetType())
	{
		case ITEM_HAIR:
			return ItemProcess_Hair(item, wDestCell);

		case ITEM_POLYMORPH:
			return ItemProcess_Polymorph(item);

		case ITEM_QUEST:
#ifdef ENABLE_QUEST_DND_EVENT
			if (IS_SET(item->GetFlag(), ITEM_FLAG_APPLICABLE))
			{
				LPITEM item2;

				if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				quest::CQuestManager::instance().DND(GetPlayerID(), item, item2, false);
				return true;
			}
#endif

#ifdef ENABLE_METIN_STONE_QUEUE
			if (item->GetVnum() >= ITEM_METIN_STONE_QUEUE_START && item->GetVnum() <= ITEM_METIN_STONE_QUEUE_END)
			{
				if (item->isLocked() || item->IsExchanging())
					return false;

				if (FindAffect(AFFECT_AUTO_METIN_FARM))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
					return false;
				}

				DWORD dwExpireTime = item->GetValue(0) == 999 ? INFINITE_AFFECT_DURATION : 60 * 60 * 24 * item->GetValue(0);
				// AddAffect(AFFECT_AUTO_METIN_FARM, 0, 0, AFF_NONE, item->GetValue(0) == 999 ? INFINITE_AFFECT_DURATION : 60 * 60 * 24 * item->GetValue(0), 0, false);
				AddAffect(AFFECT_AUTO_METIN_FARM, 0, 0, AFF_NONE, dwExpireTime, 0, false, false, (get_global_time() + dwExpireTime));

				ChatPacket(CHAT_TYPE_INFO, "You enabled premium metin stone queue.");
				ChatPacket(CHAT_TYPE_INFO, "You can now farm up to 8 metin stones.");

				item->SetCount(item->GetCount() - 1);

				return true;
			}
#endif

#ifdef RENEWAL_PICKUP_AFFECT
			if (item->GetVnum() == ITEM_AUTO_PICK_UP0 || item->GetVnum() == ITEM_AUTO_PICK_UP1 || item->GetVnum() == ITEM_AUTO_PICK_UP2 || item->GetVnum() == ITEM_AUTO_PICK_UP3
				 || item->GetVnum() == ITEM_AUTO_PICK_UP4)
			{
				if (item->isLocked() || item->IsExchanging())
					return false;

				CAffect* affect = FindAffect(AFFECT_PICKUP_ENABLE);
				if (!affect)
				{
					affect = FindAffect(AFFECT_PICKUP_DEACTIVE);
					if (affect)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already have the Auto Loot affect."));
						return false;
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already have the Auto Loot affect."));
					return false;
				}

				if (!item->GetValue(0) && item->GetVnum() != ITEM_AUTO_PICK_UP4)
				{
					ChatPacket(CHAT_TYPE_INFO, "[SYSTEM] Contact game master. This item not has time data!");
					return false;
				}

				if (item->GetVnum() == ITEM_AUTO_PICK_UP4)
					AddAffect(AFFECT_PICKUP_ENABLE, 0, 0, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false, false, (get_global_time() + INFINITE_AFFECT_DURATION));
				else
					AddAffect(AFFECT_PICKUP_ENABLE, 0, 0, AFF_NONE, item->GetValue(0), 0, false, false, (get_global_time() + item->GetValue(0)));

				// sys_err("Reached!");
	
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Succesfully added affect."));
				item->SetCount(item->GetCount() - 1);
				return true;
			}
#endif

			if (GetArena() != NULL || IsObserverMode() == true)
			{
				if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
					return false;
				}
			}

			if (!IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
			{
				if (item->GetSIGVnum() == 0)
					quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
				else
					quest::CQuestManager::instance().SIGUse(GetPlayerID(), item->GetSIGVnum(), item, false);
			}
			break;

		case ITEM_CAMPFIRE:
			{
				float fx, fy;
				GetDeltaByDegree(GetRotation(), 100.0f, &fx, &fy);

				LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy));

				if (!tree)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("모닥불을 피울 수 없는 지점입니다."));
					return false;
				}

				if (tree->IsAttr((long)(GetX()+fx), (long)(GetY()+fy), ATTR_WATER))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("물 속에 모닥불을 피울 수 없습니다."));
					return false;
				}

				LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(fishing::CAMPFIRE_MOB, GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy), 0, false, number(0, 359));

				char_event_info* info = AllocEventInfo<char_event_info>();

				info->ch = campfire;

				campfire->m_pkMiningEvent = event_create(kill_campfire_event, info, PASSES_PER_SEC(40));

				item->SetCount(item->GetCount() - 1);
			}
			break;

		case ITEM_UNIQUE:
			{
				switch (item->GetSubType())
				{
					case USE_ABILITY_UP:
						{
							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true, true);
									break;

								case APPLY_ATT_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true, true);
									break;

								case APPLY_STR:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_RESIST_MAGIC:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_RESIST_MAGIC, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;
							}
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					default:
						{
							if (item->GetSubType() == USE_SPECIAL)
							{
								sys_log(0, "ITEM_UNIQUE: USE_SPECIAL %u", item->GetVnum());

								switch (item->GetVnum())
								{
									case 71049:
										if (g_bEnableBootaryCheck)
										{
											if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
											{
												UseSilkBotary();
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개인 상점을 열 수 없는 지역입니다"));
											}
										}
										else
										{
											UseSilkBotary();
										}
										break;
								}
							}
							else
							{
								if (!item->IsEquipped())
									EquipItem(item);
								else
									UnequipItem(item);
							}
						}
						break;
				}
			}
			break;

		case ITEM_COSTUME:
		case ITEM_WEAPON:
		case ITEM_ARMOR:
		case ITEM_ROD:
		case ITEM_RING:
		case ITEM_BELT:
		// MINING
		case ITEM_PICK:
		// END_OF_MINING
#ifdef ENABLE_SHINING_ITEM_SYSTEM
		case ITEM_SHINING:
#endif
#ifdef ENABLE_BOOSTER_ITEMS
		case ITEM_BOOSTER:
#endif
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;

#ifdef ENABLE_PET_SYSTEM_EX
		case ITEM_PET:
		{
			CPetSystem *petSystem = GetPetSystem();
			if (!petSystem)
			{
				return false;
			}

			switch (item->GetSubType())
			{
			case PET_PAY:
			{
#ifdef ENABLE_COSTUME_PET
				if (!item->IsEquipped())
				{
					if (petSystem->CountSummoned() > 0)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Unequip your current pet first."));
						return false;
					}

					EquipItem(item);
				}
				else
				{
					UnequipItem(item);
				}
#else
				auto mobVnum = item->GetValue(0);
				if (!mobVnum)
				{
					ChatPacket(CHAT_TYPE_INFO, "Item Pet %d with no mob in value0", item->GetVnum());
					return false;
				}
				auto * petSystem = GetPetSystem();
				if (!petSystem)
					return false;

				if (petSystem->CountSummoned())
					petSystem->UnsummonAll();
				else
				{
					// summon
					constexpr bool bFromFar = false;
					const auto * pkMob = CMobManager::instance().Get(mobVnum);
					const auto * petName = (pkMob) ? pkMob->GetLocaleName() : "";
					petSystem->Summon(mobVnum, item, petName, bFromFar);
					// spawn effect
					constexpr const char * spawn_effect_file_name = "d:\\ymir work\\effect\\etc\\appear_die\\npc2_appear.mse";
					auto * petActor = petSystem->GetByVnum(mobVnum);
					if (petActor && petActor->GetCharacter() && item->GetSubType() == PET_PAY)
						petActor->GetCharacter()->SpecificEffectPacket(spawn_effect_file_name);
				}
#endif
			}
			break;
			default:
				return false;
			}
		}
		break;
#endif


#ifdef ENABLE_MOUNT_SYSTEM
		case ITEM_MOUNT:
		{
			switch (item->GetSubType())
			{
			case MOUNT_SUBTYPE_SEAL:
			{
				// Configuration pre-checks
				// DWORD dwMountVnum = (!GetWear(WEAR_MOUNT)) ? item->GetValue(0) : GetWear(WEAR_MOUNT)->GetValue(0);
				DWORD dwMountVnum = (!GetWear(WEAR_MOUNT)) ? item->FindApplyValue(APPLY_MOUNT) : GetWear(WEAR_MOUNT)->FindApplyValue(APPLY_MOUNT);
				GetSpecialMount(dwMountVnum);

				if (!CMobManager::instance().Get(dwMountVnum))
				{
					if (test_server)
					{
						ChatPacket(CHAT_TYPE_INFO, "This item has not been configured properly. You fucked up.");
					}
					return false;
				}

				if (GetMountVnum() || GetHorse())
				{
					StopRiding();
					HorseSummon(false);
					ClearMountItem();

					// Refresh stats
					ComputePoints();
				}
				else
				{
					HorseSummon(true, item->GetValue(1), dwMountVnum, GenerateMountName(dwMountVnum), item);
				}
			}
			break;
			case MOUNT_SUBTYPE_OVERLAY:
			{
				if (!item->IsEquipped())
				{
					EquipItem(item);
				}
				else
				{
					UnequipItem(item);
				}
			}
			break;
			}
		}
		break;
#endif

		case ITEM_DS:
			{
				if (!item->IsEquipped())
					return false;
				return DSManager::instance().PullOut(this, NPOS, item);
			break;
			}
		case ITEM_SPECIAL_DS:
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;

		case ITEM_FISH:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
					return false;
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
					return false;
				}
#endif

				if (item->GetSubType() == FISH_ALIVE)
					fishing::UseFish(this, item);
			}
			break;

		case ITEM_TREASURE_BOX:
			{
				return false;

			}
			break;

		case ITEM_TREASURE_KEY:
			{
				LPITEM item2;

				if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetType() != ITEM_TREASURE_BOX)
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("열쇠로 여는 물건이 아닌것 같다."));
					return false;
				}

				if (item->GetValue(0) == item2->GetValue(0))
				{
					DWORD dwBoxVnum = item2->GetVnum();
					std::vector <DWORD> dwVnums;
					std::vector <DWORD> dwCounts;
					std::vector <LPITEM> item_gets(0);
					int count = 0;

					if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
					{
#ifdef __DAILY_QUESTS__
						CDailyQuestManager::instance().Progress(this, item->GetVnum(), DAILY_TYPE_CHEST, true);
#endif
						
						ITEM_MANAGER::instance().RemoveItem(item);
						ITEM_MANAGER::instance().RemoveItem(item2);

						for (int i = 0; i < count; i++){
							switch (dwVnums[i])
							{
								case CSpecialItemGroup::GOLD:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), dwCounts[i]);
									break;
								case CSpecialItemGroup::EXP:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 부터 신비한 빛이 나옵니다."));
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d의 경험치를 획득했습니다."), dwCounts[i]);
									break;
								case CSpecialItemGroup::MOB:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
									break;
								case CSpecialItemGroup::SLOW:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 빨간 연기를 들이마시자 움직이는 속도가 느려졌습니다!"));
									break;
								case CSpecialItemGroup::DRAIN_HP:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자가 갑자기 폭발하였습니다! 생명력이 감소했습니다."));
									break;
								case CSpecialItemGroup::POISON:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
									break;
#ifdef ENABLE_WOLFMAN_CHARACTER
								case CSpecialItemGroup::BLEEDING:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
									break;
#endif
								case CSpecialItemGroup::MOB_GROUP:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
									break;
								default:
									if (item_gets[i])
									{
										if (dwCounts[i] > 1)
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 %d 개 나왔습니다."), item_gets[i]->GetClientName(), dwCounts[i]);
										else
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 나왔습니다."), item_gets[i]->GetClientName());

									}
							}
						}
					}
					else
					{
						ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("열쇠가 맞지 않는 것 같다."));
						return false;
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("열쇠가 맞지 않는 것 같다."));
					return false;
				}
			}
			break;

		case ITEM_GIFTBOX:
			{
#ifdef ENABLE_GM_CHEST_OPEN_BLOCK
				// SQL-Check: Gehort der Charakter zu einem Admin-Account?
				bool bIsAdminAccount = false;
				if (GetDesc())
				{
					std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT count(*) FROM common.gmlist WHERE mAccount='%s'", GetDesc()->GetAccountTable().login));
					if (pMsg->Get()->uiNumRows > 0)
					{
						MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
						if (row && row[0] && atoi(row[0]) > 0)
							bIsAdminAccount = true;
					}
				}

				if (bIsAdminAccount || IsGM())
				{
					ChatPacket(CHAT_TYPE_INFO, "Admin-Accounts durfen keine Truhen offnen.");
					return false; // Truhe bleibt zu, kein Item-Drop moeglich
				}
#endif

#ifdef ENABLE_CHEST_OPEN_RENEWAL
	#if defined(__BL_67_ATTR__)
				DWORD dwBoxVnum = item->GetVnum();
				switch (dwBoxVnum)
				{
				case POWERSHARD_CHEST:
					if (number(1, 100) <= 30)
						AutoGiveItem(CItemVnumHelper::Get67MaterialVnum(number(0, gPlayerMaxLevel)));
					else
						ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("아무것도 얻을 수 없었습니다."));
					item->SetCount(item->GetCount() - 1);
					return true;
				default:
					break;
				}
	#endif

				OpenChestItem(item);
#else
#if defined(ENABLE_NEWSTUFF) && !defined(ENABLE_QUICK_OPEN)
				if (g_BoxUseTimeLimitValue && !PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::BoxOpening, std::chrono::milliseconds(g_BoxUseTimeLimitValue)))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait."));
					return false;
				}
#endif
				if (quest::CQuestManager::instance().GetEventFlag("disable_giftbox") == 1)
				{
					ChatPacket(CHAT_TYPE_INFO, "boxes are disabled for now !");
					return false;
				}

				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if ((dwBoxVnum > 51500 && dwBoxVnum < 52000) || (dwBoxVnum >= 50255 && dwBoxVnum <= 50260))
				{
					if( !(this->DragonSoul_IsQualified()) )
					{
						ChatPacket(CHAT_TYPE_INFO,LC_TEXT("먼저 용혼석 퀘스트를 완료하셔야 합니다."));
						return false;
					}

					if (NULL != DragonSoul_RefineWindow_GetOpener())
					{
						ChatPacket(CHAT_TYPE_INFO,LC_TEXT("Close the refine window first!"));
						return false;
					}

					// if (GetEmptyInEachDsRoughCategory() == false)
					// {
					// 	ChatPacket(CHAT_TYPE_INFO, "In order to open this chest you need atleast one empty slot");
					// 	ChatPacket(CHAT_TYPE_INFO, "in each Alchemy category.");
					// 	return false;
					// }
					auto DragonSoulInventoryCheck = GetEmptyDragonSoulInventoryType();
                    if (DragonSoulInventoryCheck != 0)
                    {
                        auto DragonSoulGradeTypes = DragonSoulInventoryCheck / 10;
                        auto DragonSoulStepTypes = DragonSoulInventoryCheck % 10;

                        if (DragonSoulStepTypes == -1)
                        {
                            return false;
                        }

                        std::string col_type = "";
                        std::string row_type = "";

                        if (DragonSoulGradeTypes == 0)
                            col_type = "Normal";
                        else if (DragonSoulGradeTypes == 1)
                            col_type = "Brilliant";
                        else if (DragonSoulGradeTypes == 2)
                            col_type = "Rare";
                        else if (DragonSoulGradeTypes == 3)
                            col_type = "Ancient";
                        else if (DragonSoulGradeTypes == 4)
                            col_type = "Legendary";
                        // else if (DragonSoulGradeTypes == 5)
                        //     col_type = "Mythic";

                        if (DragonSoulStepTypes == 0)
                            row_type = "Lowest";
                        else if (DragonSoulStepTypes == 1)
                            row_type = "Low";
                        else if (DragonSoulStepTypes == 2)
                            row_type = "Mid";
                        else if (DragonSoulStepTypes == 3)
                            row_type = "High";
                        else if (DragonSoulStepTypes == 4)
                            row_type = "Highest";

                        ChatPacket(CHAT_TYPE_INFO, "%s alchemy %s inventory is full!", col_type.c_str(), row_type.c_str());
                        return false;
                    }
				}

#if defined(__BL_67_ATTR__)
				switch (dwBoxVnum)
				{
				case POWERSHARD_CHEST:
					// The Powershard Chest can be purchased from Seon-Hae in exchange for 10 Skill Books.(From Wiki)
					// It can contain Powershards of any kind or a Skill Book.(From Wiki)
					// You can edit here for skill books(From black)
					if (number(1, 100) <= 30)
						AutoGiveItem(CItemVnumHelper::Get67MaterialVnum(number(0, gPlayerMaxLevel)));
					else
						ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("아무것도 얻을 수 없었습니다."));
					item->SetCount(item->GetCount() - 1);
					return true;
				// case ELEGANT_POWERSHARD_CHEST:
				// 	if (number(1, 100) <= 60)
				// 		AutoGiveItem(CItemVnumHelper::Get67MaterialVnum(number(0, gPlayerMaxLevel)));
				// 	else
				// 		ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("아무것도 얻을 수 없었습니다."));
				// 	item->SetCount(item->GetCount() - 1);
				// 	return true;
				// case LUCENT_POWERSHARD_CHEST:
				// 	for (BYTE _i = 0; _i < 5; _i++)
				// 		AutoGiveItem(CItemVnumHelper::Get67MaterialVnum(number(0, gPlayerMaxLevel)));
				// 	item->SetCount(item->GetCount() - 1);
				// 	return true;
				default:
					break;
				}
#endif

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
#ifdef __DAILY_QUESTS__
					CDailyQuestManager::instance().Progress(this, item->GetVnum(), DAILY_TYPE_CHEST, true);
#endif
					
					item->SetCount(item->GetCount()-1);
#ifdef ENABLE_RANK_PLAYER
					RankPlayer::instance().SendInfoPlayer(this, RANK_BY_BOX_OPENED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_BOX_OPENED) + 1, false);
#endif
#ifdef __RANKING_SYSTEM__
					RankPlayer::instance().SendInfoPlayer(this, RANK_BY_BOX_OPENED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_BOX_OPENED) + 1, false);
#endif

					for (int i = 0; i < count; i++){
						switch (dwVnums[i])
						{
						case CSpecialItemGroup::GOLD:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), dwCounts[i]);
							break;
						case CSpecialItemGroup::EXP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 부터 신비한 빛이 나옵니다."));
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d의 경험치를 획득했습니다."), dwCounts[i]);
							break;
						case CSpecialItemGroup::MOB:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
							break;
						case CSpecialItemGroup::SLOW:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 빨간 연기를 들이마시자 움직이는 속도가 느려졌습니다!"));
							break;
						case CSpecialItemGroup::DRAIN_HP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자가 갑자기 폭발하였습니다! 생명력이 감소했습니다."));
							break;
						case CSpecialItemGroup::POISON:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
							break;
#ifdef ENABLE_WOLFMAN_CHARACTER
						case CSpecialItemGroup::BLEEDING:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
							break;
#endif
						case CSpecialItemGroup::MOB_GROUP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
							break;
						default:
							if (item_gets[i])
							{
								if (dwCounts[i] > 1)
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 %d 개 나왔습니다."), item_gets[i]->GetClientName(), dwCounts[i]);
								else
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 나왔습니다."), item_gets[i]->GetClientName());
							}
						}
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("아무것도 얻을 수 없었습니다."));
					return false;
				}
#endif
			}
			break;

		case ITEM_SKILLFORGET:
			{
				if (!item->GetSocket(0))
				{
					ITEM_MANAGER::instance().RemoveItem(item);
					return false;
				}

				DWORD dwVnum = item->GetSocket(0);

				if (SkillLevelDown(dwVnum))
				{
					ITEM_MANAGER::instance().RemoveItem(item);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("스킬 레벨을 내리는데 성공하였습니다."));
				}
				else
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("스킬 레벨을 내릴 수 없습니다."));
			}
			break;

		case ITEM_SKILLBOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
					return false;
				}

				DWORD dwVnum = 0;

				if (item->GetVnum() == 50300)
				{
					dwVnum = item->GetSocket(0);
				}
				else
				{
					dwVnum = item->GetValue(0);
				}

				if (0 == dwVnum)
				{
					ITEM_MANAGER::instance().RemoveItem(item);

					return false;
				}

				if (true == LearnSkillByBook(dwVnum))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

					if (distribution_test_server)
						iReadDelay /= 3;

					SetSkillNextReadTime(dwVnum, get_global_time() + iReadDelay);
				}
			}
			break;

		case ITEM_USE:
			{
				if (item->GetVnum() > 50800 && item->GetVnum() <= 50820)
				{
					if (test_server)
						sys_log (0, "ADD addtional effect : vnum(%d) subtype(%d)", item->GetOriginalVnum(), item->GetSubType());

					int affect_type = AFFECT_EXP_BONUS_EURO_FREE;
					int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
					int apply_value = item->GetValue(2);
					int apply_duration = item->GetValue(1);

					switch (item->GetSubType())
					{
						case USE_ABILITY_UP:
							if (FindAffect(affect_type, apply_type))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
								return false;
							}

							{
								switch (item->GetValue(0))
								{
									case APPLY_MOV_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_MOV_SPEED_POTION, apply_duration, 0, true, true);
										break;

									case APPLY_ATT_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_ATT_SPEED_POTION, apply_duration, 0, true, true);
										break;

									case APPLY_STR:
									case APPLY_DEX:
									case APPLY_CON:
									case APPLY_INT:
									case APPLY_CAST_SPEED:
									case APPLY_RESIST_MAGIC:
									case APPLY_ATT_GRADE_BONUS:
									case APPLY_DEF_GRADE_BONUS:
										AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, true, true);
										break;
								}
							}

							if (GetDungeon())
								GetDungeon()->UsePotion(this);

							if (GetWarMap())
								GetWarMap()->UsePotion(this, item);

							item->SetCount(item->GetCount() - 1);
							break;

					case USE_AFFECT :
						{
							if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
							}
							else
							{
								AddAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false, true);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_POTION_NODELAY:
						{
							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
									return false;
								}

								switch (item->GetVnum())
								{
									case 70020 :
									case 71018 :
									case 71019 :
									case 71020 :
										if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
										{
											if (m_nPotionLimit <= 0)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용 제한량을 초과하였습니다."));
												return false;
											}
										}
										break;

									default :
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
										return false;
										break;
								}
							}
#ifdef ENABLE_NEWSTUFF
							else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
								return false;
							}
#endif

							bool used = false;

							if (item->GetValue(0) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
								{
									if (test_server)
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("월병 또는 종자 를 사용하였습니다"));
									SetUseSeedOrMoonBottleTime();
								}
								if (GetDungeon())
									GetDungeon()->UsePotion(this);

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								m_nPotionLimit--;

								//RESTRICT_USE_SEED_OR_MOONBOTTLE
								item->SetCount(item->GetCount() - 1);
								//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
							}
						}
						break;
					}

					return true;
				}

				if (item->GetVnum() >= 27863 && item->GetVnum() <= 27883)
				{
					if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
						return false;
					}
#ifdef ENABLE_NEWSTUFF
					else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
						return false;
					}
#endif
				}

				if (test_server)
				{
					 sys_log (0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), item->GetType(), item->GetSubType(), item->GetOriginalVnum());
				}

#ifdef ENABLE_ITEM_TIME_EXTENDER
	if (item->IsItemTimeExtender())
	{
		LPITEM item2;

		if (!IsValidItemPosition(DestCell))
			return false;

		item2 = GetItem(DestCell);

		if (!item2)
			return false;

		if (item2->IsExchanging() || item2->IsEquipped())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item when trading or when it is equipped."));
			return false;
		}

		if (item2->GetType() != ITEM_COSTUME && item2->GetType() != ITEM_PET && item2->GetType() != ITEM_MOUNT)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item for this item."));
			return false;
		}

		if (item2->GetSocket(0) <= 0 && item2->GetProto()->aLimits[0].bType == LIMIT_REAL_TIME_START_FIRST_USE)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You must equip the item first to add time."));
			return false;
		}

		switch (item2->GetSubType())
		{
			case COSTUME_BODY:
			case COSTUME_HAIR:
			case COSTUME_WEAPON:
			case MOUNT_SEAL:
			case PET_PAY:
			{
				const int addTime = item->GetValue(0);
				const int addDays = addTime / 86400;

				item2->SetSocket(0, item2->GetSocket(0) + addTime);
				item->SetCount(item->GetCount() - 1);

				ChatPacket(CHAT_TYPE_INFO, addDays == 1 ? LC_TEXT("%d day has been added to the item.") : LC_TEXT("%d days have been added to the item."), addDays);
				break;
			}

			default:
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item for this item."));
				break;
			}
		}
	}
#endif

				switch (item->GetSubType())
				{
					case USE_TIME_CHARGE_PER:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}

							if (pDestItem->IsDragonSoul())
							{
								int ret;
								char buf[128];
								if (item->GetVnum() == DRAGON_HEART_VNUM)
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
								}
								else
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
								}
								if (ret > 0)
								{
									if (item->GetVnum() == DRAGON_HEART_VNUM)
									{
										sprintf(buf, "Inc %ds by item{VN:%d SOC%d:%ld}", ret, item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									}
									else
									{
										sprintf(buf, "Inc %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									}

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds have been added."), ret);
									item->SetCount(item->GetCount() - 1);
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									return true;
								}
								else
								{
									if (item->GetVnum() == DRAGON_HEART_VNUM)
									{
										sprintf(buf, "No change by item{VN:%d SOC%d:%ld}", item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									}
									else
									{
										sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									}

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot recharge this Dragon Stone."));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
									return false;
								}
							}
							else
								return false;
						}
						break;
					case USE_TIME_CHARGE_FIX:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}

							if (pDestItem->IsDragonSoul())
							{
								int ret = pDestItem->GiveMoreTime_Fix(item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
								char buf[128];
								if (ret)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds have been added."), ret);
									sprintf(buf, "Increase %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									item->SetCount(item->GetCount() - 1);
									return true;
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot recharge this Dragon Stone."));
									sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
									return false;
								}
							}
							else
								return false;
						}
						break;
					case USE_SPECIAL:

						switch (item->GetVnum())
						{
							case ITEM_NOG_POCKET:
								{
									if (FindAffect(AFFECT_NOG_ABILITY))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
										return false;
									}
									long time = item->GetValue(0);
									long moveSpeedPer	= item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer			= item->GetValue(3);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;

							case ITEM_RAMADAN_CANDY:
								{
									// @fixme147 BEGIN
									if (FindAffect(AFFECT_RAMADAN_ABILITY))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
										return false;
									}
									// @fixme147 END
									long time = item->GetValue(0);
									long moveSpeedPer	= item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer			= item->GetValue(3);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;
							case ITEM_MARRIAGE_RING:
								{
									marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
									if (pMarriage)
									{
										if (pMarriage->ch1 != NULL)
										{
											if (CArenaManager::instance().IsArenaMap(pMarriage->ch1->GetMapIndex()) == true)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
												break;
											}
										}

										if (pMarriage->ch2 != NULL)
										{
											if (CArenaManager::instance().IsArenaMap(pMarriage->ch2->GetMapIndex()) == true)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
												break;
											}
										}

										int consumeSP = CalculateConsumeSP(this);

										if (consumeSP < 0)
											return false;

										PointChange(POINT_SP, -consumeSP, false);

										WarpToPID(pMarriage->GetOther(GetPlayerID()));
									}
									else
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("결혼 상태가 아니면 결혼반지를 사용할 수 없습니다."));
								}
								break;

							case UNIQUE_ITEM_CAPE_OF_COURAGE:
							case 70057:
							case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
								AggregateMonster();
#ifndef ENABLE_UNLIMITED_CAPE_OF_COURAGE
								item->SetCount(item->GetCount()-1);
#endif
								break;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
							case ACCE_REVERSAL_VNUM_1:
							case ACCE_REVERSAL_VNUM_2:
							{
								LPITEM item2;
								if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
									return false;

								if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
									return false;

								if (!CleanAcceAttr(item, item2))
									return false;
								item->SetCount(item->GetCount()-1);
								break;
							}
#endif

							case UNIQUE_ITEM_WHITE_FLAG:
								ForgetMyAttacker();
								item->SetCount(item->GetCount()-1);
								break;

							case UNIQUE_ITEM_TREASURE_BOX:
								break;

#ifdef ENABLE_GEM_SYSTEM
							case 50926:
								item->SetCount(item->GetCount() - 1);
								PointChange(POINT_GEM, 1, false);
								break;
#endif

							case 30093:
							case 30094:
							case 30095:
							case 30096:

								{
									const int MAX_BAG_INFO = 26;
									static struct LuckyBagInfo
									{
										DWORD count;
										int prob;
										DWORD vnum;
									} b1[MAX_BAG_INFO] =
									{
										{ 1000,	302,	1 },
										{ 10,	150,	27002 },
										{ 10,	75,	27003 },
										{ 10,	100,	27005 },
										{ 10,	50,	27006 },
										{ 10,	80,	27001 },
										{ 10,	50,	27002 },
										{ 10,	80,	27004 },
										{ 10,	50,	27005 },
										{ 1,	10,	50300 },
										{ 1,	6,	92 },
										{ 1,	2,	132 },
										{ 1,	6,	1052 },
										{ 1,	2,	1092 },
										{ 1,	6,	2082 },
										{ 1,	2,	2122 },
										{ 1,	6,	3082 },
										{ 1,	2,	3122 },
										{ 1,	6,	5052 },
										{ 1,	2,	5082 },
										{ 1,	6,	7082 },
										{ 1,	2,	7122 },
										{ 1,	1,	11282 },
										{ 1,	1,	11482 },
										{ 1,	1,	11682 },
										{ 1,	1,	11882 },
									};

									LuckyBagInfo * bi = NULL;
									bi = b1;

									int pct = number(1, 1000);

									int i;
									for (i=0;i<MAX_BAG_INFO;i++)
									{
										if (pct <= bi[i].prob)
											break;
										pct -= bi[i].prob;
									}
									if (i>=MAX_BAG_INFO)
										return false;

									if (bi[i].vnum == 50300)
									{
										GiveRandomSkillBook();
									}
									else if (bi[i].vnum == 1)
									{
#ifdef ENABLE_GOLD_MAX_EXTENDED
										ChangeGold(1000);
#else
										PointChange(POINT_GOLD, 1000, true);
#endif
									}
									else
									{
										AutoGiveItem(bi[i].vnum, bi[i].count);
									}
									ITEM_MANAGER::instance().RemoveItem(item);
								}
								break;

							// CASH_ITEMS
							case 80014:
							case 80015:
							case 80016:
							case 80017:
							case 80018:
								{
									if (item->isLocked() || item->IsExchanging())
										return false;

									long lItemCashValue = item->GetValue(0);

									SetCoins(GetCoins() + lItemCashValue);
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d cash added successfully to your account!"), lItemCashValue);
									item->SetCount(item->GetCount() - 1);
								}
								break;
							// CASH_ITEMS_END

#ifdef ENABLE_BATTLE_PASS
							case 50512:
							{
								char szQuery[512];
								snprintf(szQuery, sizeof(szQuery), "SELECT * FROM battle_pass_ranking WHERE player_name = '%s' AND battle_pass_id = %d;", GetName(), item->GetValue(0));
								std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(szQuery));

								if (pmsg->Get()->uiNumRows > 0)
								{
									ChatPacket(CHAT_TYPE_INFO, "You already finished battlepass.");
									return false;
								}

								if (FindAffect(AFFECT_BATTLE_PASS))
								{
									ChatPacket(CHAT_TYPE_INFO, "You already have a battlepass on this character.");
									return false;
								}
								else
								{
									time_t cur_Time = time(NULL);
									struct tm vKey = *localtime(&cur_Time);
									int day = NumberOfDaysInMonth(vKey.tm_mon, vKey.tm_year + 1900) - vKey.tm_mday;
									int month = vKey.tm_mon;

									// 0 = January
									// 1 = February
									// 2 = March
									// 3 = April
									// 4 = May
									// 5 = June
									// 6 = July
									// 7 = August
									// 8 = September
									// 9 = Octomber
									// 10 = November
									// 11 = December

									if (month != quest::CQuestManager::instance().GetEventFlag("battle_pass_month"))
									{
										ChatPacket(CHAT_TYPE_INFO, "You can't use battlepass ticket now.");
										return false;
									}

									auto lDuration = day*24*60*60 + (23 - vKey.tm_hour)*60*60 + (59 - vKey.tm_min)*60 + (60 - vKey.tm_sec);
									AddAffect(AFFECT_BATTLE_PASS, POINT_BATTLE_PASS_ID, 1, 0, lDuration, 0, true, false, (get_global_time() + lDuration));

									ITEM_MANAGER::instance().RemoveItem(item);
								}
							}
							break;
#endif

							case 50004:
								{
									if (item->GetSocket(0))
									{
										item->SetSocket(0, item->GetSocket(0) + 1);
									}
									else
									{
										int iMapIndex = GetMapIndex();

										PIXEL_POSITION pos;

										if (SECTREE_MANAGER::instance().GetRandomLocation(iMapIndex, pos, 700))
										{
											item->SetSocket(0, 1);
											item->SetSocket(1, pos.x);
											item->SetSocket(2, pos.y);
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 곳에선 이벤트용 감지기가 동작하지 않는것 같습니다."));
											return false;
										}
									}

									int dist = 0;
									float distance = (DISTANCE_SQRT(GetX()-item->GetSocket(1), GetY()-item->GetSocket(2)));

									if (distance < 1000.0f)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이벤트용 감지기가 신비로운 빛을 내며 사라집니다."));

										struct TEventStoneInfo
										{
											DWORD dwVnum;
											int count;
											int prob;
										};
										const int EVENT_STONE_MAX_INFO = 15;
										TEventStoneInfo info_10[EVENT_STONE_MAX_INFO] =
										{
											{ 27001, 10,  8 },
											{ 27004, 10,  6 },
											{ 27002, 10, 12 },
											{ 27005, 10, 12 },
											{ 27100,  1,  9 },
											{ 27103,  1,  9 },
											{ 27101,  1, 10 },
											{ 27104,  1, 10 },
											{ 27999,  1, 12 },

											{ 25040,  1,  4 },

											{ 27410,  1,  0 },
											{ 27600,  1,  0 },
											{ 25100,  1,  0 },

											{ 50001,  1,  0 },
											{ 50003,  1,  1 },
										};
										TEventStoneInfo info_7[EVENT_STONE_MAX_INFO] =
										{
											{ 27001, 10,  1 },
											{ 27004, 10,  1 },
											{ 27004, 10,  9 },
											{ 27005, 10,  9 },
											{ 27100,  1,  5 },
											{ 27103,  1,  5 },
											{ 27101,  1, 10 },
											{ 27104,  1, 10 },
											{ 27999,  1, 14 },

											{ 25040,  1,  5 },

											{ 27410,  1,  5 },
											{ 27600,  1,  5 },
											{ 25100,  1,  5 },

											{ 50001,  1,  0 },
											{ 50003,  1,  5 },

										};
										TEventStoneInfo info_4[EVENT_STONE_MAX_INFO] =
										{
											{ 27001, 10,  0 },
											{ 27004, 10,  0 },
											{ 27002, 10,  0 },
											{ 27005, 10,  0 },
											{ 27100,  1,  0 },
											{ 27103,  1,  0 },
											{ 27101,  1,  0 },
											{ 27104,  1,  0 },
											{ 27999,  1, 25 },

											{ 25040,  1,  0 },

											{ 27410,  1,  0 },
											{ 27600,  1,  0 },
											{ 25100,  1, 15 },

											{ 50001,  1, 10 },
											{ 50003,  1, 50 },

										};

										{
											TEventStoneInfo* info;
											if (item->GetSocket(0) <= 4)
												info = info_4;
											else if (item->GetSocket(0) <= 7)
												info = info_7;
											else
												info = info_10;

											int prob = number(1, 100);

											for (int i = 0; i < EVENT_STONE_MAX_INFO; ++i)
											{
												if (!info[i].prob)
													continue;

												if (prob <= info[i].prob)
												{
													AutoGiveItem(info[i].dwVnum, info[i].count);
													break;
												}
												prob -= info[i].prob;
											}
										}

										char chatbuf[CHAT_MAX_LEN + 1];
										int len = snprintf(chatbuf, sizeof(chatbuf), "StoneDetect %u 0 0", (DWORD)GetVID());

										if (len < 0 || len >= (int) sizeof(chatbuf))
											len = sizeof(chatbuf) - 1;

										++len;

										TPacketGCChat pack_chat;
										pack_chat.header	= HEADER_GC_CHAT;
										pack_chat.size		= sizeof(TPacketGCChat) + len;
										pack_chat.type		= CHAT_TYPE_COMMAND;
										pack_chat.id		= 0;
										pack_chat.bEmpire	= GetDesc()->GetEmpire();
										//pack_chat.id	= vid;

										TEMP_BUFFER buf;
										buf.write(&pack_chat, sizeof(TPacketGCChat));
										buf.write(chatbuf, len);

										PacketAround(buf.read_peek(), buf.size());

										ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 1");
										return true;
									}
									else if (distance < 20000)
										dist = 1;
									else if (distance < 70000)
										dist = 2;
									else
										dist = 3;

									const int STONE_DETECT_MAX_TRY = 10;
									if (item->GetSocket(0) >= STONE_DETECT_MAX_TRY)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이벤트용 감지기가 흔적도 없이 사라집니다."));
										ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 0");
										AutoGiveItem(27002);
										return true;
									}

									if (dist)
									{
										char chatbuf[CHAT_MAX_LEN + 1];
										int len = snprintf(chatbuf, sizeof(chatbuf),
												"StoneDetect %u %d %d",
											   	(DWORD)GetVID(), dist, (int)GetDegreeFromPositionXY(GetX(), item->GetSocket(2), item->GetSocket(1), GetY()));

										if (len < 0 || len >= (int) sizeof(chatbuf))
											len = sizeof(chatbuf) - 1;

										++len;

										TPacketGCChat pack_chat;
										pack_chat.header	= HEADER_GC_CHAT;
										pack_chat.size		= sizeof(TPacketGCChat) + len;
										pack_chat.type		= CHAT_TYPE_COMMAND;
										pack_chat.id		= 0;
										pack_chat.bEmpire	= GetDesc()->GetEmpire();
										//pack_chat.id		= vid;

										TEMP_BUFFER buf;
										buf.write(&pack_chat, sizeof(TPacketGCChat));
										buf.write(chatbuf, len);

										PacketAround(buf.read_peek(), buf.size());
									}

								}
								break;

							case 27989:
							case 76006:
								{
									LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

									if (pMap != NULL)
									{
										item->SetSocket(0, item->GetSocket(0) + 1);

										FFindStone f;

										// <Factor> SECTREE::for_each -> SECTREE::for_each_entity
										pMap->for_each(f);

										if (f.m_mapStone.size() > 0)
										{
											std::map<DWORD, LPCHARACTER>::iterator stone = f.m_mapStone.begin();

											DWORD max = UINT_MAX;
											LPCHARACTER pTarget = stone->second;

											while (stone != f.m_mapStone.end())
											{
												DWORD dist = (DWORD)DISTANCE_SQRT(GetX()-stone->second->GetX(), GetY()-stone->second->GetY());

												if (dist != 0 && max > dist)
												{
													max = dist;
													pTarget = stone->second;
												}
												stone++;
											}

											if (pTarget != NULL)
											{
												int val = 3;

												if (max < 10000) val = 2;
												else if (max < 70000) val = 1;

												ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u %d %d", (DWORD)GetVID(), val,
														(int)GetDegreeFromPositionXY(GetX(), pTarget->GetY(), pTarget->GetX(), GetY()));
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("감지기를 작용하였으나 감지되는 영석이 없습니다."));
											}
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("감지기를 작용하였으나 감지되는 영석이 없습니다."));
										}

										if (item->GetSocket(0) >= 6)
										{
											ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u 0 0", (DWORD)GetVID());
											ITEM_MANAGER::instance().RemoveItem(item);
										}
									}
									break;
								}
								break;

							case 27996:
								item->SetCount(item->GetCount() - 1);
								AttackedByPoison(NULL); // @warme008
								break;

							case 27987:

								{
									item->SetCount(item->GetCount() - 1);

									int r = number(1, 100);

									if (r <= 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개에서 돌조각이 나왔습니다."));
										AutoGiveItem(27990);
									}
									else
									{
										const int prob_table_gb2312[] =
										{
											95, 97, 99
										};

										const int * prob_table = prob_table_gb2312;

										if (r <= prob_table[0])
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개가 흔적도 없이 사라집니다."));
										}
										else if (r <= prob_table[1])
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개에서 백진주가 나왔습니다."));
											AutoGiveItem(27992);
										}
										else if (r <= prob_table[2])
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개에서 청진주가 나왔습니다."));
											AutoGiveItem(27993);
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개에서 피진주가 나왔습니다."));
											AutoGiveItem(27994);
										}
									}
								}
								break;

							case 71013:
								CreateFly(number(FLY_FIREWORK1, FLY_FIREWORK6), this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50100:
							case 50101:
							case 50102:
							case 50103:
							case 50104:
							case 50105:
							case 50106:
								CreateFly(item->GetVnum() - 50100 + FLY_FIREWORK1, this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50200:
								if (g_bEnableBootaryCheck)
								{
									if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
									{
#ifdef __PREMIUM_PRIVATE_SHOP__
										if (IsPrivateShopOwner())
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Close your current personal shop before opening a new one."));
											return false;
										}

										OpenPrivateShopPanel();
#else
										__OpenPrivateShop();
#endif
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개인 상점을 열 수 없는 지역입니다"));
									}
								}
								else
								{
#ifdef __PREMIUM_PRIVATE_SHOP__
									if (IsPrivateShopOwner())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Close your current personal shop before opening a new one."));
										return false;
									}

									OpenPrivateShopPanel();
#else
									__OpenPrivateShop();
#endif
								}
								break;

#ifdef __PREMIUM_PRIVATE_SHOP__
							case 71221:
							{
								if (IsPrivateShopOwner())
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Close your current personal shop before opening a new one."));
									return false;
								}

								OpenPrivateShopPanel();
								ChatPacket(CHAT_TYPE_COMMAND, "SetPrivateShopPremiumBuild");
							} break;

								OpenShopSearch(MODE_TRADING);
#endif

							case fishing::FISH_MIND_PILL_VNUM:
								AddAffect(AFFECT_FISH_MIND_PILL, POINT_NONE, 0, AFF_FISH_MIND, 20*60, 0, true);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50301:
							case 50302:
							case 50303:
								{
									if (IsPolymorphed() == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 능력을 올릴 수 없습니다."));
										return false;
									}

									int lv = GetSkillLevel(SKILL_LEADERSHIP);

									if (lv < item->GetValue(0))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책은 너무 어려워 이해하기가 힘듭니다."));
										return false;
									}

									if (lv >= item->GetValue(1))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책은 아무리 봐도 도움이 될 것 같지 않습니다."));
										return false;
									}

									if (LearnSkillByBook(SKILL_LEADERSHIP))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(SKILL_LEADERSHIP, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50304:
							case 50305:
							case 50306:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									if (GetSkillLevel(SKILL_COMBO) == 0 && GetLevel() < 30)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("레벨 30이 되기 전에는 습득할 수 있을 것 같지 않습니다."));
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) == 1 && GetLevel() < 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("레벨 50이 되기 전에는 습득할 수 있을 것 같지 않습니다."));
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) >= 2)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("연계기는 더이상 수련할 수 없습니다."));
										return false;
									}

									int iPct = item->GetValue(0);

									if (LearnSkillByBook(SKILL_COMBO, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(SKILL_COMBO, get_global_time() + iReadDelay);
									}
								}
								break;
							case 50311:
							case 50312:
							case 50313:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);
									if (GetSkillLevel(dwSkillVnum)>=20 || dwSkillVnum-SKILL_LANGUAGE1+1 == GetEmpire())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 완벽하게 알아들을 수 있는 언어이다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50061 :
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 10)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 수련할 수 없습니다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50314: case 50315: case 50316:
							case 50323: case 50324:
							case 50325: case 50326:
								{
									if (IsPolymorphed() == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 능력을 올릴 수 없습니다."));
										return false;
									}

									int iSkillLevelLowLimit = item->GetValue(0);
									int iSkillLevelHighLimit = item->GetValue(1);
									int iPct = MINMAX(0, item->GetValue(2), 100);
									int iLevelLimit = item->GetValue(3);
									DWORD dwSkillVnum = 0;

									switch (item->GetVnum())
									{
										case 50314: case 50315: case 50316:
											dwSkillVnum = SKILL_POLYMORPH;
											break;

										case 50323: case 50324:
											dwSkillVnum = SKILL_ADD_HP;
											break;

										case 50325: case 50326:
											dwSkillVnum = SKILL_RESIST_PENETRATE;
											break;

										default:
											return false;
									}

									if (0 == dwSkillVnum)
										return false;

									if (GetLevel() < iLevelLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책을 읽으려면 레벨을 더 올려야 합니다."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 수련할 수 없습니다."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책은 너무 어려워 이해하기가 힘듭니다."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책으로는 더 이상 수련할 수 없습니다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50902:
							case 50903:
							case 50904:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = SKILL_CREATE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum)>=40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 수련할 수 없습니다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);

										if (test_server)
										{
											ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Success to learn skill ");
										}
									}
									else
									{
										if (test_server)
										{
											ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Failed to learn skill ");
										}
									}
								}
								break;

								// MINING
							case ITEM_MINING_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = SKILL_MINING;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum)>=40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 수련할 수 없습니다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;
								// END_OF_MINING

							case ITEM_HORSE_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = SKILL_HORSE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetLevel() < 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 승마 스킬을 수련할 수 있는 레벨이 아닙니다."));
										return false;
									}

									if (!test_server && get_global_time() < GetSkillNextReadTime(dwSkillVnum))
									{
										if (FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
										{
											RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("주안술서를 통해 주화입마에서 빠져나왔습니다."));
										}
										else
										{
											SkillLearnWaitMoreTimeMessage(GetSkillNextReadTime(dwSkillVnum) - get_global_time());
											return false;
										}
									}

									if (GetPoint(POINT_HORSE_SKILL) >= 20 ||
											GetSkillLevel(SKILL_HORSE_WILDATTACK) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60 ||
											GetSkillLevel(SKILL_HORSE_WILDATTACK_RANGE) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 승마 수련서를 읽을 수 없습니다."));
										return false;
									}

									if (number(1, 100) <= iPct)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("승마 수련서를 읽어 승마 스킬 포인트를 얻었습니다."));
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("얻은 포인트로는 승마 스킬의 레벨을 올릴 수 있습니다."));
										PointChange(POINT_HORSE_SKILL, 1);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										if (!test_server)
											SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("승마 수련서 이해에 실패하였습니다."));
									}
#ifdef ENABLE_BOOKS_STACKFIX
									item->SetCount(item->GetCount() - 1);
#else
									ITEM_MANAGER::instance().RemoveItem(item);
#endif
								}
								break;

							case 70102:
							case 70103:
								{
									if (GetAlignment() >= 0)
										return false;

									int delta = MIN(-GetAlignment(), item->GetValue(0));

									sys_log(0, "%s ALIGNMENT ITEM %d", GetName(), delta);

									UpdateAlignment(delta);
									item->SetCount(item->GetCount() - 1);

									if (delta / 10 > 0)
									{
										ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("마음이 맑아지는군. 가슴을 짓누르던 무언가가 좀 가벼워진 느낌이야."));
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("선악치가 %d 증가하였습니다."), delta/10);
									}
								}
								break;

							case 71107:
							case 39032: // @fixme169 mythical peach alternative vnum
								{
									//int val = item->GetValue(0);
									int val = 500; // Festgelegt auf 500 Punkte

									int interval = item->GetValue(1);
									quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
									if (!pPC) // @fixme169 missing check
										return false;

									/*int last_use_time = pPC->GetFlag("mythical_peach.last_use_time");

									if (get_global_time() - last_use_time < interval * 60 * 60)
									{
										if (test_server == false)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 사용할 수 없습니다."));
											return false;
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("테스트 서버 시간제한 통과"));
										}
									}
									*/
									if (GetAlignment() == 200000)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("선악치를 더 이상 올릴 수 없습니다."));
										return false;
									}

									if (200000 - GetAlignment() < val * 10)
									{
										val = (200000 - GetAlignment()) / 10;
									}

									int old_alignment = GetAlignment() / 10;

									UpdateAlignment(val*10);

									item->SetCount(item->GetCount()-1);
									pPC->SetFlag("mythical_peach.last_use_time", get_global_time());

									ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("마음이 맑아지는군. 가슴을 짓누르던 무언가가 좀 가벼워진 느낌이야."));
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("선악치가 %d 증가하였습니다."), val);

									char buf[256 + 1];
									snprintf(buf, sizeof(buf), "%d %d", old_alignment, GetAlignment() / 10);
									LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
								}
								break;

							case 71109:
							case 72719:
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;
									
									if (item2->IsItemNewStoneEquipment())
									{
										DWORD dwStone = item2->GetSocket(0);
										if (dwStone == 0)
											return false;
										
										LPITEM pItemReward = AutoGiveItem(dwStone);
										if (pItemReward != NULL)
										{
											item2->SetSocket(0, 0);
											item2->UpdatePacket();
											ITEM_MANAGER::instance().FlushDelayedSave(item2);
											item->SetCount(item->GetCount() - 1);
										}
										
										return true;
									}
									
									if (item2->GetSocketCount() == 0)
										return false;

									switch( item2->GetType() )
									{
										case ITEM_WEAPON:
											break;
										case ITEM_ARMOR:
											switch (item2->GetSubType())
											{
											case ARMOR_EAR:
											case ARMOR_WRIST:
											case ARMOR_NECK:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("빼낼 영석이 없습니다"));
												return false;
											}
											break;

										default:
											return false;
									}

									std::stack<long> socket;

									for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										socket.push(item2->GetSocket(i));

									int idx = ITEM_SOCKET_MAX_NUM - 1;

									while (socket.size() > 0)
									{
										if (socket.top() > 2 && socket.top() != ITEM_BROKEN_METIN_VNUM)
											break;

										idx--;
										socket.pop();
									}

									if (socket.size() == 0)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("빼낼 영석이 없습니다"));
										return false;
									}

									LPITEM pItemReward = AutoGiveItem(socket.top());

									if (pItemReward != NULL)
									{
										item2->SetSocket(idx, 1);

										char buf[256+1];
										snprintf(buf, sizeof(buf), "%s(%u) %s(%u)",
												item2->GetName(), item2->GetID(), pItemReward->GetName(), pItemReward->GetID());
										LogManager::instance().ItemLog(this, item, "USE_DETACHMENT_ONE", buf);

										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case 70201:
							case 70202:
							case 70203:
							case 70204:
							case 70205:
							case 70206:
								{
									// NEW_HAIR_STYLE_ADD
									if (GetPart(PART_HAIR) >= 1001)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("현재 헤어스타일에서는 염색과 탈색이 불가능합니다."));
									}
									// END_NEW_HAIR_STYLE_ADD
									else
									{
										quest::CQuestManager& q = quest::CQuestManager::instance();
										quest::PC* pPC = q.GetPC(GetPlayerID());

										if (pPC)
										{
											int last_dye_level = pPC->GetFlag("dyeing_hair.last_dye_level");

											if (last_dye_level == 0 ||
													last_dye_level+3 <= GetLevel() ||
													item->GetVnum() == 70201)
											{
												SetPart(PART_HAIR, item->GetVnum() - 70201);

												if (item->GetVnum() == 70201)
													pPC->SetFlag("dyeing_hair.last_dye_level", 0);
												else
													pPC->SetFlag("dyeing_hair.last_dye_level", GetLevel());

												item->SetCount(item->GetCount() - 1);
												UpdatePacket();
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 레벨이 되어야 다시 염색하실 수 있습니다."), last_dye_level+3);
											}
										}
									}
								}
								break;

							case ITEM_NEW_YEAR_GREETING_VNUM:
								{
									DWORD dwBoxVnum = ITEM_NEW_YEAR_GREETING_VNUM;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets;
									int count = 0;

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
										for (int i = 0; i < count; i++)
										{
											if (dwVnums[i] == CSpecialItemGroup::GOLD)
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), dwCounts[i]);
										}

										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case ITEM_VALENTINE_ROSE:
							case ITEM_VALENTINE_CHOCOLATE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;

									if (((item->GetVnum() == ITEM_VALENTINE_ROSE) && (SEX_MALE==GET_SEX(this))) ||
										((item->GetVnum() == ITEM_VALENTINE_CHOCOLATE) && (SEX_FEMALE==GET_SEX(this))))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성별이 맞지않아 이 아이템을 열 수 없습니다."));
										return false;
									}

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WHITEDAY_CANDY:
							case ITEM_WHITEDAY_ROSE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;

									if (((item->GetVnum() == ITEM_WHITEDAY_CANDY) && (SEX_MALE==GET_SEX(this))) ||
										((item->GetVnum() == ITEM_WHITEDAY_ROSE) && (SEX_FEMALE==GET_SEX(this))))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성별이 맞지않아 이 아이템을 열 수 없습니다."));
										return false;
									}

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case 50011:
								{
									DWORD dwBoxVnum = 50011;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;

									if (quest::CQuestManager::instance().GetEventFlag("disable_giftbox") == 1)
									{
										ChatPacket(CHAT_TYPE_INFO, "boxes are disabled for now !");
										return false;
									}

// 									int iEmptyCell = -1;
// 									if (item->IsDragonSoul())
// 									{
// 										if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
// 										{
// 											ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("You don't have enough space in your dragon soul inventory."));
// 											return false;
// 										}
// 									}
// 									else
// 									{
// #ifdef ENABLE_SPECIAL_INVENTORY
// 										if ((iEmptyCell = GetEmptyInventory(item)) == -1)
// #else
// 										if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
// #endif
// 										{
// 											ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("You don't have enough space in your inventory."));
// 											return false;
// 										}
// 									}

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
										for (int i = 0; i < count; i++)
										{
											char buf[50 + 1];
											snprintf(buf, sizeof(buf), "%u %u", dwVnums[i], dwCounts[i]);
											LogManager::instance().ItemLog(this, item, "MOONLIGHT_GET", buf);

											//ITEM_MANAGER::instance().RemoveItem(item);
											item->SetCount(item->GetCount() - 1);

											switch (dwVnums[i])
											{
											case CSpecialItemGroup::GOLD:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), dwCounts[i]);
												break;

											case CSpecialItemGroup::EXP:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 부터 신비한 빛이 나옵니다."));
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d의 경험치를 획득했습니다."), dwCounts[i]);
												break;

											case CSpecialItemGroup::MOB:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
												break;

											case CSpecialItemGroup::SLOW:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 빨간 연기를 들이마시자 움직이는 속도가 느려졌습니다!"));
												break;

											case CSpecialItemGroup::DRAIN_HP:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자가 갑자기 폭발하였습니다! 생명력이 감소했습니다."));
												break;

											case CSpecialItemGroup::POISON:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
												break;
#ifdef ENABLE_WOLFMAN_CHARACTER
											case CSpecialItemGroup::BLEEDING:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
												break;
#endif
											case CSpecialItemGroup::MOB_GROUP:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
												break;

											default:
												if (item_gets[i])
												{
													if (dwCounts[i] > 1)
														ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 %d 개 나왔습니다."), item_gets[i]->GetClientName(), dwCounts[i]);
													else
														ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 나왔습니다."), item_gets[i]->GetClientName());
												}
												break;
											}
										}
									}
									else
									{
										ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("아무것도 얻을 수 없었습니다."));
										return false;
									}
								}
								break;

							case ITEM_GIVE_STAT_RESET_COUNT_VNUM:
								{
									//PointChange(POINT_GOLD, -iCost);
									PointChange(POINT_STAT_RESET_COUNT, 1);
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50107:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#ifdef ENABLE_NEWSTUFF
									else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#endif

									EffectPacket(SE_CHINA_FIREWORK);
#ifdef ENABLE_FIREWORK_STUN

									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
#endif
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50108:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#ifdef ENABLE_NEWSTUFF
									else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#endif

									EffectPacket(SE_SPIN_TOP);
#ifdef ENABLE_FIREWORK_STUN

									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
#endif
									item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WONSO_BEAN_VNUM:
								PointChange(POINT_HP, GetMaxHP() - GetHP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_SUGAR_VNUM:
								PointChange(POINT_SP, GetMaxSP() - GetSP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_FRUIT_VNUM:
								PointChange(POINT_STAMINA, GetMaxStamina()-GetStamina());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_ELK_VNUM:
								{
									int iGold = item->GetSocket(0);
									ITEM_MANAGER::instance().RemoveItem(item);
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), iGold);
#ifdef ENABLE_GOLD_MAX_EXTENDED
									ChangeGold(iGold);
#else
									PointChange(POINT_GOLD, iGold);
#endif
								}
								break;

							case 70021:
								{
									int HealPrice = quest::CQuestManager::instance().GetEventFlag("MonarchHealGold");
									if (HealPrice == 0)
										HealPrice = 2000000;

									if (CMonarch::instance().HealMyEmpire(this, HealPrice))
									{
										char szNotice[256];
										snprintf(szNotice, sizeof(szNotice), LC_TEXT("군주의 축복으로 이지역 %s 유저는 HP,SP가 모두 채워집니다."), EMPIRE_NAME(GetEmpire()));
										SendNoticeMap(szNotice, GetMapIndex(), false);

										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주의 축복을 사용하였습니다."));
									}
								}
								break;

							case 27995:
								{
								}
								break;

							case 71092 :
								{
									if (m_pkChrTarget != NULL)
									{
										if (m_pkChrTarget->IsPolymorphed())
										{
											m_pkChrTarget->SetPolymorph(0);
											m_pkChrTarget->RemoveAffect(AFFECT_POLYMORPH);
										}
									}
									else
									{
										if (IsPolymorphed())
										{
											SetPolymorph(0);
											RemoveAffect(AFFECT_POLYMORPH);
										}
									}
								}
								break;

#if defined(__BL_67_ATTR__)
							case 71051 :
							case 173003 :
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
										return false;

									if (ITEM_COSTUME == item2->GetType())
									{
										switch (item2->GetSubType())
										{
											case COSTUME_BODY:
											case COSTUME_HAIR:
									#ifdef ENABLE_ACCE_COSTUME_SYSTEM
											case COSTUME_ACCE:
									#endif
									#ifdef ENABLE_COSTUME_WEAPON
											case COSTUME_WEAPON:
									#endif
												break;

											default:
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
												return false;
											}
										}
									}

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

#define __BL67_TARGET_VNUMS \
										case 172650: \
										case 172651: \
										case 172652: \
										case 172653: \
										case 172654: \
										case 172655: \
										case 172656: \
										case 172657: \
										case 172658: \
										case 172659: \
										case 172660: \
										case 172661: \
										case 172662: \
										case 172663: \
										case 172664: \
										case 172665: \
										case 172666: \
										case 172667: \
										case 172668: \
										case 172669: \
										case 172670: \
										case 172671: \
										case 172672: \
										case 172673: \
										case 172674: \
										case 172675: \
										case 172676: \
										case 172677: \
										case 172678: \
										case 172679: \
										case 172680: \
										case 172681: \
										case 172682: \
										case 172683: \
										case 172684: \
										case 172685: \
										case 172686: \
										case 172687: \
										case 172688: \
										case 172689: \
										case 172690: \
										case 172691: \
										case 172692: \
										case 172693: \
										case 172694: \
										case 172695: \
										case 172696: \
										case 172697: \
										case 172698: \
										case 172699: \
										case 172700: \
										case 172701: \
										case 172702: \
										case 172703: \
										case 172704: \
										case 172705: \
										case 172706: \
										case 172707: \
										case 172708: \
										case 172709: \
										case 79000: \
										case 79001: \
										case 79002: \
										case 79003: \
										case 79004: \
										case 79005: \
										case 79006: \
										case 79007: \
										case 79008: \
										case 79009: \
										case 79010: \
										case 79011: \
										case 79012: \
										case 79013: \
										case 79014: \
										case 79015: \
										case 79016: \
										case 79017: \
										case 79018: \
										case 79019: \
										case 79020: \
										case 79021: \
										case 79022: \
										case 79023: \
										case 79024: \
										case 79025: \
										case 79026: \
										case 79027: \
										case 79028: \
										case 79029: \
										case 79030: \
										case 79031: \
										case 79032: \
										case 79033: \
										case 79034: \
										case 79035: \
										case 79036: \
										case 79037: \
										case 79038: \
										case 79039: \
										case 79040: \
										case 79041: \
										case 79042: \
										case 79043: \
										case 79044: \
										case 79045: \
										case 79046: \
										case 79047: \
										case 79048: \
										case 79049: \
										case 79050: \
										case 79051: \
										case 79052: \
										case 79053: \
										case 79054: \
										case 79055: \
										case 79056: \
										case 79057: \
										case 79058: \
										case 79059: \
										case 79060: \
										case 79061: \
										case 79062: \
										case 79063: \
										case 79064: \
										case 79065: \
										case 79066: \
										case 79067: \
										case 79068: \
										case 79069: \
										case 79070: \
										case 79071: \
										case 79072: \
										case 79073: \
										case 79074: \
										case 79075: \
										case 79076: \
										case 79077: \
										case 79078: \
										case 79079: \
										case 79080: \
										case 79081: \
										case 79082: \
										case 79083: \
										case 79084: \
										case 79085: \
										case 79086: \
										case 79087: \
										case 79088: \
										case 79089:

									const bool bIs67SkinBonusTarget = IS_67_SKINBONUS_TARGET_VNUM(item2->GetVnum());

									if (item->GetVnum() == 173004)
									{
										if (!bIs67SkinBonusTarget)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("해당 아이템에는 사용할 수 없습니다."));
											return false;
										}
									}
									else
									{
										if (bIs67SkinBonusTarget)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("해당 아이템에는 사용할 수 없습니다."));
											return false;
										}
									}

#undef __BL67_TARGET_VNUMS

#ifdef ENABLE_ITEM_RARE_ATTR_LEVEL_PCT
									if (item2->AddRareAttribute2())
#else
									if (item2->AddRareAttribute())
#endif
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성공적으로 속성이 추가 되었습니다"));

										int iAddedIdx = 5 + item2->GetRareAttrCount() - 1;

										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										LogManager::instance().ItemLog(
												GetPlayerID(),
												item2->GetAttributeType(iAddedIdx),
												item2->GetAttributeValue(iAddedIdx),
												item->GetID(),
												"ADD_RARE_ATTR",
												buf,
												GetDesc()->GetHostName(),
												item->GetOriginalVnum());

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 이 아이템으로 속성을 추가할 수 없습니다"));
									}
								}
								break;
#endif

#if defined(__BL_67_ATTR__)
							case 71052 :
							case 173004 :
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (ITEM_COSTUME == item2->GetType())
									{
										switch (item2->GetSubType())
										{
											case COSTUME_BODY:
											case COSTUME_HAIR:
									#ifdef ENABLE_ACCE_COSTUME_SYSTEM
											case COSTUME_ACCE:
									#endif
									#ifdef ENABLE_COSTUME_WEAPON
											case COSTUME_WEAPON:
									#endif
												break;

											default:
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
												return false;
											}
										}
									}

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

#define __BL67_TARGET_VNUMS \
										case 172650: \
										case 172651: \
										case 172652: \
										case 172653: \
										case 172654: \
										case 172655: \
										case 172656: \
										case 172657: \
										case 172658: \
										case 172659: \
										case 172660: \
										case 172661: \
										case 172662: \
										case 172663: \
										case 172664: \
										case 172665: \
										case 172666: \
										case 172667: \
										case 172668: \
										case 172669: \
										case 172670: \
										case 172671: \
										case 172672: \
										case 172673: \
										case 172674: \
										case 172675: \
										case 172676: \
										case 172677: \
										case 172678: \
										case 172679: \
										case 172680: \
										case 172681: \
										case 172682: \
										case 172683: \
										case 172684: \
										case 172685: \
										case 172686: \
										case 172687: \
										case 172688: \
										case 172689: \
										case 172690: \
										case 172691: \
										case 172692: \
										case 172693: \
										case 172694: \
										case 172695: \
										case 172696: \
										case 172697: \
										case 172698: \
										case 172699: \
										case 172700: \
										case 172701: \
										case 172702: \
										case 172703: \
										case 172704: \
										case 172705: \
										case 172706: \
										case 172707: \
										case 172708: \
										case 172709: \
										case 79000: \
										case 79001: \
										case 79002: \
										case 79003: \
										case 79004: \
										case 79005: \
										case 79006: \
										case 79007: \
										case 79008: \
										case 79009: \
										case 79010: \
										case 79011: \
										case 79012: \
										case 79013: \
										case 79014: \
										case 79015: \
										case 79016: \
										case 79017: \
										case 79018: \
										case 79019: \
										case 79020: \
										case 79021: \
										case 79022: \
										case 79023: \
										case 79024: \
										case 79025: \
										case 79026: \
										case 79027: \
										case 79028: \
										case 79029: \
										case 79030: \
										case 79031: \
										case 79032: \
										case 79033: \
										case 79034: \
										case 79035: \
										case 79036: \
										case 79037: \
										case 79038: \
										case 79039: \
										case 79040: \
										case 79041: \
										case 79042: \
										case 79043: \
										case 79044: \
										case 79045: \
										case 79046: \
										case 79047: \
										case 79048: \
										case 79049: \
										case 79050: \
										case 79051: \
										case 79052: \
										case 79053: \
										case 79054: \
										case 79055: \
										case 79056: \
										case 79057: \
										case 79058: \
										case 79059: \
										case 79060: \
										case 79061: \
										case 79062: \
										case 79063: \
										case 79064: \
										case 79065: \
										case 79066: \
										case 79067: \
										case 79068: \
										case 79069: \
										case 79070: \
										case 79071: \
										case 79072: \
										case 79073: \
										case 79074: \
										case 79075: \
										case 79076: \
										case 79077: \
										case 79078: \
										case 79079: \
										case 79080: \
										case 79081: \
										case 79082: \
										case 79083: \
										case 79084: \
										case 79085: \
										case 79086: \
										case 79087: \
										case 79088: \
										case 79089:

									if (item->GetVnum() == 173003)
									{
										if (!IS_67_SKINBONUS_TARGET_VNUM(item2->GetVnum()))
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("해당 아이템에는 사용할 수 없습니다."));
											return false;
										}
									}
									else
									{
										if (IS_67_SKINBONUS_TARGET_VNUM(item2->GetVnum()))
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("해당 아이템에는 사용할 수 없습니다."));
											return false;
										}
									}

#undef __BL67_TARGET_VNUMS

#ifdef ENABLE_ITEM_RARE_ATTR_LEVEL_PCT
									if (item2->ChangeRareAttribute2())
#else
									if (item2->ChangeRareAttribute())
#endif
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변경 시킬 속성이 없습니다"));
									}
								}
								break;
#endif

#ifdef ENABLE_NEW_TYPE_OF_POTION
							case NEW_MOVE_SPEED_POTION:
							case NEW_ATTACK_SPEED_POTION:
							{
								EAffectTypes type = AFFECT_NONE;

								if (item->GetVnum() == NEW_MOVE_SPEED_POTION)
									type = AFFECT_MOV_SPEED;

								if (item->GetVnum() == NEW_ATTACK_SPEED_POTION)
									type = AFFECT_ATT_SPEED;

								if (AFFECT_NONE == type)
									break;

								CAffect * pAffect = FindAffect(type);

								if (NULL == pAffect)
								{
									EPointTypes bonus = POINT_NONE;
									EAffectBits flag = AFF_NONE;

									if (item->GetVnum() == NEW_MOVE_SPEED_POTION)
									{
										bonus = POINT_MOV_SPEED;
										flag = AFF_MOV_SPEED_POTION;
									}

									if (item->GetVnum() == NEW_ATTACK_SPEED_POTION)
									{
										bonus = POINT_ATT_SPEED;
										flag = AFF_ATT_SPEED_POTION;
									}

									AddAffect(type, bonus, item->GetValue(2), flag, INFINITE_AFFECT_DURATION, 0, true);

									item->Lock(true);
									item->SetSocket(0, true);
								}
								else
								{
									RemoveAffect(pAffect);
									item->Lock(false);
									item->SetSocket(0, false);
								}
							}
							break;
#endif

							case ITEM_AUTO_HP_RECOVERY_S:
							case ITEM_AUTO_HP_RECOVERY_M:
							case ITEM_AUTO_HP_RECOVERY_L:
							case ITEM_AUTO_HP_RECOVERY_X:
							case ITEM_AUTO_SP_RECOVERY_S:
							case ITEM_AUTO_SP_RECOVERY_M:
							case ITEM_AUTO_SP_RECOVERY_L:
							case ITEM_AUTO_SP_RECOVERY_X:

							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
							case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
							case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
										return false;
									}
#ifdef ENABLE_NEWSTUFF
									else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#endif

									EAffectTypes type = AFFECT_NONE;
									bool isSpecialPotion = false;

									switch (item->GetVnum())
									{
										case ITEM_AUTO_HP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_HP_RECOVERY_S:
										case ITEM_AUTO_HP_RECOVERY_M:
										case ITEM_AUTO_HP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
										case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
											type = AFFECT_AUTO_HP_RECOVERY;
											break;

										case ITEM_AUTO_SP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_SP_RECOVERY_S:
										case ITEM_AUTO_SP_RECOVERY_M:
										case ITEM_AUTO_SP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
										case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
											type = AFFECT_AUTO_SP_RECOVERY;
											break;
									}

									if (AFFECT_NONE == type)
										break;

									if (item->GetCount() > 1)
									{
#ifdef ENABLE_SPECIAL_INVENTORY
										int pos = GetEmptyInventory(item);
#else
										int pos = GetEmptyInventory(item->GetSize());
#endif
										if (-1 == pos)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지품에 빈 공간이 없습니다."));
											break;
										}

										item->SetCount( item->GetCount() - 1 );

										LPITEM item2 = ITEM_MANAGER::instance().CreateItem( item->GetVnum(), 1 );
										item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

										if (item->GetSocket(1) != 0)
										{
											item2->SetSocket(1, item->GetSocket(1));
										}

										item = item2;
									}

									CAffect* pAffect = FindAffect( type );

									if (NULL == pAffect)
									{
										EPointTypes bonus = POINT_NONE;

										if (true == isSpecialPotion)
										{
											if (type == AFFECT_AUTO_HP_RECOVERY)
											{
												bonus = POINT_MAX_HP_PCT;
											}
											else if (type == AFFECT_AUTO_SP_RECOVERY)
											{
												bonus = POINT_MAX_SP_PCT;
											}
										}

										AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

										item->Lock(true);
										item->SetSocket(0, true);

										AutoRecoveryItemProcess( type );
									}
									else
									{
										if (item->GetID() == pAffect->dwFlag)
										{
											RemoveAffect( pAffect );

											item->Lock(false);
											item->SetSocket(0, false);
										}
										else
										{
											LPITEM old = FindItemByID( pAffect->dwFlag );

											if (NULL != old)
											{
												old->Lock(false);
												old->SetSocket(0, false);
											}

											RemoveAffect( pAffect );

											EPointTypes bonus = POINT_NONE;

											if (true == isSpecialPotion)
											{
												if (type == AFFECT_AUTO_HP_RECOVERY)
												{
													bonus = POINT_MAX_HP_PCT;
												}
												else if (type == AFFECT_AUTO_SP_RECOVERY)
												{
													bonus = POINT_MAX_SP_PCT;
												}
											}

											AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

											item->Lock(true);
											item->SetSocket(0, true);

											AutoRecoveryItemProcess( type );
										}
									}
								}
								break;
						}
						break;

					case USE_CLEAR:
						{
							switch (item->GetVnum())
							{
#ifdef ENABLE_WOLFMAN_CHARACTER
								case 27124: // Bandage
									RemoveBleeding();
									break;
#endif
								case 27874: // Grilled Perch
								default:
									RemoveBadAffect();
									break;
							}
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_INVISIBILITY:
						{
							if (item->GetVnum() == 70026)
							{
								quest::CQuestManager& q = quest::CQuestManager::instance();
								quest::PC* pPC = q.GetPC(GetPlayerID());

								if (pPC != NULL)
								{
									int last_use_time = pPC->GetFlag("mirror_of_disapper.last_use_time");

									if (get_global_time() - last_use_time < 10*60)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 사용할 수 없습니다."));
										return false;
									}

									pPC->SetFlag("mirror_of_disapper.last_use_time", get_global_time());
								}
							}

							AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, 300, 0, true);
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_POTION_NODELAY:
						{
							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
									return false;
								}

								switch (item->GetVnum())
								{
									case 70020 :
									case 71018 :
									case 71019 :
									case 71020 :
										if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
										{
											if (m_nPotionLimit <= 0)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용 제한량을 초과하였습니다."));
												return false;
											}
										}
										break;

									default :
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
										return false;
								}
							}
#ifdef ENABLE_NEWSTUFF
							else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
								return false;
							}
#endif

							bool used = false;

							if (item->GetValue(0) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
								{
									if (test_server)
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("월병 또는 종자 를 사용하였습니다"));
									SetUseSeedOrMoonBottleTime();
								}
								if (GetDungeon())
									GetDungeon()->UsePotion(this);

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								m_nPotionLimit--;

								//RESTRICT_USE_SEED_OR_MOONBOTTLE
								item->SetCount(item->GetCount() - 1);
								//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
							}
						}
						break; 

					case USE_POTION:
						if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
						{
							if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
								return false;
							}

							switch (item->GetVnum())
							{
								case 27001 :
								case 27002 :
								case 27003 :
								case 27004 :
								case 27005 :
								case 27006 :
									if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
									{
										if (m_nPotionLimit <= 0)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용 제한량을 초과하였습니다."));
											return false;
										}
									}
									break;

								default :
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
									return false;
							}
						}
#ifdef ENABLE_NEWSTUFF
						else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
							return false;
						}
#endif

						if (item->GetValue(1) != 0)
						{
							if (GetPoint(POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
							{
								return false;
							}

							PointChange(POINT_SP_RECOVERY, item->GetValue(1) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_SPUP_BLUE);
						}

						if (item->GetValue(0) != 0)
						{
							if (GetPoint(POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
							{
								return false;
							}

							PointChange(POINT_HP_RECOVERY, item->GetValue(0) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_HPUP_RED);
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						m_nPotionLimit--;
						break;

					case USE_POTION_CONTINUE:
						{
							if (item->GetValue(0) != 0)
							{
								AddAffect(AFFECT_HP_RECOVER_CONTINUE, POINT_HP_RECOVER_CONTINUE, item->GetValue(0), 0, item->GetValue(2), 0, true);
							}
							else if (item->GetValue(1) != 0)
							{
								AddAffect(AFFECT_SP_RECOVER_CONTINUE, POINT_SP_RECOVER_CONTINUE, item->GetValue(1), 0, item->GetValue(2), 0, true);
							}
							else
								return false;
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_ABILITY_UP:
						{
							if (item->IsFishPotion())
							{
								int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
								int apply_value = item->GetValue(2);
								int apply_duration = item->GetValue(1);

								if (FindAffect(AFFECT_FISH_POTION, apply_type))
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
									return false;
								}
								AddAffect(AFFECT_FISH_POTION, apply_type, apply_value, 0, apply_duration, 0, false);
								if (GetDungeon())
								{
									GetDungeon()->UsePotion(this);
								}

								if (GetWarMap())
								{
									GetWarMap()->UsePotion(this, item);
								}

								item->SetCount(item->GetCount() - 1);
								return false;
							}

							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									AddAffect(AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
									EffectPacket(SE_DXUP_PURPLE);
#endif
									break;

								case APPLY_ATT_SPEED:
									AddAffect(AFFECT_ATT_SPEED, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
									EffectPacket(SE_SPEEDUP_GREEN);
#endif
									break;

								case APPLY_STR:
									AddAffect(AFFECT_STR, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_DEX, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_CON, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_INT, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_CAST_SPEED, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_ATT_GRADE, POINT_ATT_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_DEF_GRADE, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;
							}
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_TALISMAN:
						{
							const int TOWN_PORTAL	= 1;
							const int MEMORY_PORTAL = 2;

							if (GetMapIndex() == 200 || GetMapIndex() == 113)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("현재 위치에서 사용할 수 없습니다."));
								return false;
							}

							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
								return false;
							}
#ifdef ENABLE_NEWSTUFF
							else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
								return false;
							}
#endif

							if (m_pkWarpEvent)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이동할 준비가 되어있음으로 귀환부를 사용할수 없습니다"));
								return false;
							}

							// CONSUME_LIFE_WHEN_USE_WARP_ITEM
							int consumeLife = CalculateConsume(this);

							if (consumeLife < 0)
								return false;
							// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

							if (item->GetValue(0) == TOWN_PORTAL)
							{
								if (item->GetSocket(0) == 0)
								{
									if (!GetDungeon())
										if (!GiveRecallItem(item))
											return false;

									PIXEL_POSITION posWarp;

									if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp))
									{
										// CONSUME_LIFE_WHEN_USE_WARP_ITEM
										PointChange(POINT_HP, -consumeLife, false);
										// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

										WarpSet(posWarp.x, posWarp.y);
									}
									else
									{
										sys_err("CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)", GetName(), GetX(), GetY());
									}
								}
								else
								{
									if (test_server)
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("원래 위치로 복귀"));

									ProcessRecallItem(item);
								}
							}
							else if (item->GetValue(0) == MEMORY_PORTAL)
							{
								if (item->GetSocket(0) == 0)
								{
									if (GetDungeon())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("던전 안에서는 %s%s 사용할 수 없습니다."),
												item->GetClientName(),
												"");
										return false;
									}

									if (!GiveRecallItem(item))
										return false;
								}
								else
								{
									// CONSUME_LIFE_WHEN_USE_WARP_ITEM
									PointChange(POINT_HP, -consumeLife, false);
									// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

									ProcessRecallItem(item);
								}
							}
						}
						break;

					case USE_TUNING:
					case USE_DETACHMENT:
						{
							LPITEM item2;

							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("+3 영석은 이 아이템으로 개량할 수 없습니다"));
								return false;
							}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
							if (item->GetValue(0) == ACCE_CLEAN_ATTR_VALUE0
								|| item->GetVnum() == ACCE_REVERSAL_VNUM_1
								|| item->GetVnum() == ACCE_REVERSAL_VNUM_2
							)
							{
								if (!CleanAcceAttr(item, item2))
									return false;
								item->SetCount(item->GetCount()-1);
								return true;
							}
#endif

							if (item2->GetVnum() >= 28430 && item2->GetVnum() <= 28443)
							{
								if (item->GetVnum() == 71056)
								{
									RefineItem(item, item2);
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("영석은 이 아이템으로 개량할 수 없습니다"));
								}
							}
							else
							{
								RefineItem(item, item2);
							}
						}
						break;

					case USE_CHANGE_COSTUME_ATTR:
					case USE_RESET_COSTUME_ATTR:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}

							if (ITEM_COSTUME != item2->GetType())
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
								return false;
							}

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							if (item2->GetAttributeSetIndex() == -1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
								return false;
							}

							if (item2->GetAttributeCount() == 0)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변경할 속성이 없습니다."));
								return false;
							}

							switch (item->GetSubType())
							{
								case USE_CHANGE_COSTUME_ATTR:
									item2->ChangeAttribute();
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_COSTUME_ATTR", buf);
									}
									break;
								case USE_RESET_COSTUME_ATTR:
									item2->ClearAttribute();
									item2->AlterToMagicItem();
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "RESET_COSTUME_ATTR", buf);
									}
									break;
							}

							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경하였습니다."));

							item->SetCount(item->GetCount() - 1);
							break;
						}

						//  ACCESSORY_REFINE & ADD/CHANGE_ATTRIBUTES
					case USE_PUT_INTO_BELT_SOCKET:
					case USE_PUT_INTO_RING_SOCKET:
					case USE_PUT_INTO_ACCESSORY_SOCKET:
					case USE_ADD_ACCESSORY_SOCKET:
					case USE_CLEAN_SOCKET:
					case USE_CHANGE_ATTRIBUTE:
					case USE_CHANGE_ATTRIBUTE2:
#ifdef ENABLE_PENDANT_SWITCHER
					case USE_CHANGE_PENDANT_ATTRIBUTE:
#endif
					case USE_ADD_ATTRIBUTE:
					case USE_ADD_ATTRIBUTE2:
#ifdef ENABLE_PENDANT_SWITCHER
					case USE_ADD_PENDANT_ATTRIBUTE:
					case USE_ADD_PENDANT_FIVE_ATTRIBUTE:
#endif
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}

#if defined(__BL_67_ATTR__)
							if (ITEM_COSTUME == item2->GetType()
								&& item->GetSubType() != USE_CHANGE_ATTRIBUTE2
								&& item->GetSubType() != USE_ADD_ATTRIBUTE2)
#else
							if (ITEM_COSTUME == item2->GetType())
#endif
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
								return false;
							}

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							switch (item->GetSubType())
							{
								case USE_CLEAN_SOCKET:
									{
										int i;
										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										{
											if (item2->GetSocket(i) == ITEM_BROKEN_METIN_VNUM)
												break;
										}

										if (i == ITEM_SOCKET_MAX_NUM)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("청소할 석이 박혀있지 않습니다."));
											return false;
										}

										int j = 0;

										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										{
											if (item2->GetSocket(i) != ITEM_BROKEN_METIN_VNUM && item2->GetSocket(i) != 0)
												item2->SetSocket(j++, item2->GetSocket(i));
										}

										for (; j < ITEM_SOCKET_MAX_NUM; ++j)
										{
											if (item2->GetSocket(j) > 0)
												item2->SetSocket(j, 1);
										}

										{
											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());
											LogManager::instance().ItemLog(this, item, "CLEAN_SOCKET", buf);
										}

										item->SetCount(item->GetCount() - 1);

									}
									break;

								case USE_CHANGE_ATTRIBUTE:
								{
									if (item->GetVnum() == 96276)
									{
										if ((item2->GetType() == ITEM_WEAPON) && item2->HasAttr(72))
										{
											int16_t idx = item2->FindAttribute(72);
											int32_t currentValue = item2->GetAttributeValue(idx);
											if (currentValue < 75)
											{
												int32_t newValue = currentValue + 1;
												item2->SetForceAttribute(idx, 72, newValue);
												ChatPacket(CHAT_TYPE_INFO, "Average damage increased by +1.");
												item->SetCount(item->GetCount() - 1);
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, "Average damage is already at maximum.");
												return false;
											}
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, "You can only use this item on average damage weapons.");
											return false;
										}
									}
#ifdef ENABLE_SKINBONUS
									else if (item->GetVnum() == 173002)
									{
										if (!item2)
										{
											ChatPacket(CHAT_TYPE_INFO, "This does not work on this item. Use a normal switche2r.");
											return false;
										}

										const DWORD dwTargetVnum = item2->GetVnum();
										bool bAllowed = false;

										static const DWORD s_adwAllowedVnum[] =
										{
											172650, 172651, 172652, 172653, 172654, 172655, 172656, 172657, 172658, 172659,
											172660, 172661, 172662, 172663, 172664, 172665, 172665, 172666, 172667, 172668,
											172669, 172670, 172671, 172672, 172673, 172674, 172675, 172676, 172677, 172678,
											172679, 172680, 172681, 172682, 172683, 172684, 172685, 172686, 172687, 172688,
											172689, 172690, 172691, 172692, 172693, 172694, 172695, 172696, 172697, 172698,
											172699, 172700, 172701, 172702, 172703, 172704, 172705, 172706, 172707, 172708,
											172709,

											79000, 79001, 79002, 79003, 79004, 79005, 79006, 79007, 79008, 79009,
											79010, 79011, 79012, 79013, 79014, 79015, 79016, 79017, 79018, 79019,
											79020, 79021, 79022, 79023, 79024, 79025, 79026, 79027, 79028, 79029,
											79030, 79031, 79032, 79033, 79034, 79035, 79036, 79037, 79038, 79039,
											79040, 79041, 79042, 79043, 79044, 79045, 79046, 79047, 79048, 79049,
											79050, 79051, 79052, 79053, 79054, 79055, 79056, 79057, 79058, 79059,
											79060, 79061, 79062, 79063, 79064, 79065, 79066, 79067, 79068, 79069,
											79070, 79071, 79072, 79073, 79074, 79075, 79076, 79077, 79078, 79079,
											79080, 79081, 79082, 79083, 79084, 79085, 79086, 79087, 79088, 79089
										};

										const int kAllowedCount = sizeof(s_adwAllowedVnum) / sizeof(s_adwAllowedVnum[0]);
										for (int i = 0; i < kAllowedCount; ++i)
										{
											if (s_adwAllowedVnum[i] == dwTargetVnum)
											{
												bAllowed = true;
												break;
											}
										}

										if (!bAllowed)
										{
											ChatPacket(CHAT_TYPE_INFO, "This does not work on this item. Use a normal switcher.");
											return false;
										}
/*#ifdef switchen_with_yang
										if (GetGold() < 30000)
										{
											ChatPacket(CHAT_TYPE_INFO, "Du besitzt nicht genug Yang.");
											return true;
										}
											PointChange(POINT_GOLD, -30000, true);
#endif*/
										item2->ChangeAttribute();
										ChatPacket(CHAT_TYPE_INFO, "Attribute change successful.");

										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										if (item2)
										{
											const DWORD dwTargetVnum = item2->GetVnum();
											bool bIsBlocked = false;

											static const DWORD s_adwBlockedVnum[] =
											{
											172650, 172651, 172652, 172653, 172654, 172655, 172656, 172657, 172658, 172659,
											172660, 172661, 172662, 172663, 172664, 172665, 172665, 172666, 172667, 172668,
											172669, 172670, 172671, 172672, 172673, 172674, 172675, 172676, 172677, 172678,
											172679, 172680, 172681, 172682, 172683, 172684, 172685, 172686, 172687, 172688,
											172689, 172690, 172691, 172692, 172693, 172694, 172695, 172696, 172697, 172698,
											172699, 172700, 172701, 172702, 172703, 172704, 172705, 172706, 172707, 172708,
											172709,

											79000, 79001, 79002, 79003, 79004, 79005, 79006, 79007, 79008, 79009,
											79010, 79011, 79012, 79013, 79014, 79015, 79016, 79017, 79018, 79019,
											79020, 79021, 79022, 79023, 79024, 79025, 79026, 79027, 79028, 79029,
											79030, 79031, 79032, 79033, 79034, 79035, 79036, 79037, 79038, 79039,
											79040, 79041, 79042, 79043, 79044, 79045, 79046, 79047, 79048, 79049,
											79050, 79051, 79052, 79053, 79054, 79055, 79056, 79057, 79058, 79059,
											79060, 79061, 79062, 79063, 79064, 79065, 79066, 79067, 79068, 79069,
											79070, 79071, 79072, 79073, 79074, 79075, 79076, 79077, 79078, 79079,
											79080, 79081, 79082, 79083, 79084, 79085, 79086, 79087, 79088, 79089
											};

											const int kBlockedCount = sizeof(s_adwBlockedVnum) / sizeof(s_adwBlockedVnum[0]);
											for (int i = 0; i < kBlockedCount; ++i)
											{
												if (s_adwBlockedVnum[i] == dwTargetVnum)
												{
													bIsBlocked = true;
													break;
												}
											}

											if (bIsBlocked)
											{
												ChatPacket(CHAT_TYPE_INFO, "This does not work on this item. Use the K switcher.");
												return false;
											}
										}

										item2->ChangeAttribute();
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경하였습니다."));

										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);

										item->SetCount(item->GetCount() - 1);
									}
#else
									else
									{
										item2->ChangeAttribute();
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경하였습니다."));

										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);

										item->SetCount(item->GetCount() - 1);
									}
#endif
									break;
								}

#if defined(__BL_67_ATTR__)
								case USE_CHANGE_ATTRIBUTE2 : // @fixme123
#endif
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

#ifdef ENABLE_PENDANT_SWITCHER
									if (item2->GetSubType() == ARMOR_PENDANT)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANT_USE_THIS_USE_PEDANT_SWITCHER"));
										return false;
									}
#endif

#if defined(__BL_67_ATTR__)
									if (item->GetVnum() == 71052 || item->GetVnum() == 173004)
									{
										if (item2->GetRareAttrCount() == 0)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변경 시킬 속성이 없습니다"));
											return false;
										}
									}
									else
#endif
									{
										if (item2->GetAttributeCount() == 0)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변경할 속성이 없습니다."));
											return false;
										}
									}

									if ((GM_PLAYER == GetGMLevel()) && (false == test_server) && (g_dwItemBonusChangeTime > 0))
									{
										DWORD dwChangeItemAttrCycle = g_dwItemBonusChangeTime;

										quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());

										if (pPC)
										{
											DWORD dwNowSec = get_global_time();

											DWORD dwLastChangeItemAttrSec = pPC->GetFlag(msc_szLastChangeItemAttrFlag);

											if (dwLastChangeItemAttrSec + dwChangeItemAttrCycle > dwNowSec)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 바꾼지 %d분 이내에는 다시 변경할 수 없습니다.(%d 분 남음)"),
														dwChangeItemAttrCycle, dwChangeItemAttrCycle - (dwNowSec - dwLastChangeItemAttrSec));
												return false;
											}

											pPC->SetFlag(msc_szLastChangeItemAttrFlag, dwNowSec);
										}
									}

#if defined(__BL_67_ATTR__)
									if (item->GetVnum() == 71052 || item->GetVnum() == 173004)
									{
#ifdef ENABLE_SKINBONUS
										const bool bIs67SkinBonusTarget = IS_67_SKINBONUS_TARGET_VNUM(item2->GetVnum());

										if (item->GetVnum() == 173004)
										{
											if (!bIs67SkinBonusTarget)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("해당 아이템에는 사용할 수 없습니다."));
												return false;
											}
										}
										else
										{
											if (bIs67SkinBonusTarget)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("해당 아이템에는 사용할 수 없습니다."));
												return false;
											}
										}
#endif
										if (item2->IsExchanging() || item2->IsEquipped())
											return false;

										// if (item2->GetAttributeCount() < 5)
										// {
											// ChatPacket(CHAT_TYPE_INFO, LC_TEXT("먼저 재가비서를 이용하여 속성을 5개까지 추가시켜 주세요."));
											// return false;
										// }

										if (item2->GetRareAttrCount() == 0)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변경 시킬 속성이 없습니다"));
											return false;
										}

										if (item2->ChangeRareAttribute2())
										{
											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());
											LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);

											item->SetCount(item->GetCount() - 1);
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경하였습니다."));
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변경 시킬 속성이 없습니다"));
										}
										break;
									}
#endif

									if (item->GetSubType() == USE_CHANGE_ATTRIBUTE2)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
										{
											0, 0, 30, 40, 3
										};

										item2->ChangeAttribute(aiChangeProb);
#ifdef __RANKING_SYSTEM__
										RankPlayer::instance().SendInfoPlayer(this, RANK_BY_BONUS_CHANGED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_BONUS_CHANGED) + 1, false);
#endif
									}
									else if (item->GetVnum() == 76014)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
										{
											0, 10, 50, 39, 1
										};

										item2->ChangeAttribute(aiChangeProb);
#ifdef __RANKING_SYSTEM__
										RankPlayer::instance().SendInfoPlayer(this, RANK_BY_BONUS_CHANGED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_BONUS_CHANGED) + 1, false);
#endif
									}
									else
									{
										if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
										{
#ifndef ENABLE_GREEN_MAGIC_ON_ALL_EQ
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
											{
#endif
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("적용 레벨보다 높아 사용이 불가능합니다."));
													break;
												}
#ifndef ENABLE_GREEN_MAGIC_ON_ALL_EQ
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("무기와 갑옷에만 사용 가능합니다."));
												break;
											}
#endif
										}
										item2->ChangeAttribute();
#ifdef ENABLE_RANK_PLAYER
										RankPlayer::instance().SendInfoPlayer(this, RANK_BY_BONUS_CHANGED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_BONUS_CHANGED) + 1, false);
#endif
									}

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경하였습니다."));
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);
									}

									item->SetCount(item->GetCount() - 1);
#ifdef __RANKING_SYSTEM__
									RankPlayer::instance().SendInfoPlayer(this, RANK_BY_BONUS_CHANGED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_BONUS_CHANGED) + 1, false);
#endif
									break;

								case USE_ADD_ATTRIBUTE :
								{
#ifdef ENABLE_SKINBONUS
									{
										static const DWORD s_adwKAllowedVnum[] =
										{
											172650,
											172651,
											172652,
											172653,
											172654,
											172655,
											172656,
											172657,
											172658,
											172659,
											172660,
											172661,
											172662,
											172663,
											172664,
											172665,
											172665,
											172666,
											172667,
											172668,
											172669,
											172670,
											172671,
											172672,
											172673,
											172674,
											172675,
											172676,
											172677,
											172678,
											172679,
											172680,
											172681,
											172682,
											172683,
											172684,
											172685,
											172686,
											172687,
											172688,
											172689,
											172690,
											172691,
											172692,
											172693,
											172694,
											172695,
											172696,
											172697,
											172698,
											172699,
											172700,
											172701,
											172702,
											172703,
											172704,
											172705,
											172706,
											172707,
											172708,
											172709,

											79000,
											79001,
											79002,
											79003,
											79004,
											79005,
											79006,
											79007,
											79008,
											79009,
											79010,
											79011,
											79012,
											79013,
											79014,
											79015,
											79016,
											79017,
											79018,
											79019,
											79020,
											79021,
											79022,
											79023,
											79024,
											79025,
											79026,
											79027,
											79028,
											79029,
											79030,
											79031,
											79032,
											79033,
											79034,
											79035,
											79036,
											79037,
											79038,
											79039,
											79040,
											79041,
											79042,
											79043,
											79044,
											79045,
											79046,
											79047,
											79048,
											79049,
											79050,
											79051,
											79052,
											79053,
											79054,
											79055,
											79056,
											79057,
											79058,
											79059,
											79060,
											79061,
											79062,
											79063,
											79064,
											79065,
											79066,
											79067,
											79068,
											79069,
											79070,
											79071,
											79072,
											79073,
											79074,
											79075,
											79076,
											79077,
											79078,
											79079,
											79080,
											79081,
											79082,
											79083,
											79084,
											79085,
											79086,
											79087,
											79088,
											79089
										};

										const DWORD dwTargetVnum = item2->GetVnum();
										bool bIsKTarget = false;

										for (int i = 0; i < (int)(sizeof(s_adwKAllowedVnum) / sizeof(s_adwKAllowedVnum[0])); ++i)
										{
											if (s_adwKAllowedVnum[i] == dwTargetVnum)
											{
												bIsKTarget = true;
												break;
											}
										}

										if (item->GetVnum() == 173001)
										{
											if (!bIsKTarget)
											{
												ChatPacket(CHAT_TYPE_INFO, "This does not work on this item. Use a normal attribute adder.");
												return false;
											}

											if (item2->GetAttributeSetIndex() == -1)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
												return false;
											}

#ifdef ENABLE_PENDANT_SWITCHER
											if (item2->GetSubType() == ARMOR_PENDANT)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANT_USE_THIS_USE_PEDANT_ADDER"));
												return false;
											}
#endif

											if (item2->GetAttributeCount() >= 5)
											{
												ChatPacket(CHAT_TYPE_INFO, "You cannot add more attributes to this item.");
												return false;
											}

											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());

											item2->AddAttribute();
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성 추가에 성공하였습니다."));

											int iAddedIdx = item2->GetAttributeCount() - 1;
											LogManager::instance().ItemLog(
													GetPlayerID(),
													item2->GetAttributeType(iAddedIdx),
													item2->GetAttributeValue(iAddedIdx),
													item->GetID(),
													"ADD_ATTRIBUTE_SUCCESS",
													buf,
													GetDesc()->GetHostName(),
													item->GetOriginalVnum());

											item->SetCount(item->GetCount() - 1);
											break;
										}
										else
										{
											if (bIsKTarget)
											{
												ChatPacket(CHAT_TYPE_INFO, "This does not work on this item. Use the K attribute adder.");
												return false;
											}
										}
									}
#endif
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

#ifdef ENABLE_PENDANT_SWITCHER
									if (item2->GetSubType() == ARMOR_PENDANT)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANT_USE_THIS_USE_PEDANT_ADDER"));
										return false;
									}
#endif

									if (item2->GetAttributeCount() < 4)
									{
										if (item->GetVnum() == 71152 || item->GetVnum() == 76024)
										{
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
											{
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("적용 레벨보다 높아 사용이 불가능합니다."));
													break;
												}
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("무기와 갑옷에만 사용 가능합니다."));
												break;
											}
										}

										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

#ifdef ENABLE_INSTANT_5_BONUS
										for (int i = 0; i < MAX_NORM_ATTR_NUM; i++)
											item2->AddAttribute();
#else
										if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
										{
											item2->AddAttribute();
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성 추가에 성공하였습니다."));

											int iAddedIdx = item2->GetAttributeCount() - 1;
											LogManager::instance().ItemLog(
													GetPlayerID(),
													item2->GetAttributeType(iAddedIdx),
													item2->GetAttributeValue(iAddedIdx),
													item->GetID(),
													"ADD_ATTRIBUTE_SUCCESS",
													buf,
													GetDesc()->GetHostName(),
													item->GetOriginalVnum());
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Failed."));
											LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE_FAIL", buf);
										}
#endif

#ifndef ENABLE_UNLIMITED_USE_ADD_ATTRIBUTE
										item->SetCount(item->GetCount() - 1);
#endif
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더이상 이 아이템을 이용하여 속성을 추가할 수 없습니다."));
									}
									break;
								}

								case USE_ADD_ATTRIBUTE2 :
								{
#ifdef ENABLE_SKINBONUS
									{
										static const DWORD s_adwKAllowedVnum[] =
										{
											172650,
											172651,
											172652,
											172653,
											172654,
											172655,
											172656,
											172657,
											172658,
											172659,
											172660,
											172661,
											172662,
											172663,
											172664,
											172665,
											172665,
											172666,
											172667,
											172668,
											172669,
											172670,
											172671,
											172672,
											172673,
											172674,
											172675,
											172676,
											172677,
											172678,
											172679,
											172680,
											172681,
											172682,
											172683,
											172684,
											172685,
											172686,
											172687,
											172688,
											172689,
											172690,
											172691,
											172692,
											172693,
											172694,
											172695,
											172696,
											172697,
											172698,
											172699,
											172700,
											172701,
											172702,
											172703,
											172704,
											172705,
											172706,
											172707,
											172708,
											172709,

											79000,
											79001,
											79002,
											79003,
											79004,
											79005,
											79006,
											79007,
											79008,
											79009,
											79010,
											79011,
											79012,
											79013,
											79014,
											79015,
											79016,
											79017,
											79018,
											79019,
											79020,
											79021,
											79022,
											79023,
											79024,
											79025,
											79026,
											79027,
											79028,
											79029,
											79030,
											79031,
											79032,
											79033,
											79034,
											79035,
											79036,
											79037,
											79038,
											79039,
											79040,
											79041,
											79042,
											79043,
											79044,
											79045,
											79046,
											79047,
											79048,
											79049,
											79050,
											79051,
											79052,
											79053,
											79054,
											79055,
											79056,
											79057,
											79058,
											79059,
											79060,
											79061,
											79062,
											79063,
											79064,
											79065,
											79066,
											79067,
											79068,
											79069,
											79070,
											79071,
											79072,
											79073,
											79074,
											79075,
											79076,
											79077,
											79078,
											79079,
											79080,
											79081,
											79082,
											79083,
											79084,
											79085,
											79086,
											79087,
											79088,
											79089
										};

										const DWORD dwTargetVnum = item2->GetVnum();
										bool bIsKTarget = false;

										for (int i = 0; i < (int)(sizeof(s_adwKAllowedVnum) / sizeof(s_adwKAllowedVnum[0])); ++i)
										{
											if (s_adwKAllowedVnum[i] == dwTargetVnum)
											{
												bIsKTarget = true;
												break;
											}
										}

										if (item->GetVnum() == 173003)
										{
											if (!bIsKTarget)
											{
												ChatPacket(CHAT_TYPE_INFO, "This does not work on this item. Use a normal attribute adder.");
												return false;
											}
										}
										else
										{
											if (bIsKTarget)
											{
												ChatPacket(CHAT_TYPE_INFO, "This does not work on this item. Use the K attribute adder.");
												return false;
											}
										}
									}
#endif
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

#ifdef ENABLE_PENDANT_SWITCHER
									if (item2->GetSubType() == ARMOR_PENDANT)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANT_USE_THIS_USE_PEDANT_ADDER"));
										return false;
									}
#endif

									// if (item2->GetAttributeCount() < 5)
									// {
										// ChatPacket(CHAT_TYPE_INFO, LC_TEXT("먼저 재가비서를 이용하여 속성을 5개까지 추가시켜 주세요."));
										// return false;
									// }

									if (item2->AddRareAttribute2())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성공적으로 속성이 추가 되었습니다"));

										int iAddedIdx = 5 + item2->GetRareAttrCount() - 1;

										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										LogManager::instance().ItemLog(
												GetPlayerID(),
												item2->GetAttributeType(iAddedIdx),
												item2->GetAttributeValue(iAddedIdx),
												item->GetID(),
												"ADD_RARE_ATTR",
												buf,
												GetDesc()->GetHostName(),
												item->GetOriginalVnum());

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 이 아이템으로 속성을 추가할 수 없습니다"));
									}
									break;
								}

#ifdef ENABLE_PENDANT_SWITCHER
								case USE_ADD_PENDANT_ATTRIBUTE:
								{
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

									if (item2->GetSubType() == ARMOR_PENDANT)
									{
										if (item2->GetAttributeCount() < 4)
										{
											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());

											if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
											{
												item2->AddAttribute();
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성 추가에 성공하였습니다."));
												int iAddedIdx = item2->GetAttributeCount() - 1;
												LogManager::instance().ItemLog(
														GetPlayerID(),
														item2->GetAttributeType(iAddedIdx),
														item2->GetAttributeValue(iAddedIdx),
														item->GetID(),
														"ADD_ATTRIBUTE_SUCCESS",
														buf,
														GetDesc()->GetHostName(),
														item->GetOriginalVnum());
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성 추가에 실패하였습니다."));
												LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE_FAIL", buf);
											}

											item->SetCount(item->GetCount() - 1);
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더이상 이 아이템을 이용하여 속성을 추가할 수 없습니다."));
										}
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Item is not a pendant."));
									}
								}
								break;

								case USE_ADD_PENDANT_FIVE_ATTRIBUTE:
								{
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}
									if (item2->GetSubType() == ARMOR_PENDANT)
									{
										if (item2->GetAttributeCount() == 4)
										{
											item2->AddAttribute();
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성 추가에 성공하였습니다."));

											int iAddedIdx = item2->GetAttributeCount() - 1;

											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());
											LogManager::instance().ItemLog(
													GetPlayerID(),
													item2->GetAttributeType(iAddedIdx),
													item2->GetAttributeValue(iAddedIdx),
													item->GetID(),
													"ADD_ATTRIBUTE2_SUCCESS",
													buf,
													GetDesc()->GetHostName(),
													item->GetOriginalVnum());

											item->SetCount(item->GetCount() - 1);
										}
										else if (item2->GetAttributeCount() == 5)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 이 아이템을 이용하여 속성을 추가할 수 없습니다."));
										}
										else if (item2->GetAttributeCount() < 4)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("먼저 재가비서를 이용하여 속성을 추가시켜 주세요."));
										}
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Item is not a pendant."));
									}
								}
								break;
#endif

								case USE_ADD_ACCESSORY_SOCKET:
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (item2->IsAccessoryForSocket())
										{
											if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
#ifdef ENABLE_ADDSTONE_FAILURE
												if (number(1, 100) <= 50)
#else
												if (1)
#endif
												{
													item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소켓이 성공적으로 추가되었습니다."));
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_SUCCESS", buf);
												}
												else
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소켓 추가에 실패하였습니다."));
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_FAIL", buf);
												}

												item->SetCount(item->GetCount() - 1);
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 액세서리에는 더이상 소켓을 추가할 공간이 없습니다."));
											}
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템으로 소켓을 추가할 수 없는 아이템입니다."));
										}
									}
									break;

								case USE_PUT_INTO_BELT_SOCKET:
								case USE_PUT_INTO_ACCESSORY_SOCKET:
									if (item2->IsAccessoryForSocket() && item->CanPutInto(item2))
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
										{
											if (number(1, 100) <= aiAccessorySocketPutPct[item2->GetAccessorySocketGrade()])
											{
												item2->SetAccessorySocketGrade(item2->GetAccessorySocketGrade() + 1);
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("장착에 성공하였습니다."));
												LogManager::instance().ItemLog(this, item, "PUT_SOCKET_SUCCESS", buf);
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("장착에 실패하였습니다."));
												LogManager::instance().ItemLog(this, item, "PUT_SOCKET_FAIL", buf);
											}

											item->SetCount(item->GetCount() - 1);
										}
										else
										{
											if (item2->GetAccessorySocketMaxGrade() == 0)
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("먼저 다이아몬드로 악세서리에 소켓을 추가해야합니다."));
											else if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 액세서리에는 더이상 장착할 소켓이 없습니다."));
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다이아몬드로 소켓을 추가해야합니다."));
											}
											else
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 액세서리에는 더이상 보석을 장착할 수 없습니다."));
										}
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템을 장착할 수 없습니다."));
									}
									break;
							}
							if (item2->IsEquipped())
							{
								BuffOnAttr_AddBuffsFromItem(item2);
							}
						}
						break;
						//  END_OF_ACCESSORY_REFINE & END_OF_ADD_ATTRIBUTES & END_OF_CHANGE_ATTRIBUTES

					case USE_BAIT:
						{
							if (m_pkFishingEvent)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("낚시 중에 미끼를 갈아끼울 수 없습니다."));
								return false;
							}

							LPITEM weapon = GetWear(WEAR_WEAPON);

							if (!weapon || weapon->GetType() != ITEM_ROD)
								return false;

							if (weapon->GetSocket(2))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 꽂혀있던 미끼를 빼고 %s를 끼웁니다."), item->GetClientName());
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("낚시대에 %s를 미끼로 끼웁니다."), item->GetClientName());
							}

							weapon->SetSocket(2, item->GetValue(0));
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_CHANGE_ATTRIBUTE_DS:
					{
						LPITEM pDestItem = GetItem(DestCell);
						if (pDestItem)
						{
							if (item->GetSubType() == USE_CHANGE_ATTRIBUTE_DS)
							{
								if (pDestItem->IsDragonSoul())
								{
									if (pDestItem->IsDragonSoul() && pDestItem->IsEquipped())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot modify equipped items."));
										return false;
									}
									else
									{
										DSManager::instance().ChangeAttributeDragonSoul(pDestItem);	
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success."));
										item->SetCount(item->GetCount() - 1);
									}
								}
								else
								{
									return false;
								}
							}
						}
					}
					break;

					case USE_MOVE:
					case USE_TREASURE_BOX:
					case USE_MONEYBAG:
						break;

					case USE_AFFECT :
						{
#ifdef __PREMIUM_PRIVATE_SHOP__
							if (item->GetValue(0) == AFFECT_PREMIUM_PRIVATE_SHOP)
							{
								if (SetPremiumPrivateShopBonus(item->GetValue(3)))
									item->SetCount(item->GetCount() - 1);
								return true;
							}
#endif
							if (FindAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
							}
							else
							{
								AddAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_CREATE_STONE:
						AutoGiveItem(number(28000, 28013));
						item->SetCount(item->GetCount() - 1);
						break;
					
					case USE_TITLE:
					{
						if (item->GetSocket(1) != 0)
						{
							// item->Lock(false);
							item->SetSocket(1, 0);
							SetProtectTime("title_index", 0);
						}
						else
						{
							// deactive all titles
							for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
							{
								LPITEM item3 = GetInventoryItem(i);
								if (item3 && item3->GetSocket(1) != 0 && item3->GetType() == ITEM_USE && item3->GetSubType() == USE_TITLE)
								{
									// item3->Lock(false);
									item3->SetSocket(1, 0);
								}
							}
							
							// ACTIVE Title
							// item->Lock(true);
							item->SetSocket(1, 1);
							
							SetProtectTime("title_index", item->GetValue(0));
						}
						
						ComputePoints();
					}
					break;
				
					case USE_RECIPE :
						{
							LPITEM pSource1 = FindSpecifyItem(item->GetValue(1));
							DWORD dwSourceCount1 = item->GetValue(2);

							LPITEM pSource2 = FindSpecifyItem(item->GetValue(3));
							DWORD dwSourceCount2 = item->GetValue(4);

							if (dwSourceCount1 != 0)
							{
								if (pSource1 == NULL)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("물약 조합을 위한 재료가 부족합니다."));
									return false;
								}
							}

							if (dwSourceCount2 != 0)
							{
								if (pSource2 == NULL)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("물약 조합을 위한 재료가 부족합니다."));
									return false;
								}
							}

							if (pSource1 != NULL)
							{
								if (pSource1->GetCount() < dwSourceCount1)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("재료(%s)가 부족합니다."), pSource1->GetClientName());
									return false;
								}

								pSource1->SetCount(pSource1->GetCount() - dwSourceCount1);
							}

							if (pSource2 != NULL)
							{
								if (pSource2->GetCount() < dwSourceCount2)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("재료(%s)가 부족합니다."), pSource2->GetClientName());
									return false;
								}

								pSource2->SetCount(pSource2->GetCount() - dwSourceCount2);
							}

							LPITEM pBottle = FindSpecifyItem(50901);

							if (!pBottle || pBottle->GetCount() < 1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("빈 병이 모자릅니다."));
								return false;
							}

							pBottle->SetCount(pBottle->GetCount() - 1);

							if (number(1, 100) > item->GetValue(5))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("물약 제조에 실패했습니다."));
								return false;
							}

							AutoGiveItem(item->GetValue(0));
						}
						break;
				}
			}
			break;

		case ITEM_METIN:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetType() == ITEM_PICK) return false;
				if (item2->GetType() == ITEM_ROD) return false;
				
				if (item->GetValue(0) != 0)
				{
					if (!item2->IsItemNewStoneEquipment())
					{
						ChatPacket(CHAT_TYPE_INFO, "You cant do that");
						return false;
					}
					
					if (item->GetValue(0) == 1 && !(item2->GetWearFlag() & WEARABLE_SHIELD))
					{
						ChatPacket(CHAT_TYPE_INFO, "You cant add on this equipment!");
						return false;
					}
					else if (item->GetValue(0) == 2 && !(item2->GetWearFlag() & WEARABLE_EAR))
					{
						ChatPacket(CHAT_TYPE_INFO, "You cant add on this equipment!");
						return false;
					}
					else if (item->GetValue(0) == 3 && !(item2->GetWearFlag() & WEARABLE_WRIST))
					{
						ChatPacket(CHAT_TYPE_INFO, "You cant add on this equipment!");
						return false;
					}
					else if (item->GetValue(0) == 4 && !(item2->GetWearFlag() & WEARABLE_FOOTS))
					{
						ChatPacket(CHAT_TYPE_INFO, "You cant add on this equipment!");
						return false;
					}
					else if (item->GetValue(0) == 5 && !(item2->GetWearFlag() & WEARABLE_HEAD))
					{
						ChatPacket(CHAT_TYPE_INFO, "You cant add on this equipment!");
						return false;
					}
					else if (item->GetValue(0) == 6 && !(item2->GetWearFlag() & WEARABLE_NECK))
					{
						ChatPacket(CHAT_TYPE_INFO, "You cant add on this equipment!");
						return false;
					}
					
					if (item2->GetSocket(0) != 0)
					{
						ChatPacket(CHAT_TYPE_INFO, "You have an stone attached already!");
						return false;
					}
					
					item2->SetSocket(0, item->GetVnum());
					item2->UpdatePacket();
					ITEM_MANAGER::instance().FlushDelayedSave(item2);
					item->SetCount(item->GetCount() - 1);
					return true;
				}
				
				int i;

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				{
					DWORD dwVnum;

					if ((dwVnum = item2->GetSocket(i)) <= 2)
						continue;

					TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

					if (!p)
						continue;

					if (item->GetValue(5) == p->alValues[5])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("같은 종류의 메틴석은 여러개 부착할 수 없습니다."));
						return false;
					}
				}

				if (item2->GetType() == ITEM_ARMOR)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_BODY) || !IS_SET(item2->GetWearFlag(), WEARABLE_BODY))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 메틴석은 장비에 부착할 수 없습니다."));
						return false;
					}
				}
				else if (item2->GetType() == ITEM_WEAPON)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_WEAPON))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 메틴석은 무기에 부착할 수 없습니다."));
						return false;
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("부착할 수 있는 슬롯이 없습니다."));
					return false;
				}

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					if (item2->GetSocket(i) >= 1 && item2->GetSocket(i) <= 2 && item2->GetSocket(i) >= item->GetValue(2))
					{
#ifdef ENABLE_OWN_STONE_CHANCE
						if (number(1, 100) <= stone_chance)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("메틴석 부착에 성공하였습니다."));
							item2->SetSocket(i, item->GetVnum());
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("메틴석 부착에 실패하였습니다."));
							item2->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
						}
#else
#ifdef ENABLE_ADDSTONE_FAILURE
						if (number(1, 100) <= 30)
#else
						if (1)
#endif
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("메틴석 부착에 성공하였습니다."));
							item2->SetSocket(i, item->GetVnum());
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("메틴석 부착에 실패하였습니다."));
							item2->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
						}
#endif
						LogManager::instance().ItemLog(this, item2, "SOCKET", item->GetName());
						// ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (METIN)");
						item->SetCount(item->GetCount() - 1);
						break;
					}

				if (i == ITEM_SOCKET_MAX_NUM)
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("부착할 수 있는 슬롯이 없습니다."));
			}
			break;

		case ITEM_AUTOUSE:
		case ITEM_MATERIAL:
		case ITEM_SPECIAL:
		case ITEM_TOOL:
		case ITEM_LOTTERY:
			break;

		case ITEM_TOTEM:
			{
				if (!item->IsEquipped())
					EquipItem(item);
			}
			break;

#ifdef ENABLE_EXTENDED_BLEND
	case ITEM_BLEND:

		// sys_log(0,"ITEM_BLEND!!");
		if (Blend_Item_find(item->GetVnum()))
		{
			// Combine dews
			// src item
			int affect_type = GetAffectType(item);
			int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
			int apply_value = item->GetSocket(0);
			bool active_blend = (item->GetSocket(1) == 1) ? true : false;
			int apply_duration = item->GetSocket(2);

			// dest item
			LPITEM pDestItem;
			if (IsValidItemPosition(DestCell) && (pDestItem = GetItem(DestCell)))
			{
				if (!IsValidItemPosition(DestCell) || !(pDestItem = GetItem(DestCell)))
				{
					return false;
				}

				int affect_dest_type = GetAffectType(pDestItem);
				int apply_dest_type = aApplyInfo[pDestItem->GetValue(0)].bPointType;
				int apply_dest_value = pDestItem->GetSocket(0);
				bool active_dest_blend = (pDestItem->GetSocket(1) == 1) ? true : false;

				if (pDestItem->GetType() != ITEM_BLEND)
				{
					return false;
				}

				if (pDestItem->IsExchanging())
				{
					return false;
				}

				// Max time set to 1 day
				if (pDestItem->GetSocket(2) > 86400)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Blend time has reached its maximum!"));
					return false;
				}

				if (active_blend || active_dest_blend)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot combine active blends!"));
					return false;
				}

				if (apply_type != apply_dest_type)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot combine different blends!"));
					return false;
				}

				if (apply_value != apply_dest_value)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Blends does not have the same values!"));
					return false;
				}

				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (ITEM_BLEND COMBINE)");
				pDestItem->SetSocket(2, pDestItem->GetSocket(2) + apply_duration);
			}
			else
			{
				if (!active_blend && (((affect_type == AFFECT_BLEND || affect_type == AFFECT_BLEND_EX) && (FindAffect(AFFECT_BLEND, apply_type) || FindAffect(AFFECT_BLEND_EX, apply_type))) || ((affect_type == AFFECT_WATER && (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, apply_type) || FindAffect(AFFECT_WATER, apply_type)))) || ((affect_type == AFFECT_MALL_EX && (FindAffect(AFFECT_MALL, apply_type) || FindAffect(AFFECT_MALL_EX, apply_type))))))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
					return false;
				}

				if ((affect_type == AFFECT_BLEND_EX || affect_type == AFFECT_WATER || affect_type == AFFECT_MALL_EX))
				{
					if (active_blend)
					{
						RemoveAffect(FindAffect(affect_type, apply_type));
						item->Lock(false);
						item->SetSocket(1, 0);
					}
					else
					{
						AddAffect(affect_type, apply_type, apply_value, 0, INFINITE_AFFECT_DURATION, 0, false);
						item->Lock(true);
						item->SetSocket(1, 1);
					}
				}
				else
				{
					AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
					item->SetCount(item->GetCount() - 1);
				}
			}
		}
		break;
#else
		case ITEM_BLEND:

			sys_log(0,"ITEM_BLEND!!");
			if (Blend_Item_find(item->GetVnum()))
			{
				int		affect_type		= AFFECT_BLEND;
				int		apply_type		= aApplyInfo[item->GetSocket(0)].bPointType;
				int		apply_value		= item->GetSocket(1);
				int		apply_duration	= item->GetSocket(2);

				if (FindAffect(affect_type, apply_type))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
				}
				else
				{
					if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
					}
					else
					{
						AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
						item->SetCount(item->GetCount() - 1);
					}
				}
			}
			break;
#endif
		case ITEM_EXTRACT:
			{
				LPITEM pDestItem = GetItem(DestCell);
				if (NULL == pDestItem)
				{
					return false;
				}
				switch (item->GetSubType())
				{
				case EXTRACT_DRAGON_SOUL:
					if (pDestItem->IsDragonSoul())
					{
						return DSManager::instance().PullOut(this, NPOS, pDestItem, item);
					}
					return false;
				case EXTRACT_DRAGON_HEART:
					if (item->GetVnum() == 100701)
					{
						if (pDestItem->IsDragonSoul())
						{
							// if (DSManager::instance().IsActiveDragonSoul(pDestItem) == true)
							// 	return false;

							if (pDestItem->IsDragonSoul() && pDestItem->IsEquipped())
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot modify equipped items."));
								return false;
							}
							else
							{
								DSManager::instance().ChangeAttributeDragonSoul(pDestItem);	
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success."));
								item->SetCount(item->GetCount() - 1);
							}
						}
					}
					else
					{
						if (pDestItem->IsDragonSoul())
						{
							return DSManager::instance().ExtractDragonHeart(this, pDestItem, item);
						}
					}
					return false;
				default:
					return false;
				}
			}
			break;

		case ITEM_NONE:
			sys_err("Item type NONE %s", item->GetName());
			break;

		default:
			sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), item->GetType());
			return false;
	}

	return true;
}

int g_nPortalLimitTime = 10;

bool CHARACTER::UseItem(TItemPos Cell, TItemPos DestCell)
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	//WORD wDestCell = DestCell.cell;
	//BYTE bDestInven = DestCell.window_type;
	LPITEM item;

	if (!CanHandleItem())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
			return false;

	sys_log(0, "%s: USE_ITEM %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	if (item->IsExchanging())
		return false;

#ifdef ENABLE_SWITCHBOT_SYSTEM
	if (Cell.IsSwitchbotPosition())
	{
		CSwitchbot *pkSwitchbot = CSwitchbotManager::Instance().FindSwitchbot(GetPlayerID());
		if (pkSwitchbot && pkSwitchbot->IsActive(Cell.cell))
		{
			return false;
		}

#ifdef ENABLE_SPECIAL_INVENTORY
		int iEmptyCell = GetEmptyInventory(item);
#else
		int iEmptyCell = GetEmptyInventory(item->GetSize());
#endif
		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot remove item from switchbot. Inventory is full."));
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
#endif

	if (!item->CanUsedBy(this))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군직이 맞지않아 이 아이템을 사용할 수 없습니다."));
		return false;
	}

	if (IsStun())
		return false;

	if (false == FN_check_item_sex(this, item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성별이 맞지않아 이 아이템을 사용할 수 없습니다."));
		return false;
	}

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (IsEditingPrivateShop())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use items while editing your personal shop."));
		return false;
	}
#endif

#ifdef BLOCK_ITEMS_ON_MAP
/*
	if (IS_ENABLE_ITEM(item->GetVnum()))
	{
		if (false == IS_ENABLE_ITEM_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item on this map."));
			return false;
		}
	}
*/

	if (IS_ENABLE_ITEM2(item->GetType()))
	{
		if (false == IS_ENABLE_ITEM_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item on this map."));
			return false;
		}
	}

	if (IS_ENABLE_ITEM3(item->GetSubType()))
	{
		if (false == IS_ENABLE_ITEM_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item on this map."));
			return false;
		}
	}
#endif

#ifdef FAST_EQUIP_WORLDARD
	if(Cell.IsChangeEquipPosition())
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		int iEmptyCell = GetEmptyInventory(item);
#else
		int iEmptyCell = GetEmptyInventory(item->GetSize());
#endif
		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot remove item from fast equip. Inventory is full."));
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}

#endif

	//PREVENT_TRADE_WINDOW
	if (IS_SUMMON_ITEM(item->GetVnum()))
	{
		if (false == IS_SUMMONABLE_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용할수 없습니다."));
			return false;
		}

		if (CThreeWayWar::instance().IsThreeWayWarMapIndex(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("삼거리 전투 참가중에는 귀환부,귀환기억부를 사용할수 없습니다."));
			return false;
		}
		int iPulse = thecore_pulse();

		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("창고를 연후 %d초 이내에는 귀환부,귀환기억부를 사용할 수 없습니다."), g_nPortalLimitTime);

			if (test_server)
				ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
			return false;
		}

		if (iPulse - GetGuildStorageLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A￠°i¸| ¿￢EA %dAE AI³≫¿¡´A ±IE?ºI,±IE?±a¾iºI¸| ≫c¿eCO ¼o ¾ø½A´I´U."), g_nPortalLimitTime);

			if (test_server)
				ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetGuildStorageLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
			return false;
		}

		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			|| IsAcceOpened()
#endif
#if defined(__BL_67_ATTR__)
			|| Is67AttrOpen()
#endif
			|| IsOpenGuildStorage()
		)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래창,창고 등을 연 상태에서는 귀환부,귀환기억부 를 사용할수 없습니다."));
			return false;
		}

		//PREVENT_REFINE_HACK

		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 개량후 %d초 이내에는 귀환부,귀환기억부를 사용할 수 없습니다."), g_nPortalLimitTime);
				return false;
			}
		}
		//END_PREVENT_REFINE_HACK

		//PREVENT_ITEM_COPY
		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개인상점 사용후 %d초 이내에는 귀환부,귀환기억부를 사용할 수 없습니다."), g_nPortalLimitTime);
				return false;
			}

		}
		//END_PREVENT_ITEM_COPY

		if (item->GetVnum() != 70302)
		{
			PIXEL_POSITION posWarp;

			int x = 0;
			int y = 0;

			double nDist = 0;
			const double nDistant = 5000.0;

			if (item->GetVnum() == 22010)
			{
				x = item->GetSocket(0) - GetX();
				y = item->GetSocket(1) - GetY();
			}

			else if (item->GetVnum() == 22000)
			{
				SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp);

				if (item->GetSocket(0) == 0)
				{
					x = posWarp.x - GetX();
					y = posWarp.y - GetY();
				}
				else
				{
					x = item->GetSocket(0) - GetX();
					y = item->GetSocket(1) - GetY();
				}
			}

			nDist = sqrt(pow((float)x,2) + pow((float)y,2));

			if (nDistant > nDist)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이동 되어질 위치와 너무 가까워 귀환부를 사용할수 없습니다."));
				if (test_server)
					ChatPacket(CHAT_TYPE_INFO, "PossibleDistant %f nNowDist %f", nDistant,nDist);
				return false;
			}
		}

		//PREVENT_PORTAL_AFTER_EXCHANGE

		if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래 후 %d초 이내에는 귀환부,귀환기억부등을 사용할 수 없습니다."), g_nPortalLimitTime);
			return false;
		}
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

	}

	if ((item->GetVnum() == 50200) || (item->GetVnum() == 71049))
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			|| IsAcceOpened()
#endif
#if defined(__BL_67_ATTR__)
			|| Is67AttrOpen()
#endif	
			|| IsOpenGuildStorage()
		)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래창,창고 등을 연 상태에서는 보따리,비단보따리를 사용할수 없습니다."));
			return false;
		}

	}
	//END_PREVENT_TRADE_WINDOW

#ifdef __SKILL_COSTUME__
	if (item->GetType() == ITEM_SKILL_COSTUME)
	{
		if (m_pSkillLevels[item->GetValue(3)].bLevel < SKILL_MAX_LEVEL)
		{
			ChatPacket(CHAT_TYPE_INFO, "You cant use this because your skillgroup or skill level is invalid");
			return false;
		}
	}
#endif

#ifdef ENABLE_EXTENDED_BLEND
	if (item->GetType() == ITEM_USE && (item->GetSubType() == USE_AFFECT || item->GetSubType() == USE_ABILITY_UP))
	{
		int apply_type;
		if ((item->GetType() == ITEM_USE && item->GetSubType() == USE_AFFECT))
		{
			apply_type = aApplyInfo[item->GetValue(1)].bPointType;
		}
		else
		{
			apply_type = aApplyInfo[item->GetValue(0)].bPointType;
		}

		if ((item->IsWaterItem() && FindAffect(AFFECT_WATER, apply_type)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
			return false;
		}

		if ((item->IsDragonGodItem() && FindAffect(AFFECT_MALL_EX, apply_type)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
			return false;
		}
	}
#endif

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item if you're using quests"));
		return false;
	}
	// @fixme150 END

	if (IS_SET(item->GetFlag(), ITEM_FLAG_LOG))
	{
		DWORD vid = item->GetVID();
		DWORD oldCount = item->GetCount();
		DWORD vnum = item->GetVnum();

		char hint[ITEM_NAME_MAX_LEN + 32 + 1];
		int len = snprintf(hint, sizeof(hint) - 32, "%s", item->GetName());

		if (len < 0 || len >= (int) sizeof(hint) - 32)
			len = (sizeof(hint) - 32) - 1;

		bool ret = UseItemEx(item, DestCell);

		if (NULL == ITEM_MANAGER::instance().FindByVID(vid))
		{
			//LogManager::instance().ItemLog(this, vid, vnum, "REMOVE", hint);
		}
		else if (oldCount != item->GetCount())
		{
			snprintf(hint + len, sizeof(hint) - len, " %u", oldCount - 1);
			//LogManager::instance().ItemLog(this, vid, vnum, "USE_ITEM", hint);
		}
		return (ret);
	}
	else
		return UseItemEx(item, DestCell);
}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
bool CHARACTER::DropItem(TItemPos Cell, short bCount)
#else
bool CHARACTER::DropItem(TItemPos Cell, BYTE bCount)
#endif
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("강화창을 연 상태에서는 아이템을 옮길 수 없습니다."));
		return false;
	}
#ifdef ENABLE_NEWSTUFF
	if (g_ItemDropTimeLimitValue && !PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::ItemDrop, std::chrono::milliseconds(g_ItemDropTimeLimitValue)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait."));
		return false;
	}
#endif
	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	// Nur prufen, wenn der Charakter uberhaupt ein GM ist
	if (GetGMLevel() > GM_PLAYER && !test_server)
	{
		if (!check_allow(GetGMLevel(), GM_ALLOW_DROP_GM_ITEM))
		{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this with this gamemaster rank."));
		return false;
		}
	}
	
	if (item->IsExchanging())
		return false;

	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	// START_BLOCK_PLAYER
	if (g_isBlockedPlayer(GetName()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU HAVE BLOCK STATUS, OPEN A SUPPORT TICKET"));
		return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("버릴 수 없는 아이템입니다."));
		return false;
	}

#ifdef ENABLE_GM_BLOCK
	if (IsGM() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GM_BLOCK_FALL"));
		return false;
	}
#endif

	if (bCount == 0 || bCount > item->GetCount())
#ifdef ENABLE_EXTENDED_ITEM_COUNT
		bCount = (short)item->GetCount();
#else
		bCount = (BYTE)item->GetCount();
#endif

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);

	LPITEM pkItemToDrop;

	if (bCount == item->GetCount())
	{
		item->RemoveFromCharacter();
		pkItemToDrop = item;
	}
	else
	{
		if (bCount == 0)
		{
			if (test_server)
				sys_log(0, "[DROP_ITEM] drop item count == 0");
			return false;
		}

		item->SetCount(item->GetCount() - bCount);
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		pkItemToDrop = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), bCount);

		// copy item socket -- by mhh
		FN_copy_item_socket(pkItemToDrop, item);

		char szBuf[51 + 1];
		snprintf(szBuf, sizeof(szBuf), "%u %u", pkItemToDrop->GetID(), pkItemToDrop->GetCount());
		LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
	}

	PIXEL_POSITION pxPos = GetXYZ();

	if (pkItemToDrop->AddToGround(GetMapIndex(), pxPos))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("떨어진 아이템은 3분 후 사라집니다."));
#ifdef ENABLE_NEWSTUFF
		pkItemToDrop->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_DROPITEM]);
#else
		pkItemToDrop->StartDestroyEvent();
#endif

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemToDrop);

		char szHint[32 + 1];
		snprintf(szHint, sizeof(szHint), "%s %u %u", pkItemToDrop->GetName(), pkItemToDrop->GetCount(), pkItemToDrop->GetOriginalVnum());
		LogManager::instance().ItemLog(this, pkItemToDrop, "DROP", szHint);
		//Motion(MOTION_PICKUP);
	}

	return true;
}


#ifdef ENABLE_ITEM_MANAGER
bool CHARACTER::DestroyItem(TItemPos Cell)
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		}

		return false;
	}

	if (IsDead())
	{
		return false;
	}

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
	{
		return false;
	}

	if (item->IsExchanging() || item->IsEquipped())
	{
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	// TODO
	// if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DESTROY))
	// {
	// 	return false;
	// }

#ifdef ENABLE_SOULBIND_SYSTEM
	if (item->IsSealed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot destroy a soulbound item."));
		return false;
	}
#endif

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		return false;
	}

	if (item->GetCount() <= 0)
	{
		return false;
	}

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, WORD_MAX);
	/*
	// Give shards
	if (item->GetType() == ITEM_WEAPON || item->GetType() == ITEM_ARMOR)
	{
		if (item->GetLimitType(0) == LIMIT_LEVEL && item->GetLimitValue(0) > 40)
		{
			int shards = 0;
			shards = number(1, 12);
			AutoGiveItem(51001, shards, -1, false);
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d shards."), shards);
		}
	}
	*/

#ifdef ENABLE_BATTLE_PASS
	BYTE bBattlePassId = GetBattlePassId();
	if (bBattlePassId)
	{
		DWORD dwItemVnum, dwCount;
		if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, DESTROY_ITEM, &dwItemVnum, &dwCount))
		{
			// sys_log(0, "dwItemVnum: %d dwCount: %d || destroyedItem: %d destroyedItemCount: %d", dwItemVnum, dwCount, item->GetVnum(), item->GetCount());
			if (dwItemVnum == item->GetVnum() && GetMissionProgress(DESTROY_ITEM, bBattlePassId) < dwCount)
			{
				UpdateMissionProgress(DESTROY_ITEM, bBattlePassId, item->GetCount(), dwCount);
			}
		}
	}
#endif

	// ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have deleted %s."), item->GetName());
	ITEM_MANAGER::instance().RemoveItem(item);

	return true;
}

bool CHARACTER::SellItem(TItemPos Cell)
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		}

		return false;
	}

	if (IsDead())
	{
		return false;
	}

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
	{
		return false;
	}

	if (item->IsExchanging() || item->IsEquipped())
	{
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
	{
		return false;
	}

#ifdef ENABLE_SOULBIND_SYSTEM
	if (item->IsSealed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell a soulbound item."));
		return false;
	}
#endif

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		return false;
	}

	if (item->GetCount() <= 0)
	{
		return false;
	}

#ifdef ENABLE_GOLD_MAX_EXTENDED
	unsigned long long dwPrice;
#else
	DWORD dwPrice;
#endif
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	short bCount = item->GetCount();
#else
	BYTE bCount = item->GetCount();
#endif

	dwPrice = item->GetShopBuyPrice();

	if (IS_SET(item->GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (dwPrice == 0)
		{
			dwPrice = bCount;
		}
		else
		{
			dwPrice = bCount / dwPrice;
		}
	}
	else
	{
		dwPrice *= bCount;
	}

	// dwPrice /= 5;

#ifdef ENABLE_GOLD_MAX_EXTENDED
	const long long nTotalMoney = static_cast<long long>(GetGold()) + static_cast<long long>(dwPrice);
	if (GetAllowedGold() <= nTotalMoney)
#else
	const int64_t nTotalMoney = static_cast<int64_t>(GetGold()) + static_cast<int64_t>(dwPrice);
	if (GOLD_MAX <= nTotalMoney)
#endif
	{
		// sys_err("[OVERFLOW_GOLD] OriGold %lld AddedGold %lld id %u Name %s ", GetGold(), dwPrice, GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have reached the maximum limit of yang."));
		return false;
	}

#ifdef ENABLE_BATTLE_PASS
	BYTE bBattlePassId = GetBattlePassId();
	if (bBattlePassId)
	{
		DWORD dwItemVnum, dwSellCount;
		if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, SELL_ITEM, &dwItemVnum, &dwSellCount))
		{
			if (dwItemVnum == item->GetVnum() && GetMissionProgress(SELL_ITEM, bBattlePassId) < dwSellCount)
			{
				UpdateMissionProgress(SELL_ITEM, bBattlePassId, item->GetCount(), dwSellCount);
			}
		}
	}
#endif

	if (bCount == item->GetCount())
	{
		ITEM_MANAGER::instance().RemoveItem(item, "SELL");
	}
	else
	{
		item->SetCount(item->GetCount() - bCount);
	}

#ifdef ENABLE_GOLD_MAX_EXTENDED
	ChangeGold(dwPrice);
#else
	PointChange(POINT_GOLD, dwPrice, false);
#endif

	// ChatPacket(CHAT_TYPE_INFO, "Sold for: %d", dwPrice);
	// ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s has succesfully sell"), item->GetName());
	return true;
}
#endif

bool CHARACTER::DropGold(int gold)
{
	if (gold <= 0 || gold > GetGold())
		return false;

	if (!CanHandleItem())
		return false;

#ifdef ENABLE_NEWSTUFF
	if (g_GoldDropTimeLimitValue && !PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::BoxOpening, std::chrono::milliseconds(g_GoldDropTimeLimitValue)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to wait."));
		return false;
	}
#endif

	LPITEM item = ITEM_MANAGER::instance().CreateItem(1, gold);

	if (item)
	{
		PIXEL_POSITION pos = GetXYZ();

		if (item->AddToGround(GetMapIndex(), pos))
		{
			//Motion(MOTION_PICKUP);
#ifdef ENABLE_GOLD_MAX_EXTENDED
			ChangeGold(-gold);
#else
			PointChange(POINT_GOLD, -gold, true);
#endif

			if (gold > 1000)
				LogManager::instance().CharLog(this, gold, "DROP_GOLD", "");

#ifdef ENABLE_NEWSTUFF
			item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_DROPGOLD]);
#else
			item->StartDestroyEvent();
#endif
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("떨어진 아이템은 %d분 후 사라집니다."), 150/60);
		}

		Save();
		return true;
	}

	return false;
}

#ifdef FAST_EQUIP_WORLDARD
bool CHARACTER::ChechPositionAvailable(int iWearCell)
{
	if(iWearCell < 0){
		return false;
	}

	for (int a = 0; a <sizeof(EWearCheckPositions)/sizeof(*EWearCheckPositions); ++a)
	{
		if(iWearCell == EWearCheckPositions[a]){
			return true;
		}
	}

	return false;

}

int CHARACTER::IsWearUniqueChangeEquip(BYTE page_index_ce,LPITEM item)
{
	DWORD index_old = CHANGE_EQUIP_SLOT_COUNT-(CHANGE_EQUIP_SLOT_COUNT/page_index_ce);

	if(page_index_ce > 1){
		index_old = CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA*page_index_ce;
	}


	LPITEM item_check = GetChangeEquipItem(index_old+WEAR_UNIQUE1);
	if(item_check && item)
	{
		if(item->GetVnum() == item_check->GetVnum())
		{
			return WEAR_UNIQUE1;
		}

		return WEAR_UNIQUE2;
	}

	return WEAR_UNIQUE1;
}


bool CHARACTER::IsValidItemChangeEquip(int cell, LPITEM item)
{	

	if(cell < 0)
	{
		return false;
	}


	if(cell > CHANGE_EQUIP_SLOT_COUNT)
	{
		return false;
	}


	BYTE page_index_ce = 1;

	for (int i = 1; i < CHANGE_EQUIP_PAGE_EXTRA; ++i)
	{
		if(cell >= CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA*i && cell < (CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA)*(i+1))
		{
			page_index_ce = i;
		}
	}

	int iWearCell = item->FindEquipCell(this);

	if (item->GetType() == ITEM_UNIQUE)
	{
		iWearCell = IsWearUniqueChangeEquip(page_index_ce,item);
	}

	if(!ChechPositionAvailable(iWearCell)){
		return false;
	}

	if(GetChangeEquipItem(cell)){
		return false;
	}

	if (!item->CheckItemUseLevel(GetLevel())){
		return false;
	}	

	if(iWearCell == WEAR_ARROW){
		return false;
	}


	for (int i = 1; i < CHANGE_EQUIP_PAGE_EXTRA; ++i)
	{
		if(cell >= CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA*i && cell < (CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA)*(i+1))
		{
			cell = cell - ((CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA)*i);
		}
	}

	if(iWearCell != cell)
	{
		return false;
	}	

	if ((item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON) || item->GetType() == ITEM_WEAPON)
	{
		
		DWORD index_old = CHANGE_EQUIP_SLOT_COUNT-(CHANGE_EQUIP_SLOT_COUNT/page_index_ce);

		if(page_index_ce > 1){
			index_old = CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA*page_index_ce;
		}

		LPITEM check_weapon = NULL;
		LPITEM check_costume = NULL;

		for (int i = index_old; i < (CHANGE_EQUIP_SLOT_COUNT/CHANGE_EQUIP_PAGE_EXTRA)*(page_index_ce+1); ++i)
		{
			LPITEM item_check = GetChangeEquipItem(i);
			if(item_check)
			{
				if(item_check->GetType() == ITEM_WEAPON)
				{
					check_weapon = item_check;
				}

				if(item_check->GetType() == ITEM_COSTUME && item_check->GetSubType() == COSTUME_WEAPON)
				{
					check_costume = item_check;
				}
			}
		}

		if(item->GetType() == ITEM_WEAPON)
		{
			if(check_costume != NULL)
			{
				if(check_costume->GetValue(3) != item->GetSubType()){
					return false;
				}
			}
		}

		if(item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
		{
			if(check_weapon != NULL)
			{
				if(item->GetValue(3) != check_weapon->GetSubType()){
					return false;
				}
			}
		}

	}

	switch (GetJob())
	{
		case JOB_WARRIOR:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;
	}

	if (false == FN_check_item_sex(this, item))
	{
		return false;
	}
	
	return true;
}

#endif

#ifdef ENABLE_EXTENDED_ITEM_COUNT
bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, short count)
#else
bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, BYTE count)
#endif
{
	if (Cell == DestCell) // @fixme196
		return false;

	if (!IsValidItemPosition(Cell))
		return false;

	LPITEM item = NULL;
	if (!(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (item->GetCount() < count)
		return false;

	if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_MAX_NUM && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	if (true == item->isLocked() && !CAN_MOVE_LOCKED_ITEM(item))
		return false;

	if (!IsValidItemPosition(DestCell))
	{
		return false;
	}

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (IsEditingPrivateShop())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move items while your personal shop window is open."));
		return false;
	}
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	if (!DestCell.IsChangeEquipPosition() && item->GetSpecialInventoryType() != DestCell.GetSpecialInventoryType())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item here."));
		return false;
	}
#endif

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("강화창을 연 상태에서는 아이템을 옮길 수 없습니다."));
		return false;
	}

#ifdef FAST_EQUIP_WORLDARD
	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item) && !DestCell.IsChangeEquipPosition())
#else
	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
#endif
	// if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 벨트 인벤토리로 옮길 수 없습니다."));
		return false;
	}

#ifdef ENABLE_SWITCHBOT_SYSTEM
	if (Cell.IsSwitchbotPosition() && CSwitchbotManager::Instance().IsActive(GetPlayerID(), Cell.cell))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot move active switchbot item."));
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && !SwitchbotHelper::IsValidItem(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Invalid item type for switchbot."));
		return false;
	}

	if (Cell.IsSwitchbotPosition() && DestCell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot equip items directly from switchbot."));
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && Cell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot move equipped items to switchbot."));
		return false;
	}

	if (Cell.IsSwitchbotPosition())
	{
		if (!IsEmptyItemGrid(DestCell, item->GetSize()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot move item there!"));
			return false;
		}
	}
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	if (!DestCell.IsChangeEquipPosition() && DestCell.IsSpecialInventoryPosition() && item->GetSpecialInventoryType() != DestCell.GetSpecialInventoryType())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item here."));
		return false;
	}
#endif

#ifdef FAST_EQUIP_WORLDARD
	if(DestCell.IsChangeEquipPosition())
	{
		if(!IsValidItemChangeEquip(DestCell.cell,item)){
			ChatPacket(CHAT_TYPE_INFO,"No puedes mover el item a ese slot");
			return false;
		}

		if(Cell.IsEquipPosition()){
			return false;
		}
	}
#endif

#ifdef FAST_EQUIP_WORLDARD
	if (Cell.IsEquipPosition() && !Cell.IsChangeEquipPosition())
#else
	if (Cell.IsEquipPosition())
#endif
	{
		if (!CanUnequipNow(item))
			return false;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		int iWearCell = item->FindEquipCell(this);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}

			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
				return UnequipItem(item);
		}
#endif
	}

#ifdef FAST_EQUIP_WORLDARD
	if (DestCell.IsEquipPosition() && !DestCell.IsChangeEquipPosition())
#else
	if (DestCell.IsEquipPosition())
#endif	
	{
		if (GetItem(DestCell))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 장비를 착용하고 있습니다."));

			return false;
		}

		EquipItem(item, DestCell.cell - INVENTORY_MAX_NUM);
	}
	else
	{
		if (item->IsDragonSoul())
		{
			if (item->IsEquipped())
			{
				return DSManager::instance().PullOut(this, DestCell, item);
			}
			else
			{
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					return false;
				}

				if (!DSManager::instance().IsValidCellForThisItem(item, DestCell))
					return false;
			}
		}

		else if (DRAGON_SOUL_INVENTORY == DestCell.window_type)
			return false;

		LPITEM item2;

		if ((item2 = GetItem(DestCell)) && item != item2 && item2->IsStackable() &&
				!IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
				item2->GetVnum() == item->GetVnum())
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				if (item2->GetSocket(i) != item->GetSocket(i))
					return false;

			if (count == 0)
#ifdef ENABLE_EXTENDED_ITEM_COUNT
				count = (short)item->GetCount();
#else
				count = (BYTE)item->GetCount();
#endif

			sys_log(0, "%s: ITEM_STACK %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			count = MIN(g_bItemCountLimit - item2->GetCount(), count);

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);
			return true;
		}

#ifdef ENABLE_SPECIAL_INVENTORY
		if (item2 && ((item->GetVnum() != item2->GetVnum()) || (!item->IsStackable() || !item2->IsStackable())))
		{
			return false;
		}

		if (DestCell.GetSpecialInventoryType() != -1)
		{
			if (!IsEmptySpecialItemGrid(DestCell, item->GetSize(), Cell.cell))
			{
				return false;
			}
		}
		else
		{
			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
			{
				return false;
			}
		}
#else
		if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
			return false;
#endif

		if (count == 0 || count >= item->GetCount() || !item->IsStackable() || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			sys_log(0, "%s: ITEM_MOVE %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->RemoveFromCharacter();
#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM
			SetItem(DestCell, item, true);
#else
			SetItem(DestCell, item);
#endif
			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
		}
		else if (count < item->GetCount())
		{
			sys_log(0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->SetCount(item->GetCount() - count);
			LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), count);

			// copy socket -- by mhh
			FN_copy_item_socket(item2, item);

			item2->AddToCharacter(this, DestCell);

			char szBuf[51+1];
			snprintf(szBuf, sizeof(szBuf), "%u %u %u %u ", item2->GetID(), item2->GetCount(), item->GetCount(), item->GetCount() + item2->GetCount());
			LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
		}
	}

	return true;
}

namespace NPartyPickupDistribute
{
	struct FFindOwnership
	{
		LPITEM item;
		LPCHARACTER owner;

		FFindOwnership(LPITEM item)
			: item(item), owner(NULL)
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (item->IsOwnership(ch))
				owner = ch;
		}
	};

	struct FCountNearMember
	{
		int		total;
		int		x, y;

		FCountNearMember(LPCHARACTER center )
			: total(0), x(center->GetX()), y(center->GetY())
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				total += 1;
		}
	};

	struct FMoneyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iMoney;

		FMoneyDistributor(LPCHARACTER center, int iMoney)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iMoney(iMoney)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch!=c)
				if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
#ifdef ENABLE_GOLD_MAX_EXTENDED
					ch->ChangeGold(iMoney);
#else
					ch->PointChange(POINT_GOLD, iMoney, true);
#endif
					if (iMoney > 1000)
					{
						LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(ch, iMoney, "GET_GOLD", ""));
					}
				}
		}
	};
}

void CHARACTER::GiveGold(int iAmount)
{
	if (iAmount <= 0)
		return;

	sys_log(0, "GIVE_GOLD: %s %d", GetName(), iAmount);

#ifdef ENABLE_BATTLE_PASS
	BYTE bBattlePassId = GetBattlePassId();
	if (bBattlePassId)
	{
		DWORD dwYangCount, dwNotUsed;
		if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, FARM_YANG, &dwNotUsed, &dwYangCount))
		{
			if (GetMissionProgress(FARM_YANG, bBattlePassId) < dwYangCount)
			{
				UpdateMissionProgress(FARM_YANG, bBattlePassId, iAmount, dwYangCount);
			}
		}
	}
#endif

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist(this, dwShare);

			pParty->ForEachOnlineMember(funcMoneyDist);
		}

#ifdef ENABLE_GOLD_MAX_EXTENDED
		ChangeGold(dwMyAmount);
#else
		PointChange(POINT_GOLD, dwMyAmount, true);
#endif

		if (dwMyAmount > 1000)
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, dwMyAmount, "GET_GOLD", ""));
		}
	}
	else
	{
#ifdef ENABLE_GOLD_MAX_EXTENDED
		ChangeGold(iAmount);
#else
		PointChange(POINT_GOLD, iAmount, true);
#endif

		if (iAmount > 1000)
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, iAmount, "GET_GOLD", ""));
		}
	}
}

bool CHARACTER::PickupItem(DWORD dwVID)
{
	LPITEM item = ITEM_MANAGER::instance().FindByVID(dwVID);

	if (IsObserverMode())
		return false;

	if (!item || !item->GetSectree())
		return false;

#ifdef ENABLE_BATTLE_PASS
	bool bIsBattlePass = item->HaveOwnership();
#endif

	if (item->DistanceValid(this))
	{
		// @fixme150 BEGIN
		if (item->GetType() == ITEM_QUEST)
		{
			if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot pickup this item if you're using quests"));
				return false;
			}
		}
		// @fixme150 END

		if (item->IsOwnership(this))
		{
			if (item->GetType() == ITEM_ELK)
			{
				GiveGold(item->GetCount());
				item->RemoveFromGround();

				M2_DESTROY_ITEM(item);

				Save();
			}

			else
			{
				if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
				{
#ifdef ENABLE_EXTENDED_ITEM_COUNT
					short bCount = item->GetCount();
#else
					BYTE bCount = item->GetCount();
#endif
					for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
					{
						LPITEM item2 = GetInventoryItem(i);

						if (!item2)
							continue;

						if (item2->GetVnum() == item->GetVnum())
						{
							int j;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
								if (item2->GetSocket(j) != item->GetSocket(j))
									break;

							if (j != ITEM_SOCKET_MAX_NUM)
								continue;

#ifdef ENABLE_EXTENDED_ITEM_COUNT
							short bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
#else
							BYTE bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
#endif
							bCount -= bCount2;

#ifdef ENABLE_BATTLE_PASS
							if (bIsBattlePass)
							{
								BYTE bBattlePassId = GetBattlePassId();
								if (bBattlePassId)
								{
									DWORD dwItemVnum, dwCount;
									if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COLLECT_ITEM, &dwItemVnum, &dwCount))
									{
										if (dwItemVnum == item->GetVnum() && GetMissionProgress(COLLECT_ITEM, bBattlePassId) < dwCount)
										{
											UpdateMissionProgress(COLLECT_ITEM, bBattlePassId, bCount2, dwCount);
										}
									}
								}
							}
#endif

							item2->SetCount(item2->GetCount() + bCount2);

							if (bCount == 0)
							{
								ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("아이템 획득: %s"), item2->GetClientName());
								M2_DESTROY_ITEM(item);
								if (item2->GetType() == ITEM_QUEST)
									quest::CQuestManager::instance().PickupItem (GetPlayerID(), item2);
#ifdef ENABLE_QUEST_RECEIVE_ITEM
								SenderRecvItem(GetPlayerID(), item2);
#endif
								return true;
							}
						}
					}

#ifdef ENABLE_SPECIAL_INVENTORY
					for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
					{
						LPITEM item2 = GetInventoryItem(i);

						if (!item2)
						{
							continue;
						}

						if (item2->GetVnum() == item->GetVnum())
						{
							int j;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
							{
								if (item2->GetSocket(j) != item->GetSocket(j))
								{
									break;
								}
							}

							if (j != ITEM_SOCKET_MAX_NUM)
							{
								continue;
							}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
							short bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
#else
							BYTE bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
#endif
							bCount -= bCount2;

							item2->SetCount(item2->GetCount() + bCount2);

							if (bCount == 0)
							{
								ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("아이템 획득: %s"), item2->GetClientName());
								M2_DESTROY_ITEM(item);
								if (item2->GetType() == ITEM_QUEST)
								{
									quest::CQuestManager::instance().PickupItem(GetPlayerID(), item2);
								}
#ifdef ENABLE_QUEST_RECEIVE_ITEM
								SenderRecvItem(GetPlayerID(), item2);
#endif
								return true;
							}
						}
					}
#endif
					item->SetCount(bCount);
				}

				int iEmptyCell;
				if (item->IsDragonSoul())
				{
					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지하고 있는 아이템이 너무 많습니다."));
						return false;
					}
				}
				else
				{
#ifdef ENABLE_SPECIAL_INVENTORY
					if ((iEmptyCell = GetEmptyInventory(item)) == -1)
#else
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
#endif
					{
						sys_log(0, "No empty inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지하고 있는 아이템이 너무 많습니다."));
						return false;
					}
				}

				item->RemoveFromGround();

				if (item->IsDragonSoul())
					item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
				else
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));

				char szHint[32+1];
				snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
				LogManager::instance().ItemLog(this, item, "GET", szHint);
				ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("아이템 획득: %s"), item->GetClientName());

#ifdef ENABLE_BATTLE_PASS
				if (bIsBattlePass)
				{
					BYTE bBattlePassId = GetBattlePassId();
					if (bBattlePassId)
					{
						DWORD dwItemVnum, dwCount;
						if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COLLECT_ITEM, &dwItemVnum, &dwCount))
						{
							if (dwItemVnum == item->GetVnum() && GetMissionProgress(COLLECT_ITEM, bBattlePassId) < dwCount)
							{
								UpdateMissionProgress(COLLECT_ITEM, bBattlePassId, item->GetCount(), dwCount);
							}
						}
					}
				}
#endif

				if (item->GetType() == ITEM_QUEST)
					quest::CQuestManager::instance().PickupItem (GetPlayerID(), item);

#ifdef ENABLE_QUEST_RECEIVE_ITEM
				SenderRecvItem(GetPlayerID(), item);
#endif
			}

			//Motion(MOTION_PICKUP);
			return true;
		}
		else if (!IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP) && GetParty())
		{
			NPartyPickupDistribute::FFindOwnership funcFindOwnership(item);

			GetParty()->ForEachOnlineMember(funcFindOwnership);

			LPCHARACTER owner = funcFindOwnership.owner;
			// @fixme115
			if (!owner)
				return false;

			int iEmptyCell;

			if (item->IsDragonSoul())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyDragonSoulInventory(item)) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지하고 있는 아이템이 너무 많습니다."));
						return false;
					}
				}
			}
			else
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				if (!(owner && (iEmptyCell = owner->GetEmptyInventory(item)) != -1))
#else
				if (!(owner && (iEmptyCell = owner->GetEmptyInventory(item->GetSize())) != -1))
#endif
				{
					owner = this;

#ifdef ENABLE_SPECIAL_INVENTORY
					if ((iEmptyCell = GetEmptyInventory(item)) == -1)
#else
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
#endif
					{
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지하고 있는 아이템이 너무 많습니다."));
						return false;
					}
				}
			}

			item->RemoveFromGround();

			if (item->IsDragonSoul())
				item->AddToCharacter(owner, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
			else
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));

#ifdef ENABLE_BATTLE_PASS
			if (bIsBattlePass)
			{
				BYTE bBattlePassId = owner->GetBattlePassId();
				if (bBattlePassId)
				{
					DWORD dwItemVnum, dwCount;
					if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COLLECT_ITEM, &dwItemVnum, &dwCount))
					{
						if (dwItemVnum == item->GetVnum() && owner->GetMissionProgress(COLLECT_ITEM, bBattlePassId) < dwCount)
						{
							owner->UpdateMissionProgress(COLLECT_ITEM, bBattlePassId, item->GetCount(), dwCount);
						}
					}
				}
			}
#endif

			char szHint[32+1];
			snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
			LogManager::instance().ItemLog(owner, item, "GET", szHint);

			if (owner == this)
				ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("아이템 획득: %s"), item->GetClientName());
			else
			{
				owner->ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("아이템 획득: %s 님으로부터 %s"), GetName(), item->GetClientName());
				ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("아이템 전달: %s 님에게 %s"), owner->GetName(), item->GetClientName());
			}

			if (item->GetType() == ITEM_QUEST)
				quest::CQuestManager::instance().PickupItem (owner->GetPlayerID(), item);

#ifdef ENABLE_QUEST_RECEIVE_ITEM
			owner->SenderRecvItem(owner->GetPlayerID(), item);
#endif
			return true;
		}
	}

	return false;
}

bool CHARACTER::SwapItem(WORD wCell, WORD wDestCell)
{
	if (!CanHandleItem())
	{
		return false;
	}

	TItemPos srcCell(INVENTORY, wCell), destCell(INVENTORY, wDestCell);

	// if (wCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM || wDestCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
	{
		return false;
	}

	if (wCell == wDestCell)
	{
		return false;
	}

	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
	{
		return false;
	}

	LPITEM item1, item2;

	if (srcCell.IsEquipPosition())
	{
		item1 = GetInventoryItem(wDestCell);
		item2 = GetInventoryItem(wCell);
	}
	else
	{
		item1 = GetInventoryItem(wCell);
		item2 = GetInventoryItem(wDestCell);
	}

	if (!item1 || !item2)
	{
		return false;
	}

	if (item1 == item2)
	{
		sys_log(0, "[WARNING][WARNING][HACK USER!] : %s %d %d", m_stName.c_str(), wCell, wDestCell);
		return false;
	}

	if (!IsEmptyItemGrid(TItemPos(INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
	{
		return false;
	}

	if (item2->GetType() == ITEM_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}

	if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		BYTE bEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		WORD wInvenCell = item1->GetCell();

		if (item2->IsDragonSoul() || item2->GetType() == ITEM_BELT) // @fixme117
		{
			if (false == CanUnequipNow(item2) || false == CanEquipNow(item1))
			{
				return false;
			}
		}

		if (bEquipCell != item1->FindEquipCell(this))
		{
			return false;
		}

		item2->RemoveFromCharacter();

		if (item1->EquipTo(this, bEquipCell))
		{
			item2->AddToCharacter(this, TItemPos(INVENTORY, wInvenCell));
		}
		else
		{
			sys_err("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
		}
	}
	else
	{
		WORD wCell1 = item1->GetCell();
		WORD wCell2 = item2->GetCell();

		item1->RemoveFromCharacter();
		item2->RemoveFromCharacter();

		item1->AddToCharacter(this, TItemPos(INVENTORY, wCell2));
		item2->AddToCharacter(this, TItemPos(INVENTORY, wCell1));
	}

	return true;
}

bool CHARACTER::UnequipItem(LPITEM item)
{
	if (!item)
		return false;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	int iWearCell = item->FindEquipCell(this);
	if (iWearCell == WEAR_WEAPON)
	{
		LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
		if (costumeWeapon && !UnequipItem(costumeWeapon))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
			return false;
		}
	}
#endif

	if (GetQuestFlag("duel_restricted") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, "You can't do that while you're in a duel.");
		return false;
	}

	if (false == CanUnequipNow(item))
		return false;

	int pos = -1;
	if (item->IsDragonSoul())
		pos = GetEmptyDragonSoulInventory(item);
	else
#ifdef ENABLE_SPECIAL_INVENTORY
		pos = GetEmptyInventory(item);
#else
		pos = GetEmptyInventory(item->GetSize());
#endif

#ifdef ENABLE_FRATELLO_FIX
	if (pos < 0)
	{
		if (item->IsDragonSoul())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough space in your dragon soul inventory."));
		else
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough space in your inventory."));
		return false;
	}
#endif

	if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		ShowAlignment(true);

	item->RemoveFromCharacter();

	if (item->IsDragonSoul())
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, pos));
	else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));

	CheckMaximumPoints();
	return true;
}

//

//
bool CHARACTER::EquipItem(LPITEM item, int iCandidateCell)
{
	if (item->IsExchanging())
		return false;

	if (false == item->IsEquipable())
		return false;

	if (false == CanEquipNow(item))
		return false;

	if (GetQuestFlag("duel_restricted") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, "You can't do that while you're in a duel.");
		return false;
	}

	int iWearCell = item->FindEquipCell(this, iCandidateCell);

	if (iWearCell < 0)
		return false;

	if (iWearCell == WEAR_BODY && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 탄 상태에서 예복을 입을 수 없습니다."));
		return false;
	}

	if (iWearCell != WEAR_ARROW && IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 착용중인 장비를 변경할 수 없습니다."));
		return false;
	}

	if (FN_check_item_sex(this, item) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성별이 맞지않아 이 아이템을 사용할 수 없습니다."));
		return false;
	}

	if (item->IsRideItem())
	{
		#ifdef ENABLE_MOUNT_COSTUME_EX_SYSTEM
			#ifdef ENABLE_NEWSTUFF
			// block mount spawn
			if (g_NoMountAtGuildWar && GetWarMap())
			{
				if (IsRiding())
					StopRiding();
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 탈것을 이용중입니다."));
				return false;
			}
			#endif

			// unsummon horse first
			if (GetHorse() || IsHorseRiding()) {
				StopRiding();
				HorseSummon(false);
			}

			if (GetHorse() || IsHorseRiding())
				return false;
		#else
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 탈것을 이용중입니다."));
		return false;
		#endif
	}

	DWORD dwCurTime = get_dword_time();

	if (iWearCell != WEAR_ARROW
		&& (dwCurTime - GetLastAttackTime() <= 600 || dwCurTime - m_dwLastSkillTime <= 600))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("가만히 있을 때만 착용할 수 있습니다."));
		return false;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (iWearCell == WEAR_WEAPON)
	{
		if (item->GetType() == ITEM_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && costumeWeapon->GetValue(3) != item->GetSubType() && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}
		}
		else //fishrod/pickaxe
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}
		}
	}
	else if (iWearCell == WEAR_COSTUME_WEAPON)
	{
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
		{
			LPITEM pkWeapon = GetWear(WEAR_WEAPON);
			if (!pkWeapon || pkWeapon->GetType() != ITEM_WEAPON || item->GetValue(3) != pkWeapon->GetSubType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip the costume weapon. Wrong equipped weapon."));
				return false;
			}
		}
	}
#endif

	if (item->IsDragonSoul())
	{
		if(GetInventoryItem(INVENTORY_MAX_NUM + iWearCell))
		{
			ChatPacket(CHAT_TYPE_INFO, "이미 같은 종류의 용혼석을 착용하고 있습니다.");
			return false;
		}

		if (!item->EquipTo(this, iWearCell))
		{
			return false;
		}
	}

	else
	{
		if (GetWear(iWearCell) && !IS_SET(GetWear(iWearCell)->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		{
			if (item->GetWearFlag() == WEARABLE_ABILITY)
				return false;

			if (false == SwapItem(item->GetCell(), INVENTORY_MAX_NUM + iWearCell))
			{
				return false;
			}
		}
		else
		{
			BYTE bOldCell = item->GetCell();

			if (item->EquipTo(this, iWearCell))
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, bOldCell, iWearCell);
			}
		}
	}

	if (true == item->IsEquipped())
	{
		if (-1 != item->GetProto()->cLimitRealTimeFirstUseIndex)
		{
			if (0 == item->GetSocket(1))
			{
				long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[(unsigned char)(item->GetProto()->cLimitRealTimeFirstUseIndex)].lValue;

				if (0 == duration)
					duration = 60 * 60 * 24 * 7;

				item->SetSocket(0, time(0) + duration);
				item->StartRealTimeExpireEvent();
			}

			item->SetSocket(1, item->GetSocket(1) + 1);
		}

		if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
			ShowAlignment(false);

		const DWORD& dwVnum = item->GetVnum();

		if (true == CItemVnumHelper::IsRamadanMoonRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_RAMADAN_RING);
		}

		else if (true == CItemVnumHelper::IsHalloweenCandy(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HALLOWEEN_CANDY);
		}

		else if (true == CItemVnumHelper::IsHappinessRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HAPPINESS_RING);
		}

		else if (true == CItemVnumHelper::IsLovePendant(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_LOVE_PENDANT);
		}
		else if (ITEM_UNIQUE == item->GetType() && 0 != item->GetSIGVnum())
		{
			const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(item->GetSIGVnum());
			if (NULL != pGroup)
			{
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(pGroup->GetAttrVnum(item->GetVnum()));
				if (NULL != pAttrGroup)
				{
					const std::string& std = pAttrGroup->m_stEffectFileName;
					SpecificEffectPacket(std.c_str());
				}
			}
		}
		#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_ACCE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
		#endif

		if (item->IsOldMountItem()) // @fixme152
			quest::CQuestManager::instance().SIGUse(GetPlayerID(), quest::QUEST_NO_NPC, item, false);
		#ifdef ENABLE_MOUNT_COSTUME_EX_SYSTEM
		else if (item->IsNewMountItem()) {
			const auto mountVnum = GetPoint(POINT_MOUNT);
			MountVnum(mountVnum);
		}
		#endif

	}

	return true;
}

void CHARACTER::BuffOnAttr_AddBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->AddBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->RemoveBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_ClearAll()
{
	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		CBuffOnAttributes* pBuff = it->second;
		if (pBuff)
		{
			pBuff->Initialize();
		}
	}
}

void CHARACTER::BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue)
{
	TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(bType);

	// ? Wenn der neue Wert 0 ist → Buff komplett aus
	if (bNewValue == 0)
	{
		if (it != m_map_buff_on_attrs.end() && it->second)
		{
			it->second->Off();
		}
		return;
	}

	// ? Neuer Buff, alter war 0 → Buff-Objekt erstellen oder holen
	if (bOldValue == 0)
	{
		CBuffOnAttributes* pBuff = NULL;

		if (it == m_map_buff_on_attrs.end())
		{
			switch (bType)
			{
				case POINT_ENERGY:
				{
					static BYTE abSlot[] = { WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_SHIELD };
					static std::vector<BYTE> vec_slots(abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;

				case POINT_COSTUME_ATTR_BONUS:
				{
					static BYTE abSlot[] = {
						WEAR_COSTUME_BODY,
						WEAR_COSTUME_HAIR,
						#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
						WEAR_COSTUME_MOUNT,
						#endif
						#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
						WEAR_COSTUME_WEAPON,
						#endif
					};
					static std::vector<BYTE> vec_slots(abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;

				default:
					break;
			}

			// falls pBuff erstellt wurde, in map eintragen
			if (pBuff)
				m_map_buff_on_attrs.emplace(bType, pBuff);
		}
		else
		{
			pBuff = it->second;
		}

		if (pBuff)
			pBuff->On(bNewValue);

		return;
	}

	// ? Alter und neuer Wert existieren → Buff-Wert andern
	//    → Hier MUSS der Crash-Fix hin!
	if (it != m_map_buff_on_attrs.end() && it->second)
	{
		it->second->ChangeBuffValue(bNewValue);
	}
	// Wenn kein Buff existiert → sicher ignorieren
}


LPITEM CHARACTER::FindSpecifyItem(DWORD vnum) const
{
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
			return GetInventoryItem(i);

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
		{
			return GetInventoryItem(i);
		}
#endif

	return NULL;
}

LPITEM CHARACTER::FindItemByID(DWORD id) const
{
	for (int i=0 ; i < INVENTORY_MAX_NUM ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	for (int i=BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
		{
			return GetInventoryItem(i);
		}
	}
#endif

	return NULL;
}

int CHARACTER::CountSpecifyItem(DWORD vnum) const
{
	int	count = 0;
	LPITEM item;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		item = GetInventoryItem(i);
		if (item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}
#endif

	return count;
}

void CHARACTER::RemoveSpecifyItem(DWORD vnum, DWORD count)
{
	if (0 == count)
		return;

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (vnum >= 80003 && vnum <= 80007)
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}


#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		LPITEM item = GetInventoryItem(i);
		if (item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}

			if (vnum >= 80003 && vnum <= 80007)
			{
				LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");
			}

			if (count >= GetInventoryItem(i)->GetCount())
			{
				count -= GetInventoryItem(i)->GetCount();
				GetInventoryItem(i)->SetCount(0);

				if (0 == count)
				{
					return;
				}
			}
			else
			{
				GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
				return;
			}
		}
	}
#endif

	if (count)
		sys_log(0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %u, still remain %d", vnum, count);
}

int CHARACTER::CountSpecifyTypeItem(BYTE type) const
{
	int	count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}
#endif

	return count;
}

void CHARACTER::RemoveSpecifyTypeItem(BYTE type, DWORD count)
{
	if (0 == count)
		return;

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetType() != type)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}


#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		if (NULL == GetInventoryItem(i))
		{
			continue;
		}

		if (GetInventoryItem(i)->GetType() != type)
		{
			continue;
		}

		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
			{
				continue;
			}
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
			{
				return;
			}
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
#endif
}

void CHARACTER::AutoGiveItem(LPITEM item, bool longOwnerShip)
{
	if (NULL == item)
	{
		sys_err ("NULL point.");
		return;
	}
	if (item->GetOwner())
	{
		sys_err ("item %d 's owner exists!",item->GetID());
		return;
	}

	int cell;
	if (item->IsDragonSoul())
	{
		cell = GetEmptyDragonSoulInventory(item);
	}
	else
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		cell = GetEmptyInventory(item);
#else
		cell = GetEmptyInventory(item->GetSize());
#endif
	}

	if (cell != -1)
	{
		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell));
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));

		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = cell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround (GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (longOwnerShip)
			item->SetOwnership (this, 300);
		else
			item->SetOwnership (this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}
}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, short bCount, int iRarePct, bool bMsg)
#else
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, BYTE bCount, int iRarePct, bool bMsg)
#endif
{
	TItemTable * p = ITEM_MANAGER::instance().GetTable(dwItemVnum);

	if (!p)
		return NULL;

	DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, dwItemVnum, bCount);

	if (p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND)
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = GetInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
				short bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				auto bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("아이템 획득: %s"), item->GetClientName());

					return item;
				}
			}
		}
#ifdef ENABLE_SPECIAL_INVENTORY
		for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetInventoryItem(i);

			if (!item)
			{
				continue;
			}

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
					{
						bCount = p->alValues[1];
					}
				}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
				short bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				auto bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;
				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
					{
						ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("아이템 획득: %s"), item->GetClientName());
					}

					return item;
				}
			}
		}
#endif
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(dwItemVnum, bCount, 0, true);

	if (!item)
	{
		sys_err("cannot create item by vnum %u (name: %s)", dwItemVnum, GetName());
		return NULL;
	}

	if (item->GetType() == ITEM_BLEND)
	{
		for (int i=0; i < INVENTORY_MAX_NUM; i++)
		{
			LPITEM inv_item = GetInventoryItem(i);

			if (inv_item == NULL) continue;

			if (inv_item->GetType() == ITEM_BLEND)
			{
				if (inv_item->GetVnum() == item->GetVnum())
				{
					if (inv_item->GetSocket(0) == item->GetSocket(0) &&
							inv_item->GetSocket(1) == item->GetSocket(1) &&
							inv_item->GetSocket(2) == item->GetSocket(2) &&
							inv_item->GetCount() < g_bItemCountLimit)
					{
						inv_item->SetCount(inv_item->GetCount() + item->GetCount());
						return inv_item;
					}
				}
			}
		}
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		iEmptyCell = GetEmptyDragonSoulInventory(item);
	}
	else
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		iEmptyCell = GetEmptyInventory(item);
#else
		iEmptyCell = GetEmptyInventory(item->GetSize());
#endif
	}

	if (iEmptyCell != -1)
	{
		if (bMsg)
			ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("아이템 획득: %s"), item->GetClientName());

		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = iEmptyCell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround(GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP))
			item->SetOwnership(this, 300);
		else
			item->SetOwnership(this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}

	sys_log(0,
		"7: %d %d", dwItemVnum, bCount);
	return item;
}

bool CHARACTER::GiveItem(LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
		return false;

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot take this item if you're using quests"));
		return false;
	}
	// @fixme150 END

	LPITEM item = GetItem(Cell);

	if (item && !item->IsExchanging())
	{
		if (victim->CanReceiveItem(this, item))
		{
			victim->ReceiveItem(this, item);
			return true;
		}
	}

	return false;
}

bool CHARACTER::CanReceiveItem(LPCHARACTER from, LPITEM item) const
{
	if (IsPC())
		return false;

	// TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX
	if (DISTANCE_APPROX(GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
		return false;
	// END_OF_TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH &&
					(item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				return true;
			break;

		case fishing::FISHER_MOB:
			if (item->GetType() == ITEM_ROD)
				return true;
			break;

			// BUILDING_NPC
		case BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
			if (item->GetType() == ITEM_WEAPON &&
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
			if (item->GetType() == ITEM_ARMOR &&
					(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_ACCESSORY_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetType() == ITEM_ARMOR &&
					!(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;
			// END_OF_BUILDING_NPC

		case BLACKSMITH_MOB:
			if (item->GetRefinedVnum() && item->GetRefineSet() < 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case BLACKSMITH2_MOB:
			if (item->GetRefineSet() >= 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case ALCHEMIST_MOB:
			if (item->GetRefinedVnum())
				return true;
			break;

		case 20101:
		case 20102:
		case 20103:

			if (item->GetVnum() == ITEM_REVIVE_HORSE_1)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽지 않은 말에게 선초를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽은 말에게 사료를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20104:
		case 20105:
		case 20106:

			if (item->GetVnum() == ITEM_REVIVE_HORSE_2)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽지 않은 말에게 선초를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽은 말에게 사료를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20107:
		case 20108:
		case 20109:

			if (item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽지 않은 말에게 선초를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽은 말에게 사료를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				return false;
			}
			break;
	}

	//if (IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_GIVE))
	{
		return true;
	}

	return false;
}

void CHARACTER::ReceiveItem(LPCHARACTER from, LPITEM item)
{
	if (IsPC())
		return;

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH && (item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				fishing::Grill(from, item);
			else
			{
				// TAKE_ITEM_BUG_FIX
				from->SetQuestNPCID(GetVID());
				// END_OF_TAKE_ITEM_BUG_FIX
				quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			}
			break;

			// DEVILTOWER_NPC
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum() != 0 && item->GetRefineSet() != 0 && item->GetRefineSet() < 500)
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_MONEY_ONLY);
			}
			else
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
			}
			break;
			// END_OF_DEVILTOWER_NPC

		case BLACKSMITH_MOB:
		case BLACKSMITH2_MOB:
		case BLACKSMITH_WEAPON_MOB:
		case BLACKSMITH_ARMOR_MOB:
		case BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum())
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_NORMAL);
			}
			else
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
			}
			break;

		case 20101:
		case 20102:
		case 20103:
		case 20104:
		case 20105:
		case 20106:
		case 20107:
		case 20108:
		case 20109:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_2 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				from->ReviveHorse();
				item->SetCount(item->GetCount()-1);
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말에게 선초를 주었습니다."));
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 ||
					item->GetVnum() == ITEM_HORSE_FOOD_2 ||
					item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				from->FeedHorse();
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말에게 사료를 주었습니다."));
				item->SetCount(item->GetCount()-1);
				EffectPacket(SE_HPUP_RED);
			}
			break;

		default:
			sys_log(0, "TakeItem %s %d %s", from->GetName(), GetRaceNum(), item->GetName());
			from->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			break;
	}
}

bool CHARACTER::IsEquipUniqueItem(DWORD dwItemVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	if (dwItemVnum == UNIQUE_ITEM_RING_OF_LANGUAGE)
		return IsEquipUniqueItem(UNIQUE_ITEM_RING_OF_LANGUAGE_SAMPLE);

	return false;
}

// CHECK_UNIQUE_GROUP
bool CHARACTER::IsEquipUniqueGroup(DWORD dwGroupVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

	return false;
}
// END_OF_CHECK_UNIQUE_GROUP

void CHARACTER::SetRefineMode(int iAdditionalCell)
{
	m_iRefineAdditionalCell = iAdditionalCell;
	m_bUnderRefine = true;
}

void CHARACTER::ClearRefineMode()
{
	m_bUnderRefine = false;
	SetRefineNPC( NULL );
}

bool CHARACTER::GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD> &dwItemVnums,
											std::vector<DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);

	if (!pGroup)
	{
		sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex(idxes);

	bool bSuccess;

	for (int i = 0; i < n; i++)
	{
		bSuccess = false;
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum(idx);
#ifdef ENABLE_GOLD_MAX_EXTENDED
		int64_t dwCount = (int64_t)pGroup->GetCount(idx);
#else
		DWORD dwCount = pGroup->GetCount(idx);
#endif
		int	iRarePct = pGroup->GetRarePct(idx);
		LPITEM item_get = NULL;
		switch (dwVnum)
		{
			case CSpecialItemGroup::GOLD:
#ifdef ENABLE_GOLD_MAX_EXTENDED
				ChangeGold(dwCount);
#else
				PointChange(POINT_GOLD, dwCount);
#endif
				LogManager::instance().CharLog(this, dwCount, "TREASURE_GOLD", "");

				bSuccess = true;
				break;
			case CSpecialItemGroup::EXP:
				{
					PointChange(POINT_EXP, dwCount);
					LogManager::instance().CharLog(this, dwCount, "TREASURE_EXP", "");

					bSuccess = true;
				}
				break;

			case CSpecialItemGroup::MOB:
				{
					sys_log(0, "CSpecialItemGroup::MOB %d", dwCount);
					int x = GetX() + number(-500, 500);
					int y = GetY() + number(-500, 500);

					LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(dwCount, GetMapIndex(), x, y, 0, true, -1);
					if (ch)
						ch->SetAggressive();
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::SLOW:
				{
					sys_log(0, "CSpecialItemGroup::SLOW %d", -(int)dwCount);
					AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)dwCount, AFF_SLOW, 300, 0, true);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::DRAIN_HP:
				{
					int iDropHP = GetMaxHP()*dwCount/100;
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					iDropHP = MIN(iDropHP, GetHP()-1);
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					PointChange(POINT_HP, -iDropHP);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::POISON:
				{
					AttackedByPoison(NULL);
					bSuccess = true;
				}
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case CSpecialItemGroup::BLEEDING:
				{
					AttackedByBleeding(NULL);
					bSuccess = true;
				}
				break;
#endif
			case CSpecialItemGroup::MOB_GROUP:
				{
					int sx = GetX() - number(300, 500);
					int sy = GetY() - number(300, 500);
					int ex = GetX() + number(300, 500);
					int ey = GetY() + number(300, 500);
					CHARACTER_MANAGER::instance().SpawnGroup(dwCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);

					bSuccess = true;
				}
				break;
			default:
				{
					item_get = AutoGiveItem(dwVnum, dwCount, iRarePct);

					if (item_get)
					{
						bSuccess = true;
					}
				}
				break;
		}

		if (bSuccess)
		{
			dwItemVnums.emplace_back(dwVnum);
			dwItemCounts.emplace_back(dwCount);
			item_gets.emplace_back(item_get);
			count++;

		}
		else
		{
			return false;
		}
	}
	return bSuccess;
}

// NEW_HAIR_STYLE_ADD
bool CHARACTER::ItemProcess_Hair(LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel(GetLevel()) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 이 머리를 사용할 수 없는 레벨입니다."));
		return false;
	}

	DWORD hair = item->GetVnum();

	switch (GetJob())
	{
		case JOB_WARRIOR :
			hair -= 72000;
			break;

		case JOB_ASSASSIN :
			hair -= 71250;
			break;

		case JOB_SURA :
			hair -= 70500;
			break;

		case JOB_SHAMAN :
			hair -= 69750;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case JOB_WOLFMAN:
			break;
#endif
		default :
			return false;
			break;
	}

	if (hair == GetPart(PART_HAIR))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("동일한 머리 스타일로는 교체할 수 없습니다."));
		return true;
	}

	item->SetCount(item->GetCount() - 1);

	SetPart(PART_HAIR, hair);
	UpdatePacket();

	return true;
}
// END_NEW_HAIR_STYLE_ADD

bool CHARACTER::ItemProcess_Polymorph(LPITEM item)
{
	if (IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 둔갑중인 상태입니다."));
		return false;
	}

	if (true == IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑할 수 없는 상태입니다."));
		return false;
	}

	if (CPolymorphUtils::Instance().IsOnPolymorphMapBlacklist(GetMapIndex()))
	{
		ChatPacket(CHAT_TYPE_INFO, "You cannot polymorph in this map.");
		return false;
	}

	DWORD dwVnum = item->GetSocket(0);

	if (dwVnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("잘못된 둔갑 아이템입니다."));
		item->SetCount(item->GetCount()-1);
		return false;
	}

	const CMob* pMob = CMobManager::instance().Get(dwVnum);

	if (pMob == NULL)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("잘못된 둔갑 아이템입니다."));
		item->SetCount(item->GetCount()-1);
		return false;
	}

	switch (item->GetVnum())
	{
		case 70104 :
		case 70105 :
		case 70106 :
		case 70107 :
		case 71093 :
			{
				sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);

				int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
				if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("나보다 너무 높은 레벨의 몬스터로는 변신 할 수 없습니다."));
					return false;
				}

				int iDuration = GetSkillLevel(POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel(POLYMORPH_SKILL_ID)/40 * 25));
				iDuration *= 60;

				DWORD dwBonus = 0;

				dwBonus = (2 + GetSkillLevel(POLYMORPH_SKILL_ID)/40) * 100;

				AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
				AddAffect(AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);

				item->SetCount(item->GetCount()-1);
			}
			break;

		case 50322:
			{
				sys_log(0, "USE_POLYMORPH_BOOK: %s(%u) vnum(%u)", GetName(), GetPlayerID(), dwVnum);

				if (CPolymorphUtils::instance().PolymorphCharacter(this, item, pMob) == true)
				{
					CPolymorphUtils::instance().UpdateBookPracticeGrade(this, item);
				}
				else
				{
				}
			}
			break;

		default :
			sys_err("POLYMORPH invalid item passed PID(%d) vnum(%d)", GetPlayerID(), item->GetOriginalVnum());
			return false;
	}

	return true;
}

bool CHARACTER::CanDoCube() const
{
	if (m_bIsObserver)	return false;
	if (GetShop())		return false;
	if (GetMyShop())	return false;
	if (m_bUnderRefine)	return false;
	if (IsWarping())	return false;
#ifdef __PREMIUM_PRIVATE_SHOP__
	if (IsEditingPrivateShop() || IsShopSearch() || GetMyPrivateShop()) return false;
#endif

	return true;
}

bool CHARACTER::UnEquipSpecialRideUniqueItem()
{
	LPITEM Unique1 = GetWear(WEAR_UNIQUE1);
	LPITEM Unique2 = GetWear(WEAR_UNIQUE2);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	LPITEM MountCostume = GetWear(WEAR_COSTUME_MOUNT);
#endif

	if( NULL != Unique1 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup() )
		{
			return UnequipItem(Unique1);
		}
	}

	if( NULL != Unique2 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup() )
		{
			return UnequipItem(Unique2);
		}
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (MountCostume)
		return UnequipItem(MountCostume);
#endif

	return true;
}

void CHARACTER::AutoRecoveryItemProcess(const EAffectTypes type)
{
	if (true == IsDead() || true == IsStun())
		return;

	if (false == IsPC())
		return;

	if (AFFECT_AUTO_HP_RECOVERY != type && AFFECT_AUTO_SP_RECOVERY != type)
		return;

	if (NULL != FindAffect(AFFECT_STUN))
		return;

	{
		const DWORD stunSkills[] = { SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };

		for (size_t i=0 ; i < sizeof(stunSkills)/sizeof(DWORD) ; ++i)
		{
			const CAffect* p = FindAffect(stunSkills[i]);

			if (NULL != p && AFF_STUN == p->dwFlag)
				return;
		}
	}

	const CAffect* pAffect = FindAffect(type);
	const size_t idx_of_amount_of_used = 1;
	const size_t idx_of_amount_of_full = 2;

	if (NULL != pAffect)
	{
		LPITEM pItem = FindItemByID(pAffect->dwFlag);

		if (NULL != pItem && true == pItem->GetSocket(0))
		{
			if (!CArenaManager::instance().IsArenaMap(GetMapIndex())
#ifdef ENABLE_NEWSTUFF
				&& !(g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(pItem->GetVnum()))
#endif
			)
			{
				const long amount_of_used = pItem->GetSocket(idx_of_amount_of_used);
				const long amount_of_full = pItem->GetSocket(idx_of_amount_of_full);

				const int32_t avail = amount_of_full - amount_of_used;

				int32_t amount = 0;

				if (AFFECT_AUTO_HP_RECOVERY == type)
				{
					amount = GetMaxHP() - (GetHP() + GetPoint(POINT_HP_RECOVERY));
				}
				else if (AFFECT_AUTO_SP_RECOVERY == type)
				{
					amount = GetMaxSP() - (GetSP() + GetPoint(POINT_SP_RECOVERY));
				}

				if (amount > 0)
				{
					if (avail > amount)
					{
						const int pct_of_used = amount_of_used * 100 / amount_of_full;
						const int pct_of_will_used = (amount_of_used + amount) * 100 / amount_of_full;

						bool bLog = false;

						if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
							bLog = true;

						if (pItem->GetVnum() != ITEM_AUTO_HP_RECOVERY_X && pItem->GetVnum() != ITEM_AUTO_SP_RECOVERY_X)
							pItem->SetSocket(idx_of_amount_of_used, amount_of_used + amount, bLog);
					}
					// else
					else if (pItem->GetVnum() != ITEM_AUTO_HP_RECOVERY_X && pItem->GetVnum() != ITEM_AUTO_SP_RECOVERY_X)
					{
						amount = avail;

						ITEM_MANAGER::instance().RemoveItem( pItem );
					}

					if (AFFECT_AUTO_HP_RECOVERY == type)
					{
						PointChange( POINT_HP_RECOVERY, amount );
						EffectPacket( SE_AUTO_HPUP );
					}
					else if (AFFECT_AUTO_SP_RECOVERY == type)
					{
						PointChange( POINT_SP_RECOVERY, amount );
						EffectPacket( SE_AUTO_SPUP );
					}
				}
			}
			else
			{
				pItem->Lock(false);
				pItem->SetSocket(0, false);
				RemoveAffect( const_cast<CAffect*>(pAffect) );
			}
		}
		else
		{
			RemoveAffect( const_cast<CAffect*>(pAffect) );
		}
	}
}

bool CHARACTER::IsValidItemPosition(TItemPos Pos) const
{
	BYTE window_type = Pos.window_type;
	WORD cell = Pos.cell;

	switch (window_type)
	{
	case RESERVED_WINDOW:
		return false;

	case INVENTORY:
	case EQUIPMENT:
		return cell < (INVENTORY_AND_EQUIP_SLOT_MAX);

	case DRAGON_SOUL_INVENTORY:
		return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

	case SAFEBOX:
		if (NULL != m_pkSafebox)
			return m_pkSafebox->IsValidPosition(cell);
		else
			return false;

	case MALL:
		if (NULL != m_pkMall)
			return m_pkMall->IsValidPosition(cell);
		else
			return false;

#ifdef FAST_EQUIP_WORLDARD
	case CHANGE_EQUIP:
		return cell < CHANGE_EQUIP_SLOT_COUNT;
#endif

#ifdef ENABLE_SWITCHBOT_SYSTEM
	case SWITCHBOT:
		return cell < SWITCHBOT_SLOT_COUNT;
#endif
	default:
		return false;
	}
}

#define VERIFY_MSG(exp, msg)  \
	if (true == (exp)) { \
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT(msg)); \
			return false; \
	}

bool CHARACTER::CanEquipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	const TItemTable* itemTable = item->GetProto();
	//BYTE itemType = item->GetType();
	//BYTE itemSubType = item->GetSubType();

	if (GetQuestFlag("duel_restricted") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, "You can't do that while you're in a duel.");
		return false;
	}

	switch (GetJob())
	{
		case JOB_WARRIOR:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case JOB_WOLFMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
				return false;
			break;
#endif
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limit = itemTable->aLimits[i].lValue;
		switch (itemTable->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("레벨이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;

			case LIMIT_STR:
				if (GetPoint(POINT_ST) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("근력이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;

			case LIMIT_INT:
				if (GetPoint(POINT_IQ) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("지능이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;

			case LIMIT_DEX:
				if (GetPoint(POINT_DX) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("민첩이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;

			case LIMIT_CON:
				if (GetPoint(POINT_HT) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("체력이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;
		}
	}

	if (item->GetWearFlag() & WEARABLE_UNIQUE)
	{
		if ((GetWear(WEAR_UNIQUE1) && GetWear(WEAR_UNIQUE1)->IsSameSpecialGroup(item)) ||
			(GetWear(WEAR_UNIQUE2) && GetWear(WEAR_UNIQUE2)->IsSameSpecialGroup(item)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("같은 종류의 유니크 아이템 두 개를 동시에 장착할 수 없습니다."));
			return false;
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem(item->GetVnum()) &&
			!marriage::CManager::instance().IsMarried(GetPlayerID()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("결혼하지 않은 상태에서 예물을 착용할 수 없습니다."));
			return false;
		}

	}

	return true;
}

bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	if (ITEM_BELT == item->GetType())
		VERIFY_MSG(CBeltInventoryHelper::IsExistItemInBeltInventory(this), "벨트 인벤토리에 아이템이 존재하면 해제할 수 없습니다.");

	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	if (GetQuestFlag("duel_restricted") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, "You can't do that while you're in a duel.");
		return false;
	}

	{
		int pos = -1;

		if (item->IsDragonSoul())
			pos = GetEmptyDragonSoulInventory(item);
		else
#ifdef ENABLE_SPECIAL_INVENTORY
			pos = GetEmptyInventory(item);
#else
			pos = GetEmptyInventory(item->GetSize());
#endif

		VERIFY_MSG( -1 == pos, "소지품에 빈 공간이 없습니다." );
	}

	if (item->GetType() == ITEM_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}

	return true;
}


#ifdef ENABLE_GEM_SYSTEM
void CHARACTER::GiveGem(int iAmount)
{
	if (iAmount <= 0)
	{
		return;
	}

	PointChange(POINT_GEM, iAmount, true);
}
#endif


#ifdef ENABLE_EXTENDED_BLEND
int CHARACTER::GetAffectType(LPITEM item)
{
	switch (item->GetValue(2))
	{
	case 1:
		return AFFECT_BLEND_EX;
	case 2:
		return AFFECT_WATER;
	case 3:
		return AFFECT_MALL_EX;
	default:
		return AFFECT_BLEND;
	}
}
#endif

#ifdef RENEWAL_PICKUP_AFFECT
bool CHARACTER::CanPickupDirectly()
{
	return FindAffect(AFFECT_PICKUP_ENABLE) != NULL;
}
#endif

// bool CHARACTER::OpenChest(LPITEM item, WORD openCount)
// {
// 	if (openCount > item->GetCount())
// 		openCount = item->GetCount();

// 	if (openCount > 1000)
// 		openCount = 1000;

// 	std::map<DWORD, DWORD> dwItemVnums;
// 	const WORD openCountCache = openCount;

// 	item->Lock(true);
// 	if (GiveItemFromSpecialItemGroupNew(item->GetVnum(), openCount))
// 	{
// 		if (openCount != 0)
// 		{
// 			/*ADD RANKING / BATTLEPASS STUFF HERE*/
// 			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Chest opening stopped. Opened count: %d"), (openCountCache - openCount));
// 			item->Lock(false);
// 			item->SetCount(item->GetCount() - (openCountCache - openCount));
// 		}
// 		else
// 		{
// 			/*ADD RANKING / BATTLEPASS STUFF HERE*/
// 			item->Lock(false);
// 			item->SetCount(item->GetCount() - openCountCache);
// 		}
// 		return true;
// 	}

// 	item->Lock(false);

// 	return false;
// }

// bool CHARACTER::AutoGiveItemChest(LPITEM item, DWORD& itemCount, std::map<LPITEM, WORD>& vecUpdateItems)
// {
// 	int iEmptyCell = -1;
// 	if (item->IsDragonSoul())
// 		iEmptyCell = GetEmptyDragonSoulInventory(item);
// 	else
// #ifdef ENABLE_SPECIAL_INVENTORY
// 		iEmptyCell = GetEmptyInventory(item);
// #else
// 		iEmptyCell = GetEmptyInventory(item->GetSize());
// #endif

// 	if (iEmptyCell == -1)
// 	{
// 		ChatPacket(CHAT_TYPE_INFO, "You don't have enough space in your inventory.");
// 		return false;
// 	}

// 	const DWORD itemVnum = item->GetVnum();
// 	WORD wCount = item->GetCount();

// 	if (item->IsStackable() && item->GetType() != ITEM_BLEND)
// 	{
// 		for (auto & vecUpdateItem : vecUpdateItems)
// 		{
// 			LPITEM item2 = vecUpdateItem.first;
// 			if (itemVnum == item2->GetVnum())
// 			{
// 				const DWORD item2Count = item2->GetCount();
// 				const WORD bCount2 = MIN(g_bItemCountLimit - item2Count, wCount);
// 				if (bCount2 > 0)
// 				{
// 					wCount -= bCount2;
// 					item2->SetCount(item2Count + bCount2);
// 					if (wCount == 0)
// 					{
// 						itemCount = 0;
// 						return true;
// 					}
// 				}
// 			}
// 		}

// 		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
// 		{
// 			LPITEM item2 = GetInventoryItem(i);
// 			if (!item2)
// 				continue;

// 			if (item2->GetVnum() == itemVnum)
// 			{
// 				BYTE j;
// 				for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
// 					if (item2->GetSocket(j) != item->GetSocket(j))
// 						break;
// 				if (j != ITEM_SOCKET_MAX_NUM)
// 					continue;
// 				const DWORD item2Count = item2->GetCount();
// 				const WORD bCount2 = MIN(g_bItemCountLimit - item2Count, wCount);
// 				if (bCount2 > 0)
// 				{
// 					const auto itItem = vecUpdateItems.find(item2);
// 					if (itItem == vecUpdateItems.end())
// 						vecUpdateItems.emplace(item2, item2Count);
// 					wCount -= bCount2;
// 					item2->SetUpdateStatus(true);
// 					item2->SetCount(item2Count + bCount2);
// 					if (wCount == 0)
// 					{
// 						itemCount = 0;
// 						return true;
// 					}
// 				}
// 			}
// 		}

// #ifdef ENABLE_SPECIAL_INVENTORY
// 		for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
// 		{
// 			LPITEM item2 = GetInventoryItem(i);
// 			if (!item2)
// 				continue;

// 			if (item2->GetVnum() == itemVnum)
// 			{
// 				BYTE j;
// 				for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
// 					if (item2->GetSocket(j) != item->GetSocket(j))
// 						break;

// 				if (j != ITEM_SOCKET_MAX_NUM)
// 					continue;

// 				const DWORD item2Count = item2->GetCount();
// 				const WORD bCount2 = MIN(g_bItemCountLimit - item2Count, wCount);
// 				if (bCount2 > 0)
// 				{
// 					const auto itItem = vecUpdateItems.find(item2);
// 					if (itItem == vecUpdateItems.end())
// 						vecUpdateItems.emplace(item2, item2Count);
// 					wCount -= bCount2;
// 					item2->SetUpdateStatus(true);
// 					item2->SetCount(item2Count + bCount2);
// 					if (wCount == 0)
// 					{
// 						itemCount = 0;
// 						return true;
// 					}
// 				}
// 			}
// 		}
// #endif
// 	}

// 	if (wCount > 0)
// 	{
// 		item->SetCount(wCount);

// 		if (item->IsDragonSoul())
// 			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
// 		else
// 			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));

// 		if (item->IsStackable())
// 		{
// 			item->SetUpdateStatus(true);
// 			const auto itItem = vecUpdateItems.find(item);
// 			if (itItem == vecUpdateItems.end())
// 				vecUpdateItems.emplace(item, wCount);
// 		}
// 		itemCount = wCount;
// 		return true;
// 	}

// 	return false;
// }

// bool CHARACTER::GiveItemFromSpecialItemGroupNew(DWORD dwGroupNum, WORD& loopCount)
// {
// 	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);
// 	if (!pGroup)
// 		return false;

// 	std::map<DWORD, WORD> vecItemCounts;
// 	std::map<LPITEM, WORD> vecUpdateItems;
// 	const WORD constCount = loopCount;
// 	WORD workCount = loopCount;

// 	int returnCount = 5;

// 	for (DWORD j = 0; j < constCount; ++j)
// 	{
// 		if (returnCount <= 0 || workCount <= 0)
// 			break;
// 		std::vector <int> idxes;
// 		const int n = pGroup->GetMultiIndex(idxes);

// 		bool isOkey = false;

// 		for (int i = 0; i < n; i++)
// 		{
// 			const int idx = idxes[i];
// 			const DWORD dwVnum = pGroup->GetVnum(idx);
// 			DWORD dwCount = pGroup->GetCount(idx);

// 			switch (dwVnum)
// 			{
// 			case CSpecialItemGroup::GOLD:
// #ifdef ENABLE_GOLD_MAX_EXTENDED
// 				ChangeGold(dwCount);
// #else
// 				PointChange(POINT_GOLD, dwCount);
// #endif
// 				if (isOkey == false)
// 				{
// 					isOkey = true;
// 					workCount -= 1;
// 				}
// 				break;
// 			case CSpecialItemGroup::POISON:
// 				AttackedByPoison(nullptr);
// 				if (isOkey == false)
// 				{
// 					isOkey = true;
// 					workCount -= 1;
// 				}
// 				break;
// 			case CSpecialItemGroup::EXP:
// 				PointChange(POINT_EXP, dwCount);
// 				if (isOkey == false)
// 				{
// 					isOkey = true;
// 					workCount -= 1;
// 				}
// 				break;
// 			case CSpecialItemGroup::MOB:
// 			{
// 				LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(dwCount, GetMapIndex(), GetX() + number(-500, 500), GetY() + number(-500, 500), 0, true, -1);
// 				if (ch)
// 					ch->SetAggressive();

// 				if (isOkey == false)
// 				{
// 					isOkey = true;
// 					workCount -= 1;
// 				}
// 			}
// 			break;
// 			case CSpecialItemGroup::MOB_GROUP:
// 				CHARACTER_MANAGER::instance().SpawnGroup(dwCount, GetMapIndex(), GetX() - number(300, 500), GetY() - number(300, 500), GetX() + number(300, 500), GetY() + number(300, 500), nullptr, true);
// 				if (isOkey == false)
// 				{
// 					isOkey = true;
// 					workCount -= 1;
// 				}
// 				break;
// 			case CSpecialItemGroup::SLOW:
// 				AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)dwCount, AFF_SLOW, 300, 0, true);
// 				if (isOkey == false)
// 				{
// 					isOkey = true;
// 					workCount -= 1;
// 				}
// 				break;
// 			case CSpecialItemGroup::DRAIN_HP:
// 			{
// 				int iDropHP = GetMaxHP() * dwCount / 100;
// 				iDropHP = MIN(iDropHP, GetHP() - 1);
// 				PointChange(POINT_HP, -iDropHP);
// 				if (isOkey == false)
// 				{
// 					isOkey = true;
// 					workCount -= 1;
// 				}
// 			}
// 			break;
// 			default:
// 			{
// 				LPITEM item = ITEM_MANAGER::Instance().CreateItem(dwVnum, dwCount);
// 				if (item)
// 				{
// 					if (AutoGiveItemChest(item, dwCount, vecUpdateItems))
// 					{
// 						if (isOkey == false)
// 						{
// 							isOkey = true;
// 							workCount -= 1;
// 						}

// 						auto itCount = vecItemCounts.find(dwVnum);
// 						if (itCount != vecItemCounts.end())
// 							itCount->second += pGroup->GetCount(idx);
// 						else
// 							vecItemCounts.emplace(dwVnum, pGroup->GetCount(idx));

// 						if (dwCount == 0)
// 						{
// 							M2_DESTROY_ITEM(item);
// 							continue;
// 						}
// 					}
// 					else
// 					{
// 						M2_DESTROY_ITEM(item);
// 						returnCount -= 1;
// 					}
// 				}
// 			}
// 			}
// 		}
// 	}

// 	for (auto & vecUpdateItem : vecUpdateItems)
// 	{
// 		LPITEM item = vecUpdateItem.first;
// 		item->SetUpdateStatus(false);
// 		if (item->GetCount() != vecUpdateItem.second)
// 			item->UpdatePacket();

// 		if (item->GetType() == ITEM_QUEST || item->GetType() == ITEM_MATERIAL)
// 			quest::CQuestManager::instance().PickupItem(GetPlayerID(), item);
// #ifdef ENABLE_QUEST_RECEIVE_ITEM
// 		SenderRecvItem(GetPlayerID(), item);
// #endif
// 	}

// 	for (auto& vecItemCount : vecItemCounts)
// 	{
// 		const TItemTable* table = ITEM_MANAGER::instance().GetTable(vecItemCount.first);
// 		if (table != nullptr)
// 			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Receive: %s - %d"), table->szLocaleName, vecItemCount.second);
// 	}

// 	loopCount = workCount;
// 	return (workCount != constCount);
// }

#ifdef ENABLE_SPLIT_BY_COUNT
void CHARACTER::SendSplitItemByCount(const TItemPos &Cell, short sCount)
{
	if (!CanHandleItem())
		return;
	
	LPITEM pkItem = nullptr;
	if (!(pkItem = GetItem(Cell)) || (pkItem && !pkItem->IsStackable() || IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK)))
		return;
	
	if (sCount > ITEM_MAX_COUNT || sCount <= 0)
		return;

	if (sCount > pkItem->GetCount())
		sCount = pkItem->GetCount();
	
	LPITEM pkNewItem = nullptr;
	WORD pos = -1;
	
	short split_count = (pkItem->GetCount() / sCount);

#ifdef ENABLE_QUICK_OPEN
	SetInventoryBuffer(true);
#endif

	for (short i = 0; i < split_count; ++i)
	{
		WORD pos = -1;
#ifdef ENABLE_SPECIAL_INVENTORY
		pos = GetEmptyInventory(pkItem);
#else
		pos = GetEmptyInventory(pkItem->GetSize());
#endif

		if (!MoveItem(Cell, TItemPos(INVENTORY, pos), sCount))
			break;
	}

#ifdef ENABLE_QUICK_OPEN
	SendBufferedInventoryPacket();
#endif
}
#endif

#ifdef ENABLE_QUICK_OPEN
void CHARACTER::SendBufferedInventoryPacket()
{
	if (!GetDesc())
	{
		// Well, no desc
		// Just disable it
		us_buffered_items.clear();
		bInvBuff = false;
	}

	// Intializing buffer
	TEMP_BUFFER buf{1024 * 1024, false};
	TPacketGCInventoryHeader packHdr{};
	packHdr.bHeader = HEADER_GC_ITEM_BUFFERED;
	packHdr.wSize = sizeof(packHdr) + sizeof(TPacketGCItemSet) * us_buffered_items.size();

	for (const auto & pItem : us_buffered_items)
	{
		TPacketGCItemSet pack{};
		pack.header = HEADER_GC_ITEM_SET;
		pack.Cell = TItemPos(pItem->GetWindow(), pItem->GetCell());
		pack.count = pItem->GetCount();

		pack.vnum = pItem->GetVnum();
		pack.flags = pItem->GetFlag();
		pack.anti_flags = pItem->GetAntiFlag();
		pack.highlight = (pack.Cell.window_type == DRAGON_SOUL_INVENTORY);

		thecore_memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
		thecore_memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));

		buf.write(&pack, sizeof(pack));
	}

	// Sending data
	GetDesc()->BufferedPacket(&packHdr, sizeof(packHdr));
	GetDesc()->LargePacket(buf.read_peek(), buf.size());

	// Data sent, switching off buffer
	us_buffered_items.clear();
	bInvBuff = false;
}

void CHARACTER::QuickOpenStack(LPITEM item)
{
	if (item->GetType() == ITEM_GIFTBOX)
		return;

	// Setup buffering
	SetInventoryBuffer(true);

	// Keep on opening
	DWORD dwVID = item->GetVID();
	WORD wInitialCount = item->GetCount();
	// sys_err("InistialCount: %d", wInitialCount);

	while (item->GetCount())
	{
		// sys_err("ItemCount while opening: %d", item->GetCount());
		// Use, if function returns false, break
		if (!UseItemEx(item, NPOS))
		{
			break;
		}

		if (!ITEM_MANAGER::instance().FindByVID(dwVID)) // Item is missing, break
		{
			break;
		}

		if (item->GetCount() == wInitialCount) // Item is not consumed (cannot be opened), break
		{
			break;
		}
	}

#ifdef ENABLE_RANK_PLAYER
	RankPlayer::instance().SendInfoPlayer(this, RANK_BY_BOX_OPENED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_BOX_OPENED) + wInitialCount, false);
#endif
	// Release buffer
	SendBufferedInventoryPacket();
}
#endif

#ifdef ENABLE_FAST_STACK
bool CHARACTER::FastStack(TItemPos originPos)
{
	if (!IsValidItemPosition(originPos))
		return false;

	if (!CanHandleItem())
		return false;

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::FastStack, std::chrono::milliseconds(1000)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Pulse Manager] You need to wait 1 second."));
		return false;
	}
#endif

	LPITEM item = GetItem(originPos);

	if (item == nullptr)
		return false;

	if (item->GetCount() >= ITEM_MAX_COUNT)
	{
		ChatPacket(CHAT_TYPE_INFO, "Item max count reached!");
		return false;
	}

	int currentPos = originPos.cell + 1;

	// TODO
// #ifdef ENABLE_QUICK_OPEN
// 	SetInventoryBuffer(true);
// #endif

	while (item->GetCount() < ITEM_MAX_COUNT && (originPos.cell != currentPos))
	{
		LPITEM item2 = GetInventoryItem(currentPos);
		if (item2)
		{
			if (item2->GetVnum() == item->GetVnum())
			{
				if (item2->GetCount() < ITEM_MAX_COUNT)
				{
					int itemCount = item->GetCount();
					int item2Count = item2->GetCount();

					if (itemCount + item2Count > ITEM_MAX_COUNT)
					{
						int requiredCountToMax = (ITEM_MAX_COUNT - itemCount);
						
						MoveItem(TItemPos(INVENTORY, item2->GetCell()), originPos, requiredCountToMax);
					}
					else
					{
						MoveItem(TItemPos(INVENTORY, item2->GetCell()), originPos, item2->GetCount());
					}
				}
			}
		}

		currentPos++;

		if (currentPos >= ITEM_MAX_COUNT)
		{
			currentPos = 0;
		}
	}

// #ifdef ENABLE_QUICK_OPEN
// 	SendBufferedInventoryPacket();
// #endif

	return true;
}
#endif

#ifdef ENABLE_CHEST_OPEN_RENEWAL
bool CHARACTER::OpenChestItem(TItemPos pos, WORD bOpenCount)
{
	LPITEM item;
	if (!CanHandleItem())
		return false;
	
	if (!IsValidItemPosition(pos) || !(item = GetItem(pos)))
		return false;
	
	if (item->IsExchanging())
		return false;
	
	if (IsStun())
		return false;
	
	return OpenChestItem(item, bOpenCount);
}

bool CHARACTER::OpenChestItem(LPITEM item, WORD bOpenCount)
{
	if (!item)
		return false;
	
	if (!CanHandleItem())
		return false;
	
	if (item->IsExchanging())
		return false;
	
	if (IsStun())
		return false;

#ifdef ENABLE_NEWSTUFF
	if ((bOpenCount != 1) && (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::QuickOpen, std::chrono::milliseconds(1000))))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Pulse Manager] You need to wait 1 second."));
		return false;
	}
#endif

	if (quest::CQuestManager::instance().GetEventFlag("disable_giftbox") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, "boxes are disabled for now !");
		return false;
	}

	DWORD dwGroupNum = item->GetVnum();
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);
	if (!pGroup)
	{
		// sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	if ((dwGroupNum > 51500 && dwGroupNum < 52000) || (dwGroupNum >= 50255 && dwGroupNum <= 50260))
	{
		if (quest::CQuestManager::instance().GetEventFlag("disable_cor_giftbox") == 1)
		{
			ChatPacket(CHAT_TYPE_INFO, "cor boxes are disabled for now !");
			return false;
		}

		if (!(this->DragonSoul_IsQualified()))
		{
			ChatPacket(CHAT_TYPE_INFO,LC_TEXT("먼저 용혼석 퀘스트를 완료하셔야 합니다."));
			return false;
		}

		if (NULL != DragonSoul_RefineWindow_GetOpener())
		{
			ChatPacket(CHAT_TYPE_INFO,LC_TEXT("Close the refine window first!"));
			return false;
		}

		auto DragonSoulInventoryCheck = GetEmptyDragonSoulInventoryType();
        if (DragonSoulInventoryCheck != 0)
        {
            auto DragonSoulGradeTypes = DragonSoulInventoryCheck / 10;
            auto DragonSoulStepTypes = DragonSoulInventoryCheck % 10;

            if (DragonSoulStepTypes == -1)
            {
                return false;
            }

            std::string col_type = "";
            std::string row_type = "";

            if (DragonSoulGradeTypes == 0)
                col_type = "Normal";
            else if (DragonSoulGradeTypes == 1)
                col_type = "Brilliant";
            else if (DragonSoulGradeTypes == 2)
                col_type = "Rare";
            else if (DragonSoulGradeTypes == 3)
                col_type = "Ancient";
            else if (DragonSoulGradeTypes == 4)
                col_type = "Legendary";

            if (DragonSoulStepTypes == 0)
                row_type = "Lowest";
            else if (DragonSoulStepTypes == 1)
                row_type = "Low";
            else if (DragonSoulStepTypes == 2)
                row_type = "Mid";
            else if (DragonSoulStepTypes == 3)
                row_type = "High";
            else if (DragonSoulStepTypes == 4)
                row_type = "Highest";

            ChatPacket(CHAT_TYPE_INFO, "%s alchemy %s inventory is full!", col_type.c_str(), row_type.c_str());
            return false;
        }
	}

	if (item->isLocked())
	{
		return false;
	}
	
	WORD loopCount = MINMAX(1, MIN(bOpenCount, item->GetCount()), 1000);	// Number of chests you can quickly open
	if (loopCount > item->GetCount())
	{
		sys_err("loopCount > item->GetCount() %d", dwGroupNum);
		return false;
	}

#ifdef ENABLE_RANK_PLAYER
	RankPlayer::instance().SendInfoPlayer(this, RANK_BY_BOX_OPENED, RankPlayer::instance().GetProgressByPID(GetPlayerID(), RANK_BY_BOX_OPENED) + loopCount, false);
#endif
// #ifdef ENABLE_BATTLE_PASS
// 	BYTE bBattlePassId = GetBattlePassId();
// 	if (bBattlePassId)
// 	{
// 		DWORD dwItemVnum, dwUseCount;
// 		if (CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, USE_ITEM, &dwItemVnum, &dwUseCount))
// 		{
// 			if (dwItemVnum == item->GetVnum() && GetMissionProgress(USE_ITEM, bBattlePassId) < dwUseCount)
// 			{
// 				UpdateMissionProgress(USE_ITEM, bBattlePassId, loopCount, dwUseCount);
// 			}
// 		}
// 	}
// #endif

	item->Lock(true);
	if (test_server)
		ChatPacket(CHAT_TYPE_INFO, "Opening %d times of %d", loopCount, dwGroupNum);
	
	// umap of all itemVnums|Counts given at the end
	std::unordered_map<DWORD, long long> wGivenItems;
	for (auto oCount = 0; oCount < loopCount; oCount++)
	{
		std::vector<int> itemIndexes;
		int itemIndexesCount = pGroup->GetMultiIndex(itemIndexes);
		
		for (auto index = 0; index < itemIndexesCount; index++)
		{
			DWORD dwVnum = pGroup->GetVnum(itemIndexes[index]);
			DWORD dwCount = pGroup->GetCount(itemIndexes[index]);
			
			long long llItemCount = wGivenItems.count(dwVnum) > 0 ? wGivenItems[dwVnum] + dwCount : dwCount;
			wGivenItems[dwVnum] = llItemCount;
		}
	}
	
	if (wGivenItems.size() < 1)
	{
		ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("아무것도 얻을 수 없었습니다."));
		if (item)
			item->Lock(false);
		return false;
	}

	item->Lock(false);
	item->SetCount(MAX(0, item->GetCount() - loopCount));
	
	for (auto& info : wGivenItems)
	{
		DWORD dwItemVnum = info.first;
		long long llItemCount = info.second;
		
		LPITEM pItem;
		switch (dwItemVnum)
		{
			case CSpecialItemGroup::GOLD:
			{
#ifdef ENABLE_GOLD_MAX_EXTENDED
				ChangeGold(llItemCount);
#else
				PointChange(POINT_GOLD, llItemCount);
#endif
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), llItemCount);
			}
			break;
			
			case CSpecialItemGroup::EXP:
			{
				PointChange(POINT_EXP, llItemCount);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 부터 신비한 빛이 나옵니다."));
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d의 경험치를 획득했습니다."), llItemCount);
			}
			break;
			
			case CSpecialItemGroup::MOB:
			{
				int x = GetX() + number(-500, 500);
				int y = GetY() + number(-500, 500);
				
				LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(llItemCount, GetMapIndex(), x, y, 0, true, -1);
				if (ch)
					ch->SetAggressive();
				
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
			}
			break;
			
			case CSpecialItemGroup::SLOW:
			{
				sys_log(0, "CSpecialItemGroup::SLOW %d", -(int)llItemCount);
				AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)llItemCount, AFF_SLOW, 300, 0, true);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 빨간 연기를 들이마시자 움직이는 속도가 느려졌습니다!"));
			}
			break;
			
			case CSpecialItemGroup::DRAIN_HP:
			{
				int iDropHP = GetMaxHP()*llItemCount/100;
				iDropHP = MIN(iDropHP, GetHP()-1);
				PointChange(POINT_HP, -iDropHP);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자가 갑자기 폭발하였습니다! 생명력이 감소했습니다."));
			}
			break;
			
			case CSpecialItemGroup::POISON:
			{
				AttackedByPoison(NULL);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
			}
			break;
			
#ifdef ENABLE_WOLFMAN_CHARACTER
			case CSpecialItemGroup::BLEEDING:
			{
				AttackedByBleeding(NULL);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
			}
			break;
#endif
			case CSpecialItemGroup::MOB_GROUP:
			{
				int sx = GetX() - number(300, 500);
				int sy = GetY() - number(300, 500);
				int ex = GetX() + number(300, 500);
				int ey = GetY() + number(300, 500);
				CHARACTER_MANAGER::instance().SpawnGroup(llItemCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
			}
			break;
			
			default:
			{
				if (llItemCount > g_bItemCountLimit)
				{
					for(auto itemIndex = llItemCount; itemIndex > 0; itemIndex -= g_bItemCountLimit)
					{
						WORD wCount = MIN(itemIndex, g_bItemCountLimit);
						pItem = AutoGiveItem(dwItemVnum, wCount, -1, false);
					}
				}
				else if ((pItem = AutoGiveItem(dwItemVnum, llItemCount, -1, false)))
				{
					if (!pItem->IsStackable() && llItemCount > 1)
					{
						for (auto itemIndex = 0; itemIndex < (llItemCount - 1); itemIndex++)
							pItem = AutoGiveItem(dwItemVnum, 1, -1, false);
					}
				}
				

				if (pItem)
				{
					if (llItemCount > 1)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 %lld 개 나왔습니다."), pItem->GetClientName(), llItemCount);
					else
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 나왔습니다."), pItem->GetClientName());
				}
			}
			break;
		}
	}
	
	return true;
}
#endif


//martysama0134's aad276684955eb3421d3edd3e79cd0dc
