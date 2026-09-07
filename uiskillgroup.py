import ui
import uiScriptLocale
import net
import app

ROOT_PATH = "d:/ymir work/ui/game/skillgroup/"

GROUP_NAMES = {
	0	:	[uiScriptLocale.SKILL_GROUP_WARRIOR_1, uiScriptLocale.SKILL_GROUP_WARRIOR_2],
	1	:	[uiScriptLocale.SKILL_GROUP_ASSASSIN_1, uiScriptLocale.SKILL_GROUP_ASSASSIN_2],
	2	:	[uiScriptLocale.SKILL_GROUP_SURA_1, uiScriptLocale.SKILL_GROUP_SURA_2],
	3	:	[uiScriptLocale.SKILL_GROUP_SHAMAN_1, uiScriptLocale.SKILL_GROUP_SHAMAN_2],
}
if app.ENABLE_WOLFMAN_CHARACTER:
	GROUP_NAMES[4] = [uiScriptLocale.SKILL_GROUP_WOLFMAN_1,	""]

GROUP_NAMES_COLORS = {
	0	:	[0xff4bcd81, 0xff62b2e5],
	1	:	[0xffbdba44, 0xff8a31a1],
	2	:	[0xffa85831, 0xffb65fe7],
	3	:	[0xff9a2f5f, 0xffa38036],
}
if app.ENABLE_WOLFMAN_CHARACTER:
	GROUP_NAMES_COLORS[4] = [0xffffcc00, 0xffffcc00]

class SkillGroupWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = False

	def __LoadWindow(self, raceNum):
		if self.isLoaded == True:
			return

		self.isLoaded = True
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/SkillGroupWindow.py")
		except:
			import exception
			exception.Abort("SkillGroupWindow.LoadWindow.LoadObject")
			
		try:
			self.groupBackground = self.GetChild("GroupBackGround")
			self.textGroup1 = self.GetChild("TextGroup1")
			self.textGroup2 = self.GetChild("TextGroup2")
			self.selectGroup1 = self.GetChild("SelectGroup1")
			self.selectGroup2 = self.GetChild("SelectGroup2")
			self.closeButton = self.GetChild("CloseButton")
		except:
			import exception
			exception.Abort("SkillGroupWindow.LoadWindow.BindObject")

		self.groupBackground.LoadImage(ROOT_PATH + "%d.tga" % raceNum)

		self.textGroup1.SetText(GROUP_NAMES[raceNum][0])
		self.textGroup1.SetPackedFontColor(GROUP_NAMES_COLORS[raceNum][0])
		self.textGroup2.SetText(GROUP_NAMES[raceNum][1])
		self.textGroup2.SetPackedFontColor(GROUP_NAMES_COLORS[raceNum][1])

		self.selectGroup1.SetEvent(ui.__mem_func__(self.SelectGroup), 1)
		self.selectGroup2.SetEvent(ui.__mem_func__(self.SelectGroup), 2)

		self.closeButton.SetEvent(ui.__mem_func__(self.Close))

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Open(self, raceNum):
		self.__LoadWindow(raceNum)
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def SelectGroup(self, job):
		net.SendSkillGroupPacket(job)
		self.Close()

	def OnPressEscapeKey(self):
		self.Close()
		return True
