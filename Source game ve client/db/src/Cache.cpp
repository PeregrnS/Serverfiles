#include "stdafx.h"
#include "Cache.h"

#include "QID.h"
#include "ClientManager.h"
#include "Main.h"
#include <fmt/fmt.h>

extern CPacketInfo g_item_info;
extern int g_iPlayerCacheFlushSeconds;
extern int g_iItemCacheFlushSeconds;
extern int g_test_server;
// MYSHOP_PRICE_LIST
extern int g_iItemPriceListTableCacheFlushSeconds;
// END_OF_MYSHOP_PRICE_LIST
//
extern int g_item_count;

CItemCache::CItemCache()
{
	m_expireTime = MIN(1800, g_iItemCacheFlushSeconds);
}

CItemCache::~CItemCache()
{
}

// fixme
// by rtsummit
void CItemCache::Delete()
{
	if (m_data.vnum == 0)
		return;

	//char szQuery[QUERY_MAX_LEN];
	//szQuery[QUERY_MAX_LEN] = '\0';
	if (g_test_server)
		sys_log(0, "ItemCache::Delete : DELETE %u", m_data.id);

	m_data.vnum = 0;
	m_bNeedQuery = true;
	m_lastUpdateTime = time(0);
	OnFlush();

	//m_bNeedQuery = false;
}

void CItemCache::OnFlush()
{
	if (m_data.vnum == 0)
	{
		char szQuery[QUERY_MAX_LEN];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM item%s WHERE id=%u", GetTablePostfix(), m_data.id);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEM_DESTROY, 0, NULL);

		if (g_test_server)
			sys_log(0, "ItemCache::Flush : DELETE %u %s", m_data.id, szQuery);
	}
	else
	{
		TPlayerItem *p = &m_data;
		const auto setQuery = fmt::format(FMT_COMPILE("id={}, owner_id={}, `window`={}, pos={}, count={}, vnum={}, socket0={}, socket1={}, socket2={}, "
														"attrtype0={}, attrvalue0={}, "
														"attrtype1={}, attrvalue1={}, "
														"attrtype2={}, attrvalue2={}, "
														"attrtype3={}, attrvalue3={}, "
														"attrtype4={}, attrvalue4={}, "
														"attrtype5={}, attrvalue5={}, "
														"attrtype6={}, attrvalue6={} ")
														, p->id,
														p->owner,
														p->window,
														p->pos,
														p->count,
														p->vnum,
														p->alSockets[0],
														p->alSockets[1],
														p->alSockets[2],
														p->aAttr[0].bType, p->aAttr[0].sValue,
														p->aAttr[1].bType, p->aAttr[1].sValue,
														p->aAttr[2].bType, p->aAttr[2].sValue,
														p->aAttr[3].bType, p->aAttr[3].sValue,
														p->aAttr[4].bType, p->aAttr[4].sValue,
														p->aAttr[5].bType, p->aAttr[5].sValue,
														p->aAttr[6].bType, p->aAttr[6].sValue
		); // @fixme205

		const auto itemQuery = fmt::format(FMT_COMPILE("INSERT INTO item{} SET {} ON DUPLICATE KEY UPDATE {}"),
														GetTablePostfix(), setQuery, setQuery);

		if (g_test_server)
			sys_log(0, "ItemCache::Flush :REPLACE  (%s)", itemQuery.c_str());

		CDBManager::instance().ReturnQuery(itemQuery.c_str(), QID_ITEM_SAVE, 0, NULL);

		++g_item_count;
	}

	m_bNeedQuery = false;
}

//
// CPlayerTableCache
//
CPlayerTableCache::CPlayerTableCache()
{
	m_expireTime = MIN(1800, g_iPlayerCacheFlushSeconds);
}

CPlayerTableCache::~CPlayerTableCache()
{
}

void CPlayerTableCache::OnFlush()
{
	if (g_test_server)
		sys_log(0, "PlayerTableCache::Flush : %s", m_data.name);

	char szQuery[QUERY_MAX_LEN];
	CreatePlayerSaveQuery(szQuery, sizeof(szQuery), &m_data);
	CDBManager::instance().ReturnQuery(szQuery, QID_PLAYER_SAVE, 0, NULL);
}

// MYSHOP_PRICE_LIST
//
// CItemPriceListTableCache class implementation
//

const int CItemPriceListTableCache::s_nMinFlushSec = 1800;

CItemPriceListTableCache::CItemPriceListTableCache()
{
	m_expireTime = MIN(s_nMinFlushSec, g_iItemPriceListTableCacheFlushSeconds);
}

void CItemPriceListTableCache::UpdateList(const TItemPriceListTable* pUpdateList)
{
	std::vector<TItemPriceInfo> tmpvec;

	for (uint idx = 0; idx < m_data.byCount; ++idx)
	{
		const TItemPriceInfo* pos = pUpdateList->aPriceInfo;
		for (; pos != pUpdateList->aPriceInfo + pUpdateList->byCount && m_data.aPriceInfo[idx].dwVnum != pos->dwVnum; ++pos)
			;

		if (pos == pUpdateList->aPriceInfo + pUpdateList->byCount)
			tmpvec.emplace_back(m_data.aPriceInfo[idx]);
	}

	if (pUpdateList->byCount > SHOP_PRICELIST_MAX_NUM)
	{
		sys_err("Count overflow!");
		return;
	}

	m_data.byCount = pUpdateList->byCount;

	thecore_memcpy(m_data.aPriceInfo, pUpdateList->aPriceInfo, sizeof(TItemPriceInfo) * pUpdateList->byCount);

	int nDeletedNum;

	if (pUpdateList->byCount < SHOP_PRICELIST_MAX_NUM)
	{
		size_t sizeAddOldDataSize = SHOP_PRICELIST_MAX_NUM - pUpdateList->byCount;

		if (tmpvec.size() < sizeAddOldDataSize)
			sizeAddOldDataSize = tmpvec.size();
		if (tmpvec.size() != 0)
		{
			thecore_memcpy(m_data.aPriceInfo + pUpdateList->byCount, &tmpvec[0], sizeof(TItemPriceInfo) * sizeAddOldDataSize);
			m_data.byCount += sizeAddOldDataSize;
		}
		nDeletedNum = tmpvec.size() - sizeAddOldDataSize;
	}
	else
		nDeletedNum = tmpvec.size();

	m_bNeedQuery = true;

	sys_log(0,
			"ItemPriceListTableCache::UpdateList : OwnerID[%u] Update [%u] Items, Delete [%u] Items, Total [%u] Items",
			m_data.dwOwnerID, pUpdateList->byCount, nDeletedNum, m_data.byCount);
}

void CItemPriceListTableCache::OnFlush()
{
	char szQuery[QUERY_MAX_LEN];

	snprintf(szQuery, sizeof(szQuery), "DELETE FROM myshop_pricelist%s WHERE owner_id = %u", GetTablePostfix(), m_data.dwOwnerID);
	CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_DESTROY, 0, NULL);

	for (int idx = 0; idx < m_data.byCount; ++idx)
	{
		snprintf(szQuery, sizeof(szQuery),
#ifdef ENABLE_GOLD_MAX_EXTENDED
				"REPLACE myshop_pricelist%s(owner_id, item_vnum, price) VALUES(%u, %u, %llu)", // @fixme204 (INSERT INTO -> REPLACE)
#else
				"REPLACE myshop_pricelist%s(owner_id, item_vnum, price) VALUES(%u, %u, %u)", // @fixme204 (INSERT INTO -> REPLACE)
#endif
				GetTablePostfix(), m_data.dwOwnerID, m_data.aPriceInfo[idx].dwVnum, m_data.aPriceInfo[idx].dwPrice);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_SAVE, 0, NULL);
	}

	sys_log(0, "ItemPriceListTableCache::Flush : OwnerID[%u] Update [%u]Items", m_data.dwOwnerID, m_data.byCount);

	m_bNeedQuery = false;
}

CItemPriceListTableCache::~CItemPriceListTableCache()
{
}

// END_OF_MYSHOP_PRICE_LIST

#ifdef __PREMIUM_PRIVATE_SHOP__
CPrivateShopCache::CPrivateShopCache()
{
	m_expireTime = MIN(1800, g_iItemCacheFlushSeconds);
}

CPrivateShopCache::~CPrivateShopCache()
{
}

void CPrivateShopCache::Delete()
{
	if (m_data.dwVnum == 0)
		return;

	if (g_test_server)
		sys_log(0, "CPrivateShopCache::Delete : DELETE %u", m_data.dwOwner);

	m_data.dwVnum = 0;
	m_bNeedQuery = true;
	m_lastUpdateTime = time(0);
	OnFlush();
}

void CPrivateShopCache::OnFlush()
{
	char szQuery[1024];

	if (m_data.dwVnum == 0)
	{
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM private_shop%s WHERE owner_id=%u", GetTablePostfix(), m_data.dwOwner);
		CDBManager::instance().ReturnQuery(szQuery, QID_PRIVATE_SHOP_DELETE, 0, NULL);

		if (g_test_server)
			sys_log(0, "CPrivateShopCache::Flush : DELETE %u %s", m_data.dwOwner, szQuery);
	}
	else
	{
		char szEscapedTitle[TITLE_MAX_LEN * 2 + 1]{ 0 };
		CDBManager::instance().EscapeString(szEscapedTitle, m_data.szTitle, strlen(m_data.szTitle));

		size_t queryLen;
		queryLen = snprintf(szQuery, sizeof(szQuery),
			"REPLACE INTO private_shop%s SET "
			"owner_id = %u, "
			"owner_name = '%s', "
			"state = %u, "
			"title = '%s', "
			"title_type = %u, "
			"vnum = %u, "
			"x = %d, "
			"y = %d, "
			"map_index = %u, "
			"channel = %u, "
			"port = %u, "
			"gold = %lld, "
			"cheque = %u, "
			"page_count = %u, "
			"premium_time = %u, "
			"unlocked_slots = %u ",

			GetTablePostfix(),
			m_data.dwOwner,
			m_data.szOwnerName,
			m_data.bState,
			szEscapedTitle,
			m_data.bTitleType,
			m_data.dwVnum,
			m_data.lX,
			m_data.lY,
			m_data.lMapIndex,
			m_data.bChannel,
			m_data.wPort,
			m_data.llGold,
			m_data.dwCheque,
			m_data.bPageCount,
			m_data.tPremiumTime,
			m_data.wUnlockedSlots
		);

		CDBManager::instance().ReturnQuery(szQuery, QID_PRIVATE_SHOP_SAVE, 0, NULL);
		m_bNeedQuery = false;
	}
}

CPrivateShopItemCache::CPrivateShopItemCache()
{
	m_expireTime = MIN(1800, g_iItemCacheFlushSeconds);
}

CPrivateShopItemCache::~CPrivateShopItemCache()
{
}

void CPrivateShopItemCache::Delete()
{
	if (m_data.dwVnum == 0)
		return;

	if (g_test_server)
		sys_log(0, "CPrivateShopItemCache::Delete : DELETE %u", m_data.dwOwner);

	m_data.dwVnum = 0;
	m_bNeedQuery = true;
	m_lastUpdateTime = time(0);
	OnFlush();
}

void CPrivateShopItemCache::OnFlush()
{
	if (m_data.dwVnum == 0)
	{
		char szQuery[128 + 1];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM private_shop_item%s WHERE id = %u", GetTablePostfix(), m_data.dwID);
		CDBManager::instance().ReturnQuery(szQuery, QID_PRIVATE_SHOP_ITEM_DELETE, 0, NULL);

		if (g_test_server)
			sys_log(0, "CPrivateShopItemCache::Flush : DELETE %u %s", m_data.dwOwner, szQuery);
	}
	else
	{
		long alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
		TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
		bool isApplyRandom = false;
#endif
		bool isSocket = false, isAttr = false;

		memset(&alSockets, 0, sizeof(long) * ITEM_SOCKET_MAX_NUM);
		memset(&aAttr, 0, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM);
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
		memset(&aApplyRandom, 0, sizeof(TPlayerItemAttribute) * ITEM_APPLY_MAX_NUM);
#endif

		TPlayerPrivateShopItem* p = &m_data;

		if (memcmp(alSockets, p->alSockets, sizeof(long) * ITEM_SOCKET_MAX_NUM))
			isSocket = true;

		if (memcmp(aAttr, p->aAttr, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM))
			isAttr = true;

#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
		if (memcmp(aApplyRandom, p->aApplyRandom, sizeof(TPlayerItemAttribute) * ITEM_APPLY_MAX_NUM))
			isApplyRandom = true;
#endif

		char szColumns[2048];
		char szValues[2048];

		int iLen = snprintf(szColumns, sizeof(szColumns),
			"id, owner_id, pos, count, vnum, gold, cheque, checkin"
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
			", transmutation"
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
			", refine_element"
#endif
		);

		int iValueLen = snprintf(szValues, sizeof(szValues),
			"%u, %u, %u, %u, %u, %lld, %u, %u"
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
			", %u"
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
			", %u"
#endif
			, p->dwID, p->dwOwner, p->wPos, p->dwCount, p->dwVnum, p->TPrice.llGold, p->TPrice.dwCheque, p->tCheckin
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
			, p->dwTransmutationVnum
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
			, p->dwRefineElement
#endif
		);

		if (isSocket)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
				", socket0"
				", socket1"
				", socket2"
#ifdef ENABLE_PRIVATE_SHOP_SOCKET5
				", socket3"
				", socket4"
#endif
			);
			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
				", %lu"
				", %lu"
				", %lu"
#ifdef ENABLE_PRIVATE_SHOP_SOCKET5
				", %lu"
				", %lu"
#endif
				, p->alSockets[0]
				, p->alSockets[1]
				, p->alSockets[2]
#ifdef ENABLE_PRIVATE_SHOP_SOCKET5
				, p->alSockets[3]
				, p->alSockets[4]
#endif
			);
		}

#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
		if (isApplyRandom)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
				", apply_type0, apply_value0, apply_path0"
				", apply_type1, apply_value1, apply_path1"
				", apply_type2, apply_value2, apply_path2"
				", apply_type3, apply_value3, apply_path3"
			);

			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
				", %d, %d, %d"
				", %d, %d, %d"
				", %d, %d, %d"
				", %d, %d, %d"
				, p->aApplyRandom[0].bType, p->aApplyRandom[0].sValue, p->aApplyRandom[0].bPath
				, p->aApplyRandom[1].bType, p->aApplyRandom[1].sValue, p->aApplyRandom[1].bPath
				, p->aApplyRandom[2].bType, p->aApplyRandom[2].sValue, p->aApplyRandom[2].bPath
				, p->aApplyRandom[3].bType, p->aApplyRandom[3].sValue, p->aApplyRandom[3].bPath
			);
		}
#endif

		if (isAttr)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
				", attrtype0, attrvalue0"
				", attrtype1, attrvalue1"
				", attrtype2, attrvalue2"
				", attrtype3, attrvalue3"
				", attrtype4, attrvalue4"
				", attrtype5, attrvalue5"
				", attrtype6, attrvalue6"
			);

			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
				", %d, %d"
				", %d, %d"
				", %d, %d"
				", %d, %d"
				", %d, %d"
				", %d, %d"
				", %d, %d"
				, p->aAttr[0].bType, p->aAttr[0].sValue
				, p->aAttr[1].bType, p->aAttr[1].sValue
				, p->aAttr[2].bType, p->aAttr[2].sValue
				, p->aAttr[3].bType, p->aAttr[3].sValue
				, p->aAttr[4].bType, p->aAttr[4].sValue
				, p->aAttr[5].bType, p->aAttr[5].sValue
				, p->aAttr[6].bType, p->aAttr[6].sValue
			);
		}

		char szItemQuery[4096 + 128];
		snprintf(szItemQuery, sizeof(szItemQuery), "REPLACE INTO private_shop_item%s (%s) VALUES(%s)", GetTablePostfix(), szColumns, szValues);

		if (g_test_server)
			sys_log(0, "ItemCache::Flush :REPLACE  (%s)", szItemQuery);

		CDBManager::instance().ReturnQuery(szItemQuery, QID_PRIVATE_SHOP_ITEM_SAVE, 0, NULL);
		m_bNeedQuery = false;
	}
}

CPrivateShopSaleCache::CPrivateShopSaleCache()
{
	m_expireTime = MIN(1800, g_iItemCacheFlushSeconds);
}

CPrivateShopSaleCache::~CPrivateShopSaleCache()
{
}

void CPrivateShopSaleCache::Delete()
{
}

void CPrivateShopSaleCache::OnFlush()
{
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	bool isSocket = false, isAttr = false;

#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
	bool isApplyRandom = false;
#endif

	memset(&alSockets, 0, sizeof(long) * ITEM_SOCKET_MAX_NUM);
	memset(&aAttr, 0, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM);
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	memset(&aApplyRandom, 0, sizeof(TPlayerItemAttribute) * ITEM_APPLY_MAX_NUM);
#endif

	TPrivateShopSale* p = &m_data;

	if (memcmp(alSockets, p->TItem.alSockets, sizeof(long) * ITEM_SOCKET_MAX_NUM))
		isSocket = true;

	if (memcmp(aAttr, p->TItem.aAttr, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM))
		isAttr = true;

#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	if (memcmp(aApplyRandom, p->TItem.aApplyRandom, sizeof(TPlayerItemAttribute) * ITEM_APPLY_MAX_NUM))
		isApplyRandom = true;
#endif

	char szColumns[2048];
	char szValues[2048];

	int iLen = snprintf(szColumns, sizeof(szColumns), "id, owner_id, customer_id, customer_name, time, item_id, count, vnum, gold, cheque, checkin"
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
		", transmutation"
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
		", refine_element"
#endif
	);

	int iValueLen = snprintf(szValues, sizeof(szValues), "%u, %u, %u, '%s', FROM_UNIXTIME(%u), %u, %u, %u, %lld, %u, %u"
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
		", %u"
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
		", %u"
#endif
		, p->dwID, p->dwOwner, p->dwCustomer, p->szCustomerName, p->tTime, p->TItem.dwID, p->TItem.dwCount,
		p->TItem.dwVnum, p->TItem.TPrice.llGold, p->TItem.TPrice.dwCheque, p->TItem.tCheckin
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
		, p->TItem.dwTransmutationVnum
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
		, p->TItem.dwRefineElement
#endif
	);

	if (isSocket)
	{
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", socket0, socket1, socket2 "
#ifdef ENABLE_PRIVATE_SHOP_SOCKET5
			", socket3"
			", socket4"
#endif
		);

		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
			", %lu, %lu, %lu"
#ifdef ENABLE_PRIVATE_SHOP_SOCKET5
			", %lu"
			", %lu"
#endif
			, p->TItem.alSockets[0], p->TItem.alSockets[1], p->TItem.alSockets[2]
#ifdef ENABLE_PRIVATE_SHOP_SOCKET5
			, p->TItem.alSockets[3]
			, p->TItem.alSockets[4]
#endif
		);
	}

	if (isAttr)
	{
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
			", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3"
			", attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6");

		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
			", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
			p->TItem.aAttr[0].bType, p->TItem.aAttr[0].sValue,
			p->TItem.aAttr[1].bType, p->TItem.aAttr[1].sValue,
			p->TItem.aAttr[2].bType, p->TItem.aAttr[2].sValue,
			p->TItem.aAttr[3].bType, p->TItem.aAttr[3].sValue,
			p->TItem.aAttr[4].bType, p->TItem.aAttr[4].sValue,
			p->TItem.aAttr[5].bType, p->TItem.aAttr[5].sValue,
			p->TItem.aAttr[6].bType, p->TItem.aAttr[6].sValue);
	}

#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	if (isApplyRandom)
	{
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
			", apply_type0, apply_value0, apply_path0"
			", apply_type1, apply_value1, apply_path1"
			", apply_type2, apply_value2, apply_path2"
			", apply_type3, apply_value3, apply_path3"
		);

		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
			", %d, %d, %d"
			", %d, %d, %d"
			", %d, %d, %d"
			", %d, %d, %d"
			, p->TItem.aApplyRandom[0].bType, p->TItem.aApplyRandom[0].sValue, p->TItem.aApplyRandom[0].bPath
			, p->TItem.aApplyRandom[1].bType, p->TItem.aApplyRandom[1].sValue, p->TItem.aApplyRandom[1].bPath
			, p->TItem.aApplyRandom[2].bType, p->TItem.aApplyRandom[2].sValue, p->TItem.aApplyRandom[2].bPath
			, p->TItem.aApplyRandom[3].bType, p->TItem.aApplyRandom[3].sValue, p->TItem.aApplyRandom[3].bPath
		);
	}
#endif

	char szItemQuery[4096 + 128];
	snprintf(szItemQuery, sizeof(szItemQuery), "REPLACE INTO private_shop_sale_history%s (%s) VALUES(%s)", GetTablePostfix(), szColumns, szValues);

	if (g_test_server)
		sys_log(0, "ItemCache::Flush :REPLACE  (%s)", szItemQuery);

	CDBManager::instance().ReturnQuery(szItemQuery, QID_PRIVATE_SHOP_ITEM_SAVE, 0, NULL);
	m_bNeedQuery = false;
}
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
extern int g_iSkillColorCacheFlushSeconds;

CSKillColorCache::CSKillColorCache()
{
	m_expireTime = MIN(1800, g_iSkillColorCacheFlushSeconds);
}

CSKillColorCache::~CSKillColorCache()
{
}

void CSKillColorCache::OnFlush()
{
	char query[QUERY_MAX_LEN];
	snprintf(query, sizeof(query),
		"REPLACE INTO skill_color%s (`player_id`, "
		"`s1_col1`, `s1_col2`, `s1_col3`, `s1_col4`, `s1_col5`, "
		"`s2_col1`, `s2_col2`, `s2_col3`, `s2_col4`, `s2_col5`, "
		"`s3_col1`, `s3_col2`, `s3_col3`, `s3_col4`, `s3_col5`, "
		"`s4_col1`, `s4_col2`, `s4_col3`, `s4_col4`, `s4_col5`, "
		"`s5_col1`, `s5_col2`, `s5_col3`, `s5_col4`, `s5_col5`, "
		"`s6_col1`, `s6_col2`, `s6_col3`, `s6_col4`, `s6_col5`, " // end of skills
		"`s7_col1`, `s7_col2`, `s7_col3`, `s7_col4`, `s7_col5`, " // begin buffs
		"`s8_col1`, `s8_col2`, `s8_col3`, `s8_col4`, `s8_col5`, "
		"`s9_col1`, `s9_col2`, `s9_col3`, `s9_col4`, `s9_col5`, "
		"`s10_col1`, `s10_col2`, `s10_col3`, `s10_col4`, `s10_col5`, "
		"`s11_col1`, `s11_col2`, `s11_col3`, `s11_col4`, `s11_col5`) "
		"VALUES (%d, "
		"%d, %d, %d, %d, %d, "
		"%d, %d, %d, %d, %d, "
		"%d, %d, %d, %d, %d, "
		"%d, %d, %d, %d, %d, "
		"%d, %d, %d, %d, %d, "
		"%d, %d, %d, %d, %d, " // end of skills
		"%d, %d, %d, %d, %d, " // begin buffs
		"%d, %d, %d, %d, %d, "
		"%d, %d, %d, %d, %d, "
		"%d, %d, %d, %d, %d, "
		"%d, %d, %d, %d, %d)", GetTablePostfix(), m_data.player_id,
		m_data.dwSkillColor[0][0], m_data.dwSkillColor[0][1], m_data.dwSkillColor[0][2], m_data.dwSkillColor[0][3], m_data.dwSkillColor[0][4],
		m_data.dwSkillColor[1][0], m_data.dwSkillColor[1][1], m_data.dwSkillColor[1][2], m_data.dwSkillColor[1][3], m_data.dwSkillColor[1][4],
		m_data.dwSkillColor[2][0], m_data.dwSkillColor[2][1], m_data.dwSkillColor[2][2], m_data.dwSkillColor[2][3], m_data.dwSkillColor[2][4],
		m_data.dwSkillColor[3][0], m_data.dwSkillColor[3][1], m_data.dwSkillColor[3][2], m_data.dwSkillColor[3][3], m_data.dwSkillColor[3][4],
		m_data.dwSkillColor[4][0], m_data.dwSkillColor[4][1], m_data.dwSkillColor[4][2], m_data.dwSkillColor[4][3], m_data.dwSkillColor[4][4],
		m_data.dwSkillColor[5][0], m_data.dwSkillColor[5][1], m_data.dwSkillColor[5][2], m_data.dwSkillColor[5][3], m_data.dwSkillColor[5][4], // end of skills
		m_data.dwSkillColor[6][0], m_data.dwSkillColor[6][1], m_data.dwSkillColor[6][2], m_data.dwSkillColor[6][3], m_data.dwSkillColor[6][4], // begin buffs
		m_data.dwSkillColor[7][0], m_data.dwSkillColor[7][1], m_data.dwSkillColor[7][2], m_data.dwSkillColor[7][3], m_data.dwSkillColor[7][4],
		m_data.dwSkillColor[8][0], m_data.dwSkillColor[8][1], m_data.dwSkillColor[8][2], m_data.dwSkillColor[8][3], m_data.dwSkillColor[8][4],
		m_data.dwSkillColor[9][0], m_data.dwSkillColor[9][1], m_data.dwSkillColor[9][2], m_data.dwSkillColor[9][3], m_data.dwSkillColor[9][4],
		m_data.dwSkillColor[10][0], m_data.dwSkillColor[10][1], m_data.dwSkillColor[10][2], m_data.dwSkillColor[10][3], m_data.dwSkillColor[10][4]);

	CDBManager::instance().ReturnQuery(query, QID_SKILL_COLOR_SAVE, 0, NULL);

	if (g_test_server)
		sys_log(0, "SkillColorCache::Flush :REPLACE %u (%s)", m_data.player_id, query);

	m_bNeedQuery = false;
}
#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
