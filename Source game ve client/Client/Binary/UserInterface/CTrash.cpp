#include "stdafx.h"

#ifdef ENABLE_TRASH_SYSTEM
#include "CTrash.h"
#include "PythonNetworkStream.h"
#include "Packet.h"

PyObject* CTrash::m_py_TrashWindow;
std::map<uint8_t, TItemPos> CTrash::m_map_TrashItems;

CTrash::CTrash()
{
}

void CTrash::SetTrashWindow(PyObject* window)
{
	m_py_TrashWindow = window;
}

void CTrash::RefreshWindow()
{
	PyCallClassMemberFunc(m_py_TrashWindow, "RefreshWindow", Py_BuildValue("()"));
}

void CTrash::AddItem(uint8_t slot, const TItemPos& item_pos)
{
	if (m_map_TrashItems.find(slot) != m_map_TrashItems.end())
		return;

	m_map_TrashItems.insert(std::make_pair(slot, item_pos));
	RefreshWindow();
}

void CTrash::RemoveItem(uint8_t slot)
{
	if (!m_map_TrashItems.contains(slot))
		return;

	m_map_TrashItems.erase(slot);
	RefreshWindow();
}

void CTrash::GetItemInfo(uint8_t slot, TItemPos& item_pos)
{
	auto it = m_map_TrashItems.find(slot);
	if (it != m_map_TrashItems.end())
	{
		item_pos.cell = it->second.cell;
		item_pos.window_type = it->second.window_type;
	}
}

void CTrash::ClearItems()
{
	if (m_map_TrashItems.empty())
		return;

	m_map_TrashItems.clear();
	RefreshWindow();
}

bool CTrash::HasItem(const TItemPos& item_pos)
{
	if (m_map_TrashItems.empty())
		return false;

	for (const auto& [_, pos] : m_map_TrashItems)
		if (pos == item_pos)
			return true;
	return false;
}

void CTrash::DeleteItems()
{
	if (m_map_TrashItems.empty())
		return;

	TTrashPacketCG packet;
	packet.header = HEADER_CG_TRASH;
	packet.items_count = m_map_TrashItems.size();

	if (!CPythonNetworkStream::Instance().Send(sizeof(packet), &packet))
		return;

	for (const auto& [slot, item_pos] : m_map_TrashItems)
	{
		if (item_pos.window_type == 0)
			continue;

		
		TTrashItemInfo trash_item;
		trash_item.slot = slot;
		trash_item.item_pos = item_pos;

		if (!CPythonNetworkStream::Instance().Send(sizeof(trash_item), &trash_item))
			return;
	}

	CPythonNetworkStream::Instance().SendSequence();
}

PyObject* trashSetTrashWindow(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* pyHandle;
	if (!PyTuple_GetObject(poArgs, 0, &pyHandle))
		return Py_BadArgument();

	CTrash::SetTrashWindow(pyHandle);
	return Py_BuildNone();
}

PyObject* trashAddItem(PyObject* poSelf, PyObject* poArgs)
{
	uint8_t slot;
	if (!PyTuple_GetInteger(poArgs, 0, &slot))
		return Py_BadArgument();

	TItemPos item_pos;
	if (!PyTuple_GetInteger(poArgs, 1, &item_pos.cell))
		return Py_BadArgument();

	if (!PyTuple_GetInteger(poArgs, 2, &item_pos.window_type))
		return Py_BadArgument();

	CTrash::AddItem(slot, item_pos);
	return Py_BuildNone();
}

PyObject* trashRemoveItem(PyObject* poSelf, PyObject* poArgs)
{
	uint8_t slot;
	if (!PyTuple_GetInteger(poArgs, 0, &slot))
		return Py_BadArgument();

	CTrash::RemoveItem(slot);
	return Py_BuildNone();
}

PyObject* trashGetItemInfo(PyObject* poSelf, PyObject* poArgs)
{
	uint8_t slot;
	if (!PyTuple_GetInteger(poArgs, 0, &slot))
		return Py_BadArgument();

	TItemPos pos;
	pos.cell = 0;
	pos.window_type = 0;

	CTrash::GetItemInfo(slot, pos);
	return Py_BuildValue("(ii)", pos.cell, pos.window_type);
}

PyObject* trashClearItems(PyObject* poSelf, PyObject* poArgs)
{
	CTrash::ClearItems();
	return Py_BuildNone();
}

PyObject* trashHasItem(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos item_pos;
	if (!PyTuple_GetInteger(poArgs, 0, &item_pos.cell))
		return Py_BadArgument();

	if (!PyTuple_GetInteger(poArgs, 1, &item_pos.window_type))
		return Py_BadArgument();

	return Py_BuildValue("b", CTrash::HasItem(item_pos));
}

PyObject* trashDeleteItems(PyObject* poSelf, PyObject* poArgs)
{
	CTrash::DeleteItems();
	return Py_BuildNone();
}

void initTrash()
{
	static PyMethodDef s_methods[] =
	{
		{ "SetTrashWindow",			trashSetTrashWindow,			METH_VARARGS },
		{ "AddItem",				trashAddItem,					METH_VARARGS },
		{ "RemoveItem",				trashRemoveItem,				METH_VARARGS },
		{ "GetItemInfo",			trashGetItemInfo,				METH_VARARGS },
		{ "ClearItems",				trashClearItems,				METH_VARARGS },
		{ "HasItem",				trashHasItem,					METH_VARARGS },
		{ "DeleteItems",			trashDeleteItems,				METH_VARARGS },
		{ NULL,						NULL,							NULL },
	};

	PyObject* poModule = Py_InitModule("trash", s_methods);

	PyModule_AddIntConstant(poModule, 	"X_SLOT_COUNT",		TRASH_X_SLOT_COUNT);
	PyModule_AddIntConstant(poModule, 	"Y_SLOT_COUNT",		TRASH_Y_SLOT_COUNT);
	PyModule_AddIntConstant(poModule, 	"SLOT_MAX_NUM",		TRASH_SLOT_MAX_NUM);
}

#endif
