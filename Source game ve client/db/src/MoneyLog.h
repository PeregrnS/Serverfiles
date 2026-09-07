// vim: ts=8 sw=4
#ifndef __INC_MONEY_LOG
#define __INC_MONEY_LOG

#include <map>

class CMoneyLog : public singleton<CMoneyLog>
{
    public:
	CMoneyLog();
	virtual ~CMoneyLog();

	void Save();
#ifdef ENABLE_GOLD_MAX_EXTENDED
	void AddLog(BYTE bType, DWORD dwVnum, unsigned long long iGold);
#else
	void AddLog(BYTE bType, DWORD dwVnum, int iGold);
#endif

    private:
	std::map<DWORD, int> m_MoneyLogContainer[MONEY_LOG_TYPE_MAX_NUM];
};

#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
