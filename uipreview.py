import ui
import chr
import net
import item
import wndMgr
import app
import nonplayer
from playerSettingModule import JOB_WARRIOR, JOB_ASSASSIN, JOB_SURA, JOB_SHAMAN, JOB_MAX_NUM

class PreviewWindow(ui.ScriptWindow):

	TYPE_ITEM = 0
	TYPE_CHARACTER = 1

	LIGHT_POSITION_DEFAULT = (50.0, 150.0, 350.0)
	LIGHT_POSITION_MOUNT = (50.0, -30.0, 440.0)

	ROTATE_FACTOR = 0.5

	DISTANCE_MIN = 1000
	DISTANCE_DEFAULT = 1500
	DISTANCE_MAX = 2800
	DISTANCE_FACTOR = 35.0

	RENDER_VAL_RACE = 0

	# TOGGLE_SHINING_WEAPON_SWORD = 0
	# TOGGLE_SHINING_WEAPON_DAGGER = 1
	# TOGGLE_SHINING_WEAPON_DAGGER_LEFT = 2
	# TOGGLE_SHINING_WEAPON_BOW = 3
	# TOGGLE_SHINING_WEAPON_FAN = 4
	# TOGGLE_SHINING_WEAPON_BELL = 5

	#################################################
	## Main FUNCTIONS
	#################################################

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self._selected_job = JOB_WARRIOR
		self._selected_sex = chr.RaceToSex(net.GetMainActorRace())
		self._render_type = -1
		self._render_values = {}
		self._render_data = {}
		self._rotation_cursor = None
		self._last_added_value = None
#		self._light_data = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/PreviewWindow.py")
		except:
			import exception
			exception.Abort("PreviewWindow.LoadDialog.LoadScript")

		try:
			GetObject=self.GetChild
			self.board = GetObject("Board")

			self.main = {
				"render" : GetObject("renderer"),
				"job" : [GetObject("job_%s" % job_name) for job_name in ("warrior", "assassin", "sura", "shaman")],
				"sex" : {
					0 : GetObject("sex_female"),
					1 : GetObject("sex_male"),
				},
			}

		except:
			import exception
			exception.Abort("PreviewWindow.LoadDialog.BindObject")

		self.board.SetCloseEvent(self.Close)

#		for coord in ("x", "y", "z"):
#			self.GetChild("light_%s_plus" % coord).SetEvent(self.__ChangeLight, coord, 10)
#			self.GetChild("light_%s_minus" % coord).SetEvent(self.__ChangeLight, coord, -10)

		self.main["render"].SetMouseLeftButtonDownEvent(self.__StartRotate)
		self.main["render"].SetOnMouseLeftButtonUpEvent(self.__EndRotate)

		for i in xrange(JOB_MAX_NUM):
			self.main["job"][i].SetEvent(self.__SelectJob, i)
		for sex in self.main["sex"]:
			self.main["sex"][sex].SetEvent(self.__SelectSex, sex)
		
		# if app.ENABLE_MOUSE_WHEEL_EVENT:
		# 	self.SetScrollWheelEvent(self.OnWheelMove)

	def Destroy(self):
		self.Close()
		self.main["render"].Destroy()

		ui.ScriptWindow.Destroy(self)

		self.board = None
		self.main = None

	def Open(self, type, value):
		if not self.IsValidType(type, value):
			return False

		if not self.IsShow() or self._render_type != type:
			self._selected_job = chr.RaceToJob(net.GetMainActorRace())
			self._selected_sex = chr.RaceToSex(net.GetMainActorRace())
			self._render_values = {}

		self._render_type = type
		self.__AppendRenderValue(value)

		if not self.IsShow():
			self.main["render"].SetRenderDistance(self.DISTANCE_DEFAULT)

		self.Refresh()

		self.Show()
		self.SetTop()

	def Close(self):
		self.__EndRotate()
		self._render_data = {}
		self._render_values = {}

		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def OnUpdateLockedCursor(self, xdif, ydif):
		self.__Rotate(xdif, ydif)

#	def __ChangeLight(self, coord, amount):
#		if self._light_data is None:
#			x, y, z = self.main["render"].GetLightPosition()
#			self._light_data = {
#				"x" : x,
#				"y" : y,
#				"z" : z,
#			}

#		self._light_data[coord] += amount
#		self.main["render"].SetLightPosition(self._light_data["x"], self._light_data["y"], self._light_data["z"])

#		self.GetChild("light_%s_desc" % coord).SetText(coord + ": " + str(int(self._light_data[coord])))

	#################################################
	## Data FUNCTIONS
	#################################################

	def IsValidType(self, type, value):
		if type == self.TYPE_ITEM:
			item.SelectItem(value)

			allow_types = {
				item.ITEM_TYPE_WEAPON: (
					item.WEAPON_SWORD,
					item.WEAPON_DAGGER,
					item.WEAPON_BOW,
					item.WEAPON_TWO_HANDED,
					item.WEAPON_BELL,
					item.WEAPON_FAN,
				),
				item.ITEM_TYPE_ARMOR: (
					item.ARMOR_BODY,
				),
				item.ITEM_TYPE_COSTUME: (
					item.COSTUME_TYPE_BODY,
					item.COSTUME_TYPE_HAIR,
					item.COSTUME_TYPE_WEAPON,
					item.COSTUME_TYPE_ACCE,
				),
			#	item.ITEM_TYPE_TOGGLE: (
			#		item.TOGGLE_SHINING,
			#	),
			}

			if item.GetItemType() not in allow_types:
				return False

			if item.GetItemSubType() not in allow_types[item.GetItemType()]:
				return False

			return True

		elif type == self.TYPE_CHARACTER:
			if not nonplayer.MonsterExists(value):
				return False

			return True

		return False

	def __AppendRenderValue(self, value):
		if self._render_type == self.TYPE_ITEM:
			item.SelectItem(value)

			if item.ITEM_TYPE_WEAPON == item.GetItemType() or (item.ITEM_TYPE_COSTUME == item.GetItemType() and item.COSTUME_TYPE_WEAPON == item.GetItemSubType()):
				self._render_values[chr.PART_WEAPON] = value
				self._last_added_value = chr.PART_WEAPON
			elif item.ITEM_TYPE_ARMOR == item.GetItemType() or (item.ITEM_TYPE_COSTUME == item.GetItemType() and item.COSTUME_TYPE_BODY == item.GetItemSubType()):
				self._render_values[chr.PART_MAIN] = value
				self._last_added_value = chr.PART_MAIN
			elif item.ITEM_TYPE_COSTUME == item.GetItemType() and item.COSTUME_TYPE_HAIR == item.GetItemSubType():
				self._render_values[chr.PART_HAIR] = value
				self._last_added_value = chr.PART_HAIR
			elif item.ITEM_TYPE_COSTUME == item.GetItemType() and item.GetItemSubType() in (item.COSTUME_TYPE_ACCE):
				self._render_values[chr.PART_ACCE] = value
				self._last_added_value = chr.PART_ACCE
			# elif item.ITEM_TYPE_TOGGLE == item.GetItemType() and item.TOGGLE_SHINING == item.GetItemSubType():
			# 	self._render_values['shining'] = value
			# 	self._last_added_value = 'shining'

		elif self._render_type == self.TYPE_CHARACTER:
			self._render_values['race'] = value
			self._last_added_value = 'race'

	def __GetRenderedRace(self):
		if "race" in self._render_data:
			return self._render_data["race"]

		return 0

	def __IsRenderedMain(self):
		return self.__GetRenderedRace() == net.GetMainActorRace()

	def __GetMainPart(self, part):
		return chr.GetPart(0, part)

	def __GetRenderedItemType(self, part):
		if self._render_type != self.TYPE_ITEM:
			return 0

		if part not in self._render_values or self._render_values[part] == 0:
			return 0

		item.SelectItem(self._render_values[part])
		return item.GetItemType()

	def __GetAvailableJobs(self):
		ret = [JOB_WARRIOR, JOB_SURA, JOB_ASSASSIN, JOB_SHAMAN]

		if self._render_type == self.TYPE_ITEM:
			antiflags = [
				(item.ITEM_ANTIFLAG_WARRIOR, JOB_WARRIOR),
				(item.ITEM_ANTIFLAG_SURA, JOB_SURA),
				(item.ITEM_ANTIFLAG_ASSASSIN, JOB_ASSASSIN),
				(item.ITEM_ANTIFLAG_SHAMAN, JOB_SHAMAN),
			]

			for part in self._render_values:
				item.SelectItem(self._render_values[part])

				for flagpair in antiflags:
					if item.IsAntiFlag(flagpair[0]) and flagpair[1] in ret:
						ret.remove(flagpair[1])

		elif self._render_type == self.TYPE_CHARACTER:
			del ret[:]

		return ret

	def __GetAvailableSex(self):
		ret = [0, 1]

		if self._render_type == self.TYPE_ITEM:
			for part in self._render_values:
				item.SelectItem(self._render_values[part])

				if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE) and 1 in ret:
					ret.remove(1)
				if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE) and 0 in ret:
					ret.remove(0)

		else:
			del ret[:]

		return ret

	def OnMouseWheel(self, len):
		change = self.DISTANCE_FACTOR * (len / 120)
		distance = self.main["render"].GetRenderDistance() + change

		if distance < self.DISTANCE_MIN:
			distance = self.DISTANCE_MIN
		elif distance > self.DISTANCE_MAX:
			distance = self.DISTANCE_MAX

		self.main["render"].SetRenderDistance(distance)
		return TRUE

	#################################################
	## Render FUNCTIONS
	#################################################

	def __RenderSinglePart(self, part, func):
		if part in self._render_values:
			value = self._render_values[part]
		elif self.__IsRenderedMain():
			value = self.__GetMainPart(part)
		else:
			value = 0

		func(value)

	def __RenderHair(self):
		if item.ITEM_TYPE_COSTUME == self.__GetRenderedItemType(chr.PART_HAIR) and item.COSTUME_TYPE_HAIR == item.GetItemSubType():
			hair = item.GetValue(3)
		elif self.__IsRenderedMain():
			hair = self.__GetMainPart(chr.PART_HAIR)
		else:
			hair = 0

		self.main["render"].SetHair(hair)

	def __RenderAcce(self):
		self.__RenderSinglePart(chr.PART_ACCE, self.main["render"].SetAcce)

	def __RenderArmor(self):
		self.__RenderSinglePart(chr.PART_MAIN, self.main["render"].SetArmor)

	def __RenderWeapon(self):
		self.__RenderSinglePart(chr.PART_WEAPON, self.main["render"].SetWeapon)

	# def __ConvertShiningFlagByWeapon(self, effectID):
	# 	effectCount = 1

	# 	weaponVnum = self.main["render"].GetWeapon()
	# 	item.SelectItem(weaponVnum)

	# 	if item.GetItemType() == item.ITEM_TYPE_WEAPON:
	# 		if item.GetItemSubType() == item.WEAPON_DAGGER:
	# 			effectCount = 2
	# 			effectID += self.TOGGLE_SHINING_WEAPON_DAGGER
	# 		elif item.GetItemSubType() == item.WEAPON_BOW:
	# 			effectID += self.TOGGLE_SHINING_WEAPON_BOW
	# 		elif item.GetItemSubType() == item.WEAPON_FAN:
	# 			effectID += self.TOGGLE_SHINING_WEAPON_FAN
	# 		elif item.GetItemSubType() == item.WEAPON_BELL:
	# 			effectID += self.TOGGLE_SHINING_WEAPON_BELL
	# 		else:
	# 			effectID += self.TOGGLE_SHINING_WEAPON_SWORD

	# 	return (effectID, effectCount)

	# def __RefreshShining(self):
		# if 'shining' in self._render_values:
		# 	value = self._render_values['shining']
		# else:
		# 	value = 0

		# if value:
		# 	item.SelectItem(value)
		# 	effectID = item.GetValue(3)
		# 	weaponEffect = item.GetValue(4) == 1

		# else:
		# 	effectID = 0
		# 	weaponEffect = False

		# effectList = []

		# if effectID > 0:
		# 	effectCount = 1
		# 	if weaponEffect:
		# 		effectID, effectCount = self.__ConvertShiningFlagByWeapon(effectID)

		# 	effectID -= 1
		# 	dwordBitSize = 4 * 8

		# 	for i in xrange(effectCount):
		# 		effectList.append({
		# 			"idx" : (effectID + i) / dwordBitSize,
		# 			"flag" : 1 << ((effectID + i) % dwordBitSize),
		# 		})

		# shiningFlags = [0 for i in xrange(item.TOGGLE_SHINING_FLAG_32CNT)]
		# for effect in effectList:
		# 	shiningFlags[effect["idx"]] |= effect["flag"]

#		self.main["render"].SetToggleShining(*shiningFlags)

	#################################################
	## Event FUNCTIONS
	#################################################

	def __SelectJob(self, job):
		if self._selected_job == job:
			return

		self._selected_job = job
		self.Refresh()

	def __SelectSex(self, sex):
		if self._selected_sex == sex:
			return

		self._selected_sex = sex
		self.Refresh()

	def __StartRotate(self):
		mouse_x, mouse_y = wndMgr.GetMousePosition()
		self._rotation_cursor = app.GetCursor()

		app.SetCursor(app.CAMERA_ROTATE)
		if app.GetCursorMode() == app.CURSOR_MODE_HARDWARE:
			app.HideCursor(True)

		self.LockCursor()

	def __Rotate(self, xdif, ydif):
		if xdif == 0:
			return

		rotate_velocity = xdif * self.ROTATE_FACTOR
		self._render_data["rotation"] += rotate_velocity
		self.RefreshRenderRotation()

	def __EndRotate(self):
		if self._rotation_cursor is None:
			return

		app.SetCursor(self._rotation_cursor)
		if app.GetCursorMode() == app.CURSOR_MODE_HARDWARE:
			app.ShowCursor()

		self.UnlockCursor()

		self._rotation_cursor = None

	#################################################
	## Refresh FUNCTIONS
	#################################################

	def RefreshJob(self):
		avail_jobs = self.__GetAvailableJobs()

		for i in xrange(JOB_MAX_NUM):
			btn = self.main["job"][i]
			if i not in avail_jobs:
				btn.Disable()
				continue

			btn.Enable()
			if i == self._selected_job:
				btn.Down()
			else:
				btn.SetUp()

	def RefreshSex(self):
		avail_sex = self.__GetAvailableSex()

		for sex in self.main["sex"]:
			btn = self.main["sex"][sex]
			if sex not in avail_sex:
				btn.Disable()
				continue

			btn.Enable()
			if sex == self._selected_sex:
				btn.Down()
			else:
				btn.SetUp()

	def RefreshRender(self):
		while 1:
			avail_jobs = self.__GetAvailableJobs()
			if self._selected_job not in avail_jobs:
				if len(avail_jobs) > 0:
					self._selected_job = avail_jobs[0]

			avail_sex = self.__GetAvailableSex()
			selected_sex = self._selected_sex
			if selected_sex not in avail_sex:
				if len(avail_sex) > 0:
					selected_sex = avail_sex[0]

			if len(avail_jobs) > 0 and len(avail_sex) > 0:
				break

			if self._render_type != self.TYPE_ITEM or len(self._render_values) <= 1:
				break

			for key in self._render_values:
				if key != self._last_added_value:
					del self._render_values[key]
					break

		self._selected_sex = selected_sex
		
		race = -1
		if self._render_type == self.TYPE_ITEM:
			race = chr.JobToRace(self._selected_job, selected_sex)
		elif self._render_type == self.TYPE_CHARACTER:
			race = self._render_values["race"]

		if race < 0:
			return

		self.main["render"].SetRenderTarget(race)

		light_position = self.LIGHT_POSITION_DEFAULT
		if self._render_type == self.TYPE_CHARACTER and race >= 20110 and race <= 20299: # is_mount
			light_position = self.LIGHT_POSITION_MOUNT
		self.main["render"].SetLightPosition(*light_position)

		self._render_data["race"] = race

		if self._render_type == self.TYPE_ITEM:
			self.__RenderHair()
			self.__RenderArmor()
			self.__RenderWeapon()
			self.__RenderAcce()
			# self.__RefreshShining()

		self.RefreshRenderRotation()
		self.RefreshJob()
		self.RefreshSex()

	def RefreshRenderRotation(self):
		if not "rotation" in self._render_data:
			self._render_data["rotation"] = 0.0

		self.main["render"].SetRotation(self._render_data["rotation"])

	def Refresh(self):
		self.RefreshRender()
		
	def OnMoveWindow(self, x, y):
		if self.GetLeft() < 0:
			self.SetPosition(0, self.GetTop())
		elif self.GetRight() > wndMgr.GetScreenWidth():
			self.SetPosition(wndMgr.GetScreenWidth() - self.GetWidth(), self.GetTop())

		if self.GetTop() < 0:
			self.SetPosition(self.GetLeft(), 0)
		elif self.GetBottom() > wndMgr.GetScreenHeight():
			self.SetPosition(self.GetLeft(), wndMgr.GetScreenHeight() - self.GetHeight())