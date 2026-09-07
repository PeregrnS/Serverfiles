#include "StdAfx.h"

#ifdef ENABLE_RENDER_TARGET
#include "CRenderTarget.h"
#include "ResourceManager.h"
#include "../EterBase/CRC32.h"
#include "../EterLib/Camera.h"
#include "../EterLib/CRenderTargetManager.h"
#include "../EterPythonLib/PythonGraphic.h"
#include "../GameLib/GameType.h"
#include "../GameLib/MapType.h"
#include "../GameLib/ItemData.h"
#ifdef ENABLE_SHINING_ITEM_SYSTEM
	#include "../GameLib/ItemManager.h"
#endif
#include "../GameLib/ActorInstance.h"
#include "../UserInterface/InstanceBase.h"
#include "../UserInterface/PythonBackground.h"

CRenderTarget::~CRenderTarget() {}
CRenderTarget::CRenderTarget(const DWORD width, const DWORD height) :
	m_renderTargetTexture(nullptr),
	m_background(nullptr),
	m_pModel(nullptr),
	m_modelRotation(0),
	m_visible(false),
	m_v3Eye(0.0f, 0.0f, 0.0f),
	m_v3Target(0.0f, 0.0f, 0.0f),
	m_v3Up(0.0f, 0.0f, 1.0f),
	f_zoom(0.0f)
{
	CGraphicRenderTargetTexture* pTex = new CGraphicRenderTargetTexture;

	if (!pTex->Create(width, height, D3DFMT_X8R8G8B8, D3DFMT_D16))
	{
		TraceError("CRenderTarget::CRenderTarget: Could not create CGraphicRenderTargetTexture %dx%d", width, height);
		delete pTex;

		throw std::runtime_error("CRenderTarget::CRenderTarget: Could not create CGraphicRenderTargetTexture");
	}

	m_renderTargetTexture = std::unique_ptr<CGraphicRenderTargetTexture>(pTex);
}

void CRenderTarget::CreateTextures() const
{
	if (!m_renderTargetTexture)
	{
		return;
	}

	m_renderTargetTexture->CreateTextures();
}

void CRenderTarget::ReleaseTextures() const
{
	if (!m_renderTargetTexture)
	{
		return;
	}

	m_renderTargetTexture->ReleaseTextures();
}

#ifdef ENABLE_CLIP_MASK
void CRenderTarget::RenderTexture(RECT* pClipRect) const
#else
void CRenderTarget::RenderTexture() const
#endif
{
	if (!m_renderTargetTexture)
	{
		return;
	}

#ifdef ENABLE_CLIP_MASK
	m_renderTargetTexture->Render(pClipRect);
#else
	m_renderTargetTexture->Render();
#endif
}

void CRenderTarget::SetRenderingRect(RECT* rect) const
{
	if (!m_renderTargetTexture)
	{
		return;
	}

	m_renderTargetTexture->SetRenderingRect(rect);
}

#include "../UserInterface/PythonSkill.h"
void CRenderTarget::UseSkillWithEffect(DWORD dwSkill, DWORD dwSetSkillEffect)
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

void CRenderTarget::SelectModel(const DWORD model_vnum)
{
	if (model_vnum == -1)
	{
		m_pModel.reset();
		return;
	}

	CInstanceBase::SCreateData kCreateData{};

	kCreateData.m_bType = model_vnum > 8 ? CActorInstance::TYPE_NPC : CActorInstance::TYPE_PC;
	kCreateData.m_dwRace = model_vnum;

	auto model = std::make_unique<CInstanceBase>();
	if (!model->Create(kCreateData))
	{
		if (m_pModel)
		{
			m_pModel.reset();
		}
		return;
	}

	m_pModel = std::move(model);

	m_pModel->GetGraphicThingInstancePtr()->ClearAttachingEffect();
	m_pModel->GetGraphicThingInstancePtr()->SetSkipLod(true);	// Skip lod files
	m_pModel->Refresh(CRaceMotionData::NAME_WAIT, true);
	m_pModel->SetLoopMotion(CRaceMotionData::NAME_WAIT);
	m_pModel->EnableAlwaysRender(); // TODO : remove & test
	m_pModel->SetRotation(0.0f);
	m_pModel->NEW_SetPixelPosition(TPixelPosition(0, 0, 0));

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

		if (ptrGraphicInstance)
		{
			float fRaceHeight = ptrGraphicInstance->GetHeight();
			fTargetHeight = fRaceHeight / 2.0f;

			m_v3Eye.y = -(fRaceHeight * 8.9f);
			m_v3Eye.z = 0.0f;

			m_v3Target.z = fRaceHeight / 2.0f;
		}
	}

	// // Get Eye Position
	// TraceError("CRenderTarget::SelectModel: Eye Position : %f, %f, %f", m_v3Eye.x, m_v3Eye.y, m_v3Eye.z);
	// // Get Target Position
	// TraceError("CRenderTarget::SelectModel: Target Position : %f, %f, %f", m_v3Target.x, m_v3Target.y, m_v3Target.z);

	auto& camera_manager = CCameraManager::instance();
	camera_manager.SetCurrentCamera(CCameraManager::DEFAULT_SHOPDECO_CAMERA);
	camera_manager.GetCurrentCamera()->SetTargetHeight(fTargetHeight);
	camera_manager.ResetToPreviousCamera();

	m_modelRotation = 0.0f;
}

bool CRenderTarget::CreateBackground(const char* imgPath, const DWORD width, const DWORD height)
{
	if (m_background)
	{
		return false;
	}

	m_background = std::make_unique<CGraphicImageInstance>();

	CGraphicImage* const graphic_image = dynamic_cast<CGraphicImage*>(CResourceManager::instance().GetResourcePointer(imgPath));
	if (!graphic_image)
	{
		m_background.reset();
		return false;
	}

	m_background->SetImagePointer(graphic_image);
	m_background->SetScale(static_cast<float>(width) / graphic_image->GetWidth(), static_cast<float>(height) / graphic_image->GetHeight());
	return true;
}


void CRenderTarget::RenderBackground() const
{
	if (!m_visible)
	{
		return;
	}

	if (!m_background)
	{
		return;
	}

	// RECT rectRender = *m_renderTargetTexture->GetRenderingRect();
	m_renderTargetTexture->SetRenderTarget();

	CGraphicRenderTargetTexture::Clear();
	CPythonGraphic::Instance().SetInterfaceRenderState();

	m_background->Render();

	m_renderTargetTexture->ResetRenderTarget();
}

void CRenderTarget::UpdateModel()
{
	if (!m_visible || !m_pModel)
	{
		return;
	}

#ifdef ENABLE_RENDER_TARGET_EX
	if (m_autoRotate)
	{
		if (m_modelRotation < 360.0f)
		{
			m_modelRotation += 1.0f;
		}
		else
		{
			m_modelRotation = 0.0f;
		}
	}
#else
	if (m_modelRotation < 360.0f)
	{
		m_modelRotation += 1.0f;
	}
	else
	{
		m_modelRotation = 0.0f;
	}
#endif

	m_pModel->SetRotation(m_modelRotation);
	m_pModel->Transform();
	m_pModel->GetGraphicThingInstanceRef().RotationProcess();
}

void CRenderTarget::DeformModel() const
{
	if (!m_visible)
	{
		return;
	}

	if (m_pModel)
	{
		m_pModel->Deform();
	}
}

void CRenderTarget::RenderModel() const
{
	if (!m_visible)
	{
		return;
	}

	auto& python_graphic = CPythonGraphic::Instance();
	auto& camera_manager = CCameraManager::instance();
	auto& state_manager = CStateManager::Instance();
	auto& rectRender = *m_renderTargetTexture->GetRenderingRect();

	if (!m_pModel)
	{
		return;
	}

	m_renderTargetTexture->SetRenderTarget();
	if (!m_background)
	{
		m_renderTargetTexture->Clear();
	}

	python_graphic.ClearDepthBuffer();

	const float fov = python_graphic.GetFOV();
	const float aspect = python_graphic.GetAspect();
	const float near_y = python_graphic.GetNear();
	const float far_y = python_graphic.GetFar();

	const float width = static_cast<float>(rectRender.right - rectRender.left);
	const float height = static_cast<float>(rectRender.bottom - rectRender.top);

#ifdef ENABLE_FOG_FIX
	CPythonBackground& rkBG = CPythonBackground::Instance();
	BOOL bIsFog = rkBG.GetFogMode();
#else
	BOOL bIsFog = FALSE;
#endif

	python_graphic.SetViewport(0.0f, 0.0f, width, height);
	python_graphic.PushState();

	state_manager.SetRenderState(D3DRS_FOGENABLE, 0);
	camera_manager.SetCurrentCamera(CCameraManager::DEFAULT_SHOPDECO_CAMERA);
	if (!camera_manager.GetCurrentCamera()->IsDraging())
	{
		camera_manager.GetCurrentCamera()->SetViewParams(m_v3Eye, m_v3Target, m_v3Up);
	}

	python_graphic.UpdateViewMatrix();
	python_graphic.SetPerspective(10.0f + f_zoom, width / height, 100.0f, 15000.0f);

	m_pModel->Update();
	m_pModel->Render();
	m_pModel->GetGraphicThingInstanceRef().RenderAllAttachingEffect();

	camera_manager.ResetToPreviousCamera();
	python_graphic.RestoreViewport();
	python_graphic.PopState();
	python_graphic.SetPerspective(fov, aspect, near_y, far_y);
	m_renderTargetTexture->ResetRenderTarget();
	state_manager.SetRenderState(D3DRS_FOGENABLE, bIsFog);
}

void CRenderTarget::SetArmor(DWORD vnum)
{
	if (!m_pModel)
	{
		return;
	}

	m_pModel->ChangeArmor(vnum);
}

void CRenderTarget::SetWeapon(DWORD vnum)
{
	if (!m_pModel)
	{
		return;
	}

	m_pModel->ChangeWeapon(vnum);
}

void CRenderTarget::SetHair(DWORD vnum)
{
	if (!m_pModel)
	{
		return;
	}

	m_pModel->ChangeHair(vnum);
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CRenderTarget::SetAcce(DWORD vnum)
{
	if (!m_pModel)
	{
		return;
	}

	m_pModel->ChangeAcce(vnum);
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
void CRenderTarget::SetAura(DWORD vnum)
{
	if (!m_pModel)
	{
		return;
	}

	m_pModel->ChangeAura(vnum);
}
#endif

#ifdef ENABLE_SHINING_ITEM_SYSTEM
void CRenderTarget::SetShining(DWORD vnum)
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

void CRenderTarget::SetEye(float x, float y, float z)
{
	m_v3Eye.x = x;
	m_v3Eye.y = y;
	m_v3Eye.z = z;
}

void CRenderTarget::SetTarget(float x, float y, float z)
{
	m_v3Target.x = x;
	m_v3Target.y = y;
	m_v3Target.z = z;
}

// #ifdef ENABLE_QUEST_RENDER_EVENT
void CRenderTarget::ModelViewReset()
{
	if (!m_pModel)
	{
		return;
	}

	f_zoom = 0.0f;
	m_pModel->SetRotation(0.0f);
	m_pModel->NEW_SetPixelPosition(TPixelPosition(0.0f, 0.0f, 0.0f));
}
// #endif

#ifdef ENABLE_RENDER_TARGET_EX
void CRenderTarget::SetAutoRotate(bool value)
{
	m_autoRotate = value;
}

void CRenderTarget::MouseZoom(bool bZoom)
{
	if (!m_visible || !m_pModel)
	{
		return;
	}

	if (f_zoom == 5.0f && !bZoom)
	{
		return;
	}

	if (f_zoom == -5.0f && bZoom)
	{
		return;
	}

	if (!bZoom)
	{
		f_zoom += 1.0f;
	}
	else
	{
		f_zoom -= 1.0f;
	}
}

void CRenderTarget::MouseRotation(float value)
{
	m_modelRotation += value;
}
#endif

void CRenderTarget::SetMotion(WORD motion)
{
	if (!m_visible || !m_pModel)
	{
		return;
	}

	m_pModel->GetGraphicThingInstancePtr()->AttachEffectByID(0, NULL, 568632903);
	m_pModel->Refresh(53, true);
	m_pModel->SetLoopMotion(53);
}

#endif	// ENABLE_RENDER_TARGET

