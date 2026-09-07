import ui
import wndMgr
import localeInfo
import app
import net
import dbg
import player
import chat
import uiPickMoney
import guildstorage as guildStorageModule
import guild
from datetime import datetime
import time
import uiToolTip
import snd
import grp
from _weakref import proxy
import item
import mouseModule
import ime

DIFFERENCE_TO_UTC = 7200 #Seconds to fix servertime to UTC (example: Germany: UTC+2 -> 2*3600)

GUILD_AUTH_GS_CHECKIN = (1 << 4)
GUILD_AUTH_GS_CHECKOUT = (1 << 5)
GUILD_AUTH_GS_GOLDIN = (1 << 6)
GUILD_AUTH_GS_GOLDOUT = (1 << 7)

class MouseReflector(ui.Window):
	def __init__(self, parent):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.AddFlag("not_pick")
		self.width = self.height = 0
		self.isDown = FALSE

	def Down(self):
		self.isDown = TRUE

	def Up(self):
		self.isDown = FALSE

	def OnRender(self):

		if self.isDown:
			grp.SetColor(ui.WHITE_COLOR)
		else:
			grp.SetColor(ui.HALF_WHITE_COLOR)

		x, y = self.GetGlobalPosition()
		grp.RenderBar(x+2, y+2, self.GetWidth()-4, self.GetHeight()-4)

class CheckBox(ui.ImageBox):
	def __init__(self, parent, x, y, event, filename = "d:/ymir work/ui/public/Parameter_Slot_01.sub"):
		ui.ImageBox.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.LoadImage(filename)

		self.mouseReflector = MouseReflector(self)
		self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())

		image = ui.MakeImageBox(self, "d:/ymir work/ui/public/check_image.sub", 0, 0)
		image.AddFlag("not_pick")
		image.SetWindowHorizontalAlignCenter()
		image.SetWindowVerticalAlignCenter()
		image.Hide()
		self.Enable = TRUE
		self.image = image
		self.event = event
		self.Show()

		self.mouseReflector.UpdateRect()

	def __del__(self):
		ui.ImageBox.__del__(self)

	def SetCheck(self, flag):
		if flag:
			self.image.Show()
		else:
			self.image.Hide()
			
	def IsChecked(self):
		return self.image.IsShow()

	def Disable(self):
		self.Enable = FALSE

	def OnMouseOverIn(self):
		if not self.Enable:
			return
		self.mouseReflector.Show()

	def OnMouseOverOut(self):
		if not self.Enable:
			return
		self.mouseReflector.Hide()

	def OnMouseLeftButtonDown(self):
		if not self.Enable:
			return
		self.mouseReflector.Down()

	def OnMouseLeftButtonUp(self):
		if not self.Enable:
			return
		self.mouseReflector.Up()
		self.event()
		
class LogEntry(ui.ImageBox):
	def __init__(self):
		ui.ImageBox.__init__(self)
		self.LoadImage("guild_storage/log_line.png")
		self.Show()
		
		self.UserTextLine = ui.TextLine()
		self.UserTextLine.SetParent(self)
		self.UserTextLine.SetPosition(52, 3)
		self.UserTextLine.SetHorizontalAlignCenter()
		self.UserTextLine.SetText("")
		self.UserTextLine.Show()
		
		self.DateTextLine = ui.TextLine()
		self.DateTextLine.SetParent(self)
		self.DateTextLine.SetPosition(50+50+56, 3)
		self.DateTextLine.SetText("")
		self.DateTextLine.Show()
		
		self.ActionTextLine = ui.TextLine()
		self.ActionTextLine.SetParent(self)
		self.ActionTextLine.SetPosition(50+150+100+10, 3)
		self.ActionTextLine.SetText("")
		self.ActionTextLine.Show()
		
	
	def SetData(self, _type, name, vnum, slot, slot_new, gold, _time):
		self.DateTextLine.SetText(datetime.utcfromtimestamp(_time+(time.time()-app.GetGlobalTimeStamp())+DIFFERENCE_TO_UTC).strftime('%Y-%m-%d %H:%M:%S'))
		self.UserTextLine.SetText(name)
		desc = ''
		if _type == 0:
			item.SelectItem(vnum)
			desc += "Checkin: "+item.GetItemName()+ " to slot: "+str(slot_new)
		elif _type == 1:
			item.SelectItem(vnum)
			desc += "Checkout: "+item.GetItemName()+ " from slot: "+str(slot)
		elif _type == 2:
			item.SelectItem(vnum)
			desc += "Move: "+item.GetItemName()+ " from slot: " + str(slot) + " to slot: " + str(slot_new)
		elif _type == 3:
			desc += "Add Gold: +" + str(gold) 
		elif _type == 4:
			desc += "Remove Gold: " + str(gold)
		self.ActionTextLine.SetText(desc)
				
class LogWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.SetSize(700, 345)
		self.AddFlag("movable")
		self.SetTitleName("Logs")
		self.SetCenterPosition()
		
		self.LogEntries = []
		
		self.LoadLogs()
		self.LoadSearch()
		
		for i in range(10):
			Entry = LogEntry()
			Entry.SetParent(self.LogBG)
			Entry.SetPosition(1, 24+i*22)
			self.LogEntries.append(Entry)
		
	def Open(self):
		self.OnScroll()
		self.Show()
		
	def LoadSearch(self):
		self.SearchTxt = ui.TextLine()
		self.SearchTxt.SetParent(self)
		self.SearchTxt.SetPosition(15, 35)
		self.SearchTxt.SetText("Search user:")
		self.SearchTxt.Show()
		
		self.SlotBar = ui.SlotBar()
		self.SlotBar.SetParent(self)
		self.SlotBar.SetSize(100, 18)
		self.SlotBar.SetPosition(13, 55)
		self.SlotBar.Show()
		self.Value = ui.EditLine()
		self.Value.SetParent(self.SlotBar)
		self.Value.SetSize(100, 18)
		self.Value.SetPosition(1, 1)
		self.Value.SetMax(20)
		self.Value.SetLimitWidth(200)
		self.Value.SetMultiLine()
		self.Value.SetText("")
		self.Value.Show()
		
		self.SearchBtn = ui.Button()
		self.SearchBtn.SetParent(self)
		self.SearchBtn.SetPosition(13+100+10, 55)
		self.SearchBtn.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.SearchBtn.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.SearchBtn.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.SearchBtn.SetText("Search")
		self.SearchBtn.SetEvent(self.SearchUser)
		self.SearchBtn.Show()
		
	def SearchUser(self):
		self.OnScroll()

	def LoadLogs(self):
		self.LogBG = ui.ImageBox()
		self.LogBG.SetParent(self)
		self.LogBG.SetPosition(10, 85)
		self.LogBG.LoadImage("guild_storage/log_bg.png")
		self.LogBG.Show()
		
		self.UserHeadLine = ui.TextLine()
		self.UserHeadLine.SetParent(self)
		self.UserHeadLine.SetPosition(50, 88)
		self.UserHeadLine.SetText("User")
		self.UserHeadLine.Show()
		
		self.DateHeadLine = ui.TextLine()
		self.DateHeadLine.SetParent(self)
		self.DateHeadLine.SetPosition(50+150, 88)
		self.DateHeadLine.SetText("Date")
		self.DateHeadLine.Show()
		
		self.ActionHeadLine = ui.TextLine()
		self.ActionHeadLine.SetParent(self)
		self.ActionHeadLine.SetPosition(50+150+150+120, 88)
		self.ActionHeadLine.SetText("Action")
		self.ActionHeadLine.Show()
		
		self.ScrollBar = ui.ScrollBar()
		self.ScrollBar.SetParent(self)
		self.ScrollBar.SetPosition(665, 85)
		self.ScrollBar.SetScrollBarSize(245)
		self.ScrollBar.Show()
		self.ScrollBar.SetScrollStep(0.3)
		self.ScrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
	
	def OnScroll(self):
		for log in self.LogEntries:
			log.Hide()
		
		logs = guildStorageModule.GetLogs()
		logs.sort(key=lambda x:x[6],reverse=True)
		
		name = self.Value.GetText()
		
		count = len(logs)
		
		if count == 0:
			return
		
		scrollLineCount = max(0, count - 10)
		startIdx = int(scrollLineCount * self.ScrollBar.GetPos())
		
		found = 0
		y = 0
		while found < 10:
			i = startIdx+y
			
			if count <= i:
				break
				
			if name == "" or logs[i][1].find(name) != -1:
				self.LogEntries[found].SetData(logs[i][0],logs[i][1],logs[i][2],logs[i][3],logs[i][4],logs[i][5],logs[i][6])
				self.LogEntries[found].Show()
				found += 1
				
			y += 1

class MemberWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.SetSize(445, 425)
		self.AddFlag("movable")
		self.SetTitleName("Members")
		self.SetCenterPosition()
		
		self.MemberImgs = []
		self.MemberNames = []
		self.CheckBoxesCheckIn = []
		self.CheckBoxesCheckOut = []
		self.CheckBoxesPayIn = []
		self.CheckBoxesPayOut = []
		
		self.LoadTextLines()
		
	def Open(self):
		self.LoadGrades()
		self.Show()
		
	def LoadGrades(self):
		for mem in self.MemberImgs:
			mem.Hide()
			del mem
		for mem in self.MemberNames:
			mem.Hide()
			del mem
		for mem in self.CheckBoxesCheckIn:
			mem.Hide()
			del mem
		for mem in self.CheckBoxesCheckOut:
			mem.Hide()
			del mem
		for mem in self.CheckBoxesPayIn:
			mem.Hide()
			del mem
		for mem in self.CheckBoxesPayOut:
			mem.Hide()
			del mem
		
		self.MemberImgs = []
		self.MemberNames = []
		self.CheckBoxesCheckIn = []
		self.CheckBoxesCheckOut = []
		self.CheckBoxesPayIn = []
		self.CheckBoxesPayOut = []
		
		for i in range(15):
			name, authority = guild.GetGradeData(i+1)
			
			image = ui.ImageBox()
			image.SetParent(self)
			image.SetPosition(20, 55 + (24*i))
			image.LoadImage("d:/ymir work/ui/public/Parameter_Slot_03.sub")
			image.Show()
			text = ui.TextLine()
			text.SetParent(image)
			text.SetText(name)
			text.SetPosition(0, 2)
			text.SetHorizontalAlignCenter()
			text.SetWindowHorizontalAlignCenter()
			text.Show()
			
			self.MemberImgs.append(image)
			self.MemberNames.append(text)
			
			event = lambda argSelf=proxy(self), argIndex=i, argAuthority=GUILD_AUTH_GS_CHECKIN: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			checkbox = CheckBox(self, 35+100, 55 + (24*i), event, "d:/ymir work/ui/public/Parameter_Slot_02.sub")
			self.CheckBoxesCheckIn.append(checkbox)
			checkbox.SetCheck(authority & GUILD_AUTH_GS_CHECKIN)

			event = lambda argSelf=proxy(self), argIndex=i, argAuthority=GUILD_AUTH_GS_CHECKOUT: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			checkbox = CheckBox(self, 35+100+75, 55 + (24*i), event, "d:/ymir work/ui/public/Parameter_Slot_02.sub")
			self.CheckBoxesCheckOut.append(checkbox)
			checkbox.SetCheck(authority & GUILD_AUTH_GS_CHECKOUT)

			event = lambda argSelf=proxy(self), argIndex=i, argAuthority=GUILD_AUTH_GS_GOLDIN: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			checkbox = CheckBox(self, 35+100+75+75, 55 + (24*i), event, "d:/ymir work/ui/public/Parameter_Slot_02.sub")
			self.CheckBoxesPayIn.append(checkbox)
			checkbox.SetCheck(authority & GUILD_AUTH_GS_GOLDIN)
			
			event = lambda argSelf=proxy(self), argIndex=i, argAuthority=GUILD_AUTH_GS_GOLDOUT: apply(argSelf.OnCheckAuthority, (argIndex,argAuthority))
			checkbox = CheckBox(self, 35+100+75+75+75, 55 + (24*i), event, "d:/ymir work/ui/public/Parameter_Slot_02.sub")
			self.CheckBoxesPayOut.append(checkbox)
			checkbox.SetCheck(authority & GUILD_AUTH_GS_GOLDOUT)
			
	def LoadTextLines(self):
		self.TextLines = []
		for i in range(5):
			text = ui.TextLine()
			text.SetParent(self)
			text.Show()
			
			self.TextLines.append(text)
			
		self.TextLines[0].SetPosition(45, 35)
		self.TextLines[1].SetPosition(45+100, 35)
		self.TextLines[2].SetPosition(45+100+75, 35)
		self.TextLines[3].SetPosition(45+100+75+75, 35)
		self.TextLines[4].SetPosition(45+100+75+75+75, 35)
		self.TextLines[0].SetText("Name")
		self.TextLines[1].SetText("Check in")
		self.TextLines[2].SetText("Check out")
		self.TextLines[3].SetText("Money in")
		self.TextLines[4].SetText("Money out")
			
	def OnCheckAuthority(self, argIndex, argAuthority):
		authority = FALSE
		if argAuthority == GUILD_AUTH_GS_CHECKIN:
			if self.CheckBoxesCheckIn[argIndex].IsChecked():
				self.CheckBoxesCheckIn[argIndex].SetCheck(FALSE)
			else:
				self.CheckBoxesCheckIn[argIndex].SetCheck(TRUE)
				authority = TRUE
		elif argAuthority == GUILD_AUTH_GS_CHECKOUT:
			if self.CheckBoxesCheckOut[argIndex].IsChecked():
				self.CheckBoxesCheckOut[argIndex].SetCheck(FALSE)
			else:
				self.CheckBoxesCheckOut[argIndex].SetCheck(TRUE)
				authority = TRUE
		elif argAuthority == GUILD_AUTH_GS_GOLDIN:
			if self.CheckBoxesPayIn[argIndex].IsChecked():
				self.CheckBoxesPayIn[argIndex].SetCheck(FALSE)
			else:
				self.CheckBoxesPayIn[argIndex].SetCheck(TRUE)
				authority = TRUE
		elif argAuthority == GUILD_AUTH_GS_GOLDOUT:
			if self.CheckBoxesPayOut[argIndex].IsChecked():
				self.CheckBoxesPayOut[argIndex].SetCheck(FALSE)
			else:
				self.CheckBoxesPayOut[argIndex].SetCheck(TRUE)
				authority = TRUE
		
		name, authorityCur = guild.GetGradeData(argIndex+1)
		net.SendGuildChangeGradeAuthorityPacket(argIndex+1, authorityCur ^ argAuthority)

class MoneyWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.SetSize(170, 90)
		self.SetPosition(100, 100)
		self.AddFlag("movable")
		self.AddFlag("float")
		
		self.MoneySlot = ui.ImageBox()
		self.MoneySlot.SetParent(self)
		self.MoneySlot.SetPosition(20, 34)
		self.MoneySlot.LoadImage("d:/ymir work/ui/public/Parameter_Slot_04.sub")
		self.MoneySlot.Show()
		
		self.pickValueEditLine = ui.EditLine()
		self.pickValueEditLine.SetParent(self.MoneySlot)
		self.pickValueEditLine.SetPosition(3, 2)
		self.pickValueEditLine.SetSize(60, 18)
		self.pickValueEditLine.SetNumberMode()
		self.pickValueEditLine.SetMax(11)
		self.pickValueEditLine.Show()
		
		self.MoneyInBtn = ui.Button()
		self.MoneyInBtn.SetParent(self)
		self.MoneyInBtn.SetPosition(170/2 - 61 - 5, 58)
		self.MoneyInBtn.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		self.MoneyInBtn.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		self.MoneyInBtn.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		self.MoneyInBtn.SetText("Money In")
		self.MoneyInBtn.SetEvent(ui.__mem_func__(self.OnMoneyButton), 1)
		self.MoneyInBtn.Show()
		
		self.MoneyOutBtn = ui.Button()
		self.MoneyOutBtn.SetParent(self)
		self.MoneyOutBtn.SetPosition(170/2 + 5, 58)
		self.MoneyOutBtn.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		self.MoneyOutBtn.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		self.MoneyOutBtn.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		self.MoneyOutBtn.SetText("Money Out")
		self.MoneyOutBtn.SetEvent(ui.__mem_func__(self.OnMoneyButton), 2)
		self.MoneyOutBtn.Show()
		
	def OnMoneyButton(self, index):
		money = self.pickValueEditLine.GetText()
		if money == "":
			self.pickValueEditLine.SetText("1")
			return
			
		if index == 1:
			guildStorageModule.CheckInGold(int(money))
		else:
			guildStorageModule.CheckOutGold(int(money))
			
		self.pickValueEditLine.SetText("1")
		self.Close()
		
	def Close(self):
		self.Hide()
		
	def Open(self, unitValue=1):
		if localeInfo.IsYMIR() or localeInfo.IsCHEONMA() or localeInfo.IsHONGKONG():
			unitValue = ""

		width = self.GetWidth()
		(mouseX, mouseY) = wndMgr.GetMousePosition()

		if mouseX + width/2 > wndMgr.GetScreenWidth():
			xPos = wndMgr.GetScreenWidth() - width
		elif mouseX - width/2 < 0:
			xPos = 0
		else:
			xPos = mouseX - width/2

		self.SetPosition(xPos, mouseY - self.GetHeight() - 20)

		self.pickValueEditLine.SetText(str(unitValue))
		self.pickValueEditLine.SetFocus()

		ime.SetCursorPosition(1)

		self.unitValue = unitValue
		self.Show()
		self.SetTop()

class MainWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.SetSize(500, 356)
		self.AddFlag("movable")
		self.SetTitleName("Guildstorage")
		self.SetCenterPosition()
		
		self.wndPickMoney = MoneyWindow()
		
		self.page = 0
		
		self.toolTip = uiToolTip.ItemToolTip()
		
		self.wndMembers = MemberWindow()
		self.wndLogs = LogWindow()
		
		self.LoadGridWindows()
		self.LoadStorageBtns()
		self.LoadButtons()
		self.SetCloseEvent(ui.__mem_func__(self.Close))
	
	def Open(self, mode): # Gets Called from game
		for i in range(120):
			self.GridTable.ClearSlot(i)
		
		self.RefreshSlots()

		if mode == 0:
			self.MemberBtn.Hide()
			self.LogsBtn.Hide()
		else:
			self.MemberBtn.Show()
			self.LogsBtn.Show()
		
		self.SetTop()
		self.Show()
	
	def Close(self):
		guildStorageModule.Close()
	
	def LoadGridWindows(self):
		self.GridTable = ui.GridSlotWindow()
		self.GridTable.SetParent(self)
		self.GridTable.SetPosition(10, 32)
		self.GridTable.SetSize(32*15, 32*8)
		self.GridTable.ArrangeSlot(0, 15, 8, 32, 32, 0, 0)
		self.GridTable.Show()
		self.GridTable.SetSlotBaseImage("d:/ymir work/ui/public/slot_base.sub", 1.0, 1.0, 1.0, 1.0)
		self.GridTable.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.GridTable.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.GridTable.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		self.GridTable.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		self.GridTable.SetUnselectItemSlotEvent(ui.__mem_func__(self.UnselectItemSlot))
		self.GridTable.SetUsableItem(True)
		
	def LoadStorageBtns(self):
		self.StorageBtns = []
		
		for i in range(10):
			StorageBtn = ui.RadioButton()
			StorageBtn.SetParent(self)
			StorageBtn.SetPosition(8, 63)
			StorageBtn.SetWindowVerticalAlignBottom()
			StorageBtn.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
			StorageBtn.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
			StorageBtn.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
			StorageBtn.Hide()
		
			self.StorageBtns.append(StorageBtn)
			self.StorageBtns[i].SetPosition(26 + (45*i), 60)
			self.StorageBtns[i].Show()
			self.StorageBtns[i].SetText(str(i+1))
			self.StorageBtns[i].SetEvent(self.SelectGuildPage, i)
			
		self.StorageBtns[0].Down()
		self.RefreshSlots()
		
	def OpenMembers(self):
		self.wndMembers.Open()		
	
	def OpenLogs(self):
		self.wndLogs.Open()
			
	def LoadButtons(self):
		self.MemberBtn = ui.Button()
		self.MemberBtn.SetParent(self)
		self.MemberBtn.SetPosition(26, 34)
		self.MemberBtn.SetWindowVerticalAlignBottom()
		self.MemberBtn.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		self.MemberBtn.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		self.MemberBtn.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		self.MemberBtn.SetText("Verwaltung")
		self.MemberBtn.SetEvent(self.OpenMembers)
		self.MemberBtn.Show()
		
		self.LogsBtn = ui.Button()
		self.LogsBtn.SetParent(self)
		self.LogsBtn.SetPosition(26+90, 34)
		self.LogsBtn.SetWindowVerticalAlignBottom()
		self.LogsBtn.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
		self.LogsBtn.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
		self.LogsBtn.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
		self.LogsBtn.SetText("Logs")
		self.LogsBtn.SetEvent(self.OpenLogs)
		self.LogsBtn.Show()
		
		self.MoneyBtn = ui.Button()
		self.MoneyBtn.SetParent(self)
		self.MoneyBtn.SetPosition(26+ (45*7), 34)
		self.MoneyBtn.SetWindowVerticalAlignBottom()
		self.MoneyBtn.SetUpVisual("d:/ymir work/ui/public/parameter_slot_05.sub")
		self.MoneyBtn.SetOverVisual("d:/ymir work/ui/public/parameter_slot_05.sub")
		self.MoneyBtn.SetDownVisual("d:/ymir work/ui/public/parameter_slot_05.sub")
		self.MoneyBtn.SetEvent(self.PickMoney)
		self.MoneyBtn.Show()
		
		self.MoneyIcon = ui.ImageBox()
		self.MoneyIcon.SetParent(self.MoneyBtn)
		self.MoneyIcon.SetPosition(-18, 2)
		self.MoneyIcon.LoadImage("d:/ymir work/ui/game/windows/money_icon.sub")
		self.MoneyIcon.Show()
		
		self.MoneyText = ui.TextLine()
		self.MoneyText.SetParent(self.MoneyBtn)
		self.MoneyText.SetPosition(0, 2)
		self.MoneyText.SetHorizontalAlignCenter()
		self.MoneyText.SetWindowHorizontalAlignCenter()
		self.MoneyText.SetText("123")
		self.MoneyText.Show()
	
	def SetMoney(self,money):
		self.MoneyText.SetText(localeInfo.NumberToMoneyString(money))
	
	def PickMoney(self):
		curMoney = player.GetElk()

		if curMoney <= 0:
			return
	
		self.wndPickMoney.SetTitleName(localeInfo.PICK_MONEY_TITLE)
		self.wndPickMoney.Open()
		self.wndPickMoney.Show()
		
	def OnPickMoney(self, money):
		pass
			
	def SelectGuildPage(self, page):
		for btn in self.StorageBtns:
			btn.SetUp()
			
		self.StorageBtns[page].Down()
		
		self.page = page
		self.RefreshSlots()
		
	def RefreshSlots(self):
		for i in range(121):
			self.GridTable.ClearSlot(i)
			item = guildStorageModule.GetItem(i+(15*8*self.page))
			if item[0] != 0:
				self.GridTable.SetItemSlot(i, item[0], item[1] if item[1] != 1 else 0)
		
		self.GridTable.RefreshSlot()
		
	def OverInItem(self, index):
		self.toolTip.ClearToolTip()
		item = guildStorageModule.GetItem(index+(15*8*self.page))
		self.toolTip.AddRefineItemData(item[0], [item[2][0], item[2][1], item[2][2]], item[3]) #SPlayerItem
		
	def OverOutItem(self):
		self.toolTip.Hide()
	
	def SelectEmptySlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				guildStorageModule.CheckInItem(attachedSlotPos, attachedSlotType, selectedSlotPos+(15*8*self.page))
			elif player.SLOT_TYPE_INVENTORY != attachedSlotType and player.SLOT_TYPE_MALL != attachedSlotType and player.SLOT_TYPE_SAFEBOX != attachedSlotType and player.SLOT_TYPE_SHOP != attachedSlotType:
				guildStorageModule.MoveItem(attachedSlotPos, selectedSlotPos+(15*8*self.page))

		mouseModule.mouseController.DeattachObject()

	def UnselectItemSlot(self, selectedSlotPos):
		self.RefreshSlots()

	def SelectItemSlot(self, selectedSlotPos):
		item = guildStorageModule.GetItem(selectedSlotPos+(15*8*self.page))
		curCursorNum = app.GetCursor()
		selectedItemID = item[0]
		itemCount = item[1]

		mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_GUILDSTORAGE, selectedSlotPos+(15*8*self.page), selectedItemID, itemCount)
		snd.PlaySound("sound/ui/pick.wav")
	
	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def OnPressExitKey(self):
		self.Close()
		return TRUE
