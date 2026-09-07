import uiScriptLocale

window = {
	"name" : "ItemshopWindow",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : 800,
	"height" : 553,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_finalcore",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 800,
			"height" : 553,

			"children" :
			(
				{
					"name" : "Itemshop_TitleBar",
					"type" : "titlebar_finalcore_large",
					"style" : ("attach",),

					"x" : 500,
					"y" : -15,

					"width" : 780,

					"title" : "Itemshop",
				},


				{
					"name" : "ContentRoot",
					"type" : "window",
					"style" : ("attach",),

					"x" : 0,
					"y" : 0,

					"width" : 800,
					"height" : 553,
				},
			),
		},
	),
}
