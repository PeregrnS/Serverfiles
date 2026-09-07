import ui
import localeInfo
import chr
import item
import app
import skill
import player
import uiToolTip
import math
if app.ENABLE_PLAYER_AFFECT_REMOVE:
	import net
if app.ENABLE_PLAYER_AFFECT_REMOVE or app.RENEWAL_PICKUP_AFFECT:
	import uiCommon
import constInfo

AFF_AFK_TIME = 552

# WEDDING
class LovePointImage(ui.ExpandedImageBox):

	FILE_PATH = "d:/ymir work/ui/pattern/LovePoint/"
	FILE_DICT = {
		0 : FILE_PATH + "01.dds",
		1 : FILE_PATH + "02.dds",
		2 : FILE_PATH + "02.dds",
		3 : FILE_PATH + "03.dds",
		4 : FILE_PATH + "04.dds",
		5 : FILE_PATH + "05.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.loverName = ""
		self.lovePoint = 0

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def SetLoverInfo(self, name, lovePoint):
		self.loverName = name
		self.lovePoint = lovePoint
		self.__Refresh()

	def OnUpdateLovePoint(self, lovePoint):
		self.lovePoint = lovePoint
		self.__Refresh()

	def __Refresh(self):
		self.lovePoint = max(0, self.lovePoint)
		self.lovePoint = min(100, self.lovePoint)

		if 0 == self.lovePoint:
			loveGrade = 0
		else:
			loveGrade = self.lovePoint / 25 + 1
		fileName = self.FILE_DICT.get(loveGrade, self.FILE_PATH+"00.dds")

		try:
			self.LoadImage(fileName)
		except:
			import dbg
			dbg.TraceError("LovePointImage.SetLoverInfo(lovePoint=%d) - LoadError %s" % (self.lovePoint, fileName))

		self.SetScale(0.7, 0.7)

		self.toolTip.ClearToolTip()
		self.toolTip.SetTitle(self.loverName)
		self.toolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % (self.lovePoint))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.toolTip.HideToolTip()
# END_OF_WEDDING


class HorseImage(ui.ExpandedImageBox):

	FILE_PATH = "d:/ymir work/ui/pattern/HorseState/"

	FILE_DICT = {
		00 : FILE_PATH+"00.dds",
		01 : FILE_PATH+"00.dds",
		02 : FILE_PATH+"00.dds",
		03 : FILE_PATH+"00.dds",
		10 : FILE_PATH+"10.dds",
		11 : FILE_PATH+"11.dds",
		12 : FILE_PATH+"12.dds",
		13 : FILE_PATH+"13.dds",
		20 : FILE_PATH+"20.dds",
		21 : FILE_PATH+"21.dds",
		22 : FILE_PATH+"22.dds",
		23 : FILE_PATH+"23.dds",
		30 : FILE_PATH+"30.dds",
		31 : FILE_PATH+"31.dds",
		32 : FILE_PATH+"32.dds",
		33 : FILE_PATH+"33.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		#self.textLineList = []
		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def __GetHorseGrade(self, level):
		if 0 == level:
			return 0

		return (level-1)/10 + 1

	def SetState(self, level, health, battery):
		#self.textLineList=[]
		self.toolTip.ClearToolTip()

		if level>0:

			try:
				grade = self.__GetHorseGrade(level)
				self.__AppendText(localeInfo.LEVEL_LIST[grade])
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			try:
				healthName=localeInfo.HEALTH_LIST[health]
				if len(healthName)>0:
					self.__AppendText(healthName)
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			if health>0:
				if battery==0:
					self.__AppendText(localeInfo.NEEFD_REST)

			try:
				fileName=self.FILE_DICT[health*10+battery]
			except KeyError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - KeyError" % (level, health, battery)

			try:
				self.LoadImage(fileName)
			except:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - LoadError %s" % (level, health, battery, fileName)

		self.SetScale(0.7, 0.7)

	def __AppendText(self, text):

		self.toolTip.AppendTextLine(text)
		self.toolTip.ResizeToolTip()

		#x=self.GetWidth()/2
		#textLine = ui.TextLine()
		#textLine.SetParent(self)
		#textLine.SetSize(0, 0)
		#textLine.SetOutline()
		#textLine.Hide()
		#textLine.SetPosition(x, 40+len(self.textLineList)*16)
		#textLine.SetText(text)
		#self.textLineList.append(textLine)

	def OnMouseOverIn(self):
		#for textLine in self.textLineList:
		#	textLine.Show()

		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		#for textLine in self.textLineList:
		#	textLine.Hide()

		self.toolTip.HideToolTip()


# AUTO_POTION
class AutoPotionImage(ui.ExpandedImageBox):

	FILE_PATH_HP = "d:/ymir work/ui/pattern/auto_hpgauge/"
	FILE_PATH_SP = "d:/ymir work/ui/pattern/auto_spgauge/"

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.loverName = ""
		self.lovePoint = 0
		self.potionType = player.AUTO_POTION_TYPE_HP
		self.filePath = ""

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def SetPotionType(self, type):
		self.potionType = type

		if player.AUTO_POTION_TYPE_HP == type:
			self.filePath = self.FILE_PATH_HP
		elif player.AUTO_POTION_TYPE_SP == type:
			self.filePath = self.FILE_PATH_SP


	def OnUpdateAutoPotionImage(self):
		self.__Refresh()

	def __Refresh(self):
		print "__Refresh"

		isActivated, currentAmount, totalAmount, slotIndex = player.GetAutoPotionInfo(self.potionType)

		amountPercent = (float(currentAmount) / totalAmount) * 100.0
		grade = math.ceil(amountPercent / 20)

		if 5.0 > amountPercent:
			grade = 0

		if 80.0 < amountPercent:
			grade = 4
			if 90.0 < amountPercent:
				grade = 5

		fmt = self.filePath + "%.2d.dds"
		fileName = fmt % grade

		print self.potionType, amountPercent, fileName

		try:
			self.LoadImage(fileName)
		except:
			import dbg
			dbg.TraceError("AutoPotionImage.__Refresh(potionType=%d) - LoadError %s" % (self.potionType, fileName))

		self.SetScale(0.7, 0.7)

		self.toolTip.ClearToolTip()

		if player.AUTO_POTION_TYPE_HP == type:
			self.toolTip.SetTitle(localeInfo.TOOLTIP_AUTO_POTION_HP)
		else:
			self.toolTip.SetTitle(localeInfo.TOOLTIP_AUTO_POTION_SP)

		self.toolTip.AppendTextLine(localeInfo.TOOLTIP_AUTO_POTION_REST	% (amountPercent))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.toolTip.HideToolTip()
# END_OF_AUTO_POTION


class AffectImage(ui.ExpandedImageBox):

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.toolTipText = None
		self.isSkillAffect = True
		self.description = None
		self.endTime = 0
		self.affect = None
		self.isClocked = True
		if app.ENABLE_PLAYER_AFFECT_REMOVE:
			self.skillIndex = None

	def SetAffect(self, affect):
		self.affect = affect

	def GetAffect(self):
		return self.affect

	def SetToolTipText(self, text, x = 0, y = -19):

		if not self.toolTipText:
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetSize(0, 0)
			textLine.SetOutline()
			textLine.Hide()
			self.toolTipText = textLine

		self.toolTipText.SetText(text)
		w, h = self.toolTipText.GetTextSize()
		if localeInfo.IsARABIC():
			self.toolTipText.SetPosition(w+20, y)
		else:
			self.toolTipText.SetPosition(max(0, x + self.GetWidth()/2 - w/2), y)

	def SetDescription(self, description):
		self.description = description

	def SetDuration(self, duration):
		self.endTime = 0
		if duration > 0:
			self.endTime = app.GetGlobalTimeStamp() + duration

	def UpdateAutoPotionDescription(self):

		potionType = 0
		if self.affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
			potionType = player.AUTO_POTION_TYPE_HP
		else:
			potionType = player.AUTO_POTION_TYPE_SP

		isActivated, currentAmount, totalAmount, slotIndex = player.GetAutoPotionInfo(potionType)

		#print "UpdateAutoPotionDescription ", isActivated, currentAmount, totalAmount, slotIndex

		amountPercent = 0.0

		try:
			amountPercent = (float(currentAmount) / totalAmount) * 100.0
		except:
			amountPercent = 100.0

		self.SetToolTipText(self.description % amountPercent, 0, 40)

	def SetClock(self, isClocked):
		self.isClocked = isClocked

	def UpdateDescription(self):
		if not self.isClocked:
			self.__UpdateDescription2()
			return

		if not self.description:
			return

		toolTip = self.description
	
		if self.endTime > 0:
			leftTime = localeInfo.SecondToDHM(self.endTime - app.GetGlobalTimeStamp())
			toolTip += " (%s : %s)" % (localeInfo.LEFT_TIME, leftTime)
		self.SetToolTipText(toolTip, 0, 40)

	def __UpdateDescription2(self):
		if not self.description:
			return

		toolTip = self.description
		self.SetToolTipText(toolTip, 0, 40)

	def SetSkillAffectFlag(self, flag):
		self.isSkillAffect = flag

	def IsSkillAffect(self):
		return self.isSkillAffect

	if app.ENABLE_PLAYER_AFFECT_REMOVE:
		def SetSkillIndex(self, skillIndex):
			self.skillIndex = skillIndex

	if app.ENABLE_PLAYER_AFFECT_REMOVE:
		def IsRemovableSkill(self, skillIndex):
			SKILLS = [64, 94, 95, 96, 110, 111, 174]
			if app.ENABLE_BUFF_ITEMS_SYSTEM:
				SKILLS += [97 , 98, 99, 100, 101]

			if skillIndex in SKILLS:
				return True

			return False

	def OnMouseOverIn(self):
		if self.affect == AFF_AFK_TIME:
			toolTip = constInfo.GetInterfaceInstance().tooltipItem
			if toolTip:
				toolTip.ClearToolTip()
				toolTip.SetTitle(localeInfo.TOOLTIP_AFFECT_AFK_TIME)
				toolTip.AppendDescription(localeInfo.TOOLTIP_AFFECT_AFK_TIME_DESC, 26)
				
				toolTip.AppendSpace(2)

				affectString = toolTip.GetAffectString(item.APPLY_ATTBONUS_STONE, 10)
				toolTip.AppendTextLine(affectString, toolTip.GetAttributeColor(1, 10))

				affectString = toolTip.GetAffectString(item.APPLY_ATTBONUS_BOSS, 10)
				toolTip.AppendTextLine(affectString, toolTip.GetAttributeColor(1, 10))

				affectString = toolTip.GetAffectString(item.APPLY_ATTBONUS_MONSTER, 10)
				toolTip.AppendTextLine(affectString, toolTip.GetAttributeColor(1, 10))

				if self.endTime > 0:
					toolTip.AppendSpace(5)
					restSecond = localeInfo.SecondToDHM(self.endTime - app.GetGlobalTimeStamp())
					toolTip.AppendTextLine(localeInfo.LEFT_TIME + " : " + restSecond, toolTip.NORMAL_COLOR)
		
				toolTip.ShowToolTip()
		else:
			if self.toolTipText:
				self.toolTipText.Show()

		if app.ENABLE_PLAYER_AFFECT_REMOVE:
			if (self.IsRemovableSkill(self.skillIndex) or self.affect == chr.NEW_AFFECT_POLYMORPH):
				self.SetScale(0.8, 0.8)

	def OnMouseOverOut(self):
		if self.affect == AFF_AFK_TIME and constInfo.GetInterfaceInstance():
			toolTip = constInfo.GetInterfaceInstance().tooltipItem
			if toolTip:
				toolTip.HideToolTip()
		
		if self.toolTipText:
			self.toolTipText.Hide()

		if app.ENABLE_PLAYER_AFFECT_REMOVE:
			if (self.IsRemovableSkill(self.skillIndex) or self.affect == chr.NEW_AFFECT_POLYMORPH):
				self.SetScale(0.7, 0.7)

class AffectShower(ui.Window):

	MALL_DESC_IDX_START = 1000
	if constInfo.ENABLE_POTIONS_AFFECTSHOWER:
		DEW_DESC_IDX_START = 1200
		WATER_DESC_IDX_START = 1400
		DRAGON_GOD_DESC_IDX_START = 1600
		FISH_POTION_DESC_IDX_START = 1700

	IMAGE_STEP = 25
	AFFECT_MAX_NUM = 32

	INFINITE_AFFECT_DURATION = 0x1FFFFFFF

	if app.RENEWAL_PICKUP_AFFECT:
		pickupQuestionDialog = None

	AFFECT_DATA_DICT =	{
			chr.AFFECT_POISON : (localeInfo.SKILL_TOXICDIE, "d:/ymir work/ui/skill/common/affect/poison.sub", 1, 0),
			chr.AFFECT_SLOW : (localeInfo.SKILL_SLOW, "d:/ymir work/ui/skill/common/affect/slow.sub", 1, 0),
			chr.AFFECT_STUN : (localeInfo.SKILL_STUN, "d:/ymir work/ui/skill/common/affect/stun.sub", 1, 0),

			chr.AFFECT_ATT_SPEED_POTION : (localeInfo.SKILL_INC_ATKSPD, "d:/ymir work/ui/skill/common/affect/Increase_Attack_Speed.sub", 1, 0),
			chr.AFFECT_MOV_SPEED_POTION : (localeInfo.SKILL_INC_MOVSPD, "d:/ymir work/ui/skill/common/affect/Increase_Move_Speed.sub", 1, 0),
			chr.AFFECT_FISH_MIND : (localeInfo.SKILL_FISHMIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub", 1, 0),

			chr.AFFECT_JEONGWI : (localeInfo.SKILL_JEONGWI, "d:/ymir work/ui/skill/warrior/jeongwi_03.sub", 3, 0),
			chr.AFFECT_GEOMGYEONG : (localeInfo.SKILL_GEOMGYEONG, "d:/ymir work/ui/skill/warrior/geomgyeong_03.sub", 3, 0),
			chr.AFFECT_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub", 3, 0),
			chr.AFFECT_GYEONGGONG : (localeInfo.SKILL_GYEONGGONG, "d:/ymir work/ui/skill/assassin/gyeonggong_03.sub", 3, 0),
			chr.AFFECT_EUNHYEONG : (localeInfo.SKILL_EUNHYEONG, "d:/ymir work/ui/skill/assassin/eunhyeong_03.sub", 3, 0),
			chr.AFFECT_GWIGEOM : (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/sura/gwigeom_03.sub", 3, 0),
			chr.AFFECT_GONGPO : (localeInfo.SKILL_GONGPO, "d:/ymir work/ui/skill/sura/gongpo_03.sub", 3, 0),
			chr.AFFECT_JUMAGAP : (localeInfo.SKILL_JUMAGAP, "d:/ymir work/ui/skill/sura/jumagap_03.sub", 3, 0),
			chr.AFFECT_HOSIN : (localeInfo.SKILL_HOSIN, "d:/ymir work/ui/skill/shaman/hosin_03.sub", 3, 0),
			chr.AFFECT_BOHO : (localeInfo.SKILL_BOHO, "d:/ymir work/ui/skill/shaman/boho_03.sub", 3, 0),
			chr.AFFECT_KWAESOK : (localeInfo.SKILL_KWAESOK, "d:/ymir work/ui/skill/shaman/kwaesok_03.sub", 3, 0),
			chr.AFFECT_HEUKSIN : (localeInfo.SKILL_HEUKSIN, "d:/ymir work/ui/skill/sura/heuksin_03.sub", 3, 0),
			chr.AFFECT_MUYEONG : (localeInfo.SKILL_MUYEONG, "d:/ymir work/ui/skill/sura/muyeong_03.sub", 3, 0),
			chr.AFFECT_GICHEON : (localeInfo.SKILL_GICHEON, "d:/ymir work/ui/skill/shaman/gicheon_03.sub", 3, 0),
			chr.AFFECT_JEUNGRYEOK : (localeInfo.SKILL_JEUNGRYEOK, "d:/ymir work/ui/skill/shaman/jeungryeok_03.sub", 3, 0),
			chr.AFFECT_PABEOP : (localeInfo.SKILL_PABEOP, "d:/ymir work/ui/skill/sura/pabeop_03.sub", 3, 0),
			chr.AFFECT_FALLEN_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub", 3, 0),
			28 : (localeInfo.SKILL_FIRE, "d:/ymir work/ui/skill/sura/hwayeom_03.sub", 3, 0),
			chr.AFFECT_CHINA_FIREWORK : (localeInfo.SKILL_POWERFUL_STRIKE, "d:/ymir work/ui/skill/common/affect/powerfulstrike.sub", 3, 0),

			chr.NEW_AFFECT_NO_DEATH_PENALTY : (localeInfo.TOOLTIP_APPLY_NO_DEATH_PENALTY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),
			chr.NEW_AFFECT_SKILL_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_BONUS, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),
			chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_NO_DELAY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),

			chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_hpgauge/05.dds", 1, 0),
			chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_spgauge/05.dds", 1, 0),
			#chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			#chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub"),

			MALL_DESC_IDX_START+player.POINT_MALL_ATTBONUS : (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/att_bonus.sub", 1, 0),
			MALL_DESC_IDX_START+player.POINT_MALL_DEFBONUS : (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/def_bonus.sub", 1, 0),
			MALL_DESC_IDX_START+player.POINT_MALL_EXPBONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub", 1, 0),
			MALL_DESC_IDX_START+player.POINT_MALL_ITEMBONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS, "d:/ymir work/ui/skill/common/affect/item_bonus.sub", 1, 0),
			MALL_DESC_IDX_START+player.POINT_MALL_GOLDBONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub", 1, 0),
			MALL_DESC_IDX_START+player.POINT_CRITICAL_PCT : (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT,"d:/ymir work/ui/skill/common/affect/critical.sub", 1, 0),
			MALL_DESC_IDX_START+player.POINT_PENETRATE_PCT : (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),
			MALL_DESC_IDX_START+player.POINT_MAX_HP_PCT : (localeInfo.TOOLTIP_MAX_HP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),
			MALL_DESC_IDX_START+player.POINT_MAX_SP_PCT : (localeInfo.TOOLTIP_MAX_SP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),

			MALL_DESC_IDX_START+player.POINT_PC_BANG_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/EXP_Bonus_p_on.sub", 1, 0),
			MALL_DESC_IDX_START+player.POINT_PC_BANG_DROP_BONUS: (localeInfo.TOOLTIP_MALL_ITEMBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/Item_Bonus_p_on.sub", 1, 0),
	}
	if constInfo.ENABLE_MALL_AFFECTS:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_EXP_BONUS] = (localeInfo.TOOLTIP_MALL_EXPBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub", 1, 0)
		AFFECT_DATA_DICT[chr.NEW_AFFECT_ITEM_BONUS] = (localeInfo.TOOLTIP_MALL_ITEMBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/item_bonus.sub", 1, 0)
		AFFECT_DATA_DICT[chr.NEW_AFFECT_SAFEBOX] = (localeInfo.TOOLTIP_MALL_SAFEBOX, "d:/ymir work/ui/skill/common/affect/safebox.sub", 1, 0)
		AFFECT_DATA_DICT[chr.NEW_AFFECT_AUTOLOOT] = (localeInfo.TOOLTIP_MALL_AUTOLOOT, "d:/ymir work/ui/skill/common/affect/autoloot.sub", 1, 0)
		AFFECT_DATA_DICT[chr.NEW_AFFECT_FISH_MIND] = (localeInfo.TOOLTIP_MALL_FISH_MIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub", 1, 0)
		AFFECT_DATA_DICT[chr.NEW_AFFECT_MARRIAGE_FAST] = (localeInfo.TOOLTIP_MALL_MARRIAGE_FAST, "d:/ymir work/ui/skill/common/affect/marriage_fast.sub", 1, 0)
		AFFECT_DATA_DICT[chr.NEW_AFFECT_GOLD_BONUS] = (localeInfo.TOOLTIP_MALL_GOLDBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub", 1, 0)

	if constInfo.ENABLE_POTIONS_AFFECTSHOWER:
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/game/affectshower/50813.sub", 2, 0)
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, "d:/ymir work/ui/game/affectshower/50814.sub", 2, 0)
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, "d:/ymir work/ui/game/affectshower/50817.sub", 2, 0)
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, "d:/ymir work/ui/game/affectshower/50818.sub", 2, 0)
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_MAGIC_DEF_GRADE, "d:/ymir work/ui/game/affectshower/50819.sub", 2, 0)
		AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, "d:/ymir work/ui/game/affectshower/50820.sub", 2, 0)
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, "d:/ymir work/ui/game/affectshower/50821.sub", 2, 0)
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/game/affectshower/50822.sub", 2, 0)
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, "d:/ymir work/ui/game/affectshower/50823.sub", 2, 0)
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_RESIST_MAGIC, "d:/ymir work/ui/game/affectshower/50824.sub", 2, 0)
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+53] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER, "d:/ymir work/ui/game/affectshower/50823.sub", 2, 0)
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, "d:/ymir work/ui/game/affectshower/50825.sub", 2, 0)
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, "d:/ymir work/ui/game/affectshower/50826.sub", 2, 0)
		AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.ENERGY] = (localeInfo.TOOLTIP_ENERGY, "d:/ymir work/ui/game/affectshower/51002.sub", 2, 0)
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MAX_HP_PCT] = (localeInfo.TOOLTIP_MAX_HP_PCT, "d:/ymir work/ui/game/affectshower/71027.sub", 2, 0)
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MAX_SP_PCT] = (localeInfo.TOOLTIP_MAX_SP_PCT, "d:/ymir work/ui/game/affectshower/71029.sub", 2, 0)
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+132] = (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/game/affectshower/71028.sub", 2, 0)
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MALL_DEFBONUS] = (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, "d:/ymir work/ui/game/affectshower/71030.sub", 2, 0)
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, "d:/ymir work/ui/game/affectshower/71044.sub", 2, 0)
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/game/affectshower/71045.sub", 2, 0)
		AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+43] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_HUMAN, "d:/ymir work/ui/game/affectshower/71029.sub", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+6] = (localeInfo.TOOLTIP_APPLY_MAX_HP_PCT, "icon/item/27892.tga", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+45] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_ORC, "icon/item/27862.tga", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+47] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_UNDEAD, "icon/item/27887.tga", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+48] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_DEVIL, "icon/item/27857.tga", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+53] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER, "icon/item/27886.tga", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+93] = (localeInfo.TOOLTIP_ATT_GRADE, "icon/item/27856.tga", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+117] = (localeInfo.TOOLTIP_MALL_ITEMBONUS, "icon/item/27890.tga", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+84] = (localeInfo.TOOLTIP_MALL_GOLDBONUS, "icon/item/27860.tga", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+122] = (localeInfo.TOOLTIP_NORMAL_HIT_DAMAGE_BONUS, "icon/item/27831.tga", 2, 0)
		AFFECT_DATA_DICT[FISH_POTION_DESC_IDX_START+97] = (localeInfo.TOOLTIP_MAGIC_ATTBONUS_PER, "icon/item/27881.tga", 2, 0)

	if app.ENABLE_DRAGON_SOUL_SYSTEM:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK1] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK1, "d:/ymir work/ui/dragonsoul/buff_ds_sky1.tga", 1, 0)
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK2] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK2, "d:/ymir work/ui/dragonsoul/buff_ds_land1.tga", 1, 0)
	if app.ENABLE_WOLFMAN_CHARACTER:
		AFFECT_DATA_DICT[chr.AFFECT_BLEEDING] = (localeInfo.SKILL_BLEEDING, "d:/ymir work/ui/skill/common/affect/poison.sub", 3, 0)
		AFFECT_DATA_DICT[chr.AFFECT_RED_POSSESSION] = (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/wolfman/red_possession_03.sub", 3, 0)
		AFFECT_DATA_DICT[chr.AFFECT_BLUE_POSSESSION] = (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/wolfman/blue_possession_03.sub", 3, 0)
	if app.ENABLE_PREMIUM_PRIVATE_SHOP:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_PREMIUM_PRIVATE_SHOP] = (localeInfo.TOOLTIP_AFFECT_PREMIUM_PRIVATE_SHOP, "d:/ymir work/ui/skill/common/affect/premium_private_shop.sub", 1, 0)
	if app.ENABLE_PLAYER_AFFECT_REMOVE:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_POLYMORPH] = (localeInfo.POLYMORPH_AFFECT_TOOLTIP, "d:/ymir work/ui/skill/common/affect/polymorph.sub", 1, 0)
	if app.ENABLE_BUFF_ITEMS_SYSTEM:
		AFFECT_DATA_DICT[chr.AFFECT_RESIST_BUFF] = (localeInfo.TOOLTIP_RESIST_BUFF, "d:/ymir work/ui/skill/shaman/hosin_03.sub", 3, 0)
		AFFECT_DATA_DICT[chr.AFFECT_CRITICAL_BUFF] = (localeInfo.TOOLTIP_CRITICAL_BUFF, "d:/ymir work/ui/skill/shaman/gicheon_03.sub", 3, 0)
		AFFECT_DATA_DICT[chr.AFFECT_REFLECT_BUFF] = (localeInfo.TOOLTIP_REFLECT_BUFF, "d:/ymir work/ui/skill/shaman/boho_03.sub", 3, 0)	
		AFFECT_DATA_DICT[chr.AFFECT_MOV_SPEED_BUFF] = (localeInfo.TOOLTIP_MOV_SPEED_BUFF, "d:/ymir work/ui/skill/shaman/jeungryeok_03.sub", 3, 0)
		AFFECT_DATA_DICT[chr.AFFECT_ATT_GRADE_BUFF] = (localeInfo.TOOLTIP_ATT_GRADE_BUFF, "d:/ymir work/ui/skill/shaman/kwaesok_03.sub", 3, 0)	
	if app.ENABLE_METIN_STONE_QUEUE:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_METIN_QUEUE] =  (localeInfo.NEW_AFFECT_AUTO_METIN_FARM, "d:/ymir work/ui/game/affectshower/queue_affect.tga", 1, 0)
	if app.RENEWAL_PICKUP_AFFECT:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_PICKUP_ENABLE] =  (localeInfo.NEW_AFFECT_PICKUP_ENABLE, "d:/ymir work/ui/skill/common/affect/pickup_active.sub", 1, 0)
		AFFECT_DATA_DICT[chr.NEW_AFFECT_PICKUP_DEACTIVE] =  (localeInfo.NEW_AFFECT_PICKUP_DEACTIVE, "d:/ymir work/ui/skill/common/affect/pickup_deactive.sub", 1, 0)
	if app.ENABLE_VOTE_4_BUFF:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_VOTE_4_BUFF] = (localeInfo.NEW_AFFECT_VOTE_4_BUFF, "d:/ymir work/ui/affect/vote4buff.png", 1, 0)
	if app.ENABLE_BATTLE_PASS:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_BATTLE_PASS] =	(localeInfo.NEW_AFFECT_BATTLE_PASS, "d:/ymir work/ui/game/battle_pass/affect_icon.tga", 1, 0)
		
	AFFECT_DATA_DICT[AFF_AFK_TIME] = (localeInfo.NEW_AFFECT_BATTLE_PASS, "icon/afk/afk.png", 1, 0)
		
	if app.__BL_MULTI_LANGUAGE__:
		@staticmethod
		def ReloadVariables():
			AffectShower.AFFECT_DATA_DICT =	{
				chr.AFFECT_POISON : (localeInfo.SKILL_TOXICDIE, "d:/ymir work/ui/skill/common/affect/poison.sub", 1, 0),
				chr.AFFECT_SLOW : (localeInfo.SKILL_SLOW, "d:/ymir work/ui/skill/common/affect/slow.sub", 1, 0),
				chr.AFFECT_STUN : (localeInfo.SKILL_STUN, "d:/ymir work/ui/skill/common/affect/stun.sub", 1, 0),

				chr.AFFECT_ATT_SPEED_POTION : (localeInfo.SKILL_INC_ATKSPD, "d:/ymir work/ui/skill/common/affect/Increase_Attack_Speed.sub", 1, 0),
				chr.AFFECT_MOV_SPEED_POTION : (localeInfo.SKILL_INC_MOVSPD, "d:/ymir work/ui/skill/common/affect/Increase_Move_Speed.sub", 1, 0),
				chr.AFFECT_FISH_MIND : (localeInfo.SKILL_FISHMIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub", 1, 0),

				chr.AFFECT_JEONGWI : (localeInfo.SKILL_JEONGWI, "d:/ymir work/ui/skill/warrior/jeongwi_03.sub", 3, 0),
				chr.AFFECT_GEOMGYEONG : (localeInfo.SKILL_GEOMGYEONG, "d:/ymir work/ui/skill/warrior/geomgyeong_03.sub", 3, 0),
				chr.AFFECT_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub", 3, 0),
				chr.AFFECT_GYEONGGONG : (localeInfo.SKILL_GYEONGGONG, "d:/ymir work/ui/skill/assassin/gyeonggong_03.sub", 3, 0),
				chr.AFFECT_EUNHYEONG : (localeInfo.SKILL_EUNHYEONG, "d:/ymir work/ui/skill/assassin/eunhyeong_03.sub", 3, 0),
				chr.AFFECT_GWIGEOM : (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/sura/gwigeom_03.sub", 3, 0),
				chr.AFFECT_GONGPO : (localeInfo.SKILL_GONGPO, "d:/ymir work/ui/skill/sura/gongpo_03.sub", 3, 0),
				chr.AFFECT_JUMAGAP : (localeInfo.SKILL_JUMAGAP, "d:/ymir work/ui/skill/sura/jumagap_03.sub",  3, 0),
				chr.AFFECT_HOSIN : (localeInfo.SKILL_HOSIN, "d:/ymir work/ui/skill/shaman/hosin_03.sub", 3, 0),
				chr.AFFECT_BOHO : (localeInfo.SKILL_BOHO, "d:/ymir work/ui/skill/shaman/boho_03.sub", 3, 0),
				chr.AFFECT_KWAESOK : (localeInfo.SKILL_KWAESOK, "d:/ymir work/ui/skill/shaman/kwaesok_03.sub", 3, 0),
				chr.AFFECT_HEUKSIN : (localeInfo.SKILL_HEUKSIN, "d:/ymir work/ui/skill/sura/heuksin_03.sub", 3, 0),
				chr.AFFECT_MUYEONG : (localeInfo.SKILL_MUYEONG, "d:/ymir work/ui/skill/sura/muyeong_03.sub", 3, 0),
				chr.AFFECT_GICHEON : (localeInfo.SKILL_GICHEON, "d:/ymir work/ui/skill/shaman/gicheon_03.sub", 3, 0),
				chr.AFFECT_JEUNGRYEOK : (localeInfo.SKILL_JEUNGRYEOK, "d:/ymir work/ui/skill/shaman/jeungryeok_03.sub", 3, 0),
				chr.AFFECT_PABEOP : (localeInfo.SKILL_PABEOP, "d:/ymir work/ui/skill/sura/pabeop_03.sub", 3, 0),
				chr.AFFECT_FALLEN_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub", 3, 0),
				28 : (localeInfo.SKILL_FIRE, "d:/ymir work/ui/skill/sura/hwayeom_03.sub", 3, 0),
				chr.AFFECT_CHINA_FIREWORK : (localeInfo.SKILL_POWERFUL_STRIKE, "d:/ymir work/ui/skill/common/affect/powerfulstrike.sub", 3, 0),

				#64 - END
				chr.NEW_AFFECT_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub", 1, 0),

				chr.NEW_AFFECT_ITEM_BONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/item_bonus.sub", 1, 0),
				chr.NEW_AFFECT_SAFEBOX : (localeInfo.TOOLTIP_MALL_SAFEBOX, "d:/ymir work/ui/skill/common/affect/safebox.sub", 1, 0),
				chr.NEW_AFFECT_AUTOLOOT : (localeInfo.TOOLTIP_MALL_AUTOLOOT, "d:/ymir work/ui/skill/common/affect/autoloot.sub", 1, 0),
				chr.NEW_AFFECT_FISH_MIND : (localeInfo.TOOLTIP_MALL_FISH_MIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub", 1, 0),
				chr.NEW_AFFECT_MARRIAGE_FAST : (localeInfo.TOOLTIP_MALL_MARRIAGE_FAST, "d:/ymir work/ui/skill/common/affect/marriage_fast.sub", 1, 0),
				chr.NEW_AFFECT_GOLD_BONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub", 1, 0),

				chr.NEW_AFFECT_NO_DEATH_PENALTY : (localeInfo.TOOLTIP_APPLY_NO_DEATH_PENALTY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),
				chr.NEW_AFFECT_SKILL_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_BONUS, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),
				chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_NO_DELAY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),

				chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_hpgauge/05.dds", 1, 0),
				chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_spgauge/05.dds", 1, 0),

				AffectShower.MALL_DESC_IDX_START+player.POINT_MALL_ATTBONUS : (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/att_bonus.sub", 1, 0),
				AffectShower.MALL_DESC_IDX_START+player.POINT_MALL_DEFBONUS : (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/def_bonus.sub", 1, 0),
				AffectShower.MALL_DESC_IDX_START+player.POINT_MALL_EXPBONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/skill/common/affect/exp_bonus.sub", 1, 0),
				AffectShower.MALL_DESC_IDX_START+player.POINT_MALL_ITEMBONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS, "d:/ymir work/ui/skill/common/affect/item_bonus.sub", 1, 0),
				AffectShower.MALL_DESC_IDX_START+player.POINT_MALL_GOLDBONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub", 1, 0),
				AffectShower.MALL_DESC_IDX_START+player.POINT_CRITICAL_PCT : (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT,"d:/ymir work/ui/skill/common/affect/critical.sub", 1, 0),
				AffectShower.MALL_DESC_IDX_START+player.POINT_PENETRATE_PCT : (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),
				AffectShower.MALL_DESC_IDX_START+player.POINT_MAX_HP_PCT : (localeInfo.TOOLTIP_MAX_HP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),
				AffectShower.MALL_DESC_IDX_START+player.POINT_MAX_SP_PCT : (localeInfo.TOOLTIP_MAX_SP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub", 1, 0),

				AffectShower.MALL_DESC_IDX_START+player.POINT_PC_BANG_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/EXP_Bonus_p_on.sub", 1, 0),
				AffectShower.MALL_DESC_IDX_START+player.POINT_PC_BANG_DROP_BONUS: (localeInfo.TOOLTIP_MALL_ITEMBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/Item_Bonus_p_on.sub", 1, 0),
			}
			if app.ENABLE_DRAGON_SOUL_SYSTEM:
				AffectShower.AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK1] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK1, "d:/ymir work/ui/dragonsoul/buff_ds_sky1.tga", 1, 0)
				AffectShower.AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK2] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK2, "d:/ymir work/ui/dragonsoul/buff_ds_land1.tga", 1, 0)
			if app.ENABLE_WOLFMAN_CHARACTER:
				AffectShower.AFFECT_DATA_DICT[chr.AFFECT_BLEEDING] = (localeInfo.SKILL_BLEEDING, "d:/ymir work/ui/skill/common/affect/poison.sub", 3, 0)
				AffectShower.AFFECT_DATA_DICT[chr.AFFECT_RED_POSSESSION] = (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/wolfman/red_possession_03.sub", 3, 0)
				AffectShower.AFFECT_DATA_DICT[chr.AFFECT_BLUE_POSSESSION] = (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/wolfman/blue_possession_03.sub", 3, 0)

	def __init__(self):
		ui.Window.__init__(self)

		self.serverPlayTime=0
		self.clientPlayTime=0
		self.newPartyY=0

		self.lastUpdateTime=0
		self.affectImageDict={}
		self.horseImage=None
		self.lovePointImage=None
		if app.ENABLE_PLAYER_AFFECT_REMOVE:
			self.polymorphQuestionDialog = None
			self.skillAffectQuestionDialog = None
		self.autoPotionImageHP = AutoPotionImage()
		self.autoPotionImageSP = AutoPotionImage()
		self.SetPosition(10, 10)
		self.Show()

	def __del__(self):
		ui.Window.__del__(self)

	if app.ENABLE_PLAYER_AFFECT_REMOVE:
		def OnPolymorphQuestionDialog(self):
			self.polymorphQuestionDialog = uiCommon.QuestionDialog()
			self.polymorphQuestionDialog.SetText(localeInfo.POLYMORPH_AFFECT_REMOVE_QUESTION)
			self.polymorphQuestionDialog.SetWidth(350)
			self.polymorphQuestionDialog.SetAcceptEvent(lambda arg = True: self.OnClosePolymorphQuestionDialog(arg))
			self.polymorphQuestionDialog.SetCancelEvent(lambda arg = False: self.OnClosePolymorphQuestionDialog(arg))
			self.polymorphQuestionDialog.Open()
			
		def OnClosePolymorphQuestionDialog(self, answer):
			if not self.polymorphQuestionDialog:
				return

			self.polymorphQuestionDialog.Close()
			self.polymorphQuestionDialog = None
					
			if not answer:
				return

			net.SendChatPacket("/remove_polymorph")
			return True
			
		def OnSkillAffectQuestionDialog(self, affect):
			self.skillAffectQuestionDialog = uiCommon.QuestionDialog()
			self.skillAffectQuestionDialog.SetText(localeInfo.SKILL_AFFECT_REMOVE_QUESTION)
			self.skillAffectQuestionDialog.SetWidth(350)
			self.skillAffectQuestionDialog.SetAcceptEvent(lambda arg = True: self.OnCloseSkillAffectQuestionDialog(arg))
			self.skillAffectQuestionDialog.SetCancelEvent(lambda arg = False: self.OnCloseSkillAffectQuestionDialog(arg))
			self.skillAffectQuestionDialog.affect = affect
			self.skillAffectQuestionDialog.Open()
			
		def OnCloseSkillAffectQuestionDialog(self, answer):
			if not self.skillAffectQuestionDialog:
				return

			if not answer:
				self.skillAffectQuestionDialog.Close()
				self.skillAffectQuestionDialog = None
				return
					
			net.SendChatPacket("/player_remove_affect {0}".format(int(self.skillAffectQuestionDialog.affect+1)))
			self.skillAffectQuestionDialog.Close()
			self.skillAffectQuestionDialog = None
			return True

	if app.RENEWAL_PICKUP_AFFECT:
		def OnChangePickup(self, answer):
			if not self.pickupQuestionDialog:
				return
			if answer:
				net.SendChatPacket("/pickup_affect")
			self.pickupQuestionDialog.Close()
			self.pickupQuestionDialog = None

		def OnPickUpQuestionDialog(self, affect):
			if self.pickupQuestionDialog:
				self.pickupQuestionDialog.Close()
			pickupQuestionDialog = uiCommon.QuestionDialog()
			pickupQuestionDialog.SetText(localeInfo.PICKUP_DEACTIVE_DIALOG if affect == chr.NEW_AFFECT_PICKUP_ENABLE else localeInfo.PICKUP_ACTIVE_DIALOG)
			pickupQuestionDialog.SetAcceptEvent(lambda arg = True: self.OnChangePickup(arg))
			pickupQuestionDialog.SetCancelEvent(lambda arg = False: self.OnChangePickup(arg))
			pickupQuestionDialog.Open()
			self.pickupQuestionDialog = pickupQuestionDialog

	def ClearAllAffects(self):
		self.horseImage=None
		self.lovePointImage=None
		self.affectImageDict={}
		self.__ArrangeImageList()

	def ClearAffects(self):
		self.living_affectImageDict={}
		for key, image in self.affectImageDict.items():
			if not image.IsSkillAffect():
				self.living_affectImageDict[key] = image
		self.affectImageDict = self.living_affectImageDict
		self.__ArrangeImageList()

	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):

		print "BINARY_NEW_AddAffect", type, pointIdx, value, duration
		
		# import chat
		# chat.AppendChat(1, "type %d" % (type))
		
		# ENABLE_PLAYER_AFFECT_REMOVE
		if type < 500 and\
			not type == chr.NEW_AFFECT_POLYMORPH and\
			not type == chr.NEW_AFFECT_METIN_QUEUE and\
			not type == chr.NEW_AFFECT_PICKUP_ENABLE and\
			not type == chr.NEW_AFFECT_PICKUP_DEACTIVE and\
			not type == chr.NEW_AFFECT_VOTE_4_BUFF:
			return
		
		if type == AFF_AFK_TIME:
			return
		
		if type == chr.NEW_AFFECT_MALL:
			if constInfo.ENABLE_POTIONS_AFFECTSHOWER:
				if pointIdx >= player.POINT_MALL_EXPBONUS and pointIdx <= player.POINT_MALL_GOLDBONUS:
					affect = self.MALL_DESC_IDX_START + pointIdx
				else:
					affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
			else:
				affect = self.MALL_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_BLEND:
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and (app.ENABLE_EXTENDED_BLEND and type == chr.NEW_AFFECT_BLEND_EX):
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_WATER:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_MALL_EX:
			affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_FISH_POTION:
			affect = self.FISH_POTION_DESC_IDX_START + pointIdx
		else:
			affect = type

		if self.affectImageDict.has_key(affect):
			return

		if not self.AFFECT_DATA_DICT.has_key(affect):
			return

		if affect == chr.NEW_AFFECT_NO_DEATH_PENALTY or\
		   affect == chr.NEW_AFFECT_SKILL_BOOK_BONUS or\
		   affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or\
		   affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY or\
		   affect == chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY:
			duration = 0

		affectData = self.AFFECT_DATA_DICT[affect]
		description = affectData[0]
		filename = affectData[1]

		if pointIdx == player.POINT_MALL_ITEMBONUS or\
		   pointIdx == player.POINT_MALL_GOLDBONUS:
			value = 1 + float(value) / 100.0

		if affect != chr.NEW_AFFECT_AUTO_SP_RECOVERY and affect != chr.NEW_AFFECT_AUTO_HP_RECOVERY:
			description = description(float(value))

		try:
			print "Add affect %s" % affect
			image = AffectImage()
			image.SetParent(self)
			image.LoadImage(filename)
			image.SetDescription(description)
			image.SetDuration(duration)
			image.SetAffect(affect)
			if affect == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE or\
				affect == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE_UNDER_15 or\
				self.INFINITE_AFFECT_DURATION < duration:
				image.SetClock(False)
				image.UpdateDescription()
			elif affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
				image.UpdateAutoPotionDescription()
			else:
				image.UpdateDescription()

			if affect == chr.NEW_AFFECT_DRAGON_SOUL_DECK1 or affect == chr.NEW_AFFECT_DRAGON_SOUL_DECK2:
				image.SetScale(1, 1)
			else:
				image.SetScale(0.7, 0.7)
				
			image.SetSkillAffectFlag(False)
			image.Show()
			if app.ENABLE_PLAYER_AFFECT_REMOVE or app.RENEWAL_PICKUP_AFFECT:
				image.SAFE_SetStringEvent("mouse_left_down",self.__OnSelect, affect)
			self.affectImageDict[affect] = image
			self.__ArrangeImageList()
		except:
			print "except Aff affect"
			pass

	if app.ENABLE_PLAYER_AFFECT_REMOVE or app.RENEWAL_PICKUP_AFFECT:
		def __OnSelect(self, affect):
			if app.RENEWAL_PICKUP_AFFECT:
				if affect == chr.NEW_AFFECT_PICKUP_ENABLE or affect == chr.NEW_AFFECT_PICKUP_DEACTIVE:
					self.OnPickUpQuestionDialog(affect)

			if affect == chr.NEW_AFFECT_POLYMORPH:
				self.OnPolymorphQuestionDialog()

			if affect == chr.AFFECT_HOSIN or affect == chr.AFFECT_BOHO or affect == chr.AFFECT_GICHEON or affect == chr.AFFECT_KWAESOK or affect == chr.AFFECT_JEUNGRYEOK:
				self.OnSkillAffectQuestionDialog(affect)

			if app.ENABLE_WOLFMAN_CHARACTER:
				if affect == chr.AFFECT_BLUE_POSSESSION:
					self.OnSkillAffectQuestionDialog(affect)

			if app.ENABLE_BUFF_ITEMS_SYSTEM:
				if affect == chr.AFFECT_RESIST_BUFF or affect == chr.AFFECT_CRITICAL_BUFF or affect == chr.AFFECT_REFLECT_BUFF or affect == chr.AFFECT_MOV_SPEED_BUFF or affect == chr.AFFECT_ATT_GRADE_BUFF:
					self.OnSkillAffectQuestionDialog(affect)

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		if type == chr.NEW_AFFECT_MALL:
			if constInfo.ENABLE_POTIONS_AFFECTSHOWER:
				if pointIdx >= player.POINT_MALL_EXPBONUS and pointIdx <= player.POINT_MALL_GOLDBONUS:
					affect = self.MALL_DESC_IDX_START + pointIdx
				else:
					affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
			else:
				affect = self.MALL_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_BLEND:
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and (app.ENABLE_EXTENDED_BLEND and type == chr.NEW_AFFECT_BLEND_EX):
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_WATER:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_MALL_EX:
			affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
		elif constInfo.ENABLE_POTIONS_AFFECTSHOWER and type == chr.NEW_AFFECT_FISH_POTION:
			affect = self.FISH_POTION_DESC_IDX_START + pointIdx
		else:
			affect = type

		print "Remove Affect %s %s" % ( type , pointIdx )
		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

	def SetAffect(self, affect):
		self.__AppendAffect(affect)
		self.__ArrangeImageList()

	def ResetAffect(self, affect):
		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

	def SetLoverInfo(self, name, lovePoint):
		image = LovePointImage()
		image.SetParent(self)
		image.SetLoverInfo(name, lovePoint)
		self.lovePointImage = image
		self.__ArrangeImageList()

	def ShowLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Show()
			self.__ArrangeImageList()

	def HideLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Hide()
			self.__ArrangeImageList()

	def ClearLoverState(self):
		self.lovePointImage = None
		self.__ArrangeImageList()

	def OnUpdateLovePoint(self, lovePoint):
		if self.lovePointImage:
			self.lovePointImage.OnUpdateLovePoint(lovePoint)

	def SetHorseState(self, level, health, battery):
		if level==0:
			self.horseImage=None
		else:
			image = HorseImage()
			image.SetParent(self)
			image.SetState(level, health, battery)
			image.Show()

			self.horseImage=image
		self.__ArrangeImageList()

	def SetPlayTime(self, playTime):
		self.serverPlayTime = playTime
		self.clientPlayTime = app.GetTime()

		if localeInfo.IsVIETNAM():
			image = PlayTimeImage()
			image.SetParent(self)
			image.SetPlayTime(playTime)
			image.Show()

			self.playTimeImage=image
			self.__ArrangeImageList()

	def __AppendAffect(self, affect):

		if self.affectImageDict.has_key(affect):
			return

		try:
			affectData = self.AFFECT_DATA_DICT[affect]
		except KeyError:
			return

		name = affectData[0]
		filename = affectData[1]

		skillIndex = player.AffectIndexToSkillIndex(affect)
		if 0 != skillIndex:
			name = skill.GetSkillName(skillIndex)

		image = AffectImage()
		image.SetParent(self)
		image.SetSkillAffectFlag(True)
		if app.ENABLE_PLAYER_AFFECT_REMOVE:
			image.SetSkillIndex(skillIndex)

		try:
			image.LoadImage(filename)
		except:
			pass

		image.SetToolTipText(name, 0, 40)
		if app.ENABLE_PLAYER_AFFECT_REMOVE:
			image.SAFE_SetStringEvent("mouse_left_down",self.__OnSelect, affect)
		image.SetScale(0.7, 0.7)
		image.Show()
		self.affectImageDict[affect] = image

	def __RemoveAffect(self, affect):
		"""
		if affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY:
			self.autoPotionImageSP.Hide()

		if affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
			self.autoPotionImageHP.Hide()
		"""

		if not self.affectImageDict.has_key(affect):
			print "__RemoveAffect %s ( No Affect )" % affect
			return

		print "__RemoveAffect %s ( Affect )" % affect
		del self.affectImageDict[affect]

		self.__ArrangeImageList()

	"""def __ArrangeImageList(self):

		width = len(self.affectImageDict) * self.IMAGE_STEP
		if self.lovePointImage:
			width+=self.IMAGE_STEP
		if self.horseImage:
			width+=self.IMAGE_STEP

		self.SetSize(width, 26)

		xPos = 0

		if self.lovePointImage:
			if self.lovePointImage.IsShow():
				self.lovePointImage.SetPosition(xPos, 0)
				xPos += self.IMAGE_STEP

		if self.horseImage:
			self.horseImage.SetPosition(xPos, 0)
			xPos += self.IMAGE_STEP

		for image in self.affectImageDict.values():
			image.SetPosition(xPos, 0)
			xPos += self.IMAGE_STEP"""

	# def __ArrangeImageList(self):
		# width = self.IMAGE_STEP * 10
		# self.SetSize(width, 26)
		# xPos = 0
		# yPos = 0
		# AFFECT_COUNT = 0
		# AFFECTS_NUM_ROW = 10

		# if self.lovePointImage:
			# if self.lovePointImage.IsShow():
				# self.lovePointImage.SetPosition(xPos, 0)
				# xPos += self.IMAGE_STEP
				# AFFECT_COUNT += 1

		# if self.horseImage:
			# self.horseImage.SetPosition(xPos, 0)
			# xPos += self.IMAGE_STEP
			# AFFECT_COUNT += 1

		# for image in self.affectImageDict.values():
			# image.SetPosition(xPos, yPos)
			# xPos += self.IMAGE_STEP
			# AFFECT_COUNT += 1
			# if AFFECT_COUNT >= AFFECTS_NUM_ROW:
				# if len(self.affectImageDict) == AFFECT_COUNT: continue
				# AFFECT_COUNT = 0
				# xPos=0
				# yPos += self.IMAGE_STEP
				# self.SetSize(width, 26*((yPos/self.IMAGE_STEP)+1))

		# self.SetSize(AFFECTS_NUM_ROW * self.IMAGE_STEP, yPos + self.IMAGE_STEP)
		# self.SetPartyNewPosition(yPos)

	def get_idx(self, j):
		return j[0]

	def __ArrangeImageList(self):
		imageDict = self.affectImageDict
		sortDict = [[] for i in xrange(10)]
		yPos = 40

		if self.lovePointImage:
			sortDict[0].append([2, self.lovePointImage])
		if self.horseImage:
			sortDict[0].append([1,self.horseImage])

		for affect, affectImage in self.affectImageDict.iteritems():
			affectData = self.AFFECT_DATA_DICT[affect]
			yIdx = affectData[2] if len(affectData) > 3 else 1
			xIdx = affectData[3] if len(affectData) > 3 else 0
			sortDict[yIdx-1].append([xIdx, affectImage])

		ySize, xSize, xSizeTotal = (0, 0, 0)
		for imageList in sortDict:
			isHasImage = False

			if len(imageList) > 1:
				imageList = sorted(imageList, key= self.get_idx, reverse=False)

			for image in imageList:
				isHasImage = True
				image[1].SetPosition(xSize, ySize)
				xSize += self.IMAGE_STEP
			if xSize > xSizeTotal:
				xSizeTotal = xSize
			xSize = 0
			if isHasImage:
				ySize+=26
		self.SetSize(xSizeTotal, ySize)
		self.SetPartyNewPosition(yPos)

	def SetPartyNewPosition(self, newY):
		self.newPartyY = newY
	
	def GetPartyNewPosition(self):
		return self.newPartyY

	def OnUpdate(self):
		try:
			if app.GetGlobalTime() - self.lastUpdateTime > 500:
			#if 0 < app.GetGlobalTime():
				self.lastUpdateTime = app.GetGlobalTime()

				for image in self.affectImageDict.values():
					if image.GetAffect() == chr.NEW_AFFECT_AUTO_HP_RECOVERY or image.GetAffect() == chr.NEW_AFFECT_AUTO_SP_RECOVERY:
						image.UpdateAutoPotionDescription()
						continue

					if not image.IsSkillAffect():
						image.UpdateDescription()
		except Exception, e:
			print "AffectShower::OnUpdate error : ", e

