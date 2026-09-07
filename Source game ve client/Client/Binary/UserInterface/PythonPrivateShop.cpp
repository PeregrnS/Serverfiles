#include "StdAfx.h"
#include "PythonPrivateShop.h"
#include "PythonNetworkStream.h"

CPythonPrivateShop::CPythonPrivateShop()
{
	Clear();
	ClearMyPrivateShop();

	m_pSelectedInstance = nullptr;
	m_bResultPage = 0;
}

CPythonPrivateShop::~CPythonPrivateShop()
{
}

struct ItemStockSortFunc
{
	bool operator() (TPrivateShopItem& rkLeft, TPrivateShopItem& rkRight)
	{
		return rkLeft.wDisplayPos < rkRight.wDisplayPos;
	}
};

void CPythonPrivateShop::ClearMyPrivateShop()
{
	m_llGold = 0;
	m_dwCheque = 0;
	m_lX = 0;
	m_lY = 0;
	m_strMyTitle = "";
	m_bMyState = STATE_CLOSED;
	m_bMyPageCount = 0;

	memset(m_aMyPrivateShopItem, 0, sizeof(m_aMyPrivateShopItem));
	m_map_privateShopItemStock.clear();
}

void CPythonPrivateShop::Clear()
{
	m_strTitle = "";
	m_bState = STATE_CLOSED;
	m_bPageCount = 0;
	m_wUnlockedSlots = 0;
	m_dwActiveVID = 0;

	memset(m_aPrivateShopItem, 0, sizeof(m_aPrivateShopItem));

	m_bIsMainPlayerPrivateShop = false;
	m_bIsMarketPriceDataLoaded = false;
}

void CPythonPrivateShop::BuildPrivateShop(const char* c_szName, DWORD dwPolyVnum, BYTE bTitleType, BYTE bPageCount)
{
	std::vector<TPrivateShopItem> vec_itemStock;
	vec_itemStock.reserve(m_map_privateShopItemStock.size());

	for (auto it = m_map_privateShopItemStock.begin(); it != m_map_privateShopItemStock.end(); ++it)
	{
		vec_itemStock.push_back(it->second);
	}

	std::sort(vec_itemStock.begin(), vec_itemStock.end(), ItemStockSortFunc());

	CPythonNetworkStream::Instance().SendBuildPrivateShopPacket(c_szName, dwPolyVnum, bTitleType, bPageCount, vec_itemStock);

	// Do not clear the stock until we receive positive response from server
	// ClearPrivateShopStock();
}

void CPythonPrivateShop::ClearPrivateShopStock()
{
	m_map_privateShopItemStock.clear();
}
void CPythonPrivateShop::AddPrivateShopItemStock(TItemPos ItemPos, WORD wDisplayPos, long long llPrice, DWORD dwCheque)
{
	DelPrivateShopItemStock(ItemPos);

	TPrivateShopItem sellingItem;
	sellingItem.TPos = ItemPos;
	sellingItem.llGold = llPrice;
	sellingItem.dwCheque = dwCheque;
	sellingItem.wDisplayPos = wDisplayPos;

	m_map_privateShopItemStock.emplace(ItemPos, sellingItem);
}
void CPythonPrivateShop::DelPrivateShopItemStock(TItemPos ItemPos)
{
	if (m_map_privateShopItemStock.end() == m_map_privateShopItemStock.find(ItemPos))
		return;

	m_map_privateShopItemStock.erase(ItemPos);
}

void CPythonPrivateShop::GetPrivateShopItemPrice(TItemPos ItemPos, long long& r_llGold, DWORD& r_dwCheque)
{
	auto it = m_map_privateShopItemStock.find(ItemPos);

	if (m_map_privateShopItemStock.end() == it)
	{
		r_llGold = 0;
		r_dwCheque = 0;
		return;
	}

	const TPrivateShopItem& c_rShopItem = it->second;

	r_llGold = c_rShopItem.llGold;
	r_dwCheque = c_rShopItem.dwCheque;
}

long long CPythonPrivateShop::GetTotalStockGold()
{
	long long llTotalGold = 0;

	for (const auto& kv : m_map_privateShopItemStock)
		llTotalGold += kv.second.llGold;

	return llTotalGold;
}

DWORD CPythonPrivateShop::GetTotalStockCheque()
{
	DWORD dwTotalCheque = 0;

	for (const auto& kv : m_map_privateShopItemStock)
		dwTotalCheque += kv.second.dwCheque;

	return dwTotalCheque;
}

void CPythonPrivateShop::SetItemData(const TPrivateShopItemData& c_rShopItemData, bool bIsMainPlayerPrivateShop /* = false */)
{ 
	if (c_rShopItemData.wPos >= PRIVATE_SHOP_HOST_ITEM_MAX_NUM)
		return;

	// Set item data based on shop type
	if (bIsMainPlayerPrivateShop)
		m_aMyPrivateShopItem[c_rShopItemData.wPos] = c_rShopItemData;
	else
		m_aPrivateShopItem[c_rShopItemData.wPos] = c_rShopItemData; 
}
bool CPythonPrivateShop::GetItemData(WORD wPos, const TPrivateShopItemData** c_ppItemData, bool bIsMainPlayerPrivateShop)
{
	if (wPos >= PRIVATE_SHOP_HOST_ITEM_MAX_NUM)
		return false;

	// Select item data based on shop type
	if(bIsMainPlayerPrivateShop)
		*c_ppItemData = &m_aMyPrivateShopItem[wPos];
	else
		*c_ppItemData = &m_aPrivateShopItem[wPos];

	return true;
}

void CPythonPrivateShop::RemoveItemData(WORD wPos, bool bIsMainPlayerPrivateShop)
{
	if (wPos >= PRIVATE_SHOP_HOST_ITEM_MAX_NUM)
		return;

	if (bIsMainPlayerPrivateShop)
		memset(&m_aMyPrivateShopItem[wPos], 0, sizeof(TPrivateShopItemData));
	else
		memset(&m_aPrivateShopItem[wPos], 0, sizeof(TPrivateShopItemData));
}

void CPythonPrivateShop::ChangeItemPrice(WORD wPos, long long llGold, DWORD dwCheque)
{
	TPrivateShopItemData* pItemData;

	if (IsMainPlayerPrivateShop())
		pItemData = &m_aMyPrivateShopItem[wPos];
	else
		pItemData = &m_aPrivateShopItem[wPos];

	if (pItemData)
	{
		pItemData->TPrice.llGold = llGold;
		pItemData->TPrice.dwCheque = dwCheque;
	}
}

void CPythonPrivateShop::MoveItem(WORD wPos, WORD wChangePos)
{
	bool bIsMainPlayerPrivateShop = IsMainPlayerPrivateShop();

	if (bIsMainPlayerPrivateShop)
	{
		memcpy(&m_aMyPrivateShopItem[wChangePos], &m_aMyPrivateShopItem[wPos], sizeof(TPrivateShopItemData));
		RemoveItemData(wPos, bIsMainPlayerPrivateShop);
	}
	else
	{
		memcpy(&m_aPrivateShopItem[wChangePos], &m_aPrivateShopItem[wPos], sizeof(TPrivateShopItemData));
		RemoveItemData(wPos, bIsMainPlayerPrivateShop);
	}
}

long long CPythonPrivateShop::GetTotalGold()
{
	long long llTotalGold = GetGold();	// Gold in the stash

	for (const auto& rShopItem : m_aMyPrivateShopItem)
		llTotalGold += rShopItem.TPrice.llGold;

	return llTotalGold;
}

DWORD CPythonPrivateShop::GetTotalCheque()
{
	DWORD dwTotalCheque = GetCheque();	// Cheque in the stash

	for (const auto& rShopItem : m_aMyPrivateShopItem)
		dwTotalCheque += rShopItem.TPrice.dwCheque;

	return dwTotalCheque;
}

void CPythonPrivateShop::ClearSearchResult()
{
	m_vec_searchItem.clear();
	m_bResultPage = 0;
}

void CPythonPrivateShop::SortSearchResult()
{
#ifdef ENABLE_PRIVATE_SHOP_PRICE_SORT
	static auto currency_sort_func = [](TPrivateShopSearchData& rkLeft, TPrivateShopSearchData& rkRight)
	{
		if (rkLeft.TPrice.dwCheque != rkRight.TPrice.dwCheque)
			return rkLeft.TPrice.dwCheque < rkRight.TPrice.dwCheque;

		return rkLeft.TPrice.llGold < rkRight.TPrice.llGold;
	};

	std::sort(m_vec_searchItem.begin(), m_vec_searchItem.end(), currency_sort_func);
#endif

#ifdef ENABLE_PRIVATE_SHOP_TIME_SORT
	static auto time_sort_func = [](TPrivateShopSearchData& rkLeft, TPrivateShopSearchData& rkRight)
	{
		return rkLeft.tCheckin < rkRight.tCheckin;
	};

	std::sort(m_vec_searchItem.begin(), m_vec_searchItem.end(), time_sort_func);
#endif
}

void CPythonPrivateShop::SetSearchItemData(TPrivateShopSearchData& rSearchItem)
{
	m_vec_searchItem.push_back(rSearchItem);
}

bool CPythonPrivateShop::GetSearchItemData(WORD wIndex, const TPrivateShopSearchData** c_pSearchItem)
{
	if (wIndex >= m_vec_searchItem.size())
		return false;

	*c_pSearchItem = &m_vec_searchItem.at(wIndex);

	return true;
}

int CPythonPrivateShop::GetSearchItemDataPos(DWORD dwShopID, WORD wPos)
{
	int i = 0;
	for (const auto& rSearchItem : m_vec_searchItem)
	{
		if (rSearchItem.dwShopID == dwShopID &&
			rSearchItem.wPos == wPos)
			return i;

		++i;
	}

	return -1;
}

void CPythonPrivateShop::GetSearchItemDataPos(DWORD dwShopID, std::vector<WORD>& vecPositions)
{
	for (size_t i = 0; i < m_vec_searchItem.size(); ++i)
	{
		TPrivateShopSearchData& rSearchItem = m_vec_searchItem.at(i);
		if (rSearchItem.dwShopID == dwShopID)
			vecPositions.push_back(i);
	}
}

void CPythonPrivateShop::SetSearchItemDataState(DWORD dwShopID, BYTE bState, int iPos /*= -1*/)
{
	for (auto& rSearchItem : m_vec_searchItem)
	{
		if (rSearchItem.dwShopID != dwShopID)
			continue;

		if (iPos > -1 && iPos != rSearchItem.wPos)
			continue;

		rSearchItem.bState = bState;

		if (iPos > -1)
			break;
	}
}

void CPythonPrivateShop::SetSaleItemData(TPrivateShopSaleData& rSale)
{
	m_vec_saleItem.push_back(rSale);

	static auto sort_func = [](TPrivateShopSaleData& rkLeft, TPrivateShopSaleData& rkRight)
	{
		return rkLeft.tTime < rkRight.tTime;
	};

	std::sort(m_vec_saleItem.begin(), m_vec_saleItem.end(), sort_func);
}

bool CPythonPrivateShop::GetSaleItemData(WORD wIndex, const TPrivateShopSaleData** c_pSale)
{
	if (wIndex >= m_vec_saleItem.size())
		return false;

	*c_pSale = &m_vec_saleItem.at(wIndex);

	return true;
}

void CPythonPrivateShop::SetMarketItemPrice(TMarketItemPrice& rSale)
{
	auto it = m_map_marketItemPrice.find(rSale.dwVnum);

	if (it != m_map_marketItemPrice.end())
	{
		it->second = rSale.TPrice;
		return;
	}

	m_map_marketItemPrice.emplace(rSale.dwVnum, rSale.TPrice);
}

bool CPythonPrivateShop::GetMarketItemPrice(DWORD dwVnum, const TItemPrice** c_pPrice)
{
	auto it = m_map_marketItemPrice.find(dwVnum);

	if (it == m_map_marketItemPrice.end())
	{
		c_pPrice = nullptr;
		return false;
	}

	*c_pPrice = &it->second;
	return true;
}