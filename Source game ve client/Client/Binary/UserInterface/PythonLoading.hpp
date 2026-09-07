

#pragma once

#if defined(ENABLE_LOADING_PERFORMANCE)
extern const DWORD c_iSkillIndex_Tongsol;
extern const DWORD c_iSkillIndex_Fishing;
extern const DWORD c_iSkillIndex_Mining;
extern const DWORD c_iSkillIndex_Making;
extern const DWORD c_iSkillIndex_Combo;
extern const DWORD c_iSkillIndex_Language1;
extern const DWORD c_iSkillIndex_Language2;
extern const DWORD c_iSkillIndex_Language3;
extern const DWORD c_iSkillIndex_Polymorph;
extern const DWORD c_iSkillIndex_RoleProficiency;
extern const DWORD c_iSkillIndex_Insight;
extern const DWORD c_iSkillIndex_Hit;

extern std::map<int, CGraphicImage*> m_kMap_iEmotionIndex_pkIconImage;

enum EEmotionIcon : WORD
{
	EMOTION_CLAP = 1,

	EMOTION_CONGRATULATION,
	EMOTION_FORGIVE,
	EMOTION_ANGRY,

	EMOTION_ATTRACTIVE,
	EMOTION_SAD,
	EMOTION_SHY,
	EMOTION_CHEERUP,
	EMOTION_BANTER,
	EMOTION_JOY,

	EMOTION_CHEERS_1,
	EMOTION_CHEERS_2,

	EMOTION_DANCE_1,
	EMOTION_DANCE_2,
	EMOTION_DANCE_3,
	EMOTION_DANCE_4,
	EMOTION_DANCE_5,
	EMOTION_DANCE_6, // °­³²½ºÅ¸ÀÏ

	EMOTION_KISS = 51,
	EMOTION_FRENCH_KISS,
	EMOTION_SLAP,
#if defined(ENABLE_EXPRESSING_EMOTION)
	EMOTION_PUSH_UP,
	EMOTION_DANCE_7,
	EMOTION_EXERCISE,
	EMOTION_DOZE,
	EMOTION_SELFIE,
	EMOTION_CHARGING,
	EMOTION_NOSAY,
	EMOTION_WEATHER_1,
	EMOTION_WEATHER_2,
	EMOTION_WEATHER_3,
	EMOTION_HUNGRY,
	EMOTION_SIREN,
	EMOTION_LETTER,
	EMOTION_CALL,
	EMOTION_CELEBRATION,
	EMOTION_ALCOHOL,
	EMOTION_BUSY,
	EMOTION_WHIRL,
#endif
};

enum EComboType : BYTE
{
	COMBO_TYPE_1,
	COMBO_TYPE_2,
	COMBO_TYPE_3,
	COMBO_TYPE_MAX,
};

enum EComboIndex : BYTE
{
	COMBO_INDEX_1,
	COMBO_INDEX_2,
	COMBO_INDEX_3,
	COMBO_INDEX_4,
	COMBO_INDEX_5,
	COMBO_INDEX_6,
};

enum EDustGap : WORD
{
	DUST_GAP = 250,
	HORSE_DUST_GAP = 500,
};

enum EHorseSkill : DWORD
{
	HORSE_SKILL_WILDATTACK = CRaceMotionData::NAME_SKILL + 121,
	HORSE_SKILL_CHARGE = CRaceMotionData::NAME_SKILL + 122,
	HORSE_SKILL_SPLASH = CRaceMotionData::NAME_SKILL + 123,
};

enum EGuildSkill : DWORD
{
	GUILD_SKILL_DRAGONBLOOD = CRaceMotionData::NAME_SKILL + 101,
	GUILD_SKILL_DRAGONBLESS = CRaceMotionData::NAME_SKILL + 102,
	GUILD_SKILL_BLESSARMOR = CRaceMotionData::NAME_SKILL + 103,
	GUILD_SKILL_SPPEDUP = CRaceMotionData::NAME_SKILL + 104,
	GUILD_SKILL_DRAGONWRATH = CRaceMotionData::NAME_SKILL + 105,
	GUILD_SKILL_MAGICUP = CRaceMotionData::NAME_SKILL + 106,
};

enum ELanguageSkill : DWORD
{
	EMPIRE_LANGUAGE_1 = 126,
	EMPIRE_LANGUAGE_2 = 127,
	EMPIRE_LANGUAGE_3 = 128,
};

enum ENPCListToken : BYTE
{
	NPC_TOKEN_TYPE_VNUM,
	NPC_TOKEN_TYPE_MODEL_NAME,
	NPC_LIST_TOKEN_MODEL_SRC_NAME,
	NPC_LIST_TOKEN_MAX,
};

enum EGuildBuildingToken : BYTE
{
	GUIILD_BUILDING_TOKEN_VNUM,
	GUIILD_BUILDING_TOKEN_TYPE,
	GUIILD_BUILDING_TOKEN_NAME,
	GUIILD_BUILDING_TOKEN_LOCAL_NAME,
	GUIILD_BUILDING_NO_USE_TOKEN_SIZE_1,
	GUIILD_BUILDING_NO_USE_TOKEN_SIZE_2,
	GUIILD_BUILDING_NO_USE_TOKEN_SIZE_3,
	GUIILD_BUILDING_NO_USE_TOKEN_SIZE_4,
	GUIILD_BUILDING_TOKEN_X_ROT_LIMIT,
	GUIILD_BUILDING_TOKEN_Y_ROT_LIMIT,
	GUIILD_BUILDING_TOKEN_Z_ROT_LIMIT,
	GUIILD_BUILDING_TOKEN_PRICE,
	GUIILD_BUILDING_TOKEN_MATERIAL,
	GUIILD_BUILDING_TOKEN_NPC,
	GUIILD_BUILDING_TOKEN_GROUP,
	GUIILD_BUILDING_TOKEN_DEPEND_GROUP,
	GUIILD_BUILDING_TOKEN_ENABLE_FLAG,
#if defined(ENABLE_CONQUEROR_LEVEL)
	GUIILD_BUILDING_TOKEN_NEW_WORLD,
#endif
	GUIILD_BUILDING_LIMIT_TOKEN_COUNT,
};

enum EGuildBuildingMaterialItem : DWORD
{
	MATERIAL_STONE_ID = 90010,
	MATERIAL_LOG_ID = 90011,
	MATERIAL_PLYWOOD_ID = 90012
};

enum EGuildBuildingMaterialIndex : BYTE
{
	MATERIAL_STONE_INDEX,
	MATERIAL_LOG_INDEX,
	MATERIAL_PLYWOOD_INDEX
};

static constexpr int g_iSafeSleep = 1500; //1500
static constexpr bool g_bCacheMotionData = false;

static const std::string g_stNPCList_FileName = "npclist.txt";
static const std::string g_stRaceHeight_FileName = "race_height.txt";

// Sound Data
static const SoundDataVector g_vUseSoundData
{
	{ CPythonItem::USESOUND_DEFAULT, "sound/ui/drop.wav" },
	{ CPythonItem::USESOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav" },
	{ CPythonItem::USESOUND_ARMOR, "sound/ui/equip_metal_armor.wav" },
	{ CPythonItem::USESOUND_BOW, "sound/ui/equip_bow.wav" },
	{ CPythonItem::USESOUND_WEAPON, "sound/ui/equip_metal_weapon.wav" },
	{ CPythonItem::USESOUND_POTION, "sound/ui/eat_potion.wav" },
	{ CPythonItem::USESOUND_PORTAL, "sound/ui/potal_scroll.wav" },
} /*11*/;
static const SoundDataVector g_vDropSoundData
{
	{ CPythonItem::DROPSOUND_DEFAULT, "sound/ui/drop.wav" },
	{ CPythonItem::DROPSOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav" },
	{ CPythonItem::DROPSOUND_ARMOR, "sound/ui/equip_metal_armor.wav" },
	{ CPythonItem::DROPSOUND_BOW, "sound/ui/equip_bow.wav" },
	{ CPythonItem::DROPSOUND_WEAPON, "sound/ui/equip_metal_weapon.wav" },
};

// Effect
static const EffectDataVector g_vEffectData
{
	{ CInstanceBase::EFFECT_DUST, "", "d:/ymir work/effect/etc/dust/dust.mse", false },
	{ CInstanceBase::EFFECT_HORSE_DUST, "", "d:/ymir work/effect/etc/dust/running_dust.mse", false },
	{ CInstanceBase::EFFECT_HIT, "", "d:/ymir work/effect/hit/blow_1/blow_1_low.mse", false },

	{ CInstanceBase::EFFECT_HPUP_RED, "", "d:/ymir work/effect/etc/recuperation/drugup_red.mse", false },
	{ CInstanceBase::EFFECT_SPUP_BLUE, "", "d:/ymir work/effect/etc/recuperation/drugup_blue.mse", false },
	{ CInstanceBase::EFFECT_SPEEDUP_GREEN, "", "d:/ymir work/effect/etc/recuperation/drugup_green.mse", false },
	{ CInstanceBase::EFFECT_DXUP_PURPLE, "", "d:/ymir work/effect/etc/recuperation/drugup_purple.mse", false },

	// ÀÚµ¿¹°¾à HP, SP
	{ CInstanceBase::EFFECT_AUTO_HPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_red.mse", false },
	{ CInstanceBase::EFFECT_AUTO_SPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_blue.mse", false },

	// ¶ó¸¶´Ü ÃÊ½Â´ÞÀÇ ¹ÝÁö(71135) Âø¿ë¼ø°£ ¹ßµ¿ ÀÌÆåÆ®
	{ CInstanceBase::EFFECT_RAMADAN_RING_EQUIP, "", "d:/ymir work/effect/law_ringeffect/law_ringeffect.mse", false },

	// ÇÒ·ÎÀ© »çÅÁ Âø¿ë¼ø°£ ¹ßµ¿ ÀÌÆåÆ®
	{ CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item2.mse", false },

	// Çàº¹ÀÇ ¹ÝÁö Âø¿ë¼ø°£ ¹ßµ¿ ÀÌÆåÆ®
	{ CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item3.mse", false },

	// »ç¶ûÀÇ ÆÒ´øÆ® Âø¿ë¼ø°£ ¹ßµ¿ ÀÌÆåÆ®
	{ CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item4.mse", false },

	//{ CInstanceBase::EFFECT_MAGIC_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item5.mse", false },

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	{ CInstanceBase::EFFECT_ACCE_SUCESS_ABSORB, "", "d:/ymir work/effect/etc/buff/buff_item6.mse", false },
	{ CInstanceBase::EFFECT_ACCE_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item7.mse", false },
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	{ CInstanceBase::EFFECT_SELECT_PRIVATE_SHOP, "", "d:/ymir work/effect/etc/direction/direction_land2.mse", false },
#endif

	// ºÎÈ°Àý Äµµð(71188) Âø¿ë¼ø°£ ¹ßµ¿ ÀÌÆåÆ®
	//{ CInstanceBase::EFFECT_EASTER_CANDY_EQIP, "", "d:/ymir work/effect/etc/buff/buff_item8.mse", false },

	{ CInstanceBase::EFFECT_PENETRATE, "Bip01", "d:/ymir work/effect/hit/gwantong.mse", false },
	//{ CInstanceBase::EFFECT_BLOCK, "", "d:/ymir work/effect/etc/", false },
	//{ CInstanceBase::EFFECT_DODGE, "", "d:/ymir work/effect/etc/", false },
	{ CInstanceBase::EFFECT_FIRECRACKER, "", "d:/ymir work/effect/etc/firecracker/newyear_firecracker.mse", false },
	{ CInstanceBase::EFFECT_SPIN_TOP, "", "d:/ymir work/effect/etc/firecracker/paing_i.mse", false },
	{ CInstanceBase::EFFECT_SELECT, "", "d:/ymir work/effect/etc/click/click_select.mse", false },
	{ CInstanceBase::EFFECT_TARGET, "", "d:/ymir work/effect/etc/click/click_glow_select.mse", false },
	{ CInstanceBase::EFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun.mse", false },
	{ CInstanceBase::EFFECT_CRITICAL, "Bip01 R Hand", "d:/ymir work/effect/hit/critical.mse", false },

	{ CInstanceBase::EFFECT_DAMAGE_TARGET, "", "d:/ymir work/effect/affect/damagevalue/target.mse", false },

	{ CInstanceBase::EFFECT_DAMAGE_NOT_TARGET, "", "d:/ymir work/effect/affect/damagevalue/nontarget.mse", false },
	{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE, "", "d:/ymir work/effect/affect/damagevalue/damage.mse", false },
	{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE2, "", "d:/ymir work/effect/affect/damagevalue/damage_1.mse", false },
	{ CInstanceBase::EFFECT_DAMAGE_POISON, "", "d:/ymir work/effect/affect/damagevalue/poison.mse", false },
	{ CInstanceBase::EFFECT_DAMAGE_MISS, "", "d:/ymir work/effect/affect/damagevalue/miss.mse", false },
	{ CInstanceBase::EFFECT_DAMAGE_TARGETMISS, "", "d:/ymir work/effect/affect/damagevalue/target_miss.mse", false },
	{ CInstanceBase::EFFECT_DAMAGE_CRITICAL, "", "d:/ymir work/effect/affect/damagevalue/critical.mse", false },

	//{ CInstanceBase::EFFECT_SUCCESS, "", "d:/ymir work/effect/success.mse", false },
	//{ CInstanceBase::EFFECT_FAIL, "", "d:/ymir work/effect/fail.mse", false },

	{ CInstanceBase::EFFECT_LEVELUP_ON_14_FOR_GERMANY, "","season1/effect/paymessage_warning.mse", false }, // ¹º§¾÷ 14ÀÏ¶§ ( µ¶ÀÏÀü¿ë )
	{ CInstanceBase::EFFECT_LEVELUP_UNDER_15_FOR_GERMANY, "", "season1/effect/paymessage_decide.mse", false }, //·¹º§¾÷ 15ÀÏ¶§ ( µ¶ÀÏÀü¿ë )

	{ CInstanceBase::EFFECT_PERCENT_DAMAGE1, "", "d:/ymir work/effect/hit/percent_damage1.mse", false },
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE2, "", "d:/ymir work/effect/hit/percent_damage2.mse", false },
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE3, "", "d:/ymir work/effect/hit/percent_damage3.mse", false },
	////////////////////////////////////////////////////////////////////////////////////////////////////

	{ CInstanceBase::EFFECT_SPAWN_APPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_appear.mse", false },
	{ CInstanceBase::EFFECT_SPAWN_DISAPPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_die.mse", false },
	{ CInstanceBase::EFFECT_FLAME_ATTACK, "equip_right_hand", "d:/ymir work/effect/hit/blow_flame/flame_3_weapon.mse", false },
	{ CInstanceBase::EFFECT_FLAME_HIT, "", "d:/ymir work/effect/hit/blow_flame/flame_3_blow.mse", false },
	{ CInstanceBase::EFFECT_FLAME_ATTACH, "", "d:/ymir work/effect/hit/blow_flame/flame_3_body.mse", false },
	{ CInstanceBase::EFFECT_ELECTRIC_ATTACK, "equip_right", "d:/ymir work/effect/hit/blow_electric/light_1_weapon.mse", false },
	{ CInstanceBase::EFFECT_ELECTRIC_HIT, "", "d:/ymir work/effect/hit/blow_electric/light_1_blow.mse", false },
	{ CInstanceBase::EFFECT_ELECTRIC_ATTACH, "", "d:/ymir work/effect/hit/blow_electric/light_1_body.mse", false },

	{ CInstanceBase::EFFECT_LEVELUP, "", "d:/ymir work/effect/etc/levelup_1/level_up.mse", false },
	{ CInstanceBase::EFFECT_SKILLUP, "", "d:/ymir work/effect/etc/skillup/skillup_1.mse", false },

	{ CInstanceBase::EFFECT_EMPIRE + 1, "Bip01", "d:/ymir work/effect/etc/empire/empire_A.mse", false },
	{ CInstanceBase::EFFECT_EMPIRE + 2, "Bip01", "d:/ymir work/effect/etc/empire/empire_B.mse", false },
	{ CInstanceBase::EFFECT_EMPIRE + 3, "Bip01", "d:/ymir work/effect/etc/empire/empire_C.mse", false },

	{ CInstanceBase::EFFECT_WEAPON + 1, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_sword_loop.mse", false },
	{ CInstanceBase::EFFECT_WEAPON + 2, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_spear_loop.mse", false },

	// LOCALE
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_YMIR, "Bip01", "locale/common/effect/gm.mse", false },
	// END_OF_LOCALE

	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_POISON, "Bip01", "d:/ymir work/effect/hit/blow_poison/poison_loop.mse", false }, // Áßµ¶
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::NEW_AFFECT_POISON, "Bip01", "d:/ymir work/effect/hit/blow_poison/poison_loop.mse", false }, // Áßµ¶

	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_SLOW, "", "d:/ymir work/effect/affect/slow.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::NEW_AFFECT_SLOW, "", "d:/ymir work/effect/affect/slow.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::NEW_AFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_DUNGEON_READY, "", "d:/ymir work/effect/etc/ready/ready.mse", false },

	//{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_BUILDING_CONSTRUCTION_SMALL, "", "d:/ymir work/guild/effect/10_construction.mse", false },
	//{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_BUILDING_CONSTRUCTION_LARGE, "", "d:/ymir work/guild/effect/20_construction.mse", false },
	//{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_BUILDING_UPGRADE, "", "d:/ymir work/guild/effect/20_upgrade.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_CHEONGEUN, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", false }, // Ãµ±ÙÃß (¹Ø¿¡µµ ÀÖµû-_-)
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GYEONGGONG, "", "d:/ymir work/pc/assassin/effect/gyeonggong_loop.mse", false }, // ÀÚ°´ - °æ°ø
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_EUNHYEONG, "", "d:/ymir work/pc/assassin/effect/eunhyeongbeop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GWIGEOM, "Bip01 R Finger2", "d:/ymir work/pc/sura/effect/gwigeom_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GONGPO, "", "d:/ymir work/pc/sura/effect/fear_loop.mse", false }, // ¼ö¶ó - °øÆ÷
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_JUMAGAP, "", "d:/ymir work/pc/sura/effect/jumagap_loop.mse", false }, // ¼ö¶ó - ÁÖ¸¶°©
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_HOSIN, "", "d:/ymir work/pc/shaman/effect/3hosin_loop.mse", false }, // ¹«´ç - È£½Å
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_BOHO, "", "d:/ymir work/pc/shaman/effect/boho_loop.mse", false }, // ¹«´ç - º¸È£
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_KWAESOK, "", "d:/ymir work/pc/shaman/effect/10kwaesok_loop.mse", false }, // ¹«´ç - Äè¼Ó
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_HEUKSIN, "", "d:/ymir work/pc/sura/effect/heuksin_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_MUYEONG, "", "d:/ymir work/pc/sura/effect/muyeong_loop.mse", false },

	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_FIRE, "Bip01", "d:/ymir work/effect/hit/hwayeom_loop_1.mse", false },

#ifdef ENABLE_WOLFMAN_CHARACTER
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_RED_POSSESSION, "Bip01", "d:/ymir work/effect/hit/blow_flame/flame_loop_w.mse", false },
#endif
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GICHEON, "Bip01 R Hand", "d:/ymir work/pc/shaman/effect/6gicheon_hand.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_JEUNGRYEOK, "Bip01 L Hand", "d:/ymir work/pc/shaman/effect/jeungryeok_hand.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_PABEOP, "Bip01 Head", "d:/ymir work/pc/sura/effect/pabeop_loop.mse", false },
#ifdef ENABLE_WOLFMAN_CHARACTER
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_BLUE_POSSESSION, "", "d:/ymir work/pc3/common/effect/gyeokgongjang_loop_w.mse", false },
#endif

	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_FALLEN_CHEONGEUN, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", false }, // Ãµ±ÙÃß(Fallen)

	// 34 Polymoph
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_WAR_FLAG1, "", "d:/ymir work/effect/etc/guild_war_flag/flag_red.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_WAR_FLAG2, "", "d:/ymir work/effect/etc/guild_war_flag/flag_blue.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_WAR_FLAG3, "", "d:/ymir work/effect/etc/guild_war_flag/flag_yellow.mse", false },

#ifdef ENABLE_BUFF_ITEMS_SYSTEM
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_RESIST_BUFF, "", "d:/ymir work/my_work/effect/resistbuff.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_CRITICAL_BUFF, "Bip01 R Hand", "d:/ymir work/my_work/effect/criticalbuff.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_REFLECT_BUFF, "", "d:/ymir work/my_work/effect/reflectbuff.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_MOV_SPEED_BUFF, "", "d:/ymir work/pc/shaman/effect/10kwaesok_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_ATT_GRADE_BUFF, "Bip01 L Hand", "d:/ymir work/pc/shaman/effect/jeungryeok_hand.mse", false },
#endif

	// SWORD
	{ CInstanceBase::EFFECT_REFINED + 1, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 2, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 3, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9.mse", false },

	// BOW
	{ CInstanceBase::EFFECT_REFINED + 4, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 5, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 6, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_b.mse", false },

	// FAN
	{ CInstanceBase::EFFECT_REFINED + 7, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 8, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 9, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_f.mse", false },

	// DEGER RIGHT
	{ CInstanceBase::EFFECT_REFINED + 10, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 11, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 12, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", false },

	// DEGER LEFT
	{ CInstanceBase::EFFECT_REFINED + 13, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 14, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 15, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", false },

	// BODY
	{ CInstanceBase::EFFECT_REFINED + 16, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_7.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 17, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_8.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 18, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_9.mse", false },

	// BODY SPECIAL
	{ CInstanceBase::EFFECT_REFINED + 19, "Bip01", "D:/ymir work/pc/common/effect/armor/armor-4-2-1.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 20, "Bip01", "D:/ymir work/pc/common/effect/armor/armor-4-2-2.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 21, "Bip01", "D:/ymir work/pc/common/effect/armor/armor-5-1.mse", false },

#if defined(ENABLE_ACCE_COSTUME_SYSTEM)
	{ CInstanceBase::EFFECT_ACCE_BACK, "Bip01", "D:/ymir work/pc/common/effect/armor/acc_01.mse", false },
#endif

#if defined(ENABLE_SHINING_ITEM_SYSTEM)
	// Swords from 0 to 49
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 0, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_blaukreisel/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 1, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_dunkelgreen/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 2, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_eisblau/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 3, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_fairy/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 4, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_gelb/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 5, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_orange/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 6, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pink/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 7, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkblume/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 8, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkstrahl/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 9, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_violett/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 10, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_1/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 11, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_2/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 12, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_3/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 13, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_4/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 14, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_5/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 15, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_6/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 16, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_7/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 17, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_8/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 18, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_9/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 19, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_10/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 20, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_11/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 21, "PART_WEAPON", "D:/ymir work/effect/ridack_effect_white/weaponwhite.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 22, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_sword_yellowgreen.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 23, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_sword_redblue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 24, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_sword_purpleturq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 25, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_sword.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 26, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_1hand_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 27, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_1hand_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 28, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_1hand_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 29, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_1hand_turk.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 30, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_1hand_yellow.mse", false },


 	// Two-Handed Swords from 50 to 99
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 50, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_blaukreisel/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 51, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_dunkelgreen/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 52, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_eisblau/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 53, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_fairy/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 54, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_gelb/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 55, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_orange/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 56, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pink/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 57, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkblume/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 58, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkstrahl/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 59, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_violett/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 60, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_1/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 61, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_2/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 62, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_3/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 63, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_4/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 64, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_5/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 65, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_6/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 66, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_7/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 67, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_8/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 68, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_9/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 69, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_10/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 70, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_11/300.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 71, "PART_WEAPON", "D:/ymir work/effect/ridack_effect_white/weaponwhite_2h.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 72, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_2hand_yellowgreen.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 73, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_2hand_redblue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 74, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_2hand_purpleturq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 75, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_2hand.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 76, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_2hand_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 77, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_2hand_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 78, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_2hand_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 79, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_2hand_turk.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 80, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_2hand_yellow.mse", false },


	// Bows from 100 to 149
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 100, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_blaukreisel/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 101, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_dunkelgreen/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 102, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_eisblau/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 103, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_fairy/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 104, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_gelb/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 105, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_orange/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 106, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_pink/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 107, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkblume/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 108, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkstrahl/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 109, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_violett/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 110, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_1/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 111, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_2/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 112, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_3/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 113, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_4/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 114, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_5/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 115, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_6/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 116, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_7/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 117, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_8/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 118, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_9/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 119, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_10/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 120, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_11/900.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 121, "PART_WEAPON_LEFT", "D:/ymir work/effect/ridack_effect_white/weaponwhite_b.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 122, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_bow_yellowgreen.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 123, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_bow_redblue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 124, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_bow_purpleturq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 125, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_bow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 126, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_bow_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 127, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_bow_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 128, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_bow_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 129, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_bow_turk.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 130, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_bow_yellow.mse", false },

	// Fans from 150 to 199
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 150, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_blaukreisel/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 151, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_dunkelgreen/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 152, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_eisblau/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 153, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_fairy/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 154, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_gelb/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 155, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_orange/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 156, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pink/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 157, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkblume/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 158, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkstrahl/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 159, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_violett/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 160, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_1/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 161, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_2/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 162, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_3/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 163, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_4/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 164, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_5/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 165, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_6/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 166, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_7/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 167, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_8/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 168, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_9/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 169, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_10/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 170, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_11/700.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 172, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_fan_yellowgreen.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 173, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_fan_redblue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 174, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_fan_purpleturq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 175, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_fan.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 176, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_fan_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 177, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_fan_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 178, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_fan_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 179, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_fan_turk.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 180, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_fan_yellow.mse", false },

	// Bells from 200 to 249
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 200, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_blaukreisel/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 201, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_dunkelgreen/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 202, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_eisblau/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 203, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_fairy/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 204, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_gelb/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 205, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_orange/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 206, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pink/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 207, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkblume/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 208, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkstrahl/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 209, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_violett/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 210, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_1/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 211, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_2/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 212, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_3/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 213, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_4/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 214, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_5/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 215, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_6/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 216, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_7/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 217, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_8/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 218, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_9/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 219, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_10/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 220, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_11/600.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 221, "PART_WEAPON", "D:/ymir work/effect/ridack_effect_white/weaponwhite_bell.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 222, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell_yellowgreen.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 223, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell_redblue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 224, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell_purpleturq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 225, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 226, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 227, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 228, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 229, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_turk.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 230, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_yellow.mse", false },

	// Dagger Right from 250 to 299
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 250, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_blaukreisel/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 251, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_dunkelgreen/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 252, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_eisblau/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 253, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_fairy/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 254, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_gelb/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 255, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_orange/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 256, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pink/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 257, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkblume/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 258, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkstrahl/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 259, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/18special/special_violett/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 260, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_1/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 261, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_2/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 262, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_3/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 263, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_4/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 264, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_5/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 265, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_6/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 266, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_7/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 267, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_8/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 268, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_9/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 269, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_10/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 270, "PART_WEAPON", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_11/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 271, "PART_WEAPON", "D:/ymir work/effect/ridack_effect_white/weaponwhite_c.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 272, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell_yellowgreen.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 273, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell_redblue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 274, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell_purpleturq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 275, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 276, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 277, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 278, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 279, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_turk.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 280, "PART_WEAPON", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_yellow.mse", false },

	// Dagger left from 300 to 349
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 300, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_blaukreisel/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 301, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_dunkelgreen/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 302, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_eisblau/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 303, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_fairy/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 304, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_gelb/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 305, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_orange/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 306, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_pink/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 307, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkblume/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 308, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_pinkstrahl/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 309, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/18special/special_violett/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 310, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_1/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 311, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_2/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 312, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_3/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 313, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_4/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 314, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_5/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 315, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_6/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 316, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_7/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 317, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_8/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 318, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_9/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 319, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_10/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 320, "PART_WEAPON_LEFT", "D:/ymir work/effect/etc/shining/weapon/21specialv2/specialv2_11/800.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 321, "PART_WEAPON_LEFT", "D:/ymir work/effect/ridack_effect_white/weaponwhite_c.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 322, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell_yellowgreen.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 323, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell_redblue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 324, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell_purpleturq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 325, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/partikel_shining/partikel_daggerbell.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 326, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 327, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 328, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 329, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_turk.mse", false },
	{ CInstanceBase::EFFECT_SHINING_WEAPON + 330, "PART_WEAPON_LEFT", "D:/ymir work/effect/teufels_shining/special/special_daggerbell_yellow.mse", false },

	// Armours
	{ CInstanceBase::EFFECT_SHINING_ARMOR+0, "Bip01", "D:/ymir work/effect/mehok/shining/dragon_green_max/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+1, "Bip01", "D:/ymir work/effect/mehok/shining/winter_boj/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+2, "Bip01", "D:/ymir work/effect/mehok/shining/pink/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+3, "Bip01", "D:/ymir work/effect/mehok/shining/hall_boj/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+4, "Bip01", "D:/ymir work/effect/mehok/shining/winter_hwang/hwang.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+5, "Bip01", "D:/ymir work/effect/mehok/shining/hall_hwang/hwang.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+6, "Bip01", "D:/ymir work/effect/Dunkelheit/dunkel_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+7, "Bip01", "D:/ymir work/effect/may/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+8, "Bip01", "D:/ymir work/effect/glory_dane_armor/glory_dane_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+9, "Bip01", "D:/ymir work/effect/Wind/wind_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+10, "Bip01", "D:/ymir work/effect/ridack_effect_black_white/ridack_armorfirebw.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+11, "Bip01", "D:/ymir work/effect/baltazar/shinings/necromant_effect/necromant_effect.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+12, "Bip01", "D:/ymir work/effect/dragon_news/effect_dragon_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+13, "Bip01", "D:/ymir work/effect/dragon_news/effect_dragon_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+14, "Bip01", "D:/ymir work/effect/gold_tiger_alune/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+15, "Bip01", "D:/ymir work/effect/dragon_news/effect_dragon_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+16, "Bip01", "D:/ymir work/effect/light_black/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+17, "Bip01", "D:/ymir work/effect/light_black/armor_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+18, "Bip01", "D:/ymir work/effect/light_black/armor_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+19, "Bip01", "D:/ymir work/effect/dragon_news/effect_dragon_negru.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+20, "Bip01", "D:/ymir work/effect/ridack_effect_rainbow/ridack_armorrainbow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+21, "Bip01", "D:/ymir work/new_shining_arm/pc/ridack_work/ridack_naruto_set_effect/ridack_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+22, "Bip01", "D:/ymir work/new_shining_arm/pc/ridack_work/ridack3_armor_2/ridack_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+23, "Bip01", "D:/ymir work/new_shining_arm/pc/ridack_work/ridack_akatsuki/susanoo/ridack_effect.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+24, "Bip01", "D:/ymir work/effect/teufels_shining/unikat/aras_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+25, "Bip01", "D:/ymir work/effect/teufels_shining/unikat/aras_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+26, "Bip01", "D:/ymir work/effect/teufels_shining/unikat/aras_pinkpurple.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+27, "Bip01", "D:/ymir work/effect/teufels_shining/unikat/aras_black.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+28, "Bip01", "D:/ymir work/effect/teufels_shining/unikat/aras_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+29, "Bip01", "D:/ymir work/effect/teufels_shining/dragon/dragon_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+30, "Bip01", "D:/ymir work/effect/teufels_shining/dragon/dragon_armor_pinkpurple.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+31, "Bip01", "D:/ymir work/effect/teufels_shining/dragon/dragon_armor_turq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+32, "Bip01", "D:/ymir work/effect/teufels_shining/dragon/dragon_armor_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+33, "Bip01", "D:/ymir work/effect/teufels_shining/dragon/dragon_armor_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+34, "Bip01", "D:/ymir work/effect/teufels_shining/dragon/dragon_armor_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+35, "Bip01", "D:/ymir work/effect/teufels_shining/dragon/dragon_armor1.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+36, "Bip01", "D:/ymir work/effect/teufels_shining/partikel_shining/armor_partikel.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+37, "Bip01", "D:/ymir work/effect/teufels_shining/partikel_shining/armor_partikel_purlpleturq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+38, "Bip01", "D:/ymir work/effect/teufels_shining/partikel_shining/armor_partikel_redblue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+39, "Bip01", "D:/ymir work/effect/teufels_shining/partikel_shining/armor_partikel_yellowgreen.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+40, "Bip01", "D:/ymir work/effect/teufels_shining/special/special_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+41, "Bip01", "D:/ymir work/effect/teufels_shining/special/special_armor_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+42, "Bip01", "D:/ymir work/effect/teufels_shining/special/special_armor_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+43, "Bip01", "D:/ymir work/effect/teufels_shining/special/special_armor_purple.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+44, "Bip01", "D:/ymir work/effect/teufels_shining/special/special_armor_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+45, "Bip01", "D:/ymir work/effect/teufels_shining/special/special_armor_turk.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+46, "Bip01", "D:/ymir work/effect/teufels_shining/special/special_armor_yellow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+47, "Bip01", "D:/ymir work/effect/armor/1sternstaub/sternstaub_blau.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+48, "Bip01", "D:/ymir work/effect/armor/1sternstaub/sternstaub_schwarz.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+49, "Bip01", "D:/ymir work/effect/armor/2spektral/spektral_gelb.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+50, "Bip01", "D:/ymir work/shinings/bubbles/armor_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+51, "Bip01", "D:/ymir work/shinings/bubbles/armor_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+52, "Bip01", "D:/ymir work/shinings/bubbles/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+53, "Bip01", "D:/ymir work/effect/armor/2spektral/spektral_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+54, "Bip01", "D:/ymir work/effect/armor/2spektral/spektral_white.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+55, "Bip01", "D:/ymir work/new_shining_arm/pc/ridack_work/ridack_blacksilver/ridack_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+56, "Bip01", "D:/ymir work/new_shining_arm/effect/law_shinings/aprilbpshining/aprilbparmor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+57, "Bip01", "D:/ymir work/new_shining_arm/effect/law_shinings/susanooblack/blackkosi.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+58, "Bip01", "D:/ymir work/effect/armor/3spektralv2/spektralv2_violett.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+59, "Bip01", "D:/ymir work/effect/armor/3spektralv2/spektralv2_rot.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+60, "Bip01", "D:/ymir work/effect/armor/4aze/azeshining_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+61, "Bip01", "D:/ymir work/effect/armor/4aze/azeshining_white.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+62, "Bip01", "D:/ymir work/effect/armor/6elektro/elektroshining_blau.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+63, "Bip01", "D:/ymir work/effect/armor/6elektro/elektroshining_rot.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+64, "Bip01", "D:/ymir work/effect/armor/7nebel/nebel_orange.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+65, "Bip01", "D:/ymir work/effect/armor/7nebel/nebel_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+66, "Bip01", "D:/ymir work/effect/armor/8mikoto/mikoto_dunkelcyan.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+67, "Bip01", "D:/ymir work/effect/armor/9god/godshining_cyan.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+68, "Bip01", "D:/ymir work/effect/armor/10energy/energy_blauschwarz.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+69, "Bip01", "D:/ymir work/effect/armor/11fairy/fairy_schwarz.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+70, "Bip01", "D:/ymir work/effect/armor/12weed/weed_violett.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+71, "Bip01", "D:/ymir work/effect/armor/12weed/weed_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+72, "Bip01", "D:/ymir work/effect/armor/13aurapartikel/aurapartikel_rot.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+73, "Bip01", "D:/ymir work/effect/armor/13aurapartikel/aurapartikel_violett.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+74, "Bip01", "D:/ymir work/effect/armor/15aura/aura_blau.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+75, "Bip01", "D:/ymir work/effect/armor/15aura/aura_violett.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+76, "Bip01", "D:/ymir work/effect/armor/21rauchglitzer/rauchglitzer_blau.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+77, "Bip01", "D:/ymir work/effect/armor/21rauchglitzer/rauchglitzer_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+78, "Bip01", "D:/ymir work/effect/armor/22dragonv2/dragonv2_white.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+79, "Bip01", "D:/ymir work/effect/armor/22dragonv2/dragonv2_cyan.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+80, "Bip01", "D:/ymir work/effect/armor/25rauchv2/rauchv2_leicht/armor_7.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+81, "Bip01", "D:/ymir work/effect/armor/25rauchv2/rauchv2_mittel/armor_8.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+82, "Bip01", "D:/ymir work/effect/armor/25rauchv2/rauchv2_schwer/armor_9.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+83, "Bip01", "D:/ymir work/effect/armor/23blitz/blitz_blau.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+84, "Bip01", "D:/ymir work/effect/armor/23blitz/blitz_cyan.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+85, "Bip01", "D:/ymir work/effect/armor/23blitz/blitz_gelb.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+86, "Bip01", "D:/ymir work/effect/armor/16demon/demon_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+87, "Bip01", "D:/ymir work/effect/armor/16demon/demon_violett.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+88, "Bip01", "D:/ymir work/effect/armor/16demon/demon_white.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+89, "Bip01", "D:/ymir work/effect/armor/16demon/demon_gelb.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+91, "Bip01", "D:/ymir work/effect/armor/8mikoto/mikoto_dunkelgreen.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+92, "Bip01", "D:/ymir work/effect/armor/8mikoto/mikoto_dunkelorange.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+93, "Bip01", "D:/ymir work/effect/armor/8mikoto/mikoto_dunkelrot.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+94, "Bip01", "D:/ymir work/effect/armor/8mikoto/mikoto_dunkelviolett.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+95, "Bip01", "D:/ymir work/effect/armor/8mikoto/mikoto_dunkelwhite.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+96, "Bip01", "D:/ymir work/effect/armor/8mikoto/mikoto_white.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+97, "Bip01", "D:/ymir work/effect/blitz/shining/electric_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+98, "Bip01", "D:/ymir work/effect/dragon_new/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+99, "Bip01", "D:/ymir work/effect/dragon_new/armor_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+100, "Bip01", "D:/ymir work/effect/dragon_new/armor_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+101, "Bip01", "D:/ymir work/effect/dragon_new/armor_pink.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+102, "Bip01", "D:/ymir work/effect/dragon_new/armor_yellow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+103, "Bip01", "D:/ymir work/effect/effectcostume/effect_costume_demon.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+104, "Bip01", "D:/ymir work/effect/future_set_law/law_future_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+105, "Bip01", "D:/ymir work/effect/future_set_law/law_future_armor_ver2.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+106, "Bip01", "D:/ymir work/effect/set_10_helios_shining/armor_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+107, "Bip01", "D:/ymir work/effect/set_10_helios_shining/armor_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+108, "Bip01", "D:/ymir work/effect/set_10_helios_shining/armor_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+109, "Bip01", "D:/ymir work/effect/shining/nero/ridack_armor_black.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+110, "Bip01", "D:/ymir work/effect/shining/orange/ridack_armor_orange.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+111, "Bip01", "D:/ymir work/effect/shining/verdino/ridack_armor_jade.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+112, "Bip01", "D:/ymir work/effect/shining/viola/ridack_armor_fushiia.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+113, "Bip01", "D:/ymir work/effect/shining1/bianco/ridack_armor_white2.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+114, "Bip01", "D:/ymir work/effect/shining1/blu/ridack_armor_blue2.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+115, "Bip01", "D:/ymir work/effect/shining1/rosa/ridack_armor_pink2.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+116, "Bip01", "D:/ymir work/effect/shining1/viola/ridack_armor_purple2.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+117, "Bip01", "D:/ymir work/effect/shining2/blu/ridack_armor_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+118, "Bip01", "D:/ymir work/effect/shining2/giallo/ridack_armor_yellow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+119, "Bip01", "D:/ymir work/effect/shining2/rosso/ridack_armor_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+120, "Bip01", "D:/ymir work/effect/shining2/verde/ridack_armor_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+121, "Bip01", "D:/ymir work/effect/shining13/ridack_armor_d.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+122, "Bip01", "D:/ymir work/effect/shinyshines/aze_shining_black.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+123, "Bip01", "D:/ymir work/effect/shinyshines/aze_shining_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+124, "Bip01", "D:/ymir work/effect/shinyshines/aze_shining_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+125, "Bip01", "D:/ymir work/effect/shinyshines/aze_shining_orange.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+126, "Bip01", "D:/ymir work/effect/shinyshines/aze_shining_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+127, "Bip01", "D:/ymir work/effect/shinyshines/aze_shining_violett.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+128, "Bip01", "D:/ymir work/effect/shinyshines/aze_shining_white.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+129, "Bip01", "D:/ymir work/effect/shinyshines/aze_shining_yellow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+130, "Bip01", "D:/ymir work/effect/unikat_1/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+131, "Bip01", "D:/ymir work/effect/unikat_1/armor_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+132, "Bip01", "D:/ymir work/effect/unikat_1/armor_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+133, "Bip01", "D:/ymir work/effect/unikat_1/armor_purple.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+134, "Bip01", "D:/ymir work/effect/unikat_1/armor_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+135, "Bip01", "D:/ymir work/effect/unikat_1/armor_turq.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+136, "Bip01", "D:/ymir work/effect/harwia/futuristic_effect.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+137, "Bip01", "D:/ymir work/effect/legenda/legend.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+138, "Bip01", "D:/ymir work/kehlani/special_kehlani/armor_2.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+139, "Bip01", "D:/ymir work/shinings/bubbles/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+140, "Bip01", "D:/ymir work/shinings/bubbles/armor_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+141, "Bip01", "D:/ymir work/shinings/bubbles/armor_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+142, "Bip01", "D:/ymir work/shinings/dragonball/sypphv_kaioken/sypphv_kaioken.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+143, "Bip01", "D:/ymir work/shinings/dragonball/sypphv_kaioken/sypphv_kaioken_bluepurple.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+144, "Bip01", "D:/ymir work/shinings/dragonball/sypphv_kaioken/sypphv_kaioken_orangered.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+145, "Bip01", "D:/ymir work/shinings/mehok/mehokspecial_armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+146, "Bip01", "D:/ymir work/shinings/mehok/mehokspecial_armor_purple.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+147, "Bip01", "D:/ymir work/shinings/mehok/mehokspecial_armor_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+148, "Bip01", "D:/ymir work/shinings/mehok/mehokspecial_armor_turk.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+149, "Bip01", "D:/ymir work/kehlani/neuesshining2v/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+150, "Bip01", "D:/ymir work/kehlani/geilesshining/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+151, "Bip01", "D:/ymir work/kehlani/pvp/black.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+152, "Bip01", "D:/ymir work/kehlani/pvp/blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+153, "Bip01", "D:/ymir work/kehlani/pvp/cyan.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+154, "Bip01", "D:/ymir work/kehlani/pvp/green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+155, "Bip01", "D:/ymir work/kehlani/pvp/orange.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+156, "Bip01", "D:/ymir work/kehlani/pvp/red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+157, "Bip01", "D:/ymir work/kehlani/pvp/vio.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+158, "Bip01", "D:/ymir work/kehlani/pvp/yellow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+159, "Bip01", "D:/ymir work/shining_effect/armor/dr_shop/armor_7.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+160, "Bip01", "D:/ymir work/shining_effect/armor/dr_shop/armor_8.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+161, "Bip01", "D:/ymir work/shining_effect/armor/dr_shop/armor_9.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+162, "Bip01", "D:/ymir work/shining_effect/armor/dr_shop_v4/armor_7.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+163, "Bip01", "D:/ymir work/shining_effect/armor/dr_shop_v4/armor_8.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+164, "Bip01", "D:/ymir work/shining_effect/armor/dr_shop_v4/armor_9.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+165, "Bip01", "D:/ymir work/shining_effect/armor/dr_shop_v6/armor_7.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+166, "Bip01", "D:/ymir work/shining_effect/armor/dr_shop_v6/armor_8.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+167, "Bip01", "D:/ymir work/shining_effect/armor/dr_shop_v6/armor_9.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+168, "Bip01", "D:/ymir work/shining_effect/armor/electric_shining/v2_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+169, "Bip01", "D:/ymir work/shining_effect/armor/electric_shining/v2_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+170, "Bip01", "D:/ymir work/shining_effect/armor/electric_shining/v2_orange.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+171, "Bip01", "D:/ymir work/shining_effect/armor/electric_shining/v2_pink.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+172, "Bip01", "D:/ymir work/shining_effect/armor/electric_shining/v2_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+173, "Bip01", "D:/ymir work/shining_effect/armor/electric_shining/v2_turkise.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+174, "Bip01", "D:/ymir work/shining_effect/armor/electric_shining/v2_violett.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+175, "Bip01", "D:/ymir work/shining_effect/armor/electric_shining/v2_white.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+176, "Bip01", "D:/ymir work/shining_effect/armor/electric_shining/v2_yellow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+177, "Bip01", "D:/ymir work/shining_effect/armor/shiino_work/shiino_blackwhite.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+178, "Bip01", "D:/ymir work/shining_effect/armor/shiino_work/shiino_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+179, "Bip01", "D:/ymir work/shining_effect/armor/shiino_work/shiino_green.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+180, "Bip01", "D:/ymir work/shining_effect/armor/shiino_work/shiino_orange.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+181, "Bip01", "D:/ymir work/shining_effect/armor/shiino_work/shiino_red.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+182, "Bip01", "D:/ymir work/shining_effect/armor/shiino_work/shiino_violet.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+183, "Bip01", "D:/ymir work/shining_effect/armor/shiino_work/shiino_white.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+184, "Bip01", "D:/ymir work/shining_effect/armor/shiino_work/shiino_yellow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+185, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/armor_epic_1.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+186, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/armor_epic_2.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+187, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/armor_epic_3.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+188, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/armor_epic_4.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+189, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/armor_epic_5.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+190, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/armor_halloween.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+191, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/armor_orange.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+192, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/armor-4-2-9.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+193, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/armor-4-2-16_1.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+194, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/atreyu_armor_blue.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+195, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_shining/golden.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+196, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_work/armor_7.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+197, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_work/armor_8.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+198, "Bip01", "D:/ymir work/shining_effect/armor/zelinski_work/armor_9.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+199, "Bip01", "D:/ymir work/shining_effect/armor/1/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+200, "Bip01", "D:/ymir work/shining_effect/armor/2/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+201, "Bip01", "D:/ymir work/shining_effect/armor/3/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+202, "Bip01", "D:/ymir work/shining_effect/armor/4/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+203, "Bip01", "D:/ymir work/shining_effect/armor/5/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+204, "Bip01", "D:/ymir work/shining_effect/armor/6/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+205, "Bip01", "D:/ymir work/shining_effect/armor/7/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+206, "Bip01", "D:/ymir work/shining_effect/armor/8/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+207, "Bip01", "D:/ymir work/shining_effect/armor/9/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+208, "Bip01", "D:/ymir work/shining_effect/armor/10/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+209, "Bip01", "D:/ymir work/shining_effect/armor/11/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+210, "Bip01", "D:/ymir work/shining_effect/armor/12/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+211, "Bip01", "D:/ymir work/shining_effect/armor/13/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+212, "Bip01", "D:/ymir work/shining_effect/armor/14/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+213, "Bip01", "D:/ymir work/shining_effect/armor/15/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+214, "Bip01", "D:/ymir work/shining_effect/armor/16/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+215, "Bip01", "D:/ymir work/shining_effect/armor/17/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+216, "Bip01", "D:/ymir work/shining_effect/armor/18/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+217, "Bip01", "D:/ymir work/shining_effect/armor/19/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+218, "Bip01", "D:/ymir work/shining_effect/armor/20/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+219, "Bip01", "D:/ymir work/shining_effect/armor/21/zbroja.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+220, "Bip01", "D:/ymir work/pc/common/effect/armor/candy.mse", false },
	{ CInstanceBase::EFFECT_SHINING_ARMOR+221, "Bip01", "D:/ymir work/item/weapon/arcane_set/armor.mse", false },

	{ CInstanceBase::EFFECT_SHINING_SPECIAL+0, "Bip01", "D:/ymir work/effect/alune_black_friday/black_eff.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+1, "Bip01", "D:/ymir work/effect/alune_dolar/costume_glow.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+2, "Bip01", "D:/ymir work/pc/common/effect/armor/armor-5-1.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+3, "Bip01", "D:/ymir work/effect/ridack_work/ridack_vip/ridack_vip.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+4, "Bip01", "D:/ymir work/pc/common/effect/aura/aura_aura_lv06.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+5, "Bip01", "D:/ymir work/pc/common/effect/aura/aura_200_249_005.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+6, "Bip01", "D:/ymir work/pc/common/effect/armor/shiningsystem/blauweed.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+7, "Bip01", "D:/ymir work/new_shining_arm/effect/law_shinings/obsidian/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+8, "Bip01", "D:/ymir work/effect/diving_73/armor.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+9, "Bip01", "D:/ymir work/pc/common/effect/armor/shiningsystem/twitch.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+10, "Bip01", "D:/ymir work/pc/common/effect/armor/shiningsystem/yt.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+11, "Bip01", "D:/ymir work/effect/promotion/tiktok.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+12, "Bip01", "D:/ymir work/effect/promotion/twitch.mse", false },
	{ CInstanceBase::EFFECT_SHINING_SPECIAL+13, "Bip01", "D:/ymir work/effect/promotion/youtube.mse", false },

#endif

#ifdef ENABLE_METIN_STONE_QUEUE
	{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_METIN_QUEUE, "", "d:/ymir work/effect/metinque/ridack_metinqueue.mse", false },
#endif
};

// Fly Effect
static const FlyEffectDataVector g_vFlyEffectData
{
	{ CInstanceBase::FLY_EXP, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_yellow_small2.msf" }, // ³ë¶õ»ö (EXP)
	{ CInstanceBase::FLY_HP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_small.msf" }, // »¡°£»ö (HP) ÀÛÀº°Å
	{ CInstanceBase::FLY_HP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_big.msf" }, // »¡°£»ö (HP) Å«°Å
	{ CInstanceBase::FLY_SP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_warrior_small.msf" }, // ÆÄ¶õ»ö ²¿¸®¸¸ ÀÖ´Â°Å
	{ CInstanceBase::FLY_SP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_small.msf" }, // ÆÄ¶õ»ö ÀÛÀº°Å
	{ CInstanceBase::FLY_SP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_big.msf" }, // ÆÄ¶õ»ö Å«°Å
	{ CInstanceBase::FLY_FIREWORK1, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_1.msf" }, // ÆøÁ× 1
	{ CInstanceBase::FLY_FIREWORK2, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_2.msf" }, // ÆøÁ× 2
	{ CInstanceBase::FLY_FIREWORK3, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_3.msf" }, // ÆøÁ× 3
	{ CInstanceBase::FLY_FIREWORK4, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_4.msf" }, // ÆøÁ× 4
	{ CInstanceBase::FLY_FIREWORK5, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_5.msf" }, // ÆøÁ× 5
	{ CInstanceBase::FLY_FIREWORK6, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_6.msf" }, // ÆøÁ× 6
	{ CInstanceBase::FLY_FIREWORK_XMAS, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_xmas.msf" }, // ÆøÁ× X-Mas
	{ CInstanceBase::FLY_CHAIN_LIGHTNING, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/pc/shaman/effect/pokroe.msf" }, // Æø·Ú°Ý
	{ CInstanceBase::FLY_HP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_smallest.msf" }, // »¡°£»ö ¸Å¿ì ÀÛÀº°Å
	{ CInstanceBase::FLY_SKILL_MUYEONG, CFlyingManager::INDEX_FLY_TYPE_AUTO_FIRE, "d:/ymir work/pc/sura/effect/muyeong_fly.msf" }, // ¹«¿µÁø
#if defined(ENABLE_QUIVER_SYSTEM)
	{ CInstanceBase::FLY_QUIVER_ATTACK_NORMAL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/pc/assassin/effect/arrow_02.msf" }, // ÀÏ¹Ý È°°ø°Ý ÀÌÆåÆ® 1
#endif
	//{ CInstanceBase::FLY_CONQUEROR_EXP, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_orange_small.msf" },
};

// Emotion
static const EmotionEffectDataVector g_vEmotionEffectData
{
	{ CInstanceBase::EFFECT_EMOTICON + 0, "", "d:/ymir work/effect/etc/emoticon/sweat.mse", false, "(È²´ç)" },
	{ CInstanceBase::EFFECT_EMOTICON + 1, "", "d:/ymir work/effect/etc/emoticon/money.mse", false, "(µ·)" },
	{ CInstanceBase::EFFECT_EMOTICON + 2, "", "d:/ymir work/effect/etc/emoticon/happy.mse", false, "(±â»Ý)" },
	{ CInstanceBase::EFFECT_EMOTICON + 3, "", "d:/ymir work/effect/etc/emoticon/love_s.mse", false, "(ÁÁ¾Æ)" },
	{ CInstanceBase::EFFECT_EMOTICON + 4, "", "d:/ymir work/effect/etc/emoticon/love_l.mse", false, "(»ç¶û)" },
	{ CInstanceBase::EFFECT_EMOTICON + 5, "", "d:/ymir work/effect/etc/emoticon/angry.mse", false, "(ºÐ³ë)" },
	{ CInstanceBase::EFFECT_EMOTICON + 6, "", "d:/ymir work/effect/etc/emoticon/aha.mse", false, "(¾ÆÇÏ)" },
	{ CInstanceBase::EFFECT_EMOTICON + 7, "", "d:/ymir work/effect/etc/emoticon/gloom.mse", false, "(¿ì¿ï)" },
	{ CInstanceBase::EFFECT_EMOTICON + 8, "", "d:/ymir work/effect/etc/emoticon/sorry.mse", false, "(ÁË¼Û)" },
	{ CInstanceBase::EFFECT_EMOTICON + 9, "", "d:/ymir work/effect/etc/emoticon/!_mix_back.mse", false, "(!)" },
	{ CInstanceBase::EFFECT_EMOTICON + 10, "", "d:/ymir work/effect/etc/emoticon/question.mse", false, "(?)" },
	{ CInstanceBase::EFFECT_EMOTICON + 11, "", "d:/ymir work/effect/etc/emoticon/fish.mse", false, "(fish)" },

#if defined(ENABLE_EXPRESSING_EMOTION)
	{ CInstanceBase::EFFECT_EMOTICON + 12, "", "d:/ymir work/effect/etc/emoticon/charging.mse", false, "(charging)" },
	{ CInstanceBase::EFFECT_EMOTICON + 13, "", "d:/ymir work/effect/etc/emoticon/nosay.mse", false, "(nosay)" },
	{ CInstanceBase::EFFECT_EMOTICON + 14, "", "d:/ymir work/effect/etc/emoticon/weather1.mse", false, "(weather1)" },
	{ CInstanceBase::EFFECT_EMOTICON + 15, "", "d:/ymir work/effect/etc/emoticon/weather2.mse", false, "(weather2)" },
	{ CInstanceBase::EFFECT_EMOTICON + 16, "", "d:/ymir work/effect/etc/emoticon/weather3.mse", false, "(weather3)" },
	{ CInstanceBase::EFFECT_EMOTICON + 17, "", "d:/ymir work/effect/etc/emoticon/hungry.mse", false, "(hungry)" },
	{ CInstanceBase::EFFECT_EMOTICON + 18, "", "d:/ymir work/effect/etc/emoticon/siren.mse", false, "(siren)" },
	{ CInstanceBase::EFFECT_EMOTICON + 19, "", "d:/ymir work/effect/etc/emoticon/letter.mse", false, "(letter)" },
	{ CInstanceBase::EFFECT_EMOTICON + 20, "", "d:/ymir work/effect/etc/emoticon/call.mse", false, "(call)" },
	{ CInstanceBase::EFFECT_EMOTICON + 21, "", "d:/ymir work/effect/etc/emoticon/celebration.mse", false, "(celebration)" },
	{ CInstanceBase::EFFECT_EMOTICON + 22, "", "d:/ymir work/effect/etc/emoticon/alcohol.mse", false, "(alcohol)" },
	{ CInstanceBase::EFFECT_EMOTICON + 23, "", "d:/ymir work/effect/etc/emoticon/busy.mse", false, "(busy)" },
	{ CInstanceBase::EFFECT_EMOTICON + 24, "", "d:/ymir work/effect/etc/emoticon/whirl.mse", false, "(whirl)" },
#endif
};

// Race
static RaceDataVector g_vRaceData
{
	{ CRaceData::RACE_WARRIOR_M, "warrior_m.msm", "d:/ymir work/pc/warrior/" },
	{ CRaceData::RACE_WARRIOR_W, "warrior_w.msm", "d:/ymir work/pc2/warrior/" },
	{ CRaceData::RACE_ASSASSIN_W, "assassin_w.msm", "d:/ymir work/pc/assassin/" },
	{ CRaceData::RACE_ASSASSIN_M, "assassin_m.msm", "d:/ymir work/pc2/assassin/" },
	{ CRaceData::RACE_SURA_M , "sura_m.msm", "d:/ymir work/pc/sura/" },
	{ CRaceData::RACE_SURA_W, "sura_w.msm", "d:/ymir work/pc2/sura/" },
	{ CRaceData::RACE_SHAMAN_W, "shaman_w.msm", "d:/ymir work/pc/shaman/" },
	{ CRaceData::RACE_SHAMAN_M, "shaman_m.msm", "d:/ymir work/pc2/shaman/" },
#ifdef ENABLE_WOLFMAN_CHARACTER
	{ CRaceData::RACE_WOLFMAN_M, "wolfman_m.msm", "d:/ymir work/pc3/wolfman/" },
#endif
};

// Intro Motions
static const MotionDataVector g_vIntroMotionData
{
	{ CRaceMotionData::NAME_INTRO_WAIT, "wait.msa", 0 },
	{ CRaceMotionData::NAME_INTRO_SELECTED, "selected.msa", 0 },
	{ CRaceMotionData::NAME_INTRO_NOT_SELECTED, "not_selected.msa", 0 },
};

// General Motion Data
static const MotionDataVector g_vGeneralMotionData
{
	{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
	{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
	{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
	{ CRaceMotionData::NAME_DAMAGE, "damage.msa", 50 },
	{ CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50 },
	{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50 },
	{ CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50 },
	{ CRaceMotionData::NAME_DAMAGE_FLYING, "damage_flying.msa", 0 },
	{ CRaceMotionData::NAME_STAND_UP, "falling_stand.msa", 0 },
	{ CRaceMotionData::NAME_DAMAGE_FLYING_BACK, "back_damage_flying.msa", 0 },
	{ CRaceMotionData::NAME_STAND_UP_BACK, "back_falling_stand.msa", 0 },
	{ CRaceMotionData::NAME_DEAD, "dead.msa", 0 },
	{ CRaceMotionData::NAME_DIG, "dig.msa", 0 },
};

// New General Motion Data (Used by Wolfman)
static const MotionDataVector g_vNewGeneralMotionData
{
	{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
	{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
	{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
	{ CRaceMotionData::NAME_DAMAGE, "front_damage.msa", 50 },
	{ CRaceMotionData::NAME_DAMAGE, "front_damage1.msa", 50 },
	{ CRaceMotionData::NAME_DAMAGE_BACK, "back_damage.msa", 50 },
	{ CRaceMotionData::NAME_DAMAGE_BACK, "back_damage1.msa", 50 },
	{ CRaceMotionData::NAME_DAMAGE_FLYING, "front_damage_flying.msa", 0 },
	{ CRaceMotionData::NAME_STAND_UP, "front_falling_standup.msa", 0 },
	{ CRaceMotionData::NAME_DAMAGE_FLYING_BACK, "back_damage_flying.msa", 0 },
	{ CRaceMotionData::NAME_STAND_UP_BACK, "back_falling_standup.msa", 0 },
	{ CRaceMotionData::NAME_DEAD, "dead.msa", 0 },
	{ CRaceMotionData::NAME_DIG, "dig.msa", 0 },
};

// Action Motion Data
static const MotionDataVector g_vActionMotionData
{
	{ CRaceMotionData::NAME_CLAP, "clap.msa", 0 },
	{ CRaceMotionData::NAME_CHEERS_1, "cheers_1.msa", 0 },
	{ CRaceMotionData::NAME_CHEERS_2, "cheers_2.msa", 0 },
	{ CRaceMotionData::NAME_DANCE_1, "dance_1.msa", 0 },
	{ CRaceMotionData::NAME_DANCE_2, "dance_2.msa", 0 },
	{ CRaceMotionData::NAME_DANCE_3, "dance_3.msa", 0 },
	{ CRaceMotionData::NAME_DANCE_4, "dance_4.msa", 0 },
	{ CRaceMotionData::NAME_DANCE_5, "dance_5.msa", 0 },
	{ CRaceMotionData::NAME_DANCE_6, "dance_6.msa", 0 },
	{ CRaceMotionData::NAME_CONGRATULATION, "congratulation.msa", 0 },
	{ CRaceMotionData::NAME_FORGIVE, "forgive.msa", 0 },
	{ CRaceMotionData::NAME_ANGRY, "angry.msa", 0 },
	{ CRaceMotionData::NAME_ATTRACTIVE, "attractive.msa", 0 },
	{ CRaceMotionData::NAME_SAD, "sad.msa", 0 },
	{ CRaceMotionData::NAME_SHY, "shy.msa", 0 },
	{ CRaceMotionData::NAME_CHEERUP, "cheerup.msa", 0 },
	{ CRaceMotionData::NAME_BANTER, "banter.msa", 0 },
	{ CRaceMotionData::NAME_JOY, "joy.msa", 0 },
	{ CRaceMotionData::NAME_FRENCH_KISS_WITH_WARRIOR, "french_kiss_with_warrior.msa", 0 },
	{ CRaceMotionData::NAME_FRENCH_KISS_WITH_ASSASSIN, "french_kiss_with_assassin.msa", 0 },
	{ CRaceMotionData::NAME_FRENCH_KISS_WITH_SURA, "french_kiss_with_sura.msa", 0 },
	{ CRaceMotionData::NAME_FRENCH_KISS_WITH_SHAMAN, "french_kiss_with_shaman.msa", 0 },
#if defined(ENABLE_WOLFMAN_CHARACTER)
	{ CRaceMotionData::NAME_FRENCH_KISS_WITH_WOLFMAN, "french_kiss_with_wolfman.msa", 0 },
#endif
	{ CRaceMotionData::NAME_KISS_WITH_WARRIOR, "kiss_with_warrior.msa", 0 },
	{ CRaceMotionData::NAME_KISS_WITH_ASSASSIN, "kiss_with_assassin.msa", 0 },
	{ CRaceMotionData::NAME_KISS_WITH_SURA, "kiss_with_sura.msa", 0 },
	{ CRaceMotionData::NAME_KISS_WITH_SHAMAN, "kiss_with_shaman.msa", 0 },
#if defined(ENABLE_WOLFMAN_CHARACTER)
	{ CRaceMotionData::NAME_KISS_WITH_WOLFMAN, "kiss_with_wolfman.msa", 0 },
#endif
	{ CRaceMotionData::NAME_SLAP_HIT_WITH_WARRIOR, "slap_hit.msa", 0 },
	{ CRaceMotionData::NAME_SLAP_HIT_WITH_ASSASSIN, "slap_hit.msa", 0 },
	{ CRaceMotionData::NAME_SLAP_HIT_WITH_SURA, "slap_hit.msa", 0 },
	{ CRaceMotionData::NAME_SLAP_HIT_WITH_SHAMAN, "slap_hit.msa", 0 },
#if defined(ENABLE_WOLFMAN_CHARACTER)
	{ CRaceMotionData::NAME_SLAP_HIT_WITH_WOLFMAN, "slap_hit.msa", 0 },
#endif
	{ CRaceMotionData::NAME_SLAP_HURT_WITH_WARRIOR, "slap_hurt.msa", 0 },
	{ CRaceMotionData::NAME_SLAP_HURT_WITH_ASSASSIN, "slap_hurt.msa", 0 },
	{ CRaceMotionData::NAME_SLAP_HURT_WITH_SURA, "slap_hurt.msa", 0 },
	{ CRaceMotionData::NAME_SLAP_HURT_WITH_SHAMAN, "slap_hurt.msa", 0 },
#if defined(ENABLE_WOLFMAN_CHARACTER)
	{ CRaceMotionData::NAME_SLAP_HURT_WITH_WOLFMAN, "slap_hurt.msa", 0 },
#endif
#if defined(ENABLE_EXPRESSING_EMOTION)
	{ CRaceMotionData::NAME_PUSH_UP, "pushup.msa", 0 },
	{ CRaceMotionData::NAME_DANCE_7, "dance_7.msa", 0 },
	{ CRaceMotionData::NAME_EXERCISE, "exercise.msa", 0 },
	{ CRaceMotionData::NAME_DOZE, "doze.msa", 0 },
	{ CRaceMotionData::NAME_SELFIE, "selfie.msa", 0 },
#endif
};

// Wedding Motions
static const MotionDataVector g_vWeddingMotionData
{
	{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
	{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
	{ CRaceMotionData::NAME_RUN, "walk.msa", 0 },
};

// Fishing Motions
static const MotionDataVector g_vFishingMotionData
{
	{ CRaceMotionData::NAME_WAIT, "wait.msa", 0 },
	{ CRaceMotionData::NAME_WALK, "walk.msa", 0 },
	{ CRaceMotionData::NAME_RUN, "run.msa", 0 },
	{ CRaceMotionData::NAME_FISHING_THROW, "throw.msa", 0 },
	{ CRaceMotionData::NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
	{ CRaceMotionData::NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
	{ CRaceMotionData::NAME_FISHING_REACT, "fishing_react.msa", 0 },
	{ CRaceMotionData::NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
	{ CRaceMotionData::NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
};

// Guild Skill Motions
static const MotionDataVector g_vGuildSkillMotionData
{
	{ GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa", 0 },
	{ GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa", 0 },
	{ GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa", 0 },
	{ GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa", 0 },
	{ GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa", 0 },
	{ GUILD_SKILL_MAGICUP, "guild_jumunsul.msa", 0 },
};

// Player Skills
static const SkillIndexVectorMap g_vMapPlayerSkillIndex
{
	{ NRaceData::JOB_WARRIOR, {
		// { 1, { 1, 2, 3, 4, 5, 6, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0, }},
		// { 2, { 16, 17, 18, 19, 20, 21, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0,}},
		{ 1, { 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0, }},
		{ 2, { 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0,}},
	}},
	{ NRaceData::JOB_ASSASSIN, {
		// { 1, { 31, 32, 33, 34, 35, 36, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0,}},
		// { 2, { 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0,}},
		{ 1, { 31, 32, 33, 34, 35, 0, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0,}},
		{ 2, { 46, 47, 48, 49, 50, 0, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0,}},
	}},
	{ NRaceData::JOB_SURA, {
		{ 1, { 61, 62, 63, 64, 65, 66, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0, }},
		{ 2, { 76, 77, 78, 79, 80, 81, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0, }},
	}},
	{ NRaceData::JOB_SHAMAN, {
		{ 1, { 91, 92, 93, 94, 95, 96, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0, }},
		{ 2, { 106, 107, 108, 109, 110, 111, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0,}},
	}},
#if defined(ENABLE_WOLFMAN_CHARACTER)
	{ NRaceData::JOB_WOLFMAN, {
		{ 1, { 170, 171, 172, 173, 174, 175, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0,}},
		{ 2, { 170, 171, 172, 173, 174, 175, 0, 0, 0, 0, 137, 0, 138, 0, 139, 0, 0, 0, 0, 0, 164, 165, 166, 167, 0, 0, 0, 0, 0, 0,}},
		}},
#endif
};

// Support Skills
// @ PythonPlayerModule.cpp
static const SupportSkillIndexVector g_vSupportSkillIndex
{
	{ 121, 122, 123, 124, 126, 127, 129, 128, 131, 137, 138, 139, 140, },
};

// Guild Skills
static const GuildSkillIndexVectorMap g_vMapGuildSkillIndex
{
	{ "PASSIVE", { 151, }},
	{ "ACTIVE", { 152, 153, 154, 155, 156, 157, }},
};

// Dungeon Map Name
static const DungeonMapNameVector g_vDungeonMapName
{
	{ "metin2_map_spiderdungeon" },
	{ "metin2_map_monkeydungeon" },
	{ "metin2_map_monkeydungeon_02" },
	{ "metin2_map_monkeydungeon_03" },
	{ "metin2_map_deviltower1" },
};

// Title Name
static const TitleNameVector g_vTitleName
{
	"PVP_LEVEL0",
	"PVP_LEVEL1",
	"PVP_LEVEL2",
	"PVP_LEVEL3",
	"PVP_LEVEL4",
	"PVP_LEVEL5",
	"PVP_LEVEL6",
	"PVP_LEVEL7",
	"PVP_LEVEL8"
};

#if defined(ENABLE_GUILD_LEADER_GRADE_NAME)
static const TitleNameVector g_vGuildLeaderGradeTitleName
{
	"GUILD_LEADER_GRADE0",
	"GUILD_LEADER_GRADE1"
};
#endif

// Name Color
static const NameColorMap g_mNameColor
{
	{ CInstanceBase::NAMECOLOR_NORMAL_PC, { 255, 215, 76 }},
	{ CInstanceBase::NAMECOLOR_NORMAL_NPC, { 122, 231, 93 }},
	{ CInstanceBase::NAMECOLOR_NORMAL_MOB, { 235, 22, 9 }},
	{ CInstanceBase::NAMECOLOR_PVP, { 238, 54, 223 }},
	{ CInstanceBase::NAMECOLOR_PK, { 180, 100, 0 }},
	{ CInstanceBase::NAMECOLOR_PARTY, { 128, 192, 255 }},
	{ CInstanceBase::NAMECOLOR_WARP, { 136, 218, 241 }},
	{ CInstanceBase::NAMECOLOR_WAYPOINT, { 255, 255, 255 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_MOB, { 235, 22, 9 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_NPC, { 122, 231, 93 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_PC + 1, { 157, 0, 0 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_PC + 2, { 222, 160, 47 }},
	{ CInstanceBase::NAMECOLOR_EMPIRE_PC + 3, { 23, 30, 138 }},
#ifdef ENABLE_ATLASS_EXTENDED
	{ CInstanceBase::NAMECOLOR_BOSS, { 255, 255, 255 }},
	{ CInstanceBase::NAMECOLOR_STONES, { 3, 144, 252 }},
#endif
};

// Title Color
static const TitleColorMap g_mTitleNameColor
{
	{ CInstanceBase::TITLE_RGB_GOOD_4, { 0, 204, 255 }},
	{ CInstanceBase::TITLE_RGB_GOOD_3, { 0, 144, 255 }},
	{ CInstanceBase::TITLE_RGB_GOOD_2, { 92, 110, 255 }},
	{ CInstanceBase::TITLE_RGB_GOOD_1, { 155, 155, 255 }},
	{ CInstanceBase::TITLE_RGB_NORMAL, { 255, 255, 255 }},
	{ CInstanceBase::TITLE_RGB_EVIL_1, { 207, 117, 0 }},
	{ CInstanceBase::TITLE_RGB_EVIL_2, { 235, 83, 0 }},
	{ CInstanceBase::TITLE_RGB_EVIL_3, { 227, 0, 0 }},
	{ CInstanceBase::TITLE_RGB_EVIL_4, { 255, 0, 0 }},
};

// Emotion Icon
static const EmotionIconVector g_vEmotionIcon
{
	{ EMOTION_CLAP, "d:/ymir work/ui/game/windows/emotion_clap.sub" },
	{ EMOTION_CHEERS_1, "d:/ymir work/ui/game/windows/emotion_cheers_1.sub" },
	{ EMOTION_CHEERS_2, "d:/ymir work/ui/game/windows/emotion_cheers_2.sub" },
	{ EMOTION_DANCE_1, "icon/action/dance1.tga" },
	{ EMOTION_DANCE_2, "icon/action/dance2.tga" },
	{ EMOTION_CONGRATULATION, "icon/action/congratulation.tga" },
	{ EMOTION_FORGIVE, "icon/action/forgive.tga" },
	{ EMOTION_ANGRY, "icon/action/angry.tga" },
	{ EMOTION_ATTRACTIVE, "icon/action/attractive.tga" },
	{ EMOTION_SAD, "icon/action/sad.tga" },
	{ EMOTION_SHY, "icon/action/shy.tga" },
	{ EMOTION_CHEERUP, "icon/action/cheerup.tga" },
	{ EMOTION_BANTER, "icon/action/banter.tga" },
	{ EMOTION_JOY, "icon/action/joy.tga" },
	{ EMOTION_DANCE_1, "icon/action/dance1.tga" },
	{ EMOTION_DANCE_2, "icon/action/dance2.tga" },
	{ EMOTION_DANCE_3, "icon/action/dance3.tga" },
	{ EMOTION_DANCE_4, "icon/action/dance4.tga" },
	{ EMOTION_DANCE_5, "icon/action/dance5.tga" },
	{ EMOTION_DANCE_6, "icon/action/dance6.tga" },
	{ EMOTION_KISS, "d:/ymir work/ui/game/windows/emotion_kiss.sub" },
	{ EMOTION_FRENCH_KISS, "d:/ymir work/ui/game/windows/emotion_french_kiss.sub" },
	{ EMOTION_SLAP, "d:/ymir work/ui/game/windows/emotion_slap.sub" },
#if defined(ENABLE_EXPRESSING_EMOTION)
	{ EMOTION_PUSH_UP, "icon/action/pushup.tga" },
	{ EMOTION_DANCE_7, "icon/action/dance7.tga" },
	{ EMOTION_EXERCISE, "icon/action/exercise.tga" },
	{ EMOTION_DOZE, "icon/action/doze.tga" },
	{ EMOTION_SELFIE, "icon/action/selfie.tga" },
	{ EMOTION_CHARGING, "icon/action/charging.tga" },
	{ EMOTION_NOSAY, "icon/action/nosay.tga" },
	{ EMOTION_WEATHER_1, "icon/action/weather1.tga" },
	{ EMOTION_WEATHER_2, "icon/action/weather2.tga" },
	{ EMOTION_WEATHER_3, "icon/action/weather3.tga" },
	{ EMOTION_HUNGRY, "icon/action/hungry.tga" },
	{ EMOTION_SIREN, "icon/action/siren.tga" },
	{ EMOTION_LETTER, "icon/action/letter.tga" },
	{ EMOTION_CALL, "icon/action/call.tga" },
	{ EMOTION_CELEBRATION, "icon/action/celebration.tga" },
	{ EMOTION_ALCOHOL, "icon/action/alcohol.tga" },
	{ EMOTION_BUSY, "icon/action/busy.tga" },
	{ EMOTION_WHIRL, "icon/action/whirl.tga" }
#endif
};

// Guild Building
static constexpr BYTE g_bGuildMaterialMaxNum = 3;
static const GuildBuildingMap g_mGuildBuilding
{
	{ "HEADQUARTER", "headquarter" },
	{ "FACILITY", "facility" },
	{ "OBJECT", "object" },
	{ "WALL", "fence" },
};

static const short GetGuildMaterialIndex(const DWORD c_dwVNum)
{
	switch (c_dwVNum)
	{
	case MATERIAL_STONE_ID: // ÃÊ¼®
	case MATERIAL_LOG_ID: // Åë³ª¹«
	case MATERIAL_PLYWOOD_ID: // ÇÕÆÇ
		return c_dwVNum - MATERIAL_STONE_ID;
	}
	return -1;
}

const char* GetLocaleInfoString(const std::string& c_rstToken)
{
	PyObject* ppyModule = PyImport_AddModule("localeInfo");
	PyObject* ppyObject = PyObject_GetAttrString(ppyModule, c_rstToken.c_str());

	const char* szText = PyString_AsString(ppyObject);
	if ((szText == nullptr) || (szText[0] == '\0'))
		return _strdup("NoName");

	return szText;
}

bool IsNumber(const std::string& c_rString)
{
	return !c_rString.empty() && std::find_if(c_rString.begin(), c_rString.end(),
		[](unsigned char c)
		{
			return !std::isdigit(c);
		}) == c_rString.end();
}
#endif // ENABLE_LOADING_PERFORMANCE
