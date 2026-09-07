NEW_PATH = "d:/ymir work/ui/"
PATH = "d:/ymir work/ui/login/select/"

window = {
	"name" : "SelectCharacterWindow",
	"x" : 0, "y" : 0, "width" : SCREEN_WIDTH, "height" : SCREEN_HEIGHT,
	"style" : ("float",),
	"children" :
	(
		{
			"name" : "Background",
			"type" : "expanded_image",
			"x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1920.0,
			"y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"image" : PATH + "bg.png",
		},
		# Draw before content/slots so 3D view does not steal clicks from Start Game (btn_login).
		{
			"name" : "character_render_window", "type" : "window",
			"x" : SCREEN_WIDTH / 2, "y" : 0,
			"width" : SCREEN_WIDTH / 2, "height" : SCREEN_HEIGHT,
		},
		# Arrows must sit UNDER content+slot — if they are last in the list they cover the whole screen and block btn_login.
		{
			"name" : "btn_left",
			"type" : "button",
			"x" : SCREEN_WIDTH * (290) / 800,
			"y" : 0,
			"vertical_align" : "center",
			"default_image" : PATH + "arrow_l_01.png",
			"over_image" : PATH + "arrow_l_02.png",
			"down_image" : PATH + "arrow_l_03.png",
		},
		{
			"name" : "btn_right",
			"type" : "button",
			"x" : SCREEN_WIDTH * (500) / 800,
			"y" : 0,
			"vertical_align" : "center",
			"default_image" : PATH + "arrow_r_01.png",
			"over_image" : PATH + "arrow_r_02.png",
			"down_image" : PATH + "arrow_r_03.png",
		},
		{
			"name" : "content", "type" : "window",
			"x" : SCREEN_WIDTH / 4 + 80, "y" : SCREEN_HEIGHT / 2 - 300 / 2,
			"horizontal_align" : "right",
			"width" : 330, "height" : 410,
			"children" :
			(
				{
					"name" : "content", "type" : "image",
					"x" : 0, "y" : 0,
					"image" : PATH + "bg2.png",
					"children" :
					(
						{
							"name" : "stats_background",
							"type" : "window",
							"x" : 40, "y" : 47,
							"width" : 140, "height" : 105,
							"vertical_align" : "center",
							"horizontal_align" : "center",
							"children" :
							(
								{
									"name" : "gauge_vit",
									"type" : "gauge",
									"x" : 0, "y" : 0, "width" : 125,
									"color" : "red",
								},
								{
									"name" : "gauge_str",
									"type" : "gauge",
									"x" : 0, "y" : 26, "width" : 125,
									"color" : "pink",
								},
								{
									"name" : "gauge_int",
									"type" : "gauge",
									"x" : 0, "y" : 26*2, "width" : 125,
									"color" : "purple",
								},
								{
									"name" : "gauge_dex",
									"type" : "gauge",
									"x" : 0, "y" : 26*3, "width" : 125,
									"color" : "blue",
								},
							),
						},
						# PLAYTIME - Field
						{
							"name" : "text_timevalue", "type" : "text",
							"x" : 75, "y" : 37,
							"horizontal_align" : "right",
							"text_horizontal_align" : "center",
							"text" : "DEFAULT_TEXT",
							"fontsize" : "LARGE",
						},
						# LEVEL - Field
						{
							"name" : "text_levelvalue", "type" : "text",
							"x" : 75, "y" : 37*2-9,
							"horizontal_align" : "right",
							"text_horizontal_align" : "center",
							"text" : "DEFAULT_TEXT",
							"fontsize" : "LARGE",
						},
						# KINGDOMNAME - Field
						{
							"name" : "text_kingdomvalue", "type" : "image",
							"x" : 75+15, "y" : 37*3-9,
							"horizontal_align" : "right",
							"text_horizontal_align" : "center",
							"image" : PATH + "empire_3.png",
						},
						# GUILD - Field
						{
							"name" : "text_guildname", "type" : "text",
							"x" : 75, "y" : 37*4-9,
							"horizontal_align" : "right",
							"text_horizontal_align" : "center",
							"text" : "DEFAULT_TEXT",
							"fontsize" : "LARGE",
						},
						# LOGINBTN - Field
						{
							"name" : "btn_login", "type" : "button",
							"x" : 0, "y" : 70,
							"horizontal_align" : "center",
							"vertical_align" : "bottom",
							"default_image" : PATH + "login_btn_01.png",
							"over_image" : PATH + "login_btn_02.png",
							"down_image" : PATH + "login_btn_03.png",
						},
					),
				},
			),
		},
		#SLOT
		{
			"name" : "slot_window", "type" : "window",
			"x" : 0, "y" : 0,
			"horizontal_align" : "left",
			"vertical_align" : "center",
			"width" : 353, "height" : 770,
			"children":
			(
				{
					"name" : "slot_1", "type" : "radio_button",
					"x" : 0, "y" : 0,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn/btn_empty_01.png",
					"over_image" : PATH + "btn/btn_empty_02.png",
					"down_image" : PATH + "btn/btn_empty_02.png",
					"children":
					(
						{
							"name" : "text_playername_new0", "type" : "text",
							"x" : -78, "y" : 90,
							"text" : "",
							"fontsize":"LARGE",
							"horizontal_align":"center",
							"text_horizontal_align":"center",
						},
						{
							"name" : "btn_create_new0", "type" : "button",
							"x" : 0, "y" : 0,
							"default_image" : PATH + "btn/btn_empty_01.png",
							"over_image" : PATH + "btn/btn_empty_02.png",
							"down_image" : PATH + "btn/btn_empty_02.png",
						},
						{
							"name" : "btn_login_new0", "type" : "button",
							"x" : 200, "y" : -23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_select_01.png",
							"over_image" : PATH + "btn_select_02.png",
							"down_image" : PATH + "btn_select_03.png",
						},
						{
							"name" : "btn_delete_new0", "type" : "button",
							"x" : 200, "y" : 23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_delete_01.png",
							"over_image" : PATH + "btn_delete_02.png",
							"down_image" : PATH + "btn_delete_03.png",
						},
					),
				},
				{
					"name" : "slot_2", "type" : "radio_button",
					"x" : 0, "y" : 160,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn/btn_empty_01.png",
					"over_image" : PATH + "btn/btn_empty_02.png",
					"down_image" : PATH + "btn/btn_empty_03.png",
					"children":
					(
						{
							"name" : "text_playername_new1", "type" : "text",
							"x" : -78, "y" : 90,
							"text" : "",
							"fontsize":"LARGE",
							"horizontal_align":"center",
							"text_horizontal_align":"center",
						},
						{
							"name" : "btn_create_new1", "type" : "button",
							"x" : 0, "y" : 0,
							"default_image" : PATH + "btn/btn_empty_01.png",
							"over_image" : PATH + "btn/btn_empty_02.png",
							"down_image" : PATH + "btn/btn_empty_02.png",
						},
						{
							"name" : "btn_login_new1", "type" : "button",
							"x" : 200, "y" : -23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_select_01.png",
							"over_image" : PATH + "btn_select_02.png",
							"down_image" : PATH + "btn_select_03.png",
						},
						{
							"name" : "btn_delete_new1", "type" : "button",
							"x" : 200, "y" : 23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_delete_01.png",
							"over_image" : PATH + "btn_delete_02.png",
							"down_image" : PATH + "btn_delete_03.png",
						},
					),
				},
				{
					"name" : "slot_3", "type" : "radio_button",
					"x" : 0, "y" : 160*2,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn/btn_empty_01.png",
					"over_image" : PATH + "btn/btn_empty_02.png",
					"down_image" : PATH + "btn/btn_empty_03.png",
					"children":
					(
						{
							"name" : "text_playername_new2", "type" : "text",
							"x" : -78, "y" : 90,
							"text" : "",
							"fontsize":"LARGE",
							"horizontal_align":"center",
							"text_horizontal_align":"center",
						},
						{
							"name" : "btn_create_new2", "type" : "button",
							"x" : 0, "y" : 0,
							"default_image" : PATH + "btn/btn_empty_01.png",
							"over_image" : PATH + "btn/btn_empty_02.png",
							"down_image" : PATH + "btn/btn_empty_02.png",
						},
						{
							"name" : "btn_login_new2", "type" : "button",
							"x" : 200, "y" : -23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_select_01.png",
							"over_image" : PATH + "btn_select_02.png",
							"down_image" : PATH + "btn_select_03.png",
						},
						{
							"name" : "btn_delete_new2", "type" : "button",
							"x" : 200, "y" : 23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_delete_01.png",
							"over_image" : PATH + "btn_delete_02.png",
							"down_image" : PATH + "btn_delete_03.png",
						},
					),
				},
				{
					"name" : "slot_4", "type" : "radio_button",
					"x" : 0, "y" : 160*3,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn/btn_empty_01.png",
					"over_image" : PATH + "btn/btn_empty_02.png",
					"down_image" : PATH + "btn/btn_empty_03.png",
					"children":
					(
						{
							"name" : "text_playername_new3", "type" : "text",
							"x" : -78, "y" : 90,
							"text" : "",
							"fontsize":"LARGE",
							"horizontal_align":"center",
							"text_horizontal_align":"center",
						},
						{
							"name" : "btn_create_new3", "type" : "button",
							"x" : 0, "y" : 0,
							"default_image" : PATH + "btn/btn_empty_01.png",
							"over_image" : PATH + "btn/btn_empty_02.png",
							"down_image" : PATH + "btn/btn_empty_02.png",
						},
						{
							"name" : "btn_login_new3", "type" : "button",
							"x" : 200, "y" : -23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_select_01.png",
							"over_image" : PATH + "btn_select_02.png",
							"down_image" : PATH + "btn_select_03.png",
						},
						{
							"name" : "btn_delete_new3", "type" : "button",
							"x" : 200, "y" : 23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_delete_01.png",
							"over_image" : PATH + "btn_delete_02.png",
							"down_image" : PATH + "btn_delete_03.png",
						},
					),
				},
				{
					"name" : "slot_5", "type" : "radio_button",
					"x" : 0, "y" : 160*4,
					"horizontal_align" : "center",
					"default_image" : PATH + "btn/btn_empty_01.png",
					"over_image" : PATH + "btn/btn_empty_02.png",
					"down_image" : PATH + "btn/btn_empty_03.png",
					"children":
					(
						{
							"name" : "text_playername_new4", "type" : "text",
							"x" : -78, "y" : 90,
							"text" : "",
							"fontsize":"LARGE",
							"horizontal_align":"center",
							"text_horizontal_align":"center",
						},
						{
							"name" : "btn_create_new4", "type" : "button",
							"x" : 0, "y" : 0,
							"default_image" : PATH + "btn/btn_empty_01.png",
							"over_image" : PATH + "btn/btn_empty_02.png",
							"down_image" : PATH + "btn/btn_empty_02.png",
						},
						{
							"name" : "btn_login_new4", "type" : "button",
							"x" : 200, "y" : -23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_select_01.png",
							"over_image" : PATH + "btn_select_02.png",
							"down_image" : PATH + "btn_select_03.png",
						},
						{
							"name" : "btn_delete_new4", "type" : "button",
							"x" : 200, "y" : 23,
							"vertical_align" : "center",
							"default_image" : PATH + "btn_delete_01.png",
							"over_image" : PATH + "btn_delete_02.png",
							"down_image" : PATH + "btn_delete_03.png",
						},
					),
				},
			),
		},
	),
}
