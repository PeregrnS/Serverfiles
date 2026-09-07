#include "StdAfx.h"
#include "PythonCharacterManager.h"
#include "PythonBackground.h"
#include "PythonPlayer.h"
#include "PythonGuild.h"
#include "fmt/fmt.h"

namespace Discord
{
	constexpr auto DiscordClientID = "1477129094029246474";

	using DCDATA = std::pair<std::string, std::string>;

	inline void ReplaceStringInPlace(std::string& subject, const std::string& search,
		const std::string& replace) {
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}
	inline void capitalizeWord(std::string& str)
	{
		bool canCapitalize = true;
		for (auto& c : str)
		{
			if (isalpha(c))
			{
				if (canCapitalize)
				{
					c = std::toupper(c);
					canCapitalize = false;
				}
			}
			else
				canCapitalize = true;
		}
	}

	/*NAME*/
	inline DCDATA GetNameData()
	{
		/*Map Name*/
		auto WarpName = std::string(CPythonBackground::Instance().GetWarpMapName());

		//atlasinfo.txt
		static const std::map<std::string, std::string> DCmapname{
			{ "metin2_map_rotes_reich_map_1", "Red Empire Map 1" },
			{ "metin2_map_rotes_reich_map_2", "Red Empire Map 2" },
			{ "metin2_map_gelbes_reich_map_1", "Yellow Empire Map 1" },
			{ "metin2_map_gelbes_reich_map_2", "Yellow Empire Map 2" },
			{ "metin2_map_blaues_reich_map_1", "Blue Empire Map 1" },
			{ "metin2_map_blaues_reich_map_2", "Blue Empire Map 2" },
			{ "metin2_map_n_desert_01", "PvP Desert" },
			{ "metin2_map_orktal", "Orktal" },
			{ "metin2_map_spiderdungeon", "Spiderdungeon" },
			{ "metin2_map_spiderdungeon_02", "Spiderdungeon 2" },
			{ "metin2_map_feuerland", "Fireland" },
			{ "metin2_map_roter_wald", "Red Forest" },
			{ "metin2_map_drachenrunstart", "Dragaon Run" },
			{ "metin2_map_pvp", "PvP Map 2" },
			{ "metin2_map_n_flame_03", "High Lvl Map 1" },
			{ "map_duester", "High Lvl Map 2" },
			{ "metin2_map_b5", "High Lvl Map 3" },
			{ "metin2_map_nephritbucht", "VIP Map" },
			{ "metin2_map_donnerberg", "Train Map" },
			{ "metin2_map_land_der_riesen", "Land of The Giants" },
			{ "metin2_map_drachenrunstart", "Red Dragaon Start" },
			{ "metin2_map_new", "Valley of The Dead" },
			{ "metin2_map_grotte_1", "Grotte 1" },
			{ "metin2_map_grotte_2", "Grotte 2" },
			{ "metin2_map_grotte_boss", "Beran Run" },
			{ "metin2_map_daemonenturm", "Demon Tower" },
			{ "metin2_map_devilscatacomb", "Red Dragon Run" },
			{ "metin2_map_eisland", "Ice Land" },
			{ "metin2_map_wueste", "Desert" },
			{ "metin2_map_tempel", "Temple" },
			{ "metin2_map_gautamakliff", "High Lvl Map 4" },
			{ "metin2_map_e1", "Metin Map 1" },
			{ "metin2_map_orktal2", "Metin Map 2" },
			{ "metin2_map_kap_des_drachenfeuers", "Metin Map 3" },
		};

		if (!DCmapname.count(WarpName))
		{
			ReplaceStringInPlace(WarpName, "season1/", "");
			ReplaceStringInPlace(WarpName, "season2/", "");
			ReplaceStringInPlace(WarpName, "metin2_map_", "");
			ReplaceStringInPlace(WarpName, "metin2_", "");
			ReplaceStringInPlace(WarpName, "plechito_", "");
			ReplaceStringInPlace(WarpName, "_", " ");
			capitalizeWord(WarpName);
		}
		auto MapName = "Location: " + (DCmapname.count(WarpName) ? DCmapname.at(WarpName) : WarpName);

		/*CH Name*/
		std::string GuildName;
		CPythonGuild::Instance().GetGuildName(CPythonPlayer::Instance().GetGuildID(), &GuildName);
		auto CHName = fmt::format("{} Lv{} {}", CPythonPlayer::Instance().GetName(), CPythonPlayer::Instance().GetStatus(POINT_LEVEL), GuildName);

		return { MapName, CHName };
	}

	/*RACE*/
	inline DCDATA GetRaceData()
	{
		auto pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (!pInstance)
			return { "","" };

		auto RACENUM = pInstance->GetRace();

		/*Image*/
		auto RaceImage = "race_" + std::to_string(RACENUM);

		/*Name*/
		auto RaceName = "Warrior";
		switch (RACENUM)
		{
		case NRaceData::JOB_ASSASSIN:
		case NRaceData::JOB_ASSASSIN + 4:
			RaceName = "Assassin";
			break;
		case NRaceData::JOB_SURA:
		case NRaceData::JOB_SURA + 4:
			RaceName = "Sura";
			break;
		case NRaceData::JOB_SHAMAN:
		case NRaceData::JOB_SHAMAN + 4:
			RaceName = "Shaman";
			break;
#if defined(ENABLE_WOLFMAN_CHARACTER)
		case NRaceData::JOB_WOLFMAN + 4:
			RaceName = "Lycan";
#endif
		}
		return { RaceImage , RaceName };
	}

	/*EMPIRE*/
	inline DCDATA GetEmpireData()
	{
		auto pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (!pInstance)
			return { "","" };

		auto EmpireID = pInstance->GetEmpireID();

		/*Image*/
		auto EmpireImage = "empire_" + std::to_string(EmpireID);

		/*Name*/
		auto EmpireName = "Shinsoo";
		switch (EmpireID)
		{
		case 2:
			EmpireName = "Chunjo";
			break;
		case 3:
			EmpireName = "Jinno";
		}
		return { EmpireImage, EmpireName };
	}
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
