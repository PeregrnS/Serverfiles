import app
import ui
import player
import net
import chr

class GameButtonWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow("UIScript/gamewindow.py")

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self, filename):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, filename)
		except Exception, msg:
			import dbg
			dbg.TraceError("GameButtonWindow.LoadScript - %s" % (msg))
			app.Abort()
			return False

		try:
			self.gameButtonDict={
				"STATUS" : self.GetChild("StatusPlusButton"),
				"SKILL" : self.GetChild("SkillPlusButton"),
				"QUEST" : self.GetChild("QuestButton"),
				"HELP" : self.GetChild("HelpButton"),
				"BUILD" : self.GetChild("BuildGuildBuilding"),
				"EXIT_OBSERVER" : self.GetChild("ExitObserver"),
			}

			self.gameButtonDict["EXIT_OBSERVER"].SetEvent(ui.__mem_func__(self.__OnClickExitObserver))
			if app.ENABLE_SAVE_BLOCK_ATTR:
				self.gameButtonDict["ATTR_BLOCK"] = self.GetChild("SaveBlockAttr")
				self.gameButtonDict["ATTR_BLOCK"].SetEvent(ui.__mem_func__(self.__OnClickAttrBlock))
			if app.ENABLE_VOTE_4_BUFF:
				self.gameButtonDict["VOTE4BUFF"] = self.GetChild("Vote4Buff")
				self.gameButtonDict["VOTE4BUFF"].SetEvent(ui.__mem_func__(self.__OnClickVote4Buff))

		except Exception, msg:
			import dbg
			dbg.TraceError("GameButtonWindow.LoadScript - %s" % (msg))
			app.Abort()
			return False

		self.__HideAllGameButton()
		self.SetObserverMode(player.IsObserverMode())
		return True

	def Destroy(self):
		for key in self.gameButtonDict:
			self.gameButtonDict[key].SetEvent(0)

		self.gameButtonDict={}

	def SetButtonEvent(self, name, event):
		try:
			self.gameButtonDict[name].SetEvent(event)
		except Exception, msg:
			print "GameButtonWindow.LoadScript - %s" % (msg)
			app.Abort()
			return

	def ShowBuildButton(self):
		self.gameButtonDict["BUILD"].Show()

	def HideBuildButton(self):
		self.gameButtonDict["BUILD"].Hide()

	def HideSkillAndStatusButton(self):
		self.gameButtonDict["STATUS"].Hide()
		self.gameButtonDict["SKILL"].Hide()

	def CheckGameButton(self):

		if not self.IsShow():
			return

		statusPlusButton=self.gameButtonDict["STATUS"]
		skillPlusButton=self.gameButtonDict["SKILL"]
		helpButton=self.gameButtonDict["HELP"]
		if app.ENABLE_VOTE_4_BUFF:
			voteButton=self.gameButtonDict["VOTE4BUFF"]

			if player.CheckAffect(chr.NEW_AFFECT_VOTE_4_BUFF, 0):
				voteButton.Hide()
			else:
				voteButton.Hide()

		# if player.GetStatus(player.STAT) > 0:
			# statusPlusButton.Show()
		# else:
			# statusPlusButton.Hide()

		if self.__IsStatusStat():
			statusPlusButton.Hide()
		else:
			statusPlusButton.Hide()

		if self.__IsSkillStat():
			skillPlusButton.Hide()
		else:
			skillPlusButton.Hide()

		if 0 == player.GetPlayTime():
			helpButton.Show()
		else:
			helpButton.Hide()

	def __IsStatusStat(self):
		if player.GetLevel() < 2:
			return False
		if player.GetStatus(player.HT) >= 90:
			return False
		return True

	def __IsSkillStat(self):
		race = player.GetRace()
		if player.GetLevel() < 5:
			return False
		if race == 0 or race == 4 or race == 5 or race == 1:
			if player.GetStatus(player.SKILL_ACTIVE) < 35:
				return False
		else:
			if player.GetStatus(player.SKILL_ACTIVE) < 18:
				return False
		return True
			
	# def __IsSkillStat(self):
		# if player.GetStatus(player.SKILL_ACTIVE) > 0:
			# return True
		# return False

	def __OnClickExitObserver(self):
		net.SendChatPacket("/observer_exit")

	if app.ENABLE_SAVE_BLOCK_ATTR:
		def __OnClickAttrBlock(self):
			net.SendChatPacket("/save_block_attr")
			self.gameButtonDict["ATTR_BLOCK"].Hide()

	if app.ENABLE_VOTE_4_BUFF:
		def __OnClickVote4Buff(self):
			import constInfo
			interface = constInfo.GetInterfaceInstance()
			if interface:
				# interface.MakeVote4Buff()
				interface.OpenVoteWindow()

		def HideVote4BuffButton(self):
			self.gameButtonDict["VOTE4BUFF"].Hide()

	def __HideAllGameButton(self):
		for btn in self.gameButtonDict.values():
			btn.Hide()

	def SetObserverMode(self, isEnable):
		if isEnable:
			self.gameButtonDict["EXIT_OBSERVER"].Show()
		else:
			self.gameButtonDict["EXIT_OBSERVER"].Hide()
