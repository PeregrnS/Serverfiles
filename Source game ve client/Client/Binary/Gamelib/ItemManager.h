#pragma once

#include "ItemData.h"
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
#include <map>
#endif

class CItemManager : public CSingleton<CItemManager>
{
	public:
		enum EItemDescCol
		{
			ITEMDESC_COL_VNUM,
			ITEMDESC_COL_NAME,
			ITEMDESC_COL_DESC,
			ITEMDESC_COL_SUMM,
			ITEMDESC_COL_NUM,
		};

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		enum EItemScaleColumn
		{
			ITEMSCALE_VNUM,
			ITEMSCALE_JOB,
			ITEMSCALE_SEX,
			ITEMSCALE_SCALE_X,
			ITEMSCALE_SCALE_Y,
			ITEMSCALE_SCALE_Z,
			ITEMSCALE_POSITION_X,
			ITEMSCALE_POSITION_Y,
			ITEMSCALE_POSITION_Z,
			ITEMSCALE_NUM,
			ITEMSCALE_REQ = ITEMSCALE_SCALE_Z + 1,
			ITEMSCALE_AURA_NUM = ITEMSCALE_POSITION_X + 1,
		};
#endif

#ifdef ENABLE_CHEST_DROP_INFO
		struct SDropItemInfo
		{
			DWORD	dwDropVnum;
			int		iCount;	// BYTE
		};
#endif

#ifdef ENABLE_EXTENDED_BLEND
		typedef struct SBlendInfo
		{
			DWORD	dwVnum;
			BYTE	bApplyType;
			std::vector<int>	vec_iApplyValue;
			std::vector<int>	vec_iApplyDuration;
		} TBlendInfo;
#endif

	public:
		typedef std::map<DWORD, CItemData*> TItemMap;
		typedef std::map<std::string, CItemData*> TItemNameMap;
#ifdef ENABLE_CHEST_DROP_INFO
		typedef std::vector<SDropItemInfo> TChestDropItemInfoVec;
		typedef std::unordered_map<DWORD, TChestDropItemInfoVec> TChestDropItemInfoMap;
#endif
#ifdef ENABLE_EXTENDED_BLEND
		typedef std::map<DWORD, TBlendInfo> TBlendMap;
#endif

#ifdef ENABLE_INGAME_WIKI
	public:
		typedef std::vector<CItemData*> TItemVec;
		typedef std::vector<DWORD> TItemNumVec;

	public:
		void WikiAddVnumToBlacklist(DWORD vnum)
		{
			auto it = m_ItemMap.find(vnum);
			if (it != m_ItemMap.end())
				it->second->SetBlacklisted(true);
		};
		
		TItemNumVec* WikiGetLastItems()
		{
			return &m_tempItemVec;
		}

		BOOL CanIncrSelectedItemRefineLevel();
		BOOL CanIncrItemRefineLevel(DWORD itemVnum);
		bool CanLoadWikiItem(DWORD dwVnum);
		DWORD GetWikiItemStartRefineVnum(DWORD dwVnum);
		std::string GetWikiItemBaseRefineName(DWORD dwVnum);
		size_t WikiLoadClassItems(BYTE classType, DWORD raceFilter);
		std::tuple<const char*, int> SelectByNamePart(const char * namePart);

	protected:
		TItemNumVec m_tempItemVec;

	private:
		bool IsFilteredAntiflag(CItemData* itemData, DWORD raceFilter);
#endif

	public:
		CItemManager();
		virtual ~CItemManager();

		void			Destroy();

		BOOL			SelectItemData(DWORD dwIndex);
		CItemData *		GetSelectedItemDataPointer();

		BOOL			GetItemDataPointer(DWORD dwItemID, CItemData ** ppItemData);
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
		bool			GetItemByName(const char* c_szInput, CItemData** ppItemData);
		void			GetItemListByName(const char* c_szInput, std::vector<std::pair<DWORD, std::string>>& r_vec_itemName);
#endif

		bool			LoadItemDesc(const char* c_szFileName);
		bool			LoadItemList(const char* c_szFileName);
		bool			LoadItemTable(const char* c_szFileName);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool			LoadItemScale(const char* c_szFileName);
#endif
#ifdef ENABLE_SHINING_TABLE
		bool			LoadShiningTable(const char* szShiningTable);
#endif
#ifdef ENABLE_CHEST_DROP_INFO
		bool 			LoadChestDropInfo(const char* c_szFileName);
		TChestDropItemInfoVec* GetItemDropInfoVec(const DWORD dwVnum);
#endif
#ifdef ENABLE_EXTENDED_BLEND
		BYTE			GetAttrTypeByName(const std::string& stAttrType);
		BOOL			GetBlendInfoPointer(DWORD dwItemID, TBlendInfo ** ppBlendInfo);
		bool			LoadItemBlend(const char* c_szFileName);
#endif
		CItemData *		MakeItemData(DWORD dwIndex);

	protected:
		TItemMap m_ItemMap;
		std::vector<CItemData*>  m_vec_ItemRange;
		CItemData * m_pSelectedItemData;
#ifdef ENABLE_CHEST_DROP_INFO
		TChestDropItemInfoMap m_ItemDropInfoMap;
		TChestDropItemInfoMap m_BaseItemDropInfoMap;
#endif
#ifdef ENABLE_EXTENDED_BLEND
		TBlendMap m_BlendMap;
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
		std::multimap<std::string, CItemData*>	m_NameItemMap;
#endif
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
