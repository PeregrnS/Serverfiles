#author: dracaryS

#static
import ui, math, constInfo

#dynamic
import app, item, net, wndMgr


IMG_DIR = "spin_wheel_boss/"

TOTAL_NEED_KILL_COUNT = 50

class InfoWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.__LoadWindow()
	def Destroy(self):
		self.__children = {}
	def __LoadWindow(self):
		self.Destroy()
		
		self.AddFlag("movable")
		self.AddFlag("attach")
		self.AddFlag("float")
		self.SetCloseEvent(self.Close)
		self.SetTitleName("[ Boss Spin Wheel Rewards ]")

		bg = CreateWindow(ui.ImageBox(), self, (8, 30), IMG_DIR + "info_bg.png")
		self.__children["bg"] = bg

		backBtn = CreateWindow(ui.Button(), bg, (37, 5))
		backBtn.SetUpVisual(IMG_DIR + "left_01.png")
		backBtn.SetOverVisual(IMG_DIR + "left_02.png")
		backBtn.SetDownVisual(IMG_DIR + "left_03.png")
		backBtn.SAFE_SetEvent(self.__ClickDirection, "back")
		self.__children["backBtn"] = backBtn
	
		nextBtn = CreateWindow(ui.Button(), bg, (176, 5))
		nextBtn.SetUpVisual(IMG_DIR + "right_01.png")
		nextBtn.SetOverVisual(IMG_DIR + "right_02.png")
		nextBtn.SetDownVisual(IMG_DIR + "right_03.png")
		nextBtn.SAFE_SetEvent(self.__ClickDirection, "next")
		self.__children["nextBtn"] = nextBtn

		titleImg = CreateWindow(ui.ImageBox(), bg, (63, 5), IMG_DIR + "level_range_background.png")
		self.__children["titleImg"] = titleImg

		self.__children["titleText"] = CreateWindow(ui.TextLine(), titleImg, (titleImg.GetWidth() / 2, 2), "-", "horizontal:center")

		self.SetSize(8 + bg.GetWidth() + 8, 30 + bg.GetHeight() + 8)
		self.SetCenterPosition()
		
		self.__children["data"] = {
			0 : {
				"name" : "Level Range 1 - 175",
				"items": [[0, 95778, 5], [0, 95778, 10], [0, 95778, 15], [0, 95778, 20], [0, 95778, 25],
				[0, 71647, 5], [0, 71647, 10], [0, 71647, 15], [0, 71647, 20], [0, 71647, 25],
				[1, 19070, 5], [1, 19070, 10], [1, 19070, 15], [1, 19070, 20], [1, 19070, 25],
				[1, 50515, 10], [1, 50515, 15], [1, 50515, 20], [1, 50515, 25], [1, 50515, 30],
				[2, 77940, 2], [2, 77940, 3], [2, 77940, 5], [2, 71524, 2], [2, 77404, 3]],
				"jackpot" : [3, 80016, 0],
			},
		}

		self.SetPage(0)

	def __ClickDirection(self, direction):
		pageIdx = self.__children["pageIdx"]
		if direction == "back":
			pageIdx -= 1
			if not self.__children["data"].has_key(pageIdx):
				return
			self.SetPage(pageIdx)
		else:
			pageIdx += 1
			if not self.__children["data"].has_key(pageIdx):
				return
			self.SetPage(pageIdx)

	def SetPage(self, pageIdx):
		self.__children["pageIdx"] = pageIdx
		pageData = self.__children["data"][pageIdx]
		self.__children["titleText"].SetText(pageData["name"])
		xCount = 0
		_x = 3
		_y = 31
		bg = self.__children["bg"]
		itemList = []
		for j in xrange(25):
			try:
				itemList.append(CreateWindow(SpinItem(*pageData["items"][j]), bg, (_x, _y)))
			except:
				itemList.append(CreateWindow(SpinItem(0, -1, 0), bg, (_x, _y)))
			xCount+=1
			if xCount == 5:
				xCount = 0
				_x = 3
				_y += 44
			else:
				_x += 46
		itemList.append(CreateWindow(SpinItem(*pageData["jackpot"]), bg, (95, 272)))
		self.__children["itemList"] = itemList

	def Open(self):
		self.Show()
		self.SetTop()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def Close(self):
		self.Hide()

class SpinItem(ui.ExpandedImageBox):
	def __init__(self, rarityType, itemIdx, itemCount):
		ui.ExpandedImageBox.__init__(self)
		self.__Load(rarityType, itemIdx, itemCount)
	def __Load(self, rarityType, itemIdx, itemCount):
		rarityImages = ["rarity_blue.png", "rarity_pink.png", "rarity_red.png", "jackpot_slot.png"]
		try:
			self.LoadImage(IMG_DIR + rarityImages[rarityType])
		except:
			self.LoadImage(IMG_DIR + rarityImages[0])

		if itemIdx < 0:
			return

		item.SelectItem(itemIdx)

		self.itemData = [itemIdx, itemCount]

		self.itemImg = CreateWindow(ui.ExpandedImageBox(), self, (6, 5), item.GetIconImageFileName())
		self.itemImg.SAFE_SetStringEvent("mouse_over_in", self.__OverInItem, itemIdx)
		self.itemImg.SAFE_SetStringEvent("mouse_over_out",self.__OverOutItem)
		if itemCount > 1:
			self.itemCount = CreateWindow(ui.NumberLine(), self, (5+25, 5 + 20), str(itemCount))

	def SetAlpha(self, alpha):
		self.itemImg.SetAlpha(alpha)
		ui.ExpandedImageBox.SetAlpha(self, alpha)
	
	def __OverOutItem(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipItem:
				interface.tooltipItem.HideToolTip()

	def __OverInItem(self, itemIdx):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipItem:
				interface.tooltipItem.SetItemToolTip(itemIdx)

class WheelWindow(ui.Board):
	def __init__(self):
		ui.Board.__init__(self)
		self.__LoadWindow()
	def Destroy(self):
		self.__children = {}
	def __LoadWindow(self):
		self.Destroy()
		self.AddFlag("attach")
		#self.AddFlag("float")

		bg = CreateWindow(ui.ThinBoardCircle(), self, (8, 8), "", "", ((46 * 4) + 4, 42 + 4))
		self.__children["bg"] = bg

		self.SetSize(8 + bg.GetWidth() + 8, 8 + bg.GetHeight() + 8)
		self.SetCenterPosition()

		renderWnd = CreateWindow(ui.Window(), bg, (2, 2), "", "", ((46 * 4), 42))
		renderWnd.SetInsideRender(True)
		self.__children["renderWnd"] = renderWnd

		self.__children["needleImg"] = CreateWindow(ui.ImageBox(), bg, (bg.GetWidth() / 2 - 6, -5), IMG_DIR + "needle.png")

	def SetSpin(self, selectedItemIdx, selectedItemCount, cmdData):
		spinItemList = []
		self.__children["selectedItemData"] = [selectedItemIdx, selectedItemCount]
		if cmdData != "-":
			cmdSplit = cmdData.split("?")
			xPos = 0
			renderWnd = self.__children["renderWnd"]
			for cmd in cmdSplit:
				itemList = cmd.split("|")
				if len(itemList) == 3:
					spinItemList.append(CreateWindow(SpinItem(int(itemList[0]), int(itemList[1]), int(itemList[2])), renderWnd, (xPos, 0)))
					xPos += 44
		self.__children["spinItemList"] = spinItemList
		self.__children["spinStatus"] = True
		self.__children["spinTime"] = app.GetTime() + float(app.GetRandom(7, 12))

	def OnUpdate(self):
		currentTime = app.GetTime()
		
		spinStatus = self.__children["spinStatus"] if self.__children.has_key("spinStatus") else False
		if not spinStatus:
			leftTimeForClose = self.__children["leftTimeForClose"] if self.__children.has_key("leftTimeForClose") else False
			if leftTimeForClose < currentTime:
				self.Hide()
			return
		spinLeftTime = self.__children["spinTime"] - currentTime
		spinCurrentX = (spinLeftTime*2) * 2 if spinLeftTime > 0 else 1
		spinItemList = self.__children["spinItemList"] if self.__children.has_key("spinItemList") else []
		
		for child in spinItemList:
			idx = spinItemList.index(child)
			xPos = child.GetLocalPosition()[0] - math.ceil(spinCurrentX)
			child.SetPosition(xPos, 0)
			if spinLeftTime < 0:
				selectedItemData = self.__children["selectedItemData"]
				if child.itemData == selectedItemData:
					if xPos >= 50 and xPos <= 75:
						self.__children["spinStatus"] = False
						self.__children["leftTimeForClose"] = currentTime + 2.0
						net.SendChatPacket("/spin_wheel_boss ani_done")
						for childEx in spinItemList:
							if childEx != child:
								childEx.SetAlpha(0.6)
						return
		for child in spinItemList:
			if child.GetLocalPosition()[0] < -50:
				xPosHighest = 0
				for childEx in spinItemList:
					if xPosHighest < childEx.GetLocalPosition()[0] + 44:
						xPosHighest = childEx.GetLocalPosition()[0] + 44
				child.SetPosition(xPosHighest, 0)

	def Open(self):
		self.Show()
		self.SetTop()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		return True

class Window(ui.Window):
	__children = {}
	def __init__(self):
		ui.Window.__init__(self)
		self.__LoadWindow()
	def Destroy(self):
		wheelWindow = self.__children["wheelWindow"] if self.__children.has_key("wheelWindow") else None
		if wheelWindow:
			wheelWindow.Close()
			wheelWindow.Destroy()
			self.__children["wheelWindow"] = None
		infoWindow = self.__children["infoWindow"] if self.__children.has_key("infoWindow") else None
		if infoWindow:
			infoWindow.Close()
			infoWindow.Destroy()
			self.__children["infoWindow"] = None
		self.__children = {}
	def __LoadWindow(self):
		self.Destroy()
		self.AddFlag("attach")
		self.AddFlag("movable")

		bg = CreateWindow(ui.ExpandedImageBox(), self, (0, 0), IMG_DIR + "bg.png")
		self.__children["bg"] = bg
		self.SetSize(bg.GetWidth(), bg.GetHeight())
		self.SetCenterPosition()
		bg.AddFlag("not_pick")


		gaugeImg = CreateWindow(ui.ExpandedImageBox(), bg, (37, 14), IMG_DIR + "unavailable.png")
		gaugeImg.imageFolder = IMG_DIR + "unavailable.png"
		self.__children["gaugeImg"] = gaugeImg

		gaugeText = CreateWindow(ui.TextLine(), bg, (37 + (self.__children["gaugeImg"].GetWidth() / 2), 12), "0 / " + str(TOTAL_NEED_KILL_COUNT) , "horizontal:center")
		gaugeText.SetOutline()
		self.__children["gaugeText"] = gaugeText

		spinBtn = CreateWindow(ui.Button(), bg, (3, 3))
		spinBtn.SetUpVisual(IMG_DIR + "spin_1.png")
		spinBtn.SetOverVisual(IMG_DIR + "spin_2.png")
		spinBtn.SetDownVisual(IMG_DIR + "spin_2.png")
		spinBtn.SetDisableVisual(IMG_DIR + "spin_0.png")
		spinBtn.SetEvent(ui.__mem_func__(self.__ClickSpin), "mouse_click")
		self.__children["spinBtn"] = spinBtn

		questionMark = CreateWindow(ui.ExpandedImageBox(), bg, (144, 13), IMG_DIR + "question_mark.png")
		questionMark.SetEvent(ui.__mem_func__(self.__ClickMark), "mouse_click")
		self.__children["questionMark"] = questionMark
		
		# self.SetPosition(125, wndMgr.GetScreenHeight() - 100)

		self.__children["gaugeImg"].AddFlag("not_pick")
		self.__children["gaugeText"].AddFlag("not_pick")
		(x,y) = (290, wndMgr.GetScreenHeight() - 100)
		self.SetPosition(x, y)
		
		self.SetSpinData(0)

	def OnMoveWindow(self, x, y):
		(screenWidth, screenHeight) = (wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		if x < 0:
			x = 0
		elif x+self.GetWidth() >= screenWidth-50:
			x = screenWidth-50 - self.GetWidth()
		if y < 0:
			y = 0
		elif y+self.GetHeight() >= screenHeight-50:
			y = screenHeight-50 - self.GetHeight()
		self.SetPosition(x, y)

	def __ClickMark(self):
		infoWindow = self.__children["infoWindow"] if self.__children.has_key("infoWindow") else None
		if not infoWindow:
			infoWindow = InfoWindow()
			self.__children["infoWindow"] = infoWindow
		
		if infoWindow.IsShow():
			infoWindow.Close()
		else:
			infoWindow.Open()

	def SetSpinRewardBoss(self, selectedItemIdx, selectedItemCount, cmdData):
		wheelWindow = WheelWindow()
		(x, y) = self.GetGlobalPosition()
		wheelWindow.SetPosition(x + 30, y - 60)
		wheelWindow.SetSpin(int(selectedItemIdx), int(selectedItemCount), cmdData)
		wheelWindow.Show()
		self.__children["wheelWindow"] = wheelWindow

	def SetSpinData(self, count):
		self.__children["currentCount"] = count
		newImageFolder = IMG_DIR + "unavailable.png" if count != TOTAL_NEED_KILL_COUNT else IMG_DIR + "full.png"
		gaugeImg = self.__children["gaugeImg"]
		if newImageFolder != gaugeImg.imageFolder:
			gaugeImg.LoadImage(newImageFolder)
			gaugeImg.imageFolder = newImageFolder
		gaugeImg.SetRenderingRect(0.0, 0.0, -1.0 + float(count) / float(TOTAL_NEED_KILL_COUNT), 0.0)
		self.__children["gaugeText"].SetText(str(count) + " / " + str(TOTAL_NEED_KILL_COUNT))

		if count != TOTAL_NEED_KILL_COUNT:
			self.__children["spinBtn"].Disable()
		else:
			self.__children["spinBtn"].Enable()

	def __ClickSpin(self, emptyArg):
		if self.__children["currentCount"] != TOTAL_NEED_KILL_COUNT:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "You need finish first")
			return
		net.SendChatPacket("/spin_wheel_boss spin")
		#self.SetSpinReward(50300, 2, "0|50300|2?1|51502|2?2|51503|2?3|51504|2?1|51505|2?1|51501|2?0|51501|2?0|51501|2?0|51504|2?")

	def Open(self):
		self.Show()
		self.SetTop()
	def Close(self):
		self.Hide()
	# def OnPressEscapeKey(self):
		# self.Close()
		# return True

def CreateWindow(window, parent, windowPos, windowArgument = "", windowPositionRule = "", windowSize = (-1, -1), windowFontName = -1):
	window.SetParent(parent)
	if windowSize != (-1, -1):
		window.SetSize(*windowSize)
	if windowPositionRule:
		splitList = windowPositionRule.split(":")
		if len(splitList) == 2:
			(type, mode) = (splitList[0], splitList[1])
			if type == "horizontal":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetHorizontalAlignCenter()
					elif mode == "right":
						window.SetHorizontalAlignRight()
					elif mode == "left":
						window.SetHorizontalAlignLeft()
				else:
					if mode == "center":
						window.SetWindowHorizontalAlignCenter()
					elif mode == "right":
						window.SetWindowHorizontalAlignRight()
					elif mode == "left":
						window.SetWindowHorizontalAlignLeft()
			elif type == "vertical":
				if isinstance(window, ui.TextLine):
					if mode == "center":
						window.SetVerticalAlignCenter()
					elif mode == "top":
						window.SetVerticalAlignTop()
					elif mode == "bottom":
						window.SetVerticalAlignBottom()
				else:
					if mode == "top":
						window.SetWindowVerticalAlignTop()
					elif mode == "center":
						window.SetWindowVerticalAlignCenter()
					elif mode == "bottom":
						window.SetWindowVerticalAlignBottom()
	if windowArgument:
		if isinstance(window, ui.TextLine):
			if windowFontName != -1:
				window.SetFontName(windowFontName)
			window.SetText(windowArgument)
		elif isinstance(window, ui.NumberLine):
			window.SetNumber(windowArgument)
		elif isinstance(window, ui.ExpandedImageBox) or isinstance(window, ui.ImageBox):
			window.LoadImage(windowArgument if windowArgument.find("gr2") == -1 else "icon/item/27995.tga")
	window.SetPosition(*windowPos)
	window.Show()
	return window
