import ui
import player
import mouseModule
import net
import app
import snd
import item
import player
import chat
import grp
import uiScriptLocale
import uiRefine
import uiPickMoney
import uiCommon
import uiPrivateShopBuilder
import localeInfo
import constInfo
import ime
import wndMgr
if app.ENABLE_ACCE_COSTUME_SYSTEM:
	import acce
if app.ENABLE_CHECKINOUT_UPDATE:
	import exchange
if app.ENABLE_PREMIUM_PRIVATE_SHOP:
	import uiPrivateShop
	import privateShop
if app.ENABLE_ITEM_MANAGER:
	import uiItemManager
if app.ENABLE_SAVE_LAST_WINDOW_POSITION:
	import player
	import wndMgr
if app.BL_67_ATTR:
	import uiAttr67Add
if app.ENABLE_HIDE_COSTUME_SYSTEM:
	import time
	import chr

ITEM_MALL_BUTTON_ENABLE = True

ITEM_FLAG_APPLICABLE = 1 << 14


if app.ENABLE_HIDE_COSTUME_SYSTEM:
	class MouseReflector(ui.Window):
		def __init__(self, parent):
			ui.Window.__init__(self)
			self.SetParent(parent)
			self.AddFlag("not_pick")
			self.width = self.height = 0
			self.isDown = False
	
		def Down(self):
			self.isDown = True
	
		def Up(self):
			self.isDown = False
	
		def OnRender(self):
	
			if self.isDown:
				grp.SetColor(ui.WHITE_COLOR)
			else:
				grp.SetColor(ui.HALF_WHITE_COLOR)
	
			x, y = self.GetGlobalPosition()
			grp.RenderBar(x+2, y+2, self.GetWidth()-4, self.GetHeight()-4)

	class CheckBox(ui.ImageBox):
		def __init__(self, parent, x, y, event, status = True, filename = "d:/ymir work/ui/public/Parameter_Slot_01.sub"):
			ui.ImageBox.__init__(self)
			self.SetParent(parent)
			self.SetPosition(x, y)
			self.LoadImage(filename)
	
			self.mouseReflector = MouseReflector(self)
			self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())
	
			image = ui.MakeImageBox(self, "d:/ymir work/ui/game/costume/eye_normal_02.tga", 0, 0)
			image.AddFlag("not_pick")
			image.SetWindowHorizontalAlignCenter()
			image.SetWindowVerticalAlignCenter()
			image.Hide()
			self.Enable = True
			self.image = image
			self.event = event
			self.Show()
	
			self.mouseReflector.UpdateRect()
			
			if status == True:
				self.SetCheck(True)
			else:
				self.SetCheck(False)
		def __del__(self):
			ui.ImageBox.__del__(self)
	
		def wait_at_least(min_wait_time):
			def _inner(fn):
				last_call = [0]
				def _inner2(*args, **kwargs):
					if constInfo.CannotAct():
						chat.AppendChat(chat.CHAT_TYPE_INFO, "You need to wait.")
						return
					elapsed = time.time() - last_call[0]
					if elapsed < min_wait_time:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.COSTUME_HIDE_COOLDOWN_MESSAGE.format(min_wait_time - elapsed, fn.__name__))
						return
					last_call[0] = time.time()
					return fn(*args, **kwargs)
				return _inner2
			return _inner
	
		def IsChecked(self):
			if self.image.IsShow():
				return True
			else:
				return False
	
		def SetCheck(self, flag):
			if flag == True:
				self.image.Show()
			else:
				self.image.Hide()
	
		def Disable(self):
			self.Enable = False
	
		def OnMouseOverIn(self):
			if not self.Enable:
				return
			self.mouseReflector.Show()
	
		def OnMouseOverOut(self):
			if not self.Enable:
				return
			self.mouseReflector.Hide()
	
		# @wait_at_least(1)
		def SendCostumeUpdate(self):
			if constInfo.CannotAct():
				chat.AppendChat(chat.CHAT_TYPE_INFO, "You need to wait.")
				return
			if self.IsChecked():
				self.SetCheck(False)
				if self.event == "hair":
					constInfo.costumeHair = True
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_HAIR, False)
				elif self.event == "costume":
					constInfo.costumeCostume = True
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_BODY, False)
				elif app.ENABLE_WEAPON_COSTUME_SYSTEM and self.event == "weapon":
					constInfo.costumeWeapon = True
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_WEAPON, False)
				elif app.ENABLE_ACCE_COSTUME_SYSTEM and self.event == "acce":
					constInfo.costumeAcce = True
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_ACCE, False)
				elif app.ENABLE_AURA_COSTUME_SYSTEM and self.event == "aura":
					constInfo.costumeAura = True
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_AURA, False)
			elif not self.IsChecked():
				self.SetCheck(True)
				if self.event == "hair":
					constInfo.costumeHair = False
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_HAIR, True)
				elif self.event == "costume":
					constInfo.costumeCostume = False
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_BODY, True)
				elif app.ENABLE_WEAPON_COSTUME_SYSTEM and self.event == "weapon":
					constInfo.costumeWeapon = False
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_WEAPON, True)
				elif app.ENABLE_ACCE_COSTUME_SYSTEM and self.event == "acce":
					constInfo.costumeAcce = False
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_ACCE, True)
				elif app.ENABLE_AURA_COSTUME_SYSTEM and self.event == "aura":
					constInfo.costumeAura = False
					net.SendChangeCostumeVisibility(chr.HIDE_COSTUME_AURA, True)

		def OnMouseLeftButtonDown(self):
			if not self.Enable:
				return
			self.mouseReflector.Down()
	
		def OnMouseLeftButtonUp(self):
			if not self.Enable:
				return
			self.mouseReflector.Up()
			self.SendCostumeUpdate()

class CostumeWindow(ui.ScriptWindow):
	if constInfo.ENABLE_COSTUME_PAGES:
		COSTUME_PAGE_MAX = 1
		if app.ENABLE_SHINING_ITEM_SYSTEM:
			COSTUME_PAGE_MAX += 1
		if app.ENABLE_BOOSTER_ITEMS:
			COSTUME_PAGE_MAX += 1

	def __init__(self, wndInventory):
		import exception

		if not app.ENABLE_COSTUME_SYSTEM:
			exception.Abort("What do you do?")
			return

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;
		self.wndCostumeInventoryLayer = None

		if constInfo.ENABLE_COSTUME_PAGES:
			self.titleBar = 0
			self.costumeTitleBarDict = None

			COSTUME_PAGE_MAX = 1
			if app.ENABLE_SHINING_ITEM_SYSTEM:
				COSTUME_PAGE_MAX += 1
			if app.ENABLE_BOOSTER_ITEMS:
				COSTUME_PAGE_MAX += 1

		self.__LoadWindow()
		if constInfo.ENABLE_COSTUME_PAGES:
			self.SetCostumeInvPage(0)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.wndInventory = None

	def Show(self):
		self.__LoadWindow()
		self.RefreshCostumeSlot()

		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	if app.ENABLE_COSTUMEWINDOW_STICKED_TO_INVENTORYWINDOW:
		def GetBasePosition(self):
				x, y = self.wndInventory.GetGlobalPosition()
				return x - 139, y
	
		def AdjustPositionAndSize(self):
			bx, by = self.GetBasePosition()
	
			self.SetPosition(bx, by)
			self.SetSize(self.GetWidth(), self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			if constInfo.ENABLE_COSTUME_PAGES:
				pyScrLoader.LoadScriptFile(self, "UIScript/CostumeWindowEx.py")
			else:
				pyScrLoader.LoadScriptFile(self, "UIScript/CostumeWindow.py")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		try:
			if constInfo.ENABLE_COSTUME_PAGES:
				GetObject = self.GetChild
			self.ORIGINAL_WIDTH = self.GetWidth()
			if constInfo.ENABLE_COSTUME_PAGES:
				self.titleBar = GetObject("TitleBar")
				self.costumeTitleBarDict = []
				for i in xrange(self.COSTUME_PAGE_MAX):
					self.costumeTitleBarDict.append(self.GetChild("Costume_TitleBar_%02d" % (i+1)))

			wndEquip = self.GetChild("CostumeSlot")
			if constInfo.ENABLE_COSTUME_PAGES:
				costumePage = self.GetChild("Costume_Page")
				if app.ENABLE_SHINING_ITEM_SYSTEM:
					wndShinings= self.GetChild("ShiningSlot")
					shiningPage = self.GetChild("Shining_Page")
				if app.ENABLE_BOOSTER_ITEMS:
					wndBoosters= self.GetChild("BoosterSlot")
					boosterPage = self.GetChild("Booster_Page")

				self.costumetInvTab = []
				for i in xrange(self.COSTUME_PAGE_MAX):
					self.costumetInvTab.append(self.GetChild("Costume_Tab_%02d" % (i+1)))

			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			def __CreateText(parent, name, x, y):
				text = ui.Button()
				text.SetParent(parent)
				text.SetPosition(x, y)
				text.SetText(name)
				text.UpdateRect()
				text.Show()
				return text
		
			## Change coords at def UpdateCostumeBoxes
			self.hairBox = CheckBox(self, 61, 45, "hair", 1 if constInfo.costumeHair == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")
			self.costumeBox = CheckBox(self, 61, 8, "costume", 1 if constInfo.costumeCostume == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")
			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.weaponBox = CheckBox(self, 13, 15, "weapon", 1 if constInfo.costumeWeapon == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				self.acceBox = CheckBox(self, 22, 153, "acce", 1 if constInfo.costumeAcce == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")
			if app.ENABLE_AURA_COSTUME_SYSTEM:
				self.auraBox = CheckBox(self, 22, 153, "aura", 1 if constInfo.costumeAura == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")

		## Equipment
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

		if constInfo.ENABLE_COSTUME_PAGES:
			if app.ENABLE_SHINING_ITEM_SYSTEM:
				wndShinings.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
				wndShinings.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
				wndShinings.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
				wndShinings.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))
				wndShinings.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
				wndShinings.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))

			if app.ENABLE_BOOSTER_ITEMS:
				wndBoosters.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
				wndBoosters.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
				wndBoosters.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
				wndBoosters.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))
				wndBoosters.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
				wndBoosters.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))

			for i in xrange(self.COSTUME_PAGE_MAX):
				self.costumetInvTab[i].SetEvent(lambda arg=i: self.SetCostumeInvPage(arg))
			self.costumetInvTab[0].Down()

		self.wndEquip = wndEquip
		if constInfo.ENABLE_COSTUME_PAGES:
			self.costumePage = costumePage
			if app.ENABLE_SHINING_ITEM_SYSTEM:
				self.wndShinings = wndShinings
				self.shiningPage = shiningPage
			if app.ENABLE_BOOSTER_ITEMS:
				self.wndBoosters = wndBoosters
				self.boosterPage = boosterPage

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.UpdateCostumeBoxes()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def UpdateCostumeBoxes(self):
			self.hairBox = None
			self.hairBox = CheckBox(self, 115, 50, "hair", 1 if constInfo.costumeHair == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")

			self.costumeBox = None
			self.costumeBox = CheckBox(self, 115, 55+32*1, "costume", 1 if constInfo.costumeCostume == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")

			if app.ENABLE_WEAPON_COSTUME_SYSTEM:
				self.weaponBox = None
				self.weaponBox = CheckBox(self, 65, 50, "weapon", 1 if constInfo.costumeWeapon == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				self.acceBox = None
				self.acceBox = CheckBox(self, 115, 64+32*4, "acce", 1 if constInfo.costumeAcce == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")

			if app.ENABLE_AURA_COSTUME_SYSTEM:
				self.auraBox = None
				self.auraBox = CheckBox(self, 104, 58, "aura", 1 if constInfo.costumeAura == 0 else 0, "d:/ymir work/ui/game/costume/eye_closed_02.tga")

	if constInfo.ENABLE_COSTUME_PAGES:
		def SetCostumeInvPage(self, page):
			self.pagesPageIndex = page
			for i in xrange(self.COSTUME_PAGE_MAX):
				if i != page:
					self.costumetInvTab[i].SetUp()
					self.costumeTitleBarDict[i].Hide()
			self.costumeTitleBarDict[page].Show()

			if app.ENABLE_SHINING_ITEM_SYSTEM and self.pagesPageIndex == 1:
				self.costumePage.Hide()
				self.shiningPage.Show()
				if app.ENABLE_BOOSTER_ITEMS:
					self.boosterPage.Hide()
			elif app.ENABLE_BOOSTER_ITEMS and self.pagesPageIndex == 2:
				self.costumePage.Hide()
				self.shiningPage.Hide()
				self.boosterPage.Show()
			else:
				if app.ENABLE_SHINING_ITEM_SYSTEM:
					self.shiningPage.Hide()
				if app.ENABLE_BOOSTER_ITEMS:
					self.boosterPage.Hide()
				self.costumePage.Show()

			if app.ENABLE_HIDE_COSTUME_SYSTEM and self.pagesPageIndex != 0:
				self.hairBox.Hide()
				self.costumeBox.Hide()
				if app.ENABLE_WEAPON_COSTUME_SYSTEM:
					self.weaponBox.Hide()
				if app.ENABLE_ACCE_COSTUME_SYSTEM:
					self.acceBox.Hide()
			else:
				self.hairBox.Show()
				self.costumeBox.Show()
				if app.ENABLE_WEAPON_COSTUME_SYSTEM:
					self.weaponBox.Show()
				if app.ENABLE_ACCE_COSTUME_SYSTEM:
					self.acceBox.Show()

	def RefreshCostumeSlot(self):
		getItemVNum=player.GetItemIndex

		for i in xrange(item.COSTUME_SLOT_COUNT):
			slotNumber = item.COSTUME_SLOT_START + i
			self.wndEquip.SetItemSlot(slotNumber, getItemVNum(slotNumber), 0)

		if app.ENABLE_WEAPON_COSTUME_SYSTEM:
			self.wndEquip.SetItemSlot(item.COSTUME_SLOT_WEAPON, getItemVNum(item.COSTUME_SLOT_WEAPON), 0)
		if app.ENABLE_MOUNT_SYSTEM:
			self.wndEquip.SetItemSlot(item.COSTUME_SLOT_MOUNT, getItemVNum(item.COSTUME_SLOT_MOUNT), 0)

		self.wndEquip.RefreshSlot()

		if app.ENABLE_SHINING_ITEM_SYSTEM and constInfo.ENABLE_COSTUME_PAGES:
			for i in xrange(item.SHINING_SLOT_COUNT):
				slotNumber = item.SHINING_SLOT_START + i
				self.wndShinings.SetItemSlot(slotNumber, getItemVNum(slotNumber), 0)

			self.wndShinings.RefreshSlot()
		
		if app.ENABLE_BOOSTER_ITEMS and constInfo.ENABLE_COSTUME_PAGES:
			for i in xrange(item.BOOSTER_SLOT_COUNT):
				slotNumber = item.BOOSTER_SLOT_START + i
				self.wndBoosters.SetItemSlot(slotNumber, getItemVNum(slotNumber), 0)

			self.wndBoosters.RefreshSlot()

class BeltInventoryWindow(ui.ScriptWindow):

	def __init__(self, wndInventory):
		import exception

		if not app.ENABLE_NEW_EQUIPMENT_SYSTEM:
			exception.Abort("What do you do?")
			return

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory

		self.wndBeltInventoryLayer = None
		self.wndBeltInventorySlot = None
		self.expandBtn = None
		self.minBtn = None
		##activateall
		self.UseItemBelt = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.wndInventory = None

	def Show(self, openBeltSlot = False):
		self.__LoadWindow()
		self.RefreshSlot()

		ui.ScriptWindow.Show(self)

		if openBeltSlot:
			self.OpenInventory()
		else:
			self.CloseInventory()

	def Close(self):
		self.Hide()

	def IsOpeningInventory(self):
		return False

	def OpenInventory(self):
		pass

		if localeInfo.IsARABIC() == 0:
			self.AdjustPositionAndSize()

	def CloseInventory(self):
		self.wndBeltInventoryLayer.Hide()
		self.expandBtn.Show()

		if localeInfo.IsARABIC() == 0:
			self.AdjustPositionAndSize()

	def GetBasePosition(self):
		x, y = self.wndInventory.GetGlobalPosition()
		return x - 148, y + 241

	def AdjustPositionAndSize(self):
		bx, by = self.GetBasePosition()

		if self.IsOpeningInventory():
			self.SetPosition(bx, by)
			self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())

		else:
			self.SetPosition(bx + 138, by);
			self.SetSize(10, self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/BeltInventoryWindow.py")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		try:
			self.ORIGINAL_WIDTH = self.GetWidth()
			wndBeltInventorySlot = self.GetChild("BeltInventorySlot")
			self.wndBeltInventoryLayer = self.GetChild("BeltInventoryLayer")
			self.expandBtn = self.GetChild("ExpandBtn")
			self.minBtn = self.GetChild("MinimizeBtn")
			##activateall
			self.UseItemBelt = self.GetChild("UseBeltItemsButton")

			self.expandBtn.SetEvent(ui.__mem_func__(self.OpenInventory))
			self.minBtn.SetEvent(ui.__mem_func__(self.CloseInventory))
			##activateall
			self.UseItemBelt.SetEvent(self.ActivateAll)

			if localeInfo.IsARABIC() :
				self.expandBtn.SetPosition(self.expandBtn.GetWidth() - 2, 15)
				self.wndBeltInventoryLayer.SetPosition(self.wndBeltInventoryLayer.GetWidth() - 5, 0)
				self.minBtn.SetPosition(self.minBtn.GetWidth() + 3, 15)

			for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
				slotNumber = item.BELT_INVENTORY_SLOT_START + i
				wndBeltInventorySlot.SetCoverButton(slotNumber,	"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/belt_inventory/slot_disabled.tga", False, False)

		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		## Equipment
		wndBeltInventorySlot.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndBeltInventorySlot.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndBeltInventorySlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndBeltInventorySlot.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndBeltInventorySlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndBeltInventorySlot.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

		self.wndBeltInventorySlot = wndBeltInventorySlot

	def RefreshSlot(self):
		getItemVNum=player.GetItemIndex

		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i
			self.wndBeltInventorySlot.SetItemSlot(slotNumber, getItemVNum(slotNumber), player.GetItemCount(slotNumber))
			self.wndBeltInventorySlot.SetAlwaysRenderCoverButton(slotNumber, True)

			avail = "0"

			if player.IsAvailableBeltInventoryCell(slotNumber):
				self.wndBeltInventorySlot.EnableCoverButton(slotNumber)
			else:
				self.wndBeltInventorySlot.DisableCoverButton(slotNumber)

		self.wndBeltInventorySlot.RefreshSlot()


	##activateall
	def ActivateAll(self):
		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i
			net.SendItemUsePacket(slotNumber)

if app.ENABLE_SHINING_ITEM_SYSTEM and not constInfo.ENABLE_COSTUME_PAGES:
	class ShiningsWindow(ui.ScriptWindow):

		def __init__(self, wndInventory):
			import exception

			if not wndInventory:
				exception.Abort("wndInventory parameter must be set to InventoryWindow")
				return

			ui.ScriptWindow.__init__(self)

			self.isLoaded = 0
			self.wndInventory = wndInventory
			self.wndShiningsWindowInventoryLayer = None

			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Show(self):
			self.__LoadWindow()
			self.RefreshShiningsSlot()
			self.wndShiningsWindowInventoryLayer.Show()
			self.AdjustPositionAndSize()

			ui.ScriptWindow.Show(self)

		def Close(self):
			self.wndShiningsWindowInventoryLayer.Hide()
			self.Hide()

		def __LoadWindow(self):
			if self.isLoaded == 1:
				return

			self.isLoaded = 1

			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/ShiningsWindow.py")
			except:
				import exception
				exception.Abort("ShiningsWindow.LoadWindow.LoadObject")

			try:
				self.ORIGINAL_WIDTH = self.GetWidth()
				wndEquip = self.GetChild("CostumeSlot")
				self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
				self.wndShiningsWindowInventoryLayer = self.GetChild("wndShiningsWindowInventoryLayer")

			except:
				import exception
				exception.Abort("ShiningsWindow.LoadWindow.BindObject")

			## Equipment
			wndEquip.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
			wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
			wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
			wndEquip.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
			wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
			wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

			self.wndEquip = wndEquip

		def RefreshShiningsSlot(self):
			getItemVNum=player.GetItemIndex

			for i in xrange(item.SHINING_SLOT_COUNT):
				slotNumber = item.SHINING_SLOT_START + i
				self.wndEquip.SetItemSlot(slotNumber, getItemVNum(slotNumber), 0)

			self.wndEquip.RefreshSlot()

		def IsOpeningInventory(self):
			return self.wndShiningsWindowInventoryLayer.IsShow()

		def GetBasePosition(self):
			x, y = self.wndInventory.GetGlobalPosition()
			return x - 280, y

		def AdjustPositionAndSize(self):
			bx, by = self.GetBasePosition()
		
			if self.IsOpeningInventory():
				self.SetPosition(bx, by)
				self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())
			else:
				self.SetPosition(bx, by)
				self.SetSize(10, self.GetHeight())

class InventoryWindow(ui.ScriptWindow):

	USE_TYPE_TUPLE = ("USE_CLEAN_SOCKET", "USE_CHANGE_ATTRIBUTE", "USE_ADD_ATTRIBUTE", "USE_ADD_ATTRIBUTE2", "USE_ADD_ACCESSORY_SOCKET", "USE_PUT_INTO_ACCESSORY_SOCKET", "USE_PUT_INTO_BELT_SOCKET", "USE_PUT_INTO_RING_SOCKET")
	if app.ENABLE_USE_COSTUME_ATTR:
		USE_TYPE_TUPLE = tuple(list(USE_TYPE_TUPLE) + ["USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR"])
	if app.BL_67_ATTR:
		USE_TYPE_TUPLE = tuple(list(USE_TYPE_TUPLE) + ["USE_CHANGE_ATTRIBUTE2"])
	USE_TYPE_TUPLE = tuple(list(USE_TYPE_TUPLE) + ["USE_ADD_PENDANT_ATTRIBUTE", "USE_ADD_PENDANT_FIVE_ATTRIBUTE", "USE_CHANGE_PENDANT_ATTRIBUTE"])
	# USE_TYPE_TUPLE = tuple(list(USE_TYPE_TUPLE) + ["USE_CHANGE_ATTRIBUTE_DS"])

	questionDialog = None
	tooltipItem = None
	wndCostume = None
	if app.ENABLE_SHINING_ITEM_SYSTEM and not constInfo.ENABLE_COSTUME_PAGES:
		wndShinings = None
		isOpenedShiningsWindowWhenClosingInventory = 0
	wndBelt = None
	dlgPickMoney = None
	if app.ENABLE_SPECIAL_INVENTORY:
		inventoryTypeIndex = 0

	interface = None

	if app.ENABLE_INGAME_WIKI:
		wndWiki = None
	
	if app.ENABLE_PREMIUM_PRIVATE_SHOP:
		wndPrivateShop = None
		wndPrivateShopSearch = None

	sellingSlotNumber = -1
	isLoaded = 0
	isOpenedCostumeWindowWhenClosingInventory = 0
	isOpenedBeltWindowWhenClosingInventory = 0

	if app.ENABLE_HIGHLIGHT_SLOT_SYSTEM:
		liHighlightedItems = []

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.isOpenedBeltWindowWhenClosingInventory = 0

		self.inventoryPageIndex = 0

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine = None
			self.wndAcceAbsorption = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		if app.ENABLE_SAVE_LAST_WINDOW_POSITION:
			self.SetLastPosition()

		ui.ScriptWindow.Show(self)

		if self.isOpenedCostumeWindowWhenClosingInventory and self.wndCostume:
			self.wndCostume.Show()

		if app.ENABLE_SHINING_ITEM_SYSTEM and not constInfo.ENABLE_COSTUME_PAGES:
			if self.isOpenedShiningsWindowWhenClosingInventory and self.wndShinings:
				self.wndShinings.Show()

		if self.wndBelt:
			self.wndBelt.Show(self.isOpenedBeltWindowWhenClosingInventory)

		if app.ENABLE_GEM_SYSTEM:
			if self.interface:
				self.interface.ShowExpandedMoney()

	def BindInterfaceClass(self, interface):
		self.interface = interface
		
	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/InventoryWindow.py")
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.LoadObject")

		try:
			wndItem = self.GetChild("ItemSlot")
			wndEquip = self.GetChild("EquipmentSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.wndMoney = self.GetChild("Money")
			self.wndMoneySlot = self.GetChild("Money_Slot")
			self.mallButton = self.GetChild2("MallButton")
			if app.ENABLE_REMOTE_SHOP:
				self.RemoteShopButton = self.GetChild2("RemoteShopButton")
			self.costumeButton = self.GetChild2("CostumeButton")

			if app.ENABLE_GEM_SYSTEM:
				self.wndMoneySlot.Hide()
				self.wndMoneyIcon = self.GetChild("Money_Icon")
				self.wndMoneyIcon.Hide()

			self.inventoryTab = []
			for i in xrange(player.INVENTORY_PAGE_COUNT):
				self.inventoryTab.append(self.GetChild("Inventory_Tab_%02d" % (i+1)))

			self.equipmentTab = []
			self.equipmentTab.append(self.GetChild("Equipment_Tab_01"))
			self.equipmentTab.append(self.GetChild("Equipment_Tab_02"))

			if app.ENABLE_SORT_INVENTORY:
				self.sortButton = self.GetChild("SortButton")
				self.sortButton.SetEvent(self.OnSortInventory)

			if self.costumeButton and not app.ENABLE_COSTUME_SYSTEM:
				self.costumeButton.Hide()
				self.costumeButton.Destroy()
				self.costumeButton = 0

			# Belt Inventory Window
			self.wndBelt = None

			if app.ENABLE_NEW_EQUIPMENT_SYSTEM:
				self.wndBelt = BeltInventoryWindow(self)

		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.BindObject")

		## Item
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## Equipment
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## PickMoneyDialog
		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.Hide()

		## RefineDialog
		self.refineDialog = uiRefine.RefineDialog()
		self.refineDialog.Hide()

		## MoneySlot
		self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog))

		for i in xrange(player.INVENTORY_PAGE_COUNT):
			self.inventoryTab[i].SetEvent(lambda arg=i: self.SetInventoryPage(arg))
		self.inventoryTab[0].Down()

		if app.ENABLE_SPECIAL_INVENTORY:
			self.inventoryTypeTabDict = {
				0 : self.GetChild("Inventory_Type_01"),
				1 : self.GetChild("Inventory_Type_02"),
				2 : self.GetChild("Inventory_Type_03"),
				3 : self.GetChild("Inventory_Type_04"),
			}

			self.inventoryTypeButtonDict = {
				0 : self.GetChild("Inventory_Type_Button_01"),
				1 : self.GetChild("Inventory_Type_Button_02"),
				2 : self.GetChild("Inventory_Type_Button_03"),
				3 : self.GetChild("Inventory_Type_Button_04")
			}
			
			self.inventorySafeTypePageIndex = {
				0 : 0,
				1 : 0,
				2 : 0,
				3 : 0,
			}

			for (type, tabButton) in self.inventoryTypeButtonDict.items():
				tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabTypeButton), type)

		self.equipmentTab[0].SetEvent(lambda arg=0: self.SetEquipmentPage(arg))
		self.equipmentTab[1].SetEvent(lambda arg=1: self.SetEquipmentPage(arg))
		self.equipmentTab[0].Down()
		self.equipmentTab[0].Hide()
		self.equipmentTab[1].Hide()

		self.wndItem = wndItem
		self.wndEquip = wndEquip
		self.dlgPickMoney = dlgPickMoney

		# MallButton
		if self.mallButton:
			self.mallButton.SetEvent(ui.__mem_func__(self.ClickMallButton))

		if app.ENABLE_REMOTE_SHOP:
			if self.RemoteShopButton:
				self.RemoteShopButton.SetEvent(ui.__mem_func__(self.ClickRemoteShopButton))

		# Costume Button
		if self.costumeButton:
			self.costumeButton.SetEvent(ui.__mem_func__(self.ClickCostumeButton))

		self.wndCostume = None
		if app.ENABLE_SHINING_ITEM_SYSTEM and not constInfo.ENABLE_COSTUME_PAGES:
			self.wndShinings = None

 		#####
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.listAttachedAcces = []

		## Refresh
		self.SetInventoryPage(0)
		self.SetEquipmentPage(0)
		if app.ENABLE_SPECIAL_INVENTORY:
			self.SetInventoryType(0)
		self.RefreshItemSlot()
		self.RefreshStatus()

	if app.ENABLE_SORT_INVENTORY:
		def OnSortInventory(self):
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.INVENTORY_SORT_QUESTION)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.AcceptSortInventory))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.CancelSortInventory))
			self.questionDialog.Open()
			self.questionDialog.inventoryTypeIndex = self.inventoryTypeIndex

		def CancelSortInventory(self):
			self.OnCloseQuestionDialog()

		def AcceptSortInventory(self):
			if self.questionDialog:
				if self.questionDialog.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
					net.SendChatPacket("/sort_inventory")
				else:
					net.SendChatPacket("/sort_special_inventory %i" % (self.questionDialog.inventoryTypeIndex-1))
	
				self.questionDialog.Close()

			self.questionDialog = None		

	def Destroy(self):
		if app.ENABLE_SAVE_LAST_WINDOW_POSITION:
			if self.IsShow():
				self.SaveLastPosition()
		self.ClearDictionary()

		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0

		self.refineDialog.Destroy()
		self.refineDialog = 0

		self.tooltipItem = None
		self.wndItem = 0
		self.wndEquip = 0
		self.dlgPickMoney = 0
		self.wndMoney = 0
		self.wndMoneySlot = 0
		if app.ENABLE_SPECIAL_INVENTORY:
			self.inventoryTypeIndex = 0
		self.questionDialog = None
		self.mallButton = None
		if app.ENABLE_REMOTE_SHOP:
			self.RemoteShopButton = None
		self.costumeButton = None
		self.wndDragonSoulRefine = None
		self.interface = None
		if app.ENABLE_INGAME_WIKI:
			if self.wndWiki:
				self.wndWiki = None
	
		if self.wndCostume:
			self.wndCostume.Destroy()
			self.wndCostume = None

		if self.wndBelt:
			self.wndBelt.Destroy()
			self.wndBelt = None

		if app.ENABLE_SHINING_ITEM_SYSTEM and not constInfo.ENABLE_COSTUME_PAGES:
			if self.wndShinings:
				self.wndShinings.Destroy()
				self.wndShinings = 0

		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			if self.wndPrivateShop:
				self.wndPrivateShop = None
				
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch = None

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine = None
			self.wndAcceAbsorption = None

		self.inventoryTab = []
		self.equipmentTab = []
		if app.ENABLE_HIGHLIGHT_SLOT_SYSTEM:
			self.liHighlightedItems = []

	def Hide(self):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

		if self.wndCostume:
			self.isOpenedCostumeWindowWhenClosingInventory = self.wndCostume.IsShow()
			self.wndCostume.Close()

		if self.wndBelt:
			self.isOpenedBeltWindowWhenClosingInventory = self.wndBelt.IsOpeningInventory()
			print "Is Opening Belt Inven?? ", self.isOpenedBeltWindowWhenClosingInventory
			self.wndBelt.Close()

		if app.ENABLE_SHINING_ITEM_SYSTEM and not constInfo.ENABLE_COSTUME_PAGES:
			if self.wndShinings:
				self.isOpenedShiningsWindowWhenClosingInventory = self.wndShinings.IsShow()
				self.wndShinings.Close()

		if self.dlgPickMoney:
			self.dlgPickMoney.Close()

		if app.ENABLE_SAVE_LAST_WINDOW_POSITION:
			if self.IsShow():
				self.SaveLastPosition()

		wndMgr.Hide(self.hWnd)

	def Close(self):
		self.Hide()

	def SetInventoryPage(self, page):
		self.inventoryPageIndex = page
		for i in xrange(player.INVENTORY_PAGE_COUNT):
			if i!=page:
				self.inventoryTab[i].SetUp()
			else:
				self.inventoryTab[i].Down()

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				self.RefreshBagSlotWindow()
			else:
				self.RefreshSpecialInventory()
		else:
			self.RefreshBagSlotWindow()

	def SetEquipmentPage(self, page):
		self.equipmentPageIndex = page
		self.equipmentTab[1-page].SetUp()
		self.RefreshEquipSlotWindow()


	if app.ENABLE_SPECIAL_INVENTORY:
		def __OnClickTabTypeButton(self, type):
			self.SetInventoryType(type)

		def SetInventoryType(self, type):
			self.inventoryTab[self.inventoryPageIndex].SetUp()
			self.inventoryPageIndex = self.inventorySafeTypePageIndex[int(type)]
			self.inventoryTab[self.inventoryPageIndex].Down()
			self.inventoryTypeIndex = int(type)
			
			for (tabKey, tabButton) in self.inventoryTypeButtonDict.items():
				if type!=tabKey:
					tabButton.SetUp()

			for tabValue in self.inventoryTypeTabDict.itervalues():
				tabValue.Hide()

			self.inventoryTypeTabDict[type].Show()

			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				self.RefreshBagSlotWindow()
			else:
				self.RefreshSpecialInventory()

		def GetInventoryType(self):
			return self.inventoryTypeIndex

		def __SpecialInventoryLocalSlotPosToGlobalSlotPos(self, local):
			if player.IsEquipmentSlot(local) or player.IsCostumeSlot(local):
				return local

			start = player.SPECIAL_INVENTORY_SLOT_START
			slotCountPerType = (player.SPECIAL_INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT)
			slotStartForType = start + slotCountPerType * (self.inventoryTypeIndex-1)
			slotPageStart = slotStartForType + (player.SPECIAL_INVENTORY_PAGE_SIZE * self.inventoryPageIndex)

			return slotPageStart + local

		def RefreshSpecialInventory(self):
			if not self.wndItem:
				return

			if app.WJ_ENABLE_TRADABLE_ICON:
				if self.interface:
					onTopWindow = self.interface.GetOnTopWindow()

			for i in xrange(player.SPECIAL_INVENTORY_PAGE_SIZE):
				slotNumber = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(i)

				itemCount = player.GetItemCount(slotNumber)
				if 0 == itemCount:
					self.wndItem.ClearSlot(i)
					continue
				elif 1 == itemCount:
					itemCount = 0

				itemVnum = player.GetItemIndex(slotNumber)
				if app.ENABLE_HIGHLIGHT_SLOT_SYSTEM:
					if itemVnum == 0 and slotNumber in self.liHighlightedItems:
						self.liHighlightedItems.remove(slotNumber)

					self.wndItem.DeactivateSlotEffect(i)

				self.wndItem.SetItemSlot(i, itemVnum, itemCount)

				if app.WJ_ENABLE_TRADABLE_ICON:
					if itemVnum and self.interface and onTopWindow:
						if self.interface.MarkUnusableInvenSlotOnTopWnd(onTopWindow, slotNumber):
							if itemVnum == 50200:
								self.wndItem.SetCantMouseEventSlot(i)
							else:
								self.wndItem.SetUnusableSlotOnTopWnd(i)
						else:
							self.wndItem.SetUsableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

			if app.ENABLE_HIGHLIGHT_SLOT_SYSTEM:
				self.__RefreshHighlights()
			self.wndItem.RefreshSlot()

	def ClickMallButton(self):
		# print "click_mall_button"
		# net.SendChatPacket("/click_mall")
		net.SendChatPacket("/click_safebox")

	if app.ENABLE_REMOTE_SHOP:
		def ClickRemoteShopButton(self):
			# print "click_remote shop_button"
			self.interface.OpenRemoteShop()

	def ClickCostumeButton(self):
		# print "Click Costume Button"
		if self.wndCostume:
			if self.wndCostume.IsShow():
				self.wndCostume.Hide()
			else:
				self.wndCostume.Show()
		else:
			self.wndCostume = CostumeWindow(self)
			self.wndCostume.Show()

		if app.ENABLE_SHINING_ITEM_SYSTEM and not constInfo.ENABLE_COSTUME_PAGES:
			if self.wndShinings:
				if self.wndShinings.IsShow():
					self.wndShinings.Hide()
				else:
					self.wndShinings.Show()
			else:
				self.wndShinings = ShiningsWindow(self)
				self.wndShinings.Show()


	def OpenPickMoneyDialog(self):

		if mouseModule.mouseController.isAttached():

			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			if player.SLOT_TYPE_SAFEBOX == mouseModule.mouseController.GetAttachedType():

				if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

			mouseModule.mouseController.DeattachObject()

		else:
			curMoney = player.GetElk()

			if curMoney <= 0:
				return

			self.dlgPickMoney.SetTitleName(localeInfo.PICK_MONEY_TITLE)
			self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
			self.dlgPickMoney.Open(curMoney)
			self.dlgPickMoney.SetMax(9)

	def OnPickMoney(self, money):
		mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money)

	def OnPickItem(self, count):
		itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex
		selectedItemVNum = player.GetItemIndex(itemSlotIndex)
		if app.ENABLE_SPLIT_BY_COUNT:
			if self.dlgPickMoney.GetSplitPickFlag() is False:
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)
			else:
				net.SendSplitItemByCount(player.INVENTORY, itemSlotIndex, count)
		else:
			mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
		if player.IsEquipmentSlot(local) or player.IsCostumeSlot(local) or (app.ENABLE_NEW_EQUIPMENT_SYSTEM and player.IsBeltInventorySlot(local)):
			return local

		return self.inventoryPageIndex*player.INVENTORY_PAGE_SIZE + local
		
	def GetInventoryPageIndex(self):
		return self.inventoryPageIndex

	def RefreshBagSlotWindow(self):
		if not self.wndItem:
			return

		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVNum=self.wndItem.SetItemSlot

		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.interface:
				onTopWindow = self.interface.GetOnTopWindow()

		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			itemCount = getItemCount(slotNumber)
			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			itemVnum = getItemVNum(slotNumber)
			setItemVNum(i, itemVnum, itemCount)
			
			itemType = 0
			itemSubType = 0
			
			if itemVnum:
				item.SelectItem(itemVnum)
				itemType = item.GetItemType()
				itemSubType = item.GetItemSubType()
			
			if app.ENABLE_HIGHLIGHT_SLOT_SYSTEM:
				if itemVnum == 0 and slotNumber in self.liHighlightedItems:
					self.liHighlightedItems.remove(slotNumber)

			if constInfo.IS_AUTO_POTION(itemVnum):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]

				isActivated = 0 != metinSocket[0]

				if isActivated:
					potionType = 0
					if constInfo.IS_AUTO_POTION_HP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_HP
						self.wndItem.ActivateSlot(i, 2)
					elif constInfo.IS_AUTO_POTION_SP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_SP
						self.wndItem.ActivateSlot(i, 5)

					usedAmount = int(metinSocket[1])
					totalAmount = int(metinSocket[2])
					player.SetAutoPotionInfo(potionType, isActivated, (totalAmount - usedAmount), totalAmount, self.__InventoryLocalSlotPosToGlobalSlotPos(i))
				else:
					self.wndItem.DeactivateSlot(i)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				slotNumberChecked = 0
				for j in xrange(acce.WINDOW_MAX_MATERIALS):
					(isHere, iCell) = acce.GetAttachedItem(j)
					if isHere:
						if iCell == slotNumber:
							self.wndItem.ActivateSlot(i, 3)
							if not slotNumber in self.listAttachedAcces:
								self.listAttachedAcces.append(slotNumber)

							slotNumberChecked = 1
					else:
						if slotNumber in self.listAttachedAcces and not slotNumberChecked:
							self.wndItem.DeactivateSlot(i)
							self.listAttachedAcces.remove(slotNumber)

			if constInfo.ENABLE_ACTIVE_PET_SEAL_EFFECT and constInfo.IS_PET_SEAL(itemVnum):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				isActivated = 0 != metinSocket[2]
				if isActivated:
					self.wndItem.ActivateSlot(i, 3)
				else:
					self.wndItem.DeactivateSlot(i)

			if itemType == 43 or (itemType == 3 and itemSubType == 38):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				isActivated = 0 != metinSocket[1]
				if isActivated:
					self.wndItem.ActivateSlot(i, 3)
				else:
					self.wndItem.DeactivateSlot(i)

			if app.ENABLE_EXTENDED_BLEND:
				if item.IsBlend(itemVnum):
					metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					blendColor = constInfo.GetBlendColor(itemVnum)

					isActivated = 0 != metinSocket[1]
					
					if isActivated:
						self.wndItem.ActivateSlot(i, blendColor)
					else:
						self.wndItem.DeactivateSlot(i)

			if item.GetItemType() == item.ITEM_TYPE_MOUNT:
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]

				isActivated = 0 != metinSocket[2]

				if isActivated:
					self.wndItem.ActivateSlot(i, 6)
				else:
					self.wndItem.DeactivateSlot(i)

			if app.ENABLE_NEW_TYPE_OF_POTION and constInfo.IS_NEW_SPEED_POTION(itemVnum):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]

				isActivated = 0 != metinSocket[0]

				if isActivated:
					self.wndItem.ActivateSlot(i)
				else:
					self.wndItem.DeactivateSlot(i)

			if app.WJ_ENABLE_TRADABLE_ICON:
				if itemVnum and self.interface and onTopWindow:
					if self.interface.MarkUnusableInvenSlotOnTopWnd(onTopWindow, slotNumber):
						if itemVnum == 50200:
							self.wndItem.SetCantMouseEventSlot(i)
						else:
							self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)
				else:
					self.wndItem.SetUsableSlotOnTopWnd(i)

		self.wndItem.RefreshSlot()
		if app.ENABLE_HIGHLIGHT_SLOT_SYSTEM:
			self.__RefreshHighlights()

		if self.wndBelt:
			self.wndBelt.RefreshSlot()

		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			if self.wndPrivateShop and self.wndPrivateShop.IsShow():
				self.wndPrivateShop.RefreshLockedSlot()

	if app.ENABLE_HIGHLIGHT_SLOT_SYSTEM:
		def HighlightSlot(self, slot):
			if not slot in self.liHighlightedItems:
				self.liHighlightedItems.append(slot)
		
		def __RefreshHighlights(self):
			for i in xrange(player.INVENTORY_PAGE_SIZE):
				if app.ENABLE_SPECIAL_INVENTORY:
					if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
						slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
					else:
						slotNumber = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(i)
				else:
					slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

				if slotNumber in self.liHighlightedItems:
					self.wndItem.ActivateSlotEffect(i)

	def SetItemSlotVnum(self, slotNumber):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVNum=self.wndEquip.SetItemSlot
		itemCount = getItemCount(slotNumber)
		if itemCount <= 1:
			itemCount = 0
		setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
		return

	def RefreshEquipSlotWindow(self):
		SetItemSlotVnum = self.SetItemSlotVnum
		for i in xrange(player.EQUIPMENT_PAGE_COUNT):
			SetItemSlotVnum(player.EQUIPMENT_SLOT_START + i)

		if app.ENABLE_NEW_EQUIPMENT_SYSTEM:
			SetItemSlotVnum(item.EQUIPMENT_BELT)
		if app.ENABLE_PENDANT_SYSTEM:
			SetItemSlotVnum(item.EQUIPMENT_PENDANT)
		if app.ENABLE_GLOVE_SYSTEM:
			SetItemSlotVnum(item.EQUIPMENT_GLOVE)
		if app.ENABLE_COSTUME_PET:
			SetItemSlotVnum(item.EQUIPMENT_PET)

		self.wndEquip.RefreshSlot()

		if self.wndCostume:
			self.wndCostume.RefreshCostumeSlot()

		if app.ENABLE_SHINING_ITEM_SYSTEM and not constInfo.ENABLE_COSTUME_PAGES:
			if self.wndShinings:
				self.wndShinings.RefreshShiningsSlot()

	def RefreshItemSlot(self):
		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				self.RefreshBagSlotWindow()
			else:
				self.RefreshSpecialInventory()
		else:
			self.RefreshBagSlotWindow()
		self.RefreshEquipSlotWindow()

	def RefreshStatus(self):
		money = player.GetElk()
		self.wndMoney.SetText(localeInfo.NumberToMoneyString(money))

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SellItem(self):
		if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotNumber):
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY)
				snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnDetachMetinFromItem(self):
		if None == self.questionDialog:
			return

		#net.SendItemUseToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.__SendUseItemToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	## Slot Event
	def SelectEmptySlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)
			else:
				selectedSlotPos = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)
		else:
			selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)	

		if mouseModule.mouseController.isAttached():

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				#@fixme011 BEGIN (block ds equip)
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
					mouseModule.mouseController.DeattachObject()
					return
				#@fixme011 END

				itemCount = player.GetItemCount(attachedSlotPos)
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)

				if item.IsRefineScroll(attachedItemIndex):
					self.wndItem.SetUseMode(False)

			elif player.SLOT_TYPE_CHANGE_EQUIP == attachedSlotType and app.FAST_EQUIP_WORLDARD:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.CHANGE_EQUIP, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)

			elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
				if app.ENABLE_PREMIUM_PRIVATE_SHOP:
					if not uiPrivateShopBuilder.IsBuildingPrivateShop():
						self.wndPrivateShop.SendItemCheckoutPacket(attachedSlotPos, selectedSlotPos)
						mouseModule.mouseController.DeattachObject()
						return
						
				mouseModule.mouseController.RunCallBack("INVENTORY")

			elif player.SLOT_TYPE_SHOP == attachedSlotType:
				net.SendShopBuyPacket(attachedSlotPos)

			elif player.SLOT_TYPE_GUILDSTORAGE == attachedSlotType:
				import guildstorage
				guildstorage.CheckOutItem(attachedSlotPos, selectedSlotPos, player.SLOT_TYPE_INVENTORY)

			elif app.ENABLE_SWITCHBOT_SYSTEM and player.SLOT_TYPE_SWITCHBOT == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)

			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:

				if player.ITEM_MONEY == attachedItemIndex:
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

				else:
					net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

			elif (app.ENABLE_GUILD_SAFEBOX and player.SLOT_TYPE_GUILD_SAFEBOX == attachedSlotType):
				if player.ITEM_MONEY == attachedItemIndex:
					net.SendGuildSafeboxTakeGoldPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

				else:
					net.SendGuildSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

			elif player.SLOT_TYPE_MALL == attachedSlotType:
				net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)

			mouseModule.mouseController.DeattachObject()

	def SelectItemSlot(self, itemSlotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)
			else:
				itemSlotIndex = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)
		else:
			itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)	

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				#@fixme011 BEGIN (block ds equip)
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
					mouseModule.mouseController.DeattachObject()
					return
				#@fixme011 END
				self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)

			mouseModule.mouseController.DeattachObject()

		else:

			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex)

			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

			elif app.ENABLE_FAST_STACK and (app.IsPressed(app.DIK_LALT) and app.IsPressed(app.DIK_LSHIFT)):
				net.SendFastStackPacket(player.SLOT_TYPE_INVENTORY, itemSlotIndex)

			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(itemSlotIndex)
				ime.PasteString(link)

			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(itemSlotIndex)

				if itemCount > 1:
					self.dlgPickMoney.SetTitleName(localeInfo.PICK_ITEM_TITLE)
					self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					if app.ENABLE_SPLIT_BY_COUNT:
						self.dlgPickMoney.IsSplitPick(True)
					self.dlgPickMoney.Open(itemCount)
					self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex
				#else:
					#selectedItemVNum = player.GetItemIndex(itemSlotIndex)
					#mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum)

			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex = player.GetItemIndex(itemSlotIndex)
				
				if app.ENABLE_PREMIUM_PRIVATE_SHOP:
					if self.wndPrivateShop and self.wndPrivateShop.IsShow():
						self.wndPrivateShop.AttachItemToPrivateShop(itemSlotIndex, player.SLOT_TYPE_INVENTORY)
						return
						
					if self.wndPrivateShopSearch and self.wndPrivateShopSearch.IsShow():
						self.wndPrivateShopSearch.SelectItem(itemIndex)
						return

				if app.ENABLE_INGAME_WIKI:
					if self.wndWiki:
						if self.wndPrivateShopSearch and self.wndPrivateShopSearch.IsShow():
							return
						else:
							if self.wndWiki.IsShow():
								self.wndWiki.SelectItem(itemIndex)
								self.wndWiki.StartSearch()
								return

				if True == item.CanAddToQuickSlotItem(itemIndex):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.QUICKSLOT_REGISTER_DISABLE_ITEM)

			else:
				selectedItemVNum = player.GetItemIndex(itemSlotIndex)
				itemCount = player.GetItemCount(itemSlotIndex)
				if app.ENABLE_EXTENDED_BLEND:
					if self.__CanAttachThisItem(selectedItemVNum, itemSlotIndex):
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
				else:
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)

				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):
					self.wndItem.SetUseMode(True)
				else:
					self.wndItem.SetUseMode(False)

				snd.PlaySound("sound/ui/pick.wav")

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos):
		if srcItemSlotPos == dstItemSlotPos:
			return

		if app.ENABLE_ITEM_TIME_EXTENDER:
			if constInfo.IS_ITEM_TIME_EXTENDER(player.GetItemIndex(srcItemSlotPos)):
				item.SelectItem(player.GetItemIndex(dstItemSlotPos))

				if (
					(item.GetItemType() == item.ITEM_TYPE_COSTUME and (
						item.GetItemSubType() == item.COSTUME_TYPE_BODY or
						item.GetItemSubType() == item.COSTUME_TYPE_HAIR or
						item.GetItemSubType() == item.COSTUME_TYPE_WEAPON
					)) or
					item.GetItemType() == item.ITEM_TYPE_MOUNT or
					(item.GetItemType() == item.ITEM_TYPE_PET and item.GetItemSubType() == item.PET_PAY)
				):
					self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		# cyh itemseal 2013 11 08
		if app.ENABLE_SOULBIND_SYSTEM and item.IsSealScroll(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		
		elif srcItemVID == 71109:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
		
		elif item.IsRefineScroll(srcItemVID):
			if srcItemVID == player.GetItemIndex(dstItemSlotPos):
				self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
			else:
				self.RefineItem(srcItemSlotPos, dstItemSlotPos)
				self.wndItem.SetUseMode(False)

		elif item.IsMetin(srcItemVID):
			self.AttachMetinToItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif item.GetUseType(srcItemVID) in self.USE_TYPE_TUPLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		else:
			#snd.PlaySound("sound/ui/drop.wav")

			if player.IsEquipmentSlot(dstItemSlotPos):

				if item.IsEquipmentVID(srcItemVID):
					self.__UseItem(srcItemSlotPos)

			else:
				if app.ENABLE_EXTENDED_BLEND:
					if item.GetItemType() == item.ITEM_TYPE_BLEND:
						self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)
					else:
						self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
				else:
					self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
				#net.SendItemMovePacket(srcItemSlotPos, dstItemSlotPos, 0)

	def __SellItem(self, itemSlotPos):
		if not player.IsEquipmentSlot(itemSlotPos):
			self.sellingSlotNumber = itemSlotPos
			itemIndex = player.GetItemIndex(itemSlotPos)
			itemCount = player.GetItemCount(itemSlotPos)


			self.sellingSlotitemIndex = itemIndex
			self.sellingSlotitemCount = itemCount

			item.SelectItem(itemIndex)
			## 20140220
			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup = uiCommon.PopupDialog()
				popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			itemPrice = item.GetISellItemPrice()

			if item.Is1GoldItem():
				itemPrice = itemCount / itemPrice
			else:
				itemPrice = itemPrice * itemCount

			if not app.ENABLE_NO_SELL_PRICE_DIVIDED_BY_5:
				itemPrice /= 5

			item.GetItemName(itemIndex)
			itemName = item.GetItemName()

			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()
			self.questionDialog.count = itemCount

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop = None

	def RefineItem(self, scrollSlotPos, targetSlotPos):

		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotPos):
			return

		if app.ENABLE_REFINE_UI_RENEWAL:
			constInfo.AUTO_REFINE_TYPE = 1
			constInfo.AUTO_REFINE_DATA["ITEM"][0] = scrollSlotPos
			constInfo.AUTO_REFINE_DATA["ITEM"][1] = targetSlotPos

		###########################################################
		self.__SendUseItemToItemPacket(scrollSlotPos, targetSlotPos)
		#net.SendItemUseToItemPacket(scrollSlotPos, targetSlotPos)
		return

	def DetachMetinFromItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if not player.CanDetach(scrollIndex, targetSlotPos):
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				item.SelectItem(scrollIndex)
				if item.GetValue(0) == acce.CLEAN_ATTR_VALUE0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ACCE_FAILURE_CLEAN)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			return

		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(localeInfo.REFINE_DO_YOU_SEPARATE_METIN)
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			item.SelectItem(targetIndex)
			if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
				item.SelectItem(scrollIndex)
				if item.GetValue(0) == acce.CLEAN_ATTR_VALUE0:
					self.questionDialog.SetText(localeInfo.ACCE_DO_YOU_CLEAN)

		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos = scrollSlotPos
		self.questionDialog.targetPos = targetSlotPos

	def AttachMetinToItem(self, metinSlotPos, targetSlotPos):
		metinIndex = player.GetItemIndex(metinSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		item.SelectItem(metinIndex)
		itemName = item.GetItemName()
		
		if item.GetValue(0) != 0:
			if player.GetItemMetinSocket(targetSlotPos, 0) != 0:
				return

			# if item.GetItemType() != item.ITEM_TYPE_ARMOR:
				# return
			
			self.interface.OpenAttachMetin(metinSlotPos, targetSlotPos)
			return
		
		result = player.CanAttachMetin(metinIndex, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_CAN_NOT_ATTACH(itemName))

		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_SOCKET(itemName))

		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))

		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.interface.OpenAttachMetin(metinSlotPos, targetSlotPos)

	if app.ENABLE_ITEM_MANAGER:
		## TODO : remove
		def OnUpdate(self):
			if self.wndItem:
				self.RefreshDestroySellCover()
			
		def RefreshDestroySellCover(self):
			for i in xrange(player.INVENTORY_PAGE_SIZE):
				if app.ENABLE_SPECIAL_INVENTORY:
					if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
						slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
					else:
						slotNumber = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(i)
				else:
					slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

				if uiItemManager.IsItemAlreadyInList(slotNumber):
					self.wndItem.SetUnusableSlotOnTopWndEx(i)
				else:
					self.wndItem.SetUsableSlotOnTopWndEx(i)

	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos):
		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:
				overSlotPosGlobal = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
			else:
				overSlotPosGlobal = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
		else:
			overSlotPosGlobal = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)	

		self.wndItem.SetUsableItem(False)

		if app.ENABLE_HIGHLIGHT_SLOT_SYSTEM and overSlotPosGlobal in self.liHighlightedItems:	
			self.liHighlightedItems.remove(overSlotPosGlobal)
			self.wndItem.DeactivateSlotEffect(overSlotPos)

		if mouseModule.mouseController.isAttached():
			attachedItemType = mouseModule.mouseController.GetAttachedType()
			if player.SLOT_TYPE_INVENTORY == attachedItemType:

				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

				if attachedItemVNum==player.ITEM_MONEY: # @fixme005
					pass
				elif self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPosGlobal):
					self.wndItem.SetUsableItem(True)
					self.ShowToolTip(overSlotPosGlobal)
					return

		self.ShowToolTip(overSlotPosGlobal)


	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		if item.IsRefineScroll(srcItemVNum):
			return True
		elif item.IsMetin(srcItemVNum):
			return True
		elif item.IsDetachScroll(srcItemVNum):
			return True
		elif item.IsKey(srcItemVNum):
			return True
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True
		else:
			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return True

		if app.ENABLE_ITEM_TIME_EXTENDER:
			if constInfo.IS_ITEM_TIME_EXTENDER(srcItemVNum):
				return True

		return False

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		if srcSlotPos == dstSlotPos:
			return False

		if app.ENABLE_ITEM_TIME_EXTENDER:
			if constInfo.IS_ITEM_TIME_EXTENDER(srcItemVNum):
				item.SelectItem(player.GetItemIndex(dstSlotPos))
		
				if (
					(item.GetItemType() == item.ITEM_TYPE_COSTUME and (
						item.GetItemSubType() == item.COSTUME_TYPE_BODY or
						item.GetItemSubType() == item.COSTUME_TYPE_HAIR or
						item.GetItemSubType() == item.COSTUME_TYPE_WEAPON
					)) or
					item.GetItemType() == item.ITEM_TYPE_MOUNT or
					(item.GetItemType() == item.ITEM_TYPE_PET and item.GetItemSubType() == item.PET_PAY)
				):
					return True

		if item.IsRefineScroll(srcItemVNum):
			if player.REFINE_OK == player.CanRefine(srcItemVNum, dstSlotPos):
				return True
			## if app.ENABLE_EXTENDED_ITEM_COUNT: TODO: use PythonItemModule for item.ITEM_MAX_COUNT
			if srcItemVNum == player.GetItemIndex(dstSlotPos) and player.GetItemCount(dstSlotPos) < 10000:
				return True
		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotPos):
				return True
		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK == player.CanDetach(srcItemVNum, dstSlotPos):
				return True
		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotPos):
				return True

		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True

		else:
			useType=item.GetUseType(srcItemVNum)

			if "USE_CLEAN_SOCKET" == useType:
				if self.__CanCleanBrokenMetinStone(dstSlotPos):
					return True
			elif "USE_CHANGE_ATTRIBUTE" == useType:
				if self.__CanChangeItemAttrList(dstSlotPos):
					return True
			elif useType == "USE_CHANGE_PENDANT_ATTRIBUTE":
				if self.__CanChangePendantAttrList(dstSlotPos):
					return True
			# elif useType == "USE_CHANGE_ATTRIBUTE_DS":
			# 	if self.__CanChangeDragonSoulAttrList(dstSlotPos):
			# 		return True
			elif app.BL_67_ATTR and "USE_CHANGE_ATTRIBUTE2" == useType:
				if self.__CanChangeItemAttrList2(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE2" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif useType == "USE_ADD_PENDANT_ATTRIBUTE":
				if self.__CanAddPendantAttr(dstSlotPos):
					return True
			elif useType == "USE_ADD_PENDANT_FIVE_ATTRIBUTE":
				if self.__CanAddPendantAttr(dstSlotPos):
					return True
			elif "USE_ADD_ACCESSORY_SOCKET" == useType:
				if self.__CanAddAccessorySocket(dstSlotPos):
					return True
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == useType:
				if self.__CanPutAccessorySocket(dstSlotPos, srcItemVNum):
					return True;
			elif "USE_PUT_INTO_BELT_SOCKET" == useType:
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				print "USE_PUT_INTO_BELT_SOCKET", srcItemVNum, dstItemVNum

				item.SelectItem(dstItemVNum)

				if item.ITEM_TYPE_BELT == item.GetItemType():
					return True
			elif app.ENABLE_USE_COSTUME_ATTR and "USE_CHANGE_COSTUME_ATTR" == useType:
				if self.__CanChangeCostumeAttrList(dstSlotPos):
					return True
			elif app.ENABLE_USE_COSTUME_ATTR and "USE_RESET_COSTUME_ATTR" == useType:
				if self.__CanResetCostumeAttr(dstSlotPos):
					return True

		return False

	def __CanCleanBrokenMetinStone(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.ITEM_TYPE_WEAPON != item.GetItemType():
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotPos, i) == constInfo.ERROR_METIN_STONE:
				return True

		return False

	def __CanChangeItemAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
				return True

		return False

	def __CanChangePendantAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR or item.GetItemSubType() != item.ARMOR_PENDANT:
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
				return True

		return False

	def __CanChangeDragonSoulAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_DS:
			return False

		return True

	if app.BL_67_ATTR:
		def __CanChangeItemAttrList2(self, dstSlotPos):
			return uiAttr67Add.Attr67AddWindow.CantAttachToAttrSlot(dstSlotPos, False)

	if app.ENABLE_USE_COSTUME_ATTR:
		def __CanChangeCostumeAttrList(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return False

			item.SelectItem(dstItemVNum)

			if item.GetItemType() != item.ITEM_TYPE_COSTUME:
				return False

			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
					return True

			return False

		def __CanResetCostumeAttr(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return False

			item.SelectItem(dstItemVNum)

			if item.GetItemType() != item.ITEM_TYPE_COSTUME:
				return False

			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
					return True

			return False

	def __CanPutAccessorySocket(self, dstSlotPos, mtrlVnum):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		if mtrlVnum != constInfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			return False

		if curCount>=maxCount:
			return False

		return True

	def __CanAddAccessorySocket(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		ACCESSORY_SOCKET_MAX_SIZE = 3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return False

		return True

	def __CanAddItemAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR):
			return False

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
				attrCount += 1

		if attrCount<4:
			return True

		return False

	def __CanAddPendantAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR or item.GetItemSubType() != item.ARMOR_PENDANT:
			return False

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
				attrCount += 1

		if attrCount<4:
			return True

		return False

	def ShowToolTip(self, slotIndex):
		itemVnum = player.GetItemIndex(slotIndex)
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)
			if app.ENABLE_EMOJI_SYSTEM:
				if app.ENABLE_RENDER_TARGET_PREVIEW:
					self.tooltipItem.AppendRenderingTooltip(itemVnum)

				if app.ENABLE_CHEST_DROP_INFO:
					self.tooltipItem.AppendChestDropInfo(itemVnum)

				if app.ENABLE_ITEM_MANAGER:
					self.tooltipItem.AppendItemManagerTooltip(itemVnum)

				if app.ENABLE_CHECKINOUT_UPDATE:
					self.tooltipItem.AppendCheckInOutInfo(itemVnum)
				
				if app.ENABLE_SWITCHBOT_SYSTEM:
					self.tooltipItem.AppendCheckInSwitchbot(itemVnum)

	def OnTop(self):
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		if app.ENABLE_SPECIAL_INVENTORY:
			if self.inventoryTypeIndex == player.INVENTORY_TYPE_INVENTORY:		
				slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
			else:
				slotIndex = self.__SpecialInventoryLocalSlotPosToGlobalSlotPos(slotIndex)
		else:
			slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)

		# if app.ENABLE_QUICK_OPEN:
		# 	if app.IsPressed(app.DIK_LALT) and constInfo.IsQuickOpen(player.GetItemIndex(slotIndex)):
		# 		net.SendChatPacket("/quick_open {}".format(slotIndex))
		# 		return

		if app.BL_67_ATTR:
			if self.interface.IsShowAttr67():
				self.interface.wndAttr67Add.AutoSetItem((player.INVENTORY, slotIndex), 1)
				return

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			if self.wndDragonSoulRefine.IsShow():
				self.wndDragonSoulRefine.AutoSetItem((player.INVENTORY, slotIndex), 1)
				return

		if app.ENABLE_CHECKINOUT_UPDATE:
			if self.wndSafeBox.IsShow() and not player.IsEquipmentSlot(slotIndex):
				net.SendSafeboxCheckinPacket(slotIndex)
				return

			if self.isShowExchangeDlg() and exchange.isTrading() and not player.IsEquipmentSlot(slotIndex):
				net.SendExchangeItemAddPacket(player.INVENTORY, slotIndex, -1)
				return

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if self.isShowAcceWindow():
				acce.Add(player.INVENTORY, slotIndex, 255)
				return

		if app.ENABLE_ITEM_MANAGER and (app.IsPressed(app.DIK_LSHIFT) and app.IsPressed(app.DIK_LCONTROL)):
			if (player.IsAntiFlagBySlot(slotIndex, item.ANTIFLAG_SELL)\
				or player.IsEquipmentSlot(slotIndex)):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SELL_ITEM_FAILURE)
				return

			if not constInfo.ITEM_REMOVE_WINDOW_STATUS:
				self.interface.ShowItemManager()
			itemIndex = player.GetItemIndex(slotIndex)
			itemCount = player.GetItemCount(slotIndex)
			item.SelectItem(itemIndex)
			itemPrice = item.GetISellItemPrice()
			# if item.Is1GoldItem():
			# 	itemPrice = itemCount / itemPrice / 5
			# else:
			# 	itemPrice = itemPrice * itemCount / 5
			if item.Is1GoldItem():
				itemPrice = itemCount / itemPrice
			else:
				itemPrice = itemPrice * itemCount
			for i in xrange(item.LIMIT_MAX_NUM):
				(limitType, limitValue) = item.GetLimit(i)
				if item.LIMIT_LEVEL == limitType:
					limitLevel = limitValue
			if not uiItemManager.IsItemAlreadyInList(slotIndex):
				SELCETED_ITEMS_LENGTH =	uiItemManager.GetSelectedItemsLength()
				if SELCETED_ITEMS_LENGTH > 19:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ITEM_MANAGER_SELECT_FAILURE)
					return

				uiItemManager.TOTAL_SELL_VALUE += itemPrice
				uiItemManager.AddToSelectedItems(slotIndex)
				if item.GetItemType() == item.ITEM_TYPE_ARMOR or item.GetItemType() == item.ITEM_TYPE_WEAPON:
					if limitLevel > 40:
						uiItemManager.TOTAL_ITEM_FRAG += 1
				# if item.GetItemType() == item.ITEM_TYPE_METIN:
					# uiItemManager.TOTAL_STONE_FRAG += itemCount
				# if itemIndex == XXXX:
					# chat.AppendChat(1, "Xay something.")
			else:
				uiItemManager.TOTAL_SELL_VALUE -= itemPrice
				uiItemManager.RemoveFromSelectedItems(slotIndex)
				if item.GetItemType() == item.ITEM_TYPE_ARMOR or item.GetItemType() == item.ITEM_TYPE_WEAPON:
					if limitLevel > 40:
						uiItemManager.TOTAL_ITEM_FRAG -= 1
				# if item.GetItemType() == item.ITEM_TYPE_METIN:
					# uiItemManager.TOTAL_STONE_FRAG -= itemCount
			return

		self.__UseItem(slotIndex)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()

	def __UseItem(self, slotIndex):
		ItemVNum = player.GetItemIndex(slotIndex)
		item.SelectItem(ItemVNum)

		if app.ENABLE_QUICK_OPEN:
			if app.IsPressed(app.DIK_LALT) and constInfo.IsQuickOpen(player.GetItemIndex(slotIndex)):
				net.SendChatPacket("/quick_open {}".format(slotIndex))
				return

		if app.ENABLE_CHEST_OPEN_RENEWAL:
			if app.IsPressed(app.DIK_LALT) and item.GetItemType() == item.ITEM_TYPE_GIFTBOX:
				itemUseCount = player.GetItemCount(slotIndex) if player.GetItemCount(slotIndex) <= 1000 else 1000
				net.SendOpenChestPacket(slotIndex, itemUseCount)
				return

		if app.ENABLE_CHEST_DROP_INFO:
			if app.IsPressed(app.DIK_LCONTROL) and not self.interface.wndSwitchbot.IsShow():
				if item.HasDropInfo(ItemVNum) and self.interface:
					self.interface.OpenChestDropWindow(ItemVNum)
				return

		if app.ENABLE_RENDER_TARGET_PREVIEW:
			if app.IsPressed(app.DIK_LSHIFT):
				metinSocket = [player.GetItemMetinSocket(slotIndex, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				self.tooltipItem.ModelPreviewFull(ItemVNum, metinSocket)
				return

		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			if ItemVNum == privateShop.PRIVATE_SHOP_SLOT_UNLOCK_ITEM:
				self.wndPrivateShop.OpenUnlockSlotDialog(slotIndex)
				return

		# if app.IsPressed(app.DIK_LALT) and constInfo.IsTreasureBox(ItemVNum):
		# 	net.SendChatPacket("/quickopen click {} {} {}".format(player.INVENTORY, slotIndex, 1000))
		# 	return

		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		elif self.interface.wndSwitchbot.IsShow() and app.IsPressed(app.DIK_LCONTROL):
			for slotIdx in xrange(5):
				net.SendItemMovePacket(player.INVENTORY, slotIndex, player.SWITCHBOT, slotIdx, 1)

		else:
			self.__SendUseItemPacket(slotIndex)

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUseToItemPacket(srcSlotPos, dstSlotPos)

	def __SendUseItemPacket(self, slotPos):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemUsePacket(slotPos)

	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		net.SendItemMovePacket(srcSlotPos, dstSlotPos, srcItemCount)

	def SetDragonSoulRefineWindow(self, wndDragonSoulRefine):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoulRefine = wndDragonSoulRefine

	def OnMoveWindow(self, x, y):
		# print "Inventory Global Pos : ", self.GetGlobalPosition()
		if self.wndBelt:
			# print "Belt Global Pos : ", self.wndBelt.GetGlobalPosition()
			self.wndBelt.AdjustPositionAndSize()

		if app.ENABLE_COSTUMEWINDOW_STICKED_TO_INVENTORYWINDOW:
			if self.wndCostume:
				self.wndCostume.AdjustPositionAndSize()

		if app.ENABLE_SHINING_ITEM_SYSTEM and not constInfo.ENABLE_COSTUME_PAGES:
			if self.wndShinings:
				self.wndShinings.AdjustPositionAndSize()

	def OnRunMouseWheel(self, nLen):
		if self.wndItem.IsIn():
			if nLen > 0:
				if self.inventoryPageIndex < 3:
					self.SetInventoryPage(self.inventoryPageIndex + 1)
			else:
				if self.inventoryPageIndex > 0:
					self.SetInventoryPage(self.inventoryPageIndex - 1)
		else:
			if nLen > 0:
				if self.inventoryTypeIndex < 3:
					self.SetInventoryType(self.inventoryTypeIndex + 1)
			else:
				if self.inventoryTypeIndex > 0:
					self.SetInventoryType(self.inventoryTypeIndex - 1)

	if app.ENABLE_INGAME_WIKI:
		def BindWikiWindow(self, window):
			self.wndWiki = window

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def SetAcceWindow(self, wndAcceCombine, wndAcceAbsorption):
			self.wndAcceCombine = wndAcceCombine
			self.wndAcceAbsorption = wndAcceAbsorption

		def isShowAcceWindow(self):
			if self.wndAcceCombine:
				if self.wndAcceCombine.IsShow():
					return 1
			if self.wndAcceAbsorption:
				if self.wndAcceAbsorption.IsShow():
					return 1
			return 0

	if app.ENABLE_EXCHANGE_WINDOW_RENEWAL or app.ENABLE_CHECKINOUT_UPDATE:
		def SetExchangeDlg(self, dlgExchange):
			self.dlgExchange = dlgExchange
			
		def isShowExchangeDlg(self):
			if self.dlgExchange:
				if self.dlgExchange.IsShow():
					return 1
					
			return 0

	if app.ENABLE_CHECKINOUT_UPDATE:
		def SetSafeboxWindow(self, wndSafeBox):
			self.wndSafeBox = wndSafeBox

	if app.ENABLE_PREMIUM_PRIVATE_SHOP:
		def BindPrivateShopClass(self, window):
			self.wndPrivateShop = window
			
		def BindPrivateShopSearchClass(self, window):
			self.wndPrivateShopSearch = window	

	if app.ENABLE_EXTENDED_BLEND:
		def __CanAttachThisItem(self, itemVNum, itemSlotIndex):#try
			# if constInfo.IS_PERMANANET_BLEND_ITEM(itemVNum) or constInfo.IS_PET_SEAL(itemVNum):
				# isActivated = player.GetItemMetinSocket(itemSlotIndex, 1)
				# if isActivated == 1:
					# return False

			return True

	if app.ENABLE_SAVE_LAST_WINDOW_POSITION:
		def SetLastPosition(self):
			try:
				file = open("lib/user_data/wnd/" + player.GetName() + "/"+ "inventory" + ".pos", 'r')
				line = file.read().split(",")
				pos_x, pos_y = int(line[0]), int(line[1])
				file.close()
				if pos_x > wndMgr.GetScreenWidth() or pos_y > wndMgr.GetScreenHeight():
					return
				if pos_x < 0:
					pos_x = 0
				if pos_y < 0:
					pos_y = 0
				self.SetPosition(pos_x, pos_y)
			except:
				pass
			
		def SaveLastPosition(self):
			pos_x, pos_y = self.GetGlobalPosition()
			file = open("lib/user_data/wnd/" + player.GetName() + "/"+ "inventory" + ".pos","w")
			file.write(str(pos_x)+","+str(pos_y))
			file.close()
