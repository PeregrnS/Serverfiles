import uiScriptLocale

window = {
	"name" : "GameWindow",
	"style" : ("not_pick",),

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	(
		{
			"name":"HelpButton",
			"type":"button",
			"x" : 20,
			"y" : SCREEN_HEIGHT-210,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" :
			(
				{
					"name":"HelpButtonLabel",
					"type":"text",
					"x": 16,
					"y": 40,
					"text":uiScriptLocale.GAME_HELP,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_horizontal_align":"center"
				},
			),
		},
		{
			"name":"QuestButton",
			"type":"button",
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" :
			(
				{
					"name":"QuestButtonLabel",
					"type":"text",
					"x": 16,
					"y": 40,
					"text":uiScriptLocale.GAME_QUEST,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_horizontal_align":"center"
				},
			),
		},
		{
			"name":"StatusPlusButton",
			"type" : "button",
			"x" : SCREEN_WIDTH - 1550,
			"y" : SCREEN_HEIGHT-100, 
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" :
			(
				{
					"name":"StatusPlusLabel",
					"type":"text",
					"x": 16,
					"y": 40,
					"text":uiScriptLocale.GAME_STAT_UP,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_horizontal_align":"center"
				},
			),
		},
		{
			"name":"SkillPlusButton",
			"type" : "button",
			"x" : SCREEN_WIDTH-1480,
			"y" : SCREEN_HEIGHT-100,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" :
			(
				{
					"name":"SkillPlusLabel",
					"type":"text",
					"x": 16,
					"y": 40,
					"text":uiScriptLocale.GAME_SKILL_UP,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_horizontal_align":"center"
				},
			),
		},
		{
			"name":"ExitObserver",
			"type" : "button",
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" :
			(
				{
					"name":"ExitObserverButtonName",
					"type":"text",
					"x": 16,
					"y": 40,
					"text": uiScriptLocale.GAME_EXIT_OBSERVER,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_horizontal_align":"center"
				},
			),
		},
		{
			"name":"BuildGuildBuilding",
			"type" : "button",
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" :
			(
				{
					"name":"BuildGuildBuildingButtonName",
					"type":"text",
					"x": 16,
					"y": 40,
					"text": uiScriptLocale.GUILD_BUILDING_TITLE,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_horizontal_align":"center"
				},
			),
		},
        ## if app.ENABLE_SAVE_BLOCK_ATTR:
		{ 
			"name":"SaveBlockAttr", 
			"type" : "button", 
			"x" : 50,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",
			"children" : 
			(
				{ 
					"name":"SaveBlockAttrName", 
					"type":"text", 
					"x": 16, 
					"y": 40, 
					"text": "Unstuck", 
					"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
					"text_horizontal_align":"center" 
				},	
			),
		},
        ## if app.ENABLE_VOTE_4_BUFF:
		{
			"name":"Vote4Buff",
			"type" : "button",
			"x" : 10,
			"y" : SCREEN_HEIGHT-140,
			"default_image" : "d:/ymir work/ui/game/vote4buff/vote4buff_0.png",
			"over_image" : "d:/ymir work/ui/game/vote4buff/vote4buff_1.png",
			"down_image" : "d:/ymir work/ui/game/vote4buff/vote4buff_2.png",
		},
	),
}
