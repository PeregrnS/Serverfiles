# -*- coding: utf-8 -*-
import app
import ime
import grp
import snd
import wndMgr
import item
import skill
import localeInfo
import dbg
# MARK_BUG_FIX
import guild
# END_OF_MARK_BUG_FIX
import constInfo
import sys
if app.ENABLE_RENDER_TARGET_EX:
	import renderTarget

from _weakref import proxy

BACKGROUND_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 1.0)
DARK_COLOR = grp.GenerateColor(0.2, 0.2, 0.2, 1.0)
BRIGHT_COLOR = grp.GenerateColor(0.7, 0.7, 0.7, 1.0)
INVISIBLE_COLOR = grp.GenerateColor(1.0, 1.0, 1.0, 0.0)

if localeInfo.IsCANADA():
	SELECT_COLOR = grp.GenerateColor(0.9, 0.03, 0.01, 0.4)
else:
	SELECT_COLOR = grp.GenerateColor(0.0, 0.0, 0.5, 0.3)

WHITE_COLOR = grp.GenerateColor(1.0, 1.0, 1.0, 0.5)
HALF_WHITE_COLOR = grp.GenerateColor(1.0, 1.0, 1.0, 0.2)

createToolTipWindowDict = {}
def RegisterCandidateWindowClass(codePage, candidateWindowClass):
	EditLine.candidateWindowClassDict[codePage]=candidateWindowClass
def RegisterToolTipWindow(type, createToolTipWindow):
	createToolTipWindowDict[type]=createToolTipWindow

app.SetDefaultFontName(localeInfo.UI_DEF_FONT)

## Window Manager Event List##
##############################
## "OnMouseLeftButtonDown"
## "OnMouseLeftButtonUp"
## "OnMouseLeftButtonDoubleClick"
## "OnMouseRightButtonDown"
## "OnMouseRightButtonUp"
## "OnMouseRightButtonDoubleClick"
## "OnMouseDrag"
## "OnSetFocus"
## "OnKillFocus"
## "OnMouseOverIn"
## "OnMouseOverOut"
## "OnRender"
## "OnUpdate"
## "OnKeyDown"
## "OnKeyUp"
## "OnTop"
## "OnIMEUpdate" ## IME Only
## "OnIMETab"	## IME Only
## "OnIMEReturn" ## IME Only
##############################
## Window Manager Event List##


class __mem_func__:
	class __noarg_call__:
		def __init__(self, cls, obj, func):
			self.cls=cls
			self.obj=proxy(obj)
			self.func=proxy(func)

		def __call__(self, *arg):
			return self.func(self.obj)

	class __arg_call__:
		def __init__(self, cls, obj, func):
			self.cls=cls
			self.obj=proxy(obj)
			self.func=proxy(func)

		def __call__(self, *arg):
			return self.func(self.obj, *arg)

	def __init__(self, mfunc):
		if mfunc.im_func.func_code.co_argcount>1:
			self.call=__mem_func__.__arg_call__(mfunc.im_class, mfunc.im_self, mfunc.im_func)
		else:
			self.call=__mem_func__.__noarg_call__(mfunc.im_class, mfunc.im_self, mfunc.im_func)

	def __call__(self, *arg):
		return self.call(*arg)


class Window(object):
	def NoneMethod(cls):
		pass

	NoneMethod = classmethod(NoneMethod)

	def __init__(self, layer = "UI"):
		self.hWnd = None
		self.parentWindow = 0
		self.onMouseLeftButtonUpEvent = None
		self.onRunMouseWheelEvent = None ## Mouse Wheel Support
		if app.ENABLE_MOUSEWHEEL_EVENT:
			self.onMouseWheelEvent=None
		if app.ENABLE_RENDER_TARGET_EX:
			self.onMouseOverInEvent = None
			self.onMouseOverInArgs = None
			self.onMouseOverOutEvent = None
		if app.ENABLE_INGAME_WIKI:
			self.mouseLeftButtonDownEvent = None
			self.mouseLeftButtonDownArgs = None
			self.mouseLeftButtonUpEvent = None
			self.mouseLeftButtonUpArgs = None
			self.mouseLeftButtonDoubleClickEvent = None
			self.mouseRightButtonDownEvent = None
			self.mouseRightButtonDownArgs = None
			self.moveWindowEvent = None
			self.renderEvent = None
			self.renderArgs = None

			self.overInEvent = None
			self.overInArgs = None

			self.overOutEvent = None
			self.overOutArgs = None

		self.exPos = (0,0)

		self.RegisterWindow(layer)
		self.Hide()

	def __del__(self):
		wndMgr.Destroy(self.hWnd)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.Register(self, layer)

	def Destroy(self):
		pass

	def GetWindowHandle(self):
		return self.hWnd

	def AddFlag(self, style):
		wndMgr.AddFlag(self.hWnd, style)

	def IsRTL(self):
		return wndMgr.IsRTL(self.hWnd)

	def SetWindowName(self, Name):
		wndMgr.SetName(self.hWnd, Name)

	def GetWindowName(self):
		return wndMgr.GetName(self.hWnd)

	def SetParent(self, parent):
		wndMgr.SetParent(self.hWnd, parent.hWnd)

	if app.ENABLE_INGAME_WIKI:
		def SetParentNew(self, parent):
			wndMgr.SetParent(self.hWnd, parent.hWnd)

		def GetRenderBox(self):
			return wndMgr.GetRenderBox(self.hWnd)

		def SetInsideRender(self, val):
			wndMgr.SetInsideRender(self.hWnd, val)

	def SetParentProxy(self, parent):
		self.parentWindow=proxy(parent)
		wndMgr.SetParent(self.hWnd, parent.hWnd)

	def GetParentProxy(self):
		return self.parentWindow

	def SetPickAlways(self):
		wndMgr.SetPickAlways(self.hWnd)

	def SetWindowHorizontalAlignLeft(self):
		wndMgr.SetWindowHorizontalAlign(self.hWnd, wndMgr.HORIZONTAL_ALIGN_LEFT)

	def SetWindowHorizontalAlignCenter(self):
		wndMgr.SetWindowHorizontalAlign(self.hWnd, wndMgr.HORIZONTAL_ALIGN_CENTER)

	def SetWindowHorizontalAlignRight(self):
		wndMgr.SetWindowHorizontalAlign(self.hWnd, wndMgr.HORIZONTAL_ALIGN_RIGHT)

	def SetWindowVerticalAlignTop(self):
		wndMgr.SetWindowVerticalAlign(self.hWnd, wndMgr.VERTICAL_ALIGN_TOP)

	def SetWindowVerticalAlignCenter(self):
		wndMgr.SetWindowVerticalAlign(self.hWnd, wndMgr.VERTICAL_ALIGN_CENTER)

	def SetWindowVerticalAlignBottom(self):
		wndMgr.SetWindowVerticalAlign(self.hWnd, wndMgr.VERTICAL_ALIGN_BOTTOM)

	def SetTop(self):
		wndMgr.SetTop(self.hWnd)

	def Show(self):
		wndMgr.Show(self.hWnd)

	def Hide(self):
		wndMgr.Hide(self.hWnd)

	def SetVisible(self, isVisible):
		if isVisible:
			self.Show()
		else:
			self.Hide()

	def Lock(self):
		wndMgr.Lock(self.hWnd)

	def Unlock(self):
		wndMgr.Unlock(self.hWnd)

	def IsShow(self):
		return wndMgr.IsShow(self.hWnd)

	def UpdateRect(self):
		wndMgr.UpdateRect(self.hWnd)

	def SetSize(self, width, height):
		wndMgr.SetWindowSize(self.hWnd, width, height)

	def GetWidth(self):
		return wndMgr.GetWindowWidth(self.hWnd)

	def GetHeight(self):
		return wndMgr.GetWindowHeight(self.hWnd)

	def GetLocalPosition(self):
		return wndMgr.GetWindowLocalPosition(self.hWnd)

	def GetGlobalPosition(self):
		return wndMgr.GetWindowGlobalPosition(self.hWnd)

	def GetMouseLocalPosition(self):
		return wndMgr.GetMouseLocalPosition(self.hWnd)

	def GetLeft(self):
		x, y = self.GetLocalPosition()
		return x

	def GetGlobalLeft(self):
		x, y = self.GetGlobalPosition()
		return x

	def GetTop(self):
		x, y = self.GetLocalPosition()
		return y

	def GetGlobalTop(self):
		x, y = self.GetGlobalPosition()
		return y

	def GetRight(self):
		return self.GetLeft() + self.GetWidth()

	def GetBottom(self):
		return self.GetTop() + self.GetHeight()

	def GetRect(self):
		return wndMgr.GetWindowRect(self.hWnd)

	# def SetPosition(self, x, y):
	# 	wndMgr.SetWindowPosition(self.hWnd, x, y)

	def SetPosition(self, x, y, flag = False):
		if flag == True:
			self.exPos = (x,y)
		wndMgr.SetWindowPosition(self.hWnd, x, y)

	def SetCenterPosition(self, x = 0, y = 0):
		self.SetPosition((wndMgr.GetScreenWidth() - self.GetWidth()) / 2 + x, (wndMgr.GetScreenHeight() - self.GetHeight()) / 2 + y)

	def IsFocus(self):
		return wndMgr.IsFocus(self.hWnd)

	def SetFocus(self):
		wndMgr.SetFocus(self.hWnd)

	def KillFocus(self):
		wndMgr.KillFocus(self.hWnd)

	def GetChildCount(self):
		return wndMgr.GetChildCount(self.hWnd)

	if app.ENABLE_INGAME_WIKI:
		def IsIn(self, checkChilds = False):
			return wndMgr.IsIn(self.hWnd, checkChilds)

		def IsInPosition(self):
			xMouse, yMouse = wndMgr.GetMousePosition()
			x, y = self.GetGlobalPosition()
			return xMouse >= x and xMouse < x + self.GetWidth() and yMouse >= y and yMouse < y + self.GetHeight()

		def SetClickEvent(self, event):
			self.clickEvent = __mem_func__(event)

		def SetMouseLeftButtonDownEvent(self, event, *args):
			self.mouseLeftButtonDownEvent = event
			self.mouseLeftButtonDownArgs = args
		
		def OnMouseLeftButtonDown(self):
			if self.mouseLeftButtonDownEvent:
				apply(self.mouseLeftButtonDownEvent, self.mouseLeftButtonDownArgs)

		def SetMouseLeftButtonUpEvent(self, event, *args):
			self.mouseLeftButtonUpEvent = event
			self.mouseLeftButtonUpArgs = args

		def SetMouseLeftButtonDoubleClickEvent(self, event):
			self.mouseLeftButtonDoubleClickEvent = event

		def OnMouseLeftButtonDoubleClick(self):
			if self.mouseLeftButtonDoubleClickEvent:
				self.mouseLeftButtonDoubleClickEvent()

		def SetMouseRightButtonDownEvent(self, event, *args):
			self.mouseRightButtonDownEvent = event
			self.mouseRightButtonDownArgs = args

		def OnMouseRightButtonDown(self):
			if self.mouseRightButtonDownEvent:
				apply(self.mouseRightButtonDownEvent, self.mouseRightButtonDownArgs)

		def SetMoveWindowEvent(self, event):
			self.moveWindowEvent = event

		def OnMoveWindow(self, x, y):
			if self.moveWindowEvent:
				self.moveWindowEvent(x, y)
	else:
		def IsIn(self):
			return wndMgr.IsIn(self.hWnd)

	def IsInWindowRect(self):
		return wndMgr.IsInWindowRect(self.hWnd)

	def SetOnMouseLeftButtonUpEvent(self, event):
		self.onMouseLeftButtonUpEvent = event

	def OnMouseLeftButtonUp(self):
		if self.onMouseLeftButtonUpEvent:
			self.onMouseLeftButtonUpEvent()

	if app.ENABLE_MOUSEWHEEL_EVENT:
		def SetMouseWheelEvent(self, event):
			self.onMouseWheelEvent = event

		def OnMouseWheel(self, delta):
			# print("OnMouseWheel delta %d" % delta)
			if self.onMouseWheelEvent:
				return self.onMouseWheelEvent(delta)
			return False

	if app.ENABLE_RENDER_TARGET_EX:
		def SetOnMouseOverInEvent(self, event, *args):
			self.onMouseOverInEvent = event
			self.onMouseOverInArgs = args

		def OnMouseOverIn(self):
			if self.onMouseOverInEvent:
				apply(self.onMouseOverInEvent, self.onMouseOverInArgs)

		def SetOnMouseOverOutEvent(self, event):
			self.onMouseOverOutEvent = event

		def OnMouseOverOut(self):
			if self.onMouseOverOutEvent:
				self.onMouseOverOutEvent()

	if app.ENABLE_CLIP_MASK:	
		def SetClippingMaskRect(self, left, top, right, bottom):
			wndMgr.SetClippingMaskRect(self.hWnd, left, top, right, bottom)
			
		def SetClippingMaskWindow(self, clipping_mask_window):
			wndMgr.SetClippingMaskWindow(self.hWnd, clipping_mask_window.hWnd)

	if app.ENABLE_INGAME_WIKI:
		def SAFE_SetOverInEvent(self, func, *args):
			self.overInEvent = __mem_func__(func)
			self.overInArgs = args

		def SetOverInEvent(self, func, *args):
			self.overInEvent = func
			self.overInArgs = args

		def SAFE_SetOverOutEvent(self, func, *args):
			self.overOutEvent = __mem_func__(func)
			self.overOutArgs = args

		def SetOverOutEvent(self, func, *args):
			self.overOutEvent = func
			self.overOutArgs = args

		def OnMouseOverIn(self):
			if self.overInEvent:
				apply(self.overInEvent, self.overInArgs)

		def OnMouseOverOut(self):
			if self.overOutEvent:
				apply(self.overOutEvent, self.overOutArgs)

		def SAFE_SetRenderEvent(self, event, *args):
			self.renderEvent = __mem_func__(event)
			self.renderArgs = args

		def ClearRenderEvent(self):
			self.renderEvent = None
			self.renderArgs = None

		def OnRender(self):
			if self.renderEvent:
				apply(self.renderEvent, self.renderArgs)

	## Mouse Wheel Support
	def OnRunMouseWheel(self, nLen):
		if not self.onRunMouseWheelEvent:
			return False
		apply(self.onRunMouseWheelEvent, (bool(nLen < 0),))
		return True

	def SetOnRunMouseWheelEvent(self, event):
		self.onRunMouseWheelEvent = __mem_func__(event)

class ListBoxEx(Window):

	class Item(Window):
		def __init__(self):
			Window.__init__(self)

		def __del__(self):
			Window.__del__(self)

		def SetParent(self, parent):
			Window.SetParent(self, parent)
			self.parent=proxy(parent)

		def OnMouseLeftButtonDown(self):
			self.parent.SelectItem(self)

		def OnRender(self):
			if self.parent.GetSelectedItem()==self:
				self.OnSelectedRender()

		def OnSelectedRender(self):
			x, y = self.GetGlobalPosition()
			grp.SetColor(grp.GenerateColor(0.0, 0.0, 0.7, 0.7))
			grp.RenderBar(x, y, self.GetWidth(), self.GetHeight())

	def __init__(self):
		Window.__init__(self)

		self.viewItemCount=10
		self.basePos=0
		self.itemHeight=16
		self.itemStep=20
		self.selItem=0
		self.itemList=[]
		self.onSelectItemEvent = lambda *arg: None

		if localeInfo.IsARABIC():
			self.itemWidth=130
		else:
			self.itemWidth=100

		self.scrollBar=None
		self.__UpdateSize()

	def __del__(self):
		Window.__del__(self)

	def __UpdateSize(self):
		height=self.itemStep*self.__GetViewItemCount()

		self.SetSize(self.itemWidth, height)

	def IsEmpty(self):
		if len(self.itemList)==0:
			return 1
		return 0

	def SetItemStep(self, itemStep):
		self.itemStep=itemStep
		self.__UpdateSize()

	def SetItemSize(self, itemWidth, itemHeight):
		self.itemWidth=itemWidth
		self.itemHeight=itemHeight
		self.__UpdateSize()

	def SetViewItemCount(self, viewItemCount):
		self.viewItemCount=viewItemCount

	def SetSelectEvent(self, event):
		self.onSelectItemEvent = event

	def SetBasePos(self, basePos):
		for oldItem in self.itemList[self.basePos:self.basePos+self.viewItemCount]:
			oldItem.Hide()

		self.basePos=basePos

		pos=basePos
		for newItem in self.itemList[self.basePos:self.basePos+self.viewItemCount]:
			(x, y)=self.GetItemViewCoord(pos, newItem.GetWidth())
			newItem.SetPosition(x, y)
			newItem.Show()
			pos+=1

	def GetItemIndex(self, argItem):
		return self.itemList.index(argItem)

	def GetSelectedItem(self):
		return self.selItem

	def SelectIndex(self, index):

		if index >= len(self.itemList) or index < 0:
			self.selItem = None
			return

		try:
			self.selItem=self.itemList[index]
		except:
			pass

	def SelectItem(self, selItem):
		self.selItem=selItem
		self.onSelectItemEvent(selItem)

	def RemoveAllItems(self):
		self.selItem=None
		self.itemList=[]

		if self.scrollBar:
			self.scrollBar.SetPos(0)

	def RemoveItem(self, delItem):
		if delItem==self.selItem:
			self.selItem=None

		self.itemList.remove(delItem)

	def AppendItem(self, newItem):
		newItem.SetParent(self)
		newItem.SetSize(self.itemWidth, self.itemHeight)

		pos=len(self.itemList)
		if self.__IsInViewRange(pos):
			(x, y)=self.GetItemViewCoord(pos, newItem.GetWidth())
			newItem.SetPosition(x, y)
			newItem.Show()
		else:
			newItem.Hide()

		self.itemList.append(newItem)

	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(__mem_func__(self.__OnScroll))
		self.scrollBar=scrollBar

	def __OnScroll(self):
		self.SetBasePos(int(self.scrollBar.GetPos()*self.__GetScrollLen()))

	def __GetScrollLen(self):
		scrollLen=self.__GetItemCount()-self.__GetViewItemCount()
		if scrollLen<0:
			return 0

		return scrollLen

	def __GetViewItemCount(self):
		return self.viewItemCount

	def __GetItemCount(self):
		return len(self.itemList)

	def GetItemViewCoord(self, pos, itemWidth):
		if localeInfo.IsARABIC():
			return (self.GetWidth()-itemWidth-10, (pos-self.basePos)*self.itemStep)
		else:
			return (0, (pos-self.basePos)*self.itemStep)

	def __IsInViewRange(self, pos):
		if pos<self.basePos:
			return 0
		if pos>=self.basePos+self.viewItemCount:
			return 0
		return 1

	def GetItemCount(self):
		return len(self.itemList)

	if app.ENABLE_SWITCHBOT_SYSTEM:
		def GetItems(self):
			return self.itemList

class CandidateListBox(ListBoxEx):

	HORIZONTAL_MODE = 0
	VERTICAL_MODE = 1

	class Item(ListBoxEx.Item):
		def __init__(self, text):
			ListBoxEx.Item.__init__(self)

			self.textBox=TextLine()
			self.textBox.SetParent(self)
			self.textBox.SetText(text)
			self.textBox.Show()

		def __del__(self):
			ListBoxEx.Item.__del__(self)

	def __init__(self, mode = HORIZONTAL_MODE):
		ListBoxEx.__init__(self)
		self.itemWidth=32
		self.itemHeight=32
		self.mode = mode

	def __del__(self):
		ListBoxEx.__del__(self)

	def SetMode(self, mode):
		self.mode = mode

	def AppendItem(self, newItem):
		ListBoxEx.AppendItem(self, newItem)

	def GetItemViewCoord(self, pos):
		if self.mode == self.HORIZONTAL_MODE:
			return ((pos-self.basePos)*self.itemStep, 0)
		elif self.mode == self.VERTICAL_MODE:
			return (0, (pos-self.basePos)*self.itemStep)


class TextLine(Window):
	def __init__(self):
		Window.__init__(self)
		self.max = 0
		self.SetFontName(localeInfo.UI_DEF_FONT)

	def __del__(self):
		Window.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterTextLine(self, layer)

	def SetMax(self, max):
		wndMgr.SetMax(self.hWnd, max)

	if app.ENABLE_INGAME_WIKI:
		def GetRenderPos(self):
			return wndMgr.GetRenderPos(self.hWnd)

		def SetFixedRenderPos(self, startPos, endPos):
			wndMgr.SetFixedRenderPos(self.hWnd, startPos, endPos)

	def SetLimitWidth(self, width):
		wndMgr.SetLimitWidth(self.hWnd, width)

	def SetMultiLine(self):
		wndMgr.SetMultiLine(self.hWnd, True)

	def SetHorizontalAlignArabic(self):
		wndMgr.SetHorizontalAlign(self.hWnd, wndMgr.TEXT_HORIZONTAL_ALIGN_ARABIC)

	def SetHorizontalAlignLeft(self):
		wndMgr.SetHorizontalAlign(self.hWnd, wndMgr.TEXT_HORIZONTAL_ALIGN_LEFT)

	def SetHorizontalAlignRight(self):
		wndMgr.SetHorizontalAlign(self.hWnd, wndMgr.TEXT_HORIZONTAL_ALIGN_RIGHT)

	def SetHorizontalAlignCenter(self):
		wndMgr.SetHorizontalAlign(self.hWnd, wndMgr.TEXT_HORIZONTAL_ALIGN_CENTER)

	def SetVerticalAlignTop(self):
		wndMgr.SetVerticalAlign(self.hWnd, wndMgr.TEXT_VERTICAL_ALIGN_TOP)

	def SetVerticalAlignBottom(self):
		wndMgr.SetVerticalAlign(self.hWnd, wndMgr.TEXT_VERTICAL_ALIGN_BOTTOM)

	def SetVerticalAlignCenter(self):
		wndMgr.SetVerticalAlign(self.hWnd, wndMgr.TEXT_VERTICAL_ALIGN_CENTER)

	def SetSecret(self, Value=True):
		wndMgr.SetSecret(self.hWnd, Value)

	def SetOutline(self, Value=True):
		wndMgr.SetOutline(self.hWnd, Value)

	def SetFeather(self, value=True):
		wndMgr.SetFeather(self.hWnd, value)

	def SetFontName(self, fontName):
		wndMgr.SetFontName(self.hWnd, fontName)

	def SetDefaultFontName(self):
		wndMgr.SetFontName(self.hWnd, localeInfo.UI_DEF_FONT)

	def SetFontColor(self, red, green, blue):
		wndMgr.SetFontColor(self.hWnd, red, green, blue)

	def SetPackedFontColor(self, color):
		wndMgr.SetFontColor(self.hWnd, color)

	def SetText(self, text):
		wndMgr.SetText(self.hWnd, text)

	def GetText(self):
		return wndMgr.GetText(self.hWnd)

	def GetTextSize(self):
		return wndMgr.GetTextSize(self.hWnd)

	def SetTextColor(self, color):
		self.SetPackedFontColor(color)

	def AdjustSize(self):
		x, y = self.GetTextSize()
		wndMgr.SetWindowSize(self.hWnd, x, y)

	def GetTextWidth(self):
		w, h = self.GetTextSize()
		return w
		
	def GetTextHeight(self):
		w, h = self.GetTextSize()
		return h

	def SetRenderingRect(self, left, top, right, bottom):
		wndMgr.SetRenderingRect(self.hWnd, left, top, right, bottom)

class EmptyCandidateWindow(Window):
	def __init__(self):
		Window.__init__(self)

	def __del__(self):
		Window.__del__(self)

	def Load(self):
		pass

	def SetCandidatePosition(self, x, y, textCount):
		pass

	def Clear(self):
		pass

	def Append(self, text):
		pass

	def Refresh(self):
		pass

	def Select(self):
		pass

class EditLine(TextLine):
	candidateWindowClassDict = {}

	def __init__(self):
		TextLine.__init__(self)

		self.eventReturn = Window.NoneMethod
		self.eventEscape = Window.NoneMethod
		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			self.eventUpdate = Window.NoneMethod
			self.eventTab = Window.NoneMethod
			self.eventTabArgs = None
			self.eventUpdateArgs = None
		else:
			self.eventTab = None
		if app.ENABLE_INGAME_WIKI:
			self.eventUpdate = None
		#self.CanClick = None
		self.numberMode = False
		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			self.inputMode = ime.MODE_STRING

			self.backgroundText = TextLine()
			self.backgroundText.SetParent(self)
			self.backgroundText.SetPosition(0, 0)
			self.backgroundText.SetPackedFontColor(WHITE_COLOR)
			self.backgroundText.Hide()
		self.useIME = True

		self.bCodePage = False

		self.candidateWindowClass = None
		self.candidateWindow = None
		self.SetCodePage(app.GetDefaultCodePage())

		self.readingWnd = ReadingWnd()
		self.readingWnd.Hide()

		if app.ENABLE_INGAME_WIKI:
			self.overLay = TextLine()
			self.overLay.SetParent(self)
			if localeInfo.IsARABIC():
				self.overLay.SetPosition(15, 0)
				self.overLay.SetHorizontalAlignRight()
			else:
				self.overLay.SetPosition(0, 0)
			self.overLay.SetPackedFontColor(WHITE_COLOR)
			self.overLay.Hide()

	def __del__(self):
		TextLine.__del__(self)

		self.eventReturn = Window.NoneMethod
		self.eventEscape = Window.NoneMethod
		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			self.eventTab    = Window.NoneMethod
			self.eventUpdate = Window.NoneMethod
			self.eventUpdateArgs = None
			self.eventTabArgs = None
			self.backgroundText = None
		else:
			self.eventTab = None

	def SetCodePage(self, codePage):
		candidateWindowClass=EditLine.candidateWindowClassDict.get(codePage, EmptyCandidateWindow)
		self.__SetCandidateClass(candidateWindowClass)

	def __SetCandidateClass(self, candidateWindowClass):
		if self.candidateWindowClass==candidateWindowClass:
			return

		self.candidateWindowClass = candidateWindowClass
		self.candidateWindow = self.candidateWindowClass()
		self.candidateWindow.Load()
		self.candidateWindow.Hide()

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterTextLine(self, layer)

	if app.ENABLE_PREMIUM_PRIVATE_SHOP:
		def SAFE_SetReturnEvent(self, event):
			self.eventReturn = __mem_func__(event)		
			self.eventReturnArgs = args

		def SetReturnEvent(self, event, *args):
			self.eventReturn = event
			self.eventReturnArgs = args

		def SAFE_SetUpdateEvent(self, event, *args):
			self.eventUpdate = __mem_func__(event)		
			self.eventUpdateArgs = args
			
		def SetUpdateEvent(self, event, *args):
			self.eventUpdate = event
			self.eventUpdateArgs = args

		def SetEscapeEvent(self, event, *args):
			self.eventEscape = event
			self.eventEscapeArgs = args

		def SetTabEvent(self, event, *args):
			self.eventTab = event
			self.eventTabArgs = args
			
		def SetBackgroundText(self, text):
			self.backgroundText.SetPosition(0, 0)
			self.backgroundText.SetText(text)
			
			if not self.backgroundText.IsShow():
				self.backgroundText.Show()

		def SetTipText(self, tipText):
			input = self.GetText()
			(widht, height) = self.GetTextSize()

			tip = tipText[len(input):]
			self.backgroundText.SetPosition(widht, 0)
			self.backgroundText.SetText(tip)

			if not self.backgroundText.IsShow():
				self.backgroundText.Show()
			
		def GetBackgroundText(self):
			return self.backgroundText.GetText()
	else:
		def SAFE_SetReturnEvent(self, event):
			self.eventReturn = __mem_func__(event)

		def SetReturnEvent(self, event):
			self.eventReturn = event

		def SetEscapeEvent(self, event):
			self.eventEscape = event

		def SetTabEvent(self, event):
			self.eventTab = event

	if app.ENABLE_INGAME_WIKI:
		def SetUpdateEvent(self, event):
			self.eventUpdate = event

		def SetOverlayText(self, text):
			self.overLay.SetText(text)
			self.__RefreshOverlay()

		def GetOverlayText(self):
			return self.overLay.GetText()

		def GetDisplayText(self):
			if len(self.GetText()):
				return self.GetText()
			else:
				return self.overLay.GetText()

		def __RefreshOverlay(self):
			if len(self.GetText()):
				self.overLay.Hide()
			else:
				self.overLay.Show()

		def IsShowCursor(self):
			return wndMgr.IsShowCursor(self.hWnd)

	#def CanEdit(self, flag):
	#	self.CanClick = flag

	def SetMax(self, max):
		self.max = max
		wndMgr.SetMax(self.hWnd, self.max)
		ime.SetMax(self.max)
		self.SetUserMax(self.max)

	def SetUserMax(self, max):
		self.userMax = max
		ime.SetUserMax(self.userMax)

	def SetNumberMode(self):
		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			self.inputMode = ime.MODE_NUMBER
		else:
			self.numberMode = True

	if app.ENABLE_PREMIUM_PRIVATE_SHOP:
		def SetCurrencyMode(self):
			self.inputMode = ime.MODE_CURRENCY

	#def AddExceptKey(self, key):
	#	ime.AddExceptKey(key)

	#def ClearExceptKey(self):
	#	ime.ClearExceptKey()

	def SetIMEFlag(self, flag):
		self.useIME = flag

	def SetText(self, text):
		wndMgr.SetText(self.hWnd, text)

		if self.IsFocus():
			ime.SetText(text)

		if app.ENABLE_INGAME_WIKI:
			self.__RefreshOverlay()

	def Enable(self):
		wndMgr.ShowCursor(self.hWnd)

	def Disable(self):
		wndMgr.HideCursor(self.hWnd)

	def SetEndPosition(self):
		ime.MoveEnd()

	def OnSetFocus(self):
		Text = self.GetText()
		ime.SetText(Text)
		ime.SetMax(self.max)
		ime.SetUserMax(self.userMax)
		ime.SetCursorPosition(-1)
		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			if self.inputMode == ime.MODE_STRING:
				ime.SetStringMode()

			elif self.inputMode == ime.MODE_NUMBER:
				ime.SetNumberMode()

			elif self.inputMode == ime.MODE_CURRENCY:
				ime.SetCurrencyMode()
		else:
			if self.numberMode:
				ime.SetNumberMode()
			else:
				ime.SetStringMode()
		ime.EnableCaptureInput()
		if self.useIME:
			ime.EnableIME()
		else:
			ime.DisableIME()
		wndMgr.ShowCursor(self.hWnd, True)
		if app.ENABLE_INGAME_WIKI:
			self.__RefreshOverlay()

	def OnKillFocus(self):
		self.SetText(ime.GetText(self.bCodePage))
		self.OnIMECloseCandidateList()
		self.OnIMECloseReadingWnd()
		ime.DisableIME()
		ime.DisableCaptureInput()
		wndMgr.HideCursor(self.hWnd)

	def OnIMEChangeCodePage(self):
		self.SetCodePage(ime.GetCodePage())

	def OnIMEOpenCandidateList(self):
		self.candidateWindow.Show()
		self.candidateWindow.Clear()
		self.candidateWindow.Refresh()

		gx, gy = self.GetGlobalPosition()
		self.candidateWindow.SetCandidatePosition(gx, gy, len(self.GetText()))

		return True

	def OnIMECloseCandidateList(self):
		self.candidateWindow.Hide()
		return True

	def OnIMEOpenReadingWnd(self):
		gx, gy = self.GetGlobalPosition()
		textlen = len(self.GetText())-2
		reading = ime.GetReading()
		readinglen = len(reading)
		self.readingWnd.SetReadingPosition( gx + textlen*6-24-readinglen*6, gy )
		self.readingWnd.SetText(reading)
		if ime.GetReadingError() == 0:
			self.readingWnd.SetTextColor(0xffffffff)
		else:
			self.readingWnd.SetTextColor(0xffff0000)
		self.readingWnd.SetSize(readinglen * 6 + 4, 19)
		self.readingWnd.Show()
		return True

	def OnIMECloseReadingWnd(self):
		self.readingWnd.Hide()
		return True

	def OnIMEUpdate(self):
		snd.PlaySound("sound/ui/type.wav")
		TextLine.SetText(self, ime.GetText(self.bCodePage))

		if app.ENABLE_INGAME_WIKI:
			self.__RefreshOverlay()

			if self.eventUpdate:
				self.eventUpdate()

		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			if self.eventUpdate != None:
				if self.eventUpdateArgs:
					apply(self.eventUpdate, self.eventUpdateArgs)
				else:
					self.eventUpdate()

				if self.eventUpdate != Window.NoneMethod:
					return True
			
			return False

	def OnIMETab(self):
		if self.eventTab:
			self.eventTab()
			return True

		return False

	def OnIMEReturn(self):
		snd.PlaySound("sound/ui/click.wav")
		self.eventReturn()

		return True

	def OnPressEscapeKey(self):
		if self.IsFocus(): #@fixme018 (IsFocus check)
			self.eventEscape()
			return True
		return False

	def OnKeyDown(self, key):
		if app.DIK_F1 == key:
			return False
		if app.DIK_F2 == key:
			return False
		if app.DIK_F3 == key:
			return False
		if app.DIK_F4 == key:
			return False
		if app.DIK_LALT == key:
			return False
		if app.DIK_SYSRQ == key:
			return False
		if app.DIK_LCONTROL == key:
			return False
		if app.DIK_V == key:
			if app.IsPressed(app.DIK_LCONTROL):
				ime.PasteTextFromClipBoard()

		return True

	def OnKeyUp(self, key):
		if app.DIK_F1 == key:
			return False
		if app.DIK_F2 == key:
			return False
		if app.DIK_F3 == key:
			return False
		if app.DIK_F4 == key:
			return False
		if app.DIK_LALT == key:
			return False
		if app.DIK_SYSRQ == key:
			return False
		if app.DIK_LCONTROL == key:
			return False

		return True

	def OnIMEKeyDown(self, key):
		# Left
		if app.VK_LEFT == key:
			ime.MoveLeft()
			return True
		# Right
		if app.VK_RIGHT == key:
			ime.MoveRight()
			return True

		# Home
		if app.VK_HOME == key:
			ime.MoveHome()
			return True
		# End
		if app.VK_END == key:
			ime.MoveEnd()
			return True

		# Delete
		if app.VK_DELETE == key:
			ime.Delete()
			TextLine.SetText(self, ime.GetText(self.bCodePage))
			return True

		return True

	#def OnMouseLeftButtonDown(self):
	#	self.SetFocus()
	def OnMouseLeftButtonDown(self):
		if False == self.IsIn():
			return False

		# if False == self.CanClick:
		# 	return

		self.SetFocus()
		PixelPosition = wndMgr.GetCursorPosition(self.hWnd)
		ime.SetCursorPosition(PixelPosition)

class MarkBox(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

	def __del__(self):
		Window.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterMarkBox(self, layer)

	def Load(self):
		wndMgr.MarkBox_Load(self.hWnd)

	def SetScale(self, scale):
		wndMgr.MarkBox_SetScale(self.hWnd, scale)

	def SetIndex(self, guildID):
		MarkID = guild.GuildIDToMarkID(guildID)
		wndMgr.MarkBox_SetImageFilename(self.hWnd, guild.GetMarkImageFilenameByMarkID(MarkID))
		wndMgr.MarkBox_SetIndex(self.hWnd, guild.GetMarkIndexByMarkID(MarkID))

	def SetAlpha(self, alpha):
		wndMgr.MarkBox_SetDiffuseColor(self.hWnd, 1.0, 1.0, 1.0, alpha)

class ImageBox(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		self.eventDict={}
		self.argDict={}
		self.eventFunc = {"mouse_click" : None, "mouse_over_in" : None, "mouse_over_out" : None}
		self.eventArgs = {"mouse_click" : None, "mouse_over_in" : None, "mouse_over_out" : None}

		self.onMouseLeftStaticEvent = None
		self.onMouseRightStaticEvent = None
		self.ButtonText = None

	def __del__(self):
		Window.__del__(self)
		# self.eventFunc = None
		# self.eventArgs = None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterImageBox(self, layer)

	def SetTextOutline(self):#test
		if not self.ButtonText:
			return
		self.ButtonText.SetOutline()

	def SetPackedFontColor(self, color):
		if not self.ButtonText:
			return
		self.ButtonText.SetPackedFontColor(color)

	def SetText(self, text, x, y, center = False, idk2 = False, idk3 = False, idk4 = False):#test

		if not self.ButtonText:
			textLine = TextLine()
			textLine.SetParent(self)
			textLine.SetPosition(x, y)
			if center:
				textLine.SetVerticalAlignCenter()
				textLine.SetWindowVerticalAlignCenter()
				textLine.SetHorizontalAlignCenter()
				textLine.SetWindowHorizontalAlignCenter()
			textLine.Show()
			self.ButtonText = textLine

		self.ButtonText.SetText(text)

	def LoadImage(self, imageName):
		self.name=imageName
		wndMgr.LoadImage(self.hWnd, imageName)

		if len(self.eventDict)!=0:
			print "LOAD IMAGE", self, self.eventDict

	def UnloadImage(self):
		wndMgr.UnloadImage(self.hWnd)

	def SetToolTipThinText(self, text, x=0, y = -19):
		self.SetFormToolTipText("TEXT", text, x, y)

	def SetAlpha(self, alpha):
		wndMgr.SetDiffuseColor(self.hWnd, 1.0, 1.0, 1.0, alpha)

	def GetWidth(self):
		return wndMgr.GetWidth(self.hWnd)

	def GetHeight(self):
		return wndMgr.GetHeight(self.hWnd)

	def SetEvent(self, func, *args) :
		result = self.eventFunc.has_key(args[0])
		if result:
			self.eventFunc[args[0]] = func
			self.eventArgs[args[0]] = args
		else :
			print "[ERROR] ui.py SetEvent, Can`t Find has_key : %s" % args[0]

	def __OnMouseOverIn(self):
		try:
			apply(self.eventDict["mouse_over_in"], self.argDict["mouse_over_in"])
		except KeyError:
			pass

	def __OnMouseOverOut(self):
		try:
			apply(self.eventDict["mouse_over_out"], self.argDict["mouse_over_out"])
		except KeyError:
			pass

	def OnMouseOverIn(self) :
		if self.eventFunc["mouse_over_in"]:
			apply(self.eventFunc["mouse_over_in"], self.eventArgs["mouse_over_in"])
		else:
			self.__OnMouseOverIn()

	def OnMouseOverOut(self):
		if self.eventFunc["mouse_over_out"]:
			apply(self.eventFunc["mouse_over_out"], self.eventArgs["mouse_over_out"])
		else:
			self.__OnMouseOverOut()

	def OnMouseLeftButtonDown(self):
		if self.eventDict.has_key("mouse_left_down"):
			apply(self.eventDict["mouse_left_down"], self.argDict["mouse_left_down"])

	def OnMouseLeftButtonUp(self):
		if self.eventFunc["mouse_click"] :
			apply(self.eventFunc["mouse_click"], self.eventArgs["mouse_click"])
		elif not isinstance(self.eventArgs, dict):
			apply(self.eventFunc, self.eventArgs)

		if self.onMouseLeftStaticEvent:
			self.onMouseLeftStaticEvent()

	def OnMouseRightButtonDown(self):
		if self.eventDict.has_key("mouse_right_down"):
			apply(self.eventDict["mouse_right_down"], self.argDict["mouse_right_down"])

	def OnMouseRightButtonUp(self):
		if self.eventFunc["mouse_click"] :
			apply(self.eventFunc["mouse_click"], self.eventArgs["mouse_click"])

		elif not isinstance(self.eventArgs, dict):
			apply(self.eventFunc, self.eventArgs)

		if self.onMouseRightStaticEvent:
			self.onMouseRightStaticEvent()

	def SetStringEvent(self, event, func, *args):
		self.eventDict[event]=func
		self.argDict[event]=args

	def SAFE_SetStringEvent(self, event, func, *args):
		self.eventDict[event]=__mem_func__(func)
		self.argDict[event]=args

	def SetOnMouseLeftButtonUpEvent(self, event):
		self.onMouseLeftStaticEvent = event

	def SetOnMouseRightButtonUpEvent(self, event):
		self.onMouseRightStaticEvent = event

	if app.ENABLE_PLAYER_HP:
		def DisplayProcent(self, percent):
			wndMgr.DisplayImageProcent(self.hWnd, percent)

	if app.ENABLE_ANTI_EXP:
		def SetDiffuseColor(self, r, g, b, a):
			wndMgr.SetDiffuseColor(self.hWnd, r, g, b, a)

class ExpandedImageBox(ImageBox):
	def __init__(self, layer = "UI"):
		ImageBox.__init__(self, layer)

	def __del__(self):
		ImageBox.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterExpandedImageBox(self, layer)

	def SetScale(self, xScale, yScale):
		wndMgr.SetScale(self.hWnd, xScale, yScale)

	def SetOrigin(self, x, y):
		wndMgr.SetOrigin(self.hWnd, x, y)

	def SetRotation(self, rotation):
		wndMgr.SetRotation(self.hWnd, rotation)

	def SetRenderingMode(self, mode):
		wndMgr.SetRenderingMode(self.hWnd, mode)

	def SetRenderingRect(self, left, top, right, bottom):
		wndMgr.SetRenderingRect(self.hWnd, left, top, right, bottom)

	def SetClipRect(self, left, top, right, bottom, isVertical = False):
		wndMgr.SetClipRect(self.hWnd, left, top, right, bottom, isVertical)

	def SetPercentage(self, curValue, maxValue):
		if maxValue:
			self.SetRenderingRect(0.0, 0.0, -1.0 + float(curValue) / float(maxValue), 0.0)
		else:
			self.SetRenderingRect(0.0, 0.0, 0.0, 0.0)

	def GetWidth(self):
		return wndMgr.GetWindowWidth(self.hWnd)

	def GetHeight(self):
		return wndMgr.GetWindowHeight(self.hWnd)

	if app.ENABLE_INGAME_WIKI:
		def SetWikiImage(self, bFlag):
			wndMgr.SetWikiImage(self.hWnd, bFlag)

class AniImageBox(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		self.endFrameEvent = None
		self.endFrameArgs = None
		
		self.keyFrameEvent = None
		self.eventEndFrame = None

	def __del__(self):
		Window.__del__(self)

		self.endFrameEvent = None
		self.endFrameArgs = None
		
		self.keyFrameEvent = None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterAniImageBox(self, layer)

	def SetDelay(self, delay):
		wndMgr.SetDelay(self.hWnd, delay)

	def AppendImage(self, filename):
		wndMgr.AppendImage(self.hWnd, filename)

	def SetPercentage(self, curValue, maxValue):
		wndMgr.SetRenderingRect(self.hWnd, 0.0, 0.0, -1.0 + float(curValue) / float(maxValue), 0.0)

	def ResetFrame(self):
		wndMgr.ResetFrame(self.hWnd)
		
	def SetEndFrameEvent(self, event, *args):
		self.endFrameEvent = event
		self.endFrameArgs = args

	def SetOnEndFrame(self, event):
		self.eventEndFrame = event

	def OnEndFrame(self):
		if self.eventEndFrame:
			self.eventEndFrame()
		if self.endFrameEvent:
			apply(self.endFrameEvent, self.endFrameArgs)

	def SetKeyFrameEvent(self, event):
		self.keyFrameEvent = event
	
	def OnKeyFrame(self, curFrame):
		if self.keyFrameEvent:
			self.keyFrameEvent(curFrame)

	def SetScale(self, xScale, yScale):
		wndMgr.SetAniImgScale(self.hWnd, xScale, yScale)

	def SetPercentageWithScale(self, curValue, maxValue):
		wndMgr.SetRenderingRectWithScale(self.hWnd, 0.0, 0.0, -1.0 + float(curValue) / float(maxValue), 0.0)

class Button(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		self.eventFunc = None
		self.eventArgs = None

		self.overFunc = None
		self.overArgs = None
		self.overOutFunc = None
		self.overOutArgs = None

		self.showtooltipevent = None
		self.showtooltiparg = None
		self.hidetooltipevent = None
		self.hidetooltiparg = None

		self.ButtonText = None
		self.ToolTipText = None

		self.TextChild = []

	def __del__(self):
		Window.__del__(self)

		self.eventFunc = None
		self.eventArgs = None

		self.overFunc = None
		self.overArgs = None
		self.overOutFunc = None
		self.overOutArgs = None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterButton(self, layer)

	def SetUpVisual(self, filename):
		wndMgr.SetUpVisual(self.hWnd, filename)

	def SetOverVisual(self, filename):
		wndMgr.SetOverVisual(self.hWnd, filename)

	def SetDownVisual(self, filename):
		wndMgr.SetDownVisual(self.hWnd, filename)

	def SetDisableVisual(self, filename):
		wndMgr.SetDisableVisual(self.hWnd, filename)

	def GetUpVisualFileName(self):
		return wndMgr.GetUpVisualFileName(self.hWnd)

	def GetOverVisualFileName(self):
		return wndMgr.GetOverVisualFileName(self.hWnd)

	def GetDownVisualFileName(self):
		return wndMgr.GetDownVisualFileName(self.hWnd)

	def Flash(self):
		wndMgr.Flash(self.hWnd)

	def Enable(self):
		wndMgr.Enable(self.hWnd)

	def Disable(self):
		wndMgr.Disable(self.hWnd)

	def EnableFlash(self):
		wndMgr.EnableFlash(self.hWnd)

	def DisableFlash(self):
		wndMgr.DisableFlash(self.hWnd)

	def Down(self):
		wndMgr.Down(self.hWnd)

	def SetUp(self):
		wndMgr.SetUp(self.hWnd)

	def SAFE_SetEvent(self, func, *args):
		self.eventFunc = __mem_func__(func)
		self.eventArgs = args

	def SetEvent(self, func, *args):
		self.eventFunc = func
		self.eventArgs = args

	def SetTextColor(self, color):
		if not self.ButtonText:
			return
		self.ButtonText.SetPackedFontColor(color)

	def SetTextAddPos(self, text, x_add = 0, y_add = 0, height = 4):
		if not self.ButtonText:
			textLine = TextLine()
			textLine.SetParent(self)
			textLine.SetPosition(self.GetWidth() / 2 + x_add, self.GetHeight() / 2 + y_add)
			textLine.SetVerticalAlignCenter()
			textLine.SetHorizontalAlignCenter()
			textLine.Show()
			self.ButtonText = textLine
		self.ButtonText.SetText(text)

	def SetText(self, text, height = 4):

		if not self.ButtonText:
			textLine = TextLine()
			textLine.SetParent(self)
			textLine.SetPosition(self.GetWidth()/2, self.GetHeight()/2)
			textLine.SetVerticalAlignCenter()
			textLine.SetHorizontalAlignCenter()
			textLine.Show()
			self.ButtonText = textLine

		self.ButtonText.SetText(text)

	def SetFormToolTipText(self, type, text, x, y):
		if not self.ToolTipText:
			toolTip=createToolTipWindowDict[type]()
			toolTip.SetParent(self)
			toolTip.SetSize(0, 0)
			toolTip.SetHorizontalAlignCenter()
			toolTip.SetOutline()
			toolTip.Hide()
			toolTip.SetPosition(x + self.GetWidth()/2, y)
			self.ToolTipText=toolTip

		self.ToolTipText.SetText(text)

	def SetToolTipWindow(self, toolTip):
		self.ToolTipText=toolTip
		self.ToolTipText.SetParentProxy(self)

	def SetToolTipText(self, text, x=0, y = -19):
		self.SetFormToolTipText("TEXT", text, x, y)

	def CallEvent(self):
		snd.PlaySound("sound/ui/click.wav")

		if self.eventFunc:
			apply(self.eventFunc, self.eventArgs)

	def SetOverEvent(self, func, *args):
		self.overFunc = func
		self.overArgs = args

	def SetOverOutEvent(self, func, *args):
		self.overOutFunc = func
		self.overOutArgs = args

	def ShowToolTip(self):
		if self.ToolTipText:
			self.ToolTipText.Show()

		if self.showtooltipevent:
			apply(self.showtooltipevent, self.showtooltiparg)

	def HideToolTip(self):
		if self.ToolTipText:
			self.ToolTipText.Hide()

		if self.hidetooltipevent:
			apply(self.hidetooltipevent, self.hidetooltiparg)
		
	def OnMouseOverIn(self):
		if self.overFunc:
			apply(self.overFunc, self.overArgs )

	def OnMouseOverOut(self):
		if self.overOutFunc:
			apply(self.overOutFunc, self.overOutArgs )

	def SetShowToolTipEvent(self, func, *args):
		self.showtooltipevent = func
		self.showtooltiparg = args
		
	def SetHideToolTipEvent(self, func, *args):
		self.hidetooltipevent = func
		self.hidetooltiparg = args

	def IsDown(self):
		return wndMgr.IsDown(self.hWnd)

	if app.ENABLE_PREMIUM_PRIVATE_SHOP or app.ENABLE_SKILL_COLOR_SYSTEM or constInfo.EXTRA_UI_FEATURE:
		def GetText(self):
			if self.ButtonText:
				return self.ButtonText.GetText()
			else:
				return ""

	if app.ENABLE_PREMIUM_PRIVATE_SHOP:
		def SetAlpha(self, alpha):
			wndMgr.SetButtonDiffuseColor(self.hWnd, 1.0, 1.0, 1.0, alpha)

		def IsDisable(self):
			return wndMgr.IsDisable(self.hWnd)
		
		def SetScale(self, scale_x, scale_y):
			wndMgr.SetButtonScale(self.hWnd, scale_x, scale_y)
			
		def SetDiffuseColor(self, r, g, b, a):
			wndMgr.SetButtonDiffuseColor(self.hWnd, r, g, b, a)

	if app.ENABLE_SKILL_COLOR_SYSTEM or app.ENABLE_NEW_GAMEOPTION:
		def SetListText(self, text, x = 8):
			if not self.ButtonText:
				textLine = TextLine()
				textLine.SetParent(self)
				textLine.SetPosition(x, self.GetHeight()/2)
				textLine.SetVerticalAlignCenter()
				if localeInfo.IsARABIC():
					textLine.SetHorizontalAlignRight()
				else:
					textLine.SetHorizontalAlignLeft()
				textLine.Show()
				self.ButtonText = textLine

			self.ButtonText.SetText(text)

		def SetTextAlignLeft(self, text, x = 27, height = 4):
			if not self.ButtonText:
				textLine = TextLine()
				textLine.SetParent(self)
				textLine.SetPosition(x, self.GetHeight()/2)
				textLine.SetVerticalAlignCenter()
				textLine.SetHorizontalAlignRight()
				textLine.Show()
				self.ButtonText = textLine

			self.ButtonText.SetText(text)
			self.ButtonText.SetPosition(x, self.GetHeight()/2)
			self.ButtonText.SetVerticalAlignCenter()
			self.ButtonText.SetHorizontalAlignLeft()

	def SetRenderingRect(self, left, top, right, bottom):
		wndMgr.SetRenderingRect(self.hWnd, left, top, right, bottom)

class RadioButton(Button):
	def __init__(self):
		Button.__init__(self)

	def __del__(self):
		Button.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterRadioButton(self, layer)

	if app.ENABLE_NEW_GAMEOPTION:
		def SetRenderingRect(self, left, top, right, bottom):
			wndMgr.SetRenderingRect(self.hWnd, left, top, right, bottom)

def calculateRect(curValue, maxValue):
	try:
		return -1.0 + float(curValue) / float(maxValue)
	except:
		return 0.0

class RadioButtonGroup:
	def __init__(self):
		self.buttonGroup = []
		self.selectedBtnIdx = -1

	def __del__(self):
		self.buttonGroup = []
		self.selectedBtnIdx = -1

	def Show(self):
		for (button, selectEvent, unselectEvent) in self.buttonGroup:
			button.Show()

	def Hide(self):
		for (button, selectEvent, unselectEvent) in self.buttonGroup:
			button.Hide()

	def SetText(self, idx, text):
		if idx >= len(self.buttonGroup):
			return
		(button, selectEvent, unselectEvent) = self.buttonGroup[idx]
		button.SetText(text)

	def OnClick(self, btnIdx):
		if btnIdx == self.selectedBtnIdx:
			return
		(button, selectEvent, unselectEvent) = self.buttonGroup[self.selectedBtnIdx]
		if unselectEvent:
			unselectEvent(self.selectedBtnIdx)
		button.SetUp()

		self.selectedBtnIdx = btnIdx
		(button, selectEvent, unselectEvent) = self.buttonGroup[btnIdx]
		if selectEvent:
			selectEvent(btnIdx)

		button.Down()

	def AddButton(self, button, selectEvent, unselectEvent):
		i = len(self.buttonGroup)
		button.SetEvent(__mem_func__(self.OnClick),i)
		self.buttonGroup.append([button, selectEvent, unselectEvent])
		button.SetUp()

	def Create(rawButtonGroup):
		radioGroup = RadioButtonGroup()
		for (button, selectEvent, unselectEvent) in rawButtonGroup:
			radioGroup.AddButton(button, selectEvent, unselectEvent)

		radioGroup.OnClick(0)

		return radioGroup

	Create=staticmethod(Create)

class ToggleButton(Button):
	def __init__(self):
		Button.__init__(self)

		self.eventUp = None
		self.eventDown = None

		self.eventUpArgs = None
		self.eventDownArgs = None

	def __del__(self):
		Button.__del__(self)

		self.eventUp = None
		self.eventDown = None

	def SetToggleUpEvent(self, event, *args):
		self.eventUp = event
		self.eventUpArgs = args

	def SetToggleDownEvent(self, event, *args):
		self.eventDown = event
		self.eventDownArgs = args

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterToggleButton(self, layer)

	def OnToggleUp(self):
		if self.eventUp:
			if self.eventUpArgs:
				apply(self.eventUp, self.eventUpArgs)
			else:
				self.eventUp()

	def OnToggleDown(self):
		if self.eventDown:
			if self.eventDownArgs:
				apply(self.eventDown, self.eventDownArgs)
			else:
				self.eventDown()

class DragButton(Button):
	def __init__(self):
		Button.__init__(self)
		self.AddFlag("movable")

		self.callbackEnable = True
		self.eventMove = lambda: None

	def __del__(self):
		Button.__del__(self)

		self.eventMove = lambda: None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterDragButton(self, layer)

	def SetMoveEvent(self, event):
		self.eventMove = event

	def SetRestrictMovementArea(self, x, y, width, height):
		wndMgr.SetRestrictMovementArea(self.hWnd, x, y, width, height)

	def TurnOnCallBack(self):
		self.callbackEnable = True

	def TurnOffCallBack(self):
		self.callbackEnable = False

	def OnMove(self):
		if self.callbackEnable:
			self.eventMove()

class NumberLine(Window):

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

	def __del__(self):
		Window.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterNumberLine(self, layer)

	def SetHorizontalAlignCenter(self):
		wndMgr.SetNumberHorizontalAlignCenter(self.hWnd)

	def SetHorizontalAlignRight(self):
		wndMgr.SetNumberHorizontalAlignRight(self.hWnd)

	def SetPath(self, path):
		wndMgr.SetPath(self.hWnd, path)

	def SetNumber(self, number):
		wndMgr.SetNumber(self.hWnd, number)

###################################################################################################
## PythonScript Element
###################################################################################################

class Box(Window):

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterBox(self, layer)

	def SetColor(self, color):
		wndMgr.SetColor(self.hWnd, color)

class Bar(Window):

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterBar(self, layer)

	def SetColor(self, color):
		wndMgr.SetColor(self.hWnd, color)

class Line(Window):

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterLine(self, layer)

	def SetColor(self, color):
		wndMgr.SetColor(self.hWnd, color)

class SlotBar(Window):

	def __init__(self):
		Window.__init__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterBar3D(self, layer)

## Same with SlotBar
class Bar3D(Window):

	def __init__(self):
		Window.__init__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterBar3D(self, layer)

	def SetColor(self, left, right, center):
		wndMgr.SetColor(self.hWnd, left, right, center)

class SlotWindow(Window):

	def __init__(self):
		Window.__init__(self)

		self.StartIndex = 0

		self.eventSelectEmptySlot = None
		self.eventSelectItemSlot = None
		self.eventUnselectEmptySlot = None
		self.eventUnselectItemSlot = None
		self.eventUseSlot = None
		self.eventOverInItem = None
		self.eventOverOutItem = None
		self.eventPressedSlotButton = None

	def __del__(self):
		Window.__del__(self)

		self.eventSelectEmptySlot = None
		self.eventSelectItemSlot = None
		self.eventUnselectEmptySlot = None
		self.eventUnselectItemSlot = None
		self.eventUseSlot = None
		self.eventOverInItem = None
		self.eventOverOutItem = None
		self.eventPressedSlotButton = None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterSlotWindow(self, layer)

	def SetSlotStyle(self, style):
		wndMgr.SetSlotStyle(self.hWnd, style)

	def HasSlot(self, slotIndex):
		return wndMgr.HasSlot(self.hWnd, slotIndex)

	def SetSlotBaseImage(self, imageFileName, r, g, b, a):
		wndMgr.SetSlotBaseImage(self.hWnd, imageFileName, r, g, b, a)

	def SetCoverButton(self,\
						slotIndex,\
						upName="d:/ymir work/ui/public/slot_cover_button_01.sub",\
						overName="d:/ymir work/ui/public/slot_cover_button_02.sub",\
						downName="d:/ymir work/ui/public/slot_cover_button_03.sub",\
						disableName="d:/ymir work/ui/public/slot_cover_button_04.sub",\
						LeftButtonEnable = False,\
						RightButtonEnable = True):
		wndMgr.SetCoverButton(self.hWnd, slotIndex, upName, overName, downName, disableName, LeftButtonEnable, RightButtonEnable)

	def EnableCoverButton(self, slotIndex):
		wndMgr.EnableCoverButton(self.hWnd, slotIndex)

	def DisableCoverButton(self, slotIndex):
		wndMgr.DisableCoverButton(self.hWnd, slotIndex)

	def SetAlwaysRenderCoverButton(self, slotIndex, bAlwaysRender = True):
		wndMgr.SetAlwaysRenderCoverButton(self.hWnd, slotIndex, bAlwaysRender)

	def AppendSlotButton(self, upName, overName, downName):
		wndMgr.AppendSlotButton(self.hWnd, upName, overName, downName)

	def ShowSlotButton(self, slotNumber):
		wndMgr.ShowSlotButton(self.hWnd, slotNumber)

	def HideAllSlotButton(self):
		wndMgr.HideAllSlotButton(self.hWnd)

	def AppendRequirementSignImage(self, filename):
		wndMgr.AppendRequirementSignImage(self.hWnd, filename)

	def ShowRequirementSign(self, slotNumber):
		wndMgr.ShowRequirementSign(self.hWnd, slotNumber)

	def HideRequirementSign(self, slotNumber):
		wndMgr.HideRequirementSign(self.hWnd, slotNumber)

	if app.ENABLE_HIGHLIGHT_SLOT_SYSTEM_EX:
		def ActivateSlot(self, slotNumber, colorType = wndMgr.COLOR_TYPE_WHITE):
			wndMgr.ActivateSlot(self.hWnd, slotNumber, colorType)
	else:
		def ActivateSlot(self, slotNumber):
			wndMgr.ActivateSlot(self.hWnd, slotNumber)

	def DeactivateSlot(self, slotNumber):
		wndMgr.DeactivateSlot(self.hWnd, slotNumber)

	if app.ENABLE_HIGHLIGHT_SLOT_NEW_EFFECT:
		def ActivateSlotEffect(self, slotNumber, colorType = wndMgr.COLOR_TYPE_WHITE):
			wndMgr.ActivateSlotEffect(self.hWnd, slotNumber, colorType)

		def DeactivateSlotEffect(self, slotNumber):
			wndMgr.DeactivateSlotEffect(self.hWnd, slotNumber)

	def ShowSlotBaseImage(self, slotNumber):
		wndMgr.ShowSlotBaseImage(self.hWnd, slotNumber)

	def HideSlotBaseImage(self, slotNumber):
		wndMgr.HideSlotBaseImage(self.hWnd, slotNumber)

	def SAFE_SetButtonEvent(self, button, state, event):
		if "LEFT"==button:
			if "EMPTY"==state:
				self.eventSelectEmptySlot=__mem_func__(event)
			elif "EXIST"==state:
				self.eventSelectItemSlot=__mem_func__(event)
			elif "ALWAYS"==state:
				self.eventSelectEmptySlot=__mem_func__(event)
				self.eventSelectItemSlot=__mem_func__(event)
		elif "RIGHT"==button:
			if "EMPTY"==state:
				self.eventUnselectEmptySlot=__mem_func__(event)
			elif "EXIST"==state:
				self.eventUnselectItemSlot=__mem_func__(event)
			elif "ALWAYS"==state:
				self.eventUnselectEmptySlot=__mem_func__(event)
				self.eventUnselectItemSlot=__mem_func__(event)

	def SetSelectEmptySlotEvent(self, empty):
		self.eventSelectEmptySlot = empty

	def SetSelectItemSlotEvent(self, item):
		self.eventSelectItemSlot = item

	def SetUnselectEmptySlotEvent(self, empty):
		self.eventUnselectEmptySlot = empty

	def SetUnselectItemSlotEvent(self, item):
		self.eventUnselectItemSlot = item

	def SetUseSlotEvent(self, use):
		self.eventUseSlot = use

	def SetOverInItemEvent(self, event):
		self.eventOverInItem = event

	def SetOverOutItemEvent(self, event):
		self.eventOverOutItem = event

	def SetPressedSlotButtonEvent(self, event):
		self.eventPressedSlotButton = event

	def GetSlotCount(self):
		return wndMgr.GetSlotCount(self.hWnd)

	def SetUseMode(self, flag):
		wndMgr.SetUseMode(self.hWnd, flag)

	def SetUsableItem(self, flag):
		wndMgr.SetUsableItem(self.hWnd, flag)

	## Slot
	if app.ENABLE_SLOT_WINDOW_EX:
		def IsActivatedSlot(self, slotNumber):
			return wndMgr.IsActivatedSlot(self.hWnd, slotNumber)

		def GetSlotCoolTime(self, slotIndex):
			return wndMgr.GetSlotCoolTime(self.hWnd, slotIndex)

	def SetSlotCoolTime(self, slotIndex, coolTime, elapsedTime = 0.0):
		wndMgr.SetSlotCoolTime(self.hWnd, slotIndex, coolTime, elapsedTime)

	if app.WJ_ENABLE_TRADABLE_ICON:
		def SetCanMouseEventSlot(self, slotIndex):
			wndMgr.SetCanMouseEventSlot(self.hWnd, slotIndex)

		def SetCantMouseEventSlot(self, slotIndex):
			wndMgr.SetCantMouseEventSlot(self.hWnd, slotIndex)

		def SetUsableSlotOnTopWnd(self, slotIndex):
			wndMgr.SetUsableSlotOnTopWnd(self.hWnd, slotIndex)

		def SetUnusableSlotOnTopWnd(self, slotIndex):
			wndMgr.SetUnusableSlotOnTopWnd(self.hWnd, slotIndex)

	def DisableSlot(self, slotIndex):
		wndMgr.DisableSlot(self.hWnd, slotIndex)

	def EnableSlot(self, slotIndex):
		wndMgr.EnableSlot(self.hWnd, slotIndex)

	def LockSlot(self, slotIndex):
		wndMgr.LockSlot(self.hWnd, slotIndex)

	def UnlockSlot(self, slotIndex):
		wndMgr.UnlockSlot(self.hWnd, slotIndex)

	def RefreshSlot(self):
		wndMgr.RefreshSlot(self.hWnd)

	def ClearSlot(self, slotNumber):
		wndMgr.ClearSlot(self.hWnd, slotNumber)

	def ClearAllSlot(self):
		wndMgr.ClearAllSlot(self.hWnd)

	def AppendSlot(self, index, x, y, width, height):
		wndMgr.AppendSlot(self.hWnd, index, x, y, width, height)

	def SetSlot(self, slotIndex, itemIndex, width, height, icon, diffuseColor = (1.0, 1.0, 1.0, 1.0)):
		wndMgr.SetSlot(self.hWnd, slotIndex, itemIndex, width, height, icon, diffuseColor)

	def SetSlotCount(self, slotNumber, count):
		wndMgr.SetSlotCount(self.hWnd, slotNumber, count)

	def SetSlotCountNew(self, slotNumber, grade, count):
		wndMgr.SetSlotCountNew(self.hWnd, slotNumber, grade, count)

	def SetItemSlot(self, renderingSlotNumber, ItemIndex, ItemCount = 0, diffuseColor = (1.0, 1.0, 1.0, 1.0)):
		if 0 == ItemIndex or None == ItemIndex:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		item.SelectItem(ItemIndex)
		itemIcon = item.GetIconImage()

		item.SelectItem(ItemIndex)
		(width, height) = item.GetItemSize()

		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, ItemIndex, width, height, itemIcon, diffuseColor)
		wndMgr.SetSlotCount(self.hWnd, renderingSlotNumber, ItemCount)

	def SetSkillSlot(self, renderingSlotNumber, skillIndex, skillLevel):

		skillIcon = skill.GetIconImage(skillIndex)

		if 0 == skillIcon:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, skillIndex, 1, 1, skillIcon)
		wndMgr.SetSlotCount(self.hWnd, renderingSlotNumber, skillLevel)

	def SetSkillSlotNew(self, renderingSlotNumber, skillIndex, skillGrade, skillLevel):

		skillIcon = skill.GetIconImageNew(skillIndex, skillGrade)

		if 0 == skillIcon:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, skillIndex, 1, 1, skillIcon)

	def SetEmotionSlot(self, renderingSlotNumber, emotionIndex):
		import player
		icon = player.GetEmotionIconImage(emotionIndex)

		if 0 == icon:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, emotionIndex, 1, 1, icon)

	## Event
	def OnSelectEmptySlot(self, slotNumber):
		if self.eventSelectEmptySlot:
			self.eventSelectEmptySlot(slotNumber)

	def OnSelectItemSlot(self, slotNumber):
		if self.eventSelectItemSlot:
			self.eventSelectItemSlot(slotNumber)

	def OnUnselectEmptySlot(self, slotNumber):
		if self.eventUnselectEmptySlot:
			self.eventUnselectEmptySlot(slotNumber)

	def OnUnselectItemSlot(self, slotNumber):
		if self.eventUnselectItemSlot:
			self.eventUnselectItemSlot(slotNumber)

	def OnUseSlot(self, slotNumber):
		if self.eventUseSlot:
			self.eventUseSlot(slotNumber)

	def OnOverInItem(self, slotNumber):
		if self.eventOverInItem:
			self.eventOverInItem(slotNumber)

	def OnOverOutItem(self):
		if self.eventOverOutItem:
			self.eventOverOutItem()

	def OnPressedSlotButton(self, slotNumber):
		if self.eventPressedSlotButton:
			self.eventPressedSlotButton(slotNumber)

	def GetStartIndex(self):
		return 0

	if app.ENABLE_ITEM_MANAGER:
		def SetUsableSlotOnTopWndEx(self, slotIndex):
			wndMgr.SetUsableSlotOnTopWndEx(self.hWnd, slotIndex)
	
		def SetUnusableSlotOnTopWndEx(self, slotIndex):
			wndMgr.SetUnusableSlotOnTopWndEx(self.hWnd, slotIndex)

class GridSlotWindow(SlotWindow):

	def __init__(self):
		SlotWindow.__init__(self)

		self.startIndex = 0

	def __del__(self):
		SlotWindow.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterGridSlotWindow(self, layer)

	def ArrangeSlot(self, StartIndex, xCount, yCount, xSize, ySize, xBlank, yBlank):

		self.startIndex = StartIndex

		wndMgr.ArrangeSlot(self.hWnd, StartIndex, xCount, yCount, xSize, ySize, xBlank, yBlank)
		self.startIndex = StartIndex

	def GetStartIndex(self):
		return self.startIndex

class TitleBar(Window):

	BLOCK_WIDTH = 32
	BLOCK_HEIGHT = 23
	CENTER_WIDTH = 483
	
	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")

	def __del__(self):
		Window.__del__(self)

	def MakeTitleBar(self, width, color):

		## 현재 Color는 사용하고 있지 않음

		width = max(64, width)

		imgLeft = ImageBox()
		#imgCenterImage = ExpandedImageBox()
		imgCenter = ExpandedImageBox()
		imgRight = ImageBox()
		imgLeft.AddFlag("not_pick")
		#imgCenterImage.AddFlag("not_pick")
		imgCenter.AddFlag("not_pick")
		imgRight.AddFlag("not_pick")
		imgLeft.SetParent(self)
		imgCenter.SetParent(self)
		#imgCenterImage.SetParent(self)
		imgRight.SetParent(self)
		imgLeft.LoadImage("d:/ymir work/ui/pattern/titlebar_left.tga")
		# imgCenterImage.LoadImage("d:/ymir work/ui/pattern/titlebar_center_scale.png")
		imgCenter.LoadImage("d:/ymir work/ui/pattern/titlebar_center.tga")
		imgRight.LoadImage("d:/ymir work/ui/pattern/titlebar_right.tga")

		imgLeft.Show()
		imgCenter.Show()
		imgRight.Show()
		#imgCenterImage.Show()

		btnClose = Button()
		btnClose.SetParent(self)
		btnClose.SetUpVisual("d:/ymir work/ui/pattern/close_btn_01.png")
		btnClose.SetOverVisual("d:/ymir work/ui/pattern/close_btn_02.png")
		btnClose.SetDownVisual("d:/ymir work/ui/pattern/close_btn_03.png")
		btnClose.SetToolTipText(localeInfo.UI_CLOSE, 0, -23)
		btnClose.Show()

		self.imgLeft = imgLeft
		self.imgCenter = imgCenter
		#self.imgCenterImage = imgCenterImage
		self.imgRight = imgRight
		self.btnClose = btnClose

		self.SetWidth(width)

	def SetWidth(self, width):
		#self.imgCenterImage.SetPosition(14, 2)
		#self.imgCenterImage.SetScale(float((width) - self.CENTER_WIDTH - 28) / self.CENTER_WIDTH + 1, 1)
		
		self.imgCenter.SetRenderingRect(0.0, 0.0, float((width - self.BLOCK_WIDTH*2) - self.BLOCK_WIDTH) / self.BLOCK_WIDTH, 0.0)
		self.imgCenter.SetPosition(self.BLOCK_WIDTH, 0)

		self.imgRight.SetPosition(width - self.BLOCK_WIDTH, 0)
		self.btnClose.SetPosition(width - self.btnClose.GetWidth(), -1)
		self.SetSize(width, self.BLOCK_HEIGHT)

	def SetCloseEvent(self, event):
		self.btnClose.SetEvent(event)

	def CloseButtonHide(self) :
		self.imgRight.LoadImage("d:/ymir work/ui/pattern/titlebar_right_02.tga")
		self.btnClose.Hide()

# FinalCore Interface
class TitleBarFinalCoreSmall(Window):

	BLOCK_HEIGHT = 56

	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")
		self.closeEvent = None

	def __del__(self):
		Window.__del__(self)

	def MakeTitleBar(self, width, color):
		width = max(64, width)

		imgRightTop = ImageBox()
		imgLeftTop = ImageBox()
		# imgStretch = ExpandedImageBox()
		# imgTitleBarIcon = ImageBox()

		imgRightTop.AddFlag("not_pick")
		imgLeftTop.AddFlag("not_pick")
		# imgStretch.AddFlag("not_pick")
		# imgTitleBarIcon.AddFlag("not_pick")

		imgRightTop.SetParent(self)
		imgLeftTop.SetParent(self)
		# imgStretch.SetParent(self)
		# imgTitleBarIcon.SetParent(self)

		imgRightTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_corner_righttop.tga")
		imgLeftTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_line_top_short.tga")
		# imgStretch.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_stretch.tga")
		# imgTitleBarIcon.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_icon.tga")

		imgRightTop.Show()
		imgLeftTop.Show()
		# imgStretch.Show()
		# imgTitleBarIcon.Show()
		
		btnClose = Button()
		btnClose.SetParent(self)
		btnClose.AddFlag("float")
		btnClose.SetUpVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_01.tga")
		btnClose.SetOverVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_02.tga")
		btnClose.SetDownVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_03.tga")
		btnClose.SetToolTipText(localeInfo.UI_CLOSE, 0, -11)
		btnClose.Show()

		self.imgRightTop = imgRightTop
		self.imgLeftTop = imgLeftTop
		# self.imgStretch = imgStretch
		# self.imgTitleBarIcon = imgTitleBarIcon
		self.btnClose = btnClose

		self.SetWidth(width)

	def SetWidth(self, width):
		self.imgRightTop.SetPosition(99, 0)
		self.imgLeftTop.SetPosition(0, 0)
		self.btnClose.SetPosition(113, 18)
		# self.imgStretch.SetPosition(5, 23)
		# self.imgTitleBarIcon.SetPosition(7, 22)

		self.SetSize(width, self.BLOCK_HEIGHT)

	def DeactivateCloseButton(self):
		self.btnClose.Hide()
		self.SetWidth(self.GetWidth())

	def __ClickCloseButton(self):
		if self.closeEvent:
			self.closeEvent()

	def SetCloseEvent(self, event):
		self.closeEvent = event
		if self.btnClose:
			self.btnClose.SetEvent(self.__ClickCloseButton)

	def GetCloseEvent(self):
		return self.closeEvent

	def SetIconImage(self, x):
		return

	def SetStretchImage(self, x):
		return

# FinalCore Interface
class TitleBarFinalCoreSmall2(Window):

	BLOCK_HEIGHT = 42

	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")

	def __del__(self):
		Window.__del__(self)

	def MakeTitleBar(self, width, color):
		width = max(64, width)

		# imgRightTop = ImageBox()
		# imgLeftTop = ImageBox()
		# imgStretch = ExpandedImageBox()
		# imgTitleBarIcon = ImageBox()

		# imgRightTop.AddFlag("not_pick")
		# imgLeftTop.AddFlag("not_pick")
		# imgStretch.AddFlag("not_pick")
		# imgTitleBarIcon.AddFlag("not_pick")

		# imgRightTop.SetParent(self)
		# imgLeftTop.SetParent(self)
		# imgStretch.SetParent(self)
		# imgTitleBarIcon.SetParent(self)

		# imgRightTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_corner_righttop.tga")
		# imgLeftTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_line_top_short.tga")
		# imgStretch.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_stretch.tga")
		# imgTitleBarIcon.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_icon.tga")

		# imgRightTop.Show()
		# imgLeftTop.Show()
		# imgStretch.Show()
		# imgTitleBarIcon.Show()
		
		# btnClose = Button()
		# btnClose.SetParent(self)
		# btnClose.SetUpVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_01.tga")
		# btnClose.SetOverVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_02.tga")
		# btnClose.SetDownVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_03.tga")
		# btnClose.SetToolTipText(localeInfo.UI_CLOSE, 0, -11)
		# btnClose.Show()

		# self.imgRightTop = imgRightTop
		# self.imgLeftTop = imgLeftTop
		# self.imgStretch = imgStretch
		# self.imgTitleBarIcon = imgTitleBarIcon
		# self.btnClose = btnClose

		self.SetWidth(width)

	def SetWidth(self, width):
		# self.imgRightTop.SetPosition(99, 0)
		# self.imgLeftTop.SetPosition(0, 0)
		# self.btnClose.SetPosition(113, 18)
		# self.imgStretch.SetPosition(5, 23)
		# self.imgTitleBarIcon.SetPosition(7, 22)

		self.SetSize(width, self.BLOCK_HEIGHT)

	def DeactivateCloseButton(self):
		# self.btnClose.Hide()
		self.SetWidth(self.GetWidth())
		return

	def SetCloseEvent(self, event):
		# self.btnClose.SetEvent(event)
		return

	def GetCloseEvent(self):
		return self.btnClose.GetEvent()

	def SetIconImage(self, x):
		# self.imgTitleBarIcon.SetPosition(x, 22)
		return

	def SetStretchImage(self, x):
		# self.imgStretch.SetPosition(x, 23)
		return

class TitleBarFinalCoreMiddle(Window):

	BLOCK_HEIGHT = 42

	def __init__(self):
		Window.__init__(self)
		
		titleName = TextLine()
		titleName.SetPosition(0, 10)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()
		
		self.titleName = titleName

		self.AddFlag("attach")

	def __del__(self):
		Window.__del__(self)

	def MakeTitleBar(self, width, color):
		width = max(64, width)

		imgRightTop = ImageBox()
		imgLeftTop = ImageBox()
		imgStretch = ExpandedImageBox()
		imgTitleBarIcon = ImageBox()

		imgRightTop.AddFlag("not_pick")
		imgLeftTop.AddFlag("not_pick")
		imgStretch.AddFlag("not_pick")
		imgTitleBarIcon.AddFlag("not_pick")

		imgRightTop.SetParent(self)
		imgLeftTop.SetParent(self)
		imgStretch.SetParent(self)
		imgTitleBarIcon.SetParent(self)

		imgRightTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_corner_righttop.tga")
		imgLeftTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_line_top_medium.tga")
		imgStretch.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_stretch.tga")
		imgTitleBarIcon.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_icon.tga")

		imgRightTop.Show()
		imgLeftTop.Show()
		imgStretch.Show()
		imgTitleBarIcon.Show()
		
		btnClose = Button()
		btnClose.SetParent(self)
		btnClose.SetUpVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_01.tga")
		btnClose.SetOverVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_02.tga")
		btnClose.SetDownVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_03.tga")
		btnClose.SetToolTipText(localeInfo.UI_CLOSE, 0, -11)
		btnClose.Show()

		self.imgRightTop = imgRightTop
		self.imgLeftTop = imgLeftTop
		self.imgStretch = imgStretch
		self.imgTitleBarIcon = imgTitleBarIcon
		self.btnClose = btnClose
		
		self.titleName.SetParent(self.imgStretch)

		self.SetWidth(width)

	def SetWidth(self, width):
		self.imgRightTop.SetPosition(169, 0)
		self.imgLeftTop.SetPosition(0, 0)
		self.btnClose.SetPosition(183, 18)
		self.imgStretch.SetPosition(0, 23)
		self.imgTitleBarIcon.SetPosition(0, 22)

		self.SetSize(width, self.BLOCK_HEIGHT)

	def DeactivateCloseButton(self):
		self.btnClose.Hide()
		self.SetWidth(self.GetWidth())

	def SetCloseEvent(self, event):
		self.btnClose.SetEvent(event)

	def GetCloseEvent(self):
		return self.btnClose.GetEvent()

	def SetIconImage(self, x):
		self.imgTitleBarIcon.SetPosition(x, 22)

	def SetStretchImage(self, x):
		self.imgStretch.SetPosition(x, 23)
		
	def SetTitleName(self, name):
		self.titleName.SetText(name)

class TitleBarFinalCoreMiddleNoClose(TitleBarFinalCoreMiddle):
	def __init__(self):
		TitleBarFinalCoreMiddle.__init__(self)

	def MakeTitleBar(self, width, color):
		TitleBarFinalCoreMiddle.MakeTitleBar(self, width, color)
		self.DeactivateCloseButton()

class TitleBarFinalCoreLarge(Window):

	BLOCK_HEIGHT = 42

	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")

	def __del__(self):
		Window.__del__(self)

	def MakeTitleBar(self, width, color):
		width = max(64, width)

		imgRightTop = ImageBox()
		imgLeftTop = ImageBox()
		# imgStretch = ExpandedImageBox()
		# imgTitleBarIcon = ImageBox()

		imgRightTop.AddFlag("not_pick")
		imgLeftTop.AddFlag("not_pick")
		# imgStretch.AddFlag("not_pick")
		# imgTitleBarIcon.AddFlag("not_pick")

		imgRightTop.SetParent(self)
		imgLeftTop.SetParent(self)
		# imgStretch.SetParent(self)
		# imgTitleBarIcon.SetParent(self)

		imgRightTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_corner_righttop.tga")
		imgLeftTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_line_top_long.tga")
		# imgStretch.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_stretch.tga")
		# imgTitleBarIcon.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_icon.tga")

		imgRightTop.Show()
		imgLeftTop.Show()
		# imgStretch.Show()
		# imgTitleBarIcon.Show()
		
		btnClose = Button()
		btnClose.SetParent(self)
		btnClose.SetUpVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_01.tga")
		btnClose.SetOverVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_02.tga")
		btnClose.SetDownVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_03.tga")
		btnClose.SetToolTipText(localeInfo.UI_CLOSE, 0, -11)
		btnClose.Show()

		self.imgRightTop = imgRightTop
		self.imgLeftTop = imgLeftTop
		# self.imgStretch = imgStretch
		# self.imgTitleBarIcon = imgTitleBarIcon
		self.btnClose = btnClose

		self.SetWidth(width)

	def SetWidth(self, width):
		self.imgRightTop.SetPosition(236, 0)
		self.imgLeftTop.SetPosition(0, 0)
		self.btnClose.SetPosition(250, 18)
		# self.imgStretch.SetPosition(5, 23)
		# self.imgTitleBarIcon.SetPosition(7, 22)

		self.SetSize(width, self.BLOCK_HEIGHT)

	def DeactivateCloseButton(self):
		self.btnClose.Hide()
		self.SetWidth(self.GetWidth())

	def SetCloseEvent(self, event):
		self.btnClose.SetEvent(event)

	def GetCloseEvent(self):
		return self.btnClose.GetEvent()

	def SetIconImage(self, x):
		# self.imgTitleBarIcon.SetPosition(x, 22)
		return

	def SetStretchImage(self, x):
		# self.imgStretch.SetPosition(x, 23)
		return

	def __ClickCloseButton(self):
		if self.closeEvent:
			self.closeEvent()

	def SetCloseEvent(self, event):
		self.closeEvent = event
		if self.btnClose:
			self.btnClose.SetEvent(self.__ClickCloseButton)


class TitleBarFinalCoreLarge500(Window):

	BLOCK_HEIGHT = 42

	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")

	def __del__(self):
		Window.__del__(self)

	def MakeTitleBar(self, width, color):
		width = max(64, width)

		imgRightTop = ImageBox()
		imgLeftTop = ImageBox()
		# imgStretch = ExpandedImageBox()
		# imgTitleBarIcon = ImageBox()

		imgRightTop.AddFlag("not_pick")
		imgLeftTop.AddFlag("not_pick")
		# imgStretch.AddFlag("not_pick")
		# imgTitleBarIcon.AddFlag("not_pick")

		imgRightTop.SetParent(self)
		imgLeftTop.SetParent(self)
		# imgStretch.SetParent(self)
		# imgTitleBarIcon.SetParent(self)

		imgRightTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_corner_righttop.tga")
		imgLeftTop.LoadImage("locale/common/ui/interface/finalcore3/title_bar/board_border_line_top_long.tga")
		# imgStretch.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_stretch.tga")
		# imgTitleBarIcon.LoadImage("locale/common/ui/interface/finalcore3/title_bar/titlebar_icon.tga")

		imgRightTop.Show()
		imgLeftTop.Show()
		# imgStretch.Show()
		# imgTitleBarIcon.Show()
		
		btnClose = Button()
		btnClose.SetParent(self)
		btnClose.SetUpVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_01.tga")
		btnClose.SetOverVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_02.tga")
		btnClose.SetDownVisual("locale/common/ui/interface/finalcore3/title_bar/btn_close_03.tga")
		btnClose.SetToolTipText(localeInfo.UI_CLOSE, 0, -11)
		btnClose.Show()

		self.imgRightTop = imgRightTop
		self.imgLeftTop = imgLeftTop
		# self.imgStretch = imgStretch
		# self.imgTitleBarIcon = imgTitleBarIcon
		self.btnClose = btnClose

		self.SetWidth(width)

	def SetWidth(self, width):
		self.imgRightTop.SetPosition(236, 0)
		self.imgLeftTop.SetPosition(0, 0)
		self.btnClose.SetPosition(250, 18)
		# self.imgStretch.SetPosition(5, 23)
		# self.imgTitleBarIcon.SetPosition(7, 22)

		self.SetSize(width, self.BLOCK_HEIGHT)

	def DeactivateCloseButton(self):
		self.btnClose.Hide()
		self.SetWidth(self.GetWidth())

	def SetCloseEvent(self, event):
		self.btnClose.SetEvent(event)

	def GetCloseEvent(self):
		return self.btnClose.GetEvent()

	def SetIconImage(self, x):
		# self.imgTitleBarIcon.SetPosition(x, 22)
		return

	def SetStretchImage(self, x):
		# self.imgStretch.SetPosition(x, 23)
		return

	def __ClickCloseButton(self):
		if self.closeEvent:
			self.closeEvent()

	def SetCloseEvent(self, event):
		self.closeEvent = event
		if self.btnClose:
			self.btnClose.SetEvent(self.__ClickCloseButton)

class HorizontalBar(Window):

	BLOCK_WIDTH = 32
	BLOCK_HEIGHT = 17

	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")

	def __del__(self):
		Window.__del__(self)

	def Create(self, width):

		width = max(96, width)

		imgLeft = ImageBox()
		imgLeft.SetParent(self)
		imgLeft.AddFlag("not_pick")
		imgLeft.LoadImage("d:/ymir work/ui/pattern/horizontalbar_left.tga")
		imgLeft.Show()

		imgCenter = ExpandedImageBox()
		imgCenter.SetParent(self)
		imgCenter.AddFlag("not_pick")
		imgCenter.LoadImage("d:/ymir work/ui/pattern/horizontalbar_center.tga")
		imgCenter.Show()

		imgRight = ImageBox()
		imgRight.SetParent(self)
		imgRight.AddFlag("not_pick")
		imgRight.LoadImage("d:/ymir work/ui/pattern/horizontalbar_right.tga")
		imgRight.Show()

		self.imgLeft = imgLeft
		self.imgCenter = imgCenter
		self.imgRight = imgRight
		self.SetWidth(width)

	def SetWidth(self, width):
		self.imgCenter.SetRenderingRect(0.0, 0.0, float((width - self.BLOCK_WIDTH*2) - self.BLOCK_WIDTH) / self.BLOCK_WIDTH, 0.0)
		self.imgCenter.SetPosition(self.BLOCK_WIDTH, 0)
		self.imgRight.SetPosition(width - self.BLOCK_WIDTH, 0)
		self.SetSize(width, self.BLOCK_HEIGHT)

class Gauge(Window):

	SLOT_WIDTH = 16
	SLOT_HEIGHT = 7

	GAUGE_TEMPORARY_PLACE = 12
	GAUGE_WIDTH = 16

	def __init__(self):
		Window.__init__(self)
		self.width = 0
	def __del__(self):
		Window.__del__(self)

	def MakeGauge(self, width, color):

		self.width = max(48, width)

		imgSlotLeft = ImageBox()
		imgSlotLeft.SetParent(self)
		imgSlotLeft.LoadImage("d:/ymir work/ui/pattern/gauge_slot_left.tga")
		imgSlotLeft.Show()

		imgSlotRight = ImageBox()
		imgSlotRight.SetParent(self)
		imgSlotRight.LoadImage("d:/ymir work/ui/pattern/gauge_slot_right.tga")
		imgSlotRight.Show()
		imgSlotRight.SetPosition(width - self.SLOT_WIDTH, 0)

		imgSlotCenter = ExpandedImageBox()
		imgSlotCenter.SetParent(self)
		imgSlotCenter.LoadImage("d:/ymir work/ui/pattern/gauge_slot_center.tga")
		imgSlotCenter.Show()
		imgSlotCenter.SetRenderingRect(0.0, 0.0, float((width - self.SLOT_WIDTH*2) - self.SLOT_WIDTH) / self.SLOT_WIDTH, 0.0)
		imgSlotCenter.SetPosition(self.SLOT_WIDTH, 0)

		imgGauge = ExpandedImageBox()
		imgGauge.SetParent(self)
		imgGauge.LoadImage("d:/ymir work/ui/pattern/gauge_" + color + ".tga")
		imgGauge.Show()
		imgGauge.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
		imgGauge.SetPosition(self.GAUGE_TEMPORARY_PLACE, 0)

		imgSlotLeft.AddFlag("attach")
		imgSlotCenter.AddFlag("attach")
		imgSlotRight.AddFlag("attach")
		if app.BL_PARTY_UPDATE:
			imgSlotLeft.AddFlag("not_pick")
			imgSlotCenter.AddFlag("not_pick")
			imgSlotRight.AddFlag("not_pick")
			imgGauge.AddFlag("not_pick")

		self.imgLeft = imgSlotLeft
		self.imgCenter = imgSlotCenter
		self.imgRight = imgSlotRight
		self.imgGauge = imgGauge

		self.SetSize(width, self.SLOT_HEIGHT)

	def SetPercentage(self, curValue, maxValue):

		# PERCENTAGE_MAX_VALUE_ZERO_DIVISION_ERROR
		if maxValue > 0.0:
			percentage = min(1.0, float(curValue)/float(maxValue))
		else:
			percentage = 0.0
		# END_OF_PERCENTAGE_MAX_VALUE_ZERO_DIVISION_ERROR

		gaugeSize = -1.0 + float(self.width - self.GAUGE_TEMPORARY_PLACE*2) * percentage / self.GAUGE_WIDTH
		self.imgGauge.SetRenderingRect(0.0, 0.0, gaugeSize, 0.0)

	if app.BL_PARTY_UPDATE:	
		def GaugeImgBoxAddFlag(self, flag):
			self.imgLeft.AddFlag(flag)
			self.imgCenter.AddFlag(flag)
			self.imgRight.AddFlag(flag)
			self.imgGauge.AddFlag(flag)

if constInfo.ENABLE_ANIMATE_GUAGE:
	class AnimateGauge(Window):

		SLOT_WIDTH = 16
		SLOT_HEIGHT = 7

		GAUGE_TEMPORARY_PLACE = 12
		GAUGE_WIDTH = 16

		def __init__(self):
			Window.__init__(self)
			self.bg_curValue = 0
			self.bg_targetValue = None
			self.width = 0

		def __del__(self):
			Window.__del__(self)

		def MakeGauge(self, width, color):

			self.width = max(48, width)

			imgSlotLeft = ImageBox()
			imgSlotLeft.SetParent(self)
			imgSlotLeft.LoadImage("d:/ymir work/ui/pattern/gauge_slot_left.tga")
			imgSlotLeft.Show()

			imgSlotRight = ImageBox()
			imgSlotRight.SetParent(self)
			imgSlotRight.LoadImage("d:/ymir work/ui/pattern/gauge_slot_right.tga")
			imgSlotRight.Show()
			imgSlotRight.SetPosition(width - self.SLOT_WIDTH, 0)

			imgSlotCenter = ExpandedImageBox()
			imgSlotCenter.SetParent(self)
			imgSlotCenter.LoadImage("d:/ymir work/ui/pattern/gauge_slot_center.tga")
			imgSlotCenter.Show()
			imgSlotCenter.SetRenderingRect(0.0, 0.0, float((width - self.SLOT_WIDTH*2) - self.SLOT_WIDTH) / self.SLOT_WIDTH, 0.0)
			imgSlotCenter.SetPosition(self.SLOT_WIDTH, 0)

			bg_imgGauge = ExpandedImageBox()
			bg_imgGauge.SetParent(self)
			bg_imgGauge.LoadImage("d:/ymir work/ui/pattern/gauge_darkred.tga")
			bg_imgGauge.Show()
			bg_imgGauge.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			bg_imgGauge.SetPosition(self.GAUGE_TEMPORARY_PLACE, 0)

			imgGauge = ExpandedImageBox()
			imgGauge.SetParent(self)
			imgGauge.LoadImage("d:/ymir work/ui/pattern/gauge_" + color + ".tga")
			imgGauge.Show()
			imgGauge.SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			imgGauge.SetPosition(self.GAUGE_TEMPORARY_PLACE, 0)

			imgSlotLeft.AddFlag("attach")
			imgSlotCenter.AddFlag("attach")
			imgSlotRight.AddFlag("attach")

			self.imgLeft = imgSlotLeft
			self.imgCenter = imgSlotCenter
			self.imgRight = imgSlotRight
			self.bg_imgGauge = bg_imgGauge
			self.imgGauge = imgGauge

			self.gaugeSize = 0.0

			self.SetSize(width, self.SLOT_HEIGHT)

		def SetPercentage(self, curValue, maxValue):
			self.curValue = float(curValue)
			self.maxValue = float(maxValue)

			# PERCENTAGE_MAX_VALUE_ZERO_DIVISION_ERROR
			if maxValue > 0.0:
				percentage = min(1.0, float(curValue)/float(maxValue))
			else:
				percentage = 0.0
			# END_OF_PERCENTAGE_MAX_VALUE_ZERO_DIVISION_ERROR

			self.gaugeSize = -1.0 + float(self.width - self.GAUGE_TEMPORARY_PLACE*2) * percentage / self.GAUGE_WIDTH
			self.imgGauge.SetRenderingRect(0.0, 0.0, self.gaugeSize, 0.0)

		def SetBGPercentage(self, curValue, maxValue):
			self.bg_curValue = float(curValue)
			self.bg_maxValue = float(maxValue)

			# PERCENTAGE_MAX_VALUE_ZERO_DIVISION_ERROR
			if maxValue > 0.0:
				percentage = min(1.0, float(curValue)/float(maxValue))
			else:
				percentage = 0.0
			# END_OF_PERCENTAGE_MAX_VALUE_ZERO_DIVISION_ERROR

			gaugeSize = -1.0 + float(self.width - self.GAUGE_TEMPORARY_PLACE*2) * percentage / self.GAUGE_WIDTH
			self.bg_imgGauge.SetRenderingRect(0.0, 0.0, gaugeSize, 0.0)

		def SetEasingPercentage(self, targetValue, maxValue):
			self.bg_targetValue = float(targetValue)
			self.bg_maxValue = float(maxValue)

		def SetColor(self, color):
			self.imgGauge.LoadImage("d:/ymir work/ui/pattern/gauge_" + color + ".tga")
			self.imgGauge.SetRenderingRect(0.0, 0.0, self.gaugeSize, 0.0)

		def OnUpdate(self):
			if self.bg_targetValue == None:
				return

			if self.bg_targetValue == self.bg_curValue:
				self.bg_targetValue = None
				return

			if self.bg_curValue > self.bg_targetValue:
				self.SetBGPercentage(max(self.bg_targetValue, self.bg_curValue - 0.5), self.bg_maxValue)
			else:
				self.SetBGPercentage(min(self.bg_targetValue, self.bg_curValue + 0.5), self.bg_maxValue)

# Finalcore3 Interface
class BoardFinalCore(Window):

	BOARD2_PATH = "locale/common/ui/interface/finalcore3/board"

	CORNER_WIDTH = 32
	CORNER_HEIGHT = 32
	LINE_WIDTH = 128
	LINE_HEIGHT = 128

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self):
		Window.__init__(self)

		self.MakeBoard("%s/Board_Corner_" % self.BOARD2_PATH, "%s/Board_Line_" % self.BOARD2_PATH)
		self.MakeBase()

	def MakeBoard(self, cornerPath, linePath):

		CornerFileNames = [ cornerPath+dir+".tga" for dir in ("LeftTop", "LeftBottom", "RightTop", "RightBottom", ) ]
		LineFileNames = [ linePath+dir+".tga" for dir in ("Left", "Right", "Top", "Bottom", ) ]

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def MakeBase(self):
		self.Base = ExpandedImageBox()
		self.Base.AddFlag("not_pick")
		self.Base.LoadImage("%s/Board_Base.tga" % self.BOARD2_PATH)
		self.Base.SetParent(self)
		self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Base.Show()

	def __del__(self):
		Window.__del__(self)

	def SetCloseEvent(self, event):
		self._closeEvent = event

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH

		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)

		if self.Base:
			self.Base.SetRenderingRect(0, 0, horizontalShowingPercentage, verticalShowingPercentage)


class Board(Window):

	CORNER_WIDTH = 32
	CORNER_HEIGHT = 32
	LINE_WIDTH = 128
	LINE_HEIGHT = 128

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self):
		Window.__init__(self)

		self.MakeBoard("d:/ymir work/ui/pattern/Board_Corner_", "d:/ymir work/ui/pattern/Board_Line_")
		self.MakeBase()

	def MakeBoard(self, cornerPath, linePath):

		CornerFileNames = [ cornerPath+dir+".tga" for dir in ("LeftTop", "LeftBottom", "RightTop", "RightBottom", ) ]
		LineFileNames = [ linePath+dir+".tga" for dir in ("Left", "Right", "Top", "Bottom", ) ]
		#DecoFileNames = [
			#"d:/ymir work/ui/pattern/deco_l_top.png",
			#"d:/ymir work/ui/pattern/deco_b_left.png",
			#"d:/ymir work/ui/pattern/deco_b_right.png",
			#"d:/ymir work/ui/pattern/deco_r_top.png"
			#]

		#self.Deco = []
		#for fileName in DecoFileNames:
			#Deco = ExpandedImageBox()
			#Deco.AddFlag("not_pick")
			#Deco.LoadImage(fileName)
			#Deco.SetParent(self)
			#Deco.SetPosition(0, 0)
			#Deco.Show()
			#self.Deco.append(Deco)

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def MakeBase(self):
		self.Base = ExpandedImageBox()
		self.Base.AddFlag("not_pick")
		self.Base.LoadImage("d:/ymir work/ui/pattern/Board_Base.tga")
		self.Base.SetParent(self)
		self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Base.Show()

	def __del__(self):
		Window.__del__(self)

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		#self.Deco[0].SetParent(self.Corners[self.LT])
		#self.Deco[0].SetPosition(-5, -2)
		#self.Deco[3].SetParent(self.Corners[self.RT])
		#self.Deco[3].SetPosition(-3, -2)
		#self.Deco[1].SetParent(self.Corners[self.LB])
		#self.Deco[1].SetPosition(-5, 4)
		#self.Deco[2].SetParent(self.Corners[self.RB])
		#self.Deco[2].SetPosition(-3, 3)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)

		if self.Base:
			self.Base.SetRenderingRect(0, 0, horizontalShowingPercentage, verticalShowingPercentage)

class TitleBarWithoutButton(Window):
	BLOCK_WIDTH = 32
	BLOCK_HEIGHT = 23
	CENTER_WIDTH = 483
	
	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")

	def __del__(self):
		Window.__del__(self)

	def MakeTitleBar(self, width, color):

		width = max(64, width)

		imgLeft = ImageBox()
		imgCenter = ExpandedImageBox()
		#imgCenterImage = ExpandedImageBox()
		imgRight = ImageBox()
		imgLeft.AddFlag("not_pick")
		imgCenter.AddFlag("not_pick")
		#imgCenterImage.AddFlag("not_pick")
		imgRight.AddFlag("not_pick")
		imgLeft.SetParent(self)
		imgCenter.SetParent(self)
		#imgCenterImage.SetParent(self)
		imgRight.SetParent(self)

		if localeInfo.IsARABIC():
			imgLeft.LoadImage("locale/ae/ui/pattern/titlebar_left.tga")
			imgCenter.LoadImage("locale/ae/ui/pattern/titlebar_center.tga")
			imgRight.LoadImage("locale/ae/ui/pattern/titlebar_right_without_x.tga")
		else:
			imgLeft.LoadImage("d:/ymir work/ui/pattern/titlebar_left.tga")
			imgCenter.LoadImage("d:/ymir work/ui/pattern/titlebar_center.tga")
			imgRight.LoadImage("d:/ymir work/ui/pattern/titlebar_right_without_x.tga")

		#imgCenterImage.LoadImage("d:/ymir work/ui/pattern/titlebar_center_scale.png")

		imgLeft.Show()
		imgCenter.Show()
		#imgCenterImage.Show()
		imgRight.Show()

		self.imgLeft = imgLeft
		self.imgCenter = imgCenter
		#self.imgCenterImage = imgCenterImage
		self.imgRight = imgRight

		self.SetWidth(width)

	def SetWidth(self, width):
		#self.imgCenterImage.SetPosition(14, 2)
		#self.imgCenterImage.SetScale(float((width) - self.CENTER_WIDTH - 28) / self.CENTER_WIDTH + 1, 1)
		
		self.imgCenter.SetRenderingRect(0.0, 0.0, float((width - self.BLOCK_WIDTH*2) - self.BLOCK_WIDTH) / self.BLOCK_WIDTH, 0.0)
		self.imgCenter.SetPosition(self.BLOCK_WIDTH, 0)
		self.imgRight.SetPosition(width - self.BLOCK_WIDTH, 0)
			
		self.SetSize(width, self.BLOCK_HEIGHT)

######################################################### fratello
PATH = "locale/common/ui/interface/finalcore3/title_bar/"

class BoardWithFinalCoreTitleBar(Board): #messenger

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreSmall()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(8, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(0, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar2(Board): #switchbot

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(88, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-80, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar3(Board): # eventmanager

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(238, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-230, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar4(Board): #vote4buff

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(28, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-20, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar5(Board): #itemshop

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(488, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-480, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar6(Board): #shopsearch

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(415, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-407, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)

class BoardWithFinalCoreTitleBar7(Board):

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton
		self.closeEvent = None

		titleBar = TitleBarFinalCoreLarge500()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(438, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-430, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		self.SetCloseEvent(self.Hide)

	def __del__(self):
		self.closeEvent = None
		self.titleBar = None
		self.titleName = None
		Board.__del__(self)

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		self.closeEvent = event
		if self.titleBar:
			self.titleBar.SetCloseEvent(event)

	def GetCloseEvent(self):
		return self.closeEvent


class BoardWithFinalCoreTitleBar8(Board): #atlasinfo

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreSmall2()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(45, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-40, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar9(Board): #system option

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(178, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-170, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar10(Board): #skillcolor

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(185, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-180, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)

class BoardWithFinalCoreTitleBar11(Board): #attr67addidialog

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(18, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-10, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar12(Board): #exchange money

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreSmall()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(10, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-5, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)

class BoardWithFinalCoreTitleBar13(Board): #uiexchange

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(18, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-10, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar12(Board): #pvp duell

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreSmall()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(58, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-50, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar13(Board): #global rewards

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(168, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-160, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar14(Board): #daily quest

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(48, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-40, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)

class BoardWithFinalCoreTitleBar15(Board): #privat shop

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(-18, -17)
		titleBar.Show()
		titleBar.SetTop()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-10, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		self.titleBar = None
		self.titleName = None
		Board.__del__(self)

	def SetSize(self, width, height):
		Board.SetSize(self, width, height)
		self.titleBar.SetPosition(-18, -17)
		self.titleBar.SetWidth(width + 18)
		self.titleBar.SetTop()
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton and self.titleBar:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar16(Board): #guild

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreLarge()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition( 68, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-60, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)


class BoardWithFinalCoreTitleBar17(Board): #guild

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreSmall()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(48, -17)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-40, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)

class BoardWithFinalCoreTitleBar18(Board): #eq viewer devilay

	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton

		titleBar = TitleBarFinalCoreSmall()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, None)
		titleBar.SetPosition(10, -18)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(-20, 29)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if not self.withButton:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if not self.withButton:
			self.titleBar.SetCloseEvent(event)

#####################################################################################################################

class BoardWithTitleBar(Board):
	def __init__(self, withButton = False):
		Board.__init__(self)

		self.withButton = withButton
		titleBar = TitleBar()
		if self.withButton is True:
			titleBar = TitleBarWithoutButton()
		titleBar.SetParent(self)
		titleBar.MakeTitleBar(0, "red")
		titleBar.SetPosition(8, 7)
		titleBar.Show()

		titleName = TextLine()
		titleName.SetParent(titleBar)
		titleName.SetPosition(0, 4)
		titleName.SetWindowHorizontalAlignCenter()
		titleName.SetHorizontalAlignCenter()
		titleName.Show()

		self.titleBar = titleBar
		self.titleName = titleName

		if self.withButton is False:
			self.SetCloseEvent(self.Hide)

	def __del__(self):
		Board.__del__(self)
		self.titleBar = None
		self.titleName = None

	def SetSize(self, width, height):
		self.titleBar.SetWidth(width - 15)
		#self.pickRestrictWindow.SetSize(width, height - 30)
		Board.SetSize(self, width, height)
		self.titleName.UpdateRect()

	def SetTitleColor(self, color):
		self.titleName.SetPackedFontColor(color)

	def SetTitleName(self, name):
		self.titleName.SetText(name)

	if app.ENABLE_CHEST_DROP_INFO:
		def SetTitleFont(self, font):
			self.titleName.SetFontName(font)

	def SetCloseEvent(self, event):
		if self.withButton is False:
			self.titleBar.SetCloseEvent(event)

class ThinBoard(Window):

	CORNER_WIDTH = 16
	CORNER_HEIGHT = 16
	LINE_WIDTH = 16
	LINE_HEIGHT = 16
	BOARD_COLOR = grp.GenerateColor(0.0509, 0.17254, 0.38823, 0.4)

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		CornerFileNames = [ "d:/ymir work/ui/pattern/ThinBoard_Corner_"+dir+".tga" for dir in ["LeftTop","LeftBottom","RightTop","RightBottom"] ]
		LineFileNames = [ "d:/ymir work/ui/pattern/ThinBoard_Line_"+dir+".tga" for dir in ["Left","Right","Top","Bottom"] ]

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("attach")
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("attach")
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		Base = Bar()
		Base.SetParent(self)
		Base.AddFlag("attach")
		Base.AddFlag("not_pick")
		Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		Base.SetColor(self.BOARD_COLOR)
		Base.Show()
		self.Base = Base

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def __del__(self):
		Window.__del__(self)

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Base.SetSize(width - self.CORNER_WIDTH*2, height - self.CORNER_HEIGHT*2)

	def ShowInternal(self):
		self.Base.Show()
		for wnd in self.Lines:
			wnd.Show()
		for wnd in self.Corners:
			wnd.Show()

	def HideInternal(self):
		self.Base.Hide()
		for wnd in self.Lines:
			wnd.Hide()
		for wnd in self.Corners:
			wnd.Hide()

class ThinBoardGold(Window):
	CORNER_WIDTH = 16
	CORNER_HEIGHT = 16
	LINE_WIDTH = 16
	LINE_HEIGHT = 16

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		CornerFileNames = [ "d:/ymir work/ui/pattern/thinboardgold/ThinBoard_Corner_"+dir+"_Gold.tga" for dir in ["LeftTop","LeftBottom","RightTop","RightBottom"] ]
		LineFileNames = [ "d:/ymir work/ui/pattern/thinboardgold/ThinBoard_Line_"+dir+"_Gold.tga" for dir in ["Left","Right","Top","Bottom"] ]

		self.MakeBase()

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("attach")
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("attach")
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def __del__(self):
		Window.__del__(self)

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH

		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)

		#self.Base.GetWidth()
		#self.Base.GetHeight()
		"""
			Defalt Width : 128, Height : 128
			0.0 > 128, 1.0 > 256
		"""
		if self.Base:
			self.Base.SetRenderingRect(0, 0, (float(width)-32)/float(self.Base.GetWidth()) - 1.0, (float(height)-32)/float(self.Base.GetHeight()) - 1.0)

	def MakeBase(self):
		self.Base = ExpandedImageBox()
		self.Base.AddFlag("not_pick")
		self.Base.LoadImage("d:/ymir work/ui/pattern/Board_Base.tga")
		self.Base.SetParent(self)
		self.Base.SetPosition(16, 16)
		self.Base.SetAlpha(0.8)
		self.Base.Show()

	def ShowInternal(self):
		self.Base.Show()
		for wnd in self.Lines:
			wnd.Show()
		for wnd in self.Corners:
			wnd.Show()

	def HideInternal(self):
		self.Base.Hide()
		for wnd in self.Lines:
			wnd.Hide()
		for wnd in self.Corners:
			wnd.Hide()

class ThinBoardCircle(Window):
	CORNER_WIDTH = 4
	CORNER_HEIGHT = 4
	LINE_WIDTH = 4
	LINE_HEIGHT = 4
	BOARD_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 1.0)

	LT = 0
	LB = 1
	RT = 2
	RB = 3
	L = 0
	R = 1
	T = 2
	B = 3

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		CornerFileNames = [ "d:/ymir work/ui/pattern/thinboardcircle/ThinBoard_Corner_"+dir+"_Circle.tga" for dir in ["LeftTop","LeftBottom","RightTop","RightBottom"] ]
		LineFileNames = [ "d:/ymir work/ui/pattern/thinboardcircle/ThinBoard_Line_"+dir+"_Circle.tga" for dir in ["Left","Right","Top","Bottom"] ]

		self.Corners = []
		for fileName in CornerFileNames:
			Corner = ExpandedImageBox()
			Corner.AddFlag("attach")
			Corner.AddFlag("not_pick")
			Corner.LoadImage(fileName)
			Corner.SetParent(self)
			Corner.SetPosition(0, 0)
			Corner.Show()
			self.Corners.append(Corner)

		self.Lines = []
		for fileName in LineFileNames:
			Line = ExpandedImageBox()
			Line.AddFlag("attach")
			Line.AddFlag("not_pick")
			Line.LoadImage(fileName)
			Line.SetParent(self)
			Line.SetPosition(0, 0)
			Line.Show()
			self.Lines.append(Line)

		Base = Bar()
		Base.SetParent(self)
		Base.AddFlag("attach")
		Base.AddFlag("not_pick")
		Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
		Base.SetColor(self.BOARD_COLOR)
		Base.Show()
		self.Base = Base

		self.ButtonText = None
		self.BonusId = 0

		self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
		self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

	def __del__(self):
		Window.__del__(self)

	def SetSize(self, width, height):

		width = max(self.CORNER_WIDTH*2, width)
		height = max(self.CORNER_HEIGHT*2, height)
		Window.SetSize(self, width, height)

		self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
		self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
		self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
		self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
		self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

		verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
		horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
		self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
		self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
		self.Base.SetSize(width - self.CORNER_WIDTH*2, height - self.CORNER_HEIGHT*2)

	def SetText(self, text):
		if not self.ButtonText:
			textLine = TextLine()
			textLine.SetParent(self)
			textLine.SetPosition(self.GetWidth()/2, self.GetHeight()/2)
			textLine.SetVerticalAlignCenter()
			textLine.SetHorizontalAlignCenter()
			textLine.Show()
			self.ButtonText = textLine

		self.ButtonText.SetText(text)

	def GetText(self):
		if not self.ButtonText:
			return ""
		return self.ButtonText.GetText()

	def SetBonusId(self, bnsId):
		self.BonusId = bnsId

	def GetBonusId(self):
		if self.BonusId != 0:
			return self.BonusId

	def ShowInternal(self):
		self.Base.Show()
		for wnd in self.Lines:
			wnd.Show()
		for wnd in self.Corners:
			wnd.Show()

	def HideInternal(self):
		self.Base.Hide()
		for wnd in self.Lines:
			wnd.Hide()
		for wnd in self.Corners:
			wnd.Hide()

class ScrollBar(Window):

	SCROLLBAR_WIDTH = 17
	SCROLLBAR_MIDDLE_HEIGHT = 9
	SCROLLBAR_BUTTON_WIDTH = 17
	SCROLLBAR_BUTTON_HEIGHT = 17
	MIDDLE_BAR_POS = 5
	MIDDLE_BAR_UPPER_PLACE = 3
	MIDDLE_BAR_DOWNER_PLACE = 4
	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE

	class MiddleBar(DragButton):
		def __init__(self):
			DragButton.__init__(self)
			self.AddFlag("movable")
			#self.AddFlag("restrict_x")

		def MakeImage(self):
			top = ImageBox()
			top.SetParent(self)
			top.LoadImage("d:/ymir work/ui/pattern/ScrollBar_Top.tga")
			top.SetPosition(0, 0)
			top.AddFlag("not_pick")
			top.Show()
			bottom = ImageBox()
			bottom.SetParent(self)
			bottom.LoadImage("d:/ymir work/ui/pattern/ScrollBar_Bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()

			middle = ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage("d:/ymir work/ui/pattern/ScrollBar_Middle.tga")
			middle.SetPosition(0, 4)
			middle.AddFlag("not_pick")
			middle.Show()

			self.top = top
			self.bottom = bottom
			self.middle = middle

		def SetSize(self, height):
			height = max(12, height)
			DragButton.SetSize(self, 10, height)
			self.bottom.SetPosition(0, height-4)

			height -= 4*3
			self.middle.SetRenderingRect(0, 0, 0, float(height)/4.0)

	def __init__(self):
		Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = lambda *arg: None
		self.lockFlag = False
		self.scrollStep = 0.20
		self.scroll_span = 0


		self.CreateScrollBar()

	def __del__(self):
		Window.__del__(self)

	def CreateScrollBar(self):
		barSlot = Bar3D()
		barSlot.SetParent(self)
		barSlot.AddFlag("not_pick")
		barSlot.Show()

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(12)

		upButton = Button()
		upButton.SetParent(self)
		upButton.SetEvent(__mem_func__(self.OnUp))
		upButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_up_button_01.sub")
		upButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_up_button_02.sub")
		upButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_up_button_03.sub")
		upButton.Show()

		downButton = Button()
		downButton.SetParent(self)
		downButton.SetEvent(__mem_func__(self.OnDown))
		downButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_down_button_01.sub")
		downButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_down_button_02.sub")
		downButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_down_button_03.sub")
		downButton.Show()

		self.upButton = upButton
		self.downButton = downButton
		self.middleBar = middleBar
		self.barSlot = barSlot

		self.SCROLLBAR_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
		self.SCROLLBAR_BUTTON_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_BUTTON_HEIGHT = self.upButton.GetHeight()

	if app.ENABLE_MOUSEWHEEL_EVENT:
		def OnMouseWheel(self, delta):
			if delta > 0:
				self.SetPos(self.curPos - (self.scrollStep/4))
				return True
			elif delta < 0:
				self.SetPos(self.curPos + (self.scrollStep/4))
				return True
			return False


	def Destroy(self):
		self.middleBar = None
		self.upButton = None
		self.downButton = None
		self.eventScroll = lambda *arg: None

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def SetMiddleBarSize(self, pageScale):
		realHeight = self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2
		self.SCROLLBAR_MIDDLE_HEIGHT = int(pageScale * float(realHeight))
		self.middleBar.SetSize(self.SCROLLBAR_MIDDLE_HEIGHT)
		self.pageSize = (self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)

	def SetScrollBarSize(self, height):
		self.pageSize = (height - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.upButton.SetPosition(0, 0)
		self.downButton.SetPosition(0, height - self.SCROLLBAR_BUTTON_HEIGHT)
		self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS, self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE, self.MIDDLE_BAR_POS+2, height - self.SCROLLBAR_BUTTON_HEIGHT*2 - self.TEMP_SPACE)
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)

		self.UpdateBarSlot()

	def UpdateBarSlot(self):
		self.barSlot.SetPosition(0, self.SCROLLBAR_BUTTON_HEIGHT)
		self.barSlot.SetSize(self.GetWidth() - 2, self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2 - 2)

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, int(newPos) + self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE)
		self.OnMove()

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep
		
	def SetSpan(self, span):
		self.scroll_span = span

	def IncreaseSpan(self, span):
		self.scroll_span += span
	
	def GetSpan(self):
		return self.scroll_span

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def OnMove(self):

		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLLBAR_BUTTON_HEIGHT - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)

		self.eventScroll()

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		pickedPos = yMouseLocalPosition - self.SCROLLBAR_BUTTON_HEIGHT - self.SCROLLBAR_MIDDLE_HEIGHT/2
		newPos = float(pickedPos) / float(self.pageSize)
		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False

class ThinScrollBar(ScrollBar):

	def CreateScrollBar(self):
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.SetUpVisual("d:/ymir work/ui/public/scrollbar_thin_middle_button_01.sub")
		middleBar.SetOverVisual("d:/ymir work/ui/public/scrollbar_thin_middle_button_02.sub")
		middleBar.SetDownVisual("d:/ymir work/ui/public/scrollbar_thin_middle_button_03.sub")

		upButton = Button()
		upButton.SetParent(self)
		upButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_thin_up_button_01.sub")
		upButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_thin_up_button_02.sub")
		upButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_thin_up_button_03.sub")
		upButton.SetEvent(__mem_func__(self.OnUp))
		upButton.Show()

		downButton = Button()
		downButton.SetParent(self)
		downButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_thin_down_button_01.sub")
		downButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_thin_down_button_02.sub")
		downButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_thin_down_button_03.sub")
		downButton.SetEvent(__mem_func__(self.OnDown))
		downButton.Show()

		self.middleBar = middleBar
		self.upButton = upButton
		self.downButton = downButton

		self.SCROLLBAR_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
		self.SCROLLBAR_BUTTON_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_BUTTON_HEIGHT = self.upButton.GetHeight()
		self.MIDDLE_BAR_POS = 0
		self.MIDDLE_BAR_UPPER_PLACE = 0
		self.MIDDLE_BAR_DOWNER_PLACE = 0
		self.TEMP_SPACE = 0

	def UpdateBarSlot(self):
		pass

class SmallThinScrollBar(ScrollBar):

	def CreateScrollBar(self):
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.SetUpVisual("d:/ymir work/ui/public/scrollbar_small_thin_middle_button_01.sub")
		middleBar.SetOverVisual("d:/ymir work/ui/public/scrollbar_small_thin_middle_button_01.sub")
		middleBar.SetDownVisual("d:/ymir work/ui/public/scrollbar_small_thin_middle_button_01.sub")

		upButton = Button()
		upButton.SetParent(self)
		upButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_small_thin_up_button_01.sub")
		upButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_small_thin_up_button_02.sub")
		upButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_small_thin_up_button_03.sub")
		upButton.SetEvent(__mem_func__(self.OnUp))
		upButton.Show()

		downButton = Button()
		downButton.SetParent(self)
		downButton.SetUpVisual("d:/ymir work/ui/public/scrollbar_small_thin_down_button_01.sub")
		downButton.SetOverVisual("d:/ymir work/ui/public/scrollbar_small_thin_down_button_02.sub")
		downButton.SetDownVisual("d:/ymir work/ui/public/scrollbar_small_thin_down_button_03.sub")
		downButton.SetEvent(__mem_func__(self.OnDown))
		downButton.Show()

		self.middleBar = middleBar
		self.upButton = upButton
		self.downButton = downButton

		self.SCROLLBAR_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
		self.SCROLLBAR_BUTTON_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_BUTTON_HEIGHT = self.upButton.GetHeight()
		self.MIDDLE_BAR_POS = 0
		self.MIDDLE_BAR_UPPER_PLACE = 0
		self.MIDDLE_BAR_DOWNER_PLACE = 0
		self.TEMP_SPACE = 0

	def UpdateBarSlot(self):
		pass

class SliderBar(Window):

	def __init__(self):
		Window.__init__(self)

		self.curPos = 1.0
		self.pageSize = 1.0
		self.eventChange = None

		self.__CreateBackGroundImage()
		self.__CreateCursor()

	def __del__(self):
		Window.__del__(self)

	def __CreateBackGroundImage(self):
		if app.ENABLE_NEW_GAMEOPTION:
			img = ExpandedImageBox()
		else:
			img = ImageBox()
		img.SetParent(self)
		img.LoadImage("d:/ymir work/ui/game/windows/sliderbar.sub")
		img.Show()
		self.backGroundImage = img

		##
		self.SetSize(self.backGroundImage.GetWidth(), self.backGroundImage.GetHeight())

	def __CreateCursor(self):
		cursor = DragButton()
		cursor.AddFlag("movable")
		cursor.AddFlag("restrict_y")
		cursor.SetParent(self)
		cursor.SetMoveEvent(__mem_func__(self.__OnMove))
		cursor.SetUpVisual("d:/ymir work/ui/game/windows/sliderbar_cursor.sub")
		cursor.SetOverVisual("d:/ymir work/ui/game/windows/sliderbar_cursor.sub")
		cursor.SetDownVisual("d:/ymir work/ui/game/windows/sliderbar_cursor.sub")
		cursor.Show()
		self.cursor = cursor

		##
		self.cursor.SetRestrictMovementArea(0, 0, self.backGroundImage.GetWidth(), 0)
		self.pageSize = self.backGroundImage.GetWidth() - self.cursor.GetWidth()

	def __OnMove(self):
		(xLocal, yLocal) = self.cursor.GetLocalPosition()
		self.curPos = float(xLocal) / float(self.pageSize)

		if self.eventChange:
			self.eventChange()

	def SetSliderPos(self, pos):
		self.curPos = pos
		self.cursor.SetPosition(int(self.pageSize * pos), 0)

	def GetSliderPos(self):
		return self.curPos

	def SetEvent(self, event):
		self.eventChange = event

	def Enable(self):
		self.cursor.Show()

	def Disable(self):
		self.cursor.Hide()

class ListBox(Window):

	TEMPORARY_PLACE = 3

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		self.overLine = -1
		self.selectedLine = -1
		self.width = 0
		self.height = 0
		self.stepSize = 17
		self.basePos = 0
		self.showLineCount = 0
		self.itemCenterAlign = True
		self.itemList = []
		self.keyDict = {}
		self.textDict = {}
		self.event = lambda *arg: None
	def __del__(self):
		Window.__del__(self)

	def SetWidth(self, width):
		self.SetSize(width, self.height)

	def SetSize(self, width, height):
		Window.SetSize(self, width, height)
		self.width = width
		self.height = height

	def SetTextCenterAlign(self, flag):
		self.itemCenterAlign = flag

	def SetBasePos(self, pos):
		self.basePos = pos
		self._LocateItem()

	def ClearItem(self):
		self.keyDict = {}
		self.textDict = {}
		self.itemList = []
		self.overLine = -1
		self.selectedLine = -1

	def InsertItem(self, number, text):
		self.keyDict[len(self.itemList)] = number
		self.textDict[len(self.itemList)] = text

		textLine = TextLine()
		textLine.SetParent(self)
		textLine.SetText(text)
		textLine.Show()

		if self.itemCenterAlign:
			textLine.SetWindowHorizontalAlignCenter()
			textLine.SetHorizontalAlignCenter()

		self.itemList.append(textLine)

		self._LocateItem()

	def ChangeItem(self, number, text):
		for key, value in self.keyDict.items():
			if value == number:
				self.textDict[key] = text

				if number < len(self.itemList):
					self.itemList[key].SetText(text)

				return

	def LocateItem(self):
		self._LocateItem()

	def _LocateItem(self):

		skipCount = self.basePos
		yPos = 0
		self.showLineCount = 0

		for textLine in self.itemList:
			textLine.Hide()

			if skipCount > 0:
				skipCount -= 1
				continue

			if localeInfo.IsARABIC():
				w, h = textLine.GetTextSize()
				textLine.SetPosition(w+10, yPos + 3)
			else:
				textLine.SetPosition(0, yPos + 3)

			yPos += self.stepSize

			if yPos <= self.GetHeight():
				self.showLineCount += 1
				textLine.Show()

	def ArrangeItem(self):
		self.SetSize(self.width, len(self.itemList) * self.stepSize)
		self._LocateItem()

	def GetViewItemCount(self):
		return int(self.GetHeight() / self.stepSize)

	def GetItemCount(self):
		return len(self.itemList)

	def SetEvent(self, event):
		self.event = event

	def SelectItem(self, line):

		if not self.keyDict.has_key(line):
			return

		if line == self.selectedLine:
			return

		self.selectedLine = line
		self.event(self.keyDict.get(line, 0), self.textDict.get(line, "None"))

	def GetSelectedItem(self):
		return self.keyDict.get(self.selectedLine, 0)

	def OnMouseLeftButtonDown(self):
		if self.overLine < 0:
			return

	def OnMouseLeftButtonUp(self):
		if self.overLine >= 0:
			self.SelectItem(self.overLine+self.basePos)

	def OnUpdate(self):

		self.overLine = -1

		if self.IsIn():
			x, y = self.GetGlobalPosition()
			height = self.GetHeight()
			xMouse, yMouse = wndMgr.GetMousePosition()

			if yMouse - y < height - 1:
				self.overLine = (yMouse - y) / self.stepSize

				if self.overLine < 0:
					self.overLine = -1
				if self.overLine >= len(self.itemList):
					self.overLine = -1

	def OnRender(self):
		xRender, yRender = self.GetGlobalPosition()
		yRender -= self.TEMPORARY_PLACE
		widthRender = self.width
		heightRender = self.height + self.TEMPORARY_PLACE*2

		if localeInfo.IsCIBN10:
			if -1 != self.overLine and self.keyDict[self.overLine] != -1:
				grp.SetColor(HALF_WHITE_COLOR)
				grp.RenderBar(xRender + 2, yRender + self.overLine*self.stepSize + 4, self.width - 3, self.stepSize)

			if -1 != self.selectedLine and self.keyDict[self.selectedLine] != -1:
				if self.selectedLine >= self.basePos:
					if self.selectedLine - self.basePos < self.showLineCount:
						grp.SetColor(SELECT_COLOR)
						grp.RenderBar(xRender + 2, yRender + (self.selectedLine-self.basePos)*self.stepSize + 4, self.width - 3, self.stepSize)

		else:
			if -1 != self.overLine:
				grp.SetColor(HALF_WHITE_COLOR)
				grp.RenderBar(xRender + 2, yRender + self.overLine*self.stepSize + 4, self.width - 3, self.stepSize)

			if -1 != self.selectedLine:
				if self.selectedLine >= self.basePos:
					if self.selectedLine - self.basePos < self.showLineCount:
						grp.SetColor(SELECT_COLOR)
						grp.RenderBar(xRender + 2, yRender + (self.selectedLine-self.basePos)*self.stepSize + 4, self.width - 3, self.stepSize)



class ListBox2(ListBox):
	def __init__(self, *args, **kwargs):
		ListBox.__init__(self, *args, **kwargs)
		self.rowCount = 10
		self.barWidth = 0
		self.colCount = 0

	def SetRowCount(self, rowCount):
		self.rowCount = rowCount

	def SetSize(self, width, height):
		ListBox.SetSize(self, width, height)
		self._RefreshForm()

	def ClearItem(self):
		ListBox.ClearItem(self)
		self._RefreshForm()

	def InsertItem(self, *args, **kwargs):
		ListBox.InsertItem(self, *args, **kwargs)
		self._RefreshForm()

	def OnUpdate(self):
		mpos = wndMgr.GetMousePosition()
		self.overLine = self._CalcPointIndex(mpos)

	def OnRender(self):
		x, y = self.GetGlobalPosition()
		pos = (x + 2, y)

		if -1 != self.overLine:
			grp.SetColor(HALF_WHITE_COLOR)
			self._RenderBar(pos, self.overLine)

		if -1 != self.selectedLine:
			if self.selectedLine >= self.basePos:
				if self.selectedLine - self.basePos < self.showLineCount:
					grp.SetColor(SELECT_COLOR)
					self._RenderBar(pos, self.selectedLine-self.basePos)



	def _CalcPointIndex(self, mpos):
		if self.IsIn():
			px, py = mpos
			gx, gy = self.GetGlobalPosition()
			lx, ly = px - gx, py - gy

			col = lx / self.barWidth
			row = ly / self.stepSize
			idx = col * self.rowCount + row
			if col >= 0 and col < self.colCount:
				if row >= 0 and row < self.rowCount:
					if idx >= 0 and idx < len(self.itemList):
						return idx

		return -1

	def _CalcRenderPos(self, pos, idx):
		x, y = pos
		row = idx % self.rowCount
		col = idx / self.rowCount
		return (x + col * self.barWidth, y + row * self.stepSize)

	def _RenderBar(self, basePos, idx):
		x, y = self._CalcRenderPos(basePos, idx)
		grp.RenderBar(x, y, self.barWidth - 3, self.stepSize)

	def _LocateItem(self):
		pos = (0, self.TEMPORARY_PLACE)

		self.showLineCount = 0
		for textLine in self.itemList:
			x, y = self._CalcRenderPos(pos, self.showLineCount)
			textLine.SetPosition(x, y)
			textLine.Show()

			self.showLineCount += 1

	def _RefreshForm(self):
		if len(self.itemList) % self.rowCount:
			self.colCount = len(self.itemList) / self.rowCount + 1
		else:
			self.colCount = len(self.itemList) / self.rowCount

		if self.colCount:
			self.barWidth = self.width / self.colCount
		else:
			self.barWidth = self.width


class ComboBox(Window):

	class ListBoxWithBoard(ListBox):

		def __init__(self, layer):
			ListBox.__init__(self, layer)

		def OnRender(self):
			xRender, yRender = self.GetGlobalPosition()
			yRender -= self.TEMPORARY_PLACE
			widthRender = self.width
			heightRender = self.height + self.TEMPORARY_PLACE*2
			grp.SetColor(BACKGROUND_COLOR)
			grp.RenderBar(xRender, yRender, widthRender, heightRender)
			grp.SetColor(DARK_COLOR)
			grp.RenderLine(xRender, yRender, widthRender, 0)
			grp.RenderLine(xRender, yRender, 0, heightRender)
			grp.SetColor(BRIGHT_COLOR)
			grp.RenderLine(xRender, yRender+heightRender, widthRender, 0)
			grp.RenderLine(xRender+widthRender, yRender, 0, heightRender)

			ListBox.OnRender(self)

	def __init__(self):
		Window.__init__(self)
		self.x = 0
		self.y = 0
		self.width = 0
		self.height = 0
		self.isSelected = False
		self.isOver = False
		self.isListOpened = False
		self.event = lambda *arg: None
		self.enable = True

		self.textLine = MakeTextLine(self)
		self.textLine.SetText(localeInfo.UI_ITEM)

		self.listBox = self.ListBoxWithBoard("TOP_MOST")
		self.listBox.SetPickAlways()
		self.listBox.SetParent(self)
		self.listBox.SetEvent(__mem_func__(self.OnSelectItem))
		self.listBox.Hide()

	def __del__(self):
		Window.__del__(self)

	def Destroy(self):
		self.textLine = None
		self.listBox = None

	def SetPosition(self, x, y):
		Window.SetPosition(self, x, y)
		self.x = x
		self.y = y
		self.__ArrangeListBox()

	def SetSize(self, width, height):
		Window.SetSize(self, width, height)
		self.width = width
		self.height = height
		self.textLine.UpdateRect()
		self.__ArrangeListBox()

	def __ArrangeListBox(self):
		self.listBox.SetPosition(0, self.height + 5)
		self.listBox.SetWidth(self.width)

	def Enable(self):
		self.enable = True

	def Disable(self):
		self.enable = False
		self.textLine.SetText("")
		self.CloseListBox()

	def SetEvent(self, event):
		self.event = event

	def ClearItem(self):
		self.CloseListBox()
		self.listBox.ClearItem()

	def InsertItem(self, index, name):
		self.listBox.InsertItem(index, name)
		self.listBox.ArrangeItem()

	def SetCurrentItem(self, text):
		self.textLine.SetText(text)

	def SelectItem(self, key):
		self.listBox.SelectItem(key)

	def OnSelectItem(self, index, name):

		self.CloseListBox()
		self.event(index)

	def CloseListBox(self):
		self.isListOpened = False
		self.listBox.Hide()

	def OnMouseLeftButtonDown(self):

		if not self.enable:
			return

		self.isSelected = True

	def OnMouseLeftButtonUp(self):

		if not self.enable:
			return

		self.isSelected = False

		if self.isListOpened:
			self.CloseListBox()
		else:
			if self.listBox.GetItemCount() > 0:
				self.isListOpened = True
				self.listBox.Show()
				self.__ArrangeListBox()

	def OnUpdate(self):

		if not self.enable:
			return

		if self.IsIn():
			self.isOver = True
		else:
			self.isOver = False

	def OnRender(self):
		self.x, self.y = self.GetGlobalPosition()
		xRender = self.x
		yRender = self.y
		widthRender = self.width
		heightRender = self.height
		grp.SetColor(BACKGROUND_COLOR)
		grp.RenderBar(xRender, yRender, widthRender, heightRender)
		grp.SetColor(DARK_COLOR)
		grp.RenderLine(xRender, yRender, widthRender, 0)
		grp.RenderLine(xRender, yRender, 0, heightRender)
		grp.SetColor(BRIGHT_COLOR)
		grp.RenderLine(xRender, yRender+heightRender, widthRender, 0)
		grp.RenderLine(xRender+widthRender, yRender, 0, heightRender)

		if self.isOver:
			grp.SetColor(HALF_WHITE_COLOR)
			grp.RenderBar(xRender + 2, yRender + 3, self.width - 3, heightRender - 5)

			if self.isSelected:
				grp.SetColor(WHITE_COLOR)
				grp.RenderBar(xRender + 2, yRender + 3, self.width - 3, heightRender - 5)

if app.ENABLE_METIN_STONE_QUEUE:
	class MultiTextLine(Window):
		def __del__(self):
			Window.__del__(self)
		def Destroy(self):
			self.textRules = {}
		def __init__(self):
			Window.__init__(self)
			self.Destroy()
			self.AddFlag("not_pick")
			self.textRules["textRange"] = 15
			self.textRules["text"] = ""
			self.textRules["textType"] = ""
			self.textRules["fontName"] = ""
			self.textRules["hexColor"] = 0
			self.textRules["fontColor"] = 0
			self.textRules["outline"] = 0
		def SetTextType(self, textType):
			self.textRules["textType"] = textType
			self.Refresh()
		def SetTextRange(self, textRange):
			self.textRules["textRange"] = textRange
			self.Refresh()
		def SetOutline(self, outline):
			self.textRules["outline"] = outline
			self.Refresh()
		def SetPackedFontColor(self, hexColor):
			self.textRules["hexColor"] = hexColor
			self.Refresh()
		def SetFontColor(self, r, g, b):
			self.textRules["fontColor"] =[r, g, b]
			self.Refresh()
		def SetFontName(self, fontName):
			self.textRules["fontName"] = fontName
			self.Refresh()
		def SetText(self, newText):
			self.textRules["text"] = newText
			self.Refresh()
		def Refresh(self):
			textRules = self.textRules
			if textRules["text"] == "":
				return
			self.children=[]
			outline = textRules["outline"]
			fontColor = textRules["fontColor"]
			hexColor = textRules["hexColor"]
			yRange = textRules["textRange"]
			fontName = textRules["fontName"]
			textTypeList = textRules["textType"].split("?")
			#textType = textRules["textType"].split("#")
			totalTextList = textRules["text"].split("#")

			(xPosition, yPosition) = (0, 0)
			width = 0
			for text in totalTextList:
				childText = TextLine()
				childText.SetParent(self)
				childText.AddFlag("not_pick")
				childText.SetPosition(xPosition, yPosition)
				if fontName != "":
					childText.SetFontName(fontName)
				if hexColor != 0:
					childText.SetPackedFontColor(hexColor)
				if fontColor != 0:
					childText.SetFontColor(*fontColor)
				if outline:
					childText.SetOutline()
				for textType in textTypeList:
					self.AddTextType(childText, textType.split("#"))
				childText.SetText(str(text))
				if childText.GetTextSize()[0] > width:
					width = childText.GetTextSize()[0]
				childText.Show()
				self.children.append(childText)
				yPosition+=yRange

		def AddTextType(self, text,  typeArg):
			if len(typeArg) != 2:
				return
			_typeDict = {
				"vertical": {
					"top":text.SetVerticalAlignTop,
					"bottom":text.SetVerticalAlignBottom,
					"center":text.SetVerticalAlignCenter,
				},
				"horizontal": {
					"left":text.SetHorizontalAlignLeft,
					"right":text.SetHorizontalAlignRight,
					"center":text.SetHorizontalAlignCenter,
				},
				"all_align": {
					"1" : [text.SetHorizontalAlignCenter,text.SetVerticalAlignCenter,text.SetWindowHorizontalAlignCenter,text.SetWindowVerticalAlignCenter],
				},
			}
			(firstToken, secondToken) = tuple(typeArg)
			if _typeDict.has_key(firstToken):
				textType = _typeDict[firstToken][secondToken] if _typeDict[firstToken].has_key(secondToken) else None
				if textType != None:
					if isinstance(textType, list):
						for rule in textType:
							rule()
					else:
						textType()

if app.ENABLE_RENDER_TARGET:
	class RenderTarget(Window):
		def __init__(self, layer = "UI"):
			Window.__init__(self, layer)
			
			self.number = -1
			if app.ENABLE_RENDER_TARGET_EX:
				self.isHolding = False
				self.mouseLastXPos = 0

		def __del__(self):
			Window.__del__(self)

		def RegisterWindow(self, layer):
			self.hWnd = wndMgr.RegisterRenderTarget(self, layer)
			
		def SetRenderTarget(self, number):
			self.number = number
			wndMgr.SetRenderTarget(self.hWnd, self.number)

		if app.ENABLE_RENDER_TARGET_EX:
			def GetRenderTargetIndex(self):
				return self.number

			def OnUpdate(self):
				if self.isHolding == True:
					mouseCurrentPos = self.GetMouseLocalPosition()
					
					difference = mouseCurrentPos[0] - self.mouseLastXPos
					self.mouseLastXPos = mouseCurrentPos[0]
					renderTarget.MouseRotation(self.number, difference)

			def OnMouseLeftButtonUp(self):
				self.isHolding = False

			def OnMouseLeftButtonDown(self):
				self.isHolding = True
				self.mouseLastXPos = self.GetMouseLocalPosition()[0]

			def OnMouseMiddleScroll(self, len):
				if (len == 0):
					return
				
				if (len == 120):
					bZoom = True
				else:
					bZoom = False
				
				# chat.AppendChat(chat.CHAT_TYPE_INFO, "len : %d" % len)
				
				renderTarget.MouseZoom(self.number, bZoom)

## if app.ENABLE_BATTLE_PASS:
class BorderA(ThinBoardCircle):
	def __init__(self, layer = "UI"):
		ThinBoardCircle.__init__(self)

	def __del__(self):
		ThinBoardCircle.__del__(self)

	def SetSize(self, width, height):
		ThinBoardCircle.SetSize(self, width, height)

if app.ENABLE_INGAME_WIKI:
	class WikiRenderTarget(Window):
		def __init__(self):
			Window.__init__(self)
		
		def __del__(self):
			Window.__del__(self)
		
		def RegisterWindow(self, layer):
			self.hWnd = wndMgr.RegisterWikiRenderTarget(self, layer)

	class InGameWikiCheckBox(Window):
		def __init__(self):
			Window.__init__(self)
			
			self.backgroundImage = None
			self.checkImage = None
			
			self.eventFunc = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
			self.eventArgs = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
			
			self.CreateElements()
		
		def __del__(self):
			Window.__del__(self)
			
			self.backgroundImage = None
			self.checkImage = None
			
			self.eventFunc = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
			self.eventArgs = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
		
		def CreateElements(self):
			self.backgroundImage = ImageBox()
			self.backgroundImage.SetParent(self)
			self.backgroundImage.AddFlag("not_pick")
			self.backgroundImage.LoadImage("d:/ymir work/ui/wiki/wiki_check_box_clean.tga")
			self.backgroundImage.Show()
			
			self.checkImage = ImageBox()
			self.checkImage.SetParent(self)
			self.checkImage.AddFlag("not_pick")
			self.checkImage.LoadImage("d:/ymir work/ui/wiki/wiki_check_box_checked.tga")
			self.checkImage.Hide()
			
			self.textInfo = TextLine()
			self.textInfo.SetParent(self)
			self.textInfo.SetPosition(20, 0)
			self.textInfo.Show()
			
			self.SetSize(self.backgroundImage.GetWidth() + self.textInfo.GetTextSize()[0], self.backgroundImage.GetHeight() + self.textInfo.GetTextSize()[1])
		
		def SetTextInfo(self, info):
			if self.textInfo:
				self.textInfo.SetText(info)
			
			self.SetSize(self.backgroundImage.GetWidth() + self.textInfo.GetTextSize()[0], self.backgroundImage.GetHeight() + self.textInfo.GetTextSize()[1])
		
		def SetCheckStatus(self, flag):
			if flag:
				self.checkImage.Show()
			else:
				self.checkImage.Hide()
		
		def GetCheckStatus(self):
			if self.checkImage:
				return self.checkImage.IsShow()
			
			return False
		
		def SetEvent(self, func, *args) :
			result = self.eventFunc.has_key(args[0])
			if result:
				self.eventFunc[args[0]] = func
				self.eventArgs[args[0]] = args
			else:
				print "[ERROR] ui.py SetEvent, Can`t Find has_key : %s" % args[0]
		
		def SetBaseCheckImage(self, image):
			if not self.backgroundImage:
				return
			
			self.backgroundImage.LoadImage(image)
		
		def OnMouseLeftButtonUp(self):
			if self.checkImage:
				if self.checkImage.IsShow():
					self.checkImage.Hide()
					
					if self.eventFunc["ON_UNCKECK"]:
						apply(self.eventFunc["ON_UNCKECK"], self.eventArgs["ON_UNCKECK"])
				else:
					self.checkImage.Show()
					
					if self.eventFunc["ON_CHECK"]:
						apply(self.eventFunc["ON_CHECK"], self.eventArgs["ON_CHECK"])

if app.ENABLE_PREMIUM_PRIVATE_SHOP:
	class DynamicListBox(Window):

		def GetSelectedItemText(self):
			return self.textDict.get(self.selectedLine, "")

		TEMPORARY_PLACE = 3

		def __init__(self, layer = "UI"):
			Window.__init__(self, layer)
			self.overLine = -1
			self.selectedLine = -1
			self.width = 0
			self.height = 0
			self.stepSize = 17
			self.basePos = 0
			self.showLineCount = 0
			self.visibleLineCount = 10
			self.currentPosition = 0
			self.itemCenterAlign = TRUE
			self.itemList = []
			self.keyDict = {}
			self.textDict = {}
			self.event = lambda *arg: None

			# Update
			self.itemMode = False

		def __del__(self):
			Window.__del__(self)

		def SetWidth(self, width):
			self.SetSize(width, self.height)

		def SetSize(self, width, height):
			Window.SetSize(self, width, height)
			self.width = width
			self.height = height

		def SetTextCenterAlign(self, flag):
			self.itemCenterAlign = flag

		def SetBasePos(self, pos):
			self.basePos = pos
			self._LocateItem()

		# fix
		def SetCurrentPosition(self, pos):
			self.currentPosition = pos
			self._LocateItem()

		def ClearItem(self):
			self.keyDict = {}
			self.textDict = {}
			self.itemList = []
			self.overLine = -1
			self.selectedLine = -1

		def InsertItem(self, number, text):
			self.keyDict[len(self.itemList)] = number
			self.textDict[len(self.itemList)] = text

			textLine = TextLine()
			textLine.SetParent(self)

			# Update
			if self.itemMode == True:
				item.SelectItem(number)
				icon = item.GetIconImageFileName()
				size = item.GetItemSize()[1]

				emoji_icon = "|E%s|e" % icon

				if size == 2 or size == 3:
					emoji_icon = ""

				textLine.SetText(emoji_icon + text)
			else:
				textLine.SetText(text)

			textLine.Hide()

			if self.itemCenterAlign:
				textLine.SetWindowHorizontalAlignCenter()
				textLine.SetHorizontalAlignCenter()

			self.itemList.append(textLine)

			self._LocateItem()

		def ChangeItem(self, number, text):
			for key, value in self.keyDict.items():
				if value == number:
					self.textDict[key] = text

					if number < len(self.itemList):
						self.itemList[key].SetText(text)

					return

		def LocateItem(self):
			self._LocateItem()

		# Update
		def SetItemMode(self, bool):
			if bool == True:
				self.itemMode = True
				self.stepSize = 24
			else:
				self.itemMode = False
				self.stepSize = 19

		def SetVisibleLineCount(self, count):
			self.visibleLineCount = count

		def GetVisibleLineCount(self):
			return self.visibleLineCount

		def GetVisibleHeight(self):
			return self.visibleLineCount * self.stepSize

		def _LocateItem(self):
			yPos = 0
			self.showLineCount = 0

			i = 0
			for textLine in self.itemList:
				if i >= self.currentPosition and i < (self.currentPosition + self.GetVisibleLineCount()):
					textLine.SetPosition(0, yPos + self.stepSize / 2 - textLine.GetTextSize()[1] / 2)

					textLine.Show()
					yPos += self.stepSize
				else:
					textLine.Hide()

				self.showLineCount += 1
				i += 1

		def ArrangeItem(self):
			self.SetSize(self.width, len(self.itemList) * self.stepSize)
			self._LocateItem()

		def GetViewItemCount(self):
			return int(self.GetHeight() / self.stepSize)

		def GetItemCount(self):
			return min(len(self.itemList), 176)

		def SetEvent(self, event):
			self.event = event

		def SelectItem(self, line):

			if not self.keyDict.has_key(line):
				return

			if line == self.selectedLine:
				return

			self.selectedLine = line + self.currentPosition
			self.event(self.keyDict.get(self.selectedLine, 0), self.textDict.get(self.selectedLine, "None"))

		def GetSelectedItem(self):
			return self.keyDict.get(self.selectedLine, 0)

		def GetSelectedItemText(self):
			return self.itemList[self.selectedLine].GetText()

		def OnMouseLeftButtonDown(self):
			if self.overLine < 0:
				return

		def OnMouseLeftButtonUp(self):
			if self.overLine >= 0:
				self.SelectItem(self.overLine+self.basePos)

		def OnDown(self):
			if self.GetItemCount() > self.GetVisibleLineCount():
				self.currentPosition = min(self.GetItemCount() - self.GetVisibleLineCount(), self.currentPosition + 1)

				self._LocateItem()

		def OnUp(self):
			if self.GetItemCount() > self.GetVisibleLineCount():
				self.currentPosition = max(0, self.currentPosition - 1)

				self._LocateItem()

		def OnUpdate(self):
			self.overLine = -1

			if self.IsIn():
				x, y = self.GetGlobalPosition()
				height = self.GetHeight()
				xMouse, yMouse = wndMgr.GetMousePosition()

				if yMouse - y < height - 1:
					self.overLine = (yMouse - y) / self.stepSize

					if self.overLine < 0:
						self.overLine = -1
					if self.overLine >= len(self.itemList):
						self.overLine = -1

		def OnRender(self):
			xRender, yRender = self.GetGlobalPosition()
			yRender -= self.TEMPORARY_PLACE
			widthRender = self.width
			heightRender = self.height + self.TEMPORARY_PLACE*2

			if -1 != self.overLine:
				grp.SetColor(HALF_WHITE_COLOR)
				grp.RenderBar(xRender + 1, yRender + self.overLine*self.stepSize + 4, self.width - 2, self.stepSize + 1)

			if -1 != self.selectedLine:
				if self.selectedLine >= self.currentPosition and self.selectedLine < (self.currentPosition + self.GetVisibleLineCount()):
					if self.selectedLine - self.basePos < self.showLineCount:
						grp.SetColor(grp.GenerateColor(255.0 / 255.0, 150.0 / 255.0, 95.0 / 255.0, 0.2))
						grp.RenderBar(xRender + 2, yRender + (self.selectedLine-self.basePos-self.currentPosition) * self.stepSize + 4, self.width - 3, self.stepSize)

			max_count = min(self.GetVisibleLineCount(), self.GetItemCount())
			for i in range(1, max_count):
				grp.SetColor(HALF_WHITE_COLOR)
				grp.RenderBar(xRender, yRender + i*self.stepSize + 4, self.width, 1)

	class DropDownList(Window):
		def __init__(self, parent, x, y):
			Window.__init__(self)

			self.SetParent(parent)
			self.SetPosition(x, y)

			self.x = x
			self.y = y
			self.width = 0
			self.height = 0

			# List Configurations
			self.isSelected = False
			self.isOver = False
			self.isListOpened = False
			self.event = None
			self.eventArgs = None

			# ListBox
			self.listBox = DynamicListBox()
			self.listBox.SetParent(self)
			self.listBox.SetPickAlways()
			self.listBox.SetVisibleLineCount(10)
			self.listBox.SetEvent(__mem_func__(self.OnSelectItem))
			self.listBox.Hide()

			# scroll_bar
			self.scrollBarPos = 0.0
			self.scroll_bar = ScrollBar()
			self.scroll_bar.SetParent(self)
			self.scroll_bar.AddFlag("float")
			self.scroll_bar.SetScrollEvent(__mem_func__(self.OnScrollResultList))

		def __del__(self):
			Window.__del__(self)
			self.listBox = None
			self.event = None
			self.eventArgs = None

		def SetPosition(self, x, y):
			Window.SetPosition(self, x, y)
			self.x = x
			self.y = y

		def SetSize(self, width, height = 0):
			self.width = width
			self.height = height

			self.AdjustListBox()

		def SetItemMode(self, bool):
			if bool == True:
				self.listBox.SetItemMode(True)
			else:
				self.listBox.SetItemMode(False)

		def AdjustListBox(self):
			if self.listBox.GetItemCount() <= self.listBox.GetVisibleLineCount():
				self.listBox.SetSize(self.width, self.listBox.GetHeight())
				self.height = self.listBox.GetHeight()
			else:
				self.listBox.SetSize(self.width - self.scroll_bar.GetWidth(), self.listBox.GetVisibleHeight())
				self.height = self.listBox.GetVisibleHeight()

			# scroll_bar
			self.scroll_bar.SetScrollBarSize(self.listBox.GetVisibleHeight())
			self.scroll_bar.SetPosition(self.x + self.width, self.listBox.GetLocalPosition()[1])

			Window.SetSize(self, self.width, self.height)

		def SetEvent(self, event):
			self.event = event

		def OnSelectItem(self, index, name):
			self.CloseListBox()

			if self.event:
				self.event(index)

		def ClearItem(self):
			self.CloseListBox()
			self.listBox.ClearItem()

		def InsertItem(self, index, name):
			self.listBox.InsertItem(index, name)
			self.listBox.ArrangeItem()

		def OpenListBox(self):
			self.isListOpened = True
			self.listBox.Show()
			self.Show()

			# scroll_bar
			if self.listBox.GetItemCount() <= self.listBox.GetVisibleLineCount():
				self.scroll_bar.Hide()
			else:
				self.scroll_bar.SetMiddleBarSize(float(self.listBox.GetVisibleLineCount()) / float(self.listBox.GetItemCount()))
				self.scroll_bar.Show()

		def CloseListBox(self):
			self.isListOpened = False
			self.listBox.Hide()
			self.Hide()

			# fix
			self.listBox.SetCurrentPosition(0)

			# scroll_bar
			self.scroll_bar.SetPos(0.0)

		def IsOpened(self):
			return self.isListOpened

		def GetItemCount(self):
			return self.listBox.GetItemCount()

		def OnRunMouseWheel(self, nLen):
			if self.listBox.IsInPosition():
				if nLen > 0:
					self.listBox.OnUp()

					# scroll_bar
					self.scroll_bar.SetScrollStep(1.0 / (self.listBox.GetItemCount() + self.listBox.GetVisibleLineCount()))
					self.scroll_bar.OnUp()

					return True

				elif nLen < 0:
					self.listBox.OnDown()

					# scroll_bar
					self.scroll_bar.SetScrollStep(1.0 / (self.listBox.GetItemCount() - self.listBox.GetVisibleLineCount()))
					self.scroll_bar.OnDown()

					return True

			return False

		def OnMouseLeftButtonDown(self):
			self.isSelected = True

		def OnMouseLeftButtonUp(self):
			self.isSelected = False
			self.CloseListBox()

		def OnUpdate(self):
			if self.IsIn():
				self.isOver = True
			else:
				self.isOver = False

		def OnRender(self):
			if self.isListOpened:
				xRender, yRender = self.GetGlobalPosition()

				widthRender = self.width
				heightRender = self.height

				grp.SetColor(BACKGROUND_COLOR)
				grp.RenderBar(xRender, yRender, widthRender, heightRender)

				if self.isOver:
					grp.SetColor(HALF_WHITE_COLOR)
					grp.RenderBar(xRender + 2, yRender + 3, self.width - 3, heightRender - 5)

					if self.isSelected:
						grp.SetColor(WHITE_COLOR)
						grp.RenderBar(xRender + 2, yRender + 3, self.width - 3, heightRender - 5)

		# scroll_bar
		def OnScrollResultList(self):
			self.scrollBarPos = int(self.scroll_bar.GetPos() * max(0, self.listBox.GetItemCount() - self.listBox.GetVisibleLineCount()))
			self.listBox.SetCurrentPosition(self.scrollBarPos)

	class DynamicComboBoxImage(Window):
		class ListBoxWithBoard(DynamicListBox):

			BG_COLOR = grp.GenerateColor(33.0 / 255.0, 33.0 / 255.0, 33.0 / 255.0, 1.0)

			def __init__(self, layer):
				DynamicListBox.__init__(self, layer)

			def OnRender(self):
				xRender, yRender = self.GetGlobalPosition()
				yRender -= self.TEMPORARY_PLACE
				widthRender = self.width
				heightRender = self.height + self.TEMPORARY_PLACE*2
				grp.SetColor(BACKGROUND_COLOR)
				grp.RenderBar(xRender, yRender, widthRender, heightRender)
				grp.SetColor(WHITE_COLOR)
				grp.RenderBox(xRender, yRender, widthRender, heightRender)
				DynamicListBox.OnRender(self)

		def __init__(self, parent, name, x, y, is_big=False):
			self.isSelected = False
			self.isOver = False
			self.isListOpened = False
			self.event = lambda *arg: None
			self.enable = True
			self.imagebox = None
			self.listBox = None
			self.titleText = None

			# is_big
			self.is_big = is_big

			# scroll_bar
			self.scroll_bar = None

			Window.__init__(self)

			# ImageBox
			image = ExpandedImageBox()
			image.SetParent(parent)
			image.LoadImage(name)
			if self.is_big:
				image.SetPosition(x+76, y)
			else:
				image.SetPosition(x+25, y)
			image.Hide()
			self.imagebox = image

			# BaseSetting
			self.x = x + 1
			self.y = y + 1
			if self.is_big:
				self.width = self.imagebox.GetWidth() + 137
			else:
				self.width = self.imagebox.GetWidth() + 35
			self.height = self.imagebox.GetHeight() - 3
			self.SetParent(parent)

			# ListBox
			self.listBox = self.ListBoxWithBoard("TOP_MOST")
			self.listBox.SetParent(self)
			self.listBox.SetPickAlways()
			self.listBox.SetPosition(6, self.height + 5)
			self.listBox.SetVisibleLineCount(10)
			self.listBox.SetEvent(__mem_func__(self.OnSelectItem))
			self.listBox.Hide()

			# scroll_bar
			self.scrollBarPos = 0.0
			self.scroll_bar = ScrollBar()
			self.scroll_bar.SetParent(self)
			self.scroll_bar.SetPickAlways()
			self.scroll_bar.SetPosition(self.x + self.width + 78, self.y + self.height)
			self.scroll_bar.SetMiddleBarSize(0.3)
			self.scroll_bar.SetScrollEvent(__mem_func__(self.OnScrollResultList))
			self.scroll_bar.Hide()

			# TextLine
			self.textLine = MakeTextLine(self)
			self.textLine.SetText(localeInfo.UI_ITEM)
			self.textLine.UpdateRect()

			Window.SetPosition(self, self.x, self.y)
			Window.SetSize(self, self.width + 13, self.height)

			self.__ArrangeListBox()

		def __del__(self):
			Window.__del__(self)

		def Hide(self):
			Window.Hide(self)

			if self.listBox:
				self.CloseListBox()

			if self.imagebox:
				self.imagebox.Hide()

		def Show(self):
			Window.Show(self)

			if self.imagebox:
				self.imagebox.Show()

		def Destroy(self):
			self.textLine = None
			self.listBox = None
			self.imagebox = None

		def SetPosition(self, x, y):
			Window.SetPosition(self, x, y)
			self.imagebox.SetPosition(x, y)
			self.x = x
			self.y = y
			self.__ArrangeListBox()

		def SetSize(self, width, height):
			Window.SetSize(self, width, height)
			self.width = width
			self.height = height
			self.textLine.UpdateRect()
			self.__ArrangeListBox()

		def SetImageScale(self, scale_x, scale_y):
			self.imagebox.SetScale(scale_x, scale_y)
			self.width = self.imagebox.GetWidth() - 3
			self.height = self.imagebox.GetHeight() - 3
			Window.SetSize(self, self.width, self.height)
			self.textLine.UpdateRect()
			self.__ArrangeListBox()

		def __ArrangeListBox(self):
			self.listBox.SetPosition(6, self.height + 5)

			if self.listBox.GetItemCount() <= self.listBox.GetVisibleLineCount():
				self.listBox.SetSize(self.width, self.listBox.GetHeight())
			else:
				self.listBox.SetSize(self.width, self.listBox.GetVisibleHeight())

			# scroll_bar
			self.scroll_bar.SetScrollBarSize(self.listBox.GetVisibleHeight() + 9)

		def Enable(self):
			self.enable = True

		def Disable(self):
			self.enable = False
			self.CloseListBox()

		def SetEvent(self, event):
			self.event = event

		def SetDefaultTitle(self, title):
			self.titleText = title

			self.SetCurrentItem(self.titleText)

		def UseDefaultTitle(self):
			self.SetCurrentItem(self.titleText)

		def SetTitle(self, title):
			self.SetCurrentItem(title)

		def GetTitle(self):
			return self.titleText

		def Clear(self):
			self.SelectItem(0)

		# fix
		def SetCurrentPosition(self, key):
			self.listBox.SetCurrentPosition(key)

			# scroll_bar
			self.scroll_bar.SetPos(0.0)

		def ClearItem(self):
			self.CloseListBox()
			self.listBox.ClearItem()

		def InsertItem(self, index, name):
			self.listBox.InsertItem(index, name)
			self.listBox.ArrangeItem()

		def SetCurrentItem(self, text):
			self.textLine.SetText(text)
			self.UpdateText(text)

		def UpdateText(self, text):
			maxNameLenght = -1
			while self.textLine.GetTextSize()[0] >= 110:
				self.textLine.SetText(text[:maxNameLenght])
				maxNameLenght -= 1
			if maxNameLenght != -1:
				shortenedName = text[: maxNameLenght - 1] + '..'
				self.textLine.SetText(shortenedName)

			self.textLine.UpdateRect()

		def GetSelectedItemText(self):
			return self.listBox.GetSelectedItemText()

		def SelectItem(self, key):
			self.listBox.SelectItem(key)

		def OnSelectItem(self, index, name):
			self.CloseListBox()
			self.event(index)

		def IsOpened(self):
			return self.isListOpened

		def CloseListBox(self):
			self.isListOpened = False
			self.listBox.Hide()

			# scroll_bar
			if self.scroll_bar:
				self.scroll_bar.Hide()

		def OnMouseLeftButtonDown(self):
			if not self.enable:
				return

			self.isSelected = True

		def OnMouseLeftButtonUp(self):
			if not self.enable:
				return

			self.isSelected = False

			if self.isListOpened:
				self.CloseListBox()
			else:
				if self.listBox.GetItemCount() > 0:
					self.isListOpened = True
					self.listBox.Show()
					self.listBox.SetTop()
					self.__ArrangeListBox()

					# scroll_bar
					if self.listBox.GetItemCount() > 12:
						if self.scroll_bar:
							if self.is_big:
								self.scroll_bar.Show()
								self.scroll_bar.SetTop()
							else:
								self.scroll_bar.Hide()

		def OnRunMouseWheel(self, nLen):
			if self.listBox.IsInPosition():
				if nLen > 0:
					self.listBox.OnUp()

					# scroll_bar
					self.scroll_bar.SetScrollStep(1.0 / (self.listBox.GetItemCount() + self.listBox.GetVisibleLineCount()))
					self.scroll_bar.OnUp()

					return True

				elif nLen < 0:
					self.listBox.OnDown()

					# scroll_bar
					self.scroll_bar.SetScrollStep(1.0 / (self.listBox.GetItemCount() - self.listBox.GetVisibleLineCount()))
					self.scroll_bar.OnDown()

					return True

			return False

		def OnUpdate(self):
			if not self.enable:
				return

			if self.IsIn():
				self.isOver = True
			else:
				self.isOver = False

		def OnRender(self):
			self.x, self.y = self.GetGlobalPosition()

			if self.is_big:
				xRender = self.x + 77
			else:
				xRender = self.x + 26
			yRender = self.y + 1

			if self.is_big:
				widthRender = self.width - 141
			else:
				widthRender = self.width - 39
			heightRender = self.height - 1

			if self.isOver:
				grp.SetColor(HALF_WHITE_COLOR)
				grp.RenderBar(xRender, yRender, widthRender, heightRender)

				if self.isSelected:
					grp.SetColor(WHITE_COLOR)
					grp.RenderBar(xRender, yRender, widthRender, heightRender)

		# scroll_bar
		def OnScrollResultList(self):
			self.scrollBarPos = int(self.scroll_bar.GetPos() * max(0, self.listBox.GetItemCount() - self.listBox.GetVisibleLineCount()))
			self.listBox.SetCurrentPosition(self.scrollBarPos)

	class ShopDecoThinboard(Window):
		DEFAULT_VALUE = 16
		CORNER_WIDTH = 48
		CORNER_HEIGHT = 32
		LINE_WIDTH = 16
		LINE_HEIGHT = 32
		
		DEFAULT_CORNER_WIDTH = 16
		DEFAULT_CORNER_HEIGHT = 16
		DEFAULT_LINE_WIDTH = 16
		DEFAULT_LINE_HEIGHT = 16
		DEFAULT_BOARD_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 0.51)

		LT = 0
		LB = 1
		RT = 2
		RB = 3
		L = 0
		R = 1
		T = 2
		B = 3

		def __init__(self, type = 0, layer = "UI"):
			Window.__init__(self, layer)
			
			self.type = type
			
			base = Bar()
			base.SetParent(self)
			base.AddFlag("attach")
			base.AddFlag("not_pick")
			base.SetPosition(self.DEFAULT_CORNER_WIDTH, self.DEFAULT_CORNER_HEIGHT)
			base.SetColor(self.DEFAULT_BOARD_COLOR)
			base.Hide()
			self.base = base
		
			self.width = 190
			self.height = 32
			
			self.SetStyle(type)
			self.Refresh()
				
		def __del__(self):
			Window.__del__(self)
			
		def GetStyle(self, type):
			import privateShop

			(name, path, text_color) = privateShop.GetTitleDeco(type)
			
			CornerFileNames = [ path + "_"+dir+".tga" for dir in ["left_top","left_bottom","right_top","right_bottom"] ]
			LineFileNames = [ path + "_"+dir+".tga" for dir in ["left","right","top","bottom"] ]
			
			return CornerFileNames, LineFileNames
			
		def SetStyle(self, type):
			self.type = type
			
			CornerFileNames, LineFileNames = self.GetStyle(type)
			
			if CornerFileNames == None or LineFileNames == None :
				return

			self.Corners = []
			for fileName in CornerFileNames:
				Corner = ExpandedImageBox()
				Corner.AddFlag("attach")
				Corner.AddFlag("not_pick")
				Corner.LoadImage(fileName)
				Corner.SetParent(self)
				Corner.SetPosition(0, 0)
				Corner.Show()
				self.Corners.append(Corner)

			self.Lines = []
			for fileName in LineFileNames:
				Line = ExpandedImageBox()
				Line.AddFlag("attach")
				Line.AddFlag("not_pick")
				Line.LoadImage(fileName)
				Line.SetParent(self)
				Line.SetPosition(0, 0)
				Line.Show()
				self.Lines.append(Line)
				
			if self.type == 0:
				self.base.Show()
			else:
				self.base.Hide()
				
			self.Refresh()

		def SetBoardSize(self, width, height):
			if self.type == 0:
				self.width = max(self.DEFAULT_CORNER_WIDTH*2, width)
				self.height = max(self.DEFAULT_CORNER_HEIGHT*2, height)
				
			else:
				self.width = max(self.DEFAULT_VALUE*2, width)
				self.height = max(self.DEFAULT_VALUE*2, height)
				
			Window.SetSize(self, self.width, self.height)
			self.Refresh()

		def Refresh(self):
			if self.type == 0:
		
				self.Corners[self.LB].SetPosition(0, self.height - self.DEFAULT_CORNER_HEIGHT)
				self.Corners[self.RT].SetPosition(self.width - self.DEFAULT_CORNER_WIDTH, 0)
				self.Corners[self.RB].SetPosition(self.width - self.DEFAULT_CORNER_WIDTH, self.height - self.DEFAULT_CORNER_HEIGHT)
				
				self.Lines[self.L].SetPosition(0, self.DEFAULT_CORNER_HEIGHT)
				self.Lines[self.T].SetPosition(self.DEFAULT_CORNER_WIDTH, 0)
				self.Lines[self.R].SetPosition(self.width - self.DEFAULT_CORNER_WIDTH, self.DEFAULT_CORNER_HEIGHT)
				self.Lines[self.B].SetPosition(self.DEFAULT_CORNER_HEIGHT, self.height - self.DEFAULT_CORNER_HEIGHT)

				verticalShowingPercentage = float((self.height - self.DEFAULT_CORNER_HEIGHT*2) - self.DEFAULT_LINE_HEIGHT) / self.DEFAULT_LINE_HEIGHT
				horizontalShowingPercentage = float((self.width - self.DEFAULT_CORNER_WIDTH*2) - self.DEFAULT_LINE_WIDTH) / self.DEFAULT_LINE_WIDTH
				
				self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
				self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
				self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
				self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
				self.base.SetSize(self.width - self.DEFAULT_CORNER_WIDTH*2, self.height - self.DEFAULT_CORNER_HEIGHT*2)
				self.base.Show()
		
			else:
				self.Corners[self.LT].SetPosition(-self.CORNER_WIDTH + self.DEFAULT_VALUE, -self.CORNER_HEIGHT + self.DEFAULT_VALUE)
				self.Corners[self.LB].SetPosition(-self.CORNER_WIDTH + self.DEFAULT_VALUE, self.height - self.CORNER_HEIGHT + self.DEFAULT_VALUE)
				
				self.Corners[self.RT].SetPosition(self.width - self.DEFAULT_VALUE, -self.CORNER_HEIGHT + self.DEFAULT_VALUE)
				self.Corners[self.RB].SetPosition(self.width - self.DEFAULT_VALUE, self.height - self.CORNER_HEIGHT + self.DEFAULT_VALUE)
				
				self.Lines[self.L].SetPosition(0, self.DEFAULT_VALUE)
				self.Lines[self.R].SetPosition(self.width - self.DEFAULT_VALUE, self.DEFAULT_VALUE)
				self.Lines[self.B].SetPosition(self.DEFAULT_VALUE, self.height - self.LINE_HEIGHT + self.DEFAULT_VALUE)
				self.Lines[self.T].SetPosition(self.DEFAULT_VALUE, -self.LINE_HEIGHT + self.DEFAULT_VALUE)
				
				verticalShowingPercentage = float((self.height - self.DEFAULT_VALUE*2) - self.DEFAULT_VALUE) / self.DEFAULT_VALUE
				horizontalShowingPercentage = float((self.width - self.DEFAULT_VALUE*2) - self.DEFAULT_VALUE) / self.DEFAULT_VALUE
				
				self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
				self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
				self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
				self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
				self.base.Hide()

		def ShowInternal(self):
			for wnd in self.Lines:
				wnd.Show()
			for wnd in self.Corners:
				wnd.Show()

		def HideInternal(self):
			for wnd in self.Lines:
				wnd.Hide()
			for wnd in self.Corners:
				wnd.Hide()

class HorizontalBarCheckbox(Window):
	BLOCK_WIDTH = 32
	BLOCK_HEIGHT = 20

	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")
		self.ButtonText = None

	def __del__(self):
		Window.__del__(self)

	def Create(self, width):

		width = max(96, width)

		imgLeft = ImageBox()
		imgLeft.SetParent(self)
		imgLeft.AddFlag("not_pick")
		imgLeft.LoadImage("d:/ymir work/ui/pattern/horizontalbar_checkbox_left.tga")
		imgLeft.Show()

		imgCenter = ExpandedImageBox()
		imgCenter.SetParent(self)
		imgCenter.AddFlag("not_pick")
		imgCenter.LoadImage("d:/ymir work/ui/pattern/horizontalbar_checkbox_center.tga")
		imgCenter.Show()

		imgRight = ImageBox()
		imgRight.SetParent(self)
		imgRight.AddFlag("not_pick")
		imgRight.LoadImage("d:/ymir work/ui/pattern/horizontalbar_checkbox_right.tga")
		imgRight.Show()

		self.imgLeft = imgLeft
		self.imgCenter = imgCenter
		self.imgRight = imgRight
		self.SetWidth(width)

	def SetText(self, text):
		if not self.ButtonText:
			textLine = TextLine()
			textLine.SetParent(self)
			textLine.SetPosition(self.GetWidth()/2, self.GetHeight()/2)
			textLine.SetVerticalAlignCenter()
			textLine.SetHorizontalAlignCenter()
			textLine.Show()
			self.ButtonText = textLine

		self.ButtonText.SetText(text)

	def GetText(self):
		if not self.ButtonText:
			return ""
		return self.ButtonText.GetText()

	def SetWidth(self, width):
		self.imgCenter.SetRenderingRect(0.0, 0.0, float((width - self.BLOCK_WIDTH*2) - self.BLOCK_WIDTH) / self.BLOCK_WIDTH, 0.0)
		self.imgCenter.SetPosition(self.BLOCK_WIDTH, 0)
		self.imgRight.SetPosition(width - self.BLOCK_WIDTH, 0)
		self.SetSize(width, self.BLOCK_HEIGHT)

class HorizontalBarCheckboxSmall(Window):
	BLOCK_WIDTH = 32
	BLOCK_HEIGHT = 16

	def __init__(self):
		Window.__init__(self)
		self.AddFlag("attach")
		self.ButtonText = None

	def __del__(self):
		Window.__del__(self)

	def Create(self, width):

		width = max(96, width)

		imgLeft = ImageBox()
		imgLeft.SetParent(self)
		imgLeft.AddFlag("not_pick")
		imgLeft.LoadImage("d:/ymir work/ui/pattern/horizontalbar_checkbox_left_small.tga")
		imgLeft.Show()

		imgCenter = ExpandedImageBox()
		imgCenter.SetParent(self)
		imgCenter.AddFlag("not_pick")
		imgCenter.LoadImage("d:/ymir work/ui/pattern/horizontalbar_checkbox_center_small.tga")
		imgCenter.Show()

		imgRight = ImageBox()
		imgRight.SetParent(self)
		imgRight.AddFlag("not_pick")
		imgRight.LoadImage("d:/ymir work/ui/pattern/horizontalbar_checkbox_right_small.tga")
		imgRight.Show()

		self.imgLeft = imgLeft
		self.imgCenter = imgCenter
		self.imgRight = imgRight
		self.SetWidth(width)

	def SetText(self, text):
		if not self.ButtonText:
			textLine = TextLine()
			textLine.SetParent(self)
			textLine.SetPosition(self.GetWidth()/2, self.GetHeight()/2)
			textLine.SetVerticalAlignCenter()
			textLine.SetHorizontalAlignCenter()
			textLine.Show()
			self.ButtonText = textLine

		self.ButtonText.SetText(text)

	def GetText(self):
		if not self.ButtonText:
			return ""
		return self.ButtonText.GetText()

	def SetWidth(self, width):
		self.imgCenter.SetRenderingRect(0.0, 0.0, float((width - self.BLOCK_WIDTH*2) - self.BLOCK_WIDTH) / self.BLOCK_WIDTH, 0.0)
		self.imgCenter.SetPosition(self.BLOCK_WIDTH, 0)
		self.imgRight.SetPosition(width - self.BLOCK_WIDTH, 0)
		self.SetSize(width, self.BLOCK_HEIGHT)

class CheckBoxEx(Window):

	STATE_UNSELECTED = 0
	STATE_SELECTED = 1

	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		self.state = self.STATE_UNSELECTED
		self.eventFunc = None
		self.eventArgs = None

		self.overIn = ""

		self.btnBox = {
			self.STATE_UNSELECTED : self.__init_MakeButton("d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga"),
			self.STATE_SELECTED : self.__init_MakeButton("d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga", "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga"),
		}

		text = TextLine()
		text.SetParent(self)
		text.SetWindowVerticalAlignCenter()
		text.SetVerticalAlignCenter()
		text.Show()
		self.text = text

		self.__Refresh()

		self.SetWindowName("NONAME_CheckBox")

	def __del__(self):
		Window.__del__(self)

	def __ConvertPath(self, path, subStr):
		if path.find("%s") != -1:
			return path % subStr
		else:
			return path

	def __init_MakeButton(self, path, disablePath = None):
		btn = Button()
		btn.SetParent(self)
		btn.SetWindowVerticalAlignCenter()
		btn.SetUpVisual(self.__ConvertPath(path, "01"))
		btn.SetOverVisual(self.__ConvertPath(path, "02"))
		btn.SetDownVisual(self.__ConvertPath(path, "03"))
		if disablePath:
			btn.SetDisableVisual(disablePath)
		else:
			btn.SetDisableVisual(self.__ConvertPath(path, "01"))
		btn.SAFE_SetEvent(self.OnClickButton)
		btn.baseWidth = btn.GetWidth()
		return btn

	def __UpdateRect(self):
		if self.text.GetText():
			width = self.btnBox[self.state].baseWidth + 5 + self.text.GetTextWidth()
		else:
			width = self.btnBox[self.state].baseWidth
		height = max(self.btnBox[self.state].GetHeight(), self.text.GetTextHeight())
		self.SetSize(width, height)

		self.btnBox[self.state].SetSize(width, self.btnBox[self.state].GetHeight())
		self.text.SetPosition(self.btnBox[self.state].baseWidth + 5, 0)

		self.text.UpdateRect()
		self.btnBox[self.state].UpdateRect()
		self.UpdateRect()

	def __Refresh(self):
		self.__UpdateRect()

		self.btnBox[self.STATE_UNSELECTED].SetVisible(self.state == self.STATE_UNSELECTED)
		self.btnBox[self.STATE_SELECTED].SetVisible(self.state == self.STATE_SELECTED)

	def SAFE_SetOverInData(self, data):
		self.btnBox[self.state].SetToolTipText(data)

	def OnClickButton(self):
		if self.state == self.STATE_UNSELECTED:
			self.state = self.STATE_SELECTED
		else:
			self.state = self.STATE_UNSELECTED

		self.__Refresh()

		if self.eventFunc:
			apply(self.eventFunc, self.eventArgs)

	def SetChecked(self, state):
		self.state = state
		self.__Refresh()

	def IsChecked(self):
		return self.state != self.STATE_UNSELECTED

	def SetText(self, text):
		self.text.SetText(text)
		self.__UpdateRect()

	def SetTextColor(self, color):
		self.text.SetPackedFontColor(color)

	def SetEvent(self, event, *args):
		self.eventFunc = event
		self.eventArgs = args

	def SAFE_SetEvent(self, event, *args):
		self.eventFunc = __mem_func__(event)
		self.eventArgs = args

	def Disable(self):
		self.btnBox[self.STATE_UNSELECTED].Disable()
		self.btnBox[self.STATE_SELECTED].Disable()

	def Enable(self):
		self.btnBox[self.STATE_UNSELECTED].Enable()
		self.btnBox[self.STATE_SELECTED].Enable()

		
class BoxedBoard(Window):
	BORDER_TOP = 0
	BORDER_RIGHT = 1
	BORDER_BOTTOM = 2
	BORDER_LEFT = 3

	DEFAULT_BORDER_COLOR = grp.GenerateColor(0.3, 0.3, 0.3, 0.8)
	DEFAULT_BASE_COLOR = grp.GenerateColor(0, 0, 0, 0.5)

	def __init__(self):
		Window.__init__(self)

		self.borderSize = 1

		# Create Borders
		self.borders = [
			Bar(),
			Bar(),
			Bar(),
			Bar()
		]

		for border in self.borders:
			border.SetParent(self)
			border.AddFlag("not_pick")
			border.Show()

		# Create Base
		self.base = Bar()
		self.base.SetParent(self)
		self.base.AddFlag("not_pick")
		self.base.Show()

		# Set Default Colors
		self.SetBorderColor(self.DEFAULT_BORDER_COLOR)
		self.SetBaseColor(self.DEFAULT_BASE_COLOR)

	def __del__(self):
		self.Destroy()
		Window.__del__(self)

	def Destroy(self):
		del self.borders[:]
		self.base = None

		Window.Destroy(self)

	def SetBorderColor(self, color):
		for border in self.borders:
			border.SetColor(color)

	def SetBorderSize(self, borderSize):
		self.borderSize = borderSize
		self.SetSize(self.GetWidth(), self.GetHeight())

	def SetBaseColor(self, color):
		self.base.SetColor(color)

	def SetSize(self, width, height):
		width = max(width, (2 * self.borderSize) + 1)
		height = max(height, (2 * self.borderSize) + 1)

		Window.SetSize(self, width, height)
		self.UpdateBoard()

	def UpdateBoard(self):
		width = self.GetWidth()
		height = self.GetHeight()

		top, right, bottom, left = self.borders

		# Top Border
		top.SetSize(width - self.borderSize, self.borderSize)

		# Right Border
		right.SetSize(self.borderSize, height - self.borderSize)
		right.SetPosition(width - self.borderSize, 0)

		# Bottom Border
		bottom.SetSize(width - self.borderSize, self.borderSize)
		bottom.SetPosition(self.borderSize, height - self.borderSize)

		# Left Border
		left.SetSize(self.borderSize, height - self.borderSize)
		left.SetPosition(0, self.borderSize)

		# Base
		self.base.SetSize(width - (2 * self.borderSize), height - (2 * self.borderSize))
		self.base.SetPosition(self.borderSize, self.borderSize)


if app.ENABLE_REFINE_UI_RENEWAL or app.ENABLE_ANTI_MULTIPLE_FARM:
	class RefineCheckBox(Window):
		def __init__(self):
			Window.__init__(self)

			self.backgroundImage = None
			self.checkImage = None

			self.eventFunc = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
			self.eventArgs = { "ON_CHECK" : None, "ON_UNCKECK" : None, }

			self.CreateElements()

		def __del__(self):
			Window.__del__(self)

			self.backgroundImage = None
			self.checkImage = None

			self.eventFunc = { "ON_CHECK" : None, "ON_UNCKECK" : None, }
			self.eventArgs = { "ON_CHECK" : None, "ON_UNCKECK" : None, }

		def CreateElements(self):
			self.backgroundImage = ImageBox()
			self.backgroundImage.SetParent(self)
			self.backgroundImage.AddFlag("not_pick")
			self.backgroundImage.LoadImage("d:/ymir work/ui/game/looting/uncheck_box.sub")
			self.backgroundImage.Show()

			self.checkImage = ImageBox()
			self.checkImage.SetParent(self)
			self.checkImage.AddFlag("not_pick")
			# self.checkImage.SetPosition(0, -4)
			self.checkImage.LoadImage("d:/ymir work/ui/game/looting/check_box.sub")
			self.checkImage.Hide()

			self.textInfo = TextLine()
			self.textInfo.SetParent(self)
			self.textInfo.SetPosition(20, -2)
			if localeInfo.IsARABIC():
				self.textInfo.SetHorizontalAlignRight()
			self.textInfo.Show()

			self.SetSize(self.backgroundImage.GetWidth() + self.textInfo.GetTextSize()[0], self.backgroundImage.GetHeight() + self.textInfo.GetTextSize()[1])

		def SetTextInfo(self, info):
			if self.textInfo:
				self.textInfo.SetText(info)

			self.SetSize(self.backgroundImage.GetWidth() + self.textInfo.GetTextSize()[0], self.backgroundImage.GetHeight() + self.textInfo.GetTextSize()[1])

		def SetCheckStatus(self, flag):
			if flag:
				self.checkImage.Show()
			else:
				self.checkImage.Hide()

		def GetCheckStatus(self):
			if self.checkImage:
				return self.checkImage.IsShow()

			return False

		def SetEvent(self, func, *args) :
			result = self.eventFunc.has_key(args[0])
			if result :
				self.eventFunc[args[0]] = func
				self.eventArgs[args[0]] = args
			else :
				print "[ERROR] ui.py RefineCheckBox SetEvent, Can`t Find has_key : %s" % args[0]

		def SetBaseCheckImage(self, image):
			if not self.backgroundImage:
				return
			
			self.backgroundImage.LoadImage(image)

		def OnMouseLeftButtonUp(self):
			if self.checkImage:
				if self.checkImage.IsShow():
					self.checkImage.Hide()

					if self.eventFunc["ON_UNCKECK"]:
						apply(self.eventFunc["ON_UNCKECK"], self.eventArgs["ON_UNCKECK"])
				else:
					self.checkImage.Show()

					if self.eventFunc["ON_CHECK"]:
						apply(self.eventFunc["ON_CHECK"], self.eventArgs["ON_CHECK"])

if app.ENABLE_ANTI_MULTIPLE_FARM:
	class NewScrollBar(Window):
		SCROLLBAR_WIDTH = 17
		SCROLLBAR_MIDDLE_HEIGHT = 9
		SCROLLBAR_BUTTON_WIDTH = 17
		SCROLLBAR_BUTTON_HEIGHT = 17
		MIDDLE_BAR_POS = 0
		MIDDLE_BAR_UPPER_PLACE = 3
		MIDDLE_BAR_DOWNER_PLACE = 4
		TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE

		class MiddleBar(DragButton):
			def __init__(self):
				DragButton.__init__(self)
				self.AddFlag("movable")

			def MakeImage(self):

				middle = ExpandedImageBox()
				middle.SetParent(self)
				middle.LoadImage("d:/ymir work/ui/pattern/new_scroll/scrollbar_middle.tga")
				middle.SetPosition(0, 0)
				middle.AddFlag("not_pick")
				middle.Show()

				self.middle = middle

			def SetSize(self, height):
				height = max(12, height)
				DragButton.SetSize(self, 10, height)

				height -= 4*3
				self.middle.SetRenderingRect(0, 0, 0, float(height)/4.0)

		def __init__(self):
			Window.__init__(self)

			self.pageSize = 1
			self.curPos = 0.0
			self.eventScroll = lambda *arg: None
			self.lockFlag = False
			self.scrollStep = 0.20

			self.eventFuncCall = True

			self.CreateScrollBar()

		def __del__(self):
			Window.__del__(self)

		def CreateScrollBar(self):
			barSlot = ExpandedImageBox()
			barSlot.SetParent(self)
			barSlot.LoadImage("d:/ymir work/ui/pattern/new_scroll/base_scroll.tga")
			barSlot.AddFlag("not_pick")
			barSlot.Show()

			middleBar = self.MiddleBar()
			middleBar.SetParent(self)
			middleBar.SetMoveEvent(__mem_func__(self.OnMove))
			middleBar.Show()
			middleBar.MakeImage()
			middleBar.SetSize(12)
			
			self.middleBar = middleBar
			self.barSlot = barSlot

			self.SCROLLBAR_WIDTH = self.middleBar.GetWidth()
			self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
			self.SCROLLBAR_BUTTON_WIDTH = self.middleBar.GetWidth()
			self.SCROLLBAR_BUTTON_HEIGHT = self.middleBar.GetHeight()

		def Destroy(self):
			self.middleBar = None
			self.eventScroll = lambda *arg: None

			self.eventFuncCall	= True

		def SetEvnetFuncCall(self, callable):
			self.eventFuncCall = callable

		def SetScrollEvent(self, event):
			self.eventScroll = event

		def SetMiddleBarSize(self, pageScale):
			realHeight = self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2
			self.SCROLLBAR_MIDDLE_HEIGHT = int(pageScale * float(realHeight))
			self.middleBar.SetSize(self.SCROLLBAR_MIDDLE_HEIGHT)
			self.pageSize = (self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)

		def SetScrollBarSize(self, height):
			self.pageSize = (height - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
			self.SetSize(self.SCROLLBAR_WIDTH, height)
			self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS, self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE, self.MIDDLE_BAR_POS+2, height - self.SCROLLBAR_BUTTON_HEIGHT*2 - self.TEMP_SPACE)
			self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)

			self.UpdateBarSlot()

		def UpdateBarSlot(self):
			self.barSlot.SetPosition(0, self.SCROLLBAR_BUTTON_HEIGHT)
			height =  - self.SCROLLBAR_BUTTON_HEIGHT*2 - 2
			new_height = float(self.GetHeight()) / float(self.barSlot.GetHeight())
			self.barSlot.SetRenderingRect(0.0, 0.0, 0.0, new_height - 2.8)

		def GetPos(self):
			return self.curPos

		def SetPos(self, pos, event_callable = True):
			pos = max(0.0, pos)
			pos = min(1.0, pos)

			newPos = float(self.pageSize) * pos
			self.middleBar.SetPosition(self.MIDDLE_BAR_POS, int(newPos) + self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE)

			self.OnMove(event_callable)

		def SetScrollStep(self, step):
			self.scrollStep = step

		def GetScrollStep(self):
			return self.scrollStep

		def OnUp(self):
			self.SetPos(self.curPos-self.scrollStep, self.eventFuncCall)

		def OnDown(self):
			self.SetPos(self.curPos+self.scrollStep, self.eventFuncCall)

		def OnMove(self, event_callable = True):
			if self.lockFlag:
				return

			if 0 == self.pageSize:
				return

			(xLocal, yLocal) = self.middleBar.GetLocalPosition()
			self.curPos = float(yLocal - self.SCROLLBAR_BUTTON_HEIGHT - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)

			if event_callable:
				self.eventScroll()

		def OnMouseLeftButtonDown(self):
			(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
			pickedPos = yMouseLocalPosition - self.SCROLLBAR_BUTTON_HEIGHT - self.SCROLLBAR_MIDDLE_HEIGHT/2
			newPos = float(pickedPos) / float(self.pageSize)
			self.SetPos(newPos)

		def LockScroll(self):
			self.lockFlag = True

		def UnlockScroll(self):
			self.lockFlag = False

		def RunMouseWheel(self, nLen):
			if nLen > 0:
				self.OnUp()
			else:
				self.OnDown()

if app.ENABLE_HWID_BAN or app.ENABLE_HUNTING_SYSTEM:
	class SibBoard(Window):
		CORNER_WIDTH = 16
		CORNER_HEIGHT = 16
		LINE_WIDTH = 16
		LINE_HEIGHT = 16

		LT = 0
		LB = 1
		RT = 2
		RB = 3
		L = 0
		R = 1
		T = 2
		B = 3

		def __init__(self):
			Window.__init__(self)

			self.MakeBoard("d:/ymir work/ui/pattern/border_a_", "d:/ymir work/ui/pattern/border_a_")
			self.MakeBase()

		def MakeBoard(self, cornerPath, linePath):
			CornerFileNames = [ cornerPath+dir+".tga" for dir in ("Left_Top", "Left_Bottom", "Right_Top", "Right_Bottom", ) ]
			LineFileNames = [ linePath+dir+".tga" for dir in ("Left", "Right", "Top", "Bottom", ) ]

			self.Corners = []
			for fileName in CornerFileNames:
				Corner = ExpandedImageBox()
				Corner.AddFlag("not_pick")
				Corner.LoadImage(fileName)
				Corner.SetParent(self)
				Corner.SetPosition(0, 0)
				Corner.Show()
				self.Corners.append(Corner)

			self.Lines = []
			for fileName in LineFileNames:
				Line = ExpandedImageBox()
				Line.AddFlag("not_pick")
				Line.LoadImage(fileName)
				Line.SetParent(self)
				Line.SetPosition(0, 0)
				Line.Show()
				self.Lines.append(Line)

			self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
			self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

		def MakeBase(self):
			self.Base = ExpandedImageBox()
			self.Base.AddFlag("not_pick")
			self.Base.LoadImage("d:/ymir work/ui/pattern/border_a_center.tga")
			self.Base.SetParent(self)
			self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
			self.Base.Show()

		def __del__(self):
			Window.__del__(self)

		def SetSize(self, width, height):
			width = max(self.CORNER_WIDTH*2, width)
			height = max(self.CORNER_HEIGHT*2, height)
			Window.SetSize(self, width, height)

			self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
			self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
			self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
			self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
			self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)

			verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
			horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
			self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
			self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
			self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
			self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)

			if self.Base:
				self.Base.SetRenderingRect(0, 0, horizontalShowingPercentage, verticalShowingPercentage)

	class SibBar(Window):
		BLOCK_WIDTH = 21
		BLOCK_HEIGHT = 21

		def __init__(self):
			Window.__init__(self)
			self.AddFlag("attach")

		def __del__(self):
			Window.__del__(self)

		def Create(self, width):
			width = max(96, width)

			imgLeft = ImageBox()
			imgLeft.SetParent(self)
			imgLeft.AddFlag("not_pick")
			imgLeft.LoadImage("d:/ymir work/ui/pattern/border_c_left.tga")
			imgLeft.Show()

			imgCenter = ExpandedImageBox()
			imgCenter.SetParent(self)
			imgCenter.AddFlag("not_pick")
			imgCenter.LoadImage("d:/ymir work/ui/pattern/border_c_middle.tga")
			imgCenter.Show()

			imgRight = ImageBox()
			imgRight.SetParent(self)
			imgRight.AddFlag("not_pick")
			imgRight.LoadImage("d:/ymir work/ui/pattern/border_c_right.tga")
			imgRight.Show()

			self.imgLeft = imgLeft
			self.imgCenter = imgCenter
			self.imgRight = imgRight
			self.SetWidth(width)

		def SetWidth(self, width):
			self.imgCenter.SetRenderingRect(0.0, 0.0, float((width - self.BLOCK_WIDTH*2) - self.BLOCK_WIDTH) / self.BLOCK_WIDTH, 0.0)
			self.imgCenter.SetPosition(self.BLOCK_WIDTH, 0)
			self.imgRight.SetPosition(width - self.BLOCK_WIDTH, 0)
			self.SetSize(width, self.BLOCK_HEIGHT)

##  gif support
class GifBox(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		self.eventDict={}
		self.arg = -1
		self.argOut = -1
		self.imageLoaded = False

	def __del__(self):
		Window.__del__(self)

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterGifImageBox(self, layer)

	def LoadImage(self, imageName):
		self.name=imageName
		wndMgr.LoadGif(self.hWnd, imageName)
		self.imageLoaded = True

		if len(self.eventDict)!=0:
			print "LOAD IMAGE", self, self.eventDict

	def UnloadImage(self):
		if self.imageLoaded:
			wndMgr.UnloadGif(self.hWnd)
			self.imageLoaded = False

	def SetAlpha(self, alpha):
		wndMgr.SetDiffuseColor(self.hWnd, 1.0, 1.0, 1.0, alpha)

	def SetDiffuseColor(self, r,g,b,a=1.0):
		wndMgr.SetDiffuseColor(self.hWnd, r,g,b,a)

	def SetAlphaColor(self, r,g,b,alpha=1.0):
		wndMgr.SetDiffuseColor(self.hWnd, r,g,b, alpha)

	def GetWidth(self):
		return wndMgr.GetWidth(self.hWnd)

	def GetHeight(self):
		return wndMgr.GetHeight(self.hWnd)

	def SetRenderingRect(self, left, top, right, bottom):
		wndMgr.SetRenderingRect(self.hWnd, left, top, right, bottom)

	def SetPercentage(self, curValue, maxValue):
		if maxValue:
			self.SetRenderingRect(0.0, 0.0, -1.0 + float(curValue) / float(maxValue), 0.0)
		else:
			self.SetRenderingRect(0.0, 0.0, 0.0, 0.0)

	def SetScale(self, xScale, yScale):
			wndMgr.SetScale(self.hWnd, xScale, yScale)

class ScrollBarTemplate(Window):

	MIDDLE_BAR_POS = 5
	MIDDLE_BAR_UPPER_PLACE = 2
	MIDDLE_BAR_DOWNER_PLACE = 2
	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE

	class MiddleBar(DragButton):
		def __init__(self):
			self.middle = None
			DragButton.__init__(self)
			self.AddFlag("movable")

		def MakeImage(self, img):
			middle = ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(img)
			middle.SetPosition(0, 0)
			middle.AddFlag("not_pick")
			middle.Show()
			self.middle = middle
			self.SetSize(self.GetHeight())

		def SetSize(self, height):
			height = max(12, height)
			if self.middle:
				DragButton.SetSize(self, self.middle.GetWidth(), height)
				val = 0
				if self.middle.GetHeight() != 0:
					val = float(height)/self.middle.GetHeight()
				self.middle.SetRenderingRect(0, 0, 0, -1.0 + val)
			else:
				DragButton.SetSize(self, self.GetWidth(), height)

	def __init__(self):
		Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False

		self.SCROLLBAR_WIDTH = 0
		self.SCROLLBAR_BUTTON_WIDTH = 0
		self.SCROLLBAR_BUTTON_HEIGHT = 0
		self.SCROLLBAR_MIDDLE_HEIGHT = 0

		self.CreateScrollBar()

		self.scrollStep = 0.20

	def SetUpButton(self, upVisual, overVisual, downVisual):
		self.upButton.SetUpVisual(upVisual)
		self.upButton.SetOverVisual(overVisual)
		self.upButton.SetDownVisual(downVisual)
		self.upButton.Show()
		self.SCROLLBAR_BUTTON_WIDTH = self.upButton.GetWidth()
		self.SCROLLBAR_BUTTON_HEIGHT = self.upButton.GetHeight() + 3

	def SetDownButton(self, upVisual, overVisual, downVisual):
		self.downButton.SetUpVisual(upVisual)
		self.downButton.SetOverVisual(overVisual)
		self.downButton.SetDownVisual(downVisual)
		self.downButton.Show()

	def SetMiddleImage(self, img):
		self.middleBar.MakeImage(img)
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()
		self.MIDDLE_BAR_POS = (self.SCROLLBAR_WIDTH - self.middleBar.GetWidth()) / 2

	def SetBarPartImages(self, topImg, centerImg, bottomImg):
		self.barTopImage.LoadImage(topImg)
		self.barTopImage.Show()
		self.barCenterImage.LoadImage(centerImg)
		self.barCenterImage.SetPosition(0, self.barTopImage.GetHeight())
		self.barCenterImage.Show()
		self.barBottomImage.LoadImage(bottomImg)
		self.barBottomImage.Show()
		self.SCROLLBAR_WIDTH = max(self.barTopImage.GetWidth(), self.SCROLLBAR_WIDTH)
		self.MIDDLE_BAR_POS = (self.SCROLLBAR_WIDTH - self.middleBar.GetWidth()) / 2

	def SetBarImage(self, img):
		self.barImage.LoadImage(img)
		self.barImage.Show()
		self.SCROLLBAR_WIDTH = max(self.barImage.GetWidth(), self.SCROLLBAR_WIDTH)
		self.MIDDLE_BAR_POS = (self.SCROLLBAR_WIDTH - self.middleBar.GetWidth()) / 2

	def CreateScrollBar(self):
		barImage = ExpandedImageBox()
		barImage.SetParent(self)
		barImage.AddFlag("not_pick")
		barImage.Hide()

		barTopImage = ImageBox()
		barTopImage.SetParent(self)
		barTopImage.AddFlag("not_pick")
		barTopImage.Hide()

		barCenterImage = ExpandedImageBox()
		barCenterImage.SetParent(self)
		barCenterImage.AddFlag("not_pick")
		barCenterImage.Hide()

		barBottomImage = ImageBox()
		barBottomImage.SetParent(self)
		barBottomImage.AddFlag("not_pick")
		barBottomImage.Hide()

		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.SetSize(150)

		upButton = Button()
		upButton.SetParent(self)
		upButton.SetEvent(__mem_func__(self.OnUp))
		upButton.SetWindowHorizontalAlignCenter()
		upButton.Hide()

		downButton = Button()
		downButton.SetParent(self)
		downButton.SetEvent(__mem_func__(self.OnDown))
		downButton.SetWindowHorizontalAlignCenter()
		downButton.Hide()

		self.upButton = upButton
		self.downButton = downButton
		self.middleBar = middleBar
		self.barImage = barImage
		self.barTopImage = barTopImage
		self.barCenterImage = barCenterImage
		self.barBottomImage = barBottomImage

	def Destroy(self):
		self.middleBar = None
		self.upButton = None
		self.downButton = None
		self.barImage = None
		self.barTopImage = None
		self.barCenterImage = None
		self.barBottomImage = None
		self.eventScroll = None
		self.eventArgs = None

	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args

	# ------------------------------------------------------------------------------------------

	# Important: pageScale must be float! so parse the values to float before you use them.
	# Otherwise it simply won't work or the bar is gonna be very small

	# ------------------------------------------------------------------------------------------

	def SetMiddleBarSize(self, pageScale):
		realHeight = self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2
		self.SCROLLBAR_MIDDLE_HEIGHT = max(12, int(pageScale * float(realHeight)))
		self.middleBar.SetSize(self.SCROLLBAR_MIDDLE_HEIGHT)
		self.pageSize = (self.GetHeight() - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)

	def SetScrollBarSize(self, height):
		self.pageSize = (height - self.SCROLLBAR_BUTTON_HEIGHT*2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.upButton.SetPosition(0, 3)
		self.downButton.SetPosition(0, height - self.SCROLLBAR_BUTTON_HEIGHT)
		self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS, self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE, self.middleBar.GetWidth(), height - self.SCROLLBAR_BUTTON_HEIGHT*2 - self.TEMP_SPACE)
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)

		self.UpdateBarImage()
		self.upButton.UpdateRect()
		self.downButton.UpdateRect()

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def UpdateBarImage(self):
		if self.barImage.IsShow():
			val = 0

			if self.barImage.GetHeight() != 0:
				val = self.GetHeight() / float(self.barImage.GetHeight())

			self.barImage.SetRenderingRect(0.0, 0.0, 0.0, -1.0 + val)

		if self.barCenterImage.IsShow():
			centerHeight = self.GetHeight() - (self.barTopImage.GetHeight() + self.barBottomImage.GetHeight())

			val = 0

			if self.barCenterImage.GetHeight() != 0:
				val = (centerHeight / float(self.barCenterImage.GetHeight()))

			self.barCenterImage.SetRenderingRect(0.0, 0.0, 0.0, -1.0 + val)

		if self.barBottomImage.IsShow():
			self.barBottomImage.SetPosition(0, self.GetHeight() - self.barBottomImage.GetHeight())

	def GetPos(self):
		return self.curPos

	def OnMouseWheel(self, nLen):
		if nLen > 0:
			self.OnUp()
			return True
		elif nLen < 0:
			self.OnDown()
			return True
		return False

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos

		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, int(newPos) + self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE)
		self.OnMove()

	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)

	def OnMove(self):

		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()

		self.curPos = float(yLocal - self.SCROLLBAR_BUTTON_HEIGHT - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)

		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()

		pickedPos = yMouseLocalPosition - self.SCROLLBAR_BUTTON_HEIGHT - self.SCROLLBAR_MIDDLE_HEIGHT/2
		newPos = float(pickedPos) / float(self.pageSize)

		self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False

###################################################################################################
## Python Script Loader
###################################################################################################

class ScriptWindow(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)
		self.Children = []
		self.ElementDictionary = {}
	def __del__(self):
		Window.__del__(self)

	def ClearDictionary(self):
		self.Children = []
		self.ElementDictionary = {}
	def InsertChild(self, name, child):
		self.ElementDictionary[name] = child

	def IsChild(self, name):
		return self.ElementDictionary.has_key(name)
	def GetChild(self, name):
		return self.ElementDictionary[name]

	def GetChild2(self, name):
		return self.ElementDictionary.get(name, None)


class PythonScriptLoader(object):

	BODY_KEY_LIST = ( "x", "y", "width", "height" )

	#####

	DEFAULT_KEY_LIST = ( "type", "x", "y", )
	WINDOW_KEY_LIST = ( "width", "height", )
	IMAGE_KEY_LIST = ( "image", )
	EXPANDED_IMAGE_KEY_LIST = ( "image", )
	ANI_IMAGE_KEY_LIST = ( "images", )
	SLOT_KEY_LIST = ( "width", "height", "slot", )
	CANDIDATE_LIST_KEY_LIST = ( "item_step", "item_xsize", "item_ysize", )
	GRID_TABLE_KEY_LIST = ( "start_index", "x_count", "y_count", "x_step", "y_step", )
	EDIT_LINE_KEY_LIST = ( "width", "height", "input_limit", )
	COMBO_BOX_KEY_LIST = ( "width", "height", "item", )
	TITLE_BAR_KEY_LIST = ( "width", )
	HORIZONTAL_BAR_KEY_LIST = ( "width", )
	BOARD_KEY_LIST = ( "width", "height", )
	BOARD_WITH_TITLEBAR_KEY_LIST = ( "width", "height", "title", )
	BOX_KEY_LIST = ( "width", "height", )
	BAR_KEY_LIST = ( "width", "height", )
	LINE_KEY_LIST = ( "width", "height", )
	SLOTBAR_KEY_LIST = ( "width", "height", )
	GAUGE_KEY_LIST = ( "width", "color", )
	SCROLLBAR_KEY_LIST = ( "size", )
	LIST_BOX_KEY_LIST = ( "width", "height", )
	if app.ENABLE_RENDER_TARGET:
		RENDER_TARGET_KEY_LIST = ( "index", )

	def __init__(self):
		self.Clear()

	def Clear(self):
		self.ScriptDictionary = { "SCREEN_WIDTH" : wndMgr.GetScreenWidth(), "SCREEN_HEIGHT" : wndMgr.GetScreenHeight() }
		self.InsertFunction = 0

	def LoadScriptFile(self, window, FileName):
		import exception
		import exceptions
		import os
		import errno
		self.Clear()

		print("===== Load Script File : %s" % (FileName))

		import sys
		from utils import Sandbox
		sandbox = Sandbox(True, ["uiScriptLocale", "localeInfo", "sys", "item", "app", "player","utils"])

		import chr
		import player
		import app
		self.ScriptDictionary["PLAYER_NAME_MAX_LEN"] = chr.PLAYER_NAME_MAX_LEN
		self.ScriptDictionary["DRAGON_SOUL_EQUIPMENT_SLOT_START"] = player.DRAGON_SOUL_EQUIPMENT_SLOT_START
		self.ScriptDictionary["LOCALE_PATH"] = app.GetLocalePath()

		if __USE_EXTRA_CYTHON__:
			# sub functions
			from os.path import splitext as op_splitext, basename as op_basename, dirname as op_dirname
			def GetModName(filename):
				return op_splitext(op_basename(filename))[0]
			def IsInUiPath(filename):
				def ICmp(s1, s2):
					return s1.lower() == s2.lower()
				return ICmp(op_dirname(filename), "uiscript")
			# module name to import
			modname = GetModName(FileName)
			# lazy loading of uiscriptlib
			import uiscriptlib
			# copy scriptdictionary stuff to builtin scope (otherwise, import will fail)
			tpl2Main = (
				"SCREEN_WIDTH","SCREEN_HEIGHT",
				"PLAYER_NAME_MAX_LEN", "DRAGON_SOUL_EQUIPMENT_SLOT_START","LOCALE_PATH"
			)
			import __builtin__ as bt
			for idx in tpl2Main:
				tmpVal = self.ScriptDictionary[idx]
				exec "bt.%s = tmpVal"%idx in globals(), locals()
			# debug stuff
			import dbg
			dbg.TraceError("Loading %s (%s %s)"%(FileName, GetModName(FileName), IsInUiPath(FileName)))
		try:
			if __USE_EXTRA_CYTHON__ and IsInUiPath(FileName) and uiscriptlib.isExist(modname):
				m1 = uiscriptlib.moduleImport(modname)
				self.ScriptDictionary["window"] = m1.window.copy()
				del m1
			else:
				sandbox.execfile(FileName, self.ScriptDictionary)
		except IOError as err:
			import sys
			import dbg
			dbg.TraceError("Failed to load script file : %s" % (FileName))
			dbg.TraceError("error  : %s" % (err))
			exception.Abort("LoadScriptFile1")
		except RuntimeError as err:
			import sys
			import dbg
			dbg.TraceError("Failed to load script file : %s" % (FileName))
			dbg.TraceError("error  : %s" % (err))
			exception.Abort("LoadScriptFile2")
		except:
			import sys
			import dbg
			dbg.TraceError("Failed to load script file : %s" % (FileName))
			exception.Abort("LoadScriptFile!!!!!!!!!!!!!!")

		#####

		Body = self.ScriptDictionary["window"]
		self.CheckKeyList("window", Body, self.BODY_KEY_LIST)

		window.ClearDictionary()
		self.InsertFunction = window.InsertChild

		window.SetPosition(int(Body["x"]), int(Body["y"]))

		if localeInfo.IsARABIC():
			w = wndMgr.GetScreenWidth()
			h = wndMgr.GetScreenHeight()
			if Body.has_key("width"):
				w = int(Body["width"])
			if Body.has_key("height"):
				h = int(Body["height"])

			window.SetSize(w, h)
		else:
			window.SetSize(int(Body["width"]), int(Body["height"]))
			if True == Body.has_key("style"):
				for StyleList in Body["style"]:
					window.AddFlag(StyleList)


		self.LoadChildren(window, Body)

	def LoadChildren(self, parent, dicChildren):

		if localeInfo.IsARABIC():
			parent.AddFlag( "rtl" )

		if True == dicChildren.has_key("style"):
			for style in dicChildren["style"]:
				parent.AddFlag(style)

		if False == dicChildren.has_key("children"):
			return False

		Index = 0

		ChildrenList = dicChildren["children"]
		parent.Children = range(len(ChildrenList))
		for ElementValue in ChildrenList:
			try:
				Name = ElementValue["name"]
			except KeyError:
				Name = ElementValue["name"] = "NONAME"

			try:
				Type = ElementValue["type"]
			except KeyError:
				Type = ElementValue["type"] = "window"

			if False == self.CheckKeyList(Name, ElementValue, self.DEFAULT_KEY_LIST):
				del parent.Children[Index]
				continue

			if Type == "window":
				parent.Children[Index] = ScriptWindow()
				parent.Children[Index].SetParent(parent)
				self.LoadElementWindow(parent.Children[Index], ElementValue, parent)

			elif Type == "button":
				parent.Children[Index] = Button()
				parent.Children[Index].SetParent(parent)
				self.LoadElementButton(parent.Children[Index], ElementValue, parent)

			elif Type == "radio_button":
				parent.Children[Index] = RadioButton()
				parent.Children[Index].SetParent(parent)
				self.LoadElementButton(parent.Children[Index], ElementValue, parent)

			elif Type == "toggle_button":
				parent.Children[Index] = ToggleButton()
				parent.Children[Index].SetParent(parent)
				self.LoadElementButton(parent.Children[Index], ElementValue, parent)

			elif Type == "mark":
				parent.Children[Index] = MarkBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementMark(parent.Children[Index], ElementValue, parent)

			elif Type == "image":
				parent.Children[Index] = ImageBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementImage(parent.Children[Index], ElementValue, parent)

			elif Type == "expanded_image":
				parent.Children[Index] = ExpandedImageBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementExpandedImage(parent.Children[Index], ElementValue, parent)

			elif Type == "ani_image":
				parent.Children[Index] = AniImageBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementAniImage(parent.Children[Index], ElementValue, parent)

			elif Type == "slot":
				parent.Children[Index] = SlotWindow()
				parent.Children[Index].SetParent(parent)
				self.LoadElementSlot(parent.Children[Index], ElementValue, parent)

			elif Type == "candidate_list":
				parent.Children[Index] = CandidateListBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementCandidateList(parent.Children[Index], ElementValue, parent)

			elif Type == "grid_table":
				parent.Children[Index] = GridSlotWindow()
				parent.Children[Index].SetParent(parent)
				self.LoadElementGridTable(parent.Children[Index], ElementValue, parent)

			elif Type == "text":
				parent.Children[Index] = TextLine()
				parent.Children[Index].SetParent(parent)
				self.LoadElementText(parent.Children[Index], ElementValue, parent)

			elif Type == "editline":
				parent.Children[Index] = EditLine()
				parent.Children[Index].SetParent(parent)
				self.LoadElementEditLine(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar":
				parent.Children[Index] = TitleBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_small":
				parent.Children[Index] = TitleBarFinalCoreSmall()
				parent.Children[Index].SetParent(parent)
				self.LoadElementTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_middle":
				parent.Children[Index] = TitleBarFinalCoreMiddle()
				parent.Children[Index].SetParent(parent)
				self.LoadElementTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_middle_noclose":
				parent.Children[Index] = TitleBarFinalCoreMiddleNoClose()
				parent.Children[Index].SetParent(parent)
				self.LoadElementTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large":
				parent.Children[Index] = TitleBarFinalCoreLarge()
				parent.Children[Index].SetParent(parent)
				self.LoadElementTitleBar(parent.Children[Index], ElementValue, parent)

############################################################
			elif Type == "titlebar_finalcore_large1":
				parent.Children[Index] = BoardWithFinalCoreTitleBar6()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large2":
				parent.Children[Index] = BoardWithFinalCoreTitleBar8()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large3":
				parent.Children[Index] = BoardWithFinalCoreTitleBar9()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large4":
				parent.Children[Index] = BoardWithFinalCoreTitleBar11()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large5":
				parent.Children[Index] = BoardWithFinalCoreTitleBar12()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large6":
				parent.Children[Index] = BoardWithFinalCoreTitleBar13()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large7":
				parent.Children[Index] = BoardWithFinalCoreTitleBar15()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large8":
				parent.Children[Index] = BoardWithFinalCoreTitleBar16()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large9":
				parent.Children[Index] = BoardWithFinalCoreTitleBar17()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "titlebar_finalcore_large10":
				parent.Children[Index] = BoardWithFinalCoreTitleBar18()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

############################################################ fratello
			elif Type == "horizontalbar":
				parent.Children[Index] = HorizontalBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementHorizontalBar(parent.Children[Index], ElementValue, parent)

			elif Type == "board":
				parent.Children[Index] = Board()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoard(parent.Children[Index], ElementValue, parent)

			elif Type == "board_finalcore":
				parent.Children[Index] = BoardFinalCore()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoard(parent.Children[Index], ElementValue, parent)

			elif Type == "board_with_titlebar":
				parent.Children[Index] = BoardWithTitleBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "board_with_titlebar2":
				parent.Children[Index] = BoardWithFinalCoreTitleBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			elif Type == "thinboard":
				parent.Children[Index] = ThinBoard()
				parent.Children[Index].SetParent(parent)
				self.LoadElementThinBoard(parent.Children[Index], ElementValue, parent)

			elif Type == "thinboard_gold":
				parent.Children[Index] = ThinBoardGold()
				parent.Children[Index].SetParent(parent)
				self.LoadElementThinBoard(parent.Children[Index], ElementValue, parent)

			elif Type == "thinboard_circle":
				parent.Children[Index] = ThinBoardCircle()
				parent.Children[Index].SetParent(parent)
				self.LoadElementThinBoard(parent.Children[Index], ElementValue, parent)

			elif Type == "box":
				parent.Children[Index] = Box()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBox(parent.Children[Index], ElementValue, parent)

			elif Type == "bar":
				parent.Children[Index] = Bar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBar(parent.Children[Index], ElementValue, parent)

			elif Type == "line":
				parent.Children[Index] = Line()
				parent.Children[Index].SetParent(parent)
				self.LoadElementLine(parent.Children[Index], ElementValue, parent)

			elif Type == "slotbar":
				parent.Children[Index] = SlotBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementSlotBar(parent.Children[Index], ElementValue, parent)

			elif Type == "gauge":
				parent.Children[Index] = Gauge()
				parent.Children[Index].SetParent(parent)
				self.LoadElementGauge(parent.Children[Index], ElementValue, parent)

			elif Type == "scrollbar":
				parent.Children[Index] = ScrollBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "thin_scrollbar":
				parent.Children[Index] = ThinScrollBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "small_thin_scrollbar":
				parent.Children[Index] = SmallThinScrollBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif Type == "sliderbar":
				parent.Children[Index] = SliderBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementSliderBar(parent.Children[Index], ElementValue, parent)

			elif Type == "listbox":
				parent.Children[Index] = ListBox()
				parent.Children[Index].SetParent(parent)
				self.LoadElementListBox(parent.Children[Index], ElementValue, parent)

			elif Type == "listbox2":
				parent.Children[Index] = ListBox2()
				parent.Children[Index].SetParent(parent)
				self.LoadElementListBox2(parent.Children[Index], ElementValue, parent)
			elif Type == "listboxex":
				parent.Children[Index] = ListBoxEx()
				parent.Children[Index].SetParent(parent)
				self.LoadElementListBoxEx(parent.Children[Index], ElementValue, parent)
				
			elif app.ENABLE_RENDER_TARGET and Type == "render_target":
				parent.Children[Index] = RenderTarget()
				parent.Children[Index].SetParent(parent)
				self.LoadElementRenderTarget(parent.Children[Index], ElementValue, parent)

			elif app.ENABLE_PREMIUM_PRIVATE_SHOP and Type == "thinboard_deco":
				parent.Children[Index] = ShopDecoThinboard()
				parent.Children[Index].SetParent(parent)
				self.LoadElementThinBoard(parent.Children[Index], ElementValue, parent)

			elif Type == "checkbox":
				parent.Children[Index] = CheckBoxEx()
				parent.Children[Index].SetParent(parent)
				self.LoadElementCheckBox(parent.Children[Index], ElementValue, parent)

			elif Type == "boxed_board":
				parent.Children[Index] = BoxedBoard()
				parent.Children[Index].SetParent(parent)
				self.LoadElementThinBoard(parent.Children[Index], ElementValue, parent)

			elif app.ENABLE_HWID_BAN and Type == "sibboard":
				parent.Children[Index] = SibBoard()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoard(parent.Children[Index], ElementValue, parent)

			elif app.ENABLE_HWID_BAN and Type == "sibbar":
				parent.Children[Index] = SibBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementHorizontalBar(parent.Children[Index], ElementValue, parent)

			elif app.ENABLE_ANTI_MULTIPLE_FARM and  Type == "new_scrollbar":
				parent.Children[Index] = NewScrollBar()
				parent.Children[Index].SetParent(parent)
				self.LoadElementScrollBar(parent.Children[Index], ElementValue, parent)

			elif app.ENABLE_ANTI_MULTIPLE_FARM and Type == "board_with_titlebar_without_button":
				parent.Children[Index] = BoardWithTitleBar(True)
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoardWithTitleBar(parent.Children[Index], ElementValue, parent)

			## if app.ENABLE_BATTLE_PASS:
			elif Type == "border_a":
				parent.Children[Index] = BorderA()
				parent.Children[Index].SetParent(parent)
				self.LoadElementBoard(parent.Children[Index], ElementValue, parent)

			else:
				Index += 1
				continue

			parent.Children[Index].SetWindowName(Name)
			if 0 != self.InsertFunction:
				self.InsertFunction(Name, parent.Children[Index])

			self.LoadChildren(parent.Children[Index], ElementValue)
			Index += 1

	def CheckKeyList(self, name, value, key_list):
		for DataKey in key_list:
			if False == value.has_key(DataKey):
				print("Failed to find data key", "[" + name + "/" + DataKey + "]")
				return False

		return True

	def LoadDefaultData(self, window, value, parentWindow):
		loc_x = int(value["x"])
		loc_y = int(value["y"])
		if value.has_key("vertical_align"):
			if "center" == value["vertical_align"]:
				window.SetWindowVerticalAlignCenter()
			elif "bottom" == value["vertical_align"]:
				window.SetWindowVerticalAlignBottom()

		if parentWindow.IsRTL():
			loc_x = int(value["x"]) + window.GetWidth()
			if value.has_key("horizontal_align"):
				if "center" == value["horizontal_align"]:
					window.SetWindowHorizontalAlignCenter()
					loc_x = - int(value["x"])
				elif "right" == value["horizontal_align"]:
					window.SetWindowHorizontalAlignLeft()
					loc_x = int(value["x"]) - window.GetWidth()
					## loc_x = parentWindow.GetWidth() - int(value["x"]) + window.GetWidth()
			else:
				window.SetWindowHorizontalAlignRight()

			if value.has_key("all_align"):
				window.SetWindowVerticalAlignCenter()
				window.SetWindowHorizontalAlignCenter()
				loc_x = - int(value["x"])
		else:
			if value.has_key("horizontal_align"):
				if "center" == value["horizontal_align"]:
					window.SetWindowHorizontalAlignCenter()
				elif "right" == value["horizontal_align"]:
					window.SetWindowHorizontalAlignRight()

		window.SetPosition(loc_x, loc_y)
		window.Show()

	## Window
	def LoadElementWindow(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.WINDOW_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Button
	def LoadElementButton(self, window, value, parentWindow):

		if value.has_key("width") and value.has_key("height"):
			window.SetSize(int(value["width"]), int(value["height"]))

		if True == value.has_key("default_image"):
			window.SetUpVisual(value["default_image"])
		if True == value.has_key("over_image"):
			window.SetOverVisual(value["over_image"])
		if True == value.has_key("down_image"):
			window.SetDownVisual(value["down_image"])
		if True == value.has_key("disable_image"):
			window.SetDisableVisual(value["disable_image"])

		if True == value.has_key("text"):
			if True == value.has_key("text_height"):
				window.SetText(value["text"], value["text_height"])
			elif app.ENABLE_NEW_GAMEOPTION and True == value.has_key("text_x"):
				window.SetListText(value["text"], value["text_x"])
			else:
				window.SetText(value["text"])

			if value.has_key("text_color"):
				window.SetTextColor(value["text_color"])

		if True == value.has_key("tooltip_text"):
			if True == value.has_key("tooltip_x") and True == value.has_key("tooltip_y"):
				window.SetToolTipText(value["tooltip_text"], int(value["tooltip_x"]), int(value["tooltip_y"]))
			else:
				window.SetToolTipText(value["tooltip_text"])

		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Mark
	def LoadElementMark(self, window, value, parentWindow):

		#if False == self.CheckKeyList(value["name"], value, self.MARK_KEY_LIST):
		#	return False

		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Image
	def LoadElementImage(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.IMAGE_KEY_LIST):
			return False

		window.LoadImage(value["image"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## AniImage
	def LoadElementAniImage(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.ANI_IMAGE_KEY_LIST):
			return False

		if True == value.has_key("delay"):
			window.SetDelay(value["delay"])

		for image in value["images"]:
			window.AppendImage(image)

		if value.has_key("width") and value.has_key("height"):
			window.SetSize(value["width"], value["height"])

		if True == value.has_key("x_scale") and True == value.has_key("y_scale"):
			window.SetScale(float(value["x_scale"]), float(value["y_scale"]))

		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Expanded Image
	def LoadElementExpandedImage(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.EXPANDED_IMAGE_KEY_LIST):
			return False

		window.LoadImage(value["image"])

		if True == value.has_key("x_origin") and True == value.has_key("y_origin"):
			window.SetOrigin(float(value["x_origin"]), float(value["y_origin"]))

		if True == value.has_key("x_scale") and True == value.has_key("y_scale"):
			window.SetScale(float(value["x_scale"]), float(value["y_scale"]))

		if True == value.has_key("rect"):
			RenderingRect = value["rect"]
			window.SetRenderingRect(RenderingRect[0], RenderingRect[1], RenderingRect[2], RenderingRect[3])

		if True == value.has_key("mode"):
			mode = value["mode"]
			if "MODULATE" == mode:
				window.SetRenderingMode(wndMgr.RENDERING_MODE_MODULATE)

		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Slot
	def LoadElementSlot(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.SLOT_KEY_LIST):
			return False

		global_x = int(value["x"])
		global_y = int(value["y"])
		global_width = int(value["width"])
		global_height = int(value["height"])

		window.SetPosition(global_x, global_y)
		window.SetSize(global_width, global_height)
		window.Show()

		r = 1.0
		g = 1.0
		b = 1.0
		a = 1.0

		if True == value.has_key("image_r") and \
			True == value.has_key("image_g") and \
			True == value.has_key("image_b") and \
			True == value.has_key("image_a"):
			r = float(value["image_r"])
			g = float(value["image_g"])
			b = float(value["image_b"])
			a = float(value["image_a"])

		if True == value.has_key("x_scale") and True == value.has_key("y_scale"):
			window.SetSlotScale(float(value["x_scale"]), float(value["y_scale"]))

		SLOT_ONE_KEY_LIST = ("index", "x", "y", "width", "height")

		for slot in value["slot"]:
			if True == self.CheckKeyList(value["name"] + " - one", slot, SLOT_ONE_KEY_LIST):
				wndMgr.AppendSlot(window.hWnd,
									int(slot["index"]),
									int(slot["x"]),
									int(slot["y"]),
									int(slot["width"]),
									int(slot["height"]))

		if True == value.has_key("image"):
			wndMgr.SetSlotBaseImage(window.hWnd,
									value["image"],
									r, g, b, a)

		return True

	def LoadElementCandidateList(self, window, value, parentWindow):
		if False == self.CheckKeyList(value["name"], value, self.CANDIDATE_LIST_KEY_LIST):
			return False

		window.SetPosition(int(value["x"]), int(value["y"]))
		window.SetItemSize(int(value["item_xsize"]), int(value["item_ysize"]))
		window.SetItemStep(int(value["item_step"]))
		window.Show()

		return True

	## Table
	def LoadElementGridTable(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.GRID_TABLE_KEY_LIST):
			return False

		xBlank = 0
		yBlank = 0
		if True == value.has_key("x_blank"):
			xBlank = int(value["x_blank"])
		if True == value.has_key("y_blank"):
			yBlank = int(value["y_blank"])

		if localeInfo.IsARABIC():
			pass
		else:
			window.SetPosition(int(value["x"]), int(value["y"]))

		window.ArrangeSlot(	int(value["start_index"]),
							int(value["x_count"]),
							int(value["y_count"]),
							int(value["x_step"]),
							int(value["y_step"]),
							xBlank,
							yBlank)
		if True == value.has_key("image"):
			r = 1.0
			g = 1.0
			b = 1.0
			a = 1.0
			if True == value.has_key("image_r") and \
				True == value.has_key("image_g") and \
				True == value.has_key("image_b") and \
				True == value.has_key("image_a"):
				r = float(value["image_r"])
				g = float(value["image_g"])
				b = float(value["image_b"])
				a = float(value["image_a"])
			wndMgr.SetSlotBaseImage(window.hWnd, value["image"], r, g, b, a)

		if True == value.has_key("style"):
			if "select" == value["style"]:
				wndMgr.SetSlotStyle(window.hWnd, wndMgr.SLOT_STYLE_SELECT)
		if localeInfo.IsARABIC():
			self.LoadDefaultData(window, value, parentWindow)
		else:
			window.Show()

		return True

	## Text
	def LoadElementText(self, window, value, parentWindow):

		if value.has_key("fontsize"):
			fontSize = value["fontsize"]

			if "LARGE" == fontSize:
				window.SetFontName(localeInfo.UI_DEF_FONT_LARGE)

		elif value.has_key("fontname"):
			fontName = value["fontname"]
			window.SetFontName(fontName)

		if value.has_key("text_horizontal_align"):
			if "left" == value["text_horizontal_align"]:
				window.SetHorizontalAlignLeft()
			elif "center" == value["text_horizontal_align"]:
				window.SetHorizontalAlignCenter()
			elif "right" == value["text_horizontal_align"]:
				window.SetHorizontalAlignRight()

		if value.has_key("text_vertical_align"):
			if "top" == value["text_vertical_align"]:
				window.SetVerticalAlignTop()
			elif "center" == value["text_vertical_align"]:
				window.SetVerticalAlignCenter()
			elif "bottom" == value["text_vertical_align"]:
				window.SetVerticalAlignBottom()

		if value.has_key("all_align"):
			window.SetHorizontalAlignCenter()
			window.SetVerticalAlignCenter()
			window.SetWindowHorizontalAlignCenter()
			window.SetWindowVerticalAlignCenter()

		if value.has_key("r") and value.has_key("g") and value.has_key("b"):
			window.SetFontColor(float(value["r"]), float(value["g"]), float(value["b"]))
		elif value.has_key("color"):
			window.SetPackedFontColor(value["color"])
		else:
			window.SetFontColor(0.8549, 0.8549, 0.8549)

		if value.has_key("outline"):
			if value["outline"]:
				window.SetOutline()
		if True == value.has_key("text"):
			window.SetText(value["text"])

		self.LoadDefaultData(window, value, parentWindow)

		return True

	## EditLine
	def LoadElementEditLine(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.EDIT_LINE_KEY_LIST):
			return False


		if value.has_key("secret_flag"):
			window.SetSecret(value["secret_flag"])
		if value.has_key("with_codepage"):
			if value["with_codepage"]:
				window.bCodePage = True
		if value.has_key("only_number"):
			if value["only_number"]:
				window.SetNumberMode()
		if value.has_key("enable_codepage"):
			window.SetIMEFlag(value["enable_codepage"])
		if value.has_key("enable_ime"):
			window.SetIMEFlag(value["enable_ime"])
		if value.has_key("limit_width"):
			window.SetLimitWidth(value["limit_width"])
		if value.has_key("multi_line"):
			if value["multi_line"]:
				window.SetMultiLine()
		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			if value.has_key("only_currency"):
				if value["only_currency"]:
					window.SetCurrencyMode()

		window.SetMax(int(value["input_limit"]))
		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadElementText(window, value, parentWindow)

		return True

	## TitleBar
	def LoadElementTitleBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.TITLE_BAR_KEY_LIST):
			return False

		window.MakeTitleBar(int(value["width"]), value.get("color", "red"))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## HorizontalBar
	def LoadElementHorizontalBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.HORIZONTAL_BAR_KEY_LIST):
			return False

		window.Create(int(value["width"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Board
	def LoadElementBoard(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BOARD_KEY_LIST):
			return False

		if app.ENABLE_BATTLE_PASS:
			if True == value.has_key("size_skip"):
				window.skipMaxCheck = True

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Board With TitleBar
	def LoadElementBoardWithTitleBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BOARD_WITH_TITLEBAR_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		window.SetTitleName(value["title"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## ThinBoard
	def LoadElementThinBoard(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BOARD_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Box
	def LoadElementBox(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BOX_KEY_LIST):
			return False

		if True == value.has_key("color"):
			window.SetColor(value["color"])

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Bar
	def LoadElementBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.BAR_KEY_LIST):
			return False

		if True == value.has_key("color"):
			window.SetColor(value["color"])

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Line
	def LoadElementLine(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.LINE_KEY_LIST):
			return False

		if True == value.has_key("color"):
			window.SetColor(value["color"])

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Slot
	def LoadElementSlotBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.SLOTBAR_KEY_LIST):
			return False

		window.SetSize(int(value["width"]), int(value["height"]))
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## Gauge
	def LoadElementGauge(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.GAUGE_KEY_LIST):
			return False

		window.MakeGauge(value["width"], value["color"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## ScrollBar
	def LoadElementScrollBar(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.SCROLLBAR_KEY_LIST):
			return False

		window.SetScrollBarSize(value["size"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## SliderBar
	def LoadElementSliderBar(self, window, value, parentWindow):

		self.LoadDefaultData(window, value, parentWindow)

		return True

	## ListBox
	def LoadElementListBox(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.LIST_BOX_KEY_LIST):
			return False

		if value.has_key("item_align"):
			window.SetTextCenterAlign(value["item_align"])

		window.SetSize(value["width"], value["height"])
		self.LoadDefaultData(window, value, parentWindow)

		return True

	## ListBox2
	def LoadElementListBox2(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.LIST_BOX_KEY_LIST):
			return False

		window.SetRowCount(value.get("row_count", 10))
		window.SetSize(value["width"], value["height"])
		self.LoadDefaultData(window, value, parentWindow)

		if value.has_key("item_align"):
			window.SetTextCenterAlign(value["item_align"])

		return True
	def LoadElementListBoxEx(self, window, value, parentWindow):

		if False == self.CheckKeyList(value["name"], value, self.LIST_BOX_KEY_LIST):
			return False

		window.SetSize(value["width"], value["height"])
		self.LoadDefaultData(window, value, parentWindow)

		if value.has_key("itemsize_x") and value.has_key("itemsize_y"):
			window.SetItemSize(int(value["itemsize_x"]), int(value["itemsize_y"]))

		if value.has_key("itemstep"):
			window.SetItemStep(int(value["itemstep"]))

		if value.has_key("viewcount"):
			window.SetViewItemCount(int(value["viewcount"]))

		return True

	if app.ENABLE_RENDER_TARGET:
		def LoadElementRenderTarget(self, window, value, parentWindow):

			if False == self.CheckKeyList(value["name"], value, self.RENDER_TARGET_KEY_LIST):
				return False

			window.SetSize(value["width"], value["height"])
			
			if True == value.has_key("style"):
				for style in value["style"]:
					window.AddFlag(style)
					
			self.LoadDefaultData(window, value, parentWindow)
			
			if value.has_key("index"):
				window.SetRenderTarget(int(value["index"]))

			return True

	def LoadElementCheckBox(self, window, value, parentWindow):

		if value.has_key("text"):
			window.SetText(value["text"])

		if value.has_key("text_color"):
			window.SetTextColor(value["text_color"])

		if value.has_key("checked") and value["checked"] == True:
			window.SetChecked(window.STATE_SELECTED)

		if value.has_key("disabled") and value["disabled"] == True:
			window.Disable()

		self.LoadDefaultData(window, value, parentWindow)

class ReadingWnd(Bar):

	def __init__(self):
		Bar.__init__(self,"TOP_MOST")

		self.__BuildText()
		self.SetSize(80, 19)
		self.Show()

	def __del__(self):
		Bar.__del__(self)

	def __BuildText(self):
		self.text = TextLine()
		self.text.SetParent(self)
		self.text.SetPosition(4, 3)
		self.text.Show()

	def SetText(self, text):
		self.text.SetText(text)

	def SetReadingPosition(self, x, y):
		xPos = x + 2
		yPos = y  - self.GetHeight() - 2
		self.SetPosition(xPos, yPos)

	def SetTextColor(self, color):
		self.text.SetPackedFontColor(color)


def MakeSlotBar(parent, x, y, width, height):
	slotBar = SlotBar()
	slotBar.SetParent(parent)
	slotBar.SetSize(width, height)
	slotBar.SetPosition(x, y)
	slotBar.Show()
	return slotBar

def MakeImageBox(parent, name, x, y):
	image = ImageBox()
	image.SetParent(parent)
	image.LoadImage(name)
	image.SetPosition(x, y)
	image.Show()
	return image

def AddTextLine(parent, x, y, text, outline = 0):
	textLine = TextLine()
	textLine.SetParent(parent)
	textLine.SetPosition(x, y)
	if outline != 0:
		textLine.SetOutline()
	textLine.SetText(text)
	textLine.Show()
	return textLine

def MakeNewTextLine(parent, horizontalAlign = True, verticalAlgin = True, x = 0, y = 0):
	textLine = TextLine()
	textLine.SetParent(parent)

	if horizontalAlign == True:
		textLine.SetWindowHorizontalAlignCenter()

	if verticalAlgin == True:
		textLine.SetWindowVerticalAlignCenter()

	textLine.SetHorizontalAlignCenter()
	textLine.SetVerticalAlignCenter()

	if x != 0 and y != 0:
		textLine.SetPosition(x, y)

	textLine.Show()
	return textLine

def MakeExpandedImageBox(parent, name, x, y, flag = ""):
	image = ExpandedImageBox()
	image.SetParent(parent)
	image.LoadImage(name)
	image.SetPosition(x, y)

	if flag != "":
		image.AddFlag(flag)

	image.Show()

	return image

def MakeTextLine(parent):
	textLine = TextLine()
	textLine.SetParent(parent)
	textLine.SetWindowHorizontalAlignCenter()
	textLine.SetWindowVerticalAlignCenter()
	textLine.SetHorizontalAlignCenter()
	textLine.SetVerticalAlignCenter()
	textLine.Show()
	return textLine

def MakeButton(parent, x, y, tooltipText, path, up, over, down):
	button = Button()
	button.SetParent(parent)
	button.SetPosition(x, y)
	button.SetUpVisual(path + up)
	button.SetOverVisual(path + over)
	button.SetDownVisual(path + down)
	button.SetToolTipText(tooltipText)
	button.Show()
	return button

def MakeText(parent, textlineText, x, y, color):
	textline = TextLine()
	if parent:
		textline.SetParent(parent)
	textline.SetPosition(x, y)
	if color:
		textline.SetFontColor(color[0], color[1], color[2])
	textline.SetText(textlineText)
	textline.Show()
	return textline

def MakeThinBoardCircle(parent, x, y, width, heigh, text, bnsId = 0):
	thin = RadioButton()
	thin.SetParent(parent)
	thin.SetSize(width, heigh)
	thin.SetPosition(x, y)
	thin.SetText(text)
	thin.SetBonusId(bnsId)
	thin.Show()
	return thin

def MakeRadioButton(parent, x, y, path, up, over, down):
	button = RadioButton()
	button.SetParent(parent)
	button.SetPosition(x, y)
	button.SetUpVisual(path + up)
	button.SetOverVisual(path + over)
	button.SetDownVisual(path + down)
	button.Show()
	return button

def MakeHorizontalBarCheckbox(parent, x, y, width, text):
	horizontalBarCheckbox = HorizontalBarCheckbox()
	horizontalBarCheckbox.Create(width)
	horizontalBarCheckbox.SetParent(parent)
	horizontalBarCheckbox.SetPosition(x, y)
	horizontalBarCheckbox.SetWidth(width)
	horizontalBarCheckbox.SetText(text)
	horizontalBarCheckbox.Show()
	return horizontalBarCheckbox

def MakeHorizontalBarCheckboxSmall(parent, x, y, width, text):
	horizontalBarCheckboxSmall = HorizontalBarCheckboxSmall()
	horizontalBarCheckboxSmall.Create(width)
	horizontalBarCheckboxSmall.SetParent(parent)
	horizontalBarCheckboxSmall.SetPosition(x, y)
	horizontalBarCheckboxSmall.SetWidth(width)
	horizontalBarCheckboxSmall.SetText(text)
	horizontalBarCheckboxSmall.Show()
	return horizontalBarCheckboxSmall

def RenderRoundBox(x, y, width, height, color):
	grp.SetColor(color)
	grp.RenderLine(x+2, y, width-3, 0)
	grp.RenderLine(x+2, y+height, width-3, 0)
	grp.RenderLine(x, y+2, 0, height-4)
	grp.RenderLine(x+width, y+1, 0, height-3)
	grp.RenderLine(x, y+2, 2, -2)
	grp.RenderLine(x, y+height-2, 2, 2)
	grp.RenderLine(x+width-2, y, 2, 2)
	grp.RenderLine(x+width-2, y+height, 2, -2)

if app.ENABLE_ANTI_MULTIPLE_FARM:
	def MakeOptionsCheckBox(parent, text, x, y):
		checkBox = RefineCheckBox()
		checkBox.SetParent(parent)
		checkBox.SetPosition(x, y)
		checkBox.SetTextInfo(text)
		checkBox.SetBaseCheckImage("d:/ymir work/ui/new_options/options_checkbox.tga")
		checkBox.Show()
		return checkBox

	def MakeTextLineWithouAlign(parent, text, x, y):
		textLine = TextLine()
		textLine.SetParent(parent)
		textLine.SetPosition(x, y)
		textLine.SetText(text)
		textLine.Show()
		return textLine

if app.ENABLE_VOTE_4_BUFF:
	def MakeTextLineNew(parent, x, y, text):
		textLine = TextLine()
		textLine.SetParent(parent)
		textLine.SetPosition(x, y)
		textLine.SetText(text)
		textLine.SetOutline(True)
		textLine.Show()
		return textLine

	def MakeButtonNew(parent, x, y, text, path, up, over, down):
		button = Button()
		button.SetParent(parent)
		button.SetPosition(x, y)
		button.SetUpVisual(path + up)
		button.SetOverVisual(path + over)
		button.SetDownVisual(path + down)
		button.SetText(text)
		button.UpdateRect()
		button.Show()
		return button

def GenerateColor(r, g, b):
	r = float(r) / 255.0
	g = float(g) / 255.0
	b = float(b) / 255.0
	return grp.GenerateColor(r, g, b, 1.0)

def EnablePaste(flag):
	ime.EnablePaste(flag)

def GetHyperlink():
	return wndMgr.GetHyperlink()

RegisterToolTipWindow("TEXT", TextLine)
