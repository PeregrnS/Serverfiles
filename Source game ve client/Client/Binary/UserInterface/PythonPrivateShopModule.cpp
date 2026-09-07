#include "StdAfx.h"
#include "PythonPrivateShop.h"
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"

#include "../GameLib/ItemManager.h"

PyObject* privateShopBuild(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	DWORD dwPolyVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 1, &dwPolyVnum))
		return Py_BuildException();

	BYTE bTitleType;
	if (!PyTuple_GetByte(poArgs, 2, &bTitleType))
		return Py_BuildException();

	BYTE bPageCount;
	if (!PyTuple_GetByte(poArgs, 3, &bPageCount))
		return Py_BuildException();

	CPythonPrivateShop::Instance().BuildPrivateShop(szName, dwPolyVnum, bTitleType, bPageCount);
	return Py_BuildNone();
}

PyObject* privateShopClearPrivateShopStock(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPrivateShop::Instance().ClearPrivateShopStock();
	return Py_BuildNone();
}
PyObject* privateShopAddItemStock(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	int iDisplaySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iDisplaySlotIndex))
		return Py_BuildException();

	long long llPrice;
	if (!PyTuple_GetLongLong(poArgs, 3, &llPrice))
		return Py_BuildException();

	DWORD dwCheque;
	if (!PyTuple_GetUnsignedLong(poArgs, 4, &dwCheque))
		return Py_BuildException();

	CPythonPrivateShop::Instance().AddPrivateShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex), iDisplaySlotIndex, llPrice, dwCheque);
	return Py_BuildNone();
}
PyObject* privateShopDeleteItemStock(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	CPythonPrivateShop::Instance().DelPrivateShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex));
	return Py_BuildNone();
}

PyObject* privateShopGetStockItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	long long llGold = 0;
	DWORD dwCheque = 0;

	CPythonPrivateShop::Instance().GetPrivateShopItemPrice(TItemPos(bItemWindowType, wItemSlotIndex), llGold, dwCheque);

	return Py_BuildValue("L", llGold);
}

PyObject* privateShopGetStockChequeItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	long long llGold = 0;
	DWORD dwCheque = 0;

	CPythonPrivateShop::Instance().GetPrivateShopItemPrice(TItemPos(bItemWindowType, wItemSlotIndex), llGold, dwCheque);

	return Py_BuildValue("i", dwCheque);
}

PyObject* privateShopGetTotalStockGold(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonPrivateShop::Instance().GetTotalStockGold());
}

PyObject* privateShopGetTotalStockCheque(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetTotalStockCheque());
}

PyObject* privateShopGetLocation(PyObject* poSelf, PyObject* poArgs)
{
	long lX, lY;
	BYTE bChannel;

	CPythonPrivateShop::Instance().GetLocation(lX, lY, bChannel);

	return Py_BuildValue("iii", lX, lY, bChannel);
}

PyObject* privateShopGetTitle(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPrivateShop::Instance().GeTitle().c_str());
}

PyObject* privateShopGetMyTitle(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPrivateShop::Instance().GetMyTitle().c_str());
}

PyObject* privateShopGetGold(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonPrivateShop::Instance().GetGold());
}

PyObject* privateShopGetCheque(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetCheque());
}

PyObject* privateShopGetPremiumTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetPremiumTime());
}

PyObject* privateShopGetMyState(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetMyState());
}

PyObject* privateShopGetState(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetState());
}

PyObject* privateShopGetPageCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetPageCount());
}

PyObject* privateShopGetMyPageCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetMyPageCount());
}

PyObject* privateShopGetTotalGold(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonPrivateShop::Instance().GetTotalGold());
}

PyObject* privateShopGetTotalCheque(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetTotalCheque());
}

PyObject* privateShopGetUnlockedSlots(PyObject* poSelf, PyObject* poArgs)
{
	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	if (bIsMainPlayerPrivateShop)
		return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_PRIVATE_SHOP_UNLOCKED_SLOT));
	else
		return Py_BuildValue("i", CPythonPrivateShop::Instance().GetUnlockedSlots());
}

PyObject* privateShopGetActiveVID(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetActiveVID());
}

PyObject* privateShopGetItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", c_pItemData->dwVnum);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", c_pItemData->dwCount);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("L", c_pItemData->TPrice.llGold);

	return Py_BuildValue("L", 0);
}

PyObject* privateShopGetChequeItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", c_pItemData->TPrice.dwCheque);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", c_pItemData->alSockets[iMetinSocketIndex]);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{

		bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

		const TPrivateShopItemData* c_pItemData;
		if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
			return Py_BuildValue("ii", c_pItemData->aAttr[iAttrSlotIndex].bType, c_pItemData->aAttr[iAttrSlotIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* privateShopClearSearchResult(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPrivateShop::Instance().ClearSearchResult();
	return Py_BuildNone();
}

PyObject* privateShopGetSearchResultMaxCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetResultMaxCount());
}

PyObject* privateShopGetSearchResultPage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetResultPage());
}

PyObject* privateShopSetSearchResultPage(PyObject* poSelf, PyObject* poArgs)
{
	int iPage;
	if (!PyTuple_GetInteger(poArgs, 0, &iPage))
		return Py_BuildException();

	CPythonPrivateShop::Instance().SetResultPage((BYTE)iPage);

	return Py_BuildNone();
}

PyObject* privateShopGetSearchResult(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
#ifdef ENABLE_PRIVATE_SHOP_TIME_SORT
	if (!CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("isiLiii", 0, "", 0, 0, 0, 0, 0);

	return Py_BuildValue("isiLiii", c_pItemData->dwVnum, c_pItemData->szOwnerName, c_pItemData->dwCount, c_pItemData->TPrice.llGold, c_pItemData->TPrice.dwCheque, c_pItemData->bState
	, c_pItemData->alSockets[0]);
#else
	if (!CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("isiLii", 0, "", 0, 0, 0, 0);

	return Py_BuildValue("isiLii", c_pItemData->dwVnum, c_pItemData->szOwnerName, c_pItemData->dwCount, c_pItemData->TPrice.llGold, c_pItemData->TPrice.dwCheque, c_pItemData->bState);
#endif
}

PyObject* privateShopGetSearchItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwVnum);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSearchItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->alSockets[iMetinSocketIndex]);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSearchItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{
		const TPrivateShopSearchData* c_pItemData;
		if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
			return Py_BuildValue("ii", c_pItemData->aAttr[iAttrSlotIndex].bType, c_pItemData->aAttr[iAttrSlotIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* privateShopGetSeachItemVID(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwShopID);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSaleItemMaxCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetSaleItemMaxCount());
}

PyObject* privateShopGetSaleCustomerName(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSaleData* c_pSaleItemData;
	if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pSaleItemData))
		return Py_BuildValue("s", c_pSaleItemData->szCustomer);

	return Py_BuildValue("s", "");
}

PyObject* privateShopGetSaleTime(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSaleData* c_pSaleItemData;
	if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pSaleItemData))
		return Py_BuildValue("i", c_pSaleItemData->tTime);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSaleItem(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSaleData* c_pSaleItemData;
	if (!CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pSaleItemData))
		return Py_BuildValue("si", "", 0);

	CItemData* pItemData = nullptr;
	if (!CItemManager::Instance().GetItemDataPointer(c_pSaleItemData->TItem.dwVnum, &pItemData))
		return Py_BuildValue("si", "", 0);

	return Py_BuildValue("si", pItemData->GetName(), c_pSaleItemData->tTime);
}

PyObject* privateShopGetSaleItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSaleData* c_pSaleItemData;
	if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pSaleItemData))
		return Py_BuildValue("i", c_pSaleItemData->TItem.dwVnum);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSaleItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSaleData* c_pSaleItemData;
	if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pSaleItemData))
		return Py_BuildValue("i", c_pSaleItemData->TItem.dwCount);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSaleItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSaleData* c_pSaleItemData;
	if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pSaleItemData))
		return Py_BuildValue("L", c_pSaleItemData->TItem.TPrice.llGold);

	return Py_BuildValue("L", 0);
}

PyObject* privateShopGetSaleChequeItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSaleData* c_pSaleItemData;
	if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pSaleItemData))
		return Py_BuildValue("i", c_pSaleItemData->TItem.TPrice.dwCheque);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSaleItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	const TPrivateShopSaleData* c_pSaleItemData;
	if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pSaleItemData))
		return Py_BuildValue("i", c_pSaleItemData->TItem.alSockets[iMetinSocketIndex]);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSaleItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{
		const TPrivateShopSaleData* c_pSaleItemData;
		if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pSaleItemData))
			return Py_BuildValue("ii", c_pSaleItemData->TItem.aAttr[iAttrSlotIndex].bType, c_pSaleItemData->TItem.aAttr[iAttrSlotIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* privateShopIsMarketItemPriceDataLoaded(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPrivateShop::Instance().IsMarketItemPriceDataLoaded());
}

PyObject* privateShopGetMarketItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	const TItemPrice* pMarketPrice = nullptr;
	if (!CPythonPrivateShop::Instance().GetMarketItemPrice(dwVnum, &pMarketPrice))
		return Py_BuildValue("ii", -1, -1);

	return Py_BuildValue("Li", pMarketPrice->llGold, pMarketPrice->dwCheque);
}

PyObject* privateShopIsMainPlayerPrivateShop(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPrivateShop::Instance().IsMainPlayerPrivateShop());
}

PyObject* privateShopGetName(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CPythonPrivateShop::SPrivateShopInstance* pPrivateShopInstance = CPythonPrivateShop::Instance().GetPrivateShopInstance(iVirtualID);
	if (!pPrivateShopInstance)
		return Py_BuildValue("s", "");

	return Py_BuildValue("s", pPrivateShopInstance->GetName());
}

PyObject* privateShopGetProjectPosition(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 1, &iHeight))
		return Py_BuildException();

	CPythonPrivateShop::SPrivateShopInstance* pPrivateShopInstance = CPythonPrivateShop::Instance().GetPrivateShopInstance(iVirtualID);
	if (!pPrivateShopInstance)
		return Py_BuildValue("ii", -100, -100);

	CGraphicThingInstance* pInstance = pPrivateShopInstance->GetGraphicThingInstancePtr();
	if (!pInstance)
		return Py_BuildValue("ii", -100, -100);

	const TPixelPosition c_PixelPosition = pInstance->GetPosition();

	CPythonGraphic& rpyGraphic = CPythonGraphic::Instance();

	float fx, fy, fz;
	rpyGraphic.ProjectPosition(c_PixelPosition.x,
		c_PixelPosition.y, // Using different axis on the private shop instance -_-
		c_PixelPosition.z + float(iHeight),
		&fx, &fy, &fz);


	if (1 == int(fz))
		return Py_BuildValue("ii", -100, -100);

	return Py_BuildValue("ii", int(fx), int(fy));
}

PyObject* privateShopGetMainCharacterDistance(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CPythonPrivateShop::SPrivateShopInstance* pPrivateShopInstance = CPythonPrivateShop::Instance().GetPrivateShopInstance(iVirtualID);
	if (!pPrivateShopInstance)
		return Py_BuildValue("f", -1.0f);

	CGraphicThingInstance* pInstance = pPrivateShopInstance->GetGraphicThingInstancePtr();
	if (!pInstance)
		return Py_BuildValue("f", -1.0f);

#ifdef ENABLE_PRIVATE_SHOP_LIMITED_DISTANCE_RENDERING
	if (!pInstance->isShow())
		return Py_BuildValue("f", -1.0f);
#endif

	const TPixelPosition c_ShopPosition = pInstance->GetPosition();

	TPixelPosition MainActorPosition;
	CPythonPlayer::Instance().NEW_GetMainActorPosition(&MainActorPosition);

	float fdx = MainActorPosition.x - c_ShopPosition.x;
	float fdy = MainActorPosition.y - (-c_ShopPosition.y); // Using different axis on the private shop instance -_-

	float fDistance = sqrtf((fdx * fdx) + (fdy * fdy));

	return Py_BuildValue("f", fDistance);
}

PyObject* privateShopCreatePrivateShopSearchPos(PyObject* poSelf, PyObject* poArgs)
{
	char* c_szOwnerName;
	if (!PyTuple_GetString(poArgs, 0, &c_szOwnerName))
		return Py_BuildException();

	DWORD dwVID = CPythonPrivateShop::Instance().GetPrivateShopInstanceVID(c_szOwnerName);
	if (dwVID)
		CPythonPrivateShop::Instance().SelectSearchPrivateShop(dwVID);

	return Py_BuildNone();
}

PyObject* privateShopDeletePrivateShopSearchPos(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 0:
	{
		CPythonPrivateShop::Instance().ClearSelectedSearchPrivateShop();
	} break;

	case 1:
	{
		char* c_szOwnerName;
		if (!PyTuple_GetString(poArgs, 0, &c_szOwnerName))
			return Py_BuildException();

		DWORD dwVID = CPythonPrivateShop::Instance().GetPrivateShopInstanceVID(c_szOwnerName);
		if (dwVID)
			CPythonPrivateShop::Instance().UnselectSearchPrivateShop(dwVID);
	} break;
	}

	return Py_BuildNone();
}

PyObject* privateShopGetAppearanceDecoMaxCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetAppearanceDecoMaxCount());
}

PyObject* privateShopGetTitleDecoMaxCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetTitleDecoMaxCount());
}

PyObject* privateShopGetAppearanceDeco(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const CPythonPrivateShop::TAppearanceDeco* pAppearanceDeco = nullptr;
	if (!CPythonPrivateShop::Instance().GetAppearanceDeco(iPos, &pAppearanceDeco))
		return Py_BuildValue("si", "", 0);

	return Py_BuildValue("si", pAppearanceDeco->stName.c_str(), pAppearanceDeco->dwVnum);
}

PyObject* privateShopGetTitleDeco(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const CPythonPrivateShop::TTitleDeco* pTitleDeco = nullptr;
	if (!CPythonPrivateShop::Instance().GetTitleDeco(iPos, &pTitleDeco))
		return Py_BuildValue("sss", "", "", "");

	return Py_BuildValue("sss", pTitleDeco->stName.c_str(), pTitleDeco->stPath.c_str(), pTitleDeco->stTextColor.c_str());
}

#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
PyObject* privateShopGetItemRefineElement(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", c_pItemData->dwRefineElement);

	return Py_BuildValue("i", 0);
}
#endif

#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
PyObject* privateShopGetItemChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", c_pItemData->dwTransmutationVnum);

	return Py_BuildValue("i", 0);
}
#endif

#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
PyObject* privateShopGetItemApplyRandom(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iApplyIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iApplyIndex))
		return Py_BuildException();

	if (iApplyIndex >= 0 && iApplyIndex < ITEM_APPLY_RANDOM_SLOT_MAX_NUM)
	{
		bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

		const TPrivateShopItemData* c_pItemData;
		if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
			return Py_BuildValue("ii", c_pItemData->aApplyRandom[iApplyIndex].bType, c_pItemData->aApplyRandom[iApplyIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}
#endif

#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
PyObject* privateShopGetSearchItemRefineElement(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwRefineElement);

	return Py_BuildValue("i", 0);
}
#endif

#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
PyObject* privateShopGetSearchItemChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwTransmutationVnum);

	return Py_BuildValue("i", 0);
}
#endif

#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
PyObject* privateShopGetSearchItemApplyRandom(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iApplyIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iApplyIndex))
		return Py_BuildException();

	if (iApplyIndex >= 0 && iApplyIndex < ITEM_APPLY_RANDOM_SLOT_MAX_NUM)
	{
		const TPrivateShopSearchData* c_pItemData;
		if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
			return Py_BuildValue("ii", c_pItemData->aApplyRandom[iApplyIndex].bType, c_pItemData->aApplyRandom[iApplyIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}
#endif


#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
PyObject* privateShopGetSaleItemRefineElement(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopSaleData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->TItem.dwRefineElement);

	return Py_BuildValue("i", 0);
}
#endif

#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
PyObject* privateShopGetSaleItemChangeLookVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopSaleData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->TItem.dwTransmutationVnum);

	return Py_BuildValue("i", 0);
}
#endif

#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
PyObject* privateShopGetSaleItemApplyRandom(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iApplyIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iApplyIndex))
		return Py_BuildException();

	if (iApplyIndex >= 0 && iApplyIndex < ITEM_APPLY_RANDOM_SLOT_MAX_NUM)
	{
		bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

		const TPrivateShopSaleData* c_pItemData;
		if (CPythonPrivateShop::Instance().GetSaleItemData(iPos, &c_pItemData))
			return Py_BuildValue("ii", c_pItemData->TItem.aApplyRandom[iApplyIndex].bType, c_pItemData->TItem.aApplyRandom[iApplyIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}
#endif

void initPrivateShop()
{
	static PyMethodDef s_methods[] =
	{

		{ "BuildPrivateShop",			privateShopBuild,						METH_VARARGS },
		{ "ClearPrivateShopStock",		privateShopClearPrivateShopStock,		METH_VARARGS },
		{ "AddItemStock",				privateShopAddItemStock,				METH_VARARGS },
		{ "DeleteItemStock",			privateShopDeleteItemStock,				METH_VARARGS },
		{ "GetStockItemPrice",			privateShopGetStockItemPrice,			METH_VARARGS },
		{ "GetStockChequeItemPrice",	privateShopGetStockChequeItemPrice,		METH_VARARGS },
		{ "GetTotalStockGold",			privateShopGetTotalStockGold,			METH_VARARGS },
		{ "GetTotalStockCheque",		privateShopGetTotalStockCheque,			METH_VARARGS },
		{ "GetLocation",				privateShopGetLocation,					METH_VARARGS },
		{ "GetMyTitle",					privateShopGetMyTitle,					METH_VARARGS },
		{ "GetTitle",					privateShopGetTitle,					METH_VARARGS },
		{ "GetGold",					privateShopGetGold,						METH_VARARGS },
		{ "GetCheque",					privateShopGetCheque,					METH_VARARGS },
		{ "GetPremiumTime",				privateShopGetPremiumTime,				METH_VARARGS },
		{ "GetMyState",					privateShopGetMyState,					METH_VARARGS },
		{ "GetState",					privateShopGetState,					METH_VARARGS },
		{ "GetPageCount",				privateShopGetPageCount,				METH_VARARGS },
		{ "GetMyPageCount",				privateShopGetMyPageCount,				METH_VARARGS },
		{ "GetTotalGold",				privateShopGetTotalGold,				METH_VARARGS },
		{ "GetTotalCheque",				privateShopGetTotalCheque,				METH_VARARGS },
		{ "GetUnlockedSlots",			privateShopGetUnlockedSlots,			METH_VARARGS },
		{ "GetActiveVID",				privateShopGetActiveVID,				METH_VARARGS },

		{ "GetItemVnum",				privateShopGetItemVnum,					METH_VARARGS },
		{ "GetItemCount",				privateShopGetItemCount,				METH_VARARGS },
		{ "GetItemPrice",				privateShopGetItemPrice,				METH_VARARGS },
		{ "GetChequeItemPrice",			privateShopGetChequeItemPrice,			METH_VARARGS },
		{ "GetItemMetinSocket",			privateShopGetItemMetinSocket,			METH_VARARGS },
		{ "GetItemAttribute",			privateShopGetItemAttribute,			METH_VARARGS },

		{ "ClearSearchResult",			privateShopClearSearchResult,			METH_VARARGS },
		{ "GetSearchResultMaxCount",	privateShopGetSearchResultMaxCount,		METH_VARARGS },
		{ "GetSearchResultPage",		privateShopGetSearchResultPage,			METH_VARARGS },
		{ "SetSearchResultPage",		privateShopSetSearchResultPage,			METH_VARARGS },
		{ "GetSearchResult",			privateShopGetSearchResult,				METH_VARARGS },

		{ "GetSearchItemVnum",			privateShopGetSearchItemVnum,			METH_VARARGS },
		{ "GetSearchItemMetinSocket",	privateShopGetSearchItemMetinSocket,	METH_VARARGS },
		{ "GetSearchItemAttribute",		privateShopGetSearchItemAttribute,		METH_VARARGS },
		{ "GetSeachItemVID",			privateShopGetSeachItemVID,				METH_VARARGS },

		{ "GetSaleItemMaxCount",		privateShopGetSaleItemMaxCount,			METH_VARARGS },
		{ "GetSaleItem",				privateShopGetSaleItem,					METH_VARARGS },

		{ "GetSaleCustomerName",		privateShopGetSaleCustomerName,			METH_VARARGS },
		{ "GetSaleTime",				privateShopGetSaleTime,					METH_VARARGS },
		{ "GetSaleItemVnum",			privateShopGetSaleItemVnum,				METH_VARARGS },
		{ "GetSaleItemCount",			privateShopGetSaleItemCount,			METH_VARARGS },
		{ "GetSaleItemPrice",			privateShopGetSaleItemPrice,			METH_VARARGS },
		{ "GetSaleChequeItemPrice",		privateShopGetSaleChequeItemPrice,		METH_VARARGS },
		{ "GetSaleItemMetinSocket",		privateShopGetSaleItemMetinSocket,		METH_VARARGS },
		{ "GetSaleItemAttribute",		privateShopGetSaleItemAttribute,		METH_VARARGS },

		{ "IsMarketItemPriceDataLoaded",privateShopIsMarketItemPriceDataLoaded,	METH_VARARGS },
		{ "GetMarketItemPrice",			privateShopGetMarketItemPrice,			METH_VARARGS },

		{ "IsMainPlayerPrivateShop",	privateShopIsMainPlayerPrivateShop,		METH_VARARGS },
		{ "GetName",					privateShopGetName,						METH_VARARGS },
		{ "GetProjectPosition",			privateShopGetProjectPosition,			METH_VARARGS },
		{ "GetMainCharacterDistance",	privateShopGetMainCharacterDistance,	METH_VARARGS },
		{ "CreatePrivateShopSearchPos",	privateShopCreatePrivateShopSearchPos,	METH_VARARGS },
		{ "DeletePrivateShopSearchPos",	privateShopDeletePrivateShopSearchPos,	METH_VARARGS },

		{ "GetAppearanceDecoMaxCount",	privateShopGetAppearanceDecoMaxCount,	METH_VARARGS },
		{ "GetTitleDecoMaxCount",		privateShopGetTitleDecoMaxCount,		METH_VARARGS },
		{ "GetAppearanceDeco",			privateShopGetAppearanceDeco,			METH_VARARGS },
		{ "GetTitleDeco",				privateShopGetTitleDeco,				METH_VARARGS },
		
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
		{ "GetItemRefineElement",		privateShopGetItemRefineElement,		METH_VARARGS },
#endif
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
		{ "GetItemChangeLookVnum",		privateShopGetItemChangeLookVnum,		METH_VARARGS },
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
		{ "GetItemApplyRandom",			privateShopGetItemApplyRandom,			METH_VARARGS },
#endif

#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
		{ "GetSearchItemRefineElement",	privateShopGetSearchItemRefineElement,	METH_VARARGS },
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
		{ "GetSearchItemApplyRandom",	privateShopGetSearchItemApplyRandom,	METH_VARARGS },
#endif
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
		{ "GetSearchItemChangeLookVnum",	privateShopGetSearchItemChangeLookVnum,	METH_VARARGS },
#endif

#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
		{ "GetSaleItemRefineElement",	privateShopGetSaleItemRefineElement,	METH_VARARGS },
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
		{ "GetSaleItemApplyRandom",		privateShopGetSaleItemApplyRandom,		METH_VARARGS },
#endif
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
		{ "GetSaleItemChangeLookVnum",	privateShopGetSaleItemChangeLookVnum,	METH_VARARGS },
#endif

		{ NULL,							NULL,							NULL },
	};

	PyObject* poModule = Py_InitModule("privateShop", s_methods);

	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_PAGE_MAX_NUM", PRIVATE_SHOP_PAGE_MAX_NUM);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_WIDTH", PRIVATE_SHOP_WIDTH);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_HEIGHT", PRIVATE_SHOP_HEIGHT);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_HOST_ITEM_MAX_NUM", PRIVATE_SHOP_HOST_ITEM_MAX_NUM);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_PAGE_ITEM_MAX_NUM", PRIVATE_SHOP_PAGE_ITEM_MAX_NUM);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_LOCKED_SLOT_MAX_NUM", PRIVATE_SHOP_LOCKED_SLOT_MAX_NUM);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_SLOT_UNLOCK_ITEM", PRIVATE_SHOP_SLOT_UNLOCK_ITEM);

	PyModule_AddIntConstant(poModule, "STATE_CLOSED", STATE_CLOSED);
	PyModule_AddIntConstant(poModule, "STATE_OPEN", STATE_OPEN);
	PyModule_AddIntConstant(poModule, "STATE_MODIFY", STATE_MODIFY);

	PyModule_AddIntConstant(poModule, "TITLE_MAX_LEN", TITLE_MAX_LEN);
	PyModule_AddIntConstant(poModule, "TITLE_MIN_LEN", TITLE_MIN_LEN);

	PyModule_AddIntConstant(poModule, "MODE_NONE", MODE_NONE);
	PyModule_AddIntConstant(poModule, "MODE_LOOK", MODE_LOOK);
	PyModule_AddIntConstant(poModule, "MODE_TRADE", MODE_TRADE);
	PyModule_AddIntConstant(poModule, "RESULT_MAX_NUM", RESULT_MAX_NUM);
	PyModule_AddIntConstant(poModule, "SELECTED_ITEM_MAX_NUM", SELECTED_ITEM_MAX_NUM);

	PyModule_AddIntConstant(poModule, "STATE_REMOVED", STATE_REMOVED);
	PyModule_AddIntConstant(poModule, "STATE_AVAILABLE", STATE_AVAILABLE);
	PyModule_AddIntConstant(poModule, "STATE_RESTRICTED", STATE_RESTRICTED);

	PyModule_AddIntConstant(poModule, "MAX_VIEW_DISTANCE", MAX_VIEW_DISTANCE);

	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ITEM_VNUM", FILTER_TYPE_ITEM_VNUM);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_OWNER_NAME", FILTER_TYPE_OWNER_NAME);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ITEM_TYPE", FILTER_TYPE_ITEM_TYPE);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ITEM_SUBTYPE", FILTER_TYPE_ITEM_SUBTYPE);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_CLASS", FILTER_TYPE_CLASS);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_GENDER", FILTER_TYPE_GENDER);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MIN_STACK", FILTER_TYPE_MIN_STACK);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MAX_STACK", FILTER_TYPE_MAX_STACK);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MIN_REFINEMENT", FILTER_TYPE_MIN_REFINEMENT);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MAX_REFINEMENT", FILTER_TYPE_MAX_REFINEMENT);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MIN_LEVEL", FILTER_TYPE_MIN_LEVEL);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MAX_LEVEL", FILTER_TYPE_MAX_LEVEL);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ATTR_1", FILTER_TYPE_ATTR_1);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ATTR_2", FILTER_TYPE_ATTR_2);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ATTR_3", FILTER_TYPE_ATTR_3);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ATTR_4", FILTER_TYPE_ATTR_4);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ATTR_5", FILTER_TYPE_ATTR_5);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_SASH_ABSORPTION", FILTER_TYPE_SASH_ABSORPTION);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ALCHEMY_LEVEL", FILTER_TYPE_ALCHEMY_LEVEL);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ALCHEMY_CLARITY", FILTER_TYPE_ALCHEMY_CLARITY);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MAX_NUM", FILTER_TYPE_MAX_NUM);
}