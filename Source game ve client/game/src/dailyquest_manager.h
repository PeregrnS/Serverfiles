#pragma once
#include "constants.h"
#include <map>
#include "char.h"

class CDailyQuestManager : public singleton<CDailyQuestManager>
{
	typedef std::map<uint32_t, std::shared_ptr<TDailyQuestTable>> TDailyQuestMap;
	typedef std::map<uint32_t, std::shared_ptr<TDailyQuestRewardTable>> TDailyQuestRewardsMap;

public:
	CDailyQuestManager();
	virtual ~CDailyQuestManager();

	void SetCurrentTimeStamp();
	bool Initialize(TDailyQuestTable* table, int32_t size);
	void OnKill(LPCHARACTER pkKiller, LPCHARACTER ch);
	void EnterGame(LPCHARACTER ch);
	void ClaimReward(int8_t rewardId, LPCHARACTER ch);
	bool InitializeRewards(TDailyQuestRewardTable* table, int32_t size);
	void LoadDailyquests(LPCHARACTER ch);
	void LoadDailyRewards(LPCHARACTER ch);
	void Update(int32_t iPulse);
	void Progress(LPCHARACTER ch, uint32_t dwVnum, uint32_t type, bool checkVnum = false, int32_t progress = 1);

private:
	TDailyQuestMap m_map_Daily;
	TDailyQuestRewardsMap m_map_DailyRewards;
	uint64_t m_date;
};
//martysama0134's 44d475570bf1f57835953e726ea127bf
