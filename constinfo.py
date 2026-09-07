import app
import chrmgr
import item
import net
import player

charmeine_duel = 0

# EXTRA BEGIN
ENABLE_NEW_LEVELSKILL_SYSTEM = False # loads 5 (B,M,G,P,F) skills .mse
ENABLE_RANDOM_CHANNEL_SEL = False # don't set a random channel when you open the client
ENABLE_CLEAN_DATA_IF_FAIL_LOGIN = False # don't remove id and password if the login attempt fails
ENABLE_PASTE_FEATURE = True # ctrl+v will now work
ENABLE_FULLSTONE_DETAILS = True # display all the bonuses added by a stone instead of the first one
ENABLE_REFINE_PCT = False # enable successfulness % in the refine dialog
EXTRA_UI_FEATURE = True # enable extra ui features
NEW_678TH_SKILL_ENABLE = False # load 6th warrior / ninja skills (instead of app.NEW_678TH_SKILL_ENABLE)
SELECT_CHAR_NO_DELAY = 0.5 # change the 3sec delay while choosing a character in the select phase
ENABLE_ACTIVE_PET_SEAL_EFFECT = False # enable active effect on pet seals
# EXTRA END

EXTRA_INFO_COLOR = 0xFFCEBE94
ENABLE_ANIMATE_GUAGE = 1
ENABLE_SKYBOX_WINDOW = 1
ENABLE_COSTUME_PAGES = 1
ENABLE_ALWAYS_SHOW_EXPANDED_TASKBAR_BUTTONS = 1
INPUT_IGNORE = 0
PREMIUMMODE = [False, 0]
if app.ENABLE_VOTE_4_BUFF:
	VOTEBUFF = 0
	AFFECT_V4B = 0

if app.ENABLE_ITEMSHOP:
	ITEMSHOP_INFOS = []
	PROMOTION_CODE_REWARDS = []

if app.ENABLE_DAILY_BOSS:
	HAVEQF_WORLDBOSS = 0

if app.ENABLE_HIDE_COSTUME_SYSTEM:
	costumeHair = 1
	costumeCostume = 1
	if app.ENABLE_WEAPON_COSTUME_SYSTEM:
		costumeWeapon = 1
	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		costumeAcce = 1
	if app.ENABLE_AURA_COSTUME_SYSTEM:
		costumeAura = 1

if app.ENABLE_HUNTING_SYSTEM:
	HUNTING_MAIN_UI_SHOW = 0
	HUNTING_MINI_UI_SHOW = 0
	HUNTING_BUTTON_FLASH = 0
	HUNTING_BUTTON_IS_FLASH = 0

if app.PM_IN_GLOBAL_CHAT:
	PM_LINK = ''

if app.ENABLE_DROP_INFO:
	dropInfoDict = {}

if app.ENABLE_ITEM_MANAGER:
	ITEM_REMOVE_WINDOW_STATUS = 0
	ITEM_MANAGER_TYPE = 0

if app.ENABLE_REFINE_UI_RENEWAL:
	IS_AUTO_REFINE = False
	AUTO_REFINE_TYPE = 0
	AUTO_REFINE_DATA = {
		"ITEM" : [-1, -1],
		"NPC" : [0, -1, -1, 0]
	}

ENABLE_POTIONS_AFFECTSHOWER = 1

if app.ENABLE_GOLD_MAX_EXTENDED:
	GOLD_MAX = 999000000000000
else:
	GOLD_MAX = 2000000000
CHEQUE_MAX = 999
GEM_MAX = 1000000


# enable save account
ENABLE_SAVE_ACCOUNT = True
if ENABLE_SAVE_ACCOUNT:
	class SAB:
		ST_CACHE, ST_FILE, ST_REGISTRY = xrange(3)
		slotCount = 5
		storeType = ST_REGISTRY # 0 cache, 1 file, 2 registry
		btnName = {
			"Save": "SaveAccountButton_Save_%02d",
			"Access": "SaveAccountButton_Access_%02d",
			"Remove": "SaveAccountButton_Remove_%02d",
		}
		accData = {}
		regPath = r"SOFTWARE\Metin2"
		regName = "slot%02d_%s"
		regValueId = "id"
		regValuePwd = "pwd"
		regValuePin = "pin"
		fileExt = ".do.not.share.it.txt"
def CreateSABDataFolder(filePath):
	import os
	folderPath = os.path.split(filePath)[0]
	if not os.path.exists(folderPath):
		os.makedirs(folderPath)
def IsExistSABDataFile(filePath):
	import os
	return os.path.exists(filePath)
def GetSABDataFile(idx):
	import os
	filePath = "%s\\Metin2\\" % os.getenv('appdata')
	filePath += SAB.regName % (idx, SAB.regValueId)
	filePath += SAB.fileExt
	return filePath
def DelJsonSABData(idx):
	import os
	filePath = GetSABDataFile(idx)
	if IsExistSABDataFile(filePath):
		os.remove(filePath)
def GetJsonSABData(idx):
	(id, pwd, pin) = ("", "", "")
	filePath = GetSABDataFile(idx)
	if not IsExistSABDataFile(filePath):
		return (id, pwd, pin)
	with old_open(filePath) as data_file:
		try:
			import json
			(id, pwd, pin) = json.load(data_file)
			id = str(id) # unicode to ascii
			pwd = str(pwd) # unicode to ascii
			pin = str(pin) # unicode to ascii
		except ValueError:
			pass
	return (id, pwd, pin)
def SetJsonSABData(idx, slotData):
	filePath = GetSABDataFile(idx)
	CreateSABDataFolder(filePath)
	with old_open(filePath, "w") as data_file:
		import json
		json.dump(slotData, data_file)
def DelWinRegKeyValue(keyPath, keyName):
	try:
		import _winreg
		_winreg.CreateKey(_winreg.HKEY_CURRENT_USER, keyPath)
		_tmpKey = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, keyPath, 0, _winreg.KEY_WRITE)
		_winreg.DeleteValue(_tmpKey, keyName)
		_winreg.CloseKey(_tmpKey)
		return True
	except WindowsError:
		return False
def GetWinRegKeyValue(keyPath, keyName):
	try:
		import _winreg
		_tmpKey = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, keyPath, 0, _winreg.KEY_READ)
		keyValue, keyType = _winreg.QueryValueEx(_tmpKey, keyName)
		_winreg.CloseKey(_tmpKey)
		return str(keyValue) # unicode to ascii
	except WindowsError:
		return None
def SetWinRegKeyValue(keyPath, keyName, keyValue):
	try:
		import _winreg
		_winreg.CreateKey(_winreg.HKEY_CURRENT_USER, keyPath)
		_tmpKey = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, keyPath, 0, _winreg.KEY_WRITE)
		_winreg.SetValueEx(_tmpKey, keyName, 0, _winreg.REG_SZ, keyValue)
		_winreg.CloseKey(_tmpKey)
		return True
	except WindowsError:
		return False

# classic minmax def
def minmax(tmin, tmid, tmax):
	if tmid < tmin:
		return tmin
	elif tmid > tmax:
		return tmax
	return tmid
# EXTRA END

# TRADE_GOLD_WITH_THOUSANDS_SEPARATORS BEGIN
def intWithCommas(x, commasign='.'):
	# alternative of
	# return '{0:,}'.format(x).replace(',', commasign)
	if type(x) not in [type(0), type(0L)]:
		raise TypeError("Parameter must be an integer.")
	if x < 0:
		return '-' + intWithCommas(-x, commasign)
	result = ''
	while x >= 1000:
		x, r = divmod(x, 1000)
		result = "%s%03d%s" % (commasign, r, result)
	return "%d%s" % (x, result)
# TRADE_GOLD_WITH_THOUSANDS_SEPARATORS END

def Emoji(path):
	return "|E{}|e".format(path)

def Color(hexString):
	return "|cff{}|h".format(hexString)

def TextColor(text, hexString):
	return "|cff{}|h{}|r".format(hexString, text)

# option
IN_GAME_SHOP_ENABLE = 1
CONSOLE_ENABLE = 0
ENABLE_MALL_AFFECTS = 0
ENABLE_NEW_TIPBOAD = 1

PVPMODE_ENABLE = 1
PVPMODE_TEST_ENABLE = 0
PVPMODE_ACCELKEY_ENABLE = 1
PVPMODE_ACCELKEY_DELAY = 0.5
PVPMODE_PROTECTED_LEVEL = 15

FOG_LEVEL0 = 4800.0
FOG_LEVEL1 = 9600.0
FOG_LEVEL2 = 12800.0
FOG_LEVEL = FOG_LEVEL0
FOG_LEVEL_LIST=[FOG_LEVEL0, FOG_LEVEL1, FOG_LEVEL2]

CAMERA_MAX_DISTANCE_SHORT = 2500.0
CAMERA_MAX_DISTANCE_LONG = 3500.0
CAMERA_MAX_DISTANCE_LIST=[CAMERA_MAX_DISTANCE_SHORT, CAMERA_MAX_DISTANCE_LONG]
CAMERA_MAX_DISTANCE = CAMERA_MAX_DISTANCE_SHORT

CHRNAME_COLOR_INDEX = 0

ENVIRONMENT_NIGHT="d:/ymir work/environment/moonlight04.msenv"

# constant
if app.ENABLE_GOLD_MAX_EXTENDED:
	EXPENSIVE_SHIT = 50000000
HIGH_PRICE = 500000
MIDDLE_PRICE = 50000
ERROR_METIN_STONE = 28960
SUB2_LOADING_ENABLE = 1
EXPANDED_COMBO_ENABLE = 1
CONVERT_EMPIRE_LANGUAGE_ENABLE = 0
USE_ITEM_WEAPON_TABLE_ATTACK_BONUS = 0
ADD_DEF_BONUS_ENABLE = 0
LOGIN_COUNT_LIMIT_ENABLE = 0

USE_SKILL_EFFECT_UPGRADE_ENABLE = 1

VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = 1
GUILD_MONEY_PER_GSP = 100
GUILD_WAR_TYPE_SELECT_ENABLE = 1
TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE = 10

HAIR_COLOR_ENABLE = 1
ARMOR_SPECULAR_ENABLE = 1
WEAPON_SPECULAR_ENABLE = 1
SEQUENCE_PACKET_ENABLE = 1
KEEP_ACCOUNT_CONNETION_ENABLE = 1
MINIMAP_POSITIONINFO_ENABLE = 1

isItemQuestionDialog = 0

def GET_ITEM_QUESTION_DIALOG_STATUS():
	global isItemQuestionDialog
	return isItemQuestionDialog

def SET_ITEM_QUESTION_DIALOG_STATUS(flag):
	global isItemQuestionDialog
	isItemQuestionDialog = flag

########################

def SET_DEFAULT_FOG_LEVEL():
	global FOG_LEVEL
	app.SetMinFog(FOG_LEVEL)

def SET_FOG_LEVEL_INDEX(index):
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	try:
		FOG_LEVEL=FOG_LEVEL_LIST[index]
	except IndexError:
		FOG_LEVEL=FOG_LEVEL_LIST[0]
	app.SetMinFog(FOG_LEVEL)

def GET_FOG_LEVEL_INDEX():
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	return FOG_LEVEL_LIST.index(FOG_LEVEL)

########################

def SET_DEFAULT_CAMERA_MAX_DISTANCE():
	global CAMERA_MAX_DISTANCE
	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def SET_CAMERA_MAX_DISTANCE_INDEX(index):
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	try:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[index]
	except:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[0]

	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def GET_CAMERA_MAX_DISTANCE_INDEX():
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	return CAMERA_MAX_DISTANCE_LIST.index(CAMERA_MAX_DISTANCE)

########################

def SET_DEFAULT_CHRNAME_COLOR():
	global CHRNAME_COLOR_INDEX
	chrmgr.SetEmpireNameMode(CHRNAME_COLOR_INDEX)

def SET_CHRNAME_COLOR_INDEX(index):
	global CHRNAME_COLOR_INDEX
	CHRNAME_COLOR_INDEX=index
	chrmgr.SetEmpireNameMode(index)

def GET_CHRNAME_COLOR_INDEX():
	global CHRNAME_COLOR_INDEX
	return CHRNAME_COLOR_INDEX

def SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(index):
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = index

def GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	return VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD

def SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE():
	global CONVERT_EMPIRE_LANGUAGE_ENABLE
	net.SetEmpireLanguageMode(CONVERT_EMPIRE_LANGUAGE_ENABLE)

def SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS():
	global USE_ITEM_WEAPON_TABLE_ATTACK_BONUS
	player.SetWeaponAttackBonusFlag(USE_ITEM_WEAPON_TABLE_ATTACK_BONUS)

def SET_DEFAULT_USE_SKILL_EFFECT_ENABLE():
	global USE_SKILL_EFFECT_UPGRADE_ENABLE
	app.SetSkillEffectUpgradeEnable(USE_SKILL_EFFECT_UPGRADE_ENABLE)

def SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE():
	global TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE
	app.SetTwoHandedWeaponAttSpeedDecreaseValue(TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE)

########################

ACCESSORY_MATERIAL_LIST = [50623, 50624, 50625, 50626, 50627, 50628, 50629, 50630, 50631, 50632, 50633, 50634, 50635, 50636, 50637, 50638, 50639]
JewelAccessoryInfos = [
		# jewel		wrist	neck	ear
		[ 50634,	14420,	16220,	17220 ],
		[ 50635,	14500,	16500,	17500 ],
		[ 50636,	14520,	16520,	17520 ],
		[ 50637,	14540,	16540,	17540 ],
		[ 50638,	14560,	16560,	17560 ],
		[ 50639,	14570,	16570,	17570 ],
	]
def GET_ACCESSORY_MATERIAL_VNUM(vnum, subType):
	ret = vnum
	item_base = (vnum / 10) * 10
	for info in JewelAccessoryInfos:
		if item.ARMOR_WRIST == subType:
			if info[1] == item_base:
				return info[0]
		elif item.ARMOR_NECK == subType:
			if info[2] == item_base:
				return info[0]
		elif item.ARMOR_EAR == subType:
			if info[3] == item_base:
				return info[0]

	if vnum >= 16210 and vnum <= 16219:
		return 50625

	if item.ARMOR_WRIST == subType:
		WRIST_ITEM_VNUM_BASE = 14000
		ret -= WRIST_ITEM_VNUM_BASE
	elif item.ARMOR_NECK == subType:
		NECK_ITEM_VNUM_BASE = 16000
		ret -= NECK_ITEM_VNUM_BASE
	elif item.ARMOR_EAR == subType:
		EAR_ITEM_VNUM_BASE = 17000
		ret -= EAR_ITEM_VNUM_BASE

	type = ret/20

	if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
		type = (ret-170) / 20
		if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
			return 0

	return ACCESSORY_MATERIAL_LIST[type]

##################################################################

def GET_BELT_MATERIAL_VNUM(vnum, subType = 0):
	return 18900

##################################################################

def IS_AUTO_POTION(itemVnum):
	return IS_AUTO_POTION_HP(itemVnum) or IS_AUTO_POTION_SP(itemVnum)

def IS_AUTO_POTION_HP(itemVnum):
	if 72723 <= itemVnum and 72726 >= itemVnum:
		return 1
	elif itemVnum >= 76021 and itemVnum <= 76022:
		return 1
	elif itemVnum == 79012:
		return 1

	return 0

def IS_AUTO_POTION_SP(itemVnum):
	if 72727 <= itemVnum and 72730 >= itemVnum:
		return 1
	elif itemVnum >= 76004 and itemVnum <= 76005:
		return 1
	elif itemVnum == 79013:
		return 1

	return 0

def IS_PET_SEAL(itemVnum):
	item.SelectItem(itemVnum)
	itemType = item.GetItemType()
	itemSubType = item.GetItemSubType()

	if itemType == item.ITEM_TYPE_PET and itemSubType == item.PET_PAY:
		return 1

	return 0

if app.ENABLE_RENDER_TARGET_PREVIEW:
	def IsBossScroll(itemVnum):
		if itemVnum >= 71036 and itemVnum <= 71043:
			return 1
	
		return 0

	def IsETC(itemVnum):
		item.SelectItem(itemVnum)
		itemType = item.GetItemType()

		if itemVnum == 50011:
			return 1

		if itemType == item.ITEM_TYPE_PICK:
			return 1

		if itemType == item.ITEM_TYPE_ROD:
			return 1

		return 0

	def IsCanRender(itemVnum):
		item.SelectItem(itemVnum)
		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()

		IsPetItem = IS_PET_SEAL(itemVnum)
		IsBossScrollItem = IsBossScroll(itemVnum)
		IsEtcItems = IsETC(itemVnum)

		#if app.ENABLE_QUIVER_SYSTEM and (itemType == item.ITEM_TYPE_WEAPON and itemSubType == item.WEAPON_QUIVER):
		#	return False

		if  (itemType == item.ITEM_TYPE_WEAPON and not itemSubType == item.WEAPON_ARROW) or\
			(itemType == item.ITEM_TYPE_ARMOR and itemSubType == item.ARMOR_BODY) or\
			(itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_BODY) or\
			(itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_HAIR) or\
			(itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_WEAPON) or\
			(itemType == item.ITEM_TYPE_COSTUME and itemSubType == item.COSTUME_TYPE_ACCE) or\
			(itemType == item.ITEM_TYPE_MOUNT) or IsPetItem\
			or itemType == item.ITEM_TYPE_POLYMORPH or IsBossScrollItem or IsEtcItems or itemType == 43\
			or itemType == item.ITEM_TYPE_SHINING:
			return True

		races_m = []
		races_f = []
		if app.ENABLE_WOLFMAN_CHARACTER:
			MALES = [0, 5, 2, 7, 8]
		else:
			MALES = [0, 5, 2, 7]
		FEMALES = [4, 1, 6, 3]

		if not item.IsAntiFlag(item.ITEM_ANTIFLAG_WARRIOR):
			races_m.append(MALES[0])
			races_f.append(FEMALES[0])
		if not item.IsAntiFlag(item.ITEM_ANTIFLAG_ASSASSIN):
			races_m.append(MALES[1])
			races_f.append(FEMALES[1])
		if not item.IsAntiFlag(item.ITEM_ANTIFLAG_SURA):
			races_m.append(MALES[2])
			races_f.append(FEMALES[2])
		if not item.IsAntiFlag(item.ITEM_ANTIFLAG_SHAMAN):
			races_m.append(MALES[3])
			races_f.append(FEMALES[3])
		if app.ENABLE_WOLFMAN_CHARACTER:
			if not item.IsAntiFlag(item.ITEM_ANTIFLAG_WOLFMAN):
				races_m.append(MALES[4])
		
		if item.IsAntiFlag(item.ITEM_ANTIFLAG_FEMALE):
			races_f = []
		if item.IsAntiFlag(item.ITEM_ANTIFLAG_MALE):
			races_m = []

		if not player.GetRace() in races_m and not player.GetRace() in races_f:
			return False

		return False

def CALC_T_ARGLEN(t_arglen):
	return max(t_arglen*5 + 50, t_arglen*4 + 100, t_arglen*3 + 200)

def CALC_Q_ARGLEN(q_arglen):
	return max(q_arglen*5 + 75, q_arglen*4 + 150, q_arglen*3 + 300)

def CHECK_MAX_TEXT(text, text_2):
	if len(text) > len(text_2):
		return text

	if len(text_2) > len(text):
		return text_2

def CHECK_TEXT_LENGHT(text):
	text_length = len(text)

	if text.find('|cff') != -1:
		if text.count('|cff') > 1:
			text_length -= 14*text.count('|cff')
		else:
			text_length -= 14

	if text.find('|Eemoji/') != -1:
		if text.count('|Eemoji/') > 1:
			text_length -= 12*text.count('|Eemoji/')
		else:
			text_length -= 12

	return text_length

if app.ENABLE_INGAME_WIKI:
	HAIRSTYLE_CAMERA_CFG = {
		player.MAIN_RACE_WARRIOR_M	:	([86.0000, -241.0000, 151.0000], [-7.0000, 3.0000, 156.0000]),
		player.MAIN_RACE_ASSASSIN_W	:	([86.0000, -231.0000, 162.0000], [-7.0000, 28.0000, 155.0000]),
		player.MAIN_RACE_SURA_M		:	([86.0000, -242.0000, 162.0000], [-7.0000, 27.0000, 180.0000]),
		player.MAIN_RACE_SHAMAN_W	:	([86.0000, -224.0000, 160.0000], [-7.0000, 23.0000, 160.0000]),
		player.MAIN_RACE_WARRIOR_W	:	([86.0000, -228.0000, 162.0000], [-7.0000, 27.0000, 159.0000]),
		player.MAIN_RACE_ASSASSIN_M	:	([86.0000, -241.0000, 162.0000], [-7.0000, 21.0000, 167.0000]),
		player.MAIN_RACE_SURA_W		:	([86.0000, -204.0000, 162.0000], [-7.0000, 21.0000, 161.0000]),
		player.MAIN_RACE_SHAMAN_M	:	([86.0000, -230.0000, 182.0000], [-5.0000, 23.0000, 165.0000])
	}

	COSTUME_CAMERA_CFG = {
		player.MAIN_RACE_WARRIOR_M	:	([91.0000, -370.0000, 409.0000], [-5.0000, -1.0000, 114.0000]),
		player.MAIN_RACE_ASSASSIN_W	:	([91.0000, -370.0000, 385.0000], [1.0000, -1.0000, 120.0000]),
		player.MAIN_RACE_SURA_M		:	([91.0000, -370.0000, 453.0000], [5.0000, 0.0000, 134.0000]),
		player.MAIN_RACE_SHAMAN_W	:	([91.0000, -370.0000, 353.0000], [3.0000, 0.0000, 123.0000]),
		player.MAIN_RACE_WARRIOR_W	:	([91.0000, -370.0000, 393.0000], [-2.0000, 0.0000, 125.0000]),
		player.MAIN_RACE_ASSASSIN_M	:	([91.0000, -370.0000, 443.0000], [1.0000, -1.0000, 120.0000]),
		player.MAIN_RACE_SURA_W		:	([91.0000, -370.0000, 370.0000], [0.0000, 1.0000, 124.0000]),
		player.MAIN_RACE_SHAMAN_M	:	([91.0000, -370.0000, 423.0000], [3.0000, 2.0000, 125.0000])
	}

	WEAPON_SWORD_CAMERA_CFG = {
		player.MAIN_RACE_WARRIOR_M	:	([-550.0000, -100.0000, 240.0000], [-30.0000, -12.0000, 102.0000]),
		player.MAIN_RACE_ASSASSIN_W	:	([-510.0000, 109.0000, 0.0000], [100.0000, -19.0000, 78.0000]),
		player.MAIN_RACE_SURA_M		:	([-550.0000, 42.0000, 170.0000], [-10.0000, -2.0000, 100.0000]),
		player.MAIN_RACE_WARRIOR_W	:	([-400.0000, -257.0000, 0.0000], [0.0000, 4.0000, 80.0000]),
		player.MAIN_RACE_ASSASSIN_M	:	([-460.0000, -267.0000, 180.0000], [0.0000, 10.0000, 72.0000]),
		player.MAIN_RACE_SURA_W		:	([-373.0000, -273.0000, 80.0000], [0.0000, 1.0000, 80.0000])
	}

	WEAPON_DAGGER_CAMERA_CFG = {
		player.MAIN_RACE_ASSASSIN_W	:	([70.0000, 384.0000, 90.0000], [10.0000, -9.0000, 72.0000]),
		player.MAIN_RACE_ASSASSIN_M	:	([-130.0000, 353.0000, 100.0000], [0.0000, -3.0000, 70.0000]),
	}

	WEAPON_BOW_CAMERA_CFG = {
		player.MAIN_RACE_ASSASSIN_W	:	([586.0000, 79.0000, -30.0000], [-39.0000, -13.0000, 94.0000]),
		player.MAIN_RACE_ASSASSIN_M	:	([480.0000, -322.0000, 60.0000], [-39.0000, 15.0000, 92.0000]),
	}

	WEAPON_TWO_HAND_CAMERA_CFG = {
		player.MAIN_RACE_WARRIOR_M	:	([-510.0000, 500.0000, -134.0000], [-37.0000, 41.0000, 60.0000]),
		player.MAIN_RACE_WARRIOR_W	:	([-350.0000, 553.0000, 0.0000], [4.0000, 3.0000, 80.0000]),
	}

	WEAPON_BELL_CAMERA_CFG = {
		player.MAIN_RACE_SHAMAN_W	:	([75.0000, 514.0000, 83.0000], [-127.0000, -293.0000, 76.0000]),
		player.MAIN_RACE_SHAMAN_M	:	([-77.0000, 435.0000, 83.0000], [-44.0000, -325.0000, 131.0000])
	}

	WEAPON_FAN_CAMERA_CFG = {
		player.MAIN_RACE_SHAMAN_W	:	([105.0000, 484.0000, 83.0000], [-127.0000, -315.0000, 26.0000]),
		player.MAIN_RACE_SHAMAN_M	:	([-15.0000, 594.0000, 183.0000], [-66.0000, -485.0000, -55.0000])
	}

	PET_CAMERA_CFG = {
		34001	:	([190.0000, -510.0000, 40.0000], [0.0000, 0.0000, 194.0000]),		# 53001	Phoenix de Foc
		34003	:	([190.0000, -510.0000, 40.0000], [0.0000, 0.0000, 194.0000]),		# 53003	Phoenix de Gheata
		34004	:	([230.0000, -570.0000, 0.0000], [0.0000, 0.0000, 62.0000]),			# 53005	Baby Azrael
		34008	:	([230.0000, -430.0000, 0.0000], [0.0000, 0.0000, 33.0000]),			# 53010	Leonidas
		34007	:	([230.0000, -430.0000, 0.0000], [0.0000, 0.0000, 33.0000]),			# 53011	Khan
		34005	:	([230.0000, -430.0000, 0.0000], [0.0000, 0.0000, 33.0000]),			# 53012	Porcusor
		34006	:	([230.0000, -430.0000, 0.0000], [0.0000, 0.0000, 33.0000]),			# 53013	Rufus
		34016	:	([190.0000, -510.0000, 40.0000], [0.0000, 0.0000, 194.0000]),		# 53017	Phoenix de Jad
		34022	:	([230.0000, -430.0000, 0.0000], [0.0000, 0.0000, 38.0000]),			# 53018	(B) Phoenix de Jad
		34119	:	([230.0000, -520.0000, 0.0000], [0.0000, 10.0000, 42.0000]),		# 53023	Cooper
		34130	:	([230.0000, -430.0000, 0.0000], [0.0000, 0.0000, 33.0000]),			# 53285	Gnom de Craciun
		34135	:	([230.0000, -580.0000, 0.0000], [0.0000, 10.0000, 46.0000])			# 53296	Loverador Jucaus
	}

	MOUNT_CAMERA_CFG = {
		20204	:	([240.0000, -690.0000, 110.0000], [0.0000, -78.0000, 132.0000]),	# 52031	Leu Mic
		20113	:	([240.0000, -690.0000, 110.0000], [0.0000, -78.0000, 132.0000]),	# 52036	Leu Salbatic
		20212	:	([240.0000, -690.0000, 110.0000], [0.0000, -78.0000, 132.0000]),	# 52041	Leu Curajos
		20114	:	([240.0000, -690.0000, 110.0000], [0.0000, -78.0000, 132.0000]),	# 71124	Leul Alb
		20115	:	([240.0000, -690.0000, 110.0000], [0.0000, -48.0000, 82.0000]),		# 71125	Mistret Salbatic de Lupt
		20209	:	([240.0000, -690.0000, 110.0000], [0.0000, -48.0000, 82.0000]),		# 71126	Mistret Curajos
		20110	:	([240.0000, -690.0000, 110.0000], [0.0000, -48.0000, 82.0000]),		# 71127	Mistret Salbatic
		20201	:	([240.0000, -690.0000, 110.0000], [0.0000, -48.0000, 82.0000]),		# 71128	Mistret Mic
		20119	:	([240.0000, -690.0000, 110.0000], [0.0000, -55.0000, 152.0000]),	# 71131	(B) Cal Negru
		20257	:	([240.0000, -690.0000, 110.0000], [0.0000, -48.0000, 135.0000]),	# 71233	Bernie
		20266	:	([229.0000, -598.0000, 110.0000], [0.0000, -65.0000, 170.0000]),	# 71246	Armasar Hialin
		20273	:	([270.0000, -670.0000, 220.0000], [0.0000, -40.0000, 203.0000]),	# 71254	Lama Iarna Alba
		20277	:	([240.0000, -690.0000, 110.0000], [0.0000, -48.0000, 158.0000])		# 71260	Iepuras Grabit (Albastr)
	}

	BOSS_CHEST_CAMERA_CFG = {
		691		:	([100.0000, -530.0000, 120.0000], [0.0000, -15.0000, 208.0000]),		# 50070	Lada Orc Sef
		791		:	([170.0000, -300.0000, 236.0000], [0.0000, -7.0000, 222.0000]),			# 50071	Lada Conducator intuneca
		2091	:	([240.0000, -600.0000, 80.0000], [0.0000, -48.0000, 138.0000]),			# 50073	Lada Regina Paianjenilor
		2092	:	([470.0000, -1910.0000, 190.0000], [0.0000, -48.0000, 278.0000]),		# 50074	Lada Baroneasa Paianjen
		2191	:	([450.0000, -1100.0000, 110.0000], [0.0000, -228.0000, 108.0000]),		# 50076	Lada Testoasa Gigant
		1901	:	([220.0000, -510.0000, 110.0000], [0.0000, -18.0000, 188.0000]),		# 50077	Lada Noua Cozi
		1304	:	([240.0000, -680.0000, 110.0000], [0.0000, -88.0000, 218.0000]),		# 50078	Lada Fantoma Tigrului Ga
		2206	:	([120.0000, -420.0000, 110.0000], [0.0000, 17.0000, 243.0000]),			# 50079	Lada Regele Focului
		1091	:	([140.0000, -600.0000, 110.0000], [0.0000, -2.0000, 191.0000]),			# 50081	Lada Demon rege
		1093	:	([160.0000, -470.0000, 110.0000], [0.0000, -8.0000, 227.0000]),			# 50082	Lada Ingerul cu Coasa
		2598	:	([80.0000, -500.0000, 110.0000], [0.0000, 12.0000, 370.0000]),			# 50186	Lada Azrael
		191		:	([240.0000, -690.0000, 110.0000], [0.0000, -78.0000, 96.0000]),			# 80070	Lada Lykos
		192		:	([330.0000, -690.0000, 130.0000], [-20.0000, -48.0000, 68.0000]),		# 80071	Lada Scrofa
		193		:	([240.0000, -690.0000, 110.0000], [0.0000, -158.0000, 132.0000]),		# 80072	Lada Bera
		194		:	([240.0000, -690.0000, 110.0000], [0.0000, -68.0000, 68.0000]),			# 80073	Lada Tigris
		491		:	([240.0000, -690.0000, 110.0000], [0.0000, -8.0000, 208.0000]),			# 80074	Lada Mahon
		492		:	([150.0000, -420.0000, 110.0000], [0.0000, -8.0000, 193.0000]),			# 80075	Lada Bo
		493		:	([260.0000, -450.0000, 110.0000], [0.0000, -8.0000, 210.0000]),			# 80076	Lada Goo-Pae
		494		:	([40.0000, -310.0000, 110.0000], [0.0000, 52.0000, 188.0000]),			# 80077	Lada Chuong
		591		:	([110.0000, -310.0000, 110.0000], [0.0000, 2.0000, 205.0000]),			# 80078	Lada Capitanul Bestie
		5161	:	([240.0000, -690.0000, 110.0000], [0.0000, -108.0000, 128.0000]),		# 80079	Lada Maimuta de Piatra
		5162	:	([80.0000, -510.0000, 110.0000], [0.0000, -88.0000, 176.0000]),			# 80080	Lada Maimuta Umblatoare
		5163	:	([180.0000, -400.0000, 110.0000], [0.0000, 2.0000, 238.0000]),			# 80081	Lada Maimuta Lord
		2597	:	([150.0000, -460.0000, 110.0000], [0.0000, -18.0000, 308.0000]),		# 80082	Lada Charon
		1192	:	([120.0000, -150.0000, 140.0000], [0.0000, 4.0000, 166.0000]),			# 80083	Lada Vrajitoare Rea de G
		2492	:	([50.0000, -300.0000, 110.0000], [0.0000, -58.0000, 160.0000]),			# 80085	Lada Generalul Yonghan
		1092	:	([140.0000, -600.0000, 110.0000], [0.0000, -2.0000, 191.0000]),			# 80086	Lada Demon rege mandru
		2307	:	([370.0000, -1080.0000, 110.0000], [0.0000, -188.0000, 428.0000]),		# 80087	Lada Copac Fantoma Nobil
		2493	:	([490.0000, -1280.0000, 110.0000], [0.0000, -438.0000, 268.0000]),		# 54700	Lada Beran-Setaou
		693		:	([180.0000, -370.0000, 110.0000], [0.0000, -18.0000, 208.0000])			# 80088	Lada Orc Sef Spirit
	}

	def GetCharTypeHairCamera(char_type):
		if not HAIRSTYLE_CAMERA_CFG.has_key(char_type):
			return tuple([], [])

		return HAIRSTYLE_CAMERA_CFG[char_type]

	def GetCharTypeCostumeCamera(char_type):
		if not COSTUME_CAMERA_CFG.has_key(char_type):
			return tuple([], [])

		return COSTUME_CAMERA_CFG[char_type]

	def GetCharTypeWeaponCamera(char_type, subType):
		if subType == 0: # Sword
			if not WEAPON_SWORD_CAMERA_CFG.has_key(char_type):
				return tuple([], [])

			return WEAPON_SWORD_CAMERA_CFG[char_type]

		elif subType == 1: # Dagger
			if not WEAPON_DAGGER_CAMERA_CFG.has_key(char_type):
				return tuple([], [])

			return WEAPON_DAGGER_CAMERA_CFG[char_type]

		elif subType == 2: # Bow
			if not WEAPON_BOW_CAMERA_CFG.has_key(char_type):
				return tuple([], [])

			return WEAPON_BOW_CAMERA_CFG[char_type]

		elif subType == 3: # Two Hand
			if not WEAPON_TWO_HAND_CAMERA_CFG.has_key(char_type):
				return tuple([], [])

			return WEAPON_TWO_HAND_CAMERA_CFG[char_type]

		elif subType == 4: # Bell
			if not WEAPON_BELL_CAMERA_CFG.has_key(char_type):
				return tuple([], [])

			return WEAPON_BELL_CAMERA_CFG[char_type]

		elif subType == 5: # Fan
			if not WEAPON_FAN_CAMERA_CFG.has_key(char_type):
				return tuple([], [])

			return WEAPON_FAN_CAMERA_CFG[char_type]

	def GetCharTypePetCamera(char_type):
		if not PET_CAMERA_CFG.has_key(char_type):
			return tuple([], [])

		return PET_CAMERA_CFG[char_type]

	def GetCharTypeMountCamera(char_type):
		if not MOUNT_CAMERA_CFG.has_key(char_type):
			return tuple([], [])

		return MOUNT_CAMERA_CFG[char_type]

	def GetCharTypeBossChestCamera(char_type):
		if not BOSS_CHEST_CAMERA_CFG.has_key(char_type):
			return tuple([], [])

		return BOSS_CHEST_CAMERA_CFG[char_type]

if app.ENABLE_ITEM_MANAGER:
	def CanSell(itemVnum):
		item.SelectItem(itemVnum)
		if not item.IsAntiFlag(item.ANTIFLAG_SELL):
			return 1

		# other = {
			# 0: XXXXX,
			# 1: XXXXX,
		# }
		# if itemVnum in other.values():
			# return 1

		return 0

if app.ENABLE_EXTENDED_BLEND:
	import wndMgr
	def IS_PERMANANET_BLEND_ITEM(itemVnum):
		if 51821 <= itemVnum and 51826 >= itemVnum:
			return 1

		if 51813 <= itemVnum and 51820 >= itemVnum:
			return 1

		if 40017 <= itemVnum and 40025 >= itemVnum:
			return 1

		return 0

	def GetBlendColor(itemVnum):
		data = [
			[51821, wndMgr.COLOR_TYPE_RED],
			[51822, wndMgr.COLOR_TYPE_ORANGE],
			[51823, wndMgr.COLOR_TYPE_SKY],
			[51824, wndMgr.COLOR_TYPE_YELLOW],
			[51825, wndMgr.COLOR_TYPE_GREEN],
			[51826, wndMgr.COLOR_TYPE_WHITE],
		]

		for blendVnum, color in data:
			if itemVnum == blendVnum:
				return color
		return wndMgr.COLOR_TYPE_WHITE

_interface_instance = None
def GetInterfaceInstance():
	global _interface_instance
	return _interface_instance
def SetInterfaceInstance(instance):
	global _interface_instance
	if _interface_instance:
		del _interface_instance
	_interface_instance = instance

_game_instance = None
def GetGameInstance():
	global _game_instance
	return _game_instance
def SetGameInstance(instance):
	global _game_instance
	if _game_instance:
		del _game_instance

def IsTreasureBox(itemVnum):
	item.SelectItem(itemVnum)
	itemType = item.GetItemType()
	itemSubType = item.GetItemSubType()
	if itemType == item.ITEM_TYPE_GIFTBOX:
		return 1
	# if itemType == item.ITEM_TYPE_GACHA and itemSubType == item.USE_GACHA:
	# 	return 1

	# Special chests
	treasures = {
		0: 50012,
		1: 50013,
		2: 27987,
		3: 39067,
	}

	# IsApprenticeChest
	# if itemVnum >= 50187 and itemVnum <= 50197:
	# 	return 1
		
	if itemVnum in treasures.values():
		return 1

	return 0

def IsLimitedOpenCount(itemVnum):
	# item.SelectItem(itemVnum)
	# itemType = item.GetItemType()
	# itemSubType = item.GetItemSubType()
	# if itemType == item.ITEM_TYPE_XXXX:
	# 	return 1

	items = {
		0: 51501,
	}

	if itemVnum in items.values():
		return 1

	return 0

def TextColor(text, hexString):
	return "|cff{}|h{}|r".format(hexString, text)

if app.ENABLE_QUICK_OPEN:
	def IsQuickOpen(iVnum):
		exceptList = [27987, 39067]
		# item.SelectItem(iVnum)
		# if item.GetItemType() == item.ITEM_TYPE_GIFTBOX or iVnum in exceptList:
		# 	return True
		if iVnum in exceptList:
			return True

		return False
	
if app.ENABLE_NEW_TYPE_OF_POTION:
	def IS_NEW_SPEED_POTION(itemVnum):
		return IS_NEW_MOVE_SPEED_POTION(itemVnum) or IS_NEW_ATT_SPEED_POTION(itemVnum)

	def IS_NEW_MOVE_SPEED_POTION(itemVnum):
		if itemVnum == 27122:
			return 1

		return 0

	def IS_NEW_ATT_SPEED_POTION(itemVnum):
		if itemVnum == 27123:
			return 1

		return 0

if app.__WHISPER_FUTURES__:
	import os, app, chat
	def CheckDirectory(directory):
		try:
			os.makedirs(directory)
		except:
			pass
	def LoadWhispers(gamePointer, playerName):
		_directory = "lib/user_data/whisper_temp/{}".format(playerName)
		CheckDirectory(_directory)
		fileNameList=app.GetFileList(_directory+"/*")
		for name in fileNameList:
			try:
				file = open(_directory+"/"+name)
				lines = file.readlines()
				lineHas = False
				for line in lines:
					splitList = line.split("<!!!>")
					if len(splitList) != 2:
						continue
					chat.AppendWhisper(len(splitList[0]), name, splitList[1])
					lineHas = True
				if lineHas:
					gamePointer.interface.RecvWhisperSave(name)
				file.close()
			except:
				continue
		for name in fileNameList:
			try:
				file = open(_directory+"/"+name, "w+")
				file.close()
			except:
				continue

def CannotAct():
	if False == player.CanAttack():
		return True

	if False == player.CanMove():
		return True

	if False == player.CanAct():
		return True

	return False

import utilFile
CONFIG_INI = "metin2.ini"

def SetConfigIni(section = "", key = "", value = 0):
	utilFile.IniWrite(CONFIG_INI, section, key, value)

def ReadConfigIni(section="", key=""):
	value = utilFile.IniRead(CONFIG_INI, section, key)
	if value:
		return int(value)
	return 0

if ENABLE_SKYBOX_WINDOW:
	## 0 Default sky box
	## 1 DARK
	SKYBOX_PATH = {
		0 : "",
		1 : "d:/ymir work/environment/skybox5.msenv",
		2 : "d:/ymir work/environment/dark.msenv",
		3 : "d:/ymir work/environment/ridacksky19.msenv",
		4 : "d:/ymir work/environment/ridacksky23.msenv",
		5 : "d:/ymir work/environment/skybox_purple_nebula.msenv",
		6 : "d:/ymir work/environment/skybox_reco_blue_sky.msenv",
		7 : "d:/ymir work/environment/skybox_reco_lightblue.msenv",
		8 : "d:/ymir work/environment/skybox_reco_red_v1.msenv",
		9 : "d:/ymir work/environment/skybox_reco_red_v2.msenv",
		10 : "d:/ymir work/environment/m_world_boos_7.msenv",
		11 : "d:/ymir work/environment/sky_friendly_island.msenv",
		12 : "d:/ymir work/environment/desert.msenv",
	}

	def GetSelectedSkyBox():
		skyBoxIndexConfig = ReadConfigIni("skybox", "index")
		return skyBoxIndexConfig

if app.ENABLE_SHINING_ITEM_SYSTEM:
	def IsShiningItem(itemVnum):
		item.SelectItem(itemVnum)
		itemType = item.GetItemType()

		if itemType == item.ITEM_TYPE_SHINING:
			return 1

		return 0

if app.ENABLE_BOOSTER_ITEMS:
	def IsBoosterItem(itemVnum):
		item.SelectItem(itemVnum)
		itemType = item.GetItemType()

		if itemType == item.ITEM_TYPE_BOOSTER:
			return 1

		return 0

if app.ENABLE_ITEM_TIME_EXTENDER:
	ITEM_TIME_EXTENDER_VNUMS = [173010, 173011, 173012, 173013, 173014]
	def IS_ITEM_TIME_EXTENDER(itemVnum):
		for i in range(len(ITEM_TIME_EXTENDER_VNUMS)):
			if itemVnum == ITEM_TIME_EXTENDER_VNUMS[i]:
				return 1
		return 0
