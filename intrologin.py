# -*- coding: utf-8 -*-
# fratello_exe login screen v1.0

import os
import app
import net
import ui
import snd
import wndMgr
import ime
import musicInfo
import systemSetting
import localeInfo
import constInfo
import serverInfo
import _winreg

REG_PATH = r"SOFTWARE\Fratello_Work\Hardcore"

HOMEPAGE_LINK = "https://www.hardcore.de"
DISCORD_LINK = "https://discord.gg"

DEV_HOST_FALLBACK = "127.0.0.1"
DEV_NAME_FALLBACK = "Hardcore DEV"


def SetReg(name, value):
	try:
		_winreg.CreateKey(_winreg.HKEY_CURRENT_USER, REG_PATH)
		key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, REG_PATH, 0, _winreg.KEY_WRITE)
		_winreg.SetValueEx(key, name, 0, _winreg.REG_SZ, value)
		_winreg.CloseKey(key)
		return True
	except WindowsError:
		return False


def GetReg(name):
	try:
		key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, REG_PATH, 0, _winreg.KEY_READ)
		value, _type = _winreg.QueryValueEx(key, name)
		_winreg.CloseKey(key)
		return str(value)
	except WindowsError:
		return None


class LoginWindow(ui.ScriptWindow):

	def __init__(self, stream):
		ui.ScriptWindow.__init__(self)

		net.SetPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(self)

		self.stream = stream
		self.isLoaded = False
		self.connecting = False

		self.channelButtons = []
		self.channelStateImages = []
		self.channelCount = 7
		self.channelIndex = -1

		self.serverKey = 0
		self.serverButtons = {}
		self.serverName = ""
		self.serverHost = ""
		self.serverAuthPort = 0
		self.serverChannelPorts = [0] * 7

		self.nextChannelStateCheck = 0.0

		self.languageList = []
		self.fadeState = 0
		self.fadeStart = 0.0
		self.fadeDuration = 0.22
		self.fadeStepDelay = 0.03
		self.fadeTargets = []
		self.fadeControls = []
		self.fadeActiveTargets = []
		self.pendingLoginInfo = None
		self.connectStarted = False

		self.langFlagTR = None
		self.langFlagEN = None
		self.langFlagDE = None
		self.langFlagRO = None

	def __del__(self):
		net.ClearPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(0)
		ui.ScriptWindow.__del__(self)

	def Open(self):

		if not self.isLoaded:
			self.__LoadUI()
			self.__BindEvents()
			self.isLoaded = True

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("LoginWindow")

		self.__InitAudio()
		self.__InitIME()

		self.channelIndex = -1
		for b in self.channelButtons:
			try:
				b.SetUp()
			except:
				pass

		self.serverKey = self.__LoadSavedServerKey()
		self.__SelectServer(self.serverKey, False)

		self.RefreshAccounts()

		self.Show()
		app.ShowCursor()

	def Close(self):

		self.connecting = False

		if musicInfo.loginMusic:
			snd.FadeOutMusic("BGM/" + musicInfo.loginMusic)

		if self.stream.popupWindow:
			self.stream.popupWindow.Close()

		self.Hide()
		app.HideCursor()
		ime.ClearExceptKey()

	def OnUpdate(self):
		self.__UpdatePlaceholders()
		self.__UpdateChannelStateDots()
		self.__KeepSelectedChannelDown()
		self.__KeepSelectedServerDown()

	def __Noop(self):
		pass

	def __InitAudio(self):

		if musicInfo.loginMusic:
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/" + musicInfo.loginMusic)

		snd.SetSoundVolume(systemSetting.GetSoundVolume())

	def __InitIME(self):

		ime.AddExceptKey(91)
		ime.AddExceptKey(93)

	def __OpenURL(self, url):
		try:
			if hasattr(app, "OpenWebPage"):
				app.OpenWebPage(url)
				return
		except:
			pass
		try:
			os.system('@echo off && explorer "%s"' % url)
		except:
			pass

	def __LoadSavedServerKey(self):
		v = GetReg("server_key")
		try:
			n = int(v)
		except:
			n = 0
		if n not in (0, 1):
			n = 0
		return n

	def __SaveServerKey(self, key):
		SetReg("server_key", str(key))

	def __BuildPortsFromSrvDict(self, srvDict):
		ports = []
		for i in xrange(1, 8):
			k = "ch%d" % i
			if k in srvDict:
				ports.append(int(srvDict[k]))
			else:
				ports.append(int(srvDict.get("ch1", srvDict.get("auth", 0))))
		while len(ports) < 7:
			ports.append(ports[0] if ports else 0)
		return ports[:7]

	def __GetNormalProfile(self):
		name = ""
		host = ""
		auth = 0
		ports = [0] * 7

		try:
			name = serverInfo.SRV1["name"]
		except:
			name = "Server"

		try:
			host = serverInfo.SRV1["host"]
		except:
			host = "127.0.0.1"

		try:
			auth = int(serverInfo.SRV1["auth"])
		except:
			try:
				auth = int(serverInfo.SRV_LIST[0][0][1])
			except:
				auth = 0

		try:
			ports = self.__BuildPortsFromSrvDict(serverInfo.SRV1)
		except:
			ports = [0] * 7

		return name, host, auth, ports

	def __GetDevProfile(self):
		if hasattr(serverInfo, "DEV_SRV1"):
			try:
				srv = serverInfo.DEV_SRV1
				name = srv.get("name", DEV_NAME_FALLBACK)
				host = srv.get("host", DEV_HOST_FALLBACK)
				auth = int(srv.get("auth", 0))
				ports = self.__BuildPortsFromSrvDict(srv)
				return name, host, auth, ports
			except:
				pass

		nName, _nHost, nAuth, nPorts = self.__GetNormalProfile()
		return DEV_NAME_FALLBACK, DEV_HOST_FALLBACK, nAuth, nPorts

	def __SelectServer(self, key, popup):

		if key not in (0, 1):
			key = 0

		self.serverKey = key
		self.__SaveServerKey(key)

		if key == 1:
			self.serverName, self.serverHost, self.serverAuthPort, self.serverChannelPorts = self.__GetDevProfile()
		else:
			self.serverName, self.serverHost, self.serverAuthPort, self.serverChannelPorts = self.__GetNormalProfile()

		try:
			btnNormal = self.serverButtons.get("normal")
			btnDev = self.serverButtons.get("dev")
			if btnNormal:
				btnNormal.SetUp()
			if btnDev:
				btnDev.SetUp()
			if key == 1 and btnDev:
				btnDev.Down()
			elif btnNormal:
				btnNormal.Down()
		except:
			pass

		self.channelIndex = -1
		for b in self.channelButtons:
			try:
				b.SetUp()
			except:
				pass

		self.__ApplyConnectInfo(0)

		if popup:
			if key == 1:
				self.__Popup("DEV server selected.")
			else:
				self.__Popup("Hardcore server selected.")

	def __LoadUI(self):

		loader = ui.PythonScriptLoader()
		loader.LoadScriptFile(self, "fratello_work/login/fratello_loginwindow.py")

		self.idEdit = self.GetChild("id")
		self.pwdEdit = self.GetChild("pwd")
		self.loginButton = self.GetChild("login_button")

		if app.ENABLE_PIN_LOGIN:
			self.pinEdit = self.GetChild("pin")
		else:
			self.pinEdit = None

		self.idPlaceholder = self.GetChild("id_placeholder")
		self.pwdPlaceholder = self.GetChild("pwd_placeholder")

		if app.ENABLE_PIN_LOGIN:
			self.pinPlaceholder = self.GetChild("pin_placeholder")
		else:
			self.pinPlaceholder = None

		self.channelButtons = [
			self.GetChild("select_btn_ch1"),
			self.GetChild("select_btn_ch2"),
			self.GetChild("select_btn_ch3"),
			self.GetChild("select_btn_ch4"),
			self.GetChild("select_btn_ch5"),
			self.GetChild("select_btn_ch6"),
			self.GetChild("select_btn_ch7"),
		]

		self.channelStateImages = [
			self.GetChild("ch1_state"),
			self.GetChild("ch2_state"),
			self.GetChild("ch3_state"),
			self.GetChild("ch4_state"),
			self.GetChild("ch5_state"),
			self.GetChild("ch6_state"),
			self.GetChild("ch7_state"),
		]

		for b in self.channelButtons:
			try:
				b.Show()
			except:
				pass

		for i in self.channelStateImages:
			try:
				i.Show()
			except:
				pass

		self.serverButtons = {}
		try:
			self.serverButtons["normal"] = self.GetChild("server_list_normal")
		except:
			self.serverButtons["normal"] = None
		try:
			self.serverButtons["dev"] = self.GetChild("server_list_dev")
		except:
			self.serverButtons["dev"] = None
		try:
			self.serverButtons["website"] = self.GetChild("server_list_website")
		except:
			self.serverButtons["website"] = None
		try:
			self.serverButtons["discord"] = self.GetChild("server_list_discord")
		except:
			self.serverButtons["discord"] = None

		self.CH_DOT_RED = "fratello_work/login/ui/red.png"
		self.CH_DOT_YELLOW = "fratello_work/login/ui/yellow.png"
		self.CH_DOT_GREEN = "fratello_work/login/ui/green.png"

		self.nextChannelStateCheck = 0.0
		self.__RefreshChannelStates()

		self.accountData = {}
		for i in xrange(7):
			try:
				deleteBtn = self.GetChild("save_account%d_delete" % (i+1))
				saveBtn = self.GetChild("save_account%d_button" % (i+1))
				textLine = self.GetChild("save_account%d_text" % (i+1))
			except:
				continue
			self.accountData[i] = [deleteBtn, saveBtn, textLine]

		self.__LoadLocaleList()

		try:
			self.langFlagTR = self.GetChild("lang_flag_tr")
		except:
			self.langFlagTR = None

		try:
			self.langFlagEN = self.GetChild("lang_flag_en")
		except:
			self.langFlagEN = None

		try:
			self.langFlagDE = self.GetChild("lang_flag_de")
		except:
			self.langFlagDE = None

		try:
			self.langFlagRO = self.GetChild("lang_flag_ro")
		except:
			self.langFlagRO = None

	def __BindEvents(self):

		self.loginButton.SetEvent(ui.__mem_func__(self.__OnClickLoginButton))

		for idx, btn in enumerate(self.channelButtons):
			try:
				btn.SetEvent(ui.__mem_func__(self.SetChannel), idx)
				btn.SetUp()
			except:
				pass

		if self.serverButtons.get("normal"):
			self.serverButtons["normal"].SetEvent(ui.__mem_func__(self.__OnClickServerNormal))
		if self.serverButtons.get("dev"):
			self.serverButtons["dev"].SetEvent(ui.__mem_func__(self.__OnClickServerDev))
		if self.serverButtons.get("website"):
			self.serverButtons["website"].SetEvent(ui.__mem_func__(self.__OnClickWebsite))
		if self.serverButtons.get("discord"):
			self.serverButtons["discord"].SetEvent(ui.__mem_func__(self.__OnClickDiscord))

		self.idEdit.SetTabEvent(ui.__mem_func__(self.pwdEdit.SetFocus))
		self.idEdit.SetReturnEvent(ui.__mem_func__(self.pwdEdit.SetFocus))

		if app.ENABLE_PIN_LOGIN:
			self.pwdEdit.SetTabEvent(ui.__mem_func__(self.pinEdit.SetFocus))
			self.pwdEdit.SetReturnEvent(ui.__mem_func__(self.pinEdit.SetFocus))

			self.pinEdit.SetTabEvent(ui.__mem_func__(self.idEdit.SetFocus))
			self.pinEdit.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))
		else:
			self.pwdEdit.SetTabEvent(ui.__mem_func__(self.idEdit.SetFocus))
			self.pwdEdit.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))

		self.idEdit.SetFocus()

		for key in self.accountData:
			self.accountData[key][0].SetEvent(ui.__mem_func__(self.DeleteAccount), key)
			self.accountData[key][1].SetEvent(ui.__mem_func__(self.SaveAccount), key)

		for key in self.accountData:
			parent = self.GetChild("save_account%d" % (key+1))
			parent.SetEvent(ui.__mem_func__(self.__OnClickAccountSlot), key)

		if self.langFlagTR:
			self.langFlagTR.SetEvent(ui.__mem_func__(self.__OnClickLanguage), 0)

		if self.langFlagEN:
			self.langFlagEN.SetEvent(ui.__mem_func__(self.__OnClickLanguage), 1)

		if self.langFlagDE:
			self.langFlagDE.SetEvent(ui.__mem_func__(self.__OnClickLanguage), 2)

		if self.langFlagRO:
			self.langFlagRO.SetEvent(ui.__mem_func__(self.__OnClickLanguage), 3)

	def __OnClickServerNormal(self):
		self.__SelectServer(0, True)

	def __OnClickServerDev(self):
		self.__SelectServer(1, True)

	def __OnClickWebsite(self):
		self.__OpenURL(HOMEPAGE_LINK)

	def __OnClickDiscord(self):
		self.__OpenURL(DISCORD_LINK)

	def __UpdatePlaceholders(self):

		if self.idEdit.GetText():
			self.idPlaceholder.Hide()
		else:
			self.idPlaceholder.Show()

		if self.pwdEdit.GetText():
			self.pwdPlaceholder.Hide()
		else:
			self.pwdPlaceholder.Show()

		if app.ENABLE_PIN_LOGIN and self.pinPlaceholder:
			if self.pinEdit.GetText():
				self.pinPlaceholder.Hide()
			else:
				self.pinPlaceholder.Show()

	def __UpdateChannelStateDots(self):

		now = app.GetTime()
		if now < self.nextChannelStateCheck:
			return

		self.nextChannelStateCheck = now + 1.0
		self.__RefreshChannelStates()

	def __RefreshChannelStates(self):

		for idx in xrange(len(self.channelStateImages)):
			img = self.channelStateImages[idx]
			if not img:
				continue
			img.LoadImage(self.CH_DOT_GREEN)

	def __KeepSelectedChannelDown(self):
		if self.channelIndex < 0:
			return
		if self.channelIndex >= len(self.channelButtons):
			return
		try:
			self.channelButtons[self.channelIndex].Down()
		except:
			pass

	def __KeepSelectedServerDown(self):
		try:
			if self.serverKey == 1:
				if self.serverButtons.get("dev"):
					self.serverButtons["dev"].Down()
			else:
				if self.serverButtons.get("normal"):
					self.serverButtons["normal"].Down()
		except:
			pass

	def __ApplyConnectInfo(self, ch):

		self.stream.SetConnectInfo(
			self.serverHost,
			self.ChannelPort(ch, 0),
			self.serverHost,
			self.ChannelPort("LOGIN")
		)

		net.SetMarkServer(self.serverHost, self.ChannelPort("LOGO"))
		net.SetServerInfo(self.ChannelPort(ch, 2))

	def SetChannel(self, ch):

		if ch < 0 or ch >= self.channelCount:
			return

		if self.channelIndex == ch:
			return

		if 0 <= self.channelIndex < len(self.channelButtons):
			try:
				self.channelButtons[self.channelIndex].SetUp()
			except:
				pass

		self.channelIndex = ch

		try:
			self.channelButtons[ch].Down()
		except:
			pass

		self.__ApplyConnectInfo(ch)

	def ChannelPort(self, ch, value=0):

		if ch == "LOGIN":
			return self.serverAuthPort

		if ch == "LOGO":
			try:
				return int(self.serverChannelPorts[0])
			except:
				return self.serverAuthPort

		if value == 2:
			return "%s, CH%s" % (self.serverName, (ch + 1))

		try:
			return int(self.serverChannelPorts[ch])
		except:
			try:
				return int(self.serverChannelPorts[0])
			except:
				return self.serverAuthPort

	def Connect(self, id, pwd, pin=None):

		if constInfo.SEQUENCE_PACKET_ENABLE:
			net.SetPacketSequenceMode()

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_CONNETING, self.__Noop, localeInfo.UI_CANCEL)

		if app.ENABLE_PIN_LOGIN:
			self.stream.SetLoginInfo(id, pwd, pin)
		else:
			self.stream.SetLoginInfo(id, pwd)

		self.stream.Connect()

	def __OnClickLoginButton(self):

		if self.connecting:
			return

		self.__ApplyConnectInfo(0 if self.channelIndex < 0 else self.channelIndex)

		id = self.idEdit.GetText()
		pwd = self.pwdEdit.GetText()

		if len(id) == 0:
			self.__Popup(localeInfo.LOGIN_INPUT_ID)
			return

		if len(pwd) == 0:
			self.__Popup(localeInfo.LOGIN_INPUT_PASSWORD)
			return

		if app.ENABLE_PIN_LOGIN:
			pin = self.pinEdit.GetText()
			if len(pin) == 0:
				self.__Popup(localeInfo.LOGIN_INPUT_PIN)
				return
		else:
			pin = None

		self.connecting = True
		self.Connect(id, pwd, pin)

	def OnLoginFailure(self, error):

		self.connecting = False

		loginFailureMsgDict = {
			"ALREADY": localeInfo.LOGIN_FAILURE_ALREAY,
			"NOID": localeInfo.LOGIN_FAILURE_NOT_EXIST_ID,
			"WRONGPWD": localeInfo.LOGIN_FAILURE_WRONG_PASSWORD,
			"FULL": localeInfo.LOGIN_FAILURE_TOO_MANY_USER,
			"SHUTDOWN": localeInfo.LOGIN_FAILURE_SHUTDOWN,
			"REPAIR": localeInfo.LOGIN_FAILURE_REPAIR_ID,
			"BLOCK": localeInfo.LOGIN_FAILURE_BLOCK_ID,
			"WRONGMAT": localeInfo.LOGIN_FAILURE_WRONG_MATRIX_CARD_NUMBER,
			"QUIT": localeInfo.LOGIN_FAILURE_WRONG_MATRIX_CARD_NUMBER_TRIPLE,
			"BESAMEKEY": localeInfo.LOGIN_FAILURE_BE_SAME_KEY,
			"NOTAVAIL": localeInfo.LOGIN_FAILURE_NOT_AVAIL,
			"NOBILL": localeInfo.LOGIN_FAILURE_NOBILL,
			"BLKLOGIN": localeInfo.LOGIN_FAILURE_BLOCK_LOGIN,
			"WEBBLK": localeInfo.LOGIN_FAILURE_WEB_BLOCK,
			"BADSCLID": localeInfo.LOGIN_FAILURE_WRONG_SOCIALID,
			"AGELIMIT": localeInfo.LOGIN_FAILURE_SHUTDOWN_TIME,
		}

		if app.ENABLE_PIN_LOGIN:
			loginFailureMsgDict["WRONGPIN"] = localeInfo.LOGIN_FAILURE_WRONG_PIN_CODE

		message = loginFailureMsgDict.get(error, localeInfo.LOGIN_FAILURE_UNKNOWN + " (" + str(error) + ")")

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(message, 0, localeInfo.UI_OK)

		snd.PlaySound("sound/ui/loginfail.wav")

	def OnConnectFailure(self):
		self.connecting = False
		self.__Popup(localeInfo.LOGIN_CONNECT_FAILURE)

	def OnLoginStart(self):
		self.PopupDisplayMessage(localeInfo.LOGIN_PROCESSING)

	def OnHandShake(self):
		snd.PlaySound("sound/ui/loginok.wav")
		self.PopupDisplayMessage(localeInfo.LOGIN_CONNECT_SUCCESS)

	def OnPressEscapeKey(self):

		if self.connecting:
			net.Disconnect()
			self.connecting = False
			if self.stream.popupWindow:
				self.stream.popupWindow.Close()
			return True

		self.stream.SetPhaseWindow(0)
		return True

	def OnPressExitKey(self):
		return self.OnPressEscapeKey()

	def __Popup(self, message):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(message, 0, localeInfo.UI_OK)

	def PopupDisplayMessage(self, msg):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg)

	def SaveAccount(self, key):
		id = self.idEdit.GetText()
		pwd = self.pwdEdit.GetText()

		if not id or not pwd:
			self.__Popup("Fill the ID PW PIN")
			return

		if GetReg("id_%d" % key):
			self.__Popup("Slot is Full.")
			return

		SetReg("id_%d" % key, id)
		SetReg("pwd_%d" % key, pwd)

		if app.ENABLE_PIN_LOGIN:
			SetReg("pin_%d" % key, self.pinEdit.GetText())

		self.RefreshAccounts()
		self.__Popup("Account Saved.")

	def DeleteAccount(self, key):
		if not GetReg("id_%d" % key):
			return

		SetReg("id_%d" % key, "")
		SetReg("pwd_%d" % key, "")
		SetReg("pin_%d" % key, "")

		self.RefreshAccounts()
		self.__Popup("Account deleted.")

	def RefreshAccounts(self):
		for key in self.accountData:
			id = GetReg("id_%d" % key)
			deleteBtn, saveBtn, textLine = self.accountData[key]

			if id:
				textLine.SetFontName("Tahoma:12")
				textLine.SetText(id)
				saveBtn.Hide()
				deleteBtn.Show()
			else:
				textLine.SetFontName("Tahoma:12")
				textLine.SetText("Save")
				saveBtn.Show()
				deleteBtn.Hide()

	def __OnClickAccountSlot(self, key):
		id = GetReg("id_%d" % key)

		if not id:
			new_id = self.idEdit.GetText()
			new_pwd = self.pwdEdit.GetText()

			if not new_id or not new_pwd:
				self.__Popup("Fill the ID PW PIN")
				return

			SetReg("id_%d" % key, new_id)
			SetReg("pwd_%d" % key, new_pwd)

			if app.ENABLE_PIN_LOGIN:
				SetReg("pin_%d" % key, self.pinEdit.GetText())

			self.RefreshAccounts()
			self.__Popup("Account Saved.")
			return

		pwd = GetReg("pwd_%d" % key) or ""
		pin = GetReg("pin_%d" % key) or ""

		self.idEdit.SetText(id)
		self.pwdEdit.SetText(pwd)

		if app.ENABLE_PIN_LOGIN:
			self.pinEdit.SetText(pin)

		if self.connecting:
			return

		self.connecting = True

		if app.ENABLE_PIN_LOGIN:
			self.Connect(id, pwd, pin)
		else:
			self.Connect(id, pwd)

	def __LoadLocaleList(self):

		self.languageList = []

		try:
			with open("locale_list.txt", "rt") as f:
				lines = f.readlines()
		except:
			return

		for line in lines:
			line = line.strip()
			if not line:
				continue

			tokens = line.split()

			if len(tokens) != 3:
				continue

			name, codepage, locale = tokens

			self.languageList.append({
				"name": name,
				"codepage": codepage,
				"locale": locale.lower().strip()
			})

	def __OnClickLanguage(self, index):

		if index < 0 or index >= len(self.languageList):
			return

		lang = self.languageList[index]

		try:
			f = open("loca.cfg", "wt")
			f.write("%s %s" % (lang["codepage"], lang["locale"]))
			f.close()
		except:
			return

		app.SetReloadLocale(lang["locale"] != app.GetLocaleName())

		os.system('start hardcore.exe')
		app.Exit()