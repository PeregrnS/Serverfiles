import uiScriptLocale
import item

window = {
	"name" : "CostumeEquipmentWindow",

	"x" : SCREEN_WIDTH - 175 - 140,
	"y" : SCREEN_HEIGHT - 37 - 565 - 40,

	"style" : ("movable", "float",),

	"width" : 180,
	"height" : (180 + 47),

	"children" :
	(
		{
			"name" : "CostumeEquipmentLayer",
			"type" : "board",
			"style" : ("attach",),

			"x" : -38,
			"y" : 0,

			"width" : 180,
			"height" : (180 + 47),

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar_finalcore_small",
					"style" : ("attach",),

					"x" : 4,
					"y" : -13,

					"width" : 130,
					"color" : "yellow",

					"children" :
					(
						{
							"name":"TitleName",
							"type":"text",

							"x":60, "y":30,

							"text":uiScriptLocale.COSTUME_WINDOW_TITLE,
							"text_horizontal_align":"center"
						},
					),
				},

				## Equipment Slot
				{
					"name" : "Costume_Base",
					"type" : "image",

					"x" : 32,
					"y" : 38,

					"image" : uiScriptLocale.LOCALE_UISCRIPT_PATH + "costume/new_costume_bg.jpg",

					"children" :
					(

						{
							"name" : "CostumeEquipmentSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 127,
							"height" : 175,

							"slot" : (
								## BODY
								{"index":item.WEAR_COSTUME_BODY, "x":62, "y":45, "width":32, "height":64},
								## HAIR
								{"index":item.WEAR_COSTUME_HAIR, "x":62, "y": 9, "width":32, "height":32},
								## MOUNT
								{"index":item.WEAR_MOUNT, "x":5, "y":126, "width":32, "height":32},
								## COSTUME WEAPON
								{"index":item.WEAR_COSTUME_WEAPON, "x":13, "y":13, "width":32, "height":96},
								## ACCE
								{"index":item.WEAR_COSTUME_ACCE, "x":62, "y":126, "width":32, "height":32},
							),
						},
					),
				},
			),
		},
	),
}
