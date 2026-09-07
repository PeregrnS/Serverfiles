import ui
import constInfo
import uiSelectMusic
import musicInfo
import localeInfo
import uiScriptLocale
import dbg
import player
import chat
import wndMgr
import net
import systemSetting
import snd
import pack
import app
import background
import grp
import textTail

MUSIC_FILENAME_MAX_LEN = 25
blockMode = 0
IMG_DIR = "d:/ymir work/ui/game/gameoption/"

class GameOptionWindow(ui.BoardWithFinalCoreTitleBar9):
	def __init__(self):
		ui.BoardWithFinalCoreTitleBar9.__init__(self)
		self.children = {}
		self.Destroy()
		self.__LoadWindow()
		self.SetTitleName(uiScriptLocale.GAMEOPTION_TITLE)

	def __del__(self):
		ui.BoardWithFinalCoreTitleBar9.__del__(self)

	def Destroy(self):
		self.ctrlFovValue = None
		self.pvpModeButtonDict={}
		self.blockButtonList=[]
		self.questLetterButtonList=[]
		self.viewChatButtonList = []
		self.countryFlagButtonList = []
		self.nameColorModeButtonList = []
		self.viewTargetBoardButtonList=[]
		self.alwaysShowNameButtonList=[]
		self.showDamageButtonList=[]
		self.showRainBowButtonList=[]
		self.showsalesTextButtonList=[]
		self.showtitleTextButtonList=[]
		self.cameraModeButtonList=[]
		self.fogModeButtonList = []
		# self.showOutlineButtonList = []
		# if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
		self.mainPickModeButtonList = []
		self.mainPickUPButtonList = []
		# self.rarityFilterButtonList = []
		# if app.ENABLE_SNOW_MODE:
		# self.showSnowButtonList=[]
		
		self.affectIconsButtonList=[]
		# self.shopRangeButtonList=[]
		self.effectButtonList=[]
		self.petButtonList=[]
		self.mountButtonList=[]
		self.shopNameButtonList=[]
		# self.nightModeButtonList=[]
		# self.inventoryChangeList = []
		# if app.ENABLE_DISCORD_STUFF:
		# self.wndNotificationButtonList=[]
		# if app.ENABLE_MAP_OBJECT_OPTIMIZATION:
		self.ctrlmapObjectDist=None

		# if app.ENABLE_HIDE_OBJECTS:
		# self.VisibleButtonList = []
		# self.mylist = [background.PART_TREE, background.PART_CLOUD]
		
		if app.WJ_SHOW_MOB_INFO:
			self.showMobInfoButtonList = []

		self.ctrlSoundVolume = None
		self.ctrlMusicVolume = None
		self.selectMusicFile = None
		self.musicListDlg = None

		for j in xrange(10):
			for x in xrange(10):
				if self.children.has_key("optionWindow%d%d"%(j,x)):
					window = self.children["optionWindow%d%d"%(j,x)]
					if window:
						window.Hide()
						window.scrollBar = None
						window.ClearDictionary()
						window.Destroy()
		self.children = {}
		self.Hide()

	def __LoadWindow(self):
		self.AddFlag("movable")
		self.AddFlag("attach")
		option_data = {
			"Game Options":{
				"index": 0,
				"General":{"index": 0,},
				"Pickup":{"index": 1,},
			},
			"Graphic Options":{
			"index": 1,
				"General":{"index": 0,},
				"Interface":{"index": 1,},
				"Performance":{"index": 2,},
			},
			"Sound Options":{
				"index": 2,
				"General":{"index": 0,},
			},
		}

		for key, optionTitles in option_data.items():
			if self.IsCanContinue(key):
				continue

			if not optionTitles.has_key("index"):
				continue
			optionIndex = optionTitles["index"]

			bg = ui.ImageBox()
			bg.SetParent(self)
			bg.AddFlag("attach")
			bg.SetPosition(8,65)
			bg.LoadImage(IMG_DIR+"bg.tga")
			bg.Show()
			self.children["bg%d"%optionIndex] = bg

			titleButton = ui.RadioButton()
			titleButton.SetParent(self)
			if optionIndex == 0:
				titleButton.SetUpVisual(IMG_DIR+"first_title_0.tga")
				titleButton.SetOverVisual(IMG_DIR+"first_title_1.tga")
				titleButton.SetDownVisual(IMG_DIR+"first_title_2.tga")
				titleButton.SetPosition(21+(optionIndex*(titleButton.GetWidth()+5)), 40)
			elif optionIndex == len(option_data)-1:
				titleButton.SetUpVisual(IMG_DIR+"end_title_0.tga")
				titleButton.SetOverVisual(IMG_DIR+"end_title_1.tga")
				titleButton.SetDownVisual(IMG_DIR+"end_title_2.tga")
				titleButton.SetPosition(21+(optionIndex*(titleButton.GetWidth()+5))-5, 40)
			else:
				titleButton.SetUpVisual(IMG_DIR+"middle_title_0.tga")
				titleButton.SetOverVisual(IMG_DIR+"middle_title_1.tga")
				titleButton.SetDownVisual(IMG_DIR+"middle_title_2.tga")
				titleButton.SetPosition(21+(optionIndex*(titleButton.GetWidth()+5)), 40)
			titleButton.SetText(key)
			titleButton.SAFE_SetEvent(self.SetOptionType, optionIndex)
			titleButton.Show()
			self.children["titleBtn%d"%optionIndex] = titleButton

			for keyEx, optionTypes in optionTitles.items():
				if self.IsCanContinue(keyEx):
					continue
				if not optionTypes.has_key("index"):
					continue

				categoryIndex = optionTypes["index"]
				optionCategory = ui.RadioButton()
				optionCategory.SetParent(bg)
				optionCategory.SetUpVisual(IMG_DIR+"category_0.tga")
				optionCategory.SetOverVisual(IMG_DIR+"category_1.tga")
				optionCategory.SetDownVisual(IMG_DIR+"category_2.tga")
				optionCategory.SetPosition(7, 10+(categoryIndex*(optionCategory.GetHeight()+5)))
				optionCategory.SAFE_SetEvent(self.SetCategoryType, categoryIndex)
				optionCategory.SetText(keyEx)
				optionCategory.Show()
				self.children["optionCategory%d%d"%(optionIndex,categoryIndex)] = optionCategory

				optionWindow = self.CreateOptionWindow(bg, optionIndex,categoryIndex)
				self.children["optionWindow%d%d"%(optionIndex,categoryIndex)] = optionWindow

		self.SetOptionType(0)
		self.SetSize(8+bg.GetWidth()+10,65+bg.GetHeight()+10)
		self.SetCenterPosition()
		self.UpdateScrollBar()

	def Open(self):
		self.RefreshBlock()
		# if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
		self.OnChangePickUPMode()
		self.Show()

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			btn=buttonList[buttonIndex]
		except IndexError:
			return
		for eachButton in buttonList:
			eachButton.SetUp()
		btn.Down()

	def IsCanContinue(self, dictKey):
		if dictKey == "index":
			return True
		return False

	def SetOptionType(self, optionType):
		self.children["optionType"] = optionType
		for j in xrange(10):
			if self.children.has_key("titleBtn%d"%j):
				titleBtn = self.children["titleBtn%d"%j]
				if j == optionType:
					titleBtn.Down()
				else:
					titleBtn.SetUp()
			if self.children.has_key("bg%d"%j):
				bg = self.children["bg%d"%j]
				if j == optionType:
					bg.Show()
				else:
					bg.Hide()
		self.SetCategoryType(0)

	def SetCategoryType(self, categoryType):
		for j in xrange(3):
			for x in xrange(5):
				if self.children.has_key("optionWindow%d%d" % (j,x)):
					self.children["optionWindow%d%d"%(j,x)].Hide()

		self.children["categoryType"] = categoryType
		currentCategoryType = self.children["optionType"]
		for j in xrange(10):
			if self.children.has_key("optionCategory%d%d"%(currentCategoryType,j)):
				categoryBtn = self.children["optionCategory%d%d"%(currentCategoryType,j)]
				if j == categoryType:
					categoryBtn.Down()
				else:
					categoryBtn.SetUp()

		if self.children.has_key("optionWindow%d%d" % (currentCategoryType,categoryType)):
			self.children["optionWindow%d%d"%(currentCategoryType,categoryType)].Show()

		self.UpdateScrollBar()

	def CreateOptionWindow(self, windowBG, optionIndex, categoryIndex):
		scriptWindow = ui.ScriptWindow()
		scriptWindow.SetParent(windowBG)
		fileName = "UIScript/game_option_%d_%d.py"%(optionIndex, categoryIndex)
		pyScrLoader = ui.PythonScriptLoader()
		if pack.Exist(fileName):
			pyScrLoader.LoadScriptFile(scriptWindow, fileName)
			self.LoadCatetoryOptions(scriptWindow, optionIndex, categoryIndex)
		else:
			dbg.TraceError("[GameOption]Can't find script window for optionIndex: %d categoryIndex: %d"%(optionIndex, categoryIndex))

		if scriptWindow.GetWidth() > 0:
			#Auto add scrollbar
			scrollBar = ScrollBarNew()
			scrollBar.SetParent(scriptWindow)
			scrollBar.SetPosition(scriptWindow.GetWidth()-scrollBar.GetWidth(),0)
			scrollBar.SetScrollBarSize(scriptWindow.GetHeight()-5)
			scrollBar.Hide()
			scrollBar.SetScrollEvent(ui.__mem_func__(self.UpdateScrollBar))
			scriptWindow.scrollBar = scrollBar
			# scriptWindow.OnMouseWheel = ui.__mem_func__(self.OnMouseWheel)

			scriptChildrens = scriptWindow.Children
			for child in scriptChildrens:
				(_x,_y) = child.GetLocalPosition()
				child.SetPosition(_x,_y, True)

			scriptWindow.Show()
		else:
			scriptWindow.scrollBar = None

		return scriptWindow

	def LoadCatetoryOptions(self, scriptWindow, optionIndex, categoryIndex):
		#try:
		GetObject = scriptWindow.GetChild

		if optionIndex == 0:#Game-Options

			if categoryIndex == 0:#General
				self.pvpModeButtonDict[player.PK_MODE_PEACE] = GetObject("pvp_peace")
				self.pvpModeButtonDict[player.PK_MODE_PEACE].SAFE_SetEvent(self.__OnClickPvPModePeaceButton)
				self.pvpModeButtonDict[player.PK_MODE_REVENGE] = GetObject("pvp_revenge")
				self.pvpModeButtonDict[player.PK_MODE_REVENGE].SAFE_SetEvent(self.__OnClickPvPModeRevengeButton)
				self.pvpModeButtonDict[player.PK_MODE_GUILD] = GetObject("pvp_guild")
				self.pvpModeButtonDict[player.PK_MODE_GUILD].SAFE_SetEvent(self.__OnClickPvPModeGuildButton)
				self.pvpModeButtonDict[player.PK_MODE_FREE] = GetObject("pvp_free")
				self.pvpModeButtonDict[player.PK_MODE_FREE].SAFE_SetEvent(self.__OnClickPvPModeFreeButton)
				self.pvpModeButtonDict[5] = GetObject("pvp_fear")
				self.pvpModeButtonDict[5].SAFE_SetEvent(self.__OnClickPvPModeFearButton)
				self.__SetPeacePKMode()

				block_exchange_button = GetObject("block_exchange_button")
				block_exchange_button.SetToggleUpEvent(self.__OnClickBlockExchangeButton)
				block_exchange_button.SetToggleDownEvent(self.__OnClickBlockExchangeButton)
				self.blockButtonList.append(block_exchange_button)

				block_party_button = GetObject("block_party_button")
				block_party_button.SetToggleUpEvent(self.__OnClickBlockPartyButton)
				block_party_button.SetToggleDownEvent(self.__OnClickBlockPartyButton)
				self.blockButtonList.append(block_party_button)

				block_guild_button = GetObject("block_guild_button")
				block_guild_button.SetToggleUpEvent(self.__OnClickBlockGuildButton)
				block_guild_button.SetToggleDownEvent(self.__OnClickBlockGuildButton)
				self.blockButtonList.append(block_guild_button)

				block_whisper_button = GetObject("block_whisper_button")
				block_whisper_button.SetToggleUpEvent(self.__OnClickBlockWhisperButton)
				block_whisper_button.SetToggleDownEvent(self.__OnClickBlockWhisperButton)
				self.blockButtonList.append(block_whisper_button)

				block_friend_button = GetObject("block_friend_button")
				block_friend_button.SetToggleUpEvent(self.__OnClickBlockFriendButton)
				block_friend_button.SetToggleDownEvent(self.__OnClickBlockFriendButton)
				self.blockButtonList.append(block_friend_button)

				block_party_request_button = GetObject("block_party_request_button")
				block_party_request_button.SetToggleUpEvent(self.__OnClickBlockPartyRequest)
				block_party_request_button.SetToggleDownEvent(self.__OnClickBlockPartyRequest)
				self.blockButtonList.append(block_party_request_button)

				block_equipment_view_request_button = GetObject("block_equipment_view_request_button")
				block_equipment_view_request_button.SetToggleUpEvent(self.__OnClickBlockViewEquip)
				block_equipment_view_request_button.SetToggleDownEvent(self.__OnClickBlockViewEquip)
				self.blockButtonList.append(block_equipment_view_request_button)

				showQuestLetterBtn = GetObject("show_letter")
				showQuestLetterBtn.SAFE_SetEvent(self.__OnClickQuestLetter)
				self.questLetterButtonList.append(showQuestLetterBtn)

				hideQuestLetterBtn = GetObject("hide_letter")
				hideQuestLetterBtn.SAFE_SetEvent(self.__OnClickQuestLetter)
				self.questLetterButtonList.append(hideQuestLetterBtn)
				self.RefreshQuestLetter()

				self.viewChatButtonList.append(GetObject("view_chat_on_button"))
				self.viewChatButtonList[0].SAFE_SetEvent(self.__OnClickViewChat)
				self.viewChatButtonList.append(GetObject("view_chat_off_button"))
				self.viewChatButtonList[1].SAFE_SetEvent(self.__OnClickViewChat)
				self.RefreshViewChat()

				self.countryFlagButtonList.append(GetObject("country_flag_on"))
				self.countryFlagButtonList[0].SAFE_SetEvent(self.__OnClickCountryFlag)
				self.countryFlagButtonList.append(GetObject("country_flag_off"))
				self.countryFlagButtonList[1].SAFE_SetEvent(self.__OnClickCountryFlag)
				self.RefreshCountryFlag()

				self.nameColorModeButtonList.append(GetObject("name_color_normal"))
				self.nameColorModeButtonList[0].SAFE_SetEvent(self.__OnClickNameColorMode)
				self.nameColorModeButtonList.append(GetObject("name_color_empire"))
				self.nameColorModeButtonList[1].SAFE_SetEvent(self.__OnClickNameColorMode)
				self.RefreshNameColor()

				self.viewTargetBoardButtonList.append(GetObject("target_board_view"))
				self.viewTargetBoardButtonList[0].SAFE_SetEvent(self.__SetTargetBoardViewMode)
				self.viewTargetBoardButtonList.append(GetObject("target_board_no_view"))
				self.viewTargetBoardButtonList[1].SAFE_SetEvent(self.__SetTargetBoardViewMode)
				self.RefreshTargetBoard()
				
				if app.WJ_SHOW_MOB_INFO:
					self.showMobInfoButtonList.append(GetObject("show_mob_level_button"))
					self.showMobInfoButtonList.append(GetObject("show_mob_AI_flag_button"))
					self.showMobInfoButtonList[0].SetToggleUpEvent(self.__OnClickShowMobLevelButton)
					self.showMobInfoButtonList[0].SetToggleDownEvent(self.__OnClickShowMobLevelButton)
					self.showMobInfoButtonList[1].SetToggleUpEvent(self.__OnClickShowMobAIFlagButton)
					self.showMobInfoButtonList[1].SetToggleDownEvent(self.__OnClickShowMobAIFlagButton)
					self.RefreshShowMobInfo()
				
			elif categoryIndex == 1:#Pickup
				# if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
				pickup_filter_child_names = ["pick_up_sword","pick_up_dagger","pick_up_bow","pick_up_two_handed","pick_up_bell","pick_up_fan",\
				"pick_up_arrow","pick_up_body","pick_up_head","pick_up_shield","pick_up_wrist",\
				"pick_up_foot","pick_up_neck","pick_up_ear","pick_up_metin","pick_up_yang","pick_up_skillbook","pick_up_chest"]
				i = 0
				for names in pickup_filter_child_names:
					objectPointer = GetObject(names)
					# objectPointer.SetToggleDownEvent(ui.__mem_func__(self.__OnClickPickupIgnoreButton), i)
					# objectPointer.SetToggleUpEvent(ui.__mem_func__(self.__OnClickPickupIgnoreButton), i)
					self.mainPickUPButtonList.append(objectPointer)
					i+=1

				for (index, button) in enumerate(self.mainPickUPButtonList):
					button.SetToggleDownEvent(ui.__mem_func__(self.__OnClickPickupIgnoreButton), index)
					button.SetToggleUpEvent(ui.__mem_func__(self.__OnClickPickupIgnoreButton), index)

				pick_up_button_single = GetObject("pick_up_button_single")
				pick_up_button_single.SAFE_SetEvent(self.__OnClickPickupModeButton, False)
				# pick_up_button_single.SetToggleDownEvent(self.__OnClickPickupModeButton)
				self.mainPickModeButtonList.append(pick_up_button_single)

				pick_up_button_all = GetObject("pick_up_button_all")
				pick_up_button_all.SAFE_SetEvent(self.__OnClickPickupModeButton, True)
				# pick_up_button_all.SetToggleDownEvent(self.__OnClickPickupModeButton)
				self.mainPickModeButtonList.append(pick_up_button_all)

				self.__RefreshPickUP()

		elif optionIndex == 1:#Graphic-Options
			if categoryIndex == 0:#General
				self.showDamageButtonList.append(GetObject("show_damage_on_button"))
				self.showDamageButtonList[0].SAFE_SetEvent(self.__OnClickShowDamage)
				self.showDamageButtonList.append(GetObject("show_damage_off_button"))
				self.showDamageButtonList[1].SAFE_SetEvent(self.__OnClickShowDamage)
				self.RefreshShowDamage()

				self.cameraModeButtonList.append(GetObject("camera_short"))
				self.cameraModeButtonList[0].SAFE_SetEvent(self.__OnClickCameraMode)
				self.cameraModeButtonList.append(GetObject("camera_long"))
				self.cameraModeButtonList[1].SAFE_SetEvent(self.__OnClickCameraMode)
				self.RefreshCameraMode()

				self.fogModeButtonList.append(GetObject("fog_level0"))
				self.fogModeButtonList[0].SAFE_SetEvent(self.__OnClickFog, 0)
				self.fogModeButtonList.append(GetObject("fog_level1"))
				self.fogModeButtonList[1].SAFE_SetEvent(self.__OnClickFog, 1)
				self.fogModeButtonList.append(GetObject("fog_level2"))
				self.fogModeButtonList[2].SAFE_SetEvent(self.__OnClickFog, 2)
				self.RefreshFog()

				self.showRainBowButtonList.append(GetObject("rainbow_on"))
				self.showRainBowButtonList[0].SAFE_SetEvent(self.__OnClickShowRainBow)
				self.showRainBowButtonList.append(GetObject("rainbow_off"))
				self.showRainBowButtonList[1].SAFE_SetEvent(self.__OnClickShowRainBow)
				self.RefreshRainBow()

				# self.nightModeButtonList.append(GetObject("nightmode_on"))
				# self.nightModeButtonList[0].SAFE_SetEvent(self.__OnClickNightMode)
				# self.nightModeButtonList.append(GetObject("nightmode_off"))
				# self.nightModeButtonList[1].SAFE_SetEvent(self.__OnClickNightMode)
				# self.UpdateNightMode()

				# if app.ENABLE_DISCORD_STUFF:
				# self.wndNotificationButtonList.append(GetObject("windows_message_on"))
				# self.wndNotificationButtonList[0].SAFE_SetEvent(self.__OnClickWindowsNotification)
				# self.wndNotificationButtonList.append(GetObject("windows_message_off"))
				# self.wndNotificationButtonList[1].SAFE_SetEvent(self.__OnClickWindowsNotification)
				# self.UpdateWindowsNotification()
				# else:
				# 	GetObject("windows_message_window").Hide()

			elif categoryIndex == 1:#Interface
				self.showsalesTextButtonList.append(GetObject("salestext_on_button"))
				self.showsalesTextButtonList[0].SAFE_SetEvent(self.__OnClickSalesText)
				self.showsalesTextButtonList.append(GetObject("salestext_off_button"))
				self.showsalesTextButtonList[1].SAFE_SetEvent(self.__OnClickSalesText)
				self.RefreshShowSalesText()
	
				self.showtitleTextButtonList.append(GetObject("title_view"))
				self.showtitleTextButtonList[0].SAFE_SetEvent(self.__OnClickTitleText)
				self.showtitleTextButtonList.append(GetObject("title_hide"))
				self.showtitleTextButtonList[1].SAFE_SetEvent(self.__OnClickTitleText)
				self.RefreshShowTitleText()
	
				# if app.ENABLE_HIDE_OBJECTS:
				# for bl in xrange(2):
				# 	self.VisibleButtonList.append(GetObject("vsble" + str(bl)))
				# 	self.VisibleButtonList[bl].SetToggleUpEvent(lambda arg=bl: self.SetVisibleFunc(arg))
				# 	self.VisibleButtonList[bl].SetToggleDownEvent(lambda arg=bl: self.SetVisibleFunc(arg))
				
				# if app.ENABLE_SNOW_MODE:
				# self.showSnowButtonList.append(GetObject("show_snow_button"))
				# self.showSnowButtonList.append(GetObject("hide_snow_button"))
				# self.showSnowButtonList[0].SAFE_SetEvent(self.__SnowOn)
				# self.showSnowButtonList[1].SAFE_SetEvent(self.__SnowOff)
				# self.RefreshSnowTexture()

				# for j in xrange(3):
				# 	self.inventoryChangeList.append(GetObject("inventory_%d"%j))
				# 	self.inventoryChangeList[j].SAFE_SetEvent(self.OnClickInventory, j)
				# self.UpdateInventory()

			elif categoryIndex == 2:#Performance
				self.alwaysShowNameButtonList.append(GetObject("always_show_name_on_button"))
				self.alwaysShowNameButtonList[0].SAFE_SetEvent(self.__OnClickAlwaysShowNameOnButton)
				self.alwaysShowNameButtonList.append(GetObject("always_show_name_off_button"))
				self.alwaysShowNameButtonList[1].SAFE_SetEvent(self.__OnClickAlwaysShowNameOffButton)
				self.alwaysShowNameButtonList.append(GetObject("always_show_name_partial_button"))
				self.alwaysShowNameButtonList[2].SAFE_SetEvent(self.__OnClickAlwaysShowNamePartialButton)
				self.RefreshAlwaysShowName()

				# self.showOutlineButtonList.append(GetObject("outline_on_button"))
				# self.showOutlineButtonList[0].SAFE_SetEvent(self.__OnClickShowOutline)
				# self.showOutlineButtonList.append(GetObject("outline_off_button"))
				# self.showOutlineButtonList[1].SAFE_SetEvent(self.__OnClickShowOutline)
				# self.RefreshOutline()

				self.affectIconsButtonList.append(GetObject("show_affect_icons"))
				self.affectIconsButtonList[0].SAFE_SetEvent(self.__OnClickAffectIcons)
				self.affectIconsButtonList.append(GetObject("hide_affect_icons"))
				self.affectIconsButtonList[1].SAFE_SetEvent(self.__OnClickAffectIcons)
				self.RefreshAffectIcons()

				ctrlFovValue = GetObject("perspective_controller")
				pos = 0.0
				fog = float(systemSetting.GetFOVLevel())
				if fog < 30.0 or fog > 100.0:
					fog = 30.0
					systemSetting.SetFOVLevel(fog)

				if fog > 30.0:
					fog -= 30.0
					value =1.0/70.0
					pos = value*fog
				ctrlFovValue.SetSliderPos(pos)
				ctrlFovValue.SetEvent(ui.__mem_func__(self.OnChangeFOV))
				self.ctrlFovValue = ctrlFovValue

				# for j in xrange(5):
				# 	self.shopRangeButtonList.append(GetObject("bazaar%d"%j))
				# 	self.shopRangeButtonList[j].SAFE_SetEvent(self.__OnClickShopRange, j)

				# self.UpdateShopRange()

				self.privateShopTitleDistSlider = GetObject("private_shop_distance_controller")
				self.privateShopTitleDistSlider.SetSliderPos(systemSetting.GetPrivateShopViewDistance())
				self.privateShopTitleDistSlider.SetEvent(ui.__mem_func__(self.OnChangePrivateShopViewDistance))

				for j in xrange(5):
					self.effectButtonList.append(GetObject("effect_level%d"%j))
					self.effectButtonList[j].SAFE_SetEvent(self.__OnClickEffect, j)
				self.UpdateEffectLevel()

				self.petButtonList.append(GetObject("pet_on"))
				self.petButtonList[0].SAFE_SetEvent(self.__OnClickPetRender)
				self.petButtonList.append(GetObject("pet_off"))
				self.petButtonList[1].SAFE_SetEvent(self.__OnClickPetRender)
				self.UpdatePetRender()

				self.mountButtonList.append(GetObject("mount_on"))
				self.mountButtonList[0].SAFE_SetEvent(self.__OnClickMountRender)
				self.mountButtonList.append(GetObject("mount_off"))
				self.mountButtonList[1].SAFE_SetEvent(self.__OnClickMountRender)
				self.UpdateMountRender()

				self.shopNameButtonList.append(GetObject("npcName_on"))
				self.shopNameButtonList[0].SAFE_SetEvent(self.__OnClickShopNameStatus)
				self.shopNameButtonList.append(GetObject("npcName_off"))
				self.shopNameButtonList[1].SAFE_SetEvent(self.__OnClickShopNameStatus)
				self.UpdateShopNameStatus()

				# if app.ENABLE_MAP_OBJECT_OPTIMIZATION:
				ctrlmapObjectDist = GetObject("object_distance_controller")
				pos = 0.0
				fDist = float(systemSetting.GetMapObjectDistance())
				if fDist > 25600.0:
					fDist = 25600.0
					systemSetting.SetMapObjectDistance(float(fDist))
				value =1.0/25600.0
				pos = value*fDist
				ctrlmapObjectDist.SetSliderPos(pos)
				ctrlmapObjectDist.SetEvent(ui.__mem_func__(self.OnChangeObjectDistance))
				self.ctrlmapObjectDist = ctrlmapObjectDist

				# if systemSetting.GetDamageScale() < 0.15:
				# 	systemSetting.SetDamageScale(0.15)
				# ctrlDamageScale = GetObject("damage_scale_controller")
				# # ctrlDamageScale.SetSliderPos(systemSetting.GetDamageScale() * (1.0 / 2.5))
				# ctrlDamageScale.SetSliderPos(1.0 * (1.0 / 2.5))
				# ctrlDamageScale.SetEvent(ui.__mem_func__(self.OnChangeDamageScale))
				# self.ctrlDamageScale = ctrlDamageScale

				stoneScale = GetObject("stone_scale_controller")
				stoneScale.SetSliderPos(float(systemSetting.GetStoneScale()) / 2.0)
				stoneScale.SetEvent(ui.__mem_func__(self.OnChangeStoneScale))
				self.stoneScale = stoneScale

		elif optionIndex == 2:#Sound-Options
			if categoryIndex == 0:#General

				ctrlSoundVolume = GetObject("sound_volume_controller")
				ctrlSoundVolume.SetSliderPos(float(systemSetting.GetSoundVolume()) / 5.0)
				ctrlSoundVolume.SetEvent(ui.__mem_func__(self.OnChangeSoundVolume))
				self.ctrlSoundVolume = ctrlSoundVolume

				ctrlMusicVolume = GetObject("music_volume_controller")
				ctrlMusicVolume.SetSliderPos(float(systemSetting.GetMusicVolume()))
				ctrlMusicVolume.SetEvent(ui.__mem_func__(self.OnChangeMusicVolume))
				self.ctrlMusicVolume = ctrlMusicVolume
				GetObject("bgm_button").SAFE_SetEvent(self.__OnClickChangeMusicButton)
				self.selectMusicFile = GetObject("bgm_file")
		#except:
		#	dbg.TraceError("[GameOption]LoadCatetoryOptions optionIndex: %d categoryIndex: %d"%(optionIndex, categoryIndex))
		self.__SnowOn()
	# Dont Touch #
	def GetCurrentWindow(self):
		if self.children.has_key("optionType") and self.children.has_key("categoryType"):
			(optionType,categoryType) = (self.children["optionType"],self.children["categoryType"])
			if self.children.has_key("optionWindow%d%d"%(optionType,categoryType)):
				return self.children["optionWindow%d%d"%(optionType,categoryType)]
		return None
	def GetCurrentScrollBar(self):
		window = self.GetCurrentWindow()
		if window != None:
			return window.scrollBar
		return None
	def OnRunMouseWheel(self, nLen):
		scrollBar = self.GetCurrentScrollBar()
		if scrollBar:
			if scrollBar.IsShow():
				if nLen > 0:
					scrollBar.OnUp()
				else:
					scrollBar.OnDown()
				return True
		return False

	def UpdateScrollBar(self):
		window = self.GetCurrentWindow()
		if window == None:
			return
		scrollBar = window.scrollBar
		if scrollBar == None:
			return
		scriptChildrens = window.Children
		if len(scriptChildrens) == 0:
			return

		windowHeight = window.GetHeight()

		# OLD SHIT
		#lastChildren = scriptChildrens[len(scriptChildrens)-1]
		#screenSize = lastChildren.exPos[1]+lastChildren.GetHeight()

		screenSize = 0
		for child in scriptChildrens:
			childHeight = child.exPos[1] + child.GetHeight()
			if childHeight > screenSize:
				screenSize=childHeight

		if screenSize > windowHeight:
			scrollLen = screenSize-windowHeight
			basePos = int(scrollBar.GetPos()*scrollLen)
			for child in scriptChildrens:
				(_x,_y) = child.exPos
				child.SetPosition(_x,_y-basePos)

			scrollBar.SetMiddleBarSize(float(windowHeight-5)/float(screenSize))
			scrollBar.Show()
		else:
			scrollBar.Hide()

		_wy = window.GetGlobalPosition()[1]
		elementDictionary = window.ElementDictionary
		for childName, child in elementDictionary.items():
			(_x,_y) = child.GetGlobalPosition()
			childHeight = child.GetHeight()
			textLines = []
			images = []
			if isinstance(child, ui.ExpandedImageBox):
				images.append(child)
			elif isinstance(child, ui.TextLine):
				textLines.append(child)
			elif isinstance(child, ui.ToggleButton) or isinstance(child, ui.RadioButton):
				if child.ButtonText != None:
					textLines.append(child.ButtonText)
				images.append(child)
			elif isinstance(child, ui.SliderBar):
				if child.backGroundImage != None:
					images.append(child.backGroundImage)
				if child.cursor != None:
					images.append(child.cursor)

			for childItem in textLines:
				if _y < _wy:
					if childItem.IsShow():
						childItem.Hide()
				elif _y > (_wy+windowHeight-20):
					if childItem.IsShow():
						childItem.Hide()
				else:
					if not childItem.IsShow():
						childItem.Show()

			for childItem in images:
				if _y < _wy:
					childItem.SetRenderingRect(0,ui.calculateRect(childHeight-abs(_y-_wy),childHeight),0,0)
				elif _y+childHeight > (_wy+windowHeight-4):
					calculate = (_wy+windowHeight-4) - (_y+childHeight)
					if calculate == 0:
						return
					f = ui.calculateRect(childHeight-abs(calculate),childHeight)
					childItem.SetRenderingRect(0,0,0,f)
				else:
					childItem.SetRenderingRect(0,0,0,0)
	# Dont Touch #

	# PvP - Mode
	def __SetPeacePKMode(self):
		self.__SetPKMode(player.PK_MODE_PEACE)
	def __SetPKMode(self, mode):
		for btn in self.pvpModeButtonDict.values():
			btn.SetUp()
		if self.pvpModeButtonDict.has_key(mode):
			self.pvpModeButtonDict[mode].Down()
	def __CheckPvPProtectedLevelPlayer(self):
		if player.GetStatus(player.LEVEL)<constInfo.PVPMODE_PROTECTED_LEVEL:
			self.__SetPeacePKMode()
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_PROTECT % (constInfo.PVPMODE_PROTECTED_LEVEL))
			return True
		return False
	def __RefreshPVPButtonList(self):
		self.__SetPKMode(player.GetPKMode())
	def __OnClickPvPModePeaceButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return
		self.__RefreshPVPButtonList()
		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 0", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)
	def __OnClickPvPModeRevengeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return
		self.__RefreshPVPButtonList()
		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 1", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)
	def __OnClickPvPModeFreeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return
		self.__RefreshPVPButtonList()
		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 2", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)
	def __OnClickPvPModeFearButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return
		self.__RefreshPVPButtonList()
		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 5", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)
	def __OnClickPvPModeGuildButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return
		self.__RefreshPVPButtonList()
		if 0 == player.GetGuildID():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_CANNOT_SET_GUILD_MODE)
			return
		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 4", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)
	def OnChangePKMode(self):
		self.__RefreshPVPButtonList()
	# PvP - Mode

	# Block - Mode
	def RefreshBlock(self):
		global blockMode
		for i in xrange(len(self.blockButtonList)):
			if 0 != (blockMode & (1 << i)):
				self.blockButtonList[i].Down()
			else:
				self.blockButtonList[i].SetUp()
	def OnBlockMode(self, mode):
		global blockMode
		blockMode = mode
		self.RefreshBlock()
	def __OnClickBlockExchangeButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_EXCHANGE))
	def __OnClickBlockPartyButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY))
	def __OnClickBlockGuildButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_GUILD))
	def __OnClickBlockWhisperButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_WHISPER))
	def __OnClickBlockFriendButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_FRIEND))
	def __OnClickBlockPartyRequest(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY_REQUEST))
	def __OnClickBlockViewEquip(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_EQUIPMENT_VIEW))
	# Block - Mode

	# Quest Letter - Mode
	def RefreshQuestLetter(self):
		if systemSetting.GetQuestIconMode():
			self.questLetterButtonList[0].Down()
			self.questLetterButtonList[1].SetUp()
		else:
			self.questLetterButtonList[0].SetUp()
			self.questLetterButtonList[1].Down()

		interface = constInfo.GetInterfaceInstance()
		if interface != None:
			if systemSetting.GetQuestIconMode():
				interface.ShowAllQuestButton()
			else:
				interface.HideAllQuestButton()

	def __OnClickQuestLetter(self):
		systemSetting.SetQuestIconMode(not systemSetting.GetQuestIconMode())
		self.RefreshQuestLetter()
	# Quest Letter - Mode

	# Chat - Mode
	def RefreshViewChat(self):
		if systemSetting.IsViewChat():
			self.viewChatButtonList[0].Down()
			self.viewChatButtonList[1].SetUp()
		else:
			self.viewChatButtonList[0].SetUp()
			self.viewChatButtonList[1].Down()
	def __OnClickViewChat(self):
		systemSetting.SetViewChatFlag(not systemSetting.IsViewChat())
		self.RefreshViewChat()
	# Chat - Mode

	# Country Flag - Mode
	def RefreshCountryFlag(self):
		if systemSetting.IsShowCountryFlag():
			self.countryFlagButtonList[0].Down()
			self.countryFlagButtonList[1].SetUp()
		else:
			self.countryFlagButtonList[0].SetUp()
			self.countryFlagButtonList[1].Down()

	def __OnClickCountryFlag(self):
		systemSetting.SetShowCountryFlag(not systemSetting.IsShowCountryFlag())
		self.RefreshCountryFlag()
	# Country Flag - Mode

	# Name Color - Mode
	def __OnClickNameColorMode(self):
		constInfo.SET_CHRNAME_COLOR_INDEX(not constInfo.GET_CHRNAME_COLOR_INDEX())
		self.RefreshNameColor()

	def RefreshNameColor(self):
		index = constInfo.GET_CHRNAME_COLOR_INDEX()
		self.nameColorModeButtonList[index].Down()
		self.nameColorModeButtonList[not index].SetUp()
	# Name Color - Mode

	# Target Board - Mode
	def RefreshTargetBoard(self):
		index = constInfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD()
		self.viewTargetBoardButtonList[index].Down()
		self.viewTargetBoardButtonList[not index].SetUp()
	def __SetTargetBoardViewMode(self):
		constInfo.SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(not constInfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD())
		self.RefreshTargetBoard()
	# Target Board - Mode

	# Show Always Name - Mode
	def RefreshAlwaysShowName(self):
		x = systemSetting.IsAlwaysShowName()
		# self.alwaysShowNameButtonList[index].SetUp()
		# self.alwaysShowNameButtonList[not index].Down()
		if x == 1:
			self.alwaysShowNameButtonList[0].Down()
			self.alwaysShowNameButtonList[1].SetUp()
			self.alwaysShowNameButtonList[2].SetUp()
		elif x == 2:
			self.alwaysShowNameButtonList[0].SetUp()
			self.alwaysShowNameButtonList[1].Down()
			self.alwaysShowNameButtonList[2].SetUp()
		elif x == 3:
			textTail.HideAllTextTail()
			textTail.ShowAllTextTail()
			self.alwaysShowNameButtonList[0].SetUp()
			self.alwaysShowNameButtonList[1].SetUp()
			self.alwaysShowNameButtonList[2].Down()

	def __OnClickAlwaysShowNameOnButton(self):
		systemSetting.SetAlwaysShowNameFlag(1)
		self.RefreshAlwaysShowName()

	def __OnClickAlwaysShowNameOffButton(self):
		systemSetting.SetAlwaysShowNameFlag(2)
		self.RefreshAlwaysShowName()

	def __OnClickAlwaysShowNamePartialButton(self):
		systemSetting.SetAlwaysShowNameFlag(3)
		self.RefreshAlwaysShowName()

	# def __OnClickAlwaysShowName(self):
	# 	systemSetting.SetAlwaysShowNameFlag(not systemSetting.IsAlwaysShowName())
	# 	self.RefreshAlwaysShowName()
	# Show Always Name - Mode

	# Show Damage - Mode
	def RefreshShowDamage(self):
		index = systemSetting.IsShowDamage()
		self.showDamageButtonList[index].SetUp()
		self.showDamageButtonList[not index].Down()
	def __OnClickShowDamage(self):
		systemSetting.SetShowDamageFlag(not systemSetting.IsShowDamage())
		self.RefreshShowDamage()
	# Show Damage - Mode

	def RefreshRainBow(self):
		index = systemSetting.IsShowRainBow()
		self.showRainBowButtonList[index].SetUp()
		self.showRainBowButtonList[not index].Down()
	def __OnClickShowRainBow(self):
		systemSetting.SetShowRainBowFlag(not systemSetting.IsShowRainBow())
		self.RefreshRainBow()

	# Sales Text - Mode
	def __OnClickSalesText(self):
		systemSetting.SetShowSalesTextFlag(not systemSetting.IsShowSalesText())
		self.RefreshShowSalesText()
	def RefreshShowSalesText(self):
		index = systemSetting.IsShowSalesText()
		self.showsalesTextButtonList[index].Down()
		self.showsalesTextButtonList[not index].SetUp()
	# Sales Text - Mode

	def __OnClickTitleText(self):
		systemSetting.SetShowTitleTextFlag(not systemSetting.IsShowTitleText())
		self.RefreshShowTitleText()
	def RefreshShowTitleText(self):
		index = systemSetting.IsShowTitleText()
		self.showtitleTextButtonList[index].Down()
		self.showtitleTextButtonList[not index].SetUp()

	# Sound - Mode
	def OnChangeSoundVolume(self):
		pos = self.ctrlSoundVolume.GetSliderPos()
		snd.SetSoundVolumef(pos)
		systemSetting.SetSoundVolumef(pos)
	# Sound - Mode

	# Music - Mode
	def OnChangeMusicVolume(self):
		pos = self.ctrlMusicVolume.GetSliderPos()
		snd.SetMusicVolume(pos * net.GetFieldMusicVolume())
		systemSetting.SetMusicVolume(pos)
	def __OnChangeMusic(self, fileName):
		self.selectMusicFile.SetText(fileName[:MUSIC_FILENAME_MAX_LEN])
		if musicInfo.fieldMusic != "":
			snd.FadeOutMusic("BGM/"+ musicInfo.fieldMusic)
		if fileName==uiSelectMusic.DEFAULT_THEMA:
			musicInfo.fieldMusic=musicInfo.METIN2THEMA
		else:
			musicInfo.fieldMusic=fileName
		musicInfo.SaveLastPlayFieldMusic()
		if musicInfo.fieldMusic != "":
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)
	def __OnClickChangeMusicButton(self):
		if not self.musicListDlg:
			self.musicListDlg=uiSelectMusic.FileListDialog()
			self.musicListDlg.SAFE_SetSelectEvent(self.__OnChangeMusic)
		self.musicListDlg.Open()
	# Music - Mode

	# Camera - Mode
	def RefreshCameraMode(self):
		index = constInfo.GET_CAMERA_MAX_DISTANCE_INDEX()
		self.cameraModeButtonList[index].Down()
		self.cameraModeButtonList[not index].SetUp()
	def __OnClickCameraMode(self):
		constInfo.SET_CAMERA_MAX_DISTANCE_INDEX(not constInfo.GET_CAMERA_MAX_DISTANCE_INDEX())
		self.RefreshCameraMode()
	# Camera - Mode

	# Fog - Mode
	def RefreshFog(self):
		self.__ClickRadioButton(self.fogModeButtonList, constInfo.GET_FOG_LEVEL_INDEX())
	def __OnClickFog(self, index):
		constInfo.SET_FOG_LEVEL_INDEX(index)
		self.RefreshFog()
	# Fog - Mode

	# Outline - Mode
	def RefreshOutline(self):
		pass
		# index = systemSetting.IsShowOutline()
		# self.showOutlineButtonList[not index].Down()
		# self.showOutlineButtonList[index].SetUp()
		
	def __OnClickShowOutline(self):
		pass
		# systemSetting.SetShowOutlineFlag(not systemSetting.IsShowOutline())
		# self.RefreshOutline()
	# Outline - Mode

	def OnChangeDamageScale(self):
		pass
		# pos = self.ctrlDamageScale.GetSliderPos()*2.5
		# value =1.0/2.5
		# result = (value*pos)*2.5
		# if result < 0.15:
		# 	result = 0.15
		# systemSetting.SetDamageScale(result)
		# self.ctrlDamageScale.SetSliderPos(result * (1.0 / 2.5))

	def OnChangeStoneScale(self):
		value = self.MinMax(0.5, self.stoneScale.GetSliderPos() * 2, 2.0)
		systemSetting.SetStoneScale(value)

	def MinMax(self, min, value, max):
		temp = min
		if min > value:
			temp = min
		else:
			temp = value
			
		if max < temp:
			return max
		else:
			return temp

	# Pickup - Mode
	# if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
	def __SendPickupModeCommand(self):
		pass
		# net.SendChatPacket("/setpickupmode {} {}".format(constInfo.PICKUPMODE, constInfo.RARITYMODE))
	def __RefreshPickUP(self):
		for (index, button) in enumerate(self.mainPickUPButtonList):
			if systemSetting.GetPickUpFilter(index):
				button.Down()
			else:
				button.SetUp()


		mode_all = systemSetting.GetPickUpFilterModeAll()
		if mode_all:
			self.mainPickModeButtonList[0].SetUp()
			self.mainPickModeButtonList[1].Down()
		else:
			self.mainPickModeButtonList[0].Down()
			self.mainPickModeButtonList[1].SetUp()

		# if 0 != (constInfo.PICKUPMODE & (1 << 0)):
		# 	self.mainPickModeButtonList[0].Down()
		# 	self.mainPickModeButtonList[1].SetUp()
		# else:
		# 	self.mainPickModeButtonList[1].Down()
		# 	self.mainPickModeButtonList[0].SetUp()

		# for i in range(0, 18):
		# 	if 0 != (systemSetting.GetPickUpFilter(i)):
		# 		self.mainPickUPButtonList[i].Down()
		# 	else:
		# 		self.mainPickUPButtonList[i].SetUp()

		# for i in xrange(len(self.rarityFilterButtonList)):
		# 	if 0 != (constInfo.RARITYMODE & (1 << i)):
		# 		self.rarityFilterButtonList[i].Down()
		# 	else:
		# 		self.rarityFilterButtonList[i].SetUp()

		# window = self.children["optionWindow%d%d"%(0,1)] if self.children.has_key("optionWindow%d%d"%(0,1)) else None
		# if window:
		# 	if constInfo.PREMIUMMODE[0] == True:
		# 		window.GetChild("pickup_premimum_image").LoadImage("d:/ymir work/ui/game/gameoption/pickup/activated.tga")
		# 	else:
		# 		window.GetChild("pickup_premimum_image").LoadImage("d:/ymir work/ui/game/gameoption/pickup/not_activated.tga")
		# 		window.GetChild("pickup_premium_text_1").SetText(uiScriptLocale.AUTOMATIC_PICKUP_TEXT_1)
		# self.UpdateScrollBar()

	def FormatTime(self, seconds):
		if seconds <= 0:
			return "|cffFF7B7B0h 0m 0s"
		m, s = divmod(seconds, 60)
		h, m = divmod(m, 60)
		d, h = divmod(h, 24)
		if d > 0:
			return "%dd %dh %dm %ds"%(d,h,m,s)
		return "%dh %dm %ds" % (h,m,s)

	def OnUpdate(self):
		window = self.children["optionWindow%d%d"%(0,1)] if self.children.has_key("optionWindow%d%d"%(0,1)) else None
		if window:
			if constInfo.PREMIUMMODE[0] == True:
				window.GetChild("pickup_premium_text_1").SetText(uiScriptLocale.AUTOMATIC_PICKUP_TEXT_2 % self.FormatTime(constInfo.PREMIUMMODE[1]-app.GetGlobalTimeStamp()))

	def __OnClickPickupModeButton(self, all):
		systemSetting.SetPickUpFilterModeAll(all)
		# if 0 != (constInfo.PICKUPMODE & (1 << 0)):
		# 	constInfo.PICKUPMODE -= (1 << 0)
		# else:
		# 	constInfo.PICKUPMODE += (1 << 0)
		# self.__SendPickupModeCommand()
		self.__RefreshPickUP()
	
	def __OnClickRarityIgnoreButton(self, type, ignore):
		pass
		# if ignore:
		# 	constInfo.RARITYMODE += type
		# else:
		# 	constInfo.RARITYMODE -= type
		# self.__SendPickupModeCommand()
		# self.__RefreshPickUP()

	def __OnClickPickupIgnoreButton(self, index):
		systemSetting.SetPickUpFilter(index)
		self.__RefreshPickUP()
		# pass
		# if ignore:
		# 	constInfo.PICKUPMODE += type
		# else:
		# 	constInfo.PICKUPMODE -= type
		# self.__SendPickupModeCommand()
		# self.__RefreshPickUP()

	def OnChangePickUPMode(self):
		self.__RefreshPickUP()
	# Pickup - Mode


	# visible - Mode
	# if app.ENABLE_HIDE_OBJECTS:
	def SetVisibleFunc(self, index):
		pass
		# background.SetVisiblePart(self.mylist[index], not background.IsVisiblePart(self.mylist[index]))
		# for bl in xrange(len(self.mylist)):
		# 	if background.IsVisiblePart(self.mylist[bl]):
		# 		self.VisibleButtonList[bl].SetUp()
		# 	else:
		# 		self.VisibleButtonList[bl].Down()
	# visible - Mode

	# Mob Info - Mode
	if app.WJ_SHOW_MOB_INFO:
		def RefreshShowMobInfo(self):
			if systemSetting.IsShowMobLevel():
				self.showMobInfoButtonList[0].Down()
			else:
				self.showMobInfoButtonList[0].SetUp()
			if systemSetting.IsShowMobAIFlag():
				self.showMobInfoButtonList[1].Down()
			else:
				self.showMobInfoButtonList[1].SetUp()
		def __OnClickShowMobLevelButton(self):
			systemSetting.SetShowMobLevel(not systemSetting.IsShowMobLevel())
			self.RefreshShowMobInfo()
		def __OnClickShowMobAIFlagButton(self):
			systemSetting.SetShowMobAIFlag(not systemSetting.IsShowMobAIFlag())
			self.RefreshShowMobInfo()
	# Mob Info - Mode
	
	# Snow - Mode
	# if app.ENABLE_SNOW_MODE:
	def RefreshSnowTexture(self, isLoadinWindow=False):
		pass
	# 	index = systemSetting.IsSnowTexturesMode()
	# 	self.showSnowButtonList[not index].Down()
	# 	self.showSnowButtonList[index].SetUp()

	# 	if isLoadinWindow:
	# 		if index:
	# 			return

	# 	mapName = str(background.GetCurrentMapName())
	# 	if mapName:
	# 		snow_maps = [
	# 			"metin2_map_a1",
	# 			"metin2_map_b1",
	# 			"metin2_map_c1"
	# 		]
	# 		if index:
	# 			# Snow
	# 			snow_maps_textures = {
	# 				"metin2_map_a1" : "textureset\snow\snow_metin2_a1.txt",
	# 				"metin2_map_b1" : "textureset\snow\snow_metin2_b1.txt",
	# 				"metin2_map_c1" : "textureset\snow\snow_metin2_c1.txt", }
	# 		else:
	# 			# Normal
	# 			snow_maps_textures = {
	# 				"metin2_map_a1" : "textureset\metin2_map1.txt",
	# 				"metin2_map_b1" : "textureset\metin2_map1.txt",
	# 				"metin2_map_c1" : "textureset\metin2_map1.txt", }
	# 		if mapName in snow_maps:
	# 			background.TextureChange(snow_maps_textures[mapName])
	def __SnowOn(self):
		pass
		# systemSetting.SetSnowTexturesMode(1)
		# self.RefreshSnowTexture()
	def __SnowOff(self):
		pass
		# systemSetting.SetSnowTexturesMode(0)
		# self.RefreshSnowTexture()
	# Snow - Mode

	# Affect Icons - Mode
	def RefreshAffectIcons(self):
		index = systemSetting.GetAffectIcons()
		self.affectIconsButtonList[not index].Down()
		self.affectIconsButtonList[index].SetUp()

		game = constInfo.GetGameInstance()
		if game != None:
			if game.affectShower != None:
				if systemSetting.GetAffectIcons():
					game.affectShower.Show()
				else:
					game.affectShower.Hide()

	def __OnClickAffectIcons(self):
		systemSetting.SetAffectIcons(not systemSetting.GetAffectIcons())
		self.RefreshAffectIcons()
	# Affect Icons - Mode

	def OnChangeFOV(self):
		pos = self.ctrlFovValue.GetSliderPos()*70
		value =1.0/70.0
		realpos = (value*pos)*70
		realpos+=30.0
		systemSetting.SetFOVLevel(realpos)

	# def UpdateShopRange(self):
	# 	idx = 0

	# 	iDist = systemSetting.GetPrivateShopViewDistance()
	# 	if iDist >= 10000:
	# 		pass
	# 	elif iDist >= 6000:
	# 		idx = 1
	# 	elif iDist >= 4000:
	# 		idx = 2
	# 	elif iDist >= 2000:
	# 		idx = 3
	# 	else:
	# 		idx = 5

	# 	self.__ClickRadioButton(self.shopRangeButtonList, idx)

	# def __OnClickShopRange(self, idx):
	# 	rangeList = [10000, 6000, 4000, 2000, 0]
	# 	systemSetting.SetPrivateShopViewDistance(rangeList[idx])

	# 	self.UpdateShopRange()

	def OnChangePrivateShopViewDistance(self):
		pos = self.privateShopTitleDistSlider.GetSliderPos()
		systemSetting.SetPrivateShopViewDistance(pos)

	def UpdateEffectLevel(self):
		self.__ClickRadioButton(self.effectButtonList, grp.GetEffectOnOffLevel())

	def __OnClickEffect(self, btnIndex):
		grp.SetEffectOnOffLevel(btnIndex)
		self.UpdateEffectLevel()

	def UpdatePetRender(self):
		self.__ClickRadioButton(self.petButtonList, grp.GetPetOnOffStatus())

	def __OnClickPetRender(self):
		grp.SetPetOnOffStatus(not grp.GetPetOnOffStatus())
		self.UpdatePetRender()
	
	def UpdateMountRender(self):
		self.__ClickRadioButton(self.mountButtonList, grp.GetMountOnOffStatus())

	def __OnClickMountRender(self):
		grp.SetMountOnOffStatus(not grp.GetMountOnOffStatus())
		self.UpdateMountRender()
	
	def UpdateShopNameStatus(self):
		self.__ClickRadioButton(self.shopNameButtonList, grp.GetNPCNameOnOffStatus())

	def __OnClickShopNameStatus(self):
		grp.SetNPCNameOnOffStatus(not grp.GetNPCNameOnOffStatus())
		self.UpdateShopNameStatus()
	
	def UpdateNightMode(self):
		pass
		# self.__ClickRadioButton(self.nightModeButtonList, not systemSetting.GetNightMode())

		# game = constInfo.GetGameInstance()
		# if game != None:
		# 	if systemSetting.GetNightMode():
		# 		game.DayMode_Update("dark")
		# 	else:
		# 		game.DayMode_Update("light")
		
	def __OnClickNightMode(self):
		pass
		# systemSetting.SetNightMode(not systemSetting.GetNightMode())
		# self.UpdateNightMode()

	# if app.ENABLE_DISCORD_STUFF:
	def __OnClickWindowsNotification(self):
		pass
		# systemSetting.SetStuffMessage(not systemSetting.GetStuffMessage())
		# self.UpdateWindowsNotification()

	def UpdateWindowsNotification(self):
		pass
		# self.__ClickRadioButton(self.wndNotificationButtonList, not systemSetting.GetStuffMessage())

	# if app.ENABLE_MAP_OBJECT_OPTIMIZATION:
	def OnChangeObjectDistance(self):
		pos = self.ctrlmapObjectDist.GetSliderPos()*25600.0
		result = ((1.0/25600.0)*pos)*25600.0
		systemSetting.SetMapObjectDistance(result)

	def OnClickInventory(self, btnIndex):
		pass
		# systemSetting.SetInventoryChange(btnIndex)
		# self.UpdateInventory()

	def UpdateInventory(self):
		pass
		# self.__ClickRadioButton(self.inventoryChangeList, systemSetting.GetInventoryChange())
		# net.SendChatPacket("/change_inventory")

class ScrollBarNew(ui.Window):
	SCROLLBAR_WIDTH = 7
	SCROLL_BTN_XDIST = 0
	SCROLL_BTN_YDIST = 0
	class MiddleBar(ui.DragButton):
		def __init__(self):
			ui.DragButton.__init__(self)
			self.AddFlag("movable")
			self.SetWindowName("scrollbar_middlebar")
		def MakeImage(self):
			top = ui.ExpandedImageBox()
			top.SetParent(self)
			top.LoadImage(IMG_DIR+"scrollbar/scrollbar_top.tga")
			top.AddFlag("not_pick")
			top.Show()
			topScale = ui.ExpandedImageBox()
			topScale.SetParent(self)
			topScale.SetPosition(0, top.GetHeight())
			topScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_scale.tga")
			topScale.AddFlag("not_pick")
			topScale.Show()
			bottom = ui.ExpandedImageBox()
			bottom.SetParent(self)
			bottom.LoadImage(IMG_DIR+"scrollbar/scrollbar_bottom.tga")
			bottom.AddFlag("not_pick")
			bottom.Show()
			bottomScale = ui.ExpandedImageBox()
			bottomScale.SetParent(self)
			bottomScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_scale.tga")
			bottomScale.AddFlag("not_pick")
			bottomScale.Show()
			middle = ui.ExpandedImageBox()
			middle.SetParent(self)
			middle.LoadImage(IMG_DIR+"scrollbar/scrollbar_mid.tga")
			middle.AddFlag("not_pick")
			middle.Show()
			self.top = top
			self.topScale = topScale
			self.bottom = bottom
			self.bottomScale = bottomScale
			self.middle = middle
		def SetSize(self, height):
			minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
			height = max(minHeight, height)
			ui.DragButton.SetSize(self, 10, height)
			scale = (height - minHeight) / 2 
			extraScale = 0
			if (height - minHeight) % 2 == 1:
				extraScale = 1
			self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
			self.middle.SetPosition(0, self.top.GetHeight() + scale)
			self.bottomScale.SetPosition(0, self.middle.GetBottom())
			self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
			self.bottom.SetPosition(0, height - self.bottom.GetHeight())
	def __init__(self):
		ui.Window.__init__(self)
		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = None
		self.eventArgs = None
		self.lockFlag = False
		self.CreateScrollBar()
		self.SetScrollBarSize(0)
		self.scrollStep = 0.4
		self.SetWindowName("NONAME_ScrollBar")
	def __del__(self):
		ui.Window.__del__(self)
	def CreateScrollBar(self):
		topImage = ui.ExpandedImageBox()
		topImage.SetParent(self)
		topImage.AddFlag("not_pick")
		topImage.LoadImage(IMG_DIR+"scrollbar/scroll_top.tga")
		topImage.Show()
		bottomImage = ui.ExpandedImageBox()
		bottomImage.SetParent(self)
		bottomImage.AddFlag("not_pick")
		bottomImage.LoadImage(IMG_DIR+"scrollbar/scroll_bottom.tga")
		bottomImage.Show()
		middleImage = ui.ExpandedImageBox()
		middleImage.SetParent(self)
		middleImage.AddFlag("not_pick")
		middleImage.SetPosition(0, topImage.GetHeight())
		middleImage.LoadImage(IMG_DIR+"scrollbar/scroll_mid.tga")
		middleImage.Show()
		self.topImage = topImage
		self.bottomImage = bottomImage
		self.middleImage = middleImage
		middleBar = self.MiddleBar()
		middleBar.SetParent(self)
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.Show()
		middleBar.MakeImage()
		middleBar.SetSize(12)
		self.middleBar = middleBar
	def Destroy(self):
		self.eventScroll = None
		self.eventArgs = None
	def SetScrollEvent(self, event, *args):
		self.eventScroll = event
		self.eventArgs = args
	def SetMiddleBarSize(self, pageScale):
		self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - (self.SCROLL_BTN_YDIST*2))))
		realHeight = self.GetHeight() - (self.SCROLL_BTN_YDIST*2) - self.middleBar.GetHeight()
		self.pageSize = realHeight

	def SetScrollBarSize(self, height):
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
		middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
		self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
		self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
		self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
			self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)
	def SetScrollStep(self, step):
		self.scrollStep = step
	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)
	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)
	def GetScrollStep(self):
		return self.scrollStep
	def GetPos(self):
		return self.curPos
	def OnUp(self):
		self.SetPos(self.curPos-self.scrollStep)
	def OnDown(self):
		self.SetPos(self.curPos+self.scrollStep)
	def SetPos(self, pos, moveEvent = True):
		pos = max(0.0, pos)
		pos = min(1.0, pos)
		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
		if moveEvent == True:
			self.OnMove()
	def OnMove(self):
		if self.lockFlag:
			return
		if 0 == self.pageSize:
			return
		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)
		if self.eventScroll:
			apply(self.eventScroll, self.eventArgs)
	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		newPos = float(yMouseLocalPosition) / float(self.GetHeight())
		self.SetPos(newPos)
	def LockScroll(self):
		self.lockFlag = True
	def UnlockScroll(self):
		self.lockFlag = False
