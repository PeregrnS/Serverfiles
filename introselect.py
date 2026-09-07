import ui, net, app, grp, wndMgr, chr, playerSettingModule, localeInfo, systemSetting, snd, dbg

DEV_AUTO_ENTER_CHAR = 0  # 1 = Automatisch Enter | 0 = Deaktiviert

PLAYER_SLOT_COUNT = 5

SELECT_ENTER_DELAY_SEC = 1.0

IMAGE_PATH = "d:/ymir work/ui/login/select/btn/"


class SelectCharacterWindow(ui.ScriptWindow):

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

			for i in xrange(5):
				chr.SelectInstance(i)
				chr.RenderAllAttachingEffect()

			grp.RestoreViewport()
			grp.PopState()
			grp.SetInterfaceRenderState()

	def __init__(self, stream=None):
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_SELECT, self)
		self.stream = stream
		playerSettingModule.LoadGameData("INIT")
		self.__LoadScript("uiscript/selectcharacterwindow.py")
		self.current_slot = 0
		self.slot = self.current_slot
		self.openLoadingFlag = False
		self.startIndex = -1
		self.startReservingTime = 0
		self._slot_sig = None

	def Open(self):
		self.openLoadingFlag = False
		self.startIndex = -1
		self.startReservingTime = 0
		self.Show()
		app.ShowCursor()
		if getattr(self, "slot_window", None):
			self.slot_window.Show()
		if getattr(self, "character_render_window", None):
			self.character_render_window.Show()

		if DEV_AUTO_ENTER_CHAR:
			if self.current_slot >= 0:
				self.__SelectSlot(self.current_slot, True)
				self.__Connect()

		self.SetFocus()
		self.SetEmpire(net.GetEmpireID())
		self.LoadBoardInfo()
		self.__SelectSlot(0, True)
		self.Characters[0].Down()
		self._slot_sig = tuple(net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID) for i in xrange(PLAYER_SLOT_COUNT))

	def Close(self):
		self._slot_sig = None
		self.stream.popupWindow.Close()
		chr.DeleteInstance(0)
		self.Hide()
		self.KillFocus()
		app.HideCursor()
		self.text_timevalue = None
		self.text_kingdomvalue = None

	def SetEmpire(self, id):
		if id == 1:
			self.text_kingdomvalue.LoadImage("d:/ymir work/ui/login/select/empire_1.png")
		elif id == 2:
			self.text_kingdomvalue.LoadImage("d:/ymir work/ui/login/select/empire_2.png")
		else:
			self.text_kingdomvalue.LoadImage("d:/ymir work/ui/login/select/empire_3.png")

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_SELECT, 0)

	def __LoadScript(self, fileName):
		try:
			pyLoader = ui.PythonScriptLoader()
			pyLoader.LoadScriptFile(self, fileName)
		except:
			try:
				pyLoader = ui.PythonScriptLoader()
				alt = "Uiscript/SelectCharacterWindow.py"
				pyLoader.LoadScriptFile(self, alt)
			except:
				import exception
				exception.Abort("SelectCharacterWindow.py ## __LoadScript.LoadScriptFile")
		try:
			self.gauge_vit = self.GetChild("gauge_vit")
			self.gauge_int = self.GetChild("gauge_int")
			self.gauge_str = self.GetChild("gauge_str")
			self.gauge_dex = self.GetChild("gauge_dex")
			self.text_timevalue = self.GetChild("text_timevalue")
			self.text_guildname = self.GetChild("text_guildname")
			self.text_kingdomvalue = self.GetChild("text_kingdomvalue")
			self.text_levelvalue = self.GetChild("text_levelvalue")
			self.btn_login = self.GetChild("btn_login")
			self.btn_left = self.GetChild("btn_left")
			self.btn_right = self.GetChild("btn_right")
			self.slot_window = self.GetChild("slot_window")

			self.character_render_window = self.GetChild("character_render_window")
			self.chrRenderer = self.CharacterRenderer()
			self.chrRenderer.SetParent(self.character_render_window)
			self.chrRenderer.Show()

			self.Characters = []
			self.Characters.append(self.GetChild("slot_1"))
			self.Characters.append(self.GetChild("slot_2"))
			self.Characters.append(self.GetChild("slot_3"))
			self.Characters.append(self.GetChild("slot_4"))
			self.Characters.append(self.GetChild("slot_5"))

			attributes = [
				"text_playername_new",
				"btn_create_new",
				"btn_login_new",
				"btn_delete_new"
			]
			for i in xrange(PLAYER_SLOT_COUNT):
				for attr in attributes:
					attribute_name = "{}{}".format(attr, i)
					setattr(self, attribute_name, self.GetChild(attribute_name))

		except:
			import exception
			exception.Abort("SelectCharacterWindow.py ## __LoadScript.GetChild")

		try:
			self.btn_login.SAFE_SetEvent(self.__Connect)
			self.btn_left.SetEvent(ui.__mem_func__(self.__SelectSlot), -1)
			self.btn_right.SetEvent(ui.__mem_func__(self.__SelectSlot), 1)

			self.Characters[0].SetEvent(ui.__mem_func__(self.ChooseSlot), 0)
			self.Characters[1].SetEvent(ui.__mem_func__(self.ChooseSlot), 1)
			self.Characters[2].SetEvent(ui.__mem_func__(self.ChooseSlot), 2)
			self.Characters[3].SetEvent(ui.__mem_func__(self.ChooseSlot), 3)
			self.Characters[4].SetEvent(ui.__mem_func__(self.ChooseSlot), 4)

			self.GetChild("btn_login_new0").SetEvent(ui.__mem_func__(self.ChooseSlot), 0)
			self.GetChild("btn_login_new1").SetEvent(ui.__mem_func__(self.ChooseSlot), 1)
			self.GetChild("btn_login_new2").SetEvent(ui.__mem_func__(self.ChooseSlot), 2)
			self.GetChild("btn_login_new3").SetEvent(ui.__mem_func__(self.ChooseSlot), 3)
			self.GetChild("btn_login_new4").SetEvent(ui.__mem_func__(self.ChooseSlot), 4)

			self.GetChild("btn_delete_new0").SetEvent(ui.__mem_func__(self.InputPrivateCode), 0)
			self.GetChild("btn_delete_new1").SetEvent(ui.__mem_func__(self.InputPrivateCode), 1)
			self.GetChild("btn_delete_new2").SetEvent(ui.__mem_func__(self.InputPrivateCode), 2)
			self.GetChild("btn_delete_new3").SetEvent(ui.__mem_func__(self.InputPrivateCode), 3)
			self.GetChild("btn_delete_new4").SetEvent(ui.__mem_func__(self.InputPrivateCode), 4)
		except:
			import exception
			exception.Abort("SelectCharacterWindow.py ## __LoadScript.SetEvent")

	def ChooseSlot(self, slot):
		self.__SelectSlot(slot, True)
		self.ClickRadioButton(self.Characters, slot)

	def ClickRadioButton(self, buttonList, buttonIndex):
		try:
			Button = buttonList[buttonIndex]
		except IndexError:
			return
		for eachButton in buttonList:
			eachButton.SetUp()
		Button.Down()

	def __Close(self):
		self.stream.SetLoginPhase()

	def __DirectEnterNow(self):
		if self.openLoadingFlag:
			return
		chrSlot = self.stream.GetCharacterSlot()
		net.DirectEnter(chrSlot)
		self.openLoadingFlag = True
		playTime = net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME)
		import player
		player.SetPlayTime(playTime)
		import chat
		chat.Clear()

	def __Connect(self):
		cid = net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_ID)
		if not cid:
			return
		self.stream.SetCharacterSlot(self.current_slot)
		self.slot = self.current_slot
		self.startIndex = self.current_slot
		self.startReservingTime = app.GetTime()
		chr.PushOnceMotion(chr.MOTION_INTRO_SELECTED, 0.1)

	def __MakeCharacter(self, name, race, form, hair):
		chr.CreateInstance(0)
		chr.SelectInstance(0)
		chr.SetVirtualID(0)
		chr.SetNameString(name)

		chr.SetRace(race)
		chr.SetArmor(form)
		chr.SetHair(hair)
		chr.SetCanSelect(0)

		chr.Refresh()
		chr.SetMotionMode(chr.MOTION_MODE_GENERAL)
		chr.SetLoopMotion(chr.MOTION_INTRO_WAIT)

		chr.SetRotation(0.0)

	def __SetSlotRaceVisual(self, i, race):
		race = int(race)
		if race == 0:
			self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_warrior_m_01.png")
			self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_warrior_m_02.png")
			self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_warrior_m_02.png")
		elif race == 1:
			self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_ninja_w_01.png")
			self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_ninja_w_02.png")
			self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_ninja_w_02.png")
		elif race == 2:
			self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_sura_m_01.png")
			self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_sura_m_02.png")
			self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_sura_m_02.png")
		elif race == 3:
			self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_shaman_w_01.png")
			self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_shaman_w_02.png")
			self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_shaman_w_02.png")
		elif race == 4:
			self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_warrior_w_01.png")
			self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_warrior_w_02.png")
			self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_warrior_w_02.png")
		elif race == 5:
			self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_ninja_m_01.png")
			self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_ninja_m_02.png")
			self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_ninja_m_02.png")
		elif race == 6:
			self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_sura_w_01.png")
			self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_sura_w_02.png")
			self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_sura_w_02.png")
		elif race == 7:
			self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_shaman_m_01.png")
			self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_shaman_m_02.png")
			self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_shaman_m_02.png")
		else:
			self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_empty_01.png")
			self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_empty_02.png")
			self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_empty_02.png")

	def LoadBoardInfo(self):
		for i in xrange(PLAYER_SLOT_COUNT):
			cid = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID)
			if cid != 0:
				race = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_RACE)
				self.__SetSlotRaceVisual(i, race)
			else:
				self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_empty_01.png")
				self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_empty_02.png")
				self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_empty_02.png")

	def LoadBoardInfoDelete(self, slot):
		for i in xrange(PLAYER_SLOT_COUNT):
			id = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID)
			if id == 0:
				self.Characters[i].SetUpVisual(IMAGE_PATH + "btn_empty_01.png")
				self.Characters[i].SetOverVisual(IMAGE_PATH + "btn_empty_02.png")
				self.Characters[i].SetDownVisual(IMAGE_PATH + "btn_empty_02.png")

	def __SelectSlot(self, direction, chosebtn=False):
		max_slot = PLAYER_SLOT_COUNT - 1
		if not chosebtn:
			if (self.current_slot + direction) < 0:
				self.current_slot = max_slot
			elif (self.current_slot + direction) > max_slot:
				self.current_slot = 0
			else:
				self.current_slot += direction
		else:
			self.current_slot = direction

		self.slot = self.current_slot
		self.LoadBoardInfoDelete(self.current_slot)

		chr.DeleteInstance(0)
		id = net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_ID)

		if id:
			self.btn_login.Show()
			self.btn_login.SetTop()

			self.__MakeCharacter(
				net.GetAccountCharacterSlotDataString(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_NAME),
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_RACE),
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_FORM),
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_HAIR),
			)
			time, level, race, vit_v, int_v, str_v, dex_v, name, guild = (
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_PLAYTIME),
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_LEVEL),
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_RACE),
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_HTH),
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_INT),
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_STR),
				net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_DEX),
				net.GetAccountCharacterSlotDataString(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_NAME),
				net.GetAccountCharacterSlotDataString(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_GUILD_NAME),
			)
		else:
			self.btn_login.Hide()
			time, level, race, vit_v, int_v, str_v, dex_v, name, guild = (0, 0, 0, 0, 0, 0, 0, "-/-", "-/-")
		if guild != "":
			self.text_guildname.SetText(guild)
		else:
			self.text_guildname.SetText("-/-")

		for i in xrange(PLAYER_SLOT_COUNT):
			cid = net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID)
			if cid != 0:
				names_i = net.GetAccountCharacterSlotDataString(i, net.ACCOUNT_CHARACTER_SLOT_NAME)
				getattr(self, "text_playername_new{}".format(i)).SetText(names_i)
				getattr(self, "btn_create_new{}".format(i)).Hide()
				getattr(self, "btn_login_new{}".format(i)).Show()
				getattr(self, "btn_delete_new{}".format(i)).Show()
			else:
				getattr(self, "text_playername_new{}".format(i)).SetText("")
				getattr(self, "btn_create_new{}".format(i)).SetEvent(ui.__mem_func__(self.__CreateCharacter))
				getattr(self, "btn_create_new{}".format(i)).Show()
				getattr(self, "btn_login_new{}".format(i)).Hide()
				getattr(self, "btn_delete_new{}".format(i)).Hide()

		self.text_timevalue.SetText(str(time) + " min.")
		self.text_levelvalue.SetText(str(level))

		states_sum = float(vit_v + int_v + str_v + dex_v)
		max_states = 90
		calc_vit = (float(vit_v) * 10 / max_states)
		new_calc_vit = abs(calc_vit - vit_v / 10.0)
		calc_int = (float(int_v) * 10 / max_states)
		new_calc_int = abs(calc_int - int_v / 10.0)
		calc_str = (float(str_v) * 10 / max_states)
		new_calc_str = abs(calc_str - str_v / 10.0)
		calc_dex = (float(dex_v) * 10 / max_states)
		new_calc_dex = abs(calc_dex - dex_v / 10.0)

		self.gauge_vit.SetPercentage(new_calc_vit, 1.0)
		self.gauge_int.SetPercentage(new_calc_int, 1.0)
		self.gauge_str.SetPercentage(new_calc_str, 1.0)
		self.gauge_dex.SetPercentage(new_calc_dex, 1.0)

		self.ClickRadioButton(self.Characters, self.current_slot)

	def OnKeyDown(self, key):
		if 203 == key:
			self.__SelectSlot(-1)
		if 205 == key:
			self.__SelectSlot(1)
		if 28 == key:
			id = net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_ID)
			if 0 == id:
				self.__CreateCharacter()
			else:
				self.__Connect()
		if 1 == key:
			self.stream.SetLoginPhase()

		return True

	def __AreAllSlotEmpty(self):
		for i in xrange(PLAYER_SLOT_COUNT):
			if 0 != net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID):
				return 0
		return 1

	def GetCharacterSlotID(self, slotIndex):
		return net.GetAccountCharacterSlotDataInteger(slotIndex, net.ACCOUNT_CHARACTER_SLOT_ID)

	def __CreateCharacter(self):
		for i in xrange(PLAYER_SLOT_COUNT):
			if 0 == self.GetCharacterSlotID(i):
				self.ChooseSlot(i)
				self.stream.SetCharacterSlot(i)
				if self.__AreAllSlotEmpty():
					self.stream.SetReselectEmpirePhase()
				else:
					self.stream.SetCreateCharacterPhase()
				return

	def EmptyFunc(self):
		pass

	def __ClickBack(self):
		self.stream.SetLoginPhase()

	def __ClickExit(self):
		self.stream.SetLoginPhase()

	def OnPressExitKey(self):
		self.stream.SetLoginPhase()

	def InputPrivateCode(self, slot):
		import uiCommon
		privateInputBoard = uiCommon.InputDialogWithDescription()
		privateInputBoard.SetTitle(localeInfo.INPUT_PRIVATE_CODE_DIALOG_TITLE)
		privateInputBoard.SetAcceptEvent(ui.__mem_func__(self.AcceptInputPrivateCode))
		privateInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrivateCode))
		privateInputBoard.SetSecretMode()
		privateInputBoard.SetMaxLength(7)
		privateInputBoard.SetBoardWidth(250)
		privateInputBoard.SetDescription(localeInfo.INPUT_PRIVATE_CODE_DIALOG_DESCRIPTION)
		privateInputBoard.Open()
		self.privateInputBoard = privateInputBoard
		self.current_slot = slot

	def AcceptInputPrivateCode(self):
		privateCode = self.privateInputBoard.GetText()
		if not privateCode:
			return

		id = net.GetAccountCharacterSlotDataInteger(self.current_slot, net.ACCOUNT_CHARACTER_SLOT_ID)
		if 0 == id:
			self.PopupMessage(localeInfo.SELECT_EMPTY_SLOT)
			return

		net.SendDestroyCharacterPacket(self.current_slot, privateCode)
		self.PopupMessage(localeInfo.SELECT_DELEING)

		self.CancelInputPrivateCode()
		return True

	def CancelInputPrivateCode(self):
		self.privateInputBoard = None
		return True

	def OnDeleteSuccess(self, slot):
		self.PopupMessage(localeInfo.SELECT_DELETED)
		self.DeleteCharacter(slot)

	def OnDeleteFailure(self):
		self.PopupMessage(localeInfo.SELECT_CAN_NOT_DELETE)

	def DeleteCharacter(self, index):
		chr.DeleteInstance(0)
		self._slot_sig = None
		self.LoadBoardInfo()
		self.__SelectSlot(0, True)

	def PopupMessage(self, msg, func=0):
		if not func:
			func = self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def OnUpdate(self):
		chr.Update()
		chr.EffectUpdate()
		sig = tuple(net.GetAccountCharacterSlotDataInteger(i, net.ACCOUNT_CHARACTER_SLOT_ID) for i in xrange(PLAYER_SLOT_COUNT))
		if sig != self._slot_sig:
			self._slot_sig = sig
			self.LoadBoardInfo()
			self.__SelectSlot(self.current_slot, True)
			self.ClickRadioButton(self.Characters, self.current_slot)

		if self.startIndex != -1 and not self.openLoadingFlag:
			if app.GetTime() - self.startReservingTime > SELECT_ENTER_DELAY_SEC:
				self.__DirectEnterNow()
