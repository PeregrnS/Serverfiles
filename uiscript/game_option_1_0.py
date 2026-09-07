# coding: latin_1

import uiScriptLocale

IMG_DIR = "d:/ymir work/ui/game/gameoption/"

TITLE_IMAGE_TEXT_X = 5
TITLE_IMAGE_TEXT_Y = 4

OPTION_START_X = 17
SLIDER_POSITION_X = 50

SLIDER_START_Y = 40
BUTTON_START_Y = 33
BUTTON_NEXT_Y = 20

RADIO_BUTTON_RANGE_X = 65
TOGGLE_BUTTON_RANGE_X = 65

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
            "name" : "show_name_window",
            "type" : "window",
            "x" : 0,
            "y" : 0,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "show_damage_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_show_damage",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.OPTION_EFFECT,
                        },
                    ),
                },
                {
                    "name" : "show_damage_on_button",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },

                {
                    "name" : "show_damage_off_button",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1.2,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        {
            "name" : "camera_mode_window",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "camera_mode_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_camera_mode",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.OPTION_CAMERA_DISTANCE,
                        },
                    ),
                },
                {
                    "name" : "camera_short",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_SHORT,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "camera_long",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_LONG,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        {
            "name" : "fog_mode_window",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "fog_mode_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_fog_mode",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.OPTION_FOG,
                        },
                    ),
                },
                {
                    "name" : "fog_level0",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_FOG_DENSE,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "fog_level1",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_FOG_MIDDLE,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "fog_level2",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_FOG_LIGHT,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
		
        {
            "name" : "rainbow_mode_window",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "rainbow_mode_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_rainbow_mode",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : "Rainbow Hits",
                        },
                    ),
                },
                {
                    "name" : "rainbow_on",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "rainbow_off",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1.2,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
		
        # {
        #     "name" : "night_mode_window",
        #     "type" : "window",
        #     "x" : 0,
        #     "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
        #     "width":304,
        #     "height":SMALL_OPTION_HEIGHT,
        #     "children":
        #     (
        #         {
        #             "name" : "night_mode_title_img",
        #             "type" : "expanded_image",
        #             "x" : 0,
        #             "y" : 0,
        #             "image" : IMG_DIR+"option_title.tga",
        #             "children":
        #             (
        #                 {
        #                     "name" : "title_night_mode",
        #                     "type" : "text",
        #                     "x" : TITLE_IMAGE_TEXT_X,
        #                     "y" : TITLE_IMAGE_TEXT_Y,
        #                     "text_horizontal_align":"left",
        #                     "text" : uiScriptLocale.OPTION_NIGHT_MODE,
        #                 },
        #             ),
        #         },
        #         {
        #             "name" : "nightmode_on",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
        #             "y" : 33,
        #             "text" : uiScriptLocale.OPTION_NIGHT_MODE_ON,
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },
        #         {
        #             "name" : "nightmode_off",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
        #             "y" : 33,
        #             "text" : uiScriptLocale.OPTION_NIGHT_MODE_OFF,
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },
        #     ),
        # },
        # {
        #     "name" : "windows_message_window",
        #     "type" : "window",
        #     "x" : 0,
        #     "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
        #     "width":304,
        #     "height":SMALL_OPTION_HEIGHT,
        #     "children":
        #     (
        #         {
        #             "name" : "windows_message_title_img",
        #             "type" : "expanded_image",
        #             "x" : 0,
        #             "y" : 0,
        #             "image" : IMG_DIR+"option_title.tga",
        #             "children":
        #             (
        #                 {
        #                     "name" : "title_windows_message",
        #                     "type" : "text",
        #                     "x" : TITLE_IMAGE_TEXT_X,
        #                     "y" : TITLE_IMAGE_TEXT_Y,
        #                     "text_horizontal_align":"left",
        #                     "text" : uiScriptLocale.OPTION_WINDOWS_MESSAGE,
        #                 },
        #             ),
        #         },
        #         {
        #             "name" : "windows_message_on",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
        #             "y" : 33,
        #             "text" : uiScriptLocale.OPTION_NIGHT_MODE_ON,
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },
        #         {
        #             "name" : "windows_message_off",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
        #             "y" : 33,
        #             "text" : uiScriptLocale.OPTION_NIGHT_MODE_OFF,
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },
        #     ),
        # },
    ),
}