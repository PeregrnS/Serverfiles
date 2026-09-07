#include "StdAfx.h"
#include "../eterBase/CRC32.h"
#include "PythonWindow.h"
#include "PythonSlotWindow.h"
#include "PythonWindowManager.h"
#ifdef ENABLE_RENDER_TARGET
	#include "../eterLib/CRenderTargetManager.h"
#endif
#ifdef ENABLE_INGAME_WIKI
	#include "../eterLib/CWikiRenderTargetManager.h"
#endif

// gif support
#include "../eterpack/EterPackManager.h"
#include <array>
#include <gif_lib.h>
#include <wingdi.h>

#ifdef ENABLE_GUI_SHOWER
BOOL g_bOutlineBoxEnable = TRUE;
#else
BOOL g_bOutlineBoxEnable = FALSE;
#endif

namespace UI
{
	CWindow::CWindow(PyObject * ppyObject) :
		m_x(0),
		m_y(0),
		m_lWidth(0),
		m_lHeight(0),
		m_poHandler(ppyObject),
		m_bShow(false),
		m_pParent(NULL),
		m_dwFlag(0),
		m_isUpdatingChildren(FALSE)
#ifdef ENABLE_INGAME_WIKI
		, m_isInsideRender(false)
#endif
	{
#ifdef _DEBUG
		static DWORD DEBUG_dwGlobalCounter=0;
		DEBUG_dwCounter=DEBUG_dwGlobalCounter++;

		m_strName = "!!debug";
#endif
		//assert(m_poHandler != NULL);
		m_HorizontalAlign = HORIZONTAL_ALIGN_LEFT;
		m_VerticalAlign = VERTICAL_ALIGN_TOP;
		m_rect.bottom = m_rect.left = m_rect.right = m_rect.top = 0;
		m_limitBiasRect.bottom = m_limitBiasRect.left = m_limitBiasRect.right = m_limitBiasRect.top = 0;

#if defined(ENABLE_CLIP_MASK)
		m_bEnableMask = false;
		m_pMaskWindow = NULL;
		memset(&m_rMaskRect, 0, sizeof(m_rMaskRect));
#endif

#ifdef ENABLE_INGAME_WIKI
		memset(&m_renderBox, 0, sizeof(m_renderBox));
#endif
	}

	CWindow::~CWindow()
	{
	}

	DWORD CWindow::Type()
	{
		static DWORD s_dwType = GetCRC32("CWindow", strlen("CWindow"));
		return (s_dwType);
	}

	BOOL CWindow::IsType(DWORD dwType)
	{
		return OnIsType(dwType);
	}

	BOOL CWindow::OnIsType(DWORD dwType)
	{
		if (CWindow::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	struct FClear
	{
		void operator () (CWindow * pWin)
		{
			pWin->Clear();
		}
	};

	void CWindow::Clear()
	{
		std::for_each(m_pChildList.begin(), m_pChildList.end(), FClear());
#if defined(ENABLE_CLIP_MASK)
		m_pMaskWindow = NULL;
#endif
		m_pChildList.clear();

		m_pParent = NULL;
		DestroyHandle();
		Hide();
	}

	void CWindow::DestroyHandle()
	{
		m_poHandler = NULL;
	}

	void CWindow::Show()
	{
		m_bShow = true;
	}

	void CWindow::Hide()
	{
		m_bShow = false;
	}

#ifdef ENABLE_INGAME_WIKI
	void CWindow::OnHideWithChilds()
	{
		OnHide();
		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::OnHideWithChilds));
	}

	void CWindow::OnHide()
	{
		PyCallClassMemberFunc(m_poHandler, "OnHide", BuildEmptyTuple());
	}
#endif

	bool CWindow::IsRendering()
	{
		if (!IsShow())
			return false;

		if (!m_pParent)
			return true;

		return m_pParent->IsRendering();
	}

	void CWindow::__RemoveReserveChildren()
	{
		if (m_pReserveChildList.empty())
			return;

		TWindowContainer::iterator it;
		for(it = m_pReserveChildList.begin(); it != m_pReserveChildList.end(); ++it)
		{
			m_pChildList.remove(*it);
		}
		m_pReserveChildList.clear();
	}

	void CWindow::Update()
	{
		if (!IsShow())
			return;

		__RemoveReserveChildren();

		OnUpdate();

		m_isUpdatingChildren = TRUE;

		for (auto& Child : m_pChildList)
			Child->Update();

		m_isUpdatingChildren = FALSE;
	}

#ifdef ENABLE_INGAME_WIKI
	bool CWindow::IsShow()
	{
		if (!m_bShow)
			return false;

		if (m_isInsideRender)
			if (m_renderBox.left + m_renderBox.right >= m_lWidth || m_renderBox.top + m_renderBox.bottom >= m_lHeight)
				return false;

		return true;
	}
#endif

	void CWindow::Render()
	{
		if (!IsShow())
			return;

		OnRender();

#ifdef ENABLE_GUI_SHOWER
		if (CWindowManager::instance().GetPointWindow() == this)
		{
			CPythonGraphic::instance().SetDiffuseColor(0.0f, 1.0f, 0.0f);
			CPythonGraphic::instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
		}
#else
		if (g_bOutlineBoxEnable)
		{
			CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f);
			CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
		}
#endif

		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::void_mem_fun(&CWindow::Render));
#ifdef ENABLE_INGAME_WIKI
		OnAfterRender();
#endif
	}

	void CWindow::OnUpdate()
	{
		if (!m_poHandler)
			return;

		if (!IsShow())
			return;

		static PyObject* poFuncName_OnUpdate = PyString_InternFromString("OnUpdate");

		//PyCallClassMemberFunc(m_poHandler, "OnUpdate", BuildEmptyTuple());
		PyCallClassMemberFunc_ByPyString(m_poHandler, poFuncName_OnUpdate, BuildEmptyTuple());

	}

	void CWindow::OnRender()
	{
		if (!m_poHandler)
			return;

		if (!IsShow())
			return;

		//PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
	}

#ifdef ENABLE_INGAME_WIKI
	void CWindow::OnAfterRender()
	{
		if (!m_poHandler)
			return;

		if (!IsShow())
			return;

		PyCallClassMemberFunc(m_poHandler, "OnAfterRender", BuildEmptyTuple());
	}
#endif

	void CWindow::SetName(const char * c_szName)
	{
		m_strName = c_szName;
	}

	void CWindow::SetSize(long width, long height)
	{
		m_lWidth = width;
		m_lHeight = height;

		m_rect.right = m_rect.left + m_lWidth;
		m_rect.bottom = m_rect.top + m_lHeight;

#ifdef ENABLE_INGAME_WIKI
		if (m_isInsideRender)
			UpdateRenderBoxRecursive();
#endif
	}

	void CWindow::SetHorizontalAlign(DWORD dwAlign)
	{
		m_HorizontalAlign = (EHorizontalAlign)dwAlign;
		UpdateRect();
	}

	void CWindow::SetVerticalAlign(DWORD dwAlign)
	{
		m_VerticalAlign = (EVerticalAlign)dwAlign;
		UpdateRect();
	}

	void CWindow::SetPosition(long x, long y)
	{
		m_x = x;
		m_y = y;

		UpdateRect();
#ifdef ENABLE_INGAME_WIKI
		if (m_isInsideRender)
		{
			UpdateRenderBoxRecursive();
		}
#endif
	}

#ifdef ENABLE_INGAME_WIKI
	void CWindow::UpdateRenderBoxRecursive()
	{
		UpdateRenderBox();
		for (auto it = m_pChildList.begin(); it != m_pChildList.end(); ++it)
			(*it)->UpdateRenderBoxRecursive();
	}
#endif

	void CWindow::GetPosition(long * plx, long * ply)
	{
		*plx = m_x;
		*ply = m_y;
	}

	long CWindow::UpdateRect()
	{
		m_rect.top		= m_y;
		if (m_pParent)
		{
			switch (m_VerticalAlign)
			{
				case VERTICAL_ALIGN_BOTTOM:
					m_rect.top = m_pParent->GetHeight() - m_rect.top;
					break;
				case VERTICAL_ALIGN_CENTER:
					m_rect.top = (m_pParent->GetHeight() - GetHeight()) / 2 + m_rect.top;
					break;
			}
			m_rect.top += m_pParent->m_rect.top;
		}
		m_rect.bottom	= m_rect.top + m_lHeight;

#if defined( _USE_CPP_RTL_FLIP )
		if( m_pParent == NULL ) {
			m_rect.left		= m_x;
			m_rect.right	= m_rect.left + m_lWidth;
		} else {
			if( m_pParent->IsFlag(UI::CWindow::FLAG_RTL) == true ) {
				m_rect.left = m_pParent->GetWidth() - m_lWidth - m_x;
				switch (m_HorizontalAlign)
				{
					case HORIZONTAL_ALIGN_RIGHT:
						m_rect.left = - m_x;
						break;
					case HORIZONTAL_ALIGN_CENTER:
						m_rect.left = m_pParent->GetWidth() / 2 - GetWidth() - m_x;
						break;
				}
				m_rect.left += m_pParent->m_rect.left;
				m_rect.right = m_rect.left + m_lWidth;
			} else {
				m_rect.left		= m_x;
				switch (m_HorizontalAlign)
				{
					case HORIZONTAL_ALIGN_RIGHT:
						m_rect.left = m_pParent->GetWidth() - m_rect.left;
						break;
					case HORIZONTAL_ALIGN_CENTER:
						m_rect.left = (m_pParent->GetWidth() - GetWidth()) / 2 + m_rect.left;
						break;
				}
				m_rect.left += m_pParent->m_rect.left;
				m_rect.right = m_rect.left + m_lWidth;
			}
		}
#else
		m_rect.left		= m_x;
		if (m_pParent)
		{
			switch (m_HorizontalAlign)
			{
				case HORIZONTAL_ALIGN_RIGHT:
					m_rect.left = ::abs(m_pParent->GetWidth()) - m_rect.left;
					break;
				case HORIZONTAL_ALIGN_CENTER:
					m_rect.left = m_pParent->GetWidth() / 2 - GetWidth() / 2 + m_rect.left;
					break;
			}
			m_rect.left += 0L < m_pParent->GetWidth() ? m_pParent->m_rect.left : m_pParent->m_rect.right + ::abs(m_pParent->GetWidth());
		}
		m_rect.right = m_rect.left + m_lWidth;
#endif
		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::UpdateRect));

		OnChangePosition();

		return 1;
	}

	void CWindow::GetLocalPosition(long & rlx, long & rly)
	{
		rlx = rlx - m_rect.left;
		rly = rly - m_rect.top;
	}

	void CWindow::GetMouseLocalPosition(long & rlx, long & rly)
	{
		CWindowManager::Instance().GetMousePosition(rlx, rly);
		rlx = rlx - m_rect.left;
		rly = rly - m_rect.top;
	}

	void CWindow::AddChild(CWindow * pWin)
	{
		m_pChildList.push_back(pWin);
		pWin->m_pParent = this;
#ifdef ENABLE_INGAME_WIKI
		if (m_isInsideRender && !pWin->m_isInsideRender)
		{
			pWin->SetInsideRender(m_isInsideRender);
		}
#endif
	}


#ifdef ENABLE_INGAME_WIKI
	void CWindow::SetInsideRender(BOOL flag)
	{
		if (!m_pParent || m_isInsideRender && m_pParent->m_isInsideRender)
			return;

		if (m_isInsideRender && flag)
			return;

		m_isInsideRender = flag;
		UpdateRenderBox();
		for (auto it = m_pChildList.begin(); it != m_pChildList.end(); ++it)
			(*it)->SetInsideRender(m_isInsideRender);
	}

	void CWindow::GetRenderBox(RECT* box)
	{
		memcpy(box, &m_renderBox, sizeof(RECT));
	}
	
	void CWindow::UpdateTextLineRenderBox()
	{
		int width, height;
		((CTextLine*)this)->GetTextSize(&width, &height);
		
		int pWidth = m_pParent->GetWidth();
		int pHeight = m_pParent->GetHeight();

		if (m_x - m_pParent->m_renderBox.left < 0)
			m_renderBox.left = -m_x + m_pParent->m_renderBox.left;
		else
			m_renderBox.left = 0;

		if (m_y - m_pParent->m_renderBox.top < 0)
			m_renderBox.top = -m_y + m_pParent->m_renderBox.top;
		else
			m_renderBox.top = 0;

		if (m_x + width > pWidth - m_pParent->m_renderBox.right)
			m_renderBox.right = m_x + width - pWidth + m_pParent->m_renderBox.right;
		else
			m_renderBox.right = 0;

		if (m_y + height > pHeight - m_pParent->m_renderBox.bottom)
			m_renderBox.bottom = m_y + height - pHeight + m_pParent->m_renderBox.bottom;
		else
			m_renderBox.bottom = 0;
	}

	void CWindow::UpdateRenderBox()
	{
		if (!m_isInsideRender || !m_pParent)
			memset(&m_renderBox, 0, sizeof(m_renderBox));
		else
		{
			int width = m_lWidth;
			int height = m_lHeight;
			int pWidth = m_pParent->GetWidth();
			int pHeight = m_pParent->GetHeight();

			if (m_x - m_pParent->m_renderBox.left < 0)
				m_renderBox.left = -m_x + m_pParent->m_renderBox.left;
			else
				m_renderBox.left = 0;

			if (m_y - m_pParent->m_renderBox.top < 0)
				m_renderBox.top = -m_y + m_pParent->m_renderBox.top;
			else
				m_renderBox.top = 0;

			if (m_x + width > pWidth - m_pParent->m_renderBox.right)
				m_renderBox.right = m_x + width - pWidth + m_pParent->m_renderBox.right;
			else
				m_renderBox.right = 0;

			if (m_y + height > pHeight - m_pParent->m_renderBox.bottom)
				m_renderBox.bottom = m_y + height - pHeight + m_pParent->m_renderBox.bottom;
			else
				m_renderBox.bottom = 0;
		}
		
		OnUpdateRenderBox();
	}
#endif

	CWindow * CWindow::GetRoot()
	{
		if (m_pParent)
			if (m_pParent->IsWindow())
				return m_pParent->GetRoot();

		return this;
	}

	CWindow * CWindow::GetParent()
	{
		return m_pParent;
	}

#ifdef ENABLE_INGAME_WIKI
	bool CWindow::IsChild(CWindow* pWin, bool bCheckRecursive)
#else
	bool CWindow::IsChild(CWindow * pWin)
#endif
	{
		auto itor = m_pChildList.begin();

		while (itor != m_pChildList.end())
		{
			if (*itor == pWin)
				return true;

#ifdef ENABLE_INGAME_WIKI
			if (bCheckRecursive)
			{
				if ((*itor)->IsChild(pWin, true))
				{
					return true;
				}
			}
#endif

			++itor;
		}

		return false;
	}

	void CWindow::DeleteChild(CWindow * pWin)
	{
		if (m_isUpdatingChildren)
		{
			m_pReserveChildList.push_back(pWin);
		}
		else
		{
			m_pChildList.remove(pWin);
		}
	}

	void CWindow::SetTop(CWindow * pWin)
	{
		if (!pWin->IsFlag(CWindow::FLAG_FLOAT))
			return;

		TWindowContainer::iterator itor = std::find(m_pChildList.begin(), m_pChildList.end(), pWin);
		if (m_pChildList.end() != itor)
		{
			m_pChildList.push_back(*itor);
			m_pChildList.erase(itor);

			pWin->OnTop();
		}
		else
		{
			TraceError(" CWindow::SetTop - Failed to find child window\n");
		}
	}

	void CWindow::OnMouseDrag(long lx, long ly)
	{
		PyCallClassMemberFunc(m_poHandler, "OnMouseDrag", Py_BuildValue("(ii)", lx, ly));
	}

	void CWindow::OnMoveWindow(long lx, long ly)
	{
		PyCallClassMemberFunc(m_poHandler, "OnMoveWindow", Py_BuildValue("(ii)", lx, ly));
	}

	void CWindow::OnSetFocus()
	{
		//PyCallClassMemberFunc(m_poHandler, "OnSetFocus", BuildEmptyTuple());
		PyCallClassMemberFunc(m_poHandler, "OnSetFocus", BuildEmptyTuple());
	}

	void CWindow::OnKillFocus()
	{
		PyCallClassMemberFunc(m_poHandler, "OnKillFocus", BuildEmptyTuple());
	}

	void CWindow::OnMouseOverIn()
	{
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
	}

	void CWindow::OnMouseOverOut()
	{
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
	}

	void CWindow::OnMouseOver()
	{
	}

	void CWindow::OnDrop()
	{
		PyCallClassMemberFunc(m_poHandler, "OnDrop", BuildEmptyTuple());
	}

	void CWindow::OnTop()
	{
		PyCallClassMemberFunc(m_poHandler, "OnTop", BuildEmptyTuple());
	}

	void CWindow::OnIMEUpdate()
	{
		PyCallClassMemberFunc(m_poHandler, "OnIMEUpdate", BuildEmptyTuple());
	}

#if defined(ENABLE_CLIP_MASK)
	void CWindow::SetClippingMaskRect(const RECT& rMask)
	{
		m_bEnableMask = true;
		m_rMaskRect = rMask;

		TWindowContainer::const_reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow)
				pWindow->SetClippingMaskRect(rMask);
		}
	}

	void CWindow::SetClippingMaskWindow(CWindow* pMaskWindow)
	{
		if (!pMaskWindow)
			return;

		m_bEnableMask = true;
		m_pMaskWindow = pMaskWindow;

		TWindowContainer::const_reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow* pWindow = *itor;

			if (pWindow)
				pWindow->SetClippingMaskWindow(pMaskWindow);
		}
	}
#endif

	BOOL CWindow::RunIMETabEvent()
	{
		if (!IsRendering())
			return FALSE;

		if (OnIMETabEvent())
			return TRUE;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow * pWindow = *itor;

			if (pWindow->RunIMETabEvent())
				return TRUE;
		}

		return FALSE;
	}

	BOOL CWindow::RunIMEReturnEvent()
	{
		if (!IsRendering())
			return FALSE;

		if (OnIMEReturnEvent())
			return TRUE;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow * pWindow = *itor;

			if (pWindow->RunIMEReturnEvent())
				return TRUE;
		}

		return FALSE;
	}

	BOOL CWindow::RunIMEKeyDownEvent(int ikey)
	{
		if (!IsRendering())
			return FALSE;

		if (OnIMEKeyDownEvent(ikey))
			return TRUE;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow * pWindow = *itor;

			if (pWindow->RunIMEKeyDownEvent(ikey))
				return TRUE;
		}

		return FALSE;
	}

	CWindow * CWindow::RunKeyDownEvent(int ikey)
	{
		if (OnKeyDown(ikey))
			return this;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow * pWindow = *itor;

			if (pWindow->IsShow())
			{
				CWindow * pProcessedWindow = pWindow->RunKeyDownEvent(ikey);
				if (NULL != pProcessedWindow)
				{
					return pProcessedWindow;
				}
			}
		}

		return NULL;
	}

	BOOL CWindow::RunKeyUpEvent(int ikey)
	{
		if (OnKeyUp(ikey))
			return TRUE;

		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow * pWindow = *itor;

			if (pWindow->IsShow())
			if (pWindow->RunKeyUpEvent(ikey))
				return TRUE;
		}

		return FALSE;
	}

	BOOL CWindow::RunPressEscapeKeyEvent()
	{
		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow * pWindow = *itor;

			if (pWindow->IsShow())
			if (pWindow->RunPressEscapeKeyEvent())
				return TRUE;
		}

		if (OnPressEscapeKey())
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::RunPressExitKeyEvent()
	{
		TWindowContainer::reverse_iterator itor;
		for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
		{
			CWindow * pWindow = *itor;

			if (pWindow->RunPressExitKeyEvent())
				return TRUE;

			if (pWindow->IsShow())
			if (pWindow->OnPressExitKey())
				return TRUE;
		}

		return FALSE;
	}

	BOOL CWindow::OnMouseLeftButtonDown()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonDown", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

#ifdef ENABLE_RENDER_TARGET_EX
	BOOL CWindow::OnMouseMiddleScroll(int ikey)
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseMiddleScroll", Py_BuildValue("(i)", ikey), &lValue))
			if (0 != lValue)
			{
				return TRUE;
			}

		return FALSE;
	}
#endif

	BOOL CWindow::OnMouseLeftButtonUp()
	{
		PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonUp", BuildEmptyTuple());
		return TRUE;
	}

	BOOL CWindow::OnMouseLeftButtonDoubleClick()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonDoubleClick", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseRightButtonDown()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonDown", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseRightButtonUp()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonUp", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseRightButtonDoubleClick()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonDoubleClick", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseMiddleButtonDown()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseMiddleButtonDown", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnMouseMiddleButtonUp()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnMouseMiddleButtonUp", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	// Mouse Wheel Support
	BOOL CWindow::RunMouseWheelEvent(long nLen)
	{
		bool bValue = false;
		if (PyCallClassMemberFunc(m_poHandler, "OnRunMouseWheel", Py_BuildValue("(l)", nLen), &bValue))
		{
			return bValue;
		}
		return bValue;
	}

#ifdef ENABLE_MOUSEWHEEL_EVENT
	BOOL CWindow::OnMouseWheel(short wDelta)
	{
#ifdef _DEBUG
		Tracenf("Mouse Wheel Scroll : wDelta %d ",wDelta);
#endif
		long lValue = 0;
		return PyCallClassMemberFunc(m_poHandler , "OnMouseWheel" , Py_BuildValue("(i)" , wDelta), &lValue) && 0 != lValue;
	}
#endif

	BOOL CWindow::OnIMETabEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMETab", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEReturnEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEReturn", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEKeyDownEvent(int ikey)
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEKeyDown", Py_BuildValue("(i)", ikey), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEChangeCodePage()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEChangeCodePage", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEOpenCandidateListEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEOpenCandidateList", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMECloseCandidateListEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMECloseCandidateList", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMEOpenReadingWndEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMEOpenReadingWnd", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnIMECloseReadingWndEvent()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnIMECloseReadingWnd", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnKeyDown(int ikey)
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnKeyDown", Py_BuildValue("(i)", ikey), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnKeyUp(int ikey)
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnKeyUp", Py_BuildValue("(i)", ikey), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnPressEscapeKey()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnPressEscapeKey", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	BOOL CWindow::OnPressExitKey()
	{
		long lValue;
		if (PyCallClassMemberFunc(m_poHandler, "OnPressExitKey", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
			return TRUE;

		return FALSE;
	}

	/////

	bool CWindow::IsIn(long x, long y)
	{
		if (x >= m_rect.left && x <= m_rect.right)
			if (y >= m_rect.top && y <= m_rect.bottom)
				return true;

		return false;
	}

	bool CWindow::IsIn()
	{
		long lx, ly;
		UI::CWindowManager::Instance().GetMousePosition(lx, ly);

		return IsIn(lx, ly);
	}

	CWindow * CWindow::PickWindow(long x, long y)
	{
		auto ritor = m_pChildList.rbegin();
		for (; ritor != m_pChildList.rend(); ++ritor)
		{
			CWindow * pWin = *ritor;
			if (pWin->IsShow())
			{
				if (!pWin->IsFlag(CWindow::FLAG_IGNORE_SIZE))
				{
					if (!pWin->IsIn(x, y)) {
						if (0L <= pWin->GetWidth()) {
							continue;
						}
					}
				}

				CWindow * pResult = pWin->PickWindow(x, y);
				if (pResult)
					return pResult;
			}
		}

		if (IsFlag(CWindow::FLAG_NOT_PICK))
			return NULL;

		return (this);
	}

	CWindow * CWindow::PickTopWindow(long x, long y)
	{
		auto ritor = m_pChildList.rbegin();
		for (; ritor != m_pChildList.rend(); ++ritor)
		{
			CWindow * pWin = *ritor;
			if (pWin->IsShow())
				if (pWin->IsIn(x, y))
					if (!pWin->IsFlag(CWindow::FLAG_NOT_PICK))
						return pWin;
		}

		return NULL;
	}


#ifdef ENABLE_INGAME_WIKI
	void CWindow::iSetRenderingRect(int iLeft, int iTop, int iRight, int iBottom)
	{
		m_renderingRect.left = iLeft;
		m_renderingRect.top = iTop;
		m_renderingRect.right = iRight;
		m_renderingRect.bottom = iBottom;

		OnSetRenderingRect();
	}

	void CWindow::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
	{
		float fWidth = float(GetWidth());
		float fHeight = float(GetHeight());
		if (IsType(CTextLine::Type()))
		{
			int iWidth, iHeight;
			((CTextLine*)this)->GetTextSize(&iWidth, &iHeight);
			fWidth = float(iWidth);
			fHeight = float(iHeight);
		}
		
		iSetRenderingRect(fWidth * fLeft, fHeight * fTop, fWidth * fRight, fHeight * fBottom);
	}

	int CWindow::GetRenderingWidth()
	{
		return max(0, GetWidth() + m_renderingRect.right + m_renderingRect.left);
	}

	int CWindow::GetRenderingHeight()
	{
		return max(0, GetHeight() + m_renderingRect.bottom + m_renderingRect.top);
	}

	void CWindow::ResetRenderingRect(bool bCallEvent)
	{
		m_renderingRect.bottom = m_renderingRect.left = m_renderingRect.right = m_renderingRect.top = 0;

		if (bCallEvent)
			OnSetRenderingRect();
	}

	void CWindow::OnSetRenderingRect()
	{
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CBox::CBox(PyObject * ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)
	{
	}
	CBox::~CBox()
	{
	}

	void CBox::SetColor(DWORD dwColor)
	{
		m_dwColor = dwColor;
	}

	void CBox::OnRender()
	{
		CPythonGraphic::Instance().SetDiffuseColor(m_dwColor);
		CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CBar::CBar(PyObject * ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)
	{
	}
	CBar::~CBar()
	{
	}

	void CBar::SetColor(DWORD dwColor)
	{
		m_dwColor = dwColor;
	}

#if defined(ENABLE_CLIP_MASK)
	void CBar::OnUpdate()
	{
		if (!IsShow())
			return;

		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();

		CWindow::OnUpdate();
	}
#endif

	void CBar::OnRender()
	{
		CPythonGraphic::Instance().SetDiffuseColor(m_dwColor);
#ifdef ENABLE_INGAME_WIKI
		CPythonGraphic::Instance().RenderBar2d(m_rect.left + m_renderBox.left, m_rect.top + m_renderBox.top, m_rect.right - m_renderBox.right, m_rect.bottom - m_renderBox.bottom);
#else
		CPythonGraphic::Instance().RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
#endif
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CLine::CLine(PyObject * ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)
	{
	}
	CLine::~CLine()
	{
	}

	void CLine::SetColor(DWORD dwColor)
	{
		m_dwColor = dwColor;
	}

	void CLine::OnRender()
	{
		CPythonGraphic & rkpyGraphic = CPythonGraphic::Instance();
		rkpyGraphic.SetDiffuseColor(m_dwColor);
		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	DWORD CBar3D::Type()
	{
		static DWORD s_dwType = GetCRC32("CBar3D", strlen("CBar3D"));
		return (s_dwType);
	}

	CBar3D::CBar3D(PyObject * ppyObject) : CWindow(ppyObject)
	{
		m_dwLeftColor = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
		m_dwRightColor = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
		m_dwCenterColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	}
	CBar3D::~CBar3D()
	{
	}

	void CBar3D::SetColor(DWORD dwLeft, DWORD dwRight, DWORD dwCenter)
	{
		m_dwLeftColor = dwLeft;
		m_dwRightColor = dwRight;
		m_dwCenterColor = dwCenter;
	}

#if defined(ENABLE_CLIP_MASK)
	void CBar3D::OnUpdate()
	{
		if (!IsShow())
			return;

		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();

		CWindow::OnUpdate();
	}
#endif

	void CBar3D::OnRender()
	{
		CPythonGraphic & rkpyGraphic = CPythonGraphic::Instance();

#if defined(ENABLE_CLIP_MASK)
		if (m_bEnableMask)
		{
			if (m_rMaskRect.left >= m_rect.right || m_rMaskRect.right <= m_rect.left ||
				m_rMaskRect.top >= m_rect.bottom || m_rMaskRect.bottom <= m_rect.top)
				return;

			rkpyGraphic.SetDiffuseColor(m_dwCenterColor);
			rkpyGraphic.RenderBar2d(MAX(m_rect.left, m_rMaskRect.left),
				MAX(m_rect.top, m_rMaskRect.top),
				MIN(m_rect.right, m_rMaskRect.right),
				MIN(m_rect.bottom, m_rMaskRect.bottom));

			rkpyGraphic.SetDiffuseColor(m_dwLeftColor);
			if (m_rMaskRect.top <= m_rect.top && m_rMaskRect.bottom > m_rect.top)
				rkpyGraphic.RenderLine2d(MAX(m_rect.left, m_rMaskRect.left),
					m_rect.top,
					MIN(m_rect.right, m_rMaskRect.right),
					m_rect.top);

			if (m_rMaskRect.left <= m_rect.left && m_rMaskRect.right > m_rect.left)
				rkpyGraphic.RenderLine2d(m_rect.left,
					MAX(m_rect.top, m_rMaskRect.top),
					m_rect.left,
					MIN(m_rect.bottom, m_rMaskRect.bottom));

			rkpyGraphic.SetDiffuseColor(m_dwRightColor);
			if (m_rMaskRect.bottom > m_rect.bottom && m_rMaskRect.top <= m_rect.bottom)
				rkpyGraphic.RenderLine2d(MAX(m_rect.left, m_rMaskRect.left),
					m_rect.bottom,
					MIN(m_rect.right, m_rMaskRect.right),
					m_rect.bottom);

			if (m_rMaskRect.right > m_rect.right && m_rMaskRect.left <= m_rect.right)
				rkpyGraphic.RenderLine2d(m_rect.right,
					MAX(m_rect.top, m_rMaskRect.top),
					m_rect.right,
					MIN(m_rect.bottom, m_rMaskRect.bottom));
		}
		else
		{
			rkpyGraphic.SetDiffuseColor(m_dwCenterColor);
			rkpyGraphic.RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);

			rkpyGraphic.SetDiffuseColor(m_dwLeftColor);
			rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.top);
			rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.left, m_rect.bottom);

			rkpyGraphic.SetDiffuseColor(m_dwRightColor);
			rkpyGraphic.RenderLine2d(m_rect.left, m_rect.bottom, m_rect.right, m_rect.bottom);
			rkpyGraphic.RenderLine2d(m_rect.right, m_rect.top, m_rect.right, m_rect.bottom);
		}
#else
		rkpyGraphic.SetDiffuseColor(m_dwCenterColor);
		rkpyGraphic.RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);

		rkpyGraphic.SetDiffuseColor(m_dwLeftColor);
		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.top);
		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.left, m_rect.bottom);

		rkpyGraphic.SetDiffuseColor(m_dwRightColor);
		rkpyGraphic.RenderLine2d(m_rect.left, m_rect.bottom, m_rect.right, m_rect.bottom);
		rkpyGraphic.RenderLine2d(m_rect.right, m_rect.top, m_rect.right, m_rect.bottom);
#endif
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CTextLine::CTextLine(PyObject * ppyObject) : CWindow(ppyObject)
	{
		m_TextInstance.SetColor(0.78f, 0.78f, 0.78f);
		m_TextInstance.SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
		m_TextInstance.SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_TOP);
	}
	CTextLine::~CTextLine()
	{
		m_TextInstance.Destroy();
	}

#ifdef ENABLE_INGAME_WIKI
	DWORD CTextLine::Type()
	{
		static DWORD s_dwType = GetCRC32("CTextLine", strlen("CTextLine"));
		return (s_dwType);
	}
#endif

	void CTextLine::SetMax(int iMax)
	{
		m_TextInstance.SetMax(iMax);
	}
	void CTextLine::SetHorizontalAlign(int iType)
	{
		m_TextInstance.SetHorizonalAlign(iType);
	}
	void CTextLine::SetVerticalAlign(int iType)
	{
		m_TextInstance.SetVerticalAlign(iType);
	}
	void CTextLine::SetSecret(BOOL bFlag)
	{
		m_TextInstance.SetSecret(bFlag ? true : false);
	}
	void CTextLine::SetOutline(BOOL bFlag)
	{
		m_TextInstance.SetOutline(bFlag ? true : false);
	}
	void CTextLine::SetFeather(BOOL bFlag)
	{
		m_TextInstance.SetFeather(bFlag ? true : false);
	}
	void CTextLine::SetMultiLine(BOOL bFlag)
	{
		m_TextInstance.SetMultiLine(bFlag ? true : false);
	}
	void CTextLine::SetFontName(const char * c_szFontName)
	{
		std::string stFontName = c_szFontName;
		stFontName += ".fnt";

		CResourceManager& rkResMgr=CResourceManager::Instance();
		CResource* pkRes = rkResMgr.GetTypeResourcePointer(stFontName.c_str());
		CGraphicText* pkResFont=static_cast<CGraphicText*>(pkRes);
		m_TextInstance.SetTextPointer(pkResFont);
	}
	void CTextLine::SetFontColor(DWORD dwColor)
	{
		m_TextInstance.SetColor(dwColor);
	}
	void CTextLine::SetLimitWidth(float fWidth)
	{
		m_TextInstance.SetLimitWidth(fWidth);
	}
	void CTextLine::SetText(const char * c_szText)
	{
		OnSetText(c_szText);
	}
	void CTextLine::GetTextSize(int* pnWidth, int* pnHeight)
	{
		m_TextInstance.GetTextSize(pnWidth, pnHeight);
	}
	const char * CTextLine::GetText()
	{
		return m_TextInstance.GetValueStringReference().c_str();
	}
	void CTextLine::ShowCursor()
	{
		m_TextInstance.ShowCursor();
	}
	void CTextLine::HideCursor()
	{
		m_TextInstance.HideCursor();
	}
	bool CTextLine::IsShowCursor()
	{
		return m_TextInstance.IsShowCursor();
	}
	int CTextLine::GetCursorPosition()
	{
		long lx, ly;
		CWindow::GetMouseLocalPosition(lx, ly);
		return m_TextInstance.PixelPositionToCharacterPosition(lx);
	}

	void CTextLine::OnSetText(const char * c_szText)
	{
		m_TextInstance.SetValue(c_szText);
		m_TextInstance.Update();
#ifdef ENABLE_INGAME_WIKI
		if (m_isInsideRender)
		{
			UpdateRenderBoxRecursive();
		}
#endif
	}

#ifdef ENABLE_INGAME_WIKI
	bool CTextLine::IsShow()
	{
		if (!m_bShow)
		{
			return false;
		}

		if (m_isInsideRender)
		{
			int cW, cH;
			GetTextSize(&cW, &cH);
			if (m_renderBox.left + m_renderBox.right >= cW || m_renderBox.top + m_renderBox.bottom >= cH)
			{
				return false;
			}
		}

		return true;
	}
#endif

#if defined(ENABLE_CLIP_MASK)
	void CTextLine::OnUpdate()
	{
		if (!IsShow())
			return;

		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();

		m_TextInstance.Update();
	}
	void CTextLine::OnRender()
	{
		if (IsShow())
			m_TextInstance.Render(m_bEnableMask ? &m_rMaskRect : NULL);
	}
#else
	void CTextLine::OnUpdate()
	{
		if (IsShow())
			m_TextInstance.Update();
	}
	void CTextLine::OnRender()
	{
		if (IsShow())
			m_TextInstance.Render();
	}
#endif

	void CTextLine::OnChangePosition()
	{
		// FOR_ARABIC_ALIGN
		//if (m_TextInstance.GetHorizontalAlign() == CGraphicTextInstance::HORIZONTAL_ALIGN_ARABIC)
		if( GetDefaultCodePage() == CP_ARABIC )
		{
			m_TextInstance.SetPosition(m_rect.right, m_rect.top);
		}
		else
		{
			m_TextInstance.SetPosition(m_rect.left, m_rect.top);
		}
	}

#ifdef ENABLE_INGAME_WIKI
	int CTextLine::GetRenderingWidth()
	{
		int iTextWidth, iTextHeight;
		GetTextSize(&iTextWidth, &iTextHeight);

		return iTextWidth + m_renderingRect.right + m_renderingRect.left;
	}

	int CTextLine::GetRenderingHeight()
	{
		int iTextWidth, iTextHeight;
		GetTextSize(&iTextWidth, &iTextHeight);

		return iTextHeight + m_renderingRect.bottom + m_renderingRect.top;
	}

	void CTextLine::OnSetRenderingRect()
	{
		int iTextWidth, iTextHeight;
		GetTextSize(&iTextWidth, &iTextHeight);

		m_TextInstance.iSetRenderingRect(m_renderingRect.left, -m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CNumberLine::CNumberLine(PyObject * ppyObject) : CWindow(ppyObject)
	{
		m_strPath = "d:/ymir work/ui/game/taskbar/";
		m_iHorizontalAlign = HORIZONTAL_ALIGN_LEFT;
		m_dwWidthSummary = 0;
	}
	CNumberLine::CNumberLine(CWindow * pParent) : CWindow(NULL)
	{
		m_strPath = "d:/ymir work/ui/game/taskbar/";
		m_iHorizontalAlign = HORIZONTAL_ALIGN_LEFT;
		m_dwWidthSummary = 0;

		m_pParent = pParent;
	}
	CNumberLine::~CNumberLine()
	{
		ClearNumber();
	}

	void CNumberLine::SetPath(const char * c_szPath)
	{
		m_strPath = c_szPath;
	}
	void CNumberLine::SetHorizontalAlign(int iType)
	{
		m_iHorizontalAlign = iType;
	}
	void CNumberLine::SetNumber(const char * c_szNumber)
	{
		if (0 == m_strNumber.compare(c_szNumber))
			return;

		ClearNumber();

		m_strNumber = c_szNumber;

		for (DWORD i = 0; i < m_strNumber.size(); ++i)
		{
			char cChar = m_strNumber[i];
			std::string strImageFileName;

			if (':' == cChar)
			{
				strImageFileName = m_strPath + "colon.sub";
			}
			else if ('?' == cChar)
			{
				strImageFileName = m_strPath + "questionmark.sub";
			}
			else if ('/' == cChar)
			{
				strImageFileName = m_strPath + "slash.sub";
			}
			else if ('%' == cChar)
			{
				strImageFileName = m_strPath + "percent.sub";
			}
			else if ('+' == cChar)
			{
				strImageFileName = m_strPath + "plus.sub";
			}
			else if ('m' == cChar)
			{
				strImageFileName = m_strPath + "m.sub";
			}
			else if ('g' == cChar)
			{
				strImageFileName = m_strPath + "g.sub";
			}
			else if ('p' == cChar)
			{
				strImageFileName = m_strPath + "p.sub";
			}
			else if (cChar >= '0' && cChar <= '9')
			{
				strImageFileName = m_strPath;
				strImageFileName += cChar;
				strImageFileName += ".sub";
			}
			else
				continue;

			if (!CResourceManager::Instance().IsFileExist(strImageFileName.c_str()))
				continue;

			CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(strImageFileName.c_str());

			CGraphicImageInstance * pInstance = CGraphicImageInstance::New();
			pInstance->SetImagePointer(pImage);
			m_ImageInstanceVector.push_back(pInstance);

			m_dwWidthSummary += pInstance->GetWidth();
		}
	}

	void CNumberLine::ClearNumber()
	{
		m_ImageInstanceVector.clear();
		m_dwWidthSummary = 0;
		m_strNumber = "";
	}

#if defined(ENABLE_CLIP_MASK)
	void CNumberLine::OnUpdate()
	{
		if (!IsShow())
			return;

		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();

		CWindow::OnUpdate();
	}
#endif

	void CNumberLine::OnRender()
	{
		for (DWORD i = 0; i < m_ImageInstanceVector.size(); ++i)
		{
#if defined(ENABLE_CLIP_MASK)
			m_ImageInstanceVector[i]->Render(m_bEnableMask ? &m_rMaskRect : NULL);
#else
			m_ImageInstanceVector[i]->Render();
#endif
		}
	}

	void CNumberLine::OnChangePosition()
	{
		int ix = m_x;
		int iy = m_y;

		if (m_pParent)
		{
			ix = m_rect.left;
			iy = m_rect.top;
		}

		switch (m_iHorizontalAlign)
		{
			case HORIZONTAL_ALIGN_LEFT:
				break;
			case HORIZONTAL_ALIGN_CENTER:
				ix -= int(m_dwWidthSummary) / 2;
				break;
			case HORIZONTAL_ALIGN_RIGHT:
				ix -= int(m_dwWidthSummary);
				break;
		}

		for (DWORD i = 0; i < m_ImageInstanceVector.size(); ++i)
		{
			m_ImageInstanceVector[i]->SetPosition(ix, iy);
			ix += m_ImageInstanceVector[i]->GetWidth();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////


#ifdef ENABLE_RENDER_TARGET
	CUiRenderTarget::CUiRenderTarget(PyObject* ppyObject) : CWindow(ppyObject)
	{
		m_dwIndex = -1;
	}
#ifdef ENABLE_RENDER_TARGET_EX
	DWORD CUiRenderTarget::Type()
	{
		static DWORD s_dwType = GetCRC32("CUiRenderTarget", strlen("CUiRenderTarget"));
		return (s_dwType);
	}

	BOOL CUiRenderTarget::OnIsType(DWORD dwType)
	{
		if (CUiRenderTarget::Type() == dwType)
		{
			return TRUE;
		}

		return FALSE;
	}
#endif
	CUiRenderTarget::~CUiRenderTarget() = default;

	bool CUiRenderTarget::SetRenderTarget(uint8_t index)
	{
		if (!CRenderTargetManager::Instance().GetRenderTarget(index))
		{
			if (!CRenderTargetManager::Instance().CreateRenderTarget(index, GetWidth(), GetHeight()))
			{
				TraceError("CRenderTargetManager could not create the texture. w %d h %d", GetWidth(), GetHeight());
				return false;
			}
		}
		m_dwIndex = index;

		UpdateRect();
		return true;
	}

	void CUiRenderTarget::OnRender()
	{
		auto target = CRenderTargetManager::Instance().GetRenderTarget(m_dwIndex);
		if (!target)
		{
			TraceError("SetRenderingRect -> target empty!");
			return;
		}

		target->SetRenderingRect(&m_rect);
#ifdef ENABLE_CLIP_MASK
		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();

		target->RenderTexture(m_bEnableMask ? &m_rMaskRect : NULL);
#else
		target->RenderTexture();
#endif
	}
#endif


#ifdef ENABLE_INGAME_WIKI
	CUiWikiRenderTarget::~CUiWikiRenderTarget() = default;
	CUiWikiRenderTarget::CUiWikiRenderTarget(PyObject * ppyObject) :
		CWindow(ppyObject),
		m_dwIndex(-1) {}

	bool CUiWikiRenderTarget::SetWikiRenderTargetModule(int iRenderTargetModule)
	{
		if (!CWikiRenderTargetManager::Instance().GetRenderTarget(iRenderTargetModule))
		{
			if (!CWikiRenderTargetManager::Instance().CreateRenderTarget(iRenderTargetModule, GetWidth(), GetHeight()))
			{
				TraceError("CWikiRenderTargetManager could not create the texture. w %d h %d", GetWidth(), GetHeight());
				return false;
			}
		}

		m_dwIndex = iRenderTargetModule;

		UpdateRect();
		return true;
	}

	void CUiWikiRenderTarget::OnUpdateRenderBox()
	{
		if (m_dwIndex == -1)
			return;

		auto target = CWikiRenderTargetManager::Instance().GetRenderTarget(m_dwIndex);
		if (!target)
			return;

		target->SetRenderingBox(&m_renderBox);
	}

	void CUiWikiRenderTarget::OnRender()
	{
		if (m_dwIndex == -1)
			return;

		auto target = CWikiRenderTargetManager::Instance().GetRenderTarget(m_dwIndex);
		if (!target)
			return;

		target->SetRenderingRect(&m_rect);
		target->RenderTexture();
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CImageBox::CImageBox(PyObject * ppyObject) : CWindow(ppyObject)
	{
		m_pImageInstance = NULL;
#ifdef ENABLE_PLAYER_HP
		m_bIsShowImage = true;
		m_fDisplayProcent = -1.0f;
#endif
	}
	CImageBox::~CImageBox()
	{
		OnDestroyInstance();
	}

	void CImageBox::OnCreateInstance()
	{
		OnDestroyInstance();

		m_pImageInstance = CGraphicImageInstance::New();
	}
	void CImageBox::OnDestroyInstance()
	{
		if (m_pImageInstance)
		{
			CGraphicImageInstance::Delete(m_pImageInstance);
			m_pImageInstance=NULL;
		}
	}

	BOOL CImageBox::LoadImage(const char * c_szFileName)
	{
		if (!c_szFileName[0])
			return FALSE;

		OnCreateInstance();

		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource)
			return FALSE;
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_pImageInstance->IsEmpty())
			return FALSE;

		SetSize(m_pImageInstance->GetWidth(), m_pImageInstance->GetHeight());
		UpdateRect();

		return TRUE;
	}

	void CImageBox::SetDiffuseColor(float fr, float fg, float fb, float fa)
	{
		if (!m_pImageInstance)
			return;

		m_pImageInstance->SetDiffuseColor(fr, fg, fb, fa);
	}

	int CImageBox::GetWidth()
	{
		if (!m_pImageInstance)
			return 0;

		return m_pImageInstance->GetWidth();
	}

	int CImageBox::GetHeight()
	{
		if (!m_pImageInstance)
			return 0;

		return m_pImageInstance->GetHeight();
	}

	void CImageBox::OnUpdate()
	{
	}
	void CImageBox::OnRender()
	{
		if (!m_pImageInstance)
			return;

#ifdef ENABLE_PLAYER_HP
		if (IsShow() && m_bIsShowImage)
		{
			if (m_fDisplayProcent != -1.0f)
			{
				m_pImageInstance->DisplayImageProcent(m_fDisplayProcent);
			}
			else
			{
				m_pImageInstance->Render();
			}
		}
#else
		if (IsShow())
			m_pImageInstance->Render();
#endif
	}

	void CImageBox::OnChangePosition()
	{
		if (!m_pImageInstance)
			return;

		m_pImageInstance->SetPosition(m_rect.left, m_rect.top);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	CMarkBox::CMarkBox(PyObject * ppyObject) : CWindow(ppyObject)
	{
		m_pMarkInstance = NULL;
	}

	CMarkBox::~CMarkBox()
	{
		OnDestroyInstance();
	}

	void CMarkBox::OnCreateInstance()
	{
		OnDestroyInstance();
		m_pMarkInstance = CGraphicMarkInstance::New();
	}

	void CMarkBox::OnDestroyInstance()
	{
		if (m_pMarkInstance)
		{
			CGraphicMarkInstance::Delete(m_pMarkInstance);
			m_pMarkInstance=NULL;
		}
	}

	void CMarkBox::LoadImage(const char * c_szFilename)
	{
		OnCreateInstance();

		m_pMarkInstance->SetImageFileName(c_szFilename);
		m_pMarkInstance->Load();
		SetSize(m_pMarkInstance->GetWidth(), m_pMarkInstance->GetHeight());

		UpdateRect();
	}

	void CMarkBox::SetScale(FLOAT fScale)
	{
		if (!m_pMarkInstance)
			return;

		m_pMarkInstance->SetScale(fScale);
	}

	void CMarkBox::SetIndex(UINT uIndex)
	{
		if (!m_pMarkInstance)
			return;

		m_pMarkInstance->SetIndex(uIndex);
	}

	void CMarkBox::SetDiffuseColor(float fr, float fg, float fb, float fa)
	{
		if (!m_pMarkInstance)
			return;

		m_pMarkInstance->SetDiffuseColor(fr, fg, fb, fa);
	}

#if defined(ENABLE_CLIP_MASK)
	void CMarkBox::OnUpdate()
	{
		if (!m_pMarkInstance)
			return;

		if (!IsShow())
			return;

		if (!m_bEnableMask)
			return;

		if (!m_pMaskWindow)
			return;

		m_rMaskRect = m_pMaskWindow->GetRect();
	}

	void CMarkBox::OnRender()
	{
		if (!m_pMarkInstance)
			return;

		if (IsShow())
			m_pMarkInstance->Render(m_bEnableMask ? &m_rMaskRect : NULL);
	}
#else
	void CMarkBox::OnUpdate()
	{
	}

	void CMarkBox::OnRender()
	{
		if (!m_pMarkInstance)
			return;

		if (IsShow())
			m_pMarkInstance->Render();
	}
#endif

	void CMarkBox::OnChangePosition()
	{
		if (!m_pMarkInstance)
			return;

		m_pMarkInstance->SetPosition(m_rect.left, m_rect.top);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	DWORD CExpandedImageBox::Type()
	{
		static DWORD s_dwType = GetCRC32("CExpandedImageBox", strlen("CExpandedImageBox"));
		return (s_dwType);
	}

	BOOL CExpandedImageBox::OnIsType(DWORD dwType)
	{
		if (CExpandedImageBox::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	CExpandedImageBox::CExpandedImageBox(PyObject * ppyObject) : CImageBox(ppyObject)
	{
	}
	CExpandedImageBox::~CExpandedImageBox()
	{
		OnDestroyInstance();
	}

	void CExpandedImageBox::OnCreateInstance()
	{
		OnDestroyInstance();

		m_pImageInstance = CGraphicExpandedImageInstance::New();
	}
	void CExpandedImageBox::OnDestroyInstance()
	{
		if (m_pImageInstance)
		{
			CGraphicExpandedImageInstance::Delete((CGraphicExpandedImageInstance*)m_pImageInstance);
			m_pImageInstance=NULL;
		}
	}

	void CExpandedImageBox::SetScale(float fx, float fy)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetScale(fx, fy);
		CWindow::SetSize(long(float(GetWidth())*fx), long(float(GetHeight())*fy));
	}
	void CExpandedImageBox::SetOrigin(float fx, float fy)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetOrigin(fx, fy);
	}
	void CExpandedImageBox::SetRotation(float fRotation)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRotation(fRotation);
	}
	void CExpandedImageBox::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	}
	void CExpandedImageBox::SetImageClipRect(float fLeft, float fTop, float fRight, float fBottom, bool bIsVertical)
	{
		if (!m_pImageInstance)
			return;

		const RECT& c_rRect = GetRect();

		float fDifLeft = (c_rRect.left < fLeft) ? -(float(fLeft - c_rRect.left) / float(GetWidth())) : 0.0f;
		float fDifTop = (c_rRect.top < fTop) ? -(float(fTop - c_rRect.top) / float(GetHeight())) : 0.0f;
		float fDifRight = (c_rRect.right > fRight) ? -(float(c_rRect.right - fRight) / float(GetWidth())) : 0.0f;
		float fDifBottom = (c_rRect.bottom > fBottom) ? -(float(c_rRect.bottom - fBottom) / float(GetHeight())) : 0.0f;

		if (bIsVertical)
			((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRect(fLeft, fDifTop, fRight, fDifBottom);
		else
			((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRect(fDifLeft, fDifTop, fDifRight, fDifBottom);
	}

	void CExpandedImageBox::SetRenderingMode(int iMode)
	{
		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingMode(iMode);
	}

	void CExpandedImageBox::SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom)
	{
		if (!m_pImageInstance)
			return;
		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRectWithScale(fLeft, fTop, fRight, fBottom);
	}

#ifdef ENABLE_INGAME_WIKI
	int CExpandedImageBox::GetRenderingWidth()
	{
		return CWindow::GetWidth() + m_renderingRect.right + m_renderingRect.left;
	}

	int CExpandedImageBox::GetRenderingHeight()
	{
		return CWindow::GetHeight() + m_renderingRect.bottom + m_renderingRect.top;
	}

	void CExpandedImageBox::OnSetRenderingRect()
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
	}

	void CExpandedImageBox::SetExpandedRenderingRect(float fLeftTop, float fLeftBottom, float fTopLeft, float fTopRight, float fRightTop, float fRightBottom, float fBottomLeft, float fBottomRight)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetExpandedRenderingRect(fLeftTop, fLeftBottom, fTopLeft, fTopRight, fRightTop, fRightBottom, fBottomLeft, fBottomRight);
	}

	void CExpandedImageBox::SetTextureRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetTextureRenderingRect(fLeft, fTop, fRight, fBottom);
	}

	DWORD CExpandedImageBox::GetPixelColor(DWORD x, DWORD y)
	{
		return ((CGraphicExpandedImageInstance*)m_pImageInstance)->GetPixelColor(x, y);
	}

	void CExpandedImageBox::OnUpdateRenderBox()
	{
		if (!m_pImageInstance)
			return;

		((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderBox(m_renderBox);
	}
#endif

#if defined(ENABLE_CLIP_MASK)
	void CExpandedImageBox::OnUpdate()
	{
		if (!m_pImageInstance)
			return;

		if (!IsShow())
			return;

		if (!m_bEnableMask)
			return;

		if (!m_pMaskWindow)
			return;

		m_rMaskRect = m_pMaskWindow->GetRect();
	}
	void CExpandedImageBox::OnRender()
	{
		if (!m_pImageInstance)
			return;

		if (IsShow())
			m_pImageInstance->Render(m_bEnableMask ? &m_rMaskRect : NULL);
	}
#else
	void CExpandedImageBox::OnUpdate()
	{
	}
	void CExpandedImageBox::OnRender()
	{
		if (!m_pImageInstance)
			return;

		if (IsShow())
			m_pImageInstance->Render();
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	DWORD CAniImageBox::Type()
	{
		static DWORD s_dwType = GetCRC32("CAniImageBox", strlen("CAniImageBox"));
		return (s_dwType);
	}

	BOOL CAniImageBox::OnIsType(DWORD dwType)
	{
		if (CAniImageBox::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	CAniImageBox::CAniImageBox(PyObject * ppyObject)
		:	CWindow(ppyObject),
			m_bycurDelay(0),
			m_byDelay(4),
			m_bycurIndex(0)
			, m_fScaleX(1)
			, m_fScaleY(1)
	{
		m_ImageVector.clear();
	}
	CAniImageBox::~CAniImageBox()
	{
		for_each(m_ImageVector.begin(), m_ImageVector.end(), CGraphicExpandedImageInstance::DeleteExpandedImageInstance);
	}


#ifdef ENABLE_HIGHLIGHT_SLOT_SYSTEM_EX
	struct FSetDiffuseColor
	{
		float fr, fg, fb, fa;
		void operator () (CGraphicExpandedImageInstance * pInstance)
		{
			pInstance->SetDiffuseColor(fr, fg, fb, fa);
		}
	};
	void CAniImageBox::SetDiffuseColor(float fR, float fG, float fB, float fA)
	{
		FSetDiffuseColor setDiffuseColor;
		setDiffuseColor.fr = fR;
		setDiffuseColor.fg = fG;
		setDiffuseColor.fb = fB;
		setDiffuseColor.fa = fA;
		for_each(m_ImageVector.begin(), m_ImageVector.end(), setDiffuseColor);
	}
	void CAniImageBox::SetDiffuseColorByType(BYTE byColorType)
	{
		float fr = 1.0f, fg = 1.0f, fb = 1.0f, fa = 1.0f;
		switch (byColorType)
		{
		case COLOR_TYPE_ORANGE:
			fr = 1.0f;
			fg = 0.34509805f;
			fb = 0.035294119f;
			fa = 0.5f;
			break;
		case COLOR_TYPE_WHITE:
			fr = 1.0f;
			fg = 1.0f;
			fb = 1.0f;
			fa = 0.5f;
			break;
		case COLOR_TYPE_RED:
			fr = 1.0f;
			fg = 0.0f;
			fb = 0.0f;
			fa = 0.5f;
			break;
		case COLOR_TYPE_GREEN:
			fr = 0.0f;
			fg = 1.0f;
			fb = 0.0f;
			fa = 0.5f;
			break;
		case COLOR_TYPE_YELLOW:
			fr = 1.0f;
			fg = 1.0f;
			fb = 0.0f;
			fa = 0.5f;
			break;
		case COLOR_TYPE_SKY:
			fr = 0.0f;
			fg = 1.0f;
			fb = 1.0f;
			fa = 0.5f;
			break;
		case COLOR_TYPE_PINK:
			fr = 1.0f;
			fg = 0.0f;
			fb = 1.0f;
			fa = 0.5f;
			break;
		case COLOR_TYPE_VIOLET:
			fr = 0.69803921f;
			fg = 0.0f;
			fb = 1.0f;
			fa = 1.0f;
			break;
		default:
			return;
			break;
		}
		SetDiffuseColor(fr, fg, fb, fa);
	}
#endif

	void CAniImageBox::SetDelay(int iDelay)
	{
		m_byDelay = iDelay;
	}

	void CAniImageBox::AppendImage(const char* c_szFileName)
	{
		CResource* pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return;

		CGraphicExpandedImageInstance* pImageInstance = CGraphicExpandedImageInstance::New();
		pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));

		if (pImageInstance->IsEmpty())
		{
			CGraphicExpandedImageInstance::Delete(pImageInstance);
			return;
		}

		pImageInstance->SetScale(m_fScaleX, m_fScaleY);

		m_ImageVector.push_back(pImageInstance);
		m_bycurIndex = rand() % m_ImageVector.size();
	}

	struct FSetRenderingRect
	{
		float fLeft, fTop, fRight, fBottom;
		void operator () (CGraphicExpandedImageInstance * pInstance)
		{
			pInstance->SetRenderingRect(fLeft, fTop, fRight, fBottom);
		}
	};
	void CAniImageBox::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
	{
		FSetRenderingRect setRenderingRect;
		setRenderingRect.fLeft = fLeft;
		setRenderingRect.fTop = fTop;
		setRenderingRect.fRight = fRight;
		setRenderingRect.fBottom = fBottom;
		for_each(m_ImageVector.begin(), m_ImageVector.end(), setRenderingRect);
	}

	struct FSetRenderingRectWithScale
	{
		float fLeft, fTop, fRight, fBottom;
		void operator () (CGraphicExpandedImageInstance* pInstance)
		{
			pInstance->SetRenderingRectWithScale(fLeft, fTop, fRight, fBottom);
		}
	};

	void CAniImageBox::SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom)
	{
		FSetRenderingRectWithScale setRenderingRectWithScale;
		setRenderingRectWithScale.fLeft = fLeft;
		setRenderingRectWithScale.fTop = fTop;
		setRenderingRectWithScale.fRight = fRight;
		setRenderingRectWithScale.fBottom = fBottom;
		for_each(m_ImageVector.begin(), m_ImageVector.end(), setRenderingRectWithScale);
	}

	struct FSetRenderingMode
	{
		int iMode;
		void operator () (CGraphicExpandedImageInstance * pInstance)
		{
			pInstance->SetRenderingMode(iMode);
		}
	};
	void CAniImageBox::SetRenderingMode(int iMode)
	{
		FSetRenderingMode setRenderingMode;
		setRenderingMode.iMode = iMode;
		for_each(m_ImageVector.begin(), m_ImageVector.end(), setRenderingMode);
	}

	struct FSetAniImgScale
	{
		float fScaleX, fScaleY;
		void operator () (CGraphicExpandedImageInstance* pInstance)
		{
			pInstance->SetScale(fScaleX, fScaleY);
		}
	};
	void CAniImageBox::SetAniImgScale(float fScaleX, float fScaleY)
	{
		m_fScaleX = fScaleX;
		m_fScaleY = fScaleY;

		FSetAniImgScale setAniImgScale;
		setAniImgScale.fScaleX = m_fScaleX;
		setAniImgScale.fScaleY = m_fScaleY;

		for_each(m_ImageVector.begin(), m_ImageVector.end(), setAniImgScale);
	}

	void CAniImageBox::ResetFrame()
	{
		m_bycurIndex = 0;
	}

	void CAniImageBox::OnUpdate()
	{
#if defined(ENABLE_CLIP_MASK)
		if (m_bEnableMask && m_pMaskWindow)
			m_rMaskRect = m_pMaskWindow->GetRect();
#endif
		++m_bycurDelay;
		if (m_bycurDelay < m_byDelay)
			return;

		m_bycurDelay = 0;

		++m_bycurIndex;
		if (m_bycurIndex >= m_ImageVector.size())
		{
			m_bycurIndex = 0;

			OnEndFrame();
		}
	}
	void CAniImageBox::OnRender()
	{
		if (m_bycurIndex < m_ImageVector.size())
		{
			CGraphicExpandedImageInstance * pImage = m_ImageVector[m_bycurIndex];
#if defined(ENABLE_CLIP_MASK)
			pImage->Render(m_bEnableMask ? &m_rMaskRect : NULL);
#else
			pImage->Render();
#endif
		}
	}

	struct FChangePosition
	{
		float fx, fy;
		void operator () (CGraphicExpandedImageInstance * pInstance)
		{
			pInstance->SetPosition(fx, fy);
		}
	};

	void CAniImageBox::OnChangePosition()
	{
		FChangePosition changePosition;
		changePosition.fx = m_rect.left;
		changePosition.fy = m_rect.top;
		for_each(m_ImageVector.begin(), m_ImageVector.end(), changePosition);
	}

	void CAniImageBox::OnEndFrame()
	{
		PyCallClassMemberFunc(m_poHandler, "OnEndFrame", BuildEmptyTuple());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	// gif support
	uint32_t CGifImageBox::Type()
	{
		static uint32_t s_dwType = GetCRC32("CGifImageBox", strlen("CGifImageBox"));
		return (s_dwType);
	}

	BOOL CGifImageBox::OnIsType(uint32_t dwType)
	{
		if (CGifImageBox::Type() == dwType)
			return TRUE;

		return FALSE;
	}

	CGifImageBox::CGifImageBox(PyObject* ppyObject)
		: CAniImageBox(ppyObject)
	{
	}
	CGifImageBox::~CGifImageBox()
	{
	}

	bool RawFrameToBmpImage(SavedImage* currentFrame, GifFileType* gifFile, std::vector <uint8_t>& outData)
	{
		if (!currentFrame || !gifFile)
		{
			TraceError("RawFrameToBmpImage: Invalid parameters.");
			return false;
		}

		auto& imageDesc = currentFrame->ImageDesc;
		auto* colorMap = (imageDesc.ColorMap) ? imageDesc.ColorMap : gifFile->SColorMap;
		if (!colorMap)
		{
			TraceError("RawFrameToBmpImage: ColorMap is null.");
			return false;
		}

		const int width = imageDesc.Width;
		const int height = imageDesc.Height;
		const int rowPadding = (4 - (width * 3 % 4)) % 4;
		const std::size_t rowDataSize = static_cast<std::size_t>(width * 3 + rowPadding) * height;

		BITMAPFILEHEADER fileHeader = {
			0x4D42, // "BM"
			static_cast<uint32_t>(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + rowDataSize),
			0, // Reserved1
			0, // Reserved2
			sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
		};

		BITMAPINFOHEADER infoHeader = {
			sizeof(BITMAPINFOHEADER),
			width,
			height,
			1, // Planes
			24, // BitCount
			0, // Compression
			static_cast<uint32_t>(rowDataSize),
			0, // XPelsPerMeter
			0, // YPelsPerMeter
			0, // ClrUsed
			0  // ClrImportant
		};

		outData.resize(fileHeader.bfSize);
		std::memcpy(outData.data(), &fileHeader, sizeof(fileHeader));
		std::memcpy(outData.data() + sizeof(fileHeader), &infoHeader, sizeof(infoHeader));

		auto pixelData = outData.begin() + fileHeader.bfOffBits;
		for (int y = height - 1; y >= 0; --y)
		{
			for (int x = 0; x < width; ++x)
			{
				const int index = y * width + x;
				const GifByteType colorIndex = currentFrame->RasterBits[index];
				const GifColorType& gifColor = colorMap->Colors[colorIndex];

				*pixelData++ = gifColor.Blue;
				*pixelData++ = gifColor.Green;
				*pixelData++ = gifColor.Red;
			}
			pixelData = std::fill_n(pixelData, rowPadding, 0x00);
		}
	}

	bool RawFrameToTGAImage(SavedImage* currentFrame, GifFileType* gifFile, std::vector <uint8_t>& outData)
	{
		if (!currentFrame || !gifFile)
		{
			TraceError("RawFrameToTGAImage: Invalid parameters.");
			return false;
		}

		auto& imageDesc = currentFrame->ImageDesc;
		auto* colorMap = (imageDesc.ColorMap) ? imageDesc.ColorMap : gifFile->SColorMap;
		if (!colorMap)
		{
			TraceError("RawFrameToTGAImage: ColorMap is null.");
			return false;
		}

		const int width = imageDesc.Width;
		const int height = imageDesc.Height;

		// TGA Header
		std::array<uint8_t, 18> header = { 0 };
		header[2] = 2; // Uncompressed RGB
		header[12] = width & 0xFF;
		header[13] = (width >> 8) & 0xFF;
		header[14] = height & 0xFF;
		header[15] = (height >> 8) & 0xFF;
		header[16] = 32; // 32 bits per pixel

		outData.insert(outData.end(), header.begin(), header.end());

		// Get transparency information from the GIF
		GraphicsControlBlock gcb;
		DGifSavedExtensionToGCB(gifFile, currentFrame - gifFile->SavedImages, &gcb);

		for (int y = height - 1; y >= 0; --y)
		{
			for (int x = 0; x < width; ++x)
			{
				const int index = y * width + x;
				const GifByteType colorIndex = currentFrame->RasterBits[index];
				if (colorIndex < colorMap->ColorCount)
				{
					const GifColorType& gifColor = colorMap->Colors[colorIndex];
					outData.push_back(gifColor.Blue);
					outData.push_back(gifColor.Green);
					outData.push_back(gifColor.Red);
					// If the pixel is transparent in the GIF, make it transparent in the TGA
					outData.push_back((gcb.TransparentColor && colorIndex == gcb.TransparentColor) ? 0x00 : 0xFF);
				}
				else
				{
					TraceError("RawFrameToTGAImage: Color index out of range.");
					return false;
				}
			}
		}

		return true;
	}

	int CustomReadFunc(GifFileType* gifFile, GifByteType* buffer, int length)
	{
		std::istringstream* stream = reinterpret_cast<std::istringstream*>(gifFile->UserData);
		stream->read(reinterpret_cast<char*>(buffer), length);
		return stream->gcount(); // Return number of bytes read
	}

	bool CGifImageBox::LoadGif(const char* c_szFileName)
	{
		if (!c_szFileName || !*c_szFileName)
			return false;

		// Open GIF file
		int error;
		CMappedFile file;
		LPCVOID pData;
		CEterPackManager::instance().Get(file, c_szFileName, &pData);
		std::istringstream gifStream(std::string((char*)pData, file.Size()));
		GifFileType* gif = DGifOpen(&gifStream, CustomReadFunc, &error);
		if (!gif)
		{
			const char* c_szError = GifErrorString(error);
			TraceError("Failed to open GIF file: %s with error: %s", c_szFileName, c_szError);
			return false;
		}

		// Read the GIF into memory
		if (DGifSlurp(gif) == GIF_ERROR)
		{
			TraceError("Failed to read GIF file: %s", c_szFileName);
			DGifCloseFile(gif, &error);
			return false;
		}

		// Iterate through the frames
		for (int i = 0; i < gif->ImageCount; ++i)
		{
			// Get the current frame
			SavedImage* currentImage = &gif->SavedImages[i];

			// Convert the frame to TGA image
			std::vector <uint8_t> tgaBuffer;
			if (!RawFrameToTGAImage(currentImage, gif, tgaBuffer))
			{
				TraceError("Failed to convert GIF(%s) frame #%d to TGA image.", c_szFileName, i);
				continue;
			}

			// Create name for the frame
			char frameFileName[256]{ '\0' };
			snprintf(frameFileName, sizeof(frameFileName), "gif_frame%d_%s", i, c_szFileName);

			// Create the image instance
			CGraphicImage* pImage = new CGraphicImage(frameFileName, tgaBuffer.data(), tgaBuffer.size(), D3DX_FILTER_LINEAR);
			if (!pImage)
			{
				TraceError("Failed to create image instance for GIF(%s) frame #%d.", c_szFileName, i);
				continue;
			}

			// Create the expanded image instance
			CGraphicExpandedImageInstance* pImageInstance = CGraphicExpandedImageInstance::New();
			pImageInstance->SetImagePointer(pImage);

			// Add the image instance to the vector if it's not empty
			if (pImageInstance->IsEmpty())
			{
				CGraphicExpandedImageInstance::Delete(pImageInstance);
			}
			else
			{
				pImageInstance->SetPosition(m_rect.left, m_rect.top);
				m_ImageVector.push_back(pImageInstance);
			}
		}

		// Clean up
		DGifCloseFile(gif, &error);

		return true;
	}

	bool CGifImageBox::UnloadGif()
	{
		for (auto pImage : m_ImageVector)
			CGraphicExpandedImageInstance::Delete(pImage);

		m_ImageVector.clear();
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CButton::CButton(PyObject * ppyObject)
		:	CWindow(ppyObject),
			m_pcurVisual(NULL),
			m_bEnable(TRUE),
			m_isPressed(FALSE),
			m_isFlash(FALSE)
			, m_isFlashEnable(FALSE)
			, m_fScaleX(1.0f)
			, m_fScaleY(1.0f)
	{
		CWindow::AddFlag(CWindow::FLAG_NOT_CAPTURE);
	}
	CButton::~CButton()
	{
	}

	BOOL CButton::SetUpVisual(const char * c_szFileName)
	{
		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_upVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_upVisual.IsEmpty())
			return FALSE;

		SetSize(m_upVisual.GetWidth(), m_upVisual.GetHeight());

		SetCurrentVisual(&m_upVisual);

		return TRUE;
	}
	BOOL CButton::SetOverVisual(const char * c_szFileName)
	{
		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_overVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_overVisual.IsEmpty())
			return FALSE;

		SetSize(m_overVisual.GetWidth(), m_overVisual.GetHeight());

		return TRUE;
	}
	BOOL CButton::SetDownVisual(const char * c_szFileName)
	{
		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_downVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_downVisual.IsEmpty())
			return FALSE;

		SetSize(m_downVisual.GetWidth(), m_downVisual.GetHeight());

		return TRUE;
	}
	BOOL CButton::SetDisableVisual(const char * c_szFileName)
	{
		CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
		if (!pResource->IsType(CGraphicImage::Type()))
			return FALSE;

		m_disableVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
		if (m_downVisual.IsEmpty())
			return FALSE;

		SetSize(m_disableVisual.GetWidth(), m_disableVisual.GetHeight());

		return TRUE;
	}
	void CButton::SetScale(float fx, float fy)
	{
		m_fScaleX = fx;
		m_fScaleY = fy;

		if (m_pcurVisual)
			m_pcurVisual->SetScale(fx, fy);

		if (!m_upVisual.IsEmpty())
			m_upVisual.SetScale(fx, fy);

		if (!m_overVisual.IsEmpty())
			m_overVisual.SetScale(fx, fy);

		if (!m_downVisual.IsEmpty())
			m_downVisual.SetScale(fx, fy);

		if (!m_disableVisual.IsEmpty())
			m_disableVisual.SetScale(fx, fy);
	}

	// void CButton::SetScale(float fx, float fy)
	// {
	// 	if (m_pcurVisual)
	// 		m_pcurVisual->SetScale(fx, fy);

	// 	if (!m_upVisual.IsEmpty())
	// 		m_upVisual.SetScale(fx, fy);

	// 	if (!m_overVisual.IsEmpty())
	// 		m_overVisual.SetScale(fx, fy);

	// 	if (!m_downVisual.IsEmpty())
	// 		m_downVisual.SetScale(fx, fy);

	// 	if (!m_disableVisual.IsEmpty())
	// 		m_disableVisual.SetScale(fx, fy);

	// 	const float width = static_cast<float>(GetWidth()) * fx;
	// 	const float height = static_cast<float>(GetHeight()) * fy;

	// 	SetSize(static_cast<long>(width), static_cast<long>(height));
	// }

	void CButton::SetDiffuseColor(float fr, float fg, float fb, float fa)
	{
		if (m_pcurVisual)
			m_pcurVisual->SetDiffuseColor(fr, fg, fb, fa);

		if (!m_upVisual.IsEmpty())
			m_upVisual.SetDiffuseColor(fr, fg, fb, fa);

		if (!m_overVisual.IsEmpty())
			m_overVisual.SetDiffuseColor(fr, fg, fb, fa);

		if (!m_downVisual.IsEmpty())
			m_downVisual.SetDiffuseColor(fr, fg, fb, fa);

		if (!m_disableVisual.IsEmpty())
			m_disableVisual.SetDiffuseColor(fr, fg, fb, fa);
	}

	const char * CButton::GetUpVisualFileName()
	{
		return m_upVisual.GetGraphicImagePointer()->GetFileName();
	}
	const char * CButton::GetOverVisualFileName()
	{
		return m_overVisual.GetGraphicImagePointer()->GetFileName();
	}
	const char * CButton::GetDownVisualFileName()
	{
		return m_downVisual.GetGraphicImagePointer()->GetFileName();
	}

	void CButton::Flash()
	{
		m_isFlash = TRUE;
	}

	void CButton::EnableFlash()
	{
		m_isFlashEnable = TRUE;
	}

	void CButton::DisableFlash()
	{
		m_isFlashEnable = FALSE;
	}

#ifdef ENABLE_RENEWAL_DEAD_PACKET
	void CButton::Enable()
	{
		if (!IsDisable())
		{
			SetUp();
			m_bEnable = TRUE;
		}
	}

	void CButton::Disable()
	{
		if (IsDisable())
		{
			m_bEnable = FALSE;
			if (!m_disableVisual.IsEmpty())
			{
				SetCurrentVisual(&m_disableVisual);
			}
		}
	}
#else
	void CButton::Enable()
	{
		SetUp();
		m_bEnable = TRUE;
	}

	void CButton::Disable()
	{
		m_bEnable = FALSE;
		if (!m_disableVisual.IsEmpty())
			SetCurrentVisual(&m_disableVisual);
	}
#endif

	BOOL CButton::IsDisable()
	{
		return m_bEnable;
	}

	void CButton::SetUp()
	{
		SetCurrentVisual(&m_upVisual);
		m_isPressed = FALSE;
#if defined(__BL_MULTI_LANGUAGE__)
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
#endif
	}
	void CButton::Up()
	{
		if (IsIn())
			SetCurrentVisual(&m_overVisual);
		else
			SetCurrentVisual(&m_upVisual);

		PyCallClassMemberFunc(m_poHandler, "CallEvent", BuildEmptyTuple());
	}
	void CButton::Over()
	{
		SetCurrentVisual(&m_overVisual);
#if defined(__BL_MULTI_LANGUAGE__)
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
#endif
	}
	void CButton::Down()
	{
		m_isPressed = TRUE;
		SetCurrentVisual(&m_downVisual);
		PyCallClassMemberFunc(m_poHandler, "DownEvent", BuildEmptyTuple());
	}

#if defined(ENABLE_CLIP_MASK)
	void CButton::OnUpdate()
	{
		if (!m_bEnableMask)
			return;

		if (!m_pMaskWindow)
			return;

		m_rMaskRect = m_pMaskWindow->GetRect();
	}
	void CButton::OnRender()
	{
		if (!IsShow())
			return;

		if (m_pcurVisual)
		{
			if (m_isFlashEnable && !IsIn())
			{
				if (int(timeGetTime() / 500) % 2)
				{
					SetCurrentVisual(&m_upVisual);
				}
				else
				{
					SetCurrentVisual(&m_overVisual);
				}
			}

			if (m_isFlash)
			if (!IsIn())
			if (int(timeGetTime() / 500) % 2)
			{
				return;
			}
			m_pcurVisual->Render(m_bEnableMask ? &m_rMaskRect : NULL);
		}

		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
	}
#else
	void CButton::OnUpdate()
	{
	}
	void CButton::OnRender()
	{
		if (!IsShow())
			return;

		if (m_pcurVisual)
		{
			if (m_isFlash)
				if (!IsIn())
					if (int(timeGetTime() / 500) % 2)
					{
						return;
					}

			m_pcurVisual->Render();
		}

		PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
	}
#endif

	void CButton::OnChangePosition()
	{
		if (m_pcurVisual)
			m_pcurVisual->SetPosition(m_rect.left, m_rect.top);
	}

	BOOL CButton::OnMouseLeftButtonDown()
	{
		if (!IsEnable())
			return TRUE;

		m_isPressed = TRUE;
		Down();

		return TRUE;
	}
	BOOL CButton::OnMouseLeftButtonDoubleClick()
	{
		if (!IsEnable())
			return TRUE;

		OnMouseLeftButtonDown();

		return TRUE;
	}
	BOOL CButton::OnMouseLeftButtonUp()
	{
		if (!IsEnable())
			return TRUE;
		if (!IsPressed())
			return TRUE;

		m_isPressed = FALSE;
		Up();

		return TRUE;
	}
	void CButton::OnMouseOverIn()
	{
		if (!IsEnable())
			return;

		Over();
		PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
#if defined(ENABLE_DETAILS_UI)
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
#endif
	}
	void CButton::OnMouseOverOut()
	{
		if (!IsEnable())
			return;

		SetUp();
		PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
#if defined(ENABLE_DETAILS_UI)
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
#endif
	}
#ifdef ENABLE_NEW_GAMEOPTION
	DWORD CButton::Type()
	{
		static DWORD s_dwType = GetCRC32("CButton", strlen("CButton"));
		return (s_dwType);
	}
	BOOL CButton::OnIsType(DWORD dwType)
	{
		if (CButton::Type() == dwType)
			return TRUE;
		return FALSE;
	}

	void CButton::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
	{
		m_upVisual.SetRenderingRect(fLeft, fTop, fRight, fBottom);
		m_overVisual.SetRenderingRect(fLeft, fTop, fRight, fBottom);
		m_downVisual.SetRenderingRect(fLeft, fTop, fRight, fBottom);
		m_disableVisual.SetRenderingRect(fLeft, fTop, fRight, fBottom);
	}
#endif

#ifdef ENABLE_INGAME_WIKI
	void CButton::SetCurrentVisual(CGraphicExpandedImageInstance* pVisual)
#else
	void CButton::SetCurrentVisual(CGraphicImageInstance * pVisual)
#endif
	{
		m_pcurVisual = pVisual;
		m_pcurVisual->SetPosition(m_rect.left, m_rect.top);
#ifdef ENABLE_INGAME_WIKI
		if (m_pcurVisual == &m_upVisual)
			PyCallClassMemberFunc(m_poHandler, "OnSetUpVisual", BuildEmptyTuple());
		else if (m_pcurVisual == &m_overVisual)
			PyCallClassMemberFunc(m_poHandler, "OnSetOverVisual", BuildEmptyTuple());
		else if (m_pcurVisual == &m_downVisual)
			PyCallClassMemberFunc(m_poHandler, "OnSetDownVisual", BuildEmptyTuple());
		else if (m_pcurVisual == &m_disableVisual)
			PyCallClassMemberFunc(m_poHandler, "OnSetDisableVisual", BuildEmptyTuple());
#endif
}

	BOOL CButton::IsEnable()
	{
		return m_bEnable;
	}

	BOOL CButton::IsPressed()
	{
		return m_isPressed;
	}

#ifdef ENABLE_INGAME_WIKI
	void CButton::OnSetRenderingRect()
	{
		m_upVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
		m_overVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
		m_downVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
		m_disableVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
	}
#endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CRadioButton::CRadioButton(PyObject * ppyObject) : CButton(ppyObject)
	{
	}
	CRadioButton::~CRadioButton()
	{
	}

#ifdef ENABLE_NEW_GAMEOPTION
	DWORD CRadioButton::Type()
	{
		static DWORD s_dwType = GetCRC32("CRadioButton", strlen("CRadioButton"));
		return (s_dwType);
	}
	BOOL CRadioButton::OnIsType(DWORD dwType)
	{
		if (CRadioButton::Type() == dwType)
			return TRUE;
		return FALSE;
	}
#endif

	BOOL CRadioButton::OnMouseLeftButtonDown()
	{
		if (!IsEnable())
			return TRUE;

		if (!m_isPressed)
		{
			Down();
			PyCallClassMemberFunc(m_poHandler, "CallEvent", BuildEmptyTuple());
		}

		return TRUE;
	}
	BOOL CRadioButton::OnMouseLeftButtonUp()
	{
		return TRUE;
	}
	void CRadioButton::OnMouseOverIn()
	{
		if (!IsEnable())
			return;

		if (!m_isPressed)
		{
			SetCurrentVisual(&m_overVisual);
		}

		PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
	}
	void CRadioButton::OnMouseOverOut()
	{
		if (!IsEnable())
			return;

		if (!m_isPressed)
		{
			SetCurrentVisual(&m_upVisual);
		}

		PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CToggleButton::CToggleButton(PyObject * ppyObject) : CButton(ppyObject)
	{
	}
	CToggleButton::~CToggleButton()
	{
	}

#ifdef ENABLE_NEW_GAMEOPTION
	DWORD CToggleButton::Type()
	{
		static DWORD s_dwType = GetCRC32("CToggleButton", strlen("CToggleButton"));
		return (s_dwType);
	}
	BOOL CToggleButton::OnIsType(DWORD dwType)
	{
		if (CToggleButton::Type() == dwType)
			return TRUE;
		return FALSE;
	}
#endif

	BOOL CToggleButton::OnMouseLeftButtonDown()
	{
		if (!IsEnable())
			return TRUE;

		if (m_isPressed)
		{
			SetUp();
			if (IsIn())
				SetCurrentVisual(&m_overVisual);
			else
				SetCurrentVisual(&m_upVisual);
			PyCallClassMemberFunc(m_poHandler, "OnToggleUp", BuildEmptyTuple());
		}
		else
		{
			Down();
			PyCallClassMemberFunc(m_poHandler, "OnToggleDown", BuildEmptyTuple());
		}

		return TRUE;
	}
	BOOL CToggleButton::OnMouseLeftButtonUp()
	{
		return TRUE;
	}

	void CToggleButton::OnMouseOverIn()
	{
		if (!IsEnable())
			return;

		if (!m_isPressed)
		{
			SetCurrentVisual(&m_overVisual);
		}

		PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
	}
	void CToggleButton::OnMouseOverOut()
	{
		if (!IsEnable())
			return;

		if (!m_isPressed)
		{
			SetCurrentVisual(&m_upVisual);
		}

		PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	CDragButton::CDragButton(PyObject * ppyObject) : CButton(ppyObject)
	{
		CWindow::RemoveFlag(CWindow::FLAG_NOT_CAPTURE);
		m_restrictArea.left = 0;
		m_restrictArea.top = 0;
		m_restrictArea.right = CWindowManager::Instance().GetScreenWidth();
		m_restrictArea.bottom = CWindowManager::Instance().GetScreenHeight();
	}
	CDragButton::~CDragButton()
	{
	}

	void CDragButton::SetRestrictMovementArea(int ix, int iy, int iwidth, int iheight)
	{
		m_restrictArea.left = ix;
		m_restrictArea.top = iy;
		m_restrictArea.right = ix + iwidth;
		m_restrictArea.bottom = iy + iheight;
	}

	void CDragButton::OnChangePosition()
	{
		m_x = max(m_x, m_restrictArea.left);
		m_y = max(m_y, m_restrictArea.top);
		m_x = min(m_x, max(0, m_restrictArea.right - m_lWidth));
		m_y = min(m_y, max(0, m_restrictArea.bottom - m_lHeight));

		m_rect.left = m_x;
		m_rect.top = m_y;

		if (m_pParent)
		{
			const RECT & c_rRect = m_pParent->GetRect();
			m_rect.left += c_rRect.left;
			m_rect.top += c_rRect.top;
		}

		m_rect.right = m_rect.left + m_lWidth;
		m_rect.bottom = m_rect.top + m_lHeight;

		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::UpdateRect));

		if (m_pcurVisual)
			m_pcurVisual->SetPosition(m_rect.left, m_rect.top);

		if (IsPressed())
			PyCallClassMemberFunc(m_poHandler, "OnMove", BuildEmptyTuple());
	}

	void CDragButton::OnMouseOverIn()
	{
		if (!IsEnable())

			return;

		CButton::OnMouseOverIn();
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
	}

	void CDragButton::OnMouseOverOut()
	{
		if (!IsEnable())
			return;

		CButton::OnMouseOverIn();
		PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
	}
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
