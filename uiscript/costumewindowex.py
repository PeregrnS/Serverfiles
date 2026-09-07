import app
import localeInfo
import uiScriptLocale
import item

BUTTON_X = -5
COSTUME_PAGE_X = 19

COSTUME_START_INDEX = item.COSTUME_SLOT_START

window = {
	"name" : "CostumeWindow",

	"x" : SCREEN_WIDTH - 175 - 140,
	"y" : SCREEN_HEIGHT - 37 - 565 - 40,

	"style" : ("movable", "float",),

	"width" : 180,
	"height" : 227,

	"children" :
	[
		{
			"name" : "wndCostumeInventoryLayer",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 180,
			"height" : 227,

			"children" :
			[
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar_finalcore_small",
					"style" : ("attach",),

					"x" : 10,
					"y" : -15,

					"width" : 180,
					"color" : "yellow",

					"children" :
					[
						{ "name":"Costume_TitleBar_01", "type":"text", "x": -20, "y":30, "text" : uiScriptLocale.COSTUME_WINDOW_TITLE, "horizontal_align" : "center", "text_horizontal_align" : "center" },
						{ "name":"Costume_TitleBar_02", "type":"text", "x": -20, "y":30, "text" : uiScriptLocale.SHINING_WINDOW_TITLE, "horizontal_align" : "center", "text_horizontal_align" : "center" },
						{ "name":"Costume_TitleBar_03", "type":"text", "x":-20, "y":30, "text" : "Booster Window", "horizontal_align" : "center", "text_horizontal_align" : "center" },
					],
				},

				## Costumes
				{
					"name" : "Costume_Page",
					"type" : "expanded_image",

					"x" : COSTUME_PAGE_X,
					"y" : 38,

					"image" : uiScriptLocale.LOCALE_UISCRIPT_PATH + "costume/new_costume_bg.jpg",

					"children" :
					[
						{
							"name" : "CostumeSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 127,
							"height" : 175,

							"slot" : [
								{"index":COSTUME_START_INDEX+0, "x":62, "y":45, "width":32, "height":64},
								{"index":COSTUME_START_INDEX+1, "x":62, "y": 9, "width":32, "height":32},
							],
						},
					],
				},

				## Costumes
				{
					"name" : "Costume_Tab_01",
 					"type" : "radio_button",

 					"x" : BUTTON_X,
 					"y" : 38,

 					"default_image" : "d:/ymir work/ui/game/costume/eqtabclosed.dds",
 					"over_image" : "d:/ymir work/ui/game/costume/eqtabclosed.dds",
 					"down_image" : "d:/ymir work/ui/game/costume/eqtabopen.dds",

 					"tooltip_text" : uiScriptLocale.COSTUME_WINDOW_TITLE,

 					"children" :
					[
						{
							"name" : "Costume_Tab_01_Print",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : "I",
						},
					],
				},
			],
		},
	],
}

if app.ENABLE_MOUNT_SYSTEM:
	window["children"][0]["children"][1]["children"][0]["slot"] += [
		{"index":item.WEAR_MOUNT, "x":5, "y":126, "width":32, "height":32},] ## default x = 13
if app.ENABLE_WEAPON_COSTUME_SYSTEM:
	window["children"][0]["children"][1]["children"][0]["slot"] += [
		{"index":COSTUME_START_INDEX+4, "x":13, "y":13, "width":32, "height":96},]
if app.ENABLE_ACCE_COSTUME_SYSTEM:
	window["children"][0]["children"][1]["children"][0]["slot"] += [
		{"index":COSTUME_START_INDEX+2, "x":62, "y":126, "width":32, "height":32},] ## default x = 62

if app.ENABLE_SHINING_ITEM_SYSTEM:
	window["children"][0]["children"] = window["children"][0]["children"] + [
	## Shinings
	{
		"name" : "Shining_Page",
		"type" : "expanded_image",

		"x" : COSTUME_PAGE_X,
		"y" : 38,

		"image" : uiScriptLocale.LOCALE_UISCRIPT_PATH + "costume/new_costume_bg.jpg",

		"children" :
		[

			{
				"name" : "ShiningSlot",
				"type" : "slot",

				"x" : 3,
				"y" : 3,

				"width" : 127,
				"height" : 175,

				"slot" : [
					{"index":item.SHINING_SLOT_WEAPON1, "x":16, "y":79, "width":32, "height":32},
					{"index":item.SHINING_SLOT_WEAPON2, "x":16, "y":47, "width":32, "height":32},
					{"index":item.SHINING_SLOT_WEAPON3, "x":16, "y":15, "width":32, "height":32},
					{"index":item.SHINING_SLOT_BODY1, "x":65, "y":79, "width":32, "height":32},
					{"index":item.SHINING_SLOT_BODY2, "x":65, "y":47, "width":32, "height":32},
					{"index":item.SHINING_SLOT_SPECIAL, "x":65, "y":12, "width":32, "height":32},
				],
			},
		],
	},

	{
		"name" : "Costume_Tab_02",
		"type" : "radio_button",

		"x" : BUTTON_X,
		"y" : 38 + 22,

		"default_image" : "d:/ymir work/ui/game/costume/eqtabclosed.dds",
		"over_image" : "d:/ymir work/ui/game/costume/eqtabclosed.dds",
		"down_image" : "d:/ymir work/ui/game/costume/eqtabopen.dds",

		"tooltip_text" : uiScriptLocale.SHINING_WINDOW_TITLE,

		"children" :
		[
			{
				"name" : "Costume_Tab_02_Print",
				"type" : "text",
				"x" : 0,
				"y" : 0,
				"all_align" : "center",
				"text" : "II",
			},
		],
	},]

if app.ENABLE_BOOSTER_ITEMS:
	window["children"][0]["children"] = window["children"][0]["children"] + [
	## Shinings
	{
		"name" : "Booster_Page",
		"type" : "expanded_image",

		"x" : COSTUME_PAGE_X,
		"y" : 38,

		"image" : uiScriptLocale.LOCALE_UISCRIPT_PATH + "costume/eq_effect_page_v2.jpg",

		"children" :
		[

			{
				"name" : "BoosterSlot",
				"type" : "slot",

				"x" : 3,
				"y" : 3,

				"width" : 127,
				"height" : 175,

				"slot" : [
					{"index":item.BOOSTER_SLOT_WEAPON, "x":17, "y":93, "width":32, "height":32},
					{"index":item.BOOSTER_SLOT_BODY, "x":36, "y":43, "width":32, "height":32},
					{"index":item.BOOSTER_SLOT_HAIR, "x":58, "y":93, "width":32, "height":32},
				],
			},
		],
	},

	{
		"name" : "Costume_Tab_03",
		"type" : "radio_button",

		"x" : BUTTON_X,
		"y" : 38 + 22 + 22,

		"default_image" : "d:/ymir work/ui/game/costume/eqtabclosed.dds",
		"over_image" : "d:/ymir work/ui/game/costume/eqtabclosed.dds",
		"down_image" : "d:/ymir work/ui/game/costume/eqtabopen.dds",

		"tooltip_text" : "Booster Window",

		"children" :
		[
			{
				"name" : "Costume_Tab_03_Print",
				"type" : "text",
				"x" : 0,
				"y" : 0,
				"all_align" : "center",
				"text" : "III",
			},
		],
	},]
