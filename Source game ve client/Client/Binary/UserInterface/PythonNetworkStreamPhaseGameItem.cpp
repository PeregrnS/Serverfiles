#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "PythonItem.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonSafeBox.h"
#include "PythonCharacterManager.h"

#include "AbstractPlayer.h"

//////////////////////////////////////////////////////////////////////////
// SafeBox

bool CPythonNetworkStream::SendSafeBoxMoneyPacket(BYTE byState, DWORD dwMoney)
{
	assert(!"CPythonNetworkStream::SendSafeBoxMoneyPacket - Don't use this function");
	return false;
}

bool CPythonNetworkStream::SendSafeBoxCheckinPacket(TItemPos InventoryPos, BYTE bySafeBoxPos
#ifdef ENABLE_CHECKINOUT_UPDATE
	, bool bAutoItemPos
#endif
)
{
	__PlayInventoryItemDropSound(InventoryPos);

	TPacketCGSafeboxCheckin kSafeboxCheckin;
	kSafeboxCheckin.bHeader = HEADER_CG_SAFEBOX_CHECKIN;
	kSafeboxCheckin.ItemPos = InventoryPos;
	kSafeboxCheckin.bSafePos = bySafeBoxPos;
#ifdef ENABLE_CHECKINOUT_UPDATE
	kSafeboxCheckin.bAutoItemPos = bAutoItemPos;
#endif
	if (!Send(kSafeboxCheckin))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendSafeBoxCheckoutPacket(BYTE bySafeBoxPos, TItemPos InventoryPos
#ifdef ENABLE_CHECKINOUT_UPDATE
	, bool bAutoItemPos
#endif
)
{
	__PlaySafeBoxItemDropSound(bySafeBoxPos);

	TPacketCGSafeboxCheckout kSafeboxCheckout;
	kSafeboxCheckout.bHeader = HEADER_CG_SAFEBOX_CHECKOUT;
	kSafeboxCheckout.bSafePos = bySafeBoxPos;
	kSafeboxCheckout.ItemPos = InventoryPos;
#ifdef ENABLE_CHECKINOUT_UPDATE
	kSafeboxCheckout.bAutoItemPos = bAutoItemPos;
#endif
	if (!Send(kSafeboxCheckout))
		return false;

	return SendSequence();
}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
bool CPythonNetworkStream::SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, short byCount)
#else
bool CPythonNetworkStream::SendSafeBoxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, BYTE byCount)
#endif
{
	__PlaySafeBoxItemDropSound(bySourcePos);

	TPacketCGItemMove kItemMove;
	kItemMove.header = HEADER_CG_SAFEBOX_ITEM_MOVE;
	kItemMove.pos = TItemPos(INVENTORY, bySourcePos);
	kItemMove.num = byCount;
	kItemMove.change_pos = TItemPos(INVENTORY, byTargetPos);
	if (!Send(kItemMove))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvSafeBoxSetPacket()
{
	TPacketGCItemSet2 kItemSet;
	if (!AutoRecv(kItemSet))
		return false;

	TItemData kItemData;
	kItemData.vnum	= kItemSet.vnum;
	kItemData.count = kItemSet.count;
	kItemData.flags = kItemSet.flags;
	kItemData.anti_flags = kItemSet.anti_flags;
	for (int isocket=0; isocket<ITEM_SOCKET_SLOT_MAX_NUM; ++isocket)
		kItemData.alSockets[isocket] = kItemSet.alSockets[isocket];
	for (int iattr=0; iattr<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iattr)
		kItemData.aAttr[iattr] = kItemSet.aAttr[iattr];

	CPythonSafeBox::Instance().SetItemData(kItemSet.Cell.cell, kItemData);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!AutoRecv(kItemDel))
		return false;

	CPythonSafeBox::Instance().DelItemData(kItemDel.pos);

	__RefreshSafeboxWindow();

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxWrongPasswordPacket()
{
	TPacketGCSafeboxWrongPassword kSafeboxWrongPassword;

	if (!AutoRecv(kSafeboxWrongPassword))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnSafeBoxError", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxSizePacket()
{
	TPacketGCSafeboxSize kSafeBoxSize;
	if (!AutoRecv(kSafeBoxSize))
		return false;

	CPythonSafeBox::Instance().OpenSafeBox(kSafeBoxSize.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenSafeboxWindow", Py_BuildValue("(i)", kSafeBoxSize.bSize));

	return true;
}

bool CPythonNetworkStream::RecvSafeBoxMoneyChangePacket()
{
	TPacketGCSafeboxMoneyChange kMoneyChange;
	if (!AutoRecv(kMoneyChange))
		return false;

	CPythonSafeBox::Instance().SetMoney(kMoneyChange.dwMoney);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafeboxMoney", Py_BuildValue("()"));

	return true;
}

// SafeBox
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mall
bool CPythonNetworkStream::SendMallCheckoutPacket(BYTE byMallPos, TItemPos InventoryPos
#ifdef ENABLE_CHECKINOUT_UPDATE
	, bool bAutoItemPos
#endif
)
{
	__PlayMallItemDropSound(byMallPos);

	TPacketCGMallCheckout kMallCheckoutPacket;
	kMallCheckoutPacket.bHeader = HEADER_CG_MALL_CHECKOUT;
	kMallCheckoutPacket.bMallPos = byMallPos;
	kMallCheckoutPacket.ItemPos = InventoryPos;
#ifdef ENABLE_CHECKINOUT_UPDATE
	kMallCheckoutPacket.bAutoItemPos = bAutoItemPos;
#endif
	if (!Send(kMallCheckoutPacket))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvMallOpenPacket()
{
	TPacketGCMallOpen kMallOpen;
	if (!AutoRecv(kMallOpen))
		return false;

	CPythonSafeBox::Instance().OpenMall(kMallOpen.bSize);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenMallWindow", Py_BuildValue("(i)", kMallOpen.bSize));

	return true;
}
bool CPythonNetworkStream::RecvMallItemSetPacket()
{
	TPacketGCItemSet2 kItemSet;
	if (!AutoRecv(kItemSet))
		return false;

	TItemData kItemData;
	kItemData.vnum = kItemSet.vnum;
	kItemData.count = kItemSet.count;
	kItemData.flags = kItemSet.flags;
	kItemData.anti_flags = kItemSet.anti_flags;
	for (int isocket=0; isocket<ITEM_SOCKET_SLOT_MAX_NUM; ++isocket)
		kItemData.alSockets[isocket] = kItemSet.alSockets[isocket];
	for (int iattr=0; iattr<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++iattr)
		kItemData.aAttr[iattr] = kItemSet.aAttr[iattr];

	CPythonSafeBox::Instance().SetMallItemData(kItemSet.Cell.cell, kItemData);

	__RefreshMallWindow();

	return true;
}
bool CPythonNetworkStream::RecvMallItemDelPacket()
{
	TPacketGCItemDel kItemDel;
	if (!AutoRecv(kItemDel))
		return false;

	CPythonSafeBox::Instance().DelMallItemData(kItemDel.pos);

	__RefreshMallWindow();
	Tracef(" >> CPythonNetworkStream::RecvMallItemDelPacket\n");

	return true;
}
// Mall
//////////////////////////////////////////////////////////////////////////

// Item
// Recieve
bool CPythonNetworkStream::RecvItemSetPacket()
{
	TPacketGCItemSet packet_item_set;

	if (!AutoRecv(packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.vnum	= packet_item_set.vnum;
	kItemData.count	= packet_item_set.count;
	kItemData.flags = 0;
	for (int i=0; i<ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		kItemData.alSockets[i]=packet_item_set.alSockets[i];
	for (int j=0; j<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		kItemData.aAttr[j]=packet_item_set.aAttr[j];

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();

	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);

#ifdef ENABLE_SWITCHBOT_SYSTEM
	if (packet_item_set.Cell.window_type == SWITCHBOT)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotWindow", Py_BuildValue("()"));
		return true;
	}
#endif

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemSetPacket2()
{
	TPacketGCItemSet2 packet_item_set;

	if (!AutoRecv(packet_item_set))
		return false;

	TItemData kItemData;
	kItemData.vnum	= packet_item_set.vnum;
	kItemData.count	= packet_item_set.count;
	kItemData.flags = packet_item_set.flags;
	kItemData.anti_flags = packet_item_set.anti_flags;

	for (int i=0; i<ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		kItemData.alSockets[i]=packet_item_set.alSockets[i];
	for (int j=0; j<ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		kItemData.aAttr[j]=packet_item_set.aAttr[j];

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemData(packet_item_set.Cell, kItemData);

#ifdef ENABLE_SWITCHBOT_SYSTEM
	if (packet_item_set.Cell.window_type == SWITCHBOT)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotWindow", Py_BuildValue("()"));
		return true;
	}
#endif

	if (packet_item_set.highlight)
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Highlight_Item", Py_BuildValue("(ii)", packet_item_set.Cell.window_type, packet_item_set.Cell.cell));

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemUsePacket()
{
	TPacketGCItemUse packet_item_use;

	if (!AutoRecv(packet_item_use))
		return false;

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemUpdatePacket()
{
	TPacketGCItemUpdate packet_item_update;

	if (!AutoRecv(packet_item_update))
		return false;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.SetItemCount(packet_item_update.Cell, packet_item_update.count);
	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		rkPlayer.SetItemMetinSocket(packet_item_update.Cell, i, packet_item_update.alSockets[i]);
	for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		rkPlayer.SetItemAttribute(packet_item_update.Cell, j, packet_item_update.aAttr[j].bType, packet_item_update.aAttr[j].sValue);

	__RefreshInventoryWindow();
	return true;
}

bool CPythonNetworkStream::RecvItemGroundAddPacket()
{
	TPacketGCItemGroundAdd packet_item_ground_add;

	if (!AutoRecv(packet_item_ground_add))
		return false;

	__GlobalPositionToLocalPosition(packet_item_ground_add.lX, packet_item_ground_add.lY);

	CPythonItem::Instance().CreateItem(packet_item_ground_add.dwVID,
									   packet_item_ground_add.dwVnum,
									   packet_item_ground_add.lX,
									   packet_item_ground_add.lY,
									   packet_item_ground_add.lZ);
	return true;
}

bool CPythonNetworkStream::RecvItemOwnership()
{
	TPacketGCItemOwnership p;

	if (!AutoRecv(p))
		return false;

#ifdef ENABLE_OWNERSHIP_DURATION_TIMER
	CPythonItem::Instance().SetOwnership(p.dwVID, p.szName, p.iSec, p.bUpdate);
#else
	CPythonItem::Instance().SetOwnership(p.dwVID, p.szName);
#endif
	return true;
}

bool CPythonNetworkStream::RecvItemGroundDelPacket()
{
	TPacketGCItemGroundDel	packet_item_ground_del;

	if (!AutoRecv(packet_item_ground_del))
		return false;

	CPythonItem::Instance().DeleteItem(packet_item_ground_del.vid);
	return true;
}

bool CPythonNetworkStream::RecvQuickSlotAddPacket()
{
	TPacketGCQuickSlotAdd packet_quick_slot_add;

	if (!AutoRecv(packet_quick_slot_add))
		return false;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.AddQuickSlot(packet_quick_slot_add.pos, packet_quick_slot_add.slot.Type, packet_quick_slot_add.slot.Position);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotDelPacket()
{
	TPacketGCQuickSlotDel packet_quick_slot_del;

	if (!AutoRecv(packet_quick_slot_del))
		return false;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.DeleteQuickSlot(packet_quick_slot_del.pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::RecvQuickSlotMovePacket()
{
	TPacketGCQuickSlotSwap packet_quick_slot_swap;

	if (!AutoRecv(packet_quick_slot_swap))
		return false;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	rkPlayer.MoveQuickSlot(packet_quick_slot_swap.pos, packet_quick_slot_swap.change_pos);

	__RefreshInventoryWindow();

	return true;
}

bool CPythonNetworkStream::SendShopEndPacket()
{
	if (!__CanActMainInstance())
		return true;

#ifdef ENABLE_SPECIAL_INVENTORY
	TPacketCGShop packet_shop = {};
#else
	TPacketCGShop packet_shop;
#endif
	packet_shop.header = HEADER_CG_SHOP;
	packet_shop.subheader = SHOP_SUBHEADER_CG_END;

	if (!Send(packet_shop))
	{
		Tracef("SendShopEndPacket Error\n");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_SPECIAL_INVENTORY
bool CPythonNetworkStream::SendShopBuyPacket(WORD bPos)
#else
bool CPythonNetworkStream::SendShopBuyPacket(BYTE bPos)
#endif
{
	if (!__CanActMainInstance())
		return true;

#ifdef ENABLE_SPECIAL_INVENTORY
	TPacketCGShop PacketShop = {};
#else
	TPacketCGShop PacketShop;
#endif
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_BUY;
#ifdef ENABLE_SPECIAL_INVENTORY
	PacketShop.wPos = bPos;
	PacketShop.bCount = 1;
#endif

	if (!Send(PacketShop))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

#ifndef ENABLE_SPECIAL_INVENTORY
	BYTE bCount=1;
	if (!Send(bCount))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}

	if (!Send(bPos))
	{
		Tracef("SendShopBuyPacket Error\n");
		return false;
	}
#endif

	return SendSequence();
}

#ifdef ENABLE_SPECIAL_INVENTORY
bool CPythonNetworkStream::SendShopSellPacket(WORD bySlot)
#else
bool CPythonNetworkStream::SendShopSellPacket(BYTE bySlot)
#endif
{
	if (!__CanActMainInstance())
		return true;

#ifdef ENABLE_SPECIAL_INVENTORY
	TPacketCGShop PacketShop = {};
#else
	TPacketCGShop PacketShop;
#endif
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL;
#ifdef ENABLE_SPECIAL_INVENTORY
	PacketShop.wPos = bySlot;
#endif

	if (!Send(PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
#ifndef ENABLE_SPECIAL_INVENTORY
	if (!Send(bySlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}
#endif

	return SendSequence();
}

#ifdef ENABLE_SPECIAL_INVENTORY
#ifdef ENABLE_EXTENDED_ITEM_COUNT
bool CPythonNetworkStream::SendShopSellPacketNew(WORD wSlot, short byCount)
#else
bool CPythonNetworkStream::SendShopSellPacketNew(WORD wSlot, BYTE byCount)
#endif
#else
#ifdef ENABLE_EXTENDED_ITEM_COUNT
bool CPythonNetworkStream::SendShopSellPacketNew(BYTE bySlot, short byCount)
#else
bool CPythonNetworkStream::SendShopSellPacketNew(BYTE bySlot, BYTE byCount)
#endif
#endif // ENABLE_SPECIAL_INVENTORY
{
	if (!__CanActMainInstance())
		return true;

#ifdef ENABLE_SPECIAL_INVENTORY
	TPacketCGShop PacketShop = {};
#else
	TPacketCGShop PacketShop;
#endif
	PacketShop.header = HEADER_CG_SHOP;
	PacketShop.subheader = SHOP_SUBHEADER_CG_SELL2;
#ifdef ENABLE_SPECIAL_INVENTORY
	PacketShop.wPos = wSlot;
	PacketShop.bCount = byCount;
#endif

	if (!Send(PacketShop))
	{
		Tracef("SendShopSellPacket Error\n");
		return false;
	}
#ifndef ENABLE_SPECIAL_INVENTORY
	if (!Send(bySlot))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}
	if (!Send(byCount))
	{
		Tracef("SendShopAddSellPacket Error\n");
		return false;
	}

	Tracef(" SendShopSellPacketNew(bySlot=%d, byCount=%d)\n", bySlot, byCount);
#endif

	return SendSequence();
}

// Send
bool CPythonNetworkStream::SendItemUsePacket(TItemPos pos)
{
	if (!__CanActMainInstance())
		return true;

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().isTrading())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
			return true;
		}

		if (CPythonShop::Instance().IsOpen())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
			return true;
		}

		if (__IsPlayerAttacking())
			return true;
	}

	__PlayInventoryItemUseSound(pos);

	TPacketCGItemUse itemUsePacket;
	itemUsePacket.header = HEADER_CG_ITEM_USE;
	itemUsePacket.pos = pos;

	if (!Send(itemUsePacket))
	{
		Tracen("SendItemUsePacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemUseToItem itemUseToItemPacket;
	itemUseToItemPacket.header = HEADER_CG_ITEM_USE_TO_ITEM;
	itemUseToItemPacket.source_pos = source_pos;
	itemUseToItemPacket.target_pos = target_pos;

	if (!Send(itemUseToItemPacket))
	{
		Tracen("SendItemUseToItemPacket Error");
		return false;
	}

#ifdef _DEBUG
	Tracef(" << SendItemUseToItemPacket(src=%d, dst=%d)\n", source_pos, target_pos);
#endif

	return SendSequence();
}

bool CPythonNetworkStream::SendItemDropPacket(TItemPos pos, DWORD elk)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP;
	itemDropPacket.pos = pos;
	itemDropPacket.elk = elk;

	if (!Send(itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemDropPacketNew(TItemPos pos, DWORD elk, DWORD count)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemDrop2 itemDropPacket;
	itemDropPacket.header = HEADER_CG_ITEM_DROP2;
	itemDropPacket.pos = pos;
	itemDropPacket.gold = elk;
	itemDropPacket.count = count;

	if (!Send(itemDropPacket))
	{
		Tracen("SendItemDropPacket Error");
		return false;
	}

	return SendSequence();
}

#ifdef ENABLE_ITEM_MANAGER
bool CPythonNetworkStream::SendItemDestroyPacket(TItemPos pos)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGItemDestroy itemDestroyPacket;
	itemDestroyPacket.header = HEADER_CG_ITEM_DESTROY;
	itemDestroyPacket.pos = pos;

	if (!Send(sizeof(itemDestroyPacket), &itemDestroyPacket))
	{
		Tracen("SendItemDestroyPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemSellPacket(TItemPos pos, DWORD elk)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGItemSell itemSellPacket;
	itemSellPacket.header = HEADER_CG_ITEM_SELL;
	itemSellPacket.pos = pos;
	itemSellPacket.gold = elk;

	if (!Send(sizeof(itemSellPacket), &itemSellPacket))
	{
		Tracen("SendItemDestroyPacket Error");
		return false;
	}

	return true;
}
#endif

bool CPythonNetworkStream::__IsEquipItemInSlot(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	return rkPlayer.IsEquipItemInSlot(uSlotPos);
}

void CPythonNetworkStream::__PlayInventoryItemUseSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	DWORD dwItemID=rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayUseSound(dwItemID);
}

void CPythonNetworkStream::__PlayInventoryItemDropSound(TItemPos uSlotPos)
{
	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	DWORD dwItemID=rkPlayer.GetItemIndex(uSlotPos);

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

void CPythonNetworkStream::__PlaySafeBoxItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox=CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

void CPythonNetworkStream::__PlayMallItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox=CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotMallItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

#ifdef ENABLE_EXTENDED_ITEM_COUNT
bool CPythonNetworkStream::SendItemMovePacket(TItemPos pos, TItemPos change_pos, short num)
#else
bool CPythonNetworkStream::SendItemMovePacket(TItemPos pos, TItemPos change_pos, BYTE num)
#endif
{
	if (!__CanActMainInstance())
		return true;

	if (__IsEquipItemInSlot(pos))
	{
		if (CPythonExchange::Instance().isTrading())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_EXCHANGE"));
				return true;
			}
		}

		if (CPythonShop::Instance().IsOpen())
		{
			if (pos.IsEquipCell() || change_pos.IsEquipCell())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendNotifyMessage", Py_BuildValue("(s)", "CANNOT_EQUIP_SHOP"));
				return true;
			}
		}

		if (__IsPlayerAttacking())
			return true;
	}

	__PlayInventoryItemDropSound(pos);

	TPacketCGItemMove	itemMovePacket;
	itemMovePacket.header = HEADER_CG_ITEM_MOVE;
	itemMovePacket.pos = pos;
	itemMovePacket.change_pos = change_pos;
	itemMovePacket.num = num;

	if (!Send(itemMovePacket))
	{
		Tracen("SendItemMovePacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendItemPickUpPacket(DWORD vid)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGItemPickUp	itemPickUpPacket;
	itemPickUpPacket.header = HEADER_CG_ITEM_PICKUP;
	itemPickUpPacket.vid = vid;

	if (!Send(itemPickUpPacket))
	{
		Tracen("SendItemPickUpPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotAddPacket(BYTE wpos, BYTE type, BYTE pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotAdd quickSlotAddPacket;

	quickSlotAddPacket.header		= HEADER_CG_QUICKSLOT_ADD;
	quickSlotAddPacket.pos			= wpos;
	quickSlotAddPacket.slot.Type	= type;
	quickSlotAddPacket.slot.Position = pos;

	if (!Send(quickSlotAddPacket))
	{
		Tracen("SendQuickSlotAddPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotDelPacket(BYTE pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotDel quickSlotDelPacket;

	quickSlotDelPacket.header = HEADER_CG_QUICKSLOT_DEL;
	quickSlotDelPacket.pos = pos;

	if (!Send(quickSlotDelPacket))
	{
		Tracen("SendQuickSlotDelPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuickSlotMovePacket(BYTE pos, BYTE change_pos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGQuickSlotSwap quickSlotSwapPacket;

	quickSlotSwapPacket.header = HEADER_CG_QUICKSLOT_SWAP;
	quickSlotSwapPacket.pos = pos;
	quickSlotSwapPacket.change_pos = change_pos;

	if (!Send(quickSlotSwapPacket))
	{
		Tracen("SendQuickSlotSwapPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvSpecialEffect()
{
	TPacketGCSpecialEffect kSpecialEffect;
	if (!AutoRecv(kSpecialEffect))
		return false;

	DWORD effect = -1;
	bool bPlayPotionSound = false;
	bool bAttachEffect = true;
	switch (kSpecialEffect.type)
	{
		case SE_HPUP_RED:
			effect = CInstanceBase::EFFECT_HPUP_RED;
			bPlayPotionSound = true;
			break;
		case SE_SPUP_BLUE:
			effect = CInstanceBase::EFFECT_SPUP_BLUE;
			bPlayPotionSound = true;
			break;
		case SE_SPEEDUP_GREEN:
			effect = CInstanceBase::EFFECT_SPEEDUP_GREEN;
			bPlayPotionSound = true;
			break;
		case SE_DXUP_PURPLE:
			effect = CInstanceBase::EFFECT_DXUP_PURPLE;
			bPlayPotionSound = true;
			break;
		case SE_CRITICAL:
			effect = CInstanceBase::EFFECT_CRITICAL;
			break;
		case SE_PENETRATE:
			effect = CInstanceBase::EFFECT_PENETRATE;
			break;
		case SE_BLOCK:
			effect = CInstanceBase::EFFECT_BLOCK;
			break;
		case SE_DODGE:
			effect = CInstanceBase::EFFECT_DODGE;
			break;
		case SE_CHINA_FIREWORK:
			effect = CInstanceBase::EFFECT_FIRECRACKER;
			bAttachEffect = false;
			break;
		case SE_SPIN_TOP:
			effect = CInstanceBase::EFFECT_SPIN_TOP;
			bAttachEffect = false;
			break;
		case SE_SUCCESS :
			effect = CInstanceBase::EFFECT_SUCCESS ;
			bAttachEffect = false ;
			break ;
		case SE_FAIL :
			effect = CInstanceBase::EFFECT_FAIL ;
			break ;
		case SE_FR_SUCCESS:
			effect = CInstanceBase::EFFECT_FR_SUCCESS;
			bAttachEffect = false ;
			break;
		case SE_LEVELUP_ON_14_FOR_GERMANY:
			effect = CInstanceBase::EFFECT_LEVELUP_ON_14_FOR_GERMANY;
			bAttachEffect = false ;
			break;
		case SE_LEVELUP_UNDER_15_FOR_GERMANY:
			effect = CInstanceBase::EFFECT_LEVELUP_UNDER_15_FOR_GERMANY;
			bAttachEffect = false ;
			break;
		case SE_PERCENT_DAMAGE1:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE1;
			break;
		case SE_PERCENT_DAMAGE2:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE2;
			break;
		case SE_PERCENT_DAMAGE3:
			effect = CInstanceBase::EFFECT_PERCENT_DAMAGE3;
			break;
		case SE_AUTO_HPUP:
			effect = CInstanceBase::EFFECT_AUTO_HPUP;
			break;
		case SE_AUTO_SPUP:
			effect = CInstanceBase::EFFECT_AUTO_SPUP;
			break;
		case SE_EQUIP_RAMADAN_RING:
			effect = CInstanceBase::EFFECT_RAMADAN_RING_EQUIP;
			break;
		case SE_EQUIP_HALLOWEEN_CANDY:
			effect = CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP;
			break;
		case SE_EQUIP_HAPPINESS_RING:
 			effect = CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP;
			break;
		case SE_EQUIP_LOVE_PENDANT:
			effect = CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP;
			break;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case SE_EFFECT_ACCE_SUCESS_ABSORB:
			effect = CInstanceBase::EFFECT_ACCE_SUCESS_ABSORB;
			break;
		case SE_EFFECT_ACCE_EQUIP:
			effect = CInstanceBase::EFFECT_ACCE_EQUIP;
			break;
#endif

		default:
			TraceError("%d is not a special effect number. TPacketGCSpecialEffect",kSpecialEffect.type);
			break;
	}

	if (bPlayPotionSound)
	{
		IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
		if(rkPlayer.IsMainCharacterIndex(kSpecialEffect.vid))
		{
			CPythonItem& rkItem=CPythonItem::Instance();
			rkItem.PlayUsePotionSound();
		}
	}

	if (-1 != effect)
	{
		CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecialEffect.vid);
		if (pInstance)
		{
			if(bAttachEffect)
				pInstance->AttachSpecialEffect(effect);
			else
				pInstance->CreateSpecialEffect(effect);
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvSpecificEffect()
{
	TPacketGCSpecificEffect kSpecificEffect;
	if (!AutoRecv(kSpecificEffect))
		return false;

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kSpecificEffect.vid);
	//EFFECT_TEMP
	if (pInstance)
	{
#if defined(__BL_GRAPHIC_ON_OFF__)
		if (!pInstance->CanRenderActor() || !pInstance->CanRenderEffect())
			return true;
#endif
		CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_TEMP, "", kSpecificEffect.effect_file, false);
		pInstance->AttachSpecialEffect(CInstanceBase::EFFECT_TEMP);
	}

	return true;
}

bool CPythonNetworkStream::RecvDragonSoulRefine()
{
	TPacketGCDragonSoulRefine kDragonSoul;

	if (!AutoRecv(kDragonSoul))
		return false;

	switch (kDragonSoul.bSubType)
	{
	case DS_SUB_HEADER_OPEN:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_Open", Py_BuildValue("()"));
		break;
	case DS_SUB_HEADER_REFINE_FAIL:
	case DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE:
	case DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL:
	case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY:
	case DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL:
	case DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineFail", Py_BuildValue("(iii)",
			kDragonSoul.bSubType, kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
		break;
	case DS_SUB_HEADER_REFINE_SUCCEED:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_DragonSoulRefineWindow_RefineSucceed",
				Py_BuildValue("(ii)", kDragonSoul.Pos.window_type, kDragonSoul.Pos.cell));
		break;
	}

	return true;
}

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
bool CPythonNetworkStream::ChangeCostumeVisible(BYTE type, bool status)
{
	TPacketCGChangeCostumeVisibilityStatus kChange;
	kChange.bHeader = HEADER_CG_CHANGE_COSTUME_VISIBILITY_STATUS;
	kChange.type = type;
	kChange.status = status;

	if (!Send(sizeof(kChange), &kChange))
	{
		Tracef("ChangeCostumeVisible Error\n");
		return false;
	}

	return true;
}
#endif

#ifdef ENABLE_SPLIT_BY_COUNT
bool CPythonNetworkStream::SendSplitItemByCount(TItemPos Cell, short sCount)
{
	if (!__CanActMainInstance())
		return true;
	
	TSplitItemByCount dataPacket(HEADER_CG_SPLIT_ITEM, Cell, sCount);
	if (!Send(sizeof(TSplitItemByCount), &dataPacket))
	{
		Tracef("SendSplitItemByCount Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_FAST_STACK
bool CPythonNetworkStream::SendFastStackPacket(TItemPos itemPos)
{
	if (!__CanActMainInstance())
		return true;

	TPacketCGFastStack fastStackPacket;
	fastStackPacket.header = HEADER_CG_FAST_STACK;
	fastStackPacket.pos = itemPos;

	if (!Send(sizeof(TPacketCGFastStack), &fastStackPacket))
	{
		Tracen("SendFastStackPacket Error");
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_GUILD_SAFEBOX
void CPythonNetworkStream::__PlayGuildSafeboxItemDropSound(UINT uSlotPos)
{
	DWORD dwItemID;
	CPythonSafeBox& rkSafeBox = CPythonSafeBox::Instance();
	if (!rkSafeBox.GetSlotGuildItemID(uSlotPos, &dwItemID))
		return;

	CPythonItem& rkItem = CPythonItem::Instance();
	rkItem.PlayDropSound(dwItemID);
}

bool CPythonNetworkStream::SendGuildSafeboxOpenPacket()
{
	BYTE bHeader = HEADER_CG_GUILD_SAFEBOX_OPEN;
	if (!Send(sizeof(BYTE), &bHeader))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGuildSafeboxCheckinPacket(TItemPos InventoryPos, BYTE byGuildSafeboxPos)
{
	__PlayInventoryItemDropSound(InventoryPos);

	TPacketCGGuildSafeboxCheckin kSafeboxCheckin;
	kSafeboxCheckin.bHeader = HEADER_CG_GUILD_SAFEBOX_CHECKIN;
	kSafeboxCheckin.ItemPos = InventoryPos;
	kSafeboxCheckin.bSafePos = byGuildSafeboxPos;
	if (!Send(sizeof(kSafeboxCheckin), &kSafeboxCheckin))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGuildSafeboxCheckoutPacket(BYTE byGuildSafeboxPos, TItemPos InventoryPos)
{
	__PlayGuildSafeboxItemDropSound(byGuildSafeboxPos);

	TPacketCGGuildSafeboxCheckout kSafeboxCheckout;
	kSafeboxCheckout.bHeader = HEADER_CG_GUILD_SAFEBOX_CHECKOUT;
	kSafeboxCheckout.bSafePos = byGuildSafeboxPos;
	kSafeboxCheckout.ItemPos = InventoryPos;
	if (!Send(sizeof(kSafeboxCheckout), &kSafeboxCheckout))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGuildSafeboxItemMovePacket(BYTE bySourcePos, BYTE byTargetPos, BYTE byCount)
{
	__PlayGuildSafeboxItemDropSound(bySourcePos);

	TPacketCGItemMove kItemMove;
	kItemMove.header = HEADER_CG_GUILD_SAFEBOX_ITEM_MOVE;
	kItemMove.pos = TItemPos(GUILD_SAFEBOX, bySourcePos);
	kItemMove.num = byCount;
	kItemMove.change_pos = TItemPos(GUILD_SAFEBOX, byTargetPos);
	if (!Send(sizeof(kItemMove), &kItemMove))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGuildSafeboxGiveGoldPacket(DWORD dwGold)
{
	TPacketCGGuildSafeboxGold kGoldGive;
	kGoldGive.header = HEADER_CG_GUILD_SAFEBOX_GIVE_GOLD;
	kGoldGive.gold = dwGold;
	if (!Send(sizeof(kGoldGive), &kGoldGive))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::SendGuildSafeboxTakeGoldPacket(DWORD dwGold)
{
	TPacketCGGuildSafeboxGold kGoldTake;
	kGoldTake.header = HEADER_CG_GUILD_SAFEBOX_GET_GOLD;
	kGoldTake.gold = dwGold;
	if (!Send(sizeof(kGoldTake), &kGoldTake))
		return false;

	return SendSequence();
}

bool CPythonNetworkStream::RecvGuildSafeboxPacket()
{
	TPacketGCGuildSafebox kGuildSafebox;
	if (!Recv(sizeof(kGuildSafebox), &kGuildSafebox))
		return false;

	// TraceError("RecvGuildSafeboxPacket: subHeader %u", kGuildSafebox.subheader);

	switch (kGuildSafebox.subheader)
	{
	case GUILD_SAFEBOX_SUBHEADER_SET_ITEM:
		{
			TGuildSafeboxItem item_packet;
			if (!Recv(sizeof(TGuildSafeboxItem), &item_packet))
				return false;

			TItemData item;
			memset(&item, 0, sizeof(TItemData));

			item.vnum = item_packet.vnum;
			item.count = item_packet.count;
			memcpy(item.alSockets, item_packet.sockets, sizeof(item.alSockets));
			memcpy(item.aAttr, item_packet.attr, sizeof(item.aAttr));

			CPythonSafeBox::Instance().SetGuildItemData(item_packet.pos, item);

			__RefreshGuildSafeboxWindow();
		}
		break;

	case GUILD_SAFEBOX_SUBHEADER_REMOVE_ITEM:
		{
			BYTE bSlot;
			if (!Recv(sizeof(BYTE), &bSlot))
				return false;

			CPythonSafeBox::Instance().DelGuildItemData(bSlot);

			__RefreshGuildSafeboxWindow();
		}
		break;

	case GUILD_SAFEBOX_SUBHEADER_GOLD:
		{
			DWORD dwGold;
			if (!Recv(sizeof(DWORD), &dwGold))
				return false;

			CPythonSafeBox::Instance().SetGuildMoney(dwGold);

			__RefreshGuildSafeboxWindow();
		}
		break;

	case GUILD_SAFEBOX_SUBHEADER_OPEN:
		{
			BYTE bSize;
			if (!Recv(sizeof(BYTE), &bSize))
				return false;

			CPythonSafeBox::Instance().OpenGuild(bSize);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenGuildSafeboxWindow",
				Py_BuildValue("(i)", CPythonSafeBox::SAFEBOX_SLOT_Y_COUNT * bSize));
		}
		break;

	case GUILD_SAFEBOX_SUBHEADER_CLOSE:
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseGuildSafeboxWindow",
				Py_BuildValue("()"));
		}
		break;
	}

	return true;
}
#endif

#ifdef ENABLE_CHEST_OPEN_RENEWAL
bool CPythonNetworkStream::SendOpenChestPacket(TItemPos pos, WORD wOpenCount)
{
	if (!__CanActMainInstance())
		return true;
	__PlayInventoryItemUseSound(pos);

	TPacketCGOpenChest chestUsePacket;
	chestUsePacket.header = HEADER_CG_CHEST_OPEN;
	chestUsePacket.pos = pos;
	chestUsePacket.wOpenCount = wOpenCount;

	if (!Send(sizeof(TPacketCGOpenChest), &chestUsePacket))
	{
		Tracen("SendOpenChestPacket Error");
		return false;
	}

	return SendSequence();
}
#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
