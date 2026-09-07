import ui
import grp
import app

import constInfo
import wndMgr

class TextBar(ui.Window):
	def __init__(self, width, height):
		ui.Window.__init__(self)
		self.handle = grp.CreateTextBar(width, height)

	def __del__(self):
		ui.Window.__del__(self)
		grp.DestroyTextBar(self.handle)

	def ClearBar(self):
		grp.ClearTextBar(self.handle)

	def SetClipRect(self, x1, y1, x2, y2):
		grp.SetTextBarClipRect(self.handle, x1, y1, x2, y2)

	def TextOut(self, x, y, text):
		grp.TextBarTextOut(self.handle, x, y, text)

	def OnRender(self):
		x, y = self.GetGlobalPosition()
		grp.RenderTextBar(self.handle, x, y)

	def SetTextColor(self, r, g, b):
		grp.TextBarSetTextColor(self.handle, r, g, b)

	def GetTextExtent(self, text):
		return grp.TextBarGetTextExtent(self.handle, text)

class TipBoard(ui.Bar):

	SCROLL_WAIT_TIME = 3.0
	TIP_DURATION = 5.0
	STEP_HEIGHT = 17

	def __init__(self):
		ui.Bar.__init__(self)

		self.AddFlag("not_pick")
		self.tipList = []
		self.curPos = 0
		self.dstPos = 0
		self.nextScrollTime = 0

		self.width = 370

		self.SetPosition(0, 70)
		self.SetSize(370, 20)
		self.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.5))
		self.SetWindowHorizontalAlignCenter()

		self.__CreateTextBar()

	def __del__(self):
		ui.Bar.__del__(self)

	def __CreateTextBar(self):

		x, y = self.GetGlobalPosition()

		self.textBar = TextBar(370, 300)
		self.textBar.SetParent(self)
		self.textBar.SetPosition(3, 5)
		self.textBar.SetClipRect(0, y, wndMgr.GetScreenWidth(), y+18)
		self.textBar.Show()

	def __CleanOldTip(self):
		leaveList = []
		for tip in self.tipList:
			madeTime = tip[0]
			if app.GetTime() - madeTime > self.TIP_DURATION:
				pass
			else:
				leaveList.append(tip)

		self.tipList = leaveList

		if not leaveList:
			self.textBar.ClearBar()
			self.Hide()
			return

		self.__RefreshBoard()

	def __RefreshBoard(self):

		self.textBar.ClearBar()

		index = 0
		for tip in self.tipList:
			text = tip[1]
			self.textBar.TextOut(0, index*self.STEP_HEIGHT, text)
			index += 1

	def SetTip(self, text):

		if not app.IsVisibleNotice():
			return

		curTime = app.GetTime()
		self.tipList.append((curTime, text))
		self.__RefreshBoard()

		self.nextScrollTime = app.GetTime() + 1.0

		if not self.IsShow():
			self.curPos = -self.STEP_HEIGHT
			self.dstPos = -self.STEP_HEIGHT
			self.textBar.SetPosition(3, 5 - self.curPos)
			self.Show()

	def OnUpdate(self):

		if not self.tipList:
			self.Hide()
			return

		if app.GetTime() > self.nextScrollTime:
			self.nextScrollTime = app.GetTime() + self.SCROLL_WAIT_TIME

			self.dstPos = self.curPos + self.STEP_HEIGHT

		if self.dstPos > self.curPos:
			self.curPos += 1
			self.textBar.SetPosition(3, 5 - self.curPos)

			if self.curPos > len(self.tipList)*self.STEP_HEIGHT:
				self.curPos = -self.STEP_HEIGHT
				self.dstPos = -self.STEP_HEIGHT

				self.__CleanOldTip()


class BigTextBar(TextBar):
	def __init__(self, width, height, fontSize):
		ui.Window.__init__(self)
		self.handle = grp.CreateBigTextBar(width, height, fontSize)


class BigBoard(ui.Bar):

	SCROLL_WAIT_TIME = 5.0
	TIP_DURATION = 10.0
	FONT_WIDTH	= 18
	FONT_HEIGHT	= 18
	LINE_WIDTH  = 500
	LINE_HEIGHT	= FONT_HEIGHT + 5
	STEP_HEIGHT = LINE_HEIGHT * 2
	LINE_CHANGE_LIMIT_WIDTH = 350

	FRAME_IMAGE_FILE_NAME_LIST = [
		"season1/interface/oxevent/frame_0.sub",
		"season1/interface/oxevent/frame_1.sub",
		"season1/interface/oxevent/frame_2.sub",
	]

	FRAME_IMAGE_STEP = 256

	FRAME_BASE_X = -20
	FRAME_BASE_Y = -12

	def __init__(self):
		ui.Bar.__init__(self)

		self.AddFlag("not_pick")
		self.tipList = []
		self.curPos = 0
		self.dstPos = 0
		self.nextScrollTime = 0

		self.SetPosition(0, 150)
		self.SetSize(512, 55)
		self.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.5))
		self.SetWindowHorizontalAlignCenter()

		self.__CreateTextBar()
		self.__LoadFrameImages()


	def __LoadFrameImages(self):
		x = self.FRAME_BASE_X
		y = self.FRAME_BASE_Y
		self.imgList = []
		for imgFileName in self.FRAME_IMAGE_FILE_NAME_LIST:
			self.imgList.append(self.__LoadImage(x, y, imgFileName))
			x += self.FRAME_IMAGE_STEP

	def __LoadImage(self, x, y, fileName):
		img = ui.ImageBox()
		img.SetParent(self)
		img.AddFlag("not_pick")
		img.LoadImage(fileName)
		img.SetPosition(x, y)
		img.Show()
		return img

	def __del__(self):
		ui.Bar.__del__(self)

	def __CreateTextBar(self):

		x, y = self.GetGlobalPosition()

		self.textBar = BigTextBar(self.LINE_WIDTH, 300, self.FONT_HEIGHT)
		self.textBar.SetParent(self)
		self.textBar.SetPosition(6, 8)
		self.textBar.SetTextColor(242, 231, 193)
		self.textBar.SetClipRect(0, y+8, wndMgr.GetScreenWidth(), y+8+self.STEP_HEIGHT)
		self.textBar.Show()

	def __CleanOldTip(self):
		curTime = app.GetTime()
		leaveList = []
		for madeTime, text in self.tipList:
			if curTime + self.TIP_DURATION <= madeTime:
				leaveList.append(text)

		self.tipList = leaveList

		if not leaveList:
			self.textBar.ClearBar()
			self.Hide()
			return

		self.__RefreshBoard()

	def __RefreshBoard(self):

		self.textBar.ClearBar()

		if len(self.tipList) == 1:
			checkTime, text = self.tipList[0]
			(text_width, text_height) = self.textBar.GetTextExtent(text)
			self.textBar.TextOut((500-text_width)/2, (self.STEP_HEIGHT-8-text_height)/2, text)

		else:
			index = 0
			for checkTime, text in self.tipList:
				(text_width, text_height) = self.textBar.GetTextExtent(text)
				self.textBar.TextOut((500-text_width)/2, index*self.LINE_HEIGHT, text)
				index += 1

	def SetTip(self, text):

		if not app.IsVisibleNotice():
			return

		curTime = app.GetTime()
		self.__AppendText(curTime, text)
		self.__RefreshBoard()

		self.nextScrollTime = curTime + 1.0

		if not self.IsShow():
			self.curPos = -self.STEP_HEIGHT
			self.dstPos = -self.STEP_HEIGHT
			self.textBar.SetPosition(3, 8 - self.curPos)
			self.Show()

	def __AppendText(self, curTime, text):
		import dbg
		prevPos = 0
		while 1:
			curPos = text.find(" ", prevPos)
			if curPos < 0:
				break

			(text_width, text_height) = self.textBar.GetTextExtent(text[:curPos])
			if text_width > self.LINE_CHANGE_LIMIT_WIDTH:
				self.tipList.append((curTime, text[:prevPos]))
				self.tipList.append((curTime, text[prevPos:]))
				return

			prevPos = curPos + 1

		self.tipList.append((curTime, text))

	def OnUpdate(self):

		if not self.tipList:
			self.Hide()
			return

		if app.GetTime() > self.nextScrollTime:
			self.nextScrollTime = app.GetTime() + self.SCROLL_WAIT_TIME

			self.dstPos = self.curPos + self.STEP_HEIGHT

		if self.dstPos > self.curPos:
			self.curPos += 1
			self.textBar.SetPosition(3, 8 - self.curPos)

			if self.curPos > len(self.tipList)*self.LINE_HEIGHT:
				self.curPos = -self.STEP_HEIGHT
				self.dstPos = -self.STEP_HEIGHT

				self.__CleanOldTip()


if constInfo.ENABLE_NEW_TIPBOAD:
	class NewTipBoard(ui.ThinBoard):
		TIP_DURATION = 3
		TIP_PROGRESS_LEN = 5

		def __init__(self):
			ui.ThinBoard.__init__(self)
			
			self.end = False
			self.isActiveSlide = False
			self.isActiveSlideOut = False
			self.endTime = 0
			self.wndWidth = 0

			self.textLineNew = ui.TextLine()
			self.textLineNew.SetParent(self)
			self.textLineNew.SetWindowHorizontalAlignCenter()
			self.textLineNew.SetWindowVerticalAlignCenter()
			self.textLineNew.SetHorizontalAlignCenter()
			self.textLineNew.SetVerticalAlignCenter()
			self.textLineNew.Show()

			self.listNotification = {}

		def OnMouseLeftButtonDown(self):
			self.isActiveSlide = False
			self.isActiveSlideOut = True

		def SetTip(self, text):
			length = len(self.listNotification)
			self.listNotification[length] = text
			self.Close()

		def __del__(self):
			ui.ThinBoard.__del__(self)

		def SlideIn(self, text):
			self.SetTop()
			self.Show()
			
			self.textLineNew.SetText(text)
			self.wndWidth = self.textLineNew.GetTextSize()[0] + 35
			self.textLineNew.SetText("")
			self.widthRN = 5
			self.SetSize(self.widthRN, 22)
			
			self.text = text
			self.isActiveSlide = True
			self.endTime = app.GetGlobalTimeStamp() + 4
			
			self.end = False
		
		def SetTextArrange(self, width):
			text = self.text
	        
			if width > self.wndWidth:
				width = self.wndWidth
			elif width < 0:
				width = 0

			self.widthRN = width
			self.SetSize(width, 22)
			self.SetPosition((wndMgr.GetScreenWidth() - self.GetWidth()) / 2 + 0, 60)
	        
			strNew = ""
			
			self.textLineNew.SetText("")
			for idx, string in enumerate(text):
				if self.textLineNew.GetTextSize()[0] + 2 > self.GetWidth():
					break
	        
				strNew += string
	            
				self.textLineNew.SetText(strNew)

		def Close(self):
			if self.isActiveSlide:
				return

			if len(self.listNotification) == 0:
				self.Hide()
			else:
				for itemf in self.listNotification:
					self.SlideIn(self.listNotification[itemf])
					del self.listNotification[itemf]
					break

		def Destroy(self):
			self.Hide()
			self.listNotification = {}

		def OnUpdate(self):
			if self.isActiveSlide and self.end == False:
				self.SetTextArrange(self.widthRN + self.TIP_PROGRESS_LEN)
				
				if self.GetWidth() >= self.wndWidth:
					self.endTime = app.GetGlobalTimeStamp() + self.TIP_DURATION
					self.end = True
					
			if self.end and self.endTime <= app.GetGlobalTimeStamp() and self.isActiveSlideOut == False and self.isActiveSlide == True:
				self.isActiveSlide = False
				self.isActiveSlideOut = True
					
			if self.isActiveSlideOut and self.isActiveSlideOut == True:
				self.SetTextArrange(self.widthRN - self.TIP_PROGRESS_LEN)
				
				if self.widthRN <= 20:
					self.textLineNew.SetText("")
				
				if self.widthRN <= 5:
					self.isActiveSlideOut = False
					self.Close()

if __name__ == "__main__":
	import app
	import wndMgr
	import systemSetting
	import mouseModule
	import grp
	import ui

	#wndMgr.SetOutlineFlag(True)

	app.SetMouseHandler(mouseModule.mouseController)
	app.SetHairColorEnable(True)
	wndMgr.SetMouseHandler(mouseModule.mouseController)
	wndMgr.SetScreenSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	app.Create("METIN2 CLOSED BETA", systemSetting.GetWidth(), systemSetting.GetHeight(), 1)
	mouseModule.mouseController.Create()

	wnd = BigBoard()
	wnd.Show()
	wnd.SetTip("안녕하세요")
	wnd.SetTip("저는 빗자루 입니다")

	app.Loop()

