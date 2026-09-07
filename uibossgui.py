import app
import grp
import net
import ui
import uiScriptLocale

class DailyBossGui(ui.ScriptWindow):
	NEGATIVE_COLOR = grp.GenerateColor(1.0, 0, 0, 1.0)
	POSITIVE_COLOR = grp.GenerateColor(0.2666, 1.0, 0.2666, 1.0)
	INFO = (uiScriptLocale.WORLD_BOSS_INFORMATIONS_0, uiScriptLocale.WORLD_BOSS_INFORMATIONS_1, uiScriptLocale.WORLD_BOSS_INFORMATIONS_2)

	def __init__(self):
		ui.ScriptWindow.__init__(self)	
		self.WorldBosses = []
		self.Bosses = []
		self.Informations = []
		self.cooldownTime = None
		self.localTime = None
		self.__LoadWindow()
	
	def __del__(self):
		ui.ScriptWindow.__del__(self)
	
	def Destroy(self):
		self.board = None
		self.WorldBosses = None
		self.Bosses = None
		self.Informations = None
		self.cooldownTime = None
		self.localTime = None
		self.teleportBtn = None

	def __LoadWindow(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/world_boss.py")
		except:
			import exception
			exception.Abort("GemShopWindow.__LoadWindow.LoadObject")

		self.board = self.GetChild("board")
		self.teleportBtn = self.GetChild("teleportBtn")

		for i in xrange(2):
			self.Bosses.append(self.GetChild("Boss%d" % (i)))

		for i in xrange(2):
			self.WorldBosses.append(self.GetChild("WorldBoss%d" % (i)))
		
		for i in xrange(3):
			self.Informations.append(self.GetChild("Informations%d" % (i)))

		self.SetWorldBossText(0, "12:00", 0)
		self.SetWorldBossText(1, "20:00", 0)

		# self.SetWorldBossText(2, "23:30", 0)
		# self.SetWorldBossText(3, "17:30", 0)

		# self.SetWorldBossText(4, "20:30", 0)
		# self.SetWorldBossText(5, "00:30", 0)

		# self.SetWorldBossText(6, "18:30", 0)
		# self.SetWorldBossText(7, "21:30", 0)
		# self.SetWorldBossText(8, "01:30", 0)

		# self.SetWorldBossText(9, "22:30", 0)

		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.teleportBtn.SetEvent(self.Teleport)

	def SetWorldBossText(self, index, text, color):
		self.WorldBosses[index].SetText("%s" % str(text))

		if color == 0:
			self.WorldBosses[index].SetPackedFontColor(self.NEGATIVE_COLOR)
			return

		self.WorldBosses[index].SetPackedFontColor(self.POSITIVE_COLOR)
	
	def SetBossesText(self, index, color):		
		if color == 0:
			self.Bosses[index].SetPackedFontColor(self.NEGATIVE_COLOR)
			return

		self.Bosses[index].SetPackedFontColor(self.POSITIVE_COLOR)

	def SetInformationsText(self, index, text):
		self.Informations[index].SetText(self.INFO[index] % str(text))
		if index == 1:
			self.cooldownTime = str(text)
		elif index == 2:
			self.localTime = str(text)

	def Open(self):
		self.Show()

	def TimeToString(self, timeLeft, add = False):
		timeLeft = sum(x * int(t) for x, t in zip([3600, 60, 1], timeLeft.split(":"))) 
		if add:
			timeLeft = int(timeLeft + app.GetTime())
		else:
			timeLeft = int(timeLeft - app.GetTime())

		if timeLeft > 0:
			hours = int(timeLeft / 3600)
			minutes = int((timeLeft - hours * 3600) / 60)
			seconds = int(timeLeft - hours * 3600 - minutes * 60)

			timeLeftString = ""

			if hours < 10:
				timeLeftString += "0"

			timeLeftString += str(hours) + ":"

			if minutes < 10:
				timeLeftString += "0"

			timeLeftString += str(minutes) + ":"

			if seconds < 10:
				timeLeftString += "0"

			return timeLeftString + str(seconds)

		return "00:00:00"
		
	def OnUpdate(self):
		self.Informations[1].SetText(self.INFO[1] % self.TimeToString(self.cooldownTime))
		self.Informations[2].SetText(self.INFO[2] % self.TimeToString(self.localTime, True))

	def Close(self):
		self.Hide()

	def Teleport(self):
		# pass
		net.SendChatPacket("/timer_warp 0")
		self.Close()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True

