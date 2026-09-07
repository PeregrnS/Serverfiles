// vim:ts=8 sw=4
#ifndef __INC_DB_CACHE_H__
#define __INC_DB_CACHE_H__

#include "../../common/cache.h"

class CItemCache : public cache<TPlayerItem>
{
    public:
	CItemCache();
	virtual ~CItemCache();

	void Delete();
	virtual void OnFlush();
};

class CPlayerTableCache : public cache<TPlayerTable>
{
    public:
	CPlayerTableCache();
	virtual ~CPlayerTableCache();

	virtual void OnFlush();

	DWORD GetLastUpdateTime() { return m_lastUpdateTime; }
};

// MYSHOP_PRICE_LIST

class CItemPriceListTableCache : public cache< TItemPriceListTable >
{
    public:

	/// Constructor

	CItemPriceListTableCache(void);
	virtual ~CItemPriceListTableCache();

	void	UpdateList(const TItemPriceListTable* pUpdateList);

	virtual void	OnFlush(void);

    private:

	static const int	s_nMinFlushSec;		///< Minimum cache expire time
};
// END_OF_MYSHOP_PRICE_LIST

#ifdef __PREMIUM_PRIVATE_SHOP__
class CPrivateShopCache : public cache<TPrivateShop>
{
public:
	CPrivateShopCache();
	virtual ~CPrivateShopCache();
	void Delete();
	virtual void OnFlush();

	DWORD GetLastUpdateTime() { return m_lastUpdateTime; }
};

class CPrivateShopItemCache : public cache<TPlayerPrivateShopItem>
{
public:
	CPrivateShopItemCache();
	virtual ~CPrivateShopItemCache();
	void Delete();
	virtual void OnFlush();

	DWORD GetLastUpdateTime() { return m_lastUpdateTime; }
};

class CPrivateShopSaleCache : public cache<TPrivateShopSale>
{
public:
	CPrivateShopSaleCache();
	virtual ~CPrivateShopSaleCache();
	void Delete();
	virtual void OnFlush();

	DWORD GetLastUpdateTime() { return m_lastUpdateTime; }
};
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
class CSKillColorCache : public cache<TSkillColor>
{
	public:
		CSKillColorCache();
		virtual ~CSKillColorCache();

		virtual void OnFlush();
};
#endif

#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
