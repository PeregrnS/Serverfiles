import ui
import net
import item
import skill
import localeInfo
import wndMgr
import player
import constInfo
import mouseModule
import uiScriptLocale
import app
import background
import chat
import guild

if app.ENABLE_GEM_SYSTEM:
	import uiToolTip

if app.ENABLE_ANTI_MULTIPLE_FARM:
	import anti_multiple_farm
	MULTIPLE_FARM_STATE_IMAGES = ["d:/ymir work/ui/pattern/visible_mark_01.tga", "d:/ymir work/ui/pattern/visible_mark_03.tga"]

MOUSE_SETTINGS = [0, 0]

def InitMouseButtonSettings(left, right):
	global MOUSE_SETTINGS
	MOUSE_SETTINGS = [left, right]

def SetMouseButtonSetting(dir, event):
	global MOUSE_SETTINGS
	MOUSE_SETTINGS[dir] = event
	
def GetMouseButtonSettings():
	global MOUSE_SETTINGS
	return MOUSE_SETTINGS

def SaveMouseButtonSettings():
	global MOUSE_SETTINGS
	open("mouse.cfg", "w").write("%s\t%s" % tuple(MOUSE_SETTINGS))

def LoadMouseButtonSettings():
	global MOUSE_SETTINGS
	tokens = open("mouse.cfg", "r").read().split()

	if len(tokens) != 2:
		raise RuntimeError, "MOUSE_SETTINGS_FILE_ERROR"

	MOUSE_SETTINGS[0] = int(tokens[0])
	MOUSE_SETTINGS[1] = int(tokens[1])

def unsigned32(n):
	return n & 0xFFFFFFFFL

#-------------------Giftbox Begin------------------------------

class GiftBox(ui.ScriptWindow):
	class TextToolTip(ui.Window):
		def __init__(self):
			ui.Window.__init__(self, "TOP_MOST")
			self.SetWindowName("GiftBox")
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignCenter()
			textLine.SetOutline()
			textLine.Show()
			self.textLine = textLine

		def __del__(self):
			ui.Window.__del__(self)

		def SetText(self, text):
			self.textLine.SetText(text)

		def OnRender(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			self.textLine.SetPosition(mouseX, mouseY - 15)

	def __init__(self):
		#print "NEW TASKBAR  ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.tooltipGift = self.TextToolTip()
		self.tooltipGift.Show()
		
	def __del__(self):
		#print "---------------------------------------------------------------------------- DELETE TASKBAR"
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "giftbox.py")
		except:
			import exception
			exception.Abort("GiftBox.LoadWindow.LoadObject")		

		self.giftBoxIcon = self.GetChild("GiftBox_Icon")
		self.giftBoxToolTip = self.GetChild("GiftBox_ToolTip")
	
	def Destroy(self):		
		self.giftBoxIcon = 0
		self.giftBoxToolTip = 0		
			
#-------------------Giftbox End------------------------------

class EnergyBar(ui.ScriptWindow):
	class TextToolTip(ui.Window):
		def __init__(self):
			ui.Window.__init__(self, "TOP_MOST")
			self.SetWindowName("EnergyBar")
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignCenter()
			textLine.SetOutline()
			textLine.Show()
			self.textLine = textLine

		def __del__(self):
			ui.Window.__del__(self)

		def SetText(self, text):
			self.textLine.SetText(text)

		def OnRender(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			self.textLine.SetPosition(mouseX, mouseY - 15)

	def __init__(self):
		#print "NEW TASKBAR  ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.tooltipEnergy = self.TextToolTip()
		self.tooltipEnergy.Show()
		
	def __del__(self):
		#print "---------------------------------------------------------------------------- DELETE TASKBAR"
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "EnergyBar.py")
		except:
			import exception
			exception.Abort("EnergyBar.LoadWindow.LoadObject")

		self.energyEmpty = self.GetChild("EnergyGauge_Empty")
		self.energyHungry = self.GetChild("EnergyGauge_Hungry")
		self.energyFull = self.GetChild("EnergyGauge_Full")

		self.energyGaugeBoard = self.GetChild("EnergyGauge_Board")
		self.energyGaugeToolTip = self.GetChild("EnergyGauge_ToolTip")

		
	def Destroy(self):		
		self.energyEmpty = None
		self.energyHungry = None
		self.energyFull = None
		self.energyGaugeBoard = None
		self.energyGaugeToolTip = None
		self.tooltipEnergy = None

	## Gauge
	def RefreshStatus(self):
		pointEnergy = player.GetStatus (player.ENERGY)
		if app.ENABLE_EXTENDED_BLEND:
			if pointEnergy != 0:
				leftTimeEnergy = 7200
			else:
				leftTimeEnergy = player.GetStatus(player.ENERGY_END_TIME) - app.GetGlobalTimeStamp()
		else:
			leftTimeEnergy = player.GetStatus(player.ENERGY_END_TIME) - app.GetGlobalTimeStamp()
		self.SetEnergy (pointEnergy, leftTimeEnergy, 7200)
			
	def SetEnergy (self, point, leftTime, maxTime):
		leftTime = max (leftTime, 0)
		maxTime = max (maxTime, 0)
			
		self.energyEmpty.Hide()
		self.energyHungry.Hide()
		self.energyFull.Hide()
	
		if leftTime == 0:
			self.energyEmpty.Show()
		elif ((leftTime * 100) / maxTime) < 15:
			self.energyHungry.Show()
		else:
			self.energyFull.Show()
			
		self.tooltipEnergy.SetText("%s" % (localeInfo.TOOLTIP_ENERGY(point)))

	def OnUpdate(self):
		if self.energyGaugeToolTip and self.tooltipEnergy:
			try:
				if True == self.energyGaugeToolTip.IsIn():
					self.RefreshStatus()
					self.tooltipEnergy.Show()
					return
			except:
				pass

		if self.tooltipEnergy:
			try:
				self.tooltipEnergy.Hide()
			except:
				pass

class ExpandedTaskBar(ui.ScriptWindow):
	# BUTTON_DRAGON_SOUL = 0
	if app.ENABLE_SWITCHBOT_SYSTEM:
		BUTTON_SWITCHBOT_SYSTEM = 0
	if app.ENABLE_INGAME_WIKI:
		BUTTON_INGAME_WIKI = 1
	if app.ENABLE_HUNTING_SYSTEM:
		BUTTON_HUNTING_SYSTEM = 2
	if app.ENABLE_ANTI_EXP:
		BUTTON_ANTI_EXP = 3
	if app.ENABLE_PREMIUM_PRIVATE_SHOP:
		BUTTON_OFFLINE_SHOP_SEARCH = 4
	if app.ENABLE_EVENT_MANAGER or app.ENABLE_EVENT_CALENDAR:
		BUTTON_EVENT_MANAGER = 5
	if app.ENABLE_BATTLE_PASS:
		BUTTON_BATTLEPASS = 6
	if app.ENABLE_DUNGEON_INFO_SYSTEM:
		BUTTON_DUNGEON_INFO = 7
	if app.ENABLE_BIOLOG_SYSTEM:
		BUTTON_BIOLOG = 8

	def __init__(self):
		ui.Window.__init__(self)
		self.SetWindowName("ExpandedTaskBar")

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "ExpandedTaskBar.py")

		except:
			import exception
			exception.Abort("ExpandedTaskBar.LoadWindow.LoadObject")

		self.expandedTaskBarBoard = self.GetChild("ExpanedTaskBar_Board")

		self.toggleButtonDict = {}
		# self.toggleButtonDict[ExpandedTaskBar.BUTTON_DRAGON_SOUL] = self.GetChild("DragonSoulButton")
		# self.toggleButtonDict[ExpandedTaskBar.BUTTON_DRAGON_SOUL].SetParent(self)
		if app.ENABLE_SWITCHBOT_SYSTEM:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_SWITCHBOT_SYSTEM] = self.GetChild("SwitchBotButton")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_SWITCHBOT_SYSTEM].SetParent(self)
		if app.ENABLE_INGAME_WIKI:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_INGAME_WIKI] = self.GetChild("WikiButton")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_INGAME_WIKI].SetParent(self)
		#if app.ENABLE_HUNTING_SYSTEM:
			#self.toggleButtonDict[ExpandedTaskBar.BUTTON_HUNTING_SYSTEM] = self.GetChild("HuntinWindowButton")
			#self.toggleButtonDict[ExpandedTaskBar.BUTTON_HUNTING_SYSTEM].SetParent(self)
			## TODO
			# if constInfo.HUNTING_BUTTON_FLASH == 1 and constInfo.HUNTING_BUTTON_IS_FLASH == 0:
			# 	self.GetChild("HuntinWindowButton").EnableFlash()
			# 	constInfo.HUNTING_BUTTON_IS_FLASH = 1
			# if constInfo.HUNTING_BUTTON_FLASH == 0 and constInfo.HUNTING_BUTTON_IS_FLASH == 1:
			# 	self.GetChild("HuntinWindowButton").DisableFlash()
		if app.ENABLE_ANTI_EXP:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_ANTI_EXP] = self.GetChild("AntiExp")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_ANTI_EXP].SetParent(self)
		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_OFFLINE_SHOP_SEARCH] = self.GetChild("OfflineSearchButton")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_OFFLINE_SHOP_SEARCH].SetParent(self)
		if app.ENABLE_EVENT_MANAGER or app.ENABLE_EVENT_CALENDAR:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_EVENT_MANAGER] = self.GetChild("EventManagerButton")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_EVENT_MANAGER].SetParent(self)
		if app.ENABLE_BATTLE_PASS:
			self.GetChild("BattlePassButton").Hide()
			#self.toggleButtonDict[ExpandedTaskBar.BUTTON_BATTLEPASS] = self.GetChild("BattlePassButton")
			#self.toggleButtonDict[ExpandedTaskBar.BUTTON_BATTLEPASS].SetParent(self)
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.GetChild("DungeonInfoButton").Hide()
			#self.toggleButtonDict[ExpandedTaskBar.BUTTON_DUNGEON_INFO] = self.GetChild("DungeonInfoButton")
			#self.toggleButtonDict[ExpandedTaskBar.BUTTON_DUNGEON_INFO].SetParent(self)
		if app.ENABLE_BIOLOG_SYSTEM:
			self.GetChild("BiologButton").Hide()
			#self.toggleButtonDict[ExpandedTaskBar.BUTTON_BIOLOG] = self.GetChild("BiologButton")
			#self.toggleButtonDict[ExpandedTaskBar.BUTTON_BIOLOG].SetParent(self)

	def SetTop(self):
		super(ExpandedTaskBar, self).SetTop()
		for button in self.toggleButtonDict.values():
			button.SetTop()

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def SetToolTipText(self, eButton, text):
		self.toggleButtonDict[eButton].SetToolTipText(text)

	def SetToggleButtonEvent(self, eButton, kEventFunc):
		if eButton in self.toggleButtonDict:
			self.toggleButtonDict[eButton].SetEvent(kEventFunc)
		else:
			print("Warnung: Button {} existiert nicht in toggleButtonDict".format(eButton))

	if app.ENABLE_ANTI_EXP:
		def RefreshAntiExpButton(self, status):
			if status:
				self.toggleButtonDict[ExpandedTaskBar.BUTTON_ANTI_EXP].SetUpVisual("d:/ymir work/ui/game/TaskBar/anti_exp_on.tga")
				self.toggleButtonDict[ExpandedTaskBar.BUTTON_ANTI_EXP].SetOverVisual("d:/ymir work/ui/game/TaskBar/anti_exp_hover.tga")
				self.toggleButtonDict[ExpandedTaskBar.BUTTON_ANTI_EXP].SetDownVisual("d:/ymir work/ui/game/TaskBar/anti_exp_normal.tga")
			else:
				self.toggleButtonDict[ExpandedTaskBar.BUTTON_ANTI_EXP].SetUpVisual("d:/ymir work/ui/game/TaskBar/anti_exp1_off.tga")
				self.toggleButtonDict[ExpandedTaskBar.BUTTON_ANTI_EXP].SetOverVisual("d:/ymir work/ui/game/TaskBar/anti_exp1_hover.tga")
				self.toggleButtonDict[ExpandedTaskBar.BUTTON_ANTI_EXP].SetDownVisual("d:/ymir work/ui/game/TaskBar/anti_exp1_normal.tga")

	# def OnPressEscapeKey(self):
	# 	self.Close()
	# 	return True

if app.ENABLE_GEM_SYSTEM:
	class ExpandedMoneyTaskBar(ui.ScriptWindow):

		def __init__(self):
			ui.Window.__init__(self)

			self.wndMoney = None
			self.wndMoneySlot = None
			self.wndMoneyIcon = None

			self.wndGem = None
			self.wndGemSlot = None
			self.wndGemIcon = None

		def __del__(self):
			ui.Window.__del__(self)
	
		def Destroy(self):	
			self.wndMoney = None
			self.wndMoneySlot = None
			self.wndMoneyIcon = None

			self.wndGem = None
			self.wndGemSlot = None
			self.wndGemIcon = None
		
		def LoadWindow(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/ExpandedMoneyTaskbar.py")
				
				self.wndMoney = self.GetChild("Money")
				self.wndMoneyIcon = self.GetChild("Money_Icon")
				self.wndMoneySlot = self.GetChild("Money_Slot")

				self.wndGem = self.GetChild("Gem")
				self.wndGemIcon = self.GetChild("Gem_Icon")
				self.wndGemSlot = self.GetChild("Gem_Slot")

				self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 0)
				self.wndGemIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_in", 1)
				
				self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 0)
				self.wndGemIcon.SetEvent(ui.__mem_func__(self.EventProgress), "mouse_over_out", 1)

				self.toolTip = uiToolTip.ToolTip()
				self.toolTip.ClearToolTip()

			except:
				import exception
				exception.Abort("ExpandedMoneyTaskBar.LoadWindow.LoadObject")

		def RefreshStatus(self):
			money = player.GetElk()
			self.wndMoney.SetText(localeInfo.DottedNumber(money))
			gem = player.GetGem()
			self.wndGem.SetText(localeInfo.DottedNumber(gem))
		
		def GetMoneySlot(self):
				if self.wndMoneySlot:
					return self.wndMoneySlot
				else:
					return None
		def GetMoney(self):		
				if self.wndMoney:
					return self.wndMoney
				else:
					return None
		def GetMoneyIcon(self):
				if self.wndMoneyIcon:
					return self.wndMoneyIcon
				else:
					return None
				
		def GetGem(self):
			if self.wndGem:
				return self.wndGem
			else:
				return None

		def GetGemSlot(self):
			if self.wndGemSlot:
				return self.wndGemSlot
			else:
				return None
	
		def GetGemIcon(self):
			if self.wndGemIcon:
				return self.wndGemIcon
			else:
				return None

		def Show(self):
			self.SetTop()
			ui.ScriptWindow.Show(self)

		def Close(self):
			self.Hide()

		def OverInToolTip(self, arg) :	
			arglen = len(str(arg))
			pos_x, pos_y = wndMgr.GetMousePosition()
			
			self.toolTip.ClearToolTip()
			self.toolTip.SetThinBoardSize(11 * arglen)
			self.toolTip.SetToolTipPosition(pos_x + 5, pos_y - 5)
			self.toolTip.AppendTextLine(arg, 0xffffff00)
			self.toolTip.Show()
		
		def OverOutToolTip(self) :
			self.toolTip.Hide()
		
		def EventProgress(self, event_type, idx) :
			if "mouse_over_in" == str(event_type) :
				if idx == 0 :
					self.OverInToolTip(localeInfo.CHEQUE_SYSTEM_UNIT_YANG)
				elif idx == 1 :
					self.OverInToolTip(localeInfo.CHEQUE_SYSTEM_UNIT_GEM)
				else:
					return 
			elif "mouse_over_out" == str(event_type) :
				self.OverOutToolTip()
			else:
				return

		def OnPressEscapeKey(self):
			self.Close()
			return True

class TaskBar(ui.ScriptWindow):

	BUTTON_CHARACTER = 0
	BUTTON_INVENTORY = 1
	BUTTON_MESSENGER = 2
	BUTTON_SYSTEM = 3
	BUTTON_CHAT = 4
	BUTTON_EXPAND = 4
	if app.ENABLE_GEM_SYSTEM:
		BUTTON_EXPAND_MONEY = 5
	if app.ENABLE_ANTI_MULTIPLE_FARM:
		BUTTON_ANTI_MULTIPLE_FARM = 6
				
	IS_EXPANDED = False

	MOUSE_BUTTON_LEFT = 0
	MOUSE_BUTTON_RIGHT = 1
	NONE = 255

	EVENT_MOVE = 0
	EVENT_ATTACK = 1
	EVENT_MOVE_AND_ATTACK = 2
	EVENT_CAMERA = 3
	EVENT_SKILL = 4
	EVENT_AUTO = 5
	EVENT_QUEUE_ON = 6

	GAUGE_WIDTH = 95
	GAUGE_HEIGHT = 13

	QUICKPAGE_NUMBER_FILENAME = [
		"d:/ymir work/ui/game/taskbar/1.sub",
		"d:/ymir work/ui/game/taskbar/2.sub",
		"d:/ymir work/ui/game/taskbar/3.sub",
		"d:/ymir work/ui/game/taskbar/4.sub",
	]

	def ShowGift(self):
		self.wndGiftBox.Show()
	
	def HideGift(self):
		self.wndGiftBox.Hide()

	class TextToolTip(ui.Window):
		def __init__(self):
			ui.Window.__init__(self, "TOP_MOST")

			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignCenter()
			textLine.SetOutline()
			textLine.Show()
			self.textLine = textLine

		def __del__(self):
			ui.Window.__del__(self)

		def SetText(self, text):
			self.textLine.SetText(text)

		def OnRender(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			self.textLine.SetPosition(mouseX, mouseY - 15)

	class SkillButton(ui.SlotWindow):

		def __init__(self):
			ui.SlotWindow.__init__(self)

			self.event = 0
			self.arg = 0

			self.slotIndex = 0
			self.skillIndex = 0

			slotIndex = 0
			wndMgr.SetSlotBaseImage(self.hWnd, "d:/ymir work/ui/public/slot_base.sub", 1.0, 1.0, 1.0, 1.0)
			wndMgr.AppendSlot(self.hWnd, slotIndex, 0, 0, 32, 32)
			self.SetCoverButton(slotIndex,	"d:/ymir work/ui/public/slot_cover_button_01.sub",\
											"d:/ymir work/ui/public/slot_cover_button_02.sub",\
											"d:/ymir work/ui/public/slot_cover_button_03.sub",\
											"d:/ymir work/ui/public/slot_cover_button_04.sub", True, False)
			self.SetSize(32, 32)

		def __del__(self):
			ui.SlotWindow.__del__(self)

		def Destroy(self):
			if 0 != self.tooltipSkill:
				self.tooltipSkill.HideToolTip()

		def RefreshSkill(self):
			if 0 != self.slotIndex:
				self.SetSkill(self.slotIndex)

		def SetSkillToolTip(self, tooltip):
			self.tooltipSkill = tooltip

		def SetSkill(self, skillSlotNumber):
			slotNumber = 0
			skillIndex = player.GetSkillIndex(skillSlotNumber)
			skillGrade = player.GetSkillGrade(skillSlotNumber)
			skillLevel = player.GetSkillLevel(skillSlotNumber)
			skillType = skill.GetSkillType(skillIndex)

			self.skillIndex = skillIndex
			if 0 == self.skillIndex:
				self.ClearSlot(slotNumber)
				return

			self.slotIndex = skillSlotNumber

			self.SetSkillSlotNew(slotNumber, skillIndex, skillGrade, skillLevel)
			self.SetSlotCountNew(slotNumber, skillGrade, skillLevel)

			if player.IsSkillCoolTime(skillSlotNumber):
				(coolTime, elapsedTime) = player.GetSkillCoolTime(skillSlotNumber)
				self.SetSlotCoolTime(slotNumber, coolTime, elapsedTime)

			if player.IsSkillActive(skillSlotNumber):
				self.ActivateSlot(slotNumber)

		def SetSkillEvent(self, event, arg=0):
			self.event = event
			self.arg = arg

		def GetSkillIndex(self):
			return self.skillIndex

		def GetSlotIndex(self):
			return self.slotIndex

		def Activate(self, coolTime):
			self.SetSlotCoolTime(0, coolTime)

			if skill.IsToggleSkill(self.skillIndex):
				self.ActivateSlot(0)

		def Deactivate(self):
			if skill.IsToggleSkill(self.skillIndex):
				self.DeactivateSlot(0)

		def OnOverInItem(self, dummy):
			self.tooltipSkill.SetSkill(self.skillIndex)

		def OnOverOutItem(self):
			self.tooltipSkill.HideToolTip()

		def OnSelectItemSlot(self, dummy):
			if 0 != self.event:
				if 0 != self.arg:
					self.event(self.arg)
				else:
					self.event()

	def __init__(self):
		#print "NEW TASKBAR  ----------------------------------------------------------------------------"

		ui.ScriptWindow.__init__(self, "TOP_MOST")

		self.quickPageNumImageBox = None
		self.tooltipItem = 0
		self.tooltipSkill = 0
		self.mouseModeButtonList = [ ui.ScriptWindow("TOP_MOST"), ui.ScriptWindow("TOP_MOST") ]

		self.tooltipHP = self.TextToolTip()
		self.tooltipHP.Show()
		self.tooltipSP = self.TextToolTip()
		self.tooltipSP.Show()
		self.tooltipST = self.TextToolTip()
		self.tooltipST.Show()
		self.tooltipEXP = self.TextToolTip()
		self.tooltipEXP.Show()
		
		if app.ENABLE_ANTI_MULTIPLE_FARM:
			self.anti_farm_info = None
			
		self.skillCategoryNameList = [ "ACTIVE_1", "ACTIVE_2", "ACTIVE_3" ]
		self.skillPageStartSlotIndexDict = {
			"ACTIVE_1" : 1, 
			"ACTIVE_2" : 21, 
			"ACTIVE_3" : 41, 
		}

		self.selectSkillButtonList = []
		
		self.lastUpdateQuickSlot = 0
		self.guildleaderbtn = None
		self.itemDropQuestionDialog = None
		self.interface = None
		self.SetWindowName("TaskBar")

	def __del__(self):
		#print "---------------------------------------------------------------------------- DELETE TASKBAR"
		ui.ScriptWindow.__del__(self)

	def BindInterface(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/TaskBar.py")
			pyScrLoader.LoadScriptFile(self.mouseModeButtonList[self.MOUSE_BUTTON_LEFT], "UIScript/MouseButtonWindow.py")
			pyScrLoader.LoadScriptFile(self.mouseModeButtonList[self.MOUSE_BUTTON_RIGHT], "UIScript/RightMouseButtonWindow.py")
		except:
			import exception
			exception.Abort("TaskBar.LoadWindow.LoadObject")

		self.quickslot = []
		self.quickslot.append(self.GetChild("quick_slot_1"))
		self.quickslot.append(self.GetChild("quick_slot_2"))
		for slot in self.quickslot:
			slot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			slot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptyQuickSlot))
			slot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemQuickSlot))
			slot.SetUnselectItemSlotEvent(ui.__mem_func__(self.UnselectItemQuickSlot))
			slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		toggleButtonDict = {}
		toggleButtonDict[TaskBar.BUTTON_CHARACTER]=self.GetChild("CharacterButton")
		toggleButtonDict[TaskBar.BUTTON_INVENTORY]=self.GetChild("InventoryButton")
		toggleButtonDict[TaskBar.BUTTON_MESSENGER]=self.GetChild("MessengerButton")
		toggleButtonDict[TaskBar.BUTTON_SYSTEM]=self.GetChild("SystemButton")

		if app.ENABLE_ANTI_MULTIPLE_FARM:
			pos_x, pos_y = (tuple(x-y for x,y in zip(toggleButtonDict[TaskBar.BUTTON_CHARACTER].GetLocalPosition(), [toggleButtonDict[TaskBar.BUTTON_CHARACTER].GetWidth() + 2, 0])))
			tmpButton = ui.MakeButton(self, pos_x - 38, pos_y, "", "d:/ymir work/ui/anti_multiple_farm/", "anti_multiple_farm_bt.tga", "anti_multiple_farm_bt.tga", "anti_multiple_farm_bt.tga")
			toggleButtonDict[TaskBar.BUTTON_ANTI_MULTIPLE_FARM] = tmpButton
			self.anti_farm_info = ui.MakeImageBox(toggleButtonDict[TaskBar.BUTTON_ANTI_MULTIPLE_FARM],\
					"d:/ymir work/ui/pattern/visible_mark_01.tga", 0, 0)

		pos_x, pos_y = (tuple(x-y for x,y in zip(toggleButtonDict[TaskBar.BUTTON_CHARACTER].GetLocalPosition(), [toggleButtonDict[TaskBar.BUTTON_CHARACTER].GetWidth() + 2, 0])))
		tmpButton = ui.MakeButton(self, pos_x - 38 - 38, pos_y, "Guild meeting", "d:/ymir work/ui/anti_multiple_farm/", "krone1.png", "krone2.png", "krone3.png")
		self.guildleaderbtn = tmpButton
		self.guildleaderbtn.SetEvent(ui.__mem_func__(self.OpenTeleportLeader))
	
		try:
			toggleButtonDict[TaskBar.BUTTON_CHAT]=self.GetChild("ChatButton")
		except:
			toggleButtonDict[TaskBar.BUTTON_EXPAND]=self.GetChild("ExpandButton")
			TaskBar.IS_EXPANDED = True

		if app.ENABLE_GEM_SYSTEM:
			toggleButtonDict[TaskBar.BUTTON_EXPAND_MONEY]=self.GetChild("ExpandMoneyButton")

		self.quickPageNumImageBox=self.GetChild("QuickPageNumber")

		self.GetChild("QuickPageUpButton").SetEvent(ui.__mem_func__(self.__OnClickQuickPageUpButton))
		self.GetChild("QuickPageDownButton").SetEvent(ui.__mem_func__(self.__OnClickQuickPageDownButton))

		mouseLeftButtonModeButton = self.GetChild("LeftMouseButton")
		mouseRightButtonModeButton = self.GetChild("RightMouseButton")
		mouseLeftButtonModeButton.SetEvent(ui.__mem_func__(self.ToggleLeftMouseButtonModeWindow))		
		mouseRightButtonModeButton.SetEvent(ui.__mem_func__(self.ToggleRightMouseButtonModeWindow))
		self.curMouseModeButton = [ mouseLeftButtonModeButton, mouseRightButtonModeButton ]

		(xLocalRight, yLocalRight) = mouseRightButtonModeButton.GetLocalPosition()
		self.curSkillButton = self.SkillButton()
		self.curSkillButton.SetParent(self)
		self.curSkillButton.SetPosition(xLocalRight, 3)
		self.curSkillButton.SetSkillEvent(ui.__mem_func__(self.ToggleRightMouseButtonModeWindow))
		self.curSkillButton.Hide()

		(xLeft, yLeft) = mouseLeftButtonModeButton.GetGlobalPosition()
		(xRight, yRight) = mouseRightButtonModeButton.GetGlobalPosition()
		leftModeButtonList = self.mouseModeButtonList[self.MOUSE_BUTTON_LEFT]
		leftModeButtonList.SetPosition(xLeft, yLeft - leftModeButtonList.GetHeight()-5)
		rightModeButtonList = self.mouseModeButtonList[self.MOUSE_BUTTON_RIGHT]
		rightModeButtonList.SetPosition(xRight - rightModeButtonList.GetWidth() + 32, yRight - rightModeButtonList.GetHeight()-5)
		rightModeButtonList.GetChild("button_skill").SetEvent(lambda adir=self.MOUSE_BUTTON_RIGHT, aevent=self.EVENT_SKILL: self.SelectMouseButtonEvent(adir, aevent))
		rightModeButtonList.GetChild("button_skill").Hide()

		mouseImage = ui.ImageBox("TOP_MOST")
		mouseImage.AddFlag("float")
		mouseImage.LoadImage("d:/ymir work/ui/ingame/camera_0.png")
		mouseImage.SetPosition(xRight, wndMgr.GetScreenHeight() - 34)
		mouseImage.Hide()
		self.mouseImage = mouseImage

		dir = self.MOUSE_BUTTON_LEFT
		wnd = self.mouseModeButtonList[dir]
		wnd.GetChild("button_move_and_attack").SetEvent(lambda adir=dir, aevent=self.EVENT_MOVE_AND_ATTACK: self.SelectMouseButtonEvent(adir, aevent))
		wnd.GetChild("button_auto_attack").SetEvent(lambda adir=dir, aevent=self.EVENT_AUTO: self.SelectMouseButtonEvent(adir, aevent))
		wnd.GetChild("button_camera").SetEvent(lambda adir=dir, aevent=self.EVENT_CAMERA: self.SelectMouseButtonEvent(adir, aevent))
		wnd.GetChild("button_queue_on").SetEvent(lambda adir = dir, aevent = self.EVENT_QUEUE_ON: self.SelectMouseButtonEvent(adir, aevent))

		dir = self.MOUSE_BUTTON_RIGHT
		wnd = self.mouseModeButtonList[dir]
		wnd.GetChild("button_move_and_attack").SetEvent(lambda adir=dir, aevent=self.EVENT_MOVE_AND_ATTACK: self.SelectMouseButtonEvent(adir, aevent))
		wnd.GetChild("button_camera").SetEvent(lambda adir=dir, aevent=self.EVENT_CAMERA: self.SelectMouseButtonEvent(adir, aevent))

		self.toggleButtonDict = toggleButtonDict

		if app.ENABLE_ITEMSHOP:
			self.rampageGauge1 = self.GetChild("RampageGauge")
			# self.rampageGauge2 = self.GetChild("RampageGauge2")
			self.rampageButton = self.GetChild("RampageButton")

			self.rampageButton.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.__RampageGauge_Click))


		self.hpGauge = self.GetChild("HPGauge")
		self.mpGauge = self.GetChild("SPGauge")
		self.stGauge = self.GetChild("STGauge")
		self.hpRecoveryGaugeBar = self.GetChild("HPRecoveryGaugeBar")
		self.spRecoveryGaugeBar = self.GetChild("SPRecoveryGaugeBar")

		self.hpGaugeBoard=self.GetChild("HPGauge_Board")
		self.mpGaugeBoard=self.GetChild("SPGauge_Board")
		self.stGaugeBoard=self.GetChild("STGauge_Board")

		self.expGauge = []
		for i in xrange(4):
			self.expGauge.append(self.GetChild("EXPGauge_%02d" % (i + 1)))

		self.expGaugeBoard = self.GetChild("EXP_Gauge_Board")

		#giftbox object
		wndGiftBox = GiftBox()
		wndGiftBox.LoadWindow()
		self.wndGiftBox = wndGiftBox
	
		self.__LoadMouseSettings()
		self.RefreshStatus()
		self.RefreshQuickSlot()

	def OpenTeleportLeader(self):
		import uiCommon
		itemDropQuestionDialog = uiCommon.QuestionDialog()
		itemDropQuestionDialog.SetText("Do you want to teleport your guild members?")
		itemDropQuestionDialog.SetAcceptEvent(lambda arg=True: self.RequestWarpGuildLeader(arg))
		itemDropQuestionDialog.SetCancelEvent(lambda arg=False: self.RequestWarpGuildLeader(arg))
		itemDropQuestionDialog.Open()
		self.itemDropQuestionDialog = itemDropQuestionDialog

	def RequestWarpGuildLeader(self, answer):
		if not self.itemDropQuestionDialog:
			return

		if answer:
			net.SendChatPacket("/teleport_guild")
					
		self.itemDropQuestionDialog.Close()
		self.itemDropQuestionDialog = None

	def __RampageGauge_Click(self):
		if self.interface and app.ENABLE_ITEMSHOP:
			self.interface.OpenItemshop()

		if self.wndGiftBox:
			self.wndGiftBox.Hide()

	def __LoadMouseSettings(self):
		try:
			LoadMouseButtonSettings()
			(mouseLeftButtonEvent, mouseRightButtonEvent) = GetMouseButtonSettings()
			if not self.__IsInSafeMouseButtonSettingRange(mouseLeftButtonEvent) or not self.__IsInSafeMouseButtonSettingRange(mouseRightButtonEvent):
					raise RuntimeError, "INVALID_MOUSE_BUTTON_SETTINGS"
		except:
			InitMouseButtonSettings(self.EVENT_MOVE_AND_ATTACK, self.EVENT_CAMERA)
			(mouseLeftButtonEvent, mouseRightButtonEvent) = GetMouseButtonSettings()

		try:
			self.SelectMouseButtonEvent(self.MOUSE_BUTTON_LEFT,	mouseLeftButtonEvent)
			self.SelectMouseButtonEvent(self.MOUSE_BUTTON_RIGHT,	mouseRightButtonEvent)
		except:
			InitMouseButtonSettings(self.EVENT_MOVE_AND_ATTACK, self.EVENT_CAMERA)
			(mouseLeftButtonEvent, mouseRightButtonEvent) = GetMouseButtonSettings()

			self.SelectMouseButtonEvent(self.MOUSE_BUTTON_LEFT,	mouseLeftButtonEvent)
			self.SelectMouseButtonEvent(self.MOUSE_BUTTON_RIGHT,	mouseRightButtonEvent)

	def __IsInSafeMouseButtonSettingRange(self, arg):
		# return arg >= self.EVENT_MOVE and arg <= self.EVENT_AUTO
		return arg >= self.EVENT_MOVE and arg <= self.EVENT_QUEUE_ON

	def Destroy(self):		
		SaveMouseButtonSettings()

		self.ClearDictionary()
		self.mouseModeButtonList[0].ClearDictionary()
		self.mouseModeButtonList[1].ClearDictionary()
		self.mouseModeButtonList = None
		self.curMouseModeButton = None
		self.curSkillButton = None
		self.selectSkillButtonList = []
		self.guildleaderbtn = None
		self.itemDropQuestionDialog = None

		self.expGauge = None
		self.hpGauge = None
		self.mpGauge = None
		self.stGauge = None
		self.hpRecoveryGaugeBar = None
		self.spRecoveryGaugeBar = None
		self.expGauge = None
		
		self.tooltipItem = None
		self.tooltipSkill = None
		self.quickslot = None
		self.toggleButtonDict = None

		self.hpGaugeBoard = None
		self.mpGaugeBoard = None
		self.stGaugeBoard = None
		
		self.expGaugeBoard = None

		self.tooltipHP = None
		self.tooltipSP = None
		self.tooltipST = None
		self.tooltipEXP = None

		self.mouseImage = None

	def __OnClickQuickPageUpButton(self):
		player.SetQuickPage(player.GetQuickPage()-1)

	def __OnClickQuickPageDownButton(self):
		player.SetQuickPage(player.GetQuickPage()+1)

	def SetToggleButtonEvent(self, eButton, kEventFunc):
		self.toggleButtonDict[eButton].SetEvent(kEventFunc)

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SetSkillToolTip(self, tooltipSkill):
		self.tooltipSkill = tooltipSkill
		self.curSkillButton.SetSkillToolTip(self.tooltipSkill)

	## Mouse Image
	def ShowMouseImage(self):
		self.mouseImage.SetTop()
		self.mouseImage.Show()

	def HideMouseImage(self):
		player.SetQuickCameraMode(False)
		self.mouseImage.Hide()

	## Gauge
	def RefreshStatus(self):
		curHP = player.GetStatus(player.HP)
		maxHP = player.GetStatus(player.MAX_HP)
		curSP = player.GetStatus(player.SP)
		maxSP = player.GetStatus(player.MAX_SP)
		curEXP = unsigned32(player.GetStatus(player.EXP))
		nextEXP = unsigned32(player.GetStatus(player.NEXT_EXP))
		recoveryHP = player.GetStatus(player.HP_RECOVERY)
		recoverySP = player.GetStatus(player.SP_RECOVERY)
		
		self.RefreshStamina()

		self.SetHP(curHP, recoveryHP, maxHP)
		self.SetSP(curSP, recoverySP, maxSP)
		self.SetExperience(curEXP, nextEXP)

		if self.guildleaderbtn:
			mainCharacterName = player.GetMainCharacterName()
			masterName = guild.GetGuildMasterName()
			
			if mainCharacterName == masterName:
				self.guildleaderbtn.Show()
			else:
				self.guildleaderbtn.Hide()
		
	def RefreshStamina(self):
		curST = player.GetStatus(player.STAMINA)
		maxST = player.GetStatus(player.MAX_STAMINA)
		self.SetST(curST, maxST)

	def RefreshSkill(self):
		self.curSkillButton.RefreshSkill()
		for button in self.selectSkillButtonList:
			button.RefreshSkill()

	if app.ENABLE_SKILL_COOLTIME_UPDATE:
		def SkillClearCoolTime(self, usedSlotIndex):
			QUICK_SLOT_SLOT_COUNT = 4
			slotIndex = 0
			for slotWindow in self.quickslot:
				for i in xrange(QUICK_SLOT_SLOT_COUNT):
					(Type, Position) = player.GetLocalQuickSlot(slotIndex)
					if Type == player.SLOT_TYPE_SKILL:
						if usedSlotIndex == Position:
							slotWindow.SetSlotCoolTime(slotIndex, 0)
							return
					slotIndex += 1

	def SetHP(self, curPoint, recoveryPoint, maxPoint):
		curPoint = min(curPoint, maxPoint)
		if maxPoint > 0:
			self.hpGauge.SetPercentage(curPoint, maxPoint)
			self.tooltipHP.SetText("%s : %s / %s" % (localeInfo.TASKBAR_HP, localeInfo.DottedNumber(curPoint), localeInfo.DottedNumber(maxPoint)))

			if 0 == recoveryPoint:
				self.hpRecoveryGaugeBar.Hide()
			else:
				destPoint = min(maxPoint, curPoint + recoveryPoint)
				newWidth = int(self.GAUGE_WIDTH * (float(destPoint) / float(maxPoint)))
				self.hpRecoveryGaugeBar.SetSize(newWidth, self.GAUGE_HEIGHT)
				self.hpRecoveryGaugeBar.Show()

	def SetSP(self, curPoint, recoveryPoint, maxPoint):
		curPoint = min(curPoint, maxPoint)
		if maxPoint > 0:
			self.mpGauge.SetPercentage(curPoint, maxPoint)
			self.tooltipSP.SetText("%s : %s / %s" % (localeInfo.TASKBAR_SP, localeInfo.DottedNumber(curPoint), localeInfo.DottedNumber(maxPoint)))

			if 0 == recoveryPoint:
				self.spRecoveryGaugeBar.Hide()
			else:
				destPoint = min(maxPoint, curPoint + recoveryPoint)
				newWidth = int(self.GAUGE_WIDTH * (float(destPoint) / float(maxPoint)))
				self.spRecoveryGaugeBar.SetSize(newWidth, self.GAUGE_HEIGHT)
				self.spRecoveryGaugeBar.Show()

	def SetST(self, curPoint, maxPoint):
		curPoint = min(curPoint, maxPoint)
		if maxPoint > 0:
			self.stGauge.SetPercentage(curPoint, maxPoint)
			self.tooltipST.SetText("%s : %d / %d" % (localeInfo.TASKBAR_ST, curPoint, maxPoint))


	def SetExperience(self, curPoint, maxPoint):

		curPoint = min(curPoint, maxPoint)
		curPoint = max(curPoint, 0)
		maxPoint = max(maxPoint, 0)

		quarterPoint = maxPoint / 4
		FullCount = 0

		if 0 != quarterPoint:
			FullCount = min(4, curPoint / quarterPoint)

		for i in xrange(4):
			self.expGauge[i].Hide()

		for i in xrange(FullCount):
			self.expGauge[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.expGauge[i].Show()

		if 0 != quarterPoint:
			if FullCount < 4:
				Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
				self.expGauge[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.expGauge[FullCount].Show()

		###
		self.tooltipEXP.SetText("%s : %.2f%%" % (localeInfo.TASKBAR_EXP, float(curPoint) / max(1, float(maxPoint)) * 100))
	
		
	## QuickSlot
	def RefreshQuickSlot(self):

		pageNum = player.GetQuickPage()

		try:
			self.quickPageNumImageBox.LoadImage(TaskBar.QUICKPAGE_NUMBER_FILENAME[pageNum])
		except:
			pass

		startNumber = 0
		for slot in self.quickslot:

			for i in xrange(4):

				slotNumber = i+startNumber

				(Type, Position) = player.GetLocalQuickSlot(slotNumber)

				if player.SLOT_TYPE_NONE == Type:
					slot.ClearSlot(slotNumber)
					continue

				if player.SLOT_TYPE_INVENTORY == Type:

					itemIndex = player.GetItemIndex(Position)
					itemCount = player.GetItemCount(Position)
					if itemCount <= 1:
						itemCount = 0
					
					if constInfo.IS_AUTO_POTION(itemIndex):
						metinSocket = [player.GetItemMetinSocket(Position, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
						
						if 0 != int(metinSocket[0]):
							slot.ActivateSlot(slotNumber)
						else:
							slot.DeactivateSlot(slotNumber)
					
					slot.SetItemSlot(slotNumber, itemIndex, itemCount)

				elif player.SLOT_TYPE_SKILL == Type:

					skillIndex = player.GetSkillIndex(Position)
					if 0 == skillIndex:
						slot.ClearSlot(slotNumber)
						continue

					skillType = skill.GetSkillType(skillIndex)
					if skill.SKILL_TYPE_GUILD == skillType:
						import guild
						skillGrade = 0
						skillLevel = guild.GetSkillLevel(Position)

					else:
						skillGrade = player.GetSkillGrade(Position)
						skillLevel = player.GetSkillLevel(Position)

					slot.SetSkillSlotNew(slotNumber, skillIndex, skillGrade, skillLevel)
					slot.SetSlotCountNew(slotNumber, skillGrade, skillLevel)
					slot.SetCoverButton(slotNumber)

					if player.IsSkillCoolTime(Position):
						(coolTime, elapsedTime) = player.GetSkillCoolTime(Position)
						slot.SetSlotCoolTime(slotNumber, coolTime, elapsedTime)

					if player.IsSkillActive(Position):
						slot.ActivateSlot(slotNumber)

				elif player.SLOT_TYPE_EMOTION == Type:

					emotionIndex = Position
					slot.SetEmotionSlot(slotNumber, emotionIndex)
					slot.SetCoverButton(slotNumber)
					slot.SetSlotCount(slotNumber, 0)

			slot.RefreshSlot()
			startNumber += 4

	def canAddQuickSlot(self, Type, slotNumber):

		if player.SLOT_TYPE_INVENTORY == Type:

			itemIndex = player.GetItemIndex(slotNumber)
			return item.CanAddToQuickSlotItem(itemIndex)

		return True

	def AddQuickSlot(self, localSlotIndex):
		AttachedSlotType = mouseModule.mouseController.GetAttachedType()
		AttachedSlotNumber = mouseModule.mouseController.GetAttachedSlotNumber()
		AttachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

		if player.SLOT_TYPE_QUICK_SLOT == AttachedSlotType:
			player.RequestMoveGlobalQuickSlotToLocalQuickSlot(AttachedSlotNumber, localSlotIndex)

		elif player.SLOT_TYPE_EMOTION == AttachedSlotType:

			player.RequestAddLocalQuickSlot(localSlotIndex, AttachedSlotType, AttachedItemIndex)

		elif True == self.canAddQuickSlot(AttachedSlotType, AttachedSlotNumber):

			## Online Code
			player.RequestAddLocalQuickSlot(localSlotIndex, AttachedSlotType, AttachedSlotNumber)
		
		mouseModule.mouseController.DeattachObject()
		self.RefreshQuickSlot()

	def SelectEmptyQuickSlot(self, slotIndex):

		if True == mouseModule.mouseController.isAttached():
			self.AddQuickSlot(slotIndex)

	def SelectItemQuickSlot(self, localQuickSlotIndex):

		if True == mouseModule.mouseController.isAttached():
			self.AddQuickSlot(localQuickSlotIndex)

		else:
			globalQuickSlotIndex=player.LocalQuickSlotIndexToGlobalQuickSlotIndex(localQuickSlotIndex)
			mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_QUICK_SLOT, globalQuickSlotIndex, globalQuickSlotIndex)

	def UnselectItemQuickSlot(self, localSlotIndex):

		if False == mouseModule.mouseController.isAttached():
			player.RequestUseLocalQuickSlot(localSlotIndex)
			return

		elif mouseModule.mouseController.isAttached():
			mouseModule.mouseController.DeattachObject()
			return


	def OnUseSkill(self, usedSlotIndex, coolTime):

		QUICK_SLOT_SLOT_COUNT = 4
		slotIndex = 0

		## Current Skill Button
		if usedSlotIndex == self.curSkillButton.GetSlotIndex():
			self.curSkillButton.Activate(coolTime)

		## Quick Slot
		for slotWindow in self.quickslot:

			for i in xrange(QUICK_SLOT_SLOT_COUNT):

				(Type, Position) = player.GetLocalQuickSlot(slotIndex)

				if Type == player.SLOT_TYPE_SKILL:
					if usedSlotIndex == Position:
						slotWindow.SetSlotCoolTime(slotIndex, coolTime)
						return

				slotIndex += 1

	def OnActivateSkill(self, usedSlotIndex):
		slotIndex = 0

		## Current Skill Button
		if usedSlotIndex == self.curSkillButton.GetSlotIndex():
			self.curSkillButton.Deactivate()

		## Quick Slot
		for slotWindow in self.quickslot:

			for i in xrange(4):

				(Type, Position) = player.GetLocalQuickSlot(slotIndex)

				if Type == player.SLOT_TYPE_SKILL:
					if usedSlotIndex == Position:
						slotWindow.ActivateSlot(slotIndex)
						return

				slotIndex += 1

	def OnDeactivateSkill(self, usedSlotIndex):
		slotIndex = 0

		## Current Skill Button
		if usedSlotIndex == self.curSkillButton.GetSlotIndex():
			self.curSkillButton.Deactivate()

		## Quick Slot
		for slotWindow in self.quickslot:

			for i in xrange(4):

				(Type, Position) = player.GetLocalQuickSlot(slotIndex)

				if Type == player.SLOT_TYPE_SKILL:
					if usedSlotIndex == Position:
						slotWindow.DeactivateSlot(slotIndex)
						return

				slotIndex += 1

	def OverInItem(self, slotNumber):
		if mouseModule.mouseController.isAttached():
			return

		(Type, Position) = player.GetLocalQuickSlot(slotNumber)

		if player.SLOT_TYPE_INVENTORY == Type:
			self.tooltipItem.SetInventoryItem(Position)
			self.tooltipSkill.HideToolTip()

		elif player.SLOT_TYPE_SKILL == Type:

			skillIndex = player.GetSkillIndex(Position)
			skillType = skill.GetSkillType(skillIndex)

			if skill.SKILL_TYPE_GUILD == skillType:
				import guild
				skillGrade = 0
				skillLevel = guild.GetSkillLevel(Position)

			else:
				skillGrade = player.GetSkillGrade(Position)
				skillLevel = player.GetSkillLevel(Position)

			self.tooltipSkill.SetSkillNew(Position, skillIndex, skillGrade, skillLevel)
			self.tooltipItem.HideToolTip()

	def OverOutItem(self):
		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()
		if 0 != self.tooltipSkill:
			self.tooltipSkill.HideToolTip()

	def __IsInWindow(self, wnd):
		if not wnd:
			return False

		try:
			return True == wnd.IsIn()
		except:
			return False

	def __ShowWindow(self, wnd):
		if not wnd:
			return

		try:
			wnd.Show()
		except:
			pass

	def __HideWindow(self, wnd):
		if not wnd:
			return

		try:
			wnd.Hide()
		except:
			pass

	def OnUpdate(self):
		if app.GetGlobalTime() - self.lastUpdateQuickSlot > 500:
			self.lastUpdateQuickSlot = app.GetGlobalTime()
			self.RefreshQuickSlot()

		if self.__IsInWindow(self.hpGaugeBoard):
			self.__ShowWindow(self.tooltipHP)
		else:
			self.__HideWindow(self.tooltipHP)

		if self.__IsInWindow(self.mpGaugeBoard):
			self.__ShowWindow(self.tooltipSP)
		else:
			self.__HideWindow(self.tooltipSP)

		if self.__IsInWindow(self.stGaugeBoard):
			self.__ShowWindow(self.tooltipST)
		else:
			self.__HideWindow(self.tooltipST)
		
		if self.__IsInWindow(self.expGaugeBoard):
			self.__ShowWindow(self.tooltipEXP)
		else:
			self.__HideWindow(self.tooltipEXP)

	def ToggleLeftMouseButtonModeWindow(self):

		wndMouseButtonMode = self.mouseModeButtonList[self.MOUSE_BUTTON_LEFT]

		if True == wndMouseButtonMode.IsShow():

			wndMouseButtonMode.Hide()

		else:
			wndMouseButtonMode.Show()

	def ToggleRightMouseButtonModeWindow(self):

		wndMouseButtonMode = self.mouseModeButtonList[self.MOUSE_BUTTON_RIGHT]

		if True == wndMouseButtonMode.IsShow():

			wndMouseButtonMode.Hide()
			self.CloseSelectSkill()

		else:
			wndMouseButtonMode.Show()
			self.OpenSelectSkill()

	def OpenSelectSkill(self):

		PAGE_SLOT_COUNT = 6

		(xSkillButton, y) = self.curSkillButton.GetGlobalPosition()
		y -= (37 + 32 + 1)

		for key in self.skillCategoryNameList:

			appendCount = 0
			startNumber = self.skillPageStartSlotIndexDict[key]
			x = xSkillButton

			getSkillIndex=player.GetSkillIndex
			getSkillLevel=player.GetSkillLevel
			for i in xrange(PAGE_SLOT_COUNT):

				skillIndex = getSkillIndex(startNumber+i)
				skillLevel = getSkillLevel(startNumber+i)

				if 0 == skillIndex:
					continue
				if 0 == skillLevel:
					continue
				if skill.IsStandingSkill(skillIndex):
					continue

				skillButton = self.SkillButton()
				skillButton.SetSkill(startNumber+i)
				skillButton.SetPosition(x, y)
				skillButton.SetSkillEvent(ui.__mem_func__(self.CloseSelectSkill), startNumber+i+1)
				skillButton.SetSkillToolTip(self.tooltipSkill)
				skillButton.SetTop()
				skillButton.Show()
				self.selectSkillButtonList.append(skillButton)

				appendCount += 1
				x -= 32

			if appendCount > 0:
				y -= 32

	def CloseSelectSkill(self, slotIndex=-1):

		self.mouseModeButtonList[self.MOUSE_BUTTON_RIGHT].Hide()
		for button in self.selectSkillButtonList:
			button.Destroy()

		self.selectSkillButtonList = []

		if -1 != slotIndex:
			self.curSkillButton.Show()
			self.curMouseModeButton[self.MOUSE_BUTTON_RIGHT].Hide()
			player.SetMouseFunc(player.MBT_RIGHT, player.MBF_SKILL)
			player.ChangeCurrentSkillNumberOnly(slotIndex-1)
		else:
			self.curSkillButton.Hide()
			self.curMouseModeButton[self.MOUSE_BUTTON_RIGHT].Show()

	def SelectMouseButtonEvent(self, dir, event):
		SetMouseButtonSetting(dir, event)

		self.CloseSelectSkill()
		self.mouseModeButtonList[dir].Hide()

		btn = 0
		type = self.NONE
		func = self.NONE
		tooltip_text = ""		
		
		if self.MOUSE_BUTTON_LEFT == dir:
			type = player.MBT_LEFT

		elif self.MOUSE_BUTTON_RIGHT == dir:
			type = player.MBT_RIGHT

		if self.EVENT_MOVE == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_move")
			func = player.MBF_MOVE
			tooltip_text = localeInfo.TASKBAR_MOVE
		
		elif self.EVENT_ATTACK == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_attack")
			func = player.MBF_ATTACK
			tooltip_text = localeInfo.TASKBAR_ATTACK
		
		elif self.EVENT_AUTO == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_auto_attack")
		
			# Liste der Maps, auf denen Auto-Hit deaktiviert ist
			blocked_maps = ["metin2_map_a1", "metin2_map_b1", "metin2_map_c1", "metin2_map_worldboss1"]
		
			# Prüfe, ob der Spieler sich auf einer der blockierten Maps befindet
			if background.GetCurrentMapName() in blocked_maps:
				func = None  # Deaktiviert Auto-Hit auf diesen Maps
				tooltip_text = "Auto-Hit ist auf dieser Map deaktiviert."
				chat.AppendChat(1, "Auto-Hit ist auf dieser Map deaktiviert.")
			else:
				func = player.MBF_AUTO  # Auto-Hit funktioniert nur auf anderen Maps
				tooltip_text = localeInfo.TASKBAR_AUTO
		elif self.EVENT_MOVE_AND_ATTACK == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_move_and_attack")
			func = player.MBF_SMART
			tooltip_text = localeInfo.TASKBAR_ATTACK
		elif self.EVENT_CAMERA == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_camera")
			func = player.MBF_CAMERA
			tooltip_text = localeInfo.TASKBAR_CAMERA
		elif self.EVENT_SKILL == event:
			btn = self.mouseModeButtonList[dir].GetChild("button_skill")
			func = player.MBF_SKILL
			tooltip_text = localeInfo.TASKBAR_SKILL
		elif event == self.EVENT_QUEUE_ON:
			btn = self.mouseModeButtonList[dir].GetChild("button_queue_on")
			func = player.MBF_QUEUE_ON
			tooltip_text = "Metin Stone Queue"

		if 0 != btn:
			self.curMouseModeButton[dir].SetToolTipText(tooltip_text, 0, -18)
			self.curMouseModeButton[dir].SetUpVisual(btn.GetUpVisualFileName())
			self.curMouseModeButton[dir].SetOverVisual(btn.GetOverVisualFileName())
			self.curMouseModeButton[dir].SetDownVisual(btn.GetDownVisualFileName())
			self.curMouseModeButton[dir].Show()

		player.SetMouseFunc(type, func)

	def OnChangeCurrentSkill(self, skillSlotNumber):
		self.curSkillButton.SetSkill(skillSlotNumber)
		self.curSkillButton.Show()
		self.curMouseModeButton[self.MOUSE_BUTTON_RIGHT].Hide()

	if app.ENABLE_ANTI_MULTIPLE_FARM:
		def ReloadAntiMultipleFarmState(self):
			if not self.anti_farm_info:
				return

			playerState = anti_multiple_farm.GetAntiFarmMainPlayerStatus()
			self.anti_farm_info.LoadImage(MULTIPLE_FARM_STATE_IMAGES[playerState])
