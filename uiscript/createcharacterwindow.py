PATH = "d:/ymir work/ui/login/create/"
PATH2 = "d:/ymir work/ui/login/select/"
PATH3 = "d:/ymir work/ui/login/login/"

window = {
	"name" : "CreateCharacterWindow",
	"x" : 0, "y" : 0, "width" : SCREEN_WIDTH, "height" : SCREEN_HEIGHT,
	"style" : ("float",),
	"children" :
	(
		{
			"name" : "Background",
			"type" : "expanded_image",
			"x" : 0,
			"y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1920.0,
			"y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"image" : PATH2 + "bg.png",
		},

		{
			"name" : "content_create", "type" : "image",
			"x" : SCREEN_WIDTH / 4 - 440 / 2, "y" : SCREEN_HEIGHT / 2 - 290 / 2,
			"image" : PATH + "bg2.png",
			"children" :
			(
				#SLOT
				{
					"name" : "slot_1", "type" : "radio_button",
					"x" : 0, "y" : 73,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn/btn_warrior_m_01.png",
					"over_image" : PATH + "btn/btn_warrior_m_02.png",
					"down_image" : PATH + "btn/btn_warrior_m_02.png",
				},
				{
					"name" : "slot_2", "type" : "radio_button",
					"x" : 0, "y" : 73,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn/btn_warrior_m_01.png",
					"over_image" : PATH + "btn/btn_warrior_m_02.png",
					"down_image" : PATH + "btn/btn_warrior_m_02.png",
				},
				{
					"name" : "slot_3", "type" : "radio_button",
					"x" : 0, "y" : 73,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn/btn_warrior_m_01.png",
					"over_image" : PATH + "btn/btn_warrior_m_02.png",
					"down_image" : PATH + "btn/btn_warrior_m_02.png",
				},
				{
					"name" : "slot_4", "type" : "radio_button",
					"x" : 0, "y" : 73,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn/btn_warrior_m_01.png",
					"over_image" : PATH + "btn/btn_warrior_m_02.png",
					"down_image" : PATH + "btn/btn_warrior_m_02.png",
				},

				## Arrow
				{
					"name" : "arrow_left", "type" : "button",
					"x" : -60, "y" : 82,
					"horizontal_align" : "center",
					"default_image" : PATH + "arrow_l_01.png",
					"over_image" : PATH + "arrow_l_02.png",
					"down_image" : PATH + "arrow_l_03.png",
				},
				{
					"name" : "arrow_right", "type" : "button",
					"x" : 60, "y" : 82,
					"horizontal_align" : "center",
					"default_image" : PATH + "arrow_r_01.png",
					"over_image" : PATH + "arrow_r_02.png",
					"down_image" : PATH + "arrow_r_03.png",
				},

				## Gender
				{
					"name" : "btn_gender_01", "type" : "radio_button",
					"x" : 60, "y" : 137,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn_female_01.png",
					"over_image" : PATH + "btn_female_02.png",
					"down_image" : PATH + "btn_female_02.png",
				},
				{
					"name" : "btn_gender_02", "type" : "radio_button",
					"x" : -60, "y" : 137,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn_male_01.png",
					"over_image" : PATH + "btn_male_02.png",
					"down_image" : PATH + "btn_male_02.png",
				},
				## Shape
				{
					"name" : "btn_shape_01", "type" : "radio_button",
					"x" : -60, "y" : 183,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn_shape1_01.png",
					"over_image" : PATH + "btn_shape1_02.png",
					"down_image" : PATH + "btn_shape1_02.png",
				},
				{
					"name" : "btn_shape_02", "type" : "radio_button",
					"x" : 60, "y" : 183,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn_shape2_01.png",
					"over_image" : PATH + "btn_shape2_02.png",
					"down_image" : PATH + "btn_shape2_02.png",
				},
				## Character Name
				{
					"name" : "CharacterName", "type" : "image",
					"x" : 0, "y" : 130,
					"horizontal_align" : "center",
					"vertical_align" : "bottom",
					"image" : PATH3 + "input_id.png",
					"children":
					(
						{
							"name" : "edit_name", "type" : "editline",
							"x" : 60, "y" : 11, "width" : 220, "height" : 26,
							"input_limit" : 24,
							"text" : "",
							"Info_text" : "Username",
						},
					),
				},
				#Create Button
				{
					"name" : "btn_create", "type" : "button",
					"x" : -80, "y" : 70,
					"horizontal_align" : "center",
					"vertical_align" : "bottom",
					"default_image" : PATH + "btn_create_01.png",
					"over_image" : PATH + "btn_create_02.png",
					"down_image" : PATH + "btn_create_03.png",
				},
				## Canel Button
				{
					"name" : "exit_button",
					"type" : "button",
					"x" : 80, "y" : 70,
					"horizontal_align" : "center",
					"vertical_align" : "bottom",
					"default_image" : PATH + "btn_cancel_01.png",
					"over_image" : PATH + "btn_cancel_02.png",
					"down_image" : PATH + "btn_cancel_03.png",
				},
			),
		},
		{
			"name" : "character_render_window", "type" : "window",
			"x" : SCREEN_WIDTH, "y" : 0,
			"width" : SCREEN_WIDTH / 2, "height" : SCREEN_HEIGHT,
		},
		{
			"name" : "btn_left",
			"type" : "button",

			"x" : SCREEN_WIDTH * (290) / 800,
			"y" : 0,
			# "horizontal_align" : "center",
			"vertical_align" : "center",
			"default_image" : PATH2 + "arrow_l_01.png",
			"over_image" : PATH2 + "arrow_l_02.png",
			"down_image" : PATH2 + "arrow_l_03.png",
		},
		{
			"name" : "btn_right",
			"type" : "button",

			"x" : SCREEN_WIDTH * (500) / 800,
			"y" : 0,
			# "horizontal_align" : "center",
			"vertical_align" : "center",

			"default_image" : PATH2 + "arrow_r_01.png",
			"over_image" : PATH2 + "arrow_r_02.png",
			"down_image" : PATH2 + "arrow_r_03.png",
		},

	),
}
