#include "StdAfx.h"

#ifdef ENABLE_INGAME_WIKI
#include "../EterLib/Camera.h"
#include "../EterLib/CWikiRenderTargetManager.h"
#include "../EterPythonLib/PythonGraphic.h"
#include "../EterBase/CRC32.h"
#include "../GameLib/GameType.h"
#include "../GameLib/MapType.h"
#include "../GameLib/ItemData.h"
#ifdef ENABLE_SHINING_ITEM_SYSTEM
	#include "../GameLib/ItemManager.h"
#endif
#include "../GameLib/ActorInstance.h"
#include "../UserInterface/InstanceBase.h"

#include "ResourceManager.h"
#include "CWikiRenderTarget.h"

CWikiRenderTarget::~CWikiRenderTarget() {}
CWikiRenderTarget::CWikiRenderTarget(const DWORD width, const DWORD height) :
																		m_renderTargetTexture(nullptr),
																		m_pModel(nullptr),
																		m_background(nullptr),
																		m_modelRotation(0),
																		m_visible(false),
																		m_v3Eye(0.0f, 0.0f, 0.0f),
																		m_v3Target(0.0f, 0.0f, 0.0f),
																		m_v3Up(0.0f, 0.0f, 1.0f)
{
	auto pTex = new CGraphicWikiRenderTargetTexture;
	
	if (!pTex->Create(width, height, D3DFMT_X8R8G8B8, D3DFMT_D16)) {
		TraceError("CWikiRenderTarget::CWikiRenderTarget: Could not create CGraphicRenderTargetTexture %dx%d", width, height);
		delete pTex;
		
		throw std::runtime_error("CWikiRenderTarget::CWikiRenderTarget: Could not create CGraphicRenderTargetTexture");
	}
	
	m_renderTargetTexture = std::unique_ptr<CGraphicWikiRenderTargetTexture>(pTex);
}

/*----------------------------
--------PUBLIC CLASS FUNCTIONS
-----------------------------*/

void CWikiRenderTarget::CreateTextures() const
{
	if (!m_renderTargetTexture)
		return;
	
	m_renderTargetTexture->CreateTextures();
}

void CWikiRenderTarget::ReleaseTextures() const
{
	if (!m_renderTargetTexture)
		return;
	
	m_renderTargetTexture->ReleaseTextures();
}

void CWikiRenderTarget::RenderTexture() const
{
	if (!m_renderTargetTexture)
		return;
	
	m_renderTargetTexture->Render();
}

void CWikiRenderTarget::SetRenderingRect(RECT* rect) const
{
	if (!m_renderTargetTexture)
		return;
	
	m_renderTargetTexture->SetRenderingRect(rect);
}

void CWikiRenderTarget::SetRenderingBox(RECT* renderBox) const
{
	if (!m_renderTargetTexture)
		return;
	
	m_renderTargetTexture->SetRenderingBox(renderBox);
}

void CWikiRenderTarget::SelectModel(const DWORD model_vnum)
{
	CInstanceBase::SCreateData kCreateData{};
	
	kCreateData.m_bType = CActorInstance::TYPE_PC;
	kCreateData.m_dwRace = model_vnum;
	
	auto model = std::make_unique<CInstanceBase>();
	if (!model->Create(kCreateData))
	{
		if (m_pModel)
			m_pModel.reset();
		return;
	}
	
	m_pModel = std::move(model);
	m_pModel->NEW_SetPixelPosition(TPixelPosition(0,0,0));
	m_pModel->GetGraphicThingInstancePtr()->ClearAttachingEffect();
	m_pModel->Refresh(CRaceMotionData::NAME_WAIT, true);
	m_pModel->SetLoopMotion(CRaceMotionData::NAME_WAIT);
	m_pModel->SetRotation(0.0f);
	
	float fTargetHeight = 110.0f;
	
	m_v3Eye.x = 0.0f;
	m_v3Eye.y = -1000.0f;
	m_v3Eye.z = 600.0f;
	
	m_v3Target.x = 0.0f;
	m_v3Target.y = 0.0f;
	m_v3Target.z = 95.0f;
	
	if (model_vnum > (NRaceData::JOB_MAX_NUM * 2))
	{
		auto ptrGraphicInstance = m_pModel->GetGraphicThingInstancePtr();
		if (ptrGraphicInstance) {
			float fRaceHeight = ptrGraphicInstance->GetHeight();
			fTargetHeight = fRaceHeight / 2.0f;
			
			m_v3Eye.y = -(fRaceHeight * 8.9f);
			m_v3Eye.z = 0.0f;
			
			m_v3Target.z = fRaceHeight/2.0f;
		}
	}
	
	auto& camera_manager = CCameraManager::instance();
	camera_manager.SetCurrentCamera(CCameraManager::INGAME_WIKI_CAMERA);
	camera_manager.GetCurrentCamera()->SetTargetHeight(fTargetHeight);
	camera_manager.ResetToPreviousCamera();
	
	m_modelRotation = 0.0f;
}

void CWikiRenderTarget::UpdateModel()
{
	if (!m_visible || !m_pModel)
		return;
	
	if (m_modelRotation < 360.0f)
		m_modelRotation += 1.0f;
	else
		m_modelRotation = 0.0f;
	
	m_pModel->SetRotation(m_modelRotation);
	m_pModel->Transform();
	m_pModel->GetGraphicThingInstanceRef().RotationProcess();
}

void CWikiRenderTarget::DeformModel() const
{
	if (!m_visible)
		return;

	if (m_pModel)
		m_pModel->Deform();
}

void CWikiRenderTarget::RenderModel() const
{
	if (!m_visible)
		return;
	
	auto& python_graphic = CPythonGraphic::Instance();
	auto& camera_manager = CCameraManager::instance();
	auto& state_manager = CStateManager::Instance();
	auto& rectRender = *m_renderTargetTexture->GetRenderingRect();
	
	if (!m_pModel)
		return;
	
	m_renderTargetTexture->SetRenderTarget();
	if (!m_background) {
		m_renderTargetTexture->Clear();
	}

	python_graphic.ClearDepthBuffer();
	
	const auto fov = python_graphic.GetFOV();
	const auto aspect = python_graphic.GetAspect();
	const auto near_y = python_graphic.GetNear();
	const auto far_y = python_graphic.GetFar();
	
	const auto width = static_cast<float>(rectRender.right - rectRender.left);
	const auto height = static_cast<float>(rectRender.bottom - rectRender.top);
	
	state_manager.SetRenderState(D3DRS_FOGENABLE, FALSE);
	
	python_graphic.SetViewport(0.0f, 0.0f, width, height);
	python_graphic.PushState();
	
	camera_manager.SetCurrentCamera(CCameraManager::INGAME_WIKI_CAMERA);
	if (!camera_manager.GetCurrentCamera()->IsDraging())
		camera_manager.GetCurrentCamera()->SetViewParams(m_v3Eye, m_v3Target, m_v3Up);
	
	python_graphic.UpdateViewMatrix();
	python_graphic.SetPerspective(10.0f, width / height, 100.0f, 15000.0f);
	
	m_pModel->Render();
	m_pModel->GetGraphicThingInstanceRef().WikiRenderAllAttachingModuleEffect();
	
	camera_manager.ResetToPreviousCamera();
	
	python_graphic.RestoreViewport();
	python_graphic.PopState();
	python_graphic.SetPerspective(fov, aspect, near_y, far_y);
	
	m_renderTargetTexture->ResetRenderTarget();
	
	state_manager.SetRenderState(D3DRS_FOGENABLE, TRUE);
}

void CWikiRenderTarget::SetArmor(DWORD vnum)
{
	if (!m_pModel)
		return;
	
	m_pModel->ChangeArmor(vnum);
}

#include "../UserInterface/PythonSkill.h"
void CWikiRenderTarget::UseSkillWithEffect(DWORD dwSkill, DWORD dwSetSkillEffect)
{
	if (!m_pModel)
		return;

	for (int i = 0; i < ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT; ++i)
	{
		m_pModel->m_bSkillMotion[i] = dwSetSkillEffect;
	}

	CPythonSkill::TSkillData* pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(dwSkill, &pSkillData))
	{
		Tracenf("CPythonPlayer::__UseSkill(dwSlotIndex=%d) There is no skill data", dwSkill);
		return;
	}

	DWORD dwMotionIndex = pSkillData->GetSkillMotionIndex(3);
	if (!m_pModel->NEW_UseSkill(dwSkill, dwMotionIndex, 1, false))
	{
		Tracenf("CPythonPlayer::__UseSkill(%d) - m_pModel->NEW_UseSkill - ERROR", dwSkill);
		return;
	}
}

void CWikiRenderTarget::SetWeapon(DWORD vnum)
{
	if (!m_pModel)
		return;
	
	m_pModel->ChangeWeapon(vnum);
}

void CWikiRenderTarget::SetHair(DWORD vnum)
{
	if (!m_pModel)
		return;
	
	m_pModel->ChangeHair(vnum);
}

#ifdef ENABLE_COSTUME_AURA_SYSTEM
void CWikiRenderTarget::SetAuraModel(DWORD vnum)
{
	if (!m_pModel)
		return;

	m_pModel->ChangeAura(vnum);
}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CWikiRenderTarget::SetSashModel(DWORD vnum)
{
	if (!m_pModel)
		return;

	m_pModel->ChangeAcce(vnum);
}
#endif

#ifdef ENABLE_SHINING_ITEM_SYSTEM
void CWikiRenderTarget::SetShiningModel(DWORD vnum)
{
	if (!m_pModel)
	{
		return;
	}

	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(vnum, &pItemData))
	{
		return;
	}

	BYTE num;
	DWORD itemSubType = pItemData->GetSubType();
	if ((CItemData::SHINING_WEAPON == itemSubType))
	{
		num = 0/*CHR_SHINING_NUM - CHR_SHINING_WEAPON_1*/;
	}
	else if ((CItemData::SHINING_ARMOR == itemSubType))
	{
		num = 3/*CHR_SHINING_NUM - CHR_SHINING_ARMOR_1*/;
	}
	else
	{
		num = 5/*CHR_SHINING_NUM - CHR_SHINING_SPECIAL*/;
	}

	m_pModel->SetShining(num, vnum);
}
#endif

void CWikiRenderTarget::SetModelV3Eye(float x, float y, float z)
{
	m_v3Eye.x = x;
	m_v3Eye.y = y;
	m_v3Eye.z = z;
}

void CWikiRenderTarget::SetModelV3Target(float x, float y, float z)
{
	m_v3Target.x = x;
	m_v3Target.y = y;
	m_v3Target.z = z;
}

bool CWikiRenderTarget::CreateBackground(const char* imgPath, const DWORD width, const DWORD height)
{
	if (m_background)
		return false;

	m_background = std::make_unique<CGraphicImageInstance>();
	
	const auto graphic_image = dynamic_cast<CGraphicImage*>(CResourceManager::instance().GetResourcePointer(imgPath));
	if (!graphic_image)
	{
		m_background.reset();
		return false;
	}

	m_background->SetImagePointer(graphic_image);
	m_background->SetScale(static_cast<float>(width) / graphic_image->GetWidth(), static_cast<float>(height) / graphic_image->GetHeight());
	return true;
}

void CWikiRenderTarget::RenderBackground() const
{
	if (!m_visible)
		return;

	if (!m_background)
		return;

	auto& rectRender = *m_renderTargetTexture->GetRenderingRect();
	m_renderTargetTexture->SetRenderTarget();

	CGraphicWikiRenderTargetTexture::Clear();
	CPythonGraphic::Instance().SetInterfaceRenderState();

	m_background->Render();
	m_renderTargetTexture->ResetRenderTarget();
}

#endif
