import uiScriptLocale

window = {
	"name" : "QuestionDialog",

	"x" : SCREEN_WIDTH/2 - 12500,
	"y" : SCREEN_HEIGHT/2 - 5200,

	"width" : 280,
	"height" : 75,

	"children" :
	(
		# FINALCORE TITLEBAR
		{
			"name" : "TitleBar",
			"type" : "titlebar_finalcore_middle_noclose",
			"x" : 35,
			"y" : -20,
			"width" : 280,
			"color" : "gray",
			"title" : uiScriptLocale.MESSAGE,  # wird eh in Python überschrieben
		},
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 280,
			"height" : 75,

			"children" :
			(
				{
					"name" : "message",
					"type" : "text",

					"x" : 0,
					"y" : 25,

					"text" : uiScriptLocale.LOGIN_CONNECTING,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "countdown_message",
					"type" : "text",

					"x" : 0,
					"y" : 50,

					"text" : uiScriptLocale.MESSAGE,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
			),

		},
	),
}
