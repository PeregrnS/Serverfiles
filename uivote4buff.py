import ui 
import uiCommon
import app
import wndMgr
import net 
import player 
import uiScriptLocale
import constInfo
import localeInfo
import event
import chat
import os
import chr
from _weakref import proxy

ROOT = 'd:/ymir work/ui/game/interfaces/vote4buff/'

class VoteWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.currentPage = 0
		self.__LoadWindow()
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
			
	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1
		
		self.AddFlag("float")
		self.AddFlag("movable")
		
		self.Board = ui.BoardWithFinalCoreTitleBar4()
		self.Board.SetParent(self)
		self.Board.SetPosition(0, 0)
		self.Board.SetSize(340, 300)
		self.Board.AddFlag("not_pick")
		self.Board.SetTitleName("Vote 4 Buff")
		self.Board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.Board.Show()
		
		self.backBoard = ui.ExpandedImageBox()
		self.backBoard.SetParent(self.Board)
		self.backBoard.SetPosition(10, 32)
		self.backBoard.AddFlag("not_pick")
		self.backBoard.LoadImage(ROOT + "background.png")
		self.backBoard.SetScale(1.0, 1.143)
		self.backBoard.Show()

		self.titleBar = ui.MakeExpandedImageBox(self.backBoard, ROOT + "choose_background.png", 1, 42)

		self.titleName = ui.MakeTextLineNew(self.titleBar, 0, 7, uiScriptLocale.V4B_INFO)
		self.titleName.SetWindowHorizontalAlignCenter()
		self.titleName.SetHorizontalAlignCenter()
		self.titleName.Show()
		
		self.bonusBoardTwo = ui.MakeExpandedImageBox(self.backBoard, ROOT + "bonuses_pvp.png", 3, 103)
		
		self.bonusOnePvP = ui.MakeTextLineNew(self.bonusBoardTwo, 3, 4, uiScriptLocale.V4B_BONUS_1_PVP)
		# self.bonusTwoPvP = ui.MakeTextLineNew(self.bonusBoardTwo, -105, 4, uiScriptLocale.V4B_BONUS_2_PVP)
		# self.bonusThreePvP = ui.MakeTextLineNew(self.bonusBoardTwo, 102, 4, uiScriptLocale.V4B_BONUS_3_PVP)

		self.bonusBoard = ui.MakeExpandedImageBox(self.backBoard, ROOT + "bonuses.png", 3, 103)
		self.voteState = ui.MakeExpandedImageBox(self.backBoard, ROOT + "available.png", 3, 218)

		self.bonusOne = ui.MakeTextLineNew(self.bonusBoard, -2, 4, uiScriptLocale.V4B_BONUS_1)
		self.bonusTwo = ui.MakeTextLineNew(self.bonusBoard, -105, 4, uiScriptLocale.V4B_BONUS_2)
		self.bonusThree = ui.MakeTextLineNew(self.bonusBoard, 102, 4, uiScriptLocale.V4B_BONUS_3)
		self.voteText = ui.MakeTextLineNew(self.voteState, 0, 10, uiScriptLocale.V4B_CAN_VOTE_NOW)
		
		supposedTextlines = [self.bonusOne, self.bonusTwo, self.bonusThree, self.voteText, self.bonusOnePvP]
		for texts in supposedTextlines:
			texts.SetWindowHorizontalAlignCenter()
			texts.SetHorizontalAlignCenter()

		self.btnChooseFirst = ui.MakeButtonNew(self.bonusBoard, 3 + 5, 83, uiScriptLocale.V4B_CHOOSE, ROOT, "select01.png", "select02.png", "select03.png")
		self.btnChooseSecond = ui.MakeButtonNew(self.bonusBoard, 2 + 110, 83, uiScriptLocale.V4B_CHOOSE, ROOT, "select01.png", "select02.png", "select03.png")
		self.btnChooseThird = ui.MakeButtonNew(self.bonusBoard, 3 + 214, 83, uiScriptLocale.V4B_CHOOSE, ROOT, "select01.png", "select02.png", "select03.png")
		
		self.btnChooseFirst.SetEvent(ui.__mem_func__(self.ExtendedFunctions), 1)
		self.btnChooseSecond.SetEvent(ui.__mem_func__(self.ExtendedFunctions), 2)
		self.btnChooseThird.SetEvent(ui.__mem_func__(self.ExtendedFunctions), 3)

		self.btnWeb = ui.MakeButtonNew(self.backBoard, 4, 5, uiScriptLocale.V4B_WEBSITE, ROOT, "submit01.png", "submit02.png", "submit03.png")
		self.btnWeb.SetEvent(ui.__mem_func__(self.ExtendedFunctions), 69)
		
		# self.vCoinsText = ui.MakeTextLineNew(self.titleBar, 280, 7, "")
		# if player.GetCash() <= 0:
		# 	self.vCoinsText.SetText("|Eemoji/e_antisell|e 0")
		# else:
		# 	self.vCoinsText.SetText("|Eemoji/e_antisell|e %d" % (player.GetCash()))

		self.btnPageOne = ui.MakeButtonNew(self.backBoard, 33+60, 73, "PvM & PvP", "d:/ymir work/ui/game/character/", "slot_normal.tga", "slot_hover.tga", "slot_active.tga")
		
		self.SetPage(1)
		
		self.SetSize(self.Board.GetWidth(), self.Board.GetHeight())
		
	def ExtendedFunctions(self, index):
		if index == 69:
			# event.QuestButtonClick(int(constInfo.VOTEBUFF))
			os.system("start https://lyrius.eu/")
		else:
			net.SendChatPacket("/vote4buff %d" % index)
			self.Close()
			
	def SetPage(self, page):
		self.currentPage = page
		self.bonusBoard.Show()
		
	def OnUpdate(self):
		if constInfo.AFFECT_V4B > 0:
			self.voteState.LoadImage(ROOT + "unavailable.png")
			self.voteText.SetText("%s: %s" % (localeInfo.LEFT_TIME, self.FormatTime(constInfo.AFFECT_V4B - app.GetGlobalTimeStamp())))
		else:
			self.voteState.LoadImage(ROOT + "available.png")
			self.voteText.SetText(uiScriptLocale.V4B_CAN_VOTE_NOW)
			
	def FormatTime(self, seconds):
		if seconds <= 0:
			return "0s"
		m, s = divmod(seconds, 60)
		h, m = divmod(m, 60)
		d, h = divmod(h, 24)
		timeText = ""
		if d > 0:
			timeText += "{}d".format(d)
			timeText += " "
		if h > 0:
			timeText += "{}h".format(h)
			timeText += " "
		if m > 0:
			timeText += "{}m".format(m)
			timeText += " "
		if s > 0:
			timeText += "{}s".format(s)
		return timeText

	def Destroy(self):
		self.ClearDictionary()
		
	def Close(self):
		self.Hide()

	def Show(self):
		if app.ENABLE_VOTE_4_BUFF:
			if player.CheckAffect(chr.NEW_AFFECT_VOTE_4_BUFF, 0):
				chat.AppendChat(2, "Your bonus is active, now u can change the bonus for 5 Gaya!")
				chat.AppendChat(2, "Dein Bonus ist aktiv, jetzt kannst du den Bonus fur 5 Gaya wechseln!")
				# return
		self.SetTop()
		self.SetCenterPosition()
		ui.ScriptWindow.Show(self)

	def OnPressEscapeKey(self):
		self.Close()
		return True
		