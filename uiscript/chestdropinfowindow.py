import uiScriptLocale

PATTERN_PATH			= "d:/ymir work/ui/pattern/"
CHATTING_PATH			= "d:/ymir work/ui/chat/"
WINDOW_WIDTH			= 279 ## default 180
WINDOW_HEIGHT			= 240 ## default 380
PATTERN_WINDOW_WIDTH	= WINDOW_WIDTH - 15
PATTERN_WINDOW_HEIGHT	= WINDOW_HEIGHT - 46

PATTERN_X_COUNT = (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT = (PATTERN_WINDOW_HEIGHT - 32) / 16

MIDDLE_BUTTON_WIDTH 	= 65
MIDDLE_BUTTON_HEIGHT 	= 25

window = {
	"name" : "ChestDropInfoWindow",
	"style" : ("movable", "float",),
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.CHEST_DROP_TITLE,
			"children" :
			(
				## base pattern
				{
					"name" : "base_pattern",
					"type" : "window",
					"style" : ("attach", "ltr",),
					"x" : 7,
					"y" : 32,
					"width" : PATTERN_WINDOW_WIDTH,
					"height" :PATTERN_WINDOW_HEIGHT,
					"children" :
					(
						## LeftTop 1
						{
							"name" : "pattern_left_top_img",
							"type" : "image",
							"style" : ("ltr",),
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "pattern_right_top_img",
							"type" : "image",
							"style" : ("ltr",),
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "pattern_left_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							"x" : 0,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "pattern_right_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "pattern_top_cetner_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "pattern_left_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "pattern_right_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "pattern_bottom_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "pattern_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, PATTERN_Y_COUNT),
						},
					),
				},
				## Item Slot
				{
					"name" : "drop_item_slot",
					"type" : "grid_table",

					"x" : 13,
					"y" : 36,

					"start_index" : 0,
					"x_count" : 8,
					"y_count" : 5,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
				},

				{
					"name" : "prev_button", 
					"type" : "button",
					
					"x" : 94, 
					"y" : 203,

					"default_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
					"over_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_02.sub",
					"down_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
				},
				{
					"name" : "next_button", 
					"type" : "button",

					"x" : 152, 
					"y" : 203,

					"default_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
					"over_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_02.sub",
					"down_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
				},
				{
					"name" : "CurrentPageBack",
					"type" : "thinboard_circle",
					
					"x" : 118,
					"y" : 200,
					
					"width" : 30,
					"height" : 20,
					
					"children" :
					(
						{
							"name" : "CurrentPage",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							
							"vertical_align" : "center",
							"horizontal_align" : "center",
							
							"text_vertical_align" : "center",
							"text_horizontal_align" : "center",

							"text" : "1",
						},
					),
				},
			),
		},
	),
}