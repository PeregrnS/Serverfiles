#ifndef __INC_METIN_II_GAME_SHOP_MANAGER_H__
#define __INC_METIN_II_GAME_SHOP_MANAGER_H__

class CShop;
typedef class CShop * LPSHOP;

class CShopManager : public singleton<CShopManager>
{
public:
	typedef std::map<DWORD, CShop *> TShopMap;

public:
	CShopManager();
	virtual ~CShopManager();

	bool	Initialize(TShopTable * table, int size);
	void	Destroy();

	LPSHOP	Get(DWORD dwVnum);
	LPSHOP	GetByNPCVnum(DWORD dwVnum);

	bool	StartShopping(LPCHARACTER pkChr, LPCHARACTER pkShopKeeper, int iShopVnum = 0);
	void	StopShopping(LPCHARACTER ch);

	void	Buy(LPCHARACTER ch, BYTE pos);
	void	Sell(LPCHARACTER ch
#ifdef ENABLE_SPECIAL_INVENTORY
	, WORD wCell
#else
	, BYTE bCell
#endif
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	, short bCount = 0
#else
	, BYTE bCount = 0
#endif
	);

#ifdef ENABLE_EXTENDED_ITEM_COUNT
	LPSHOP	CreatePCShop(LPCHARACTER ch, TShopItemTable * pTable, short bItemCount);
#else
	LPSHOP	CreatePCShop(LPCHARACTER ch, TShopItemTable * pTable, BYTE bItemCount);
#endif
	LPSHOP	FindPCShop(DWORD dwVID);
	void	DestroyPCShop(LPCHARACTER ch);

#if defined(ENABLE_RENEWAL_SHOPEX)
	bool	InitializeShopEX(TShopTable* table, int size);
#endif

private:
	TShopMap	m_map_pkShop;
	TShopMap	m_map_pkShopByNPCVnum;
	TShopMap	m_map_pkShopByPC;

	bool	ReadShopTableEx(const char* stFileName);

#ifdef ENABLE_GEM_SYSTEM
public:
	bool InitializeGemShop(TGemShopTable *table, int size);
	DWORD GemShopGetVnumById(DWORD id);
	BYTE GemShopGetCountById(DWORD id);
	DWORD GemShopGetPriceById(DWORD id);
	int GemShopGetTableSize() { return m_iGemShopTableSize; }

private:
	int m_iGemShopTableSize;
	TGemShopTable *m_pGemShopTable;
#endif
};

#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
