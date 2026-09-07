import ui
import app
import net
import grp 
import wndMgr
import localeInfo
import player
import mouseModule
import chat
import ui
import item

from _weakref import proxy

EMPIRE_FLAGS = ["d:/ymir work/ui/game/flag/shinsoo.tga", # 1
				"d:/ymir work/ui/game/flag/chunjo.tga", # 2
				"d:/ymir work/ui/game/flag/jinno.tga"]

JOBS = ["Warrior", # Warrior
		"Assasin", # Assasin
		"Sura", # Sura
		"Shaman" # Shaman
		# "Wolfman" # Wolfman
]

TEXT_CATEGORY = ["  ", 
				"  ", 
				"  ",
				"  ", 
				"  ",
				"  ",
				"  ",
				"  ",
				"  ",
				"  ",
				"  ",
				"  ",
]

MAX_CATEGORY_NUM = len(TEXT_CATEGORY)

class ListBox(ui.Window):
	class NewItem(ui.Window):
		def __init__(self, index, func, parent):
			ui.Window.__init__(self)
			self.Index = index
			self.select = False
			self.Reinitialize()

			self.DoChange = ui.__mem_func__(func)

			self.background = ui.ExpandedImageBox()
			self.background.SetParent(self)
			self.background.AddFlag("not_pick")
			self.background.SetPosition(0, 0)
			self.background.LoadImage("d:/ymir work/ranking/rank_category.png")
			self.background.Show()
			
			self.SetSize(self.background.GetWidth(), self.background.GetHeight())		

			self.Icon = ui.ExpandedImageBox()
			self.Icon.SetParent(self)
			self.Icon.AddFlag("not_pick")
			self.Icon.SetPosition(8, 0)
			self.Icon.LoadImage("d:/ymir work/ranking/category/%d.png" % (index))
			self.Icon.Show()
		
			self.textCat = ui.TextLine()
			self.textCat.SetParent(self.background)
			self.textCat.SetPosition(100, 7)
			text = TEXT_CATEGORY[index]
			text1 = text.find(' ')
			if text1 != -1:
				self.textCat.SetText(text[:text1 + 1])
			else:
				self.textCat.SetText(text)
			self.textCat.Show()
			
			self.text2Cat = None
			if text1 != -1:
				self.text2Cat = ui.TextLine()
				self.text2Cat.SetParent(self.background)
				self.text2Cat.SetPosition(100, 18)
				self.text2Cat.SetText(text[text1 + 1:])
				self.text2Cat.Show()

		def OnRender(self):		
			xList, yList = self.parent.GetGlobalPosition()
			widthList, heightList = self.parent.GetWidth(), self.parent.GetHeight()
			
			if self.background:
				self.background.SetClipRect(xList, yList, xList + widthList, yList + heightList)

			if self.Icon:
				self.Icon.SetClipRect(xList, yList, xList + widthList, yList + heightList)

			textList = [self.textCat, self.text2Cat]		
			for text in textList:
				if text:
					xText, yText = text.GetGlobalPosition()

					if yText < yList or yText + text.GetTextSize()[1] > yList + heightList:
						text.Hide()
					else:
						text.Show()

		def SelectImage(self):
			self.background.LoadImage("d:/ymir work/ranking/rank_category_selected.png")
			# if not self.select:
			self.DoChange(self.Index)
			self.select = True

		def OverInImage(self):
			self.background.LoadImage("d:/ymir work/ranking/rank_category_selected.png")

		def OverOutImage(self):
			if self.select == False:
				self.background.LoadImage("d:/ymir work/ranking/rank_category.png")
	
		def SetSelect(self):
			self.select = True
			self.background.LoadImage("d:/ymir work/ranking/rank_category_selected.png")
	
		def OverOutForce(self):
			self.select = False
			self.background.LoadImage("d:/ymir work/ranking/rank_category.png")
		
		def __del__(self):
			ui.Window.__del__(self)
			self.Reinitialize()
			
		def Reinitialize(self):
			self.selected = False
			self.xBase = 0
			self.yBase = 0

			self.overInEvent = None
			self.overOutEvent = None
			self.clickEvent = None
			self.background = None		
		
		def SetParent(self, parent):
			ui.Window.SetParent(self, parent)
			self.parent = proxy(parent)

		def SetBasePosition(self, x, y):
			self.xBase = x
			self.yBase = y
			
		def GetBasePosition(self):
			return (self.xBase, self.yBase)
			
		def SetOverInEvent(self, event):
			self.overInEvent = event
			
		def SetOverOutEvent(self, event):
			self.overOutEvent = event
			
		def SetClickEvent(self, event):
			self.clickEvent = event
			
		def OnMouseOverIn(self):
			if self.overInEvent:
				self.overInEvent()
			
		def OnMouseOverOut(self):
			if self.overOutEvent:
				self.overOutEvent()
				
		def OnMouseLeftButtonDown(self):	
			if self.clickEvent:
				self.clickEvent()

	def __init__(self):
		ui.Window.__init__(self)
		self.Reinitialize()

	def __del__(self):
		ui.Window.__del__(self)
		self.Reinitialize()
		
	def Destroy(self):
		self.Reinitialize()
		
	def Reinitialize(self):
		self.itemList = []
		self.scrollBar = None
		self.tooltipItem = None
		
		self.selectEvent = None

	def SetParent(self, parent):
		ui.Window.SetParent(self, parent)
		
		self.SetPosition(5, 5)
		self.SetSize(parent.GetWidth() - 10, parent.GetHeight() - 10)
		
	def SetScrollBar(self, scrollBar):
		scrollBar.SetScrollEvent(ui.__mem_func__(self.__OnScroll))
		scrollBar.SetScrollStep(0.2)
		self.scrollBar = scrollBar

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem
		
	def SetSelectEvent(self, event):
		self.selectEvent = event
		
	def __OnScroll(self):
		self.AdjustItemPositions(True)
			
	def GetTotalItemHeight(self):
		totalHeight = 0
		
		if self.itemList:
			for itemH in self.itemList:
				totalHeight += itemH.GetHeight() + 2
			
		return totalHeight
			
	def OnMouseWheel(self, nLen):
		if self.scrollBar:
			self.scrollBar.OnMouseWheel(nLen)
			
	def GetItemCount(self):
		return len(self.itemList)
			
	def OverOutAll(self, y):
		for x in xrange(len(self.itemList)):
			self.itemList[x].OverOutForce()
			
		self.itemList[y].SetSelect()
			
	def AppendItem(self, ItemVnum, func):
		item = self.NewItem(ItemVnum, func, self)
		item.SetParent(self)
		
		if len(self.itemList) == 0:
			item.SetBasePosition(0, 0)
		else:
			x, y = self.itemList[-1].GetLocalPosition()
			y += 2
			item.SetBasePosition(0, y + self.itemList[-1].GetHeight())

		item.SetOverInEvent(ui.__mem_func__(item.OverInImage))
		item.SetOverOutEvent(ui.__mem_func__(item.OverOutImage))
		item.SetClickEvent(ui.__mem_func__(item.SelectImage))
			
		item.Show()
		self.itemList.append(item)
		
		self.AdjustScrollBar()
		self.AdjustItemPositions()

	def AdjustScrollBar(self):
		totalHeight = float(self.GetTotalItemHeight())
		if totalHeight:
			scrollBarHeight = min(float(self.GetHeight() - 10) / totalHeight, 1.0)
		else:
			scrollBarHeight = 1.0
			
		self.scrollBar.SetMiddleBarSize(scrollBarHeight)
		
	def ResetScrollbar(self):
		self.scrollBar.SetPos(0)
				
	def AdjustItemPositions(self, scrolling = False, startIndex = -1):		
		scrollPos = self.scrollBar.GetPos()
		totalHeight = self.GetTotalItemHeight() - self.GetHeight()

		idx = 0
		if startIndex >= 0:
			idx = startIndex

		for item in self.itemList[idx:]:
			xB, yB = item.GetBasePosition()
			
			if startIndex >= 0:
				yB -= ITEM_HEIGHT + 2
			
			if scrolling:
				setPos = yB - int(scrollPos * totalHeight)
				item.SetPosition(xB, setPos)
			else:
				item.SetPosition(xB, yB)
				
			item.SetBasePosition(xB, yB)

	def SelectItem(self):
		if self.selectEvent:
			self.selectEvent()

	def Clear(self):
		if len(self.itemList) == 0:
			return
	
		for item in self.itemList:
			item.Reinitialize()
			item.Hide()
			del item

		self.itemList = []

class RankInfo(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.dicInfoRank = {}
		for x in xrange(MAX_CATEGORY_NUM):
			self.dicInfoRank[x] = {}
			for y in xrange(10):
				self.dicInfoRank[x][y] = {"Name": "", "Value": 1, "Empire": 1}
		self.Size = {}
		self.bLoaded = False
		self.bCurPage = 0
		self.MyPos = 0
		self.LoadWindow()
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.bTab = {}
		self.bInfoPos = {}
		self.bInfoName = {}
		self.bInfoValue = {}
		self.bInfoEmpire = {}
		self.dicInfoRank = {}
		
		self.ListBoxItem.Clear()

	def LoadWindow(self):
		if self.bLoaded == True:
			return
			
		self.bLoaded = True
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/rank_info.py")
		except:
			import exception
			exception.Abort("rank_info.LoadWindow.LoadObject")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.board = self.GetChild("board")
			self.wndTextCount = self.GetChild("wndTextCount")
		except:
			import exception
			exception.Abort("rank_info.__LoadWindow.BindObject")

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.ScrollBar = ui.ScrollBar()
		self.ScrollBar.SetParent(self.board)
		self.ScrollBar.SetScrollBarSize(343)
		self.ScrollBar.SetPosition(410, 40)
		self.ScrollBar.Show()		

		self.ListBoxItem = ListBox()
		self.ListBoxItem.SetParent(self.board)
		self.ListBoxItem.SetScrollBar(self.ScrollBar)
		self.ListBoxItem.SetSize(150, 330)
		self.ListBoxItem.SetPosition(433, 50)
		self.ListBoxItem.Show()		
		
		self.CreateButtonCategory()
		
		# self.wndAnimMask = ui.ThinBoardCircle()
		# self.wndAnimMask.SetParent(self)
		# self.wndAnimMask.SetPosition(28, 87)
		# self.wndAnimMask.SetSize(350, 259)
		# self.wndAnimMask.Hide()
		
		# self.wndAnim = ui.AniImageBox()
		# self.wndAnim.SetDelay(8)
		# self.wndAnim.SetParent(self)
		# self.wndAnim.SetOnEndFrame(ui.__mem_func__(self.SearchRank))
		# self.wndAnim.Hide()

		# for i in xrange(8):
		# 	self.wndAnim.AppendImage("d:/ymir work/ui/loading_ani/0%d.tga" % (i))

		self.bInfoMyPos = ui.TextLine()
		self.bInfoMyPos.SetParent(self)
		self.bInfoMyPos.SetPosition(365, 354)
		
		self.bInfoMyName = ui.TextLine()
		self.bInfoMyName.SetParent(self)
		self.bInfoMyName.SetPosition(-5, 354)
		self.bInfoMyName.SetWindowHorizontalAlignCenter()	
		self.bInfoMyName.SetHorizontalAlignCenter()

		self.bInfoMyEmpire = ui.ImageBox()
		self.bInfoMyEmpire.SetParent(self)
		self.bInfoMyEmpire.AddFlag("not_pick")
		self.bInfoMyEmpire.SetPosition(175, 354)

		self.bInfoMyValue = ui.TextLine()
		self.bInfoMyValue.SetParent(self)
		self.bInfoMyValue.SetPosition(-227, 354)
		self.bInfoMyValue.SetWindowHorizontalAlignCenter()	
		self.bInfoMyValue.SetHorizontalAlignCenter()

	def Open(self):
		self.LoadWindow()
		self.SetCenterPosition()
		self.DoChange(0) # Select Automatic
		ui.ScriptWindow.Show(self)

	def OnRunMouseWheel(self, nLen):
		if nLen > 0:
			self.ScrollBar.OnUp()
		else:
			self.ScrollBar.OnDown()

	def CreateButtonCategory(self):
		for x in xrange(MAX_CATEGORY_NUM):
			self.ListBoxItem.AppendItem(x, self.DoChange)

	def DoChange(self, arg):
		self.ListBoxItem.OverOutAll(arg)
		self.bCurPage = arg
	
		self.bTab = {}
		self.bInfoPos = {}
		self.bInfoName = {}
		self.bInfoValue = {}
		self.bInfoEmpire = {}
	
		# self.wndAnim.SetPosition(190, 190)
		# self.wndAnim.ResetFrame()
		# self.wndAnimMask.Show()
		# self.wndAnim.Show()

		self.bInfoMyPos.Hide()
		self.bInfoMyName.Hide()
		self.bInfoMyEmpire.Hide()
		self.bInfoMyValue.Hide()

	def SearchRank(self):
		# self.wndAnimMask.Hide()
		# self.wndAnim.Hide()
		net.SendChatPacket("/req_info_rank %d 0" % (self.bCurPage))
		self.ListBoxItem.OverOutAll(self.bCurPage)
		self.bCurPage = self.bCurPage

		if self.bCurPage == 0:
			self.wndTextCount.SetText("  ")
		elif self.bCurPage == 1:
			self.wndTextCount.SetText("  ")
		else:
			self.wndTextCount.SetText("  ")

	def AppendInfo(self, mode, MyPos, pos, name, value, empire):
		if MyPos != -1:
			self.bInfoMyPos.SetText(str(pos))
			self.bInfoMyPos.Show()

			self.bInfoMyName.SetText(name)
			self.bInfoMyName.Show()

			self.bInfoMyEmpire.LoadImage(EMPIRE_FLAGS[empire - 1])
			self.bInfoMyEmpire.Show()

			price = localeInfo.DottedNumber(int(value))
			self.bInfoMyValue.SetText(str(price))
			self.bInfoMyValue.Show()
		else:
			self.dicInfoRank[mode][pos] = {"Name": name, "Value": value, "Empire": empire}
			self.Refresh(pos)

	def Refresh(self, size):
		self.bTab = {}
		self.bInfoPos = {}
		self.bInfoName = {}
		self.bInfoValue = {}
		self.bInfoEmpire = {}

		y_pos = [94, 120, 145, 171, 197, 223, 248, 275, 300, 328] # Position
		colors = ["|cffffff00", "|cff888888", "|cFFA52A2A" ]

		for x in xrange(1, size):
			if x > len(y_pos):
				break
		
			self.bInfoPos[x] = ui.TextLine()
			self.bInfoPos[x].SetParent(self)
			self.bInfoPos[x].SetPosition(365, y_pos[x-1])
			self.bInfoPos[x].SetText(str(x))
			self.bInfoPos[x].Show()
			
			self.bInfoName[x] = ui.TextLine()
			self.bInfoName[x].SetParent(self)
			self.bInfoName[x].SetPosition(-5, y_pos[x-1])
			self.bInfoName[x].SetWindowHorizontalAlignCenter()	
			self.bInfoName[x].SetHorizontalAlignCenter()
			
			if x <= 3:
				self.bInfoName[x].SetText(colors[x-1] + self.dicInfoRank[self.bCurPage][x]["Name"])
			else:
				self.bInfoName[x].SetText(self.dicInfoRank[self.bCurPage][x]["Name"])
			
			self.bInfoName[x].Show()
			
			self.bInfoEmpire[x] = ui.ImageBox()
			self.bInfoEmpire[x].SetParent(self)
			self.bInfoEmpire[x].AddFlag("not_pick")
			self.bInfoEmpire[x].SetPosition(175, y_pos[x-1])
			self.bInfoEmpire[x].LoadImage(EMPIRE_FLAGS[self.dicInfoRank[self.bCurPage][x]["Empire"] - 1])
			self.bInfoEmpire[x].Show()

			self.bInfoValue[x] = ui.TextLine()
			self.bInfoValue[x].SetParent(self)
			self.bInfoValue[x].SetPosition(-227, y_pos[x-1])
			self.bInfoValue[x].SetWindowHorizontalAlignCenter()	
			self.bInfoValue[x].SetHorizontalAlignCenter()
			price = localeInfo.DottedNumber(int(self.dicInfoRank[self.bCurPage][x]["Value"]))
			if x <= 3:
				self.bInfoValue[x].SetText(colors[x-1] + str(price))
			else:
				self.bInfoValue[x].SetText(str(price))
			self.bInfoValue[x].Show()

	def Close(self):
		self.Hide()
