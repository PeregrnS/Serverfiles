#pragma once

#include "PythonWindow.h"
#include "../UserInterface/Locale_inc.h"

namespace UI
{
	enum
	{
		ITEM_WIDTH = 32,
		ITEM_HEIGHT = 32,

		SLOT_NUMBER_NONE = 0xffffffff,
	};

	enum ESlotStyle
	{
		SLOT_STYLE_NONE,
		SLOT_STYLE_PICK_UP,
		SLOT_STYLE_SELECT,
	};

	enum ESlotState
	{
		SLOT_STATE_LOCK		= (1 << 0),
		SLOT_STATE_CANT_USE	= (1 << 1),
		SLOT_STATE_DISABLE	= (1 << 2),
		SLOT_STATE_ALWAYS_RENDER_COVER = (1 << 3),
#ifdef WJ_ENABLE_TRADABLE_ICON
		SLOT_STATE_CANT_MOUSE_EVENT = (1 << 4),
		SLOT_STATE_UNUSABLE = (1 << 5),
#endif
#ifdef ENABLE_ITEM_MANAGER
		SLOT_STATE_UNUSABLE_EX = (1 << 6),
#endif
	};

#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM_EX
	enum ESlotDiffuseColorType
	{
		COLOR_TYPE_ORANGE,
		COLOR_TYPE_WHITE,
		COLOR_TYPE_RED,
		COLOR_TYPE_GREEN,
		COLOR_TYPE_YELLOW,
		COLOR_TYPE_SKY,
		COLOR_TYPE_PINK,
		COLOR_TYPE_VIOLET,
		COLOR_TYPE_MAX,
	};
#endif

	class CSlotWindow : public CWindow
	{
		public:
			static DWORD Type();

		public:
			class CSlotButton;
			class CCoverButton;
			class CCoolTimeFinishEffect;

			friend class CSlotButton;
			friend class CCoverButton;

			typedef struct SSlot
			{
				DWORD	dwState;
				DWORD	dwSlotNumber;
				DWORD	dwCenterSlotNumber;
				DWORD	dwItemIndex;
				BOOL	isItem;

				// CoolTime
				float	fCoolTime;
				float	fStartCoolTime;

				// Toggle
				BOOL	bActive;
#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM_EX
				BYTE	byToggleColorType;
#endif
#ifdef ENABLE_HIGHLIGHT_SLOT_NEW_EFFECT
				BOOL	bActiveNewEffect;
#endif

				int		ixPosition;
				int		iyPosition;

				int		ixCellSize;
				int		iyCellSize;

				BYTE	byxPlacedItemSize;
				BYTE	byyPlacedItemSize;

				CGraphicImageInstance * pInstance;
				CNumberLine * pNumberLine;

				bool	bRenderBaseSlotImage;
				CCoverButton * pCoverButton;
				CSlotButton * pSlotButton;
				CImageBox * pSignImage;
				CAniImageBox * pFinishCoolTimeEffect;
			} TSlot;
			typedef std::list<TSlot> TSlotList;
			typedef TSlotList::iterator TSlotListIterator;

		public:
			CSlotWindow(PyObject * ppyObject);
			virtual ~CSlotWindow();

			void Destroy();

			// Manage Slot
			void SetSlotType(DWORD dwType);
			void SetSlotStyle(DWORD dwStyle);
			void SetSlotScale(float fx, float fy);

			void AppendSlot(DWORD dwIndex, int ixPosition, int iyPosition, int ixCellSize, int iyCellSize);
			void SetCoverButton(DWORD dwIndex, const char * c_szUpImageName, const char * c_szOverImageName, const char * c_szDownImageName, const char * c_szDisableImageName, BOOL bLeftButtonEnable, BOOL bRightButtonEnable);
			void SetSlotBaseImage(const char * c_szFileName, float fr, float fg, float fb, float fa);
			void AppendSlotButton(const char * c_szUpImageName, const char * c_szOverImageName, const char * c_szDownImageName);
			void AppendRequirementSignImage(const char * c_szImageName);

			void EnableCoverButton(DWORD dwIndex);
			void DisableCoverButton(DWORD dwIndex);
			void SetAlwaysRenderCoverButton(DWORD dwIndex, bool bAlwaysRender = false);

			void ShowSlotBaseImage(DWORD dwIndex);
			void HideSlotBaseImage(DWORD dwIndex);
			BOOL IsDisableCoverButton(DWORD dwIndex);
			BOOL HasSlot(DWORD dwIndex);

			void ClearAllSlot();
			void ClearSlot(DWORD dwIndex);
			void SetSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage * pImage, D3DXCOLOR& diffuseColor);
			void SetSlotCount(DWORD dwIndex, DWORD dwCount);
			void SetSlotCountNew(DWORD dwIndex, DWORD dwGrade, DWORD dwCount);
			void SetSlotCoolTime(DWORD dwIndex, float fCoolTime, float fElapsedTime = 0.0f);
#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM_EX
			void ActivateSlot(DWORD dwIndex, BYTE byColorType);
#else
			void ActivateSlot(DWORD dwIndex);
#endif
			void DeactivateSlot(DWORD dwIndex);
#ifdef ENABLE_HIGHLIGHT_SLOT_NEW_EFFECT
			void ActivateSlotEffect(DWORD dwIndex, BYTE byColorType);
			void DeactivateSlotEffect(DWORD dwIndex);
#endif
			void RefreshSlot();

#ifdef ENABLE_SLOT_WINDOW_EX
			float GetSlotCoolTime(DWORD dwIndex, float * fElapsedTime);
			bool IsActivatedSlot(DWORD dwIndex);
#endif

			DWORD GetSlotCount();

			void LockSlot(DWORD dwIndex);
			void UnlockSlot(DWORD dwIndex);
			BOOL IsLockSlot(DWORD dwIndex);
			void SetCantUseSlot(DWORD dwIndex);
			void SetUseSlot(DWORD dwIndex);
			BOOL IsCantUseSlot(DWORD dwIndex);
			void EnableSlot(DWORD dwIndex);
			void DisableSlot(DWORD dwIndex);
			BOOL IsEnableSlot(DWORD dwIndex);

#ifdef WJ_ENABLE_TRADABLE_ICON
			void SetCanMouseEventSlot(DWORD dwIndex);
			void SetCantMouseEventSlot(DWORD dwIndex);
			void SetUsableSlotOnTopWnd(DWORD dwIndex);
			void SetUnusableSlotOnTopWnd(DWORD dwIndex);
#endif
			// Select
			void ClearSelected();
			void SelectSlot(DWORD dwSelectingIndex);
			BOOL isSelectedSlot(DWORD dwIndex);
			DWORD GetSelectedSlotCount();
			DWORD GetSelectedSlotNumber(DWORD dwIndex);

			// Slot Button
			void ShowSlotButton(DWORD dwSlotNumber);
			void HideAllSlotButton();
			void OnPressedSlotButton(DWORD dwType, DWORD dwSlotNumber, BOOL isLeft = TRUE);

			// Requirement Sign
			void ShowRequirementSign(DWORD dwSlotNumber);
			void HideRequirementSign(DWORD dwSlotNumber);

			// ToolTip
			BOOL OnOverInItem(DWORD dwSlotNumber);
			void OnOverOutItem();

			// For Usable Item
			void SetUseMode(BOOL bFlag);
			void SetUsableItem(BOOL bFlag);

			// CallBack
			void ReserveDestroyCoolTimeFinishEffect(DWORD dwSlotIndex);

#ifdef ENABLE_ITEM_MANAGER
			void SetUsableSlotOnTopWndEx(DWORD dwIndex);
			void SetUnusableSlotOnTopWndEx(DWORD dwIndex);
#endif

		protected:
			void __Initialize();
			void __CreateToggleSlotImage();
			void __CreateSlotEnableEffect();
#ifdef ENABLE_HIGHLIGHT_SLOT_NEW_EFFECT
			void __CreateSlotEnableNewEffect();
#endif
			void __CreateFinishCoolTimeEffect(TSlot * pSlot);
			void __CreateBaseImage(const char * c_szFileName, float fr, float fg, float fb, float fa);

			void __DestroyToggleSlotImage();
			void __DestroySlotEnableEffect();
#ifdef ENABLE_HIGHLIGHT_SLOT_NEW_EFFECT
			void __DestroySlotEnableNewEffect();
#endif
			void __DestroyFinishCoolTimeEffect(TSlot * pSlot);
			void __DestroyBaseImage();

			// Event
			void OnUpdate();
			void OnRender();
			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonUp();
			BOOL OnMouseRightButtonDown();
			BOOL OnMouseLeftButtonDoubleClick();
			void OnMouseOverOut();
			void OnMouseOver();
			void RenderSlotBaseImage();
			void RenderLockedSlot();
			virtual void OnRenderPickingSlot();
			virtual void OnRenderSelectedSlot();

			// Select
			void OnSelectEmptySlot(int iSlotNumber);
			void OnSelectItemSlot(int iSlotNumber);
			void OnUnselectEmptySlot(int iSlotNumber);
			void OnUnselectItemSlot(int iSlotNumber);
			void OnUseSlot();

			// Manage Slot
			BOOL GetSlotPointer(DWORD dwIndex, TSlot ** ppSlot);
			BOOL GetSelectedSlotPointer(TSlot ** ppSlot);
			virtual BOOL GetPickedSlotPointer(TSlot ** ppSlot);
			void ClearSlot(TSlot * pSlot);
			virtual void OnRefreshSlot();

			// ETC
			BOOL OnIsType(DWORD dwType);

		protected:
			DWORD m_dwSlotType;
			DWORD m_dwSlotStyle;
			std::list<DWORD> m_dwSelectedSlotIndexList;
			TSlotList m_SlotList;
			DWORD m_dwToolTipSlotNumber;

			BOOL m_isUseMode;
			BOOL m_isUsableItem;

			float	m_fScaleX;
			float	m_fScaleY;

			CGraphicImageInstance * m_pBaseImageInstance;
			CImageBox * m_pToggleSlotImage;
#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM
			CAniImageBox * m_apSlotActiveEffect[3];
#else
			CAniImageBox * m_pSlotActiveEffect;
#endif
#ifdef ENABLE_HIGHLIGHT_SLOT_NEW_EFFECT
			CAniImageBox * m_apSlotActiveNewEffect[3];
#endif
			std::deque<DWORD> m_ReserveDestroyEffectDeque;
	};
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
