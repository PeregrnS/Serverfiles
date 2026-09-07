import uiScriptLocale

ROOT = "d:/ymir work/ui/minimap/"

window = {
	"name" : "AtlasWindow",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH - 136 - 256 - 10,
	"y" : 0,

	"width" : 256 + 15,
	"height" : 256 + 38,

	"children" :
	(
		## MAIN FINALCORE BOARD (HINTERGRUND)
		{
			"name" : "board",
			"type" : "board_finalcore",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,
			"width" : 256 + 15,
			"height" : 256 + 38,

			"children" :
			(
				## TITLE BAR
				{
					"name" : "TitleBar",
					"type" : "titlebar_finalcore_small",
					"style" : ("attach",),

					"x" : 10,
					"y" : -18,
					"width" : 256,

					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",

							"x" : -60,
							"y" : 18,
							"text" : uiScriptLocale.ZONE_MAP,
							"all_align" : "center",
						},
					),
				},
			),
		},
	),
}
