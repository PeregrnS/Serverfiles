# fratello_exe login screen v1.0

ROOT_PATH = "fratello_work/login/ui/"

window = {
	"name" : "LoginWindow",
	"style" : ("float",),

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	[
		# FULL BACKGROUND
		{
			"name" : "background",
			"type" : "expanded_image",
			"x" : 0,
			"y" : 0,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"image" : ROOT_PATH + "background.png",
		},
		{
			"name" : "background_panel",
			"type" : "expanded_image",
			"x" : 0,
			"y" : 0,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"image" : ROOT_PATH + "background_panel.png",
		},
		# MAIN FRAME
		{
			"name" : "board",
			"type" : "image",
			"x" : 0,
			"y" : 0,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"image" : ROOT_PATH + "board.png",

			"children" :
			[
				# LOGO NAME (IM BOARD)
				{
					"name" : "logo_name",
					"type" : "image",

					"x" : 0,
					"y" : 160,
					"horizontal_align" : "center",

					"image" : ROOT_PATH + "logo_name.png",
				},
				# ACCOUNT PANEL IMAGE (MITTIG)
				{
					"name" : "account_panel",
					"type" : "image",
					"x" : 0,
					"y" : 235,
					"horizontal_align" : "center",
					"image" : ROOT_PATH + "account.png",

					"children" :
					[
						# ---------------- ID ----------------
						{
							"name" : "id",
							"type" : "editline",
							"x" : 80,
							"y" : 52,
							"horizontal_align" : "center",
							"width" : 360,
							"height" : 30,
							"input_limit" : 30,
							"text" : "",
						},
						{
							"name" : "id_placeholder",
							"type" : "text",
							"x" : -92,
							"y" : 52,
							"horizontal_align" : "center",
							"text" : "Account",
							"color" : 0x66FFFFFF,
						},

						# ---------------- PASSWORD ----------------
						{
							"name" : "pwd",
							"type" : "editline",
							"x" : 80,
							"y" : 118,
							"horizontal_align" : "center",
							"width" : 360,
							"height" : 30,
							"input_limit" : 30,
							"secret_flag" : 1,
							"text" : "",
						},
						{
							"name" : "pwd_placeholder",
							"type" : "text",
							"x" : -92,
							"y" : 118,
							"horizontal_align" : "center",
							"text" : "Password",
							"color" : 0x66FFFFFF,
						},

						# ---------------- PIN ----------------
						{
							"name" : "pin",
							"type" : "editline",
							"x" : 80,
							"y" : 172,
							"horizontal_align" : "center",
							"width" : 200,
							"height" : 30,
							"input_limit" : 4,
							"secret_flag" : 1,
							"text" : "",
						},
						{
							"name" : "pin_placeholder",
							"type" : "text",
							"x" : 0,
							"y" : 171,
							"horizontal_align" : "center",
							"text" : "PIN",
							"color" : 0x66FFFFFF,
						},
					]
				},
				{
					"name" : "account_list",
					"type" : "image",
			
					"x" : 265,
					"y" : 0,
					"horizontal_align" : "center",
					"vertical_align" : "center",
					"image" : ROOT_PATH + "list.png",
				},
				{
					"name" : "account_name",
					"type" : "image",

					"x" : 265,
					"y" : 277,
					"horizontal_align" : "center",

					"image" : ROOT_PATH + "account_name.png",
				},
				{
					"name" : "server_list",
					"type" : "image",
			
					"x" : -265,
					"y" : 0,
					"horizontal_align" : "center",
					"vertical_align" : "center",
					"image" : ROOT_PATH + "list.png",
				},
				{
					"name" : "server_name",
					"type" : "image",

					"x" : -265,
					"y" : 275,
					"horizontal_align" : "center",

					"image" : ROOT_PATH + "server.png",
				},
				{
					"name" : "server_list_normal",
					"type" : "button",
					"x" : -264,
					"y" : 300,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "server_list_btn_normal.png",
					"over_image" : ROOT_PATH + "server_list_btn_hover.png",
					"down_image" : ROOT_PATH + "server_list_btn_press.png",

					"children" :
					[
						{
							"name" : "server",
							"type" : "image",
							"x" : 17,
							"y" : 10,
							"image" : ROOT_PATH + "logo_name_small.png",
							"style" : ("not_pick",),
						},
					],
				},
				{
					"name" : "server_list_dev",
					"type" : "button",
					"x" : -264,
					"y" : 340,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "server_list_btn_normal.png",
					"over_image" : ROOT_PATH + "server_list_btn_hover.png",
					"down_image" : ROOT_PATH + "server_list_btn_press.png",

					"children" :
					[
						{
							"name" : "server_dev",
							"type" : "image",
							"x" : 40,
							"y" : 9,
							"image" : ROOT_PATH + "logo_dev.png",
							"style" : ("not_pick",),
						},
					],
				},

				{
					"name" : "fill",
					"type" : "image",
			
					"x" : -265,
					"y" : 7,
					"horizontal_align" : "center",
					"vertical_align" : "center",
					"image" : ROOT_PATH + "fill.png",
				},

				{
					"name" : "server_list_website",
					"type" : "button",
					"x" : -264,
					"y" : 400,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "server_list_btn_normal.png",
					"over_image" : ROOT_PATH + "server_list_btn_hover.png",
					"down_image" : ROOT_PATH + "server_list_btn_press.png",

					"children" :
					[
						{
							"name" : "server_website",
							"type" : "image",
							"x" : 22,
							"y" : 10,
							"image" : ROOT_PATH + "website.png",
							"style" : ("not_pick",),
						},
					],
				},
				{
					"name" : "server_list_discord",
					"type" : "button",
					"x" : -264,
					"y" : 440,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "server_list_btn_normal.png",
					"over_image" : ROOT_PATH + "server_list_btn_hover.png",
					"down_image" : ROOT_PATH + "server_list_btn_press.png",

					"children" :
					[
						{
							"name" : "server_discord",
							"type" : "image",
							"x" : 22,
							"y" : 10,
							"image" : ROOT_PATH + "discord.png",
							"style" : ("not_pick",),
						},
					],
				},
				{
					"name" : "save_account1",
					"type" : "button",
					"x" : 267,
					"y" : 295,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "account_save_btn_normal.png",
					"over_image" : ROOT_PATH + "account_save_btn_hover.png",
					"down_image" : ROOT_PATH + "account_save_btn_press.png",
					"children" :
					(
						{
							"name" : "save_account1_text",
							"type" : "text",
							"x" : -13,
							"y" : -7,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"text" : "",
						},
						{
							"name" : "save_account1_button",
							"type" : "button",
							"x" : 45,
							"y" : 0,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_btn_normal.png",
							"over_image" : ROOT_PATH + "save_btn_hover.png",
							"down_image" : ROOT_PATH + "save_btn_press.png",
						},
						{
							"name" : "save_account1_delete",
							"type" : "button",
							"x" : 105,
							"y" : 0,
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_delete_btn_normal.png",
							"over_image" : ROOT_PATH + "save_delete_btn_hover.png",
							"down_image" : ROOT_PATH + "save_delete_btn_press.png",
						},
					),
				},

				{
					"name" : "save_account2",
					"type" : "button",
					"x" : 267,
					"y" : 323,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "account_save_btn_normal.png",
					"over_image" : ROOT_PATH + "account_save_btn_hover.png",
					"down_image" : ROOT_PATH + "account_save_btn_press.png",
					"children" :
					(
						{
							"name" : "save_account2_text",
							"type" : "text",
							"x" : -13,
							"y" : -7,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"text" : "",
						},
						{
							"name" : "save_account2_button",
							"type" : "button",
							"x" : 45,
							"y" : 0,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_btn_normal.png",
							"over_image" : ROOT_PATH + "save_btn_hover.png",
							"down_image" : ROOT_PATH + "save_btn_press.png",
						},
						{
							"name" : "save_account2_delete",
							"type" : "button",
							"x" : 105,
							"y" : 0,
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_delete_btn_normal.png",
							"over_image" : ROOT_PATH + "save_delete_btn_hover.png",
							"down_image" : ROOT_PATH + "save_delete_btn_press.png",
						},
					),
				},

				{
					"name" : "save_account3",
					"type" : "button",
					"x" : 267,
					"y" : 351,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "account_save_btn_normal.png",
					"over_image" : ROOT_PATH + "account_save_btn_hover.png",
					"down_image" : ROOT_PATH + "account_save_btn_press.png",
					"children" :
					(
						{
							"name" : "save_account3_text",
							"type" : "text",
							"x" : -13,
							"y" : -7,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"text" : "",
						},
						{
							"name" : "save_account3_button",
							"type" : "button",
							"x" : 45,
							"y" : 0,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_btn_normal.png",
							"over_image" : ROOT_PATH + "save_btn_hover.png",
							"down_image" : ROOT_PATH + "save_btn_press.png",
						},
						{
							"name" : "save_account3_delete",
							"type" : "button",
							"x" : 105,
							"y" : 0,
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_delete_btn_normal.png",
							"over_image" : ROOT_PATH + "save_delete_btn_hover.png",
							"down_image" : ROOT_PATH + "save_delete_btn_press.png",
						},
					),
				},

				{
					"name" : "save_account4",
					"type" : "button",
					"x" : 267,
					"y" : 379,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "account_save_btn_normal.png",
					"over_image" : ROOT_PATH + "account_save_btn_hover.png",
					"down_image" : ROOT_PATH + "account_save_btn_press.png",
					"children" :
					(
						{
							"name" : "save_account4_text",
							"type" : "text",
							"x" : -13,
							"y" : -7,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"text" : "",
						},
						{
							"name" : "save_account4_button",
							"type" : "button",
							"x" : 45,
							"y" : 0,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_btn_normal.png",
							"over_image" : ROOT_PATH + "save_btn_hover.png",
							"down_image" : ROOT_PATH + "save_btn_press.png",
						},
						{
							"name" : "save_account4_delete",
							"type" : "button",
							"x" : 105,
							"y" : 0,
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_delete_btn_normal.png",
							"over_image" : ROOT_PATH + "save_delete_btn_hover.png",
							"down_image" : ROOT_PATH + "save_delete_btn_press.png",
						},
					),
				},

				{
					"name" : "save_account5",
					"type" : "button",
					"x" : 267,
					"y" : 407,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "account_save_btn_normal.png",
					"over_image" : ROOT_PATH + "account_save_btn_hover.png",
					"down_image" : ROOT_PATH + "account_save_btn_press.png",
					"children" :
					(
						{
							"name" : "save_account5_text",
							"type" : "text",
							"x" : -13,
							"y" : -7,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"text" : "",
						},
						{
							"name" : "save_account5_button",
							"type" : "button",
							"x" : 45,
							"y" : 0,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_btn_normal.png",
							"over_image" : ROOT_PATH + "save_btn_hover.png",
							"down_image" : ROOT_PATH + "save_btn_press.png",
						},
						{
							"name" : "save_account5_delete",
							"type" : "button",
							"x" : 105,
							"y" : 0,
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_delete_btn_normal.png",
							"over_image" : ROOT_PATH + "save_delete_btn_hover.png",
							"down_image" : ROOT_PATH + "save_delete_btn_press.png",
						},
					),
				},

				{
					"name" : "save_account6",
					"type" : "button",
					"x" : 267,
					"y" : 435,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "account_save_btn_normal.png",
					"over_image" : ROOT_PATH + "account_save_btn_hover.png",
					"down_image" : ROOT_PATH + "account_save_btn_press.png",
					"children" :
					(
						{
							"name" : "save_account6_text",
							"type" : "text",
							"x" : -13,
							"y" : -7,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"text" : "",
						},
						{
							"name" : "save_account6_button",
							"type" : "button",
							"x" : 45,
							"y" : 0,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_btn_normal.png",
							"over_image" : ROOT_PATH + "save_btn_hover.png",
							"down_image" : ROOT_PATH + "save_btn_press.png",
						},
						{
							"name" : "save_account6_delete",
							"type" : "button",
							"x" : 105,
							"y" : 0,
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_delete_btn_normal.png",
							"over_image" : ROOT_PATH + "save_delete_btn_hover.png",
							"down_image" : ROOT_PATH + "save_delete_btn_press.png",
						},
					),
				},

				{
					"name" : "save_account7",
					"type" : "button",
					"x" : 267,
					"y" : 463,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "account_save_btn_normal.png",
					"over_image" : ROOT_PATH + "account_save_btn_hover.png",
					"down_image" : ROOT_PATH + "account_save_btn_press.png",
					"children" :
					(
						{
							"name" : "save_account7_text",
							"type" : "text",
							"x" : -13,
							"y" : -8,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"text" : "",
						},
						{
							"name" : "save_account7_button",
							"type" : "button",
							"x" : 45,
							"y" : 0,
							"horizontal_align" : "center",
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_btn_normal.png",
							"over_image" : ROOT_PATH + "save_btn_hover.png",
							"down_image" : ROOT_PATH + "save_btn_press.png",
						},
						{
							"name" : "save_account7_delete",
							"type" : "button",
							"x" : 105,
							"y" : 0,
							"vertical_align" : "center",
							"default_image" : ROOT_PATH + "save_delete_btn_normal.png",
							"over_image" : ROOT_PATH + "save_delete_btn_hover.png",
							"down_image" : ROOT_PATH + "save_delete_btn_press.png",
						},
					),
				},
				# ---------------- LOGIN BUTTON ----------------
				{
					"name" : "login_button",
					"type" : "button",

					"x" : 0,
					"y" : 460,
					"horizontal_align" : "center",

					"default_image" : ROOT_PATH + "login_btn_normal.png",
					"over_image" : ROOT_PATH + "login_btn_hover.png",
					"down_image" : ROOT_PATH + "login_btn_press.png",
				},

				# ---------------- LANGUAGE FLAGS ----------------
				{
					"name" : "lang_flag_tr",
					"type" : "button",
					"x" : -105,
					"y" : 525,
					"horizontal_align" : "center",
					"default_image" : "d:/ymir work/ui/login/login/language/flag_en_1.tga",
					"over_image" : "d:/ymir work/ui/login/login/language/flag_en_2.tga",
					"down_image" : "d:/ymir work/ui/login/login/language/flag_en_2.tga",
				},
				{
					"name" : "lang_flag_en",
					"type" : "button",
					"x" : -35,
					"y" : 525,
					"horizontal_align" : "center",
					"default_image" : "d:/ymir work/ui/login/login/language/flag_de_1.tga",
					"over_image" : "d:/ymir work/ui/login/login/language/flag_de_2.tga",
					"down_image" : "d:/ymir work/ui/login/login/language/flag_de_2.tga",
				},
				{
					"name" : "lang_flag_de",
					"type" : "button",
					"x" : 35,
					"y" : 525,
					"horizontal_align" : "center",
					"default_image" : "d:/ymir work/ui/login/login/language/flag_tr_1.tga",
					"over_image" : "d:/ymir work/ui/login/login/language/flag_tr_2.tga",
					"down_image" : "d:/ymir work/ui/login/login/language/flag_tr_2.tga",
				},
				{
					"name" : "lang_flag_ro",
					"type" : "button",
					"x" : 105,
					"y" : 525,
					"horizontal_align" : "center",
					"default_image" : "d:/ymir work/ui/login/login/language/flag_ro_1.tga",
					"over_image" : "d:/ymir work/ui/login/login/language/flag_ro_2.tga",
					"down_image" : "d:/ymir work/ui/login/login/language/flag_ro_2.tga",
				},

				{
					"name" : "select_btn_ch1",
					"type" : "button",
					"x" : -300,
					"y" : 601,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "channel_btn_normal.png",
					"over_image" : ROOT_PATH + "channel_btn_hover.png",
					"down_image" : ROOT_PATH + "channel_btn_press.png",
					"text" : "CH1",

					"children" :
					[
						{
							"name" : "ch1_state",
							"type" : "image",
							"x" : 10,
							"y" : 8,
							"image" : ROOT_PATH + "yellow.png",
							"style" : ("not_pick",),
						},
					],
				},
				{
					"name" : "select_btn_ch2",
					"type" : "button",
					"x" : -200,
					"y" : 601,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "channel_btn_normal.png",
					"over_image" : ROOT_PATH + "channel_btn_hover.png",
					"down_image" : ROOT_PATH + "channel_btn_press.png",
					"text" : "CH2",

					"children" :
					[
						{
							"name" : "ch2_state",
							"type" : "image",
							"x" : 10,
							"y" : 8,
							"image" : ROOT_PATH + "yellow.png",
							"style" : ("not_pick",),
						},
					],
				},
				{
					"name" : "select_btn_ch3",
					"type" : "button",
					"x" : -100,
					"y" : 601,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "channel_btn_normal.png",
					"over_image" : ROOT_PATH + "channel_btn_hover.png",
					"down_image" : ROOT_PATH + "channel_btn_press.png",
					"text" : "CH3",

					"children" :
					[
						{
							"name" : "ch3_state",
							"type" : "image",
							"x" : 10,
							"y" : 8,
							"image" : ROOT_PATH + "yellow.png",
							"style" : ("not_pick",),
						},
					],
				},
				{
					"name" : "select_btn_ch4",
					"type" : "button",
					"x" : 0,
					"y" : 601,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "channel_btn_normal.png",
					"over_image" : ROOT_PATH + "channel_btn_hover.png",
					"down_image" : ROOT_PATH + "channel_btn_press.png",
					"text" : "CH4",

					"children" :
					[
						{
							"name" : "ch4_state",
							"type" : "image",
							"x" : 10,
							"y" : 8,
							"image" : ROOT_PATH + "yellow.png",
							"style" : ("not_pick",),
						},
					],
				},
				{
					"name" : "select_btn_ch5",
					"type" : "button",
					"x" : 100,
					"y" : 601,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "channel_btn_normal.png",
					"over_image" : ROOT_PATH + "channel_btn_hover.png",
					"down_image" : ROOT_PATH + "channel_btn_press.png",
					"text" : "CH5",

					"children" :
					[
						{
							"name" : "ch5_state",
							"type" : "image",
							"x" : 10,
							"y" : 8,
							"image" : ROOT_PATH + "yellow.png",
							"style" : ("not_pick",),
						},
					],
				},
				{
					"name" : "select_btn_ch6",
					"type" : "button",
					"x" : 200,
					"y" : 601,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "channel_btn_normal.png",
					"over_image" : ROOT_PATH + "channel_btn_hover.png",
					"down_image" : ROOT_PATH + "channel_btn_press.png",
					"text" : "CH6",

					"children" :
					[
						{
							"name" : "ch6_state",
							"type" : "image",
							"x" : 10,
							"y" : 8,
							"image" : ROOT_PATH + "yellow.png",
							"style" : ("not_pick",),
						},
					],
				},
				{
					"name" : "select_btn_ch7",
					"type" : "button",
					"x" : 300,
					"y" : 601,
					"horizontal_align" : "center",
					"default_image" : ROOT_PATH + "channel_btn_normal.png",
					"over_image" : ROOT_PATH + "channel_btn_hover.png",
					"down_image" : ROOT_PATH + "channel_btn_press.png",
					"text" : "CH7",

					"children" :
					[
						{
							"name" : "ch7_state",
							"type" : "image",
							"x" : 10,
							"y" : 8,
							"image" : ROOT_PATH + "yellow.png",
							"style" : ("not_pick",),
						},
					],
				},
			],
		},
	],
}