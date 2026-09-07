import ui
import uiScriptLocale
import wndMgr
import player
import miniMap
import localeInfo
import net
import app
import colorInfo
import constInfo
import background
import time

# if app.ENABLE_BIOLOG_SYSTEM:
# 	import biologmgr

if app.ENABLE_ATLAS_MOVE_TO_POS:
	import chr

if app.ENABLE_ATLASS_EXTENDED:
	import time

from constInfo import TextColor

class MapTextToolTip(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)

		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetHorizontalAlignCenter()
		textLine.SetOutline()
		textLine.SetHorizontalAlignRight()
		textLine.Show()
		self.textLine = textLine

	def __del__(self):
		ui.Window.__del__(self)

	def SetText(self, text):
		self.textLine.SetText(text)

	def SetTooltipPosition(self, PosX, PosY):
		if localeInfo.IsARABIC():
			w, h = self.textLine.GetTextSize()
			self.textLine.SetPosition(PosX - w - 5, PosY)
		else:
			self.textLine.SetPosition(PosX - 5, PosY)

	def SetTextColor(self, TextColor):
		self.textLine.SetPackedFontColor(TextColor)

	def GetTextSize(self):
		return self.textLine.GetTextSize()

class AtlasWindow(ui.ScriptWindow):

	class AtlasRenderer(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.AddFlag("not_pick")
			if not app.ENABLE_ATLAS_MOVE_TO_POS:
				self.AddFlag("not_pick")

		def OnUpdate(self):
			miniMap.UpdateAtlas()

		def OnRender(self):
			(x, y) = self.GetGlobalPosition()
			fx = float(x)
			fy = float(y)
			miniMap.RenderAtlas(fx, fy)

		def HideAtlas(self):
			miniMap.HideAtlas()

		def ShowAtlas(self):
			miniMap.ShowAtlas()

	def __init__(self):
		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Hide()
		if app.ENABLE_ATLASS_EXTENDED:
			self.tooltipInfoRespawn = MapTextToolTip()
			self.tooltipInfoRespawn.Hide()
		self.infoGuildMark = ui.MarkBox()
		self.infoGuildMark.Hide()
		self.AtlasMainWindow = None
		self.mapName = ""
		self.board = 0

		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def SetMapName(self, mapName):
		if 949==app.GetDefaultCodePage():
			try:
				self.board.SetTitleName(localeInfo.MINIMAP_ZONE_NAME_DICT[mapName])
			except:
				pass

	def __GetAtlasScriptByMap(self):
		mapName = background.GetCurrentMapName()

		if mapName == "metin2_map_blaues_reich_map_1":
			return "UIScript/metin2_map_blaues_reich_map_1.py"
		elif mapName == "metin2_map_blaues_reich_map_2":
			return "UIScript/metin2_map_blaues_reich_map_2.py"
		elif mapName == "metin2_map_gelbes_reich_map_1":
			return "UIScript/metin2_map_gelbes_reich_map_1.py"
		elif mapName == "metin2_map_gelbes_reich_map_2":
			return "UIScript/metin2_map_gelbes_reich_map_2.py"
		elif mapName == "metin2_map_rotes_reich_map_1":
			return "UIScript/metin2_map_rotes_reich_map_1.py"
		elif mapName == "metin2_map_rotes_reich_map_2":
			return "UIScript/metin2_map_rotes_reich_map_2.py"
		elif mapName == "metin2_map_orktal":
			return "UIScript/metin2_map_orktal.py"
		elif mapName == "metin2_map_orktal2":
			return "UIScript/metin2_map_orktal2.py"
		elif mapName == "metin2_map_tempel":
			return "UIScript/metin2_map_tempel.py"
		elif mapName == "metin2_map_gautamakliff":
			return "UIScript/metin2_map_gautamakliff.py"
		elif mapName == "metin2_map_a1_snow":
			return "UIScript/metin2_map_a1_snow.py"
		elif mapName == "metin2_map_b1_snow":
			return "UIScript/metin2_map_b1_snow.py"
		elif mapName == "metin2_map_c1_snow":
			return "UIScript/metin2_map_c1_easter.py"
		elif mapName == "metin2_map_a1_easter":
			return "UIScript/metin2_map_a1_easter.py"
		elif mapName == "metin2_map_b1_easter":
			return "UIScript/metin2_map_b1_easter.py"
		elif mapName == "metin2_map_c1_easter":
			return "UIScript/metin2_map_c1_easter.py"
		elif mapName == "metin2_map_a1_halloween":
			return "UIScript/metin2_map_a1_halloween.py"
		elif mapName == "metin2_map_b1_halloween":
			return "UIScript/metin2_map_b1_halloween.py"
		elif mapName == "metin2_map_c1_halloween":
			return "UIScript/metin2_map_c1_halloween.py"


		# Default Atlas
		return "UIScript/AtlasWindow.py"



	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, self.__GetAtlasScriptByMap())
		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.LoadScript")


		try:
			self.board = self.GetChild("board")
			try:
				self.titleBar = self.GetChild("TitleBar")
				self.titleBar.SetCloseEvent(ui.__mem_func__(self.Hide))
			except:
				self.titleBar = None

		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.BindObject")

		self.AtlasMainWindow = self.AtlasRenderer()

		if app.ENABLE_ATLAS_MOVE_TO_POS:
			self.AtlasMainWindow.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.GoToLocalPosition))
		if hasattr(self.board, "SetCloseEvent"):
			self.board.SetCloseEvent(ui.__mem_func__(self.Hide))
		else:
			if self.titleBar and hasattr(self.titleBar, "SetCloseEvent"):
				self.titleBar.SetCloseEvent(ui.__mem_func__(self.Hide))
			else:
				for closeName in ("close_button", "CloseButton", "btn_close", "MiniMapCloseButton"):
					try:
						btn = self.GetChild(closeName)
						btn.SetEvent(ui.__mem_func__(self.Hide))
						break
					except:
						pass
		self.AtlasMainWindow.SetParent(self.board)
		self.AtlasMainWindow.SetPosition(7, 30)
		self.tooltipInfo.SetParent(self.board)
		if app.ENABLE_ATLASS_EXTENDED:
			self.tooltipInfoRespawn.SetParent(self.board)
		self.infoGuildMark.SetParent(self.board)
		self.SetPosition(wndMgr.GetScreenWidth() - 136 - 256 - 10, 0)
		self.Hide()

		miniMap.RegisterAtlasWindow(self)

	def Destroy(self):
		miniMap.UnregisterAtlasWindow()
		self.ClearDictionary()
		self.AtlasMainWindow = None
		self.tooltipAtlasClose = 0
		self.tooltipInfo = None
		if app.ENABLE_ATLASS_EXTENDED:
			self.tooltipInfoRespawn = None
		self.infoGuildMark = None
		self.board = None

	def OnUpdate(self):

		if not self.tooltipInfo:
			return

		if not self.infoGuildMark:
			return

		self.infoGuildMark.Hide()
		self.tooltipInfo.Hide()
		if app.ENABLE_ATLASS_EXTENDED:
			self.tooltipInfoRespawn.Hide()

		if False == self.board.IsIn():
			return

		(mouseX, mouseY) = wndMgr.GetMousePosition()
		if app.ENABLE_ATLASS_EXTENDED:
			(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID, dwRespTime) = miniMap.GetAtlasInfo(mouseX, mouseY)
		else:
			(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID) = miniMap.GetAtlasInfo(mouseX, mouseY)

		if False == bFind:
			if app.ENABLE_ATLAS_MOVE_TO_POS:
				if chr.IsGameMaster(player.GetMainCharacterIndex()):
					(iPosX, iPosY) = miniMap.MousePosToAtlasPos(mouseX, mouseY)
					dwTextColor = 0xffffffff
				else:
					return
			else:
				return

		if "empty_guild_area" == sName:
			sName = localeInfo.GUILD_EMPTY_AREA

		if app.ENABLE_ATLAS_MOVE_TO_POS:
			if bFind == False:
				self.tooltipInfo.SetText("%d, %d |Eemoji/key_lclick|e" % (iPosX, iPosY))
			elif localeInfo.IsARABIC() and sName[-1].isalnum():
				self.tooltipInfo.SetText("(%s)%d, %d" % (sName, iPosX, iPosY))
			else:
				self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
		else:
			if localeInfo.IsARABIC() and sName[-1].isalnum():
				self.tooltipInfo.SetText("(%s)%d, %d" % (sName, iPosX, iPosY))
			else:
				self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))

		(x, y) = self.GetGlobalPosition()
		self.tooltipInfo.SetTooltipPosition(mouseX - x, mouseY - y)
		self.tooltipInfo.SetTextColor(dwTextColor)
		self.tooltipInfo.Show()
		self.tooltipInfo.SetTop()

		if app.ENABLE_ATLASS_EXTENDED:
			if 0 != dwRespTime and self.tooltipInfoRespawn:
				self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
				self.tooltipInfoRespawn.SetText(localeInfo.MINIMAP_BOSS_RESPAWN_TOOLTIP.format(localeInfo.SecondToDHMS(dwRespTime)))
				self.tooltipInfoRespawn.SetTooltipPosition(mouseX - x, mouseY - y+14)
				self.tooltipInfoRespawn.SetTextColor(0xffff9124)
				self.tooltipInfoRespawn.Show()
				self.tooltipInfoRespawn.SetTop()
			else:
				self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))

		if 0 != dwGuildID:
			textWidth, textHeight = self.tooltipInfo.GetTextSize()
			self.infoGuildMark.SetIndex(dwGuildID)
			self.infoGuildMark.SetPosition(mouseX - x - textWidth - 18 - 5, mouseY - y)
			self.infoGuildMark.Show()

	def Hide(self):
		if self.AtlasMainWindow:
			self.AtlasMainWindow.HideAtlas()
			self.AtlasMainWindow.Hide()
		ui.ScriptWindow.Hide(self)

	def Show(self):
		if self.AtlasMainWindow:
			(bGet, iSizeX, iSizeY) = miniMap.GetAtlasSize()
			if bGet:
				self.SetSize(iSizeX + 15, iSizeY + 38)

				if localeInfo.IsARABIC():
					self.board.SetPosition(iSizeX+15, 0)

				self.board.SetSize(iSizeX + 15, iSizeY + 38)
				#self.AtlasMainWindow.SetSize(iSizeX, iSizeY)
				self.AtlasMainWindow.ShowAtlas()
				self.AtlasMainWindow.Show()
		ui.ScriptWindow.Show(self)

	def SetCenterPositionAdjust(self, x, y):
		self.SetPosition((wndMgr.GetScreenWidth() - self.GetWidth()) / 2 + x, (wndMgr.GetScreenHeight() - self.GetHeight()) / 2 + y)

	if app.ENABLE_ATLAS_MOVE_TO_POS:
		def GoToLocalPosition(self):
			if False == self.board.IsIn() or False == chr.IsGameMaster(player.GetMainCharacterIndex()):
				return

			(mouseX, mouseY) = wndMgr.GetMousePosition()
			(iPosX, iPosY) = miniMap.MousePosToAtlasPos(mouseX, mouseY)
			net.SendChatPacket("/goto %d %d" % (iPosX, iPosY))

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def OnPressExitKey(self):
		self.Hide()
		return True

def __RegisterMiniMapColor(type, rgb):
	miniMap.RegisterColor(type, rgb[0], rgb[1], rgb[2])

class MiniMap(ui.ScriptWindow):

	CANNOT_SEE_INFO_MAP_DICT = {
		"metin2_map_monkeydungeon" : False,
		"metin2_map_monkeydungeon_02" : False,
		"metin2_map_monkeydungeon_03" : False,
		"metin2_map_devilsCatacomb" : False,
		"metin2_map_worldboss1" : False,
		"metin2_map_laby" : False,
		"metin2_map_dungeonentry1" : False,
	}

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__Initialize()

		miniMap.Create()
		miniMap.SetScale(2.0)

		self.AtlasWindow = AtlasWindow()
		self.AtlasWindow.LoadWindow()
		self.AtlasWindow.Hide()

		self.tooltipMiniMapOpen = MapTextToolTip()
		self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP)
		self.tooltipMiniMapOpen.Show()
		self.tooltipMiniMapClose = MapTextToolTip()
		self.tooltipMiniMapClose.SetText(localeInfo.UI_CLOSE)
		self.tooltipMiniMapClose.Show()
		self.tooltipScaleUp = MapTextToolTip()
		self.tooltipScaleUp.SetText(localeInfo.MINIMAP_INC_SCALE)
		self.tooltipScaleUp.Show()
		self.tooltipScaleDown = MapTextToolTip()
		self.tooltipScaleDown.SetText(localeInfo.MINIMAP_DEC_SCALE)
		self.tooltipScaleDown.Show()
		self.tooltipAtlasOpen = MapTextToolTip()
		self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_SHOW_AREAMAP)
		self.tooltipAtlasOpen.Show()

		if app.ENABLE_EVENT_MANAGER:
			self.tooltipInGameEventButton = MapTextToolTip()
			self.tooltipInGameEventButton.SetText(uiScriptLocale.EVENT_ALARM_TITLE)
			self.tooltipInGameEventButton.Show()

		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.tooltipDungeonInfoOpen = MapTextToolTip()
			self.tooltipDungeonInfoOpen.SetText(localeInfo.DUNGEON_INFO_MINIMAP_TOOL_TIP)
			self.tooltipDungeonInfoOpen.Show()

		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Show()

		if miniMap.IsAtlas():
			self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_SHOW_AREAMAP)
		else:
			self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_CAN_NOT_SHOW_AREAMAP)

		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Show()

		self.mapName = ""

		self.isLoaded = 0
		self.canSeeInfo = True

		# AUTOBAN
		self.imprisonmentDuration = 0
		self.imprisonmentEndTime = 0
		self.imprisonmentEndTimeText = ""
		# END_OF_AUTOBAN

	def __del__(self):
		miniMap.Destroy()
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		self.positionInfo = 0
		self.dataTime = 0
		self.observerCount = 0

		self.OpenWindow = 0
		self.CloseWindow = 0
		self.ScaleUpButton = 0
		self.ScaleDownButton = 0
		self.MiniMapHideButton = 0
		self.MiniMapShowButton = 0
		self.AtlasShowButton = 0

		self.tooltipMiniMapOpen = 0
		self.tooltipMiniMapClose = 0
		self.tooltipScaleUp = 0
		self.tooltipScaleDown = 0
		self.tooltipAtlasOpen = 0
		self.tooltipInfo = None
		self.serverInfo = None

		self.interface = None

		# if app.ENABLE_EVENT_MANAGER:
		# 	self.InGameEventButton = 0
		# 	self.tooltipInGameEventButton = 0

		# if app.ENABLE_BIOLOG_SYSTEM:
		# 	self.BiologButton = 0

		# if app.ENABLE_INGAME_WIKI:
		# 	self.wikipediaButton = 0

		# if app.ENABLE_DUNGEON_INFO_SYSTEM:
		# 	self.DungeonInfoShowButton = 0

		# if app.ENABLE_BATTLE_PASS:
		# 	self.btnBattlePass = 0

	def SetMapName(self, mapName):
		self.mapName=mapName
		self.AtlasWindow.SetMapName(mapName)

		if self.CANNOT_SEE_INFO_MAP_DICT.has_key(mapName):
			self.canSeeInfo = False
			self.HideMiniMap()
			self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP_CANNOT_SEE)
		else:
			self.canSeeInfo = True
			self.ShowMiniMap()
			self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP)

	# AUTOBAN
	def SetImprisonmentDuration(self, duration):
		self.imprisonmentDuration = duration
		self.imprisonmentEndTime = app.GetGlobalTimeStamp() + duration

		self.__UpdateImprisonmentDurationText()

	def __UpdateImprisonmentDurationText(self):
		restTime = max(self.imprisonmentEndTime - app.GetGlobalTimeStamp(), 0)

		imprisonmentEndTimeText = localeInfo.SecondToDHM(restTime)
		if imprisonmentEndTimeText != self.imprisonmentEndTimeText:
			self.imprisonmentEndTimeText = imprisonmentEndTimeText
			self.serverInfo.SetText("%s: %s" % (uiScriptLocale.AUTOBAN_QUIZ_REST_TIME, self.imprisonmentEndTimeText))
	# END_OF_AUTOBAN

	def Show(self):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			if localeInfo.IsARABIC():
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "Minimap.py")
			else:
				pyScrLoader.LoadScriptFile(self, "UIScript/MiniMap.py")
		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.LoadScript")

		try:
			self.OpenWindow = self.GetChild("OpenWindow")
			self.MiniMapWindow = self.GetChild("MiniMapWindow")
			self.ScaleUpButton = self.GetChild("ScaleUpButton")
			self.ScaleDownButton = self.GetChild("ScaleDownButton")
			self.MiniMapHideButton = self.GetChild("MiniMapHideButton")
			self.AtlasShowButton = self.GetChild("AtlasShowButton")
			self.CloseWindow = self.GetChild("CloseWindow")
			self.MiniMapShowButton = self.GetChild("MiniMapShowButton")
			self.positionInfo = self.GetChild("PositionInfo")
			self.dataTime = self.GetChild("Datetime")
			self.observerCount = self.GetChild("ObserverCount")
			self.serverInfo = self.GetChild("ServerInfo")

			# if app.ENABLE_EVENT_MANAGER:
			# 	self.InGameEventButton = self.GetChild("InGameEventButton")
			# if app.ENABLE_BIOLOG_SYSTEM:
			# 	self.BiologButton = self.GetChild("biologButton")
			# if app.ENABLE_INGAME_WIKI:
			# 	self.wikipediaButton = self.GetChild("WikipediaButton")
			# if app.ENABLE_DUNGEON_INFO_SYSTEM:
			# 	self.DungeonInfoShowButton = self.GetChild("DungeonInfoShowButton")
			# if app.ENABLE_BATTLE_PASS:
			# 	self.btnBattlePass = self.GetChild("BattlePass")
		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.Bind")

		if constInfo.MINIMAP_POSITIONINFO_ENABLE==0:
			self.positionInfo.Hide()

		self.serverInfo.SetText(net.GetServerInfo())
		self.ScaleUpButton.SetEvent(ui.__mem_func__(self.ScaleUp))
		self.ScaleDownButton.SetEvent(ui.__mem_func__(self.ScaleDown))
		self.MiniMapHideButton.SetEvent(ui.__mem_func__(self.HideMiniMap))
		self.MiniMapShowButton.SetEvent(ui.__mem_func__(self.ShowMiniMap))

		# if app.ENABLE_EVENT_MANAGER:
		# 	self.InGameEventButton.SetEvent(ui.__mem_func__(self.ToggleInGameEvent))

		# if app.ENABLE_BIOLOG_SYSTEM:
		# 	self.BiologButton.SetEvent(ui.__mem_func__(self.__ToggleBiolog))

		# if app.ENABLE_INGAME_WIKI:
		# 	self.wikipediaButton.SetEvent(ui.__mem_func__(self.ToggleWikipedia))

		# if app.ENABLE_DUNGEON_INFO_SYSTEM:
		# 	self.DungeonInfoShowButton.SetEvent(ui.__mem_func__(self.ShowDungeonInfo))

		if miniMap.IsAtlas():
			self.AtlasShowButton.SetEvent(ui.__mem_func__(self.ToggleAtlasWindow)) # @fixme014 ShowAtlas

		self.RefreshTooltipPosition()

		self.ShowMiniMap()

	def RefreshTooltipPosition(self):
		if self.MiniMapShowButton and self.tooltipMiniMapOpen:
			(ButtonPosX, ButtonPosY) = self.MiniMapShowButton.GetGlobalPosition()
			self.tooltipMiniMapOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)

		if self.MiniMapHideButton and self.tooltipMiniMapClose:
			(ButtonPosX, ButtonPosY) = self.MiniMapHideButton.GetGlobalPosition()
			self.tooltipMiniMapClose.SetTooltipPosition(ButtonPosX, ButtonPosY)

		if self.ScaleUpButton and self.tooltipScaleUp:
			(ButtonPosX, ButtonPosY) = self.ScaleUpButton.GetGlobalPosition()
			self.tooltipScaleUp.SetTooltipPosition(ButtonPosX, ButtonPosY)

		if self.ScaleDownButton and self.tooltipScaleDown:
			(ButtonPosX, ButtonPosY) = self.ScaleDownButton.GetGlobalPosition()
			self.tooltipScaleDown.SetTooltipPosition(ButtonPosX, ButtonPosY)

		if self.AtlasShowButton and self.tooltipAtlasOpen:
			(ButtonPosX, ButtonPosY) = self.AtlasShowButton.GetGlobalPosition()
			self.tooltipAtlasOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)

		# if app.ENABLE_EVENT_MANAGER:
		# 	if self.InGameEventButton and self.tooltipInGameEventButton:
		# 		(ButtonPosX, ButtonPosY) = self.InGameEventButton.GetGlobalPosition()
		# 		self.tooltipInGameEventButton.SetTooltipPosition(ButtonPosX, ButtonPosY)

		# if app.ENABLE_DUNGEON_INFO_SYSTEM:
		# 	if self.tooltipDungeonInfoOpen and self.tooltipDungeonInfoOpen:
		# 		(ButtonPosX, ButtonPosY) = self.DungeonInfoShowButton.GetGlobalPosition()
		# 		self.tooltipDungeonInfoOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)

	def Destroy(self):
		self.HideMiniMap()

		self.AtlasWindow.Destroy()
		self.AtlasWindow = None

		self.ClearDictionary()

		self.__Initialize()

	def UpdateCurrentChannel(self, channelID):
		(serverName, channelName) = net.GetServerInfo().split(",")
		channelName = TextColor("CH-{}".format(channelID), "FFffFF")
		net.SetServerInfo("{}, {}".format(serverName, channelName))
		if self.serverInfo:
			self.serverInfo.SetText(net.GetServerInfo())

	def UpdateObserverCount(self, observerCount):
		if observerCount>0:
			self.observerCount.Show()
		elif observerCount<=0:
			self.observerCount.Hide()

		self.observerCount.SetText(localeInfo.MINIMAP_OBSERVER_COUNT % observerCount)

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		miniMap.Update(x, y)

		if localeInfo.IsARABIC():
			self.positionInfo.SetText("%.0f, %.0f" % (x/100, y/100))
		else:
			self.positionInfo.SetText("(%.0f, %.0f)" % (x/100, y/100))
		self.dataTime.SetText(time.strftime("[%H:%M:%S - %d/%m/%Y]"))

		if self.tooltipInfo:
			if True == self.MiniMapWindow.IsIn():
				(mouseX, mouseY) = wndMgr.GetMousePosition()
				(bFind, sName, iPosX, iPosY, dwTextColor) = miniMap.GetInfo(mouseX, mouseY)
				if bFind == 0:
					self.tooltipInfo.Hide()
				elif not self.canSeeInfo:
					self.tooltipInfo.SetText("%s(%s)" % (sName, localeInfo.UI_POS_UNKNOWN))
					self.tooltipInfo.SetTooltipPosition(mouseX - 5, mouseY)
					self.tooltipInfo.SetTextColor(dwTextColor)
					self.tooltipInfo.Show()
				else:
					if localeInfo.IsARABIC() and sName[-1].isalnum():
						self.tooltipInfo.SetText("(%s)%d, %d" % (sName, iPosX, iPosY))
					else:
						self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
					self.tooltipInfo.SetTooltipPosition(mouseX - 5, mouseY)
					self.tooltipInfo.SetTextColor(dwTextColor)
					self.tooltipInfo.Show()
			else:
				self.tooltipInfo.Hide()

			# AUTOBAN
			if self.imprisonmentDuration:
				self.__UpdateImprisonmentDurationText()
			# END_OF_AUTOBAN

		if True == self.MiniMapShowButton.IsIn():
			self.tooltipMiniMapOpen.Show()
		else:
			self.tooltipMiniMapOpen.Hide()

		if True == self.MiniMapHideButton.IsIn():
			self.tooltipMiniMapClose.Show()
		else:
			self.tooltipMiniMapClose.Hide()

		if True == self.ScaleUpButton.IsIn():
			self.tooltipScaleUp.Show()
		else:
			self.tooltipScaleUp.Hide()

		if True == self.ScaleDownButton.IsIn():
			self.tooltipScaleDown.Show()
		else:
			self.tooltipScaleDown.Hide()

		if True == self.AtlasShowButton.IsIn():
			self.tooltipAtlasOpen.Show()
		else:
			self.tooltipAtlasOpen.Hide()

		# if app.ENABLE_EVENT_MANAGER:
		# 	if True == self.InGameEventButton.IsIn():
		# 		self.tooltipInGameEventButton.Show()
		# 	else:
		# 		self.tooltipInGameEventButton.Hide()

		# if app.ENABLE_DUNGEON_INFO_SYSTEM:
		# 	if True == self.DungeonInfoShowButton.IsIn():
		# 		self.tooltipDungeonInfoOpen.Show()
		# 	else:
		# 		self.tooltipDungeonInfoOpen.Hide()

	def OnRender(self):
		(x, y) = self.GetGlobalPosition()
		fx = float(x)
		fy = float(y)
		miniMap.Render(fx + 4.0, fy + 5.0)

	def Close(self):
		self.HideMiniMap()

	def HideMiniMap(self):
		miniMap.Hide()
		if self.OpenWindow:
			self.OpenWindow.Hide()
		if localeInfo.IsARABIC():
			self.SetPosition(wndMgr.GetScreenWidth() - 36, 0)
			self.RefreshTooltipPosition()

		if self.CloseWindow:
			self.CloseWindow.Show()

	def ShowMiniMap(self):
		if not self.canSeeInfo:
			return
		self.CloseWindow.Hide()
		if localeInfo.IsARABIC():
			self.SetPosition(wndMgr.GetScreenWidth() - 136, 0)
			self.RefreshTooltipPosition()
		miniMap.Show()
		self.OpenWindow.Show()

	def isShowMiniMap(self):
		return miniMap.isShow()

	def ScaleUp(self):
		miniMap.ScaleUp()

	def ScaleDown(self):
		miniMap.ScaleDown()

	def ShowAtlas(self):
		if not miniMap.IsAtlas():
			return
		if not self.AtlasWindow.IsShow():
			self.AtlasWindow.Show()

	def ToggleAtlasWindow(self):
		if not miniMap.IsAtlas():
			return
		if self.AtlasWindow.IsShow():
			self.AtlasWindow.Hide()
		else:
			self.AtlasWindow.Show()

	if app.ENABLE_LOADING_PERFORMANCE:
		def IsShowingAtlas(self):
			return self.AtlasWindow.IsShow()

	# if app.ENABLE_EVENT_MANAGER:
	# 	def ToggleInGameEvent(self):
	# 		self.interface.ToggleInGameEvent()

	# if app.ENABLE_BIOLOG_SYSTEM:
	# 	def __ToggleBiolog(self):
	# 		biologmgr.SendPacket(net.BIOLOG_MANAGER_OPEN)

	# if app.ENABLE_INGAME_WIKI:
	# 	def ToggleWikipedia(self):
	# 		self.interface.OpenWikiWindow()

	# if app.ENABLE_DUNGEON_INFO_SYSTEM:
	# 	def ShowDungeonInfo(self):
	# 		self.interface.ToggleDungeonInfoWindow()

	def BindInterfaceClass(self, interface):
		from _weakref import proxy
		self.interface = proxy(interface)
