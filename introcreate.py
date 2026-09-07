import ui, net, app, grp, wndMgr, chr, playerSettingModule, localeInfo, snd, musicInfo, systemSetting, dbg

BLACKLIST = ["NAMES"]

IMAGE_PATH = "d:/ymir work/ui/login/create/btn/"

PLAYER_VALUES = {
	0 : 0,
	1 : 5,
	2 : 2,
	3 : 7,
	4 : 4,
	5 : 1,
	6 : 6,
	7 : 3,
}

class CreateCharacterWindow(ui.ScriptWindow):

	class CharacterRenderer(ui.Window):
		def OnRender(self):
			grp.ClearDepthBuffer()

			grp.SetGameRenderState()
			grp.PushState()
			grp.SetOmniLight()

			screenWidth = wndMgr.GetScreenWidth()
			screenHeight = wndMgr.GetScreenHeight()
			newScreenWidth = float(screenWidth)
			newScreenHeight = float(screenHeight)

			grp.SetViewport(0.0, 0.0, newScreenWidth / screenWidth, newScreenHeight / screenHeight)

			app.SetCenterPosition(0.0, 0.0, 0.0)
			app.SetCamera(1550.0, 15.0, 180.0, 95.0)
			grp.SetPerspective(11.0, newScreenWidth / newScreenHeight, 1000.0, 3000.0)

			(x, y) = app.GetCursorPosition()
			grp.SetCursorPosition(x, y)

			chr.Deform()
			chr.Render()

			grp.RestoreViewport()
			grp.PopState()
			grp.SetInterfaceRenderState()

	def __init__(self, stream = None):
		ui.ScriptWindow.__init__(self)
		self.stream = stream
		playerSettingModule.LoadGameData("INIT")
		net.SetPhaseWindow(net.PHASE_WINDOW_CREATE, self)
		self.__LoadScript("UIScript/createcharacterwindow.py")
		self.rotation = 0
		self.current_slot = 0
		self.female_add = 0
		self.shapeIndex = 0
		self.reservingRaceIndex = -1
		self.is_female = FALSE
		self.Open()
				
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_CREATE, 0)
		
	def Open(self):
		playerSettingModule.LoadGameData("INIT")
		app.SetCamera(500.0, 10.0, 180.0, 95.0)
		self.Show()

		if musicInfo.createMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/"+musicInfo.createMusic)

		app.ShowCursor()
		self.__SelectSlot(0)

	def Close(self):
		self.edit_name.Enable()
		self.stream=0

		if musicInfo.createMusic != "":
			snd.FadeOutMusic("BGM/"+musicInfo.createMusic)

		chr.DeleteInstance(0)

		self.Hide()

		app.HideCursor()

	def __LoadScript(self, fileName):
		try:
			pyLoader = ui.PythonScriptLoader()
			pyLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("CreateCharacterWindow.py ## __LoadScript.LoadScriptFile")
		try:
			self.btnExit			= self.GetChild("exit_button")
			self.edit_name		= self.GetChild("edit_name")
			self.content_create		= self.GetChild("content_create")
			self.btn_create		= self.GetChild("btn_create")
			self.btn_left		= self.GetChild("btn_left")
			self.btn_right		= self.GetChild("btn_right")

			self.gender_btn = {
				0 : self.GetChild("btn_gender_01"),
				1 : self.GetChild("btn_gender_02"),
			}
			self.shape_btn = {
				0 : self.GetChild("btn_shape_01"),
				1 : self.GetChild("btn_shape_02"),
			}
			self.arrow_btn = {
				0 : self.GetChild("arrow_left"),
				1 : self.GetChild("arrow_right"),
			}

			self.Characters = []
			self.Characters.append(self.GetChild("slot_1"))
			self.Characters.append(self.GetChild("slot_2"))
			self.Characters.append(self.GetChild("slot_3"))
			self.Characters.append(self.GetChild("slot_4"))

			self.character_render_window	= self.GetChild("character_render_window")

			self.chrRenderer = self.CharacterRenderer()
			self.chrRenderer.SetParent(self.character_render_window)
			self.chrRenderer.Show()
		except:
			import exception
			exception.Abort("CreateCharacterWindow.py ## __LoadScript.GetChild")
		try:
			self.btnExit.SetEvent(ui.__mem_func__(self.ExitCreate))
			self.btn_create.SetEvent(ui.__mem_func__(self.__CreateCharacter))
			self.btn_right.SetEvent(ui.__mem_func__(self.__SelectSlot), 1)
			self.btn_left.SetEvent(ui.__mem_func__(self.__SelectSlot), -1)

			for i in range(len(self.gender_btn)):
				self.gender_btn[i].SetEvent(ui.__mem_func__(self.__ChangeGender),i)
			for i in range(len(self.shape_btn)):
				self.shape_btn[i].SetEvent(ui.__mem_func__(self.__ChangeShape),i)
				
			self.arrow_btn[0].SetEvent(ui.__mem_func__(self.__SelectSlot), 1)
			self.arrow_btn[1].SetEvent(ui.__mem_func__(self.__SelectSlot), -1)

			self.edit_name.SetReturnEvent(ui.__mem_func__(self.__CreateCharacter))
			self.edit_name.SetEscapeEvent(ui.__mem_func__(self.CancelCreate))
			self.edit_name.SetMax(16)
			self.edit_name.SetFocus()

			self.Characters[0].SetEvent(ui.__mem_func__(self.ChooseSlot), 0)
			self.Characters[1].SetEvent(ui.__mem_func__(self.ChooseSlot), 1)
			self.Characters[2].SetEvent(ui.__mem_func__(self.ChooseSlot), 2)
			self.Characters[3].SetEvent(ui.__mem_func__(self.ChooseSlot), 3)

			self.Characters[0].Down()
			self.gender_btn[1].Down()
			
			# self.currentCharacterIndex = None
			# for i in xrange(1, 5):
				# flag_name = "flag_0" + str(i)
				# flag_element = self.GetChild(flag_name)
				# self.languages.append(flag_element)
				# bLanguage = self.languageLists[i - 1]
				# if app.GetLocaleName() == bLanguage:
					# self.flagImage.LoadImage("d:/ymir work/ui/login/login/language/flag_" + bLanguage + "_01.png")
					# self.flagImage.Show()
					# self.currentCharacterIndex = i - 1
				# else:
					# flag_element.SAFE_SetEvent(self.OnSelectLanguageButton, bLanguage)
			self.HideCharacterOptions()
			# if self.currentCharacterIndex is None:
				# self.currentCharacterIndex = 0
		except:
			import exception
			exception.Abort("CreateCharacterWindow.py ## __LoadScript.SetEvent")

	def __Close(self):
		self.CancelCreate()

	def __ChangeGender(self, id):
		for i in range(len(self.gender_btn)):
			self.gender_btn[i].SetUp()
		self.gender_btn[id].Down()
		if id == 0:
			self.is_female = TRUE
			self.SelectRaceFemale(self.current_slot + self.female_add)
		else:
			self.is_female = FALSE
			self.SelectRaceMale(self.current_slot + self.female_add)

		self.__SelectSlot(self.current_slot, True)

	def __ChangeShape(self, id):
		for i in range(len(self.shape_btn)):
			self.shape_btn[i].SetUp()
		self.shape_btn[id].Down()
		chr.ChangeShape(id)
		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)
		self.shapeIndex = id

	def ChooseSlot(self, slot):
		self.__SelectSlot(slot, True)
		self.__ClickRadioButton(self.Characters, slot)

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			selButton=buttonList[buttonIndex]
		except IndexError:
			return
		for eachButton in buttonList:
			eachButton.SetUp()
		selButton.Down()

	def __MakeCharacter(self, race):
		chr.CreateInstance(0)
		chr.SelectInstance(0)
		chr.SetVirtualID(0)
		chr.SetNameString("")

		chr.SetRace(race)
		chr.SetArmor(0)
		chr.SetHair(0)

		chr.Refresh()
		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)

		chr.SetRotation(0.0)

	def __SelectSlot(self, slot, chosebtn = False):
		if not chosebtn:
			if (self.current_slot + slot) < 0:
				self.current_slot = 3
			elif (self.current_slot + slot) >3:
				self.current_slot = 0
			else:
				self.current_slot += slot
		else:
			self.current_slot = slot

		if self.is_female:
			self.female_add = 4
		else:
			self.female_add = 0
		chr.DeleteInstance(0)
		self.__ChangeShape(0)

		if self.is_female:
			self.SelectRaceMale(self.current_slot + self.female_add)
		else:
			self.SelectRaceFemale(self.current_slot + self.female_add)
			
		self.__MakeCharacter(PLAYER_VALUES[self.current_slot + self.female_add])

	def SelectRaceMale(self, race_index):
		self.HideCharacterOptions()

		if race_index == 4:
			self.Characters[self.current_slot].SetUpVisual(IMAGE_PATH + "btn_warrior_w_01.png")
			self.Characters[self.current_slot].SetOverVisual(IMAGE_PATH + "btn_warrior_w_02.png")
			self.Characters[self.current_slot].SetDownVisual(IMAGE_PATH + "btn_warrior_w_02.png")
		elif race_index == 5:
			self.Characters[self.current_slot].SetUpVisual(IMAGE_PATH + "btn_ninja_w_01.png")
			self.Characters[self.current_slot].SetOverVisual(IMAGE_PATH + "btn_ninja_w_02.png")
			self.Characters[self.current_slot].SetDownVisual(IMAGE_PATH + "btn_ninja_w_02.png")
		elif race_index == 6:
			self.Characters[self.current_slot].SetUpVisual(IMAGE_PATH + "btn_sura_w_01.png")
			self.Characters[self.current_slot].SetOverVisual(IMAGE_PATH + "btn_sura_w_02.png")
			self.Characters[self.current_slot].SetDownVisual(IMAGE_PATH + "btn_sura_w_02.png")
		elif race_index == 7:
			self.Characters[self.current_slot].SetUpVisual(IMAGE_PATH + "btn_shaman_w_01.png")
			self.Characters[self.current_slot].SetOverVisual(IMAGE_PATH + "btn_shaman_w_02.png")
			self.Characters[self.current_slot].SetDownVisual(IMAGE_PATH + "btn_shaman_w_02.png")

		for i in xrange(len(self.Characters)):
			if i == self.current_slot:
				self.Characters[i].Show()
			else:
				self.Characters[i].Hide()

		self.__ClickRadioButton(self.Characters, self.current_slot)

	def SelectRaceFemale(self, race_index):
		self.HideCharacterOptions()

		if race_index == 0:
			self.Characters[self.current_slot].SetUpVisual(IMAGE_PATH + "btn_warrior_m_01.png")
			self.Characters[self.current_slot].SetOverVisual(IMAGE_PATH + "btn_warrior_m_02.png")
			self.Characters[self.current_slot].SetDownVisual(IMAGE_PATH + "btn_warrior_m_02.png")
		elif race_index == 1:
			self.Characters[self.current_slot].SetUpVisual(IMAGE_PATH + "btn_ninja_m_01.png")
			self.Characters[self.current_slot].SetOverVisual(IMAGE_PATH + "btn_ninja_m_02.png")
			self.Characters[self.current_slot].SetDownVisual(IMAGE_PATH + "btn_ninja_m_02.png")
		elif race_index == 2:
			self.Characters[self.current_slot].SetUpVisual(IMAGE_PATH + "btn_sura_m_01.png")
			self.Characters[self.current_slot].SetOverVisual(IMAGE_PATH + "btn_sura_m_02.png")
			self.Characters[self.current_slot].SetDownVisual(IMAGE_PATH + "btn_sura_m_02.png")
		elif race_index == 3:
			self.Characters[self.current_slot].SetUpVisual(IMAGE_PATH + "btn_shaman_m_01.png")
			self.Characters[self.current_slot].SetOverVisual(IMAGE_PATH + "btn_shaman_m_02.png")
			self.Characters[self.current_slot].SetDownVisual(IMAGE_PATH + "btn_shaman_m_02.png")

		for i in xrange(len(self.Characters)):
			if race_index == self.current_slot:
				self.Characters[self.current_slot].Show()
			else:
				self.Characters[i].Hide()

		self.__ClickRadioButton(self.Characters, self.current_slot)

	def HideCharacterOptions(self):
		for i in xrange(len(self.Characters)):
			self.Characters[i].Hide()

	def OnKeyDown(self, key):
		if 203 == key:
			self.__SelectSlot(-1)
		if 205 == key:
			self.__SelectSlot(1)
		if 28 == key:
			self.__CreateCharacter()

		return TRUE
		
	def __CreateCharacter(self):
		self.DisableWindow()
		textName = self.edit_name.GetText()
		if FALSE == self.__CheckCreateCharacter(textName):
			return
		if musicInfo.selectMusic != "":
			snd.FadeLimitOutMusic("BGM/"+musicInfo.selectMusic, systemSetting.GetMusicVolume()*0.05)
		self.reservingStartTime = app.GetTime()
		self.reservingRaceIndex = chr.GetRace()
		chr.PushOnceMotion(chr.MOTION_INTRO_SELECTED)
			
	def __CheckCreateCharacter(self, name):
		if len(name) == 0:
			self.PopupMessage(localeInfo.CREATE_INPUT_NAME, self.EnableWindow)
			return FALSE

		if name.find(localeInfo.CREATE_GM_NAME)!=-1:
			self.PopupMessage(localeInfo.CREATE_ERROR_GM_NAME, self.EnableWindow)
			return FALSE

		if net.IsInsultIn(name):
			self.PopupMessage(localeInfo.CREATE_ERROR_INSULT_NAME, self.EnableWindow)
			return FALSE
			

		if name.upper() in BLACKLIST:
			self.PopupMessage("Teamlernamen nicht erlaubt!", self.EnableWindow)
			return FALSE

		return TRUE
		
	def EnableWindow(self):
		self.btn_create.Enable()
		self.gender_btn[0].Enable()
		self.gender_btn[1].Enable()
		self.shape_btn[0].Enable()
		self.shape_btn[1].Enable()
		self.edit_name.SetFocus()
		chr.BlendLoopMotion(chr.MOTION_INTRO_WAIT, 0.1)
		
	def DisableWindow(self):
		self.btn_create.Disable()
		self.gender_btn[0].Disable()
		self.gender_btn[1].Disable()
		self.shape_btn[0].Disable()
		self.shape_btn[1].Disable()
		self.edit_name.Disable()
		
	def OnCreateSuccess(self):
		self.stream.SetSelectCharacterPhase()

	def OnCreateFailure(self, type):
		if 1 == type:
			self.PopupMessage(localeInfo.CREATE_EXIST_SAME_NAME, self.EnableWindow)
		else:
			self.PopupMessage(localeInfo.CREATE_FAILURE, self.EnableWindow)
		
	def PopupMessage(self, msg, func=0):
		if not func:
			func=self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)
			
	def EmptyFunc(self):
		pass

	def OnPressExitKey(self):
		self.CancelCreate()
		
	def CancelCreate(self):
		self.stream.SetSelectCharacterPhase()
		
	def OnUpdate(self):
		chr.Update()
		# chr.SetRotation(self.rotation)
		# self.rotation += 0.4
		
		if -1 != self.reservingRaceIndex:
			if app.GetTime() - self.reservingStartTime >= 1.5:

				chrSlot=self.stream.GetCharacterSlot()
				textName = self.edit_name.GetText()
				if self.is_female:
					self.female_add = 4
				else:
					self.female_add = 0
				net.SendCreateCharacterPacket(chrSlot, textName, PLAYER_VALUES[self.current_slot + self.female_add], self.shapeIndex, 0, 0, 0, 0)

				self.reservingRaceIndex = -1

	def ExitCreate(self):
		self.stream.popupWindow.Close()
		self.stream.SetSelectCharacterPhase()