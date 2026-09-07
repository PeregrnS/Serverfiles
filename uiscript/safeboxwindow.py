import uiScriptLocale

window = {
	"name" : "SafeboxWindow",

	"x" : 100,
	"y" : 20,

	"style" : ("movable", "float",),

	"width" : 176,
	"height" : 250,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_finalcore",

			"x" : 0,
			"y" : 0,

			"width" : 176,
			"height" : 250,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar_finalcore_small",
					"style" : ("attach",),

					"x" : 5,
					"y" : -15,

					"width" : 161,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":77, "y":30, "text":uiScriptLocale.SAFE_TITLE, "text_horizontal_align":"center" },
					),
				},

				## Button
				{
					"name" : "ChangePasswordButton",
					"type" : "button",

					"x" : 0,
					"y" : 58,

					"text" : uiScriptLocale.SAFE_CHANGE_PASSWORD,
					"horizontal_align" : "center",
					"vertical_align" : "bottom",

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name" : "ExitButton",
					"type" : "button",

					"x" : 0,
					"y" : 37,

					"text" : uiScriptLocale.CLOSE,
					"horizontal_align" : "center",
					"vertical_align" : "bottom",

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},

			),
		},
	),
}
