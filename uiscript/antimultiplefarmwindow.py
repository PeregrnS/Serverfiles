import uiScriptLocale

BOARD_WIDTH = 183
BOARD_HEIGHT = 181

window = {
	"name" : "AntiMultipleFarmWnd",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	[
		## BOARD
		{
			"name" : "board",
			"type" : "board_with_titlebar2",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"title" : uiScriptLocale.ANTI_MULTIPLE_FARM_WND_TITLE,
			"children" :
			[
				{
					"name" : "anti_farm_bg_layer",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 7,
					"y" : 32,
					
					"image" : "d:/ymir work/ui/anti_multiple_farm/anti_farm_bg_layer.tga",
					"children" :
					[
						{
							"name" : "base_text_info",
							"type" : "text",
							
							"x" : 0,
							"y" : 3,
							
							"text_horizontal_align" : "center",
							"horizontal_align" : "center",
							
							"text" : uiScriptLocale.ANTI_MULTIPLE_FARM_BASE_TEXT,
						},
					],
				},
				{
					"name" : "scrollbar",
					"type" : "new_scrollbar",

					"x" : 168,
					"y" : 40,
					"size" : 130,
				},
				{
					"name" : "view_window",
					"type" : "window",
					"style" : ("attach",),
					
					"width" : 168,
					"height" : 21,
					
					"x" : 0,
					"y" : 153,
					
					"children" :
					[
						{
							"name" : "edit_button",
							"type" : "button",

							"x" : 10,
							"y" : -5,

							"text" : "Edit",

							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
                    
							# "width" : 61,
							# "height" : 21,

							# "default_image" : "d:/ymir work/ui/anti_multiple_farm/edit_norm.tga",
							# "over_image" : "d:/ymir work/ui/anti_multiple_farm/edit_hover.tga",
							# "down_image" : "d:/ymir work/ui/anti_multiple_farm/edit_down.tga",
						},
						{
							"name" : "close_button",
							"type" : "button",

							"x" : 55,
							"y" : -5,

							"text" : "Close",

							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",

							# "width" : 41,
							# "height" : 21,

							# "default_image" : "d:/ymir work/ui/new_options/close_norm.tga",
							# "over_image" : "d:/ymir work/ui/new_options/close_hover.tga",
							# "down_image" : "d:/ymir work/ui/new_options/close_down.tga",
						},

						{
							"name" : "refresh_button",
							"type" : "button",

							"x" : 110,
							"y" : -5,

							"text" : "Refresh",

							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",

							# "width" : 41,
							# "height" : 21,

							# "default_image" : "d:/ymir work/ui/new_options/reload_norm.tga",
							# "over_image" : "d:/ymir work/ui/new_options/reload_hover.tga",
							# "down_image" : "d:/ymir work/ui/new_options/reload_down.tga",
						},
					],
				},
				{
					"name" : "edit_window",
					"type" : "window",
					"style" : ("attach",),
					
					"width" : 168,
					"height" : 21,
					
					"x" : 0,
					"y" : 153,
					
					"children" :
					[
						{
							"name" : "save_edit_button",
							"type" : "button",

							"x" : 15,
							"y" : 0,

							"text" : "Accept",

							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
							# "width" : 61,
							# "height" : 21,

							# "default_image" : "d:/ymir work/ui/new_options/ok_norm.tga",
							# "over_image" : "d:/ymir work/ui/new_options/ok_hover.tga",
							# "down_image" : "d:/ymir work/ui/new_options/ok_down.tga",
						},
						{
							"name" : "close_edit_button",
							"type" : "button",

							"x" : 60,
							"y" : 0,

							"text" : "Close",

							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
							# "width" : 41,
							# "height" : 21,

							# "default_image" : "d:/ymir work/ui/new_options/close_norm.tga",
							# "over_image" : "d:/ymir work/ui/new_options/close_hover.tga",
							# "down_image" : "d:/ymir work/ui/new_options/close_down.tga",
						},
					],
				},
			],
		},
	],
}
