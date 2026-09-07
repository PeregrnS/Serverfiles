import uiScriptLocale

window = {
	"name" : "AtlasWindow",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH - 136 - 256 - 10,
	"y" : 0,

	"width" : 256 + 15,
	"height" : 256 + 38,

	"children" :
	(
		{
			"name" : "board",
			"type" : "window",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 256 + 15,
			"height" : 256 + 38,

			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar_finalcore_large",

					"x" : 0,
					"y" : 0,

					"width" : 256 + 15,
					"height" : 42,

					"title" : uiScriptLocale.ZONE_MAP,
				},
			),
		},
	),
}