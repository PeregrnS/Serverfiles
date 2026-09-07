import uiScriptLocale

ROOT = "d:/ymir work/ui/game/taskbar/"
INTERFACE = "d:/ymir work/ui/ingame/"

window = {
	"name" : "RightButtonWindow",

	"x" : 0,
	"y" : 0,

	"width" : 32 * 2,
	"height" : 32,

	"children" :
	(
		{
			"name" : "button_move_and_attack",
			"type" : "button",

			"x" : 0,
			"y" : 0,

			"tooltip_text" : uiScriptLocale.MOUSEBUTTON_ATTACK,
			"tooltip_x" : -40,
			"tooltip_y" : 9,

			"default_image" : INTERFACE + "attack_0.png",
			"over_image" : INTERFACE + "attack_1.png",
			"down_image" : INTERFACE + "attack_2.png",
		},
		{
			"name" : "button_camera",
			"type" : "button",

			"x" : 32,
			"y" : 0,

			"tooltip_text" : uiScriptLocale.MOUSEBUTTON_CAMERA,
			"tooltip_x" : -40,
			"tooltip_y" : 9,

			"default_image" : INTERFACE + "camera_0.png",
			"over_image" : INTERFACE + "camera_1.png",
			"down_image" : INTERFACE + "camera_2.png",
		},
		{
			"name" : "button_skill",
			"type" : "button",

			"x" : 64,
			"y" : 0,

			"tooltip_text" : uiScriptLocale.MOUSEBUTTON_SKILL,
			"tooltip_x" : -40,
			"tooltip_y" : 9,

			"default_image" : ROOT + "Mouse_Button_Skill_01.sub",
			"over_image" : ROOT + "Mouse_Button_Skill_02.sub",
			"down_image" : ROOT + "Mouse_Button_Skill_03.sub",
		},
	),
}