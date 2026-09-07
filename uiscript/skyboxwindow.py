import uiScriptLocale

ROOT = "d:/ymir work/ui/public/"

SKYBOX_COUNT = 13

WINDOW_WIDTH = 180
WINDOW_HEIGHT = 31 * SKYBOX_COUNT + 35

window = {
	"name" : "SkyBoxWindow",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_finalcore",

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,

			"children" :
			(
				## Title
				{
					"name" : "titlebar",
					"type" : "titlebar_finalcore_small",
					"style" : ("attach",),

					"x" : 7,
					"y" : -17,

					"width" : 140,
					"color" : "gray",

					"children" :
					(
						{ 
							"name":"titlename", "type":"text", "x":0, "y":30, 
							"horizontal_align":"center", "text_horizontal_align":"center",
							"text" : uiScriptLocale.SKYBOX_WINDOW_TITLE,
						},
					),
				},
				## Buttons
				{
					"name" : "sky_button_0",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33,

					"text" : uiScriptLocale.SKYBOX_NORMAL,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_1",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30,

					"text" : uiScriptLocale.SKYBOX_NIGHT,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_2",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*2,

					"text" : uiScriptLocale.SKYBOX_SKY_I,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_3",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*3,

					"text" : uiScriptLocale.SKYBOX_SKY_II,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_4",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*4,

					"text" : uiScriptLocale.SKYBOX_SKY_III, 

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_5",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*5,

					"text" : uiScriptLocale.SKYBOX_SKY_IV,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_6",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*6,

					"text" : uiScriptLocale.SKYBOX_SKY_V,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_7",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*7,

					"text" : uiScriptLocale.SKYBOX_SKY_VI,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_8",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*8,

					"text" : uiScriptLocale.SKYBOX_SKY_VII,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_9",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*9,

					"text" : uiScriptLocale.SKYBOX_SKY_VIII,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_10",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*10,

					"text" : uiScriptLocale.SKYBOX_SKY_IX,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_11",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*11,

					"text" : uiScriptLocale.SKYBOX_SKY_X,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
				{
					"name" : "sky_button_12",
					"type" : "radio_button",

					"x" : 45,
					"y" : 33 + 30*12,

					"text" : uiScriptLocale.SKYBOX_SKY_XI,

					"default_image" : ROOT + "Large_Button_01.sub",
					"over_image" : ROOT + "Large_Button_02.sub",
					"down_image" : ROOT + "Large_Button_03.sub",
				},
			),
		},
	),
}
