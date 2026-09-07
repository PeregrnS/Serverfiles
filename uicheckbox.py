import app
import net
import player
import item
import ui
import uiToolTip
import mouseModule
import localeInfo
import uiScriptLocale
import systemSetting
import uiCommon
import constInfo
import wndMgr
import chat
import grp

class MouseReflector(ui.Window):
	def __init__(self, parent):
		ui.Window.__init__(self)
		self.SetParent(parent)
		self.AddFlag("not_pick")
		self.width = self.height = 0
		self.isDown = False

	def __del__(self):
		ui.Window.__del__(self)

	def Down(self):
		self.isDown = True

	def Up(self):
		self.isDown = False

	def OnRender(self):
		if self.isDown:
			grp.SetColor(ui.WHITE_COLOR)
		else:
			grp.SetColor(ui.HALF_WHITE_COLOR)

		x, y = self.GetGlobalPosition()
		grp.RenderBar(x+2, y+2, self.GetWidth()-4, self.GetHeight()-4)

class CheckBox(ui.ImageBox):
	def __init__(self, parent, x, y, small=False):
		ui.ImageBox.__init__(self)
		self.SetParent(parent)

		# Text background
		x -= 5
		self.x = x
		self.y = y
		self.small = small

		self.SetPosition(x, y)
		if self.small == True:
			self.LoadImage("d:/ymir work/ui/custom/checkbox_unselected_small.tga")
		else:
			self.LoadImage("d:/ymir work/ui/custom/checkbox_unselected.tga")

		# Checkbox
		if self.small == True:
			image = ui.MakeImageBox(self, "d:/ymir work/ui/custom/checkbox_selected_small.tga", 0, 0)
		else:
			image = ui.MakeImageBox(self, "d:/ymir work/ui/custom/checkbox_selected.tga", 0, 0)
		image.AddFlag("not_pick")
		image.Hide()
		self.image = image

		# Text background
		if self.small == True:
			text_bg = ui.MakeHorizontalBarCheckboxSmall(self, 23, 0, 0, "")
		else:
			text_bg = ui.MakeHorizontalBarCheckbox(self, 30, 0, 0, "")
		text_bg.Show()
		self.text_bg = text_bg

		# Update
		text_desc = ui.TextLine()
		text_desc.SetParent(self)
		if self.small == True:
			text_desc.SetPosition(23, 3)
		else:
			text_desc.SetPosition(31, 3)
		text_desc.Show()
		self.text_desc = text_desc

		# Mouse reflector
		self.mouseReflector = MouseReflector(self)
		self.mouseReflector.SetSize(self.GetWidth(), self.GetHeight())
		self.mouseReflector.UpdateRect()

		# if constInfo.ENABLE_EXTENDED_TOOLTIP:
		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.HideToolTip()

		self.Show()

		# if constInfo.ENABLE_EXTENDED_TOOLTIP:
		self.toolTipText	= ""

		self.enable			= True
		self.selected		= False
		self.event			= None

	def __del__(self):
		ui.ImageBox.__del__(self)

	# Update
	def SetWindowHorizontalAlignCenterFromPos(self):
		self.SetPosition(self.x - (self.image.GetWidth() + self.text_desc.GetTextSize()[0]) / 2, self.y)

	def CalcElementsSize(self):
		calc_width = (self.image.GetWidth() + self.text_desc.GetTextSize()[0])
		calc_height = (self.image.GetHeight())

		# Text background
		text_bg_width = 5 + self.text_desc.GetTextSize()[0] + 5
		self.text_bg.SetWidth(text_bg_width)

		# Text
		self.text_desc.SetPosition(self.image.GetWidth() + text_bg_width / 2 - self.text_desc.GetTextSize()[0] / 2 - 1, self.image.GetHeight() / 2 - self.text_desc.GetTextSize()[1] / 2 - 1)

		self.SetSize(calc_width, calc_height)

	def SetText(self, text):
		self.text_desc.SetText(text)
		self.CalcElementsSize()

	# if constInfo.ENABLE_EXTENDED_TOOLTIP:
	def SetTooltipText(self, text):
		self.toolTipText = text

	def ChangeState(self):
		if self.selected == True:
			self.image.Hide()
			self.selected = False
		else:
			self.image.Show()
			self.selected = True

		self.event()

	def IsSelectedNumber(self):
		if self.selected == True:
			return 1
		else:
			return 0

	def SetSelectFromNumber(self, number):
		if number == 1:
			self.selected = True
		else:
			self.selected = False

	def IsSelected(self):
		return self.selected

	def SetEvent(self, event):
		self.event = event

	def SetNotSelected(self):
		self.selected = False
		self.image.Hide()

	def Enable(self):
		self.enable = False

	def Disable(self):
		self.enable = False

	def OnMouseOverIn(self):
		if not self.enable:
			return

		self.mouseReflector.Show()

		# if constInfo.ENABLE_EXTENDED_TOOLTIP:
		text = self.toolTipText
		if len(text):
			self.toolTip.ClearToolTip()
			t_arglen = constInfo.CHECK_TEXT_LENGHT(text)

			self.toolTip.SetThinBoardSize(constInfo.CALC_T_ARGLEN(t_arglen))
			self.toolTip.AppendTextLine(text, constInfo.EXTRA_INFO_COLOR)
			self.toolTip.ShowToolTip() # Revised

	def OnMouseOverOut(self):
		if not self.enable:
			return

		self.mouseReflector.Hide()

		# if constInfo.ENABLE_EXTENDED_TOOLTIP:
		self.toolTip.HideToolTip()

	def OnMouseLeftButtonDown(self):
		if not self.enable:
			return

		self.mouseReflector.Down()

	def OnMouseLeftButtonUp(self):
		if not self.enable:
			return

		self.mouseReflector.Up()
		self.ChangeState()

