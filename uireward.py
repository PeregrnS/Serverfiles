#author: dracaryS

#static
import ui, constInfo, localeInfo
from operator import truediv

#dynamic
import grp, player, wndMgr, item, net, skill, nonplayer

IMG_DIR = "d:/ymir work/ui/game/reward_info/"
IMG_ICON_DIR = "d:/ymir work/ui/game/reward_info/icons/"

global_reward_info = {
	0 : {
		"type": player.REWARD_MISSION_LEVEL_UP,
		"subValue": 175,
		"icon_name" : "lvl_150",
		"reward_items" : [ [80017, 1], [77404, 25] ],
	},
	1 : {
		"type": player.REWARD_MISSION_SKILL_UPGRADE,
		"subValue": 40,
		"icon_name" : "complete_skills",
		"reward_items" : [ [80017, 1] ],
	},
	2 : {
		"type": player.REWARD_MISSION_FIRST_ITEM,
		"subValue": 166079,
		"icon_name" : "zodiac_9",
		"reward_items" : [ [80018, 1], [77404, 50] ],
	},
	3 : {
		"type": player.REWARD_MISSION_FIRST_ITEM,
		"subValue": 166119,
		"icon_name" : "glocke_9",
		"reward_items" : [ [80018, 1], [77404, 50] ],
	},
	4 : {
		"type": player.REWARD_MISSION_FIRST_ITEM,
		"subValue": 166099,
		"icon_name" : "dolch_9",
		"reward_items" : [ [80018, 1], [77404, 50] ],
	},
	5 : {
		"type": player.REWARD_MISSION_FIRST_ITEM,
		"subValue": 77943,
		"icon_name" : "mount",
		"reward_items" : [ [80018, 1], [77942, 1] ],
	},
	6 : {
		"type": player.REWARD_MISSION_FIRST_ITEM,
		"subValue": 77912,
		"icon_name" : "pet",
		"reward_items" : [ [80018, 1], [77914, 1] ],
	},
	7 : {
		"type": player.REWARD_MISSION_FIRST_ITEM,
		"subValue": 51003,
		"icon_name" : "energy_crystal",
		"reward_items" : [ [80018, 1] ],
	},
	8 : {
		"type": player.REWARD_MISSION_BATTLEPASS,
		"subValue": 0,
		"icon_name" : "battlepass",
		"reward_items" : [ [80018, 1] ],
	},
	9 : {
		"type": player.REWARD_MISSION_BIOLOG,
		"subValue": 120,
		"icon_name" : "bio_94",
		"reward_items" : [ [80018, 1] ],
	},
	10 : {
		"type": player.REWARD_MISSION_OFFLINESHOP_SLOT,
		"subValue": 40,
		"icon_name" : "offlineshop_slot",
		"reward_items" : [ [80018, 1] ],
	},
}

# solo_reward_info = {
	# 0 : {
		# "type": player.REWARD_MISSION_LEVEL_UP,
		# "subValue": 120,
		# "max_value": 1,
		# "level_difference" : 0,
		# "icon_name" : "lvl_120",
		# "reward_items" : [ [25041, 10] ],
	# },
	# 1 : {
		# "type": player.REWARD_MISSION_KILL_STONE,
		# "subValue": 0,
		# "max_value": 25000,
		# "level_difference" : 15,
		# "icon_name" : "metinstone",
		# "reward_items" : [ [39069, 5] ],
	# },
	# 2 : {
		# "type": player.REWARD_MISSION_KILL_BOSS,
		# "subValue": 2493,
		# "max_value": 100,
		# "level_difference" : 0,
		# "icon_name" : "beran_setaou",
		# "reward_items" : [ [72066, 5] ],
	# },
	# 3 : {
		# "type": player.REWARD_MISSION_KILL_BOSS,
		# "subValue": 2598,
		# "max_value": 100,
		# "level_difference" : 0,
		# "icon_name" : "azrail",
		# "reward_items" : [ [50926, 250] ],
	# },
	# 4 : {
		# "type": player.REWARD_MISSION_KILL_BOSS,
		# "subValue": 2092,
		# "max_value": 100,
		# "level_difference" : 0,
		# "icon_name" : "barones",
		# "reward_items" : [ [54317, 250] ],
	# },
	# 5 : {
		# "type": player.REWARD_MISSION_KILL_BOSS,
		# "subValue": 6091,
		# "max_value": 100,
		# "level_difference" : 0,
		# "icon_name" : "razador",
		# "reward_items" : [ [27987, 250] ],
	# },
	# 6 : {
		# "type": player.REWARD_MISSION_KILL_BOSS,
		# "subValue": 6191,
		# "max_value": 100,
		# "level_difference" : 0,
		# "icon_name" : "nemere",
		# "reward_items" : [ [31031, 5] ],
	# },
	# 7 : {
		# "type": player.REWARD_MISSION_KILL_BOSS,
		# "subValue": 0,
		# "max_value": 2500,
		# "level_difference" : 0,
		# "icon_name" : "boss",
		# "reward_items" : [ [50512, 1] ],
	# },
	# 8 : {
		# "type": player.REWARD_MISSION_PLAYTIME,
		# "subValue": 43200,
		# "max_value": 43200,
		# "level_difference" : 0,
		# "icon_name" : "playtime",
		# "reward_items" : [ [54319, 50] ],
	# },
	# 9 : {
		# "type": player.REWARD_MISSION_USE_ITEM,
		# "subValue": 71084,
		# "max_value": 1000000,
		# "level_difference" : 0,
		# "icon_name" : "enchant",
		# "reward_items" : [ [80025, 1] ],
	# },
# }

SCROLLBAR_HEIGHT = 337

def GetMissionName(pageIdx, type, subValue, levelDifference = 0):
	__localeText = {
		player.REWARD_MISSION_LEVEL_UP : "Level Up: %d",
		player.REWARD_MISSION_LEVEL_UP_PET : "Pet Level Up: %d",
		player.REWARD_MISSION_INVENTORY_SLOT : "Inventory Slot Opened: %dx",
		player.REWARD_MISSION_OFFLINESHOP_SLOT : "OfflineShop Slot Opened: %dx",
		player.REWARD_MISSION_AVERAGE_BONUS : "Average Bonus: %d",
		player.REWARD_MISSION_BATTLEPASS : "Finish BattlePass",
		player.REWARD_MISSION_CUSTOM_SASH : "Custom Sash",
		player.REWARD_MISSION_BIOLOG : "Biolog Mission: %d Level",
		player.REWARD_MISSION_COMPLETE_SKILL : "Complete All Skill To P",
	}
	missionName = ""
	if not __localeText.has_key(type):
		if type == player.REWARD_MISSION_PASSIVE_SKILL_COMPLETE:
			missionName = "Complete Passive Skill: %s" % skill.GetSkillName(subValue)
		elif type == player.REWARD_MISSION_SKILL_UPGRADE:
			skillGradeName = ""
			if subValue >= 50:
				skillGradeName = "Sage Perfect Master"
			elif subValue >= 40:
				skillGradeName = "Perfect Master"
			elif subValue >= 30:
				skillGradeName = "Grand Master"
			elif subValue >= 20:
				skillGradeName = "Master"
			missionName = "Complete a skill to %s" % skillGradeName
		elif type == player.REWARD_MISSION_FIRST_ITEM:
			if subValue == 0:
				missionName = "Capture a item"
			else:
				item.SelectItem(subValue)
				missionName = localeInfo.CAPTURE_ITEM % item.GetItemName()
		elif type == player.REWARD_MISSION_KILL_BOSS:
			if subValue == 0:
				missionName = "Kill All Boss"
			else:
				missionName = "Kill Boss: %s" % nonplayer.GetMonsterName(subValue)
		elif type == player.REWARD_MISSION_KILL_STONE:
			if subValue == 0:
				missionName = "Kill All Stone"
			else:
				missionName = "Kill Stone: %s" % nonplayer.GetMonsterName(subValue)
		elif type == player.REWARD_MISSION_KILL_MONSTER:
			if subValue == 0:
				missionName = "Kill All Monster"
			else:
				missionName = "Kill Monster: %s" % nonplayer.GetMonsterName(subValue)
		elif type == player.REWARD_MISSION_DUNGEON:
			if subValue == 0:
				missionName = "Make All Dungeons"
			else:
				__dungeonNames = {
					1093 : "Devil Tower"
				}
				missionName = "Make Dungeon: %s" % (__dungeonNames[subValue] if __dungeonNames.has_key(subValue) else "Unknown")
		elif type == player.REWARD_MISSION_USE_ITEM:
			if subValue == 0:
				missionName = "Use All Item"
			else:
				item.SelectItem(subValue)
				missionName = "Use Item: %s" % item.GetItemName()
		elif type == player.REWARD_MISSION_SELL_ITEM:
			if subValue == 0:
				missionName = "Sell All Item"
			else:
				item.SelectItem(subValue)
				missionName = "Sell Item: %s" % item.GetItemName()
		elif type == player.REWARD_MISSION_BUY_ITEM:
			if subValue == 0:
				missionName = "Buy All Item"
			else:
				item.SelectItem(subValue)
				missionName = "Buy Item: %s" % item.GetItemName()
		elif type == player.REWARD_MISSION_PLAYTIME:
			missionName = "Play Time: %s" % localeInfo.SecondToDHM(subValue)
	else:
		try:
			missionName = __localeText[type] % subValue
		except:
			missionName = __localeText[type]
	if levelDifference != 0:
		missionName += " " + ("(+- %d Level Difference)" % levelDifference)
	return missionName

class RewardWindow(ui.Window):

	class RewardItemGlobal(ui.ImageBox):
		def Destroy(self):
			self.__children = {}
		def GetRewardIndex(self):
			return self.__children["rewardIndex"] if self.__children.has_key("rewardIndex") else 0
		def __init__(self, pageIdx, rewardIndex, rewardData):
			ui.ImageBox.__init__(self)
			self.LoadReward(pageIdx, rewardIndex, rewardData)
		def LoadReward(self, pageIdx, rewardIndex, rewardData):
			self.LoadImage(IMG_DIR+"item_0.tga")
			self.Destroy()
			self.__children["rewardIndex"] = rewardIndex

			self.__children["rewardImage"] = CreateWindow(ui.ImageBox(), self, (0, 0), IMG_ICON_DIR+rewardData["icon_name"]+".tga")

			rewardName = CreateWindow(ui.TextLine(), self, (95, 55), GetMissionName(pageIdx, rewardData["type"], rewardData["subValue"]), "horizontal:left")
			rewardName.SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))
			self.__children["rewardName"] = rewardName

			self.SetRewardPlayer(rewardData["player_name"])

			grid = Grid(3,2)
			grid.reset()

			rewardItems = CreateWindow(ui.GridSlotWindow(), self, (345, 9))
			rewardItems.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			rewardItems.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			rewardItems.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			rewardItems.ArrangeSlot(0, 3, 2, 32, 32, 0, 0)
			rewardItems.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			for rewardItem in rewardData["reward_items"]:
				(itemVnum,itemCount) = (rewardItem[0],rewardItem[1])
				item.SelectItem(itemVnum)
				(width,height) = item.GetItemSize()
				pos = grid.find_blank(width, height)
				grid.put(pos, width, height)
				rewardItems.SetItemSlot(pos, itemVnum, itemCount)
			rewardItems.RefreshSlot()
			self.__children["rewardItems"] = rewardItems
		def SetRewardPlayer(self, playerName):
			if playerName == "":
				return
			rewardPlayer = CreateWindow(ui.TextLine(), self, (95, 18), playerName, "horizontal:left")
			if player.GetName() == playerName:
				rewardPlayer.SetPackedFontColor(grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0))
			self.__children["rewardPlayer"] = rewardPlayer

		def OverInItem(self, slotIndex):
			global global_reward_info
		
			# Welcher Reward-Eintrag bin ich? (0,1,2,...) aus global_reward_info
			rewardIndex = self.GetRewardIndex()
			rewardData = global_reward_info[rewardIndex] if global_reward_info.has_key(rewardIndex) else None
			if not rewardData:
				return
		
			# SlotIndex -> passendes Item in reward_items
			if slotIndex < 0 or slotIndex >= len(rewardData["reward_items"]):
				return
		
			itemVnum = rewardData["reward_items"][slotIndex][0]
		
			interface = constInfo.GetInterfaceInstance()
			if interface and interface.tooltipItem:
				interface.tooltipItem.SetItemToolTip(itemVnum)
		def OverOutItem(self):
			interface = constInfo.GetInterfaceInstance()
			if interface:
				if interface.tooltipItem:
					interface.tooltipItem.HideToolTip()
	# class RewardItemSolo(ui.ImageBox):
		# def Destroy(self):
			# self.__children = {}
		# def GetRewardIndex(self):
			# return self.__children["rewardIndex"] if self.__children.has_key("rewardIndex") else 0
		# def __init__(self, pageIdx, rewardIndex, rewardData):
			# ui.ImageBox.__init__(self)
			# self.LoadReward(pageIdx, rewardIndex, rewardData)
		# def LoadReward(self, pageIdx, rewardIndex, rewardData):
			# self.LoadImage(IMG_DIR+"item_1.tga")
			# self.Destroy()
			# self.__children["rewardIndex"] = rewardIndex

			# self.__children["rewardImage"] = CreateWindow(ui.ImageBox(), self, (0, 0), IMG_ICON_DIR+rewardData["icon_name"]+".tga")
			# self.__children["bar"] = CreateWindow(ui.ExpandedImageBox(), self, (92, 32), IMG_DIR+"bar.tga")

			# rewardName = CreateWindow(ui.TextLine(), self, (95, 55), GetMissionName(pageIdx, rewardData["type"], rewardData["subValue"], rewardData["level_difference"] if rewardData.has_key("level_difference") else 0), "horizontal:left")
			# rewardName.SetPackedFontColor(grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0))
			# self.__children["rewardName"] = rewardName

			# self.SetRewardValue(rewardData["value"])

			# grid = Grid(3,2)
			# grid.reset()

			# rewardItems = CreateWindow(ui.GridSlotWindow(), self, (345, 9))
			# rewardItems.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			# rewardItems.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			# rewardItems.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			# rewardItems.ArrangeSlot(0, 3, 2, 32, 32, 0, 0)
			# rewardItems.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			# for rewardItem in rewardData["reward_items"]:
				# (itemVnum,itemCount) = (rewardItem[0],rewardItem[1])
				# item.SelectItem(itemVnum)
				# (width,height) = item.GetItemSize()
				# pos = grid.find_blank(width, height)
				# grid.put(pos, width, height)
				# rewardItems.SetItemSlot(pos, itemVnum, itemCount)
			# rewardItems.RefreshSlot()
			# self.__children["rewardItems"] = rewardItems
		
		# def SetRewardValue(self, value):
			# maxValue = solo_reward_info[self.GetRewardIndex()]["max_value"]

			# self.__children["value"] = CreateWindow(ui.TextLine(), self, (95, 11), NumberToDecimalString(value)+" / "+NumberToDecimalString(maxValue))
			# if value:
				# self.__children["bar"].Show()
				# self.__children["bar"].SetScale(float(value) * (1.0 / float(maxValue)), 1.0)
			# else:
				# self.__children["bar"].Hide()

		# def OverInItem(self, index):
			# global global_reward_info
			# rewardData = global_reward_info[index] if global_reward_info.has_key(index) else None
			# if not rewardData:
				# return
			# interface = constInfo.GetInterfaceInstance()
			# if interface:
				# if interface.tooltipItem:
					# interface.tooltipItem.SetItemToolTip(rewardData["reward_items"][index][0])
		# def OverOutItem(self):
			# interface = constInfo.GetInterfaceInstance()
			# if interface:
				# if interface.tooltipItem:
					# interface.tooltipItem.HideToolTip()

	def Destroy(self):
		self.__children = {}

	def __init__(self):
		ui.Window.__init__(self)
		self.__LoadWindow()

	def __LoadWindow(self):
		self.Destroy()

		self.AddFlag("movable")
		self.AddFlag("attach")
		self.AddFlag("float")
		# self.AddFlag("animate")

		board = CreateWindow(ui.BoardWithFinalCoreTitleBar13(), self, (0, 0), "", "", (484, 385))
		board.AddFlag("attach")
		board.SetCloseEvent(self.Close)
		self.__children["board"] = board

		boardImage = CreateWindow(ui.ImageBox(), board, (8, 30), IMG_DIR+"board.tga")
		self.__children["boardImage"] = boardImage

		scrollBar = CreateWindow(ScrollBarSpecial(False), boardImage, (459, 1), "", "", (9, SCROLLBAR_HEIGHT))
		scrollBar.SetScale(SCROLLBAR_HEIGHT/2, SCROLLBAR_HEIGHT)
		self.__children["scrollBar"] = scrollBar

		listBox = CreateWindow(ui.ListBoxEx(), boardImage, (4, 4), "", "", (boardImage.GetWidth(), boardImage.GetHeight()))
		listBox.SetViewItemCount(4)
		listBox.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)
		listBox.SetItemSize(450, 81)
		listBox.SetItemStep(83)
		listBox.SetScrollBar(scrollBar)
		self.__children["listBox"] = listBox

		# pageBtnImg = CreateWindow(ui.ImageBox(), self, (3, 30 + boardImage.GetHeight() + 5), IMG_DIR+"page_0.tga")
		# self.__children["pageBtnImg"] = pageBtnImg

		# pageBtn = CreateWindow(ui.ImageBox(), pageBtnImg, (0, 0), IMG_DIR+"btn.tga")
		# self.__children["pageBtn"] = pageBtn

		self.SetSize(484, 420)
		self.SetCenterPosition()

		for key, data in global_reward_info.items():
			data["player_name"] = ""

		# for key, data in solo_reward_info.items():
			# data["value"] = 0

		self.__SetPage("", 0)

	def __SetPage(self, emptyArg, pageIdx):
		self.__children["pageIdx"] = pageIdx

		# self.__children["pageBtn"].SetPosition(14 if pageIdx else 75, 8)
		# self.__children["pageBtn"].SetEvent(ui.__mem_func__(self.__SetPage), "mouse_click", 0)
		# self.__children["pageBtnImg"].LoadImage("{}page_{}.tga".format(IMG_DIR, pageIdx))

		listBox = self.__children["listBox"] if self.__children.has_key("listBox") else None
		scrollBar = self.__children["scrollBar"] if self.__children.has_key("scrollBar") else None
		listBox.RemoveAllItems()

		scrollBar.SetPos(0)

		totalY = 0
		if pageIdx == 0:
			#global
			for key, data in global_reward_info.items():
				listBox.AppendItem(self.RewardItemGlobal(pageIdx, key, data))
				totalY += 83
		# elif pageIdx == 1:
			# for key, data in solo_reward_info.items():
				# listBox.AppendItem(self.RewardItemSolo(pageIdx, key, data))
				# totalY += 83

		# self.__children["board"].SetTitleName(["Global Rewards", "Solo Rewards"][pageIdx])
		self.__children["board"].SetTitleName(["Global Rewards"][pageIdx])
		scrollBar.SetScale(SCROLLBAR_HEIGHT, totalY if totalY > SCROLLBAR_HEIGHT else SCROLLBAR_HEIGHT)

	def RewardData(self, isGlobal, isNeedClean, commandText):
		pageIdx = self.__children["pageIdx"]
		if isGlobal:
			if isNeedClean:
				for key, data in global_reward_info.items():
					data["player_name"] = ""

			if commandText != "empty":
				dataList = commandText.split("#")
				for data in dataList:
					dataSplit = data.split("?")
					if len(dataSplit) == 2:
						idx = int(dataSplit[0])
						if global_reward_info.has_key(idx):
							global_reward_info[idx]["player_name"] = dataSplit[1]

						if pageIdx == 0:
							listBox = self.__children["listBox"] if self.__children.has_key("listBox") else None
							for child in listBox.itemList:
								if child.GetRewardIndex() == idx:
									child.SetRewardPlayer(dataSplit[1])
									break
		else:
			# if isNeedClean:
				# for key, data in solo_reward_info.items():
					# data["value"] = 0
			if commandText != "empty":
				dataList = commandText.split("#")
				for data in dataList:
					dataSplit = data.split("?")
					if len(dataSplit) == 2:
						idx = int(dataSplit[0])
						# if solo_reward_info.has_key(idx):
							# solo_reward_info[idx]["value"] = int(dataSplit[1])

						if pageIdx == 1:
							listBox = self.__children["listBox"] if self.__children.has_key("listBox") else None
							for child in listBox.itemList:
								if child.GetRewardIndex() == idx:
									child.SetRewardValue(int(dataSplit[1]))
									break

	def OnMouseWheel(self, length):
		scrollBar = self.__children["scrollBar"] if self.__children.has_key("scrollBar") else None
		if scrollBar:
			if scrollBar.IsShow():
				scrollBar.OnMouseWheel(length)
				return True
		return False

	def Open(self):
		packetStatus = self.__children["packetStatus"] if self.__children.has_key("packetStatus") else False
		if not packetStatus:
			net.SendChatPacket("/reward_system update")
			self.__children["packetStatus"] = True
		self.SetTop()
		self.Show()
	def Close(self):
		self.Hide()
	def OnPressEscapeKey(self):
		self.Close()
		return True

class ScrollBarSpecial(ui.Window):
	BASE_COLOR = grp.GenerateColor(0.0, 0.0, 0.0, 1.0)
	CORNERS_AND_LINES_COLOR = grp.GenerateColor(0.3411, 0.3411, 0.3411, 1.0)
	BAR_NUMB = 9 #This is static value! Please dont touch in him.
	SCROLL_WIDTH= 8
	class MiddleBar(ui.DragButton):
		MIDDLE_BAR_COLOR = grp.GenerateColor(0.6470, 0.6470, 0.6470, 1.0)
		def __init__(self, horizontal_scroll):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.horizontal_scroll = horizontal_scroll
			self.middle = ui.Bar()
			self.middle.SetParent(self)
			self.middle.AddFlag("attach")
			self.middle.AddFlag("not_pick")
			self.middle.SetColor(self.MIDDLE_BAR_COLOR)
			self.middle.SetSize(1, 1)
			self.middle.Show()
		def SetStaticScale(self, size):
			(base_width, base_height) = (self.middle.GetWidth(), self.middle.GetHeight())
			if not self.horizontal_scroll:
				ui.DragButton.SetSize(self, base_width, size)
				self.middle.SetSize(base_width, size)
			else:
				ui.DragButton.SetSize(self, size, base_height)
				self.middle.SetSize(size, base_height)
		def SetSize(self, selfSize, fullSize):
			(base_width, base_height) = (self.middle.GetWidth(), self.middle.GetHeight())
			
			if not self.horizontal_scroll:
				ui.DragButton.SetSize(self, base_width, truediv(int(selfSize), int(fullSize)) * selfSize)
				self.middle.SetSize(base_width, truediv(int(selfSize), int(fullSize)) * selfSize)
			else:
				ui.DragButton.SetSize(self, truediv(int(selfSize), int(fullSize)) * selfSize, base_height)
				self.middle.SetSize(truediv(int(selfSize), int(fullSize)) * selfSize, base_height)
		def SetStaticSize(self, size):
			size = max(2, size)
			
			if not self.horizontal_scroll:
				ui.DragButton.SetSize(self, size, self.middle.GetHeight())
				self.middle.SetSize(size, self.middle.GetHeight())
			else:
				ui.DragButton.SetSize(self, self.middle.GetWidth(), size)
				self.middle.SetSize(self.middle.GetWidth(), size)
	def __init__(self, horizontal_scroll = False):
		ui.Window.__init__(self)
		self.horizontal_scroll = horizontal_scroll
		self.scrollEvent = None
		self.scrollSpeed = 50
		self.sizeScale = 1.0
		self.bars = []
		for i in xrange(self.BAR_NUMB):
			br = ui.Bar()
			br.SetParent(self)
			br.AddFlag("attach")
			br.AddFlag("not_pick")
			br.SetColor([self.CORNERS_AND_LINES_COLOR, self.BASE_COLOR][i == (self.BAR_NUMB-1)])
			if not (i % 2 == 0): br.SetSize(1, 1)
			br.Show()
			self.bars.append(br)
		self.middleBar = self.MiddleBar(self.horizontal_scroll)
		self.middleBar.SetParent(self)
		self.middleBar.SetMoveEvent(ui.__mem_func__(self.OnScrollMove))
		self.middleBar.Show()
	def OnScrollMove(self):
		if not self.scrollEvent:
			return
		arg = float(self.middleBar.GetLocalPosition()[1] - 1) / float(self.GetHeight() - 2 - self.middleBar.GetHeight()) if not self.horizontal_scroll else\
				float(self.middleBar.GetLocalPosition()[0] - 1) / float(self.GetWidth() - 2 - self.middleBar.GetWidth())
		self.scrollEvent(arg)
	def SetScrollEvent(self, func):
		self.scrollEvent = func
	def SetScrollSpeed(self, speed):
		self.scrollSpeed = speed
	def OnMouseWheel(self, length):
		if not self.IsShow():
			return False
		length = int((length * 0.01) * self.scrollSpeed)
		if not self.horizontal_scroll:
			val = min(max(1, self.middleBar.GetLocalPosition()[1] - (length * 0.01) * self.scrollSpeed * self.sizeScale), self.GetHeight() - self.middleBar.GetHeight() - 1)
			self.middleBar.SetPosition(1, val)
		else:
			val = min(max(1, self.middleBar.GetLocalPosition()[0] - (length * 0.01) *  self.scrollSpeed * self.sizeScale), self.GetWidth() - self.middleBar.GetWidth() - 1)
			self.middleBar.SetPosition(val, 1)
		self.OnScrollMove()
		return True
	def GetPos(self):
		return float(self.middleBar.GetLocalPosition()[1] - 1) / float(self.GetHeight() - 2 - self.middleBar.GetHeight()) if not self.horizontal_scroll else float(self.middleBar.GetLocalPosition()[0] - 1) / float(self.GetWidth() - 2 - self.middleBar.GetWidth())
	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		if not self.horizontal_scroll:
			if xMouseLocalPosition == 0 or xMouseLocalPosition == self.GetWidth():
				return
			y_pos = (yMouseLocalPosition - self.middleBar.GetHeight() / 2)
			self.middleBar.SetPosition(1, y_pos)
		else:
			if yMouseLocalPosition == 0 or yMouseLocalPosition == self.GetHeight():
				return
			x_pos = (xMouseLocalPosition - self.middleBar.GetWidth() / 2)
			self.middleBar.SetPosition(x_pos, 1)
		self.OnScrollMove()
	def SetSize(self, w, h):
		(width, height) = (max(3, w), max(3, h))
		ui.Window.SetSize(self, width, height)
		self.bars[0].SetSize(1, (height - 2))
		self.bars[0].SetPosition(0, 1)
		self.bars[2].SetSize((width - 2), 1)
		self.bars[2].SetPosition(1, 0)
		self.bars[4].SetSize(1, (height - 2))
		self.bars[4].SetPosition((width - 1), 1)
		self.bars[6].SetSize((width - 2), 1)
		self.bars[6].SetPosition(1, (height - 1))
		self.bars[8].SetSize((width - 2), (height - 2))
		self.bars[8].SetPosition(1, 1)
		self.bars[1].SetPosition(0, 0)
		self.bars[3].SetPosition((width - 1), 0)
		self.bars[5].SetPosition((width - 1), (height - 1))
		self.bars[7].SetPosition(0, (height - 1))
		if not self.horizontal_scroll:
			self.middleBar.SetStaticSize(width - 2)
			self.middleBar.SetSize(12, self.GetHeight())
		else:
			self.middleBar.SetStaticSize(height - 2)
			self.middleBar.SetSize(12, self.GetWidth())
		self.middleBar.SetRestrictMovementArea(1, 1, width - 2, height - 2)
	def SetScale(self, selfSize, fullSize):
		self.sizeScale = float(selfSize)/float(fullSize)
		if self.sizeScale <= 0.0305:
			self.sizeScale = 0.05
		self.middleBar.SetSize(selfSize, fullSize)
	def SetStaticScale(self, r_size):
		self.middleBar.SetStaticScale(r_size)
	def SetPosScale(self, fScale):
		pos = (math.ceil((self.GetHeight() - 2 - self.middleBar.GetHeight()) * fScale) + 1) if not self.horizontal_scroll else (math.ceil((self.GetWidth() - 2 - self.middleBar.GetWidth()) * fScale) + 1)
		self.SetPos(pos)
	def SetPos(self, pos):
		wPos = (1, pos) if not self.horizontal_scroll else (pos, 1)
		self.middleBar.SetPosition(*wPos)
		self.OnScrollMove()

class Grid:
	def __init__(self, width, height):
		self.width = width
		self.height = height
		self.reset()
	def find_blank(self, width, height):
		if width > self.width or height > self.height:
			return -1
		for row in range(self.height):
			for col in range(self.width):
				index = row * self.width + col
				if self.is_empty(index, width, height):
					return index
		return -1
	def put(self, pos, width, height):
		if not self.is_empty(pos, width, height):
			return False
		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			col = 1
			while col < width:
				self.grid[start + col] = True
				col += 1
		return True
	def clear(self, pos, width, height):
		if pos < 0 or pos >= (self.width * self.height):
			return
		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			col = 1
			while col < width:
				self.grid[start + col] = False
				col += 1
	def is_empty(self, pos, width, height):
		if pos < 0:
			return False
		row = pos // self.width
		if (row + height) > self.height:
			return False
		if (pos + width) > ((row * self.width) + self.width):
			return False
		for row in range(height):
			start = pos + (row * self.width)
			if self.grid[start]:
				return False
			col = 1
			while col < width:
				if self.grid[start + col]:
					return False
				col += 1
		return True
	def get_size(self):
		return self.width * self.height
	def reset(self):
		self.grid = [False] * (self.width * self.height)

def CreateWindow(window, parent, windowPos, windowArgument = "", windowPositionRule = "", windowSize = (-1, -1), windowFontName = -1):
	window.SetParent(parent)
	window.SetPosition(*windowPos)
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
	window.Show()
	return window

def NumberToDecimalString(n):
	return str('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))
