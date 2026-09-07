# -*- coding: utf-8 -*-
import player
import exchange
import net
import app
import localeInfo
import chat
import item
import ui
import mouseModule
import uiPickMoney
import wndMgr

if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
	import playerSettingModule
	import time
	import uiCommon

if app.WJ_ENABLE_TRADABLE_ICON:
	INVENTORY_PAGE_SIZE = player.INVENTORY_PAGE_SIZE
	if app.ENABLE_SPECIAL_INVENTORY:
		SPECIAL_INVENTORY_PAGE_SIZE = player.SPECIAL_INVENTORY_PAGE_SIZE

		INVENTORY_TYPE_SKILLBOOK = player.INVENTORY_TYPE_SKILLBOOK
		INVENTORY_TYPE_STONE = player.INVENTORY_TYPE_STONE
		INVENTORY_TYPE_MATERIAL = player.INVENTORY_TYPE_MATERIAL

		SKILLBOOK_INVENTORY_SLOT_START = player.SKILLBOOK_INVENTORY_SLOT_START
		STONE_INVENTORY_SLOT_START = player.STONE_INVENTORY_SLOT_START
		MATERIAL_INVENTORY_SLOT_START = player.MATERIAL_INVENTORY_SLOT_START

###################################################################################################
## Exchange
class ExchangeDialog(ui.ScriptWindow):
	if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
		FACE_IMAGE_DICT = {
			playerSettingModule.RACE_WARRIOR_M	: "icon/face/warrior_m.tga",
			playerSettingModule.RACE_WARRIOR_W	: "icon/face/warrior_w.tga",
			playerSettingModule.RACE_ASSASSIN_M	: "icon/face/assassin_m.tga",
			playerSettingModule.RACE_ASSASSIN_W	: "icon/face/assassin_w.tga",
			playerSettingModule.RACE_SURA_M		: "icon/face/sura_m.tga",
			playerSettingModule.RACE_SURA_W		: "icon/face/sura_w.tga",
			playerSettingModule.RACE_SHAMAN_M	: "icon/face/shaman_m.tga",
			playerSettingModule.RACE_SHAMAN_W	: "icon/face/shaman_w.tga",
		}
		if app.ENABLE_WOLFMAN_CHARACTER:
			FACE_IMAGE_DICT.update({playerSettingModule.RACE_WOLFMAN_M  : "icon/face/wolfman_m.tga",})
		FACE_IMAGE_DICT.update({10  : "d:/ymir work/ui/polymorph_race.tga",})

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.TitleName = 0
		self.tooltipItem = 0
		self.xStart = 0
		self.yStart = 0

		if app.WJ_ENABLE_TRADABLE_ICON or app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			self.interface = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.inven = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
		class Item(ui.ListBoxEx.Item):
			def __init__(self,parent, text, value=0):
				ui.ListBoxEx.Item.__init__(self)
				self.textBox = ui.TextLine()
				self.textBox.SetParent(self)
				if localeInfo.IsARABIC():
					self.textBox.SetWindowHorizontalAlignRight()
					self.textBox.SetHorizontalAlignLeft()
				self.textBox.SetText(text)
				self.textBox.Show()
				self.value = value

			def GetValue(self):
				return self.value

			def __del__(self):
				ui.ListBoxEx.Item.__del__(self)

	def LoadDialog(self):
		PythonScriptLoader = ui.PythonScriptLoader()
		if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			PythonScriptLoader.LoadScriptFile(self, "UIScript/exchangedialogex.py")
		else:
			PythonScriptLoader.LoadScriptFile(self, "UIScript/exchangedialog.py")

		## Owner
		self.OwnerSlot = self.GetChild("Owner_Slot")
		self.OwnerSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectOwnerEmptySlot))
		self.OwnerSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectOwnerItemSlot))
		self.OwnerSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInOwnerItem))
		self.OwnerSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.OwnerMoney = self.GetChild("Owner_Money_Value")
		if not app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			self.OwnerAcceptLight = self.GetChild("Owner_Accept_Light")
			self.OwnerAcceptLight.Disable()
		self.OwnerMoneyButton = self.GetChild("Owner_Money")
		self.OwnerMoneyButton.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog))

		## Target
		self.TargetSlot = self.GetChild("Target_Slot")
		self.TargetSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInTargetItem))
		self.TargetSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.TargetMoney = self.GetChild("Target_Money_Value")
		if not app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			self.TargetAcceptLight = self.GetChild("Target_Accept_Light")
			self.TargetAcceptLight.Disable()

		## PickMoneyDialog
		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
		dlgPickMoney.SetTitleName(localeInfo.EXCHANGE_MONEY)
		if app.ENABLE_GOLD_MAX_EXTENDED:
			dlgPickMoney.SetMax(14)
		else:
			dlgPickMoney.SetMax(9) #@warme001 7 to 9
		dlgPickMoney.Hide()
		self.dlgPickMoney = dlgPickMoney

		## Button
		self.AcceptButton = self.GetChild("Owner_Accept_Button")
		self.AcceptButton.SetToggleDownEvent(ui.__mem_func__(self.AcceptExchange))

		self.TitleName = self.GetChild("TitleName")
		self.GetChild("TitleBar").SetCloseEvent(net.SendExchangeExitPacket)
		if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			self.TargetAcceptButton = self.GetChild("Target_Accept_Button")
			self.FaceOwnerImage = self.GetChild("FaceOwner_Image")
			self.FaceTargetImage = self.GetChild("FaceTarget_Image")
			self.TargetName = self.GetChild("target_NameText")
			self.TargetLevel = self.GetChild("target_LvText")
			self.ExchangeLogs = self.GetChild("ExchangeLogs")

			self.LogsScrollBar = ui.ThinScrollBar()
			self.LogsScrollBar.SetParent(self.ExchangeLogs)
			self.LogsScrollBar.SetPosition(442 - 75, 17)
			self.LogsScrollBar.SetScrollBarSize(50)
			self.LogsScrollBar.Show()

			self.LogsDropList = ui.ListBoxEx()
			self.LogsDropList.SetParent(self.ExchangeLogs)
			self.LogsDropList.itemHeight = 12
			self.LogsDropList.itemStep = 13
			if localeInfo.IsARABIC():
				self.LogsDropList.SetPosition(self.GetWidth() - self.LogsDropList.GetWidth() - 30, 27)
			else:
				self.LogsDropList.SetPosition(35, 27)
			self.LogsDropList.SetSize(0, 45)
			self.LogsDropList.SetScrollBar(self.LogsScrollBar)
			self.LogsDropList.SetViewItemCount(2)
			self.LogsDropList.Show()

			self.listOwnerSlot = []
			self.listTargetSlot = []

	def Destroy(self):
		print "---------------------------------------------------------------------------- DESTROY EXCHANGE"
		self.ClearDictionary()
		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0
		self.OwnerSlot = 0
		self.OwnerMoney = 0
		self.OwnerMoneyButton = 0
		self.TargetSlot = 0
		self.TargetMoney = 0
		self.TitleName = 0
		self.AcceptButton = 0
		self.tooltipItem = 0
		if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			self.TargetAcceptButton = 0
			self.FaceOwnerImage = None
			self.FaceTargetImage = None
			self.TargetName = None
			self.TargetLevel = None
			self.ExchangesLogsWindow = None
			self.LogsDropList.RemoveAllItems()
			self.LogsScrollBar = None
			self.LogsDropList = None
		else:
			self.OwnerAcceptLight = 0
			self.TargetAcceptLight = 0

		if app.WJ_ENABLE_TRADABLE_ICON or app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			self.interface = 0

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.inven = None

	def OpenDialog(self):
		if app.ENABLE_LEVEL_IN_TRADE:
			self.TitleName.SetText(localeInfo.EXCHANGE_TITLE_LEVEL % (exchange.GetNameFromTarget(), exchange.GetLevelFromTarget()))
		else:
			self.TitleName.SetText(localeInfo.EXCHANGE_TITLE % (exchange.GetNameFromTarget()))
		self.AcceptButton.Enable()
		self.AcceptButton.SetUp()

		ownerRace = exchange.GetRaceFromSelf()
		if ownerRace > 8:
			ownerRace= 10

		targetRace = exchange.GetRaceFromTarget()
		if targetRace > 8:
			targetRace = 10

		if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			self.TargetAcceptButton.Disable()
			self.TargetAcceptButton.SetUp()
			self.FaceOwnerImage.LoadImage(self.FACE_IMAGE_DICT[ownerRace])
			self.FaceTargetImage.LoadImage(self.FACE_IMAGE_DICT[targetRace])
			self.TargetName.SetText(exchange.GetNameFromTarget())
			self.TargetLevel.SetText(localeInfo.NEW_EXCHANGE_LEVEL % (exchange.GetLevelFromTarget()))
			self.LogsDropList.RemoveAllItems()
			self.LogsDropList.AppendItem(self.Item(self, localeInfo.NEW_EXCHANGE_YOU_READY % (str(time.strftime("%H:%M:%S"))), 0))

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.SetTop()
		self.Show()

		(self.xStart, self.yStart, z) = player.GetMainCharacterPosition()

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.ItemListIdx = []

	def CloseDialog(self):
		wndMgr.OnceIgnoreMouseLeftButtonUpEvent()

		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

		self.dlgPickMoney.Close()
		self.Hide()
		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.interface:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

			self.ItemListIdx = None

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def OpenPickMoneyDialog(self):

		if exchange.GetElkFromSelf() > 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXCHANGE_CANT_EDIT_MONEY)
			return

		self.dlgPickMoney.Open(player.GetElk())

	def OnPickMoney(self, money):
		if app.ENABLE_GOLD_MAX_EXTENDED:
			net.SendExchangeElkAddPacket(long(money))
		else:
			net.SendExchangeElkAddPacket(money)

	if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
		def AcceptExchange(self):
			atLeastOneItem = 0
			atLeastOneYang = 0
			for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
				itemCount = exchange.GetItemCountFromTarget(i)
				if itemCount >= 1:
					atLeastOneYang = 1
					break
			
			if exchange.GetElkFromTarget() >= 1:
				atLeastOneYang = 1
			
			if atLeastOneItem or atLeastOneYang:
				net.SendExchangeAcceptPacket()
				self.AcceptButton.Disable()
			else:
				atLeastOneItem = 0
				atLeastOneYang = 0
				for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
					itemCount = exchange.GetItemCountFromSelf(i)
					if itemCount >= 1:
						atLeastOneYang = 1
						break
				
				if exchange.GetElkFromSelf() >= 1:
					atLeastOneYang = 1
				
				if atLeastOneItem or atLeastOneYang:
					self.questionDialog = uiCommon.QuestionDialog2()
					self.questionDialog.SetText1(localeInfo.NEW_EXCHANGE_ALERT1)
					self.questionDialog.SetText2(localeInfo.NEW_EXCHANGE_ALERT2)
					self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.AcceptQuestion))
					self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
					self.questionDialog.Open()
				else:
					net.SendExchangeAcceptPacket()
					self.AcceptButton.Disable()

		def AcceptQuestion(self):
			net.SendExchangeAcceptPacket()
			self.AcceptButton.Disable()
			if self.questionDialog:
				self.questionDialog.Close()
			
			self.questionDialog = None

		def OnCloseQuestionDialog(self):
			if self.questionDialog:
				self.questionDialog.Close()
			
			self.questionDialog = None
			self.AcceptButton.Enable()
			self.AcceptButton.SetUp()
	else:
		def AcceptExchange(self):
			net.SendExchangeAcceptPacket()
			self.AcceptButton.Disable()

	def SelectOwnerEmptySlot(self, SlotIndex):

		if False == mouseModule.mouseController.isAttached():
			return

		if mouseModule.mouseController.IsAttachedMoney():
			if app.ENABLE_GOLD_MAX_EXTENDED:
				net.SendExchangeElkAddPacket(long(mouseModule.mouseController.GetAttachedMoneyAmount()))
			else:
				net.SendExchangeElkAddPacket(mouseModule.mouseController.GetAttachedMoneyAmount())
		else:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			if (player.SLOT_TYPE_INVENTORY == attachedSlotType
				or player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType):

				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				SrcSlotNumber = mouseModule.mouseController.GetAttachedSlotNumber()
				DstSlotNumber = SlotIndex

				itemID = player.GetItemIndex(attachedInvenType, SrcSlotNumber)
				item.SelectItem(itemID)

				if item.IsAntiFlag(item.ANTIFLAG_GIVE):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.EXCHANGE_CANNOT_GIVE)
					mouseModule.mouseController.DeattachObject()
					return

				net.SendExchangeItemAddPacket(attachedInvenType, SrcSlotNumber, DstSlotNumber)
				if app.WJ_ENABLE_TRADABLE_ICON:
					self.ItemListIdx.append(SrcSlotNumber)

		mouseModule.mouseController.DeattachObject()

	def SelectOwnerItemSlot(self, SlotIndex):

		if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():

			money = mouseModule.mouseController.GetAttachedItemCount()
			if app.ENABLE_GOLD_MAX_EXTENDED:
				net.SendExchangeElkAddPacket(long(money))
			else:
				net.SendExchangeElkAddPacket(money)

	def RefreshOwnerSlot(self):
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromSelf(i)
			itemCount = exchange.GetItemCountFromSelf(i)
			if 1 == itemCount:
				itemCount = 0
			self.OwnerSlot.SetItemSlot(i, itemIndex, itemCount)
			# if app.ENABLE_CHANGE_LOOK_SYSTEM:
			# 	itemTransmutedVnum = exchange.GetItemTransmutation(i, True)
			# 	if itemTransmutedVnum:
			# 		self.OwnerSlot.DisableCoverButton(i)
			# 	else:
			# 		self.OwnerSlot.EnableCoverButton(i)
		self.OwnerSlot.RefreshSlot()

	def RefreshTargetSlot(self):
		for i in xrange(exchange.EXCHANGE_ITEM_MAX_NUM):
			itemIndex = exchange.GetItemVnumFromTarget(i)
			itemCount = exchange.GetItemCountFromTarget(i)
			if 1 == itemCount:
				itemCount = 0
			self.TargetSlot.SetItemSlot(i, itemIndex, itemCount)
			# if app.ENABLE_CHANGE_LOOK_SYSTEM:
			# 	itemTransmutedVnum = exchange.GetItemTransmutation(i, False)
			# 	if itemTransmutedVnum:
			# 		self.TargetSlot.DisableCoverButton(i)
			# 	else:
			# 		self.TargetSlot.EnableCoverButton(i)
		self.TargetSlot.RefreshSlot()

	def Refresh(self):

		self.RefreshOwnerSlot()
		self.RefreshTargetSlot()

		if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			self.OwnerMoney.SetText(localeInfo.DottedNumber(exchange.GetElkFromSelf()))
			self.TargetMoney.SetText(localeInfo.DottedNumber(exchange.GetElkFromTarget()))
		else:
			self.OwnerMoney.SetText(str(exchange.GetElkFromSelf()))
			self.TargetMoney.SetText(str(exchange.GetElkFromTarget()))

		if True == exchange.GetAcceptFromSelf():
			if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
				self.OwnerSlot.SetSlotBaseImage("d:/ymir work/ui/public/slot_base.sub", 0.3500, 0.8500, 0.3500, 1.0)
				self.LogsDropList.AppendItem(self.Item(self, localeInfo.NEW_EXCHANGE_YOU_ACCEPT % (str((time.strftime("%H:%M:%S")))), 0))
			else:
				self.OwnerAcceptLight.Down()
		else:
			self.AcceptButton.Enable()
			self.AcceptButton.SetUp()
			if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
				self.OwnerSlot.SetSlotBaseImage("d:/ymir work/ui/public/slot_base.sub", 1.0, 1.0, 1.0, 1.0)
			else:
				self.OwnerAcceptLight.SetUp()

		if app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			if exchange.GetAcceptFromTarget() == True:
				self.TargetAcceptButton.Down()
				self.TargetSlot.SetSlotBaseImage("d:/ymir work/ui/public/slot_base.sub", 0.3500, 0.8500, 0.3500, 1.0)
				self.LogsDropList.AppendItem(self.Item(self, localeInfo.NEW_EXCHANGE_ACCEPT % (exchange.GetNameFromTarget() ,str((time.strftime("%H:%M:%S")))), 0))
			else:
				if self.TargetAcceptButton.IsDown() == True:
					self.LogsDropList.AppendItem(self.Item(self, localeInfo.NEW_EXCHANGE_ABORT % (exchange.GetNameFromTarget(), str((time.strftime("%H:%M:%S")))), 0))
				
				self.TargetAcceptButton.SetUp()
				self.TargetSlot.SetSlotBaseImage("d:/ymir work/ui/public/slot_base.sub", 1.0, 1.0, 1.0, 1.0)
		else:
			if True == exchange.GetAcceptFromTarget():
				self.TargetAcceptLight.Down()
			else:
				self.TargetAcceptLight.SetUp()

	def OverInOwnerItem(self, slotIndex):

		if 0 != self.tooltipItem:
			self.tooltipItem.SetExchangeOwnerItem(slotIndex)

	def OverInTargetItem(self, slotIndex):

		if 0 != self.tooltipItem:
			self.tooltipItem.SetExchangeTargetItem(slotIndex)

	def OverOutItem(self):

		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	if not app.WJ_ENABLE_TRADABLE_ICON:
		def OnTop(self):
			self.tooltipItem.SetTop()

	def OnPressEscapeKey(self):
		net.SendExchangeExitPacket()
		return True

	def OnUpdate(self):

		USE_EXCHANGE_LIMIT_RANGE = 1000

		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xStart) > USE_EXCHANGE_LIMIT_RANGE or abs(y - self.yStart) > USE_EXCHANGE_LIMIT_RANGE:
			(self.xStart, self.yStart, z) = player.GetMainCharacterPosition()
			net.SendExchangeExitPacket()

		if app.WJ_ENABLE_TRADABLE_ICON:
			if not self.inven:
				return

			page = self.inven.GetInventoryPageIndex() # range 0 ~ 1

			for i in self.ItemListIdx :
				if app.ENABLE_SPECIAL_INVENTORY and self.inven.GetInventoryType() == INVENTORY_TYPE_SKILLBOOK:
					if (SKILLBOOK_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (SKILLBOOK_INVENTORY_SLOT_START + ((page + 1) * SPECIAL_INVENTORY_PAGE_SIZE)):
						lock_idx = i - (SKILLBOOK_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE))
						self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

				elif app.ENABLE_SPECIAL_INVENTORY and self.inven.GetInventoryType() == INVENTORY_TYPE_STONE:
					if (STONE_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (STONE_INVENTORY_SLOT_START + ((page + 1) * SPECIAL_INVENTORY_PAGE_SIZE)):
						lock_idx = i - (STONE_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE))
						self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

				elif app.ENABLE_SPECIAL_INVENTORY and self.inven.GetInventoryType() == INVENTORY_TYPE_MATERIAL:
					if (MATERIAL_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE)) <= i < (MATERIAL_INVENTORY_SLOT_START + ((page + 1) * SPECIAL_INVENTORY_PAGE_SIZE)):
						lock_idx = i - (MATERIAL_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE))
						self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

				else:
					if (page * INVENTORY_PAGE_SIZE) <= i < ((page + 1) * INVENTORY_PAGE_SIZE): # range 0 ~ 179
						lock_idx = i - (page * INVENTORY_PAGE_SIZE) 
						self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

	if app.WJ_ENABLE_TRADABLE_ICON or app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItem(self, slotIndex):
			itemIndex = player.GetItemIndex(slotIndex)

			if itemIndex:
				# if app.ENABLE_SOUL_BIND_SYSTEM:
				# 	if player.GetItemSealDate(player.INVENTORY, slotIndex) == -1 or player.GetItemSealDate(player.INVENTORY, slotIndex) > 0: #ºÀÀÎ¾ÆÀÌÅÛ °É·¯³¿.
				# 		return True

				return player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_GIVE)

			return False

		def OnTop(self):
			self.tooltipItem.SetTop()
			if not self.interface:
				return

			self.interface.SetOnTopWindow(player.ON_TOP_WND_EXCHANGE)
			self.interface.RefreshMarkInventoryBag()

		def SetInven(self, inven):
			self.inven = inven

		def AddExchangeItemSlotIndex(self, idx):
			self.ItemListIdx.append(idx)
