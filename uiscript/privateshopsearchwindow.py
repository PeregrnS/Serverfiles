import uiScriptLocale
import item
import app
import localeInfo

PRIVATESEARCH_PATH = "d:/ymir work/ui/privatesearch/"
PRIVATESHOP_PATH = "d:/ymir work/ui/game/premium_private_shop/"
GOLD_COLOR	= 0xFFFEE3AE
GREY_COLOR	= 0xFFA0A0A0
NEW_HEIGHT = 125
NEW_RECT_HEIGHT = 7.5
BOARD_WIDTH = 723
BOARD_HEIGHT = 410 + 125

window = {
	"name" : "PrivateShopSearchDialog",

	"x" : ( SCREEN_WIDTH / 2 ) - ( BOARD_WIDTH / 2 ),
	"y" : ( SCREEN_HEIGHT / 2 ) -( BOARD_HEIGHT / 2 ),

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "titlebar_finalcore_large1",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
			
			"title" : uiScriptLocale.PRIVATESHOPSEARCH_SEARCH_BAR,
		
			"children" :
			(
				{
					"name" : "LeftTop",
					"type" : "image",
					"x" : 7,
					"y" : 32,
					"image" : PRIVATESEARCH_PATH+"private_mainboxlefttop.sub",
				},

				{
					"name" : "RightTop",
					"type" : "image",
					"x" : 157,
					"y" : 32,
					"image" : PRIVATESEARCH_PATH+"private_mainboxrighttop.sub",
				},

				{
					"name" : "LeftBottom",
					"type" : "image",
					"x" : 7,
					"y" : 356 + NEW_HEIGHT,
					"image" : PRIVATESEARCH_PATH+"private_mainboxleftbottom.sub",
				},

				{
					"name" : "RightBottom",
					"type" : "image",
					"x" : 157,
					"y" : 356 + NEW_HEIGHT,
					"image" : PRIVATESEARCH_PATH+"private_mainboxrightbottom.sub",
				},

				{
					"name" : "leftcenterImg",
					"type" : "expanded_image",
					"x" : 7,
					"y" : 48,
					"image" : PRIVATESEARCH_PATH+"private_leftcenterImg.tga",
					"rect" : (0.0, 0.0, 0, 17.5 + NEW_RECT_HEIGHT),
				},

				{
					"name" : "rightcenterImg",
					"type" : "expanded_image",
					"x" : 156,
					"y" : 48,
					"image" : PRIVATESEARCH_PATH+"private_rightcenterImg.tga",
					"rect" : (0.0, 0.0, 0, 17.5 + NEW_RECT_HEIGHT),
				},

				{
					"name" : "topcenterImg",
					"type" : "expanded_image",
					"x" : 7 + 10,
					"y" : 32,
					"image" : PRIVATESEARCH_PATH+"private_topcenterImg.tga",
					"rect" : (0.0, 0.0, 7.5, 0),
				},

				{
					"name" : "bottomcenterImg",
					"type" : "expanded_image",
					"x" : 7 + 10,
					"y" : 356 + NEW_HEIGHT,
					"image" : PRIVATESEARCH_PATH+"private_bottomcenterImg.tga",
					"rect" : (0.0, 0.0, 7.5, 0),
				},

				{
					"name" : "centerImg",
					"type" : "expanded_image",
					"x" : 7 + 10,
					"y" : 48,
					"image" : PRIVATESEARCH_PATH+"private_centerImg.tga",
					"rect" : (0.0, 0.0, 7.5, 17.5 + NEW_RECT_HEIGHT),
				},

				## ItemName
				{
					"name" : "ItemNameBackground",
					"type" : "expanded_image",

					"x" : 11,
					"y" : 35,

					"image" : "d:/ymir work/ui/tab_menu_02.tga",

					"children" :
					(
						{ "name" : "ItemNameText", "type" : "text", "text_horizontal_align" : "center", "horizontal_align" : "center", "x" : 0, "y" : 5, "text" : uiScriptLocale.PRIVATESHOPSEARCH_ITEMNAME, "color" : GOLD_COLOR },
					),
				},
				
				## ItemNameEditLine
				{
					"name" : "ItemNameSlot",
					"type" : "expanded_image",
					
					"x" : 10,
					"y" : 59,

					"x_scale" : 1.22,
					"y_scale" : 1.00,
					
					"image" : PRIVATESEARCH_PATH+"private_slot_dynamic.tga",
					
					"children" :
					(
						{
							"name" : "NameInput",
							"type" : "editline",
							"x" : 4,
							"y" : 3,
							"width" : 150,
							"height" : 15,
							"input_limit" : 20,
							"text" : "",
						},
					),
				},

				## FilterButton
				{
					"name" : "FilterButton",
					"type" : "button",
					
					"x" : 128 + 22,
					"y" : 58,

					"default_image" : PRIVATESHOP_PATH + "filter_button_default.sub",
					"over_image" : PRIVATESHOP_PATH + "filter_button_over.sub",
					"down_image" : PRIVATESHOP_PATH + "filter_button_down.sub",
				},

				## ItemModeButton
				{
					"name" : "ItemModeButton",
					"type" : "radio_button",
					
					"x" : 15,
					"y" : 80,

					"default_image" : PRIVATESHOP_PATH + "_item_mode_button_default_2.sub",
					"over_image" : PRIVATESHOP_PATH + "_item_mode_button_over_2.sub",
					"down_image" : PRIVATESHOP_PATH + "_item_mode_button_down_2.sub",
				},

				## PlayerModeButton
				{
					"name" : "PlayerModeButton",
					"type" : "radio_button",
					
					# "x" : 11 + (38 + 2) * 1,
					"x" : 91,
					"y" : 80,

					"default_image" : PRIVATESHOP_PATH + "_player_mode_button_default_2.sub",
					"over_image" : PRIVATESHOP_PATH + "_player_mode_button_over_2.sub",
					"down_image" : PRIVATESHOP_PATH + "_player_mode_button_down_2.sub",
				},

				## FilterBackground
				{
					"name" : "FilterBackground",
					"type" : "expanded_image",

					"x" : 11,
					"y" : 102 + 5,

					"image" : "d:/ymir work/ui/tab_menu_02.tga",

					"children" :
					(
						{ "name" : "ItemNameText", "type" : "text", "text_horizontal_align" : "center", "horizontal_align" : "center", "x" : 0, "y" : 5, "text" : uiScriptLocale.PRIVATESHOPSEARCH_FILTER_TITLE, "color" : GOLD_COLOR },
					),
				},

				{
					"name" : "CategoryWindow",
					"type" : "window",

					"x" : 8,
					"y" : 125 + 5,

					"width" : 164,
					"height" : 245 + NEW_HEIGHT,

					"children" : 
					(
						## CategoryScrollBar
						{
							"name"	: "CategoryScrollBar",
							"type"	: "scrollbar",
							
							"x"		: 166 - 18,
							"y"		: 0,
							
							"size"	: 245 + NEW_HEIGHT,
						},
					),
				},

				## FindButton
				{
					"name" : "SearchButton",
					"type" : "button",

					"x" : 50,
					"y" : 338 + 38 + NEW_HEIGHT,

					"default_image" : PRIVATESHOP_PATH + "search_button_default.sub",
					"over_image" : PRIVATESHOP_PATH + "search_button_over.sub",
					"down_image" : PRIVATESHOP_PATH + "search_button_down.sub",
				},

				## BuyButton
				{
					"name" : "BuyButton",
					"type" : "button",

					"x" : BOARD_WIDTH - 137,
					"y" : 338 + 38 + NEW_HEIGHT,

					"default_image" : PRIVATESHOP_PATH + "buy_button_default.sub",
					"over_image" : PRIVATESHOP_PATH + "buy_button_over.sub",
					"down_image" : PRIVATESHOP_PATH + "buy_button_down.sub",
				},

				## ClearSelectedItemButton
				{
					"name" : "ClearSelectedItemButton",
					"type" : "button",

					"x" : BOARD_WIDTH - 48,
					"y" : 338 + 38 + NEW_HEIGHT,

					"default_image" : PRIVATESHOP_PATH + "clear_selected_item_button_default.sub",
					"over_image" : PRIVATESHOP_PATH + "clear_selected_item_button_over.sub",
					"down_image" : PRIVATESHOP_PATH + "clear_selected_item_button_down.sub",
				},

				## LeftTop
				{
					"name" : "LeftTop",
					"type" : "image",
					"x" : 133 + 40,
					"y" : 32,
					"image" : PRIVATESEARCH_PATH+"private_mainboxlefttop.sub",
				},
				## RightTop
				{
					"name" : "RightTop",
					"type" : "image",
					"x" : 659 + 40,
					"y" : 32,
					"image" : PRIVATESEARCH_PATH+"private_mainboxrighttop.sub",
				},
				## LeftBottom
				{
					"name" : "LeftBottom",
					"type" : "image",
					"x" : 133 + 40,
					"y" : 356 + NEW_HEIGHT,
					"image" : PRIVATESEARCH_PATH+"private_mainboxleftbottom.sub",
				},
				## RightBottom
				{
					"name" : "RightBottom",
					"type" : "image",
					"x" : 659 + 40,
					"y" : 356 + NEW_HEIGHT,
					"image" : PRIVATESEARCH_PATH+"private_mainboxrightbottom.sub",
				},
				## leftcenterImg
				{
					"name" : "leftcenterImg",
					"type" : "expanded_image",
					"x" : 133 + 40,
					"y" : 48,
					"image" : PRIVATESEARCH_PATH+"private_leftcenterImg.tga",
					"rect" : (0.0, 0.0, 0, 17.5 + NEW_RECT_HEIGHT),
				},
				## rightcenterImg
				{
					"name" : "rightcenterImg",
					"type" : "expanded_image",
					"x" : 658 + 40,
					"y" : 48,
					"image" : PRIVATESEARCH_PATH+"private_rightcenterImg.tga",
					"rect" : (0.0, 0.0, 0, 17.5 + NEW_RECT_HEIGHT),
				},
				## topcenterImg
				{
					"name" : "topcenterImg",
					"type" : "expanded_image",
					"x" : 149 + 40,
					"y" : 32,
					"image" : PRIVATESEARCH_PATH+"private_topcenterImg.tga",
					"rect" : (0.0, 0.0, 29, 0),
				},
				## bottomcenterImg
				{
					"name" : "bottomcenterImg",
					"type" : "expanded_image",
					"x" : 149 + 40,
					"y" : 356 + NEW_HEIGHT,
					"image" : PRIVATESEARCH_PATH+"private_bottomcenterImg.tga",
					"rect" : (0.0, 0.0, 29, 0),
				},
				## centerImg
				{
					"name" : "centerImg",
					"type" : "expanded_image",
					"x" : 149 + 40,
					"y" : 48,
					"image" : PRIVATESEARCH_PATH+"private_centerImg.tga",
					"rect" : (0.0, 0.0, 29, 17.5 + NEW_RECT_HEIGHT),
				},

				## tab_menu_01
				{
					"name" : "ItemTypeImg",
					"type" : "expanded_image",
					"x" : 136 + 40,
					"y" : 35,
					"width" : 10,
					"image" : "d:/ymir work/ui/tab_menu_01.tga",
					"x_scale" : 1.22, 
					"y_scale" : 1.0,
					"children" :
					(
						## Text
						{ "name" : "ResultNameText1", "type" : "text", "x" : 45, "y" : 4,  "text" : uiScriptLocale.PRIVATESHOPSEARCH_COUNT, },
						{ "name" : "ResultNameText2", "type" : "text", "x" : 100, "y" : 4, "text" : uiScriptLocale.PRIVATESHOPSEARCH_ITEMNAME, },
						# { "name" : "ResultNameText3", "type" : "text", "x" : 215, "y" : 4, "text" : localeInfo.CHEQUE_SYSTEM_UNIT_WON, },
						{ "name" : "ResultNameText4", "type" : "text", "x" : 250, "y" : 4, "text" : localeInfo.CHEQUE_SYSTEM_UNIT_YANG, },
						{ "name" : "ResultNameText5", "type" : "text", "x" : 415, "y" : 4, "text" : uiScriptLocale.PRIVATESHOPSEARCH_SELLER, },
					),
				},

				{
					"name" : "ItemResultWindow",
					"type" : "window",

					"x" : 173,
					"y" : 57,

					"width" : 538,
					"height" : 314 + NEW_HEIGHT,

					"children" :
					(
						{
							"name" : "ItemResultWindowMask",
							"type" : "window",

							"x" : 0,
							"y" : 0,

							"width" : 538 - 14,
							"height" : 312 + NEW_HEIGHT,

							# ENABLE_PRIVATE_SHOP_NO_RESULT_UPDATE
							"children" :
							(
								{
									"name" : "noResultsImage",
									"type" : "expanded_image",
									"style" : ("attach",),

									"x" : 262 - 99,
									"y" : 202 - 66,

									"x_scale" : 1.0,
									"y_scale" : 1.0,

									"image" : "d:/ymir work/ui/privatesearch/no_results_with_text_bg.tga",
								},
								{
									"name" : "noResultsText",
									"type" : "text",

									"x" : 10,
									"y" : 250,

									"text_horizontal_align" : "center",
									"horizontal_align" : "center",

									"text" : localeInfo.PRIVATE_SHOP_SEARCH_VNUM_ITEM_NOT_FOUND,
									"color" : GOLD_COLOR,
								},
							),
						},

						{
							"name" : "ItemResultScrollBar",
							"type" : "scrollbar",

							"x" : 538 - 15,
							"y" : 0,

							"size" : 313 + NEW_HEIGHT,
						},

					),
				},

				{
					"name" : "FirstPrevButton", "type" : "button",
					"x" : 230+20 + 40, "y" : 368 + 15 + NEW_HEIGHT,

					"default_image" : PRIVATESEARCH_PATH + "private_first_prev_btn_01.sub",
					"over_image" 	: PRIVATESEARCH_PATH + "private_first_prev_btn_02.sub",
					"down_image" 	: PRIVATESEARCH_PATH + "private_first_prev_btn_01.sub",
				},
				{
					"name" : "PrevButton", "type" : "button",
					"x" : 260+20 + 40, "y" : 368 + 15 + NEW_HEIGHT,

					"default_image" : PRIVATESEARCH_PATH + "private_prev_btn_01.sub",
					"over_image" 	: PRIVATESEARCH_PATH + "private_prev_btn_02.sub",
					"down_image" 	: PRIVATESEARCH_PATH + "private_prev_btn_01.sub",
				},
				{
					"name" : "Page1Button", "type" : "button",
					"x" : 275+30 + 40, "y" : 368 + 12 + NEW_HEIGHT,

					"text" : "1",

					"default_image" : PRIVATESEARCH_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "Page2Button", "type" : "button",
					"x" : 310+30 + 40, "y" : 368 + 12 + NEW_HEIGHT,

					"text" : "2",

					"default_image" : PRIVATESEARCH_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "Page3Button", "type" : "button",
					"x" : 345+30 + 40, "y" : 368 + 12 + NEW_HEIGHT,
					
					"text" : "3",

					"default_image" : PRIVATESEARCH_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "Page4Button", "type" : "button",
					"x" : 380+30 + 40, "y" : 368 + 12 + NEW_HEIGHT,

					"text" : "4",

					"default_image" : PRIVATESEARCH_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "Page5Button", "type" : "button",
					"x" : 415+30 + 40, "y" : 368 + 12 + NEW_HEIGHT,

					"text" : "5",

					"default_image" : PRIVATESEARCH_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PRIVATESEARCH_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "NextButton", "type" : "button",
					"x" : 453+40 + 40, "y" : 368 + 15 + NEW_HEIGHT,

					"default_image" : PRIVATESEARCH_PATH + "private_next_btn_01.sub",
					"over_image" 	: PRIVATESEARCH_PATH + "private_next_btn_02.sub",
					"down_image" 	: PRIVATESEARCH_PATH + "private_next_btn_01.sub",
				},
				{
					"name" : "LastNextButton", "type" : "button",
					"x" : 518+40, "y" : 368 + 15 + NEW_HEIGHT,

					"default_image" : PRIVATESEARCH_PATH + "private_last_next_btn_01.sub",
					"over_image" 	: PRIVATESEARCH_PATH + "private_last_next_btn_02.sub",
					"down_image" 	: PRIVATESEARCH_PATH + "private_last_next_btn_01.sub",
				},

				{
					"name" : "FilterWindow",
					"type" : "window",

					"x" : 7,
					"y" : 122,

					"width" : 166,
					"height" : 245 + NEW_HEIGHT,

					"children" : 
					(
						## GeneralFilterModeButton
						{
							"name" : "GeneralFilterModeButton",
							"type" : "radio_button",
							
							"x" : -38 - 5,
							"y" : 220,

							"horizontal_align" : "center",

							"default_image" : PRIVATESHOP_PATH + "general_filter_mode_button_default.sub",
							"over_image" : PRIVATESHOP_PATH + "general_filter_mode_button_over.sub",
							"down_image" : PRIVATESHOP_PATH + "general_filter_mode_button_down.sub",
						},

						## AttrFilterModeButton
						{
							"name" : "AttrFilterModeButton",
							"type" : "radio_button",
							
							"x" : 0,
							"y" : 220,

							"horizontal_align" : "center",

							"default_image" : PRIVATESHOP_PATH + "attributes_filter_mode_button_default.sub",
							"over_image" : PRIVATESHOP_PATH + "attributes_filter_mode_button_over.sub",
							"down_image" : PRIVATESHOP_PATH + "attributes_filter_mode_button_down.sub",
						},

						## ClearFilterModeButton
						{
							"name" : "ClearFilterButton",
							"type" : "button",
							
							# "x" : 38 + 5,

							"x" : 0,
							"y" : 220 + NEW_HEIGHT,

							"horizontal_align" : "center",

							"default_image" : PRIVATESHOP_PATH + "clear_filter_button_default.sub",
							"over_image" : PRIVATESHOP_PATH + "clear_filter_button_over.sub",
							"down_image" : PRIVATESHOP_PATH + "clear_filter_button_down.sub",
						},
					),
				},
			),
		},
	),
}