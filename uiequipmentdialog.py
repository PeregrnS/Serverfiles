import ui
import chr
import player
import app
import localeInfo
import item

COSTUME_DICT = (item.WEAR_COSTUME_BODY, item.WEAR_COSTUME_HAIR,)
if app.ENABLE_MOUNT_SYSTEM:
	COSTUME_DICT += (item.WEAR_MOUNT,)
if app.ENABLE_WEAPON_COSTUME_SYSTEM:
	COSTUME_DICT += (item.WEAR_COSTUME_WEAPON,)
if app.ENABLE_ACCE_COSTUME_SYSTEM:
	COSTUME_DICT += (item.WEAR_COSTUME_ACCE,)

class CostumeEquipmentDialog(ui.ScriptWindow):
	def __init__(self, wndEquipment):
		import exception

		if not wndEquipment:
			exception.Abort("wndEquipment parameter must be set to CostumeEquipmentDialog")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndEquipment = wndEquipment
		self.wndCostumeEquipmentLayer = None
		self.wndCostumeEquipmentSlot = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		if self.wndCostumeEquipmentLayer:
			self.wndCostumeEquipmentLayer.Show()
		self.AdjustPositionAndSize()
		self.RefreshSlot()
		ui.ScriptWindow.Show(self)

	def Close(self):
		if self.wndCostumeEquipmentLayer:
			self.wndCostumeEquipmentLayer.Hide()
		self.Hide()

	def IsOpenedCostumeEquipment(self):
		return self.wndCostumeEquipmentLayer.IsShow()

	def GetBasePosition(self):
		x, y = self.wndEquipment.GetGlobalPosition()
		return x - 140, y

	def AdjustPositionAndSize(self):
		bx, by = self.GetBasePosition()
	
		if self.IsOpenedCostumeEquipment():
			self.SetPosition(bx, by)
			self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())
		else:
			self.SetPosition(bx, by)
			self.SetSize(10, self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/CostumeEquipmentDialog.py")
		except:
			import exception
			exception.Abort("CostumeEquipmentDialog.LoadWindow.LoadObject")
		try:
			self.ORIGINAL_WIDTH = self.GetWidth()
			wndCostumeEquipmentSlot = self.GetChild("CostumeEquipmentSlot")
			self.wndCostumeEquipmentLayer = self.GetChild("CostumeEquipmentLayer")

		except:
			import exception
			exception.Abort("CostumeEquipmentDialog.LoadWindow.BindObject")

		try:
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		except:
			try:
				self.GetChild("Board").SetCloseEvent(ui.__mem_func__(self.Close))
			except:
				pass

		wndCostumeEquipmentSlot.SetOverInItemEvent(ui.__mem_func__(self.wndEquipment.OverInItem))
		wndCostumeEquipmentSlot.SetOverOutItemEvent(ui.__mem_func__(self.wndEquipment.OverOutItem))
		if app.ENABLE_RENDER_TARGET_PREVIEW:
			wndCostumeEquipmentSlot.SetUseSlotEvent(ui.__mem_func__(self.wndEquipment.UseItemSlot))
			wndCostumeEquipmentSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndEquipment.UseItemSlot))

		self.wndCostumeEquipmentSlot = wndCostumeEquipmentSlot

	def RefreshSlot(self):
		equipmentDict = self.wndEquipment.itemDataDict
		for i in COSTUME_DICT:
			if equipmentDict.has_key(i):
				self.wndCostumeEquipmentSlot.SetItemSlot(i, equipmentDict[i][0], equipmentDict[i][1])

class EquipmentDialog(ui.ScriptWindow):
	wndCostumeEquipment = None

	def __init__(self):
		# print "NEW EQUIPMENT DIALOG ----------------------------------------------------------------------------"
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()

		self.vid = None
		self.eventClose = None
		self.itemDataDict = {}
		self.tooltipItem = None

	def __del__(self):
		# print "---------------------------------------------------------------------------- DELETE EQUIPMENT DIALOG "
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/EquipmentDialog.py")

			getObject = self.GetChild
			self.board = getObject("Board")
			self.slotWindow = getObject("EquipmentSlot")

			self.costumeButton = self.GetChild2("CostumeButton")
			self.wndCostumeEquipment = CostumeEquipmentDialog(self)

		except:
			import exception
			exception.Abort("EquipmentDialog.LoadDialog.BindObject")

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.slotWindow.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.slotWindow.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		if app.ENABLE_RENDER_TARGET_PREVIEW:
			self.slotWindow.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			self.slotWindow.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))

		if self.costumeButton:
			self.costumeButton.SetEvent(ui.__mem_func__(self.ClickCostumeButton))

	def Open(self, vid):

		self.vid = vid
		self.itemDataDict = {}

		name = chr.GetNameByVID(vid)
		self.board.SetTitleName(localeInfo.EQUIPMENT_VIEW_TITLE % name)

		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def ClickCostumeButton(self):
		# print "Click Costume Button"
		if self.wndCostumeEquipment:
			if self.wndCostumeEquipment.IsShow():
				self.wndCostumeEquipment.Hide()
			else:
				self.wndCostumeEquipment.Show()
		else:
			self.wndCostumeEquipment = CostumeEquipmentDialog(self)
			self.wndCostumeEquipment.Show()

	def Destroy(self):
		self.eventClose = None

		self.Close()
		self.ClearDictionary()

		self.board = None
		self.slotWindow = None

		if self.wndCostumeEquipment:
			self.wndCostumeEquipment.Destroy()
			self.wndCostumeEquipment = None

	def SetEquipmentDialogItem(self, slotIndex, vnum, count):
		if count <= 1:
			count = 0
		self.slotWindow.SetItemSlot(slotIndex, vnum, count)

		emptySocketList = []
		emptyAttrList = []
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			emptySocketList.append(0)
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			emptyAttrList.append((0, 0))

		self.itemDataDict[slotIndex] = (vnum, count, emptySocketList, emptyAttrList)

	def SetEquipmentDialogSocket(self, slotIndex, socketIndex, value):
		if not slotIndex in self.itemDataDict:
			return
		if socketIndex < 0 or socketIndex > player.METIN_SOCKET_MAX_NUM:
			return
		self.itemDataDict[slotIndex][2][socketIndex] = value

	def SetEquipmentDialogAttr(self, slotIndex, attrIndex, type, value):
		if not slotIndex in self.itemDataDict:
			return
		if attrIndex < 0 or attrIndex > player.ATTRIBUTE_SLOT_MAX_NUM:
			return
		self.itemDataDict[slotIndex][3][attrIndex] = (type, value)

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SetCloseEvent(self, event):
		self.eventClose = event

	def OverInItem(self, slotIndex):

		if None == self.tooltipItem:
			return

		if not slotIndex in self.itemDataDict:
			return

		itemVnum = self.itemDataDict[slotIndex][0]
		if 0 == itemVnum:
			return

		self.tooltipItem.ClearToolTip()
		metinSlot = self.itemDataDict[slotIndex][2]
		attrSlot = self.itemDataDict[slotIndex][3]

		self.tooltipItem.AddItemData(itemVnum, metinSlot, attrSlot)
		self.tooltipItem.ShowToolTip()
		if app.ENABLE_RENDER_TARGET_PREVIEW:
			self.tooltipItem.AppendRenderingTooltip(itemVnum)

	if app.ENABLE_RENDER_TARGET_PREVIEW:
		def UseItemSlot(self, slotIndex):
			ItemVNum = self.itemDataDict[slotIndex][0]
			if app.IsPressed(app.DIK_LSHIFT):
				self.tooltipItem.ModelPreviewFull(ItemVNum)
				return

	def OverOutItem(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()
			self.tooltipItem.ClearToolTip()

	def Close(self):
		self.OverOutItem()
		self.itemDataDict = {}
		self.tooltipItem = None
		self.Hide()

		if self.eventClose:
			self.eventClose(self.vid)

		if self.wndCostumeEquipment:
			self.wndCostumeEquipment.Close()

	def OnMoveWindow(self, x, y):
		if self.wndCostumeEquipment:
			self.wndCostumeEquipment.AdjustPositionAndSize()

	def OnPressEscapeKey(self):
		self.Close()
		return True
