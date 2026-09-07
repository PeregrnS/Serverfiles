# coding: latin_1

import uiScriptLocale

IMG_DIR = "d:/ymir work/ui/game/gameoption/"
IMG_PICKUP_DIR = "d:/ymir work/ui/game/gameoption/pickup/"

TITLE_IMAGE_TEXT_X = 5
TITLE_IMAGE_TEXT_Y = 4

OPTION_START_X = 17
SLIDER_POSITION_X = 50

SLIDER_START_Y = 40
BUTTON_START_Y = 33
BUTTON_NEXT_Y = 20

RADIO_BUTTON_RANGE_X = 65
TOGGLE_BUTTON_RANGE_X = 85
TOGGLE_BUTTON_RANGE_X_NEW = 70

RADIO_BUTTON_TEXT_X = 25
TOGGLE_BUTTON_TEXT_X = 20

SMALL_OPTION_HEIGHT = 65
NORMAL_OPTION_HEIGHT = 80
SLIDER_OPTION_HEIGHT = 65


window = {
    "name" : "GameOptionDialog",
    # Dont touch these lines!
    "style" : (),
    "x" : 171,
    "y" : 3,
    "width" : 300,
    "height" : 324,
    # Dont touch these lines!
    "children" :
    (
        {
            "name" : "pickup_premimum_image",
            "type" : "expanded_image",
            "x" : 3,
            "y" : 0,
            "image" : IMG_PICKUP_DIR+"not_activated.tga",
            "children":
            (
                {
                    "name" : "pickup_premium_item",
                    "type" : "expanded_image",
                    "x" : 34,
                    "y" : 13,
                    "image" : "icon/item/70002.tga",
                },
                
                {
                    "name" : "pickup_premium_text_0",
                    "type" : "text",
                    "x" : 81,
                    "y" : 24,
                    "text_horizontal_align":"left",
                    "text":uiScriptLocale.AUTOMATIC_PICKUP_TEXT_0,
                    "outline":1,
                    "color":0xFFDADADA,
                },
                
                {
                    "name" : "pickup_premium_text_1",
                    "type" : "text",
                    "x" : 81,
                    "y" : 24+14,
                    "text_horizontal_align":"left",
                    "text":uiScriptLocale.AUTOMATIC_PICKUP_TEXT_1,
                    "outline":1,
                },

                {
                    "name" : "pickup_premium_text_2",
                    "type" : "text",
                    "x" : 7,
                    "y" : 52,
                    "text_horizontal_align":"left",
                    "text":uiScriptLocale.AUTOMATIC_PICKUP_TEXT_3,
                    "outline":1,
                    "color":0xFFFFA900,
                },

                {
                    "name" : "pickup_premium_text_3",
                    "type" : "text",
                    "x" : 7,
                    "y" : 70,
                    "text_horizontal_align":"left",
                    "text":uiScriptLocale.AUTOMATIC_PICKUP_TEXT_4,
                    "outline":1,
                    "color":0xFFDADADA,
                },
                
                {
                    "name" : "pickup_premium_text_4",
                    "type" : "text",
                    "x" : 7,
                    "y" : 80,
                    "text_horizontal_align":"left",
                    "text":uiScriptLocale.AUTOMATIC_PICKUP_TEXT_5,
                    "outline":1,
                    "color":0xFFDADADA,
                },
            ),
        },

        {
            "name" : "pickup_window",
            "type" : "window",
            "x" : 0,
            "y" : 107+8,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (

                {
                    "name" : "pickup_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_pickup",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.AUTOMATIC_PICK_UP,
                        },
                    ),
                },
                {
                    "name" : "pick_up_button_single",
                    "type" : "radio_button",
                    # "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X*0,
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
                    "y" : 33,
                    "text" : "Single",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    # "default_image" : IMG_DIR + "toggle_unselected.tga",
                    # "over_image" : IMG_DIR + "toggle_unselected.tga",
                    # "down_image" : IMG_DIR + "toggle_selected.tga",
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "pick_up_button_all",
                    "type" : "radio_button",
                    # "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X*1,
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
                    "y" : 33,
                    "text" : "ALL",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    # "default_image" : IMG_DIR + "toggle_unselected.tga",
                    # "over_image" : IMG_DIR + "toggle_unselected.tga",
                    # "down_image" : IMG_DIR + "toggle_selected.tga",
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },

        {
            "name" : "filter_window",
            "type" : "window",
            "x" : 0,
            "y" : 107+8+SMALL_OPTION_HEIGHT,
            "width":304,
            "height":NORMAL_OPTION_HEIGHT+NORMAL_OPTION_HEIGHT-20,
            "children":
            (
                {
                    "name" : "filter_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "filter_pickup",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.AUTOMATIC_PICK_UP_FILTER,
                        },
                    ),
                },
                {
                    "name" : "pick_up_sword",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*0,
                    "y" : 33,
                    "text" : "Sword",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_dagger",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*1,
                    "y" : 33,
                    "text" : "Dagger",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_bow",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*2,
                    "y" : 33,
                    "text" : "Bow",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_two_handed",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*3,
                    "y" : 33,
                    "text" : "Two-Hand",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_bell",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*0,
                    "y" : 33+BUTTON_NEXT_Y,
                    "text" : "Bell",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_fan",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*1,
                    "y" : 33+BUTTON_NEXT_Y,
                    "text" : "Fan",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_arrow",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*2,
                    "y" : 33+BUTTON_NEXT_Y,
                    "text" : "Arrow",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_body",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*3,
                    "y" : 33+BUTTON_NEXT_Y,
                    "text" : "Armor",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                
                {
                    "name" : "pick_up_head",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*0,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "Hair",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_shield",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*1,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "Shield",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_wrist",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*2,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "Wrist",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_foot",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*3,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "Foot",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                
                {
                    "name" : "pick_up_neck",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*0,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "Neck",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_ear",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*1,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "Ear",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_metin",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*2,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "Metin",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_yang",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*3,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "Yang",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                
                {
                    "name" : "pick_up_skillbook",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*0,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "SkillBook",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
                {
                    "name" : "pick_up_chest",
                    "type" : "toggle_button",
                    "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X_NEW*1,
                    "y" : 33+BUTTON_NEXT_Y+BUTTON_NEXT_Y+BUTTON_NEXT_Y+BUTTON_NEXT_Y,
                    "text" : "Chest",
                    "text_x" : TOGGLE_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "toggle_unselected.tga",
                    "over_image" : IMG_DIR + "toggle_unselected.tga",
                    "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
                },
            ),
        },
        # {
        #     "name" : "filter_rarity_window",
        #     "type" : "window",
        #     "x" : 0,
        #     "y" : 107+8+SMALL_OPTION_HEIGHT+NORMAL_OPTION_HEIGHT+NORMAL_OPTION_HEIGHT-20,
        #     "width":304,
        #     "height":NORMAL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT-25,
        #     "children":
        #     (
        #         {
        #             "name" : "filter_rarity_title_img",
        #             "type" : "expanded_image",
        #             "x" : 0,
        #             "y" : 0,
        #             "image" : IMG_DIR+"option_title.tga",
        #             "children":
        #             (
        #                 {
        #                     "name" : "filter_rarity_pickup",
        #                     "type" : "text",
        #                     "x" : TITLE_IMAGE_TEXT_X,
        #                     "y" : TITLE_IMAGE_TEXT_Y,
        #                     "text_horizontal_align":"left",
        #                     "text" : uiScriptLocale.AUTOMATIC_PICK_UP_FILTER_RARITY,
        #                 },
        #             ),
        #         },
        #         {
        #             "name" : "rarity_normal",
        #             "type" : "toggle_button",
        #             "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X*0,
        #             "y" : 33,
        #             "text" : uiScriptLocale.AUTOMATIC_RARITY_NORMAL,
        #             "text_x" : TOGGLE_BUTTON_TEXT_X,
        #             "text_color":0xFF6B6869,
        #             "default_image" : IMG_DIR + "toggle_unselected.tga",
        #             "over_image" : IMG_DIR + "toggle_unselected.tga",
        #             "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
        #         },
        #         {
        #             "name" : "rarity_uncommun",
        #             "type" : "toggle_button",
        #             "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X*1,
        #             "y" : 33,
        #             "text" : uiScriptLocale.AUTOMATIC_RARITY_UNCOMMUN,
        #             "text_x" : TOGGLE_BUTTON_TEXT_X,
        #             "text_color":0xFFDADADA,
        #             "default_image" : IMG_DIR + "toggle_unselected.tga",
        #             "over_image" : IMG_DIR + "toggle_unselected.tga",
        #             "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
        #         },
        #         {
        #             "name" : "rarity_rare",
        #             "type" : "toggle_button",
        #             "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X*2,
        #             "y" : 33,
        #             "text" : uiScriptLocale.AUTOMATIC_RARITY_RARE,
        #             "text_x" : TOGGLE_BUTTON_TEXT_X,
        #             "text_color":0xFF0071C3,
        #             "default_image" : IMG_DIR + "toggle_unselected.tga",
        #             "over_image" : IMG_DIR + "toggle_unselected.tga",
        #             "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
        #         },
        #         {
        #             "name" : "rarity_epic",
        #             "type" : "toggle_button",
        #             "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X*0,
        #             "y" : 33+BUTTON_NEXT_Y,
        #             "text" : uiScriptLocale.AUTOMATIC_RARITY_EPIC,
        #             "text_x" : TOGGLE_BUTTON_TEXT_X,
        #             "text_color":0xFFDA443D,
        #             "default_image" : IMG_DIR + "toggle_unselected.tga",
        #             "over_image" : IMG_DIR + "toggle_unselected.tga",
        #             "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
        #         },
        #         {
        #             "name" : "rarity_relic",
        #             "type" : "toggle_button",
        #             "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X*1,
        #             "y" : 33+BUTTON_NEXT_Y,
        #             "text" : uiScriptLocale.AUTOMATIC_RARITY_RELIC,
        #             "text_x" : TOGGLE_BUTTON_TEXT_X,
        #             "text_color":0xFF84CD4A,
        #             "default_image" : IMG_DIR + "toggle_unselected.tga",
        #             "over_image" : IMG_DIR + "toggle_unselected.tga",
        #             "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
        #         },
        #         {
        #             "name" : "rarity_legendary",
        #             "type" : "toggle_button",
        #             "x" : OPTION_START_X+TOGGLE_BUTTON_RANGE_X*2,
        #             "y" : 33+BUTTON_NEXT_Y,
        #             "text" : uiScriptLocale.AUTOMATIC_RARITY_LEGENDARY,
        #             "text_x" : TOGGLE_BUTTON_TEXT_X,
        #             "text_color":0xFFDE55D0,
        #             "default_image" : IMG_DIR + "toggle_unselected.tga",
        #             "over_image" : IMG_DIR + "toggle_unselected.tga",
        #             "down_image" : IMG_PICKUP_DIR + "toggle_selected.tga",
        #         },
                
        #         {
        #             "name" : "rarity_info",
        #             "type" : "expanded_image",
        #             "x" : 3,
        #             "y" : 33+BUTTON_NEXT_Y+25,
        #             "image" : IMG_PICKUP_DIR + "box_info.tga",
        #             "children":
        #             (
        #                 {
        #                     "name" : "filter_rarity_info",
        #                     "type" : "text",
        #                     "x" : 0,
        #                     "y" : 0,
        #                     "outline":1,
        #                     "all_align":1,
        #                     "text" : uiScriptLocale.AUTOMATIC_RARITY_INFO,
        #                     "color":0xFFFF4141,
        #                 },
        #             ),
        #         },
        #     ),
        # },
    ),
}