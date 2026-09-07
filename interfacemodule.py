##
## Interface
##
import constInfo
import systemSetting
import wndMgr
import chat
import app
import player
import uiTaskBar
import uiCharacter
import uiInventory
import uiDragonSoul
import uiChat
import uiMessenger
import guild

import ui
import uiHelp
import uiWhisper
import uiPointReset
import uiShop
import uiExchange
import uiAttachMetin
import uiSystem
import uiRestart
import uiToolTip
import uiMiniMap
import uiParty
import uiSafebox
import uiGuild
import uiQuest
import uiPrivateShopBuilder
import uiCommon
import uiRefine
import uiEquipmentDialog
import uiGameButton
import uiTip
import uiCube
import miniMap
# ACCESSORY_REFINE_ADD_METIN_STONE
import uiSelectItem
# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE
import uiScriptLocale
import uinewguildstorage
import uiBossTime

import event
import localeInfo

if app.__DAILY_QUESTS__:
	import uidailyquests

if app.FAST_EQUIP_WORLDARD:
	import uifastequip

if app.ENABLE_ACCE_COSTUME_SYSTEM:
	import uiacce

if app.ENABLE_MOVE_CHANNEL:
	import uiMoveChannel

if app.ENABLE_RENDER_TARGET_PREVIEW:
	import uiRenderTargetWindow

if app.ENABLE_LOADING_PERFORMANCE:
	import uiAffectShower

if app.ENABLE_CHEST_DROP_INFO:
	import uiChestDropInfo

if app.ENABLE_GEM_SYSTEM:
	import uiGemShop
	import uiSelectItemEx

if app.ENABLE_SWITCHBOT_SYSTEM:
	import uiSwitchbot

if app.ENABLE_INGAME_WIKI:
	import ingamewiki

if app.ENABLE_EVENT_MANAGER:
	import uiEvent

if app.ENABLE_BIOLOG_SYSTEM:
	import biologmgr
	import uiBiologManager

if app.ENABLE_SKILL_GROUP_GUI:
	import uiSkillGroup

if app.__AUTO_SKILL_READER__:
	import uiAutoSkillReader

if app.ENABLE_PREMIUM_PRIVATE_SHOP or app.ENABLE_ANTI_EXP or app.ENABLE_ITEMSHOP or app.__BL_MULTI_LANGUAGE_PREMIUM__:
	import net

if app.ENABLE_PREMIUM_PRIVATE_SHOP:
	import uiPrivateShop
	import uiPrivateShopSearch

if app.ENABLE_ITEM_MANAGER:
	import uiItemManager

if app.ENABLE_REMOTE_SHOP:
	import uiRemoteShop

if app.ENABLE_PICK_FILTER:
	import uiPickUpFilter

if app.ENABLE_SAVE_LAST_WINDOW_POSITION:
	import os

if app.ENABLE_MAINTENANCE_SYSTEM:
	import uiMaintenance

if app.ENABLE_HUNTING_SYSTEM:
	import uiHunting

if app.BL_67_ATTR:
	import uiAttr67Add

if app.__SPIN_WHEEL__:
	import uiSpinWheel
	
if app.__SPIN_WHEEL_BOSS__:
	import uiSpinWheelBoss

if app.ENABLE_DUNGEON_INFO_SYSTEM:
	import uiDungeonInfo

if app.ENABLE_GLOBAL_REWARD:
	import uiReward

if constInfo.ENABLE_SKYBOX_WINDOW:
	import uiSkyBox

if app.ENABLE_ANTI_MULTIPLE_FARM:
	import uiAntiMultipleFarm
	import anti_multiple_farm

if app.ENABLE_VOTE_4_BUFF:
	import uiVote4Buff

if app.ENABLE_RANK_PLAYER:
	import uiRankInfo

if app.ENABLE_PLAYER_RANKING:
	import uiRankInfo

if app.ENABLE_BATTLE_PASS:
	import uiBattlePass

if app.ENABLE_DAILY_BOSS:
	import uiBossGui

if app.ENABLE_ITEMSHOP:
	import uiItemShop

if app.ENABLE_EVENT_CALENDAR:
	import uiEventCalendar

IsQBHide = 0
charmeine_duel = 0

class Interface(object):
	CHARACTER_STATUS_TAB = 1
	CHARACTER_SKILL_TAB = 2

	def __init__(self):
		systemSetting.SetInterfaceHandler(self)
		self.windowOpenPosition = 0
		if app.ENABLE_SAVE_BLOCK_ATTR:
			self.blockAttrBlockTime = 0
		self.dlgWhisperWithoutTarget = None
		self.inputDialog = None
		self.tipBoard = None
		self.bigBoard = None
		
		# ITEM_MALL
		self.mallPageDlg = None
		# END_OF_ITEM_MALL

		self.wndWeb = None
		self.wndTaskBar = None
		self.wndCharacter = None
		self.wndInventory = None
		self.wndExpandedTaskBar = None
		self.wndDragonSoul = None
		self.wndDragonSoulRefine = None
		self.wndChat = None
		self.wndMessenger = None
		self.wndMiniMap = None
		self.wndGuild = None
		self.wndGuildBuilding = None
		self.bosstime = None

		if app.ENABLE_MOVE_CHANNEL:
			self.wndMoveChannel = None
		if app.ENABLE_RENDER_TARGET_PREVIEW:
			self.wndTargetRender = None
		if app.ENABLE_LOADING_PERFORMANCE:
			self.wndWarpShower = None
		if app.ENABLE_CHEST_DROP_INFO:
			self.wndChestDropInfo = None
		if app.ENABLE_GEM_SYSTEM:
			self.wndExpandedMoneyTaskBar = None
			self.wndGemShop = None
		if app.ENABLE_SWITCHBOT_SYSTEM:
			self.wndSwitchbot = None
		if app.ENABLE_INGAME_WIKI:
			self.wndWiki = None
		if app.ENABLE_EVENT_MANAGER:
			self.wndEventOverview = None
		if app.ENABLE_BIOLOG_SYSTEM:
			self.wndBiologManager = {}
		if app.ENABLE_SKILL_GROUP_GUI:
			self.wndSkillGroup = None
		if app.__AUTO_SKILL_READER__:
			self.wndAutoSkillReader = None
		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			self.wndPrivateShopPanel = None
			self.wndPrivateShopSearch = None
			self.privateShopTitleBoardDict = {}
		if app.ENABLE_ITEM_MANAGER:
			self.wndItemManager = None
		if app.ENABLE_REMOTE_SHOP:
			self.wndRemoteShop = None
		if app.ENABLE_PICK_FILTER:
			self.wndPickUpFilter = None
		if app.ENABLE_MAINTENANCE_SYSTEM:
			self.wndMaintenance = None
		if app.ENABLE_HUNTING_SYSTEM:
			self.wndHunting = None
			self.wndHuntingSelect = None
			self.wndHuntingReward = None
		if app.__SPIN_WHEEL__:
			self.wndSpinWheel = None
		if app.__SPIN_WHEEL_BOSS__:
			self.wndSpinWheelBoss = None
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.wndDungeonInfo = None
		if app.ENABLE_GLOBAL_REWARD:
			self.wndReward = None
		if app.ENABLE_ANTI_MULTIPLE_FARM:
			self.wndAntiMultipleFarm = None
		if app.ENABLE_VOTE_4_BUFF:
			# self.vote4BuffTime = 0
			self.wndVote4Buff = None
		if app.ENABLE_BATTLE_PASS:
			self.wndBattlePass = None
			self.wndBattlePassButton = None
		if app.ENABLE_ITEMSHOP:
			self.wndItemshop = None
		if app.ENABLE_GUILD_SAFEBOX:
			self.wndGuildSafebox = None
		if app.ENABLE_EVENT_CALENDAR:
			self.wndEventManager = None
			self.wndEventIcon = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.OnTopWindow = None
			self.dlgShop = None
			self.dlgExchange = None
			self.privateShopBuilder = None
			self.wndSafebox = None
			self.dlgRefineNew = None

		self.listGMName = {}
		self.wndQuestWindow = {}
		self.wndQuestWindowNewKey = 0
		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}

		if app.__DAILY_QUESTS__:
			self.wndDailyQuests = None

		if app.__BL_MULTI_LANGUAGE_PREMIUM__:
			self.EMPIRE_NAME = {
				net.EMPIRE_A : localeInfo.EMPIRE_A,
				net.EMPIRE_B : localeInfo.EMPIRE_B,
				net.EMPIRE_C : localeInfo.EMPIRE_C
			}

		event.SetInterfaceWindow(self)

		if app.ENABLE_SAVE_LAST_WINDOW_POSITION:
			if not os.path.exists("lib/user_data/wnd/"):
				os.makedirs("lib/user_data/wnd/")
			if not os.path.exists("lib/user_data/wnd/" + player.GetName()):
				os.makedirs("lib/user_data/wnd/" + player.GetName())

	def __del__(self):
		systemSetting.DestroyInterfaceHandler()
		event.SetInterfaceWindow(None)

	################################
	## Make Windows & Dialogs
	def __MakeUICurtain(self):
		wndUICurtain = ui.Bar("TOP_MOST")
		wndUICurtain.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		wndUICurtain.SetColor(0x77000000)
		wndUICurtain.Hide()
		self.wndUICurtain = wndUICurtain

	def __MakeMessengerWindow(self):
		self.wndMessenger = uiMessenger.MessengerWindow()

		from _weakref import proxy
		self.wndMessenger.SetWhisperButtonEvent(lambda n,i=proxy(self):i.OpenWhisperDialog(n))
		self.wndMessenger.SetGuildButtonEvent(ui.__mem_func__(self.ToggleGuildWindow))

	def __MakeGuildWindow(self):
		self.wndGuild = uiGuild.GuildWindow()

	def __MakeChatWindow(self):

		wndChat = uiChat.ChatWindow()

		wndChat.SetSize(wndChat.CHAT_WINDOW_WIDTH, 0)
		# wndChat.SetPosition(wndMgr.GetScreenWidth()/2 - wndChat.CHAT_WINDOW_WIDTH/2, wndMgr.GetScreenHeight() - wndChat.EDIT_LINE_HEIGHT - 37)
		wndChat.SetPosition(wndMgr.GetScreenWidth()/2 - wndChat.CHAT_WINDOW_WIDTH/2, wndMgr.GetScreenHeight() - wndChat.EDIT_LINE_HEIGHT - 43)
		wndChat.SetHeight(200)
		wndChat.Refresh()
		wndChat.Show()

		self.wndChat = wndChat
		self.wndChat.BindInterface(self)
		self.wndChat.SetSendWhisperEvent(ui.__mem_func__(self.OpenWhisperDialogWithoutTarget))
		self.wndChat.SetOpenChatLogEvent(ui.__mem_func__(self.ToggleChatLogWindow))

	def __MakeTaskBar(self):
		wndTaskBar = uiTaskBar.TaskBar()
		wndTaskBar.LoadWindow()
		self.wndTaskBar = wndTaskBar
		self.wndTaskBar.BindInterface(self)
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHARACTER, ui.__mem_func__(self.ToggleCharacterWindowStatusPage))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_INVENTORY, ui.__mem_func__(self.ToggleInventoryWindow))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_MESSENGER, ui.__mem_func__(self.ToggleMessenger))
		self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_SYSTEM, ui.__mem_func__(self.ToggleSystemDialog))
		# if app.ENABLE_SWITCHBOT_SYSTEM:
			# self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_SWITCHBOT, ui.__mem_func__(self.ToggleSwitchbotWindow))
		# if app.ENABLE_PREMIUM_PRIVATE_SHOP:
		# 	self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_OFFLINE_SHOP_SEARCH, ui.__mem_func__(self.OpenSearchShopCommand))
		if app.ENABLE_ANTI_MULTIPLE_FARM:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_ANTI_MULTIPLE_FARM, ui.__mem_func__(self.ToggleAntiMultipleFarmWindow))
		if uiTaskBar.TaskBar.IS_EXPANDED:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_EXPAND, ui.__mem_func__(self.ToggleExpandedButton))
			self.wndExpandedTaskBar = uiTaskBar.ExpandedTaskBar()
			self.wndExpandedTaskBar.LoadWindow()
			# self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_DRAGON_SOUL, ui.__mem_func__(self.ToggleDragonSoulWindow))
			if app.ENABLE_SWITCHBOT_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_SWITCHBOT_SYSTEM, ui.__mem_func__(self.ToggleSwitchbotWindow))
			if app.ENABLE_INGAME_WIKI:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_INGAME_WIKI, ui.__mem_func__(self.OpenWikiWindow))
			if app.ENABLE_HUNTING_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_HUNTING_SYSTEM, ui.__mem_func__(self.ToggleHuntingWindow))
			if app.ENABLE_ANTI_EXP:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_ANTI_EXP, ui.__mem_func__(self.__ClickAntiExp))
			if app.ENABLE_PREMIUM_PRIVATE_SHOP:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_OFFLINE_SHOP_SEARCH, ui.__mem_func__(self.OpenSearchShopCommand))
			if app.ENABLE_EVENT_MANAGER:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_EVENT_MANAGER, ui.__mem_func__(self.ToggleInGameEvent))
			if app.ENABLE_EVENT_CALENDAR:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_EVENT_MANAGER, ui.__mem_func__(self.OpenEventCalendar))
			if app.ENABLE_BATTLE_PASS:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_BATTLEPASS, ui.__mem_func__(self.RequestOpenBattlePass))
			if app.ENABLE_DUNGEON_INFO_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_DUNGEON_INFO, ui.__mem_func__(self.ToggleDungeonInfoWindow))
			if app.ENABLE_BIOLOG_SYSTEM:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_BIOLOG, ui.__mem_func__(self.__ToggleBiolog))
		else:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_CHAT, ui.__mem_func__(self.ToggleChat))

		self.wndEnergyBar = None
		if app.ENABLE_ENERGY_SYSTEM:
			wndEnergyBar = uiTaskBar.EnergyBar()
			wndEnergyBar.LoadWindow()
			self.wndEnergyBar = wndEnergyBar

		if app.ENABLE_GEM_SYSTEM:
			self.wndTaskBar.SetToggleButtonEvent(uiTaskBar.TaskBar.BUTTON_EXPAND_MONEY, ui.__mem_func__(self.ToggleExpandedMoneyButton))
			self.wndExpandedMoneyTaskBar = uiTaskBar.ExpandedMoneyTaskBar()
			self.wndExpandedMoneyTaskBar.LoadWindow()

	def __MakeParty(self):
		wndParty = uiParty.PartyWindow()
		wndParty.Hide()
		self.wndParty = wndParty

	def __MakeGameButtonWindow(self):
		wndGameButton = uiGameButton.GameButtonWindow()
		wndGameButton.SetTop()
		wndGameButton.Show()
		wndGameButton.SetButtonEvent("STATUS", ui.__mem_func__(self.__OnClickStatusPlusButton))
		wndGameButton.SetButtonEvent("SKILL", ui.__mem_func__(self.__OnClickSkillPlusButton))
		wndGameButton.SetButtonEvent("QUEST", ui.__mem_func__(self.__OnClickQuestButton))
		wndGameButton.SetButtonEvent("HELP", ui.__mem_func__(self.__OnClickHelpButton))
		wndGameButton.SetButtonEvent("BUILD", ui.__mem_func__(self.__OnClickBuildButton))

		self.wndGameButton = wndGameButton

	def __IsChatOpen(self):
		return True

	def __MakeWindows(self):
		wndCharacter = uiCharacter.CharacterWindow()
		wndInventory = uiInventory.InventoryWindow()
		wndInventory.BindInterfaceClass(self)
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			wndDragonSoul = uiDragonSoul.DragonSoulWindow()
			wndDragonSoulRefine = uiDragonSoul.DragonSoulRefineWindow()
		else:
			wndDragonSoul = None
			wndDragonSoulRefine = None

		wndMiniMap = uiMiniMap.MiniMap()
		if app.ENABLE_LOADING_PERFORMANCE:
			wndAffectShower = uiAffectShower.AffectShower()
		wndSafebox = uiSafebox.SafeboxWindow()

		# ITEM_MALL
		wndMall = uiSafebox.MallWindow()
		self.wndMall = wndMall
		# END_OF_ITEM_MALL

		wndChatLog = uiChat.ChatLogWindow()
		wndChatLog.BindInterface(self)

		self.wndCharacter = wndCharacter
		self.wndInventory = wndInventory
		self.wndDragonSoul = wndDragonSoul
		self.wndDragonSoulRefine = wndDragonSoulRefine
		self.wndMiniMap = wndMiniMap
		if app.ENABLE_LOADING_PERFORMANCE:
			self.wndAffectShower = wndAffectShower
		self.wndSafebox = wndSafebox
		self.wndChatLog = wndChatLog
		self.bosstime = uiBossTime.BossTime()

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)
			self.wndDragonSoulRefine.SetInventoryWindows(self.wndInventory, self.wndDragonSoul)
			self.wndInventory.SetDragonSoulRefineWindow(self.wndDragonSoulRefine)

		if app.__DAILY_QUESTS__:
			self.wndDailyQuests = uidailyquests.DailyWindow()

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.wndSafebox.BindInterface(self)

		if app.ENABLE_CHECKINOUT_UPDATE:
			self.wndInventory.SetSafeboxWindow(self.wndSafebox)

		if app.ENABLE_MOVE_CHANNEL:
			self.wndMoveChannel = uiMoveChannel.MoveChannelWindow()

		if app.ENABLE_RENDER_TARGET_PREVIEW:
			self.wndTargetRender = uiRenderTargetWindow.RenderTargetWindow()
			self.wndTargetRender.Hide()

		if app.ENABLE_LOADING_PERFORMANCE:
			self.wndWarpShower = None

		if app.ENABLE_CHEST_DROP_INFO:
			self.wndChestDropInfo = uiChestDropInfo.ChestDropInfoWindow()
			if app.ENABLE_RENDER_TARGET_PREVIEW:
				self.wndChestDropInfo.BindInterface(self)

		if app.ENABLE_SWITCHBOT_SYSTEM:
			self.wndSwitchbot = uiSwitchbot.SwitchbotWindow()

		if app.ENABLE_INGAME_WIKI:
			self.wndWiki = ingamewiki.InGameWiki()
			self.wndInventory.BindWikiWindow(self.wndWiki)

		if app.ENABLE_GEM_SYSTEM:
			self.wndGemShop = uiGemShop.GemShopWindow()
			if app.ENABLE_RENDER_TARGET_PREVIEW or app.ENABLE_CHEST_DROP_INFO:
				self.wndGemShop.BindInterface(self)

		if app.ENABLE_EVENT_MANAGER:
			wndMiniMap.BindInterfaceClass(self)
			self.wndEventOverview = uiEvent.EventOverview()
			if app.ENABLE_CHEST_DROP_INFO:
				self.wndEventOverview.BindInterface(self)
			
		if app.ENABLE_BIOLOG_SYSTEM:
			self.wndBiologManager["PANEL"] = uiBiologManager.BiologManager()
			# if app.ENABLE_CHEST_DROP_INFO:
			# 	self.wndBiologManager["PANEL"].BindInterface(self)
			self.wndBiologManager["ALERT"] = uiBiologManager.BiologManager_Alert()

		if app.ENABLE_SKILL_GROUP_GUI:
			self.wndSkillGroup = uiSkillGroup.SkillGroupWindow()

		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			self.wndPrivateShopPanel = uiPrivateShop.PrivateShopPanel()
			self.wndPrivateShopPanel.BindInterfaceClass(self)
			self.wndPrivateShopPanel.BindInventoryClass(self.wndInventory)
			self.wndPrivateShopPanel.BindDragonSoulInventoryClass(self.wndDragonSoul)

			self.wndDragonSoul.BindPrivateShopClass(self.wndPrivateShopPanel)
			self.wndDragonSoul.BindPrivateShopSearchClass(self.wndPrivateShopSearch)
			
			self.wndPrivateShopSearch = uiPrivateShopSearch.PrivateShopSeachWindow()
			self.wndPrivateShopSearch.BindInterfaceClass(self)

			if app.WJ_ENABLE_TRADABLE_ICON:
				self.wndPrivateShopSearch.SetInven(self.wndInventory)
			# 	self.wndInventory.BindWindow(self.wndPrivateShopSearch)
			# 	self.wndInventory.BindWindow(self.wndPrivateShopPanel)

			# self.wndInventory.BindWindow(self.wndPrivateShopPanel)
			self.wndInventory.BindPrivateShopClass(self.wndPrivateShopPanel)
			self.wndInventory.BindPrivateShopSearchClass(self.wndPrivateShopSearch)

		if app.ENABLE_ITEM_MANAGER:
			self.wndItemManager = uiItemManager.ItemManager()
			self.wndItemManager.BindInterface(self)

		if app.ENABLE_REMOTE_SHOP:
			self.wndRemoteShop = uiRemoteShop.RemoteShopDialog()

		if app.ENABLE_PICK_FILTER:
			self.wndPickUpFilter = uiPickUpFilter.PickUpFilterWindow()

		if app.BL_67_ATTR:
			self.wndAttr67Add = uiAttr67Add.Attr67AddWindow()
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.wndAttr67Add.BindInterface(self)
				self.wndAttr67Add.SetInven(self.wndInventory)
				# self.wndInventory.BindWindow(self.wndAttr67Add)

		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.wndDungeonInfo = uiDungeonInfo.DungeonInfoWindow()
			self.wndMiniMap.BindInterfaceClass(self)

		if constInfo.ENABLE_SKYBOX_WINDOW:
			self.wndSkyBox = uiSkyBox.SkyBoxWindow()

		if app.ENABLE_RANK_PLAYER:
			self.wndRankInfo = uiRankInfo.RankInfo()

		if app.ENABLE_PLAYER_RANKING:
			self.wndRankInfo = uiRankInfo.RankInfo()

		if app.ENABLE_BATTLE_PASS:
			self.wndBattlePass = uiBattlePass.BattlePassWindow()
			self.wndBattlePassButton = uiBattlePass.BattlePassButton()
			self.wndBattlePassButton.BindInterface(self)

		if app.ENABLE_GUILD_SAFEBOX:
			wndGuildSafebox = uiSafebox.GuildSafeboxWindow()
			self.wndGuildSafebox = wndGuildSafebox

	def __MakeDialogs(self):
		self.wndGuildStorage = uinewguildstorage.MainWindow()
		

		self.dlgExchange = uiExchange.ExchangeDialog()
		if app.ENABLE_EXCHANGE_WINDOW_RENEWAL or app.ENABLE_CHECKINOUT_UPDATE:
			if self.wndInventory:
				self.wndInventory.SetExchangeDlg(self.dlgExchange)
		if app.WJ_ENABLE_TRADABLE_ICON or app.ENABLE_EXCHANGE_WINDOW_RENEWAL:
			self.dlgExchange.BindInterface(self)
		self.dlgExchange.LoadDialog()
		self.dlgExchange.SetCenterPosition()
		self.dlgExchange.Hide()

		self.dlgAttachMetin = uiAttachMetin.AttachMetinDialog()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgAttachMetin.SetInven(self.wndInventory)
		self.dlgAttachMetin.Hide()

		self.dlgPointReset = uiPointReset.PointResetDialog()
		self.dlgPointReset.LoadDialog()
		self.dlgPointReset.Hide()

		self.dlgShop = uiShop.ShopDialog()
		self.dlgShop.LoadDialog()
		if app.ENABLE_CHEST_DROP_INFO or app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgShop.BindInterface(self)
		self.dlgShop.Hide()

		self.dlgRestart = uiRestart.RestartDialog()
		self.dlgRestart.LoadDialog()
		self.dlgRestart.Hide()

		self.dlgSystem = uiSystem.SystemDialog()
		self.dlgSystem.LoadDialog()
		self.dlgSystem.SetOpenHelpWindowEvent(ui.__mem_func__(self.OpenHelpWindow))
		self.dlgSystem.BindInterface(self)
		self.dlgSystem.Hide()

		self.dlgPassword = uiSafebox.PasswordDialog()
		self.dlgPassword.Hide()

		self.hyperlinkItemTooltip = uiToolTip.HyperlinkItemToolTip()
		self.hyperlinkItemTooltip.BindInterface(self)
		self.hyperlinkItemTooltip.Hide()

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.BindInterface(self)
		self.tooltipItem.Hide()

		self.tooltipSkill = uiToolTip.SkillToolTip()
		self.tooltipSkill.Hide()

		self.privateShopBuilder = uiPrivateShopBuilder.PrivateShopBuilder()
		self.privateShopBuilder.Hide()

		self.dlgRefineNew = uiRefine.RefineDialogNew()
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgRefineNew.SetInven(self.wndInventory)
		self.dlgRefineNew.Hide()

		if app.ENABLE_HUNTING_SYSTEM:
			self.wndHunting = uiHunting.HuntingWindow()
			self.wndHuntingSelect = uiHunting.HuntingSelectWindow()
			self.wndHuntingReward = uiHunting.HuntingRewardWindow()

		if app.__BL_MULTI_LANGUAGE_PREMIUM__:
			self.countryTooltip = uiToolTip.CountryToolTip()
			self.countryTooltip.Hide()

		if app.ENABLE_VOTE_4_BUFF:
			self.wndVote4Buff = uiVote4Buff.VoteWindow()
			self.wndVote4Buff.Hide()

	if app.FAST_EQUIP_WORLDARD:
		def __MakeFastEquip(self):
			self.wndFastEquip = uifastequip.UiFastEquip()
			self.wndFastEquip.LoadWindow()
			self.wndFastEquip.Hide()

	def __MakeHelpWindow(self):
		self.wndHelp = uiHelp.HelpWindow()
		self.wndHelp.LoadDialog()
		self.wndHelp.SetCloseEvent(ui.__mem_func__(self.CloseHelpWindow))
		self.wndHelp.Hide()

	def __MakeTipBoard(self):
		if constInfo.ENABLE_NEW_TIPBOAD:
			self.tipBoard = uiTip.NewTipBoard()
		else:
			self.tipBoard = uiTip.TipBoard()
		self.tipBoard.Hide()

		self.bigBoard = uiTip.BigBoard()
		self.bigBoard.Hide()

	def __MakeWebWindow(self):
		if constInfo.IN_GAME_SHOP_ENABLE:
			import uiWeb
			self.wndWeb = uiWeb.WebWindow()
			self.wndWeb.LoadWindow()
			self.wndWeb.Hide()

	def __MakeCubeWindow(self):
		self.wndCube = uiCube.CubeWindow()
		self.wndCube.LoadWindow()
		self.wndCube.Hide()

	def __MakeCubeResultWindow(self):
		self.wndCubeResult = uiCube.CubeResultWindow()
		self.wndCubeResult.LoadWindow()
		self.wndCubeResult.Hide()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def __MakeAcceWindow(self):
			self.wndAcceCombine = uiacce.CombineWindow()
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.wndAcceCombine.BindInterface(self)
			self.wndAcceCombine.LoadWindow()
			self.wndAcceCombine.Hide()

			self.wndAcceAbsorption = uiacce.AbsorbWindow()
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.wndAcceAbsorption.BindInterface(self)
			self.wndAcceAbsorption.LoadWindow()
			self.wndAcceAbsorption.Hide()

			if self.wndInventory:
				self.wndInventory.SetAcceWindow(self.wndAcceCombine, self.wndAcceAbsorption)

	if app.ENABLE_ANTI_MULTIPLE_FARM:
		def __MakeAntiMultipleFarmWnd(self):
			self.wndAntiMultipleFarm = uiAntiMultipleFarm.AntiMultipleFarmWnd()
			self.wndAntiMultipleFarm.Hide()

	if app.ENABLE_DAILY_BOSS:
		def __MakeWorldBossWindow(self):
			self.wndWorldBoss = uiBossGui.DailyBossGui()
			self.wndWorldBoss.Close()

	if app.ENABLE_ITEMSHOP:
		def __MakeItemshop(self):
			self.wndItemshop = uiItemShop.ItemshopWindow()
			self.wndItemshop.BindInterface(self)
			self.wndItemshop.Hide()

	# ACCESSORY_REFINE_ADD_METIN_STONE
	def __MakeItemSelectWindow(self):
		self.wndItemSelect = uiSelectItem.SelectItemWindow()
		self.wndItemSelect.Hide()
		if app.ENABLE_GEM_SYSTEM:
			self.wndItemSelectEx = uiSelectItemEx.SelectItemWindow()
			self.wndItemSelectEx.Hide()
	# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

	def MakeInterface(self):
		self.__MakeMessengerWindow()
		self.__MakeGuildWindow()
		self.__MakeChatWindow()
		self.__MakeParty()
		self.__MakeWindows()
		self.__MakeDialogs()

		self.__MakeUICurtain()
		self.__MakeTaskBar()
		self.__MakeGameButtonWindow()
		self.__MakeHelpWindow()
		self.__MakeTipBoard()
		self.__MakeWebWindow()
		self.__MakeCubeWindow()
		self.__MakeCubeResultWindow()
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.__MakeAcceWindow()
		if app.ENABLE_ANTI_MULTIPLE_FARM:
			self.__MakeAntiMultipleFarmWnd()
		if app.ENABLE_DAILY_BOSS:
			self.__MakeWorldBossWindow()
		if app.ENABLE_ITEMSHOP:
			self.__MakeItemshop()

		if app.FAST_EQUIP_WORLDARD:
			self.__MakeFastEquip()
			
		# ACCESSORY_REFINE_ADD_METIN_STONE
		self.__MakeItemSelectWindow()
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}

		self.wndInventory.SetItemToolTip(self.tooltipItem)
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.SetItemToolTip(self.tooltipItem)
			self.wndDragonSoulRefine.SetItemToolTip(self.tooltipItem)
		self.wndSafebox.SetItemToolTip(self.tooltipItem)
		self.wndCube.SetItemToolTip(self.tooltipItem)
		self.wndCubeResult.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine.SetItemToolTip(self.tooltipItem)
			self.wndAcceAbsorption.SetItemToolTip(self.tooltipItem)

		# ITEM_MALL
		self.wndMall.SetItemToolTip(self.tooltipItem)
		# END_OF_ITEM_MALL

		self.wndCharacter.SetSkillToolTip(self.tooltipSkill)
		self.wndTaskBar.SetItemToolTip(self.tooltipItem)
		self.wndTaskBar.SetSkillToolTip(self.tooltipSkill)
		self.wndGuild.SetSkillToolTip(self.tooltipSkill)

		# ACCESSORY_REFINE_ADD_METIN_STONE
		self.wndItemSelect.SetItemToolTip(self.tooltipItem)
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		self.dlgShop.SetItemToolTip(self.tooltipItem)
		self.dlgExchange.SetItemToolTip(self.tooltipItem)
		self.privateShopBuilder.SetItemToolTip(self.tooltipItem)
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.dlgExchange.SetInven(self.wndInventory)

		if app.__DAILY_QUESTS__:
			self.wndDailyQuests.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_SWITCHBOT_SYSTEM:
			self.wndSwitchbot.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_GEM_SYSTEM:
			self.wndGemShop.SetItemToolTip(self.tooltipItem)
			self.wndItemSelectEx.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_BIOLOG_SYSTEM:
			self.wndBiologManager["PANEL"].SetItemToolTip(self.tooltipItem)

		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			self.wndPrivateShopPanel.SetItemToolTip(self.tooltipItem)
			self.wndPrivateShopSearch.SetItemToolTip(self.tooltipItem)
			self.privateShopTitleBoardDict = {}

		if app.ENABLE_BATTLE_PASS:
			self.wndBattlePass.SetItemToolTip(self.tooltipItem)

		if app.ENABLE_GUILD_SAFEBOX:
			self.wndGuildSafebox.SetItemToolTip(self.tooltipItem)

		self.__InitWhisper()
		self.DRAGON_SOUL_IS_QUALIFIED = True if app.ENABLE_NO_DSS_QUALIFICATION else False

	def MakeHyperlinkTooltip(self, hyperlink):
		tokens = hyperlink.split(":")
		if tokens and len(tokens):
			type = tokens[0]
			if "item" == type:
				self.hyperlinkItemTooltip.SetHyperlinkItem(tokens)
			elif app.PM_IN_GLOBAL_CHAT and (("PM_LINK" == type or "msg" == type) and str(tokens[1]) != player.GetMainCharacterName()):
				self.OpenWhisperDialog(str(tokens[1]))

	## Make Windows & Dialogs
	################################

	def Close(self):
		if self.dlgWhisperWithoutTarget:
			self.dlgWhisperWithoutTarget.Destroy()
			del self.dlgWhisperWithoutTarget

		if uiQuest.QuestDialog.__dict__.has_key("QuestCurtain"):
			uiQuest.QuestDialog.QuestCurtain.Close()

		if self.wndQuestWindow:
			for key, eachQuestWindow in self.wndQuestWindow.items():
				eachQuestWindow.nextCurtainMode = -1
				eachQuestWindow.CloseSelf()
				eachQuestWindow.Hide()
				eachQuestWindow = None
		self.wndQuestWindow = {}

		if self.wndChat:
			self.wndChat.Destroy()

		if self.wndTaskBar:
			self.wndTaskBar.Destroy()

		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Destroy()

		if self.wndEnergyBar:
			self.wndEnergyBar.Destroy()

		if self.wndCharacter:
			self.wndCharacter.Hide()
			self.wndCharacter.Destroy()

		if self.wndInventory:
			self.wndInventory.Hide()#fix
			self.wndInventory.Destroy()

		if self.wndDragonSoul:
			self.wndDragonSoul.Destroy()

		if self.wndDragonSoulRefine:
			self.wndDragonSoulRefine.Destroy()

		if self.bosstime:
			self.bosstime.Destroy()

		if self.dlgExchange:
			self.dlgExchange.Destroy()

		if self.dlgAttachMetin:
			self.dlgAttachMetin.Destroy()

		if self.dlgPointReset:
			self.dlgPointReset.Destroy()

		if self.dlgShop:
			self.dlgShop.Destroy()

		if self.dlgRestart:
			self.dlgRestart.Destroy()

		if self.dlgSystem:
			self.dlgSystem.Destroy()

		if self.dlgPassword:
			self.dlgPassword.Destroy()

		if self.wndMiniMap:
			self.wndMiniMap.Destroy()

		if self.wndSafebox:
			self.wndSafebox.Destroy()

		if self.wndWeb:
			self.wndWeb.Destroy()
			self.wndWeb = None

		if self.wndMall:
			self.wndMall.Destroy()

		if self.wndParty:
			self.wndParty.Destroy()

		if self.wndHelp:
			self.wndHelp.Destroy()

		if self.wndCube:
			self.wndCube.Destroy()

		if app.ENABLE_ACCE_COSTUME_SYSTEM and self.wndAcceCombine:
			self.wndAcceCombine.Destroy()

		if app.ENABLE_ACCE_COSTUME_SYSTEM and self.wndAcceAbsorption:
			self.wndAcceAbsorption.Destroy()

		if app.ENABLE_MOVE_CHANNEL and self.wndMoveChannel:
			self.wndMoveChannel.Destroy()
			self.wndMoveChannel = None

		if self.wndCubeResult:
			self.wndCubeResult.Destroy()

		if self.wndMessenger:
			self.wndMessenger.Destroy()

		if self.wndGuild:
			self.wndGuild.Destroy()

		if self.wndGuildStorage:
			self.wndGuildStorage.Destroy()

		if self.privateShopBuilder:
			self.privateShopBuilder.Destroy()

		if self.dlgRefineNew:
			self.dlgRefineNew.Destroy()

		if self.wndGuildBuilding:
			self.wndGuildBuilding.Destroy()

		if self.wndGameButton:
			self.wndGameButton.Destroy()
			
		# ITEM_MALL
		if self.mallPageDlg:
			self.mallPageDlg.Destroy()
		# END_OF_ITEM_MALL

		# ACCESSORY_REFINE_ADD_METIN_STONE
		if self.wndItemSelect:
			self.wndItemSelect.Destroy()
		# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

		if app.ENABLE_RENDER_TARGET_PREVIEW:
			if self.wndTargetRender:
				self.wndTargetRender.Hide()
				self.wndTargetRender.Destroy()
				del self.wndTargetRender

		if app.ENABLE_LOADING_PERFORMANCE:
			if self.wndAffectShower:
				self.wndAffectShower.Destroy()
				del self.wndAffectShower

		if app.ENABLE_SWITCHBOT_SYSTEM:
			if self.wndSwitchbot:
				self.wndSwitchbot.Hide()
				self.wndSwitchbot.Destroy()
				self.wndSwitchbot = None
				del self.wndSwitchbot

		if app.ENABLE_GEM_SYSTEM:
			if self.wndExpandedMoneyTaskBar:
				self.wndExpandedMoneyTaskBar.Destroy()
				del self.wndExpandedMoneyTaskBar

			if self.wndGemShop:
				self.wndGemShop.Destroy()
				del self.wndGemShop
				
			if self.wndItemSelectEx:
				self.wndItemSelectEx.Destroy()
				del self.wndItemSelectEx

		if app.ENABLE_EVENT_MANAGER:
			if self.wndEventOverview:
				self.wndEventOverview.Hide()
				self.wndEventOverview.Destroy()
			del self.wndEventOverview

		if app.ENABLE_BIOLOG_SYSTEM:
			if self.wndBiologManager:
				for wnd in self.wndBiologManager.values():
					wnd.Destroy()
			for wnd in self.wndBiologManager.values():
				del wnd

		if app.ENABLE_SKILL_GROUP_GUI:
			if self.wndSkillGroup:
				self.wndSkillGroup.Destroy()
				del self.wndSkillGroup

		if app.__AUTO_SKILL_READER__:
			if self.wndAutoSkillReader:
				self.wndAutoSkillReader.Close()
				self.wndAutoSkillReader.Destroy()
				self.wndAutoSkillReader = None

		if app.ENABLE_INGAME_WIKI:
			if self.wndWiki:
				self.wndWiki.Hide()
				self.wndWiki.Destroy()
			del self.wndWiki

		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.Hide()
				self.wndPrivateShopPanel.Destroy()
				
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.Hide()
				self.wndPrivateShopSearch.Destroy()

			del self.wndPrivateShopPanel
			del self.wndPrivateShopSearch
			self.privateShopTitleBoardDict = {}

		if app.ENABLE_ITEM_MANAGER:
			if self.wndItemManager:
				del self.wndItemManager

		if app.ENABLE_REMOTE_SHOP:
			if self.wndRemoteShop:
				del self.wndRemoteShop

		if app.ENABLE_PICK_FILTER:
			if self.wndPickUpFilter:
				self.wndPickUpFilter.Destroy()
				del self.wndPickUpFilter

		if app.ENABLE_MAINTENANCE_SYSTEM:
			if self.wndMaintenance:
				self.wndMaintenance.Destroy()
			del self.wndMaintenance

		if app.ENABLE_HUNTING_SYSTEM:
			if self.wndHunting:
				self.wndHunting.Destroy()
			del self.wndHunting
			if self.wndHuntingSelect:
				self.wndHuntingSelect.Destroy()
			del self.wndHuntingSelect
			if self.wndHuntingReward:
				self.wndHuntingReward.Destroy()
			del self.wndHuntingReward

		if app.BL_67_ATTR:
			if self.wndAttr67Add:
				del self.wndAttr67Add

		if app.__SPIN_WHEEL__:
			if self.wndSpinWheel:
				self.wndSpinWheel.Close()
				self.wndSpinWheel.Destroy()
				self.wndSpinWheel = None

		if app.__SPIN_WHEEL_BOSS__:
			if self.wndSpinWheelBoss:
				self.wndSpinWheelBoss.Close()
				self.wndSpinWheelBoss.Destroy()
				self.wndSpinWheelBoss = None

		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			if self.wndDungeonInfo:
				self.wndDungeonInfo.Destroy()
				del self.wndDungeonInfo

		if app.ENABLE_GLOBAL_REWARD:
			if self.wndReward:
				self.wndReward.Close()
				self.wndReward.Destroy()
				self.wndReward = None

		if constInfo.ENABLE_SKYBOX_WINDOW:
			if self.wndSkyBox:
				self.wndSkyBox.Hide()
				self.wndSkyBox.Destroy()
				del self.wndSkyBox

		if app.ENABLE_ANTI_MULTIPLE_FARM:
			if self.wndAntiMultipleFarm:
				self.wndAntiMultipleFarm.Hide()
				self.wndAntiMultipleFarm.Destroy()
			del self.wndAntiMultipleFarm

		if app.ENABLE_VOTE_4_BUFF:
			if self.wndVote4Buff:
				self.wndVote4Buff.Hide()
				self.wndVote4Buff.Destroy()
				# self.wndVote4Buff = None
			del self.wndVote4Buff

		if app.ENABLE_RANK_PLAYER:
			if self.wndRankInfo:
				self.wndRankInfo.Hide()
				self.wndRankInfo.Destroy()
				del self.wndRankInfo

		if app.ENABLE_PLAYER_RANKING:
			if self.wndRankInfo:
				self.wndRankInfo.Hide()
				self.wndRankInfo.Destroy()
				del self.wndRankInfo

		if app.ENABLE_BATTLE_PASS:
			if self.wndBattlePass:
				self.wndBattlePass.Destroy()
				del self.wndBattlePass
				
			if self.wndBattlePassButton:
				self.wndBattlePassButton.Destroy()	
				del self.wndBattlePassButton

		if app.ENABLE_ITEMSHOP:
			if self.wndItemshop:
				self.wndItemshop.Hide()
				self.wndItemshop.Destroy()
			del self.wndItemshop

		if app.ENABLE_GUILD_SAFEBOX:
			if self.wndGuildSafebox:
				self.wndGuildSafebox.Destroy()
			del self.wndGuildSafebox

		del self.wndGuildStorage

		if app.ENABLE_EVENT_CALENDAR:
			if self.wndEventManager:
				self.wndEventManager.Hide()
				self.wndEventManager.Destroy()
				self.wndEventManager = None

			if self.wndEventIcon:
				self.wndEventIcon.Hide()
				self.wndEventIcon.Destroy()
				self.wndEventIcon = None

		if app.FAST_EQUIP_WORLDARD:
			if self.wndFastEquip:
				self.wndFastEquip.Close()
				self.wndFastEquip.Destroy()
				del self.wndFastEquip

		self.wndChatLog.Destroy()
		for btn in self.questButtonList:
			btn.SetEvent(0)
		for btn in self.whisperButtonList:
			btn.SetEvent(0)
		for dlg in self.whisperDialogDict.itervalues():
			dlg.Destroy()
		for brd in self.guildScoreBoardDict.itervalues():
			brd.Destroy()
		for dlg in self.equipmentDialogDict.itervalues():
			dlg.Destroy()

		# ITEM_MALL
		del self.mallPageDlg
		# END_OF_ITEM_MALL

		del self.wndGuild
		del self.wndMessenger
		del self.wndUICurtain
		del self.wndChat
		del self.wndTaskBar
		if self.wndExpandedTaskBar:
			del self.wndExpandedTaskBar
		del self.wndEnergyBar
		del self.wndCharacter
		del self.wndInventory
		if self.wndDragonSoul:
			del self.wndDragonSoul
		if self.wndDragonSoulRefine:
			del self.wndDragonSoulRefine
		del self.bosstime
		del self.dlgExchange
		del self.dlgAttachMetin
		del self.dlgPointReset
		del self.dlgShop
		del self.dlgRestart
		del self.dlgSystem
		del self.dlgPassword
		del self.hyperlinkItemTooltip
		del self.tooltipItem
		del self.tooltipSkill
		del self.wndMiniMap
		del self.wndSafebox
		del self.wndMall
		del self.wndParty
		del self.wndHelp
		del self.wndCube
		del self.wndCubeResult
		del self.privateShopBuilder
		del self.inputDialog
		del self.wndChatLog
		del self.dlgRefineNew
		del self.wndGuildBuilding
		del self.wndGameButton
		del self.tipBoard
		del self.bigBoard
		del self.wndItemSelect

		if app.__DAILY_QUESTS__:
			if self.wndDailyQuests:
				self.wndDailyQuests.Hide()
				self.wndDailyQuests.Destroy()
				del self.wndDailyQuests

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			del self.wndAcceCombine
			del self.wndAcceAbsorption

		if app.ENABLE_CHEST_DROP_INFO:
			if self.wndChestDropInfo:
				del self.wndChestDropInfo

		if app.__BL_MULTI_LANGUAGE_PREMIUM__:
			del self.countryTooltip

		self.questButtonList = []
		self.whisperButtonList = []
		self.whisperDialogDict = {}
		self.privateShopAdvertisementBoardDict = {}
		self.guildScoreBoardDict = {}
		self.equipmentDialogDict = {}

		uiChat.DestroyChatInputSetWindow()

	## Skill
	def OnUseSkill(self, slotIndex, coolTime):
		self.wndCharacter.OnUseSkill(slotIndex, coolTime)
		self.wndTaskBar.OnUseSkill(slotIndex, coolTime)
		self.wndGuild.OnUseSkill(slotIndex, coolTime)

	def OnActivateSkill(self, slotIndex):
		self.wndCharacter.OnActivateSkill(slotIndex)
		self.wndTaskBar.OnActivateSkill(slotIndex)

	def OnDeactivateSkill(self, slotIndex):
		self.wndCharacter.OnDeactivateSkill(slotIndex)
		self.wndTaskBar.OnDeactivateSkill(slotIndex)

	def OnChangeCurrentSkill(self, skillSlotNumber):
		self.wndTaskBar.OnChangeCurrentSkill(skillSlotNumber)

	def SelectMouseButtonEvent(self, dir, event):
		self.wndTaskBar.SelectMouseButtonEvent(dir, event)

	## Refresh
	def RefreshAlignment(self):
		self.wndCharacter.RefreshAlignment()

	def RefreshStatus(self):
		self.wndTaskBar.RefreshStatus()
		self.wndCharacter.RefreshStatus()
		self.wndInventory.RefreshStatus()
		if self.wndEnergyBar:
			self.wndEnergyBar.RefreshStatus()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.RefreshStatus()
		if app.ENABLE_GEM_SYSTEM:
			self.wndExpandedMoneyTaskBar.RefreshStatus()
		if app.__AUTO_SKILL_READER__:
			if self.wndAutoSkillReader:
				if self.wndAutoSkillReader.IsShow():
					self.wndAutoSkillReader.Refresh()
		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			if self.wndPrivateShopPanel.IsShow():
				self.wndPrivateShopPanel.Refresh()
		if app.ENABLE_ITEMSHOP:
			if self.wndItemshop.IsShow():
				self.wndItemshop.SetCoins()

	def RefreshStamina(self):
		self.wndTaskBar.RefreshStamina()

	def RefreshSkill(self):
		self.wndCharacter.RefreshSkill()
		self.wndTaskBar.RefreshSkill()

	def RefreshInventory(self):
		self.wndTaskBar.RefreshQuickSlot()
		self.wndInventory.RefreshItemSlot()
		if app.FAST_EQUIP_WORLDARD:
			self.wndFastEquip.RefreshEquipSlotWindow()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.RefreshItemSlot()
		if app.__AUTO_SKILL_READER__:
			if self.wndAutoSkillReader:
				if self.wndAutoSkillReader.IsShow():
					self.wndAutoSkillReader.Refresh()

	def RefreshCharacter(self):
		self.wndCharacter.RefreshCharacter()
		self.wndTaskBar.RefreshQuickSlot()

	def RefreshQuest(self):
		self.wndCharacter.RefreshQuest()

	def RefreshSafebox(self):
		self.wndSafebox.RefreshSafebox()

	# ITEM_MALL
	def RefreshMall(self):
		self.wndMall.RefreshMall()

	def OpenItemMall(self):
		if not self.mallPageDlg:
			self.mallPageDlg = uiShop.MallPageDialog()

		self.mallPageDlg.Open()
	# END_OF_ITEM_MALL

	def RefreshMessenger(self):
		self.wndMessenger.RefreshMessenger()

	def RefreshGuildInfoPage(self):
		self.wndGuild.RefreshGuildInfoPage()

	def RefreshGuildBoardPage(self):
		self.wndGuild.RefreshGuildBoardPage()

	def RefreshGuildMemberPage(self):
		self.wndGuild.RefreshGuildMemberPage()

	def RefreshGuildMemberPageGradeComboBox(self):
		self.wndGuild.RefreshGuildMemberPageGradeComboBox()

	def RefreshGuildSkillPage(self):
		self.wndGuild.RefreshGuildSkillPage()

	def RefreshGuildGradePage(self):
		self.wndGuild.RefreshGuildGradePage()

	def DeleteGuild(self):
		self.wndMessenger.ClearGuildMember()
		self.wndGuild.DeleteGuild()

	def OnBlockMode(self, mode):
		self.dlgSystem.OnBlockMode(mode)

	## Calling Functions
	# PointReset
	def OpenPointResetDialog(self):
		self.dlgPointReset.Show()
		self.dlgPointReset.SetTop()

	def ClosePointResetDialog(self):
		self.dlgPointReset.Close()

	# Shop
	def OpenShopDialog(self, vid, tabCount = 0):
		self.wndInventory.Show()
		self.wndInventory.SetTop()
		self.dlgShop.Open(vid, tabCount)
		self.dlgShop.SetTop()

	def CloseShopDialog(self):
		self.dlgShop.Close()

	def RefreshShopDialog(self):
		self.dlgShop.Refresh()

	## Quest
	def OpenCharacterWindowQuestPage(self):
		self.wndCharacter.Show()
		self.wndCharacter.SetState("QUEST")

	def OpenQuestWindow(self, skin, idx):

		wnds = ()

		q = uiQuest.QuestDialog(skin, idx)
		q.SetWindowName("QuestWindow" + str(idx))
		q.Show()
		if skin:
			q.Lock()
			wnds = self.__HideWindows()

			# UNKNOWN_UPDATE
			q.AddOnDoneEvent(lambda tmp_self, args=wnds: self.__ShowWindows(args))
			# END_OF_UNKNOWN_UPDATE

		if skin:
			q.AddOnCloseEvent(q.Unlock)
		q.AddOnCloseEvent(lambda key = self.wndQuestWindowNewKey:ui.__mem_func__(self.RemoveQuestDialog)(key))
		self.wndQuestWindow[self.wndQuestWindowNewKey] = q

		self.wndQuestWindowNewKey = self.wndQuestWindowNewKey + 1

		# END_OF_UNKNOWN_UPDATE

	def RemoveQuestDialog(self, key):
		del self.wndQuestWindow[key]

	## Exchange
	def StartExchange(self):
		self.dlgExchange.OpenDialog()
		self.dlgExchange.Refresh()

	def EndExchange(self):
		self.dlgExchange.CloseDialog()

	def RefreshExchange(self):
		self.dlgExchange.Refresh()
		
	if app.WJ_ENABLE_TRADABLE_ICON:
		def AddExchangeItemSlotIndex(self, idx):
			self.dlgExchange.AddExchangeItemSlotIndex(idx)

	def OpenAttachMetin(self, metinSlotPos, targetSlotPos):
		self.dlgAttachMetin.Open(metinSlotPos, targetSlotPos)

	## Party
	if app.BL_PARTY_UPDATE:
		def AddPartyMember(self, pid, name, mapIdx, channel):
			self.wndParty.AddPartyMember(pid, name, mapIdx, channel)
			self.__ArrangeQuestButton()
	else:
		def AddPartyMember(self, pid, name):
			self.wndParty.AddPartyMember(pid, name)
			self.__ArrangeQuestButton()
			

	def UpdatePartyMemberInfo(self, pid):
		self.wndParty.UpdatePartyMemberInfo(pid)

	def RemovePartyMember(self, pid):
		self.wndParty.RemovePartyMember(pid)

		self.__ArrangeQuestButton()

	if app.BL_PARTY_UPDATE:
		def LinkPartyMember(self, pid, vid, mapIdx, channel):
			self.wndParty.LinkPartyMember(pid, vid, mapIdx, channel)
	else:
		def LinkPartyMember(self, pid, vid):
			self.wndParty.LinkPartyMember(pid, vid)

	def UnlinkPartyMember(self, pid):
		self.wndParty.UnlinkPartyMember(pid)

	def UnlinkAllPartyMember(self):
		self.wndParty.UnlinkAllPartyMember()

	def ExitParty(self):
		self.wndParty.ExitParty()

		self.__ArrangeQuestButton()

	def PartyHealReady(self):
		self.wndParty.PartyHealReady()

	def ChangePartyParameter(self, distributionMode):
		self.wndParty.ChangePartyParameter(distributionMode)

	def UpdatePartyPosition(self, newY):
		self.wndParty.UpdatePartyWindowPosition(newY)

	## Safebox
	def AskSafeboxPassword(self):
		if self.wndSafebox.IsShow():
			return

		# SAFEBOX_PASSWORD
		self.dlgPassword.SetTitle(localeInfo.PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/safebox_password ")
		# END_OF_SAFEBOX_PASSWORD

		self.dlgPassword.ShowDialog()

	def OpenSafeboxWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndSafebox.ShowWindow(size)

	def RefreshSafeboxMoney(self):
		self.wndSafebox.RefreshSafeboxMoney()

	def CommandCloseSafebox(self):
		self.wndSafebox.CommandCloseSafebox()

	# ITEM_MALL
	def AskMallPassword(self):
		if self.wndMall.IsShow():
			return
		self.dlgPassword.SetTitle(localeInfo.MALL_PASSWORD_TITLE)
		self.dlgPassword.SetSendMessage("/mall_password ")
		self.dlgPassword.ShowDialog()

	def OpenMallWindow(self, size):
		self.dlgPassword.CloseDialog()
		self.wndMall.ShowWindow(size)

	def CommandCloseMall(self):
		self.wndMall.CommandCloseMall()
	# END_OF_ITEM_MALL

	## Guild
	def OnStartGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnStartGuildWar(guildSelf, guildOpp)

		guildWarScoreBoard = uiGuild.GuildWarScoreBoard()
		guildWarScoreBoard.Open(guildSelf, guildOpp)
		guildWarScoreBoard.Show()
		self.guildScoreBoardDict[uiGuild.GetGVGKey(guildSelf, guildOpp)] = guildWarScoreBoard

	def OnEndGuildWar(self, guildSelf, guildOpp):
		self.wndGuild.OnEndGuildWar(guildSelf, guildOpp)

		key = uiGuild.GetGVGKey(guildSelf, guildOpp)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].Destroy()
		del self.guildScoreBoardDict[key]

	# GUILDWAR_MEMBER_COUNT
	def UpdateMemberCount(self, gulidID1, memberCount1, guildID2, memberCount2):
		key = uiGuild.GetGVGKey(gulidID1, guildID2)

		if not self.guildScoreBoardDict.has_key(key):
			return

		self.guildScoreBoardDict[key].UpdateMemberCount(gulidID1, memberCount1, guildID2, memberCount2)
	# END_OF_GUILDWAR_MEMBER_COUNT

	def OnRecvGuildWarPoint(self, gainGuildID, opponentGuildID, point):
		key = uiGuild.GetGVGKey(gainGuildID, opponentGuildID)
		if not self.guildScoreBoardDict.has_key(key):
			return

		guildBoard = self.guildScoreBoardDict[key]
		guildBoard.SetScore(gainGuildID, opponentGuildID, point)

	## PK Mode
	def OnChangePKMode(self):
		self.wndCharacter.RefreshAlignment()
		self.dlgSystem.OnChangePKMode()

	## Refine
	def OpenRefineDialog(self, targetItemPos, nextGradeItemVnum, cost, prob, type):
		self.dlgRefineNew.Open(targetItemPos, nextGradeItemVnum, cost, prob, type)

	def AppendMaterialToRefineDialog(self, vnum, count):
		self.dlgRefineNew.AppendMaterial(vnum, count)

	## Show & Hide
	def ShowDefaultWindows(self):
		self.wndTaskBar.Show()
		self.wndMiniMap.Show()
		self.wndMiniMap.ShowMiniMap()
		if self.wndEnergyBar:
			self.wndEnergyBar.Show()
		# if app.ENABLE_ITEMSHOP and self.wndItemshop:
			# self.wndItemshop.Show()

	def ShowAllWindows(self):
		self.wndTaskBar.Show()
		self.wndCharacter.Show()
		self.wndInventory.Show()
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.Show()
			self.wndDragonSoulRefine.Show()
		self.wndChat.Show()
		self.wndMiniMap.Show()
		if app.ENABLE_LOADING_PERFORMANCE:
			self.wndAffectShower.Show()
		if self.wndEnergyBar:
			self.wndEnergyBar.Show()
		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Show()
			self.wndExpandedTaskBar.SetTop()
		if app.ENABLE_GEM_SYSTEM:
			if self.wndExpandedMoneyTaskBar:
				self.wndExpandedMoneyTaskBar.Show()
				self.wndExpandedMoneyTaskBar.SetTop()

	def HideAllWindows(self):
		if self.wndTaskBar:
			self.wndTaskBar.Hide()

		if self.wndEnergyBar:
			self.wndEnergyBar.Hide()

		if app.ENABLE_DETAILS_UI or app.ENABLE_SKILL_COLOR_SYSTEM:
			if self.wndCharacter:
				self.wndCharacter.Close()
		else:
			if self.wndCharacter:
				self.wndCharacter.Hide()

		if self.wndInventory:
			self.wndInventory.Hide()

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.Hide()
			self.wndDragonSoulRefine.Hide()

		if self.wndChat:
			if app.ENABLE_LOADING_PERFORMANCE:
				self.wndChat.imgChatBarLeft.Hide()
				self.wndChat.imgChatBarRight.Hide()
				self.wndChat.imgChatBarMiddle.Hide()
			if app.__BL_MULTI_LANGUAGE_ULTIMATE__:
				self.wndChat.HideShit()
			self.wndChat.Hide()

		if app.ENABLE_LOADING_PERFORMANCE:
			if self.wndAffectShower:
				self.wndAffectShower.Hide()

		if self.wndMiniMap:
			self.wndMiniMap.Hide()

		if self.wndMessenger:
			self.wndMessenger.Hide()

		if self.wndGuild:
			self.wndGuild.Hide()

		if app.__DAILY_QUESTS__:
			if self.wndDailyQuests:
				self.wndDailyQuests.Hide()

		if self.bosstime:
			self.bosstime.Hide()

		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.Hide()

		if app.ENABLE_MOVE_CHANNEL:
			if self.wndMoveChannel:
				self.wndMoveChannel.Hide()

		if app.ENABLE_CHEST_DROP_INFO:
			if self.wndChestDropInfo:
				self.wndChestDropInfo.Hide()

		if app.ENABLE_SWITCHBOT_SYSTEM:
			if self.wndSwitchbot:
				self.wndSwitchbot.Hide()

		if app.ENABLE_INGAME_WIKI:
			if self.wndWiki:
				self.wndWiki.Hide()

		if app.ENABLE_GEM_SYSTEM:
			if self.wndExpandedMoneyTaskBar:
				self.wndExpandedMoneyTaskBar.Hide()

		if app.ENABLE_BIOLOG_SYSTEM:
			if self.wndBiologManager:
				for wnd in self.wndBiologManager.values():
					wnd.Hide()

		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			## TODO
			self.wndPrivateShopPanel.Hide()
			self.wndPrivateShopSearch.Hide()

			self.wndPrivateShopPanel.CancelInputPrice()
			self.wndPrivateShopPanel.CancelItemCheckin()

		if app.ENABLE_PICK_FILTER:
			if self.wndPickUpFilter:
				self.wndPickUpFilter.Hide()

		if app.ENABLE_HUNTING_SYSTEM:
			if self.wndHunting:
				self.wndHunting.Hide()
			if self.wndHuntingSelect:
				self.wndHuntingSelect.Hide()
			if self.wndHuntingReward:
				self.wndHuntingReward.Hide()

		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			if self.wndDungeonInfo:
				self.wndDungeonInfo.Hide()

		if app.ENABLE_ANTI_MULTIPLE_FARM:
			if self.wndAntiMultipleFarm:
				self.wndAntiMultipleFarm.Hide()

		if app.ENABLE_VOTE_4_BUFF:
			if self.wndVote4Buff:
				self.wndVote4Buff.Hide()

			if self.wndGameButton:
				self.wndGameButton.HideVote4BuffButton()

		if app.__SPIN_WHEEL__:
			if self.wndSpinWheel:
				self.wndSpinWheel.Hide()

		if app.__SPIN_WHEEL_BOSS__:
			if self.wndSpinWheelBoss:
				self.wndSpinWheelBoss.Hide()

		if app.ENABLE_ITEMSHOP and self.wndItemshop:
			self.wndItemshop.Hide()

		if app.ENABLE_EVENT_CALENDAR:
			if self.wndEventManager:
				self.wndEventManager.Hide()

			if self.wndEventIcon:
				self.wndEventIcon.Hide()

		if self.wndGameButton:
			self.wndGameButton.HideSkillAndStatusButton()

	def OpenBossTime(self):
		if self.bosstime:
			if self.bosstime.IsShow():
				self.bosstime.Close()
			else:
				self.bosstime.Open()

	def ShowMouseImage(self):
		self.wndTaskBar.ShowMouseImage()

	def HideMouseImage(self):
		self.wndTaskBar.HideMouseImage()

	def ToggleChat(self):
		if True == self.wndChat.IsEditMode():
			self.wndChat.CloseChat()
		else:
			if self.wndWeb and self.wndWeb.IsShow():
				pass
			else:
				self.wndChat.OpenChat()

	def IsOpenChat(self):
		return self.wndChat.IsEditMode()

	def SetChatFocus(self):
		self.wndChat.SetChatFocus()

	if app.ENABLE_RENEWAL_DEAD_PACKET:
		def OpenRestartDialog(self, d_time):
			self.dlgRestart.OpenDialog(d_time)
			self.dlgRestart.SetTop()
	else:
		def OpenRestartDialog(self):
			self.dlgRestart.OpenDialog()
			self.dlgRestart.SetTop()

	def CloseRestartDialog(self):
		self.dlgRestart.Close()

	def ToggleSystemDialog(self):
		if False == self.dlgSystem.IsShow():
			self.dlgSystem.OpenDialog()
			self.dlgSystem.SetTop()
		else:
			self.dlgSystem.Close()

	def OpenSystemDialog(self):
		self.dlgSystem.OpenDialog()
		self.dlgSystem.SetTop()

	def ToggleMessenger(self):
		if self.wndMessenger.IsShow():
			self.wndMessenger.Hide()
		else:
			self.wndMessenger.SetTop()
			self.wndMessenger.Show()

	def ToggleMiniMap(self):
		if app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT):
			if False == self.wndMiniMap.isShowMiniMap():
				self.wndMiniMap.ShowMiniMap()
				self.wndMiniMap.SetTop()
			else:
				self.wndMiniMap.HideMiniMap()

		else:
			self.wndMiniMap.ToggleAtlasWindow()

	def PressMKey(self):
		if app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT):
			self.ToggleMessenger()

		else:
			self.ToggleMiniMap()

	def SetMapName(self, mapName):
		self.wndMiniMap.SetMapName(mapName)

	def MiniMapScaleUp(self):
		self.wndMiniMap.ScaleUp()

	def MiniMapScaleDown(self):
		self.wndMiniMap.ScaleDown()

	def ToggleCharacterWindow(self, state):
		if False == player.IsObserverMode():
			if False == self.wndCharacter.IsShow():
				self.OpenCharacterWindowWithState(state)
			else:
				if state == self.wndCharacter.GetState():
					self.wndCharacter.OverOutItem()
					if app.ENABLE_DETAILS_UI or app.ENABLE_SKILL_COLOR_SYSTEM:
						self.wndCharacter.Close()
					else:
						self.wndCharacter.Hide()
				else:
					self.wndCharacter.SetState(state)

	def OpenCharacterWindowWithState(self, state):
		if False == player.IsObserverMode():
			self.wndCharacter.SetState(state)
			self.wndCharacter.Show()
			self.wndCharacter.SetTop()

	def ToggleCharacterWindowStatusPage(self):
		self.ToggleCharacterWindow("STATUS")

	def ToggleInventoryWindow(self):
		if False == player.IsObserverMode():
			if False == self.wndInventory.IsShow():
				self.wndInventory.Show()
				self.wndInventory.SetTop()
			else:
				self.wndInventory.OverOutItem()
				self.wndInventory.Close()

	def ToggleExpandedButton(self):
		if False == player.IsObserverMode():
			if False == self.wndExpandedTaskBar.IsShow():
				self.wndExpandedTaskBar.Show()
				self.wndExpandedTaskBar.SetTop()
			else:
				self.wndExpandedTaskBar.Close()

	def DragonSoulActivate(self, deck):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.ActivateDragonSoulByExtern(deck)

	def DragonSoulDeactivate(self):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.DeactivateDragonSoul()

	def Highligt_Item(self, inven_type, inven_pos):
		if player.DRAGON_SOUL_INVENTORY == inven_type:
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				self.wndDragonSoul.HighlightSlot(inven_pos)
		elif app.ENABLE_HIGHLIGHT_SLOT_SYSTEM and player.SLOT_TYPE_INVENTORY == inven_type:
			self.wndInventory.HighlightSlot(inven_pos)

	def DragonSoulGiveQuilification(self):
		self.DRAGON_SOUL_IS_QUALIFIED = True
		if self.wndExpandedTaskBar:
			self.wndExpandedTaskBar.SetToolTipText(uiTaskBar.ExpandedTaskBar.BUTTON_DRAGON_SOUL, uiScriptLocale.TASKBAR_DRAGON_SOUL)

	def ToggleDragonSoulWindow(self):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if False == self.wndDragonSoul.IsShow():
					if self.DRAGON_SOUL_IS_QUALIFIED:
						self.wndDragonSoul.Show()
					else:
						try:
							self.wndPopupDialog.SetText(localeInfo.DRAGON_SOUL_UNQUALIFIED)
							self.wndPopupDialog.Open()
						except:
							self.wndPopupDialog = uiCommon.PopupDialog()
							self.wndPopupDialog.SetText(localeInfo.DRAGON_SOUL_UNQUALIFIED)
							self.wndPopupDialog.Open()
				else:
					self.wndDragonSoul.Close()

	def ToggleDragonSoulWindowWithNoInfo(self):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if False == self.wndDragonSoul.IsShow():
					if self.DRAGON_SOUL_IS_QUALIFIED:
						self.wndDragonSoul.Show()
				else:
					self.wndDragonSoul.Close()

	def FailDragonSoulRefine(self, reason, inven_type, inven_pos):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if True == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.RefineFail(reason, inven_type, inven_pos)

	def SucceedDragonSoulRefine(self, inven_type, inven_pos):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if True == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.RefineSucceed(inven_type, inven_pos)

	def OpenDragonSoulRefineWindow(self):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if False == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.Show()
					if None != self.wndDragonSoul:
						if False == self.wndDragonSoul.IsShow():
							self.wndDragonSoul.Show()

	def CloseDragonSoulRefineWindow(self):
		if False == player.IsObserverMode():
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				if True == self.wndDragonSoulRefine.IsShow():
					self.wndDragonSoulRefine.Close()


	def ToggleGuildWindow(self):
		if not self.wndGuild.IsShow():
			if self.wndGuild.CanOpen():
				self.wndGuild.Open()
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GUILD_YOU_DO_NOT_JOIN)
		else:
			self.wndGuild.OverOutItem()
			self.wndGuild.Hide()

	def ToggleChatLogWindow(self):
		if self.wndChatLog.IsShow():
			self.wndChatLog.Hide()
		else:
			self.wndChatLog.Show()

	def CheckGameButton(self):
		if self.wndGameButton:
			self.wndGameButton.CheckGameButton()

	def __OnClickStatusPlusButton(self):
		self.ToggleCharacterWindow("STATUS")

	def __OnClickSkillPlusButton(self):
		self.ToggleCharacterWindow("SKILL")

	def __OnClickQuestButton(self):
		self.ToggleCharacterWindow("QUEST")

	def __OnClickHelpButton(self):
		player.SetPlayTime(1)
		self.CheckGameButton()
		self.OpenHelpWindow()

	def __OnClickBuildButton(self):
		self.BUILD_OpenWindow()

	def OpenHelpWindow(self):
		self.wndUICurtain.Show()
		self.wndHelp.Open()

	def CloseHelpWindow(self):
		self.wndUICurtain.Hide()
		self.wndHelp.Close()

	def OpenWebWindow(self, url):
		self.wndWeb.Open(url)

		self.wndChat.CloseChat()

	# show GIFT
	def ShowGift(self):
		self.wndTaskBar.ShowGift()

	if app.FAST_EQUIP_WORLDARD:
		def OpenFastEquip(self):
			if self.wndFastEquip:
				if self.wndFastEquip.IsShow():
					self.wndFastEquip.Hide()
				else:
					self.wndFastEquip.Show()

	def CloseWbWindow(self):
		self.wndWeb.Close()

	def OpenCubeWindow(self):
		self.wndCube.Open()

		if False == self.wndInventory.IsShow():
			self.wndInventory.Show()

		if app.ENABLE_GEM_SYSTEM:
			if False == self.wndExpandedMoneyTaskBar.IsShow():
				self.wndExpandedMoneyTaskBar.Show()

	def UpdateCubeInfo(self, gold, itemVnum, count):
		self.wndCube.UpdateInfo(gold, itemVnum, count)

	def CloseCubeWindow(self):
		self.wndCube.Close()

	def FailedCubeWork(self):
		self.wndCube.Refresh()

	def SucceedCubeWork(self, itemVnum, count):
		self.wndCube.Clear()

		if 0:
			self.wndCubeResult.SetPosition(*self.wndCube.GetGlobalPosition())
			self.wndCubeResult.SetCubeResultItem(itemVnum, count)
			self.wndCubeResult.Open()
			self.wndCubeResult.SetTop()

	if app.ENABLE_MOVE_CHANNEL:
		def ToggleMoveChannelWindow(self):
			if not player.IsObserverMode():
				if not self.wndMoveChannel.IsShow():
					self.wndMoveChannel.Open()
				else:
					self.wndMoveChannel.Hide()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def ActRefreshInventory(self):
			if app.ENABLE_SPECIAL_INVENTORY:
				if self.wndInventory.GetInventoryType() == player.INVENTORY_TYPE_INVENTORY:
					self.wndInventory.RefreshBagSlotWindow()
				else:
					self.wndInventory.RefreshSpecialInventory()
			else:
				self.wndInventory.RefreshBagSlotWindow()

		def ActAcce(self, iAct, bWindow):
			board = (self.wndAcceAbsorption,self.wndAcceCombine)[int(bWindow)]
			if iAct == 1:
				self.ActAcceOpen(board)
			elif iAct == 2:
				self.ActAcceClose(board)
			elif iAct == 3 or iAct == 4:
				self.ActAcceRefresh(board, iAct)

		def ActAcceOpen(self,board):
			if not board.IsOpened():
				board.Open()
			if not self.wndInventory.IsShow():
				self.wndInventory.Show()
			self.wndInventory.RefreshBagSlotWindow()

		def ActAcceClose(self,board):
			if board.IsOpened():
				board.Close()
			self.wndInventory.RefreshBagSlotWindow()

		def ActAcceRefresh(self,board,iAct):
			if board.IsOpened():
				board.Refresh(iAct)
			self.ActRefreshInventory()

	if app.__DAILY_QUESTS__:
		def RecvDailyQuests(self, id, type, vnum, value, amount, progress):
			self.wndDailyQuests.AppendQuest(id, type, vnum, amount, progress)
			
		def RecvDailyRewards(self, rewards, progress):
			self.wndDailyQuests.AppendRewards(rewards, progress)
			
		def RecvUpdateDailyQuests(self, id, progress):
			self.wndDailyQuests.UpdateQuest(id, progress)
			
		def RecvUpdateDailyRewards(self, progress):
			self.wndDailyQuests.UpdateDailyRewards(progress)
			
		def ResetDailyRewards(self):
			self.wndDailyQuests.ResetDailyRewards()
					
		def ToggleDailyQuest(self):
			if self.wndDailyQuests.IsShow():
				self.wndDailyQuests.Hide()
			else:
				self.wndDailyQuests.Show()

	def __HideWindows(self):
		hideWindows = self.wndTaskBar,\
						self.wndCharacter,\
						self.wndInventory,\
						self.wndMiniMap,\
						self.wndGuild,\
						self.wndMessenger,\
						self.wndChat,\
						self.wndParty,\
						self.wndGameButton,

		if self.wndEnergyBar:
			hideWindows += self.wndEnergyBar,

		if self.wndExpandedTaskBar:
			hideWindows += self.wndExpandedTaskBar,

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			hideWindows += self.wndDragonSoul,\
						self.wndDragonSoulRefine,

		if app.ENABLE_RENDER_TARGET_PREVIEW:
			if self.wndTargetRender:
				hideWindows += self.wndTargetRender,
	
		if app.ENABLE_LOADING_PERFORMANCE:
			if self.wndAffectShower:
				hideWindows += self.wndAffectShower,

		if app.ENABLE_SWITCHBOT_SYSTEM:
			if self.wndSwitchbot:
				hideWindows += self.wndSwitchbot,

		if app.ENABLE_GEM_SYSTEM:
			if self.wndExpandedMoneyTaskBar:
				hideWindows += self.wndExpandedMoneyTaskBar,

		if app.ENABLE_PREMIUM_PRIVATE_SHOP:
			hideWindows += self.wndPrivateShopPanel,\
						self.wndPrivateShopSearch

		if app.ENABLE_MOVE_CHANNEL and self.wndMoveChannel:
			hideWindows += self.wndMoveChannel,

		if app.ENABLE_HUNTING_SYSTEM:
			if self.wndHunting:
				hideWindows += self.wndHunting,
			if self.wndHuntingSelect:
				hideWindows += self.wndHuntingSelect,
			if self.wndHuntingReward:
				hideWindows += self.wndHuntingReward,

		if app.ENABLE_ANTI_MULTIPLE_FARM and self.wndAntiMultipleFarm:
			hideWindows += self.wndAntiMultipleFarm,

		if app.ENABLE_DAILY_BOSS:
			if self.wndWorldBoss:
				hideWindows += self.wndWorldBoss,

		if app.ENABLE_ITEMSHOP and self.wndItemshop:
			hideWindows += self.wndItemshop,

		hideWindows = filter(lambda x:x.IsShow(), hideWindows)
		map(lambda x:x.Hide(), hideWindows)

		self.HideAllQuestButton()
		self.HideAllWhisperButton()

		if self.wndChat.IsEditMode():
			self.wndChat.CloseChat()

		return hideWindows

	def __ShowWindows(self, wnds):
		map(lambda x:x.Show(), wnds)
		global IsQBHide
		if not IsQBHide:
			self.ShowAllQuestButton()
		else:
			self.HideAllQuestButton()

		self.ShowAllWhisperButton()

	def BINARY_OpenAtlasWindow(self):
		if self.wndMiniMap:
			self.wndMiniMap.ShowAtlas()

	def BINARY_SetObserverMode(self, flag):
		self.wndGameButton.SetObserverMode(flag)

	# ACCESSORY_REFINE_ADD_METIN_STONE
	def BINARY_OpenSelectItemWindow(self):
		self.wndItemSelect.Open()
	# END_OF_ACCESSORY_REFINE_ADD_METIN_STONE

	#####################################################################################
	### Private Shop ###

	def OpenPrivateShopInputNameDialog(self):
		#if player.IsInSafeArea():
		#	chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CANNOT_OPEN_PRIVATE_SHOP_IN_SAFE_AREA)
		#	return

		inputDialog = uiCommon.InputDialog()
		inputDialog.SetTitle(localeInfo.PRIVATE_SHOP_INPUT_NAME_DIALOG_TITLE)
		inputDialog.SetMaxLength(32)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OpenPrivateShopBuilder))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.ClosePrivateShopInputNameDialog))
		inputDialog.Open()
		self.inputDialog = inputDialog

	def ClosePrivateShopInputNameDialog(self):
		self.inputDialog = None
		return True

	def OpenPrivateShopBuilder(self):

		if not self.inputDialog:
			return True

		if not len(self.inputDialog.GetText()):
			return True

		self.privateShopBuilder.Open(self.inputDialog.GetText())
		self.ClosePrivateShopInputNameDialog()
		return True

	def AppearPrivateShop(self, vid, text):

		board = uiPrivateShopBuilder.PrivateShopAdvertisementBoard()
		board.Open(vid, text)

		self.privateShopAdvertisementBoardDict[vid] = board

	def DisappearPrivateShop(self, vid):

		if not self.privateShopAdvertisementBoardDict.has_key(vid):
			return

		del self.privateShopAdvertisementBoardDict[vid]
		uiPrivateShopBuilder.DeleteADBoard(vid)

	#####################################################################################
	### Equipment ###

	def OpenEquipmentDialog(self, vid):
		dlg = uiEquipmentDialog.EquipmentDialog()
		dlg.SetItemToolTip(self.tooltipItem)
		dlg.SetCloseEvent(ui.__mem_func__(self.CloseEquipmentDialog))
		dlg.Open(vid)

		self.equipmentDialogDict[vid] = dlg

	def SetEquipmentDialogItem(self, vid, slotIndex, vnum, count):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogItem(slotIndex, vnum, count)

	def SetEquipmentDialogSocket(self, vid, slotIndex, socketIndex, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogSocket(slotIndex, socketIndex, value)

	def SetEquipmentDialogAttr(self, vid, slotIndex, attrIndex, type, value):
		if not vid in self.equipmentDialogDict:
			return
		self.equipmentDialogDict[vid].SetEquipmentDialogAttr(slotIndex, attrIndex, type, value)

	def CloseEquipmentDialog(self, vid):
		if not vid in self.equipmentDialogDict:
			return
		del self.equipmentDialogDict[vid]

	#####################################################################################

	#####################################################################################
	### Quest ###
	def BINARY_ClearQuest(self, index):
		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

	def RecvQuest(self, index, name):
		# QUEST_LETTER_IMAGE
		self.BINARY_RecvQuest(index, name, "file", localeInfo.GetLetterImageName())
		# END_OF_QUEST_LETTER_IMAGE

	def BINARY_RecvQuest(self, index, name, iconType, iconName):

		btn = self.__FindQuestButton(index)
		if 0 != btn:
			self.__DestroyQuestButton(btn)

		btn = uiWhisper.WhisperButton()

		# QUEST_LETTER_IMAGE
		import item
		if "item"==iconType:
			item.SelectItem(int(iconName))
			buttonImageFileName=item.GetIconImageFileName()
		else:
			buttonImageFileName=iconName

		if iconName and (iconType not in ("item", "file")): # type "ex" implied
			btn.SetUpVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName.replace("open", "close")))
			btn.SetOverVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName))
			btn.SetDownVisual("d:/ymir work/ui/game/quest/questicon/%s" % (iconName))
		else:
			if localeInfo.IsEUROPE():
				btn.SetUpVisual(localeInfo.GetLetterCloseImageName())
				btn.SetOverVisual(localeInfo.GetLetterOpenImageName())
				btn.SetDownVisual(localeInfo.GetLetterOpenImageName())
			else:
				btn.SetUpVisual(buttonImageFileName)
				btn.SetOverVisual(buttonImageFileName)
				btn.SetDownVisual(buttonImageFileName)
				btn.Flash()
		# END_OF_QUEST_LETTER_IMAGE

		if localeInfo.IsARABIC():
			btn.SetToolTipText(name, 0, 35)
			btn.ToolTipText.SetHorizontalAlignCenter()
		else:
			btn.SetToolTipText(name, -20, 35)
			btn.ToolTipText.SetHorizontalAlignLeft()

		listOfTypes = iconType.split(",")
		if "blink" in listOfTypes:
			btn.Flash()

		listOfColors = {
			"golden":	0xFFffa200,
			"green":	0xFF00e600,
			"blue":		0xFF0099ff,
			"purple":	0xFFcc33ff,

			"fucsia":	0xFFcc0099,
			"aqua":		0xFF00ffff,
		}
		for k,v in listOfColors.iteritems():
			if k in listOfTypes:
				btn.ToolTipText.SetPackedFontColor(v)

		btn.SetEvent(ui.__mem_func__(self.__StartQuest), btn)
		btn.Show()

		btn.index = index
		btn.name = name

		self.questButtonList.insert(0, btn)
		self.__ArrangeQuestButton()

	def __ArrangeQuestButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		if self.wndParty.IsShow():
			xPos = 100 + 30
		else:
			xPos = 20

		if localeInfo.IsARABIC():
			xPos = xPos + 15

		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63

		count = 0
		for btn in self.questButtonList:

			btn.SetPosition(xPos + (int(count/yCount) * 100), yPos + (count%yCount * 63))
			count += 1
			global IsQBHide
			if IsQBHide:
				btn.Hide()
			else:
				btn.Show()

	def __StartQuest(self, btn):
		event.QuestButtonClick(btn.index)
		self.__DestroyQuestButton(btn)

	def __FindQuestButton(self, index):
		for btn in self.questButtonList:
			if btn.index == index:
				return btn

		return 0

	def __DestroyQuestButton(self, btn):
		btn.SetEvent(0)
		self.questButtonList.remove(btn)
		self.__ArrangeQuestButton()

	def HideAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Hide()

	def ShowAllQuestButton(self):
		for btn in self.questButtonList:
			btn.Show()
	#####################################################################################

	#####################################################################################
	### Whisper ###

	def __InitWhisper(self):
		chat.InitWhisper(self)

	def OpenWhisperDialogWithoutTarget(self):
		if not self.dlgWhisperWithoutTarget:
			dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
			dlgWhisper.BindInterface(self)
			dlgWhisper.LoadDialog()
			dlgWhisper.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)
			dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
			dlgWhisper.Show()
			self.dlgWhisperWithoutTarget = dlgWhisper

			self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		else:
			self.dlgWhisperWithoutTarget.SetTop()
			self.dlgWhisperWithoutTarget.OpenWithoutTarget(self.RegisterTemporaryWhisperDialog)

	def RegisterTemporaryWhisperDialog(self, name):
		if not self.dlgWhisperWithoutTarget:
			return

		btn = self.__FindWhisperButton(name)
		if 0 != btn:
			self.__DestroyWhisperButton(btn)

		elif self.whisperDialogDict.has_key(name):
			oldDialog = self.whisperDialogDict[name]
			oldDialog.Destroy()
			del self.whisperDialogDict[name]

		self.whisperDialogDict[name] = self.dlgWhisperWithoutTarget
		self.dlgWhisperWithoutTarget.OpenWithTarget(name)
		self.dlgWhisperWithoutTarget = None
		self.__CheckGameMaster(name)

	def OpenWhisperDialog(self, name):
		if not self.whisperDialogDict.has_key(name):
			dlg = self.__MakeWhisperDialog(name)
			dlg.OpenWithTarget(name)
			dlg.chatLine.SetFocus()
			dlg.Show()

			self.__CheckGameMaster(name)
			btn = self.__FindWhisperButton(name)
			if 0 != btn:
				self.__DestroyWhisperButton(btn)

	def RecvWhisper(self, name):
		if not self.whisperDialogDict.has_key(name):
			btn = self.__FindWhisperButton(name)
			if 0 == btn:
				btn = self.__MakeWhisperButton(name)
				btn.Flash()
				if app.ENABLE_FLASH_APPLICATION:
					app.FlashApplication()

				chat.AppendChat(chat.CHAT_TYPE_NOTICE, localeInfo.RECEIVE_MESSAGE % (name))

			else:
				if app.ENABLE_FLASH_APPLICATION:
					app.FlashApplication()
				btn.Flash()
		elif self.IsGameMasterName(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	if app.__WHISPER_FUTURES__:
		def RecvWhisperSave(self, name):
			if not self.whisperDialogDict.has_key(name):
				btn = self.__FindWhisperButton(name)
				if 0 == btn:
					self.__MakeWhisperButton(name)
					chat.AppendChat(chat.CHAT_TYPE_NOTICE, localeInfo.RECEIVE_MESSAGE % (name))
			elif self.IsGameMasterName(name):
				dlg = self.whisperDialogDict[name]
				dlg.SetGameMasterLook()

	def MakeWhisperButton(self, name):
		self.__MakeWhisperButton(name)

	def ShowWhisperDialog(self, btn):
		try:
			self.__MakeWhisperDialog(btn.name)
			dlgWhisper = self.whisperDialogDict[btn.name]
			dlgWhisper.OpenWithTarget(btn.name)
			dlgWhisper.Show()
			self.__CheckGameMaster(btn.name)
		except:
			import dbg
			dbg.TraceError("interface.ShowWhisperDialog - Failed to find key")

		self.__DestroyWhisperButton(btn)

	def MinimizeWhisperDialog(self, name):

		if 0 != name:
			self.__MakeWhisperButton(name)

		self.CloseWhisperDialog(name)

	def CloseWhisperDialog(self, name):

		if 0 == name:

			if self.dlgWhisperWithoutTarget:
				self.dlgWhisperWithoutTarget.Destroy()
				self.dlgWhisperWithoutTarget = None

			return

		try:
			dlgWhisper = self.whisperDialogDict[name]
			dlgWhisper.Destroy()
			del self.whisperDialogDict[name]
		except:
			import dbg
			dbg.TraceError("interface.CloseWhisperDialog - Failed to find key")

	def __ArrangeWhisperButton(self):

		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()

		xPos = screenWidth - 70
		yPos = 170 * screenHeight / 600
		yCount = (screenHeight - 330) / 63
		#yCount = (screenHeight - 285) / 63

		count = 0
		for button in self.whisperButtonList:

			button.SetPosition(xPos + (int(count/yCount) * -50), yPos + (count%yCount * 63))
			count += 1

	def __FindWhisperButton(self, name):
		for button in self.whisperButtonList:
			if button.name == name:
				return button

		return 0

	def __MakeWhisperDialog(self, name):
		dlgWhisper = uiWhisper.WhisperDialog(self.MinimizeWhisperDialog, self.CloseWhisperDialog)
		dlgWhisper.BindInterface(self)
		dlgWhisper.LoadDialog()
		dlgWhisper.SetPosition(self.windowOpenPosition*30,self.windowOpenPosition*30)
		self.whisperDialogDict[name] = dlgWhisper

		self.windowOpenPosition = (self.windowOpenPosition+1) % 5

		return dlgWhisper

	def __MakeWhisperButton(self, name):
		whisperButton = uiWhisper.WhisperButton()
		whisperButton.SetUpVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		whisperButton.SetOverVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		whisperButton.SetDownVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
		if self.IsGameMasterName(name):
			whisperButton.SetUpVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
			whisperButton.SetOverVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
			whisperButton.SetDownVisual("d:/ymir work/ui/game/windows/btn_mail_up.sub")
			whisperButton.SetToolTipTextWithColor(name, 0xffffa200)
		else:
			whisperButton.SetToolTipText(name)
		whisperButton.ToolTipText.SetHorizontalAlignCenter()
		whisperButton.SetEvent(ui.__mem_func__(self.ShowWhisperDialog), whisperButton)
		whisperButton.Show()
		whisperButton.name = name

		self.whisperButtonList.insert(0, whisperButton)
		self.__ArrangeWhisperButton()

		return whisperButton

	def __DestroyWhisperButton(self, button):
		button.SetEvent(0)
		self.whisperButtonList.remove(button)
		self.__ArrangeWhisperButton()

	def HideAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Hide()

	def ShowAllWhisperButton(self):
		for btn in self.whisperButtonList:
			btn.Show()

	def __CheckGameMaster(self, name):
		if not self.listGMName.has_key(name):
			return
		if self.whisperDialogDict.has_key(name):
			dlg = self.whisperDialogDict[name]
			dlg.SetGameMasterLook()

	def RegisterGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return
		self.listGMName[name] = "GM"

	def IsGameMasterName(self, name):
		if self.listGMName.has_key(name):
			return True
		else:
			return False

	#####################################################################################

	#####################################################################################
	### Guild Building ###

	def BUILD_OpenWindow(self):
		self.wndGuildBuilding = uiGuild.BuildGuildBuildingWindow()
		self.wndGuildBuilding.Open()
		self.wndGuildBuilding.wnds = self.__HideWindows()
		self.wndGuildBuilding.SetCloseEvent(ui.__mem_func__(self.BUILD_CloseWindow))

	def BUILD_CloseWindow(self):
		self.__ShowWindows(self.wndGuildBuilding.wnds)
		self.wndGuildBuilding = None

	def BUILD_OnUpdate(self):
		if not self.wndGuildBuilding:
			return

		if self.wndGuildBuilding.IsPositioningMode():
			import background
			x, y, z = background.GetPickingPoint()
			self.wndGuildBuilding.SetBuildingPosition(x, y, z)

	def BUILD_OnMouseLeftButtonDown(self):
		if not self.wndGuildBuilding:
			return

		# GUILD_BUILDING
		if self.wndGuildBuilding.IsPositioningMode():
			self.wndGuildBuilding.SettleCurrentPosition()
			return True
		elif self.wndGuildBuilding.IsPreviewMode():
			pass
		else:
			return True
		# END_OF_GUILD_BUILDING
		return False

	def BUILD_OnMouseLeftButtonUp(self):
		if not self.wndGuildBuilding:
			return

		if not self.wndGuildBuilding.IsPreviewMode():
			return True

		return False

	def BULID_EnterGuildArea(self, areaID):
		# GUILD_BUILDING
		mainCharacterName = player.GetMainCharacterName()
		masterName = guild.GetGuildMasterName()

		if mainCharacterName != masterName:
			return

		if areaID != player.GetGuildID():
			return
		# END_OF_GUILD_BUILDING

		self.wndGameButton.ShowBuildButton()

	def BULID_ExitGuildArea(self, areaID):
		self.wndGameButton.HideBuildButton()

	#####################################################################################

	def IsEditLineFocus(self):
		if self.ChatWindow.chatLine.IsFocus():
			return 1

		if self.ChatWindow.chatToLine.IsFocus():
			return 1

		return 0

	def EmptyFunction(self):
		pass
		
	def GetInventoryPageIndex(self):
		if self.wndInventory:
			return self.wndInventory.GetInventoryPageIndex()
		else:
			return -1

	if app.WJ_ENABLE_TRADABLE_ICON:
		def AttachInvenItemToOtherWindowSlot(self, slotIndex):
			return False

		def MarkUnusableInvenSlotOnTopWnd(self, onTopWnd, InvenSlot):
			if onTopWnd == player.ON_TOP_WND_SHOP and self.dlgShop and self.dlgShop.CantSellInvenItem(InvenSlot):
				return True
			elif onTopWnd == player.ON_TOP_WND_SAFEBOX and self.wndSafebox and self.wndSafebox.CantCheckInItem(InvenSlot):
				return True
			elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP and self.privateShopBuilder and self.privateShopBuilder.CantTradableItem(InvenSlot):
				return True
			elif onTopWnd == player.ON_TOP_WND_EXCHANGE and self.dlgExchange and self.dlgExchange.CantTradableItem(InvenSlot):
				return True

			if app.ENABLE_GROWTH_PET_SYSTEM:
				if onTopWnd == player.ON_TOP_WND_PET_FEED and self.wndPetInfoWindow and self.wndPetInfoWindow.CantFeedItem(InvenSlot):
					return True

				if onTopWnd == player.ON_TOP_WND_PET_ATTR_CHANGE and self.wndPetInfoWindow and self.wndPetInfoWindow.CantAttrChangeItem(InvenSlot):
					return True

				if onTopWnd == player.ON_TOP_WND_PET_PRIMIUM_FEEDSTUFF and self.wndPetInfoWindow and self.wndPetInfoWindow.CantPremiumFeedItem(InvenSlot):
					return True

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				if onTopWnd == player.ON_TOP_WND_COMBINE and self.wndAcceCombine and self.wndAcceCombine.CantCombineItem(InvenSlot):
					return True

				if onTopWnd == player.ON_TOP_WND_ABSORB_PHASE1 and self.wndAcceAbsorption and self.wndAcceAbsorption.CantAbsorbPhaseOne(InvenSlot):
					return True

				if onTopWnd == player.ON_TOP_WND_ABSORB_PHASE2 and self.wndAcceAbsorption and not self.wndAcceAbsorption.CantAbsorbPhaseTwo(InvenSlot):
					return True

			# if constInfo.ENABLE_CUBE_MARK_MATERIAL:
			# 	if onTopWnd == player.ON_TOP_WND_CUBE and self.wndCube and self.wndCube.IsMaterial(InvenSlot):
			# 		return True

			return False

		def SetOnTopWindow(self, onTopWnd):
			self.OnTopWindow = onTopWnd

		def GetOnTopWindow(self):
			return self.OnTopWindow

		def RefreshMarkInventoryBag(self):
			# if self.wndInventory and self.wndInventory.IsShow():
			if app.ENABLE_SPECIAL_INVENTORY:
				if self.wndInventory.GetInventoryType() == player.INVENTORY_TYPE_INVENTORY:
					self.wndInventory.RefreshBagSlotWindow()
				else:
					self.wndInventory.RefreshSpecialInventory()
			else:
				self.wndInventory.RefreshBagSlotWindow()

	if app.ENABLE_RENDER_TARGET_PREVIEW:
		def OpenRenderTarget(self, vnum):
			self.tooltipItem.ModelPreviewFull(vnum)

	if app.ENABLE_LOADING_PERFORMANCE:
		def OpenWarpShowerWindow(self):
			constInfo.SavePMInfo = False
			if self.wndMiniMap and self.wndMiniMap.IsShowingAtlas():
				self.wndMiniMap.ToggleAtlasWindow()

			if self.dlgSystem:
				self.dlgSystem.Close()
				self.dlgSystem.Destroy()

			self.HideAllQuestButton()
			self.HideAllWhisperButton()

			self.HideAllWindows()

		def CloseWarpShowerWindow(self):
			constInfo.SavePMInfo = True

	if app.ENABLE_CHEST_DROP_INFO:
		def OpenChestDropWindow(self, itemVnum):
			if self.wndChestDropInfo:
				self.wndChestDropInfo.Open(itemVnum)

	if app.ENABLE_SWITCHBOT_SYSTEM:
		def ToggleSwitchbotWindow(self):
			if True == self.wndSwitchbot.IsShow():
				self.wndSwitchbot.Close()
			else:
				self.wndSwitchbot.Open()

		def RefreshSwitchbotWindow(self):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotWindow()

		def RefreshSwitchbotItem(self, slot):
			if self.wndSwitchbot and self.wndSwitchbot.IsShow():
				self.wndSwitchbot.RefreshSwitchbotItem(slot)

	if app.ENABLE_INGAME_WIKI:
		def OpenWikiWindow(self):
			if not self.wndWiki.RequestOpen():
				self.wndWiki.Close()

	if app.ENABLE_GEM_SYSTEM:
		def ToggleExpandedMoneyButton(self):
			if False == self.wndExpandedMoneyTaskBar.IsShow():
				self.wndExpandedMoneyTaskBar.Show()
				self.wndExpandedMoneyTaskBar.SetTop()
			else:
				self.wndExpandedMoneyTaskBar.Close()	

		def ShowExpandedMoney(self):
			self.wndExpandedMoneyTaskBar.Show()		

		def OpenGemShop(self):
			self.wndGemShop.Open()	

		def CloseGemShop(self):
			self.wndGemShop.Close()
				
		def BINARY_OpenSelectItemExWindow(self):
			self.wndItemSelectEx.Open()

	if app.ENABLE_EVENT_MANAGER:
		def ToggleInGameEvent(self):
			if False == player.IsObserverMode():
				if False == self.wndEventOverview.IsShow():
					self.wndEventOverview.Open()
				else:
					self.wndEventOverview.Close()
					
		def OpenInGameEvent(self):
			if self.wndEventOverview:
				if not self.wndEventOverview.IsShow():
					self.wndEventOverview.Open()
					
		def CloseInGameEvent(self):
			if self.wndEventOverview:
				if self.wndEventOverview.IsShow():
					self.wndEventOverview.Close()

		def KingdomWarScore(self, score_shinsoo, score_chunjo, score_jinno):
			if self.wndEventOverview:
				self.wndEventOverview.KingdomWarScore(score_shinsoo, score_chunjo, score_jinno)

	if app.ENABLE_BIOLOG_SYSTEM:
		def __ToggleBiolog(self):
			biologmgr.SendPacket(net.BIOLOG_MANAGER_OPEN)

		def ToggleBiologManager(self):
			if self.wndBiologManager["PANEL"].IsShow():
				self.wndBiologManager["PANEL"].Close()
			else:
				self.wndBiologManager["PANEL"].Open()

		def BiologManagerUpdate(self):
			self.wndBiologManager["PANEL"].RecvUpdate()

		def BiologManager_Alert(self):
			self.wndBiologManager["ALERT"].RecvUpdate(5)

		def BiologManagerClose(self):
			if self.wndBiologManager["PANEL"].IsShow():
				self.wndBiologManager["PANEL"].Close()

	if app.ENABLE_SKILL_GROUP_GUI:
		def OpenSkillGroup(self, job):
			self.wndSkillGroup.Open(job)

	if app.__AUTO_SKILL_READER__:
		def OpenAutoSkillReader(self):
			exorcism_count = player.GetItemCountByVnum(71001)
			concentrated_count = player.GetItemCountByVnum(76034)
			if exorcism_count <= 0 or concentrated_count <= 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "You need at least 1 of each Concentraded Reading & Exorcism Scroll.")
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Du brauchst eine Exo & Konzi um es zu ffnen.")
				return

			if self.wndAutoSkillReader == None:
				self.wndAutoSkillReader = uiAutoSkillReader.Window()
			if self.wndAutoSkillReader.IsShow():
				self.wndAutoSkillReader.Close()
			else:
				self.wndAutoSkillReader.Open()

		def AutoSkillStatus(self, status):
			if self.wndAutoSkillReader:
				self.wndAutoSkillReader.ServerSetStatus(status)

	if app.ENABLE_PREMIUM_PRIVATE_SHOP:
		def OpenPrivateShopPanel(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.Open()
				
			if not self.wndInventory.IsShow():
				self.wndInventory.Show()
				
		def ClosePrivateShopPanel(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.Close(False)
				
		def RefreshPrivateShopWindow(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.Refresh()
				self.wndPrivateShopPanel.RefreshWindow()
				
		def TogglePrivateShopPanelWindow(self):
			if False == player.IsObserverMode():
				if not self.wndPrivateShopPanel.RequestOpen():
					self.wndPrivateShopPanel.Close()
					
		def OpenPrivateShopSearch(self, mode):
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.Open(mode)

		def OpenSearchShopCommand(self):
			if self.wndPrivateShopSearch.IsShow():
				self.wndPrivateShopSearch.Hide()
				return

			net.SendChatPacket("/open_search")

		def PrivateShopSearchUpdate(self, index, state):
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.UpdateResult(index, state)

		def PrivateShopSearchRefresh(self):
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch.RefreshPage()
				
		def AppendMarketItemPrice(self, gold, cheque):
			if self.wndPrivateShopPanel and self.wndPrivateShopPanel.IsShow():
				self.wndPrivateShopPanel.AppendMarketItemPrice(gold, cheque)
				
			elif self.self.privateShopBuilder and self.self.privateShopBuilder.IsShow():
				self.privateShopBuilder.AppendMarketItemPrice(gold, cheque)
				
		def AddPrivateShopTitleBoard(self, vid, text, type):

			board = uiPrivateShop.PrivateShopTitleBoard(type)
			board.Open(vid, text)

			self.privateShopAdvertisementBoardDict[vid] = board

		def RemovePrivateShopTitleBoard(self, vid):

			if not self.privateShopAdvertisementBoardDict.has_key(vid):
				return

			del self.privateShopAdvertisementBoardDict[vid]
			uiPrivateShop.DeleteTitleBoard(vid)
			
		def SetPrivateShopPremiumBuild(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.SetPremiumBuildMode()
				self.wndPrivateShopPanel.RefreshWindow()
				
		def PrivateShopStateUpdate(self):
			if self.wndPrivateShopPanel:
				self.wndPrivateShopPanel.OnStateUpdate()

	if app.ENABLE_ITEM_MANAGER:
		def ShowItemManager(self):
			self.wndItemManager.Open()

	if app.ENABLE_REMOTE_SHOP:
		def OpenRemoteShop(self):
			if self.wndRemoteShop:
				if self.wndRemoteShop.IsShowWindow():				
					self.wndRemoteShop.Close()
				else:
					self.wndRemoteShop.Show()

	if app.ENABLE_REFINE_UI_RENEWAL:
		def CheckRefineDialog(self, isFail):
			self.dlgRefineNew.CheckRefine(isFail)

	if app.ENABLE_PICK_FILTER:
		def OpenPickUpWindow(self):
			if self.wndPickUpFilter:
				self.wndPickUpFilter.Open()

	if app.ENABLE_SAVE_BLOCK_ATTR:
		def CheckBlockAttrStatus(self):
			now = app.GetGlobalTimeStamp()
			if self.blockAttrBlockTime > now:
				return
			self.blockAttrBlockTime = now + 1

			button = self.wndGameButton.gameButtonDict["ATTR_BLOCK"] if self.wndGameButton.gameButtonDict.has_key("ATTR_BLOCK") else None
			if button:
				if button.IsShow():
					if not player.IsInBlockArea():
						button.Hide()
				else:
					if player.IsInBlockArea():
						button.Show()

	if app.ENABLE_SKILL_COOLTIME_UPDATE:
		def SkillClearCoolTime(self, slotIndex):
			self.wndCharacter.SkillClearCoolTime(slotIndex)
			self.wndTaskBar.SkillClearCoolTime(slotIndex)

	if app.ENABLE_MAINTENANCE_SYSTEM:
		def OpenMaintenance(self, remaining_time, cause, duration):
			if not self.wndMaintenance:
				self.wndMaintenance = uiMaintenance.MaintenanceWindow()

			self.wndMaintenance.Open(remaining_time, cause, duration)

	if app.ENABLE_ANTI_EXP:
		def __ClickAntiExp(self):
			net.SendChatPacket("/anti_exp")

		def RefreshAntiExpButton(self, status):
			# if self.wndTaskBar:
			# 	self.wndTaskBar.RefreshAntiExpButton(status)
			if self.wndExpandedTaskBar:
				self.wndExpandedTaskBar.RefreshAntiExpButton(status)

	if app.ENABLE_HUNTING_SYSTEM:
		def ToggleHuntingWindow(self):
			if self.wndHunting.IsShow():
				self.wndHunting.Close()
			elif self.wndHuntingSelect.IsShow():
				self.wndHuntingSelect.Close()
			else:
				net.SendHuntingAction(1, 0)
				
		def OpenHuntingWindowMain(self, level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
			if self.wndHunting:
				self.wndHunting.OpenMain(level, monster, cur_count, dest_count, money_min, money_max, exp_min, exp_max, race_item, race_item_count)
				self.wndHunting.SetTop()
				constInfo.HUNTING_BUTTON_FLASH = 0
				
		def OpenHuntingWindowSelect(self, level, type ,monster, count, money_min, money_max, exp_min, exp_max, race_item, race_item_count):
			if self.wndHunting and self.wndHunting.IsShow():
				self.wndHunting.CloseWithMini()
			self.wndHuntingSelect.OpenSelect(level, type ,monster, count, money_min, money_max, exp_min, exp_max, race_item, race_item_count)
			constInfo.HUNTING_BUTTON_FLASH = 0
			
		def OpenHuntingWindowReward(self, level, reward, reward_count, random_reward, random_reward_count, money, exp):
			if False == self.wndHuntingReward.IsShow():
				self.wndHuntingReward.OpenReward(level, reward, reward_count, random_reward, random_reward_count, money, exp)
				self.wndHuntingReward.SetTop()
				
		def UpdateHuntingMission(self, count):
			if self.wndHunting:
				self.wndHunting.UpdateMission(count)
		
		def HuntingSetRandomItemsMain(self, item_vnum, item_count):
			if self.wndHunting:
				self.wndHunting.SetRandomItemTable(item_vnum, item_count)
				
		def HuntingSetRandomItemsSelect(self, item_vnum, item_count):
			if self.wndHuntingSelect:
				self.wndHuntingSelect.SetRandomItemTable(item_vnum, item_count)

	if app.BL_67_ATTR:
		def OpenAttr67AddDlg(self):
			if self.wndAttr67Add:
				self.wndAttr67Add.Show()

		def IsShowAttr67(self):
			if self.wndAttr67Add:
				return self.wndAttr67Add.IsShow()
			return False

		if app.WJ_ENABLE_TRADABLE_ICON:
			def IsAttr67RegistItem(self):
				return self.wndAttr67Add and self.wndAttr67Add.RegistSlotIndex != -1

			def IsAttr67SupportItem(self):
				return self.wndAttr67Add and self.wndAttr67Add.SupportSlotIndex != -1

	if app.__SPIN_WHEEL__:
		def SetSpinReward(self, selectedItemIdx, selectedItemCount, cmdData):
			if self.wndSpinWheel:
				self.wndSpinWheel.SetSpinReward(selectedItemIdx, selectedItemCount, cmdData)

		def SetSpinWheel(self, count, show):
			if self.wndSpinWheel == None:
				self.wndSpinWheel = uiSpinWheel.Window()

			self.wndSpinWheel.SetSpinData(int(count))
			if int(show):
				self.wndSpinWheel.Open()
			else:
				self.wndSpinWheel.Close()

	if app.__SPIN_WHEEL_BOSS__:
		def SetSpinRewardBoss(self, selectedItemIdx, selectedItemCount, cmdData):
			if self.wndSpinWheelBoss:
				self.wndSpinWheelBoss.SetSpinRewardBoss(selectedItemIdx, selectedItemCount, cmdData)

		def SetSpinWheelBoss(self, count, show):
			if self.wndSpinWheelBoss == None:
				self.wndSpinWheelBoss = uiSpinWheelBoss.Window()

			self.wndSpinWheelBoss.SetSpinData(int(count))
			if int(show):
				self.wndSpinWheelBoss.Open()
			else:
				self.wndSpinWheelBoss.Close()

	if app.ENABLE_DUNGEON_INFO_SYSTEM:
		def ToggleDungeonInfoWindow(self):
			if False == player.IsObserverMode():
				if False == self.wndDungeonInfo.IsShow():
					self.wndDungeonInfo.Open()
				else:
					self.wndDungeonInfo.Close()

		def DungeonInfoOpen(self):
			if self.wndDungeonInfo:
				self.wndDungeonInfo.OnOpen()

		def DungeonRankingRefresh(self):
			if self.wndDungeonInfo:
				self.wndDungeonInfo.OnRefreshRanking()

		def DungeonInfoReload(self, onReset):
			if self.wndDungeonInfo:
				self.wndDungeonInfo.OnReload(onReset)

	if app.ENABLE_GLOBAL_REWARD:
		def RewardData(self, isGlobal, isNeedClean, commandText):
			if self.wndReward:
				self.wndReward.RewardData(int(isGlobal), int(isNeedClean), commandText)

		def OpenRewardWindow(self):
			if self.wndReward == None:
				self.wndReward = uiReward.RewardWindow()
			if self.wndReward.IsShow():
				self.wndReward.Close()
			else:
				self.wndReward.Open()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			self.wndInventory.RefreshVisibleCostume()

	if constInfo.ENABLE_SKYBOX_WINDOW:
		def ToggleSkyBoxWindow(self):
			if self.wndSkyBox.IsShow():
				self.wndSkyBox.Close()
			else:
				self.wndSkyBox.Open()

	if app.ENABLE_ANTI_MULTIPLE_FARM:
		def ToggleAntiMultipleFarmWindow(self):
			if not self.wndAntiMultipleFarm:
				return
			
			if anti_multiple_farm.GetAntiFarmPlayerCount() <= anti_multiple_farm.MULTIPLE_FARM_MAX_ACCOUNT:
				try:
					self.wndPopupDialog.SetText("You need to have at least {} accounts connected!.".format(anti_multiple_farm.MULTIPLE_FARM_MAX_ACCOUNT+1))
					self.wndPopupDialog.Open()
				except:
					self.wndPopupDialog = uiCommon.PopupDialog()
					self.wndPopupDialog.SetText("You need to have at least {} accounts connected!".format(anti_multiple_farm.MULTIPLE_FARM_MAX_ACCOUNT+1))
					self.wndPopupDialog.Open()
				return
			
			isShow = self.wndAntiMultipleFarm.IsShow()
			self.wndAntiMultipleFarm.Close() if isShow else self.wndAntiMultipleFarm.Open()
		
		def SendAntiFarmReload(self):
			if self.wndTaskBar: #Refresh main character state
				self.wndTaskBar.ReloadAntiMultipleFarmState()
			
			if self.wndAntiMultipleFarm.IsShow(): #D? refresh ? pagina do sistema caso esteja aberta
				self.wndAntiMultipleFarm.OnRefreshData()

				#Caso esteja aberta e o jogador esteja a editar a mesma, sa? do modo de edi?o e avisa o porqu
				if self.wndAntiMultipleFarm.page_manage_mode != 0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ANTI_MULTIPLE_FARM_REFRESHED)
		
		def RegistItemGive(self, itemVnum, itemCount):
			if not self.wndGiveItem:
				return
			
			self.wndGiveItem.Open(itemVnum, itemCount)

	if app.ENABLE_VOTE_4_BUFF:
		# def CheckVoteForBuff(self):
		# 	now = app.GetGlobalTimeStamp()
		# 	if self.vote4BuffTime > now:
		# 		return
			
		# 	if not self.wndVote4Buff:
		# 		self.CreateVote4Buff()
			
		# 	self.vote4BuffTime = now + 1

		# 	button = self.wndGameButton.gameButtonDict["VOTE4BUFF"] if self.wndGameButton.gameButtonDict.has_key("VOTE4BUFF") else None
		# 	if button:
		# 		time = self.wndVote4Buff.GetVote4Bonus()
		# 		if time > now:
		# 			button.Hide()
		# 		else:
		# 			button.Show()

		# def MakeVote4Buff(self):
		# 	if not self.wndVote4Buff:
		# 		self.wndVote4Buff = uiVote4Buff.Vote4Buff()
		# 		self.wndVote4Buff.Show()
		# 	else:
		# 		self.OpenVote4Buff()

		# def CreateVote4Buff(self):
		# 	if not self.wndVote4Buff:
		# 		self.wndVote4Buff = uiVote4Buff.Vote4Buff()

		# def OpenVote4Buff(self):
		# 	if self.wndVote4Buff.IsShow():
		# 		self.wndVote4Buff.Close()
		# 	else:
		# 		self.wndVote4Buff.Open()
		def OpenVoteWindow(self):
			if self.wndVote4Buff.IsShow():
				self.wndVote4Buff.Close()
			else:
				self.wndVote4Buff.Show()

	if app.ENABLE_RANK_PLAYER:
		def ShowRankGlobal(self):
			self.wndRankInfo.Show()

		def AppendInfoRankGlobal(self, mode, my_pos, pos, name, value, empire):
			self.wndRankInfo.AppendInfo(mode, my_pos, pos, name, value, empire)

	if app.ENABLE_PLAYER_RANKING:
		def ShowRankGlobal(self):
			if False == player.IsObserverMode():
				if not self.wndRankInfo.IsShow():
					self.wndRankInfo.Show()
				else:
					self.wndRankInfo.Close()			
					
		def AppendInfoRankGlobal(self, my_pos, mode, pos, name, value, empire):
			self.wndRankInfo.AppendInfo(my_pos, mode, pos, name, value, empire)

	if app.ENABLE_BATTLE_PASS:
		def OpenBattlePass(self):
			if False == player.IsObserverMode():
				if not self.wndBattlePass.IsShow():
					self.wndBattlePass.Open()
					# self.wndBattlePassButton.CompleteLoading()
				else:
					self.wndBattlePass.Close()

		def AddBattlePassMission(self, missionType, missionInfo1, missionInfo2, missionInfo3):
			if self.wndBattlePass:
				self.wndBattlePass.AddMission(missionType, missionInfo1, missionInfo2, missionInfo3)
				
		def UpdateBattlePassMission(self, missionType, newProgress):
			if self.wndBattlePass:
				self.wndBattlePass.UpdateMission(missionType, newProgress)
				
		def AddBattlePassMissionReward(self, missionType, itemVnum, itemCount):
			if self.wndBattlePass:
				self.wndBattlePass.AddMissionReward(missionType, itemVnum, itemCount)
				
		def AddBattlePassReward(self, itemVnum, itemCount):
			if self.wndBattlePass:
				self.wndBattlePass.AddReward(itemVnum, itemCount)
				
		def AddBattlePassRanking(self, pos, playerName, finishTime):
			if self.wndBattlePass:
				self.wndBattlePass.AddRanking(pos, playerName, finishTime)
				
		def RefreshBattlePassRanking(self):
			if self.wndBattlePass:
				self.wndBattlePass.RefreshRanking()
				
		def OpenBattlePassRanking(self):
			if self.wndBattlePass:
				self.wndBattlePass.OpenRanking()

		def RequestOpenBattlePass(self):
			if self.wndBattlePass:
				if self.wndBattlePass.IsShow():
					self.wndBattlePass.Close()
					return

			net.SendBattlePassAction(1)

	if app.ENABLE_DAILY_BOSS:
		def SetWorldBossText(self, index, text, color):
			self.wndWorldBoss.SetWorldBossText(int(index), str(text), int(color))

		def SetBossesText(self, index, color):
			self.wndWorldBoss.SetBossesText(int(index), int(color))
		
		def SetInformationsText(self, index, text):
			self.wndWorldBoss.SetInformationsText(int(index), str(text))

		def SetInformationsTextT(self, index, text):
			self.wndTanakaEvent.SetInformationsText(int(index), str(text))

		def ShowWorldBoss(self):
			self.wndWorldBoss.Show()

	if app.ENABLE_ITEMSHOP:
		def ToggleItemshop(self):
			if self.wndItemshop.IsShow():
				self.wndItemshop.Close()
			else:
				self.wndItemshop.Open()

		def OpenItemshop(self):
			if not self.wndItemshop.IsShow():
				self.wndItemshop.Open()

		def ReloadItemshop(self):
			if self.wndItemshop:
				self.wndItemshop.ReloadItemshop()

		def ReloadItemshopItem(self, item):
			if self.wndItemshop:
				self.wndItemshop.ReloadItemshopItem(item)
				self.wndItemshop.ReloadItemshopContainer(item)

		def RemoveItemshopItem(self, hash):
			if self.wndItemshop:
				self.wndItemshop.RemoveItemshopItem(hash)
				self.wndItemshop.RemoveItemFromItemshopContainer(hash)

		def AddItemshopItem(self, obj):
			if self.wndItemshop:
				self.wndItemshop.AddItemshopItem(obj)

		def SetCoins(self, coins):
			if self.wndItemshop:
				self.wndItemshop.SetCoins(coins)

		def StartPromotionAnimation(self, answer):
			if self.wndItemshop:
				self.wndItemshop.StartPromotionAnimation(answer)

	if app.__BL_MULTI_LANGUAGE__:
		def LanguageChange(self):
			if self.dlgSystem:
				self.dlgSystem.LanguageChange()

	if app.__BL_MULTI_LANGUAGE_PREMIUM__:
		def __MakeFlagTooltip(self, arg):
			if not arg:
				return

			pos_x, pos_y = wndMgr.GetMousePosition()
			self.countryTooltip.ClearToolTip()
			self.countryTooltip.SetThinBoardSize(11 * len(arg))
			self.countryTooltip.SetToolTipPosition(pos_x, pos_y)
			self.countryTooltip.AppendTextLine(arg, 0xffffff00)
			self.countryTooltip.ShowToolTip()

		def MakeCountryTooltip(self, arg):
			self.__MakeFlagTooltip(uiScriptLocale.LOCALE_NAME_DICT.get(arg, ""))

		def MakeEmpireTooltip(self, arg):
			self.__MakeFlagTooltip(self.EMPIRE_NAME.get(arg, ""))

	if app.__BL_MULTI_LANGUAGE_ULTIMATE__:
		def LanguageChangeAnonymous(self):
			if self.dlgSystem:
				self.dlgSystem.LanguageChangeAnonymous()

	if app.ENABLE_GUILD_SAFEBOX:
		def RefreshGuildSafebox(self):
			self.wndGuildSafebox.RefreshGuildSafebox()

		def OpenGuildSafeboxWindow(self, size):
			self.wndGuildSafebox.ShowWindow(size)

		def RefreshGuildSafeboxMoney(self):
			self.wndGuildSafebox.RefreshGuildSafeboxMoney()

		def CommandCloseGuildSafebox(self):
			self.wndGuildSafebox.CommandCloseGuildSafebox()

	if app.ENABLE_EVENT_CALENDAR:
		def MakeEventIcon(self):
			if self.wndEventIcon == None:
				self.wndEventIcon = uiEventCalendar.MovableImage()
				self.wndEventIcon.Show()

		def MakeEventCalendar(self):
			if self.wndEventManager == None:
				self.wndEventManager = uiEventCalendar.EventCalendarWindow()

		def OpenEventCalendar(self):
			self.MakeEventCalendar()
			if self.wndEventManager.IsShow():
				self.wndEventManager.Close()
			else:
				self.wndEventManager.Open()

		def RefreshEventStatus(self, eventID, eventStatus, eventendTime, eventEndTimeText):
			if eventendTime != 0:
				eventendTime += app.GetGlobalTimeStamp()
			uiEventCalendar.SetEventStatus(eventID, eventStatus, eventendTime, eventEndTimeText)
			self.RefreshEventManager()

		def ClearEventManager(self):
			uiEventCalendar.server_event_data={}

		def RefreshEventManager(self):
			if self.wndEventManager:
				self.wndEventManager.Refresh()
			if self.wndEventIcon:
				self.wndEventIcon.Refresh()

		def AppendEvent(self, dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart):
			self.MakeEventCalendar()
			self.MakeEventIcon()
			#import dbg
			#dbg.TraceError("startTime: %d endTime: %d"%(startRealTime, endRealTime))
			if startRealTime != 0:
				startRealTime += app.GetGlobalTimeStamp()
			if endRealTime != 0:
				endRealTime += app.GetGlobalTimeStamp()
			uiEventCalendar.SetServerData(dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart)


if __name__ == "__main__":

	import app
	import wndMgr
	import systemSetting
	import mouseModule
	import grp
	import ui
	import localeInfo

	app.SetMouseHandler(mouseModule.mouseController)
	app.SetHairColorEnable(True)
	wndMgr.SetMouseHandler(mouseModule.mouseController)
	wndMgr.SetScreenSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	app.Create(localeInfo.APP_TITLE, systemSetting.GetWidth(), systemSetting.GetHeight(), 1)
	mouseModule.mouseController.Create()

	class TestGame(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)

			localeInfo.LoadLocaleData()
			player.SetItemData(0, 27001, 10)
			player.SetItemData(1, 27004, 10)

			self.interface = Interface()
			self.interface.MakeInterface()
			self.interface.ShowDefaultWindows()
			self.interface.RefreshInventory()
			#self.interface.OpenCubeWindow()

		def __del__(self):
			ui.Window.__del__(self)

		def OnUpdate(self):
			app.UpdateGame()

		def OnRender(self):
			app.RenderGame()
			grp.PopState()
			grp.SetInterfaceRenderState()

	game = TestGame()
	game.SetSize(systemSetting.GetWidth(), systemSetting.GetHeight())
	game.Show()

	app.Loop()
