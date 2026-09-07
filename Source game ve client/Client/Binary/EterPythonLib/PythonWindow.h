#pragma once

#include "../eterBase/Utils.h"
#include "../UserInterface/Locale_inc.h"

#ifdef ENABLE_RENDER_TARGET
	#include <cstdint>
#endif
#ifdef ENABLE_INGAME_WIKI
	#include "../EterLib/GrpRenderTargetTexture.h"
#endif

namespace UI
{
	class CWindow
	{
		public:
			typedef std::list<CWindow *> TWindowContainer;

			static DWORD Type();
			BOOL IsType(DWORD dwType);

			enum EHorizontalAlign
			{
				HORIZONTAL_ALIGN_LEFT = 0,
				HORIZONTAL_ALIGN_CENTER = 1,
				HORIZONTAL_ALIGN_RIGHT = 2,
			};

			enum EVerticalAlign
			{
				VERTICAL_ALIGN_TOP = 0,
				VERTICAL_ALIGN_CENTER = 1,
				VERTICAL_ALIGN_BOTTOM = 2,
			};

			enum EFlags
			{
				FLAG_MOVABLE			= (1 <<  0),
				FLAG_LIMIT				= (1 <<  1),
				FLAG_SNAP				= (1 <<  2),
				FLAG_DRAGABLE			= (1 <<  3),
				FLAG_ATTACH				= (1 <<  4),
				FLAG_RESTRICT_X			= (1 <<  5),
				FLAG_RESTRICT_Y			= (1 <<  6),
				FLAG_NOT_CAPTURE		= (1 <<  7),
				FLAG_FLOAT				= (1 <<  8),
				FLAG_NOT_PICK			= (1 <<  9),
				FLAG_IGNORE_SIZE		= (1 << 10),
				FLAG_RTL				= (1 << 11),	// Right-to-left
			};

		public:
			CWindow(PyObject * ppyObject);
			virtual ~CWindow();

			void			AddChild(CWindow * pWin);

			void			Clear();
			void			DestroyHandle();
			void			Update();
			void			Render();

			void			SetName(const char * c_szName);
			const char *	GetName()		{ return m_strName.c_str(); }
			void			SetSize(long width, long height);
			long			GetWidth()		{ return m_lWidth; }
			long			GetHeight()		{ return m_lHeight; }

			void			SetHorizontalAlign(DWORD dwAlign);
			void			SetVerticalAlign(DWORD dwAlign);
			void			SetPosition(long x, long y);
			void			GetPosition(long * plx, long * ply);
			long			GetPositionX( void ) const		{ return m_x; }
			long			GetPositionY( void ) const		{ return m_y; }
			RECT &			GetRect()		{ return m_rect; }
			void			GetLocalPosition(long & rlx, long & rly);
			void			GetMouseLocalPosition(long & rlx, long & rly);
			long			UpdateRect();

			RECT &			GetLimitBias()	{ return m_limitBiasRect; }
			void			SetLimitBias(long l, long r, long t, long b) { m_limitBiasRect.left = l, m_limitBiasRect.right = r, m_limitBiasRect.top = t, m_limitBiasRect.bottom = b; }

			void			Show();
			void			Hide();
#ifdef ENABLE_INGAME_WIKI
			virtual	bool	IsShow();
			void			OnHideWithChilds();
			void			OnHide();
#else
			bool			IsShow() { return m_bShow; }
#endif
			bool			IsRendering();

			bool			HasParent()		{ return m_pParent ? true : false; }
			bool			HasChild()		{ return m_pChildList.empty() ? false : true; }
			int				GetChildCount()	{ return m_pChildList.size(); }

			CWindow *		GetRoot();
			CWindow *		GetParent();
#ifdef ENABLE_INGAME_WIKI
			bool			IsChild(CWindow* pWin, bool bCheckRecursive = false);
#else
			bool			IsChild(CWindow * pWin);
#endif
			void			DeleteChild(CWindow * pWin);
			void			SetTop(CWindow * pWin);

			bool			IsIn(long x, long y);
			bool			IsIn();
			CWindow *		PickWindow(long x, long y);
			CWindow *		PickTopWindow(long x, long y);

			void			__RemoveReserveChildren();

			void			AddFlag(DWORD flag)		{ SET_BIT(m_dwFlag, flag);		}
			void			RemoveFlag(DWORD flag)	{ REMOVE_BIT(m_dwFlag, flag);	}
			bool			IsFlag(DWORD flag)		{ return (m_dwFlag & flag) ? true : false;	}
			/////////////////////////////////////

#ifdef ENABLE_INGAME_WIKI
			void			SetInsideRender(BOOL flag);
			void			GetRenderBox(RECT* box);
			void			UpdateTextLineRenderBox();
			void			UpdateRenderBox();
			void			UpdateRenderBoxRecursive();
			virtual void	OnAfterRender();
			virtual void	OnUpdateRenderBox() {}
#endif

			virtual void	OnRender();
			virtual void	OnUpdate();
			virtual void	OnChangePosition(){}

			virtual void	OnSetFocus();
			virtual void	OnKillFocus();

			virtual void	OnMouseDrag(long lx, long ly);
			virtual void	OnMouseOverIn();
			virtual void	OnMouseOverOut();
			virtual void	OnMouseOver();
			virtual void	OnDrop();
			virtual void	OnTop();
			virtual void	OnIMEUpdate();

			virtual void	OnMoveWindow(long x, long y);

			///////////////////////////////////////

			BOOL			RunIMETabEvent();
			BOOL			RunIMEReturnEvent();
			BOOL			RunIMEKeyDownEvent(int ikey);

			CWindow *		RunKeyDownEvent(int ikey);
			BOOL			RunKeyUpEvent(int ikey);
			BOOL			RunPressEscapeKeyEvent();
			BOOL			RunPressExitKeyEvent();

			virtual BOOL	OnIMETabEvent();
			virtual BOOL	OnIMEReturnEvent();
			virtual BOOL	OnIMEKeyDownEvent(int ikey);

			virtual BOOL	OnIMEChangeCodePage();
			virtual BOOL	OnIMEOpenCandidateListEvent();
			virtual BOOL	OnIMECloseCandidateListEvent();
			virtual BOOL	OnIMEOpenReadingWndEvent();
			virtual BOOL	OnIMECloseReadingWndEvent();

			virtual BOOL	OnMouseLeftButtonDown();
			virtual BOOL	OnMouseLeftButtonUp();
			virtual BOOL	OnMouseLeftButtonDoubleClick();
			virtual BOOL	OnMouseRightButtonDown();
			virtual BOOL	OnMouseRightButtonUp();
			virtual BOOL	OnMouseRightButtonDoubleClick();
			virtual BOOL	OnMouseMiddleButtonDown();
			virtual BOOL	OnMouseMiddleButtonUp();
			virtual BOOL	RunMouseWheelEvent(long nLen);	// Mouse Wheel Support
#ifdef ENABLE_RENDER_TARGET_EX
			virtual BOOL	OnMouseMiddleScroll(int ikey);
#endif
#ifdef ENABLE_MOUSEWHEEL_EVENT
			virtual BOOL	OnMouseWheel(short wDelta);
#endif
			virtual BOOL	OnKeyDown(int ikey);
			virtual BOOL	OnKeyUp(int ikey);
			virtual BOOL	OnPressEscapeKey();
			virtual BOOL	OnPressExitKey();
			///////////////////////////////////////

			virtual void	SetColor(DWORD dwColor){}
			virtual BOOL	OnIsType(DWORD dwType);
			/////////////////////////////////////

			virtual BOOL	IsWindow() { return TRUE; }
			/////////////////////////////////////

#if defined(ENABLE_CLIP_MASK)
			virtual void	SetClippingMaskRect(const RECT& rMask);
			virtual void	SetClippingMaskWindow(CWindow* pMaskWindow);
#endif

#ifdef ENABLE_INGAME_WIKI
		public:
			virtual void iSetRenderingRect(int iLeft, int iTop, int iRight, int iBottom);
			virtual void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
			virtual int GetRenderingWidth();
			virtual int GetRenderingHeight();
			void ResetRenderingRect(bool bCallEvent = true);

		private:
			virtual void OnSetRenderingRect();

		protected:
			RECT m_renderingRect;
			BOOL m_isInsideRender;
			RECT m_renderBox;
#endif

		protected:
			std::string			m_strName;

			EHorizontalAlign	m_HorizontalAlign;
			EVerticalAlign		m_VerticalAlign;
			long				m_x, m_y;
			long				m_lWidth, m_lHeight;
			RECT				m_rect;
			RECT				m_limitBiasRect;

			bool				m_bMovable;
			bool				m_bShow;

			DWORD				m_dwFlag;

			PyObject *			m_poHandler;

			CWindow	*			m_pParent;
			TWindowContainer	m_pChildList;

			BOOL				m_isUpdatingChildren;
			TWindowContainer	m_pReserveChildList;

#if defined(ENABLE_CLIP_MASK)
			bool				m_bEnableMask;
			CWindow*			m_pMaskWindow;
			RECT				m_rMaskRect;
#endif

#ifdef _DEBUG
		public:
			DWORD				DEBUG_dwCounter;
#endif
	};

	class CLayer : public CWindow
	{
		public:
			CLayer(PyObject * ppyObject) : CWindow(ppyObject) {}
			virtual ~CLayer() {}

			BOOL IsWindow() { return FALSE; }
	};

	class CBox : public CWindow
	{
		public:
			CBox(PyObject * ppyObject);
			virtual ~CBox();

			void SetColor(DWORD dwColor);

		protected:
			void OnRender();

		protected:
			DWORD m_dwColor;
	};

	class CBar : public CWindow
	{
		public:
			CBar(PyObject * ppyObject);
			virtual ~CBar();

			void SetColor(DWORD dwColor);

		protected:
			void OnRender();
#if defined(ENABLE_CLIP_MASK)
			void OnUpdate();
#endif

		protected:
			DWORD m_dwColor;
	};

	class CLine : public CWindow
	{
		public:
			CLine(PyObject * ppyObject);
			virtual ~CLine();

			void SetColor(DWORD dwColor);

		protected:
			void OnRender();

		protected:
			DWORD m_dwColor;
	};

	class CBar3D : public CWindow
	{
		public:
			static DWORD Type();

		public:
			CBar3D(PyObject * ppyObject);
			virtual ~CBar3D();

			void SetColor(DWORD dwLeft, DWORD dwRight, DWORD dwCenter);

		protected:
#if defined(ENABLE_CLIP_MASK)
			void OnUpdate();
#endif
			void OnRender();

		protected:
			DWORD m_dwLeftColor;
			DWORD m_dwRightColor;
			DWORD m_dwCenterColor;
	};

	// Text
	class CTextLine : public CWindow
	{
#ifdef ENABLE_INGAME_WIKI
		public:
			static DWORD Type();
#endif
		public:
			CTextLine(PyObject * ppyObject);
			virtual ~CTextLine();

			void SetMax(int iMax);
			void SetHorizontalAlign(int iType);
			void SetVerticalAlign(int iType);
			void SetSecret(BOOL bFlag);
			void SetOutline(BOOL bFlag);
			void SetFeather(BOOL bFlag);
			void SetMultiLine(BOOL bFlag);
			void SetFontName(const char * c_szFontName);
			void SetFontColor(DWORD dwColor);
			void SetLimitWidth(float fWidth);

			void ShowCursor();
			void HideCursor();
			bool IsShowCursor();
			int GetCursorPosition();

			void SetText(const char * c_szText);
			const char * GetText();

			void GetTextSize(int* pnWidth, int* pnHeight);

		protected:
			void OnUpdate();
			void OnRender();
			void OnChangePosition();

			virtual void OnSetText(const char * c_szText);

		protected:
			CGraphicTextInstance m_TextInstance;

#ifdef ENABLE_INGAME_WIKI
		public:
			void SetFixedRenderPos(WORD startPos, WORD endPos) { m_TextInstance.SetFixedRenderPos(startPos, endPos); }
			void GetRenderPositions(WORD& startPos, WORD& endPos) { m_TextInstance.GetRenderPositions(startPos, endPos); }
			bool IsShow();
			int GetRenderingWidth();
			int GetRenderingHeight();
			void OnSetRenderingRect();

		protected:
			void OnUpdateRenderBox() {
				UpdateTextLineRenderBox();
				m_TextInstance.SetRenderBox(m_renderBox);
			}
#endif
	};

	class CNumberLine : public CWindow
	{
		public:
			CNumberLine(PyObject * ppyObject);
			CNumberLine(CWindow * pParent);
			virtual ~CNumberLine();

			void SetPath(const char * c_szPath);
			void SetHorizontalAlign(int iType);
			void SetNumber(const char * c_szNumber);

		protected:
			void ClearNumber();
#if defined(ENABLE_CLIP_MASK)
			void OnUpdate();
#endif
			void OnRender();
			void OnChangePosition();

		protected:
			std::string m_strPath;
			std::string m_strNumber;
			std::vector<CGraphicImageInstance *> m_ImageInstanceVector;

			int m_iHorizontalAlign;
			DWORD m_dwWidthSummary;
	};

#ifdef ENABLE_RENDER_TARGET
	class CUiRenderTarget : public CWindow
	{
	public:
#ifdef ENABLE_RENDER_TARGET_EX
		static DWORD Type();
#endif
		CUiRenderTarget(PyObject* ppyObject);
		virtual ~CUiRenderTarget();

#ifdef ENABLE_RENDER_TARGET_EX
		BOOL OnIsType(DWORD dwType);
#endif

		bool SetRenderTarget(uint8_t index);

	protected:
		DWORD m_dwIndex;
		void OnRender();
	};
#endif

#ifdef ENABLE_INGAME_WIKI
	class CUiWikiRenderTarget : public CWindow
	{
		public:
			CUiWikiRenderTarget(PyObject* ppyObject);
			virtual ~CUiWikiRenderTarget();

		public:
			bool SetWikiRenderTargetModule(int iRenderTargetModule);
			void OnUpdateRenderBox();

		protected:
			void OnRender();

		protected:
			DWORD m_dwIndex;
	};
#endif

	// Image
	class CImageBox : public CWindow
	{
		public:
			CImageBox(PyObject * ppyObject);
			virtual ~CImageBox();

			BOOL LoadImage(const char * c_szFileName);
			void SetDiffuseColor(float fr, float fg, float fb, float fa);

			int GetWidth();
			int GetHeight();

#ifdef ENABLE_PLAYER_HP
			void ShowImage() { m_bIsShowImage = true; }
			void HideImage() { m_bIsShowImage = false; }
			void DisplayImageProcent(float procent) { m_fDisplayProcent = procent; }
#endif

#ifdef ENABLE_INGAME_WIKI
			void UnloadImage()
			{
				OnDestroyInstance();
				SetSize(GetWidth(), GetHeight());
				UpdateRect();
			}
#endif

		protected:
			virtual void OnCreateInstance();
			virtual void OnDestroyInstance();

			virtual void OnUpdate();
			virtual void OnRender();
			void OnChangePosition();

		protected:
			CGraphicImageInstance * m_pImageInstance;
#ifdef ENABLE_PLAYER_HP
			bool m_bIsShowImage;
			float m_fDisplayProcent;
#endif
	};
	class CMarkBox : public CWindow
	{
		public:
			CMarkBox(PyObject * ppyObject);
			virtual ~CMarkBox();

			void LoadImage(const char * c_szFilename);
			void SetDiffuseColor(float fr, float fg, float fb, float fa);
			void SetIndex(UINT uIndex);
			void SetScale(FLOAT fScale);

		protected:
			virtual void OnCreateInstance();
			virtual void OnDestroyInstance();

			virtual void OnUpdate();
			virtual void OnRender();
			void OnChangePosition();
		protected:
			CGraphicMarkInstance * m_pMarkInstance;
	};
	class CExpandedImageBox : public CImageBox
	{
		public:
			static DWORD Type();

		public:
			CExpandedImageBox(PyObject * ppyObject);
			virtual ~CExpandedImageBox();

			void SetScale(float fx, float fy);
			void SetOrigin(float fx, float fy);
			void SetRotation(float fRotation);
			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
			void SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom);
			void SetImageClipRect(float fLeft, float fTop, float fRight, float fBottom, bool bIsVertical = false);
			void SetRenderingMode(int iMode);

#ifdef ENABLE_INGAME_WIKI
			int GetRenderingWidth();
			int GetRenderingHeight();
			void OnSetRenderingRect();
			void SetExpandedRenderingRect(float fLeftTop, float fLeftBottom, float fTopLeft, float fTopRight, float fRightTop, float fRightBottom, float fBottomLeft, float fBottomRight);
			void SetTextureRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
			DWORD GetPixelColor(DWORD x, DWORD y);
#endif

		protected:
			void OnCreateInstance();
			void OnDestroyInstance();

			virtual void OnUpdate();
			virtual void OnRender();

			BOOL OnIsType(DWORD dwType);
#ifdef ENABLE_INGAME_WIKI
			void OnUpdateRenderBox();
#endif
	};
	class CAniImageBox : public CWindow
	{
		public:
			static DWORD Type();

		public:
			CAniImageBox(PyObject * ppyObject);
			virtual ~CAniImageBox();

			void SetDelay(int iDelay);
			void AppendImage(const char * c_szFileName);
			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
			void SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom);
			void SetRenderingMode(int iMode);
#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM_EX
			void SetDiffuseColor(float fR, float fG, float fB, float fA);
			void SetDiffuseColorByType(BYTE byColorType);
#endif
			void SetAniImgScale(float x, float y);

			void ResetFrame();

		protected:
			void OnUpdate();
			void OnRender();
			void OnChangePosition();
			virtual void OnEndFrame();

			BOOL OnIsType(DWORD dwType);

		protected:
			BYTE m_bycurDelay;
			BYTE m_byDelay;
			BYTE m_bycurIndex;
			float m_fScaleX;
			float m_fScaleY;
			std::vector<CGraphicExpandedImageInstance*> m_ImageVector;
	};

	// gif support
	class CGifImageBox : public CAniImageBox
	{
	public:
		static uint32_t Type();

	public:
		CGifImageBox(PyObject* ppyObject);
		virtual ~CGifImageBox();

		bool LoadGif(const char* c_szFileName);
		bool UnloadGif();

	protected:
		BOOL OnIsType(uint32_t dwType);
	};

	// Button
	class CButton : public CWindow
	{
		public:
			CButton(PyObject * ppyObject);
			virtual ~CButton();

			BOOL SetUpVisual(const char * c_szFileName);
			BOOL SetOverVisual(const char * c_szFileName);
			BOOL SetDownVisual(const char * c_szFileName);
			BOOL SetDisableVisual(const char * c_szFileName);

#ifdef ENABLE_NEW_GAMEOPTION
			static DWORD Type();
			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
#endif

			void SetDiffuseColor(float fr, float fg, float fb, float fa);

			const char * GetUpVisualFileName();
			const char * GetOverVisualFileName();
			const char * GetDownVisualFileName();

			void Flash();
			void Enable();
			void Disable();
			void EnableFlash();
			void DisableFlash();

			void SetUp();
			void Up();
			void Over();
			void Down();

			BOOL IsDisable();
			BOOL IsPressed();

			void SetScale(float fx, float fy);

#ifdef ENABLE_INGAME_WIKI
			void OnSetRenderingRect();
#endif

		protected:
			void OnUpdate();
			void OnRender();
			void OnChangePosition();

			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonDoubleClick();
			BOOL OnMouseLeftButtonUp();
			void OnMouseOverIn();
			void OnMouseOverOut();

			BOOL IsEnable();

#ifdef ENABLE_NEW_GAMEOPTION
			BOOL OnIsType(DWORD dwType);
#endif

#ifdef ENABLE_INGAME_WIKI
			void SetCurrentVisual(CGraphicExpandedImageInstance* pVisual);
#else
			void SetCurrentVisual(CGraphicImageInstance * pVisual);
#endif

		protected:
			BOOL m_bEnable;
			BOOL m_isPressed;
			BOOL m_isFlash;
			BOOL m_isFlashEnable;
			float m_fScaleX;
			float m_fScaleY;
#ifdef ENABLE_INGAME_WIKI
			CGraphicExpandedImageInstance* m_pcurVisual;
			CGraphicExpandedImageInstance m_upVisual;
			CGraphicExpandedImageInstance m_overVisual;
			CGraphicExpandedImageInstance m_downVisual;
			CGraphicExpandedImageInstance m_disableVisual;
#else
			CGraphicImageInstance * m_pcurVisual;
			CGraphicImageInstance m_upVisual;
			CGraphicImageInstance m_overVisual;
			CGraphicImageInstance m_downVisual;
			CGraphicImageInstance m_disableVisual;
#endif
	};
	class CRadioButton : public CButton
	{
		public:
#ifdef ENABLE_NEW_GAMEOPTION
			static DWORD Type();
#endif
			CRadioButton(PyObject * ppyObject);
			virtual ~CRadioButton();

		protected:
			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonUp();
			void OnMouseOverIn();
			void OnMouseOverOut();
#ifdef ENABLE_NEW_GAMEOPTION
			BOOL OnIsType(DWORD dwType);
#endif
	};
	class CToggleButton : public CButton
	{
		public:
#ifdef ENABLE_NEW_GAMEOPTION
			static DWORD Type();
#endif
			CToggleButton(PyObject * ppyObject);
			virtual ~CToggleButton();

		protected:
			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonUp();
			void OnMouseOverIn();
			void OnMouseOverOut();
#ifdef ENABLE_NEW_GAMEOPTION
			BOOL OnIsType(DWORD dwType);
#endif
	};
	class CDragButton : public CButton
	{
		public:
			CDragButton(PyObject * ppyObject);
			virtual ~CDragButton();

			void SetRestrictMovementArea(int ix, int iy, int iwidth, int iheight);

		protected:
			void OnChangePosition();
			void OnMouseOverIn();
			void OnMouseOverOut();

		protected:
			RECT m_restrictArea;
	};
};

extern BOOL g_bOutlineBoxEnable;
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
