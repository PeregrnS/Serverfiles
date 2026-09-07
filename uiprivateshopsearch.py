import ui
import net
import app
import item
import chat
import grp
import uiCommon
import localeInfo
import uiScriptLocale
import shop
import background
import player
import privateShop
import uiToolTip
import math
import os
import constInfo

import cPickle as pickle

from _weakref import proxy

PRIVATESEARCH_PATH = "d:/ymir work/ui/privatesearch/"
PRIVATESHOP_PATH = "d:/ymir work/ui/game/premium_private_shop/"

MAIN_CATEGORY_X = 4
SUB_CATEGORY_X = 4 + 17
MAIN_CATEGORY_Y = 0

NONE_SELECTED = 0

filter_config_template = {
	privateShop.FILTER_TYPE_ITEM_VNUM 		: 0,
	privateShop.FILTER_TYPE_OWNER_NAME		: "",
	privateShop.FILTER_TYPE_ITEM_TYPE 		: -1,
	privateShop.FILTER_TYPE_ITEM_SUBTYPE 	: -1,

	privateShop.FILTER_TYPE_CLASS 			: -1,
	privateShop.FILTER_TYPE_GENDER 			: -1,

	privateShop.FILTER_TYPE_MIN_STACK 		: 0,
	privateShop.FILTER_TYPE_MAX_STACK		: 0,

	privateShop.FILTER_TYPE_MIN_REFINEMENT 	: 0,
	privateShop.FILTER_TYPE_MAX_REFINEMENT	: 0,

	privateShop.FILTER_TYPE_MIN_LEVEL 		: 0,
	privateShop.FILTER_TYPE_MAX_LEVEL		: 0,

	privateShop.FILTER_TYPE_ATTR_1 			: [0, 0],
	privateShop.FILTER_TYPE_ATTR_2 			: [0, 0],
	privateShop.FILTER_TYPE_ATTR_3 			: [0, 0],
	privateShop.FILTER_TYPE_ATTR_4 			: [0, 0],
	privateShop.FILTER_TYPE_ATTR_5 			: [0, 0],

	privateShop.FILTER_TYPE_SASH_ABSORPTION	: 0,

	privateShop.FILTER_TYPE_ALCHEMY_LEVEL	: 0,
	privateShop.FILTER_TYPE_ALCHEMY_CLARITY	: 0,
}

class FilterSlot(ui.ScriptWindow):
	def __init__(self, parent, mainWindow, x, y):
		ui.ScriptWindow.__init__(self)

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.HideToolTip()

		self.backgroundImage = ui.ExpandedImageBox()
		self.backgroundImage.SetParent(self)
		self.backgroundImage.LoadImage(PRIVATESHOP_PATH + "filter_slot.sub")
		self.backgroundImage.Show()

		self.applyButton = ui.Button()
		self.applyButton.SetParent(self)
		self.applyButton.SetPosition(150, 5)
		self.applyButton.SetUpVisual(PRIVATESHOP_PATH + "mini_accept_button_default.sub")
		self.applyButton.SetOverVisual(PRIVATESHOP_PATH + "mini_accept_button_over.sub")
		self.applyButton.SetDownVisual(PRIVATESHOP_PATH + "mini_accept_button_down.sub")
		self.applyButton.SetEvent(ui.__mem_func__(self.OnApplyFilter))
		self.applyButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "APPLY")
		self.applyButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
		self.applyButton.Show()

		self.removeButton = ui.Button()
		self.removeButton.SetParent(self)
		self.removeButton.SetPosition(167, 5)
		self.removeButton.SetUpVisual(PRIVATESHOP_PATH + "mini_cancel_button_default.sub")
		self.removeButton.SetOverVisual(PRIVATESHOP_PATH + "mini_cancel_button_over.sub")
		self.removeButton.SetDownVisual(PRIVATESHOP_PATH + "mini_cancel_button_down.sub")
		self.removeButton.SetEvent(ui.__mem_func__(self.OnRemoveFilter))
		self.removeButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "REMOVE")
		self.removeButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
		self.removeButton.Show()

		self.titleText = ui.TextLine()
		self.titleText.SetParent(self)
		self.titleText.SetPosition(9, 3)
		self.titleText.Show()

		self.config = None
		self.questionDialog = None
		self.orig_x = x
		self.orig_y = y

		self.SetParentProxy(parent)
		self.SetPosition(x, y)
		self.SetSize(self.backgroundImage.GetWidth(), self.backgroundImage.GetHeight())

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def SetTitle(self, title):
		self.titleText.SetText(title)

	def GetTitle(self):
		return self.titleText.GetText()

	def SetConfiguration(self, config):
		self.config = config

	def GetConfiguration(self):
		return self.config

	def OnApplyFilter(self):
		# Revision variables
		text = localeInfo.PRIVATE_SHOP_SEARCH_APPLY_FILTER_QUESTION % self.titleText.GetText()
		q_arglen = constInfo.CHECK_TEXT_LENGHT(text)

		questionDialog = uiCommon.QuestionDialog() # Revised
		questionDialog.SetText(text)
		questionDialog.SetWidth(constInfo.CALC_Q_ARGLEN(q_arglen))
		questionDialog.SetAcceptEvent(self.OnApplyFilterAcceptEvent)
		questionDialog.SetCancelEvent(self.OnCloseQuestionDialog)
		questionDialog.Open()

		self.questionDialog = questionDialog

	def OnRemoveFilter(self):
		# Revision variables
		text = localeInfo.PRIVATE_SHOP_SEARCH_REMOVE_FILTER_QUESTION % self.titleText.GetText()
		q_arglen = constInfo.CHECK_TEXT_LENGHT(text)

		questionDialog = uiCommon.QuestionDialog() # Revised
		questionDialog.SetText(text)
		questionDialog.SetWidth(constInfo.CALC_Q_ARGLEN(q_arglen))
		questionDialog.SetAcceptEvent(self.OnRemoveFilterAcceptEvent)
		questionDialog.SetCancelEvent(self.OnCloseQuestionDialog)
		questionDialog.Open()

		self.questionDialog = questionDialog

	def OnApplyFilterAcceptEvent(self):
		self.OnCloseQuestionDialog()
		self.GetParentProxy().ApplyFilter(self.GetConfiguration())

	def OnRemoveFilterAcceptEvent(self):
		self.OnCloseQuestionDialog()
		self.GetParentProxy().RemoveFilter(self)

	def OnCloseQuestionDialog(self):
		if self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None

	def AdjustOriginalPosition(self, x = 0, y = 0):
		self.orig_x += x
		self.orig_y += y

	def AdjustPosition(self, x = 0, y = 0):
		self.SetPosition(self.orig_x + x, self.orig_y + y)

	def __OnOverInButton(self, button):
		self.toolTip.ClearToolTip()

		text = ""
		if button == "APPLY":
			text = uiScriptLocale.PRIVATESHOPSEARCH_APPLY_FILTER

		elif button == "REMOVE":
			text = uiScriptLocale.PRIVATESHOPSEARCH_REMOVE_FILTER

		t_arglen = constInfo.CHECK_TEXT_LENGHT(text)

		self.toolTip.SetThinBoardSize(constInfo.CALC_T_ARGLEN(t_arglen))
		self.toolTip.AppendTextLine(text, constInfo.EXTRA_INFO_COLOR)
		self.toolTip.ShowToolTip() # Revised

	def __OnOverOutButton(self):
		if 0 != self.toolTip:
			self.toolTip.HideToolTip()

class FilterSelectDialog(ui.ScriptWindow):
	def __init__(self, mainWindow):
		ui.ScriptWindow.__init__(self)

		self.filterList = []
		self.mainWindow = mainWindow
		self.__LoadWindow()
		self.__LoadFilter()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/privateshopsearchfilterdialog.py")
		except:
			import exception
			exception.Abort("FilterSelectDialog.__LoadWindow.UIScript/PrivateShopSearchFilterDialog.py")

		try:
			self.board = self.GetChild("Board")
			self.board.SetCloseEvent(ui.__mem_func__(self.Close))

			self.filterScrollBar = self.GetChild("FilterScrollBar")
			self.filterScrollBar.SetScrollEvent(ui.__mem_func__(self.OnFilterScroll))

			self.filterSlotWindowMask = self.GetChild("FilterSlotWindowMask")
			if app.ENABLE_INGAME_WIKI:
				self.filterSlotWindowMask.SetInsideRender(True)

		except:
			import exception
			exception.Abort("PrivateShopSeachWindow.__LoadWindow.PrivateShopSearchDialog")

	def Open(self):
		self.Show()

	def Close(self):
		self.Hide()

	def AppendFilter(self, filter_config, title, save = True):
		filter = FilterSlot(self.filterSlotWindowMask, self, 0, (22 + 2) * len(self.filterList))
		filter.SetConfiguration(filter_config)
		filter.SetTitle(title)
		filter.Show()

		self.filterList.append(filter)

		if save:
			self.__SaveFilter()
		self.RefreshFilter()

	def RemoveFilter(self, filter):
		for i, filterSlot in enumerate(self.filterList):
			if filter == filterSlot:
				for filterSlot in self.filterList[i + 1:]:
					filterSlot.AdjustOriginalPosition(y = -filterSlot.GetHeight() - 2)

				self.filterList.remove(filter)
				break

		self.RefreshFilter()

		self.__SaveFilter()

	def ApplyFilter(self, config):
		self.mainWindow.OnApplyFilter(config)

	def RefreshFilter(self):
		itemCount = len(self.filterList)

		span = (22 + 3) * itemCount - 2
		if span > self.filterSlotWindowMask.GetHeight():
			self.filterScrollBar.SetSpan((22 + 2) * itemCount - self.filterSlotWindowMask.GetHeight() - 2)
			self.filterScrollBar.SetScrollStep(1.0 / itemCount)
			self.filterScrollBar.SetSpan(span - self.filterScrollBar.GetHeight())
			self.filterScrollBar.SetMiddleBarSize(4.0 / itemCount)
			self.filterScrollBar.SetPos(self.filterScrollBar.GetPos()) # Used to corregate scrollbar's position when it expands

			if not self.filterScrollBar.IsShow():
				self.filterScrollBar.SetPos(0.0)

			self.filterScrollBar.Show()
			self.OnFilterScroll()
		else:
			for filterSlot in self.filterList:
				filterSlot.AdjustPosition(x = 7)

			self.filterScrollBar.Hide()

	def OnFilterScroll(self):
		pos = self.filterScrollBar.GetPos() * self.filterScrollBar.GetSpan()

		for filterSlot in self.filterList:
			filterSlot.AdjustPosition(y = -pos)

	def OnRunMouseWheel(self, nLen):
		if self.IsInWindowRect():
			if nLen > 0:
				self.filterScrollBar.OnUp()
				return True

			elif nLen < 0:
				self.filterScrollBar.OnDown()
				return True

		return False

	def __SaveFilter(self):
		filter_list = [(filter.GetTitle(), filter.GetConfiguration()) for filter in self.filterList]

		file = open('filter.cfg', 'wb')
		pickle.dump(filter_list, file)

	def __LoadFilter(self):
		file = open('filter.cfg', 'rb')
		try:
			data = pickle.load(file)

			for (title, filter_config) in data:
				self.AppendFilter(filter_config, title, False)
		except EOFError:
			pass

class SubCategory(ui.Button):
	def __init__(self, parent):
		ui.Button.__init__(self)

		self.SetParent(parent)
		self.SetUpVisual(PRIVATESHOP_PATH + "category_small_button_default.sub")
		self.SetOverVisual(PRIVATESHOP_PATH + "category_small_button_over.sub")
		self.SetDownVisual(PRIVATESHOP_PATH + "category_small_button_down.sub")
		self.Hide()

		self.textLine = ui.TextLine()
		self.textLine.SetParent(self)
		self.textLine.SetPosition(10, 2)
		self.textLine.Show()

		self.id = -1
		self.subType = -1

		self.orig_x = 0
		self.orig_y = 0

		self.active = False

	def __del__(self):
		ui.Button.__del__(self)

	def SetID(self, id):
		self.id = id

	def GetID(self):
		return self.id

	def SetActive(self, active):
		self.active = active

		if active:
			self.SetUpVisual(PRIVATESHOP_PATH + "category_small_button_down.sub")
			self.SetOverVisual(PRIVATESHOP_PATH + "category_small_button_down.sub")
			self.SetDownVisual(PRIVATESHOP_PATH + "category_small_button_down.sub")
		else:
			self.SetUpVisual(PRIVATESHOP_PATH + "category_small_button_default.sub")
			self.SetOverVisual(PRIVATESHOP_PATH + "category_small_button_over.sub")
			self.SetDownVisual(PRIVATESHOP_PATH + "category_small_button_down.sub")

	def IsActive(self):
		return self.active

	def SetSubType(self, subType):
		self.subType = subType

	def GetSubType(self):
		return self.subType

	def SetTitle(self, title):
		self.textLine.SetText(title)

	def AdjustOriginalPosition(self, x = 0, y = 0):
		self.orig_x += x
		self.orig_y += y

	def SetOriginalPosition(self, x, y):
		self.orig_x = x
		self.orig_y = y

	def AdjustPosition(self, x = 0, y = 0):
		self.SetPosition(self.orig_x + x, self.orig_y + y)

class MainCategory(ui.Button):
	def __init__(self, parent):
		ui.Button.__init__(self)

		self.SetParent(parent)
		self.SetUpVisual(PRIVATESHOP_PATH + "category_big_button_default.sub")
		self.SetOverVisual(PRIVATESHOP_PATH + "category_big_button_over.sub")
		self.SetDownVisual(PRIVATESHOP_PATH + "category_big_button_down.sub")
		self.Hide()

		self.textLine = ui.TextLine()
		self.textLine.SetParent(self)
		self.textLine.SetPosition(19, 2)
		self.textLine.Show()

		self.icon = ui.ImageBox()
		self.icon.SetParent(self)
		self.icon.SetPosition(6, 5)
		self.icon.LoadImage(PRIVATESEARCH_PATH + "arrow_down.tga")
		self.icon.Show()

		self.subCategories = {}
		self.id = -1
		self.type = -1
		self.active = False
		self.orig_x = 0
		self.orig_y = 0

	def __del__(self):
		ui.Button.__del__(self)

	def SetID(self, id):
		self.id = id

	def GetID(self):
		return self.id

	def SetActive(self, active):
		self.active = active

		if active:
			self.icon.LoadImage(PRIVATESEARCH_PATH + "arrow_up.tga")

			self.SetUpVisual(PRIVATESHOP_PATH + "category_big_button_down.sub")
			self.SetOverVisual(PRIVATESHOP_PATH + "category_big_button_over.sub")
			self.SetDownVisual(PRIVATESHOP_PATH + "category_big_button_default.sub")
		else:
			self.icon.LoadImage(PRIVATESEARCH_PATH + "arrow_down.tga")

			self.SetUpVisual(PRIVATESHOP_PATH + "category_big_button_default.sub")
			self.SetOverVisual(PRIVATESHOP_PATH + "category_big_button_over.sub")
			self.SetDownVisual(PRIVATESHOP_PATH + "category_big_button_down.sub")

	def IsActive(self):
		return self.active

	def SetType(self, type):
		self.type = type

	def GetType(self):
		return self.type

	def SetTitle(self, title):
		self.textLine.SetText(title)

	def GetTitle(self):
		return self.textLine.GetText()

	def AddSubCategory(self, id, subCategory):
		self.subCategories[id] = subCategory

	def GetSubCategory(self, id):
		return self.subCategories.get(id)

	def GetSubCategoryCount(self):
		return len(self.subCategories)

	def GetSubCategoryList(self):
		return list(self.subCategories.values())

	def Close(self):
		for subCategory in self.subCategories.values():
			subCategory.Hide()

		self.Hide()

	def AdjustOriginalPosition(self, x = 0, y = 0):
		self.orig_x += x
		self.orig_y += y

	def SetOriginalPosition(self, x, y):
		self.orig_x = x
		self.orig_y = y

	def AdjustPosition(self, x = 0, y = 0):
		self.SetPosition(self.orig_x + x, self.orig_y + y)

class RangeFilterWindow(ui.Window):
	def __init__(self, parent):
		ui.Window.__init__(self)
		self.SetParent(parent)

		self.SetSize(115, 30)

		# Title
		self.titleText = ui.TextLine()
		self.titleText.SetParent(self)
		self.titleText.SetPosition(self.GetWidth() / 2, 0)
		self.titleText.SetHorizontalAlignCenter()
		self.titleText.Show()

		# Min Slot BG
		min_bg = ui.ImageBox()
		min_bg.SetParent(self)
		min_bg.SetPosition(0, 13)
		min_bg.LoadImage(PRIVATESEARCH_PATH + "private_slot_dynamic_half.tga")
		min_bg.Show()
		self.min_bg = min_bg

		# Min Value Editline
		min_input = ui.EditLine()
		min_input.SetParent(self.min_bg)
		min_input.SetSize(self.min_bg.GetWidth() + 3, self.min_bg.GetHeight() + 3)
		min_input.SetPosition(2, 3)
		min_input.SetMax(6)
		min_input.SetNumberMode()
		min_input.SetBackgroundText(" " + localeInfo.PRIVATE_SHOP_SEARCH_MIN)
		min_input.SAFE_SetUpdateEvent(self.OnChangeMinValue)
		min_input.Show()
		self.min_input = min_input

		# Max Slot BG
		max_bg = ui.ImageBox()
		max_bg.SetParent(self)
		max_bg.LoadImage(PRIVATESEARCH_PATH + "private_slot_dynamic_half.tga")
		max_bg.SetPosition(self.GetWidth() - max_bg.GetWidth(), 13)
		max_bg.Show()
		self.max_bg = max_bg

		# Max Value Editline
		max_input = ui.EditLine()
		max_input.SetParent(self.max_bg)
		max_input.SetSize(self.max_bg.GetWidth() + 3, self.max_bg.GetHeight() + 3)
		max_input.SetPosition(2, 3)
		max_input.SetMax(6)
		max_input.SetNumberMode()
		max_input.SetBackgroundText(" " + localeInfo.PRIVATE_SHOP_SEARCH_MAX)
		max_input.SAFE_SetUpdateEvent(self.OnChangeMaxValue)
		max_input.Show()
		self.max_input = max_input

		# Seperator
		self.separator = ui.TextLine()
		self.separator.SetParent(self)
		self.separator.SetPosition(self.GetWidth() / 2, 16)
		self.separator.SetHorizontalAlignCenter()
		self.separator.SetText("~")
		self.separator.Show()

		self.minInputEvent		= None
		self.maxInputEvent		= None

		# Update
		self.max				= None

	def __del__(self):
		ui.Window.__del__(self)

	def Clear(self):
		self.min_input.SetText("")
		self.max_input.SetText("")

		self.OnChangeMinValue()
		self.OnChangeMaxValue()

	def SetTitle(self, title):
		self.titleText.SetText(title)

	def SetMinInputTabEvent(self, event, *args):
		if args:
			self.min_input.SetTabEvent(event, args)
		else:
			self.min_input.SetTabEvent(event)

	def SetMaxInputTabEvent(self, event, *args):
		if args:
			self.max_input.SetTabEvent(event, args)
		else:
			self.max_input.SetTabEvent(event)

	def SetMinInputReturnEvent(self, event, *args):
		if args:
			self.min_input.SetReturnEvent(event, args)
		else:
			self.min_input.SetReturnEvent(event)

	def SetMaxInputReturnEvent(self, event, *args):
		if args:
			self.max_input.SetReturnEvent(event, args)
		else:
			self.max_input.SetReturnEvent(event)

	def SetMinInputEscapeEvent(self, event, *args):
		if args:
			self.min_input.SetEscapeEvent(event, args)
		else:
			self.min_input.SetEscapeEvent(event)

	def SetMaxInputEscapeEvent(self, event, *args):
		if args:
			self.max_input.SetEscapeEvent(event, args)
		else:
			self.max_input.SetEscapeEvent(event)

	def SetInputMaxLenght(self, lenght):
		self.min_input.SetMax(lenght)
		self.max_input.SetMax(lenght)

	def SelectMinInput(self):
		self.min_input.SetFocus()
		self.min_input.SetEndPosition()

	def SelectMaxInput(self):
		self.max_input.SetFocus()
		self.max_input.SetEndPosition()

	def SetMinInputUpdateEvent(self, event):
		self.minInputEvent = event

	def SetMaxInputUpdateEvent(self, event):
		self.maxInputEvent = event

	def SetMin(self, min):
		if int(min) > 0:
			self.min_input.SetText(min)
		self.OnChangeMinValue()

	def SetMax(self, max):
		if int(max) > 0:
			self.max_input.SetText(max)
		self.OnChangeMaxValue()

	# Update
	def SetMaxValue(self, max):
		self.max = max

	def OnChangeMinValue(self):
		value = self.min_input.GetText()

		if value == "":
			self.min_input.SetBackgroundText(" " + localeInfo.PRIVATE_SHOP_SEARCH_MIN)
			self.minInputEvent(0)
		else:
			value = int(value)
			self.min_input.SetBackgroundText("")

			self.minInputEvent(value)

	def OnChangeMaxValue(self):
		value = self.max_input.GetText()

		if value == "":
			self.max_input.SetBackgroundText(" " + localeInfo.PRIVATE_SHOP_SEARCH_MAX)
			self.maxInputEvent(0)
		else:
			value = int(value)

		if value == "" or value == "0":
			value = 0
		else:
			value = int(value)
			self.max_input.SetBackgroundText("")

			self.maxInputEvent(value)

	def KillFocus(self):
		self.min_input.KillFocus()
		self.max_input.KillFocus()

	# Update
	def OnUpdate(self):
		min_input = self.min_input.GetText()
		max_input = self.max_input.GetText()

		if len(min_input) > 0:
			if int(min_input) > int(self.max):
				self.min_input.SetText(str(self.max))

			self.OnChangeMinValue()

		if len(max_input) > 0:
			if int(max_input) > int(self.max):
				self.max_input.SetText(str(self.max))

			self.OnChangeMaxValue()

class TypeValueFilter(ui.Window):
	def __init__(self, parent, x, y):
		self.typeSelector = None
		self.valueBackground = None
		self.valueInput = None
		ui.Window.__init__(self)

		typeSelector = ui.DynamicComboBoxImage(self, "d:/ymir work/ui/privatesearch/private_slot_dynamic.tga", -73, 0, True)
		typeSelector.SetEvent(lambda type: self.OnChangeItemType(type))
		typeSelector.Show()
		self.typeSelector = typeSelector

		valueBackground = ui.ImageBox()
		valueBackground.SetParent(self)
		valueBackground.LoadImage("d:/ymir work/ui/privatesearch/private_slot_dynamic_half.tga")
		valueBackground.SetPosition(120, 0)
		valueBackground.Show()
		self.valueBackground = valueBackground

		valueInput = ui.EditLine()
		valueInput.SetParent(self)
		valueInput.SetPosition(122, 4)
		valueInput.SetSize(valueBackground.GetWidth() + 3, valueBackground.GetHeight() + 3)
		valueInput.SetNumberMode()
		valueInput.SetMax(4)
		valueInput.SAFE_SetUpdateEvent(self.OnChangeItemValue)
		valueInput.SetBackgroundText(" 0")
		valueInput.Show()
		self.valueInput = valueInput

		self.SetParent(parent)
		self.SetSize(160, 17)
		self.SetPosition(x, y)

		self.typeChangeEvent = None
		self.valueChangeEvent = None

	def __del__(self):
		ui.Window.__del__(self)

	def Clear(self):
		# fix
		self.typeSelector.SetCurrentPosition(0)

		self.typeSelector.SelectItem(0)
		self.valueInput.SetText("")

		self.OnChangeItemValue()

	def IsOpened(self):
		if self.typeSelector.IsOpened():
			return True
		return False

	def SelectInput(self):
		self.valueInput.SetFocus()
		self.valueInput.SetEndPosition()

	def SetItem(self, key):
		self.typeSelector.SelectItem(key)

	def SetValue(self, value):
		self.valueInput.SetText(value)
		self.OnChangeItemValue()

	def SetItemValue(self, key, value):
		self.typeSelector.SelectItem(key)
		self.valueInput.SetText(value)
		self.OnChangeItemValue()

	def SetDefaultTitle(self, title):
		self.typeSelector.SetDefaultTitle(title)

	def UseDefaultTitle(self):
		self.typeSelector.UseDefaultTitle()

	def SetTitle(self, title):
		self.typeSelector.SetTitle(title)

	def InsertItem(self, type, value):
		self.typeSelector.InsertItem(type, value)

	def SetInputMaxLenght(self, lenght):
		self.valueInput.SetMax(lenght)

	def SetInputTabEvent(self, event, *args):
		if args:
			self.valueInput.SetTabEvent(event, args)
		else:
			self.valueInput.SetTabEvent(event)

	def SetInputReturnEvent(self, event, *args):
		if args:
			self.valueInput.SetReturnEvent(event, args)
		else:
			self.valueInput.SetReturnEvent(event)

	def SetInputEscapeEvent(self, event, *args):
		if args:
			self.valueInput.SetEscapeEvent(event, args)
		else:
			self.valueInput.SetEscapeEvent(event)

	def SetTypeChangeEvent(self, event):
		self.typeChangeEvent = event

	def SetValueChangeEvent(self, event):
		self.valueChangeEvent = event

	def OnChangeItemValue(self):
		value = self.valueInput.GetText()
		if value == "" or value == "0":
			value = 0
		else:
			value = int(value)

		if value == 0:
			self.valueInput.SetBackgroundText(" 0")
		else:
			self.valueInput.SetBackgroundText("")

		self.valueChangeEvent(value)

	def OnChangeItemType(self, value):
		self.typeChangeEvent(value)

	def KillFocus(self):
		self.valueInput.KillFocus()

class ItemSlot(ui.Window):
	def __init__(self, parent, x, y, size = 1, index = -1):
		ui.Window.__init__(self)

		# Button Background
		self.mainButton = ui.Button()
		self.mainButton.SetParent(self)
		self.mainButton.SetUpVisual(PRIVATESHOP_PATH + "item_slot_%d_default.sub" % (size))
		self.mainButton.SetOverVisual(PRIVATESHOP_PATH + "item_slot_%d_over.sub" % (size))
		self.mainButton.SetDownVisual(PRIVATESHOP_PATH + "item_slot_%d_down.sub" % (size))
		self.mainButton.SetPosition(0, 0)
		self.mainButton.SetEvent(ui.__mem_func__(self.OnSelect))
		self.mainButton.Show()

		# Item Information
		self.itemSlot = ui.ExpandedImageBox()
		self.itemSlot.SetParent(self.mainButton)
		self.itemSlot.SetPosition(7, 3)
		self.itemSlot.SetEvent(ui.__mem_func__(self.__OnOverInItem), "mouse_over_in")
		self.itemSlot.SetEvent(ui.__mem_func__(self.__OnOverOutItem), "mouse_over_out")
		self.itemSlot.Show()

		self.itemCountBackground = ui.ExpandedImageBox()
		self.itemCountBackground.SetParent(self.mainButton)
		self.itemCountBackground.AddFlag("not_pick")
		self.itemCountBackground.LoadImage("d:/ymir work/ui/search_count_slot.tga")
		self.itemCountBackground.Hide()

		self.itemCountText = ui.TextLine()
		self.itemCountText.SetParent(self.mainButton)
		self.itemCountText.Show()

		self.itemNameText = ui.TextLine()
		self.itemNameText.SetParent(self.mainButton)
		self.itemNameText.Show()

		# Price Information
		self.chequePriceIcon = ui.ExpandedImageBox()
		self.chequePriceIcon.SetParent(self.mainButton)
		self.chequePriceIcon.AddFlag("not_pick")
		self.chequePriceIcon.LoadImage("d:/ymir work/ui/game/windows/cheque_icon.sub")
		self.chequePriceIcon.Hide()

		self.chequePriceText = ui.TextLine()
		self.chequePriceText.SetParent(self.mainButton)
		self.chequePriceText.Hide()

		self.goldPriceIcon = ui.ExpandedImageBox()
		self.goldPriceIcon.SetParent(self.mainButton)
		self.goldPriceIcon.AddFlag("not_pick")
		self.goldPriceIcon.LoadImage("d:/ymir work/ui/game/windows/money_icon.sub")
		self.goldPriceIcon.Show()

		self.goldPriceText = ui.TextLine()
		self.goldPriceText.SetParent(self.mainButton)
		self.goldPriceText.Show()

		# Seller Information
		self.searchSellerContentButton = ui.Button()
		self.searchSellerContentButton.SetParent(self.mainButton)
		self.searchSellerContentButton.SetUpVisual(PRIVATESHOP_PATH + "sandglass_button_default.sub")
		self.searchSellerContentButton.SetOverVisual(PRIVATESHOP_PATH + "sandglass_button_over.sub")
		self.searchSellerContentButton.SetDownVisual(PRIVATESHOP_PATH + "sandglass_button_down.sub")
		self.searchSellerContentButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "SEARCH")
		self.searchSellerContentButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
		self.searchSellerContentButton.SetEvent(ui.__mem_func__(self.OnSearchSeller))
		self.searchSellerContentButton.Show()

		self.contactSellerButton = ui.Button()
		self.contactSellerButton.SetParent(self.mainButton)
		self.contactSellerButton.SetUpVisual(PRIVATESHOP_PATH + "letter_button_default.sub")
		self.contactSellerButton.SetOverVisual(PRIVATESHOP_PATH + "letter_button_over.sub")
		self.contactSellerButton.SetDownVisual(PRIVATESHOP_PATH + "letter_button_down.sub")
		self.contactSellerButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "CONTACT")
		self.contactSellerButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
		self.contactSellerButton.SetEvent(ui.__mem_func__(self.OnContactSeller))
		self.contactSellerButton.Show()

		self.sellerNameText = ui.TextLine()
		self.sellerNameText.SetParent(self.mainButton)
		self.sellerNameText.Show()

		self.SetParentProxy(parent)
		self.SetSize(self.mainButton.GetWidth(), self.mainButton.GetHeight())
		self.SetPosition(x, y)

		self.orig_x = x
		self.orig_y = y

		self.isSelected = False
		self.state = privateShop.STATE_AVAILABLE
		self.size = size
		self.index = index
		self.toolTip = None

	def __del__(self):
		ui.Window.__del__(self)

	def Initialize(self, size):
		self.isSelected = False
		self.state = privateShop.STATE_AVAILABLE
		self.size = size

		self.mainButton.SetUpVisual(PRIVATESHOP_PATH + "item_slot_%d_default.sub" % (size))
		self.mainButton.SetOverVisual(PRIVATESHOP_PATH + "item_slot_%d_over.sub" % (size))
		self.mainButton.SetDownVisual(PRIVATESHOP_PATH + "item_slot_%d_down.sub" % (size))
		self.mainButton.SetDiffuseColor(1.0, 1.0, 1.0, 1.0)

		self.SetSize(self.mainButton.GetWidth(), self.mainButton.GetHeight())

	def SetToolTip(self, toolTip):
		self.toolTip = proxy(toolTip)

	def SetSelected(self, selected):
		self.isSelected = selected

	def IsSelected(self):
		return self.isSelected

	def GetSize(self):
		return self.size

	def SetIndex(self, index):
		self.index = index

	def GetIndex(self):
		return self.index

	def OnSelect(self):
		if self.state == privateShop.STATE_REMOVED or self.state == privateShop.STATE_RESTRICTED:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATE_SHOP_BUY_ITEM_SELECTED_FAILURE)
			return

		result = self.onSelectEvent(self.GetIndex(), not self.IsSelected())

		if result:
			if self.IsSelected():
				self.Unselect()
				self.SetSelected(False)
			else:
				self.Select()
				self.SetSelected(True)

	def Select(self):
		self.mainButton.SetDownVisual(PRIVATESHOP_PATH + "item_slot_%d_default.sub" % (self.GetSize()))
		self.mainButton.SetUpVisual(PRIVATESHOP_PATH + "item_slot_%d_down.sub" % (self.GetSize()))
		self.mainButton.SetDiffuseColor(0.0, 1.0, 0.0, 1.0)

	def Unselect(self):
		self.mainButton.SetUpVisual(PRIVATESHOP_PATH + "item_slot_%d_default.sub" % (self.GetSize()))
		self.mainButton.SetDownVisual(PRIVATESHOP_PATH + "item_slot_%d_down.sub" % (self.GetSize()))
		if self.IsSelected():
			self.mainButton.SetDiffuseColor(1.0, 1.0, 1.0, 1.0)

	def OnSearchSeller(self):
		if self.onSearchSellerEvent:
			self.onSearchSellerEvent(self.fullSellerName)

	def OnContactSeller(self):
		if self.onContactSellerEvent:
			self.onContactSellerEvent(self.fullSellerName)

	def SetState(self, state):
		# Remove it from selected list if it was there
		if state == privateShop.STATE_REMOVED:
			if self.IsSelected():
				self.Unselect()
				self.SetSelected(False)

		# Once the item is removed its state cannot be changed
		if self.state == privateShop.STATE_REMOVED:
			return

		self.state = state

		if state == privateShop.STATE_REMOVED:
			self.mainButton.SetDiffuseColor(1.0, 0.0, 0.0, 1.0)

		elif state == privateShop.STATE_RESTRICTED:
			self.mainButton.SetDiffuseColor(1.0, 1.0, 0.0, 1.0)

		elif state == privateShop.STATE_AVAILABLE:
			self.mainButton.SetDiffuseColor(1.0, 1.0, 1.0, 1.0)

	def SetItem(self, image):
		self.itemSlot.LoadImage(image)

	def SetSellerName(self, name):
		self.fullSellerName = name
		self.sellerNameText.SetText(name)

		maxNameLenght = -1
		while self.sellerNameText.GetTextSize()[0] >= 100:
			self.sellerNameText.SetText(name[:maxNameLenght])
			maxNameLenght -= 1

		if maxNameLenght != -1:
			shortenedName = name[: maxNameLenght - 1] + '..'
			self.sellerNameText.SetText(shortenedName)

		self.sellerNameText.SetPosition(415, self.GetHeight() / 2 - self.sellerNameText.GetTextSize()[1] / 2)

	def SetItemName(self, name):
		self.itemNameText.SetText(name)

		maxNameLenght = -1
		while self.itemNameText.GetTextSize()[0] >= 125:
			self.itemNameText.SetText(name[:maxNameLenght])
			maxNameLenght -= 1

		if maxNameLenght != -1:
			shortenedName = name[: maxNameLenght - 1] + '..'
			self.itemNameText.SetText(shortenedName)

	if app.ENABLE_PRIVATE_SHOP_TIME_SORT:
		def IsItemWithTime(self, item_vnum):
			item.SelectItem(item_vnum)
			item_type = item.GetItemType()
			item_sub_type = item.GetItemSubType()

			if item_type == item.ITEM_TYPE_UNIQUE:
				if item.GetValue(5) > 0 and not item.IsFlag(item.ITEM_FLAG_IRREMOVABLE):
					return True

			if item_type == item.ITEM_TYPE_PET:
				return True

			if item_type == item.ITEM_TYPE_MOUNT:
				return True

			if item_type == item.ITEM_TYPE_COSTUME:
				if item_sub_type == item.COSTUME_TYPE_HAIR:
					return True

				if item_sub_type == item.COSTUME_TYPE_BODY:
					return True

				# if app.ENABLE_MOUNT_COSTUME_SYSTEM:
					# if item_sub_type == item.COSTUME_TYPE_MOUNT:
						# return True

				if app.ENABLE_WEAPON_COSTUME_SYSTEM:
					if item_sub_type == item.COSTUME_TYPE_WEAPON:
						return True

		def SetItemCount(self, item_vnum, count, socket):
			endTime = app.GetGlobalTimeStamp() + socket
			leftSec = max(0, endTime - app.GetGlobalTimeStamp())

			if self.IsItemWithTime(item_vnum):
				if socket >= 3600:
					self.itemCountText.SetText("|cffFF9D35|h{}".format(localeInfo.SecondToH(leftSec)))
				else:
					self.itemCountText.SetText("|cffFF9D35|h{}".format(localeInfo.SecondToM(leftSec)))
				self.itemCountBackground.Show()

				self.itemNameText.SetPosition(50 + 37, self.GetHeight() / 2 - self.itemNameText.GetTextSize()[1] / 2)
			elif count > 1:
				self.itemCountText.SetText("|cffFF9D35|hx{}".format(str(count)))
				self.itemCountBackground.Show()

				self.itemNameText.SetPosition(50 + 37, self.GetHeight() / 2 - self.itemNameText.GetTextSize()[1] / 2)
			else:
				self.itemCountText.SetText("")
				self.itemCountBackground.Hide()

				# Return to the initial position now that there is no count background
				self.itemNameText.SetPosition(50, self.mainButton.GetHeight() / 2 - self.itemNameText.GetTextSize()[1] / 2)

			# Set position in the middle after we see how large is the mainButton and how large the itemCountText is
			self.itemCountBackground.SetPosition(48, self.mainButton.GetHeight() / 2 - self.itemCountBackground.GetHeight() / 2)
			self.chequePriceIcon.SetPosition(215, self.mainButton.GetHeight() / 2 - self.chequePriceIcon.GetHeight() / 2)
			self.chequePriceText.SetPosition(233, self.mainButton.GetHeight() / 2 - self.chequePriceText.GetTextSize()[1] / 2)
			self.goldPriceIcon.SetPosition(250, self.mainButton.GetHeight() / 2 - self.goldPriceIcon.GetHeight() / 2)
			self.goldPriceText.SetPosition(268, self.mainButton.GetHeight() / 2 - self.goldPriceText.GetTextSize()[1] / 2)
			self.searchSellerContentButton.SetPosition(370, self.mainButton.GetHeight() / 2 - self.searchSellerContentButton.GetHeight() / 2)
			self.contactSellerButton.SetPosition(390, self.mainButton.GetHeight() / 2 - self.contactSellerButton.GetHeight() / 2)
			self.itemCountText.SetPosition(64 - self.itemCountText.GetTextSize()[0] / 2, self.mainButton.GetHeight() / 2 - self.itemCountText.GetTextSize()[1] / 2)
	else:
		def SetItemCount(self, count):
			if count > 1:
				self.itemCountText.SetText("|cffFF9D35|h[{}]".format(str(count)))
				self.itemCountBackground.Show()

				self.itemCountText.SetPosition(64 - self.itemCountText.GetTextSize()[0] / 2, self.mainButton.GetHeight() / 2 - self.itemCountText.GetTextSize()[1] / 2)
				self.itemNameText.SetPosition(50 + 37, self.GetHeight() / 2 - self.itemNameText.GetTextSize()[1] / 2)
			else:
				self.itemCountText.SetText("")
				self.itemCountBackground.Hide()

				self.itemCountText.SetPosition(64, self.mainButton.GetHeight() / 2 - self.itemCountText.GetTextSize()[1] / 2)
				self.itemNameText.SetPosition(50, self.GetHeight() / 2 - self.itemNameText.GetTextSize()[1] / 2)

	def SetGoldPrice(self, goldPrice):
		if goldPrice == 0:
			self.goldPriceText.SetText("")
			self.goldPriceText.Hide()

			self.goldPriceIcon.Hide()
		else:
			self.goldPriceText.SetText(localeInfo.NumberToMoneyStringNoUnit(goldPrice))
			self.goldPriceText.Show()

			self.goldPriceIcon.Show()

	def SetChequePrice(self, chequePrice):
		if chequePrice == 0:
			self.chequePriceText.SetText("")
			self.chequePriceText.Hide()

			self.chequePriceIcon.Hide()
		else:
			self.chequePriceText.SetText("|cff00D7FF|h" + localeInfo.NumberToMoneyStringNoUnit(chequePrice))
			self.chequePriceText.Show()

			self.chequePriceIcon.Show()

	def SetOnOverInItemEvent(self, event):
		self.onOverInItemEvent = event

	def SetOnOverOutItemEvent(self, event):
		self.onOverOutItemEvent = event

	def __OnOverInItem(self):
		self.onOverInItemEvent(self.GetIndex())

	def __OnOverOutItem(self):
		self.onOverOutItemEvent()

	def SetOnSelectEvent(self, event):
		self.onSelectEvent = event

	def SetOnSearchSellerEvent(self, event):
		self.onSearchSellerEvent = event

	def SetOnContactSellerEvent(self, event):
		self.onContactSellerEvent = event

	def SetOriginalPosition(self, x, y):
		self.orig_x = x
		self.orig_y = y

	def AdjustPosition(self, x = 0, y = 0):
		self.SetPosition(self.orig_x + x, self.orig_y + y)

	def __OnOverInButton(self, button):
		if not self.toolTip:
			return

		self.toolTip.ClearToolTip()

		text = ""
		if button == "SEARCH":
			text = uiScriptLocale.PRIVATESHOPSEARCH_SEARCH_SELLER

		elif button == "CONTACT":
			text = uiScriptLocale.PRIVATESHOPSEARCH_CONTACT_SELLER

		t_arglen = constInfo.CHECK_TEXT_LENGHT(text)

		self.toolTip.SetThinBoardSize(constInfo.CALC_T_ARGLEN(t_arglen))
		self.toolTip.AppendTextLine(text, constInfo.EXTRA_INFO_COLOR)
		self.toolTip.ShowToolTip() # Revised

	def __OnOverOutButton(self):
		if self.toolTip:
			self.toolTip.HideToolTip()

class PrivateShopSeachWindow(ui.ScriptWindow):
	CLICK_LIMIT_TIME	= 3
	PAGE_NUMBER_SIZE	= 5
	PAGEONE_MAX_SIZE = privateShop.RESULT_MAX_NUM * PAGE_NUMBER_SIZE
	SUGGESTION_MINIMAL_CHAR_REQ	= 3
	ATTRIBUTE_MAX_NUM			= 5
	ATTRIBUTE_TYPE				= 0
	ATTRIBUTE_VALUE				= 1
	MAX_CATEGORY				= 12

	FILTER_CATEGORY				= 1
	FILTER_DETAILS				= 2

	ITEM_SEARCH_MODE			= 1
	PLAYER_SEARCH_MODE			= 2

	SPECIAL_TITLE_COLOR  = 0xff4E3D30

	JOB_MAX_COUNT = 4
	JOB_NAME_DICT = {
		NONE_SELECTED					:		localeInfo.PRIVATE_SHOP_SEARCH_SELECT_NONE,
		1								:		localeInfo.JOB_WARRIOR,
		2								:		localeInfo.JOB_ASSASSIN,
		3								:		localeInfo.JOB_SURA,
		4								:		localeInfo.JOB_SHAMAN,
	}

	GENDER_MAX_COUNT = 2
	GENDER_NAME_DICT = {
		NONE_SELECTED					:		localeInfo.PRIVATE_SHOP_SEARCH_SELECT_NONE,
		1								:		localeInfo.PRIVATE_SHOP_SEARCH_GENDER_MALE,
		2								:		localeInfo.PRIVATE_SHOP_SEARCH_GENDER_FEMALE,
	}

	AFFECT_DICT = {
		NONE_SELECTED						:		localeInfo.PRIVATE_SHOP_SEARCH_SELECT_NONE,
		item.APPLY_MAX_HP					:		uiScriptLocale.SELECT_HP,
		item.APPLY_MAX_SP					:		uiScriptLocale.SELECT_SP,
		item.APPLY_CON						:		localeInfo.DETAILS_CON,
		item.APPLY_INT						:		localeInfo.DETAILS_INT,
		item.APPLY_STR						:		localeInfo.DETAILS_STR,
		item.APPLY_DEX						:		localeInfo.DETAILS_DEX,
		item.APPLY_ATT_SPEED				:		localeInfo.STAT_TOOLTIP_ATT_SPEED,
		item.APPLY_MOV_SPEED				:		localeInfo.STAT_TOOLTIP_MOVE_SPEED,
		item.APPLY_CAST_SPEED				:		localeInfo.STAT_TOOLTIP_CAST_SPEED,
		item.APPLY_HP_REGEN					:		localeInfo.DETAILS_61,
		item.APPLY_SP_REGEN					:		localeInfo.DETAILS_62,
		item.APPLY_POISON_PCT				:		localeInfo.DETAILS_55,
		item.APPLY_STUN_PCT					:		localeInfo.DETAILS_53,
		item.APPLY_SLOW_PCT					:		localeInfo.DETAILS_54,
		item.APPLY_CRITICAL_PCT				:		localeInfo.DETAILS_20,
		item.APPLY_PENETRATE_PCT			:		localeInfo.DETAILS_21,
		item.APPLY_ATTBONUS_HUMAN			:		localeInfo.DETAILS_1,
		item.APPLY_ATTBONUS_ANIMAL			:		localeInfo.DETAILS_7,
		item.APPLY_ATTBONUS_ORC				:		localeInfo.DETAILS_3,
		item.APPLY_ATTBONUS_MILGYO			:		localeInfo.DETAILS_8,
		item.APPLY_ATTBONUS_UNDEAD			:		localeInfo.DETAILS_4,
		item.APPLY_ATTBONUS_DEVIL			:		localeInfo.DETAILS_9,
		item.APPLY_STEAL_HP					:		localeInfo.DETAILS_59,
		item.APPLY_STEAL_SP					:		localeInfo.DETAILS_60,
		item.APPLY_MANA_BURN_PCT			:		localeInfo.DETAILS_6,
		item.APPLY_BLOCK					:		localeInfo.DETAILS_63,
		item.APPLY_DODGE					:		localeInfo.DETAILS_64,
		item.APPLY_RESIST_SWORD				:		localeInfo.DETAILS_46,
		item.APPLY_RESIST_TWOHAND			:		localeInfo.DETAILS_47,
		item.APPLY_RESIST_DAGGER			:		localeInfo.DETAILS_48,
		item.APPLY_RESIST_BELL				:		localeInfo.DETAILS_50,
		item.APPLY_RESIST_FAN				:		localeInfo.DETAILS_51,
		item.APPLY_RESIST_BOW				:		localeInfo.DETAILS_52,
		item.APPLY_RESIST_FIRE				:		localeInfo.DETAILS_27,
		item.APPLY_RESIST_ELEC				:		localeInfo.DETAILS_24,
		item.APPLY_RESIST_MAGIC				:		localeInfo.DETAILS_76,
		item.APPLY_RESIST_WIND				:		localeInfo.DETAILS_28,
		item.APPLY_REFLECT_MELEE			:		localeInfo.DETAILS_65,
		item.APPLY_POISON_REDUCE			:		localeInfo.DETAILS_56,
		item.APPLY_EXP_DOUBLE_BONUS			:		localeInfo.DETAILS_68,
		item.APPLY_GOLD_DOUBLE_BONUS		:		localeInfo.DETAILS_69,
		item.APPLY_ITEM_DROP_BONUS			:		localeInfo.DETAILS_70,
		item.APPLY_IMMUNE_STUN				:		localeInfo.DETAILS_FAINT_RES,
		item.APPLY_IMMUNE_SLOW				:		localeInfo.DETAILS_SLOW_RES,
		item.APPLY_ATT_GRADE_BONUS			:		localeInfo.DETAILS_12,
		item.APPLY_NORMAL_HIT_DAMAGE_BONUS	:		localeInfo.DETAILS_14,
		item.APPLY_SKILL_DAMAGE_BONUS		:		localeInfo.DETAILS_16,
	}

	ALCHEMY_LEVEL_DICT = {
		player.DRAGON_SOUL_STEP_LOWEST		:		localeInfo.DRAGON_SOUL_STRENGTH(player.DRAGON_SOUL_STEP_LOWEST),
		player.DRAGON_SOUL_STEP_LOW			:		localeInfo.DRAGON_SOUL_STRENGTH(player.DRAGON_SOUL_STEP_LOW),
		player.DRAGON_SOUL_STEP_MID			:		localeInfo.DRAGON_SOUL_STRENGTH(player.DRAGON_SOUL_STEP_MID),
		player.DRAGON_SOUL_STEP_HIGH		:		localeInfo.DRAGON_SOUL_STRENGTH(player.DRAGON_SOUL_STEP_HIGH),
		player.DRAGON_SOUL_STEP_HIGHEST		:		localeInfo.DRAGON_SOUL_STRENGTH(player.DRAGON_SOUL_STEP_HIGHEST),
	}

	ALCHEMY_CLARITY_DICT = {
		player.DRAGON_SOUL_GRADE_NORMAL		:		localeInfo.DRAGON_SOUL_STEP_LEVEL1,
		player.DRAGON_SOUL_GRADE_BRILLIANT	:		localeInfo.DRAGON_SOUL_STEP_LEVEL2,
		player.DRAGON_SOUL_GRADE_RARE		:		localeInfo.DRAGON_SOUL_STEP_LEVEL3,
		player.DRAGON_SOUL_GRADE_ANCIENT	:		localeInfo.DRAGON_SOUL_STEP_LEVEL4,
		player.DRAGON_SOUL_GRADE_LEGENDARY	:		localeInfo.DRAGON_SOUL_STEP_LEVEL5,
	}

	CATEGORY_FILTER_DICT = {
		0 : (
			item.ITEM_TYPE_WEAPON,
			localeInfo.CATEGORY_EQUIPMENT_WEAPON,
			{
				item.WEAPON_SWORD				:		localeInfo.CATEGORY_WEAPON_WEAPON_SWORD,
				item.WEAPON_TWO_HANDED			:		localeInfo.CATEGORY_WEAPON_WEAPON_TWO_HANDED,
				item.WEAPON_DAGGER				:		localeInfo.CATEGORY_WEAPON_WEAPON_DAGGER,
				item.WEAPON_BOW					:		localeInfo.CATEGORY_WEAPON_WEAPON_BOW,
				item.WEAPON_ARROW				:		localeInfo.CATEGORY_WEAPON_WEAPON_ARROW,
				item.WEAPON_BELL				:		localeInfo.CATEGORY_WEAPON_WEAPON_BELL,
				item.WEAPON_FAN					:		localeInfo.CATEGORY_WEAPON_WEAPON_FAN,
			}
		),

		1 : (
			item.ITEM_TYPE_ARMOR,
			localeInfo.CATEGORY_EQUIPMENT_ARMOR,
			{
				item.ARMOR_BODY					:		localeInfo.CATEGORY_ARMOR_ARMOR_BODY,
				item.ARMOR_HEAD					:		localeInfo.CATEGORY_ARMOR_ARMOR_HEAD,
				item.ARMOR_SHIELD				:		localeInfo.CATEGORY_ARMOR_ARMOR_SHIELD,
				item.ARMOR_WRIST				:		localeInfo.CATEGORY_JEWELRY_ARMOR_WRIST,
				item.ARMOR_FOOTS				:		localeInfo.CATEGORY_JEWELRY_ARMOR_FOOTS,
				item.ARMOR_NECK					:		localeInfo.CATEGORY_JEWELRY_ARMOR_NECK,
				item.ARMOR_EAR					:		localeInfo.CATEGORY_JEWELRY_ARMOR_EAR,
			}
		),

		2 : (
			item.ITEM_TYPE_COSTUME,
			localeInfo.CATEGORY_COSTUMES,
			{
				item.COSTUME_TYPE_BODY				:		localeInfo.CATEGORY_COSTUMES_COSTUME_BODY,
				item.COSTUME_TYPE_HAIR				:		localeInfo.CATEGORY_COSTUMES_COSTUME_HAIR,
				item.COSTUME_TYPE_WEAPON			:		localeInfo.CATEGORY_COSTUMES_COSTUME_WEAPON,
				# item.COSTUME_TYPE_MOUNT				:		localeInfo.CATEGORY_MOUNT_PET_MOUNT,
				item.COSTUME_TYPE_ACCE				:		localeInfo.CATEGORY_COSTUMES_SASH,
			}
		),

		3 : (
			item.ITEM_TYPE_BELT,
			localeInfo.CATEGORY_JEWELRY_ITEM_BELT,
			{
				0									:		localeInfo.CATEGORY_JEWELRY_ITEM_BELT,
			}
		),

		4 : (
			item.ITEM_TYPE_PET,
			localeInfo.CATEGORY_MOUNT_PET_CHARGED_PET,
			{
				0									:		localeInfo.CATEGORY_MOUNT_PET_CHARGED_PET,
			}
		),

		5 : (
			item.ITEM_TYPE_GIFTBOX,
			localeInfo.CATEGORY_ETC_GIFTBOX,
			{
				0									:		localeInfo.CATEGORY_ETC_GIFTBOX,
			}
		),


		6 : (
			item.ITEM_TYPE_METIN,
			"Stone",
			{
				0									:		"All Stone",
			}
		),

		7 : (
			item.ITEM_TYPE_MOUNT,
			localeInfo.CATEGORY_MOUNT_PET_MOUNT,
			{
				0									:		localeInfo.CATEGORY_MOUNT_PET_MOUNT,
			}
		),

		8 : (
			item.ITEM_TYPE_BLEND,
			"Blends",
			{
				0									:		"Blends",
			}
		),

		9 : (
			item.ITEM_TYPE_DS,
			"Alchemy",
			{
				0									:		"All Dragon Stone",
			}
		),
	}

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.interface			= None
		self.toolTipItem		= None
		self.questionDialog		= None
		self.itemNamesListBox	= None

		self.mode					= privateShop.MODE_NONE
		self.filter_mode			= self.FILTER_CATEGORY
		self.search_mode			= self.ITEM_SEARCH_MODE

		self.lastSearchClickTime	= -1
		self.lastBuyClickTime		= -1

		self.selectedItemSet		= set()
		self.selectedCategory		= -1
		self.selectedSubCategory	= -1
		self.firstCategoryIndex		= 0
		self.activeCategoryList		= []

		self.filterItemDict			= {
			"PAGE_1" : {},
			"PAGE_2" : {},
		}

		self.selectedFilterPage		= "PAGE_1"
		self.filter_config			= filter_config_template.copy()

		self.categoryDict			= {}
		self.itemSlotResultDict		= {}
		self.pageButtonDict			= None
		self.currentPageNumber		= 1
		self.pageCount				= 0
		self.bigPageCount			= 1

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.lookingGlassPos = 0

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.HideToolTip()

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.wndInventory = None
			# self.lockedItem = (-1,-1)

		self.__LoadWindow()

	def __del__(self):
		self.toolTipItem		= None
		self.questionDialog		= None

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.wndInventory = None
			# self.lockedItem = (-1,-1)

		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/privateshopsearchwindow.py")
		except:
			import exception
			exception.Abort("PrivateShopSeachWindow.__LoadWindow.UIScript/PrivateShopSearchWindow.py")
		try:
			self.board = self.GetChild("board")
			self.board.SetCloseEvent(ui.__mem_func__(self.Close))

			self.filterButton = self.GetChild("FilterButton")
			self.filterButton.SetEvent(ui.__mem_func__(self.ChangeFilterMode))

			self.categoryWindow = self.GetChild("CategoryWindow")
			if app.ENABLE_INGAME_WIKI:
				self.categoryWindow.SetInsideRender(True)

			self.categoryScrollBar = self.GetChild("CategoryScrollBar")
			self.categoryScrollBar.SetScrollEvent(ui.__mem_func__(self.OnCategoryScroll))

			self.searchButton = self.GetChild("SearchButton")
			self.searchButton.SetEvent(ui.__mem_func__(self.Search))

			self.buyButton = self.GetChild("BuyButton")
			self.buyButton.SetEvent(ui.__mem_func__(self.Buy))

			self.clearSelectedItemButton = self.GetChild("ClearSelectedItemButton")
			self.clearSelectedItemButton.SetEvent(ui.__mem_func__(self.ClearSelectedItem))

			self.fitlerWindow = self.GetChild("FilterWindow")
			self.fitlerWindow.Hide()

			self.itemModeButton = self.GetChild("ItemModeButton")
			self.itemModeButton.SetEvent(ui.__mem_func__(self.SetItemSearchMode))

			self.playerModeButton = self.GetChild("PlayerModeButton")
			self.playerModeButton.SetEvent(ui.__mem_func__(self.SetPlayerSearchMode))

			self.generalPageFilterButton = self.GetChild("GeneralFilterModeButton")
			self.generalPageFilterButton.SetEvent(ui.__mem_func__(self.SelectFilterPage), "PAGE_1")
			self.generalPageFilterButton.Hide()

			self.attrPageFilterButton = self.GetChild("AttrFilterModeButton")
			self.attrPageFilterButton.SetEvent(ui.__mem_func__(self.SelectFilterPage), "PAGE_2")
			self.attrPageFilterButton.Hide()

			self.clearFilterButton = self.GetChild("ClearFilterButton")
			self.clearFilterButton.SetEvent(ui.__mem_func__(self.ClearFilter))

			self.nameInput = self.GetChild("NameInput")
			self.nameInput.SetBackgroundText("Search")
			self.nameInput.SetEscapeEvent(ui.__mem_func__(self.Close))
			self.nameInput.SetUpdateEvent(ui.__mem_func__(self.__OnNameInputUpdate))
			self.nameInput.SetTabEvent(ui.__mem_func__(self.__SetNameInputResult))

			self.itemResultScrollBar = self.GetChild("ItemResultScrollBar")
			self.itemResultScrollBar.SetScrollEvent(ui.__mem_func__(self.OnItemResultScroll))
			self.itemResultScrollBar.Hide()

			self.pageButtonDict = {
				"PAGE_FIRST_PREV"	: self.GetChild("FirstPrevButton"),
				"PAGE_PREV"			: self.GetChild("PrevButton"),
				"PAGE_1"			: self.GetChild("Page1Button"),
				"PAGE_2"			: self.GetChild("Page2Button"),
				"PAGE_3"			: self.GetChild("Page3Button"),
				"PAGE_4"			: self.GetChild("Page4Button"),
				"PAGE_5"			: self.GetChild("Page5Button"),
				"PAGE_NEXT"			: self.GetChild("NextButton"),
				"PAGE_LAST_NEXT"	: self.GetChild("LastNextButton"),
			}

			self.pageButtonDict["PAGE_FIRST_PREV"].SetEvent(ui.__mem_func__(self.FirstPrevPage))
			self.pageButtonDict["PAGE_PREV"].SetEvent(ui.__mem_func__(self.PrevPage))
			self.pageButtonDict["PAGE_1"].SetEvent(ui.__mem_func__(self.SelectPage), 1)
			self.pageButtonDict["PAGE_2"].SetEvent(ui.__mem_func__(self.SelectPage), 2)
			self.pageButtonDict["PAGE_3"].SetEvent(ui.__mem_func__(self.SelectPage), 3)
			self.pageButtonDict["PAGE_4"].SetEvent(ui.__mem_func__(self.SelectPage), 4)
			self.pageButtonDict["PAGE_5"].SetEvent(ui.__mem_func__(self.SelectPage), 5)
			self.pageButtonDict["PAGE_NEXT"].SetEvent(ui.__mem_func__(self.NextPage))
			self.pageButtonDict["PAGE_LAST_NEXT"].SetEvent(ui.__mem_func__(self.LastNextPage))

			self.itemResultWindowMask = self.GetChild("ItemResultWindowMask")
			if app.ENABLE_INGAME_WIKI:
				self.itemResultWindowMask.SetInsideRender(True)

			if app.ENABLE_PRIVATE_SHOP_NO_RESULT_UPDATE:
				self.noResultsImage = self.GetChild("noResultsImage")
				self.noResultsText = self.GetChild("noResultsText")
				self.noResultsImage.Hide()
				self.noResultsText.Hide()

			# Button Tooltips
			self.filterButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "FILTER")
			self.filterButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.buyButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "BUY")
			self.buyButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.searchButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "SEARCH")
			self.searchButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.buyButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "BUY")
			self.buyButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.clearSelectedItemButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "CLEAR_SELECTED_ITEMS")
			self.clearSelectedItemButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.itemModeButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "ITEM_MODE")
			self.itemModeButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.playerModeButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "PLAYER_MODE")
			self.playerModeButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.generalPageFilterButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "GENERAL_FILTER")
			self.generalPageFilterButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.attrPageFilterButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "ATTR_FILTER")
			self.attrPageFilterButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.clearFilterButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "CLEAR_FILTER")
			self.clearFilterButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.pageButtonDict["PAGE_FIRST_PREV"].SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "PAGE_FIRST_PREV")
			self.pageButtonDict["PAGE_FIRST_PREV"].SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.pageButtonDict["PAGE_PREV"].SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "PAGE_PREV")
			self.pageButtonDict["PAGE_PREV"].SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.pageButtonDict["PAGE_NEXT"].SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "PAGE_NEXT")
			self.pageButtonDict["PAGE_NEXT"].SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.pageButtonDict["PAGE_LAST_NEXT"].SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "PAGE_LAST_NEXT")
			self.pageButtonDict["PAGE_LAST_NEXT"].SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.__MakeResultSlots()
			self.__MakeFilter()
			self.SelectFilterPage("PAGE_1")

			self.__MakeCategoryButton()
			self.__ShowCategory()

			self.itemNamesListBox = ui.DropDownList(self, -12, 77)
			self.itemNamesListBox.SetSize(width = 200)
			self.itemNamesListBox.SetItemMode(True)
			self.itemNamesListBox.SetEvent(self.SelectItem)

			self.filterSelectDialog = FilterSelectDialog(self)
			self.filterSelectDialog.SetParent(self)
			self.filterSelectDialog.SetPosition(175, 55)

			self.SetItemSearchMode()

			# Hide unavailable buttons
			self.HidePageButton()
			self.buyButton.Hide()

			self.clearSelectedItemButton.Hide()
			self.SetCenterPosition()
		except:
			import exception
			exception.Abort("PrivateShopSeachWindow.__LoadWindow.PrivateShopSearchDialog")

	def __MakeResultSlots(self):
		for i in range(privateShop.RESULT_MAX_NUM):
			itemSlot = ItemSlot(self.itemResultWindowMask, 3, 0)
			itemSlot.SetOnOverInItemEvent(lambda index = i: self.OverInItem(index))
			itemSlot.SetOnOverOutItemEvent(self.OverOutItem)
			itemSlot.SetOnSelectEvent(lambda index, selected: self.OnSelectItem(index, selected))
			itemSlot.SetToolTip(self.toolTip)
			itemSlot.SetOnContactSellerEvent(self.OnContactSeller)
			itemSlot.SetOnSearchSellerEvent(self.OnSearchSeller)
			itemSlot.Hide()

			self.itemSlotResultDict[i] = itemSlot

	def __MakeCategoryButton(self):
		idx = 0
		for (type, name, subcategoryDict) in sorted(self.CATEGORY_FILTER_DICT.values()):
			category = MainCategory(self.categoryWindow)
			category.SetID(idx)
			category.SetTitle(name)
			category.SetType(type)
			category.SAFE_SetEvent(self.OnClickMainCategory, idx)
			category.Hide()

			subCategoryCount = len(subcategoryDict)
			if subCategoryCount > 0:
				for subtype, name in sorted(subcategoryDict.items()):
					idx += 1

					subCategory = SubCategory(self.categoryWindow)
					subCategory.SetID(idx)
					subCategory.SetTitle(name)
					subCategory.SetSubType(subtype)
					subCategory.SAFE_SetEvent(self.OnClickSubCategory, idx)
					subCategory.Hide()

					category.AddSubCategory(idx, subCategory)

			self.categoryDict[category.GetID()] = category
			idx += 1

	def __MakeFilter(self):
		self.__MakeFilterDefault()
		self.__MakeFilterAttributes()

	def __MakeFilterDefault(self):
		# Class
		Y_POS = 9
		classifiersBackground = ui.ImageBox()
		classifiersBackground.SetParent(self.fitlerWindow)
		classifiersBackground.LoadImage(PRIVATESEARCH_PATH + "title.tga")
		classifiersBackground.SetPosition(0, Y_POS)
		classifiersBackground.SetWindowHorizontalAlignCenter()
		classifiersBackground.Show()
		self.filterItemDict["PAGE_1"]["classifiersBackground"] = classifiersBackground

		classifiersTitle = ui.TextLine()
		classifiersTitle.SetParent(self.fitlerWindow)
		classifiersTitle.SetPosition(self.fitlerWindow.GetWidth() / 2, Y_POS + 3)
		classifiersTitle.SetHorizontalAlignCenter()
		classifiersTitle.SetText(localeInfo.PRIVATE_SHOP_SEARCH_TITLE_CLASSIFIERS)
		classifiersTitle.SetPackedFontColor(0xFFFEE3AE)
		classifiersTitle.Show()
		self.filterItemDict["PAGE_1"]["classifiersTitle"] = classifiersTitle

		# Limits
		Y_POS += 60
		limitsBackground = ui.ImageBox()
		limitsBackground.SetParent(self.fitlerWindow)
		limitsBackground.LoadImage(PRIVATESEARCH_PATH + "title.tga")
		limitsBackground.SetPosition(0, Y_POS)
		limitsBackground.SetWindowHorizontalAlignCenter()
		limitsBackground.Show()
		self.filterItemDict["PAGE_1"]["limitsBackground"] = limitsBackground

		limitsTitle = ui.TextLine()
		limitsTitle.SetParent(self.fitlerWindow)
		limitsTitle.SetPosition(self.fitlerWindow.GetWidth() / 2, Y_POS + 3)
		limitsTitle.SetHorizontalAlignCenter()
		limitsTitle.SetText(localeInfo.PRIVATE_SHOP_SEARCH_TITLE_LIMITS)
		limitsTitle.SetPackedFontColor(0xFFFEE3AE)
		limitsTitle.Show()
		self.filterItemDict["PAGE_1"]["limitsTitle"] = limitsTitle

		Y_POS += 25
		## Stack
		stackLimit = RangeFilterWindow(self.fitlerWindow)
		stackLimit.SetPosition(0, Y_POS)
		stackLimit.SetWindowHorizontalAlignCenter()
		stackLimit.SetTitle(localeInfo.PRIVATE_SHOP_SEARCH_LIMIT_STACK)
		stackLimit.Show()
		self.filterItemDict["PAGE_1"]["stackLimit"] = stackLimit

		Y_POS += stackLimit.GetHeight() + 8
		## Refinement
		refinementLimit = RangeFilterWindow(self.fitlerWindow)
		refinementLimit.SetPosition(0, Y_POS)
		refinementLimit.SetWindowHorizontalAlignCenter()
		refinementLimit.SetTitle(localeInfo.PRIVATE_SHOP_SEARCH_LIMIT_REFINEMENT)
		refinementLimit.Show()
		self.filterItemDict["PAGE_1"]["refinementLimit"] = refinementLimit

		Y_POS += refinementLimit.GetHeight() + 8
		## Level
		levelLimit = RangeFilterWindow(self.fitlerWindow)
		levelLimit.SetPosition(0, Y_POS)
		levelLimit.SetWindowHorizontalAlignCenter()
		levelLimit.SetTitle(localeInfo.PRIVATE_SHOP_SEARCH_LIMIT_LEVEL)
		levelLimit.Show()
		self.filterItemDict["PAGE_1"]["levelLimit"] = levelLimit

		# Bind events for limit filters
		stackLimit.SetMinInputTabEvent(stackLimit.SelectMaxInput)
		stackLimit.SetMaxInputTabEvent(stackLimit.SelectMinInput)
		stackLimit.SetMinInputReturnEvent(stackLimit.SelectMaxInput)
		stackLimit.SetMaxInputReturnEvent(refinementLimit.SelectMinInput)
		stackLimit.SetMinInputEscapeEvent(self.Close)
		stackLimit.SetMaxInputEscapeEvent(self.Close)
		stackLimit.SetMinInputUpdateEvent(lambda value: self.OnChangeMinStack(value))
		stackLimit.SetMaxInputUpdateEvent(lambda value: self.OnChangeMaxStack(value))
		stackLimit.SetInputMaxLenght(3)
		stackLimit.SetMaxValue(250)

		refinementLimit.SetMinInputTabEvent(refinementLimit.SelectMaxInput)
		refinementLimit.SetMaxInputTabEvent(refinementLimit.SelectMinInput)
		refinementLimit.SetMinInputReturnEvent(refinementLimit.SelectMaxInput)
		refinementLimit.SetMaxInputReturnEvent(levelLimit.SelectMinInput)
		refinementLimit.SetMinInputEscapeEvent(self.Close)
		refinementLimit.SetMaxInputEscapeEvent(self.Close)
		refinementLimit.SetMinInputUpdateEvent(lambda value: self.OnChangeMinRefinement(value))
		refinementLimit.SetMaxInputUpdateEvent(lambda value: self.OnChangeMaxRefinement(value))
		refinementLimit.SetInputMaxLenght(1)
		refinementLimit.SetMaxValue(9)

		levelLimit.SetMinInputTabEvent(levelLimit.SelectMaxInput)
		levelLimit.SetMaxInputTabEvent(levelLimit.SelectMinInput)
		levelLimit.SetMinInputReturnEvent(levelLimit.SelectMaxInput)
		levelLimit.SetMaxInputReturnEvent(stackLimit.SelectMinInput)
		levelLimit.SetMinInputEscapeEvent(self.Close)
		levelLimit.SetMaxInputEscapeEvent(self.Close)
		levelLimit.SetMinInputUpdateEvent(lambda value: self.OnChangeMinLevel(value))
		levelLimit.SetMaxInputUpdateEvent(lambda value: self.OnChangeMaxLevel(value))
		levelLimit.SetInputMaxLenght(2)
		levelLimit.SetMaxValue(99)

		# @note: We're spawning this class last & reversed as rendering order will be messed up when opening the list
		Y_POS = 32

		# Class
		classSelectSlot = ui.DynamicComboBoxImage(self.fitlerWindow, "d:/ymir work/ui/privatesearch/private_slot_dynamic.tga", 1, Y_POS)
		for key, text in self.JOB_NAME_DICT.items():
			classSelectSlot.InsertItem(key, text)
		classSelectSlot.SetDefaultTitle(localeInfo.PRIVATE_SHOP_SEARCH_SELECT_JOB)
		classSelectSlot.SetEvent(lambda class_type: self.OnChangeClass(class_type))
		classSelectSlot.Show()
		self.filterItemDict["PAGE_1"]["classSelectSlot"] = classSelectSlot

		# Hide filter contents if selection list is oppened
		self.limitsBackground = limitsBackground
		self.limitsTitle = limitsTitle
		self.stackLimit = stackLimit
		self.refinementLimit = refinementLimit
		self.levelLimit = levelLimit

		self.classSelectSlot = classSelectSlot

	def __MakeFilterAttributes(self):
		# @note: We're only spawning selectors in reversed as rendering order
		# will be messed up when opening the list otherwise
		Y_POS = 6

		attributesBackground = ui.ImageBox()
		attributesBackground.SetParent(self.fitlerWindow)
		attributesBackground.LoadImage(PRIVATESEARCH_PATH + "title.tga")
		attributesBackground.SetPosition(0, Y_POS + 215)
		attributesBackground.SetWindowHorizontalAlignCenter()
		attributesBackground.Show()
		self.filterItemDict["PAGE_1"]["attributesBackground"] = attributesBackground

		attributesTitle = ui.TextLine()
		attributesTitle.SetParent(self.fitlerWindow)
		attributesTitle.SetPosition(self.fitlerWindow.GetWidth() / 2, Y_POS + 3 + 215)
		attributesTitle.SetHorizontalAlignCenter()
		attributesTitle.SetText(localeInfo.PRIVATE_SHOP_SEARCH_TITLE_ATTRIBUTES)
		attributesTitle.SetPackedFontColor(0xFFFEE3AE)
		attributesTitle.Show()
		self.filterItemDict["PAGE_1"]["attributesTitle"] = attributesTitle

		Y_POS = 147
		Y_POS += 48
		Y_POS = 122 - 20 + 215

		temp_attr_filter_list = []
		# Attributes
		for i in range(5):
			attrFilter = TypeValueFilter(self.fitlerWindow, 3, Y_POS)

			for (key, apply) in self.AFFECT_DICT.items():
				attrFilter.InsertItem(key, apply)

			attrFilter.SetDefaultTitle(localeInfo.PRIVATE_SHOP_SEARCH_SELECT_ATTR % (5 - i))
			attrFilter.SetInputMaxLenght(4)
			attrFilter.Show()
			temp_attr_filter_list.append(attrFilter)

			Y_POS -= attrFilter.GetHeight() + 1

		temp_attr_filter_list.reverse()

		for i, filter in enumerate(temp_attr_filter_list):
			next_filter_idx = i + 1
			if next_filter_idx >= len(temp_attr_filter_list):
				next_filter_idx = 0

			filter.SetInputTabEvent(temp_attr_filter_list[next_filter_idx].SelectInput)
			filter.SetInputReturnEvent(temp_attr_filter_list[next_filter_idx].SelectInput)
			filter.SetInputEscapeEvent(self.Close)
			filter.SetTypeChangeEvent(lambda type, argSlot = i + 1: self.OnChangeAttrType(argSlot, type))
			filter.SetValueChangeEvent(lambda value, argSlot = i + 1: self.OnChangeAttrValue(argSlot, value))

			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter%d" % (i + 1)] = filter

	def SetItemSearchMode(self):
		self.itemModeButton.Down()
		self.playerModeButton.SetUp()

		self.search_mode = self.ITEM_SEARCH_MODE

		self.nameInput.SetMax(item.ITEM_NAME_MAX_LEN)
		self.nameInput.SetBackgroundText("")
		self.nameInput.SetText("")
		self.nameInput.SetFocus()

		if self.itemNamesListBox.IsOpened():
			self.itemNamesListBox.CloseListBox()

		self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = 0
		self.filter_config[privateShop.FILTER_TYPE_OWNER_NAME] = ""

	def SetPlayerSearchMode(self):
		self.itemModeButton.SetUp()
		self.playerModeButton.Down()

		self.search_mode = self.PLAYER_SEARCH_MODE

		self.nameInput.SetMax(player.CHARACTER_NAME_MAX_LEN)
		self.nameInput.SetBackgroundText("")
		self.nameInput.SetText("")
		self.nameInput.SetFocus()

		if self.itemNamesListBox.IsOpened():
			self.itemNamesListBox.CloseListBox()

		self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = 0
		self.filter_config[privateShop.FILTER_TYPE_OWNER_NAME] = ""

	def GetSearchMode(self):
		return self.search_mode

	def SelectFilterPage(self, page):
		self.selectedFilterPage = page

		for key, elem_dict in self.filterItemDict.items():
			for ui_elem in elem_dict.values():
				if key == page:
					ui_elem.Show()
				else:
					ui_elem.Hide()

		if page == "PAGE_1":
			self.generalPageFilterButton.Down()
			self.attrPageFilterButton.SetUp()

		elif page == "PAGE_2":
			self.generalPageFilterButton.SetUp()
			self.attrPageFilterButton.Down()

		self.SetFocus()

	def __HideCategory(self):
		for category in self.categoryDict.values():
			category.Close()

		self.categoryScrollBar.Hide()

	def __ShowCategory(self):
		self.__RefreshCategory()

	def __RefreshCategory(self):
		self.__HideCategory()

		self.activeCategoryList = []
		POS_Y = MAIN_CATEGORY_Y

		self.filter_config[privateShop.FILTER_TYPE_ITEM_TYPE] = -1
		self.filter_config[privateShop.FILTER_TYPE_ITEM_SUBTYPE] = -1

		# Show main categories if none is selected
		if self.selectedCategory < 0:
			for id, category in sorted(self.categoryDict.items()):
				self.activeCategoryList.append(category)

		# Show main categories and sub-categories from selected one
		else:
			for id, category in sorted(self.categoryDict.items()):
				if id != self.selectedCategory:
					self.activeCategoryList.append(category)
				else:
					self.activeCategoryList.append(category)
					self.activeCategoryList.extend(category.GetSubCategoryList())

		for category in self.activeCategoryList:
			if category.GetID() == self.selectedCategory or category.GetID() == self.selectedSubCategory:
				category.SetActive(True)
				if isinstance(category, MainCategory):
					self.filter_config[privateShop.FILTER_TYPE_ITEM_TYPE] = category.GetType()
				else:
					self.filter_config[privateShop.FILTER_TYPE_ITEM_SUBTYPE] = category.GetSubType()
			else:
				category.SetActive(False)

			POS_X = MAIN_CATEGORY_X
			if category.GetID() not in self.categoryDict.keys():
				POS_X = SUB_CATEGORY_X

			category.SetOriginalPosition(POS_X, POS_Y)
			category.Show()

			POS_Y +=  18 + 2

		itemCount = len(self.activeCategoryList)
		span = (18 + 2) * itemCount - 2
		if span > self.categoryWindow.GetHeight():
			if self.filter_mode == self.FILTER_CATEGORY:
				self.categoryScrollBar.SetSpan(span - self.categoryWindow.GetHeight())
				self.categoryScrollBar.SetMiddleBarSize(7.0 / itemCount)
				self.categoryScrollBar.SetPos(self.categoryScrollBar.GetPos()) # Used to corregate scrollbar's position when it expands
				self.categoryScrollBar.SetScrollStep(1.0 / itemCount)
				self.categoryScrollBar.Show()
		else:
			self.categoryScrollBar.SetPos(0.0)
			self.categoryScrollBar.Hide()

		# Update categories pos based on scroll position
		self.OnCategoryScroll()

	def OnClickMainCategory(self, id):
		resetState = self.selectedCategory == id

		if not resetState and self.search_mode == self.ITEM_SEARCH_MODE:
			if self.nameInput.GetText():
				self.nameInput.SetText("")
				self.nameInput.SetBackgroundText("")
				self.nameInput.SetFocus()

				self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = 0

		for cat_id, category in sorted(self.categoryDict.items()):
			if cat_id == self.selectedCategory and not resetState:
				category.SetActive(True)
			else:
				category.SetActive(False)

		if resetState:
			self.selectedCategory = -1
			self.selectedSubCategory = -1
		else:
			self.selectedCategory = id

		self.__RefreshCategory()

	def OnClickSubCategory(self, id):
		self.selectedSubCategory = id

		if self.search_mode == self.ITEM_SEARCH_MODE:
			if self.nameInput.GetText():
				self.nameInput.SetText("")
				self.nameInput.SetBackgroundText("")
				self.nameInput.SetFocus()

				self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = 0

		self.__RefreshCategory()

	def OnCategoryScroll(self):
		pos = self.categoryScrollBar.GetPos() * self.categoryScrollBar.GetSpan()
		if not self.categoryScrollBar.IsShow():
			pos = 0.0

		for category in self.activeCategoryList:
			# Move categories to the middle if scroll bar is hidden
			x = 0
			if not self.categoryScrollBar.IsShow():
				x += 8

			category.AdjustPosition(x, -pos)

	def OnRunMouseWheel(self, nLen):
		if self.categoryWindow.IsInWindowRect() and self.filter_mode == self.FILTER_CATEGORY:
			if self.categoryScrollBar.IsShow():
				if nLen > 0:
					self.categoryScrollBar.OnUp()
					return True

				elif nLen < 0:
					self.categoryScrollBar.OnDown()
					return True

		elif self.itemResultWindowMask.IsInWindowRect():
			if self.itemResultScrollBar.IsShow():
				if nLen > 0:
					self.itemResultScrollBar.OnUp()
					return True

				elif nLen < 0:
					self.itemResultScrollBar.OnDown()
					return True

		return False

	def ChangeFilterMode(self):
		if self.filter_mode == self.FILTER_CATEGORY:
			self.fitlerWindow.Show()
			self.categoryWindow.Hide()
			self.filter_mode = self.FILTER_DETAILS

		elif self.filter_mode == self.FILTER_DETAILS:
			self.fitlerWindow.Hide()
			self.categoryWindow.Show()
			self.filter_mode = self.FILTER_CATEGORY

	def SetMainCategoryWindow(self):
		self.fitlerWindow.Hide()
		self.categoryWindow.Show()
		self.filter_mode = self.FILTER_CATEGORY

	def OnChangeClass(self, value):
		self.filter_config[privateShop.FILTER_TYPE_CLASS] = value - 1

		if value:
			title = self.JOB_NAME_DICT[value]
			self.filterItemDict["PAGE_1"]["classSelectSlot"].SetTitle(title)
		else:
			self.filterItemDict["PAGE_1"]["classSelectSlot"].UseDefaultTitle()

	def OnChangeMinStack(self, value):
		self.filter_config[privateShop.FILTER_TYPE_MIN_STACK] = value

	def OnChangeMaxStack(self, value):
		self.filter_config[privateShop.FILTER_TYPE_MAX_STACK] = value

	def OnChangeMinRefinement(self, value):
		self.filter_config[privateShop.FILTER_TYPE_MIN_REFINEMENT] = value

	def OnChangeMaxRefinement(self, value):
		self.filter_config[privateShop.FILTER_TYPE_MAX_REFINEMENT] = value

	def OnChangeMinLevel(self, value):
		self.filter_config[privateShop.FILTER_TYPE_MIN_LEVEL] = value

	def OnChangeMaxLevel(self, value):
		self.filter_config[privateShop.FILTER_TYPE_MAX_LEVEL] = value

	def OnChangeAttrType(self, attr, type):
		if not type:
			self.filter_config[privateShop.FILTER_TYPE_ATTR_1 + attr - 1] = [0, 0]

			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter%d" % (attr)].Clear()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter%d" % (attr)].UseDefaultTitle()

			self.SetFocus()
			return

		self.filter_config[privateShop.FILTER_TYPE_ATTR_1 + attr - 1][0] = type
		title = self.AFFECT_DICT[type]
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter%d" % (attr)].SetTitle(title)

	def OnChangeAttrValue(self, attr, value):
		self.filter_config[privateShop.FILTER_TYPE_ATTR_1 + attr - 1][1] = value

	def SetItemToolTip(self, tooltip):
		self.toolTipItem = tooltip

	def BindInterfaceClass(self, interface):
		self.interface = interface

	if app.WJ_ENABLE_TRADABLE_ICON:
		def GetLookingGlassPos(self):
			for i in xrange(player.INVENTORY_PAGE_SIZE * 4):
				if player.GetItemIndex(i) == 60005:
					return i

	def Open(self, mode):
		self.mode = mode

		if app.WJ_ENABLE_TRADABLE_ICON:
			self.lookingGlassPos = self.GetLookingGlassPos()

		self.ClearFilter()
		self.__RefreshCategory()
		self.nameInput.SetFocus()

		# if app.WJ_ENABLE_TRADABLE_ICON:
		# 	self.SetCantMouseEventSlot(self.lookingGlassPos)

		self.SetTop()
		self.Show()

	def Close(self):
		if self.filterSelectDialog.IsShow():
			self.filterSelectDialog.Close()
			return

		self.currentPageNumber		= 1
		self.bigPageCount			= 1
		self.mode					= privateShop.MODE_NONE
		self.filter_mode			= self.FILTER_CATEGORY
		self.lastSearchClickTime	= -1
		self.lastBuyClickTime		= -1
		self.selectedItemSet		= set()
		self.selectedCategory		= -1
		self.selectedSubCategory	= -1

		# if app.WJ_ENABLE_TRADABLE_ICON:
		# 	self.lockedItem = (-1,-1)
		# 	self.SetCanMouseEventSlot(self.lookingGlassPos)

		for category in self.categoryDict.values():
			category.Close()

		self.SetMainCategoryWindow()
		self.SetItemSearchMode()
		self.nameInput.SetText("")
		self.nameInput.SetBackgroundText("")


		# Reset focus from possible focused editlines
		self.KillFocus()

		privateShop.ClearSearchResult()

		self.HidePageButton()
		self.buyButton.Hide()

		if self.questionDialog:
			self.questionDialog.Close()
			self.questionDialog = None

		privateShop.DeletePrivateShopSearchPos()
		net.SendClosePrivateShopSearchPacket()

		for itemSlot in self.itemSlotResultDict.values():
			itemSlot.Hide()

		self.itemResultScrollBar.Hide()
		self.clearSelectedItemButton.Hide()

		if app.ENABLE_PRIVATE_SHOP_NO_RESULT_UPDATE:
			self.noResultsImage.Hide()
			self.noResultsText.Hide()

		self.Hide()

	if app.WJ_ENABLE_TRADABLE_ICON:
		# def SetCanMouseEventSlot(self, slotIndex):
		# 	itemInvenPage = slotIndex / player.INVENTORY_PAGE_SIZE
		# 	localSlotPos = slotIndex - (itemInvenPage * player.INVENTORY_PAGE_SIZE)
		# 	self.lockedItem = (-1,-1)

		# 	if itemInvenPage == self.wndInventory.GetInventoryPageIndex():
		# 		self.wndInventory.wndItem.SetCanMouseEventSlot(localSlotPos)

		# def SetCantMouseEventSlot(self, slotIndex):
		# 	itemInvenPage = slotIndex / player.INVENTORY_PAGE_SIZE
		# 	localSlotPos = slotIndex - (itemInvenPage * player.INVENTORY_PAGE_SIZE)
		# 	self.lockedItem = (itemInvenPage, localSlotPos)

		# 	if itemInvenPage == self.wndInventory.GetInventoryPageIndex():
		# 		self.wndInventory.wndItem.SetCantMouseEventSlot(localSlotPos)

		def SetInven(self, wndInventory):
			from _weakref import proxy
			self.wndInventory = proxy(wndInventory)

		# def RefreshLockedSlot(self):
		# 	if self.wndInventory:
		# 		itemInvenPage, itemSlotPos = self.lockedItem
		# 		if self.wndInventory.GetInventoryPageIndex() == itemInvenPage:
		# 			self.wndInventory.wndItem.SetCantMouseEventSlot(itemSlotPos)

		# 		self.wndInventory.wndItem.RefreshSlot()

	def ClearFilter(self):
		self.filterItemDict["PAGE_1"]["stackLimit"].Clear()
		self.filterItemDict["PAGE_1"]["refinementLimit"].Clear()
		self.filterItemDict["PAGE_1"]["levelLimit"].Clear()

		self.filterItemDict["PAGE_1"]["classSelectSlot"].Clear()

		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].Clear()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].Clear()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].Clear()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].Clear()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].Clear()

	def Destroy(self):
		self.ClearDictionary()

		self.toolTipItem = None

		# if app.WJ_ENABLE_TRADABLE_ICON:
		# 	self.wndInventory = None
		# 	self.lockedItem = (-1,-1)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnUpdate(self):
		if (app.GetGlobalTimeStamp() - self.lastSearchClickTime) > self.CLICK_LIMIT_TIME and self.searchButton.IsDisable() == False:
			self.searchButton.Enable()
			self.searchButton.SetUp()

		if (app.GetGlobalTimeStamp() - self.lastBuyClickTime) > self.CLICK_LIMIT_TIME and self.buyButton.IsDisable() == False and self.mode == privateShop.MODE_TRADE:
			self.buyButton.Enable()
			self.buyButton.SetUp()

			self.clearSelectedItemButton.Enable()
			self.clearSelectedItemButton.SetUp()

		# Hide filter contents if selection list is oppened
		if self.classSelectSlot.IsOpened():
			self.limitsBackground.Hide()
			self.limitsTitle.Hide()
			self.stackLimit.Hide()
			self.refinementLimit.Hide()
			self.levelLimit.Hide()
		else:
			self.limitsBackground.Show()
			self.limitsTitle.Show()
			self.stackLimit.Show()
			self.refinementLimit.Show()
			self.levelLimit.Show()

		# Position filter attributes contents if selection list is oppened
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].Show()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].Show()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].Show()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].Show()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].Show()

		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].SetPosition(3, 245)
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].SetPosition(3, 263)
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].SetPosition(3, 281)
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].SetPosition(3, 299)
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].SetPosition(3, 317)

		if self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].IsOpened():
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].SetPosition(3, 245)
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].Hide()

		if self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].IsOpened():
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].SetPosition(3, 245)
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].Hide()

		if self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].IsOpened():
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].SetPosition(3, 245)
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].Hide()

		if self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].IsOpened():
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].SetPosition(3, 245)
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].Hide()

		if self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].IsOpened():
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].Hide()
			self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].SetPosition(3, 245)

	def Refresh(self):
		maxCount = privateShop.GetSearchResultMaxCount()
		page = privateShop.GetSearchResultPage()

		self.ClearSelectedItem()

		if maxCount:
			self.ShowPageButton(maxCount, page)

			# Enable/Disable Previous Page Button
			if self.bigPageCount == 1:
				self.pageButtonDict["PAGE_PREV"].Disable()
				self.pageButtonDict["PAGE_PREV"].Down()
				self.pageButtonDict["PAGE_PREV"].Hide()
				self.__OnOverOutButton()
			else:
				self.pageButtonDict["PAGE_PREV"].Enable()
				self.pageButtonDict["PAGE_PREV"].Show()

			# Enable/Disable First Page Button
			if self.bigPageCount - 1 <= 1:
				self.pageButtonDict["PAGE_FIRST_PREV"].Disable()
				self.pageButtonDict["PAGE_FIRST_PREV"].Down()
				self.pageButtonDict["PAGE_FIRST_PREV"].Hide()
				self.__OnOverOutButton()
			else:
				self.pageButtonDict["PAGE_FIRST_PREV"].Enable()
				self.pageButtonDict["PAGE_FIRST_PREV"].Show()

			# Enable/Disable Next Page Button
			if maxCount <= self.bigPageCount * self.PAGEONE_MAX_SIZE:
				self.pageButtonDict["PAGE_NEXT"].Disable()
				self.pageButtonDict["PAGE_NEXT"].Down()
				self.pageButtonDict["PAGE_NEXT"].Hide()
				self.__OnOverOutButton()
			else:
				self.pageButtonDict["PAGE_NEXT"].Enable()
				self.pageButtonDict["PAGE_NEXT"].Show()

			# Enable/Disable Last Page Button
			if maxCount <= (self.bigPageCount+1) * self.PAGEONE_MAX_SIZE:
				self.pageButtonDict["PAGE_LAST_NEXT"].Disable()
				self.pageButtonDict["PAGE_LAST_NEXT"].Down()
				self.pageButtonDict["PAGE_LAST_NEXT"].Hide()
				self.__OnOverOutButton()
			else:
				self.pageButtonDict["PAGE_LAST_NEXT"].Enable()
				self.pageButtonDict["PAGE_LAST_NEXT"].Show()

			if page and self.mode == privateShop.MODE_TRADE:
				self.buyButton.Show()

				self.clearSelectedItemButton.Show()
		else:
			self.HidePageButton()
			self.buyButton.Hide()

			self.clearSelectedItemButton.Show()

	def RefreshPage(self):
		self.ShowItemResult(self.currentPageNumber)
		self.Refresh()

	def UpdateResult(self, index, state):
		for itemSlot in self.itemSlotResultDict.values():
			if itemSlot.GetIndex() == index:
				itemSlot.SetState(state)

		# In case the index is not present on the current page
		if index in self.selectedItemSet:
			self.selectedItemSet.remove(index)

	def ShowItemResult(self, page):
		nextPage = page - 1

		y_pos = 1
		x_pos = 3
		visibleItemCount = 0

		minRange = nextPage * privateShop.RESULT_MAX_NUM
		maxRange = min(minRange + privateShop.RESULT_MAX_NUM, privateShop.GetSearchResultMaxCount())

		for i, itemSlot in self.itemSlotResultDict.items():
			resultIndex = minRange + i

			if resultIndex >= maxRange:
				itemSlot.Hide()
				continue

			if app.ENABLE_PRIVATE_SHOP_TIME_SORT:
				(item_vnum, seller_name, item_count, gold, cheque, state, socket) = privateShop.GetSearchResult(resultIndex)
			else:
				(item_vnum, seller_name, item_count, gold, cheque, state) = privateShop.GetSearchResult(resultIndex)

			item.SelectItem(item_vnum)
			(width, height) = item.GetItemSize()

			itemSlot.SetOriginalPosition(x_pos, y_pos)
			itemSlot.AdjustPosition()
			itemSlot.Initialize(height)
			itemSlot.SetIndex(resultIndex)
			itemSlot.SetItem(item.GetIconImageFileName())

			itemSlot.SetItemName(item.GetItemName())
			itemSlot.SetSellerName(seller_name)
			if app.ENABLE_PRIVATE_SHOP_TIME_SORT:
				itemSlot.SetItemCount(item_vnum, item_count, socket)
			else:
				itemSlot.SetItemCount(item_count)
			itemSlot.SetGoldPrice(gold)
			itemSlot.SetChequePrice(cheque)
			itemSlot.SetState(state)

			itemSlot.Show()

			if itemSlot.GetIndex() in self.selectedItemSet:
				itemSlot.Select()
			else:
				itemSlot.Unselect()

			y_pos += itemSlot.GetHeight() + 3
			visibleItemCount += 1

			if app.ENABLE_PRIVATE_SHOP_NO_RESULT_UPDATE:
				if resultIndex >= 0:
					self.noResultsImage.Hide()
					self.noResultsText.Hide()

		if y_pos > self.itemResultWindowMask.GetHeight():
			self.itemResultScrollBar.SetSpan(y_pos - self.itemResultWindowMask.GetHeight() - 3)
			self.itemResultScrollBar.SetScrollStep(1.0 / visibleItemCount)
			self.itemResultScrollBar.SetMiddleBarSize(5.0 / max(6, visibleItemCount))
			self.itemResultScrollBar.SetPos(0.0)
			self.itemResultScrollBar.Show()
		else:
			self.itemResultScrollBar.Hide()

	def __OnNameInputUpdate(self):
		inputText =  self.nameInput.GetText()

		if self.search_mode == self.ITEM_SEARCH_MODE:
			if self.selectedCategory >= 0:
				self.categoryScrollBar.SetPos(0.0)
				self.OnClickMainCategory(self.selectedCategory)

			if len(inputText) >= self.SUGGESTION_MINIMAL_CHAR_REQ:
				itemSuggestionList = item.GetItemListByName(inputText)

				# Get rid of previous suggestions
				if self.itemNamesListBox.GetItemCount():
					self.itemNamesListBox.ClearItem()

				if len(itemSuggestionList) <= 0:
					self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = 0
					self.nameInput.SetBackgroundText("")
					return

				# Build up new suggestions list
				for i in range(len(itemSuggestionList)):
					(vnum, itemName) = itemSuggestionList[i]

					# Set background text for the first suggested item
					if i == 0:
						self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = vnum
						self.nameInput.SetTipText(itemName)

					self.itemNamesListBox.InsertItem(vnum, itemName)

				self.itemNamesListBox.AdjustListBox()
				self.itemNamesListBox.OpenListBox()

				self.ClearFilter()
			else:
				self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = 0
				self.nameInput.SetBackgroundText("")
				self.itemNamesListBox.CloseListBox()

		elif self.search_mode == self.PLAYER_SEARCH_MODE:
			self.filter_config[privateShop.FILTER_TYPE_OWNER_NAME] = inputText

	def __SetNameInputResult(self):
		if self.search_mode == self.ITEM_SEARCH_MODE:
			inputText = self.nameInput.GetText()
			suggestionText = self.nameInput.GetBackgroundText()
			(vnum, itemName) = item.GetItemByName(inputText + suggestionText)

			self.nameInput.SetBackgroundText("")
			self.nameInput.SetText(itemName)
			self.nameInput.SetEndPosition()

			self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = vnum

			# Clear suggestion list
			self.itemNamesListBox.CloseListBox()

	def SelectItem(self, itemVnum):
		if itemVnum <= 0:
			return

		item.SelectItem(itemVnum)
		itemName = item.GetItemName()

		self.nameInput.SetText(itemName)
		self.nameInput.SetBackgroundText("")
		self.nameInput.SetEndPosition()

		# Clear suggestion list
		self.itemNamesListBox.CloseListBox()

		self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = itemVnum

	def Search(self):
		tradeItemCount = player.GetItemCountByVnum(60005)

		if tradeItemCount <= 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATESHOPSEARCH_NEED_ITEM_FIND)
			self.Close()
			return

		if self.search_mode == self.ITEM_SEARCH_MODE:
			if self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] <= 0 and self.filter_config[privateShop.FILTER_TYPE_ITEM_TYPE] < 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATE_SHOP_SEARCH_VNUM_ITEM_NOT_SELECTED)
				return

		elif self.search_mode == self.PLAYER_SEARCH_MODE:
			if self.filter_config[privateShop.FILTER_TYPE_OWNER_NAME] == "":
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATE_SHOP_SEARCH_PLAYER_NAME_NOT_SELECTED)
				return

		self.lastSearchClickTime = app.GetGlobalTimeStamp()

		privateShop.ClearSearchResult()
		privateShop.SetSearchResultPage(0)

		self.searchButton.Disable()
		self.searchButton.Down()
		self.__OnOverOutButton()

		self.buyButton.Hide()
		self.clearSelectedItemButton.Hide()
		self.HidePageButton()

		# Reset buttons
		self.bigPageCount = 1
		self.currentPageNumber = 1
		for i in range(1, self.PAGE_NUMBER_SIZE + 1):
			self.pageButtonDict["PAGE_%d" % i].SetText(str(i))

		for itemSlot in self.itemSlotResultDict.values():
			itemSlot.Hide()

		self.itemResultScrollBar.Hide()
		self.ClearSelectedItem()

		if app.ENABLE_PRIVATE_SHOP_NO_RESULT_UPDATE:
			self.noResultsImage.Show()
			self.noResultsText.Show()

		net.SendPrivateShopSearchPacket(self.filter_config)

	def Buy(self):
		tradeItemCount = player.GetItemCountByVnum(60005)

		if tradeItemCount <= 0 and self.mode == privateShop.MODE_TRADE:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATESHOPSEARCH_NEED_ITEM_BUY)
			self.Close()
			return

		# Revision variables
		text = localeInfo.PRIVATESHOPSEARCH_BUYTIME
		q_arglen = constInfo.CHECK_TEXT_LENGHT(text)

		questionDialog = uiCommon.QuestionDialog() # Revised
		questionDialog.SetText(text)
		questionDialog.SetWidth(constInfo.CALC_Q_ARGLEN(q_arglen))
		questionDialog.SetAcceptEvent(self.OnBuyAcceptEvent)
		questionDialog.SetCancelEvent(self.OnBuyCloseEvent)
		questionDialog.Open()

		self.questionDialog = questionDialog

	def OnBuyAcceptEvent(self):
		if len(self.selectedItemSet) == 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATE_SHOP_BUY_VNUM_ITEM_NOT_SELECTED)
			return

		self.lastBuyClickTime = app.GetGlobalTimeStamp()
		self.buyButton.Disable()
		self.buyButton.Down()

		self.clearSelectedItemButton.Disable()
		self.clearSelectedItemButton.Down()

		net.SendPrivateShopSearchBuyPacket(list(self.selectedItemSet))

		self.questionDialog.Close()
		self.questionDialog = None

	def OnBuyCloseEvent(self):
		self.questionDialog.Close()
		self.questionDialog = None

	def ClearSelectedItem(self):
		for itemSlot in self.itemSlotResultDict.values():
			if itemSlot.GetIndex() in self.selectedItemSet:
				itemSlot.OnSelect()

		self.selectedItemSet.clear()

	def OnItemResultScroll(self):
		pos = self.itemResultScrollBar.GetPos() * self.itemResultScrollBar.GetSpan()
		for itemSlot in self.itemSlotResultDict.values():
			itemSlot.AdjustPosition(y = -pos)

	def ClearPageButtonColor(self):
		for i in range(1, self.PAGE_NUMBER_SIZE + 1):
			self.pageButtonDict["PAGE_%d" % i].SetTextColor(0xffffffff)
			self.pageButtonDict["PAGE_%d" % i].SetUp()
			self.pageButtonDict["PAGE_%d" % i].Enable()

	def HidePageButton(self):
		for button in self.pageButtonDict.values():
			button.Hide()

	def ShowPageButton(self, maxSize, page):
		if self.bigPageCount > 1:
			maxSize = maxSize - ((self.bigPageCount-1) * self.PAGEONE_MAX_SIZE)
			page = page - (self.bigPageCount-1) * 5

		self.pageCount = maxSize / privateShop.RESULT_MAX_NUM

		if not maxSize % privateShop.RESULT_MAX_NUM == 0:
			self.pageCount = self.pageCount + 1

		if self.pageCount > 5:
			self.pageCount = 5

		for i in range(1, self.pageCount + 1):
			self.pageButtonDict["PAGE_%d" % i].Show()

		self.pageButtonDict["PAGE_FIRST_PREV"].Show()
		self.pageButtonDict["PAGE_PREV"].Show()
		self.pageButtonDict["PAGE_NEXT"].Show()
		self.pageButtonDict["PAGE_LAST_NEXT"].Show()

		self.ClearPageButtonColor()

		self.pageButtonDict["PAGE_%d" % page].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pageButtonDict["PAGE_%d" % page].Down()
		self.pageButtonDict["PAGE_%d" % page].Disable()

	def SelectPage(self, page):
		if page == self.currentPageNumber:
			return

		if self.bigPageCount > 1:
			if page == self.currentPageNumber - (self.bigPageCount-1) * 5:
				return

		self.ClearPageButtonColor()

		self.pageButtonDict["PAGE_%d" % page].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pageButtonDict["PAGE_%d" % page].Down()
		self.pageButtonDict["PAGE_%d" % page].Disable()

		if self.bigPageCount > 1:
			page = page + (self.bigPageCount-1) * 5

		self.ShowItemResult(page)
		self.currentPageNumber = page

		privateShop.SetSearchResultPage(self.currentPageNumber)
		self.Refresh()

	def PrevPage(self):
		if self.bigPageCount <= 1:
			return

		self.ClearPageButtonColor()
		self.bigPageCount -= 1

		for i in range(1, self.PAGE_NUMBER_SIZE + 1):
			pageNumber = int(self.pageButtonDict["PAGE_%d" % i].GetText()) - self.PAGE_NUMBER_SIZE
			self.pageButtonDict["PAGE_%d" % i].SetText(str(pageNumber))

		newPageNumber = int(self.pageButtonDict["PAGE_1"].GetText())

		self.ShowItemResult(newPageNumber)
		self.currentPageNumber = newPageNumber

		self.pageButtonDict["PAGE_1"].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pageButtonDict["PAGE_1"].Down()
		self.pageButtonDict["PAGE_1"].Disable()

		privateShop.SetSearchResultPage(self.currentPageNumber)
		self.Refresh()

	def NextPage(self):
		maxItemCount = privateShop.GetSearchResultMaxCount()
		if maxItemCount < self.bigPageCount * self.PAGEONE_MAX_SIZE:
			return

		for i in range(1, self.PAGE_NUMBER_SIZE + 1):
			pageNumber = int(self.pageButtonDict["PAGE_%d" % i].GetText()) + self.PAGE_NUMBER_SIZE
			self.pageButtonDict["PAGE_%d" % i].SetText(str(pageNumber))

		newPageNumber = int(self.pageButtonDict["PAGE_1"].GetText())

		self.ShowItemResult(newPageNumber)
		self.currentPageNumber = newPageNumber

		self.bigPageCount += 1

		self.HidePageButton()
		self.ClearPageButtonColor()

		self.pageButtonDict["PAGE_1"].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pageButtonDict["PAGE_1"].Down()
		self.pageButtonDict["PAGE_1"].Disable()

		privateShop.SetSearchResultPage(self.currentPageNumber)
		self.Refresh()

	def FirstPrevPage(self):
		if self.bigPageCount - 1 <= 1:
			return

		self.ClearPageButtonColor()

		self.bigPageCount = 1
		for i in range(1, self.PAGE_NUMBER_SIZE + 1):
			self.pageButtonDict["PAGE_%d" % i].SetText(str(i))

		newPageNumber = int(self.pageButtonDict["PAGE_1"].GetText())

		self.ShowItemResult(newPageNumber)
		self.currentPageNumber = newPageNumber

		self.pageButtonDict["PAGE_1"].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pageButtonDict["PAGE_1"].Down()
		self.pageButtonDict["PAGE_1"].Disable()

		privateShop.SetSearchResultPage(self.currentPageNumber)
		self.Refresh()

	def LastNextPage(self):
		maxSize = privateShop.GetSearchResultMaxCount()
		self.pageCount = int(math.ceil(float(maxSize) / privateShop.RESULT_MAX_NUM))

		self.HidePageButton()
		self.ClearPageButtonColor()

		if self.pageCount % self.PAGE_NUMBER_SIZE == 0:
			self.bigPageCount = (self.pageCount / self.PAGE_NUMBER_SIZE)
		else:
			self.bigPageCount = (self.pageCount / self.PAGE_NUMBER_SIZE) + 1

		pageNumber = self.PAGE_NUMBER_SIZE * (self.pageCount / self.PAGE_NUMBER_SIZE)
		if pageNumber == self.pageCount:
			pageNumber -= self.PAGE_NUMBER_SIZE

		for i in range(1, self.PAGE_NUMBER_SIZE + 1):
			self.pageButtonDict["PAGE_%d" % i].SetText(str(i + pageNumber))

		self.ShowItemResult(self.pageCount)
		self.currentPageNumber = self.pageCount

		lastPageNumber = self.currentPageNumber - (self.bigPageCount - 1) * self.PAGE_NUMBER_SIZE
		self.pageButtonDict["PAGE_%d" % lastPageNumber].SetTextColor(self.SPECIAL_TITLE_COLOR)
		self.pageButtonDict["PAGE_%d" % lastPageNumber].Down()
		self.pageButtonDict["PAGE_%d" % lastPageNumber].Disable()

		privateShop.SetSearchResultPage(self.currentPageNumber)
		self.Refresh()

	def GetAffectString(self, affect):
		try:
			return self.AFFECT_DICT[affect]
		except:
			return uiScriptLocale.PRIVATESHOPSEARCH_SELECT

	def OnSelectItem(self, slotIndex, selected):
		if selected and len(self.selectedItemSet) >= privateShop.SELECTED_ITEM_MAX_NUM:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.PRIVATE_SHOP_BUY_ITEM_SELECTED_OVERFLOW)
			return False

		if selected:
			if app.ENABLE_PRIVATE_SHOP_TIME_SORT:
				(item_vnum, seller_name, item_count, gold, cheque, state, socket) = privateShop.GetSearchResult(slotIndex)
			else:
				(item_vnum, seller_name, item_count, gold, cheque, state) = privateShop.GetSearchResult(slotIndex)

			privateShop.CreatePrivateShopSearchPos(seller_name)

			self.selectedItemSet.add(slotIndex)
		else:
			# Do not remove search position when unselecting an item from a private shop
			# from which other items are still selected
			if app.ENABLE_PRIVATE_SHOP_TIME_SORT:
				(item_vnum, seller_name, item_count, gold, cheque, state, socket) = privateShop.GetSearchResult(slotIndex)
			else:
				(item_vnum, seller_name, item_count, gold, cheque, state) = privateShop.GetSearchResult(slotIndex)

			remove = True
			for selectedSlotIndex in self.selectedItemSet:
				if selectedSlotIndex != slotIndex:

					if app.ENABLE_PRIVATE_SHOP_TIME_SORT:
						(item_vnum2, seller_name2, item_count2, gold2, cheque2, state2, socket2) = privateShop.GetSearchResult(selectedSlotIndex)
					else:
						(item_vnum2, seller_name2, item_count2, gold2, cheque2, state2) = privateShop.GetSearchResult(selectedSlotIndex)

					if seller_name == seller_name2:
						remove = False
						break

			if remove:
				privateShop.DeletePrivateShopSearchPos(seller_name)

			self.selectedItemSet.remove(slotIndex)

		return True

	def OverInItem(self, slotIndex):
		if self.toolTipItem:
			self.toolTipItem.ClearToolTip()
			self.toolTipItem.SetPrivateShopSearchItem(slotIndex)
			self.toolTipItem.ShowToolTip()

	def OverOutItem(self):
		if self.toolTipItem:
			self.toolTipItem.HideToolTip()

	def OnContactSeller(self, sellerName):
		self.interface.OpenWhisperDialog(sellerName)

	# if app.ENABLE_PRIVATE_SHOP_SEARCH_FROM_REFINE or app.ENABLE_PRIVATE_SHOP_WHISPER_DETAILS:
	def LookingGlassPacket(self):
		if self.lookingGlassPos == 0:
			net.SendItemUsePacket(self.GetLookingGlassPos())
		else:
			net.SendItemUsePacket(self.lookingGlassPos)

	# if app.ENABLE_PRIVATE_SHOP_SEARCH_FROM_REFINE:
	def OnSearchItem(self, vnum):

		if self.searchButton.IsDisable() == False:
			return

		self.ClearFilter()
		self.SetItemSearchMode()
		self.SelectItem(vnum)
		self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = vnum
		self.SetFocus()
		self.Search()

	def OnSearchSeller(self, sellerName):
		if self.searchButton.IsDisable() == False:
			return

		self.ClearFilter()
		self.SetPlayerSearchMode()
		self.nameInput.SetText(sellerName)
		self.filter_config[privateShop.FILTER_TYPE_OWNER_NAME] = sellerName
		self.SetFocus()
		self.Search()

	def OnToggleFilterDialog(self):
		if self.filterSelectDialog.IsShow():
			self.filterSelectDialog.Close()

			self.KillFocus()
			self.filterSelectDialog.SetFocus()
		else:
			self.filterSelectDialog.Open()

			self.SetFocus()

	def OnSaveFilter(self):
		if self.questionDialog:
			return

		questionDialog = uiCommon.InputDialog()
		questionDialog.SetTitle(uiScriptLocale.PRIVATESHOPSEARCH_FILTER_SAVE_TITLE)
		questionDialog.SetAcceptEvent(self.OnFilterSaveAcceptEvent)
		questionDialog.SetCancelEvent(self.OnFilterSaveCloseEvent)
		questionDialog.SetMaxLength(20)
		questionDialog.Open()

		self.questionDialog = questionDialog

	def OnFilterSaveAcceptEvent(self):
		print(self.filter_config)
		self.filterSelectDialog.AppendFilter(self.filter_config, self.questionDialog.GetText())
		self.OnFilterSaveCloseEvent()

	def OnFilterSaveCloseEvent(self):
		self.questionDialog.Close()
		self.questionDialog = None

	def OnApplyFilter(self, filter_config):
		self.ClearFilter()

		if self.selectedCategory:
			self.OnClickMainCategory(self.selectedCategory)

		if filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] != 0:
			self.SetItemSearchMode()

			item.SelectItem(filter_config[privateShop.FILTER_TYPE_ITEM_VNUM])
			self.nameInput.SetText(item.GetItemName())
			self.nameInput.SetEndPosition()
			self.filter_config[privateShop.FILTER_TYPE_ITEM_VNUM] = filter_config[privateShop.FILTER_TYPE_ITEM_VNUM]

		elif filter_config[privateShop.FILTER_TYPE_OWNER_NAME] != "":
			self.SetPlayerSearchMode()

			self.nameInput.SetText(filter_config[privateShop.FILTER_TYPE_OWNER_NAME])
			self.nameInput.SetEndPosition()
			self.filter_config[privateShop.FILTER_TYPE_OWNER_NAME] = filter_config[privateShop.FILTER_TYPE_OWNER_NAME]

		self.filterItemDict["PAGE_1"]["stackLimit"].SetMin(str(filter_config[privateShop.FILTER_TYPE_MIN_STACK]))
		self.filterItemDict["PAGE_1"]["stackLimit"].SetMax(str(filter_config[privateShop.FILTER_TYPE_MAX_STACK]))
		self.filterItemDict["PAGE_1"]["refinementLimit"].SetMin(str(filter_config[privateShop.FILTER_TYPE_MIN_REFINEMENT]))
		self.filterItemDict["PAGE_1"]["refinementLimit"].SetMax(str(filter_config[privateShop.FILTER_TYPE_MAX_REFINEMENT]))
		self.filterItemDict["PAGE_1"]["levelLimit"].SetMin(str(filter_config[privateShop.FILTER_TYPE_MIN_LEVEL]))
		self.filterItemDict["PAGE_1"]["levelLimit"].SetMax(str(filter_config[privateShop.FILTER_TYPE_MAX_LEVEL]))
		self.filterItemDict["PAGE_1"]["classSelectSlot"].SelectItem(filter_config[privateShop.FILTER_TYPE_CLASS])

		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].SetItemValue(filter_config[privateShop.FILTER_TYPE_ATTR_1][0], str(filter_config[privateShop.FILTER_TYPE_ATTR_1][1]))
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].SetItemValue(filter_config[privateShop.FILTER_TYPE_ATTR_2][0], str(filter_config[privateShop.FILTER_TYPE_ATTR_2][1]))
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].SetItemValue(filter_config[privateShop.FILTER_TYPE_ATTR_3][0], str(filter_config[privateShop.FILTER_TYPE_ATTR_3][1]))
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].SetItemValue(filter_config[privateShop.FILTER_TYPE_ATTR_4][0], str(filter_config[privateShop.FILTER_TYPE_ATTR_4][1]))
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].SetItemValue(filter_config[privateShop.FILTER_TYPE_ATTR_5][0], str(filter_config[privateShop.FILTER_TYPE_ATTR_5][1]))

	def __OnOverInButton(self, button):
		self.toolTip.ClearToolTip()

		text = ""
		if button == "FILTER":
			if self.filter_mode == self.FILTER_CATEGORY:
				text = uiScriptLocale.PRIVATESHOPSEARCH_VIEW_FILTER
			else:
				text = uiScriptLocale.PRIVATESHOPSEARCH_VIEW_CATEGORY

		elif button == "FILTER_SELECT":
			if self.filterSelectDialog.IsShow():
				text = uiScriptLocale.PRIVATESHOPSEARCH_HIDE_FILTER_SELECT_DIALOG
			else:
				text = uiScriptLocale.PRIVATESHOPSEARCH_VIEW_FILTER_SELECT_DIALOG

		elif button == "SAVE_FILTER":
			text = uiScriptLocale.PRIVATESHOPSEARCH_SAVE_FILTER

		elif button == "SEARCH":
			text = uiScriptLocale.PRIVATESHOPSEARCH_SEARCH

		elif button == "BUY":
			if len(self.selectedItemSet) > 0:
				text = uiScriptLocale.PRIVATESHOPSEARCH_BUY_SELECTED_ITEMS % (len(self.selectedItemSet), privateShop.SELECTED_ITEM_MAX_NUM)
			else:
				text = uiScriptLocale.PRIVATESHOPSEARCH_BUY

		elif button == "CLEAR_SELECTED_ITEMS":
			if len(self.selectedItemSet) > 0:
				text = uiScriptLocale.PRIVATESHOPSEARCH_CLEAR_SELECTED_ITEMS % (len(self.selectedItemSet), privateShop.SELECTED_ITEM_MAX_NUM)
			else:
				text = uiScriptLocale.PRIVATESHOPSEARCH_CLEAR_SELECTED

		elif button == "ITEM_MODE":
			text = uiScriptLocale.PRIVATESHOPSEARCH_ITEM_SEARCH_MODE

		elif button == "PLAYER_MODE":
			text = uiScriptLocale.PRIVATESHOPSEARCH_PLAYER_SEARCH_MODE

		elif button == "GENERAL_FILTER":
			text = uiScriptLocale.PRIVATESHOPSEARCH_GENERAL_FILTER_CONFIG

		elif button == "ATTR_FILTER":
			text = uiScriptLocale.PRIVATESHOPSEARCH_ATTR_FILTER_CONFIG

		elif button == "CLEAR_FILTER":
			text = uiScriptLocale.PRIVATESHOPSEARCH_CLEAR_FILTER_CONFIG

		elif button == "PAGE_FIRST_PREV":
			text = uiScriptLocale.PRIVATESHOPSEARCH_PAGE_FIRST_PREV

		elif button == "PAGE_PREV":
			text = uiScriptLocale.PRIVATESHOPSEARCH_PAGE_PREV

		elif button == "PAGE_NEXT":
			text = uiScriptLocale.PRIVATESHOPSEARCH_PAGE_NEXT

		elif button == "PAGE_LAST_NEXT":
			text = uiScriptLocale.PRIVATESHOPSEARCH_PAGE_LAST_NEXT

		t_arglen = constInfo.CHECK_TEXT_LENGHT(text)

		self.toolTip.SetThinBoardSize(constInfo.CALC_T_ARGLEN(t_arglen))
		self.toolTip.AppendTextLine(text, constInfo.EXTRA_INFO_COLOR)
		self.toolTip.ShowToolTip() # Revised

	def __OnOverOutButton(self):
		if 0 != self.toolTip:
			self.toolTip.HideToolTip()

	def KillFocus(self):
		self.filterItemDict["PAGE_1"]["stackLimit"].KillFocus()
		self.filterItemDict["PAGE_1"]["refinementLimit"].KillFocus()
		self.filterItemDict["PAGE_1"]["levelLimit"].KillFocus()

		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter1"].KillFocus()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter2"].KillFocus()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter3"].KillFocus()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter4"].KillFocus()
		self.filterItemDict["PAGE_1"]["attributeTypeValueFilter5"].KillFocus()

		self.nameInput.KillFocus()