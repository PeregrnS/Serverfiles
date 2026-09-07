#include "StdAfx.h"
#include "PythonApplication.h"
#include "PythonNetworkStream.h"
#include "PythonGuildStorage.h"

#define Recv(pack) (CPythonNetworkStream::Instance().Recv(sizeof(pack), &pack))
#define Send(pack) (CPythonNetworkStream::Instance().Send(sizeof(pack) , &pack))

CPythonGuildStorage* CPythonGuildStorage::currentInstance = nullptr;

CPythonGuildStorage::CPythonGuildStorage()
{
	Init();
}

CPythonGuildStorage::~CPythonGuildStorage()
{
}

void CPythonGuildStorage::Init()
{
	m_currentItemCount = 0;
}

CPythonGuildStorage* CPythonGuildStorage::Instance()
{
	if (!currentInstance)
		currentInstance = new CPythonGuildStorage();
	return currentInstance;
}

void CPythonGuildStorage::RecvGuildStoragePacket()
{
	TPacketGuildStorage packet;
	if (!Recv(packet))
	{
		TraceError("!Recv RecvGuildStoragePacket");
		return;
	}

	switch (packet.subHeader)
	{
	case GUILDSTORAGE_GC_RECV_GSITEMS:
		RecvGuildStorageOpenItems();
		break;
	case GUILDSTORAGE_GC_RECV_OPEN:
		RecvGuildStorageOpen();
		break;
	case GUILDSTORAGE_GC_RECV_CLOSE:
		RecvGuildStorageClose();
		break;
	case GUILDSTORAGE_GC_REMOVE_ITEM:
		RecvGuildStorageRemoveItem();
		break;
	case GUILDSTORAGE_GC_ADD_ITEM:
		RecvGuildStorageAddItem();
		break;
	case GUILDSTORAGE_GC_GOLD_UPDATE:
		RecvGuildStorageGoldUpdate();
		break;
	case GUILDSTORAGE_GC_RECV_LOG:
		RecvGuildStorageLogs();
		break;
	case GUILDSTORAGE_GC_SEND_LOGENTRY:
		RecvGuildStorageLogEntry();
		break;
	default:
		TraceError("Unkown subheader for guildstorage!");
		break;
	}
}

void CPythonGuildStorage::RecvGuildStorageOpenItems()
{
	TPacketGCGuildStorageOpenItems pack;
	if (!Recv(pack))
	{
		TraceError("!Recv RecvGuildStorageOpenItems");
		return;
	}

	if (pack.clear)
	{
		m_guildStorageItems.clear();
		m_currentItemCount = 0;
	}
		

	for (uint16_t i = 0; i < pack.item_count; i++)
	{
		TPlayerItem item;
		if (!Recv(item))
		{
			TraceError("!Recv RecvGuildStorageOpenItems Item %d", i);
			return;
		}
		auto new_item = std::make_shared<TPlayerItem>();
		memcpy(new_item.get(), &item, sizeof(TPlayerItem));
		m_guildStorageItems.insert(std::make_pair(new_item->pos, std::move(new_item)));
		m_currentItemCount++;
	}
}

void CPythonGuildStorage::RecvGuildStorageOpen()
{
	TPacketGCGuildStorageOpen pack;
	if (!Recv(pack))
	{
		TraceError("!Recv RecvGuildStorageOpen");
		return;
	}
	
	PyCallClassMemberFunc(CPythonNetworkStream::Instance().GetPhaseWindow(5), "BINARY_GUILDSTORAGE_OPEN", Py_BuildValue("(i)", pack.isAdmin));
}

void CPythonGuildStorage::RecvGuildStorageClose()
{
	PyCallClassMemberFunc(CPythonNetworkStream::Instance().GetPhaseWindow(5), "BINARY_GUILDSTORAGE_CLOSE", Py_BuildValue("()"));
}

void CPythonGuildStorage::RecvGuildStorageRemoveItem()
{
	TPacketGCGuildStorageRemoveItem pack;
	if (!Recv(pack))
	{
		TraceError("!Recv RecvGuildStorageRemoveItem");
		return;
	}
	
	auto item = m_guildStorageItems.find(pack.slot);
	if (item != m_guildStorageItems.end())
		m_guildStorageItems.erase(item);

	PyCallClassMemberFunc(CPythonNetworkStream::Instance().GetPhaseWindow(5), "BINARY_GUILDSTORAGE_REFRESH", Py_BuildValue("()"));
}

void CPythonGuildStorage::RecvGuildStorageAddItem()
{
	TPacketGCGuildStorageAddItem pack;
	if (!Recv(pack))
	{
		TraceError("!Recv RecvGuildStorageAddItem");
		return;
	}
	
	auto new_item = std::make_shared<TPlayerItem>();
	memcpy(new_item.get(), &pack.item, sizeof(TPlayerItem));
	m_guildStorageItems.insert(std::make_pair(new_item->pos, new_item));

	PyCallClassMemberFunc(CPythonNetworkStream::Instance().GetPhaseWindow(5), "BINARY_GUILDSTORAGE_REFRESH", Py_BuildValue("()"));
}

void CPythonGuildStorage::RecvGuildStorageGoldUpdate()
{
	TPacketGuildStorageGoldUpdate pack;
	if (!Recv(pack))
	{
		TraceError("!Recv RecvGuildStorageGoldUpdate");
		return;
	}

	PyCallClassMemberFunc(CPythonNetworkStream::Instance().GetPhaseWindow(5), "BINARY_GUILDSTORAGE_SET_GOLD", Py_BuildValue("(L)", pack.gold));
}

void CPythonGuildStorage::RecvGuildStorageLogs()
{
	TPacketGCGuildStorageLog pack;
	if (!Recv(pack))
	{
		TraceError("!Recv RecvGuildStorageLogs");
		return;
	}

	if (pack.clear)
		m_guildStorageLogs.clear();

	for (uint16_t i = 0; i < pack.log_count; i++)
	{
		TGSLogEntry log;
		if (!Recv(log))
		{
			TraceError("!Recv RecvGuildStorageLogs log %d", i);
			return;
		}
		auto new_log = std::make_shared<TGSLogEntry>();
		memcpy(new_log.get(), &log, sizeof(TGSLogEntry));
		m_guildStorageLogs.push_back(std::move(new_log));
	}
}

void CPythonGuildStorage::RecvGuildStorageLogEntry()
{
	TGSLogEntry log;
	if (!Recv(log))
	{
		TraceError("!Recv RecvGuildStorageLogEntry");
		return;
	}
	auto new_log = std::make_shared<TGSLogEntry>();
	memcpy(new_log.get(), &log, sizeof(TGSLogEntry));
	m_guildStorageLogs.push_back(std::move(new_log));

	PyCallClassMemberFunc(CPythonNetworkStream::Instance().GetPhaseWindow(5), "BINARY_GUILDSTORAGE_REFRESH_LOGS", Py_BuildValue("()"));
}

bool CPythonGuildStorage::SendItemCheckoutRequest(uint16_t slot, TItemPos item_pos)
{
	TPacketGuildStorage pack;
	pack.header = HEADER_CG_GUILDSTORAGE;
	pack.size = sizeof(pack) + sizeof(TPacketCGGuildStorageCheckout);
	pack.subHeader = GUILDSTORAGE_CG_REQUEST_CHECKOUT;

	TPacketCGGuildStorageCheckout subpack;
	subpack.slot = slot;
	subpack.inventoryPos = item_pos;

	if (!Send(pack))
	{
		TraceError("!SendItemCheckoutRequest pack");
		return false;
	}

	if (!Send(subpack))
	{
		TraceError("!SendItemCheckoutRequest subpack");
		return false;
	}

	return true;
}

bool CPythonGuildStorage::SendItemCheckinRequest(TItemPos item_pos, uint16_t new_slot)
{
	TPacketGuildStorage pack;
	pack.header = HEADER_CG_GUILDSTORAGE;
	pack.size = sizeof(pack) + sizeof(TPacketCGGuildStorageCheckin);
	pack.subHeader = GUILDSTORAGE_CG_REQUEST_CHECKIN;

	TPacketCGGuildStorageCheckin subpack;
	subpack.inventoryPos = item_pos;
	subpack.slot = new_slot;

	if (!Send(pack))
	{
		TraceError("!SendItemCheckinRequest pack");
		return false;
	}

	if (!Send(subpack))
	{
		TraceError("!SendItemCheckinRequest subpack");
		return false;
	}

	return true;
}

bool CPythonGuildStorage::SendItemMoveRequest(uint16_t slot, uint16_t new_slot)
{
	TPacketGuildStorage pack;
	pack.header = HEADER_CG_GUILDSTORAGE;
	pack.size = sizeof(pack) + sizeof(TPacketCGGuildStorageMove);
	pack.subHeader = GUILDSTORAGE_CG_REQUEST_MOVE;

	TPacketCGGuildStorageMove subpack;
	subpack.slot = slot;
	subpack.new_slot = new_slot;

	if (!Send(pack))
	{
		TraceError("!SendItemMoveRequest pack");
		return false;
	}

	if (!Send(subpack))
	{
		TraceError("!SendItemMoveRequest subpack");
		return false;
	}

	return true;
}

bool CPythonGuildStorage::SendCloseRequest()
{
	TPacketGuildStorage pack;
	pack.header = HEADER_CG_GUILDSTORAGE;
	pack.size = sizeof(pack);
	pack.subHeader = GUILDSTORAGE_CG_REQUEST_CLOSE;
	if (!Send(pack))
	{
		TraceError("!SendItemMoveRequest pack");
		return false;
	}

	return true;
}

bool CPythonGuildStorage::SendGoldCheckinRequest(long long gold)
{
	TPacketGuildStorage pack;
	pack.header = HEADER_CG_GUILDSTORAGE;
	pack.size = sizeof(pack) + sizeof(TPacketGuildStorageGoldUpdate);
	pack.subHeader = GUILDSTORAGE_CG_REQUEST_GOLDIN;

	TPacketGuildStorageGoldUpdate subpack;
	subpack.gold = gold;

	if (!Send(pack))
	{
		TraceError("!SendGoldCheckinRequest pack");
		return false;
	}

	if (!Send(subpack))
	{
		TraceError("!SendGoldCheckinRequest subpack");
		return false;
	}

	return true;
}

bool CPythonGuildStorage::SendGoldCheckoutRequest(long long gold)
{
	TPacketGuildStorage pack;
	pack.header = HEADER_CG_GUILDSTORAGE;
	pack.size = sizeof(pack) + sizeof(TPacketGuildStorageGoldUpdate);
	pack.subHeader = GUILDSTORAGE_CG_REQUEST_GOLDOUT;

	TPacketGuildStorageGoldUpdate subpack;
	subpack.gold = gold;

	if (!Send(pack))
	{
		TraceError("!SendGoldCheckoutRequest pack");
		return false;
	}

	if (!Send(subpack))
	{
		TraceError("!SendGoldCheckoutRequest subpack");
		return false;
	}

	return true;
}
