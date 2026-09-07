#ifndef __INC_GuildStorageManager_H__
#define __INC_GuildStorageManager_H__


//#include "../../common/stl.h"
//#include "../../common/length.h"
#include "../../common/tables.h"
//#include "packet.h"
//#include "db.h"

#include <unordered_map>
#include <map>
//#include "vid.h"
#include "buffer_manager.h"
#include "char.h"
//15*8

class GuildStorageManager : public singleton<GuildStorageManager>
{
public:
	typedef struct SGAME_guildStorage
	{
		long long gold;
		std::unordered_map<uint16_t, std::shared_ptr<TPlayerItem>> m_items;
		std::unordered_map<uint32_t, LPCHARACTER> viewers;
		std::vector<uint16_t> lockedSlots;
		std::vector<std::shared_ptr<TGSLogEntry>> m_logs;

		SGAME_guildStorage()
		{
			viewers.clear();
			m_items.clear();
			m_logs.clear();
			lockedSlots.clear();
			gold = 0;
		}

	} guildStorage;
	
	GuildStorageManager();
	~GuildStorageManager();

	void Initialize();

private:
	//Viewer
	bool AddViewer(uint32_t guildId, LPCHARACTER ch);
	bool RemoveViewer(uint32_t guildId, LPCHARACTER ch);

public:
	//Basic
	bool OpenGuildStorage(LPCHARACTER ch);
	bool CloseGuildStorage(LPCHARACTER ch);
	bool IsLoadedGuildStorage(uint32_t guildId);

	bool AddItem(LPCHARACTER ch, TItemPos slot, uint16_t new_slot);
	bool RemoveItem(LPCHARACTER ch, int16_t slot, TItemPos new_slot);
	bool MoveItem(LPCHARACTER ch, uint16_t slot, uint16_t new_slot);

	bool AddGold(LPCHARACTER ch, long long gold);
	bool RemoveGold(LPCHARACTER ch, long long gold);
		
	//P2P
	void RecvSlots(uint32_t guildId, std::vector<TPlayerItem>* items);
	void RecvSlotLock(uint32_t guildId, uint16_t slot);
	void RecvSlotUnlock(uint32_t guildId, uint16_t slot);
	void RecvItemAdd(uint32_t guildId, TPlayerItem item);
	void RecvItemRemove(uint32_t guildId, uint16_t slot);
	void RecvGoldUpdate(uint32_t guildId, long long gold, uint32_t player_id, long long player_gold);
	void RecvLogs(uint32_t guildId, std::vector<TGSLogEntry>* items);
	void RecvLogEntry(uint32_t guildId, TGSLogEntry* log);


	void RecvRequestedItemRemove(uint32_t guildId, uint32_t player_id, uint16_t slot, TItemPos itemPos);
	
protected:
	std::unordered_map<uint32_t, std::shared_ptr<guildStorage>> guildIdToStorage;
	std::vector<LPCHARACTER> waitingCharQueue;

private:
	std::shared_ptr<guildStorage> GetGuildStorage(uint32_t guildId)
	{
		const auto gs = guildIdToStorage.find(guildId);
		if (gs == guildIdToStorage.end())
			return nullptr;

		return gs->second;
	}
	
public:
	bool HasViewersGuildID(uint32_t guildId)
	{
		const auto gs = guildIdToStorage.find(guildId);
		if (gs == guildIdToStorage.end())
			return false;
		
		if (!gs->second || gs->second->viewers.size() == 0)
			return false;
		
		return true;
	}
};


#endif

