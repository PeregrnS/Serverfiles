import uiScriptLocale
import localeInfo

LOCALE_PATH = "d:/ymir work/ui/bossbilgi/"

MAINBOARD_WIDTH = 700
MAINBOARD_HEIGHT = 380
MAINBOARD_X = (SCREEN_WIDTH/2) - (MAINBOARD_WIDTH/2)
MAINBOARD_Y = (SCREEN_HEIGHT/2) - (MAINBOARD_HEIGHT/2)


RIGHTBOARD_WIDTH = 196
RIGHTBOARD_HEIGHT = 299+12+10
RIGHTBOARD_X = 192
RIGHTBOARD_Y = 36
RENDERBOARD_WIDTH = 180
RENDERBOARD_HEIGHT = 305

window = {
	"name" : "BossTimeWindow",
	"style" : ("movable", "float",),
	
	"x" : MAINBOARD_X,
	"y" : MAINBOARD_Y,	

	"width" : MAINBOARD_WIDTH,
	"height" : MAINBOARD_HEIGHT,
	
	"children" :
	(
		## MainBoard
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach", ),
			
			"x" : 0,
			"y" : 0,
			
			"width" : MAINBOARD_WIDTH,
			"height" : MAINBOARD_HEIGHT,
			
			"title" : uiScriptLocale.BOSS_TRACKING_INFO_WINDOW_TITLE,

			"name" : "BossTimeBoard",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : MAINBOARD_WIDTH,
			"height" : MAINBOARD_HEIGHT,
			"children" :
			(
				## Title Bar
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6, "y" : 7, "width" : MAINBOARD_WIDTH - 13,
					
					"children" :
					(
						{ "name" : "TitleName", "type" : "text", "x" : 0, "y" : -2, "text": "Map Boss Tracking", "all_align":"center" },
					),
				},
				
				{
					"name" : "ButtonBoard",
					"type" : "image",
					"x" : 7, "y" : 30, "width" : 170, "height" : RIGHTBOARD_HEIGHT,
					"image" : LOCALE_PATH +"background.png",
				},
				

				## Right Board
				{
					"name" : "RightBoard",
					"type" : "thinboard_circle",
					"x" : RIGHTBOARD_X-27, "y" : RIGHTBOARD_Y+30, "width" : RIGHTBOARD_WIDTH+105, "height" : RIGHTBOARD_HEIGHT-255,

					"children" : 
					(
						## Title Bar
						{
							"name" : "areaInfo",
							"type" : "thinboard_gold",
							
							"x" : 7, "y" : 3, "width" : RIGHTBOARD_WIDTH+92, "height" : 60,
							"children" :
							(
								{ "name" : "textInfo", "type" : "text", "x" : 10, "y" : 7, "text" : "Position: ", "align":"left" },
								{ "name" : "areaText", "type" : "text", "x" : 95, "y" : 7, "text" : " ", "align":"left" },
								{ "name" : "textInfo", "type" : "text", "x" : 10, "y" : 20, "text" : "Safe Zone: ", "align":"left" },
								{ "name" : "secText", "type" : "text", "x" : 85, "y" : 20, "text" : " ", "align":"left" },
								{ "name" : "textInfo", "type" : "text", "x" : 10, "y" : 33, "text" : "CH: ", "align":"left" },
								{ "name" : "chText", "type" : "text", "x" : 105, "y" : 33, "text" : " ", "align":"left" },
								{ "name" : "bosstext", "type" : "text", "x" : 0, "y" : -30, "text" : localeInfo.BOSSTEXT, "align":"left" },
							),
						},
					),
				}, ## Right Board End


				{
					"name" : "board_render",
					"type" : "thinboard_circle",
					"style" : ("attach",),

					"x" : RIGHTBOARD_X + 288,
					"y" : RIGHTBOARD_Y+38,

					"width" : RIGHTBOARD_WIDTH-3,
					"height" : RIGHTBOARD_HEIGHT-50,
					
					"children" :
					(
						{
							"name" : "RenderBoss",
							"type" : "render_target",
		
							"x" : 8,
							"y" : 3,
		
							"width" : RENDERBOARD_WIDTH-3,
							"height" : RENDERBOARD_HEIGHT-50,
							"index" : "2",
						},
					),
				}, # renderboard end

			),
		}, ## MainBoard End
	),
}