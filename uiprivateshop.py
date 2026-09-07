import ui
import app
import snd
import privateShop
import localeInfo
import uiScriptLocale
import uiToolTip
import mouseModule
import background
import net
import uiCommon
import wndMgr
import item
import chat
import player
import chr
import systemSetting
from grid import Grid
import constInfo
import grp

if app.ENABLE_RENDER_TARGET:
	import renderTarget

WINDOW_WIDTH			= 196 + 88
WINDOW_HEIGHT			= 452

SHOP_WINDOW_HEIGHT		= 365
SHOP_WINDOW_X_POS		= wndMgr.GetScreenWidth() - 400
SHOP_WINDOW_Y_POS		= 10

TAB_BUTTON_Y_DELTA		= 20

INFORMATION_GROUP_X		= 40
INFORMATION_GROUP_Y		= 361

BUTTON_GROUP_X			= 22
BUTTON_GROUP_Y			= 413


ROOT_PATH = "d:/ymir work/ui/game/premium_private_shop/"
UNLOCKED_SLOT_PATH		= "d:/ymir work/ui/public/Slot_Base.sub"
LOCKED_SLOT_PATH		= ROOT_PATH + "locked_slot_01.sub"

if app.ENABLE_RENDER_TARGET:
	RENDER_TARGET_INDEX = 3

PRICE_RESTRICTION_IGNORE_DICT = {
	# Item-Vnum  [Vnum, Vnum, ...]
	"VNUM"	:	[
	],

	# Item-Type : [Sub-Type, Sub-Type, ...] (If empty every Sub-Type will be ignored)
	"TYPE" : {
		item.ITEM_TYPE_WEAPON	:	[],
		item.ITEM_TYPE_ARMOR	:	[],
		item.ITEM_TYPE_COSTUME	:	[],
		item.ITEM_TYPE_DS		:	[],
		item.ITEM_TYPE_BELT		:	[],
	},
}

g_privateShopPriceDict = {}
g_privateShopTitleBoardDict = {}
g_openedPrivateShopBoardList = []

def UpdateTitleBoard():	
	for key, title in g_privateShopTitleBoardDict.items():

		if systemSetting.IsShowSalesText():
			title.Show()
		else:
			title.Hide()
			continue

		valid_distance = privateShop.MAX_VIEW_DISTANCE * systemSetting.GetPrivateShopViewDistance()
		current_distance =  privateShop.GetMainCharacterDistance(title.vid)

		if current_distance > valid_distance or current_distance < 0:
			title.Hide()
		else:
			title.Show()

def DeleteTitleBoard(vid):
	if not g_privateShopTitleBoardDict.has_key(vid):
		return
			
	del g_privateShopTitleBoardDict[vid]
	
class PrivateShopTitleBoard(ui.ShopDecoThinboard):
	def __init__(self, type = 0):
		ui.ShopDecoThinboard.__init__(self, type, "UI_BOTTOM")
		self.vid = None
		self.type = type
		
		self.__MakeTextLine()
		self.__MakeCheckIcon()
		self.__CalcFontColor()

	def __del__(self):
		ui.ShopDecoThinboard.__del__(self)

	def __MakeTextLine(self):
		self.textLine = ui.TextLine()
		self.textLine.SetParent(self)
		self.textLine.SetWindowHorizontalAlignCenter()
		self.textLine.SetWindowVerticalAlignCenter()
		self.textLine.SetHorizontalAlignCenter()
		self.textLine.SetVerticalAlignCenter()
		self.textLine.Show()

	def __MakeCheckIcon(self):
		self.check_icon = ui.ImageBox()
		self.check_icon.SetParent(self)
		self.check_icon.LoadImage("d:/ymir work/ui/game/premium_private_shop/check_icon.sub")
		self.check_icon.SetWindowVerticalAlignCenter()
		self.check_icon.Hide()
		
	def __CalcFontColor(self):
		(name, path, text_color) = privateShop.GetTitleDeco(self.type)
		text_color = int(text_color, 16)
		
		RED		= (text_color & 0xFF0000) >> 16
		GREEN	= (text_color & 0x00FF00) >> 8
		BLUE	= text_color & 0x0000FF
		
		self.textLine.SetFontColor(float(RED)/255.0, float(GREEN)/255.0, float(BLUE)/255.0)

	def __ShowCheckIcon(self):
		if self.vid not in g_openedPrivateShopBoardList:
			return

		if privateShop.GetName(self.vid) != player.GetMainCharacterName():
			self.check_icon.Show()

	def __HideCheckIcon(self):
		self.check_icon.Hide()

	def Open(self, vid, text):
		self.vid = vid

		self.textLine.SetText(text)
		self.textLine.UpdateRect()

		width = len(text) * 6 + 10 * 5
		height = 20

		self.check_icon.SetPosition(width - self.check_icon.GetWidth() - 10, 0)
		self.check_icon.SetWindowVerticalAlignCenter()

		self.SetBoardSize(width, height) #ui.ShopDecoThinboard
		self.Show() 
				
		g_privateShopTitleBoardDict[vid] = self

		self.__ShowCheckIcon()
		
	def OnMouseLeftButtonUp(self):
		if not self.vid:
			return False

		if self.vid not in g_openedPrivateShopBoardList:
			g_openedPrivateShopBoardList.append(self.vid)

		self.__ShowCheckIcon()

		net.SendPrivateShopStartPacket(self.vid)
		return True
		
	def OnUpdate(self):
		if not self.vid:
			return True

		x, y = privateShop.GetProjectPosition(self.vid, 230)
		width = x - self.GetWidth()/2
		height = y - self.GetHeight()/2
		self.SetPosition(width, height)
	
class PrivateShopDecorationDialog(ui.ScriptWindow):

	SLOT_MAX_COUNT = 9
	MODE_APPEARANCE = 1
	MODE_TITLE_TYPE = 2
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.wndPrivateShop = None
		self.modeButtonList = []
		self.buttonList = []
		self.firstSlotIndex = 0
		
		self.selectedAppearanceIdx = 0
		self.selectedTitleTypeIdx = 0
		
		self.mode = self.MODE_APPEARANCE
		
		self.__LoadWindow()
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/privateshopdecorationdialog.py")
			
			self.modeTitle = self.GetChild("ModeTitleText")
			self.background = self.GetChild("BackgroundBoard")
			self.scroll_bar = self.GetChild("ScrollBar")
			self.modeButtonList.append(self.GetChild("AppearanceButton"))
			self.modeButtonList.append(self.GetChild("TitleButton"))
			
			self.modeButtonList[0].SAFE_SetEvent(self.__OnClickAppearanceMode)
			self.modeButtonList[1].SAFE_SetEvent(self.__OnClickTitleMode)
			
			self.scroll_bar.SetScrollStep(0.15)
			self.scroll_bar.SetScrollEvent(ui.__mem_func__(self.__OnScrollResultList))
		except:
			import exception
			exception.Abort("PrivateShopDecoration.LoadWindow.LoadObject")

		try:
			self.Board = self.GetChild("Board")
			
			for i in range(self.SLOT_MAX_COUNT):
				button = ui.Button()
				button.SetParent(self)
				button.SetPosition(17, 41 + i*26)
				
				button.SetUpVisual("d:/ymir work/ui/game/myshop_deco/select_btn_01.sub")
				button.SetOverVisual("d:/ymir work/ui/game/myshop_deco/select_btn_02.sub")
				button.SetDownVisual("d:/ymir work/ui/game/myshop_deco/select_btn_03.sub")
				
				button.SetScale(1.13, 1.0)
				
				button.SetEvent(ui.__mem_func__(self.OnSelectButton), i)
				
				button.Hide()
				
				self.buttonList.append(button)
		except:
			import exception
			exception.Abort("PrivateShopDecoration.LoadWindow.BindObject")
			
	def BindPrivateShopWindow(self, wndPrivateShop):
		self.wndPrivateShop = wndPrivateShop
			
	def Open(self):
		self.__OnClickAppearanceMode()
		
		self.wndPrivateShop.SetShopAppearance(self.selectedAppearanceIdx)
		self.wndPrivateShop.SetShopTitleType(self.selectedTitleTypeIdx)
		
		self.Show()
		
	def Close(self):
		self.selectedAppearanceIdx = 0
		self.selectedTitleTypeIdx = 0
		self.firstSlotIndex = 0
		self.scroll_bar.SetPos(0.0)
		
		self.Hide()
		
	def __OnClickAppearanceMode(self):
		self.mode = self.MODE_APPEARANCE
		
		if privateShop.GetAppearanceDecoMaxCount() <= self.SLOT_MAX_COUNT:
			self.scroll_bar.Hide()
			self.firstSlotIndex = 0
		else:
			self.scroll_bar.SetPos(0.0)
			self.scroll_bar.SetMiddleBarSize(float(self.SLOT_MAX_COUNT) / float(privateShop.GetAppearanceDecoMaxCount()))
			self.scroll_bar.Show()
				
		self.Refresh()
		
	def	__OnClickTitleMode(self):
		self.mode = self.MODE_TITLE_TYPE
		
		if privateShop.GetTitleDecoMaxCount() <= self.SLOT_MAX_COUNT:
			self.scroll_bar.Hide()
			self.firstSlotIndex = 0
		else:
			self.scroll_bar.SetPos(0.0)
			self.scroll_bar.SetMiddleBarSize(float(self.MAX_CATSLOT_MAX_COUNTEGORY) / float(privateShop.GetTitleDecoMaxCount()))
			self.scroll_bar.Show()
				
		self.Refresh()
		
	def Refresh(self):
		if self.mode == self.MODE_APPEARANCE:
		
			self.modeButtonList[0].Down()
			self.modeButtonList[1].SetUp()
			self.modeTitle.SetText(uiScriptLocale.MYSHOP_DECO_SELECT_MODEL)
				
			for i, button in enumerate(self.buttonList):
				if i >= privateShop.GetAppearanceDecoMaxCount():
					button.Hide()
					continue
				
				idx = i + self.firstSlotIndex
				
				(name, vnum) = privateShop.GetAppearanceDeco(idx)
				button.SetText(name)
				button.Show()
				
				# Select previously seleceted appearance
				if idx == self.selectedAppearanceIdx:
					button.Down()
					button.Disable()
				else:
					button.SetUp()
					button.Enable()
					
				# Re-position buttons if scroll bar isn't needed
				xPos = 17
				if privateShop.GetAppearanceDecoMaxCount() <= self.SLOT_MAX_COUNT:
					xPos += 7
					
				button.SetPosition(xPos, 41 + i*26)
				
		elif self.mode == self.MODE_TITLE_TYPE:
		
			self.modeButtonList[0].SetUp()
			self.modeButtonList[1].Down()
			self.modeTitle.SetText(uiScriptLocale.MYSHOP_DECO_SELECT_TITLE)
			
			for i, button in enumerate(self.buttonList):
				if i >= privateShop.GetTitleDecoMaxCount():
					button.Hide()
					continue
				
				idx = i + self.firstSlotIndex
				
				(name, path, text_color) = privateShop.GetTitleDeco(idx)
				button.SetText(name)
				button.Show()
				
				# Select previously seleceted style
				if idx == self.selectedTitleTypeIdx:
					button.Down()
					button.Disable()
				else:
					button.SetUp()
					button.Enable()
					
				# Re-position buttons if scroll bar isn't needed
				xPos = 17
				if privateShop.GetTitleDecoMaxCount() <= self.SLOT_MAX_COUNT:
					xPos += 7
					
				button.SetPosition(xPos, 41 + i*26)
					
	def __OnScrollResultList(self):
		max_count = 0
		
		if self.mode == self.MODE_APPEARANCE:
			max_count = privateShop.GetAppearanceDecoMaxCount()
		elif self.mode == self.MODE_TITLE_TYPE:
			max_count = privateShop.GetTitleDecoMaxCount()
			
		scrollLineCount = max(0, max_count - self.SLOT_MAX_COUNT)
		startIndex = int(scrollLineCount * self.scroll_bar.GetPos())

		if startIndex != self.firstSlotIndex:
			self.firstSlotIndex = startIndex
			self.Refresh()
			
	def OnRunMouseWheel(self, nLen):
		if self.background.IsIn():
			if nLen > 0:
				self.scroll_bar.OnUp()
			else:
				self.scroll_bar.OnDown()
				
			return True
				
		for button in self.buttonList:
			if button.IsIn():
				if nLen > 0:
					self.scroll_bar.OnUp()
				else:
					self.scroll_bar.OnDown()
					
				return True
				
		return False
				
	def OnSelectButton(self, index):
		for button in self.buttonList :
			button.SetUp()
			button.Enable()
			
		self.buttonList[index].Down()
		self.buttonList[index].Disable()
		
		if self.mode == self.MODE_APPEARANCE:
			self.selectedAppearanceIdx = index + self.firstSlotIndex
			self.wndPrivateShop.SetShopAppearance(self.selectedAppearanceIdx)
			
		elif self.mode == self.MODE_TITLE_TYPE:
			self.selectedTitleTypeIdx = index + self.firstSlotIndex
			self.wndPrivateShop.SetShopTitleType(self.selectedTitleTypeIdx)
		
	def AdjustPosition(self):
		(x, y) = self.wndPrivateShop.GetGlobalPosition()
		
		x_pos = x - 210
		y_pos = y + 75

		self.SetPosition(x_pos, y_pos)

# It would probably be wise to refactor the class and set each mode as
# a separate class instead of having state cluster-fuck	
class PrivateShopPanel(ui.ScriptWindow):

	MODE_BUILD			= 1
	MODE_DECO			= 2
	MODE_DEFAULT		= 3
	MODE_SALE_ITEM		= 4
	
	BUILD_DEFAULT		= 1
	BUILD_PREMIUM		= 2
	
	MAX_SALE_ITEM_COUNT = 14
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.toolTip			= None
		self.toolTipItem		= None
		self.interface			= None
		self.wndInventory			= None
		self.wndDragonSoulInventory	= None
		# self.wndSpecialInventory	= None
		self.questionDialog		= None
		self.priceInputDialog	= None
		self.inputDialog		= None
		self.decoDialog			= None
		self.privateShopTab		= []
		self.saleItemSlotList	= []
		self.saleItemList		= []
		self.selectedItemPos	= -1
		self.checkinSrcPos		= -1
		self.checkinSrcWindow	= -1
		self.checkinDstPos		= -1
		self.lastModifyTime		= 0
		self.page				= 0
		
		# Build Private Shop Info
		self.firstSlotIndex		= 0
		self.itemStockDict		= {}
		self.buildTitle			= ""
		self.shopAppearance		= 0
		self.titleAppearance	= 0
		self.buildMode			= self.BUILD_DEFAULT
		self.mode				= self.MODE_DEFAULT
		self.grids				= []
		
		for i in range(privateShop.PRIVATE_SHOP_PAGE_MAX_NUM):
			self.grids.append(Grid(privateShop.PRIVATE_SHOP_WIDTH, privateShop.PRIVATE_SHOP_HEIGHT))
		
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/PrivateShopWindow.py")
		except:
			import exception
			exception.Abort("PrivateShopWindow.LoadWindow.LoadObject")

		try:
			self.board			= self.GetChild("board")
			self.locationButton	= self.GetChild("location_button")
			self.titleText		= self.GetChild("shop_name_text")
			self.titleButton	= self.GetChild("shop_name_text_button")
			self.titleButtonFlash	= self.GetChild("shop_name_text_flash_ani")
			self.itemSlot		= self.GetChild("item_slot")
			self.shopNoticeText	= self.GetChild("shop_notice_text")

			self.tab_button_group	= self.GetChild("TabButtonGroup")
			
			self.privateShopTab.append(self.GetChild("tab1"))
			self.privateShopTab.append(self.GetChild("tab2"))
			
			self.background_group			= self.GetChild("BackgroundGroup")
			self.render_title				= self.GetChild("RenderTitle")
			self.render_title_text			= self.GetChild("RenderTitleText")
			if app.ENABLE_RENDER_TARGET:
				self.render_target				= self.GetChild("RenderTarget")
		
			self.information_group			= self.GetChild("information_group")
			self.remainTimeText				= self.GetChild("remain_time_text")
			self.goldText					= self.GetChild("gold_text")
			if app.ENABLE_PRIVATE_SHOP_CHEQUE:
				self.chequeText				= self.GetChild("cheque_text")
			
			self.button_group				= self.GetChild("button_group")
			self.decoButton					= self.GetChild("deco_button")
			self.modifyButton				= self.GetChild("modify_button")
			self.reopenButton				= self.GetChild("reopen_button")
			self.withdrawButton				= self.GetChild("tax_adjustment_button")
			self.closeButton				= self.GetChild("shop_close_button")
			self.saleButton					= self.GetChild("shop_sales_button")
			
			self.sandglassIcon				= self.GetChild("sandglass_icon")
			if app.ENABLE_PRIVATE_SHOP_CHEQUE:
				self.chequeIcon				= self.GetChild("cheque_icon")
			self.goldIcon					= self.GetChild("gold_icon")

			self.item_sale_group			= self.GetChild("ItemSaleGroup")
			self.scroll_bar					= self.GetChild("ItemSaleScrollBar")
			
			self.board.SetCloseEvent(ui.__mem_func__(self.Close))
			
			# Icon Tooltips
			self.sandglassIcon.SetEvent(ui.__mem_func__(self.__IconToolTip), "mouse_over_in", "TIME")
			self.sandglassIcon.SetEvent(ui.__mem_func__(self.__IconToolTip), "mouse_over_out", "TIME")
			
			if app.ENABLE_PRIVATE_SHOP_CHEQUE:
				self.chequeIcon.SetEvent(ui.__mem_func__(self.__IconToolTip), "mouse_over_in", "CHEQUE")
				self.chequeIcon.SetEvent(ui.__mem_func__(self.__IconToolTip), "mouse_over_out", "CHEQUE")
			
			self.goldIcon.SetEvent(ui.__mem_func__(self.__IconToolTip), "mouse_over_in", "GOLD")
			self.goldIcon.SetEvent(ui.__mem_func__(self.__IconToolTip), "mouse_over_out", "GOLD")
			
			# Button Tooltips
			self.modifyButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "MODIFY")
			self.modifyButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
			self.modifyButton.SetEvent(ui.__mem_func__(self.__OnClickModifyPrivateShop))
			
			self.withdrawButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "WITHDRAW")
			self.withdrawButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
			self.withdrawButton.SetEvent(ui.__mem_func__(self.__OnClickWithdrawPrivateShop))
			
			self.reopenButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "REOPEN")
			self.reopenButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
			self.reopenButton.SetEvent(ui.__mem_func__(self.__OnClickOpenPrivateShop))
			
			self.closeButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "CLOSE")
			self.closeButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
			self.closeButton.SetEvent(ui.__mem_func__(self.__OnClickClosePrivateShop))
			
			self.saleButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "SALE_ITEM")
			self.saleButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
			self.saleButton.SetEvent(ui.__mem_func__(self.__OnClickViewSaleButton))
			
			self.titleButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "CHANGE_TITLE")
			self.titleButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
			self.titleButton.SetEvent(ui.__mem_func__(self.__OpenTitleChangeDialog))
			
			self.decoButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "DECORATE")
			self.decoButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
			self.decoButton.SetEvent(ui.__mem_func__(self.__OnClickDecoPrivateShop))

			self.locationButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "LOCATION")
			self.locationButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
			self.locationButton.SetEvent(ui.__mem_func__(self.__OnClickWarpPrivateShop))
			
			self.scroll_bar.SetScrollStep(0.15)
			self.scroll_bar.SetScrollEvent(ui.__mem_func__(self.__OnScrollResultList))
		
			for i in range(privateShop.PRIVATE_SHOP_PAGE_MAX_NUM):
				self.privateShopTab[i].SetEvent(lambda arg=i: self.SetPrivateShopPage(arg))

			self.itemSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.OnSelectItemSlot))
			self.itemSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.OnSelectEmptySlot))
			self.itemSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.OnUnselectItemSlot))
			self.itemSlot.SetUseSlotEvent(ui.__mem_func__(self.__UseItemSlot))
			self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.__OnOverInItem))
			self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OnOverOutItem))
				
			self.toolTip = uiToolTip.ToolTip()
			self.toolTip.HideToolTip()
			
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.Hide()

			self.priceInputDialog = uiCommon.PrivateShopPriceInputDialog()
			
			self.CreateSaleItemSlots()
			self.SetPrivateShopPage(0)
			
			self.decoDialog = PrivateShopDecorationDialog()
			self.decoDialog.BindPrivateShopWindow(self)
			self.decoDialog.Hide()

			if app.ENABLE_RENDER_TARGET:
				renderTarget.SetBackground(RENDER_TARGET_INDEX, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
				renderTarget.SetVisibility(RENDER_TARGET_INDEX, False)
			
		except:
			import exception
			exception.Abort("PrivateShopWindow.LoadWindow.BindObject")
			
	def CreateSaleItemSlots(self):
		yPos = 21
		
		for i in range(self.MAX_SALE_ITEM_COUNT):
			itemSlotButton = ui.Button()
			itemSlotButton.SetParent(self.item_sale_group)
			itemSlotButton.SetPosition(0, yPos)
			
			itemSlotButton.SetUpVisual("d:/ymir work/ui/game/premium_private_shop/shop_sale_slot_default.sub")
			itemSlotButton.SetOverVisual("d:/ymir work/ui/game/premium_private_shop/shop_sale_slot_over.sub")
			itemSlotButton.SetDownVisual("d:/ymir work/ui/game/premium_private_shop/shop_sale_slot_over.sub")

			itemSlotButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInItem), i)
			itemSlotButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutItem))
			
			itemNameText = ui.TextLine()
			itemNameText.SetParent(itemSlotButton)
			itemNameText.SetPosition(5, 2)
			itemNameText.SetText("Item Name Text")
			itemNameText.Show()
			
			dateText = ui.TextLine()
			dateText.SetParent(itemSlotButton)
			dateText.SetPosition(175, 2)
			dateText.SetText("Date Text")
			dateText.Show()
			
			itemSlotButton.Hide()
			
			yPos += itemSlotButton.GetHeight() + 1
			
			self.saleItemSlotList.append(itemSlotButton)
			self.saleItemList.append((itemNameText, dateText))
			
	def BindInterfaceClass(self, interface):
		self.interface = interface
		
	def BindInventoryClass(self, inventory):
		self.wndInventory = inventory

	def BindDragonSoulInventoryClass(self, dragonSoulInventory):
		self.wndDragonSoulInventory = dragonSoulInventory

	# def BindSpecialInventoryClass(self, specialInventory):
	# 	self.wndSpecialInventory = specialInventory
		
	def SetItemToolTip(self, tooltipItem):
		self.toolTipItem = tooltipItem

	def Destroy(self):
		self.ClearDictionary()
		
		if self.questionDialog:
			self.questionDialog.Destroy()
			
		self.privateShopTab = []
		self.selectedItemPos	= -1
		self.checkinSrcPos		= -1
		self.checkinSrcWindow	= -1
		self.checkinDstPos		= -1
		self.lastModifyTime		= 0
		self.buildTitle			= ""
		self.itemStockDict		= {}
		self.shopAppearance		= 0
		self.titleAppearance	= 0
		self.grids				= []

		self.toolTip				= None
		self.toolTipItem			= None
		self.interface				= None
		self.wndInventory			= None
		self.wndDragonSoulInventory	= None
		# self.wndSpecialInventory	= None
		self.questionDialog			= None
		self.priceInputDialog		= None
		self.inputDialog			= None
		self.decoDialog				= None

	def __GetSlotLocalPosition(self, slotIndex):
		start_x, start_y = self.itemSlot.GetLocalPosition()

		index = 0
		for i in range(privateShop.PRIVATE_SHOP_HEIGHT):
			for j in range(privateShop.PRIVATE_SHOP_WIDTH):
				if index == slotIndex:
					return (start_x + j * 32, start_y + i * 32)

				index += 1

		return (-1, -1)

	def __RepositionPriceInputDlg(self, localSlotIndex):
		# Adapt position of the board
		gx, gy = self.GetGlobalPosition()
		sx, sy = self.__GetSlotLocalPosition(localSlotIndex)

		priceBoardWidth = self.priceInputDialog.GetWidth()
		privateShopWidth = self.GetWidth()
		lx = (privateShopWidth - priceBoardWidth) / 2

		priceBoardHeight = self.priceInputDialog.GetHeight()
		# privateShopHeight = self.GetHeight()
		# ly = privateShopHeight / 2 - priceBoardHeight / 2
		ly = sy

		# TODO: Use item size to determine height difference
		if localSlotIndex >= privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM / 2:
			ly -= (priceBoardHeight + 16)
		else:
			ly += 36
		
		self.priceInputDialog.SetPosition(gx + lx, gy + ly)

	def __IsLockedSlot(self, slotIndex):
		if not app.ENABLE_PRIVATE_SHOP_LOCKED_SLOTS:
			return False

		if slotIndex >= privateShop.GetUnlockedSlots() + privateShop.PRIVATE_SHOP_LOCKED_SLOT_MAX_NUM:	
			return True

		return False
		
	def Open(self):
		# Clear build information in case we had build window open before opening another
		self.ClearBuildInfo()

		self.SetPrivateShopPage(0)
		
		self.RefreshWindow()
		self.Refresh()
		
		self.Show()
		self.SetTop()

		self.titleButtonFlash.Hide()
		
		if privateShop.IsMainPlayerPrivateShop():
			if privateShop.GetMyState() == privateShop.STATE_MODIFY or self.mode == self.MODE_BUILD:
				self.interface.SetOnTopWindow(player.ON_TOP_WND_PRIVATE_SHOP)
				self.interface.RefreshMarkInventoryBag()
			
			if privateShop.GetMyState() == privateShop.STATE_MODIFY:
				if not privateShop.IsMarketItemPriceDataLoaded():
					net.SendPrivateShopMarketItemPriceDataReq()

			if self.mode == self.MODE_BUILD:
				if app.ENABLE_PRIVATE_SHOP_BUNDLE_REQ:
					bundlePos = player.GetItemSlotIndex(50200)
					decoBundlePos = player.GetItemSlotIndex(71221)
					
					if bundlePos < 0 and decoBundlePos < 0:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_NO_BUNDLE_ITEM)

				if self.buildTitle == '':
					self.titleButtonFlash.Show()
		
	def RequestOpen(self):
		# Return if panel window is already shown
		if self.IsShow() and privateShop.IsMainPlayerPrivateShop():
			return False
			
		net.SendOpenPrivateShopPanelPacket()
		return True

	def ClearBuildInfo(self):
		if len(self.itemStockDict):
			for privatePos, (windowType, itemSlotPos) in self.itemStockDict.items():
			
				itemInvenPage = itemSlotPos / player.INVENTORY_PAGE_SIZE
				localSlotPos = itemSlotPos - (itemInvenPage * player.INVENTORY_PAGE_SIZE)
		
				if self.wndInventory.GetInventoryPageIndex() == itemInvenPage:
					self.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
					

		# Reset state of the deco button
		if self.mode == self.MODE_DECO:
			self.__OnClickDecoPrivateShop()

			# Hide the tooltip
			self.__OnOverOutButton()

		self.itemStockDict		= {}
		self.buildTitle			= ""
		self.shopAppearance			= 0
		self.titleAppearance		= 0
		self.buildMode				= self.BUILD_DEFAULT
		privateShop.ClearPrivateShopStock()
		
	def Close(self, bSendServer = True):
		if self.questionDialog:
			self.questionDialog.Close()
			
		if self.decoDialog.IsShow():
			self.decoDialog.Close()

		if self.selectedItemPos:
			self.CancelInputPrice()
		
		if self.checkinDstPos:
			self.CancelItemCheckin()

		self.lastModifyTime		= 0
		
		if self.IsShow() and bSendServer:
			if privateShop.IsMainPlayerPrivateShop():
				net.SendClosePrivateShopPanelPacket()
			else:
				net.SendPrivateShopEndPacket()

		self.Hide()
		self.ClearBuildInfo()

		# Return to build mode if we're decorating the shop and closing after
		if self.mode == self.MODE_DECO:
			self.mode = self.MODE_BUILD

		self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
		self.interface.RefreshMarkInventoryBag()
		
	def OnTop(self):
		if self.interface and privateShop.IsMainPlayerPrivateShop() and (privateShop.GetMyState() == privateShop.STATE_MODIFY or self.mode == self.MODE_BUILD):
			self.interface.SetOnTopWindow(player.ON_TOP_WND_PRIVATE_SHOP)
			self.interface.RefreshMarkInventoryBag()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def __GetShopNoticeText(self, state):
		if state == privateShop.STATE_CLOSED:
			return uiScriptLocale.PREMIUM_PRIVATE_SHOP_NOTICE_NONE
			
		elif state == privateShop.STATE_OPEN:
			return uiScriptLocale.PREMIUM_PRIVATE_SHOP_NOTICE_OPEN
			
		elif state == privateShop.STATE_MODIFY:
			return uiScriptLocale.PREMIUM_PRIVATE_SHOP_NOTICE_MODIFY
			
		return uiScriptLocale.PREMIUM_PRIVATE_SHOP_NOTICE_NONE
			
	def Refresh(self):
		# Title
		title = ""
		if privateShop.IsMainPlayerPrivateShop():
			if self.mode == self.MODE_DEFAULT or self.mode == self.MODE_SALE_ITEM:
				title = privateShop.GetMyTitle()
			elif self.mode == self.MODE_BUILD or self.mode == self.MODE_DECO:
				title = self.buildTitle
		else:
			title = privateShop.GetTitle()
			
		self.titleText.SetText(title)
		
		# Shop Notice
		notice = ""
		if privateShop.IsMainPlayerPrivateShop():
			notice = self.__GetShopNoticeText(privateShop.GetMyState())
		else:
			notice = self.__GetShopNoticeText(privateShop.GetState())
			
		self.shopNoticeText.SetText(notice)
		
		# Remaining Time
		premiumTime = privateShop.GetPremiumTime()
		remainingTime = premiumTime - app.GetGlobalTimeStamp()
		
		if remainingTime > 0:
			self.remainTimeText.SetText(localeInfo.SecondToDHM(remainingTime))
		else:
			self.remainTimeText.SetText(localeInfo.PREMIUM_PRIVATE_SHOP_TIME_EXPIRED)
		
		(gold, cheque) = (privateShop.GetGold(), privateShop.GetCheque())
		if self.mode == self.MODE_BUILD:
			(gold, cheque) = self.__GetPrivateShopWorth()

		# Yang
		self.goldText.SetText(localeInfo.NumberToMoneyStringNoUnit(gold))
		
		# Cheque
		if app.ENABLE_PRIVATE_SHOP_CHEQUE:
			self.chequeText.SetText(localeInfo.NumberToMoneyStringNoUnit(cheque))

		# Item Slot
		for i in range(privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM):
			slotIndex = self.__LocalPosToGlobalPos(i)

			self.itemSlot.ClearSlot(i)

			if self.__IsLockedSlot(slotIndex):
				self.itemSlot.SetItemSlot(i, 0, 0)
				self.itemSlot.SetCoverButton(i, LOCKED_SLOT_PATH, LOCKED_SLOT_PATH, LOCKED_SLOT_PATH, LOCKED_SLOT_PATH, True, True)
				self.itemSlot.SetAlwaysRenderCoverButton(i)
				continue

			if self.mode == self.MODE_DEFAULT:
				itemCount = privateShop.GetItemCount(slotIndex)
				if itemCount <= 1:
					itemCount = 0

				self.itemSlot.SetItemSlot(i, privateShop.GetItemVnum(slotIndex), itemCount)
				
			elif self.mode == self.MODE_BUILD:
				# We're on a 2nd page, show locked slots
				if not self.itemStockDict.has_key(slotIndex):
					continue
			
				type, pos = self.itemStockDict[slotIndex]
				
				itemCount = player.GetItemCount(type, pos)
				
				if itemCount <= 1:
					itemCount = 0 

				self.itemSlot.SetItemSlot(i, player.GetItemIndex(type, pos), itemCount)
				
		self.itemSlot.RefreshSlot()
		
		# Sale Item Slot
		for i in range(self.MAX_SALE_ITEM_COUNT):

			if i >= privateShop.GetSaleItemMaxCount() or self.mode != self.MODE_SALE_ITEM:
				self.saleItemSlotList[i].Hide()
				continue
			
			item_id = i + self.firstSlotIndex
			(item_name, timestamp) = privateShop.GetSaleItem(item_id)
			
			(itemNameText, dateText) = self.saleItemList[i]
			
			itemNameText.SetText(item_name)
			dateText.SetText(localeInfo.GetDateFormat(timestamp))

			self.saleItemSlotList[i].Show()
			
		self.RefreshLockedSlot()

		if self.mode == self.MODE_DEFAULT or self.mode == self.MODE_BUILD:
			self.RefreshGrid()
		
	def RefreshWindow(self):
		page_count = 0
		if privateShop.IsMainPlayerPrivateShop():
			page_count = privateShop.GetMyPageCount()
		else:
			page_count = self.__GetPrivateShopMaxPage()

			# Reset the mode as well in case it was sale history, ...
			self.mode = self.MODE_DEFAULT

		if privateShop.IsMainPlayerPrivateShop():
		
			# Determine mode based on the page count if we're on the default window
			if self.mode == self.MODE_DEFAULT and page_count <= 0:
				self.mode = self.MODE_BUILD
				
			elif (self.mode == self.MODE_BUILD or self.mode == self.MODE_DECO) and page_count > 0:
				self.mode = self.MODE_DEFAULT
				
			self.button_group.Show()

			if self.mode == self.MODE_BUILD:

				self.itemSlot.Show()
				self.information_group.Show()
				self.item_sale_group.Hide()
				if app.ENABLE_RENDER_TARGET:
					self.render_target.Hide()
				self.render_title.Hide()
				self.render_title_text.Hide()
				
				self.decoButton.Show()
				self.modifyButton.Hide()
				self.withdrawButton.Hide()
				self.reopenButton.Show()
				self.closeButton.Hide()
				self.saleButton.Show()
				
				self.decoButton.Enable()
				self.modifyButton.Disable()
				self.withdrawButton.Disable()
				self.reopenButton.Enable()
				self.closeButton.Disable()
				self.saleButton.Enable()
				
				self.titleButton.Enable()
				
				for slot in self.saleItemSlotList:
					slot.Hide()
				
				if self.GetBuildMode() == self.BUILD_DEFAULT:
					self.tab_button_group.Hide()
					page_count = 1

				elif self.GetBuildMode() == self.BUILD_PREMIUM:
					self.tab_button_group.Show()
					page_count = privateShop.PRIVATE_SHOP_PAGE_MAX_NUM
					
			elif self.mode == self.MODE_DECO:
				self.tab_button_group.Hide()
				self.itemSlot.Hide()
				self.information_group.Hide()
				self.item_sale_group.Hide()
				if app.ENABLE_RENDER_TARGET:
					self.render_target.Show()
				self.render_title.Show()
				self.render_title_text.Show()
				
				self.decoButton.Show()
				self.modifyButton.Hide()
				self.withdrawButton.Hide()
				self.reopenButton.Show()
				self.closeButton.Hide()
				self.saleButton.Show()
				
				self.decoButton.Enable()
				self.modifyButton.Disable()
				self.withdrawButton.Disable()
				self.reopenButton.Enable()
				self.closeButton.Disable()
				self.saleButton.Disable()
				
				self.titleButton.Enable()
				
				for slot in self.saleItemSlotList:
					slot.Hide()
				
				# Just a workaround for keepign the same size as at build mode
				self.tab_button_group.Hide()
				
				if self.GetBuildMode() == self.BUILD_DEFAULT:
					page_count = 1

				elif self.GetBuildMode() == self.BUILD_PREMIUM:
					page_count = privateShop.PRIVATE_SHOP_PAGE_MAX_NUM

			elif self.mode == self.MODE_DEFAULT:
				self.itemSlot.Show()
				self.information_group.Show()
				self.item_sale_group.Hide()
				if app.ENABLE_RENDER_TARGET:
					self.render_target.Hide()
				self.render_title.Hide()
				self.render_title_text.Hide()
				
				self.decoButton.Hide()
				self.modifyButton.Show()
				self.withdrawButton.Show()
				self.reopenButton.Hide()
				self.closeButton.Show()
				self.saleButton.Show()
				
				self.decoButton.Disable()
				self.modifyButton.Enable()
				self.withdrawButton.Enable()
				self.reopenButton.Enable()
				self.closeButton.Enable()
				self.titleButton.Enable()
				self.saleButton.Enable()

				for slot in self.saleItemSlotList:
					slot.Hide()
					
				if page_count <= 1:
					self.tab_button_group.Hide()
				else:
					self.tab_button_group.Show()

			elif self.mode == self.MODE_SALE_ITEM:
				self.itemSlot.Hide()
				self.tab_button_group.Hide()
				self.information_group.Hide()
				self.item_sale_group.Show()
				if app.ENABLE_RENDER_TARGET:
					self.render_target.Hide()
				self.render_title.Hide()
				self.render_title_text.Hide()
				
				# Disable other buttons while in sale-history mode
				self.decoButton.Disable()
				self.modifyButton.Disable()
				self.withdrawButton.Disable()
				self.reopenButton.Disable()
				self.closeButton.Disable()
				self.titleButton.Disable()
				self.saleButton.Enable()
				
				for slot in self.saleItemSlotList:
					slot.Show()
					
				if privateShop.GetSaleItemMaxCount() > self.MAX_SALE_ITEM_COUNT:
					if privateShop.GetSaleItemMaxCount():
						step = max(0.02, 1.0 / privateShop.GetSaleItemMaxCount())
						self.scroll_bar.SetScrollStep(step)
					
					self.scroll_bar.SetMiddleBarSize(float(self.MAX_SALE_ITEM_COUNT) / float(privateShop.GetSaleItemMaxCount()))
					self.scroll_bar.Show()
				else:
					self.scroll_bar.Hide()

			self.background_group.Show()
			
			window_height = WINDOW_HEIGHT
			
			if page_count <= 1:
				self.information_group.SetPosition(INFORMATION_GROUP_X, INFORMATION_GROUP_Y - TAB_BUTTON_Y_DELTA / 2)
			else:
				self.information_group.SetPosition(INFORMATION_GROUP_X, INFORMATION_GROUP_Y)
						
			#Update size
			self.board.SetSize(WINDOW_WIDTH, window_height)
			self.SetSize(WINDOW_WIDTH, window_height)
		else:
		
			#Hide unnecessary UI elements
			self.information_group.Hide()
			self.button_group.Hide()
			self.scroll_bar.Hide()
			self.background_group.Hide()
			self.item_sale_group.Hide()
			if app.ENABLE_RENDER_TARGET:
				self.render_target.Hide()
			self.render_title.Hide()
			self.render_title_text.Hide()
			self.itemSlot.Show()

			if page_count <= 1:
				self.tab_button_group.Hide()
			else:
				self.tab_button_group.Show()
			
			window_height = SHOP_WINDOW_HEIGHT
			
			if page_count <= 1:
				window_height -= TAB_BUTTON_Y_DELTA
			
			#Update size and position
			self.board.SetSize(WINDOW_WIDTH, window_height)
			self.SetSize(WINDOW_WIDTH, window_height)
			
			# Position the window in the upper right corner
			# self.SetPosition(SHOP_WINDOW_X_POS, SHOP_WINDOW_Y_POS)

	def RefreshGrid(self):
		# Reset old data
		for i in range(privateShop.PRIVATE_SHOP_PAGE_MAX_NUM):
			self.grids[i].reset()

		# Fill grids with locked slots
		for i in range(privateShop.PRIVATE_SHOP_HOST_ITEM_MAX_NUM):
			page = int(i / privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM)

			if self.__IsLockedSlot(i):
				grid_pos = i - int(i / privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM) * privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM
				self.grids[page].put(grid_pos, 1, 1)

		# Fill grids with items
		for i in range(privateShop.PRIVATE_SHOP_HOST_ITEM_MAX_NUM):
			page = int(i / privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM)
			
			item_vnum = -1

			if self.mode == self.MODE_DEFAULT:
				itemCount = privateShop.GetItemCount(i)
				if itemCount <= 0:
					# print("Position {} is empty".format(i))
					continue

				item_vnum = privateShop.GetItemVnum(i)

			elif self.mode == self.MODE_BUILD:
				if not self.itemStockDict.has_key(i):
					continue

				type, pos = self.itemStockDict[i]
				item_vnum = player.GetItemIndex(type, pos)

			item.SelectItem(item_vnum)
			(x_size, y_size) = item.GetItemSize()

			grid_pos = i - int(i / privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM) * privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM
			self.grids[page].put(grid_pos, x_size, y_size)

	def GetBuildMode(self):
		if not app.ENABLE_PRIVATE_SHOP_BUNDLE_REQ:
			return self.BUILD_PREMIUM

		return self.buildMode
		
	def SetPremiumBuildMode(self):
		self.buildMode = self.BUILD_PREMIUM
		
		if self.mode != self.MODE_DECO:
			self.__OnClickDecoPrivateShop()
			
			if self.toolTip.IsShow():
				self.toolTip.HideToolTip()

	def SetBuildMode(self, buildMode):
		self.buildMode = buildMode
			
	def SetPrivateShopPage(self, page):
		self.page = page
		
		for tab in self.privateShopTab:
			tab.SetUp()
			
		self.privateShopTab[page].Down()
		
		self.Refresh()
		
	def __OnClickModifyPrivateShop(self):
		if self.mode != self.MODE_DEFAULT:
			return
			
		if self.questionDialog.IsShow():
			return
			
		if not privateShop.IsMainPlayerPrivateShop():
			return
			
		remainingTime = app.GetGlobalTimeStamp() - self.lastModifyTime
		if remainingTime > 1:
			net.SendModifyPrivateShopPacket()
			self.lastModifyTime = app.GetGlobalTimeStamp() + 1
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_MODIFY_STATE_OPEN_FAIL_TIME)
		
	def __OnClickWithdrawPrivateShop(self):
		if self.questionDialog.IsShow():
			return
			
		if not privateShop.IsMainPlayerPrivateShop():
			return
			
		self.questionDialog.SetText(localeInfo.PREMIUM_PRIVATE_SHOP_WITHDRAW_QUESTIONG)
		self.questionDialog.SetAcceptEvent(lambda arg=True: self.__ShopWithdrawAnswer(arg))
		self.questionDialog.SetCancelEvent(lambda arg=False: self.__ShopWithdrawAnswer(arg))
		self.questionDialog.Open()
		
	def __ShopWithdrawAnswer(self, answer):
		self.questionDialog.Close()
		
		if answer:
			curGold = player.GetElk()
			shopGold = privateShop.GetGold()
			
			if (curGold + shopGold) > constInfo.GOLD_MAX:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_TAX_ADJUSTMENT_LIMIT_YANG)
				return
				
			if app.ENABLE_PRIVATE_SHOP_CHEQUE:
				shopCheque = privateShop.GetCheque()
				curCheque = player.GetCheque()

				if (curCheque + shopCheque) > constInfo.CHEQUE_MAX:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_TAX_ADJUSTMENT_LIMIT_WON)
					return
	
			net.SendWithdrawPrivateShopPacket()
		
	def __OnClickClosePrivateShop(self):
		if self.questionDialog.IsShow():
			return
			
		if not privateShop.IsMainPlayerPrivateShop():
			return
			
		self.questionDialog.SetText(localeInfo.PREMIUM_PRIVATE_SHOP_CLOSE_QUESTIONG)
		self.questionDialog.SetAcceptEvent(lambda arg=True: self.__ShopCloseAnswer(arg))
		self.questionDialog.SetCancelEvent(lambda arg=False: self.__ShopCloseAnswer(arg))
		self.questionDialog.Open()
		
	def __ShopCloseAnswer(self, answer):
		self.questionDialog.Close()
		
		if answer:
			net.SendClosePrivateShopPacket()
			
	def __OnClickViewSaleButton(self):

		if self.mode == self.MODE_DEFAULT or self.mode == self.MODE_BUILD:

			self.mode = self.MODE_SALE_ITEM
			
			self.saleButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "DEFAULT_WINDOW")

			# Update tooltip in real time
			self.__OnOverInButton("DEFAULT_WINDOW")
			
			self.saleButton.SetUpVisual(ROOT_PATH + "return_home_button_default.sub")
			self.saleButton.SetOverVisual(ROOT_PATH + "return_home_button_over.sub")
			self.saleButton.SetDownVisual(ROOT_PATH + "return_home_button_down.sub")
			
		elif self.mode == self.MODE_SALE_ITEM:
			if privateShop.GetMyPageCount():
				self.mode = self.MODE_DEFAULT
			else:
				self.mode = self.MODE_BUILD
			
			self.saleButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "SALE_ITEM")
			
			# Update tooltip in real time
			self.__OnOverInButton("SALE_ITEM")
			
			self.saleButton.SetUpVisual(ROOT_PATH + "sale_button_default.sub")
			self.saleButton.SetOverVisual(ROOT_PATH + "sale_button_over.sub")
			self.saleButton.SetDownVisual(ROOT_PATH + "sale_button_down.sub")

		self.RefreshWindow()
		self.Refresh()
		
	def __OnClickDecoPrivateShop(self):
		if self.mode != self.MODE_BUILD and self.mode != self.MODE_DECO:
			return
			
		if app.ENABLE_PRIVATE_SHOP_BUNDLE_REQ:
			if self.GetBuildMode() != self.BUILD_PREMIUM:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_NO_PREMIUM_BUNDLE_ITEM)
				return

		if self.mode == self.MODE_BUILD:
			self.mode = self.MODE_DECO
			
			self.decoButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "DEFAULT_WINDOW")
			
			# Update tooltip in real time
			self.__OnOverInButton("DEFAULT_WINDOW")
			
			self.decoButton.SetUpVisual(ROOT_PATH + "return_home_button_default.sub")
			self.decoButton.SetOverVisual(ROOT_PATH + "return_home_button_over.sub")
			self.decoButton.SetDownVisual(ROOT_PATH + "return_home_button_down.sub")
			
			self.decoDialog.Open()
			self.decoDialog.AdjustPosition()
			
			self.RefreshWindow()

			if app.ENABLE_RENDER_TARGET:
				renderTarget.SetVisibility(RENDER_TARGET_INDEX, True)
			
		elif self.mode == self.MODE_DECO:
			self.mode = self.MODE_BUILD
			
			self.decoButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "DECORATE")
			
			# Update tooltip in real time
			self.__OnOverInButton("DECORATE")
			
			self.decoButton.SetUpVisual(ROOT_PATH + "deco_button_default.sub")
			self.decoButton.SetOverVisual(ROOT_PATH + "deco_button_over.sub")
			self.decoButton.SetDownVisual(ROOT_PATH + "deco_button_down.sub")
			
			self.decoDialog.Hide()
			
			self.RefreshWindow()

			if app.ENABLE_RENDER_TARGET:
				renderTarget.SetVisibility(RENDER_TARGET_INDEX, False)
		
	def __OnClickOpenPrivateShop(self):
		page_count = 1

		if app.ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS:
			if not self.__CanBuildPrivateShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_BUILD_RESTRICTED)
				return
		
		if app.ENABLE_PRIVATE_SHOP_BUNDLE_REQ:
			bundlePos = player.GetItemSlotIndex(50200)
			decoBundlePos = player.GetItemSlotIndex(71221)
			
			if bundlePos < 0 and decoBundlePos < 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_NO_BUNDLE_ITEM)
				return
				
			# Adjust private shop settings incase of unintended behaviour
			if self.GetBuildMode() == self.BUILD_PREMIUM:
				page_count = privateShop.PRIVATE_SHOP_PAGE_MAX_NUM
			else:
				self.shopAppearance = 0
				self.titleAppearance = 0
				
		else:
			page_count = privateShop.PRIVATE_SHOP_PAGE_MAX_NUM
		
		if len(self.buildTitle) < privateShop.TITLE_MIN_LEN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_SHORT_TITLE)
			return
		
		privateShop.BuildPrivateShop(self.buildTitle, self.shopAppearance, self.titleAppearance, page_count)

		# Do not close the window just yet
		# self.Close()

	def __OnClickWarpPrivateShop(self):
		if not privateShop.IsMainPlayerPrivateShop():
			return
			
		net.SendPrivateShopWarpRequest()
			
	def OpenBuyItemDialog(self, slotPos):
		if self.questionDialog.IsShow():
			return
			
		itemVnum = privateShop.GetItemVnum(slotPos)

		goldPrice = privateShop.GetItemPrice(slotPos)
		chequePrice = privateShop.GetChequeItemPrice(slotPos)
			
		itemCount = privateShop.GetItemCount(slotPos)

		item.SelectItem(itemVnum)
		itemName = item.GetItemName()
		
		if app.ENABLE_PRIVATE_SHOP_CHEQUE:
			self.questionDialog.SetText(localeInfo.DO_YOU_BUY_ITEM(itemName, itemCount, localeInfo.NumberToMoneyString(goldPrice), chequePrice))
		else:
			self.questionDialog.SetText(localeInfo.DO_YOU_BUY_ITEM(itemName, itemCount, localeInfo.NumberToMoneyStringNoUnit(goldPrice)))

		self.questionDialog.SetAcceptEvent(lambda arg=True: self.__BuyItemAnswer(arg))
		self.questionDialog.SetCancelEvent(lambda arg=False: self.__BuyItemAnswer(arg))
		self.selectedItemPos = slotPos
		self.LockPrivateShopSlot(slotPos)
		self.questionDialog.Open()
		
	def __BuyItemAnswer(self, answer):
		if answer:
			net.SendPrivateShopBuyPacket(self.selectedItemPos)
			
		self.UnlockPrivateShopSlot(self.selectedItemPos)
		self.selectedItemPos = -1
		self.questionDialog.Close()
		
	def __OpenTitleChangeDialog(self):
		if self.inputDialog:
			return
			
		if not privateShop.IsMainPlayerPrivateShop():
			return

		if app.ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS:
			if not self.__CanBuildPrivateShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_BUILD_RESTRICTED)
				return
			
		self.inputDialog = uiCommon.InputDialog()
		self.inputDialog.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_NAME_DIALOG_TITLE)
		self.inputDialog.SetMaxLength(privateShop.TITLE_MAX_LEN)
		self.inputDialog.SetAcceptEvent(lambda arg=True: self.__ChangePrivateShopTitleAnswer(arg))
		self.inputDialog.SetCancelEvent(lambda arg=False: self.__ChangePrivateShopTitleAnswer(arg))
		self.inputDialog.Open()
		
	def __ChangePrivateShopTitleAnswer(self, answer):
		if answer:
			title = self.inputDialog.GetText()

			if not len(title):
				return
				
			if len(title) < privateShop.TITLE_MIN_LEN:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_SHORT_TITLE)
				return 

			if self.mode == self.MODE_DEFAULT:
				net.SendTitleChangePrivateShopPacket(title)

			elif self.mode == self.MODE_BUILD or self.mode == self.MODE_DECO:
				self.buildTitle = title
				self.render_title_text.SetText(title)

				# Title has been set, no need to alert the user anymore
				self.titleButtonFlash.Hide()

				self.Refresh()
			
		self.inputDialog.Close()
		self.inputDialog = None

	def OpenUnlockSlotDialog(self, keyPos):
		if self.questionDialog.IsShow():
			return

		if not privateShop.IsMainPlayerPrivateShop():
			return

		self.selectedItemPos = keyPos
		self.UnlockPrivateShopSlot(self.selectedItemPos)

		self.questionDialog.SetText(localeInfo.PREMIUM_PRIVATE_SHOP_LOCKED_SLOT_QUESTION)
		self.questionDialog.SetAcceptEvent(lambda arg = True : self.__UnlockSlotAnswer(arg))
		self.questionDialog.SetCancelEvent(lambda arg = False : self.__UnlockSlotAnswer(arg))
		self.questionDialog.Open()

	def __UnlockSlotAnswer(self, answer):
		if answer:
			net.SendPrivateShopSlotUnlockRequest(self.selectedItemPos)

		self.UnlockPrivateShopSlot(self.selectedItemPos)
		self.selectedItemPos = -1
		self.questionDialog.Close()
		
	# ToolTips	
	def __IconToolTip(self, event, icon):
		self.toolTip.ClearToolTip()
		
		if event == "mouse_over_in":
			if not privateShop.IsMainPlayerPrivateShop():
				return
				
			text = ""
					
			if icon == "TIME":
				text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_REMAIN_TEXT_ICON
				
			elif icon == "GOLD":
				text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_YANG_ICON
				
			elif icon == "CHEQUE":
				text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_WON_ICON
				
			self.toolTip.SetThinBoardSize(len(text)*4 + 50, 10)
			self.toolTip.AppendTextLine(text, self.toolTip.SPECIAL_TITLE_COLOR)
			self.toolTip.ShowToolTip()
			
		elif event == "mouse_over_out":
			if 0 != self.toolTip:
				self.toolTip.HideToolTip()
			
	def __OnOverInButton(self, button):
		if not privateShop.IsMainPlayerPrivateShop():
			return

		self.toolTip.ClearToolTip()
		text = ""
		
		if button == "MODIFY":
			text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_MODIFY_BUTTON
			
		elif button == "WITHDRAW":
			text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_TAX_ADJUSTMENT_BUTTON
			
		elif button == "REOPEN":
			text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_REOPEN_BUTTON
			
		elif button == "CLOSE":
			text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_SHOP_CLOSE_BUTTON
			
		elif button == "CHANGE_TITLE":
			text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_SHOP_RENAME_BUTTON
			
		elif button == "SALE_ITEM":
			text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_SHOP_SALE_BUTTON
			
		elif button == "DEFAULT_WINDOW":
			text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_TITLE
			
		elif button == "DECORATE":
			text = uiScriptLocale.PREMIUM_PRIVATE_SHOP_DECO_BUTTON

		elif button == "LOCATION":
			if self.mode != self.MODE_BUILD:
				(xPos, yPos, channel) = privateShop.GetLocation()
				(mapName, xBase, yBase) = background.GlobalPositionToMapInfo(xPos, yPos)
				
				localeMapName = localeInfo.MINIMAP_ZONE_NAME_DICT.get(mapName, "")
				if localeMapName != "":						
					xPos = int(xPos-xBase) / 100
					yPos = int(yPos-yBase) / 100
					
					text = localeInfo.PREMIUM_PRIVATE_SHOP_POSITION % (localeMapName, xPos, yPos, channel)

					self.toolTip.SetThinBoardSize(len(localeInfo.PREMIUM_PRIVATE_SHOP_HYPERLINK_HELP_TOOLTIP) * 3 + 50, 10)
					self.toolTip.AppendTextLine(text, self.toolTip.TITLE_COLOR)
					self.toolTip.AppendSpace(3)
					
					self.toolTip.AppendTextLine(localeInfo.PREMIUM_PRIVATE_SHOP_WARP_HELP_TOOLTIP)
					# self.toolTip.AppendTextLine(localeInfo.PREMIUM_PRIVATE_SHOP_HYPERLINK_HELP_TOOLTIP)

					(gold, cheque) = self.__GetPrivateShopWorth()
					self.toolTip.AppendSpace(3)
					self.toolTip.AppendTextLine(localeInfo.PREMIUM_PRIVATE_SHOP_TOTAL_WORTH, self.toolTip.TITLE_COLOR)
					self.toolTip.AppendSpace(1)
					self.toolTip.AppendTextLine(localeInfo.NumberToMoneyString(gold), grp.GenerateColor(1.0, 0.7843, 0.0, 1.0))

					if app.ENABLE_PRIVATE_SHOP_CHEQUE:
						if cheque > 0:
							self.toolTip.AppendTextLine(str(cheque) + " " + localeInfo.CHEQUE_SYSTEM_UNIT_WON, grp.GenerateColor(0.0, 0.8470, 1.0, 1.0))

					self.toolTip.ShowToolTip()
			return
			
			
		self.toolTip.SetThinBoardSize(len(text)*4 + 50, 10)
		self.toolTip.AppendTextLine(text, self.toolTip.SPECIAL_TITLE_COLOR)
		
		self.toolTip.ShowToolTip()
			
		
	def __OnOverOutButton(self):
		if 0 != self.toolTip:
			self.toolTip.HideToolTip()
		
	# Item Slot
	def __LocalPosToGlobalPos(self, i):
		return self.page * privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM + i

	def __OnOverInItem(self, localSlotIndex):
		globalSlotIndex = self.__LocalPosToGlobalPos(localSlotIndex)
		vnum = privateShop.GetItemVnum(globalSlotIndex)

		if self.mode == self.MODE_BUILD:

			if self.itemStockDict.has_key(globalSlotIndex):
				self.toolTipItem.SetPrivateShopBuilderItem(*self.itemStockDict[globalSlotIndex] + (globalSlotIndex,))
				
		elif self.mode == self.MODE_DEFAULT:
			if mouseModule.mouseController.isAttached():
				self.itemSlot.SetUsableItem(False)
						
			if 0 != self.toolTipItem:
				self.toolTipItem.ClearToolTip()
				self.toolTipItem.SetPrivateShopItem(globalSlotIndex)
				self.toolTipItem.AppendRenderingTooltip(vnum)

		elif self.mode == self.MODE_SALE_ITEM:
			if 0 != self.toolTipItem:
				self.toolTipItem.ClearToolTip()
				
				itemIndex = localSlotIndex + self.firstSlotIndex
				self.toolTipItem.SetPrivateShopSaleItem(itemIndex)
				
	def __OnOverOutItem(self):
		if self.mode == self.MODE_DEFAULT:
			self.itemSlot.SetUsableItem(False)
		
		if 0 != self.toolTipItem:
			self.toolTipItem.HideToolTip()
			
	def __OnScrollResultList(self):
		max_count = privateShop.GetSaleItemMaxCount()
		scrollLineCount = max(0, max_count - self.MAX_SALE_ITEM_COUNT)
		startIndex = int(scrollLineCount * self.scroll_bar.GetPos())

		if startIndex != self.firstSlotIndex:
			self.firstSlotIndex = startIndex
			self.Refresh()
			
	def OnRunMouseWheel(self, nLen):
		for slot in self.saleItemSlotList:
			if slot.IsIn():
				if nLen > 0:
					self.scroll_bar.OnUp()
				else:
					self.scroll_bar.OnDown()
					
				return True
			
		if self.background_group.IsIn():
			if nLen > 0:
				self.scroll_bar.OnUp()
			else:
				self.scroll_bar.OnDown()
				
			return True
			
		return False

	def LockPrivateShopSlot(self, slotIndex):
		if slotIndex == -1:
			return

		shopPage = self.page
		
		min_range = shopPage * privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM
		max_range = (shopPage + 1) * privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM
			
		if min_range <= slotIndex < max_range:
			slotIndex = slotIndex - min_range
			self.itemSlot.SetCantMouseEventSlot(slotIndex)

	def UnlockPrivateShopSlot(self, slotIndex):
		if slotIndex == -1:
			return
			
		shopPage = self.page
		
		min_range = shopPage * privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM
		max_range = (shopPage + 1) * privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM
			
		if min_range <= slotIndex < max_range:
			slotIndex = slotIndex - min_range
			self.itemSlot.SetCanMouseEventSlot(slotIndex)	
			
	def LockInventorySlot(self, slotIndex, windowType = player.INVENTORY):
		if windowType == player.INVENTORY:
			if slotIndex >= 0 and slotIndex < player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT: 
				if self.wndInventory:
					inventoryPage = slotIndex / player.INVENTORY_PAGE_SIZE
					localSlotPos = slotIndex - (inventoryPage * player.INVENTORY_PAGE_SIZE)

					if self.wndInventory.GetInventoryPageIndex() == inventoryPage:
						self.wndInventory.wndItem.SetCantMouseEventSlot(localSlotPos)
					
			# elif self.wndSpecialInventory:
			# 	special_page = self.wndSpecialInventory.GetInventoryPageIndex()
				
			# 	if self.wndSpecialInventory.GetInventoryType() == 0:
			# 		if (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE)) <= slotIndex < (item.SKILL_BOOK_INVENTORY_SLOT_START + (((special_page + 1) * player.SPECIAL_INVENTORY_PAGE_SIZE))):
			# 			lock_idx = slotIndex - (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE))
			# 			self.wndSpecialInventory.wndItem.SetCantMouseEventSlot(lock_idx)

			# 	elif self.wndSpecialInventory.GetInventoryType() == 1:
			# 		if (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE)) <= slotIndex < (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (((special_page + 1) * player.SPECIAL_INVENTORY_PAGE_SIZE))):
			# 			lock_idx = slotIndex - (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE))
			# 			self.wndSpecialInventory.wndItem.SetCantMouseEventSlot(lock_idx)

			# 	elif self.wndSpecialInventory.GetInventoryType() == 2:
			# 		if (item.STONE_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE)) <= slotIndex < (item.STONE_INVENTORY_SLOT_START + (((special_page + 1) * player.SPECIAL_INVENTORY_PAGE_SIZE))):
			# 			lock_idx = slotIndex - (item.STONE_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE))
			# 			self.wndSpecialInventory.wndItem.SetCantMouseEventSlot(lock_idx)

			# 	elif self.wndSpecialInventory.GetInventoryType() == 3:
			# 		if (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE)) <= slotIndex < (item.GIFT_BOX_INVENTORY_SLOT_START + (((special_page + 1) * player.SPECIAL_INVENTORY_PAGE_SIZE))):
			# 			lock_idx = slotIndex - (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE))
			# 			self.wndSpecialInventory.wndItem.SetCantMouseEventSlot(lock_idx)

		elif windowType == player.DRAGON_SOUL_INVENTORY:
			if self.wndDragonSoulInventory:
				DSKindIndex = int(slotIndex / (player.DRAGON_SOUL_GRADE_MAX * player.DRAGON_SOUL_PAGE_SIZE))
				DSInventoryPage = int((slotIndex - DSKindIndex * (player.DRAGON_SOUL_GRADE_MAX * player.DRAGON_SOUL_PAGE_SIZE)) / player.DRAGON_SOUL_PAGE_SIZE)
				localSlotPos = slotIndex - (DSKindIndex * player.DRAGON_SOUL_GRADE_MAX * player.DRAGON_SOUL_PAGE_SIZE) - DSInventoryPage * player.DRAGON_SOUL_PAGE_SIZE

				if self.wndDragonSoulInventory.GetInventoryPageIndex() == DSInventoryPage and\
					self.wndDragonSoulInventory.GetDSKindIndex() == DSKindIndex:
					self.wndDragonSoulInventory.wndItem.SetCantMouseEventSlot(localSlotPos)
				
	def UnlockInventorySlot(self, slotIndex, windowType = player.INVENTORY):
		if windowType == player.INVENTORY:
			if slotIndex >= 0 and slotIndex < player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT: 
				if self.wndInventory:
					inventoryPage = slotIndex / player.INVENTORY_PAGE_SIZE
					localSlotPos = slotIndex - (inventoryPage * player.INVENTORY_PAGE_SIZE)
					
					if self.wndInventory.GetInventoryPageIndex() == inventoryPage:
						self.wndInventory.wndItem.SetCanMouseEventSlot(localSlotPos)

			# elif self.wndSpecialInventory:
			# 	special_page = self.wndSpecialInventory.GetInventoryPageIndex()
				
			# 	if self.wndSpecialInventory.GetInventoryType() == 0:
			# 		if (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE)) <= slotIndex < (item.SKILL_BOOK_INVENTORY_SLOT_START + (((special_page + 1) * player.SPECIAL_INVENTORY_PAGE_SIZE))):
			# 			lock_idx = slotIndex - (item.SKILL_BOOK_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE))
			# 			self.wndSpecialInventory.wndItem.SetCanMouseEventSlot(lock_idx)

			# 	elif self.wndSpecialInventory.GetInventoryType() == 1:
			# 		if (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE)) <= slotIndex < (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (((special_page + 1) * player.SPECIAL_INVENTORY_PAGE_SIZE))):
			# 			lock_idx = slotIndex - (item.UPGRADE_ITEMS_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE))
			# 			self.wndSpecialInventory.wndItem.SetCanMouseEventSlot(lock_idx)

			# 	elif self.wndSpecialInventory.GetInventoryType() == 2:
			# 		if (item.STONE_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE)) <= slotIndex < (item.STONE_INVENTORY_SLOT_START + (((special_page + 1) * player.SPECIAL_INVENTORY_PAGE_SIZE))):
			# 			lock_idx = slotIndex - (item.STONE_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE))
			# 			self.wndSpecialInventory.wndItem.SetCanMouseEventSlot(lock_idx)

			# 	elif self.wndSpecialInventory.GetInventoryType() == 3:
			# 		if (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE)) <= slotIndex < (item.GIFT_BOX_INVENTORY_SLOT_START + (((special_page + 1) * player.SPECIAL_INVENTORY_PAGE_SIZE))):
			# 			lock_idx = slotIndex - (item.GIFT_BOX_INVENTORY_SLOT_START + (special_page * player.SPECIAL_INVENTORY_PAGE_SIZE))
			# 			self.wndSpecialInventory.wndItem.SetCanMouseEventSlot(lock_idx)

		elif windowType == player.DRAGON_SOUL_INVENTORY:
			if self.wndDragonSoulInventory:
				DSKindIndex = int(slotIndex / (player.DRAGON_SOUL_GRADE_MAX * player.DRAGON_SOUL_PAGE_SIZE))
				DSInventoryPage = int((slotIndex - DSKindIndex * (player.DRAGON_SOUL_GRADE_MAX * player.DRAGON_SOUL_PAGE_SIZE)) / player.DRAGON_SOUL_PAGE_SIZE)
				localSlotPos = slotIndex - (DSKindIndex * player.DRAGON_SOUL_GRADE_MAX * player.DRAGON_SOUL_PAGE_SIZE) - DSInventoryPage * player.DRAGON_SOUL_PAGE_SIZE

				if self.wndDragonSoulInventory.GetInventoryPageIndex() == DSInventoryPage and\
					self.wndDragonSoulInventory.GetDSKindIndex() == DSKindIndex:
					self.wndDragonSoulInventory.wndItem.SetCanMouseEventSlot(localSlotPos)
			
	def RefreshLockedSlot(self):
		if self.mode == self.MODE_DEFAULT:
			self.LockInventorySlot(self.checkinSrcPos, self.checkinSrcWindow)

			if self.selectedItemPos >= 0:
				self.LockPrivateShopSlot(self.selectedItemPos)
						
		elif self.mode == self.MODE_BUILD:
			for privatePos, (windowType, itemSlotPos) in self.itemStockDict.items():
				self.LockInventorySlot(itemSlotPos, windowType)

		if self.wndInventory:
			self.wndInventory.wndItem.RefreshSlot()

		if self.wndDragonSoulInventory:
			self.wndDragonSoulInventory.wndItem.RefreshSlot()

		# if self.wndSpecialInventory:
		# 	self.wndSpecialInventory.wndItem.RefreshSlot()
			
	def OnSelectItemSlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			return False
				
		localSlotPos = selectedSlotPos
		selectedSlotPos = self.__LocalPosToGlobalPos(selectedSlotPos)

		if privateShop.IsMainPlayerPrivateShop():
			selectedItemVnum = 0
			selectedItemCount = 0
				
			if self.mode == self.MODE_DEFAULT:
				# If CTRL button is held down, proceed with taking the item out immidiately 
				# (game will choose the first empty spot for the item)
				if app.IsPressed(app.DIK_LCONTROL):
					# function requires global slot position and not local!
					self.SendItemCheckoutPacket(selectedSlotPos)
					return
				
				# Attach item to mouse for future replace/remove
				selectedItemVnum = privateShop.GetItemVnum(selectedSlotPos)
				selectedItemCount = privateShop.GetItemCount(selectedSlotPos)
			
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_PRIVATE_SHOP, selectedSlotPos, selectedItemVnum, selectedItemCount)
			if self.mode == self.MODE_BUILD:
				if not selectedSlotPos in self.itemStockDict:
					return

				(invenType, invenPos) = self.itemStockDict[selectedSlotPos]
				privateShop.DeleteItemStock(invenType, invenPos)

				self.UnlockInventorySlot(invenPos, invenType)
				
				del self.itemStockDict[selectedSlotPos]
				
				self.Refresh()
		else:
			# Buy the item
			self.OpenBuyItemDialog(selectedSlotPos)
			
	def OnSelectEmptySlot(self, selectedSlotPos):
		if self.questionDialog.IsShow():
			return

		if not privateShop.IsMainPlayerPrivateShop():
			return

		if self.checkinDstPos >= 0:
			self.CancelItemCheckin()

		if self.selectedItemPos >= 0:
			self.CancelInputPrice()

		localSlotPos = selectedSlotPos
		if selectedSlotPos >= 0:
			selectedSlotPos = self.__LocalPosToGlobalPos(selectedSlotPos)

		if self.__IsLockedSlot(selectedSlotPos):
			if self.mode == self.MODE_DEFAULT:
				if privateShop.GetMyState() != privateShop.STATE_MODIFY:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_NOT_MODIFY_STATE)
					return

			keyPos = player.GetItemSlotIndex(privateShop.PRIVATE_SHOP_SLOT_UNLOCK_ITEM)
			if keyPos < 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_UNLOCK_KEY_MISSING)
				return

			self.OpenUnlockSlotDialog(keyPos)
			return

		if not mouseModule.mouseController.isAttached():
			return
		
		attachedSlotType = mouseModule.mouseController.GetAttachedType()
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()

		mouseModule.mouseController.DeattachObject()

		if app.ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS:
			if self.mode == self.MODE_BUILD:
				if not self.__CanBuildPrivateShop():
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_BUILD_RESTRICTED)
					return
		
		if self.mode == self.MODE_DEFAULT:
			if privateShop.GetMyState() != privateShop.STATE_MODIFY:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_NOT_MODIFY_STATE)
				return
		
		if attachedSlotType == player.SLOT_TYPE_PRIVATE_SHOP:
			if self.mode == self.MODE_DEFAULT:
				net.SendItemMovePrivateShopPacket(attachedSlotPos, selectedSlotPos)
			
		elif attachedSlotType == player.SLOT_TYPE_INVENTORY or attachedSlotType == player.SLOT_TYPE_DRAGON_SOUL_INVENTORY:
		
			attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				
			itemVnum = player.GetItemIndex(attachedInvenType, attachedSlotPos)
			item.SelectItem(itemVnum)
			
			# Cancel if item cannot be sold
			if item.IsAntiFlag(item.ANTIFLAG_GIVE) or item.IsAntiFlag(item.ANTIFLAG_MYSHOP):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATE_SHOP_CANNOT_SELL_ITEM)
				return
				
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				if player.IsEquipmentSlot(attachedSlotPos) or player.IsCostumeSlot(attachedSlotPos):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_CANNOT_SELL_EQUIPMENT)
					return
					
			elif player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType:
				if player.IsDSEquipmentSlot(attachedSlotPos):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_CANNOT_SELL_EQUIPMENT)
					return

			(x_size, y_size) = item.GetItemSize()
			if localSlotPos < 0:
				recommendedPos =  self.__GetBlankSpace(x_size, y_size)
				if recommendedPos < 0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_INVALID_ITEM_CHECKIN_POSITION)
					return

				self.checkinDstPos = recommendedPos
			else:
				if not self.__IsEmptySpace(localSlotPos ,x_size, y_size):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_INVALID_ITEM_CHECKIN_POSITION)
					return
				
				self.checkinDstPos = selectedSlotPos
				
			self.LockInventorySlot(attachedSlotPos, attachedInvenType)

			# Fetch market item price
			(marketGoldPrice, marketChequePrice) = privateShop.GetMarketItemPrice(itemVnum)

			if marketGoldPrice < 0 and marketChequePrice < 0:
				net.SendPrivateShopMarketItemPriceReq(itemVnum)

			(goldPrice, chequePrice) = self.__GetStoredPrice(attachedSlotPos, attachedInvenType)
				
			self.checkinSrcPos = attachedSlotPos
			self.checkinSrcWindow = attachedInvenType

			self.priceInputDialog.SetAcceptEvent(ui.__mem_func__(self.AcceptItemCheckin))
			self.priceInputDialog.SetCancelEvent(ui.__mem_func__(self.CancelItemCheckin))
			self.priceInputDialog.Open()
			self.priceInputDialog.SetItemVnum(itemVnum)
			self.priceInputDialog.SetItemCount(attachedItemCount)
			if chequePrice:
				self.priceInputDialog.SetCheque(chequePrice)
			if goldPrice:
				self.priceInputDialog.SetValue(goldPrice)
			self.priceInputDialog.SetMarketValue(marketGoldPrice, marketChequePrice)

			self.__RepositionPriceInputDlg(localSlotPos)

	def __UseItemSlot(self, localSlotIndex):
		globalSlotIndex = self.__LocalPosToGlobalPos(localSlotIndex)
		vnum = privateShop.GetItemVnum(globalSlotIndex)
		item.SelectItem(vnum)

		if app.IsPressed(app.DIK_LCONTROL):
			if item.HasDropInfo(vnum):
				self.Open(vnum)
			return

		if app.ENABLE_RENDER_TARGET_PREVIEW and app.IsPressed(app.DIK_LSHIFT):
			self.interface.OpenRenderTarget(vnum)
			return

	def OnUnselectItemSlot(self, selectedSlotPos):
		if app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_LSHIFT):
			self.__UseItemSlot(selectedSlotPos)
			return

		if self.questionDialog.IsShow():
			return
			
		if mouseModule.mouseController.isAttached():
			return False

		if self.checkinDstPos >= 0:
			self.CancelItemCheckin()

		if self.selectedItemPos >= 0:
			self.CancelInputPrice()
			
		localSlotPos = selectedSlotPos
		selectedSlotPos = self.__LocalPosToGlobalPos(selectedSlotPos)

		if privateShop.IsMainPlayerPrivateShop():
			# Not used when building a private shop
			if self.mode == self.MODE_BUILD:
				return False
				
			if privateShop.GetMyState() != privateShop.STATE_MODIFY:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_SHOP_NOT_MODIFY_STATE)
				return False
				
			# Open change item's price dialog
			if self.priceInputDialog.IsShow():
				return False

			itemVnum = privateShop.GetItemVnum(selectedSlotPos)
			itemCount = privateShop.GetItemCount(selectedSlotPos)
			goldPrice = privateShop.GetItemPrice(selectedSlotPos)
			chequePrice = privateShop.GetChequeItemPrice(selectedSlotPos)

			self.selectedItemPos = selectedSlotPos
			self.LockPrivateShopSlot(selectedSlotPos)

			# Fetch market item price
			(marketGoldPrice, marketChequePrice) = privateShop.GetMarketItemPrice(itemVnum)

			if marketGoldPrice < 0 and marketChequePrice < 0:
				net.SendPrivateShopMarketItemPriceReq(itemVnum)
		
			self.priceInputDialog.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrice))
			self.priceInputDialog.SetCancelEvent(ui.__mem_func__(self.CancelInputPrice))
			self.priceInputDialog.Open()
			self.priceInputDialog.SetItemVnum(itemVnum)
			self.priceInputDialog.SetItemCount(itemCount)
			if chequePrice:
				self.priceInputDialog.SetCheque(chequePrice)
			if goldPrice:
				self.priceInputDialog.SetValue(goldPrice)
			self.priceInputDialog.SetMarketValue(marketGoldPrice, marketChequePrice)
			
			self.__RepositionPriceInputDlg(localSlotPos)
			
		else:
			# Buy the item
			self.OpenBuyItemDialog(selectedSlotPos)
			
	def AttachItemToPrivateShop(self, slotIndex, slotType):
		if not privateShop.IsMainPlayerPrivateShop():
			return False
			
		if mouseModule.mouseController.isAttached():
			self.OnSelectEmptySlot(-1)
			return True
			
		else:
			selectedItemVnum = player.GetItemIndex(slotIndex)
			itemCount = player.GetItemCount(slotIndex)	
			mouseModule.mouseController.AttachObject(self, slotType, slotIndex, selectedItemVnum, itemCount)
			self.OnSelectEmptySlot(-1)
			return True
			
	def SendItemCheckoutPacket(self, globalSrcPos, dstPos = -1):
		if not self.IsShow():
			return False
			
		if not privateShop.IsMainPlayerPrivateShop():
			return False

		net.SendItemCheckoutPrivateShopPacket(globalSrcPos, dstPos)
		
		return True
		
	def AcceptInputPrice(self):
		if not self.priceInputDialog:
			return True
			
		goldPriceText = self.priceInputDialog.GetText()
		k_pos = goldPriceText.find('K')
		if k_pos >= 0:
			goldPriceText = goldPriceText[:k_pos] + '000' * goldPriceText.count('K')

		goldPrice = int(goldPriceText)

		chequePrice = 0
		if app.ENABLE_PRIVATE_SHOP_CHEQUE:
			chequePrice = int(self.priceInputDialog.GetCheque())

		if not goldPrice and not chequePrice:
			return True
		
		oldGoldPrice = privateShop.GetItemPrice(self.selectedItemPos)
		oldChequePrice = privateShop.GetChequeItemPrice(self.selectedItemPos)

		itemVnum = self.priceInputDialog.GetItemVnum()
		itemCount = self.priceInputDialog.GetItemCount()

		if app.ENABLE_PRIVATE_SHOP_DIFFERENT_PRICE_RESTRICTION:
			if not self.__IsItemPriceRestricted(itemVnum):
				(storedGoldPrice, storedChequePrice) = self.__GetStoredPrice(self.selectedItemPos, player.RESERVED_WINDOW, self.selectedItemPos)

				if storedGoldPrice > 0 and goldPrice != storedGoldPrice or \
					storedChequePrice > 0 and chequePrice != storedChequePrice:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_ITEM_ADD_DIFFERENCE_PRICE_WARNING)
					return

		if (player.GetElk() + privateShop.GetTotalGold() - oldGoldPrice + goldPrice) > constInfo.GOLD_MAX:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_MODIFY_LIMIT_YANG)

			# Reset the board
			self.CancelInputPrice()
			return
			
		if app.ENABLE_PRIVATE_SHOP_CHEQUE:
			if (player.GetCheque() + privateShop.GetTotalCheque() - oldChequePrice + chequePrice) > constInfo.CHEQUE_MAX:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_MODIFY_LIMIT_WON)

				# Reset the board
				self.CancelInputPrice()
				return

		if self.mode == self.MODE_DEFAULT:
			net.SendItemPriceChangePrivateShopPacket(self.selectedItemPos, goldPrice, chequePrice)

		# Reset the board
		self.CancelInputPrice()
			
	def CancelInputPrice(self):
		if self.priceInputDialog:
			self.priceInputDialog.KillFocus()
			self.priceInputDialog.Clear()
			self.priceInputDialog.Hide()

		self.UnlockPrivateShopSlot(self.selectedItemPos)
		self.selectedItemPos = -1
		
	def AppendMarketItemPrice(self, marketGoldPrice, marketChequePrice):
		if self.priceInputDialog and self.priceInputDialog.IsShow():
			self.priceInputDialog.SetMarketValue(marketGoldPrice, marketChequePrice)
		
	def AcceptItemCheckin(self):
		if not self.priceInputDialog:
			return True

		goldPriceText = self.priceInputDialog.GetText()
		k_pos = goldPriceText.find('K')
		if k_pos >= 0:
			goldPriceText = goldPriceText[:k_pos] + '000' * goldPriceText.count('K')

		goldPrice = int(goldPriceText)
		
		chequePrice = 0
		if app.ENABLE_PRIVATE_SHOP_CHEQUE:
			chequePrice = int(self.priceInputDialog.GetCheque())

		if not goldPrice and not chequePrice:
			return True
		
		itemVnum = self.priceInputDialog.GetItemVnum()
		itemCount = self.priceInputDialog.GetItemCount()

		if app.ENABLE_PRIVATE_SHOP_DIFFERENT_PRICE_RESTRICTION:
			if not self.__IsItemPriceRestricted(itemVnum):
				(savedGoldPrice, savedChequePrice) = self.__GetStoredPrice(self.checkinSrcPos, self.checkinSrcWindow)

				if goldPrice > 0 and savedGoldPrice > 0 and goldPrice != savedGoldPrice or \
					chequePrice > 0 and savedChequePrice > 0 and chequePrice != savedChequePrice:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_ITEM_ADD_DIFFERENCE_PRICE_WARNING)
					return

		if self.mode == self.MODE_DEFAULT:
			if (player.GetElk() + privateShop.GetTotalGold() + goldPrice) > constInfo.GOLD_MAX:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_ITEM_ADD_LIMIT_YANG)

				# Reset the board
				self.CancelItemCheckin()
				return
				
			if app.ENABLE_PRIVATE_SHOP_CHEQUE:
				if (player.GetCheque() + privateShop.GetTotalCheque() + chequePrice) > constInfo.CHEQUE_MAX:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_ITEM_ADD_LIMIT_WON)

					# Reset the board
					self.CancelItemCheckin()
					return
				
			net.SendItemCheckinPrivateShopPacket(self.checkinSrcPos, self.checkinSrcWindow, goldPrice, chequePrice, self.checkinDstPos)
			
		elif self.mode == self.MODE_BUILD:
			if (player.GetElk() + privateShop.GetTotalStockGold() + goldPrice) > constInfo.GOLD_MAX:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_ITEM_ADD_LIMIT_YANG)

				# Reset the board
				self.CancelItemCheckin()
				return
				
			if app.ENABLE_PRIVATE_SHOP_CHEQUE:
				if (player.GetCheque() + privateShop.GetTotalStockCheque() + chequePrice) > constInfo.CHEQUE_MAX:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PREMIUM_PRIVATE_ITEM_ADD_LIMIT_WON)

					# Reset the board
					self.CancelItemCheckin()
					return
				
			privateShop.AddItemStock(self.checkinSrcWindow, self.checkinSrcPos, self.checkinDstPos, goldPrice, chequePrice)
			self.itemStockDict[self.checkinDstPos] = (self.checkinSrcWindow, self.checkinSrcPos)
			snd.PlaySound("sound/ui/drop.wav")
			self.Refresh()

		# Reset the board
		self.CancelItemCheckin()
		
	def CancelItemCheckin(self):
		if self.priceInputDialog:
			self.priceInputDialog.KillFocus()
			self.priceInputDialog.Clear()
			self.priceInputDialog.Hide()

		if self.mode == self.MODE_DEFAULT:
			self.UnlockInventorySlot(self.checkinSrcPos, self.checkinSrcWindow)

		# Failed to add an item, unlock it
		elif self.mode == self.MODE_BUILD:
			if self.checkinDstPos not in self.itemStockDict.keys():
				self.UnlockInventorySlot(self.checkinSrcPos, self.checkinSrcWindow)
		
		self.checkinSrcPos		= -1
		self.checkinSrcWindow	= -1
		self.checkinDstPos		= -1
		
	def OnMoveWindow(self, x, y):
		if self.decoDialog:
			self.decoDialog.AdjustPosition()
		
	def SetShopAppearance(self, polyType):
		(name, vnum) = privateShop.GetAppearanceDeco(polyType)
		
		self.shopAppearance = vnum

		if app.ENABLE_RENDER_TARGET:
			renderTarget.SelectModel(RENDER_TARGET_INDEX, vnum)

	def SetShopTitleType(self, titleType):
		self.titleAppearance = titleType
		
		(name, path, text_color) = privateShop.GetTitleDeco(titleType)
		text_color = int(text_color, 16)
		
		RED		= (text_color & 0xFF0000) >> 16
		GREEN	= (text_color & 0x00FF00) >> 8
		BLUE	= text_color & 0x0000FF
		
		self.render_title_text.SetText(self.buildTitle)
		
		if titleType:
			self.render_title_text.SetFontColor(float(RED)/255.0, float(GREEN)/255.0, float(BLUE)/255.0)
		else:
			self.render_title_text.SetFontColor(0.8549, 0.8549, 0.8549)
		
		self.render_title.SetStyle(titleType)
		
	def __CanBuildPrivateShop(self):
		maps = [
		'metin2_map_blaues_reich_map_1',
		'metin2_map_blaues_reich_map_2',
		'metin2_map_gelbes_reich_map_1',
		'metin2_map_gelbes_reich_map_2',
		'metin2_map_rotes_reich_map_1',
		'metin2_map_rotes_reich_map_2'
		]
		current_map = background.GetCurrentMapName()

		if current_map in maps:
			return True

		return False

	def __IsEmptySpace(self, pos, x_size, y_size):
		if self.grids[self.page].is_empty(pos, x_size, y_size):
			return True

		return False

	def __GetBlankSpace(self, x_size, y_size):
		print(x_size, y_size)
		for i in range(privateShop.PRIVATE_SHOP_PAGE_MAX_NUM):
			pos = self.grids[i].find_blank(x_size, y_size)

			if pos >= 0:
				print("Found blank space on pos {} page {} -> {}".format(pos, i, self.__IsEmptySpace(pos, x_size, y_size)))
				return pos + i * privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM

		return -1
	
	def __GetStoredPrice(self, slotIndex, window, excludeSlotIndex = -1):
		if window == player.INVENTORY or window == player.DRAGON_SOUL_INVENTORY:
				vnum = player.GetItemIndex(window, slotIndex)
				count = player.GetItemCount(window, slotIndex)
				metinSlot = [player.GetItemMetinSocket(window, slotIndex, i) for i in range(player.METIN_SOCKET_MAX_NUM)]
		else:
				vnum = privateShop.GetItemVnum(slotIndex)
				count = privateShop.GetItemCount(slotIndex)
				metinSlot = [privateShop.GetItemMetinSocket(slotIndex, i) for i in range(player.METIN_SOCKET_MAX_NUM)]

		if self.mode == self.MODE_BUILD:
			for i in range(privateShop.PRIVATE_SHOP_HOST_ITEM_MAX_NUM):
				if excludeSlotIndex >= 0 and i == excludeSlotIndex:
					continue

				if not self.itemStockDict.has_key(i):
					continue
			
				type, pos = self.itemStockDict[i]
				
				itemVnum = player.GetItemIndex(type, pos)
				itemCount = player.GetItemCount(type, pos)

				if itemVnum <= 0:
					continue

				item.SelectItem(itemVnum)

				# Compare vnum for skill books, polymorphs
				if itemVnum == 50300 or item.GetItemType() == item.ITEM_TYPE_POLYMORPH:
					if metinSlot[0] != player.GetItemMetinSocket(type, pos, 0):
						continue

				# Compare value-time for blend items
				elif item.GetItemType() == item.ITEM_TYPE_BLEND:
					if metinSlot[1] != player.GetItemMetinSocket(type, pos, 1) or\
						metinSlot[2] != player.GetItemMetinSocket(type, pos, 2):
						continue

				if itemVnum == vnum and itemCount == count:
					goldPrice = privateShop.GetStockItemPrice(type, pos)
					chequePrice = privateShop.GetStockChequeItemPrice(type, pos)

					return (goldPrice, chequePrice)

		else:
			for i in range(privateShop.PRIVATE_SHOP_HOST_ITEM_MAX_NUM):
				if excludeSlotIndex >= 0 and i == excludeSlotIndex:
					continue

				itemVnum = privateShop.GetItemVnum(i)
				itemCount = privateShop.GetItemCount(i)

				if itemVnum <= 0:
					continue

				item.SelectItem(itemVnum)

				# Compare vnum for skill books, polymorphs
				if itemVnum == 50300 or item.GetItemType() == item.ITEM_TYPE_POLYMORPH:
					if metinSlot[0] != privateShop.GetItemMetinSocket(i, 0):
						continue

				# Compare value-time for blend items
				elif item.GetItemType() == item.ITEM_TYPE_BLEND:
					if metinSlot[1] != privateShop.GetItemMetinSocket(i, 1) or\
						metinSlot[2] != privateShop.GetItemMetinSocket(i, 2):
						continue

				if itemVnum == vnum and itemCount == count:
					goldPrice = privateShop.GetItemPrice(i)
					chequePrice = privateShop.GetChequeItemPrice(i)

					return (goldPrice, chequePrice)
				
		return (0, 0)
	
	def __IsItemPriceRestricted(self, itemVnum):
		item.SelectItem(itemVnum)

		if itemVnum in PRICE_RESTRICTION_IGNORE_DICT["VNUM"]:
			return True

		if item.GetItemType() in PRICE_RESTRICTION_IGNORE_DICT["TYPE"].keys():
			if len(PRICE_RESTRICTION_IGNORE_DICT["TYPE"][item.GetItemType()]) <= 0:
				return True
		
			else:
				if item.GetItemSubType() in PRICE_RESTRICTION_IGNORE_DICT["TYPE"][item.GetItemType()]:
					return True
				
		return False

	def OnStateUpdate(self):
		if privateShop.IsMainPlayerPrivateShop():
			if privateShop.GetMyState() == privateShop.STATE_MODIFY:
				self.modifyButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "REOPEN")

				# Update tooltip in real time
				if self.toolTip.IsShow():
					self.__OnOverInButton("REOPEN")
				
				self.modifyButton.SetUpVisual(ROOT_PATH + "reopen_button_default.sub")
				self.modifyButton.SetOverVisual(ROOT_PATH + "reopen_button_over.sub")
				self.modifyButton.SetDownVisual(ROOT_PATH + "reopen_button_down.sub")

				self.interface.SetOnTopWindow(player.ON_TOP_WND_PRIVATE_SHOP)
				self.interface.RefreshMarkInventoryBag()

			elif privateShop.GetMyState() == privateShop.STATE_OPEN:
				self.modifyButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "MODIFY")

				# Update tooltip in real time
				if self.toolTip.IsShow():
					self.__OnOverInButton("MODIFY")
				
				self.modifyButton.SetUpVisual(ROOT_PATH + "modify_button_default.sub")
				self.modifyButton.SetOverVisual(ROOT_PATH + "modify_button_over.sub")
				self.modifyButton.SetDownVisual(ROOT_PATH + "modify_button_down.sub")

				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.interface.RefreshMarkInventoryBag()

		else:
			# Close buy dialog as price of the item might have been changed
			if self.questionDialog.IsShow():
				self.__BuyItemAnswer(False)
				
				
	def __GetPrivateShopWorth(self):
		gold = 0
		cheque = 0

		if self.mode == self.MODE_BUILD:
			for i in range(privateShop.PRIVATE_SHOP_HOST_ITEM_MAX_NUM):
				if not self.itemStockDict.has_key(i):
					continue
			
				type, pos = self.itemStockDict[i]
				
				itemVnum = player.GetItemIndex(type, pos)

				if itemVnum <= 0:
					continue

				gold += privateShop.GetStockItemPrice(type, pos)
				cheque += privateShop.GetStockChequeItemPrice(type, pos)

		else:
			for i in range(privateShop.PRIVATE_SHOP_HOST_ITEM_MAX_NUM):
				itemVnum = privateShop.GetItemVnum(i)

				if itemVnum <= 0:
					continue

				gold += privateShop.GetItemPrice(i)
				cheque += privateShop.GetChequeItemPrice(i)
				
		return (gold, cheque)
		
	def __GetPrivateShopMaxPage(self):
		page = 1

		for i in range(privateShop.PRIVATE_SHOP_HOST_ITEM_MAX_NUM):
			itemVnum = privateShop.GetItemVnum(i)

			if itemVnum <= 0:
				continue

			page = max(int(i / privateShop.PRIVATE_SHOP_PAGE_ITEM_MAX_NUM) + 1, page)

		return page
