import uiScriptLocale

NEW_PATH = "d:/ymir work/ui/login/login/"

window = {
	"name" : "LoginWindow",
	"style" : ("movable",),
	"x" : 0, "y" : 0,
	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,
	"children" : 
	(
		{
			"name" : "background",
			"type" : "ani_image",
			"x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1920.0,
			"y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"delay" : 30,
			"images" :
			(
				NEW_PATH + "ani/bg000.dds",
			),
		},
		{
			"name" : "Main",
			"type" : "image",
			"x" : 0, "y" : 0,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"image" : NEW_PATH + "bg.png",
			"children" :
			(
				{
					"name" : "Channel",
					"type" : "window",
					"x" : 0, "y" : -10,
					"width" : 659, "height" : 120,
					"horizontal_align" : "center",
					"vertical_align" : "center",
					"children" :
					(

						{
							"name" : "select_btn_ch1",
							"type" : "radio_button",
							"x" : -160, "y" : 0,
							"horizontal_align" : "center",
							"default_image" : NEW_PATH + "btn/btn_ch1_01.png",
							"over_image" : NEW_PATH + "btn/btn_ch1_02.png",
							"down_image" : NEW_PATH + "btn/btn_ch1_02.png",
						},
						{
							"name" : "select_btn_ch2",
							"type" : "radio_button",
							"x" : 0, "y" : 0,
							"horizontal_align" : "center",
							"default_image" : NEW_PATH + "btn/btn_ch2_01.png",
							"over_image" : NEW_PATH + "btn/btn_ch2_02.png",
							"down_image" : NEW_PATH + "btn/btn_ch2_02.png",
						},
						{
							"name" : "select_btn_ch3",
							"type" : "radio_button",
							"x" : 160, "y" : 0,
							"horizontal_align" : "center",
							"default_image" : NEW_PATH + "btn/btn_ch3_01.png",
							"over_image" : NEW_PATH + "btn/btn_ch3_02.png",
							"down_image" : NEW_PATH + "btn/btn_ch3_02.png",
						},
						{
							"name" : "select_btn_ch4",
							"type" : "radio_button",
							"x" : -45*2, "y" : 70,
							"horizontal_align" : "center",
							"default_image" : NEW_PATH + "btn/btn_ch4_01.png",
							"over_image" : NEW_PATH + "btn/btn_ch4_02.png",
							"down_image" : NEW_PATH + "btn/btn_ch4_02.png",
						},
						{
							"name" : "select_btn_ch5",
							"type" : "radio_button",
							"x" : 45*2, "y" : 70,
							"horizontal_align" : "center",
							"default_image" : NEW_PATH + "btn/btn_ch5_01.png",
							"over_image" : NEW_PATH + "btn/btn_ch5_02.png",
							"down_image" : NEW_PATH + "btn/btn_ch5_02.png",
						},
					),
				},#Board Right
				{
					"name" : "board",
					"type" : "window",
					"x" : 0, "y" : 130,
					"width" : 659, "height" : 80,
					"vertical_align" : "center",
					"horizontal_align" : "center",
					"children" :
					(
						{
							"name" : "input_user",
							"type" : "image",
							"x" : -75, "y" : 0,
							"image" : NEW_PATH + "input_id.png",
							"horizontal_align" : "center",
							"children" : 
							(
								{
									"name" : "id",
									"type" : "editline",
									"x" : 55, "y" : 10,
									"width" : 230, "height" : 25,
									"input_limit": 16,
									"Info_text": "Username...",
								},
							),
						},
						{
							"name" : "input_password",
							"type" : "image",
							"x" : -75, "y" : 47,
							"image" : NEW_PATH + "input_pw.png",
							"horizontal_align" : "center",
							"children" : 
							(
								{
									"name" : "pwd",
									"type" : "editline",
									"x" : 55, "y" : 10,
									"width" : 230, "height" : 25,
									"input_limit": 16,
									"secret_flag": 1,
									"Info_text": "Password...",
								},
							),
						},
						{
							"name" : "input_pincode",
							"type" : "image",
							"x" : 150, "y" : 0,
							"image" : NEW_PATH + "input_pin.png",
							"horizontal_align" : "center",
							"children" : 
							(
								{
									"name" : "pin",
									"type" : "editline",
									"x" : 40, "y" : 10,
									"width" : 230, "height" : 25,
									"input_limit": 16,
									"secret_flag": 1,
									"Info_text": "Pincode...",
								},
							),
						},
						{
							"name" : "login_button",
							"type" : "button",
							"horizontal_align" : "center",
							"x" : 150, "y" : 45,
							"default_image" : NEW_PATH + "btn_login_01.png",
							"over_image" : NEW_PATH + "btn_login_02.png",
							"down_image" : NEW_PATH + "btn_login_03.png",
						},
					),
				},

				{
					"name" : "Account",
					"type" : "window",
					"x" :  0, "y" : 260,
					"width" : 659, "height" : 80,
					"horizontal_align" : "center",
					"vertical_align" : "center",
					"children" :
					(
						{
							"name" : "f1_login_header",
							"type" : "window",
							"x" : -200, "y" : 0,
							"width" : 171, "height" : 32,
							"horizontal_align" : "center",
							"children" : 
							(
								{
									"name" : "f1_login_button",
									"type" : "button",
									"x" : 0, "y" : 0,
									"default_image" : NEW_PATH + "btn_acc_login_01.png",
									"over_image" : NEW_PATH + "btn_acc_login_02.png",
									"down_image" : NEW_PATH + "btn_acc_login_03.png",
									"children" : 
									(
										{ "name" : "f1_account_text", "type" : "text", "x" : 0, "y" : 8, "fontsize": "LARGE", "text" : "Free Slot", "horizontal_align" : "center", "text_horizontal_align" : "center",},
									),
								},
								{
									"name" : "f1_save_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_save_account_01.png",
									"over_image" : NEW_PATH + "btn_save_account_02.png",
									"down_image" : NEW_PATH + "btn_save_account_03.png",
								},
								{
									"name" : "f1_delete_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_cancel_01.png",
									"over_image" : NEW_PATH + "btn_cancel_02.png",
									"down_image" : NEW_PATH + "btn_cancel_03.png",
								},
							),
						},
						{
							"name" : "f2_login_header",
							"type" : "window",
							"x" : 0, "y" : 0,
							"width" : 171, "height" : 32,
							"horizontal_align" : "center",
							"children" : 
							(
								{
									"name" : "f2_login_button",
									"type" : "button",
									"x" : 0, "y" : 0,
									"default_image" : NEW_PATH + "btn_acc_login_01.png",
									"over_image" : NEW_PATH + "btn_acc_login_02.png",
									"down_image" : NEW_PATH + "btn_acc_login_03.png",
									"children" : 
									(
										{ "name" : "f2_account_text", "type" : "text", "x" : 0, "y" : 8, "fontsize": "LARGE", "text" : "Free Slot", "horizontal_align" : "center", "text_horizontal_align" : "center",},
									),
								},
								{
									"name" : "f2_save_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_save_account_01.png",
									"over_image" : NEW_PATH + "btn_save_account_02.png",
									"down_image" : NEW_PATH + "btn_save_account_03.png",
								},
								{
									"name" : "f2_delete_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_cancel_01.png",
									"over_image" : NEW_PATH + "btn_cancel_02.png",
									"down_image" : NEW_PATH + "btn_cancel_03.png",
								},
							),
						},
						{
							"name" : "f3_login_header",
							"type" : "window",
							"x" : 200, "y" : 0,
							"width" : 171, "height" : 32,
							"horizontal_align" : "center",
							"children" : 
							(
								{
									"name" : "f3_login_button",
									"type" : "button",
									"x" : 0, "y" : 0,
									"default_image" : NEW_PATH + "btn_acc_login_01.png",
									"over_image" : NEW_PATH + "btn_acc_login_02.png",
									"down_image" : NEW_PATH + "btn_acc_login_03.png",
									"children" : 
									(
										{ "name" : "f3_account_text", "type" : "text", "x" : 0, "y" : 8, "fontsize": "LARGE", "text" : "Free Slot", "horizontal_align" : "center", "text_horizontal_align" : "center",},
									),
								},
								{
									"name" : "f3_save_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_save_account_01.png",
									"over_image" : NEW_PATH + "btn_save_account_02.png",
									"down_image" : NEW_PATH + "btn_save_account_03.png",
								},
								{
									"name" : "f3_delete_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_cancel_01.png",
									"over_image" : NEW_PATH + "btn_cancel_02.png",
									"down_image" : NEW_PATH + "btn_cancel_03.png",
								},
							),
						},
						{
							"name" : "f4_login_header",
							"type" : "window",
							"x" : -200, "y" : 50,
							"width" : 171, "height" : 32,
							"horizontal_align" : "center",
							"children" : 
							(
								{
									"name" : "f4_login_button",
									"type" : "button",
									"x" : 0, "y" : 0,
									"default_image" : NEW_PATH + "btn_acc_login_01.png",
									"over_image" : NEW_PATH + "btn_acc_login_02.png",
									"down_image" : NEW_PATH + "btn_acc_login_03.png",
									"children" : 
									(
										{ "name" : "f4_account_text", "type" : "text", "x" : 0, "y" : 8, "fontsize": "LARGE", "text" : "Free Slot", "horizontal_align" : "center", "text_horizontal_align" : "center",},
									),
								},
								{
									"name" : "f4_save_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_save_account_01.png",
									"over_image" : NEW_PATH + "btn_save_account_02.png",
									"down_image" : NEW_PATH + "btn_save_account_03.png",
								},
								{
									"name" : "f4_delete_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_cancel_01.png",
									"over_image" : NEW_PATH + "btn_cancel_02.png",
									"down_image" : NEW_PATH + "btn_cancel_03.png",
								},
							),
						},
						{
							"name" : "f5_login_header",
							"type" : "window",
							"x" : 0, "y" : 50,
							"width" : 171, "height" : 32,
							"horizontal_align" : "center",
							"children" : 
							(
								{
									"name" : "f5_login_button",
									"type" : "button",
									"x" : 0, "y" : 0,
									"default_image" : NEW_PATH + "btn_acc_login_01.png",
									"over_image" : NEW_PATH + "btn_acc_login_02.png",
									"down_image" : NEW_PATH + "btn_acc_login_03.png",
									"children" : 
									(
										{ "name" : "f5_account_text", "type" : "text", "x" : 0, "y" : 8, "fontsize": "LARGE", "text" : "Free Slot", "horizontal_align" : "center", "text_horizontal_align" : "center",},
									),
								},
								{
									"name" : "f5_save_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_save_account_01.png",
									"over_image" : NEW_PATH + "btn_save_account_02.png",
									"down_image" : NEW_PATH + "btn_save_account_03.png",
								},
								{
									"name" : "f5_delete_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_cancel_01.png",
									"over_image" : NEW_PATH + "btn_cancel_02.png",
									"down_image" : NEW_PATH + "btn_cancel_03.png",
								},
							),
						},
						{
							"name" : "f6_login_header",
							"type" : "window",
							"x" : 200, "y" : 50,
							"width" : 171, "height" : 32,
							"horizontal_align" : "center",
							"children" : 
							(
								{
									"name" : "f6_login_button",
									"type" : "button",
									"x" : 0, "y" : 0,
									"default_image" : NEW_PATH + "btn_acc_login_01.png",
									"over_image" : NEW_PATH + "btn_acc_login_02.png",
									"down_image" : NEW_PATH + "btn_acc_login_03.png",
									"children" : 
									(
										{ "name" : "f6_account_text", "type" : "text", "x" : 0, "y" : 8, "fontsize": "LARGE", "text" : "Free Slot", "horizontal_align" : "center", "text_horizontal_align" : "center",},
									),
								},
								{
									"name" : "f6_save_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_save_account_01.png",
									"over_image" : NEW_PATH + "btn_save_account_02.png",
									"down_image" : NEW_PATH + "btn_save_account_03.png",
								},
								{
									"name" : "f6_delete_button",
									"type" : "button",
									"x" : 36, "y" : 0,
									"horizontal_align" : "right",
									"default_image" : NEW_PATH + "btn_cancel_01.png",
									"over_image" : NEW_PATH + "btn_cancel_02.png",
									"down_image" : NEW_PATH + "btn_cancel_03.png",
								},
							),
						},
					),
				},
				{
					"name" : "LanguageBoard",
					"type" : "window",
					"x" : 0, "y" : 370,
					"width" : 350, "height" : 45,
					"horizontal_align" : "center",
					"vertical_align" : "center",
					"children" :
					(

						{
							"name": "flag_01",
							"type": "button",
							"x": 0,
							"y": 0,
							"vertical_align" : "center",
							"default_image" : NEW_PATH + "language/flag_en_0.tga",
							"over_image" : NEW_PATH + "language/flag_en_1.tga",
							"down_image" : NEW_PATH + "language/flag_en_2.tga",
						},
						{
							"name": "flag_02",
							"type": "button",
							"x": 70,
							"y": 0,
							"vertical_align" : "center",
							"default_image" : NEW_PATH + "language/flag_de_0.tga",
							"over_image" : NEW_PATH + "language/flag_de_1.tga",
							"down_image" : NEW_PATH + "language/flag_de_2.tga",
						},
						{
							"name": "flag_03",
							"type": "button",
							"x": 70*2,
							"y": 0,
							"vertical_align" : "center",
							"default_image" : NEW_PATH + "language/flag_tr_0.tga",
							"over_image" : NEW_PATH + "language/flag_tr_1.tga",
							"down_image" : NEW_PATH + "language/flag_tr_2.tga",
						},
						{
							"name": "flag_04",
							"type": "button",
							"x": 70*3,
							"y": 0,
							"vertical_align" : "center",
							"default_image" : NEW_PATH + "language/flag_ro_0.tga",
							"over_image" : NEW_PATH + "language/flag_ro_1.tga",
							"down_image" : NEW_PATH + "language/flag_ro_2.tga",
						},
						{
							"name": "flag_05",
							"type": "button",
							"x": 70*4,
							"y": 0,
							"vertical_align" : "center",
							"default_image" : NEW_PATH + "language/it_default.png",
							"over_image" : NEW_PATH + "language/it_over.png",
							"down_image" : NEW_PATH + "language/it_down.png",
						},
					),
				},
				{
					"name": "homepage_btn",
					"type": "button",
					"x": -70,
					# "x": 0,
					"y": 440,
					"horizontal_align" : "center",
					"vertical_align" : "center",
					"default_image" : NEW_PATH + "btn_homepage_01.png",
					"over_image" : NEW_PATH + "btn_homepage_02.png",
					"down_image" : NEW_PATH + "btn_homepage_03.png",
				},
				{
					"name": "discord_btn",
					"type": "button",
					"x": 75,
					"y": 440,
					"horizontal_align" : "center",
					"vertical_align" : "center",
					"default_image" : NEW_PATH + "btn_discord_01.png",
					"over_image" : NEW_PATH + "btn_discord_02.png",
					"down_image" : NEW_PATH + "btn_discord_03.png",
				},
			),
		},
	),
}