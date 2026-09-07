import uiScriptLocale

BOARD_WIDTH = 650
BOARD_HEIGHT = 410

window = {
	"name" : "WorldBossPanel",
	"x" : (SCREEN_WIDTH - BOARD_WIDTH)/2,
	"y" : (SCREEN_HEIGHT - BOARD_HEIGHT)/2,
	"style" : ("movable", "float",),
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	"children" :
	(		
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"x" : 0,
			"y" : 0,
			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
			"title" : "World Bosses",
			"children" :
			(
				{
					"name" : "MainBorder",
					"type" : "border_a",
					"style" : ("attach",),
					"x" : 10,
					"y" : 30,
					"width" : 630,
					"height" : 300+70,
					"children" :
					(
						{
							"name" : "board2",
							"type" : "thinboard",
							"x" : 15,
							"y" : 5,
							"width" : 600,
							"height" : 60,
							"children" :
							(
								{ "name":"TextThin0", "type":"text", "x":30, "y":8,	"text":uiScriptLocale.WORLD_BOSS_TEXT_1, "outline":"1" },
								{ "name":"TextThin1", "type":"text", "x":30, "y":23,"text":uiScriptLocale.WORLD_BOSS_TEXT_2, "outline":"1" },
								{ "name":"TextThin2", "type":"text", "x":30, "y":38,"text":uiScriptLocale.WORLD_BOSS_TEXT_3, "outline":"1" },
							),							
						},	
						{
							"name" : "board3",
							"type" : "thinboard",
							"x" : 15,
							"y" : 250,
							"width" : 600,
							"height" : 75,
							"children" :
							(
								{ "name":"Boss1", "type":"text", "x":30,	"y":5,	"text":uiScriptLocale.WORLD_BOSS_TEXT_4, "outline":"1" },
								{ "name":"WorldBoss0",	"type":"text", "x":300, "y":5,	"text":"00:00:00", "outline":"1" },
								# { "name":"WorldBoss1",	"type":"text", "x":425, "y":5,	"text":"00:00:00", "outline":"1" },
								# { "name":"WorldBoss2",	"type":"text", "x":550, "y":5,	"text":"00:00:00", "outline":"1" },

								{ "name":"Boss2",		"type":"text", "x":30,	"y":22, "text":uiScriptLocale.WORLD_BOSS_TEXT_5, "outline":"1" },
								{ "name":"WorldBoss1",	"type":"text", "x":300,	"y":22, "text":"00:00:00", "outline":"1" },
								# { "name":"WorldBoss3",	"type":"text", "x":425,	"y":22, "text":"00:00:00", "outline":"1" },
								# { "name":"WorldBoss5",	"type":"text", "x":550,	"y":22, "text":"00:00:00", "outline":"1" },

								# { "name":"Avarius",		"type":"text", "x":30,	"y":39, "text":uiScriptLocale.WORLD_BOSS_TEXT_6, "outline":"1" },
								# { "name":"WorldBoss6",	"type":"text", "x":300,	"y":39,	"text":"00:00:00", "outline":"1" },
								# { "name":"WorldBoss7",	"type":"text", "x":425,	"y":39,	"text":"00:00:00", "outline":"1" },
								# { "name":"WorldBoss8",	"type":"text", "x":550,	"y":39,	"text":"00:00:00", "outline":"1" },

								# { "name":"Odegon",		"type":"text", "x":30,	"y":56, "text":uiScriptLocale.WORLD_BOSS_TEXT_7, "outline":"1" },
								# { "name":"WorldBoss9",	"type":"text", "x":300,	"y":56, "text":"00:00:00", "outline":"1" },
							),
						},
						{
							"name" : "board2",
							"type" : "thinboard",
							"x" : 15,
							"y" : 75,
							"width" : 600,
							"height" : 168,
							"children" :
							(			
								{
									"name" : "ItemSlotBG",
									"type" : "image",
									"x" : 19,
									"y" : 1,
									"image" : "interface/worldboss/boss_images.png",
								},
								{ "name":"Boss0",	"type":"text", "x":206,	"y":145, "text":"Boss1", "color": 0xFFFF0000, "outline":"1" },
								{ "name":"Boss1",	"type":"text", "x":357,	"y":145, "text":"Boss2", "color": 0xFFFF0000, "outline":"1" },	
								# { "name":"Boss2",	"type":"text", "x":357,	"y":145, "text":"AVARIUS",	"color": 0xFFFF0000, "outline":"1" },	
								# { "name":"Boss3",	"type":"text", "x":505,	"y":145, "text":"ODEGON",	"color": 0xFFFF0000, "outline":"1" },	
							),
						},
						{ "name":"Informations0", "type":"text", "x":30, "y":323, "text":"", "outline": 1 },
						{ "name":"Informations1", "type":"text", "x":30, "y":338, "text":"", "outline": 1 },
						{ "name":"Informations2", "type":"text", "x":30, "y":353, "text":"", "outline": 1 },
						{
							"name" : "teleportBtn",
							"type" : "button",
							"x" : 225,
							"y" : 335,
							"width" : 61,
							"height" : 21,
							"horizontal_align" : "center",
							"text" : uiScriptLocale.WORLD_BOSS_TEXT_8,
							"default_image" : "d:/ymir work/ui/game/myshop_deco/select_btn_01.sub",
							"over_image" : "d:/ymir work/ui/game/myshop_deco/select_btn_02.sub",
							"down_image" : "d:/ymir work/ui/game/myshop_deco/select_btn_03.sub",
						},
					),
				},
			),
		},
	),
}