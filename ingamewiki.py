import app, item, nonplayer, ui, ingamewikiui,\
 wiki, wndMgr, grp, player

INGAME_WIKI_DROPDOWN_SEARCH = True

class InGameWiki(ui.Window):
	def __init__(self):
		self.searchEdit = None
		
		if INGAME_WIKI_DROPDOWN_SEARCH:
			self.searchEditHint = None
			self.searchEditHintDropDown = None
		
		ui.Window.__init__(self)
		
		self.SetWindowName("InGameWiki")
		wiki.RegisterClass(self)
		
		self.objList = {}
		self.windowHistory = []
		self.currSelected = 0
		self.BuildUI()
		self.SetCenterPosition()
		self.Hide()

	def __del__(self):
		wiki.UnregisterClass()
		ui.Window.__del__(self)

	def Show(self):
		wndMgr.Show(self.hWnd)
		wiki.ShowModelViewManager(True)

	if INGAME_WIKI_DROPDOWN_SEARCH:
		def Hide(self):
			wndMgr.Hide(self.hWnd)
			wiki.ShowModelViewManager(False)
			
			if self.searchEdit:
				self.searchEdit.KillFocus()
				self.searchEdit.SetText("")
				self.searchEdit.SetBackgroundText("")

			if self.searchEditHint:
				self.searchEditHint.SetText("")

			if self.searchEditHintDropDown:
				self.searchEditHintDropDown.CloseListBox()

	else:
		def Hide(self):
			wndMgr.Hide(self.hWnd)
			wiki.ShowModelViewManager(False)
			
			if self.searchEdit:
				self.searchEdit.KillFocus()

	def Close(self):
		self.Hide()

	def Open(self):
		self.Show()

	def RequestOpen(self):
		## Return if window is already shown
		if self.IsShow():
			return False

		self.Show()
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True
	
	def BINARY_LoadInfo(self, objID, vnum):
		if objID in self.objList:
			self.objList[objID].NoticeMe()
	
	def BuildUI(self):
		ingamewikiui.InitMainWindow(self)
		ingamewikiui.BuildBaseMain(self)
	
	def OpenSpecialPage(self, oldWindow, vnum, isMonster = False):
		del self.windowHistory[self.currSelected + 1:]

		try:
			if oldWindow:
				del self.windowHistory[:]
				
				self.currSelected = 0
				self.windowHistory.append(oldWindow)
			
			if len(self.windowHistory) > 0:
				self.windowHistory[-1].Hide()
			
			newSpec = ingamewikiui.SpecialPageWindow(vnum, isMonster)
			newSpec.AddFlag("attach")
			newSpec.SetParent(self)
			newSpec.SetPosition(ingamewikiui.mainBoardPos[0] + 7, ingamewikiui.mainBoardPos[1] + 7)
			newSpec.Show()
			
			self.windowHistory.append(newSpec)
			self.currSelected = self.windowHistory.index(newSpec)
		except ReferenceError:
			pass

	if INGAME_WIKI_DROPDOWN_SEARCH:
		def IsItemSearchMode(self):
			if not self.monsterSearch.IsSelected():
				self.searchEditHintDropDown.SetItemMode(True)
				return True

			self.searchEditHintDropDown.SetItemMode(False)
			return False

		def ClearNameInput(self):
			self.searchEdit.SetBackgroundText("")
			self.searchEdit.SetText("")
			self.searchEdit.SetEndPosition()

			# Clear suggestion list
			self.searchEditHintDropDown.CloseListBox()

		def OnPressNameEscapeKey(self):
			if not self.searchEdit:
				return

			if not self.searchEdit.IsShowCursor() or self.searchEdit.GetText() == "":
				self.OnPressEscapeKey()
			else:
				self.searchEdit.SetText("")
				self.searchEdit.SetBackgroundText("")
				self.searchEditHint.SetText("")

				# Update
				self.searchEditHintDropDown.CloseListBox()

		def OnNameInputUpdate(self):
			inputText = self.searchEdit.GetText()

			if len(inputText) >= 3:
				# Item
				if self.IsItemSearchMode():
					itemSuggestionList = item.GetItemListByName(inputText)

				# Monster
				if not self.IsItemSearchMode():
					##TODO : GetMonsterListByName
					itemSuggestionList = nonplayer.GetMonsterDataByNamePart(inputText)

				# Get rid of previous suggestions
				if self.searchEditHintDropDown.GetItemCount():
					self.searchEditHintDropDown.ClearItem()

				if len(itemSuggestionList) <= 0:
					self.selectedItemVnum = 0
					self.searchEdit.SetBackgroundText("")
					return

				# Build up new suggestions list
				for i in range(len(itemSuggestionList)):
					(vnum, itemName) = itemSuggestionList[i]

					# Set background text for the first suggested item
					if i == 0:
						self.selectedItemVnum = vnum
						self.searchEdit.SetTipText(itemName)

					self.searchEditHintDropDown.InsertItem(vnum, itemName)

				self.searchEditHintDropDown.AdjustListBox()
				self.searchEditHintDropDown.OpenListBox()
			else:
				self.selectedItemVnum = 0
				self.searchEdit.SetBackgroundText("")
				self.searchEditHintDropDown.CloseListBox()

		def SetNameInputResult(self):
			inputText = self.searchEdit.GetText()
			suggestionText = self.searchEdit.GetBackgroundText()

			# Item
			if self.IsItemSearchMode():
				(vnum, itemName) = item.GetItemByName(inputText + suggestionText)

			# Monster
			if not self.IsItemSearchMode():
				(itemName, vnum) = nonplayer.GetMonsterDataByNamePart(inputText + suggestionText)

			self.searchEdit.SetBackgroundText("")
			self.searchEdit.SetText(itemName)
			self.searchEdit.SetEndPosition()

			self.selectedItemVnum = vnum

			# Clear suggestion list
			self.searchEditHintDropDown.CloseListBox()

		def SelectItem(self, itemVnum):
			if itemVnum <= 0:
				return

			# Item
			if self.IsItemSearchMode():
				item.SelectItem(itemVnum)
				itemName = item.GetItemName()

			# Monster
			if not self.IsItemSearchMode():
				itemName = nonplayer.GetMonsterName(itemVnum)

			self.searchEdit.SetText(itemName)
			self.searchEdit.SetBackgroundText("")
			self.searchEdit.SetEndPosition()

			# Clear suggestion list
			self.searchEditHintDropDown.CloseListBox()

			self.selectedItemVnum = itemVnum

		def StartSearch(self):
			search_text = self.searchEdit.GetText()

			if len(search_text):

				# Item
				if self.IsItemSearchMode():
					self.CloseBaseWindows()

					item.SelectItem(self.selectedItemVnum)
					checkedVnum = ((int(self.selectedItemVnum / 10) * 10) if wiki.CanIncrRefineLevel() else self.selectedItemVnum)
					self.OpenSpecialPage(None, checkedVnum, False)

				# Monster
				if not self.IsItemSearchMode():
					(search_name, search_vnum) = nonplayer.GetMonsterDataByNamePart(search_text)
					if search_vnum == -1:
						return

					self.CloseBaseWindows()
					self.OpenSpecialPage(None, search_vnum, True)

	def GoToLanding(self):
		self.CloseBaseWindows()
		self.categ.NotifyCategorySelect(None)
		self.customPageWindow.LoadFile("landing_page.txt")

	def CloseBaseWindows(self):
		self.mainWeaponWindow.Hide()
		self.mainChestWindow.Hide()
		self.mainBossWindow.Hide()
		self.customPageWindow.Hide()
		self.costumePageWindow.Hide()

		del self.windowHistory[:]
		self.currSelected = 0
