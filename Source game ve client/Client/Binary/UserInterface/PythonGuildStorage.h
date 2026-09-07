#pragma once

#include "Packet.h"
#include <unordered_map>

class CPythonGuildStorage {
public:
	CPythonGuildStorage();
	~CPythonGuildStorage();

	void Init();

	void RecvGuildStoragePacket();

	void RecvGuildStorageOpenItems();
	void RecvGuildStorageOpen();
	void RecvGuildStorageClose();
	void RecvGuildStorageRemoveItem();
	void RecvGuildStorageAddItem();
	void RecvGuildStorageGoldUpdate();
	void RecvGuildStorageLogs();
	void RecvGuildStorageLogEntry();

	std::shared_ptr<TPlayerItem> GetItem(uint32_t slot)
	{
		auto item = m_guildStorageItems.find(slot);
		if (item == m_guildStorageItems.end())
			return nullptr;
		return item->second;
	}

	size_t GetLogCount()
	{
		return m_guildStorageLogs.size();
	}

	bool SendItemCheckoutRequest(uint16_t slot, TItemPos item_pos);
	bool SendItemCheckinRequest(TItemPos item_pos, uint16_t new_slot);
	bool SendItemMoveRequest(uint16_t slot, uint16_t new_slot);
	bool SendGoldCheckinRequest(long long gold);
	bool SendGoldCheckoutRequest(long long gold);
	bool SendCloseRequest();

	static CPythonGuildStorage* Instance();
	std::vector<std::shared_ptr<TGSLogEntry>> m_guildStorageLogs;
private:
	static CPythonGuildStorage * currentInstance;
	uint32_t m_currentItemCount; // REMOVE
	std::unordered_map<uint32_t, std::shared_ptr<TPlayerItem>> m_guildStorageItems;
	
};

