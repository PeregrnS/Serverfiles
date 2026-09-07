import ui
import item
import uiToolTip
import app
import localeInfo

class ChestDropInfoWindow(ui.ScriptWindow):
	DROP_SLOT_SIZE = 5 * 8 ## Same with CGrid @ UserInterface/PythonItemModule.cpp
	ITEM = 0
	COUNT = 1

	def __init__(self) :
		ui.ScriptWindow.__init__(self)
		self.isLoaded = False
		self.ItemVnum = -1
		self.PageCount = 0
		self.CurrentPage = 0
		self.DropDict = dict()
		self.DropItemSlot = None
		self.prevButton = None
		self.nextButton = None
		self.currentPageText = None
		self.tooltipitem = uiToolTip.ItemToolTip()
		if app.ENABLE_RENDER_TARGET_PREVIEW:
			self.interface = None

	def __del__(self) :
		ui.ScriptWindow.__del__(self)
		self.DropDict = None
		self.DropItemSlot = None
		self.prevButton = None
		self.nextButton = None
		self.currentPageText = None
		self.tooltipitem = None
		if app.ENABLE_RENDER_TARGET_PREVIEW:
			self.interface = None

	def __LoadWindow(self):
		if self.isLoaded:
			return

		self.isLoaded = True

		# script
		try:
			self.__LoadScript("UIScript/ChestDropInfoWindow.py")
		except:
			import exception
			exception.Abort("ChestDropInfoWindow.__LoadWindow.__LoadScript")

		# object
		try:
			self.__BindObject()
		except:
			import exception
			exception.Abort("ChestDropInfoWindow.__LoadWindow.__BindObject")

		# event
		try:
			self.__BindEvent()
		except:
			import exception
			exception.Abort("ChestDropInfoWindow.__LoadWindow.__BindEvent")

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __BindObject(self):
		self.Board				= self.GetChild("board")
		self.DropItemSlot		= self.GetChild("drop_item_slot")
		self.prevButton			= self.GetChild("prev_button")
		self.nextButton			= self.GetChild("next_button")
		self.currentPageText	= self.GetChild("CurrentPage")
		if localeInfo.IsARABIC():
			self.prevButton.LeftRightReverse()
			self.nextButton.LeftRightReverse()

	def __BindEvent(self):
		self.Board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.SetCenterPosition()
		self.DropItemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInDropItemSlot))
		self.DropItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.DropItemSlot.SetUseSlotEvent(ui.__mem_func__(self.__UseItemSlot))
		self.DropItemSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.__UseItemSlot))

		self.prevButton.SetEvent(ui.__mem_func__(self.SetPage), -1)
		self.nextButton.SetEvent(ui.__mem_func__(self.SetPage), +1)
	
	def UpdateItems(self):
		for i in range(ChestDropInfoWindow.DROP_SLOT_SIZE):
			self.DropItemSlot.ClearSlot(i)

		if self.CurrentPage in self.DropDict:
			for pos in self.DropDict[self.CurrentPage]:
				data = self.DropDict[self.CurrentPage][pos]
				self.DropItemSlot.SetItemSlot(pos, data[ChestDropInfoWindow.ITEM], data[ChestDropInfoWindow.COUNT])
		
		self.DropItemSlot.RefreshSlot()
	
	def SetPage(self, page):
		nextpage = page + self.CurrentPage
		if 0 <= nextpage <= self.PageCount:
			self.CurrentPage = nextpage
			self.currentPageText.SetText(str(self.CurrentPage + 1))
			self.UpdateItems()
	
	def SetUp(self, itemVnum):
		self.ItemVnum = itemVnum
		item.SelectItem(itemVnum)

		self.Board.SetTitleFont(localeInfo.UI_DEF_BOLD_FONT)
		self.Board.SetTitleName(item.GetItemName())

		(self.PageCount, DropList) = item.GetDropInfo(self.ItemVnum)
		
		# sys_err("PageCount: %d" % self.PageCount)

		self.DropDict.clear()
		for i in range(self.PageCount + 1):
			self.DropDict[i] = dict()

		for page, pos, vnum, count in DropList:
			self.DropDict[page][pos] = (vnum, count)

		self.CurrentPage = 0
		if self.PageCount > 0:
			self.prevButton.Show()
			self.nextButton.Show()
		else:
			self.prevButton.Hide()
			self.nextButton.Hide()

		self.SetPage(0)
	
	def Open(self, itemVnum):
		self.__LoadWindow()
		self.SetUp(itemVnum)
		
		self.SetTop()
		self.Show()
	
	def OverInDropItemSlot(self, slotIndex):
		if self.tooltipitem:
			if self.CurrentPage in self.DropDict:
				if slotIndex in self.DropDict[self.CurrentPage]:
					data = self.DropDict[self.CurrentPage][slotIndex]
					self.tooltipitem.SetItemToolTip(data[ChestDropInfoWindow.ITEM])
					if app.ENABLE_RENDER_TARGET_PREVIEW:
						self.tooltipitem.AppendRenderingTooltip(data[ChestDropInfoWindow.ITEM])
					self.tooltipitem.AppendChestDropInfo(data[ChestDropInfoWindow.ITEM])

	def OverOutItem(self):
		if self.tooltipitem:
			self.tooltipitem.HideToolTip()
			self.tooltipitem.ClearToolTip()

	def __UseItemSlot(self, slotIndex):
		data = self.DropDict[self.CurrentPage][slotIndex]
		vnum = data[ChestDropInfoWindow.ITEM]
		item.SelectItem(vnum)

		if app.IsPressed(app.DIK_LCONTROL):
			if item.HasDropInfo(vnum):
				self.Open(vnum)
			return

		if app.ENABLE_RENDER_TARGET_PREVIEW and app.IsPressed(app.DIK_LSHIFT):
			self.interface.OpenRenderTarget(vnum)
			return

	def Close(self):
		self.OverOutItem()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	if app.ENABLE_RENDER_TARGET_PREVIEW:
		def BindInterface(self, interface):
			self.interface = interface
