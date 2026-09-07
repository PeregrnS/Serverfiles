import app
import uiScriptLocale

ROOT = "d:/ymir work/ui/minimap/"

INTERFACE_PATH = "locale/common/ui/interface/finalcore3/minimap/"

window = {
	"name" : "MiniMap",

	"x" : SCREEN_WIDTH - 136,
	"y" : 0,

	"width" : 136,
	"height" : 137,

	"children" :
	[
		## OpenWindow
		{
			"name" : "OpenWindow",
			"type" : "window",

			"x" : 0,
			"y" : 0,

			"width" : 136,
			"height" : 137,

			"children" :
			[
				{
					"name" : "OpenWindowBGI",
					"type" : "image",
					"x" : -30,
					"y" : -0,
					"image" : INTERFACE_PATH + "minimap.tga",
				},
				## MiniMapWindow
				{
					"name" : "MiniMapWindow",
					"type" : "window",

					"x" : 4,
					"y" : 5,

					"width" : 128,
					"height" : 128,
				},
				## ScaleUpButton
				{
					"name" : "ScaleUpButton",
					"type" : "button",

					"x" : 103,
					"y" : 116,

					"default_image" : INTERFACE_PATH + "minimap_btn_plus_01.tga",
					"over_image" : INTERFACE_PATH + "minimap_btn_plus_02.tga",
					"down_image" : INTERFACE_PATH + "minimap_btn_plus_03.tga",
				},
				## ScaleDownButton
				{
					"name" : "ScaleDownButton",
					"type" : "button",

					"x" : 117,
					"y" : 103,

					"default_image" : INTERFACE_PATH + "minimap_btn_minus_01.tga",
					"over_image" : INTERFACE_PATH + "minimap_btn_minus_02.tga",
					"down_image" : INTERFACE_PATH + "minimap_btn_minus_03.tga",
				},
				## MiniMapHideButton
				{
					"name" : "MiniMapHideButton",
					"type" : "button",

					"x" : 113,
					"y" : 6,

					"default_image" : INTERFACE_PATH + "minimap_btn_close_01.tga",
					"over_image" : INTERFACE_PATH + "minimap_btn_close_02.tga",
					"down_image" : INTERFACE_PATH + "minimap_btn_close_03.tga",
				},
				## AtlasShowButton
				{
					"name" : "AtlasShowButton",
					"type" : "button",

					"x" : 14,
					"y" : 12,

					"default_image" : INTERFACE_PATH + "minimap_btn_map_01.tga",
					"over_image" : INTERFACE_PATH + "minimap_btn_map_02.tga",
					"down_image" : INTERFACE_PATH + "minimap_btn_map_03.tga",
				},
				## ServerInfo
				{
					"name" : "ServerInfo",
					"type" : "text",

					"text_horizontal_align" : "center",

					"outline" : 1,

					"x" : 70,
					"y" : 150,

					"text" : "",
				},
				## Datetime
				{
					"name" : "Datetime",
					"type" : "text",

					"text_horizontal_align" : "center",

					"outline" : 1,

					"x" : 70,
					"y" : 165,

					"text" : "",
				},
				## PositionInfo
				{
					"name" : "PositionInfo",
					"type" : "text",

					"text_horizontal_align" : "center",

					"outline" : 1,

					"x" : 70,
					"y" : 180,

					"text" : "",
				},
				## ObserverCount
				{
					"name" : "ObserverCount",
					"type" : "text",

					"text_horizontal_align" : "center",

					"outline" : 1,

					"x" : 70,
					"y" : 180,

					"text" : "",
				},
			],
		},
		{
			"name" : "CloseWindow",
			"type" : "window",

			"x" : 0,
			"y" : 0,

			"width" : 132,
			"height" : 48,

			"children" :
			[
				## ShowButton
				{
					"name" : "MiniMapShowButton",
					"type" : "button",

					"x" : 100,
					"y" : 4,

					"default_image" : ROOT + "minimap_open_default.sub",
					"over_image" : ROOT + "minimap_open_default.sub",
					"down_image" : ROOT + "minimap_open_default.sub",
				},
			],
		},
	],
}

# if app.ENABLE_EVENT_MANAGER:
# 	window["children"][0]["children"] = window["children"][0]["children"] + [
# 		{
# 			"name" : "InGameEventButton",
# 			"type" : "button",

# 			"x" : 0,
# 			"y" : 35,

# 			# "tooltip_text" : uiScriptLocale.EVENT_ALARM_TITLE,
# 			"default_image" : "d:/ymir work/ui/minimap/e_open_default.tga",
# 			"over_image" : "d:/ymir work/ui/minimap/e_open_over.tga",
# 			"down_image" : "d:/ymir work/ui/minimap/e_open_down.tga",
# 		},
# 	]

# if app.ENABLE_BIOLOG_SYSTEM:
# 	window["children"][0]["children"] = window["children"][0]["children"] + [
# 		{
# 			"name" : "biologButton",
# 			"type" : "button",

# 			"x" : 13 + 1,
# 			"y" : 42 + 21 * 3,
			
# 			"default_image" : ROOT + "biolog_button01.tga",
# 			"over_image" : ROOT + "biolog_button02.tga",
# 			"down_image" : ROOT + "biolog_button01.tga",
			
			
# 			"tooltip_text" : uiScriptLocale.BIOLOG_WINDOW_MINIMAP_TOOLTIP,
# 		},
# 	]

# if app.ENABLE_INGAME_WIKI:
# 	window["children"][0]["children"] = window["children"][0]["children"] + [
# 		{
# 			"name" : "WikipediaButton",
# 			"type" : "button",

# 			"x" : 13 + 21,
# 			"y" : 42 + 21 * 3 + 13,

# 			"default_image" : ROOT + "wiki_open_default.tga",
# 			"over_image" : ROOT + "wiki_open_over.tga",
# 			"down_image" : ROOT + "wiki_open_down.tga",

# 			"tooltip_text" : uiScriptLocale.WIKI_TITLE,
# 		},
# 	]

# if app.ENABLE_DUNGEON_INFO_SYSTEM:
# 	window["children"][0]["children"] = window["children"][0]["children"] + [
# 		{
# 			"name" : "DungeonInfoShowButton",
# 			"type" : "button",

# 			"x" : 42,
# 			"y" : 120,

# 			"default_image" : "d:/ymir work/ui/game/dungeon_info/minimap/minimap_dungeon_info_default.tga",
# 			"over_image" : "d:/ymir work/ui/game/dungeon_info/minimap/minimap_dungeon_info_over.tga",
# 			"down_image" : "d:/ymir work/ui/game/dungeon_info/minimap/minimap_dungeon_info_down.tga",
# 		},
# 	]

# if app.ENABLE_BATTLE_PASS:
# 	window["children"][0]["children"] = window["children"][0]["children"] + [
# 		{
# 			"name" : "BattlePass",
# 			"type" : "button",

# 			"x" : 101 + 15,
# 			"y" : 116 - 38,

# 			"default_image" : ROOT + "battle_pass_default.tga",
# 			"over_image" : ROOT + "battle_pass_over.tga",
# 			"down_image" : ROOT + "battle_pass_down.tga",
# 		},
# 	]
