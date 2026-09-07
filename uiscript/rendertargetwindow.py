import uiScriptLocale

WINDOW_WIDTH = 410
WINDOW_HEIGHT = 440

BUTTON_ROTATION_WIDTH = 32

window = {
	"name" : "RenderTargetWindow",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children":
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar_finalcore_large",
					"style" : ("attach",),

					"x" : 65 + BUTTON_ROTATION_WIDTH,
					"y" : -18,

					"width" : WINDOW_WIDTH - 15 - BUTTON_ROTATION_WIDTH,
					"color" : "yellow",

					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",

							"x" : (WINDOW_WIDTH - 220) / 2,
							"y" : 30,

							"text" : uiScriptLocale.GUILD_BUILDING_PREVIEW,
							"text_horizontal_align":"center"
						},
					),
				},
				{
					"name" : "RenderRotationButton",
					"type" : "button",

					"x" : 20,
					"y" : 10,

					"default_image" : "d:/ymir work/ui/game/monster_card/button/mv_reset/mv_reset_button_default.sub",
					"over_image" : "d:/ymir work/ui/game/monster_card/button/mv_reset/mv_reset_button_over.sub",
					"down_image" : "d:/ymir work/ui/game/monster_card/button/mv_reset/mv_reset_button_down.sub",
				},
				# {
					# "name" : "RotateImage",
					# "type" : "image",
					# "style" : ("attach",),
					
					# "x" : 5, 
					# "y" : 30,
					
					# "image" : "icon/emoji/key_lclick.tga",
				# },
				# {
					# "name" : "ZoomInImage",
					# "type" : "image",
					# "style" : ("attach",),
					
					# "x" : 5 + 16, 
					# "y" : 30,
					
					# "image" : "icon/emoji/key_rclick.tga",
				# },
			),
		},
	),
}