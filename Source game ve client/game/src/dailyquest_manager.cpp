#include "stdafx.h"
#include <ctime>
#include "dailyquest_manager.h"

#include "buffer_manager.h"
#include "char_manager.h"
#include "config.h"
#include "desc.h"
#include "packet.h"
#include "utils.h"

#ifdef __DAILY_QUESTS__
inline bool time_to_tm_local(time_t t, std::tm &out)
{
#ifdef _WIN32
    return localtime_s(&out, &t) == 0;
#else
    return localtime_r(&t, &out) != nullptr;
#endif
}

inline bool is_same_day(time_t a, time_t b)
{
    std::tm ta{}, tb{};
    if (!time_to_tm_local(a, ta) || !time_to_tm_local(b, tb))
        return false;

    return ta.tm_year == tb.tm_year &&
           ta.tm_mon  == tb.tm_mon  &&
           ta.tm_mday == tb.tm_mday;
}

CDailyQuestManager::CDailyQuestManager()
{
}

CDailyQuestManager::~CDailyQuestManager()
{
}

bool CDailyQuestManager::Initialize(TDailyQuestTable* table, int32_t size)
{
	for (int32_t i = 0; i < size; ++i, ++table)
	{
		sys_log(0, "DAILY_QUESTS %d %d %d %d %d %d %d", table->id, table->type, table->vnum, table->value, table->amount, table->min_level, table->max_level);
		const auto& reward = std::make_shared<TDailyQuestTable>();
		memcpy(reward.get(), &(*table), sizeof(TDailyQuestTable));
		m_map_Daily.emplace(table->id, reward);
	}

	SetCurrentTimeStamp();

	sys_log(0, "DAILY_QUESTS: COUNT %d", m_map_Daily.size());
	return true;
}

void CDailyQuestManager::SetCurrentTimeStamp()
{
	m_date = get_global_time();
}

bool CDailyQuestManager::InitializeRewards(TDailyQuestRewardTable* table, int32_t size)
{
	for (int32_t i = 0; i < size; ++i, ++table)
	{
		const auto& reward = std::make_shared<TDailyQuestRewardTable>();
		memcpy(reward.get(), &(*table), sizeof(TDailyQuestRewardTable));
		m_map_DailyRewards.emplace(table->id, reward);
	}

	sys_log(0, "DAILY_QUEST_REWARDS: COUNT %d", m_map_DailyRewards.size());
	return true;
}

void CDailyQuestManager::ClaimReward(int8_t rewardId, LPCHARACTER ch)
{
	if (!ch->CanWarp() || ch->IsDead())
		return;

	const std::string questFlag = "daily_quest.rewards_" + std::to_string(rewardId);
	const std::string claimedFlag = "daily_quest.claimed_" + std::to_string(rewardId);
	const std::string finishedFlag = "daily_quest.finished_quest";
	const auto dailyRewardId = ch->GetQuestFlag(questFlag);

	const auto rewards = m_map_DailyRewards.find(dailyRewardId);

	const auto requiredCount = 2 + rewardId;

	if (ch->GetQuestFlag(finishedFlag) < requiredCount)
		return;

	if (ch->GetQuestFlag(claimedFlag))
		return;

	if (rewards != m_map_DailyRewards.end())
	{
		for (int i = 0; i < 6; ++i)
		{
			if (rewards->second->rewardVnum[i] > 0)
			{
				if (rewards->second->rewardVnum[i] == 1)
					ch->PointChange(POINT_GOLD, rewards->second->rewardCount[i]);
				else
					ch->AutoGiveItem(rewards->second->rewardVnum[i], rewards->second->rewardCount[i]);
			}
		}

		ch->SetQuestFlag(claimedFlag, 1);
	}
}

void CDailyQuestManager::EnterGame(LPCHARACTER ch)
{
	const std::string dayflag = "daily_quest.quest_day";
	if (is_same_day(ch->GetQuestFlag(dayflag), get_global_time()))
	{
		LoadDailyquests(ch);
		LoadDailyRewards(ch);
		return;
	}

	for (int8_t j = 0; j < 5; j++)
	{
		const std::string questFlag = "daily_quest.quest_" + std::to_string(j + 1);
		const std::string progressFlag = "daily_quest.progress_" + std::to_string(ch->GetQuestFlag(questFlag));
		ch->SetQuestFlag(questFlag, 0);
		ch->SetQuestFlag(progressFlag, 0);
	}
	for (int8_t h = 0; h < 3; h++)
	{
		const std::string questFlag = "daily_quest.rewards_" + std::to_string(h + 1);
		const std::string claimedFlag = "daily_quest.claimed_" + std::to_string(h + 1);
		ch->SetQuestFlag(questFlag, 0);
		ch->SetQuestFlag(claimedFlag, 0);
	}
	ch->SetQuestFlag("daily_quest.finished_quest", 0);

	std::vector<std::shared_ptr<TDailyQuestTable>> filteredQuests;
	for (const auto& quest : m_map_Daily)
	{
		if (ch->GetLevel() >= quest.second->min_level && ch->GetLevel() <= quest.second->max_level)
		{
			filteredQuests.emplace_back(quest.second);
		}
	}

	std::vector<std::shared_ptr<TDailyQuestRewardTable>> filteredRewards;
	for (const auto& reward : m_map_DailyRewards)
	{
		if (ch->GetLevel() >= reward.second->min_level && ch->GetLevel() <= reward.second->max_level)
		{
			filteredRewards.emplace_back(reward.second);
		}
	}

	std::vector<std::shared_ptr<TDailyQuestTable>> playerQuests;
	while (playerQuests.size() < 5 && !filteredQuests.empty())
	{
		int rndNum = number(0, filteredQuests.size() - 1);
		const auto& randomQuest = filteredQuests[rndNum];

		if (std::find_if(playerQuests.begin(), playerQuests.end(),
			[&randomQuest](const std::shared_ptr<SDailyQuestTable>& playerQuest) {
			return playerQuest->id == randomQuest->id;
		}) == playerQuests.end())
		{
			playerQuests.emplace_back(randomQuest);
		}

		filteredQuests.erase(filteredQuests.begin() + rndNum);
	}

	std::vector<std::shared_ptr<TDailyQuestRewardTable>> playerRewards;
	int8_t curCount = 0;
	const int8_t maxCount = 100;
	while (playerRewards.size() < 3 && !filteredRewards.empty())
	{
		int rndNum = number(0, filteredRewards.size() - 1);
		const auto& randomReward = filteredRewards[rndNum];

		if (std::find_if(playerRewards.begin(), playerRewards.end(),
			[&randomReward](const std::shared_ptr<TDailyQuestRewardTable>& playerReward) {
			return playerReward->id == randomReward->id;
		}) == playerRewards.end())
		{
			if (randomReward->grade == playerRewards.size() + 1)
				playerRewards.emplace_back(randomReward);
		}
		curCount++;
		if (curCount > maxCount)
			break;
	}

	int x = 1;
	for (const auto& chQuests : playerQuests)
	{
		const std::string questFlag = "daily_quest.quest_" + std::to_string(x);
		ch->SetQuestFlag(questFlag, chQuests->id);
		x++;
	}

	int i = 1;
	for (const auto& chRewards : playerRewards)
	{
		const std::string questFlag = "daily_quest.rewards_" + std::to_string(i);
		ch->SetQuestFlag(questFlag, chRewards->id);
		i++;
	}

	ch->SetQuestFlag(dayflag, get_global_time());

	LoadDailyquests(ch);
	LoadDailyRewards(ch);
}

void CDailyQuestManager::OnKill(LPCHARACTER pkKiller, LPCHARACTER ch)
{
	for (int8_t i = 0; i < 5; i++)
	{
		const std::string questFlag = "daily_quest.quest_" + std::to_string(i + 1);
		const std::string finishedFlag = "daily_quest.finished_quest";

		const auto& quest = m_map_Daily.find(pkKiller->GetQuestFlag(questFlag));
		if (quest == m_map_Daily.end())
			continue;

		const std::string flag = "daily_quest.progress_" + std::to_string(quest->first);

		bool bUpdate = false;
		bool bUpdateRewards = false;

		if (quest->second->vnum != 0 && quest->second->vnum != ch->GetRaceNum())
			continue;

		if (quest->second->type == DAILY_TYPE_KILL_STONE)
		{
			if (ch->IsStone())
			{
				const auto newFlag = pkKiller->GetQuestFlag(flag) + 1;
				if (newFlag <= quest->second->amount)
				{
					pkKiller->SetQuestFlag(flag, newFlag);
					bUpdate = true;

					if (newFlag == quest->second->amount)
					{
						pkKiller->SetQuestFlag(finishedFlag, pkKiller->GetQuestFlag(finishedFlag) + 1);
						bUpdateRewards = true;
					}
				}
			}
		}
		else if (quest->second->type == DAILY_TYPE_KILL_BOSS)
		{
			if (ch->GetMobRank() >= MOB_RANK_BOSS && !ch->IsStone())
			{
				const auto newFlag = pkKiller->GetQuestFlag(flag) + 1;
				if (newFlag <= quest->second->amount)
				{
					pkKiller->SetQuestFlag(flag, newFlag);
					bUpdate = true;

					if (newFlag == quest->second->amount)
					{
						pkKiller->SetQuestFlag(finishedFlag, pkKiller->GetQuestFlag(finishedFlag) + 1);
						bUpdateRewards = true;
					}
				}
			}
		}
		else if (quest->second->type == DAILY_TYPE_KILL_MONSTER)
		{
			if (!ch->IsPC())
			{
				const auto newFlag = pkKiller->GetQuestFlag(flag) + 1;
				if (newFlag <= quest->second->amount)
				{
					pkKiller->SetQuestFlag(flag, newFlag);
					bUpdate = true;

					if (newFlag == quest->second->amount)
					{
						pkKiller->SetQuestFlag(finishedFlag, pkKiller->GetQuestFlag(finishedFlag) + 1);
						bUpdateRewards = true;
					}
				}
			}
		}
		else if (quest->second->type == DAILY_TYPE_KILL_PLAYER)
		{
			if (ch->IsPC() && pkKiller->IsPC())
			{
				if (ch->GetLevel() >= quest->second->vnum)
				{
					bool bKilled = true;
					if (quest->second->value > 0 && ch->GetEmpire() != quest->second->value)
						bKilled = false;

					if (bKilled)
					{
						const auto newFlag = pkKiller->GetQuestFlag(flag) + 1;
						if (newFlag <= quest->second->amount)
						{
							pkKiller->SetQuestFlag(flag, newFlag);
							bUpdate = true;

							if (newFlag == quest->second->amount)
							{
								pkKiller->SetQuestFlag(finishedFlag, pkKiller->GetQuestFlag(finishedFlag) + 1);
								bUpdateRewards = true;
							}
						}
					}
				}
			}
		}

		if (bUpdate)
		{
			TPacketGCUpdateDailyQuest p;
			p.header = HEADER_GC_UPDATE_DAILY_QUESTS;
			p.id = quest->first;
			p.progress = pkKiller->GetQuestFlag(flag);
			pkKiller->GetDesc()->Packet(&p, sizeof p);
		}

		if (bUpdateRewards)
		{
			TPacketGCUpdateDailyReward p;
			p.header = HEADER_GC_UPDATE_DAILY_REWARDS;
			p.progress = pkKiller->GetQuestFlag(finishedFlag);
			pkKiller->GetDesc()->Packet(&p, sizeof p);
		}
	}
}

void CDailyQuestManager::LoadDailyquests(LPCHARACTER ch)
{
	TEMP_BUFFER buf;
	TPacketGCSendDailyQuests pack;
	pack.header = HEADER_GC_SEND_DAILY_QUESTS;
	pack.count = 0;

	if (!ch->GetDesc())
		return;

	for (int32_t x = 0; x < 5; x++)
	{
		const std::string questFlag = "daily_quest.quest_" + std::to_string(x + 1);

		const auto& quest = m_map_Daily.find(ch->GetQuestFlag(questFlag));
		if (quest == m_map_Daily.end())
			continue;

		const std::string progressFlag = "daily_quest.progress_" + std::to_string(quest->first);

		pack.count++;

		TPacketGCSendDailyQuest dailyQuest;
		dailyQuest.id = quest->second->id;
		dailyQuest.type = quest->second->type;
		dailyQuest.vnum = quest->second->vnum;
		dailyQuest.value = quest->second->value;
		dailyQuest.amount = quest->second->amount;
		dailyQuest.progress = ch->GetQuestFlag(progressFlag);
		buf.write(&dailyQuest, sizeof dailyQuest);
	}

	pack.size = sizeof pack + buf.size();

	ch->GetDesc()->BufferedPacket(&pack, sizeof pack);
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CDailyQuestManager::LoadDailyRewards(LPCHARACTER ch)
{
	TEMP_BUFFER buf;
	TPacketGCSendDailyRewards pack;
	pack.header = HEADER_GC_SEND_DAILY_REWARDS;
	pack.count = 0;

	if (!ch || !ch->GetDesc())
		return;

	for (int32_t x = 0; x < 3; x++)
	{
		const std::string questFlag = "daily_quest.rewards_" + std::to_string(x + 1);

		const auto rewardId = ch->GetQuestFlag(questFlag);

		const auto& reward = m_map_DailyRewards.find(rewardId);
		if (reward == m_map_DailyRewards.end())
			continue;

		const std::string finishedFlag = "daily_quest.finished_quest";

		pack.count++;

		TPacketGCSendDailyReward dailyReward;
		dailyReward.id = reward->second->id;
		for (int32_t i = 0; i < 6; i++)
		{
			dailyReward.rewardVnum[i] = reward->second->rewardVnum[i];
			dailyReward.rewardCount[i] = reward->second->rewardCount[i];
		}
		dailyReward.progress = ch->GetQuestFlag(finishedFlag);
		buf.write(&dailyReward, sizeof dailyReward);
	}

	pack.size = sizeof pack + buf.size();

	if (buf.size() == 0) //No entry found! No need for packet!
	{
		sys_log(0, "CDailyQuestManager::LoadDailyRewards: No rewards for character %s", ch->GetName());
		return;
	}
	ch->GetDesc()->BufferedPacket(&pack, sizeof pack);
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CDailyQuestManager::Update(int32_t iPulse)
{
	if (0 == (iPulse % PASSES_PER_SEC(5)))
	{
		if (is_same_day(m_date, get_global_time()))
			return;

		SetCurrentTimeStamp();

		CHARACTER_MANAGER::Instance().for_each_pc([&](LPCHARACTER ch) {
			EnterGame(ch);
		});
	}
}

void CDailyQuestManager::Progress(LPCHARACTER ch, uint32_t dwVnum, uint32_t type, bool checkVnum, int32_t progress)
{
	for (int8_t i = 0; i < 5; i++)
	{
		const std::string questFlag = "daily_quest.quest_" + std::to_string(i + 1);
		const std::string finishedFlag = "daily_quest.finished_quest";

		const auto& quest = m_map_Daily.find(ch->GetQuestFlag(questFlag));
		if (quest == m_map_Daily.end())
			continue;

		const std::string flag = "daily_quest.progress_" + std::to_string(quest->first);

		if (quest->second->type == type)
		{
			bool bUpdateRewards = false;
			bool bUpdate = false;

			const auto oldFlag = ch->GetQuestFlag(flag);
			auto newFlag = ch->GetQuestFlag(flag) + progress;

			if (oldFlag < quest->second->amount)
				newFlag = ch->GetQuestFlag(flag) + progress > quest->second->amount ? quest->second->amount : ch->GetQuestFlag(flag) + progress;

			if (quest->second->vnum != 0 && quest->second->vnum != dwVnum && checkVnum)
				continue;

			if (newFlag <= quest->second->amount)
			{
				ch->SetQuestFlag(flag, newFlag);
				bUpdate = true;

				if (newFlag == quest->second->amount)
				{
					ch->SetQuestFlag(finishedFlag, ch->GetQuestFlag(finishedFlag) + 1);
					bUpdateRewards = true;
				}
			}

			if (bUpdate)
			{
				TPacketGCUpdateDailyQuest p;
				p.header = HEADER_GC_UPDATE_DAILY_QUESTS;
				p.id = quest->first;
				p.progress = ch->GetQuestFlag(flag);
				ch->GetDesc()->Packet(&p, sizeof p);
			}

			if (bUpdateRewards)
			{
				TPacketGCUpdateDailyReward p;
				p.header = HEADER_GC_UPDATE_DAILY_REWARDS;
				p.progress = ch->GetQuestFlag(finishedFlag);
				ch->GetDesc()->Packet(&p, sizeof p);
			}
		}
	}
}
#endif
//martysama0134's 44d475570bf1f57835953e726ea127bf
