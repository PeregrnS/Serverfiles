import uiScriptLocale

UI_WIDTH	= 420
UI_HEIGHT	= 235

window = {
	"name" : "SkillGroupWindow",
	"style" : ("movable", "float",),
	"x" : SCREEN_WIDTH / 2 - UI_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - UI_HEIGHT / 2,
	"width" : UI_WIDTH,
	"height" : UI_HEIGHT,
	"children" :
	(
		{
			"name" : "SkillGroupTitle",
			"type" : "text",
			"x" : 220,
			"y" : 5,
			"text" : uiScriptLocale.SKILL_GROUP_TITLE,
			"color" : 0xffffcc00,
			"fontsize" : "BOLD",
		},
		{
			"name" : "GroupBackGround",
			"type" : "image",

			"x" : 0,
			"y" : 0,

			"image" : "d:/ymir work/ui/game/skillgroup/0.tga",
		},
		{ 
			"name":"TextGroup1",
			"type":"text",
			
			"x": 195,
			"y": 50,
			
			"text" : "Group 1",
			"fontsize" : "BOLD",
			"text_horizontal_align":"center"
		},
		{ 
			"name":"TextGroup2",
			"type":"text",
			
			"x": 195,
			"y": 140,
			
			"text" : "Group 2",
			"fontsize" : "BOLD",
			"text_horizontal_align":"center"
		},
		{
			"name" : "SelectGroup1",
			"type" : "button",
			"x" : 180,
			"y" : 70,
			"text" : "",
			"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
			"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
			"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
		},
		{
			"name" : "SelectGroup2",
			"type" : "button",
			"x" : 170,
			"y" : 160,
			"text" : "",
			"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
			"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
			"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
		},
		{
			"name" : "CloseButton",
			"type" : "button",
			"x" : 180,
			"y" : 212,
			"text" : uiScriptLocale.SKILL_GROUP_LATER,
			"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
		},
	),
}