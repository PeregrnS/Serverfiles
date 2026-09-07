#pragma once

#include <cstdint>
#include <memory>
#include "GrpRenderTargetTexture.h"

class CInstanceBase;
class CGraphicImageInstance;

class CRenderTarget
{
	using TCharacterInstanceMap = std::map<DWORD, CInstanceBase*>;

public:
	CRenderTarget(DWORD width, DWORD height);
	virtual ~CRenderTarget();

public:
	void		SetVisibility(bool flag) { m_visible = flag; }
	bool		IsReady() { return ((m_visible && m_pModel) || (m_visible)); }
	std::unique_ptr<CInstanceBase>& GetModel() { return m_pModel; }

	void		CreateTextures() const;
	void		ReleaseTextures() const;
#ifdef ENABLE_CLIP_MASK
	void		RenderTexture(RECT* pClipRect = NULL) const;
#else
	void		RenderTexture() const;
#endif

	void		SetRenderingRect(RECT* rect) const;
	void		UseSkillWithEffect(DWORD dwSkill, DWORD dwSetSkillEffect);

	void		SelectModel(DWORD model_vnum);
	bool		CreateBackground(const char* imgPath, DWORD width, DWORD height);
	void		RenderBackground() const;
	void		UpdateModel();
	void		DeformModel() const;
	void		RenderModel() const;

	void		SetWeapon(DWORD dwVnum);
	void		SetArmor(DWORD vnum);
	void		SetHair(DWORD vnum);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	void		SetAcce(DWORD vnum);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	void 		SetAura(DWORD vnum);
#endif
#ifdef ENABLE_SHINING_ITEM_SYSTEM
	void 		SetShining(DWORD vnum);
#endif

	void		SetEye(float x, float y, float z);
	void		SetTarget(float x, float y, float z);

// #ifdef ENABLE_QUEST_RENDER_EVENT
	void		ModelViewReset();
// #endif

#ifdef ENABLE_RENDER_TARGET_EX
	void		SetAutoRotate(bool value);
	void		MouseZoom(bool bZoom);
	void		MouseRotation(float value);
#endif

	void		SetMotion(WORD motion);

private:
	std::unique_ptr<CInstanceBase>						m_pModel;
	std::unique_ptr<CGraphicImageInstance>				m_background;
	std::unique_ptr<CGraphicRenderTargetTexture>		m_renderTargetTexture;
	float												m_modelRotation;
	bool												m_visible;
	D3DXVECTOR3											m_v3Eye;
	D3DXVECTOR3											m_v3Target;
	D3DXVECTOR3											m_v3Up;
#ifdef ENABLE_RENDER_TARGET_EX
	float												f_zoom;
	bool												m_autoRotate = true;
#endif
};
