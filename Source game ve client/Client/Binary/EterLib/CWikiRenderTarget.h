#pragma once

#include <cstdint>
#include <memory>
#include "GrpWikiRenderTargetTexture.h"

class CInstanceBase;
class CGraphicImageInstance;

class CWikiRenderTarget
{
	using TCharacterInstanceMap = std::map<DWORD, CInstanceBase*>;
	
	public:
		CWikiRenderTarget(DWORD width, DWORD height);
		virtual ~CWikiRenderTarget();
	
	public:
		void		SetVisibility(bool flag) { m_visible = flag; }
		
		void		CreateTextures() const;
		void		ReleaseTextures() const;
		void		RenderTexture() const;
		
		void		SetRenderingRect(RECT* rect) const;
		void		SetRenderingBox(RECT* renderBox) const;
		
		void		SelectModel(DWORD model_vnum);
		void		UpdateModel();
		void		DeformModel() const;
		void		RenderModel() const;
		
		void		UseSkillWithEffect(DWORD dwSkill, DWORD dwSetSkillEffect);

		void		SetWeapon(DWORD dwVnum);
		void		SetArmor(DWORD vnum);
		void		SetHair(DWORD vnum);
#ifdef ENABLE_COSTUME_AURA_SYSTEM
		void		SetAuraModel(DWORD vnum);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		void		SetSashModel(DWORD vnum);
#endif
#ifdef ENABLE_SHINING_ITEM_SYSTEM
		void		SetShiningModel(DWORD vnum);
#endif

		void		SetModelV3Eye(float x, float y, float z);
		void		SetModelV3Target(float x, float y, float z);
	
		bool		CreateBackground(const char* imgPath, DWORD width, DWORD height);
		void		RenderBackground() const;

	private:
		std::unique_ptr<CInstanceBase>						m_pModel;
		std::unique_ptr<CGraphicImageInstance>				m_background;
		std::unique_ptr<CGraphicWikiRenderTargetTexture>	m_renderTargetTexture;
		float												m_modelRotation;
		bool												m_visible;
		D3DXVECTOR3											m_v3Eye;
		D3DXVECTOR3											m_v3Target;
		D3DXVECTOR3											m_v3Up;
};
