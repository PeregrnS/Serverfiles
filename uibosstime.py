import ui, wndMgr, renderTarget, uiToolTip, localeInfo

BOSS_NAME = 0
BOSS_VNUM = 1
REB_INFO = 2
AREA_INFO = 3
SEC_INFO = 4
CH_INFO = 5
RENDER_TARGET_INDEX = 2

class BossTime(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.button_x = [
			# boss name  - vnum - saat - bolge ------------- korumali  -- kanallar
			[["The Start", 9668, 30, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["Orc Valley", 9176, 10, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["Yongi Desert", 9136, 10, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["Nord Wind", 9200, 10, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["Fire Land", 9201, 10, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["Grotto", 9199, 2, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["Palm Desert", 9198, 10, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["Fire Land 2", 9194, 10, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["Yongi Desert 2", 9138, 10, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["The Forest", 9195, 10, "Exact position on the minimap", "No", "(1,2,3,4)"]],
			[["The Sky Stage", 9192, 10, "Exact position on the minimap", "No", "(1,2,3,4)"]],
		]

		self.buttons = {}
		self.button_s = []
		self.hourInfo = { "icon" : [], "hour" : [] }
		self.select = 0
		self.isLoaded = FALSE
		if FALSE == self.isLoaded:
			self.LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.ClearDictionary()
		self.buttons = {}
		self.button_s = []
		self.hourInfo = { "icon" : [], "hour" : [] }
		self.select = 0

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/bosstime.py")
		except:
			import exception
			exception.Abort("UiBossTime.LoadWindow")
		try:
			self.bosstext = self.GetChild("bosstext")
			self.areaText = self.GetChild("areaText")
			self.secText = self.GetChild("secText")
			self.chText = self.GetChild("chText")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

			count = 0
			for a in self.button_x:
				self.buttons[count] = ui.RadioButton()
				self.buttons[count].SetParent(self)
				self.buttons[count].SetPosition(15,33+ count*30)
				self.buttons[count].SetUpVisual("d:/ymir work/ui/bossbilgi/select_btn_01.png")
				self.buttons[count].SetOverVisual("d:/ymir work/ui/bossbilgi/select_btn_02.png")
				self.buttons[count].SetDownVisual("d:/ymir work/ui/bossbilgi/select_btn_03.png")
				self.buttons[count].SetText(a[0][0])
				self.buttons[count].SetEvent(self.__Procces,count, a[0][1])
				self.buttons[count].Show()
				count +=1

			for i in range(len(self.button_x)):
				self.button_s.append(self.buttons[i])

			self.__Procces(0, self.button_x[0][0][BOSS_VNUM])
			self.isLoaded = True
		except:
			import exception
			exception.Abort("UiBossTime.LoadWindow")

	def __Procces(self, index, boss):
		self.select = index
		for btn in self.button_s:
			btn.SetUp()
		self.button_s[index].Down()
		self.areaText.SetText(str(self.button_x[self.select][0][AREA_INFO]))
		self.secText.SetText(str(self.button_x[self.select][0][SEC_INFO]))
		self.chText.SetText(str(self.button_x[self.select][0][CH_INFO]))
		renderTarget.SelectModel(RENDER_TARGET_INDEX, boss)

		self.hourInfo["icon"][:] = []
		self.hourInfo["hour"][:] = []

		# Dynamische Blöcke
		column_limit = 8  # Maximal 8 Zeilen pro Spalte
		for i in range(int(24 * 60 / self.button_x[self.select][0][REB_INFO])):
			info = ui.TextLine()
			info.SetParent(self)
		
			minutes = i * self.button_x[self.select][0][REB_INFO]
			hours = minutes // 60
			mins = minutes % 60
		
			# Berechnung von Spalte und Zeile
			column = i // column_limit
			row = i % column_limit
		
			# Position berechnen
			x_offset = 175 + (column * 50)
			y_offset = 155 + (row * 24)
		
			# Setze die Position für das Textobjekt
			info.SetPosition(x_offset, y_offset)
			time_text = "{:02d}:{:02d}".format(hours, mins)
			info.SetPackedFontColor(0xff00e5ee)
			info.SetText(time_text)
			info.Show()
		
			# Positionierung des Icons
			clock = ui.ExpandedImageBox()
			clock.SetParent(self)
			clock.SetPosition(x_offset - 10, y_offset - 5)
			clock.LoadImage("bossbilgi/hour.png")
			clock.Show()
		
			self.hourInfo["hour"].append(info)
			self.hourInfo["icon"].append(clock)

	def Open(self):
		self.SetCenterPosition()
		ui.ScriptWindow.Show(self)
		renderTarget.SetBackground(RENDER_TARGET_INDEX, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
		renderTarget.SetVisibility(RENDER_TARGET_INDEX, True)
		renderTarget.SelectModel(RENDER_TARGET_INDEX, self.button_x[self.select][0][BOSS_VNUM])

	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def OnPressExitKey(self):
		self.Close()
		return True

	def Close(self):
		wndMgr.Hide(self.hWnd)
		self.SetCenterPosition()
		renderTarget.SetVisibility(RENDER_TARGET_INDEX, False)