import app
import ui
import background
import constInfo

MAX_SKYBOX_COUNT = 13

class SkyBoxWindow(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = False
		self.titleBar = 0
		self.skyBoxList = []

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def Destroy(self):
		self.ClearDictionary()
		self.titleBar = 0
		self.skyBoxList = []

	def __Load_LoadScript(self, fileName):
		pyScriptLoader = ui.PythonScriptLoader()
		pyScriptLoader.LoadScriptFile(self, fileName)

	def __Load_BindObject(self):
		GetObject = self.GetChild
		self.titleBar = GetObject("titlebar")
		for i in xrange(MAX_SKYBOX_COUNT):
			self.skyBoxList.append(GetObject("sky_button_%d" % i))

	def __Load_BindEvent(self):
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		for i in xrange(MAX_SKYBOX_COUNT):
			self.skyBoxList[i].SetEvent(ui.__mem_func__(self.__OnClickSkyboxSelectButton), i)

	def __LoadWindow(self):
		if self.isLoaded:
			return

		self.isLoaded = True

		try:
			self.__Load_LoadScript("UIScript/SkyBoxWindow.py")
		except:
			import exception
			exception.Abort("SkyBoxWindow.__Load_LoadScript")

		try:
			self.__Load_BindObject()
		except:
			import exception
			exception.Abort("SkyBoxWindow.__Load_BindObject")


		try:
			self.__Load_BindEvent()
		except:
			import exception
			exception.Abort("SkyBoxWindow.__Load_BindEvent")

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			skyButton = buttonList[buttonIndex]
		except IndexError:
			return

		for eachButton in buttonList:
			eachButton.SetUp()

		skyButton.Down()

	def OnSetSkyBox(self, index):
		self.__ClickRadioButton(self.skyBoxList, index)

	def __OnClickSkyboxSelectButton(self, index):
		if index != 0:
			background.RegisterEnvironmentData(index, constInfo.SKYBOX_PATH[index])
		background.SetEnvironmentData(index)
		# if app.ENABLE_FOG_FIX:
		# 	if background.GetFogMode():
		# 		background.SetFogMode(True)
		# 	else:
		# 		background.SetFogMode(False)
		self.OnSetSkyBox(index)
		constInfo.SetConfigIni("skybox", "index", index)

	def Open(self):
		self.__LoadWindow()

		self.OnSetSkyBox(constInfo.GetSelectedSkyBox())

		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True
