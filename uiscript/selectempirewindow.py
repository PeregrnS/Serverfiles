PATH = "d:/ymir work/ui/login/empire/"
PATH_SELECT = "d:/ymir work/ui/login/select/"

window = {
	"name" : "SelectEmpireWindow",
	"x" : 0, "y" : 0, "width" : SCREEN_WIDTH, "height" : SCREEN_HEIGHT,
	"style" : ("float",),
	"children" :
	(
		{
			"name" : "Background_1",
			"type" : "expanded_image",
			"x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1920.0,
			"y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"image" : PATH + "bg.png",
		},
		#RED
		{
			"name" : "red", "type" : "image",
			"x" : -500, "y" : 0,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"image" : PATH + "shinso.png",
			"children":
			(
				{
					"name" : "select_0",
					"type" : "radio_button",
					"horizontal_align" : "center",
					"vertical_align" : "bottom",
					"x" : 0, "y" : 65,
					"default_image": PATH + "bttn_select_01.png",
					"over_image": PATH + "bttn_select_02.png",
					"down_image": PATH + "bttn_select_03.png",
				},
			),
		},
		#YELLOW
		{
			"name" : "yellow", "type" : "image",
			"x" : 0, "y" : 0,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"image" : PATH + "chunjo.png",
			"children":
			(
				{
					"name" : "select_1",
					"type" : "radio_button",
					"horizontal_align" : "center",
					"vertical_align" : "bottom",
					"x" : 0, "y" : 65,
					"default_image": PATH + "bttn_select_01.png",
					"over_image": PATH + "bttn_select_02.png",
					"down_image": PATH + "bttn_select_03.png",
				},
			),
		},
		

		#BLUE
		{
			"name" : "yellow", "type" : "image",
			"x" : 500, "y" : 0,
			"horizontal_align" : "center",
			"vertical_align" : "center",
			"image" : PATH + "jinno.png",
			"children":
			(
				{
					"name" : "select_2",
					"type" : "radio_button",
					"horizontal_align" : "center",
					"vertical_align" : "bottom",
					"x" : 0, "y" : 65,
					"default_image": PATH + "bttn_select_01.png",
					"over_image": PATH + "bttn_select_02.png",
					"down_image": PATH + "bttn_select_03.png",
				},
			),
		},
	),
}
