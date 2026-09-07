#pragma once

#ifndef __ANTI_MULTIPLE_FARM__
#define __ANTI_MULTIPLE_FARM__

#include "packet.h"
class CAntiMultipleFarm : public CSingleton<CAntiMultipleFarm>
{
	typedef std::vector<TAntiFarmPlayerInfo> TAntiFarmData;
	
	public:
		CAntiMultipleFarm();
		~CAntiMultipleFarm();
		
		auto	AddNewPlayerData(TAntiFarmPlayerInfo data) -> void;
		auto	ClearAntiFarmData() -> void { v_AntiFarmData.clear(); };
		auto	GetAntiFarmPlayerCount() -> int { return v_AntiFarmData.size(); }
		auto	GetAntiFarmPlayerData(int idx, TAntiFarmPlayerInfo * sPInfo) -> bool;
		auto	GetMainCharacterDropState() -> bool;
		
		/*get computer data stuff*/
		auto	GetClientMacAdress() -> const char *;
	protected:
		TAntiFarmData	v_AntiFarmData;
};

#endif // !__ANTI_MULTIPLE_FARM__
