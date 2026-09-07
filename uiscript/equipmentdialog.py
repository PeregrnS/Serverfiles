import uiScriptLocale
import localeInfo
import app

window = {
	"name" : "EquipmentDialog",
	"style" : ["movable", "float",],

	"x" : 0,
	"y" : 0,

	"width" : 180,
	"height" : 230,

	"children" :
	[
		{
			"name" : "Board",
			"type" : "titlebar_finalcore_large10",

			"x" : 0,
			"y" : 0,

			"width" : 180,
			"height" : 230,

			"title" : "Character Name",

			"children" :
			[
				{
					"name":"EquipmentBaseImage",
					"type":"image",
					"style" : ["attach",],

					"x" : 0,
					"y" : 9,
					"horizontal_align" : "center",
					"vertical_align" : "center",

					"image" : "d:/ymir work/ui/equipment_bg_without_ring_old01.tga",

					"children" :
					[

						{
							"name" : "EquipmentSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 150,
							"height" : 182,

							## Get the index numbers from length.h
							"slot" : [
										{"index":0, "x":39, "y":37, "width":32, "height":64},
										{"index":1, "x":39, "y":2, "width":32, "height":32},
										{"index":2, "x":39, "y":145, "width":32, "height":32},
										{"index":3, "x":75, "y":67, "width":32, "height":32},
										{"index":4, "x":3, "y":3, "width":32, "height":96},
										{"index":5, "x":114, "y":67, "width":32, "height":32},
										{"index":6, "x":114, "y":35, "width":32, "height":32},
										{"index":7, "x":2, "y":145, "width":32, "height":32},
										{"index":8, "x":75, "y":145, "width":32, "height":32},
										{"index":9, "x":114, "y":2, "width":32, "height":32},
										{"index":10, "x":75, "y":35, "width":32, "height":32},
										{"index":27, "x":39, "y":106, "width":32, "height":32}, ## Belt
									],
						},
						## CostumeButton
						{
							"name" : "CostumeButton",
							"type" : "button",

							"x" : 78,
							"y" : 5,

							"tooltip_text" : uiScriptLocale.COSTUME_TITLE,

							"default_image" : "d:/ymir work/ui/game/taskbar/costume_Button_01.png",
							"over_image" : "d:/ymir work/ui/game/taskbar/costume_Button_02.png",
							"down_image" : "d:/ymir work/ui/game/taskbar/costume_Button_03.png",
						},
					],
				},
			],
		},
	],
}

if app.ENABLE_PENDANT_SYSTEM: 
	window["children"][0]["children"][0]["children"][0]["slot"] += [
		{"index":28, "x":3, "y":106, "width":32, "height":32},]
if app.ENABLE_GLOVE_SYSTEM:
	window["children"][0]["children"][0]["children"][0]["slot"] += [
		{"index":29, "x":75, "y":106, "width":32, "height":32},]
if app.ENABLE_COSTUME_PET:
	window["children"][0]["children"][0]["children"][0]["slot"] += [
		{"index":30, "x":114, "y":106, "width":32, "height":32},]
