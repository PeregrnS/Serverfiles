#pragma once

#ifdef ENABLE_TRASH_SYSTEM

#include "GameType.h"

class CTrash
{
	public:
		CTrash();
		virtual ~CTrash() = default;

		static void SetTrashWindow(PyObject* window);
		static void RefreshWindow();
		static void AddItem(uint8_t slot, const TItemPos& item_pos);
		static void RemoveItem(uint8_t slot);
		static void GetItemInfo(uint8_t slot, TItemPos& item_pos);
		static void ClearItems();
		static bool HasItem(const TItemPos& item_pos);
		static void DeleteItems();

	private:
		static PyObject*	m_py_TrashWindow;
		static std::map<uint8_t, TItemPos> m_map_TrashItems;
};

#endif
