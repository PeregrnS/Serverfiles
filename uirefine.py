import app
import net
import player
import item
import ui
import uiToolTip
import mouseModule
import localeInfo
import uiCommon
import constInfo
if app.ENABLE_REFINE_UI_RENEWAL:
	import wndMgr

	TOOLTIP_DATA = {
		'materials' : [],
		'slot_count': 0
	}

if app.WJ_ENABLE_TRADABLE_ICON and app.ENABLE_SPECIAL_INVENTORY:
	SPECIAL_INVENTORY_PAGE_SIZE = player.SPECIAL_INVENTORY_PAGE_SIZE

	INVENTORY_TYPE_SKILLBOOK = player.INVENTORY_TYPE_SKILLBOOK
	INVENTORY_TYPE_STONE = player.INVENTORY_TYPE_STONE
	INVENTORY_TYPE_MATERIAL = player.INVENTORY_TYPE_MATERIAL

	SKILLBOOK_INVENTORY_SLOT_START = player.SKILLBOOK_INVENTORY_SLOT_START
	STONE_INVENTORY_SLOT_START = player.STONE_INVENTORY_SLOT_START
	MATERIAL_INVENTORY_SLOT_START = player.MATERIAL_INVENTORY_SLOT_START

class RefineDialog(ui.ScriptWindow):

	makeSocketSuccessPercentage = ( 100, 33, 20, 15, 10, 5, 0 )
	upgradeStoneSuccessPercentage = ( 30, 29, 28, 27, 26, 25, 24, 23, 22 )
	upgradeArmorSuccessPercentage = ( 99, 66, 33, 33, 33, 33, 33, 33, 33 )
	upgradeAccessorySuccessPercentage = ( 99, 88, 77, 66, 33, 33, 33, 33, 33 )
	upgradeSuccessPercentage = ( 99, 66, 33, 33, 33, 33, 33, 33, 33 )

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadScript()

		self.scrollItemPos = 0
		self.targetItemPos = 0

	def __LoadScript(self):

		self.__LoadQuestionDialog()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/refinedialog.py")

		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.LoadObject")

		try:
			self.board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")
			self.successPercentage = self.GetChild("SuccessPercentage")
			self.GetChild("AcceptButton").SetEvent(self.OpenQuestionDialog)
			self.GetChild("CancelButton").SetEvent(self.Close)
		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.BindObject")

		##if 936 == app.GetDefaultCodePage():
		if constInfo.ENABLE_REFINE_PCT:
			self.successPercentage.Show()
		else:
			self.successPercentage.Show()

		toolTip = uiToolTip.ItemToolTip()
		toolTip.SetParent(self)
		toolTip.SetPosition(15, 38)
		toolTip.SetFollow(False)
		toolTip.Show()
		self.toolTip = toolTip

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadQuestionDialog(self):
		self.dlgQuestion = ui.ScriptWindow()

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self.dlgQuestion, "uiscript/questiondialog2.py")
		except:
			import exception
			exception.Abort("RefineDialog.__LoadQuestionDialog.LoadScript")

		try:
			GetObject=self.dlgQuestion.GetChild
			GetObject("message1").SetText(localeInfo.REFINE_DESTROY_WARNING)
			GetObject("message2").SetText(localeInfo.REFINE_WARNING2)
			GetObject("accept").SetEvent(ui.__mem_func__(self.Accept))
			GetObject("cancel").SetEvent(ui.__mem_func__(self.dlgQuestion.Hide))
		except:
			import exception
			exception.Abort("SelectCharacterWindow.__LoadQuestionDialog.BindObject")

	def Destroy(self):
		self.ClearDictionary()
		self.board = 0
		self.successPercentage = 0
		self.titleBar = 0
		self.toolTip = 0
		self.dlgQuestion = 0

	def GetRefineSuccessPercentage(self, scrollSlotIndex, itemSlotIndex):

		if -1 != scrollSlotIndex:
			if player.IsRefineGradeScroll(scrollSlotIndex):
				curGrade = player.GetItemGrade(itemSlotIndex)
				itemIndex = player.GetItemIndex(itemSlotIndex)

				item.SelectItem(itemIndex)
				itemType = item.GetItemType()
				itemSubType = item.GetItemSubType()

				if item.ITEM_TYPE_METIN == itemType:

					if curGrade >= len(self.upgradeStoneSuccessPercentage):
						return 0
					return self.upgradeStoneSuccessPercentage[curGrade]

				elif item.ITEM_TYPE_ARMOR == itemType:

					if item.ARMOR_BODY == itemSubType:
						if curGrade >= len(self.upgradeArmorSuccessPercentage):
							return 0
						return self.upgradeArmorSuccessPercentage[curGrade]
					else:
						if curGrade >= len(self.upgradeAccessorySuccessPercentage):
							return 0
						return self.upgradeAccessorySuccessPercentage[curGrade]

				else:

					if curGrade >= len(self.upgradeSuccessPercentage):
						return 0
					return self.upgradeSuccessPercentage[curGrade]

		for i in xrange(player.METIN_SOCKET_MAX_NUM+1):
			if 0 == player.GetItemMetinSocket(itemSlotIndex, i):
				break

		return self.makeSocketSuccessPercentage[i]

	def Open(self, scrollItemPos, targetItemPos):
		self.scrollItemPos = scrollItemPos
		self.targetItemPos = targetItemPos

		percentage = self.GetRefineSuccessPercentage(scrollItemPos, targetItemPos)
		if 0 == percentage:
			return
		self.successPercentage.SetText(localeInfo.REFINE_SUCCESS_PROBALITY % (percentage))

		itemIndex = player.GetItemIndex(targetItemPos)
		self.toolTip.ClearToolTip()
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(targetItemPos, i))
		self.toolTip.AddItemData(itemIndex, metinSlot)

		self.UpdateDialog()
		self.SetTop()
		self.Show()

	def UpdateDialog(self):
		newWidth = self.toolTip.GetWidth() + 30
		newHeight = self.toolTip.GetHeight() + 98
		self.board.SetSize(newWidth, newHeight)
		self.titleBar.SetWidth(newWidth-15)
		self.SetSize(newWidth, newHeight)

		(x, y) = self.GetLocalPosition()
		self.SetPosition(x, y)

	def OpenQuestionDialog(self):
		percentage = self.GetRefineSuccessPercentage(-1, self.targetItemPos)
		if 100 == percentage:
			self.Accept()
			return

		# links neben dem Refine-Fenster
		rx, ry = self.GetLocalPosition()
		qx = rx - 280 - 10   # 280 = width vom questiondialog2.py (bei dir evtl 340)
		qy = ry + 60

		self.dlgQuestion.SetPosition(qx, qy)
		self.dlgQuestion.SetTop()
		self.dlgQuestion.Show()

	def Accept(self):
		net.SendItemUseToItemPacket(self.scrollItemPos, self.targetItemPos)
		self.Close()

	def Close(self):
		self.dlgQuestion.Hide()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

class RefineDialogNew(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.isLoaded = False
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.inven = None

	def __Initialize(self):
		self.dlgQuestion = None
		self.children = []
		self.vnum = 0
		self.targetItemPos = 0
		self.dialogHeight = 0
		self.cost = 0
		self.percentage = 0
		self.type = 0
		if app.ENABLE_REFINE_UI_RENEWAL:
			self.renewHeight = 0

	def __LoadScript(self):

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/refinedialog.py")

		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.LoadObject")

		try:
			self.board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")
			self.probText = self.GetChild("SuccessPercentage")
			self.costText = self.GetChild("Cost")
			self.successPercentage = self.GetChild("SuccessPercentage")
			self.successPercentage.SetWindowHorizontalAlignCenter()
			self.successPercentage.SetHorizontalAlignCenter()
			self.successPercentage.SetWindowVerticalAlignBottom()
			self.successPercentage.SetVerticalAlignBottom()
			self.successPercentage.SetPosition(0, 65)

			self.GetChild("AcceptButton").SetEvent(self.OpenQuestionDialog)
			self.GetChild("CancelButton").SetEvent(self.CancelRefine)
		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.BindObject")

		##if 936 == app.GetDefaultCodePage():
		if constInfo.ENABLE_REFINE_PCT:
			self.successPercentage.Show()
			self.successPercentage.SetWindowHorizontalAlignCenter()
			self.successPercentage.SetHorizontalAlignCenter()
			self.successPercentage.SetWindowVerticalAlignBottom()
			self.successPercentage.SetVerticalAlignBottom()
			self.successPercentage.SetPosition(0, 65)

		toolTip = uiToolTip.ItemToolTip()
		toolTip.SetParent(self)
		toolTip.SetFollow(False)
		toolTip.SetPosition(15, 38)
		toolTip.Show()
		self.toolTip = toolTip

		self.slotList = []
		for i in xrange(3):
			slot = self.__MakeSlot()
			slot.SetParent(toolTip)
			slot.SetWindowVerticalAlignCenter()
			self.slotList.append(slot)

		itemImage = self.__MakeItemImage()
		itemImage.SetParent(toolTip)
		itemImage.SetWindowVerticalAlignCenter()
		itemImage.SetPosition(-35, 0)
		self.itemImage = itemImage

		if app.ENABLE_REFINE_UI_RENEWAL:
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()
			self.checkBox = ui.RefineCheckBox()
			self.checkBox.SetParent(self)
			self.checkBox.SetPosition(0, 60)
			self.checkBox.SetWindowHorizontalAlignCenter()
			self.checkBox.SetWindowVerticalAlignBottom()
			self.checkBox.SetEvent(ui.__mem_func__(self.AutoRefine), "ON_CHECK", True)
			self.checkBox.SetEvent(ui.__mem_func__(self.AutoRefine), "ON_UNCKECK", False)
			self.checkBox.SetCheckStatus(constInfo.IS_AUTO_REFINE)
			self.checkBox.SetTextInfo(localeInfo.KEEP_REFINE_OPEN)
			self.checkBox.Show()

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.CancelRefine))
		self.isLoaded = True

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __MakeSlot(self):
		slot = ui.ImageBox()
		slot.LoadImage("d:/ymir work/ui/public/slot_base.sub")
		slot.Show()
		self.children.append(slot)
		return slot

	def __MakeItemImage(self):
		itemImage = ui.ImageBox()
		itemImage.Show()
		self.children.append(itemImage)
		return itemImage

	def __MakeThinBoard(self):
		thinBoard = ui.ThinBoard()
		thinBoard.SetParent(self)
		thinBoard.Show()
		self.children.append(thinBoard)
		return thinBoard

	def Destroy(self):
		self.ClearDictionary()
		self.dlgQuestion = None
		self.board = 0
		self.probText = 0
		self.costText = 0
		self.titleBar = 0
		self.toolTip = 0
		self.successPercentage = None
		self.slotList = []
		self.children = []
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.inven = None

	def Open(self, targetItemPos, nextGradeItemVnum, cost, prob, type):

		if False == self.isLoaded:
			self.__LoadScript()

		self.__Initialize()

		self.targetItemPos = targetItemPos
		self.vnum = nextGradeItemVnum
		self.cost = cost
		self.percentage = prob
		self.type = type

		self.probText.SetText(localeInfo.REFINE_SUCCESS_PROBALITY % (self.percentage))
		if app.ENABLE_REFINE_UI_RENEWAL:
			self.costText.SetFontName("Tahoma:12")
			self.costText.SetText(localeInfo.REFINE_COST_EX % (localeInfo.DottedNumber(self.cost)))
		else:
			self.costText.SetText(localeInfo.REFINE_COST % (self.cost))

		self.toolTip.ClearToolTip()
		metinSlot = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(targetItemPos, i))

		attrSlot = []
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(player.GetItemAttribute(targetItemPos, i))

		self.toolTip.AddRefineItemData(nextGradeItemVnum, metinSlot, attrSlot)

		item.SelectItem(nextGradeItemVnum)
		self.itemImage.LoadImage(item.GetIconImageFileName())
		xSlotCount, ySlotCount = item.GetItemSize()
		for slot in self.slotList:
			slot.Hide()
		for i in xrange(min(3, ySlotCount)):
			self.slotList[i].SetPosition(-35, i*32 - (ySlotCount-1)*16)
			self.slotList[i].Show()

		if app.ENABLE_REFINE_UI_RENEWAL:
			if player.GetMoney() >= self.cost:
				self.costText.SetFontColor(0.33, 0.80, 0.46)
			else:
				self.costText.SetFontColor(1.00, 0.0, 0.0)

			self.dialogHeight = max([self.toolTip.GetHeight() + 46, ySlotCount*32 - (ySlotCount-1)*16 + 50])
		else:
			self.dialogHeight = self.toolTip.GetHeight() + 46
		self.UpdateDialog()

		if app.ENABLE_REFINE_UI_RENEWAL:
			if constInfo.AUTO_REFINE_TYPE == 2:
				self.checkBox.Hide()
			else:
				self.checkBox.Show()
		else:
			self.SetTop()
		self.Show()

	def Close(self):
		self.dlgQuestion = None
		self.Hide()

	if app.ENABLE_REFINE_UI_RENEWAL:
		def AppendMaterial(self, vnum, count):
			slotIndex = len(TOOLTIP_DATA['materials'])
			item.SelectItem(vnum)

			baseX_slot = 15
			baseX_text = 50
			baseY = self.toolTip.GetHeight() + 55  # unter Tooltip starten

			haveCount = player.GetItemCountByVnum(vnum)
			enough = (haveCount >= count)

			if not enough:
				slot = self.__MakeItemSlotNon(slotIndex)
			else:
				slot = self.__MakeItemSlotAvail(slotIndex)

			slot.SetPosition(baseX_slot, baseY + self.renewHeight)
			slot.SetItemSlot(slotIndex, vnum, count)
			TOOLTIP_DATA['materials'].append(vnum)

			thinBoard = self.__MakeThinBoard()
			thinBoard.SetPosition(baseX_text, baseY + self.renewHeight)
			thinBoard.SetSize(191, 20)

			# Rechts: "ItemName x benötigteAnzahl"
			textLine = ui.TextLine()
			textLine.SetParent(thinBoard)
			textLine.SetFontName(localeInfo.UI_DEF_FONT)
			textLine.SetText("%s x %d" % (item.GetItemName(), count))
			textLine.SetOutline()
			textLine.SetFeather(False)
			textLine.SetWindowVerticalAlignCenter()
			textLine.SetVerticalAlignCenter()
			textLine.SetPosition(100, 0)
			if enough:
				textLine.SetFontColor(0.33, 0.80, 0.46)  # grün
			else:
				textLine.SetFontColor(1.00, 0.0, 0.0)    # rot
			textLine.Show()
			self.children.append(textLine)

			# Links: "(xx)" = im Inventar
			textLine2 = ui.TextLine()
			textLine2.SetParent(thinBoard)
			textLine2.SetFontName(localeInfo.UI_DEF_FONT)
			textLine2.SetText("(%dx)" % haveCount)
			textLine2.SetOutline()
			textLine2.SetFeather(False)
			textLine2.SetWindowVerticalAlignCenter()
			textLine2.SetVerticalAlignCenter()
			textLine2.SetHorizontalAlignRight()
			textLine2.SetPosition(40, 0)  # links neben dem Namen
			if enough:
				textLine2.SetFontColor(0.33, 0.80, 0.46)
			else:
				textLine2.SetFontColor(1.00, 0.0, 0.0)
			textLine2.Show()
			self.children.append(textLine2)

			self.renewHeight += 34
			self.UpdateDialog()
	else:
		def AppendMaterial(self, vnum, count):
			slot = self.__MakeSlot()
			slot.SetParent(self)
			slot.SetPosition(15, self.dialogHeight)

			itemImage = self.__MakeItemImage()
			itemImage.SetParent(slot)
			item.SelectItem(vnum)
			itemImage.LoadImage(item.GetIconImageFileName())

			thinBoard = self.__MakeThinBoard()
			thinBoard.SetPosition(50, self.dialogHeight)
			thinBoard.SetSize(191, 20)

			textLine = ui.TextLine()
			textLine.SetParent(thinBoard)
			textLine.SetFontName(localeInfo.UI_DEF_FONT)
			textLine.SetPackedFontColor(0xffdddddd)
			textLine.SetText("%s x %02d" % (item.GetItemName(), count))
			textLine.SetOutline()
			textLine.SetFeather(False)
			textLine.SetWindowVerticalAlignCenter()
			textLine.SetVerticalAlignCenter()

			if localeInfo.IsARABIC():
				(x,y) = textLine.GetTextSize()
				textLine.SetPosition(x, 0)
			else:
				textLine.SetPosition(15, 0)

			textLine.Show()
			self.children.append(textLine)

			self.dialogHeight += 34
			self.UpdateDialog()

	if app.ENABLE_REFINE_UI_RENEWAL:
		def UpdateDialog(self):
			newWidth = self.toolTip.GetWidth() + 60

			# Oberer Item-/Tooltip-Bereich
			baseY = self.toolTip.GetHeight() + 55

			# Ende der Materialliste
			materialEndY = baseY + self.renewHeight

			# Unterer Block sauber darunter
			checkBoxY = materialEndY + 2
			percentY = checkBoxY + 14
			costY = percentY + 12
			buttonY = costY + 14

			# Fensterhöhe muss alles aufnehmen
			newHeight = buttonY + 55
			newHeight -= 8

			if localeInfo.IsARABIC():
				self.board.SetPosition(newWidth, 0)
				(x, y) = self.titleBar.GetLocalPosition()
				self.titleBar.SetPosition(newWidth - 15, y)

			self.board.SetSize(newWidth, newHeight)
			self.toolTip.SetPosition(15 + 35, 38)
			self.titleBar.SetWidth(newWidth - 15)
			self.SetSize(newWidth, newHeight)

			self.SetPosition(
				wndMgr.GetScreenWidth()/2 - newWidth/2,
				wndMgr.GetScreenHeight()/2 - newHeight/2
			)

			# Kosten
			if self.costText:
				self.costText.SetWindowHorizontalAlignCenter()
				self.costText.SetHorizontalAlignCenter()
				self.costText.SetWindowVerticalAlignTop()
				self.costText.SetVerticalAlignTop()
				self.costText.SetPosition(0, costY)

			# Prozentanzeige
			if self.successPercentage:
				self.successPercentage.SetWindowHorizontalAlignCenter()
				self.successPercentage.SetHorizontalAlignCenter()
				self.successPercentage.SetWindowVerticalAlignTop()
				self.successPercentage.SetVerticalAlignTop()
				self.successPercentage.SetPosition(0, percentY)

			# Checkbox
			if app.ENABLE_REFINE_UI_RENEWAL and self.checkBox:
				self.checkBox.SetWindowHorizontalAlignCenter()
				self.checkBox.SetWindowVerticalAlignTop()
				self.checkBox.SetPosition(0, checkBoxY)

			# Buttons
			acceptButton = self.GetChild("AcceptButton")
			cancelButton = self.GetChild("CancelButton")

			acceptButton.SetWindowHorizontalAlignCenter()
			acceptButton.SetWindowVerticalAlignTop()
			acceptButton.SetPosition(-35, buttonY)

			cancelButton.SetWindowHorizontalAlignCenter()
			cancelButton.SetWindowVerticalAlignTop()
			cancelButton.SetPosition(35, buttonY)

	def OpenQuestionDialog(self):

		if 100 == self.percentage:
			self.Accept()
			return

		if 5 == self.type:
			self.Accept()
			return

		dlgQuestion = uiCommon.QuestionDialog2()
		dlgQuestion.SetText2(localeInfo.REFINE_WARNING2)
		dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.Accept))
		dlgQuestion.SetCancelEvent(ui.__mem_func__(dlgQuestion.Close))

		if 3 == self.type:
			dlgQuestion.SetText1(localeInfo.REFINE_DESTROY_WARNING_WITH_BONUS_PERCENT_1)
			dlgQuestion.SetText2(localeInfo.REFINE_DESTROY_WARNING_WITH_BONUS_PERCENT_2)
		elif 2 == self.type:
			dlgQuestion.SetText1(localeInfo.REFINE_DOWN_GRADE_WARNING)
		else:
			dlgQuestion.SetText1(localeInfo.REFINE_DESTROY_WARNING)

		dlgQuestion.Open()
		self.dlgQuestion = dlgQuestion

	def Accept(self):
		net.SendRefinePacket(self.targetItemPos, self.type)
		if not app.ENABLE_REFINE_UI_RENEWAL:
			self.Close()

	def CancelRefine(self):
		net.SendRefinePacket(255, 255)
		self.Close()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.SetCanMouseEventSlot(self.targetItemPos)

		if app.ENABLE_REFINE_UI_RENEWAL:
			constInfo.AUTO_REFINE_TYPE = 0
			constInfo.AUTO_REFINE_DATA = {
				"ITEM" : [-1, -1],
				"NPC" : [0, -1, -1, 0]
			}

	if app.ENABLE_REFINE_UI_RENEWAL:
		def __MakeItemSlotNon(self,c):
			itemslot = ui.SlotWindow()
			itemslot.SetParent(self)
			itemslot.SetSize(32, 32)
			itemslot.SetSlotBaseImage("d:/ymir work/ui/game/refine/slot_base_non.tga", 1.0, 1.0, 1.0, 1.0)
			itemslot.AppendSlot(c, 0, 0, 32, 32)
			itemslot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			itemslot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			itemslot.RefreshSlot()
			itemslot.Show()
			self.children.append(itemslot)
			return itemslot

		def __MakeItemSlotAvail(self,c):
			itemslot = ui.SlotWindow()
			itemslot.SetParent(self)
			itemslot.SetSize(32, 32)
			itemslot.SetSlotBaseImage("d:/ymir work/ui/game/refine/slot_base_avail.tga", 1.0, 1.0, 1.0, 1.0)
			itemslot.AppendSlot(c, 0, 0, 32, 32)
			itemslot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			itemslot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			itemslot.RefreshSlot()
			itemslot.Show()
			self.children.append(itemslot)
			return itemslot

		def OverInItem(self, slotIndex):
			if slotIndex > len(TOOLTIP_DATA['materials']):
				return

			if self.tooltipItem:
				self.tooltipItem.ClearToolTip()
				self.tooltipItem.AddItemData(TOOLTIP_DATA['materials'][slotIndex], 0, 0, 0, 0)
				self.tooltipItem.AlignHorizonalCenter()
				self.tooltipItem.ShowToolTip()

		def OverOutItem(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def AutoRefine(self, checkType, autoFlag):
			constInfo.IS_AUTO_REFINE = autoFlag

		def CheckRefine(self, isFail):
			if constInfo.IS_AUTO_REFINE == True:
				if constInfo.AUTO_REFINE_TYPE == 1:
					if constInfo.AUTO_REFINE_DATA["ITEM"][0] != -1 and constInfo.AUTO_REFINE_DATA["ITEM"][1] != -1:
						scrollIndex = player.GetItemIndex(constInfo.AUTO_REFINE_DATA["ITEM"][0])
						itemIndex = player.GetItemIndex(constInfo.AUTO_REFINE_DATA["ITEM"][1])

						#chat.AppendChat(chat.CHAT_TYPE_INFO, "%d %d" % (itemIndex, int(itemIndex %10)))
						if scrollIndex == 0 or (itemIndex % 10 == 8 and not isFail):
							self.Close()
						else:
							net.SendItemUseToItemPacket(constInfo.AUTO_REFINE_DATA["ITEM"][0], constInfo.AUTO_REFINE_DATA["ITEM"][1])
				elif constInfo.AUTO_REFINE_TYPE == 2:
					npcData = constInfo.AUTO_REFINE_DATA["NPC"]
					if npcData[0] != 0 and npcData[1] != -1 and npcData[2] != -1 and npcData[3] != 0:
						itemIndex = player.GetItemIndex(npcData[1], npcData[2])
						if (itemIndex % 10 == 8 and not isFail) or isFail:
							self.Close()
						else:
							net.SendGiveItemPacket(npcData[0], npcData[1], npcData[2], npcData[3])
				else:
					self.Close()
			else:
				self.Close()

	def OnPressEscapeKey(self):
		self.CancelRefine()
		return True

	if app.WJ_ENABLE_TRADABLE_ICON:
		def SetInven(self, inven):
			self.inven = inven

		def SetCanMouseEventSlot(self, idx):
			if idx >= player.INVENTORY_PAGE_SIZE:
				page = self.inven.GetInventoryPageIndex() # 0,1,2,3
				if app.ENABLE_SPECIAL_INVENTORY and self.inven.GetInventoryType() == INVENTORY_TYPE_SKILLBOOK:
					idx -= (page * SPECIAL_INVENTORY_PAGE_SIZE)
				else:
					if app.ENABLE_EXTEND_INVEN_SYSTEM:
						idx -= (page * player.INVENTORY_PAGE_SIZE)
					else:
						idx -= player.INVENTORY_PAGE_SIZE

			self.inven.wndItem.SetCanMouseEventSlot(idx)

		def OnUpdate(self):
			if not self.inven:
				return

			targetItemPos = self.targetItemPos
			if targetItemPos < 0:
				return

			page = self.inven.GetInventoryPageIndex() # range 0 ~ 3

			if app.ENABLE_SPECIAL_INVENTORY and self.inven.GetInventoryType() == INVENTORY_TYPE_SKILLBOOK:
				if (SKILLBOOK_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE)) <= targetItemPos < (SKILLBOOK_INVENTORY_SLOT_START + ((page + 1) * SPECIAL_INVENTORY_PAGE_SIZE)):
					lock_idx = targetItemPos - (SKILLBOOK_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE))
					self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

			elif app.ENABLE_SPECIAL_INVENTORY and self.inven.GetInventoryType() == INVENTORY_TYPE_STONE:
				if (STONE_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE)) <= targetItemPos < (STONE_INVENTORY_SLOT_START + ((page + 1) * SPECIAL_INVENTORY_PAGE_SIZE)):
					lock_idx = targetItemPos - (STONE_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE))
					self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

			elif app.ENABLE_SPECIAL_INVENTORY and self.inven.GetInventoryType() == INVENTORY_TYPE_MATERIAL:
				if (MATERIAL_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE)) <= targetItemPos < (MATERIAL_INVENTORY_SLOT_START + ((page + 1) * SPECIAL_INVENTORY_PAGE_SIZE)):
					lock_idx = targetItemPos - (MATERIAL_INVENTORY_SLOT_START + (page * SPECIAL_INVENTORY_PAGE_SIZE))
					self.inven.wndItem.SetCantMouseEventSlot(lock_idx)

			else:
				if (page * player.INVENTORY_PAGE_SIZE) <= targetItemPos < ((page + 1) * player.INVENTORY_PAGE_SIZE): # range 0 ~ 179
					lock_idx = targetItemPos - (page * player.INVENTORY_PAGE_SIZE)
					self.inven.wndItem.SetCantMouseEventSlot(lock_idx)
