import localeInfo
import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/windows/"
INTERFACE = "d:/ymir work/ui/character/"

SMALL_VALUE_FILE = INTERFACE + "small_value_file.png"
MIDDLE_VALUE_FILE = INTERFACE + "middle_value_file.png"
LARGE_VALUE_FILE = INTERFACE + "large_value_file.png"
LARGE_STATS_VALUE_FILE = INTERFACE + "large_stat_value_file.png"
ICON_SLOT_FILE = INTERFACE + "slot.png"
QUEST_ICON_BACKGROUND = 'd:/ymir work/ui/game/quest/slot_base.sub'


window = {
	"name" : "CharacterWindow",
	"style" : ("movable", "float",),

	"x" : 24,
	"y" : (SCREEN_HEIGHT - 37 - 381) / 2,

	"width" : 330,
	"height" : 441,

	"children" :
	(
		{
			"name" : "board",
			"type" : "expanded_image",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,
			"image" : INTERFACE+"board.png",

			"children" :
			[

				## Title Area
				{
					"name" : "Character_TitleBar",
					"type" : "image",
					"style" : ("attach",),
					"x" : 10,
					"y" : 10,
					"image" : INTERFACE+"titlebar.png",
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":-1, "text":uiScriptLocale.CHARACTER_MAIN, "all_align":"center" },
					),
				},
				{
					"name" : "Skill_TitleBar",
					"type" : "image",
					"style" : ("attach",),
					"x" : 10,
					"y" : 10,
					"image" : INTERFACE+"titlebar.png",
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":-1, "text":uiScriptLocale.CHARACTER_SKILL, "all_align":"center" },
					),
				},
				{
					"name" : "Emoticon_TitleBar",
					"type" : "image",
					"style" : ("attach",),
					"x" : 10,
					"y" : 10,
					"image" : INTERFACE+"titlebar.png",
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":-1, "text":uiScriptLocale.CHARACTER_ACTION, "all_align":"center" },
					),
				},
				{
					"name" : "Quest_TitleBar",
					"type" : "image",
					"style" : ("attach",),
					"x" : 10,
					"y" : 10,
					"image" : INTERFACE+"titlebar.png",
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":-1, "text":uiScriptLocale.CHARACTER_QUEST, "all_align":"center" },
					),
				},

				{
					"name" : "background",
					"type" : "image",

					"x" : 0,
					"y" : 32,
					"horizontal_align":"center",
					"image" : INTERFACE+"background.png",
				},
				## Tab Area
				{
					"name" : "TabControl",
					"type" : "window",

					"x" : 0,
					"y" : 395,

					"width" : 330,
					"height" : 34,

					"children" :
					(
						{
							"name" : "TabBackground",
							"type" : "image",
							"x" : 0,
							"y" : -13,
							"horizontal_align":"center",
							"image" : INTERFACE+"tab_bg.png",
						},
						## RadioButton
						{
							"name" : "Tab_Button_01",
							"type" : "radio_button",

							"x" : 23,
							"y" : -6,

							"default_image" : INTERFACE+"btn_tab_0.png",
							"over_image" : INTERFACE+"btn_tab_0.png",
							"down_image" : INTERFACE+"btn_tab_0.png",
							"text":uiScriptLocale.CHARACTER_MAIN,
						},
						{
							"name" : "Tab_Button_02",
							"type" : "radio_button",

							"x" : 23 + 72,
							"y" : -6,

							"default_image" : INTERFACE+"btn_tab_0.png",
							"over_image" : INTERFACE+"btn_tab_0.png",
							"down_image" : INTERFACE+"btn_tab_0.png",
							"text":uiScriptLocale.CHARACTER_SKILL,
						},
						{
							"name" : "Tab_Button_03",
							"type" : "radio_button",

							"x" : 23 + 72*2,
							"y" : -6,

							"default_image" : INTERFACE+"btn_tab_0.png",
							"over_image" : INTERFACE+"btn_tab_0.png",
							"down_image" : INTERFACE+"btn_tab_0.png",
							"text":uiScriptLocale.CHARACTER_ACTION,
						},
						{
							"name" : "Tab_Button_04",
							"type" : "radio_button",

							"x" : 23 + 72*3,
							"y" : -6,

							"default_image" : INTERFACE+"btn_tab_0.png",
							"over_image" : INTERFACE+"btn_tab_0.png",
							"down_image" : INTERFACE+"btn_tab_0.png",
							"text":uiScriptLocale.CHARACTER_QUEST,
						},
					),
				},
				 # Page Area
				{
					"name" : "Character_Page",
					"type" : "window",
					"style" : ("attach",),
				
					"x" : 0,
					"y" : 0,
				
					"width" : 330,
					"height" : 380,
				
					"children" :
					(
						## Guild Name Slot
						{
							"name" : "Guild_Name_Slot",
							"type" : "image",
							"x" : 198,
							"y" : 62,
							"image" : INTERFACE + "large_value_file.png",
				
							"children" :
							(
								{
									"name" : "Guild_Info",
									"type":"text",
									"text": uiScriptLocale.CHARACTERWINDOW_GUILD,
									"x":0,
									"y":-20,
									"all_align" : "center",
								},
								{
									"name" : "Guild_Name",
									"type":"text",
									"text":"Guild Name",
									"x":0,
									"y":0,
									"r":1.0, "g":1.0, "b":1.0, "a":1.0,
									"all_align" : "center",
								},
							),
						},
						## Character Name Slot
						{
							"name" : "Character_Name_Slot",
							"type" : "image",
							"x" : 85,
							"y" : 62,
							"image" : INTERFACE + "large_value_file.png",
				
							"children" :
							(
								{
									"name" : "Character_Info",
									"type":"text",
									"text": uiScriptLocale.CHARACTERWINDOW_NAME,
									"x":0,
									"y":-20,
									"all_align" : "center",
								},
								{
									"name" : "Character_Name",
									"type":"text",
									"text":"Player Name",
									"x":0,
									"y":0,
									"r":1.0, "g":1.0, "b":1.0, "a":1.0,
									"all_align" : "center",
								},
							),
						},
						{
							"name":"Status_Lv", "type":"window", "x":170, "y":62, "width":37, "height":42,
							"children" :
							(
								{ "name":"Level_Header", "type":"text", "x":16, "y":-13, "fontsize":"SMALL", "text":"Lv.", "r":1.0, "g":0.0, "b":0.0, "a":1.0, "text_horizontal_align":"center" },
								{ "name":"Level_Value", "type":"text", "x":31, "y":-13, "fontsize":"SMALL", "text":"999", "r":1.0, "g":0.0, "b":0.0, "a":1.0, "text_horizontal_align":"center" },
							),
						},
						## Face Slot
						
						{ "name" : "Face_Slot", "type" : "image", "x" : 22, "y" : 42, "image" : INTERFACE + "face_box.png", "children" : ({ "name" : "Face_Image", "type" : "image", "x" : 7, "y" : 8, "image" : INTERFACE + "face/2.png" },)},
						{
							"name" : "Status_Plus_Label",
							"type" : "image",
							"x" : 0,
							"y" : 105,
							"horizontal_align" : "center",
							"image" : INTERFACE+"horizontal_bar.png",
							"children" : (
								{ "name":"Status_text", "type":"text", "x": 10, "y": 4, "text": uiScriptLocale.CHARACTERWINDOW_STATISTIC, },
								{ "name":"Status__av_text", "type":"text", "x": 30, "y": 4, "text": uiScriptLocale.CHARACTERWINDOW_AVAILABLE, "horizontal_align":"right", "text_horizontal_align":"right"},
								{ "name":"Status_Plus_Value", "type":"text", "x": 10, "y":4, "text":"99", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "horizontal_align":"right", "text_horizontal_align":"right" },
							),
						},
						{
							"name":"Status_Standard", "type":"window", "x":30, "y": 110, "width":330, "height":250, "horizontal_align" : "center",
							"children" :
							[
				
								## 기본 능력치
								{
									"name":"base_info", "type":"window", "x":0, "y":26, "width":200, "height":150,
									"children" :
									[
										## HTH
										{ "name":"HTH_Slot", "type":"image", "x":25, "y":0, "image":SMALL_VALUE_FILE },
										{ "name":"HTH_Value", "type":"text", "x":42, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"HTH_Plus", "type" : "button", "x":68, "y":5, "default_image" : INTERFACE+"btn_plus_up.png", "over_image" : INTERFACE+"btn_plus_over.png", "down_image" : INTERFACE+"btn_plus_down.png", },
				
										## INT
										{ "name":"INT_Slot", "type":"image", "x":25, "y":31, "image":SMALL_VALUE_FILE },
										{ "name":"INT_Value", "type":"text", "x":42, "y":34, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"INT_Plus", "type" : "button", "x" : 68, "y" : 35, "default_image" : INTERFACE+"btn_plus_up.png", "over_image" : INTERFACE+"btn_plus_over.png", "down_image" : INTERFACE+"btn_plus_down.png", },
				
										## STR
										{ "name":"STR_Slot", "type":"image", "x":25, "y":62, "image":SMALL_VALUE_FILE },
										{ "name":"STR_Value", "type":"text", "x":42, "y":65, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"STR_Plus", "type" : "button", "x" : 68, "y" : 66, "default_image" : INTERFACE+"btn_plus_up.png", "over_image" : INTERFACE+"btn_plus_over.png", "down_image" : INTERFACE+"btn_plus_down.png", },
				
										## DEX
										{ "name":"DEX_Slot", "type":"image", "x":25, "y":93, "image":SMALL_VALUE_FILE },
										{ "name":"DEX_Value", "type":"text", "x":42, "y":96, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"DEX_Plus", "type" : "button", "x" : 68, "y" : 97, "default_image" : INTERFACE+"btn_plus_up.png", "over_image" : INTERFACE+"btn_plus_over.png", "down_image" : INTERFACE+"btn_plus_down.png", },
				
										## 이미지들
										{ "name":"HTH_Text", "type":"text", "x":10, "y":3, "text":uiScriptLocale.CREATE_HP, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"INT_Text", "type":"text", "x":10, "y":33, "text":uiScriptLocale.CREATE_SP, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"STR_Text", "type":"text", "x":10, "y":64, "text":uiScriptLocale.CREATE_ATT_GRADE, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"DEX_Text", "type":"text", "x":10, "y":95, "text":uiScriptLocale.CREATE_DEX_GRADE, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
				
										{ "name":"HP_Text", "type":"text", "x":130, "y":2, "text":uiScriptLocale.CHARACTERWINDOW_HP, "r":1.0, "g":1.0, "b":1.0, "a":1.0, },
										{ "name":"SP_Text", "type":"text", "x":130, "y":33, "text":uiScriptLocale.CHARACTERWINDOW_SP, "r":1.0, "g":1.0, "b":1.0, "a":1.0, },
										{ "name":"ATT_Text", "type":"text", "x":130, "y":64, "text":uiScriptLocale.CHARACTERWINDOW_ATT, "r":1.0, "g":1.0, "b":1.0, "a":1.0, },
										{ "name":"DEF_Text", "type":"text", "x":130, "y":95, "text":uiScriptLocale.CHARACTERWINDOW_DEF, "r":1.0, "g":1.0, "b":1.0, "a":1.0,  },
				
				
									],
								},
				
								{ "name":"HTH_Minus", "type" : "button", "x":90, "y":36-5, "default_image" : INTERFACE+"btn_minus_up.png", "over_image" : INTERFACE+"btn_minus_over.png", "down_image" : INTERFACE+"btn_minus_down.png", },
								{ "name":"INT_Minus", "type" : "button", "x":90, "y":67-6, "default_image" : INTERFACE+"btn_minus_up.png", "over_image" : INTERFACE+"btn_minus_over.png", "down_image" : INTERFACE+"btn_minus_down.png", },
								{ "name":"STR_Minus", "type" : "button", "x":90, "y":98-6, "default_image" : INTERFACE+"btn_minus_up.png", "over_image" : INTERFACE+"btn_minus_over.png", "down_image" : INTERFACE+"btn_minus_down.png", },
								{ "name":"DEX_Minus", "type" : "button", "x":90, "y":129-6, "default_image" : INTERFACE+"btn_minus_up.png", "over_image" : INTERFACE+"btn_minus_over.png", "down_image" : INTERFACE+"btn_minus_down.png", },
				
								## HP
								{
									"name":"HEL_Label", "type":"window", "x":175, "y":33-7, "width":50, "height":20,
									"children" :
									[
										{ "name":"HP_Slot", "type":"image", "x":0, "y":0, "image":LARGE_STATS_VALUE_FILE },
										{ "name":"HP_Value", "type":"text", "x":45, "y":3, "text":"9999/9999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									]
								},
								## SP
								{
									"name":"SP_Label", "type":"window", "x":175, "y":64-7, "width":50, "height":20, 
									"children" :
									[
										{ "name":"SP_Slot", "type":"image", "x":0, "y":0, "image":LARGE_STATS_VALUE_FILE },
										{ "name":"SP_Value", "type":"text", "x":45, "y":3, "text":"9999/9999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									]
								},
								## ATT
								{
									"name":"ATT_Label", "type":"window", "x":175, "y":95-7, "width":50, "height":20, 
									"children" :
									[
										{ "name":"ATT_Slot", "type":"image", "x":0, "y":0, "image":LARGE_STATS_VALUE_FILE },
										{ "name":"ATT_Value", "type":"text", "x":45, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									]
								},
								## DEF
								{
									"name":"DEF_Label", "type":"window", "x":175, "y":126-7, "width":50, "height":20, 
									"children" :
									[
										{ "name":"DEF_Slot", "type":"image", "x":0, "y":0, "image":LARGE_STATS_VALUE_FILE },
										{ "name":"DEF_Value", "type":"text", "x":45, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
		 							]
								},
							],
						},
						{ "name":"Base_Infobg", "type":"image", "horizontal_align" : "center", "x": 0, "y": 255, "image": INTERFACE+"horizontal_bar.png",  "children" : ( { "name":"StatusTitle", "type":"text", "x":10, "y": 4, "text":uiScriptLocale.CHARACTER_ATTRIBUTES,},),},
						{
							"name":"Status_Extent", "type":"window", "x":8, "y":265, "width":330, "height":125,
							"children" :
							(
				
								{ "name":"MSPD_IMG", "type":"text", "x":20+30+10-9+10, "y":31 - 25+15-1, "text": uiScriptLocale.ATTRIBUTE_MOV1, "text_horizontal_align":"right",},
								{ "name":"MSPD_IMG", "type":"text", "x":20+30+10-9+10, "y":31 - 25+15+9, "text": uiScriptLocale.ATTRIBUTE_MOV2, "text_horizontal_align":"right",},
								
								{ "name":"ASPD_IMG", "type":"text", "x":20+30+10-9+10, "y":62 - 25+15-1, "text": uiScriptLocale.ATTRIBUTE_ATTACKSP, "text_horizontal_align":"right",},
								{ "name":"MSPD_IMG", "type":"text", "x":20+30+10-9+10, "y":62 - 25+15+9, "text": uiScriptLocale.ATTRIBUTE_ATTACKSP2, "text_horizontal_align":"right",},
								
								{ "name":"CSPD_IMG", "type":"text", "x":20+30+10-9+10, "y":93 - 25+15-1, "text": uiScriptLocale.ATTRIBUTE_ZS, "text_horizontal_align":"right",},
								{ "name":"MSPD_IMG", "type":"text", "x":20+30+10-9+10, "y":93 - 25+15+9, "text": uiScriptLocale.ATTRIBUTE_ZS2, "text_horizontal_align":"right",},
				
								{ "name":"MATT_IMG", "type":"text", "x":118+20+80, "y":31 - 25+15-1, "text": uiScriptLocale.ATTRIBUTE_MAW1, "text_horizontal_align":"right", },
								{ "name":"MSPD_IMG", "type":"text", "x":118+20+80, "y":31 - 25+9+15, "text": uiScriptLocale.ATTRIBUTE_MAW2, "text_horizontal_align":"right",},
								
								{ "name":"MDEF_IMG", "type":"text", "x":118+20+80, "y":62 - 25+15-1, "text": uiScriptLocale.ATTRIBUTE_MADEFF1, "text_horizontal_align":"right",},
								{ "name":"MSPD_IMG", "type":"text", "x":118+20+80, "y":62 - 25+9+15, "text": uiScriptLocale.ATTRIBUTE_MADEFF2, "text_horizontal_align":"right",},
								
								{ "name":"ER_IMG", "type":"text", "x":118+20+80, "y":93 - 25+15+4, "text": uiScriptLocale.ATTRIBUTE_FLEE, "text_horizontal_align":"right",},
				
								## MSPD - ?? ??
								{
									"name":"MOV_Label", "type":"window", "x":60+16, "y":33 - 25+15, "width":60, "height":20,
									"children" :
									(
										{ "name":"MSPD_Slot", "type":"image", "x":0, "y":0, "image": MIDDLE_VALUE_FILE },
										{ "name":"MSPD_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},
				
								## ASPD - ?? ??
								{
									"name":"ASPD_Label", "type":"window", "x":60+16, "y":54, "width":60, "height":20,
									"children" :
									(
										{ "name":"ASPD_Slot", "type":"image", "x":0, "y":0, "image": MIDDLE_VALUE_FILE },
										{ "name":"ASPD_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},
				
								## CSPD - ?? ??
								{
									"name":"CSPD_Label", "type":"window", "x":60+16, "y":95 - 25+15, "width":50, "height":20,
									"children" :
									(
										{ "name":"CSPD_Slot", "type":"image", "x":0, "y":0, "image": MIDDLE_VALUE_FILE },
										{ "name":"CSPD_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},
				
								## MATT - ?? ???
								{
									"name":"MATT_Label", "type":"window", "x":148+83, "y":33 - 25+15, "width":60, "height":20,
									"children" :
									(
										{ "name":"MATT_Slot", "type":"image", "x":0, "y":0, "image": MIDDLE_VALUE_FILE },
										{ "name":"MATT_Value", "type":"text", "x":26, "y":3, "text":"999-999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},
				
								## MDEF - ?? ???
								{
									"name":"MDEF_Label", "type":"window", "x":148+83, "y":64 - 25+15, "width":50, "height":20,
									"children" :
									(
										{ "name":"MDEF_Slot", "type":"image", "x":0, "y":0, "image": MIDDLE_VALUE_FILE  },
										{ "name":"MDEF_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},
				
								## ???
								{
									"name":"ER_Label", "type":"window", "x":148+83, "y":95 - 25+15, "width":50, "height":20,
									"children" :
									(
										{ "name":"ER_Slot", "type":"image", "x":0, "y":0, "image": MIDDLE_VALUE_FILE  },
										{ "name":"ER_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},
				
							),
						},
					),
				},
				{
					"name" : "Skill_Page",
					"type" : "window",
					"style" : ("attach",),
				
					"x" : 0,
					"y" : 24,
				
					"width" : 330,
					"height" : 340,
				
					"children" :
					(
						{
							"name" : "Skill_Active_Title_Bar",
							"type" : "image",
							"x" : 0,
							"y" : 20,
							"horizontal_align" : "center",
							"image" : INTERFACE+"horizontal_bar.png",
							"children" : (
								{ "name":"Active_Skill_text", "type":"text", "x": 10, "y": 4, "text": uiScriptLocale.CHARACTERWINDOW_STATISTIC, },
								{ "name":"Active_Skill__av_text", "type":"text", "x": 30, "y": 4, "text": uiScriptLocale.CHARACTERWINDOW_AVAILABLE, "horizontal_align":"right", "text_horizontal_align":"right"},
								{ "name":"Active_Skill_Point_Value", "type":"text", "x": 10, "y":4, "text":"99", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "horizontal_align":"right", "text_horizontal_align":"right" },
								## Group Button
								{
									"name" : "Skill_Group_Button_1",
									"type" : "radio_button",
				
									"x" : 5,
									"y" : 4,
				
									"text" : "Group1",
									"text_color" : 0xFFFFE3AD,
				
									"default_image" : INTERFACE + "skill_tab_button_01.png",
									"over_image" : INTERFACE + "skill_tab_button_02.png",
									"down_image" : INTERFACE + "skill_tab_button_03.png",
								},
				
								{
									"name" : "Skill_Group_Button_2",
									"type" : "radio_button",
				
									"x" : 50,
									"y" : 4,
									"text" : "Group2",
									"default_image" : INTERFACE + "skill_tab_button_01.png",
									"over_image" : INTERFACE + "skill_tab_button_02.png",
									"down_image" : INTERFACE + "skill_tab_button_03.png",
								},
							),
						},
						{ "name":"Skill_ETC_Title_Bar", "type":"image", "horizontal_align" : "center", "x": 0, "y": 215, "image": INTERFACE+"horizontal_bar.png","horizontal_align" : "center", "children" : ( { "name":"Support_Skill_Group_Name", "type":"text", "x":10, "y": 4, "text":uiScriptLocale.SKILL_SUPPORT_TITLE,},{ "name":"Support_Skill_Point_Value", "type":"text", "x":10, "y":4, "text":"99", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "horizontal_align":"right", "text_horizontal_align":"right" },),},
						{ "name":"Skill_ETC_Deco", "type":"image", "horizontal_align" : "center", "x": 0, "y": 52, "image": INTERFACE+"deco.png",},
				
						## Active Slot
						{
							"name" : "Skill_Active_Slot",
							"type" : "slot",
							"horizontal_align" : "center",
				
							"x" : 40,
							"y" : 55,
				
							"width" : 250,
							"height" : 300,
							"image" : ICON_SLOT_FILE,
				
							"slot" :	(
											{"index": 1, "x": 1, "y":  4, "width":32, "height":32},
											{"index":21, "x":38, "y":  4, "width":32, "height":32},
											{"index":41, "x":75, "y":  4, "width":32, "height":32},
				
											{"index": 3, "x": 1, "y": 40, "width":32, "height":32},
											{"index":23, "x":38, "y": 40, "width":32, "height":32},
											{"index":43, "x":75, "y": 40, "width":32, "height":32},
				
											{"index": 5, "x": 1, "y": 76, "width":32, "height":32},
											{"index":25, "x":38, "y": 76, "width":32, "height":32},
											{"index":45, "x":75, "y": 76, "width":32, "height":32},
				
											{"index": 7, "x": 1, "y":112, "width":32, "height":32},
											{"index":27, "x":38, "y":112, "width":32, "height":32},
											{"index":47, "x":75, "y":112, "width":32, "height":32},
				
											####
				
											{"index": 2, "x":113+30, "y":  4, "width":32, "height":32},
											{"index":22, "x":150+30, "y":  4, "width":32, "height":32},
											{"index":42, "x":187+30, "y":  4, "width":32, "height":32},
																# +30
											{"index": 4, "x":113+30, "y": 40, "width":32, "height":32},
											{"index":24, "x":150+30, "y": 40, "width":32, "height":32},
											{"index":44, "x":187+30, "y": 40, "width":32, "height":32},
																# +30
											{"index": 6, "x":113+30, "y": 76, "width":32, "height":32},
											{"index":26, "x":150+30, "y": 76, "width":32, "height":32},
											{"index":46, "x":187+30, "y": 76, "width":32, "height":32},
																# +30
											{"index": 8, "x":113+30, "y":112, "width":32, "height":32},
											{"index":28, "x":150+30, "y":112, "width":32, "height":32},
											{"index":48, "x":187+30, "y":112, "width":32, "height":32},
										),
						},
				
						## ETC Slot
						{
							"name" : "Skill_ETC_Slot",
							"type" : "grid_table",
							"x" : 55,
							"y" : 260,
							"start_index" : 101,
							"x_count" : 6,
							"y_count" : 2,
							"x_step" : 32,
							"y_step" : 32,
							"x_blank" : 5,
							"y_blank" : 4,
							"image" : ICON_SLOT_FILE,
						},
				
					),
				},
			],
		},
		{
			"name" : "close_button",
			"type" : "button",
			"x" : 298,
			"y" : 10,
			"default_image" : INTERFACE+"btn_close_up.png",
			"over_image" : INTERFACE+"btn_close_over.png",
			"down_image" : INTERFACE+"btn_close_down.png",
		},
	),
}

window["children"][0]["children"] = window["children"][0]["children"] + [
{
	"name" : "Quest_Page",
	"type" : "window",
	"style" : ("attach",),

	"x" : 10,
	"y" : 30,

	"width" : 330,
	"height" : 340,

	"children" :
	(
		{
			"name" : "Quest_Slot",
			"type" : "grid_table",
			"x" : 18,
			"y" : 20,
			"start_index" : 0,
			"x_count" : 1,
			"y_count" : 5,
			"x_step" : 32,
			"y_step" : 32,
			"y_blank" : 28,
			"image" : QUEST_ICON_BACKGROUND,
		},

		{
			"name" : "Quest_ScrollBar",
			"type" : "scrollbar",

			"x" : 45,
			"y" : 12,
			"size" : 325,
			"horizontal_align" : "right",
		},

		{ "name" : "Quest_Name_00", "type" : "text", "text" : "이름입니다", "x" : 60, "y" : 14 },
		{ "name" : "Quest_LastTime_00", "type" : "text", "text" : "남은 시간 입니다", "x" : 60, "y" : 30 },
		{ "name" : "Quest_LastCount_00", "type" : "text", "text" : "남은 개수 입니다", "x" : 60, "y" : 46 },

		{ "name" : "Quest_Name_01", "type" : "text", "text" : "이름입니다", "x" : 60, "y" : 74 },
		{ "name" : "Quest_LastTime_01", "type" : "text", "text" : "남은 시간 입니다", "x" : 60, "y" : 90 },
		{ "name" : "Quest_LastCount_01", "type" : "text", "text" : "남은 개수 입니다", "x" : 60, "y" : 106 },

		{ "name" : "Quest_Name_02", "type" : "text", "text" : "이름입니다", "x" : 60, "y" : 134 },
		{ "name" : "Quest_LastTime_02", "type" : "text", "text" : "남은 시간 입니다", "x" : 60, "y" : 150 },
		{ "name" : "Quest_LastCount_02", "type" : "text", "text" : "남은 개수 입니다", "x" : 60, "y" : 166 },

		{ "name" : "Quest_Name_03", "type" : "text", "text" : "이름입니다", "x" : 60, "y" : 194 },
		{ "name" : "Quest_LastTime_03", "type" : "text", "text" : "남은 시간 입니다", "x" : 60, "y" : 210 },
		{ "name" : "Quest_LastCount_03", "type" : "text", "text" : "남은 개수 입니다", "x" : 60, "y" : 226 },

		{ "name" : "Quest_Name_04", "type" : "text", "text" : "이름입니다", "x" : 60, "y" : 254 },
		{ "name" : "Quest_LastTime_04", "type" : "text", "text" : "남은 시간 입니다", "x" : 60, "y" : 270 },
		{ "name" : "Quest_LastCount_04", "type" : "text", "text" : "남은 개수 입니다", "x" : 60, "y" : 286 },
	),
},]

window["children"][0]["children"] = window["children"][0]["children"] + [
{
	"name" : "Emoticon_Page",
	"type" : "window",
	"style" : ("attach",),

	"x" : 0,
	"y" : 14,

	"width" : 330,
	"height" : 340,

	"children" :
	(
		{
			"name" : "Action_Bar",
			"type" : "image",
			"x" : 0,
			"y" : 30,
			"horizontal_align" : "center",
			"image" : INTERFACE+"horizontal_bar.png",
			"children" : (
				{ "name":"Action_Bar_Text", "type":"text", "x": 10, "y": 4, "text": uiScriptLocale.CHARACTER_NORMAL_ACTION, },
			),
		},

		{
			"name" : "SoloEmotionSlot",
			"type" : "grid_table",
			"x" : 35,
			"y" : 60,
			"horizontal_align" : "center",
			"start_index" : 1,
			"x_count" : 7,
			"y_count" : 6,
			"x_step" : 38,
			"y_step" : 38,
			"x_blank" : 0,
			"y_blank" : 0,
			"image" : ICON_SLOT_FILE,
		},

		{
			"name" : "Reaction_Bar",
			"type" : "image",
			"x" : 0,
			"y" : 290,
			"horizontal_align" : "center",
			"image" : INTERFACE+"horizontal_bar.png",
			"children" : (
				{ "name":"Reaction_Bar_Text", "type":"text", "x": 10, "y": 4, "text": uiScriptLocale.CHARACTER_MUTUAL_ACTION, },
			),
		},

		{
			"name" : "DualEmotionSlot",
			"type" : "grid_table",
			"x" : 35,
			"y" : 323,
			"start_index" : 51,
			"x_count" : 7,
			"y_count" : 1,
			"x_step" : 38,
			"y_step" : 38,
			"x_blank" : 0,
			"y_blank" : 0,
			"image" : ICON_SLOT_FILE,
		},
	),
},]
