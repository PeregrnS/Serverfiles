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
            "name" : "salestext_window",
            "type" : "window",
            "x" : 0,
            "y" : 0,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "salestext_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_salestext",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.OPTION_SALESTEXT,
                        },
                    ),
                },
                {
                    "name" : "salestext_on_button",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1.2,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_OFF,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },

                {
                    "name" : "salestext_off_button",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_ON,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        {
            "name" : "visible_window",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "visible_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_visible",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : "Visible Custom Title",
                        },
                    ),
                },
                {
                    "name" : "title_view",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1.2,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "title_hide",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        # {
        #     "name" : "snow_window",
        #     "type" : "window",
        #     "x" : 0,
        #     "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
        #     "width":304,
        #     "height":SMALL_OPTION_HEIGHT,
        #     "children":
        #     (
        #         {
        #             "name" : "snow_title_img",
        #             "type" : "expanded_image",
        #             "x" : 0,
        #             "y" : 0,
        #             "image" : IMG_DIR+"option_title.tga",
        #             "children":
        #             (
        #                 {
        #                     "name" : "title_snow",
        #                     "type" : "text",
        #                     "x" : TITLE_IMAGE_TEXT_X,
        #                     "y" : TITLE_IMAGE_TEXT_Y,
        #                     "text_horizontal_align":"left",
        #                     "text" : "Textura",
        #                 },
        #             ),
        #         },
        #         {
        #             "name" : "show_snow_button",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
        #             "y" : 33,
        #             "text" : "Winter",
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },

        #         {
        #             "name" : "hide_snow_button",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
        #             "y" : 33,
        #             "text" : "Normal",
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },
        #     ),
        # },
        # {
        #     "name" : "inventorychange_window",
        #     "type" : "window",
        #     "x" : 0,
        #     "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
        #     "width":304,
        #     "height":SMALL_OPTION_HEIGHT,
        #     "children":
        #     (
        #         {
        #             "name" : "inventorychange_title_img",
        #             "type" : "expanded_image",
        #             "x" : 0,
        #             "y" : 0,
        #             "image" : IMG_DIR+"option_title.tga",
        #             "children":
        #             (
        #                 {
        #                     "name" : "inventorychange_message",
        #                     "type" : "text",
        #                     "x" : TITLE_IMAGE_TEXT_X,
        #                     "y" : TITLE_IMAGE_TEXT_Y,
        #                     "text_horizontal_align":"left",
        #                     "text":uiScriptLocale.INVENTORY_TITLE,
        #                 },
        #             ),
        #         },
        #         {
        #             "name" : "inventory_0",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
        #             "y" : 33,
        #             "text" : "I",
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },
        #         {
        #             "name" : "inventory_1",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
        #             "y" : 33,
        #             "text" : "II",
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },
        #         {
        #             "name" : "inventory_2",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
        #             "y" : 33,
        #             "text" : "III",
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },
        #     ),
        # },
    ),
}