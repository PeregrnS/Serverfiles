import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/preview/"
KEY_PATH = "icon/emoticons/actions/"

BG_WIDTH = 355
BG_HEIGHT = 432

BOARD_WIDTH = BG_WIDTH + 7 * 2
BOARD_HEIGHT = BG_HEIGHT + 31 + 63 + 5

window = {
	"name" : "TransmuteWindow",

	"x" : (SCREEN_WIDTH - BOARD_WIDTH) / 2,
	"y" : (SCREEN_HEIGHT - BOARD_HEIGHT) / 2,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"title" : uiScriptLocale.PREVIEW_ITEM_WINDOW_TITLE,

			"children" :
			(
				{
					"name" : "renderer",
					"type" : "render_target",

					"x" : 7,
					"y" : 31,

					"image" : ROOT_PATH + "bg.tga",
					"width" : BOARD_WIDTH - 14,
					"height" : BOARD_HEIGHT - 31 * 2,
				},
				{
					"name" : "rotate",
					"type" : "image",

					"x" : 20,
					"y" : 420,

					"image" : KEY_PATH + "key_lclick.tga",
				},
				{
					"name" : "rotate_text",
					"type" : "text",

					"x" : 40,
					"y" : 420,

					"text" : uiScriptLocale.PREVIEW_ITEM_WINDOW_ROTATE,
				},
				{
					"name" : "zoom",
					"type" : "image",

					"x" : 20,
					"y" : 440,

					"image" : KEY_PATH + "key_scroll_click.tga",
				},
				{
					"name" : "zoom_text",
					"type" : "text",

					"x" : 40,
					"y" : 440,

					"text" : uiScriptLocale.PREVIEW_ITEM_WINDOW_ZOOM,
				},
				############################# TEMP LIGHTING
#				{
#					"name" : "light_x_desc",
#					"type" : "text",
#
#					"x" : 10,
#					"y" : 31 + 3,
#
#					"text" : "x",
#				},
#				{
#					"name" : "light_x_plus",
#					"type" : "button",
#
#					"x" : 10 + 50,
#					"y" : 31,
#
#					"default_image" : "d:/ymir work/ui/public/Small_Button_01.sub",
#					"over_image" : "d:/ymir work/ui/public/Small_Button_02.sub",
#					"down_image" : "d:/ymir work/ui/public/Small_Button_03.sub",
#
#					"text" : "+",
#				},
#				{
#					"name" : "light_x_minus",
#					"type" : "button",
#
#					"x" : 10 + 50 + 55,
#					"y" : 31,
#
#					"default_image" : "d:/ymir work/ui/public/Small_Button_01.sub",
#					"over_image" : "d:/ymir work/ui/public/Small_Button_02.sub",
#					"down_image" : "d:/ymir work/ui/public/Small_Button_03.sub",
#
#					"text" : "-",
#				},
#				{
#					"name" : "light_y_desc",
#					"type" : "text",
#
#					"x" : 10,
#					"y" : 54 + 3,
#
#					"text" : "y",
#				},
#				{
#					"name" : "light_y_plus",
#					"type" : "button",
#
#					"x" : 10 + 50,
#					"y" : 54,
#
#					"default_image" : "d:/ymir work/ui/public/Small_Button_01.sub",
#					"over_image" : "d:/ymir work/ui/public/Small_Button_02.sub",
#					"down_image" : "d:/ymir work/ui/public/Small_Button_03.sub",
#
#					"text" : "+",
#				},
#				{
#					"name" : "light_y_minus",
#					"type" : "button",
#
#					"x" : 10 + 50 + 55,
#					"y" : 54,
#
#					"default_image" : "d:/ymir work/ui/public/Small_Button_01.sub",
#					"over_image" : "d:/ymir work/ui/public/Small_Button_02.sub",
#					"down_image" : "d:/ymir work/ui/public/Small_Button_03.sub",
#
#					"text" : "-",
#				},
#				{
#					"name" : "light_z_desc",
#					"type" : "text",
#
#					"x" : 10,
#					"y" : 77 + 3,
#
#					"text" : "z",
#				},
#				{
#					"name" : "light_z_plus",
#					"type" : "button",
#
#					"x" : 10 + 50,
#					"y" : 77,
#
#					"default_image" : "d:/ymir work/ui/public/Small_Button_01.sub",
#					"over_image" : "d:/ymir work/ui/public/Small_Button_02.sub",
#					"down_image" : "d:/ymir work/ui/public/Small_Button_03.sub",
#
#					"text" : "+",
#				},
#				{
#					"name" : "light_z_minus",
#					"type" : "button",
#
#					"x" : 10 + 50 + 55,
#					"y" : 77,
#
#					"default_image" : "d:/ymir work/ui/public/Small_Button_01.sub",
#					"over_image" : "d:/ymir work/ui/public/Small_Button_02.sub",
#					"down_image" : "d:/ymir work/ui/public/Small_Button_03.sub",
#
#					"text" : "-",
#				},
#				############################# TEMP LIGHTING END
				{
					"name" : "bottom_bg",
					"type" : "image",
					"style" : ("not_pick",),

					"x" : 7,
					"y" : 63 + 5,

					"vertical_align" : "bottom",

					"image" : ROOT_PATH + "down_bg.tga",

					"children" :
					(
						{
							"name" : "sex_male",
							"type" : "radio_button",

							"x" : 20,
							"y" : 0,

							"vertical_align" : "center",

							"default_image" : ROOT_PATH + "male_button.tga",
							"over_image" : ROOT_PATH + "male_button_hover.tga",
							"down_image" : ROOT_PATH + "male_button_select.tga",
							"disable_image" : ROOT_PATH + "male_button_disabled.tga",
						},
						{
							"name" : "job_warrior",
							"type" : "radio_button",

							"x" : 69,
							"y" : 0,

							"vertical_align" : "center",

							"default_image" : ROOT_PATH + "warrior_inactive.tga",
							"over_image" : ROOT_PATH + "warrior_hover.tga",
							"down_image" : ROOT_PATH + "warrior_active.tga",
							"disable_image" : ROOT_PATH + "warrior_disabled.tga",
						},
						{
							"name" : "job_sura",
							"type" : "radio_button",

							"x" : 126,
							"y" : 0,

							"vertical_align" : "center",

							"default_image" : ROOT_PATH + "sura_inactive.tga",
							"over_image" : ROOT_PATH + "sura_hover.tga",
							"down_image" : ROOT_PATH + "sura_active.tga",
							"disable_image" : ROOT_PATH + "sura_disabled.tga",
						},
						{
							"name" : "job_assassin",
							"type" : "radio_button",

							"x" : 182,
							"y" : 0,

							"vertical_align" : "center",

							"default_image" : ROOT_PATH + "assassin_inactive.tga",
							"over_image" : ROOT_PATH + "assassin_hover.tga",
							"down_image" : ROOT_PATH + "assassin_active.tga",
							"disable_image" : ROOT_PATH + "assassin_disabled.tga",
						},
						{
							"name" : "job_shaman",
							"type" : "radio_button",

							"x" : 237,
							"y" : 0,

							"vertical_align" : "center",

							"default_image" : ROOT_PATH + "shaman_inactive.tga",
							"over_image" : ROOT_PATH + "shaman_hover.tga",
							"down_image" : ROOT_PATH + "shaman_active.tga",
							"disable_image" : ROOT_PATH + "shaman_disabled.tga",
						},
						{
							"name" : "sex_female",
							"type" : "radio_button",

							"x" : 304,
							"y" : 0,

							"vertical_align" : "center",

							"default_image" : ROOT_PATH + "female_button.tga",
							"over_image" : ROOT_PATH + "female_button_hover.tga",
							"down_image" : ROOT_PATH + "female_button_select.tga",
							"disable_image" : ROOT_PATH + "female_button_disabled.tga",
						},
					),
				},
			),
		},
	),
}
