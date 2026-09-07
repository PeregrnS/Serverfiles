import uiScriptLocale
import localeInfo

RESET_INFO_BOARD_WIDTH = 250
RESET_INFO_BOARD_HEIGHT = 70

Y_SPACER = 4

RANKING_PATH = "d:/ymir work/ui/game/player_ranking/"

window = {
	"name" : "RankInfo",

	"x" : SCREEN_WIDTH - 175 - 650,
	"y" : SCREEN_HEIGHT - 37 - 575,

	"style" : ("movable", "float",),

	"width" : 400 + 205,
	"height" : 380 + 25 + RESET_INFO_BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 10,
			"y" : 7,
			
			"width" : 400 + 205,
			"height" : 380+ 25,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar_finalcore_large",
					"style" : ("attach",),

					"x" : 290,
					"y" : -20,

					"width" : 400 + 205 - 14,

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":30, "text":uiScriptLocale.PLAYER_RANKING_TITLE, "text_horizontal_align":"center" },
					),
				},

				{
					"name" : "boardyz",
					"type" : "image",
					"style" : ("attach",),
					
					"x" : 6+2, 
					"y" : 32,
					
					"image" : RANKING_PATH + "ranking_bg.png",
				},
				{
					"name" : "wndTextName",
					"type" : "text", 
					"x": 288+2, "y" : 61-Y_SPACER+2,
					
					"text" : uiScriptLocale.PLAYER_RANKING_NAME,
				},

				{
					"name" : "wndTextEmpire",
					"type" : "text", 
					"x": 405+2, "y" : 61-Y_SPACER+2,
					
					"text" : uiScriptLocale.PLAYER_RANKING_EMPIRE,
				},

				{
					"name" : "wndTextCount",
					"type" : "text", 
					"x": 510+2, "y" : 61-Y_SPACER+2,
					
					"text" : uiScriptLocale.PLAYER_RANKING_COUNT,
				},

			),
		},
		
	),
}