import uiScriptLocale
import item
import app
import player

EQUIPMENT_START_INDEX = player.EQUIPMENT_SLOT_START

if app.ENABLE_GEM_SYSTEM:
	INVENTORY_HEIGHT = 565 + 18
else:
	INVENTORY_HEIGHT = 565 + 40

window = {
	"name" : "InventoryWindow",
	"x" : SCREEN_WIDTH - 186, ## 176 standart
	"y" : SCREEN_HEIGHT - 37 - 565 - 40,
	"style" : ("movable", "float",),
	"width" : 186,
	"height" : INVENTORY_HEIGHT,
	"children" :
	[
		## Inventory, Equipment Slots
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : 186,
			"height" : INVENTORY_HEIGHT,
			"children" :
			[
		## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar_finalcore_small",
					"style" : ("attach",),
					## if app.ENABLE_SORT_INVENTORY:
					"x" : 15,
					"y" : -18,
					"width" : 161,
					## else:
					# "x" : 8,
					# "y" : 7,
					# "width" : 161,
					"color" : "yellow",
					"children" :
					[
						{ "name":"TitleName", "type":"text", "x":75, "y":30, "text":uiScriptLocale.INVENTORY_TITLE, "text_horizontal_align":"center" },
					],
				},
            ## if app.ENABLE_SORT_INVENTORY:
				{
					"name" : "SortButton_Base",
					"type" : "image",
					"style" : ("attach",),

					"x" : 8,
					"y" : 10,

					"image" : "d:/ymir work/ui/pattern/titlebar_inv_refresh_baseframe.tga",

					"children" :
					(
						{
							"name" : "SortButton",
							"type" : "button",

							"x" : 11,
							"y" : 3,

							"default_image" : "d:/ymir work/ui/refresh_small_button_01.sub",
							"over_image" : "d:/ymir work/ui/refresh_small_button_02.sub",
							"down_image" : "d:/ymir work/ui/refresh_small_button_03.sub",
							"disable_image" : "d:/ymir work/ui/refresh_small_button_04.sub",
						},
					),
				},	
				## Equipment Slot
				{
					"name" : "Equipment_Base",
					"type" : "image",
					"x" : 16,
					"y" : 33,
					"image" : "d:/ymir work/ui/equipment_bg_without_ring_old01.tga",
					"children" :
					[
						{
							"name" : "EquipmentSlot",
							"type" : "slot",
							"x" : 3,
							"y" : 3,
							"width" : 150,
							"height" : 182,
							"slot" : (
										{"index":item.EQUIPMENT_BODY, "x":39, "y":37, "width":32, "height":64},
										{"index":item.EQUIPMENT_HEAD, "x":39, "y":2, "width":32, "height":32},
										{"index":item.EQUIPMENT_SHOES, "x":39, "y":145, "width":32, "height":32},
										{"index":item.EQUIPMENT_WRIST, "x":75, "y":67, "width":32, "height":32},
										{"index":item.EQUIPMENT_WEAPON, "x":3, "y":3, "width":32, "height":96},
										{"index":item.EQUIPMENT_NECK, "x":114, "y":67, "width":32, "height":32},
										{"index":item.EQUIPMENT_EAR, "x":114, "y":35, "width":32, "height":32},
										{"index":item.EQUIPMENT_UNIQUE1, "x":2, "y":144, "width":32, "height":32},
										{"index":item.EQUIPMENT_UNIQUE2, "x":74, "y":144, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARROW, "x":114, "y":2, "width":32, "height":32},
										{"index":item.EQUIPMENT_SHIELD, "x":75, "y":35, "width":32, "height":32},
										{"index":item.EQUIPMENT_BELT, "x":39, "y":106, "width":32, "height":32},
										{"index":item.EQUIPMENT_PENDANT, "x":2, "y":106, "width":32, "height":32},
										{"index":item.EQUIPMENT_PET, "x":75, "y":106, "width":32, "height":32}, ## if app.ENABLE_COSTUME_PET:
									),
						},

						# ENABLE_REMOTE_SHOP
						{
							"name" : "RemoteShopButton",
							"type" : "button",
							"x" : 118,
							"y" : 107,
							"tooltip_text" : uiScriptLocale.REMOTE_SHOP_TITLE,
							"default_image" : "d:/ymir work/ui/game/taskbar/remoteshop_button_01.tga",
							"over_image" : "d:/ymir work/ui/game/taskbar/remoteshop_button_02.tga",
							"down_image" : "d:/ymir work/ui/game/taskbar/remoteshop_button_03.tga",
						},
						## MallButton
						{
							"name" : "MallButton",
							"type" : "button",
							"x" : 118,
							"y" : 148,
							"tooltip_text" : "Safebox",
							"default_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_01.png",
							"over_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_02.png",
							"down_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_03.png",
						},
						## CostumeButton
						{
							"name" : "CostumeButton",
							"type" : "button",
							"x" : 77,
							"y" : 4,
							"tooltip_text" : uiScriptLocale.COSTUME_TITLE,
							"default_image" : "d:/ymir work/ui/game/taskbar/costume_Button_01.png",
							"over_image" : "d:/ymir work/ui/game/taskbar/costume_Button_02.png",
							"down_image" : "d:/ymir work/ui/game/taskbar/costume_Button_03.png",
						},
					],
				},

				{
					"name" : "Equipment_Tab_01",
					"type" : "radio_button",
					"x" : 86,
					"y" : 161,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
					"children" :
					[
						{
							"name" : "Equipment_Tab_01_Print",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : "I",
						},
					],
				},
				{
					"name" : "Equipment_Tab_02",
					"type" : "radio_button",
					"x" : 86 + 32,
					"y" : 161,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
					"children" :
					[
						{
							"name" : "Equipment_Tab_02_Print",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : "II",
						},
					],
				},

				## if app.ENABLE_SPECIAL_INVENTORY:
				{
					"name" : "TabControl",
					"type" : "window",
			
					"x" : 10,
					"y" : 33 + 191,
			
					"width" : 169,
					"height" : 32,
			
					"children" :
					[
						## Tab
						{
							"name" : "Inventory_Type_01",
							"type" : "image",
			
							"x" : 0,
							"y" : 0,
			
							"width" : 169,
							"height" : 32,
			
							"image" : "d:/ymir work/ui/game/inventory/inventory_typ_tab_1.sub",
						},
						{
							"name" : "Inventory_Type_02",
							"type" : "image",
			
							"x" : 0,
							"y" : 0,
			
							"width" : 169,
							"height" : 32,
			
							"image" : "d:/ymir work/ui/game/inventory/inventory_typ_tab_2.sub",
						},
						{
							"name" : "Inventory_Type_03",
							"type" : "image",
			
							"x" : 0,
							"y" : 0,
			
							"width" : 169,
							"height" : 32,
			
							"image" : "d:/ymir work/ui/game/inventory/inventory_typ_tab_3.sub",
						},
						{
							"name" : "Inventory_Type_04",
							"type" : "image",
			
							"x" : 0,
							"y" : 0,
			
							"width" : 169,
							"height" : 32,
			
							"image" : "d:/ymir work/ui/game/inventory/inventory_typ_tab_4.sub",
						},

						{
							"name" : "Inventory_Type_Button_01",
							"type" : "radio_button",
							"x" : 3, "y" : 0,
							"width" : 42, "height" : 30,
							"tooltip_text" : uiScriptLocale.INVENTORY_TITLE,
							"tooltip_x" : 0, "tooltip_y" : -13,
							"tooltip_text_color" : 0xffffba00,
						},
						{
							"name" : "Inventory_Type_Button_02",
							"type" : "radio_button",
							"x" : 3+42, "y" : 0,
							"width" : 41, "height" : 30,
							"tooltip_text" : uiScriptLocale.SPECIAL_INVENTORY_SKILLBOOK_TITLE,
							"tooltip_x" : 0, "tooltip_y" : -13,
							"tooltip_text_color" : 0xffffba00,
						},
						{
							"name" : "Inventory_Type_Button_03",
							"type" : "radio_button",
							"x" : 3+42+41, "y" : 0,
							"width" : 41, "height" : 30,
							"tooltip_text" : uiScriptLocale.SPECIAL_INVENTORY_STONE_TITLE,
							"tooltip_x" : 0, "tooltip_y" : -13,
							"tooltip_text_color" : 0xffffba00,
						},
						{
							"name" : "Inventory_Type_Button_04",
							"type" : "radio_button",
							"x" : 3+42+41+41, "y" : 0,
							"width" : 42, "height" : 30,
							"tooltip_text" : uiScriptLocale.SPECIAL_INVENTORY_MATERIAL_TITLE,
							"tooltip_x" : 0, "tooltip_y" : -13,
							"tooltip_text_color" : 0xffffba00,
						},
					],
				},

				{
					"name" : "Inventory_Tab_01",
					"type" : "radio_button",
					"x" : 12,
					"y" : 33 + 191 + 34,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_1,
					"children" :
					[
						{
							"name" : "Inventory_Tab_01_Print",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : "I",
						},
					],
				},
				{
					"name" : "Inventory_Tab_02",
					"type" : "radio_button",
					#"x" : 10 + 78,
					"x" : 10 + 39 + 4,
					"y" : 33 + 191 + 34,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_2,
					"children" :
					[
						{
							"name" : "Inventory_Tab_02_Print",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : "II",
						},
					],
				},
				{
					"name" : "Inventory_Tab_03",
					"type" : "radio_button",
					"x" : 10 + 39 + 39 + 6,
					"y" : 33 + 191 + 34,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_3,
					"children" :
					[
						{
							"name" : "Inventory_Tab_03_Print",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : "III",
						},
					],
				},
				{
					"name" : "Inventory_Tab_04",
					"type" : "radio_button",
					"x" : 10 + 39 + 39 + 39 + 8,
					"y" : 33 + 191 + 34,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_large_half_03.sub",
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_4,
					"children" :
					[
						{
							"name" : "Inventory_Tab_04_Print",
							"type" : "text",
							"x" : 0,
							"y" : 0,
							"all_align" : "center",
							"text" : "IV",
						},
					],
				},
				## Item Slot
				{
					"name" : "ItemSlot",
					"type" : "grid_table",
					"x" : 13,
					"y" : 246 + 36,
					"start_index" : 0,
					"x_count" : 5,
					"y_count" : 9,
					"x_step" : 32,
					"y_step" : 32,
					"image" : "d:/ymir work/ui/public/Slot_Base.sub"
				},
				## Print
				{
					"name":"Money_Slot",
					"type":"button",
					"x":8,
					"y":28,
					"horizontal_align":"center",
					"vertical_align":"bottom",
					"default_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
					"over_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
					"down_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
					"children" :
					[
						{
							"name":"Money_Icon",
							"type":"image",
							"x":-18,
							"y":2,
							"image":"d:/ymir work/ui/game/windows/money_icon.sub",
						},
						{
							"name" : "Money",
							"type" : "text",
							"x" : 3,
							"y" : 3,
							"horizontal_align" : "right",
							"text_horizontal_align" : "right",
							"text" : "123456789",
						},
					],
				},
			],
		},
	],
}
