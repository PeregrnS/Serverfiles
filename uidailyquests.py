import ui
import player
import constInfo
import net
import localeInfo
import nonplayer

class DailyWindow(ui.BoardWithFinalCoreTitleBar14):
	QUEST_TYPES = QUEST_TYPES = ["", "DAILY_TYPE_KILL_STONE", "DAILY_TYPE_KILL_BOSS", "DAILY_TYPE_KILL_MONSTER", "DAILY_TYPE_KILL_PLAYER", "DAILY_TYPE_UPGRADE", "DAILY_TYPE_FISH", "DAILY_TYPE_MINE", "DAILY_TYPE_CHEST", "DAILY_TYPE_YANG", "DAILY_TYPE_RECEIVE_DAMAGE", "DAILY_TYPE_SEND_DAMAGE"]
	def __init__(self):
		ui.BoardWithFinalCoreTitleBar14.__init__(self)
		self.SetSize(350, 560)
		self.SetCenterPosition()
		self.SetTitleName("Daily Quests")
		self.AddFlag("movable")
		self.AddFlag("float")
		
		self.isLoaded = False
		self.LoadWindow()
		
	def OnPressEscapeKey(self):
		self.Hide()
		return True
		
	def LoadWindow(self):
		if self.isLoaded:
			return
			
		self.quests = {}
		self.questLen = 0
		self.rewards = {}
		self.rewardLen = 0
		self.rewardItems = []
			
		self.questBG = ui.ImageBox()
		self.questBG.SetParent(self)
		self.questBG.SetPosition(0, 32)
		self.questBG.SetWindowHorizontalAlignCenter()
		self.questBG.LoadImage("d:/ymir work/ui/daily_quests/quest_outline.png")
		self.questBG.Show()
		
		self.questHeader = ui.ImageBox()
		self.questHeader.SetParent(self.questBG)
		self.questHeader.SetPosition(0, 2)
		self.questHeader.SetWindowHorizontalAlignCenter()
		self.questHeader.LoadImage("d:/ymir work/ui/daily_quests/quest_header.png")
		self.questHeader.SetText(localeInfo.DAILY_QUEST_INFO, 0, 0, True)
		self.questHeader.SetPackedFontColor(0xffffffff)
		self.questHeader.Show()
			
		self.rewardHeader = ui.ImageBox()
		self.rewardHeader.SetParent(self)
		self.rewardHeader.SetPosition(0, self.questBG.GetHeight() + 7 + 32)
		self.rewardHeader.SetWindowHorizontalAlignCenter()
		self.rewardHeader.LoadImage("d:/ymir work/ui/daily_quests/overview_header.png")
		self.rewardHeader.Show()
			
		self.rewardBG = ui.ImageBox()
		self.rewardBG.SetParent(self)
		self.rewardBG.SetPosition(0, self.questBG.GetHeight() + 7 + 32 + self.rewardHeader.GetHeight() + 6)
		self.rewardBG.SetWindowHorizontalAlignCenter()
		self.rewardBG.LoadImage("d:/ymir work/ui/daily_quests/reward_outline.png")
		self.rewardBG.Show()
		
		self.tooltipItem = None
			
		self.isLoaded = True

	def SetItemToolTip(self, itemTooltip):
		self.tooltipItem = itemTooltip
		
	def UpdateQuest(self, quest_id, progress):
		questData = self.quests[quest_id]
		
		quest_bg, quest_progress_bg, quest_progress_gauge, questType, questVnum, requiredCount, _ = questData
		
		quest_progress_gauge.SetRenderingRect(0.0, 0.0, -1.0 + float(progress) / float(requiredCount), 0.0)
		quest_progress_gauge.SetText(str(int((float(progress) / float(requiredCount) * 100))) + "%", 0, 0, True)
		
	def UpdateDailyRewards(self, progress):
		for i in range(self.rewardLen):
			index = i + 1
			
			rewardData = self.rewards[index]
			reward_progress_gauge = rewardData[5]
			
			maxVal = 2 + index
			
			realProgress = progress
			if realProgress > maxVal:
				realProgress = maxVal
			
			reward_progress_gauge.SetRenderingRect(0.0, 0.0, -1.0 + float(realProgress) / float(maxVal), 0.0)
			reward_progress_gauge.SetText(str(realProgress) + " of " + str(maxVal) + " completed", 0, 0, True)
			
			reward_title_text2 = rewardData[7]
			
			rewardtext = localeInfo.DAILY_QUEST_NOT
			reward_title_text2.SetPackedFontColor(0xffffffff)
			if realProgress == maxVal:
				rewardtext = localeInfo.DAILY_QUEST_DONE
				reward_title_text2.SetPackedFontColor(0xffffffff)
			reward_title_text2.SetText(rewardtext)
		
	def AppendQuest(self, quest_id, questType, questVnum, requiredCount, progress):
		quest_bg = ui.ImageBox()
		quest_bg.SetParent(self.questBG)
		quest_bg.LoadImage("d:/ymir work/ui/daily_quests/quest_inline.png")
		quest_bg.SetPosition(0, 1 + self.questHeader.GetHeight() + (quest_bg.GetHeight() * self.questLen))
		quest_bg.SetWindowHorizontalAlignCenter()
		quest_bg.Show()
		
		questText = ""
		
		if questVnum > 0:
			key = '{}1'.format(self.QUEST_TYPES[questType])
			if hasattr(localeInfo, key):
				questText = getattr(localeInfo, key)
				questText = str(self.questLen+1) + ". " + questText % (requiredCount, nonplayer.GetMonsterName(questVnum))
		else:
			key = '{}'.format(self.QUEST_TYPES[questType])
			if hasattr(localeInfo, key):
				questText = getattr(localeInfo, key)
				questText = str(self.questLen+1) + ". " + questText % requiredCount
		quest_bg.SetText(questText, 10, 5, False, False, True)
		quest_bg.SetPackedFontColor(0xffffffff)
		
		quest_progress_bg = ui.ImageBox()
		quest_progress_bg.SetParent(quest_bg)
		quest_progress_bg.LoadImage("d:/ymir work/ui/daily_quests/empty_quest_progress.png")
		quest_progress_bg.SetPosition(quest_progress_bg.GetWidth() + 5, 0)
		quest_progress_bg.SetWindowHorizontalAlignRight()
		quest_progress_bg.SetWindowVerticalAlignCenter()
		quest_progress_bg.Show()
		
		quest_progress_gauge = ui.ExpandedImageBox()
		quest_progress_gauge.SetParent(quest_bg)
		quest_progress_gauge.LoadImage("d:/ymir work/ui/daily_quests/full_quest_progress.png")
		quest_progress_gauge.SetPosition(quest_progress_bg.GetWidth() + 5, 0)
		quest_progress_gauge.SetRenderingRect(0.0, 0.0, -1.0 + float(progress) / float(requiredCount), 0.0)
		quest_progress_gauge.SetText(str(int((float(progress) / float(requiredCount) * 100))) + "%", 0, 0, True)
		quest_progress_gauge.SetWindowHorizontalAlignRight()
		quest_progress_gauge.SetWindowVerticalAlignCenter()
		quest_progress_gauge.Show()
		
		questData = (quest_bg, quest_progress_bg, quest_progress_gauge, questType, questVnum, requiredCount, progress)
		
		self.quests[quest_id] = (questData)
		self.questLen += 1
		
	def __ClaimReward(self, rewardId):
		net.SendChatPacket("/claim_daily %d" % rewardId)
		
	def OverInItem(self, overSlotPos):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.rewardItems[overSlotPos], 0, 0)

	def OnOverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
			
	def ResetDailyRewards(self):
		self.quests = {}
		self.questLen = 0
		self.rewards = {}
		self.rewardLen = 0
		self.rewardItems = []
		
	def AppendRewards(self, rewards, progress):
		self.rewardLen += 1
		
		rewards_header = ui.ImageBox()
		rewards_header.SetParent(self.rewardBG)
		rewards_header.LoadImage("d:/ymir work/ui/daily_quests/reward_header.png")
		rewards_header.SetPosition(0, 3 + (120*(self.rewardLen-1)))
		rewards_header.SetText(localeInfo.DAILY_QUEST_REWARD + str(self.rewardLen), 0, 0, True)
		rewards_header.SetPackedFontColor(0xffffffff)
		rewards_header.SetTextOutline()
		rewards_header.SetWindowHorizontalAlignCenter()
		rewards_header.Show()
		
		rewards_bg = ui.ImageBox()
		rewards_bg.SetParent(self.rewardBG)
		rewards_bg.LoadImage("d:/ymir work/ui/daily_quests/reward_field.png")
		rewards_bg.SetPosition(0, 3 + rewards_header.GetHeight() + (120*(self.rewardLen-1)))
		rewards_bg.SetWindowHorizontalAlignCenter()
		rewards_bg.Show()
		
		rewardSlots = ui.GridSlotWindow()
		rewardSlots.SetParent(rewards_bg)
		rewardSlots.ArrangeSlot((self.rewardLen - 1) * 6, 3, 2, 32, 32, 0, 0)
		rewardSlots.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
		rewardSlots.SetWindowHorizontalAlignRight()
		rewardSlots.SetPosition(32*3+7, 6)
		
		rewardIdx = (self.rewardLen - 1) * 6
		for reward in rewards:
			rewardSlots.SetItemSlot(rewardIdx, reward[0], reward[1])
			rewardIdx += 1	
			self.rewardItems.append(reward[0])
			
		rewardSlots.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			
		rewardSlots.SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))
		rewardSlots.Show()
		
		claimRewardBtn = ui.Button()
		claimRewardBtn.SetParent(rewards_bg)
		claimRewardBtn.SetPosition(32*3+7, 4+64+6)
		claimRewardBtn.SetWindowHorizontalAlignRight()
		claimRewardBtn.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
		claimRewardBtn.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
		claimRewardBtn.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")
		claimRewardBtn.SAFE_SetEvent(self.__ClaimReward, self.rewardLen)
		claimRewardBtn.SetText(localeInfo.DAILY_QUEST_CLAIM)
		claimRewardBtn.SetTextColor(0xffffffff)
		claimRewardBtn.Show()
		
		reward_progress_bg = ui.ImageBox()
		reward_progress_bg.SetParent(rewards_bg)
		reward_progress_bg.LoadImage("d:/ymir work/ui/daily_quests/progress_gauge_empty.png")
		reward_progress_bg.SetPosition(8, 7 + reward_progress_bg.GetHeight())
		reward_progress_bg.SetWindowVerticalAlignBottom()
		reward_progress_bg.Show()
			
		maxVal = 2 + self.rewardLen
		if progress > maxVal:
			progress = maxVal
		
		reward_progress_gauge = ui.ExpandedImageBox()
		reward_progress_gauge.SetParent(rewards_bg)
		reward_progress_gauge.LoadImage("d:/ymir work/ui/daily_quests/progress_gauge_full.png")
		reward_progress_gauge.SetPosition(8, 7 + reward_progress_gauge.GetHeight())
		reward_progress_gauge.SetRenderingRect(0.0, 0.0, -1.0 + float(progress) / float(2 + self.rewardLen), 0.0)
		reward_progress_gauge.SetWindowVerticalAlignBottom()
		reward_progress_gauge.SetText(str(progress) + localeInfo.DAILY_QUEST_OF + str(2 + self.rewardLen) + localeInfo.DAILY_QUEST_LAST, 0, 0, True)
		reward_progress_gauge.Show()
		
		reward_title_text1 = ui.TextLine()
		reward_title_text1.SetParent(rewards_bg)
		reward_title_text1.SetText(localeInfo.DAILY_QUEST_COMP + str(2 + self.rewardLen) + localeInfo.DAILY_QUEST_COM + str(self.rewardLen))
		reward_title_text1.SetPackedFontColor(0xffffffff)
		reward_title_text1.SetLimitWidth(120)
		reward_title_text1.SetMultiLine()
		reward_title_text1.SetPosition(8, 10)
		reward_title_text1.Show()
		
		reward_title_text2 = ui.TextLine()
		rewardtext = localeInfo.DAILY_QUEST_NOT
		reward_title_text2.SetPackedFontColor(0xffffffff)
		if progress == 2 + self.rewardLen:
			rewardtext = localeInfo.DAILY_QUEST_DONE
			reward_title_text2.SetPackedFontColor(0xffffffff)
		reward_title_text2.SetParent(rewards_bg)
		reward_title_text2.SetText(rewardtext)
		reward_title_text2.SetPackedFontColor(0xffffffff)
		reward_title_text2.SetPosition(8, 50)
		reward_title_text2.Show()
		
		rewardData = (rewards_header, rewards_bg, rewardSlots, claimRewardBtn, reward_progress_bg, reward_progress_gauge, reward_title_text1, reward_title_text2, rewards, progress)
		
		self.rewards[self.rewardLen] = rewardData