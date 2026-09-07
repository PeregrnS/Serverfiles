import wndMgr
import ui
import ime
import localeInfo
import app

class PickMoneyDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.unitValue = 1
		self.maxValue = 0
		self.eventAccept = 0

		if app.ENABLE_SPLIT_BY_COUNT:
			self.isSplitPick = False
			self.v_SplitPickFlag = False

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/PickMoneyDialog.py")
		except:
			import exception
			exception.Abort("MoneyDialog.LoadDialog.LoadScript")

		try:
			self.board = self.GetChild("board")
			self.maxValueTextLine = self.GetChild("max_value")
			self.pickValueEditLine = self.GetChild("money_value")
			self.acceptButton = self.GetChild("accept_button")
			self.cancelButton = self.GetChild("cancel_button")
		except:
			import exception
			exception.Abort("MoneyDialog.LoadDialog.BindObject")

		self.pickValueEditLine.SetReturnEvent(ui.__mem_func__(self.OnAccept))
		self.pickValueEditLine.SetEscapeEvent(ui.__mem_func__(self.Close))
		self.acceptButton.SetEvent(ui.__mem_func__(self.OnAccept))
		self.cancelButton.SetEvent(ui.__mem_func__(self.Close))
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		if app.ENABLE_SPLIT_BY_COUNT:
			self.base_t = (self.GetWidth(), self.GetHeight())

	def Destroy(self):
		self.ClearDictionary()
		self.eventAccept = 0
		self.maxValue = 0
		self.pickValueEditLine = 0
		self.acceptButton = 0
		self.cancelButton = 0
		self.board = None
		if app.ENABLE_SPLIT_BY_COUNT:
			self.isSplitPick = False
			self.v_SplitPickFlag = False

	def SetTitleName(self, text):
		self.board.SetTitleName(text)

	def SetAcceptEvent(self, event):
		self.eventAccept = event

	def SetMax(self, max):
		self.pickValueEditLine.SetMax(max)

	if app.ENABLE_SPLIT_BY_COUNT:
		def IsSplitPick(self, isSplit = False):
			self.isSplitPick = isSplit

		def SplitPickFlag(self, checkType, autoFlag):
			self.v_SplitPickFlag = autoFlag
		
		def GetSplitPickFlag(self):
			return self.v_SplitPickFlag

	def Open(self, maxValue, unitValue=1):

		if localeInfo.IsYMIR() or localeInfo.IsCHEONMA() or localeInfo.IsHONGKONG():
			unitValue = ""

		if app.ENABLE_SPLIT_BY_COUNT:
			width = self.base_t[0]
		else:
			width = self.GetWidth()
		(mouseX, mouseY) = wndMgr.GetMousePosition()

		if mouseX + width/2 > wndMgr.GetScreenWidth():
			xPos = wndMgr.GetScreenWidth() - width
		elif mouseX - width/2 < 0:
			xPos = 0
		else:
			xPos = mouseX - width/2

		if app.ENABLE_SPLIT_BY_COUNT:
			self.SetSize(self.base_t[0], self.base_t[1])
			if self.isSplitPick is True:
				self.SetSize(width, self.GetHeight() + 15)
				self.board.SetSize(width, self.GetHeight() + 15)
				self.checkBox = ui.RefineCheckBox()
				self.checkBox.SetParent(self)
				self.checkBox.SetPosition(-3, 15)
				self.checkBox.SetWindowHorizontalAlignCenter()
				self.checkBox.SetWindowVerticalAlignBottom()
				self.checkBox.SetCheckStatus(self.v_SplitPickFlag)
				self.checkBox.SetEvent(ui.__mem_func__(self.SplitPickFlag), "ON_CHECK", True)
				self.checkBox.SetEvent(ui.__mem_func__(self.SplitPickFlag), "ON_UNCKECK", False)
				self.checkBox.SetTextInfo("Split All")
				self.checkBox.Show()

		self.SetPosition(xPos, mouseY - self.GetHeight() - 20)

		if localeInfo.IsARABIC():
			self.maxValueTextLine.SetText("/" + str(maxValue))
		else:
			self.maxValueTextLine.SetText(" / " + str(maxValue))

		self.pickValueEditLine.SetText(str(unitValue))
		self.pickValueEditLine.SetFocus()

		ime.SetCursorPosition(1)

		self.unitValue = unitValue
		self.maxValue = maxValue
		self.Show()
		self.SetTop()

	def Close(self):
		if app.ENABLE_SPLIT_BY_COUNT:
			self.isSplitPick = False
			self.v_SplitPickFlag = False

		self.pickValueEditLine.KillFocus()
		self.Hide()

	def OnAccept(self):

		text = self.pickValueEditLine.GetText()
		text = text.replace("k", "000")

		if len(text) > 0 and text.isdigit():

			money = int(text)
			money = min(money, self.maxValue)

			if money > 0:
				if self.eventAccept:
					self.eventAccept(money)

		self.Close()
