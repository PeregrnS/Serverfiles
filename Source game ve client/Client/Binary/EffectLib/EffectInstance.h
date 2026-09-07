#pragma once

#include "../eterlib/GrpObjectInstance.h"
#include "../eterlib/Pool.h"
#include "../mileslib/Type.h"

#include "EffectElementBaseInstance.h"
#include "EffectData.h"
#include "EffectMeshInstance.h"
#include "ParticleSystemInstance.h"
#include "SimpleLightInstance.h"

class CEffectInstance : public CGraphicObjectInstance
{
	public:
 		typedef std::vector<CEffectElementBaseInstance*> TEffectElementInstanceVector;

		enum
		{
			ID = EFFECT_OBJECT
		};
		int GetType() const
		{
			return CEffectInstance::ID;
		}
#ifdef ENABLE_MAP_OBJECT_OPTIMIZATION
		const D3DXMATRIX& GetGlobalMatrix() const { return m_matGlobal; };
#endif

		bool GetBoundingSphere(D3DXVECTOR3 & v3Center, float & fRadius);

		static void DestroySystem();

		static CEffectInstance* New();
		static void Delete(CEffectInstance* pkEftInst);

		static void ResetRenderingEffectCount();
		static int GetRenderingEffectCount();

	public:
		CEffectInstance();
		virtual ~CEffectInstance();

		bool LessRenderOrder(CEffectInstance* pkEftInst);

		void SetEffectDataPointer(CEffectData* pEffectData
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		, DWORD * dwSkillColor = NULL, DWORD EffectID = 0
#endif
		);

		void Clear();
		BOOL isAlive();
		void SetActive();
		void SetDeactive();
		void SetGlobalMatrix(const D3DXMATRIX & c_rmatGlobal);
		void UpdateSound();
		void OnUpdate();
		void OnRender();
		void OnBlendRender() {} // Not used
		void OnRenderToShadowMap() {} // Not used
		void OnRenderShadow() {} // Not used
		void OnRenderPCBlocker() {} // Not used

	protected:
		void					__Initialize();

		void					__SetParticleData(CParticleSystemData * pData);
		void					__SetMeshData(CEffectMeshScript * pMesh);
		void					__SetLightData(CLightData * pData);

		virtual void			OnUpdateCollisionData(const CStaticCollisionDataVector * pscdVector) {} // Not used
		virtual void			OnUpdateHeighInstance(CAttributeInstance * pAttributeInstance) {}
		virtual bool			OnGetObjectHeight(float fX, float fY, float * pfHeight) { return false; }

	protected:
		BOOL					m_isAlive;
		DWORD					m_dwFrame;
		D3DXMATRIX				m_matGlobal;

		CEffectData * m_pkEftData;

 		std::vector<CParticleSystemInstance*>	m_ParticleInstanceVector;
		std::vector<CEffectMeshInstance*>		m_MeshInstanceVector;
		std::vector<CLightInstance*>			m_LightInstanceVector;

		NSound::TSoundInstanceVector *	m_pSoundInstanceVector;

		float m_fBoundingSphereRadius;
		D3DXVECTOR3 m_v3BoundingSpherePosition;

		float m_fLastTime;

	public:
		static CDynamicPool<CEffectInstance>	ms_kPool;
		static int ms_iRenderingEffectCount;


#ifdef ENABLE_RENDER_TARGET
	public:
		void SetIgnoreFrustum(bool frustum)
		{
			m_ignoreFrustum = frustum;
			FEffectFrustum f2(m_ignoreFrustum);
			std::for_each(m_ParticleInstanceVector.begin(), m_ParticleInstanceVector.end(), f2);
			std::for_each(m_MeshInstanceVector.begin(), m_MeshInstanceVector.end(), f2);
			std::for_each(m_LightInstanceVector.begin(), m_LightInstanceVector.end(), f2);
		}

	protected:
		bool m_ignoreFrustum;

		struct FEffectFrustum
		{
			bool ignoreFrustum;
			FEffectFrustum(bool igno)
				: ignoreFrustum(igno)
			{
			}
			void operator () (CEffectElementBaseInstance* pInstance)
			{
				pInstance->SetIgnoreFrustum(ignoreFrustum);
			}
		};
#endif

#ifdef ENABLE_INGAME_WIKI
	public:
		void SetWikiIgnoreFrustum(bool flag)
		{
			m_wikiIgnoreFrustum = flag;
			FWikiEffectFrustum f2(m_wikiIgnoreFrustum);
			std::for_each(m_ParticleInstanceVector.begin(), m_ParticleInstanceVector.end(), f2);
			std::for_each(m_MeshInstanceVector.begin(), m_MeshInstanceVector.end(), f2);
			std::for_each(m_LightInstanceVector.begin(), m_LightInstanceVector.end(), f2);
		}

	protected:
		struct FWikiEffectFrustum
		{
			FWikiEffectFrustum(bool igno) : wikiIgnoreFrustum(igno) {}
			void operator () (CEffectElementBaseInstance* pInstance)
			{
				pInstance->SetWikiIgnoreFrustum(wikiIgnoreFrustum);
			}
			bool wikiIgnoreFrustum;
		};
		bool m_wikiIgnoreFrustum;
#endif

#if defined(__BL_GRAPHIC_ON_OFF__)
	public:
		void			SetID(DWORD dwID);
		DWORD			GetID() const;
		void			SetRenderState(bool bCanRender);
		bool			CanRender() const;

	protected:
		DWORD			m_dwID;
		bool			m_bCanRender;
#endif
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
