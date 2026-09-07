import ui
import wndMgr
import dbg
import app
import net
import event
import _weakref
import localeInfo
import uiScriptLocale
import time

class SelectEmpireWindow(ui.ScriptWindow):

	def __init__(self,stream):
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, self)

		self.stream=stream
		self.empireID=app.GetRandom(1, 2)
		self.descIndex=0
		self.empireArea = {}
		self.empireFlag = {}
		self.empireAreaFlag = {}
		self.empireAreaCurAlpha = { net.EMPIRE_A:0.0, net.EMPIRE_B:0.0 }
		self.empireAreaDestAlpha = { net.EMPIRE_A:0.0, net.EMPIRE_B:0.0 }
		self.empireAreaFlagCurAlpha = { net.EMPIRE_A:0.0, net.EMPIRE_B:0.0 }
		self.empireAreaFlagDestAlpha = { net.EMPIRE_A:0.0, net.EMPIRE_B:0.0 }
		self.empireFlagCurAlpha = { net.EMPIRE_A:0.0, net.EMPIRE_B:0.0 }
		self.empireFlagDestAlpha = { net.EMPIRE_A:0.0, net.EMPIRE_B:0.0 }
		self.Open()
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, 0)

	def Close(self):
		self.ClearDictionary()
		self.empireArea = None

		self.KillFocus()
		self.Hide()

		app.HideCursor()
		event.Destroy()

	def Open(self):
		self.SetWindowName("SelectEmpireWindow")
		self.Show()

		if not self.__LoadScript("uiscript/SelectEmpireWindow.py"):
			dbg.TraceError("SelectEmpireWindow.Open - __LoadScript Error")
			return

		# self.SelectFlag(self.empireID)
		app.ShowCursor()

	def OnOverInEmpire(self, arg):
		self.empireAreaDestAlpha[arg] = 1.0

	def OnOverOutEmpire(self, arg):
		if arg != self.empireID:
			self.empireAreaDestAlpha[arg] = 0.0

	def OnSelectEmpire(self, arg):
		for key in self.empireArea.keys():
			self.empireAreaDestAlpha[key] = 0.0
			self.empireAreaFlagDestAlpha[key] = 0.0
			self.empireFlagDestAlpha[key] = 0.0
		self.empireAreaDestAlpha[arg] = 1.0
		self.empireAreaFlagDestAlpha[arg] = 1.0
		self.empireFlagDestAlpha[arg] = 1.0
		self.empireID = arg

		event.ClearEventSet(self.descIndex)


	def __LoadScript(self, fileName):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("SelectEmpireWindow.__LoadScript.LoadObject")

		try:
			GetObject=self.GetChild
			
			self.board = {
				"confirm" : [GetObject("select_%d" % (i)) for i in xrange(3)],
			}
		except:
			import exception
			exception.Abort("SelectEmpireWindow.__LoadScript.BindObject")

		self.board["confirm"][0].SetEvent(ui.__mem_func__(self.SelectFlag), 0)
		self.board["confirm"][1].SetEvent(ui.__mem_func__(self.SelectFlag), 1)
		self.board["confirm"][2].SetEvent(ui.__mem_func__(self.SelectFlag), 2)
		return 1
		
	def SelectFlag(self, flag):
		self.empireID = flag+1

		net.SendSelectEmpirePacket(self.empireID)
		self.stream.SetCreateCharacterPhase()

	def OnUpdate(self):
		self.__UpdateAlpha(self.empireArea, self.empireAreaCurAlpha, self.empireAreaDestAlpha)
		self.__UpdateAlpha(self.empireAreaFlag, self.empireAreaFlagCurAlpha, self.empireAreaFlagDestAlpha)
		self.__UpdateAlpha(self.empireFlag, self.empireFlagCurAlpha, self.empireFlagDestAlpha)

	def __UpdateAlpha(self, dict, curAlphaDict, destAlphaDict):
		for key, img in dict.items():

			curAlpha = curAlphaDict[key]
			destAlpha = destAlphaDict[key]

			if abs(destAlpha - curAlpha) / 10 > 0.0001:
				curAlpha += (destAlpha - curAlpha) / 7
			else:
				curAlpha = destAlpha

			curAlphaDict[key] = curAlpha
			img.SetAlpha(curAlpha)

	def ClickExitButton(self):
		self.stream.SetLoginPhase()

	def OnPressExitKey(self):
		self.ClickExitButton()

	def OnPressEscapeKey(self):
		self.ClickExitButton()
		return True

class ReselectEmpireWindow(SelectEmpireWindow):
	def ClickSelectButton(self):
		net.SendSelectEmpirePacket(self.empireID)
		self.stream.SetCreateCharacterPhase()

	def ClickExitButton(self):
		self.stream.SetSelectCharacterPhase()
