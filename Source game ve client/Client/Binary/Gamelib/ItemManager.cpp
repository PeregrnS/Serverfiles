#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"
#include "../eterLib/ResourceManager.h"

#include "ItemManager.h"

static DWORD s_adwItemProtoKey[4] =
{
	173217,
	72619434,
	408587239,
	27973291
};

BOOL CItemManager::SelectItemData(DWORD dwIndex)
{
	TItemMap::iterator f = m_ItemMap.find(dwIndex);

	if (m_ItemMap.end() == f)
	{
		int n = m_vec_ItemRange.size();
		for (int i = 0; i < n; i++)
		{
			CItemData * p = m_vec_ItemRange[i];
			const CItemData::TItemTable * pTable = p->GetTable();
			if ((pTable->dwVnum < dwIndex) &&
				dwIndex < (pTable->dwVnum + pTable->dwVnumRange))
			{
				m_pSelectedItemData = p;
				return TRUE;
			}
		}
		Tracef(" CItemManager::SelectItemData - FIND ERROR [%d]\n", dwIndex);
		return FALSE;
	}

	m_pSelectedItemData = f->second;

	return TRUE;
}

CItemData * CItemManager::GetSelectedItemDataPointer()
{
	return m_pSelectedItemData;
}

BOOL CItemManager::GetItemDataPointer(DWORD dwItemID, CItemData ** ppItemData)
{
	if (0 == dwItemID)
		return FALSE;

	TItemMap::iterator f = m_ItemMap.find(dwItemID);

	if (m_ItemMap.end() == f)
	{
		int n = m_vec_ItemRange.size();
		for (int i = 0; i < n; i++)
		{
			CItemData * p = m_vec_ItemRange[i];
			const CItemData::TItemTable * pTable = p->GetTable();
			if ((pTable->dwVnum < dwItemID) &&
				dwItemID < (pTable->dwVnum + pTable->dwVnumRange))
			{
				*ppItemData = p;
				return TRUE;
			}
		}
		Tracef(" CItemManager::GetItemDataPointer - FIND ERROR [%d]\n", dwItemID);
		return FALSE;
	}

	*ppItemData = f->second;

	return TRUE;
}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
bool CItemManager::GetItemByName(const char* c_szInput, CItemData** ppItemData)
{
	std::string strInput = c_szInput;
	std::transform(strInput.begin(), strInput.end(), strInput.begin(),
		[](unsigned char c) { return std::tolower(c); });

	for (const auto& kv : m_NameItemMap)
	{
		std::string strItemName = kv.first;
		CItemData* pItemData = kv.second;

		if (pItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_MYSHOP) || pItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_GIVE))
			continue;

		std::transform(strItemName.begin(), strItemName.end(), strItemName.begin(),
			[](unsigned char c) { return std::tolower(c); });

		// Ignore names that are shorter than the input
		if (strItemName.length() < strInput.length())
			continue;

		if (!strItemName.substr(0, strInput.length()).compare(strInput))
		{
			*ppItemData = pItemData;
			return true;
		}
	}

	return false;
}

void CItemManager::GetItemListByName(const char* c_szInput, std::vector<std::pair<DWORD, std::string>>& r_vec_itemName)
{
	std::string strInput = c_szInput;
	std::transform(strInput.begin(), strInput.end(), strInput.begin(),
		[](unsigned char c) { return std::tolower(c); });

	for (const auto& kv : m_NameItemMap)
	{
		std::string strItemName = kv.first;
		CItemData* pItemData = kv.second;

		if (pItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_MYSHOP) || pItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_GIVE))
			continue;

		std::transform(strItemName.begin(), strItemName.end(), strItemName.begin(),
			[](unsigned char c) { return std::tolower(c); });

		// Ignore names that are shorter than the input
		if (strItemName.length() < strInput.length())
			continue;

		if (!strItemName.substr(0, strInput.length()).compare(strInput))
			r_vec_itemName.push_back(std::make_pair(pItemData->GetTable()->dwVnum, pItemData->GetName()));
	}
}
#endif

CItemData * CItemManager::MakeItemData(DWORD dwIndex)
{
	TItemMap::iterator f = m_ItemMap.find(dwIndex);

	if (m_ItemMap.end() == f)
	{
		CItemData * pItemData = CItemData::New();

		m_ItemMap.insert(TItemMap::value_type(dwIndex, pItemData));

		return pItemData;
	}

	return f->second;
}

////////////////////////////////////////////////////////////////////////////////////////
// Load Item Table

bool CItemManager::LoadItemList(const char * c_szFileName)
{
	CMappedFile File;
	LPCVOID pData;

	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	CTokenVector TokenVector;
    for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;

		if (!(TokenVector.size() == 3 || TokenVector.size() == 4))
		{
			TraceError(" CItemManager::LoadItemList(%s) - StrangeLine in %d\n", c_szFileName, i);
			continue;
		}

		const std::string & c_rstrID = TokenVector[0];
		//const std::string & c_rstrType = TokenVector[1];
		const std::string & c_rstrIcon = TokenVector[2];

		DWORD dwItemVNum=atoi(c_rstrID.c_str());

		CItemData * pItemData = MakeItemData(dwItemVNum);

		extern BOOL USE_VIETNAM_CONVERT_WEAPON_VNUM;
		if (USE_VIETNAM_CONVERT_WEAPON_VNUM)
		{
			extern DWORD Vietnam_ConvertWeaponVnum(DWORD vnum);
			DWORD dwMildItemVnum = Vietnam_ConvertWeaponVnum(dwItemVNum);
			if (dwMildItemVnum == dwItemVNum)
			{
				if (4 == TokenVector.size())
				{
					const std::string & c_rstrModelFileName = TokenVector[3];
					pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
				}
				else
				{
					pItemData->SetDefaultItemData(c_rstrIcon.c_str());
				}
			}
			else
			{
				DWORD dwMildBaseVnum = dwMildItemVnum / 10 * 10;
				char szMildIconPath[MAX_PATH];
				sprintf(szMildIconPath, "icon/item/%.5d.tga", dwMildBaseVnum);
				if (4 == TokenVector.size())
				{
					char szMildModelPath[MAX_PATH];
					sprintf(szMildModelPath, "d:/ymir work/item/weapon/%.5d.gr2", dwMildBaseVnum);
					pItemData->SetDefaultItemData(szMildIconPath, szMildModelPath);
				}
				else
				{
					pItemData->SetDefaultItemData(szMildIconPath);
				}
			}
		}
		else
		{
			if (4 == TokenVector.size())
			{
				const std::string & c_rstrModelFileName = TokenVector[3];
				pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
			}
			else
			{
				pItemData->SetDefaultItemData(c_rstrIcon.c_str());
			}
		}
	}

	return true;
}

const std::string& __SnapString(const std::string& c_rstSrc, std::string& rstTemp)
{
	UINT uSrcLen=c_rstSrc.length();
	if (uSrcLen<2)
		return c_rstSrc;

	if (c_rstSrc[0]!='"')
		return c_rstSrc;

	UINT uLeftCut=1;

	UINT uRightCut=uSrcLen;
	if (c_rstSrc[uSrcLen-1]=='"')
		uRightCut=uSrcLen-1;

	rstTemp=c_rstSrc.substr(uLeftCut, uRightCut-uLeftCut);
	return rstTemp;
}

bool CItemManager::LoadItemDesc(const char* c_szFileName)
{
	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
	{
		Tracenf("CItemManager::LoadItemDesc(c_szFileName=%s) - Load Error", c_szFileName);
		return false;
	}

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	std::string stTemp;

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		while (kTokenVector.size()<ITEMDESC_COL_NUM)
			kTokenVector.push_back("");

		//assert(kTokenVector.size()==ITEMDESC_COL_NUM);

		DWORD dwVnum=atoi(kTokenVector[ITEMDESC_COL_VNUM].c_str());
		const std::string& c_rstDesc=kTokenVector[ITEMDESC_COL_DESC];
		const std::string& c_rstSumm=kTokenVector[ITEMDESC_COL_SUMM];
		TItemMap::iterator f = m_ItemMap.find(dwVnum);
		if (m_ItemMap.end() == f)
			continue;

		CItemData* pkItemDataFind = f->second;

		pkItemDataFind->SetDescription(__SnapString(c_rstDesc, stTemp));
		pkItemDataFind->SetSummary(__SnapString(c_rstSumm, stTemp));
	}
	return true;
}

DWORD GetHashCode( const char* pString )
{
	   unsigned long i,len;
	   unsigned long ch;
	   unsigned long result;

	   len     = strlen( pString );
	   result = 5381;
	   for( i=0; i<len; i++ )
	   {
	   	   ch = (unsigned long)pString[i];
	   	   result = ((result<< 5) + result) + ch; // hash * 33 + ch
	   }

	   return result;
}

bool CItemManager::LoadItemTable(const char* c_szFileName)
{
	CMappedFile file;
	LPCVOID pvData;

	if (!CEterPackManager::Instance().Get(file, c_szFileName, &pvData))
		return false;

	DWORD dwFourCC, dwElements, dwDataSize;
	DWORD dwVersion=0;
	DWORD dwStride=0;

	file.Read(&dwFourCC, sizeof(DWORD));

	if (dwFourCC == MAKEFOURCC('M', 'I', 'P', 'X'))
	{
		file.Read(&dwVersion, sizeof(DWORD));
		file.Read(&dwStride, sizeof(DWORD));

		if (dwVersion != 1)
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] VERSION[%d]", c_szFileName, dwVersion);
			return false;
		}

#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
		if (!CItemData::TItemTableAll::IsValidStruct(dwStride))
#else
		if (dwStride != sizeof(CItemData::TItemTable))
#endif
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] STRIDE[%d] != sizeof(SItemTable)",
				c_szFileName, dwStride, sizeof(CItemData::TItemTable));
			return false;
		}
	}
	else if (dwFourCC != MAKEFOURCC('M', 'I', 'P', 'T'))
	{
		TraceError("CPythonItem::LoadItemTable: invalid item proto type %s", c_szFileName);
		return false;
	}

	file.Read(&dwElements, sizeof(DWORD));
	file.Read(&dwDataSize, sizeof(DWORD));

	BYTE * pbData = new BYTE[dwDataSize];
	file.Read(pbData, dwDataSize);

	/////

	CLZObject zObj;

	if (!CLZO::Instance().Decompress(zObj, pbData, s_adwItemProtoKey))
	{
		delete [] pbData;
		return false;
	}

	/////

	char szName[64+1];
	std::map<DWORD,DWORD> itemNameMap;
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	m_NameItemMap.clear();
#endif

	for (DWORD i = 0; i < dwElements; ++i)
	{
#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
		CItemData::TItemTable t = {0};
		CItemData::TItemTableAll::Process(zObj.GetBuffer(), dwStride, i, t);
#else
		CItemData::TItemTable & t = *((CItemData::TItemTable *) zObj.GetBuffer() + i);
#endif
		CItemData::TItemTable * table = &t;

		CItemData * pItemData;
		DWORD dwVnum = table->dwVnum;

		TItemMap::iterator f = m_ItemMap.find(dwVnum);
		if (m_ItemMap.end() == f)
		{
			_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", dwVnum);
#ifdef ENABLE_INGAME_WIKI
			pItemData = CItemData::New();
#endif

			if (CResourceManager::Instance().IsFileExist(szName) == false)
			{
				std::map<DWORD, DWORD>::iterator itVnum = itemNameMap.find(GetHashCode(table->szName));

				if (itVnum != itemNameMap.end())
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", itVnum->second);
				else
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", dwVnum-dwVnum % 10);

				if (CResourceManager::Instance().IsFileExist(szName) == false)
				{
#ifdef ENABLE_INGAME_WIKI
					pItemData->ValidateImage(false);
#endif
					#ifdef _DEBUG
					TraceError("%16s(#%-5d) cannot find icon file. setting to default.", table->szName, dwVnum);
					#endif
					const DWORD EmptyBowl = 27995;
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", EmptyBowl);
				}
			}

			pItemData = CItemData::New();

			pItemData->SetDefaultItemData(szName);
			m_ItemMap.insert(TItemMap::value_type(dwVnum, pItemData));
#ifdef ENABLE_INGAME_WIKI
			pItemData->SetItemTableData(table);
			if (!CResourceManager::Instance().IsFileExist(pItemData->GetIconFileName().c_str()))
				pItemData->ValidateImage(false);
#endif
		}
		else
		{
			pItemData = f->second;
#ifdef ENABLE_INGAME_WIKI
			pItemData->SetItemTableData(table);
#endif
		}
		if (itemNameMap.find(GetHashCode(table->szName)) == itemNameMap.end())
			itemNameMap.insert(std::map<DWORD,DWORD>::value_type(GetHashCode(table->szName),table->dwVnum));
		pItemData->SetItemTableData(table);

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
		m_NameItemMap.emplace(pItemData->GetName(), pItemData);
#endif

		if (0 != table->dwVnumRange)
		{
			m_vec_ItemRange.push_back(pItemData);
		}
	}

	delete [] pbData;
	return true;
}

void CItemManager::Destroy()
{
	TItemMap::iterator i;
	for (i=m_ItemMap.begin(); i!=m_ItemMap.end(); ++i)
		CItemData::Delete(i->second);

	m_ItemMap.clear();
#ifdef ENABLE_EXTENDED_BLEND
	m_BlendMap.clear();
#endif
#ifdef ENABLE_INGAME_WIKI
	m_tempItemVec.clear();
#endif
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
bool CItemManager::LoadItemScale(const char * c_szFileName)
{
	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
		return false;

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		if (kTokenVector.size() < ITEMSCALE_REQ)
		{
			TraceError("LoadItemScale: invalid line %d (%s).", i, c_szFileName);
			continue;
		}

		static std::map<std::pair<std::string, std::string>, std::string> scaleMap = {
			{std::make_pair("JOB_WOLFMAN",	"M"),	"0"},
			{std::make_pair("JOB_ASSASSIN",	"M"),	"0.26"},
			{std::make_pair("JOB_ASSASSIN",	"F"),	"0.38"},
			{std::make_pair("JOB_SHAMAN",	"M"),	"0.18"},
			{std::make_pair("JOB_SHAMAN",	"F"),	"0.26"},
			{std::make_pair("JOB_SURA",	"M"),	"0.21"},
			{std::make_pair("JOB_SURA",	"F"),	"0.3"},
			{std::make_pair("JOB_WARRIOR",	"M"),	"0.15"},
			{std::make_pair("JOB_WARRIOR",	"F"),	"0.26"},
		};
		std::string strJob = kTokenVector[ITEMSCALE_JOB];
		std::string strSex = kTokenVector[ITEMSCALE_SEX];
		std::transform(strJob.begin(), strJob.end(), strJob.begin(), ::toupper);
		std::transform(strSex.begin(), strSex.end(), strSex.begin(), ::toupper);

		const std::string & strScaleX = kTokenVector[ITEMSCALE_SCALE_X];
		const std::string & strScaleY = kTokenVector[ITEMSCALE_SCALE_Y];
		const std::string & strScaleZ = kTokenVector[ITEMSCALE_SCALE_Z];
		std::string strPositionX = "0";
		std::string strPositionY = "0";
		std::string strPositionZ = scaleMap[std::make_pair(strJob, strSex)];
		if (kTokenVector.size() == ITEMSCALE_NUM)
		{
			strPositionX = kTokenVector[ITEMSCALE_POSITION_X];
			strPositionY = kTokenVector[ITEMSCALE_POSITION_Y];
			strPositionZ = kTokenVector[ITEMSCALE_POSITION_Z];
		}
		else if (kTokenVector.size() == ITEMSCALE_AURA_NUM)
		{
			strPositionZ = kTokenVector[ITEMSCALE_POSITION_X];
		}
		//TraceError("strPositionX %s strPositionY %s strPositionZ %s", strPositionX.c_str(), strPositionY.c_str(), strPositionZ.c_str());

		for (int j = 0; j < 5; ++j)
		{
			CItemData * pItemData = MakeItemData(atoi(kTokenVector[ITEMSCALE_VNUM].c_str()) + j);
			pItemData->SetItemScale(strJob, strSex, strScaleX, strScaleY, strScaleZ, strPositionX, strPositionY, strPositionZ);
		}
	}

	return true;
}
#endif

#ifdef ENABLE_SHINING_TABLE
bool CItemManager::LoadShiningTable(const char* szShiningTable)
{
	CMappedFile File;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(File, szShiningTable, &pData))
	{
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);
	CTokenVector TokenVector;

	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
		{
			continue;
		}


		if (TokenVector.size() > (1 + CItemData::ITEM_SHINING_MAX_COUNT))
		{
			TraceError("CItemManager::LoadShiningTable(%s) - LoadShiningTable in %d\n - RowSize: %d MaxRowSize: %d", szShiningTable, i, TokenVector.size(), CItemData::ITEM_SHINING_MAX_COUNT);
		}

		const std::string & c_rstrID = TokenVector[0];

		int pos = c_rstrID.find("~");

		if (std::string::npos == pos)
		{
			DWORD dwItemVNum = atoi(c_rstrID.c_str());

			CItemData * pItemData = MakeItemData(dwItemVNum);
			if (pItemData)
			{
				for (BYTE i = 0; i < CItemData::ITEM_SHINING_MAX_COUNT; i++)
				{
					if (i < (TokenVector.size() - 1))
					{
						const std::string & c_rstrEffectPath = TokenVector[i + 1];
						pItemData->SetItemShiningTableData(i, c_rstrEffectPath.c_str());
					}
					else
					{
						pItemData->SetItemShiningTableData(i, "");
					}
				}
			}
		}
		else
		{
			std::string s_start_vnum(c_rstrID.substr(0, pos));
			std::string s_end_vnum(c_rstrID.substr(pos + 1));

			int start_vnum = atoi(s_start_vnum.c_str());
			int end_vnum = atoi(s_end_vnum.c_str());
			DWORD vnum = start_vnum;

			while (vnum <= end_vnum)
			{
				CItemData * pItemData = MakeItemData(vnum);
				if (pItemData)
				{
					for (BYTE i = 0; i < CItemData::ITEM_SHINING_MAX_COUNT; i++)
					{
						if (i < (TokenVector.size() - 1))
						{
							const std::string & c_rstrEffectPath = TokenVector[i + 1];
							pItemData->SetItemShiningTableData(i, c_rstrEffectPath.c_str());
						}
						else
						{
							pItemData->SetItemShiningTableData(i, "");
						}
					}
				}

				++vnum;
			}
		}
	}

	return true;
}
#endif

#ifdef ENABLE_CHEST_DROP_INFO
bool CItemManager::LoadChestDropInfo(const char* c_szFileName)
{
	if (m_ItemDropInfoMap.empty() == false)
	{
		return true;
	}

	CMappedFile file;
	LPCVOID pvData;

	if (!CEterPackManager::Instance().Get(file, c_szFileName, &pvData))
	{
		return false;
	}

	size_t mapSize = 0;
	file.Read(&mapSize, sizeof(mapSize));

	for (size_t i = 0; i < mapSize; i++)
	{
		DWORD dwItemVnum = 0;
		file.Read(&dwItemVnum, sizeof(dwItemVnum));

		size_t vecSize = 0;
		file.Read(&vecSize, sizeof(vecSize));

		TChestDropItemInfoVec& vecDrop = m_ItemDropInfoMap[dwItemVnum];
		vecDrop.reserve(vecSize);

		for (size_t j = 0; j < vecSize; j++)
		{
			DWORD dwDropVnum = 0;
			file.Read(&dwDropVnum, sizeof(dwDropVnum));

			int iCount = 0;
			file.Read(&iCount, sizeof(iCount));

			vecDrop.push_back({ dwDropVnum, iCount });
		}
	}

	for (CItemManager::TChestDropItemInfoMap::iterator it = m_ItemDropInfoMap.begin(); it != m_ItemDropInfoMap.end(); ++it)
	{
		CItemManager::TChestDropItemInfoVec& vecDrop = it->second;

		std::sort(vecDrop.begin(), vecDrop.end(),
				  [](const CItemManager::SDropItemInfo & a, const CItemManager::SDropItemInfo & b)
		{
			CItemData* pItemData[2];
			if (CItemManager::Instance().GetItemDataPointer(a.dwDropVnum, &pItemData[0]) && CItemManager::Instance().GetItemDataPointer(b.dwDropVnum, &pItemData[1]))
			{
				return pItemData[0]->GetSize() < pItemData[1]->GetSize();
			}

			return false;
		});
	}

	return true;
}

CItemManager::TChestDropItemInfoVec* CItemManager::GetItemDropInfoVec(const DWORD dwVnum)
{
	CItemManager::TChestDropItemInfoMap::iterator it = m_ItemDropInfoMap.find(dwVnum);
	if (it != m_ItemDropInfoMap.end())
	{
		return &(it->second);
	}

	return nullptr;
}
#endif


#ifdef ENABLE_INGAME_WIKI
BOOL CItemManager::CanIncrSelectedItemRefineLevel()
{
	auto* tbl = GetSelectedItemDataPointer();
	if (!tbl)
		return FALSE;

	return (tbl->GetType() == CItemData::ITEM_TYPE_ARMOR || tbl->GetType() == CItemData::ITEM_TYPE_WEAPON || tbl->GetType() == CItemData::ITEM_TYPE_BELT || tbl->GetType() == CItemData::ITEM_TYPE_UNIQUE
		|| tbl->GetType() == CItemData::ITEM_TYPE_METIN
		);
}

BOOL CItemManager::CanIncrItemRefineLevel(DWORD itemVnum)
{
	CItemData* tbl = nullptr;
	if (!GetItemDataPointer(itemVnum, &tbl))
		return false;

	if (!tbl)
		return FALSE;

	return (tbl->GetType() == CItemData::ITEM_TYPE_ARMOR || tbl->GetType() == CItemData::ITEM_TYPE_WEAPON || tbl->GetType() == CItemData::ITEM_TYPE_BELT || tbl->GetType() == CItemData::ITEM_TYPE_UNIQUE
		
		|| tbl->GetType() == CItemData::ITEM_TYPE_METIN
		);
}

bool CItemManager::CanLoadWikiItem(DWORD dwVnum)
{
	DWORD StartRefineVnum = GetWikiItemStartRefineVnum(dwVnum);
	
	if (StartRefineVnum != dwVnum)
		return false;
	
	if (StartRefineVnum % 10 != 0)
		return false;
	
	CItemData * tbl = nullptr;
	if (!GetItemDataPointer(StartRefineVnum, &tbl))
		return false;
	
	return true;
}

DWORD CItemManager::GetWikiItemStartRefineVnum(DWORD dwVnum)
{
	auto baseItemName = GetWikiItemBaseRefineName(dwVnum);
	if (!baseItemName.size())
		return 0;
	
	DWORD manage_vnum = dwVnum;
	while (!(strcmp(baseItemName.c_str(), GetWikiItemBaseRefineName(manage_vnum).c_str())))
		--manage_vnum;
	
	return (manage_vnum + 1);
}

std::string CItemManager::GetWikiItemBaseRefineName(DWORD dwVnum)
{
	CItemData * tbl = nullptr;
	if (!GetItemDataPointer(dwVnum, &tbl))
		return "";
	
	auto* p = const_cast<char*>(strrchr(tbl->GetName(), '+'));
	if (!p)
		return "";
	
	std::string sFirstItemName(tbl->GetName(),
				(tbl->GetName() + (p - tbl->GetName())));
	
	return sFirstItemName;
}

bool CItemManager::IsFilteredAntiflag(CItemData* itemData, DWORD raceFilter)
{
	if (raceFilter != 0)
	{
		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_SHAMAN) && raceFilter & CItemData::ITEM_ANTIFLAG_SHAMAN)
			return false;

		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_SURA) && raceFilter & CItemData::ITEM_ANTIFLAG_SURA)
			return false;

		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_ASSASSIN) && raceFilter & CItemData::ITEM_ANTIFLAG_ASSASSIN)
			return false;

		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_WARRIOR) && raceFilter & CItemData::ITEM_ANTIFLAG_WARRIOR)
			return false;

#if defined(ENABLE_INGAME_WIKI) && defined(ENABLE_WOLFMAN)
		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_WOLFMAN) && raceFilter & CItemData::ITEM_ANTIFLAG_WOLFMAN)
			return false;
#endif
	}

	return true;
}

size_t CItemManager::WikiLoadClassItems(BYTE classType, DWORD raceFilter)
{
	m_tempItemVec.clear();

	for (TItemMap::iterator it = m_ItemMap.begin(); it != m_ItemMap.end(); ++it)
	{
		if (!it->second->IsValidImage() || it->first < 10 || it->second->IsBlacklisted())
			continue;
		
		bool _can_load = CanLoadWikiItem(it->first);
		
		switch (classType)
		{
		case 0: // weapon
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_WEAPON && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 1: // body
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_BODY && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 2:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_EAR && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 3:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_FOOTS && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 4:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_HEAD && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 5:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_NECK && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 6:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_SHIELD && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 7:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_WRIST && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;
		case 8: // chests
			if (it->second->GetType() == CItemData::ITEM_TYPE_GIFTBOX)
				m_tempItemVec.push_back(it->first);
			break;
		case 9: // belts
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_BELT)
				m_tempItemVec.push_back(it->first);
			break;
		case 10: // talisman
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_PENDANT && !IsFilteredAntiflag(it->second, raceFilter))
				m_tempItemVec.push_back(it->first);
			break;

		case 11: //stones
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_METIN)
				m_tempItemVec.push_back(it->first);
			break;

		case 12: //rings
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_UNIQUE)
				m_tempItemVec.push_back(it->first);
			break;

		}
	}

	return m_tempItemVec.size();
}

std::tuple<const char*, int> CItemManager::SelectByNamePart(const char* namePart)
{
	char searchName[CItemData::ITEM_NAME_MAX_LEN + 1];
	memcpy(searchName, namePart, sizeof(searchName));
	for (size_t j = 0; j < sizeof(searchName); j++)
	{
		searchName[j] = static_cast<char>(tolower(searchName[j]));
	}

	std::string tempSearchName = searchName;

	for (TItemMap::iterator i = m_ItemMap.begin(); i != m_ItemMap.end(); i++)
	{
		const CItemData::TItemTable* tbl = i->second->GetTable();

		if (!i->second->IsBlacklisted())
		{
			DWORD StartRefineVnum = GetWikiItemStartRefineVnum(i->first);
			if (StartRefineVnum != 0)
			{
				CItemData * _sRb = nullptr;
				if (!GetItemDataPointer(StartRefineVnum, &_sRb))
					continue;

				if (_sRb->IsBlacklisted())
					continue;
			}
		}
		else
			continue;
		
		CItemData* itemData = nullptr;
		if (!GetItemDataPointer(i->first, &itemData))
			continue;

		std::string tempName = itemData->GetName();
		if (!tempName.size())
			continue;

		std::transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);

		const size_t tempSearchNameLenght = tempSearchName.length();
		if (tempName.length() < tempSearchNameLenght)
			continue;

		if (!tempName.substr(0, tempSearchNameLenght).compare(tempSearchName))
			return std::make_tuple(itemData->GetName(), i->first);
	}
	
	return std::make_tuple("", -1);
}
#endif


#ifdef ENABLE_EXTENDED_BLEND
#define ATTR_INFO std::pair<const char*, BYTE>
BYTE CItemManager::GetAttrTypeByName(const std::string& stAttrType)
{
	if (stAttrType == "")
	{
		return CItemData::APPLY_NONE;
	}

	static const ATTR_INFO astAttrInfo[] =
	{
		ATTR_INFO("NONE", CItemData::APPLY_NONE),
		ATTR_INFO("MAX_HP", CItemData::APPLY_MAX_HP),
		ATTR_INFO("MAX_SP", CItemData::APPLY_MAX_SP),
		ATTR_INFO("CON", CItemData::APPLY_CON),
		ATTR_INFO("STR", CItemData::APPLY_STR),
		ATTR_INFO("INT", CItemData::APPLY_INT),
		ATTR_INFO("DEX", CItemData::APPLY_DEX),
		ATTR_INFO("ATT_SPEED", CItemData::APPLY_ATT_SPEED),
		ATTR_INFO("ATTACK_SPEED", CItemData::APPLY_ATT_SPEED),
		ATTR_INFO("MOV_SPEED", CItemData::APPLY_MOV_SPEED),
		ATTR_INFO("STUN_PCT", CItemData::APPLY_STUN_PCT),
		ATTR_INFO("CRITICAL_PCT", CItemData::APPLY_CRITICAL_PCT),
		ATTR_INFO("PENETRATE_PCT", CItemData::APPLY_PENETRATE_PCT),
		ATTR_INFO("POISON_PCT", CItemData::APPLY_POISON_PCT),
		ATTR_INFO("BLOCK", CItemData::APPLY_BLOCK),
		ATTR_INFO("DODGE", CItemData::APPLY_DODGE),
		ATTR_INFO("RESIST_MAGIC", CItemData::APPLY_RESIST_MAGIC),
		ATTR_INFO("ITEM_DROP_BONUS", CItemData::APPLY_ITEM_DROP_BONUS),
		ATTR_INFO("ATT_BONUS", CItemData::APPLY_ATT_GRADE_BONUS),
		ATTR_INFO("ATT_GRADE_BONUS", CItemData::APPLY_ATT_GRADE_BONUS),
		ATTR_INFO("ATTBONUS_HUMAN", CItemData::APPLY_ATTBONUS_HUMAN),
		ATTR_INFO("ATTBONUS_MONSTER", CItemData::APPLY_ATT_BONUS_TO_MONSTER),
		ATTR_INFO("DEF_BONUS", CItemData::APPLY_DEF_GRADE_BONUS),
		ATTR_INFO("DEF_GRADE_BONUS", CItemData::APPLY_DEF_GRADE_BONUS),
		ATTR_INFO("NORMAL_HIT_DAMAGE_BONUS", CItemData::APPLY_NORMAL_HIT_DAMAGE_BONUS),
		ATTR_INFO("NORMAL_HIT_DEFEND_BONUS", CItemData::APPLY_NORMAL_HIT_DEFEND_BONUS),
		ATTR_INFO("SKILL_DAMAGE_BONUS", CItemData::APPLY_SKILL_DAMAGE_BONUS),
		ATTR_INFO("SKILL_DEFEND_BONUS", CItemData::APPLY_SKILL_DEFEND_BONUS),
		ATTR_INFO("ANTI_PENETRATE_PCT", CItemData::APPLY_ANTI_PENETRATE_PCT),
		ATTR_INFO("ATTBONUS_UNDEAD", CItemData::APPLY_ATTBONUS_UNDEAD),
		ATTR_INFO("CAST_SPEED", CItemData::APPLY_CAST_SPEED),
		ATTR_INFO("ENERGY", CItemData::APPLY_ENERGY),
		ATTR_INFO("ATTBONUS_ANIMAL", CItemData::APPLY_ATTBONUS_ANIMAL),
		ATTR_INFO("ATTBONUS_ORC", CItemData::APPLY_ATTBONUS_ORC),
		ATTR_INFO("ATTBONUS_MILGYO", CItemData::APPLY_ATTBONUS_MILGYO),
		ATTR_INFO("ATTBONUS_DEVIL", CItemData::APPLY_ATTBONUS_DEVIL),
		ATTR_INFO("RESIST_ICE", CItemData::APPLY_RESIST_ICE),
		ATTR_INFO("RESIST_EARTH", CItemData::APPLY_RESIST_EARTH),
		ATTR_INFO("RESIST_DARK", CItemData::APPLY_RESIST_DARK),
		ATTR_INFO("RESIST_FIRE", CItemData::APPLY_RESIST_FIRE),
		ATTR_INFO("RESIST_ELEC", CItemData::APPLY_RESIST_ELEC),
		ATTR_INFO("RESIST_WIND", CItemData::APPLY_RESIST_WIND),
		ATTR_INFO("ANTI_CRITICAL_PCT", CItemData::APPLY_ANTI_CRITICAL_PCT),
		ATTR_INFO("REFLECT_MELEE", CItemData::APPLY_REFLECT_MELEE),
		ATTR_INFO("HP_REGEN", CItemData::APPLY_HP_REGEN),
		ATTR_INFO("SP_REGEN", CItemData::APPLY_SP_REGEN),
		ATTR_INFO("STEAL_HP", CItemData::APPLY_STEAL_HP),
		ATTR_INFO("STEAL_SP", CItemData::APPLY_STEAL_SP),
		ATTR_INFO("POTION_BONUS", CItemData::APPLY_POTION_BONUS),
		ATTR_INFO("MALL_ATTBONUS", CItemData::APPLY_MALL_ATTBONUS),
		ATTR_INFO("MALL_DEFBONUS", CItemData::APPLY_MALL_DEFBONUS),
		ATTR_INFO("MAX_HP_PCT", CItemData::APPLY_MAX_HP_PCT),
		ATTR_INFO("MAX_SP_PCT", CItemData::APPLY_MAX_SP_PCT),
		ATTR_INFO("IMMUNE_STUN", CItemData::APPLY_IMMUNE_STUN),
		ATTR_INFO("GOLD_DOUBLE_BONUS", CItemData::APPLY_GOLD_DOUBLE_BONUS),
		ATTR_INFO("MELEE_MAGIC_ATTBONUS_PER", CItemData::APPLY_MELEE_MAGIC_ATTBONUS_PER),
		ATTR_INFO("SLOW_PCT", CItemData::APPLY_SLOW_PCT),
		ATTR_INFO("MANA_BURN_PCT", CItemData::APPLY_MANA_BURN_PCT),
		ATTR_INFO("RESIST_SWORD", CItemData::APPLY_RESIST_SWORD),
		ATTR_INFO("RESIST_TWOHAND", CItemData::APPLY_RESIST_TWOHAND),
		ATTR_INFO("RESIST_DAGGER", CItemData::APPLY_RESIST_DAGGER),
		ATTR_INFO("RESIST_BELL", CItemData::APPLY_RESIST_BELL),
		ATTR_INFO("RESIST_FAN", CItemData::APPLY_RESIST_FAN),
		ATTR_INFO("RESIST_BOW", CItemData::APPLY_RESIST_BOW),
		ATTR_INFO("POISON_REDUCE", CItemData::APPLY_POISON_REDUCE),
		ATTR_INFO("EXP_DOUBLE_BONUS", CItemData::APPLY_EXP_DOUBLE_BONUS),
		ATTR_INFO("IMMUNE_SLOW", CItemData::APPLY_IMMUNE_SLOW),
	};

	for (int i = 0; i < sizeof(astAttrInfo) / sizeof(ATTR_INFO); ++i)
	{
		if (!stricmp(stAttrType.c_str(), astAttrInfo[i].first))
		{
			return astAttrInfo[i].second;
		}
	}

	TraceError("GetAttrTypeByName(%s) - cannot get attribute type\n", stAttrType.c_str());
	return CItemData::APPLY_NONE;
}
#undef ATTR_INFO

BOOL CItemManager::GetBlendInfoPointer(DWORD dwItemID, TBlendInfo ** ppBlendInfo)
{
	if (0 == dwItemID)
	{
		return FALSE;
	}

	TBlendMap::iterator f = m_BlendMap.find(dwItemID);
	if (f == m_BlendMap.end())
	{
		return FALSE;
	}

	*ppBlendInfo = &f->second;
	return TRUE;
}

bool CItemManager::LoadItemBlend(const char* c_szFileName)
{
	CMappedFile File;
	LPCVOID pData;

	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pData))
	{
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	TBlendInfo kBlendInfo;
	BYTE bIndex = 0;

	CTokenVector TokenVector;
	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
		{
			continue;
		}

		switch (bIndex)
		{
		case 0:
		{
			if (TokenVector.size() >= 1 && TokenVector[0].find_first_of("section") == 0)
			{
				kBlendInfo.dwVnum = 0;
				kBlendInfo.bApplyType = 0;
				kBlendInfo.vec_iApplyValue.clear();
				kBlendInfo.vec_iApplyDuration.clear();
				++bIndex;
			}
		}
		break;

		case 1:
		{
			if (TokenVector.size() >= 2)
			{
				if (TokenVector[0].find_first_of("item_vnum") != 0)
				{
					TraceError("invalid section line [item_vnum] %u", i);
					bIndex = 0;
					break;
				}

				kBlendInfo.dwVnum = atoi(TokenVector[1].c_str());
				++bIndex;
			}
		}
		break;

		case 2:
		{
			if (TokenVector.size() >= 2)
			{
				if (TokenVector[0].find_first_of("apply_type") != 0)
				{
					TraceError("invalid section line [apply_type] %u", i);
					bIndex = 0;
					break;
				}

				kBlendInfo.bApplyType = GetAttrTypeByName(TokenVector[1]);
				++bIndex;
			}
		}
		break;

		case 3:
		{
			if (TokenVector.size() >= 2)
			{
				if (TokenVector[0].find_first_of("apply_value") != 0)
				{
					TraceError("invalid section line [apply_value] %u", i);
					bIndex = 0;
					break;
				}

				for (int j = 1; j < TokenVector.size(); ++j)
				{
					kBlendInfo.vec_iApplyValue.push_back(atoi(TokenVector[j].c_str()));
				}
				++bIndex;
			}
		}
		break;

		case 4:
		{
			if (TokenVector.size() >= 2)
			{
				if (TokenVector[0].find_first_of("apply_duration") != 0)
				{
					TraceError("invalid section line [apply_duration] %u", i);
					bIndex = 0;
					break;
				}

				for (int j = 1; j < TokenVector.size(); ++j)
				{
					kBlendInfo.vec_iApplyDuration.push_back(atoi(TokenVector[j].c_str()));
				}
				++bIndex;
			}
		}
		break;

		case 5:
		{
			if (TokenVector.size() >= 1 && TokenVector[0].find_first_of("end") == 0)
			{
				bIndex = 0;

				if (kBlendInfo.dwVnum == 0)
				{
					TraceError("invalid vnum for blend line %u vnum 0", i);
					break;
				}

				if (kBlendInfo.bApplyType == 0)
				{
					TraceError("invalid apply type for blend vnum %u", kBlendInfo.dwVnum);
					break;
				}

				if (kBlendInfo.vec_iApplyValue.size() != kBlendInfo.vec_iApplyDuration.size())
				{
					TraceError("invalid size compare of iApplyValue and iApplyDuration vnum %u", kBlendInfo.dwVnum);
					break;
				}

				TBlendInfo& rkInfo = m_BlendMap[kBlendInfo.dwVnum];
				rkInfo.dwVnum = kBlendInfo.dwVnum;
				rkInfo.bApplyType = kBlendInfo.bApplyType;
				for (int j = 0; j < kBlendInfo.vec_iApplyValue.size(); ++j)
				{
					rkInfo.vec_iApplyValue.push_back(kBlendInfo.vec_iApplyValue[j]);
					rkInfo.vec_iApplyDuration.push_back(kBlendInfo.vec_iApplyDuration[j]);
				}
			}
		}
		break;
		}
	}

	return true;
}
#endif

CItemManager::CItemManager() : m_pSelectedItemData(NULL)
{
}
CItemManager::~CItemManager()
{
	Destroy();
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
