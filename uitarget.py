import app
import ui
import player
import net
import wndMgr
import messenger
import guild
import chr
import nonplayer
import localeInfo
import constInfo

import constInfo
import uiscriptlocale

if app.ENABLE_PLAYER_HP:
	import mouseModule
	import colorInfo
if app.ENABLE_DROP_INFO:
	import uiToolTip
	import item
	from grid import Grid
	from _weakref import proxy

if app.ENABLE_DROP_INFO:
	RACE_FLAGS = {
			nonplayer.RACE_FLAG_ATT_ELEC : localeInfo.TARGET_RACE_FLAG_ATT_ELEC,
			nonplayer.RACE_FLAG_ATT_FIRE : localeInfo.TARGET_RACE_FLAG_ATT_FIRE,
			nonplayer.RACE_FLAG_ATT_ICE : localeInfo.TARGET_RACE_FLAG_ATT_ICE,
			nonplayer.RACE_FLAG_ATT_WIND : localeInfo.TARGET_RACE_FLAG_ATT_WIND,
			nonplayer.RACE_FLAG_ATT_EARTH : localeInfo.TARGET_RACE_FLAG_ATT_EARTH,
			nonplayer.RACE_FLAG_ATT_DARK : localeInfo.TARGET_RACE_FLAG_ATT_DARK,
			}
	class ElementalImage(ui.ThinBoard):
		IMAGE_LIST = {
							nonplayer.RACE_FLAG_ATT_ELEC : "elect.sub",
							nonplayer.RACE_FLAG_ATT_FIRE: "fire.sub",
							nonplayer.RACE_FLAG_ATT_ICE : "ice.sub",
							nonplayer.RACE_FLAG_ATT_WIND : "wind.sub",
							nonplayer.RACE_FLAG_ATT_EARTH : "earth.sub",
							nonplayer.RACE_FLAG_ATT_DARK : "dark.sub",
					}
		IMAGE_PATH = "d:/ymir work/ui/game/12zi/element/%s"

		def	__init__(self, parentProxy):
			ui.ThinBoard.__init__(self)

			self.windowToolTip = uiToolTip.ToolTip()
			self.iElement = len(RACE_FLAGS)-1
			self.parent = proxy(parentProxy)

			self.imgElement = ui.ExpandedImageBox()
			self.imgElement.SetParent(self)
			self.imgElement.SetWindowHorizontalAlignCenter()
			self.imgElement.SetWindowVerticalAlignCenter()
			self.imgElement.SetStringEvent("mouse_over_in", ui.__mem_func__(self.__OnOverToolTip))
			self.imgElement.SetStringEvent("mouse_over_out", ui.__mem_func__(self.__OnOutToolTip))
			self.imgElement.Show()

		def	__del__(self):
			ui.ThinBoard.__del__(self)
			self.imgElement = None
			self.windowToolTip = None

		def	LoadElement(self, iVid):
			(iType, sImage) = self.__ExtractElement(iVid, nonplayer.GetRaceFlagByVID(iVid))
			if len(sImage) == 0:
				print "Element not exists"
				return

			self.iElement = iType
			self.imgElement.LoadImage(self.IMAGE_PATH % sImage)
			self.SetSize(self.imgElement.GetWidth() + 4, self.imgElement.GetHeight() + 4)
			self.imgElement.SetPosition(0, 0)

			self.Show()
			self.parent.UpdatePosition()

		def	Close(self):
			self.Hide()
			self.parent.UpdatePosition()

		def	__ExtractElement(self, iVid, dwFlag):
			for k, v in self.IMAGE_LIST.items():
				if nonplayer.IsRaceFlag(iVid, k):
					return (k, v)

			return (-1, "")

		def	__OnOverToolTip(self):
			if self.windowToolTip:
				self.windowToolTip.ClearToolTip()
				self.windowToolTip.AppendTextLine(RACE_FLAGS[self.iElement])
				self.windowToolTip.ShowToolTip()

		def	__OnOutToolTip(self):
			if self.windowToolTip:
				self.windowToolTip.HideToolTip()

	class DropInfoBoard(ui.ThinBoard):
		MARGIN = 8
		def __init__(self, interface):
			ui.ThinBoard.__init__(self)
			self.wndItem = None
			self.slotDataDict = {}
			self.itemNameList = []
			self.toolTipItem = uiToolTip.ItemToolTip()
			self.toolTipItem.HideToolTip()
			self.interface = interface

		def __del__(self):
			ui.ThinBoard.__del__(self)

		def GetWidth(self):
			return self.MARGIN * 2 + (9 * 32)

		def Refresh(self, mob_vnum):
			self.Show()

			self.mob_vnum = mob_vnum

			data = constInfo.dropInfoDict.get(mob_vnum)
			if not data: return

			row = self.__UpdateGridSize(data)

			if self.wndItem:
				self.wndItem.ClearSlot(self.wndItem.GetSlotCount())
				self.wndItem.Hide()
				self.wndItem = None

			self.wndItem = ui.GridSlotWindow()
			self.wndItem.SetParent(self)
			self.wndItem.SetPosition(self.MARGIN, self.MARGIN)
			self.wndItem.ArrangeSlot(0, 9, row, 32, 32, 0, 0)
			self.wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			self.wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			if app.ENABLE_CHEST_DROP_INFO or app.ENABLE_RENDER_TARGET_PREVIEW:
				self.wndItem.SetUseSlotEvent(ui.__mem_func__(self.__UseItemSlot))
				self.wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.__UseItemSlot))
			self.wndItem.Show()

			for pos, data in self.slotDataDict.items():
				if data["vnum"][0] == 1:
					self.wndItem.SetItemSlot(pos, data["vnum"][0], 0)
				else:
					self.wndItem.SetItemSlot(pos, data["vnum"][0], data["max_count"])

			self.wndItem.RefreshSlot()
			self.SetSize(self.MARGIN * 2 + (9 * 32), self.MARGIN * 2 + (row * 32))

		def OverInItem(self, slotIndex):
			data = self.slotDataDict[slotIndex]
			vnum = data["vnum"][0]
			self.toolTipItem.SetItemToolTip(vnum)

			if data["min_count"] != data["max_count"]:
				self.toolTipItem.AppendSpace(5)
				self.toolTipItem.AppendTextLine(localeInfo.TOOLTIP_DROP_INFO_RANGE_COUNT % (
					localeInfo.human_format(data["min_count"]), localeInfo.human_format(data["max_count"])))

			if len(data["vnum"]) > 1:
				self.toolTipItem.AppendSpace(5)
				self.toolTipItem.AppendTextLine(localeInfo.TOOLTIP_DROP_INFO_SINGLE_COUNT)
				for vnum in data["vnum"]:
					item.SelectItem(vnum)
					self.toolTipItem.AppendTextLine(localeInfo.TOOLTIP_DROP_INFO_PREFIX % item.GetItemName())

			if app.ENABLE_RENDER_TARGET_PREVIEW:
				self.toolTipItem.AppendRenderingTooltip(vnum)
			if app.ENABLE_CHEST_DROP_INFO:
				self.toolTipItem.AppendChestDropInfo(vnum)

		def OverOutItem(self):
			if self.toolTipItem:
				self.toolTipItem.HideToolTip()

		if app.ENABLE_CHEST_DROP_INFO or app.ENABLE_RENDER_TARGET_PREVIEW:
			def __UseItemSlot(self, slotIndex):
				data = self.slotDataDict[slotIndex]
				vnum = data["vnum"][0]
				item.SelectItem(vnum)
				if app.ENABLE_RENDER_TARGET_PREVIEW and app.IsPressed(app.DIK_LSHIFT):
					self.interface.OpenRenderTarget(vnum)
					return

				if app.ENABLE_CHEST_DROP_INFO and app.IsPressed(app.DIK_LCONTROL):
					if self.interface and item.HasDropInfo(vnum):
						self.interface.OpenChestDropWindow(vnum)
					return

		def __UpdateGridSize(self, data):
			row = len(data) / 9

			self.slotDataDict = {}
			self.itemNameList = []

			while True:
				grid_ok = True
				grid = Grid(width=9, height=row)

				for slot in data:
					if not slot["max_count"]:
						continue

					item.SelectItem(slot["vnum"][0])

					(width, height) = item.GetItemSize()
					item_name = item.GetItemName()
					if item_name.find("+") != -1:
						item_name = item_name.split("+")[0]

						if item_name in self.itemNameList:
							for key, value in self.slotDataDict.items():
								if value["name"] == item_name and slot["vnum"][0] not in value["vnum"]:
									self.slotDataDict[key]["vnum"].append(slot["vnum"][0])
									break

							continue

					pos = grid.find_blank(width, height)

					if pos == -1:
						grid_ok = False
						break

					grid.put(pos, width, height)
					slot.update({"name" : item_name})

					self.slotDataDict[pos] = slot
					self.itemNameList.append(item_name)

				if not grid_ok:
					row += 1
					self.slotDataDict = {}
					self.itemNameList = []
				else:
					break

			return row

if app.ENABLE_METIN_STONE_QUEUE:
	class AutoFarmInfoWindow(ui.ThinBoard):
		def __init__(self):
			ui.ThinBoard.__init__(self)
			self.__LoadWindow()

		def __LoadWindow(self):
			self.SetSize(356, 166)
			self.AddFlag("float")
			self.AddFlag("not_pick")

			title = ui.TextLine()
			title.SetParent(self)
			title.AddFlag("not_pick")
			title.SetHorizontalAlignCenter()
			title.SetPackedFontColor(0xFFFBC401)
			title.SetOutline()
			title.SetText("Target Metin Queue")
			title.SetPosition(self.GetWidth() / 2, 15)
			title.Show()
			self.title = title
			
			firstText = "Pressing |cFF74FF20Shift + Right Click|r on Metinstones your character will target#"\
			"all Metinstone and after you destroyed it, it will move to |cFF74FF20another one|r.#"\
			"|cFF74FF20Rotate your field of vision when you can't run any further!|r."

			firstDescription = ui.MultiTextLine()
			firstDescription.SetParent(self)
			firstDescription.AddFlag("not_pick")
			firstDescription.SetTextRange(13)
			firstDescription.SetTextType("horizontal#center")
			firstDescription.SetPosition(self.GetWidth() / 2, 35)
			firstDescription.SetOutline(1)
			firstDescription.SetText(firstText)
			firstDescription.Show()
			self.firstDescription = firstDescription

			secondText = "Max. 3 queued Metinstones (without Premium Metinfarm)#"\
			"Max. 8 queued Metinstones (with Premium Metinfarm)"

			secondDescription = ui.MultiTextLine()
			secondDescription.SetParent(self)
			secondDescription.AddFlag("not_pick")
			secondDescription.SetTextRange(13)
			secondDescription.SetTextType("horizontal#center")
			secondDescription.SetPosition(self.GetWidth() / 2, 73)
			secondDescription.SetOutline(1)
			secondDescription.SetPackedFontColor(0xFFFFD071)
			secondDescription.SetText(secondText)
			secondDescription.Show()
			self.secondDescription = secondDescription


			item.SelectItem(61400)
			itemIcon = ui.ImageBox()
			itemIcon.SetParent(self)
			itemIcon.AddFlag("not_pick")
			itemIcon.LoadImage(item.GetIconImageFileName())
			itemIcon.SetPosition((self.GetWidth() / 2) - 16, 105)
			itemIcon.Show()
			self.itemIcon = itemIcon

			itemName = ui.TextLine()
			itemName.SetParent(self)
			itemName.AddFlag("not_pick")
			itemName.SetHorizontalAlignCenter()
			itemName.SetPackedFontColor(0xFFFBC401)
			itemName.SetOutline()
			itemName.SetText("Premium Metinfarm")
			itemName.SetPosition(self.GetWidth() / 2, 105 + 32 + 5)
			itemName.Show()
			self.itemName = itemName

			self.SetCenterPosition()

		def Open(self):
			self.Show()
			self.SetTop()

		def Close(self):
			self.Hide()

		def OnPressEscapeKey(self):
			self.Close()
			return True

class TargetBoard(ui.ThinBoard):

	BUTTON_NAME_LIST = (
		localeInfo.TARGET_BUTTON_WHISPER,
		localeInfo.TARGET_BUTTON_EXCHANGE,
		localeInfo.TARGET_BUTTON_FIGHT,
		localeInfo.TARGET_BUTTON_ACCEPT_FIGHT,
		localeInfo.TARGET_BUTTON_AVENGE,
		localeInfo.TARGET_BUTTON_FRIEND,
		localeInfo.TARGET_BUTTON_INVITE_PARTY,
		localeInfo.TARGET_BUTTON_LEAVE_PARTY,
		localeInfo.TARGET_BUTTON_EXCLUDE,
		localeInfo.TARGET_BUTTON_INVITE_GUILD,
		localeInfo.TARGET_BUTTON_DISMOUNT,
		localeInfo.TARGET_BUTTON_EXIT_OBSERVER,
		localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT,
		localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY,
		localeInfo.TARGET_BUTTON_BUILDING_DESTROY,
		localeInfo.TARGET_BUTTON_EMOTION_ALLOW,
		"VOTE_BLOCK_CHAT",
	)

	GRADE_NAME =	{
						nonplayer.PAWN : localeInfo.TARGET_LEVEL_PAWN,
						nonplayer.S_PAWN : localeInfo.TARGET_LEVEL_S_PAWN,
						nonplayer.KNIGHT : localeInfo.TARGET_LEVEL_KNIGHT,
						nonplayer.S_KNIGHT : localeInfo.TARGET_LEVEL_S_KNIGHT,
						nonplayer.BOSS : localeInfo.TARGET_LEVEL_BOSS,
						nonplayer.KING : localeInfo.TARGET_LEVEL_KING,
					}
	EXCHANGE_LIMIT_RANGE = 3000

	if app.ENABLE_DROP_INFO:
		## Values from UserInterface/ItemData.h
		RACE_FLAG_DICT = {
			nonplayer.RACE_FLAG_ANIMAL: [localeInfo.TARGET_RACE_FLAG_ANIMAL, 44],
			nonplayer.RACE_FLAG_UNDEAD: [localeInfo.TARGET_RACE_FLAG_UNDEAD, 47],
			nonplayer.RACE_FLAG_DEVIL: [localeInfo.TARGET_RACE_FLAG_DEVIL, 48],
			nonplayer.RACE_FLAG_HUMAN: [localeInfo.TARGET_RACE_FLAG_HUMAN, 43],
			nonplayer.RACE_FLAG_ORC: [localeInfo.TARGET_RACE_FLAG_ORC, 45],
			nonplayer.RACE_FLAG_MILGYO: [localeInfo.TARGET_RACE_FLAG_MILGYO, 46],
			nonplayer.RACE_FLAG_INSECT: [localeInfo.TARGET_RACE_FLAG_INSECT, 49],
			nonplayer.RACE_FLAG_FIRE: [localeInfo.TARGET_RACE_FLAG_FIRE, 50],
			nonplayer.RACE_FLAG_ICE: [localeInfo.TARGET_RACE_FLAG_ICE, 51],
			nonplayer.RACE_FLAG_DESERT: [localeInfo.TARGET_RACE_FLAG_DESERT, 52],
			nonplayer.RACE_FLAG_TREE: [localeInfo.TARGET_RACE_FLAG_TREE, 58],
			nonplayer.RACE_FLAG_ATT_ELEC: [localeInfo.TARGET_RACE_FLAG_ATT_ELEC, 148],
			nonplayer.RACE_FLAG_ATT_FIRE: [localeInfo.TARGET_RACE_FLAG_ATT_FIRE, 149],
			nonplayer.RACE_FLAG_ATT_ICE: [localeInfo.TARGET_RACE_FLAG_ATT_ICE, 150],
			nonplayer.RACE_FLAG_ATT_WIND: [localeInfo.TARGET_RACE_FLAG_ATT_WIND, 151],
			nonplayer.RACE_FLAG_ATT_EARTH: [localeInfo.TARGET_RACE_FLAG_ATT_EARTH, 152],
			nonplayer.RACE_FLAG_ATT_DARK: [localeInfo.TARGET_RACE_FLAG_ATT_DARK, 153]
		}

		IMMUNE_FLAG_DICT = {
			nonplayer.IMMUNE_STUN: localeInfo.TARGET_IMMUNE_STUN,
			nonplayer.IMMUNE_SLOW: localeInfo.TARGET_IMMUNE_SLOW,
			nonplayer.IMMUNE_FALL: localeInfo.TARGET_IMMUNE_FALL,
			nonplayer.IMMUNE_CURSE: localeInfo.TARGET_IMMUNE_CURSE,
			nonplayer.IMMUNE_POISON: localeInfo.TARGET_IMMUNE_POISON,
			nonplayer.IMMUNE_TERROR: localeInfo.TARGET_IMMUNE_TERROR,
			nonplayer.IMMUNE_REFLECT: localeInfo.TARGET_IMMUNE_REFLECT
		}

		RESIST_DICT = {
			nonplayer.MOB_RESIST_SWORD: localeInfo.MOB_RESIST_SWORD,
			nonplayer.MOB_RESIST_TWOHAND: localeInfo.MOB_RESIST_TWOHAND,
			nonplayer.MOB_RESIST_DAGGER: localeInfo.MOB_RESIST_DAGGER,
			nonplayer.MOB_RESIST_BELL: localeInfo.MOB_RESIST_BELL,
			nonplayer.MOB_RESIST_FAN: localeInfo.MOB_RESIST_FAN,
			nonplayer.MOB_RESIST_BOW: localeInfo.MOB_RESIST_BOW,
			nonplayer.MOB_RESIST_FIRE: localeInfo.MOB_RESIST_FIRE,
			nonplayer.MOB_RESIST_ELECT: localeInfo.MOB_RESIST_ELECT,
			nonplayer.MOB_RESIST_MAGIC: localeInfo.MOB_RESIST_MAGIC,
			nonplayer.MOB_RESIST_WIND: localeInfo.MOB_RESIST_WIND,
			nonplayer.MOB_RESIST_POISON: localeInfo.MOB_RESIST_POISON
		}

	if app.__BL_MULTI_LANGUAGE__:
		@staticmethod
		def ReloadVariables():
			TargetBoard.BUTTON_NAME_LIST = (
				localeInfo.TARGET_BUTTON_WHISPER,
				localeInfo.TARGET_BUTTON_EXCHANGE,
				localeInfo.TARGET_BUTTON_FIGHT,
				localeInfo.TARGET_BUTTON_ACCEPT_FIGHT,
				localeInfo.TARGET_BUTTON_AVENGE,
				localeInfo.TARGET_BUTTON_FRIEND,
				localeInfo.TARGET_BUTTON_INVITE_PARTY,
				localeInfo.TARGET_BUTTON_LEAVE_PARTY,
				localeInfo.TARGET_BUTTON_EXCLUDE,
				localeInfo.TARGET_BUTTON_INVITE_GUILD,
				localeInfo.TARGET_BUTTON_DISMOUNT,
				localeInfo.TARGET_BUTTON_EXIT_OBSERVER,
				localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT,
				localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY,
				localeInfo.TARGET_BUTTON_BUILDING_DESTROY,
				localeInfo.TARGET_BUTTON_EMOTION_ALLOW,
				"VOTE_BLOCK_CHAT",
			)

			TargetBoard.GRADE_NAME = {
				nonplayer.PAWN : localeInfo.TARGET_LEVEL_PAWN,
				nonplayer.S_PAWN : localeInfo.TARGET_LEVEL_S_PAWN,
				nonplayer.KNIGHT : localeInfo.TARGET_LEVEL_KNIGHT,
				nonplayer.S_KNIGHT : localeInfo.TARGET_LEVEL_S_KNIGHT,
				nonplayer.BOSS : localeInfo.TARGET_LEVEL_BOSS,
				nonplayer.KING : localeInfo.TARGET_LEVEL_KING,
			}

	def __init__(self, interface):
		ui.ThinBoard.__init__(self)
		self.interface = interface

		name = ui.TextLine()
		name.SetParent(self)
		name.SetDefaultFontName()
		name.SetOutline()
		name.Show()

		if constInfo.ENABLE_ANIMATE_GUAGE:
			hpGauge = ui.AnimateGauge()
		else:
			hpGauge = ui.Gauge()
		hpGauge.SetParent(self)
		hpGauge.MakeGauge(130, "red")
		hpGauge.Hide()

		if app.ENABLE_MOB_HP:
			hpText = ui.TextLine()
			hpText.SetParent(hpGauge)
			hpText.SetPosition(2, -15)
			hpText.SetFontName("Tahoma:12")
			hpText.SetWindowHorizontalAlignCenter()
			hpText.SetHorizontalAlignCenter()
			hpText.SetOutline()
			hpText.Hide()

		if app.ENABLE_DROP_INFO:
			dropInfoButton = ui.Button()
			dropInfoButton.SetParent(self)
			dropInfoButton.SetUpVisual("d:/ymir work/ui/game/mobdrop/question_mark_1.tga")
			dropInfoButton.SetOverVisual("d:/ymir work/ui/game/mobdrop/question_mark_2.tga")
			dropInfoButton.SetDownVisual("d:/ymir work/ui/game/mobdrop/question_mark_3.tga")
			dropInfoButton.SetPosition(45, 13)
			dropInfoButton.SetWindowHorizontalAlignRight()
			dropInfoButton.SetEvent(ui.__mem_func__(self.__OnClickDropInfoButton))
			dropInfoButton.ShowToolTip = ui.__mem_func__(self.__OnDropInfoButtonOverIn)
			dropInfoButton.HideToolTip = ui.__mem_func__(self.__OnDropInfoButtonOverOut)
			dropInfoButton.Hide()
			self.dropInfoButton = dropInfoButton

			self.toolTipItem = uiToolTip.ItemToolTip()
			self.toolTipItem.HideToolTip()

			dropInfoBoard = DropInfoBoard(self.interface)
			dropInfoBoard.SetPosition((wndMgr.GetScreenWidth() - dropInfoBoard.GetWidth()) / 2, 60)
			dropInfoBoard.Hide()
			self.dropInfoBoard = dropInfoBoard

		if app.ENABLE_METIN_STONE_QUEUE:
			autoFarmText = ui.TextLine()
			autoFarmText.SetParent(self)
			autoFarmText.SetOutline()
			autoFarmText.SetHorizontalAlignCenter()
			autoFarmText.SetText("|Eemoji/key_shift|e + |Eemoji/key_rclick|e Target Metin Quque")
			autoFarmText.Hide()
			self.autoFarmText = autoFarmText

			autoFarmBtn = ui.Button()
			autoFarmBtn.SetParent(self)
			autoFarmBtn.SetUpVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
			autoFarmBtn.SetOverVisual("d:/ymir work/ui/pattern/q_mark_02.tga")
			autoFarmBtn.SetDownVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
			autoFarmBtn.SetEvent(ui.__mem_func__(self.__AutoFarmInfo))
			autoFarmBtn.Hide()
			self.autoFarmBtn = autoFarmBtn

			self.autoFarmInfo = None

		closeButton = ui.Button()
		closeButton.SetParent(self)
		closeButton.SetUpVisual("d:/ymir work/ui/public/close_button_01.sub")
		closeButton.SetOverVisual("d:/ymir work/ui/public/close_button_02.sub")
		closeButton.SetDownVisual("d:/ymir work/ui/public/close_button_03.sub")
		closeButton.SetPosition(30, 13)

		if localeInfo.IsARABIC():
			hpGauge.SetPosition(55, 17)
			hpGauge.SetWindowHorizontalAlignLeft()
			closeButton.SetWindowHorizontalAlignLeft()
		else:
			hpGauge.SetPosition(175, 17)
			hpGauge.SetWindowHorizontalAlignRight()
			closeButton.SetWindowHorizontalAlignRight()

		closeButton.SetEvent(ui.__mem_func__(self.OnPressedCloseButton))
		closeButton.Show()

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.toolTipCostumeHide = uiToolTip.ToolTip(160)
			self.toolTipCostumeHide.HideToolTip()

			if not app.ENABLE_PLAYER_HP:
				hideCostumeButton = ui.ImageBox()
				hideCostumeButton.SetParent(self)
				hideCostumeButton.SetPosition(32, 13)
				hideCostumeButton.SetWindowHorizontalAlignLeft()
				hideCostumeButton.LoadImage("d:/ymir work/ui/pattern/button_show_0.tga")
				hideCostumeButton.OnMouseOverIn = ui.__mem_func__(self.OnOverInHideCostumeButton)
				hideCostumeButton.OnMouseOverOut = ui.__mem_func__(self.OnOverOutHideCostumeButton)
				hideCostumeButton.Hide()

		self.buttonDict = {}
		self.showingButtonList = []
		for buttonName in self.BUTTON_NAME_LIST:
			button = ui.Button()
			button.SetParent(self)

			if localeInfo.IsARABIC():
				button.SetUpVisual("d:/ymir work/ui/public/Small_Button_01.sub")
				button.SetOverVisual("d:/ymir work/ui/public/Small_Button_02.sub")
				button.SetDownVisual("d:/ymir work/ui/public/Small_Button_03.sub")
			else:
				button.SetUpVisual("d:/ymir work/ui/public/small_thin_button_01.sub")
				button.SetOverVisual("d:/ymir work/ui/public/small_thin_button_02.sub")
				button.SetDownVisual("d:/ymir work/ui/public/small_thin_button_03.sub")

			button.SetWindowHorizontalAlignCenter()
			button.SetText(buttonName)
			button.Hide()
			self.buttonDict[buttonName] = button
			self.showingButtonList.append(button)

		self.buttonDict[localeInfo.TARGET_BUTTON_WHISPER].SetEvent(ui.__mem_func__(self.OnWhisper))
		self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE].SetEvent(ui.__mem_func__(self.OnExchange))
		self.buttonDict[localeInfo.TARGET_BUTTON_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_ACCEPT_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_AVENGE].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeInfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeInfo.TARGET_BUTTON_INVITE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyInvite))
		self.buttonDict[localeInfo.TARGET_BUTTON_LEAVE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyExit))
		self.buttonDict[localeInfo.TARGET_BUTTON_EXCLUDE].SetEvent(ui.__mem_func__(self.OnPartyRemove))

		self.buttonDict[localeInfo.TARGET_BUTTON_INVITE_GUILD].SAFE_SetEvent(self.__OnGuildAddMember)
		self.buttonDict[localeInfo.TARGET_BUTTON_DISMOUNT].SAFE_SetEvent(self.__OnDismount)
		self.buttonDict[localeInfo.TARGET_BUTTON_EXIT_OBSERVER].SAFE_SetEvent(self.__OnExitObserver)
		self.buttonDict[localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT].SAFE_SetEvent(self.__OnViewEquipment)
		self.buttonDict[localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY].SAFE_SetEvent(self.__OnRequestParty)
		self.buttonDict[localeInfo.TARGET_BUTTON_BUILDING_DESTROY].SAFE_SetEvent(self.__OnDestroyBuilding)
		self.buttonDict[localeInfo.TARGET_BUTTON_EMOTION_ALLOW].SAFE_SetEvent(self.__OnEmotionAllow)

		self.buttonDict["VOTE_BLOCK_CHAT"].SetEvent(ui.__mem_func__(self.__OnVoteBlockChat))

		if app.ENABLE_DROP_INFO:
			self.elementInfo = ElementalImage(self)
		self.name = name
		self.hpGauge = hpGauge
		if app.ENABLE_MOB_HP:
			self.hpText = hpText
		self.closeButton = closeButton
		self.nameString = 0
		self.nameLength = 0
		self.vid = 0
		self.eventWhisper = None
		self.isShowButton = False
		if constInfo.ENABLE_ANIMATE_GUAGE:
			self.initHP = False

		if app.ENABLE_PLAYER_HP:
			self.cirleBGImg = ui.ImageBox()
			self.cirleBGImg.LoadImage("d:/ymir work/ui/game/targetboard/circle_bg.tga")
			self.cirleBGImg.SetParent(self)
			self.cirleBGImg.SetWindowVerticalAlignCenter()
			self.cirleBGImg.SetPosition(-50, 0)

			self.cirleHPImg = ui.ImageBox()
			self.cirleHPImg.SetParent(self.cirleBGImg)
			self.cirleHPImg.SetPosition(0, 0)
			self.cirleHPImg.Hide()

			self.cirleRaceImg = ui.ImageBox()
			self.cirleRaceImg.SetParent(self.cirleBGImg)
			self.cirleRaceImg.SetPosition(0, 0)
			self.cirleRaceImg.Hide()

			## TODO
			self.hpTooltipHolder = ui.ImageBox()
			self.hpTooltipHolder.LoadImage("d:/ymir work/ui/pattern/seperator.tga")
			self.hpTooltipHolder.SetParent(self.cirleBGImg)
			self.hpTooltipHolder.SetPosition(-65, 60)
			self.hpTooltipHolder.Hide()

			self.tooltipHPText = ui.TextLine()
			self.tooltipHPText.SetParent(self.hpTooltipHolder)
			self.tooltipHPText.SetPosition(0, 20)
			self.tooltipHPText.SetFontName(localeInfo.UI_DEF_BOLD_FONT)
			self.tooltipHPText.SetText("69/69")
			self.tooltipHPText.SetWindowHorizontalAlignCenter()
			self.tooltipHPText.SetHorizontalAlignCenter()
			self.tooltipHPText.Show()
			
			self.pcHpText = ui.TextLine()
			self.pcHpText.SetParent(self.hpGauge)   # wichtig: Parent = TargetBoard
			self.pcHpText.SetPosition(-155, -8)   # LINKS neben dem Balken
			self.pcHpText.SetFontName("Tahoma:12")
			self.pcHpText.SetOutline()
			self.pcHpText.SetHorizontalAlignLeft()
			self.pcHpText.SetWindowHorizontalAlignLeft()
			self.pcHpText.Hide()

		if app.ENABLE_HIDE_COSTUME_SYSTEM and not app.ENABLE_PLAYER_HP:
			self.hideCostumeButton = hideCostumeButton

		self.__Initialize()
		self.ResetTargetBoard()

	def __del__(self):
		ui.ThinBoard.__del__(self)

		print "===================================================== DESTROYED TARGET BOARD"

	def __Initialize(self):
		self.nameString = ""
		self.nameLength = 0
		self.vid = 0
		self.isShowButton = False

	def Destroy(self):
		self.eventWhisper = None
		self.closeButton = None
		self.showingButtonList = None
		self.buttonDict = None
		self.name = None
		self.hpGauge = None
		if app.ENABLE_MOB_HP:
			self.hpText = None
		if app.ENABLE_DROP_INFO:
			self.dropInfoButton = None
			self.dropInfoBoard.Destroy()
			self.dropInfoBoard = None
			self.toolTipItem.ClearToolTip()
			self.toolTipItem = None
			self.elementInfo.Hide()
			self.elementInfo = None
		if app.ENABLE_PLAYER_HP:
			self.cirleBGImg = None
			self.cirleHPImg = None
			self.cirleRaceImg = None
			self.hpTooltipHolder = None
			self.tooltipHPText = None
		if app.ENABLE_HIDE_COSTUME_SYSTEM and not app.ENABLE_PLAYER_HP:
			self.hideCostumeButton = None

		self.__Initialize()

	def OnPressedCloseButton(self):
		player.ClearTarget()
		self.Close()

	def Close(self):
		if app.ENABLE_PLAYER_HP and app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.toolTipCostumeHide.HideToolTip()
		if app.ENABLE_DROP_INFO:
			self.dropInfoBoard.Hide()
			self.elementInfo.Hide()
		if app.ENABLE_METIN_STONE_QUEUE:
			if self.autoFarmInfo:
				self.autoFarmInfo.Close()
				self.autoFarmInfo=None
		self.__Initialize()
		self.Hide()

	def Open(self, vid, name, hpPercentage, curHP, maxHP):
		if vid:
			if not constInfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
				if not player.IsSameEmpire(vid):
					self.Hide()
					return

			if vid != self.GetTargetVID():
				self.ResetTargetBoard()
				self.SetTargetVID(vid)
				self.SetTargetName(name)

			if player.IsMainCharacterIndex(vid):
				self.__ShowMainCharacterMenu()
			elif chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
				self.Hide()
			else:
				self.RefreshButton()
				self.Show()
		else:
			self.HideAllButton()
			self.__ShowButton(localeInfo.TARGET_BUTTON_WHISPER)
			self.__ShowButton("VOTE_BLOCK_CHAT")
			self.__ArrangeButtonPosition()
			self.SetTargetName(name)
			self.Show()

	def Refresh(self):
		if self.IsShow():
			if self.IsShowButton():
				self.RefreshButton()

	def RefreshByVID(self, vid):
		if vid == self.GetTargetVID():
			self.Refresh()

	def RefreshByName(self, name):
		if name == self.GetTargetName():
			self.Refresh()

	def __ShowMainCharacterMenu(self):
		canShow=0

		self.HideAllButton()

		if player.IsMountingHorse():
			self.__ShowButton(localeInfo.TARGET_BUTTON_DISMOUNT)
			canShow=1
			self.name.SetPosition(0, 10)   # X, Y -> Y kleiner = höher
			self.name.SetHorizontalAlignCenter()
			self.name.SetWindowHorizontalAlignCenter()

		if player.IsObserverMode():
			self.__ShowButton(localeInfo.TARGET_BUTTON_EXIT_OBSERVER)
			canShow=1

		if canShow:
			self.__ArrangeButtonPosition()
			self.Show()
		else:
			self.Hide()

	def __ShowNameOnlyMenu(self):
		self.HideAllButton()

	def SetWhisperEvent(self, event):
		self.eventWhisper = event

	def UpdatePosition(self):
		if app.ENABLE_MAINTENANCE_SYSTEM:
			if self.interface and self.interface.wndMaintenance:
				if self.interface.wndMaintenance.IsShow():
					if app.ENABLE_PLAYER_HP:
						self.SetPosition(wndMgr.GetScreenWidth()/2 - self.GetWidth()/2 + 25, 42)
					else:
						self.SetPosition(wndMgr.GetScreenWidth()/2 - self.GetWidth()/20, 32)
					if app.ENABLE_DROP_INFO:
						self.elementInfo.SetPosition(self.GetGlobalPosition()[0]-self.elementInfo.GetWidth()-30, 32 + (self.GetHeight()-self.elementInfo.GetHeight())/2)
					return

		if app.ENABLE_PLAYER_HP:
			self.SetPosition(wndMgr.GetScreenWidth()/2 - self.GetWidth()/2 + 25, 10)
		else:
			self.SetPosition(wndMgr.GetScreenWidth()/2 - self.GetWidth()/2, 10)

		if app.ENABLE_DROP_INFO:
			self.elementInfo.SetPosition(self.GetGlobalPosition()[0]-self.elementInfo.GetWidth()-30, 10 + (self.GetHeight()-self.elementInfo.GetHeight())/2)

		if app.ENABLE_METIN_STONE_QUEUE:
			if chr.GetInstanceType(self.vid) == chr.INSTANCE_TYPE_STONE:
				self.SetSize(self.GetWidth(), self.GetHeight() + 20)

				self.autoFarmText.SetPosition(self.GetWidth() / 2, 30)
				self.autoFarmText.Show()

				self.autoFarmBtn.SetPosition((self.GetWidth() / 2) + (self.autoFarmText.GetTextSize()[1]/2) + 85, 30)
				self.autoFarmBtn.Show()
			else:
				self.autoFarmText.Hide()
				self.autoFarmBtn.Hide()
				if self.autoFarmInfo:
					self.autoFarmInfo.Close()

	def ResetTargetBoard(self):

		for btn in self.buttonDict.values():
			btn.Hide()

		self.__Initialize()

		self.name.SetPosition(1, 0)
		self.name.SetHorizontalAlignCenter()
		self.name.SetWindowHorizontalAlignCenter()
		self.hpGauge.Hide()
		if app.ENABLE_MOB_HP:
			self.hpText.Hide()
		if app.ENABLE_DROP_INFO:
			self.dropInfoButton.Hide()
			self.dropInfoBoard.Hide()
			self.elementInfo.Close()
		if app.ENABLE_HIDE_COSTUME_SYSTEM and not app.ENABLE_PLAYER_HP:
			self.hideCostumeButton.Hide()
		self.SetSize(250, 40)

	def SetTargetVID(self, vid):
		self.vid = vid

	def SetEnemyVID(self, vid):
		if constInfo.ENABLE_ANIMATE_GUAGE:
			if self.GetTargetVID() == vid:
				return

			self.initHP = True
		self.SetTargetVID(vid)

		name = chr.GetNameByVID(vid)
		level = nonplayer.GetLevelByVID(vid)
		grade = nonplayer.GetGradeByVID(vid)
		if app.ENABLE_DROP_INFO:
			vnum = nonplayer.GetRaceNumByVID(vid)

		nameFront = ""
		if -1 != level:
			nameFront += "Lv." + str(level) + " "
		if self.GRADE_NAME.has_key(grade):
			nameFront += "(" + self.GRADE_NAME[grade] + ") "

		self.SetTargetName(nameFront + name)
		if app.ENABLE_DROP_INFO:
			if vnum >= 101:
				self.dropInfoButton.Show()

	def GetTargetVID(self):
		return self.vid

	def GetTargetName(self):
		return self.nameString

	def SetTargetName(self, name):
		self.nameString = name
		self.nameLength = len(name)
		self.name.SetText(name)

	def SetHP(self, hpPercentage, curHP, maxHP):
		if not self.hpGauge.IsShow():

			self.SetSize(260 + 7*self.nameLength, self.GetHeight())

			if localeInfo.IsARABIC():
				self.name.SetPosition( self.GetWidth()-23, 13)
			else:
				self.name.SetPosition(23, 13)

			self.name.SetWindowHorizontalAlignLeft()
			self.name.SetHorizontalAlignLeft()
			self.hpGauge.Show()
			if app.ENABLE_MOB_HP:
				self.hpText.Show()
			self.UpdatePosition()

		if app.ENABLE_DROP_INFO:
			self.elementInfo.LoadElement(self.vid)
		self.hpGauge.SetPercentage(hpPercentage, 100)
		
		# >>> SPIELER HP TEXT <<<
		if app.ENABLE_PLAYER_HP:
			if hasattr(self, "pcHpText") and self.pcHpText:
				self.pcHpText.Hide()
				self.pcHpText.SetText("%s / %s" % (
					localeInfo.DottedNumber(curHP),
					localeInfo.DottedNumber(maxHP)
				))

		if constInfo.ENABLE_ANIMATE_GUAGE:
			self.hpGauge.SetEasingPercentage(hpPercentage, 100)
			if self.initHP:
				self.hpGauge.SetBGPercentage(hpPercentage, 100)
				self.initHP = False
			else:
				self.hpGauge.SetEasingPercentage(hpPercentage, 100)

		if app.ENABLE_MOB_HP:
			if hpPercentage > 60 : color = ui.GenerateColor(*colorInfo.CHAT_RGB_SHOUT)
			elif hpPercentage > 24 : color = ui.GenerateColor(*colorInfo.CHAT_RGB_NOTICE)
			elif hpPercentage < 25 : color = ui.GenerateColor(*colorInfo.TITLE_RGB_EVIL_4)

			self.hpText.SetPackedFontColor(color)
			# self.hpText.SetText(localeInfo.DottedNumber(curHP) + "/" + localeInfo.DottedNumber(maxHP))
			self.hpText.SetText(self.__GetTargetActualHP(hpPercentage, curHP))

		if app.ENABLE_PLAYER_HP:
			self.cirleBGImg.Hide()
			self.SetPosition(wndMgr.GetScreenWidth()/2 - self.GetWidth()/2, 10)

	def __DottedFormat(self, n, char='.'):
		return '{:,}'.format(n).replace(',', char)

	def __GetTargetActualHP(self, percHp, iActualW = 0):
		race = chr.GetRaceByVID(self.GetTargetVID())
		if race == 0:
			return "{}/{} {}%".format(0, 0, percHp)
		
		if percHp == 100:
			maxPrint = self.__DottedFormat(nonplayer.GetMonsterMaxHP(race))
			return "{}/{} ({}%)".format(maxPrint, maxPrint, percHp)
			
		maxHp = nonplayer.GetMonsterMaxHP(race)
		iActual = (maxHp/100)*percHp
		if iActualW != 0:
			iActual = iActualW
		
		hpPrint = self.__DottedFormat(iActual)
		maxPrint = self.__DottedFormat(maxHp)
		return "{}/{} ({}%)".format(hpPrint, maxPrint, percHp)

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def GetInfoHide(self,index):
			if index == 0:
				return localeInfo.COSTUME_HIDE_ON

			return localeInfo.COSTUME_HIDE_OFF	

		def SetCostumeHide(self, vid, hair, body, weapon, acce):
			self.toolTipCostumeHide.ClearToolTip()
			self.toolTipCostumeHide.SetTitle(localeInfo.COSTUME_HIDE_STATUS)
			self.toolTipCostumeHide.AppendTextLine(localeInfo.COSTUME_HIDE_HAIR.format(self.GetInfoHide(hair)))
			self.toolTipCostumeHide.AppendTextLine(localeInfo.COSTUME_HIDE_ARMOR.format(self.GetInfoHide(body)))
			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.toolTipCostumeHide.AppendTextLine(localeInfo.COSTUME_HIDE_WEAPON.format(self.GetInfoHide(weapon)))
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				self.toolTipCostumeHide.AppendTextLine(localeInfo.COSTUME_HIDE_ACCE.format(self.GetInfoHide(acce)))
			# if app.ENABLE_AURA_COSTUME_SYSTEM:
			# 	self.toolTipCostumeHide.AppendTextLine(localeInfo.COSTUME_HIDE_AURA.format(self.GetInfoHide(aura)))

		if not app.ENABLE_PLAYER_HP:
			def OnOverInHideCostumeButton(self):
				self.toolTipCostumeHide.ShowToolTip()

			def OnOverOutHideCostumeButton(self):
				self.toolTipCostumeHide.HideToolTip()

	def ShowDefaultButton(self):

		self.isShowButton = True
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_WHISPER])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_FIGHT])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_EMOTION_ALLOW])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT])
		for button in self.showingButtonList:
			button.Show()

	def HideAllButton(self):
		self.isShowButton = False
		for button in self.showingButtonList:
			button.Hide()
		self.showingButtonList = []

	def __ShowButton(self, name):

		if not self.buttonDict.has_key(name):
			return

		self.buttonDict[name].Show()
		self.showingButtonList.append(self.buttonDict[name])

	def __HideButton(self, name):

		if not self.buttonDict.has_key(name):
			return

		button = self.buttonDict[name]
		button.Hide()

		for btnInList in self.showingButtonList:
			if btnInList == button:
				self.showingButtonList.remove(button)
				break

	def OnWhisper(self):
		if None != self.eventWhisper:
			self.eventWhisper(self.nameString)

	def OnExchange(self):
		net.SendExchangeStartPacket(self.vid)

	def OnPVP(self):
		net.SendChatPacket("/pvp %d" % (self.vid))

	def DuelNormal(self): 
		constInfo.charmeine_duel = 0
		self.Button1.Hide()
		self.choix.Hide()
		self.Button2.Hide()
		net.SendChatPacket("/pvp %d 0 0" % (self.vid)) # cu critica

	def DuelRomanian(self):
		constInfo.charmeine_duel = 0
		self.Button1.Hide()
		self.Button2.Hide()
		self.choix.Hide()
		net.SendChatPacket("/pvp %d 1 1" % (self.vid))
		

	def DuelGerman(self):
		constInfo.charmeine_duel = 0
		self.Button1.Hide()
		self.Button2.Hide()
		self.choix.Hide()
		net.SendChatPacket("/pvp %d 1 2" % (self.vid))
		

	def DuelAvansat(self):
		constInfo.charmeine_duel = 0
		self.Button1.Hide()
		self.Button2.Hide()
		self.choix.Hide()
		net.SendChatPacket("/pvp %d 1" % (self.vid)) # fara critica
		

	def OnPVPaccept(self):
		net.SendChatPacket("/pvp %d" % (self.vid))

	def OnAppendToMessenger(self):
		net.SendMessengerAddByVIDPacket(self.vid)

	def OnPartyInvite(self):
		net.SendPartyInvitePacket(self.vid)

	def OnPartyExit(self):
		net.SendPartyExitPacket()

	def OnPartyRemove(self):
		net.SendPartyRemovePacketVID(self.vid)

	def __OnGuildAddMember(self):
		net.SendGuildAddMemberPacket(self.vid)

	def __OnDismount(self):
		net.SendChatPacket("/unmount")

	def __OnExitObserver(self):
		net.SendChatPacket("/observer_exit")

	def __OnViewEquipment(self):
		net.SendChatPacket("/view_equip " + str(self.vid))

	def __OnRequestParty(self):
		net.SendChatPacket("/party_request " + str(self.vid))

	def __OnDestroyBuilding(self):
		net.SendChatPacket("/build d %d" % (self.vid))

	def __OnEmotionAllow(self):
		net.SendChatPacket("/emotion_allow %d" % (self.vid))

	def __OnVoteBlockChat(self):
		cmd = "/vote_block_chat %s" % (self.nameString)
		net.SendChatPacket(cmd)

	def OnPressEscapeKey(self):
		self.OnPressedCloseButton()
		return True

	def IsShowButton(self):
		return self.isShowButton

	def RefreshButton(self):

		self.HideAllButton()

		if chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
			#self.__ShowButton(localeInfo.TARGET_BUTTON_BUILDING_DESTROY)
			#self.__ArrangeButtonPosition()
			return

		if player.IsPVPInstance(self.vid) or player.IsObserverMode():
			# PVP_INFO_SIZE_BUG_FIX
			self.SetSize(200 + 7*self.nameLength, 40)
			self.UpdatePosition()
			# END_OF_PVP_INFO_SIZE_BUG_FIX
			return

		self.ShowDefaultButton()

		if guild.MainPlayerHasAuthority(guild.AUTH_ADD_MEMBER):
			if not guild.IsMemberByName(self.nameString):
				if 0 == chr.GetGuildID(self.vid):
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_GUILD)

		if not messenger.IsFriendByName(self.nameString):
			self.__ShowButton(localeInfo.TARGET_BUTTON_FRIEND)

		if player.IsPartyMember(self.vid):

			self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

			if player.IsPartyLeader(self.vid):
				self.__ShowButton(localeInfo.TARGET_BUTTON_LEAVE_PARTY)
			elif player.IsPartyLeader(player.GetMainCharacterIndex()):
				self.__ShowButton(localeInfo.TARGET_BUTTON_EXCLUDE)

		else:
			if player.IsPartyMember(player.GetMainCharacterIndex()):
				if player.IsPartyLeader(player.GetMainCharacterIndex()):
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)
			else:
				if chr.IsPartyMember(self.vid):
					self.__ShowButton(localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY)
				else:
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)

			if player.IsRevengeInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeInfo.TARGET_BUTTON_AVENGE)
			elif player.IsChallengeInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeInfo.TARGET_BUTTON_ACCEPT_FIGHT)
			elif player.IsCantFightInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

			if not player.IsSameEmpire(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)
				self.__HideButton(localeInfo.TARGET_BUTTON_FRIEND)
				# self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

		distance = player.GetCharacterDistance(self.vid)
		if distance > self.EXCHANGE_LIMIT_RANGE:
			self.__HideButton(localeInfo.TARGET_BUTTON_EXCHANGE)
			self.__ArrangeButtonPosition()

		self.__ArrangeButtonPosition()

	def __ArrangeButtonPosition(self):
		showingButtonCount = len(self.showingButtonList)

		pos = -(showingButtonCount / 2) * 68
		if 0 == showingButtonCount % 2:
			pos += 34

		for button in self.showingButtonList:
			button.SetPosition(pos, 33)
			pos += 68

		self.SetSize(max(150, showingButtonCount * 75), 55)
		self.UpdatePosition()

	if app.ENABLE_DROP_INFO:
		def __OnClickDropInfoButton(self):
			if self.dropInfoBoard.IsShow():
				self.dropInfoBoard.Hide()
				return

			vnum = chr.GetVirtualNumber(self.GetTargetVID())

			data = constInfo.dropInfoDict.get(vnum)
			if data:
				self.DropInfoRefresh(vnum)
			else:
				net.SendDropInfoPacket()

		def DropInfoRefresh(self, mob_vnum):
			self.dropInfoBoard.Show()
			self.dropInfoBoard.Refresh(mob_vnum)

		def DropInfoClear(self):
			if self.dropInfoBoard.IsShow():
				net.SendDropInfoPacket()

		def __OnDropInfoButtonOverIn(self):
			vid = self.GetTargetVID()

			raceflag = nonplayer.GetRaceFlagByVID(vid)
			self.toolTipItem.ClearToolTip()
			self.toolTipItem.ShowToolTip()
			if not chr.IsStone(vid):
				added = False
				self.toolTipItem.AppendTextLine(localeInfo.TARGET_RACE_TEXT)
				for flag, data in self.RACE_FLAG_DICT.items():
					if raceflag & flag:
						self.toolTipItem.AppendTextLine("* %s (+%d%%)" % (data[0], player.GetStatus(data[1])))
						added = True

				if not added:
					self.toolTipItem.AppendTextLine(localeInfo.TARGET_RACE_NON)

				immuneflag = nonplayer.GetImmuneFlagByVID(vid)
				resistflag = nonplayer.GetResistByVID(vid)

				self.toolTipItem.AppendHorizontalLine()
				self.toolTipItem.AppendTextLine(localeInfo.TARGET_RESIST_TEXT)

				added = False
				for flag, text in self.IMMUNE_FLAG_DICT.items():
					if flag == nonplayer.IMMUNE_POISON:
						continue

					if immuneflag & flag:
						self.toolTipItem.AppendTextLine("* %s" % text)
						added = True

				for resist, text in self.RESIST_DICT.items():
					if resistflag[resist] > 0:
						self.toolTipItem.AppendTextLine("* %s (%d%%)" % (text, resistflag[resist]))
						added = True

				if not added:
					self.toolTipItem.AppendTextLine(localeInfo.TARGET_RACE_NON)

				if app.ENABLE_EMOJI_SYSTEM:
					self.toolTipItem.AppendTextLine(localeInfo.DROP_INFO_TOOLTIP_INFO)
			else:
				if app.ENABLE_EMOJI_SYSTEM:
					self.toolTipItem.AppendTextLine(localeInfo.DROP_INFO_TOOLTIP_INFO)

		def __OnDropInfoButtonOverOut(self):
			self.toolTipItem.HideToolTip()

	def OnUpdate(self):
		if self.isShowButton:

			exchangeButton = self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE]
			distance = player.GetCharacterDistance(self.vid)

			if distance < 0:
				return

			if exchangeButton.IsShow():
				if distance > self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()

			else:
				if distance < self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()

		if app.ENABLE_PLAYER_HP:
			if self.cirleBGImg and self.cirleBGImg.IsShow():
				self.cirleHPImg.LoadImage("d:/ymir work/ui/game/targetboard/circle_red.tga")

				radius = 39
				mouse = mouseModule.mouseController
				x, y = self.cirleBGImg.GetGlobalPosition()
				posX = mouse.x - (x + radius)
				posY = mouse.y - (y + radius)

				if app.ENABLE_HIDE_COSTUME_SYSTEM:
					if pow(posX, 2) + pow(posY, 2) <= pow(radius, 2):
						self.toolTipCostumeHide.ShowToolTip()
					else:
						self.toolTipCostumeHide.HideToolTip()

	if app.ENABLE_PLAYER_HP:
		def SetTargetHP(self, hpPercent, isPC, curHP, maxHP):
			if isPC:
				# Kreis aus
				if self.cirleBGImg:
					self.cirleBGImg.Hide()

			# Balken an
			if not self.hpGauge.IsShow():
				self.hpGauge.Show()

			# Balken füllen
			self.hpGauge.SetPercentage(hpPercent, 100)
			if isPC:
				self.pcHpText.SetText("%s / %s" % (localeInfo.DottedNumber(curHP), localeInfo.DottedNumber(maxHP)))
				self.pcHpText.Show()
			else:
				self.pcHpText.Hide()

			# wie vorher
			self.cirleBGImg.Hide()

			##Calculate HP percent
			if hpPercent > 60 : color = ui.GenerateColor(*colorInfo.CHAT_RGB_SHOUT)
			elif hpPercent > 24 : color = ui.GenerateColor(*colorInfo.CHAT_RGB_NOTICE)
			elif hpPercent < 25 : color = ui.GenerateColor(*colorInfo.TITLE_RGB_EVIL_4)

			##Convert HP
			# if curHP > 10000 : curHP = "{:,}".format(curHP/1000).replace(',','.') + "k"		
			# if maxHP > 10000 : maxHP = "{:,}".format(maxHP/1000).replace(',','.') + "k"
			# tooltipHPText = str("%s / %s" % (str(curHP), str(maxHP)))

			self.tooltipHPText.SetPackedFontColor(color)
			#self.tooltipHPText.SetText(tooltipHPText)
			self.tooltipHPText.SetText("%s / %s" % (localeInfo.DottedNumber(curHP), localeInfo.DottedNumber(maxHP)))

			if player.IsMainCharacterIndex(self.vid) and player.IsMountingHorse():
				self.pcHpText.Hide()   # <-- HIER Text ausblenden beim Reiten
			else:
				self.pcHpText.SetText("%s / %s" % (
					localeInfo.DottedNumber(curHP),
					localeInfo.DottedNumber(maxHP)
				))
				self.pcHpText.Show()

			if player.IsMainCharacterIndex(self.vid) and player.IsMountingHorse():
				if self.hpGauge:
					self.hpGauge.Hide()
				if hasattr(self, "pcHpText"):
					self.pcHpText.Hide()
				if hasattr(self, "hpText"):
					self.hpText.Hide()
				return

	if app.ENABLE_METIN_STONE_QUEUE:
		def __AutoFarmInfo(self):
			if not self.autoFarmInfo:
				self.autoFarmInfo = AutoFarmInfoWindow()
			if self.autoFarmInfo.IsShow():
				self.autoFarmInfo.Close()
			else:
				self.autoFarmInfo.Open()
				(x, y) = self.autoFarmBtn.GetGlobalPosition()
				self.autoFarmInfo.SetPosition(x + 10, y + 30)
