#include "StdAfx.h"
#include "Packet.h"
#include "PythonApplication.h"
#include "PythonNetworkStream.h"
#include "PythonGuildStorage.h"

PyObject * guildstorageCheckOutItem(PyObject* poSelf, PyObject* poArgs)
{
	int slot;
	if (!PyTuple_GetInteger(poArgs, 0, &slot))
		return Py_BuildException();

	TItemPos inventoryPos;
	if (!PyTuple_GetInteger(poArgs, 1, &inventoryPos.cell))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &inventoryPos.window_type))
		return Py_BuildException();

	if (slot < 0 || slot > 1200)
		return Py_BuildException();

	return Py_BuildValue("b", CPythonGuildStorage::Instance()->SendItemCheckoutRequest(slot, inventoryPos));
}

PyObject * guildstorageCheckInItem(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos inventoryPos;
	if (!PyTuple_GetInteger(poArgs, 0, &inventoryPos.cell))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &inventoryPos.window_type))
		return Py_BuildException();

	int slot;
	if (!PyTuple_GetInteger(poArgs, 2, &slot))
		return Py_BuildException();

	if (slot < 0 || slot > 1200)
		return Py_BuildException();

	return Py_BuildValue("b", CPythonGuildStorage::Instance()->SendItemCheckinRequest(inventoryPos, slot));
}

PyObject * guildstorageMoveItem(PyObject* poSelf, PyObject* poArgs)
{
	int slot;
	if (!PyTuple_GetInteger(poArgs, 0, &slot))
		return Py_BuildException();

	if (slot < 0 || slot > 1200)
		return Py_BuildException();


	int new_slot;
	if (!PyTuple_GetInteger(poArgs, 1, &new_slot))
		return Py_BuildException();

	if (new_slot < 0 || new_slot > 1200)
		return Py_BuildException();

	return Py_BuildValue("b", CPythonGuildStorage::Instance()->SendItemMoveRequest(slot, new_slot));
}

PyObject * guildstorageGetItem(PyObject* poSelf, PyObject* poArgs)
{
	int slot;
	if (!PyTuple_GetInteger(poArgs, 0, &slot))
		return Py_BuildException();

	if (slot < 0 || slot > 1200)
		return Py_BuildException();
	PyObject* item = PyTuple_New(4);

	auto pItem = CPythonGuildStorage::Instance()->GetItem(slot);
	if (pItem == nullptr)
	{
		PyTuple_SetItem(item, 0, Py_BuildValue("i", 0));
		PyTuple_SetItem(item, 1, Py_BuildValue("i", 0));
		PyTuple_SetItem(item, 2, Py_BuildValue("i", 0));
		PyTuple_SetItem(item, 3, Py_BuildValue("i", 0));
	}
	else
	{
		PyTuple_SetItem(item, 0, Py_BuildValue("i", pItem->vnum)); // vnum
		PyTuple_SetItem(item, 1, Py_BuildValue("i", pItem->count)); // count
		PyObject* sockets = PyTuple_New(CItemData::ITEM_SOCKET_MAX_NUM); // sockets
		for (int x = 0; x < CItemData::ITEM_SOCKET_MAX_NUM; x++)
			PyTuple_SetItem(sockets, x, Py_BuildValue("i", pItem->alSockets[x]));
		PyTuple_SetItem(item, 2, sockets); // sockets
		//

		PyObject* attrs = PyTuple_New(ITEM_ATTRIBUTE_SLOT_MAX_NUM); // attrs
		for (int x = 0; x < ITEM_ATTRIBUTE_SLOT_MAX_NUM; x++)
		{
			PyObject* attr = PyTuple_New(ITEM_ATTRIBUTE_SLOT_MAX_NUM); // attrs
			PyTuple_SetItem(attr, 0, Py_BuildValue("i", pItem->aAttr[x].bType)); // type
			PyTuple_SetItem(attr, 1, Py_BuildValue("i", pItem->aAttr[x].sValue)); // value
			PyTuple_SetItem(attrs, x, attr);
		}

		PyTuple_SetItem(item, 3, attrs); // sockets
		//PyTuple_SetItem(item, 4,  Py_BuildValue("i", pItem->dwVnum)); // projection SPlayerItem
	}

	return item;
}

PyObject * guildstorageClose(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonGuildStorage::Instance()->SendCloseRequest());
}

PyObject * guildstorageCheckInGold(PyObject* poSelf, PyObject* poArgs)
{
	long long gold;
	if (!PyTuple_GetLongLong(poArgs, 0, &gold))
		return Py_BuildException();

	return Py_BuildValue("b", CPythonGuildStorage::Instance()->SendGoldCheckinRequest(gold));
}

PyObject * guildstorageCheckOutGold(PyObject* poSelf, PyObject* poArgs)
{
	long long gold;
	if (!PyTuple_GetLongLong(poArgs, 0, &gold))
		return Py_BuildException();

	return Py_BuildValue("b", CPythonGuildStorage::Instance()->SendGoldCheckoutRequest(gold));
}

PyObject * guildstorageGetLogs(PyObject* poSelf, PyObject* poArgs)
{

	PyObject* logs = PyList_New(CPythonGuildStorage::Instance()->GetLogCount());

	uint32_t i = 0;
	for (auto logEntry : CPythonGuildStorage::Instance()->m_guildStorageLogs)
	{
		PyObject* log = PyTuple_New(7);

		if (logEntry->slot == UINT16_MAX)
			PyTuple_SetItem(log, 0, Py_BuildValue("i", 0)); // Checkin
		else if (logEntry->slot_new == UINT16_MAX)
			PyTuple_SetItem(log, 0, Py_BuildValue("i", 1)); // Checkout
		else if (logEntry->gold > 0)
			PyTuple_SetItem(log, 0, Py_BuildValue("i", 3)); // Add Gold
		else if (logEntry->gold < 0)
			PyTuple_SetItem(log, 0, Py_BuildValue("i", 4)); // Remove Gold
		else
			PyTuple_SetItem(log, 0, Py_BuildValue("i", 2)); // Move

		PyTuple_SetItem(log, 1, Py_BuildValue("s", logEntry->name));
		PyTuple_SetItem(log, 2, Py_BuildValue("i", logEntry->vnum));
		PyTuple_SetItem(log, 3, Py_BuildValue("i", logEntry->slot));
		PyTuple_SetItem(log, 4, Py_BuildValue("i", logEntry->slot_new));
		PyTuple_SetItem(log, 5, Py_BuildValue("i", logEntry->gold));
		PyTuple_SetItem(log, 6, Py_BuildValue("i", logEntry->time));

		PyList_SetItem(logs, i, log);
		i++;
	}
	
	return logs;
}

void initguildstorage() {
	static PyMethodDef s_methods[] =
	{
		{ "GetItem", guildstorageGetItem, METH_VARARGS },
		{ "CheckOutItem", guildstorageCheckOutItem, METH_VARARGS },
		{ "CheckInItem", guildstorageCheckInItem, METH_VARARGS },
		
		{ "MoveItem", guildstorageMoveItem, METH_VARARGS },
		{ "Close", guildstorageClose, METH_VARARGS },

		
		{ "CheckInGold", guildstorageCheckInGold, METH_VARARGS },
		{ "CheckOutGold", guildstorageCheckOutGold, METH_VARARGS },

		
		{ "GetLogs", guildstorageGetLogs, METH_VARARGS },

		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule("guildstorage", s_methods);

}
