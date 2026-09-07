#include "StdAfx.h"
#include "PythonPrivateShop.h"
#include "PythonNetworkStream.h"
#include "PythonBackground.h"
#include "PythonTextTail.h"
#include "PythonPlayer.h"
#include "PythonSystem.h"
#include "PythonMiniMap.h"

#include "../EterPack/EterPackManager.h"
#include "../GameLib/ActorInstance.h"
#include "../GameLib/RaceManager.h"
#include "../GameLib/RaceMotionData.h"

const char* c_szEffectName = "d:/ymir work/effect/etc/direction/direction_land2.mse";

CPythonPrivateShop::TPrivateShopInstance* CPythonPrivateShop::CreatePrivateShopInstance(DWORD dwVirtualID, DWORD dwVirtualNumber, const char* c_szName, long lX, long lY, long lZ)
{
	auto it = m_map_privateShopInstance.find(dwVirtualID);

	if (it != m_map_privateShopInstance.end())
		DeletePrivateShopInstance(dwVirtualID);

	std::unique_ptr<TPrivateShopInstance> upPrivateShopInstance = std::make_unique<TPrivateShopInstance>();
	TPrivateShopInstance* pPrivateShopInstance = upPrivateShopInstance.get();

	CPythonBackground& rkBgMgr = CPythonBackground::Instance();
	rkBgMgr.GlobalPositionToLocalPosition(lX, lY);
	lZ = CPythonBackground::Instance().GetHeight(lX, lY);

	pPrivateShopInstance->SetVID(dwVirtualID);
	pPrivateShopInstance->SetName(c_szName);

	if (!pPrivateShopInstance->LoadRace(dwVirtualNumber, TPixelPosition((float)lX, (float)lY, (float)lZ)))
	{
		delete(pPrivateShopInstance);
		return nullptr;
	}

	m_map_privateShopInstance.emplace(dwVirtualID, std::move(upPrivateShopInstance));
	CPythonTextTail::Instance().RegisterPrivateShopTextTail(dwVirtualID);

	return pPrivateShopInstance;
}

CPythonPrivateShop::TPrivateShopInstance* CPythonPrivateShop::GetPrivateShopInstance(DWORD dwVirtualID)
{
	auto it = m_map_privateShopInstance.find(dwVirtualID);

	if (it == m_map_privateShopInstance.end())
		return nullptr;

	return it->second.get();
}

DWORD CPythonPrivateShop::GetPrivateShopInstanceVID(const char* c_szOwnerName)
{
	for (const auto& kv : m_map_privateShopInstance)
	{
		TPrivateShopInstance* pPrivateShopInstance = kv.second.get();
		if (strcmp(pPrivateShopInstance->GetName(), c_szOwnerName) == 0)
			return pPrivateShopInstance->GetVID();
	}

	return 0;

}
bool CPythonPrivateShop::DeletePrivateShopInstance(DWORD dwVirtualID)
{
	auto it = m_map_privateShopInstance.find(dwVirtualID);

	if (it == m_map_privateShopInstance.end())
		return false;

	CPythonTextTail::Instance().DeletePrivateShopTextTail(dwVirtualID);
	UnselectSearchPrivateShop(dwVirtualID);

	TPrivateShopInstance* pPrivateShopInstance =it->second.get();
	pPrivateShopInstance->GetGraphicThingInstancePtr()->Clear();

	m_map_privateShopInstance.erase(it);
	return true;
}

bool CPythonPrivateShop::GetPickedInstanceVID(DWORD* p_dwPrivateShopVID)
{
	if (!GetSelectedInstance())
		return false;

	*p_dwPrivateShopVID = GetSelectedInstance()->GetVID();
	return true;
}

bool CPythonPrivateShop::GetPrivateShopPosition(DWORD dwVirtualID, TPixelPosition* pPosition)
{
	CPythonPrivateShop::TPrivateShopInstance* pPrivateShopInstance = GetPrivateShopInstance(dwVirtualID);
	if (!pPrivateShopInstance)
		return false;

	const D3DXVECTOR3& rkD3DVct3 = pPrivateShopInstance->GetGraphicThingInstanceRef().GetPosition();

	pPosition->x = +rkD3DVct3.x;
	pPosition->y = -rkD3DVct3.y;
	pPosition->z = +rkD3DVct3.z;

	return true;
}

void CPythonPrivateShop::SelectSearchPrivateShop(DWORD dwVirtualID)
{
	auto it = m_map_selectedPrivateShop.find(dwVirtualID);
	if (it != m_map_selectedPrivateShop.end())
		return;

	TPrivateShopInstance* pPrivateShopInstance = GetPrivateShopInstance(dwVirtualID);
	if (!pPrivateShopInstance)
		return;
	
	const D3DXVECTOR3& rkD3DVct3 = pPrivateShopInstance->GetGraphicThingInstanceRef().GetPosition();
	CPythonMiniMap::Instance().AddWayPoint(CPythonMiniMap::TYPE_TARGET, dwVirtualID, rkD3DVct3.x, -rkD3DVct3.y, "", dwVirtualID);

	pPrivateShopInstance->AttachSelectEffect();
	m_map_selectedPrivateShop.emplace(dwVirtualID, pPrivateShopInstance);
}

void CPythonPrivateShop::UnselectSearchPrivateShop(DWORD dwVirtualID)
{
	auto it = m_map_selectedPrivateShop.find(dwVirtualID);
	if (it == m_map_selectedPrivateShop.end())
		return;

	TPrivateShopInstance* pPrivateShopInstance = it->second;
	pPrivateShopInstance->DetachSelectEffect();

	CPythonMiniMap::Instance().RemoveWayPoint(pPrivateShopInstance->GetVID());
	
	m_map_selectedPrivateShop.erase(it);
}

void CPythonPrivateShop::ClearSelectedSearchPrivateShop()
{
	for (const auto kv : m_map_selectedPrivateShop)
	{
		TPrivateShopInstance* pPrivateShopInstance = kv.second;
		pPrivateShopInstance->DetachSelectEffect();

		CPythonMiniMap::Instance().RemoveWayPoint(pPrivateShopInstance->GetVID());
	}

	m_map_selectedPrivateShop.clear();
}

void CPythonPrivateShop::Render()
{
	if (m_map_privateShopInstance.empty())
		return;

	for (const auto& kv : m_map_privateShopInstance)
	{
		TPrivateShopInstance* pPrivateShopInstance = kv.second.get();

		CGraphicThingInstance* pInstance = pPrivateShopInstance->GetGraphicThingInstancePtr();
		if (pInstance && pInstance->isShow())
			pInstance->Render();
	}
}

void CPythonPrivateShop::Deform()
{
	for (const auto& kv : m_map_privateShopInstance)
	{
		TPrivateShopInstance* pPrivateShopInstance = kv.second.get();

		CGraphicThingInstance* pInstance = pPrivateShopInstance->GetGraphicThingInstancePtr();
		if (pInstance && pInstance->isShow())
			pInstance->Deform();
	}
}

void CPythonPrivateShop::Update()
{
	for (const auto& kv : m_map_privateShopInstance)
	{
		TPrivateShopInstance* pPrivateShopInstance = kv.second.get();

		CGraphicThingInstance* pInstance = pPrivateShopInstance->GetGraphicThingInstancePtr();
		if (pInstance)
		{
			// Check view distance
			{
				const TPixelPosition c_ShopPosition = pInstance->GetPosition();

				TPixelPosition MainActorPosition;
				CPythonPlayer::Instance().NEW_GetMainActorPosition(&MainActorPosition);

				float fdx = MainActorPosition.x - c_ShopPosition.x;
				float fdy = MainActorPosition.y - (-c_ShopPosition.y); // Using different axis on the private shop instance -_-

				float fDistance = sqrtf((fdx * fdx) + (fdy * fdy));

#ifdef ENABLE_PRIVATE_SHOP_LIMITED_DISTANCE_RENDERING
				auto dist = CPythonSystem::Instance().GetPrivateShopViewDistance() * MAX_VIEW_DISTANCE;
				if (fDistance > dist)
				{
					if (pInstance->isShow())
					{
						pPrivateShopInstance->HideAllAttachingEffect();
						pInstance->Hide();
					}
				}
				else
				{
					if (!pInstance->isShow())
					{
						pPrivateShopInstance->ShowAllAttachingEffect();
						pInstance->Show();
					}
				}
#endif
			}

			if (pInstance->isShow())
			{
				pInstance->Update();
				pPrivateShopInstance->UpdateAttachingEffect();
				pInstance->Transform();
			}
		}
	}

	for (const auto& kv : m_map_privateShopInstance)
	{
		TPrivateShopInstance* pPrivateShopInstance = kv.second.get();

		CGraphicThingInstance* pInstance = pPrivateShopInstance->GetGraphicThingInstancePtr();
		if (!pInstance)
			continue;

		if (!pInstance->isShow())
			continue;

		float fU, fV, fT;
		if (pInstance->Intersect(&fU, &fV, &fT))
		{
			SetSelectedInstance(pPrivateShopInstance);
			return;
		}
	}

	// This part is reached if no instance intersects 
	if (GetSelectedInstance())
		SetSelectedInstance(nullptr);
}

void CPythonPrivateShop::Destroy()
{
	if (!m_map_privateShopInstance.empty())
	{
		for (const auto& kv : m_map_privateShopInstance)
		{
			TPrivateShopInstance* pPrivateShopInstance = kv.second.get();

			CPythonTextTail::Instance().DeletePrivateShopTextTail(pPrivateShopInstance->GetVID());
			pPrivateShopInstance->GetGraphicThingInstancePtr()->Clear();
		}
	}

	m_map_privateShopInstance.clear();
	m_vec_saleItem.clear();
	m_map_marketItemPrice.clear();
	m_dwPremiumTime = 0;

	ClearMyPrivateShop();
	Clear();

	SetSelectedInstance(nullptr);
}

bool CPythonPrivateShop::SPrivateShopInstance::LoadRace(DWORD dwVirtualNumber, const TPixelPosition& c_rPixelPosition)
{
	CRaceData* pRaceData;
	if (!CRaceManager::Instance().GetRaceDataPointer(dwVirtualNumber, &pRaceData))
	{
		TraceError("Could not load data for race %d", dwVirtualNumber);
		return false;
	}

	CGraphicThingInstance* pInstance = CGraphicThingInstance::New();
	m_pGraphicThingInstance = pInstance;

	m_pGraphicThingInstance->ReserveModelThing(1);
	m_pGraphicThingInstance->ReserveModelInstance(1);
	m_pGraphicThingInstance->RegisterModelThing(0, (CGraphicThing*)CResourceManager::Instance().GetResourcePointer(pRaceData->GetBaseModelFileName()));
	m_pGraphicThingInstance->SetModelInstance(0, 0, 0);
	m_pGraphicThingInstance->SetPosition(c_rPixelPosition.x, -c_rPixelPosition.y, c_rPixelPosition.z);
	
	// Load motions for the instance
	CRaceData::TMotionModeDataIterator itor;
	if (pRaceData->CreateMotionModeIterator(itor))
	{
		do
		{
			WORD wMotionMode = itor->first;
			CRaceData::TMotionModeData* pMotionModeData = itor->second;

			CRaceData::TMotionVectorMap::iterator itorMotion = pMotionModeData->MotionVectorMap.begin();
			for (; itorMotion != pMotionModeData->MotionVectorMap.end(); ++itorMotion)
			{
				WORD wMotionIndex = itorMotion->first;
				const CRaceData::TMotionVector& c_rMotionVector = itorMotion->second;
				CRaceData::TMotionVector::const_iterator it;
				DWORD i;
				for (i = 0, it = c_rMotionVector.begin(); it != c_rMotionVector.end(); ++i, ++it)
				{
					DWORD dwMotionKey = MAKE_RANDOM_MOTION_KEY(wMotionMode, wMotionIndex, i);
					m_pGraphicThingInstance->RegisterMotionThing(dwMotionKey, it->pMotion);
				}
			}
		} while (pRaceData->NextMotionModeIterator(itor));
	}
	
	// Get motion key for the wait animation as its the only one being used
	DWORD dwMotionKey;
	if (!pRaceData->GetMotionKey(CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_WAIT, &dwMotionKey))
	{
		TraceError("Cannot find wait motion key for race %d", dwVirtualNumber);
		delete(pInstance);
		m_pGraphicThingInstance = nullptr;

		return false;
	}

	// Load attaching effects
	for (DWORD i = 0; i < pRaceData->GetAttachingDataCount(); ++i)
	{
		const NRaceData::TAttachingData* c_pAttachingData;
		if (!pRaceData->GetAttachingDataPointer(i, &c_pAttachingData))
			continue;

		switch (c_pAttachingData->dwType)
		{
			case NRaceData::ATTACHING_DATA_TYPE_EFFECT:
			{
				if (c_pAttachingData->isAttaching)
				{
					AttachEffectByName(0, c_pAttachingData->strAttachingBoneName.c_str(), c_pAttachingData->pEffectData->strFileName.c_str());
				}
				else
				{
					AttachEffectByName(0, 0, c_pAttachingData->pEffectData->strFileName.c_str());
				}
			} break;
		}
	}

	m_pGraphicThingInstance->SetMotion(dwMotionKey);

	m_pGraphicThingInstance->Show();
	m_pGraphicThingInstance->Update();
	m_pGraphicThingInstance->Transform();
	m_pGraphicThingInstance->Deform();

	return true;
}

DWORD CPythonPrivateShop::SPrivateShopInstance::AttachEffectByName(DWORD dwParentPartIndex, const char* c_pszBoneName, const char* c_pszEffectName)
{
	std::string str;
	DWORD dwCRC;
	StringPath(c_pszEffectName, str);
	dwCRC = GetCaseCRC32(str.c_str(), str.length());

	return AttachEffectByID(dwParentPartIndex, c_pszBoneName, dwCRC);
}

DWORD CPythonPrivateShop::SPrivateShopInstance::AttachEffectByID(DWORD dwParentPartIndex, const char* c_pszBoneName, DWORD dwEffectID, const D3DXVECTOR3* c_pv3Position)
{
	TAttachingEffect ae;
	ae.iLifeType = EFFECT_LIFE_INFINITE;
	ae.dwEndTime = 0;
	ae.dwModelIndex = dwParentPartIndex;
	ae.dwEffectIndex = CEffectManager::Instance().GetEmptyIndex();
	ae.isAttaching = TRUE;
	if (c_pv3Position)
	{
		D3DXMatrixTranslation(&ae.matTranslation, c_pv3Position->x, c_pv3Position->y, c_pv3Position->z);
	}
	else
	{
		D3DXMatrixIdentity(&ae.matTranslation);
	}
	CEffectManager& rkEftMgr = CEffectManager::Instance();
	rkEftMgr.CreateEffectInstance(ae.dwEffectIndex, dwEffectID);

	if (c_pszBoneName)
	{
		int iBoneIndex;

		if (!GetGraphicThingInstancePtr()->FindBoneIndex(dwParentPartIndex, c_pszBoneName, &iBoneIndex))
		{
			ae.iBoneIndex = -1;
			TraceError("Could not find bone index for bone %s", c_pszBoneName);
		}
		else
		{
			ae.iBoneIndex = iBoneIndex;
		}
	}
	else
	{
		ae.iBoneIndex = -1;
	}

	m_list_attachingEffect.push_back(ae);

	return ae.dwEffectIndex;
}

void CPythonPrivateShop::SPrivateShopInstance::DetachEffect(DWORD dwEffectID)
{
	auto it = m_list_attachingEffect.begin();

	while (it != m_list_attachingEffect.end())
	{
		TAttachingEffect& rkAttEft = (*it);

		if (rkAttEft.dwEffectIndex == dwEffectID)
		{
			it = m_list_attachingEffect.erase(it);
			CEffectManager::Instance().DestroyEffectInstance(dwEffectID);
		}
		else
		{
			++it;
		}
	}
}

void CPythonPrivateShop::SPrivateShopInstance::AttachSelectEffect()
{
	m_dwCurrentSelectEffect = AttachEffectByName(0, nullptr, c_szEffectName);
}

void CPythonPrivateShop::SPrivateShopInstance::DetachSelectEffect()
{
	if (!m_dwCurrentSelectEffect)
		return;

	DetachEffect(m_dwCurrentSelectEffect);
	m_dwCurrentSelectEffect = 0;
}

void CPythonPrivateShop::SPrivateShopInstance::ShowAllAttachingEffect()
{
	for (const auto& rEffectInstance : m_list_attachingEffect)
	{
		CEffectManager::Instance().SelectEffectInstance(rEffectInstance.dwEffectIndex);
		CEffectManager::Instance().ShowEffect();
	}
}

void CPythonPrivateShop::SPrivateShopInstance::HideAllAttachingEffect()
{
	for (const auto& rEffectInstance : m_list_attachingEffect)
	{
		CEffectManager::Instance().SelectEffectInstance(rEffectInstance.dwEffectIndex);
		CEffectManager::Instance().HideEffect();
	}
}

void CPythonPrivateShop::SPrivateShopInstance::ClearAttachingEffect()
{
	for (const auto& rEffectInstance : m_list_attachingEffect)
	{
		CEffectManager::Instance().DestroyEffectInstance(rEffectInstance.dwEffectIndex);
	}

	m_list_attachingEffect.clear();
}

void CPythonPrivateShop::SPrivateShopInstance::UpdateAttachingEffect()
{
	CEffectManager& rkEftMgr = CEffectManager::Instance();
	DWORD dwCurrentTime = CTimer::Instance().GetCurrentMillisecond();

	for (const auto& rEffectInstance : m_list_attachingEffect)
	{
		if (rEffectInstance.isAttaching)
		{
			rkEftMgr.SelectEffectInstance(rEffectInstance.dwEffectIndex);

			if (rEffectInstance.iBoneIndex == -1)
			{
				D3DXMATRIX matTransform;
				matTransform = rEffectInstance.matTranslation;
				matTransform *= GetGraphicThingInstancePtr()->GetWorldMatrix();
				rkEftMgr.SetEffectInstanceGlobalMatrix(matTransform);
			}
			else
			{
				D3DXMATRIX* pBoneMat;
				if (GetGraphicThingInstancePtr()->GetBoneMatrix(rEffectInstance.dwModelIndex, rEffectInstance.iBoneIndex, &pBoneMat))
				{
					D3DXMATRIX matTransform;
					matTransform = *pBoneMat;
					matTransform *= rEffectInstance.matTranslation;
					matTransform *= GetGraphicThingInstancePtr()->GetWorldMatrix();
					rkEftMgr.SetEffectInstanceGlobalMatrix(matTransform);
				}
			}
		}
	}
}

bool CPythonPrivateShop::SPrivateShopInstance::IntersectBoundingBox()
{
	float fU, fV, fT;
	return GetGraphicThingInstancePtr()->Intersect(&fU, &fV, &fT);
}

CGraphicThingInstance& CPythonPrivateShop::SPrivateShopInstance::GetGraphicThingInstanceRef()
{
	return *m_pGraphicThingInstance;
}

CGraphicThingInstance* CPythonPrivateShop::SPrivateShopInstance::GetGraphicThingInstancePtr()
{
	return m_pGraphicThingInstance;
}

bool CPythonPrivateShop::LoadDecoration(const char* c_szFilePath)
{
	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFilePath, &pvData))
	{
		Tracenf("CPythonPrivateShop::LoadDecoration(c_szFileName=%s) - Load Error", c_szFilePath);
		return false;
	}

	m_vec_appearanceDeco.clear();
	m_vec_titleDeco.clear();

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		BYTE bDecoType = atoi(kTokenVector[0].c_str());

		if (bDecoType == DECO_TYPE_APPEARANCE)
		{
			if (kTokenVector.size() < APPEARANCE_MAX_TOKEN)
			{
				Tracef("CPythonPrivateShop::LoadDecoration(%s) - StrangeLine in %d\n", c_szFilePath, i);
				continue;
			}

			const std::string& c_rstName = kTokenVector[APPEARANCE_TOKEN_NAME];
			DWORD dwVnum = atoi(kTokenVector[APPEARANCE_TOKEN_VNUM].c_str());

			m_vec_appearanceDeco.push_back({ c_rstName, dwVnum });
		}
		else if (bDecoType == DECO_TYPE_TITLE)
		{
			if (kTokenVector.size() < TITLE_MAX_TOKEN)
			{
				Tracef("CPythonPrivateShop::LoadDecoration(%s) - StrangeLine in %d\n", c_szFilePath, i);
				continue;
			}

			const std::string& c_rstName = kTokenVector[TITLE_TOKEN_NAME];
			const std::string& c_rstPath = kTokenVector[TITLE_TOKEN_PATH];
			const std::string& c_rstTextColor = kTokenVector[TITLE_TOKEN_TEXTCOLOR];

			m_vec_titleDeco.push_back({ c_rstName, c_rstPath, c_rstTextColor });
		}
	}

	return true;
}

bool CPythonPrivateShop::GetAppearanceDeco(size_t index, const TAppearanceDeco** c_pAppearanceDeco)
{
	if (index >= m_vec_appearanceDeco.size())
		return false;

	*c_pAppearanceDeco = &m_vec_appearanceDeco.at(index);
	return true;
}

bool CPythonPrivateShop::GetTitleDeco(size_t index, const TTitleDeco** c_pTitleDeco)
{
	if (index >= m_vec_titleDeco.size())
		return false;

	*c_pTitleDeco = &m_vec_titleDeco.at(index);
	return true;
}
