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
                    "name" : "show_name_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_show_name",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME,
                        },
                    ),
                },
                {
                    "name" : "always_show_name_on_button",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_ON,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },

                {
                    "name" : "always_show_name_off_button",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_OFF,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },

                {
                    "name" : "always_show_name_partial_button",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_OUTLINE_INFO,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        # {
        #     "name" : "outline_window",
        #     "type" : "window",
        #     "x" : 0,
        #     "y" : SMALL_OPTION_HEIGHT,
        #     "width":304,
        #     "height":SMALL_OPTION_HEIGHT,
        #     "children":
        #     (
        #         {
        #             "name" : "outline_title_img",
        #             "type" : "expanded_image",
        #             "x" : 0,
        #             "y" : 0,
        #             "image" : IMG_DIR+"option_title.tga",
        #             "children":
        #             (
        #                 {
        #                     "name" : "title_outline",
        #                     "type" : "text",
        #                     "x" : TITLE_IMAGE_TEXT_X,
        #                     "y" : TITLE_IMAGE_TEXT_Y,
        #                     "text_horizontal_align":"left",
        #                     "text" : uiScriptLocale.OPTION_OUTLINE_INFO,
        #                 },
        #             ),
        #         },
        #         {
        #             "name" : "outline_on_button",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
        #             "y" : 33,
        #             "text" : uiScriptLocale.OPTION_OUTLINE_ON,
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },

        #         {
        #             "name" : "outline_off_button",
        #             "type" : "radio_button",
        #             "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
        #             "y" : 33,
        #             "text" : uiScriptLocale.OPTION_OUTLINE_OFF,
        #             "text_x" : RADIO_BUTTON_TEXT_X,
        #             "default_image" : IMG_DIR + "radio_unselected.tga",
        #             "over_image" : IMG_DIR + "radio_unselected.tga",
        #             "down_image" : IMG_DIR + "radio_selected.tga",
        #         },
        #     ),
        # },
        {
            "name" : "affect_icons_window",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "affect_icons_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_affect_icons",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : "Affect Icons",
                        },
                    ),
                },
                {
                    "name" : "show_affect_icons",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_OUTLINE_ON,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },

                {
                    "name" : "hide_affect_icons",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
                    "y" : 33,
                    "text" : uiScriptLocale.OPTION_OUTLINE_OFF,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        {
            "name" : "perspective_window",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SLIDER_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "perspective_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_perspective",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.OPTION_FOV,
                        },
                    ),
                },
                {
                    "name" : "perspective_controller",
                    "type" : "sliderbar",
                    "x" : OPTION_START_X+SLIDER_POSITION_X,
                    "y" : SLIDER_START_Y,
                },
            ),
        },
        # {
        #     "name" : "damage_scale_window",
        #     "type" : "window",
        #     "x" : 0,
        #     "y" : SLIDER_OPTION_HEIGHT+SLIDER_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+NORMAL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+100,
        #     "width":304,
        #     "height":SLIDER_OPTION_HEIGHT,
        #     "children":
        #     (
        #         {
        #             "name" : "damage_scale_title_img",
        #             "type" : "expanded_image",
        #             "x" : 0,
        #             "y" : 0,
        #             "image" : IMG_DIR+"option_title.tga",
        #             "children":
        #             (
        #                 {
        #                     "name" : "title_damage_scale",
        #                     "type" : "text",
        #                     "x" : TITLE_IMAGE_TEXT_X,
        #                     "y" : TITLE_IMAGE_TEXT_Y,
        #                     "text_horizontal_align":"left",
        #                     "text" : "Damage Scale",
        #                 },
        #             ),
        #         },
        #         {
        #             "name" : "damage_scale_controller",
        #             "type" : "sliderbar",
        #             "x" : OPTION_START_X+SLIDER_POSITION_X,
        #             "y" : SLIDER_START_Y,
        #         },
        #     ),
        # },
        {
            "name" : "stone_scale_window",
            "type" : "window",
            "x" : 0,
            "y" : SLIDER_OPTION_HEIGHT+SLIDER_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+NORMAL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+40,
            "width":304,
            "height":SLIDER_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "stone_scale_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_stone_scale",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : "Stone Scale",
                        },
                    ),
                },
                {
                    "name" : "stone_scale_controller",
                    "type" : "sliderbar",
                    "x" : OPTION_START_X+SLIDER_POSITION_X,
                    "y" : SLIDER_START_Y,
                },
            ),
        },
        {
            "name" : "effect_level_title",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SLIDER_OPTION_HEIGHT,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                #effect
                {
                    "name" : "effect_level_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "effect_level",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL,
                        },
                    ),
                },
                {
                    "name" : "effect_level0",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*0,
                    "y" : 33,
                    "text" :  "1",
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL1_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "effect_level1",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*1,
                    "y" : 33,
                    "text" :  "2",
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL2_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "effect_level2",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*2,
                    "y" : 33,
                    "text" :  "3",
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL3_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "effect_level3",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*3,
                    "y" : 33,
                    "text" :  "4",
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL4_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "effect_level4",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*4,
                    "y" : 33,
                    "text" :  "5",
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL5_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        {
            "name" : "private_shop_distance_window",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SLIDER_OPTION_HEIGHT,
            "width":304,
            "height":SLIDER_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "private_shop_distance_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "private_shop_distance_scale",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.PREMIUM_PRIVATE_SHOP_TITLE_VIEW_DISTANCE,
                        },
                    ),
                },
                {
                    "name" : "private_shop_distance_controller",
                    "type" : "sliderbar",
                    "x" : OPTION_START_X+SLIDER_POSITION_X,
                    "y" : SLIDER_START_Y,
                },
            ),
        },

        {
            "name" : "PetTitle",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SLIDER_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                #effect
                {
                    "name" : "pet_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "PetTitleText",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.GRAPHICONOFF_PET_STATUS,
                        },
                    ),
                },
                {
                    "name" : "pet_on",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*0,
                    "y" : 33,
                    "text" :  uiScriptLocale.GRAPHICONOFF_PET_STATUS_ON,
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_PET_STATUS_ON_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "pet_off",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*1,
                    "y" : 33,
                    "text" :  uiScriptLocale.GRAPHICONOFF_PET_STATUS_OFF,
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_PET_STATUS_OFF_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        {
            "name" : "MountTitle",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SLIDER_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "mount_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "MountTitleText",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.GRAPHICONOFF_MOUNT_STATUS,
                        },
                    ),
                },
                {
                    "name" : "mount_on",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*0,
                    "y" : 33,
                    "text" :  uiScriptLocale.GRAPHICONOFF_MOUNT_STATUS_ON,
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_MOUNT_STATUS_ON_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "mount_off",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*1,
                    "y" : 33,
                    "text" :  uiScriptLocale.GRAPHICONOFF_MOUNT_STATUS_OFF,
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_MOUNT_STATUS_OFF_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        {
            "name" : "npcName",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SLIDER_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SMALL_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "npcName_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "npcNameTitleText",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS,
                        },
                    ),
                },
                {
                    "name" : "npcName_on",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*0,
                    "y" : 33,
                    "text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS_ON,
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS_ON_TOOLTIP,
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
                {
                    "name" : "npcName_off",
                    "type" : "radio_button",
                    "x" : OPTION_START_X+(RADIO_BUTTON_RANGE_X-15)*1,
                    "y" : 33,
                    "text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS_OFF,
                    "tooltip_text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS_OFF_TOOLTIP,  
                    "text_x" : RADIO_BUTTON_TEXT_X,
                    "default_image" : IMG_DIR + "radio_unselected.tga",
                    "over_image" : IMG_DIR + "radio_unselected.tga",
                    "down_image" : IMG_DIR + "radio_selected.tga",
                },
            ),
        },
        {
            "name" : "object_distance_window",
            "type" : "window",
            "x" : 0,
            "y" : SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SLIDER_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT+SMALL_OPTION_HEIGHT,
            "width":304,
            "height":SLIDER_OPTION_HEIGHT,
            "children":
            (
                {
                    "name" : "object_distance_title_img",
                    "type" : "expanded_image",
                    "x" : 0,
                    "y" : 0,
                    "image" : IMG_DIR+"option_title.tga",
                    "children":
                    (
                        {
                            "name" : "title_object_distance",
                            "type" : "text",
                            "x" : TITLE_IMAGE_TEXT_X,
                            "y" : TITLE_IMAGE_TEXT_Y,
                            "text_horizontal_align":"left",
                            "text" : uiScriptLocale.OPTION_OBJECT_DISTANCE,
                        },
                    ),
                },
                {
                    "name" : "object_distance_controller",
                    "type" : "sliderbar",
                    "x" : OPTION_START_X+SLIDER_POSITION_X,
                    "y" : SLIDER_START_Y,
                },
            ),
        },
    ),
}
