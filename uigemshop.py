import uiCommon
import chat
import app
import net
import player
import item
import wndMgr
import mouseModule
import localeInfo
import uiScriptLocale
import ui

class GemShopWindow(ui.ScriptWindow):
	GEM_SHOP_SLOT_MAX = 9
	GEM_SHOP_WINDOW_LIMIT_RANGE = 500
	GEM_SHOP_MAX_PAGE = 5

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.tooltipItem = 0
		self.xGemShopStart = 0
		self.yGemShopStart = 0
		self.questionDialog = None
		
		self.priceDict = {}
		
		self.currentPage = 0

		if app.ENABLE_CHEST_DROP_INFO or app.ENABLE_RENDER_TARGET_PREVIEW:
			self.interface = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def __LoadWindow(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			if localeInfo.IsARABIC():
				PythonScriptLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "GemShopWindow.py")
			else:
				PythonScriptLoader.LoadScriptFile(self, "UIScript/GemShopWindow.py")
		except:
			import exception
			exception.Abort("GemShopWindow.__LoadWindow.LoadObject")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.itemSlot = self.GetChild("SellItemSlot")
			
			self.prevPageButton = self.GetChild("PrevPageButton")
			self.nextPageButton = self.GetChild("NextPageButton")
			
			self.currentPageText = self.GetChild("CurrentPageText")
			
			for i in xrange(self.GEM_SHOP_SLOT_MAX):
				self.priceDict["slot_%d_price" % i] = self.GetChild("slot_%s_price" % str(i+1))

		except:
			import exception
			exception.Abort("GemShopWindow.__LoadWindow.BindObject")
			
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		
		self.prevPageButton.SetEvent(ui.__mem_func__(self.ClickPrevPageButton))
		self.nextPageButton.SetEvent(ui.__mem_func__(self.ClickNextPageButton))
		
		self.itemSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		# self.itemSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		self.itemSlot.SAFE_SetButtonEvent("RIGHT", "EXIST", self.UseItemSlot)
		
		self.currentPageText.SetText("%d/%d" % (self.currentPage + 1, self.GEM_SHOP_MAX_PAGE))
		
	def ClickPrevPageButton(self):
		if self.currentPage == 0:
			return
			
		self.currentPage -= 1
		self.RefreshItemSlot()
		
		self.currentPageText.SetText("%d/%d" % (self.currentPage + 1, self.GEM_SHOP_MAX_PAGE))
		
	def ClickNextPageButton(self):
		if self.currentPage == (self.GEM_SHOP_MAX_PAGE - 1):
			return
			
		self.currentPage += 1
		self.RefreshItemSlot()
		
		self.currentPageText.SetText("%d/%d" % (self.currentPage + 1, self.GEM_SHOP_MAX_PAGE))
			
	def Close(self):
		self.Hide()
		
	def Destroy(self):
		self.ClearDictionary()
		if app.ENABLE_CHEST_DROP_INFO or app.ENABLE_RENDER_TARGET_PREVIEW:
			self.interface = None

		self.questionDialog = None
		
	def Open(self):
		self.RefreshItemSlot()
	
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

		(self.xGemShopStart, self.yGemShopStart, z) = player.GetMainCharacterPosition()
			
	def SetItemToolTip(self, tooltip):
		self.tooltipItem = tooltip
			
	def OverInItem(self, slotIndex):
		if mouseModule.mouseController.isAttached():
			return

		realSlot = self.GetRealSlotIndex(slotIndex)
		itemVNum = player.GetGemShopItemVnum(realSlot)

		if 0 != self.tooltipItem:
			self.tooltipItem.SetItemToolTip(itemVNum)
			if app.ENABLE_EMOJI_SYSTEM:
				self.tooltipItem.AppendGemBuyTooltip()
			if app.ENABLE_EMOJI_SYSTEM and app.ENABLE_RENDER_TARGET_PREVIEW:
				self.tooltipItem.AppendRenderingTooltip(itemVNum)
			if app.ENABLE_EMOJI_SYSTEM and app.ENABLE_CHEST_DROP_INFO:
				self.tooltipItem.AppendChestDropInfo(itemVNum)

	def OverOutItem(self):
		if 0 != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	if app.ENABLE_CHEST_DROP_INFO or app.ENABLE_RENDER_TARGET_PREVIEW:
		def UseItemSlot(self, slotIndex):
			realSlot = self.GetRealSlotIndex(slotIndex)
			itemVnum = player.GetGemShopItemVnum(realSlot)
			item.SelectItem(itemVnum)
			if app.IsPressed(app.DIK_LCONTROL):
				if item.HasDropInfo(itemVnum) and self.interface:
					self.interface.OpenChestDropWindow(itemVnum)

			elif app.IsPressed(app.DIK_LSHIFT):
				self.interface.OpenRenderTarget(itemVnum)

			elif player.GetGem() < player.GetGemShopItemPrice(realSlot):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_SYSTEM_NOT_ENOUGH_HP_GEM)

			else:
				self.GemShopSlotBuy(realSlot)
	else:
		def UseItemSlot(self, slotIndex):
			realSlot = self.GetRealSlotIndex(slotIndex)
			if player.GetGem() < player.GetGemShopItemPrice(realSlot):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GEM_SYSTEM_NOT_ENOUGH_HP_GEM)
			else:
				self.GemShopSlotBuy(realSlot)
		
	def GetRealSlotIndex(self, slot):
		return self.currentPage * self.GEM_SHOP_SLOT_MAX + slot
		
	def RefreshItemSlot(self):
		for i in xrange(self.GEM_SHOP_SLOT_MAX):
			realSlot = self.GetRealSlotIndex(i)
			itemCount = player.GetGemShopItemCount(realSlot)
			itemPrice = player.GetGemShopItemPrice(realSlot)
			if itemCount <= 1:
				itemCount = 0
			
			self.itemSlot.SetItemSlot(i, player.GetGemShopItemVnum(realSlot), itemCount)
			
			self.priceDict["slot_%d_price" % i].SetText(str(itemPrice))

		wndMgr.RefreshSlot(self.itemSlot.GetWindowHandle())
		
	def SlotAddQuestionCancel(self):
		if self.questionDialog:
			self.questionDialog.Close()

		self.questionDialog = None
			
	def GemShopSlotBuy(self, slotIndex):
		realSlot = self.GetRealSlotIndex(slotIndex)
		itemVnum = player.GetGemShopItemVnum(realSlot)
		name = item.GetItemName()
		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText(localeInfo.GEM_SYSTEM_BUY_ITEM % name)
		questionDialog.SetAcceptEvent(lambda arg = int(slotIndex): ui.__mem_func__(self.SendBuyAccept)(arg))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.SlotAddQuestionCancel))
		questionDialog.Open()
		self.questionDialog = questionDialog
		
	def SendBuyAccept(self, slotIndex):
		self.SlotAddQuestionCancel()
		net.SendGemShopBuy(slotIndex)

	if app.ENABLE_CHEST_DROP_INFO or app.ENABLE_RENDER_TARGET_PREVIEW:
		def BindInterface(self, interface):
			self.interface = interface

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xGemShopStart) > self.GEM_SHOP_WINDOW_LIMIT_RANGE or abs(y - self.yGemShopStart) > self.GEM_SHOP_WINDOW_LIMIT_RANGE:
			self.Close()

	def OnPressEscapeKey(self):
		self.Close()
		return True
