import ui
import player
import item
import app
import renderTarget
import renderTargetInfo

class RenderTargetWindow(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.RENDER_TARGET_INDEX = 1
		self.modelRenderer = None
		# self.modelRotateImage = None
		self.modelRotation = True
		# self.Initialize()

		self.__LoadDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def DisplayUser(self, race = 0, weaponVnum = 0, armorVnum = 0, hairVnum = 0, acceVnum = 0, auraVnum = 0, shiningVnum = 0, bChangeMotion = False, bCheckPosition = False, bCheckHairPosition = True, skillColor = 0):
		renderTarget.SetBackground(self.RENDER_TARGET_INDEX, "d:/ymir work/ui/game/myshop_deco/render_target.tga")
		renderTarget.SetVisibility(self.RENDER_TARGET_INDEX, True)
		renderTarget.SelectModel(self.RENDER_TARGET_INDEX, race)
		renderTarget.ModelViewReset(self.RENDER_TARGET_INDEX)
		
		if skillColor:
			renderTarget.MouseZoom(self.RENDER_TARGET_INDEX, False)
			renderTarget.MouseZoom(self.RENDER_TARGET_INDEX, False)
			renderTarget.MouseZoom(self.RENDER_TARGET_INDEX, False)
			
		if bCheckPosition and renderTargetInfo.NPC_CAMERA_CFG.has_key(race):
			(V3Eye, V3Target) = renderTargetInfo.GetNpcCamera(race)
			if len(V3Eye) and len(V3Target):
				renderTarget.SetEye(self.RENDER_TARGET_INDEX, *V3Eye)
				renderTarget.SetTarget(self.RENDER_TARGET_INDEX, *V3Target)

		if app.ENABLE_WOLFMAN_CHARACTER:
			if race > 8:
				return
		else:
			if race > 7:
				return

		currentRace = player.GetRace()
		
		if armorVnum != 0:
			renderTarget.SetArmor(self.RENDER_TARGET_INDEX, armorVnum)
		else:
			if currentRace == race:
				if player.GetItemIndex(item.COSTUME_SLOT_START) == 0:
					renderTarget.SetArmor(self.RENDER_TARGET_INDEX, player.GetArmor())
				else:
					renderTarget.SetArmor(self.RENDER_TARGET_INDEX, player.GetItemIndex(item.COSTUME_SLOT_BODY))
			else:
				renderTarget.SetArmor(self.RENDER_TARGET_INDEX, 0)

		if weaponVnum != 0:
			"""
			# if renderTargetInfo.WEAPONS_CAMERA_CFG.has_key(weaponVnum):
			(V3Eye, V3Target) = renderTargetInfo.GetWeaponCamera(weaponVnum)
			if len(V3Eye) and len(V3Target):
				renderTarget.SetEye(self.RENDER_TARGET_INDEX, *V3Eye)
				renderTarget.SetTarget(self.RENDER_TARGET_INDEX, *V3Target)
			"""
			renderTarget.SetWeapon(self.RENDER_TARGET_INDEX, weaponVnum)
		else:
			if currentRace == race:
				if  player.GetItemIndex(item.COSTUME_SLOT_WEAPON) == 0:
					renderTarget.SetWeapon(self.RENDER_TARGET_INDEX, player.GetWeapon())
				else:
					renderTarget.SetWeapon(self.RENDER_TARGET_INDEX, player.GetItemIndex(item.COSTUME_SLOT_WEAPON))
			else:
				renderTarget.SetWeapon(self.RENDER_TARGET_INDEX, 0)

		if hairVnum != 0:
			(V3Eye, V3Target) = renderTargetInfo.GetHairCamera(race)
			if len(V3Eye) and len(V3Target) and bCheckHairPosition:
				renderTarget.SetEye(self.RENDER_TARGET_INDEX, *V3Eye)
				renderTarget.SetTarget(self.RENDER_TARGET_INDEX, *V3Target)
			renderTarget.SetHair(self.RENDER_TARGET_INDEX, hairVnum)
		else:
			equippedHair = player.GetItemIndex(item.COSTUME_SLOT_HAIR)
			if equippedHair != 0:
				item.SelectItem(equippedHair)
				if currentRace == race:
					renderTarget.SetHair(self.RENDER_TARGET_INDEX, item.GetValue(3))

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if acceVnum != 0:
				renderTarget.SetAcce(self.RENDER_TARGET_INDEX, acceVnum)
			else:
				if currentRace == race:
					if  player.GetItemIndex(item.COSTUME_SLOT_ACCE) == 0:
						renderTarget.SetAcce(self.RENDER_TARGET_INDEX, player.GetAcce())
					else:
						renderTarget.SetAcce(self.RENDER_TARGET_INDEX, player.GetItemIndex(item.COSTUME_SLOT_ACCE))
				else:
					renderTarget.SetAcce(self.RENDER_TARGET_INDEX, 0)
		
		if skillColor:
			item.SelectItem(skillColor)
			renderTarget.SetUseSkillWithEffect(self.RENDER_TARGET_INDEX, item.GetValue(3), item.GetValue(4))

		if app.ENABLE_SHINING_ITEM_SYSTEM:
			if shiningVnum != 0:
				renderTarget.SetShining(self.RENDER_TARGET_INDEX, shiningVnum)
			else:
				for i in xrange(item.SHINING_SLOT_COUNT):
					renderTarget.SetShining(self.RENDER_TARGET_INDEX, player.GetItemIndex(item.SHINING_SLOT_START + i))

	def __LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/RenderTargetWindow.py")
		except:
			import exception
			exception.Abort("RenderTargetWindow.LoadDialog.LoadScript")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.board = self.GetChild("board")
			self.renderRotationButton = self.GetChild("RenderRotationButton") ## TODO
		except:
			import exception
			exception.Abort("RenderTargetWindow.LoadDialog.BindObject")

		self.modelRenderer =  ui.RenderTarget()
		self.modelRenderer.SetParent(self.board)
		self.modelRenderer.SetSize(390,400)
		self.modelRenderer.SetPosition(10, 30)
		self.modelRenderer.SetRenderTarget(self.RENDER_TARGET_INDEX)
		self.modelRenderer.Show()

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.renderRotationButton.SetEvent(ui.__mem_func__(self.__ModelSetRotation))
		
		self.SetCenterPosition()

	def Destroy(self):
		self.Close()

	def Open(self):
		self.Show()
		self.SetTop()

	def Close(self):
		self.Hide()

	def __ModelSetRotation(self):
		if self.modelRotation:
			renderTarget.SetAutoRotate(self.RENDER_TARGET_INDEX, False)
			self.modelRotation = False
		else:
			renderTarget.SetAutoRotate(self.RENDER_TARGET_INDEX, True)
			self.modelRotation = True

	def __ModelViewReset(self):
		if self.IsShow() == False:
			renderTarget.ModelViewReset(self.RENDER_TARGET_INDEX)

	def OnPressEscapeKey(self):
		self.Close()
		return True
