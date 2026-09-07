import app
import ui
import player
import wndMgr
import localeInfo
import net
import constInfo
import uiInventory
import snd

TOTAL_SELL_VALUE = 0
TOTAL_ITEM_FRAG = 0
TOTAL_STONE_FRAG = 0
SELECTED_ITEMS = []

def GetSelectedItemsLength():
	return len(SELECTED_ITEMS)
	
def GetSelectedItemsValue(idx):
	return SELECTED_ITEMS[idx]

def AddToSelectedItems(itemSlotPos):
	SELECTED_ITEMS.append(itemSlotPos)

def RemoveFromSelectedItems(itemSlotPos):
	for i in range(len(SELECTED_ITEMS)):
		if SELECTED_ITEMS[i-1] == itemSlotPos:
			del SELECTED_ITEMS[i-1]

def IsItemAlreadyInList(itemSlotPos):
	for i in range (len(SELECTED_ITEMS)):
		if SELECTED_ITEMS[i-1] == itemSlotPos:
			return True
	return False

def ClearSelectedItems():
	del SELECTED_ITEMS[:]
	TOTAL_SELL_VALUE = 0
	TOTAL_ITEM_FRAG = 0
	TOTAL_STONE_FRAG = 0

class ItemManager(ui.BoardWithTitleBar):
	type = 1
	def __init__(self):
		self.interface = None
		ui.BoardWithTitleBar.__init__(self)
		self.LoadBuildshop()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def LoadBuildshop(self):
		self.SetTitleName(localeInfo.ITEM_MANAGER_COUNT_INFO % (str(len(SELECTED_ITEMS))))
		self.SetSize(180,160)
		self.SetPosition(wndMgr.GetScreenWidth() - 175 - 190, wndMgr.GetScreenHeight() - 37 - 575)
		self.AddFlag("movable")
		self.SetCloseEvent(self.Close)
		
		self.img = ui.ImageBox()
		self.img.SetParent(self)
		self.img.LoadImage("d:/ymir work/ui/game/windows/money_icon.sub")
		self.img.SetPosition(13, 40)
		self.img.AddFlag("not_pick")
		self.img.Show()
		
		self.slotbar = ui.SlotBar()
		self.slotbar.SetParent(self)
		self.slotbar.SetSize(130, 20)
		self.slotbar.SetPosition(35, 38)
		self.slotbar.Show()
		
		self.text = ui.TextLine()
		self.text.SetParent(self.slotbar)
		self.text.SetPosition(127, 3)
		self.text.SetMultiLine()
		self.text.SetHorizontalAlignRight()
		self.text.SetText("0")
		self.text.Show()

		self.sell = ui.RadioButton()
		self.sell.SetParent(self)
		self.sell.SetPosition(13+20, 70)
		self.sell.SetUpVisual("d:/ymir work/ui/game/itemmanager/button_0.tga")
		self.sell.SetOverVisual("d:/ymir work/ui/game/itemmanager/button_0.tga")
		self.sell.SetDownVisual("d:/ymir work/ui/game/itemmanager/button_1.tga")
		self.sell.SetText(localeInfo.ITEM_MANAGER_SELL)
		self.sell.SetEvent(self.sellType)
		self.sell.Show()
		
		self.destroy = ui.RadioButton()
		self.destroy.SetParent(self)
		self.destroy.SetPosition(13+20, 70+25)
		self.destroy.SetUpVisual("d:/ymir work/ui/game/itemmanager/button_0.tga")
		self.destroy.SetOverVisual("d:/ymir work/ui/game/itemmanager/button_0.tga")
		self.destroy.SetDownVisual("d:/ymir work/ui/game/itemmanager/button_1.tga")
		self.destroy.SetText(localeInfo.ITEM_MANAGER_DESTROY)
		self.destroy.SetEvent(self.destroyType)
		self.destroy.Show()

		self.validate = ui.Button()
		self.validate.SetParent(self)
		self.validate.SetPosition(65, 95+32)
		self.validate.SetUpVisual("d:/ymir work/ui/public/acceptbutton00.sub")
		self.validate.SetOverVisual("d:/ymir work/ui/public/acceptbutton01.sub")
		self.validate.SetDownVisual("d:/ymir work/ui/public/acceptbutton02.sub")
		self.validate.SetEvent(self.RequestHatching)
		self.validate.Show()

		self.sell.Down()
		self.destroy.SetUp()

	def BindInterface(self, interface):
		self.interface = interface

	def RequestHatching(self):
		if self.type == 1:
			self.sellAll()
		elif self.type == 2:
			self.destroyAll()
		self.Close()

	def destroyAll(self):
		for i in range (int(GetSelectedItemsLength())):
			itemPos = GetSelectedItemsValue(i)
			net.SendItemDestroyPacket(itemPos)

		snd.PlaySound("sound/ui/jaeryun_fail.wav")

	def sellAll(self):
		for i in range (int(GetSelectedItemsLength())):
			itemPos = GetSelectedItemsValue(i)
			net.SendItemSellPacket(itemPos)
		
		snd.PlaySound("sound/ui/money.wav")
	
	def ShowResult(self):
		if self.type == 1:
			money = localeInfo.NumberToMoneyString(TOTAL_SELL_VALUE)
			self.text.SetText("%s" % money)
		else:
			self.text.SetText(localeInfo.ITEM_MANAGER_COUNT_SHARDS % str(TOTAL_ITEM_FRAG*12))
	
	def sellType(self):
		self.type = 1
		constInfo.ITEM_MANAGER_TYPE = 1
		self.img.LoadImage("d:/ymir work/ui/game/windows/money_icon.sub")
		money = localeInfo.NumberToMoneyString(TOTAL_SELL_VALUE)
		self.text.SetText("%s" % money)
		self.SetSize(180,160)
		self.validate.SetPosition(65, 95+32)
		self.destroy.SetPosition(13+20, 70+25)
		self.sell.SetPosition(13+20, 70)
		self.refreshType()

	def destroyType(self):
		self.type = 2
		constInfo.ITEM_MANAGER_TYPE = 2
		self.img.LoadImage("d:/ymir work/ui/game/itemmanager/shard.tga")
		self.text.SetText(localeInfo.ITEM_MANAGER_COUNT_SHARDS % str(TOTAL_ITEM_FRAG*12))
		self.refreshType()

	def refreshType(self):
		if self.type == 1:
			self.sell.Down()
			self.destroy.SetUp()
		else:
			self.destroy.Down()
			self.sell.SetUp()
		
	def OnUpdate(self):
		self.ShowResult()
		self.SetTitleName(localeInfo.ITEM_MANAGER_COUNT_INFO % (str(len(SELECTED_ITEMS))))

	def Open(self):
		self.Show()
		constInfo.ITEM_REMOVE_WINDOW_STATUS = 1

	def Close(self):
		global TOTAL_SELL_VALUE
		global TOTAL_ITEM_FRAG
		global TOTAL_STONE_FRAG
		global ClearSelectedItems
		ClearSelectedItems()
		TOTAL_SELL_VALUE = 0
		TOTAL_ITEM_FRAG = 0
		TOTAL_STONE_FRAG = 0
		constInfo.ITEM_REMOVE_WINDOW_STATUS = 0
		constInfo.ITEM_MANAGER_TYPE = 0
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True
