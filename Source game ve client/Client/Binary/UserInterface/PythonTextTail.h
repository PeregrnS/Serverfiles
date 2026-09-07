#pragma once

#include "../eterBase/Singleton.h"

class CPythonTextTail : public CSingleton<CPythonTextTail>
{
	public:
		typedef struct STextTail
		{
			CGraphicTextInstance*			pTextInstance;
			CGraphicTextInstance*			pOwnerTextInstance;

			CGraphicMarkInstance*			pMarkInstance;
			CGraphicTextInstance*			pGuildNameTextInstance;

			CGraphicTextInstance*			pTitleTextInstance;
			CGraphicTextInstance*			pLevelTextInstance;

#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
			CGraphicExpandedImageInstance*	pCountryFlagImageInstance{};
#endif
			CGraphicTextInstance* pSpecialTitleInstance;

			CGraphicObjectInstance *		pOwner;

			DWORD							dwVirtualID;

			float							x, y, z;
			float							fDistanceFromPlayer;
			D3DXCOLOR						Color;
			BOOL							bNameFlag;

			float							xStart, yStart;
			float							xEnd, yEnd;

			DWORD							LivingTime;

			float							fHeight;

			STextTail() {}
			virtual ~STextTail() {}
		} TTextTail;

		typedef std::map<DWORD, TTextTail*>		TTextTailMap;
		typedef std::list<TTextTail*>			TTextTailList;
		typedef TTextTailMap					TChatTailMap;

	public:
		CPythonTextTail(void);
		virtual ~CPythonTextTail(void);

		void GetInfo(std::string* pstInfo);

		void Initialize();
		void Destroy();
		void Clear();

		void UpdateAllTextTail();
		void UpdateShowingTextTail();
		void Render();

		void ArrangeTextTail();
		void HideAllTextTail();
		void HideItemTextTail();
		void ShowAllTextTail();
		void ShowCharacterTextTail(DWORD VirtualID);
		void ShowItemTextTail(DWORD VirtualID);

		void RegisterCharacterTextTail(DWORD dwGuildID, DWORD dwVirtualID, const D3DXCOLOR & c_rColor, float fAddHeight=10.0f);
		void RegisterItemTextTail(DWORD VirtualID, const char * c_szText, CGraphicObjectInstance * pOwner);
		void RegisterChatTail(DWORD VirtualID, const char * c_szChat);
		void RegisterInfoTail(DWORD VirtualID, const char * c_szChat);
		void SetCharacterTextTailColor(DWORD VirtualID, const D3DXCOLOR & c_rColor);
#ifdef ENABLE_OWNERSHIP_DURATION_TIMER
		void SetItemTextTailOwner(DWORD dwVID, const char* c_szName, int iSec, bool bUpdate);
#else
		void SetItemTextTailOwner(DWORD dwVID, const char * c_szName);
#endif
		void DeleteCharacterTextTail(DWORD VirtualID);
		void DeleteItemTextTail(DWORD VirtualID);

		int Pick(int ixMouse, int iyMouse);
		void SelectItemName(DWORD dwVirtualID);

		bool GetTextTailPosition(DWORD dwVID, float* px, float* py, float* pz);
		bool IsChatTextTail(DWORD dwVID);

		void EnablePKTitle(BOOL bFlag);
		void AttachTitle(DWORD dwVID, const char * c_szName, const D3DXCOLOR& c_rColor);
		void DetachTitle(DWORD dwVID);

		void AttachLevel(DWORD dwVID, const char* c_szText, const D3DXCOLOR& c_rColor);
		void DetachLevel(DWORD dwVID);

#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
		void AttachFlag(DWORD dwVID, const std::string& countryName);
		void DetachFlag(DWORD dwVID);
#endif

		void AttachSpecialTitle(DWORD dwVID, const char* c_szName, const DWORD dwColor);
		void DetachSpecialTitle(DWORD dwVID);

	protected:
		TTextTail * RegisterTextTail(DWORD dwVirtualID, const char * c_szText, CGraphicObjectInstance * pOwner, float fHeight, const D3DXCOLOR & c_rColor);
		void DeleteTextTail(TTextTail * pTextTail);

		void UpdateTextTail(TTextTail * pTextTail);
		void RenderTextTailBox(TTextTail * pTextTail);
		void RenderTextTailName(TTextTail * pTextTail);
		void UpdateDistance(const TPixelPosition & c_rCenterPosition, TTextTail * pTextTail);

		bool isIn(TTextTail * pSource, TTextTail * pTarget);

	protected:
		TTextTailMap				m_CharacterTextTailMap;
		TTextTailMap				m_ItemTextTailMap;
		TChatTailMap				m_ChatTailMap;

		TTextTailList				m_CharacterTextTailList;
		TTextTailList				m_ItemTextTailList;

	private:
		CDynamicPool<STextTail>		m_TextTailPool;

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	public:
		void ShowPrivateShopTextTail(DWORD dwVirtualID);
		void RegisterPrivateShopTextTail(DWORD dwVirtualID);
		void DeletePrivateShopTextTail(DWORD dwVirtualID);

	protected:
		TTextTailMap				m_PrivateShopTextTailMap;
		TTextTailList				m_PrivateShopTextTailList;
#endif
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
