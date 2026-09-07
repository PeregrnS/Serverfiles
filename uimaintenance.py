import ui
import app
import localeInfo
import wndMgr

class MaintenanceWindow(ui.ThinBoard):

	def __init__(self):
		super(MaintenanceWindow, self).__init__()
		self.__Elements = dict()
		self.__LoadWindow()
		
		self.__Reason = ""
		self.__Duration = 0
		self.__RemainTime = 0
		self.__EndTime = 0
	
	def __del__(self):
		super(MaintenanceWindow, self).__del__()
	
	def __LoadWindow(self):
		self.SetSize(100, 16)
		self.SetPosition(30, 0)
		
		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetPosition(12, 8)
		textLine.SetOutline()
		textLine.Show()
		self.__Elements["text"] = textLine
		
		closeButton = ui.Button()
		closeButton.SetParent(self)
		closeButton.SetUpVisual("d:/ymir work/ui/game/maintenance/cancel_button.tga")
		closeButton.SetOverVisual("d:/ymir work/ui/game/maintenance/cancel_button_hover.tga")
		closeButton.SetDownVisual("d:/ymir work/ui/game/maintenance/cancel_button_select.tga")
		closeButton.SetPosition(16, 8)
		closeButton.SetWindowHorizontalAlignRight()
		closeButton.SetEvent(ui.__mem_func__(self.Close))
		closeButton.Show()
		
		self.__Elements["closeButton"] = closeButton
		
	def Destroy(self):
		self.Close()
		self.__Reason = ""
		self.__Duration = 0
		self.__RemainTime = 0
		self.__EndTime = 0
		self.__Elements = dict()

	def Open(self, remaining_time, cause, duration):
		self.__EndTime = app.GetLocalTime() + remaining_time
		self.__RemainTime = self.GetRemainingTime()
		if self.__RemainTime == 0:
			self.Close()
			return
			
		self.__Reason = cause
		self.__Duration = duration
		self.__RefreshRemainingTime()
		
		self.Show()

	def Close(self):
		self.Hide()

	def OnUpdate(self):
		remaining_time = self.GetRemainingTime()
		if remaining_time == self.__RemainTime:
			return
		self.__RemainTime = remaining_time

		if self.__RemainTime <= 0:
			self.Close()
			return

		self.__RefreshRemainingTime()

	def GetRemainingTime(self):
		time = int(self.__EndTime - app.GetLocalTime())
		if time <= 0:
			return 0

		return time

	def __RefreshRemainingTime(self):
		self.__Elements["text"].SetText(localeInfo.MAINTENANCE_INFORMATION.format(localeInfo.SecondToDHMS(self.__Duration), self.__Reason, localeInfo.SecondToDHMS(self.__RemainTime)))
		self.SetSize(self.__Elements["text"].GetTextSize()[0] + 30, 16)
		self.SetPosition((wndMgr.GetScreenWidth()-self.GetWidth())/2, 0)