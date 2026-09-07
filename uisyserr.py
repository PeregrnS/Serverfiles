# -*- coding: utf-8 -*-
"""
uisyserr.py
Version: 1.0
Author: fratello_exe
"""

import ui
import os
import app
import wndMgr

if app.ENABLE_SYSERR_VIEWER:

	__SYSERR_VIEWER = None

	DEFAULT_FONT = "Tahoma:12"
	DEFAULT_WHEEL_LINES = 0.1

	def _clamp(v, a, b):
		if v < a:
			return a
		if v > b:
			return b
		return v

	def _safe_add_flag(w, flag):
		try:
			w.AddFlag(flag)
			return True
		except:
			return False

	def _safe_not_pick(w):
		_safe_add_flag(w, "not_pick")

	def _safe_float(w):
		_safe_add_flag(w, "float")

	def _safe_top(w):
		try:
			w.SetTop()
		except:
			pass

	def _now():
		try:
			return app.GetTime()
		except:
			return 0.0

	def _cursor():
		try:
			return app.GetCursorPosition()
		except:
			return (0, 0)

	def _is_left_down():
		try:
			return bool(wndMgr.IsMouseLeftButtonDown())
		except:
			pass
		try:
			return bool(getattr(wndMgr, "GetMouseLeftButtonState")())
		except:
			pass
		try:
			st = wndMgr.GetMouseState()
			return bool(int(st) & 1)
		except:
			pass
		try:
			if hasattr(app, "DIK_LBUTTON"):
				return bool(app.IsPressed(app.DIK_LBUTTON))
		except:
			pass
		return False

	def _is_right_down():
		try:
			return wndMgr.IsMouseRightButtonDown()
		except:
			return False

	def _key_down(dik):
		try:
			return app.IsPressed(dik)
		except:
			return False

	def _ctrl_down():
		try:
			return app.IsPressed(app.DIK_LCONTROL) or app.IsPressed(app.DIK_RCONTROL)
		except:
			return False

	def _shift_down():
		try:
			return app.IsPressed(app.DIK_LSHIFT) or app.IsPressed(app.DIK_RSHIFT)
		except:
			return False

	def _alt_down():
		try:
			return app.IsPressed(app.DIK_LALT) or app.IsPressed(app.DIK_RALT)
		except:
			return False

	def _screen_size():
		try:
			return (int(wndMgr.GetScreenWidth()), int(wndMgr.GetScreenHeight()))
		except:
			pass
		try:
			return (int(app.GetScreenWidth()), int(app.GetScreenHeight()))
		except:
			return (1024, 768)

	def _safe_str(s):
		if s is None:
			return ""
		try:
			if isinstance(s, unicode):
				try:
					return s.encode("utf8", "replace")
				except:
					return str(s)
		except:
			pass
		try:
			if not isinstance(s, str):
				return str(s)
		except:
			return ""
		return s

	def _abs_pos(w):
		try:
			return w.GetGlobalPosition()
		except:
			pass
		try:
			x, y = w.GetLocalPosition()
		except:
			return (0, 0)
		try:
			p = w.GetParent()
		except:
			p = None
		while p:
			try:
				px, py = p.GetLocalPosition()
				x += px
				y += py
			except:
				break
			try:
				p = p.GetParent()
			except:
				break
		return (x, y)

	def _global_to_local(parent, gx, gy):
		px, py = _abs_pos(parent)
		return (int(gx - px), int(gy - py))

	def _clamp_in_rect(x, y, w, h, rx, ry, rw, rh, pad=2):
		if w > rw - pad*2:
			w = rw - pad*2
		if h > rh - pad*2:
			h = rh - pad*2
		if x + w > rx + rw - pad:
			x = (rx + rw - pad) - w
		if y + h > ry + rh - pad:
			y = (ry + rh - pad) - h
		if x < rx + pad:
			x = rx + pad
		if y < ry + pad:
			y = ry + pad
		return (int(x), int(y), int(w), int(h))

	def _safe_set_font(w, font_name):
		try:
			w.SetFontName(font_name)
			return True
		except:
			return False

	class _TickerHub(object):
		def __init__(self):
			self._tick = []
		def Add(self, o):
			if not o:
				return
			if o in self._tick:
				return
			self._tick.append(o)
		def Remove(self, o):
			try:
				self._tick.remove(o)
			except:
				pass
		def Tick(self, now):
			for o in tuple(self._tick):
				try:
					o.Tick(now)
				except:
					pass

	class _PxCache(object):
		def __init__(self, measure_line, max_items):
			self._m = measure_line
			self._max = max_items
			self._cache = {}
		def Reset(self):
			self._cache = {}
		def Get(self, s):
			s = _safe_str(s)
			w = self._cache.get(s)
			if w is not None:
				return w
			try:
				self._m.SetText(s)
				try:
					w = int(self._m.GetTextSize()[0])
				except:
					w = len(s) * 6
			except:
				w = len(s) * 6
			if len(self._cache) >= self._max:
				self._cache = {}
			self._cache[s] = w
			return w

	class _Theme(object):
		def __init__(self):
			self._mode = 0
			self._dark = {
				"bg":0xcc060606,
				"frame_o":0xff141414,
				"frame_i":0xff0f0f0f,
				"title_g1":0xff1b1b1b,
				"title_g2":0xff121212,
				"title_line":0xff2d7bd6,
				"panel":0xff0b0b0b,
				"panel2":0xff101010,
				"panel3":0xff0d0d0d,
				"text":0xfff0f0f0,
				"muted":0x90d0d0d0,
				"hint":0x809a9a9a,
				"accent":0xff2d7bd6,
				"accent2":0xffa14cff,
				"btn_up":0xff191919,
				"btn_over":0xff262626,
				"btn_down":0xff0f0f0f,
				"btn_dis":0xff121212,
				"btn_txt":0xfff0f0f0,
				"btn_txt_dis":0x80707070,
				"btn_line":0xff2d7bd6,
				"btn_line_off":0xff191919,
				"sel":0x55325a9a,
				"match":0x332d7bd6,
				"match_cur":0x66325a9a,
				"err":0xffff4a4a,
				"info":0xffe6e6e6,
				"menu_bg":0xee0b0b0b,
				"menu_in":0xff141414,
				"scroll_track":0xff0f0f0f,
				"scroll_track2":0xff090909,
				"scroll_thumb":0xff2a2a2a,
				"scroll_thumb_over":0xff3a3a3a,
				"scroll_thumb_down":0xff151515,
				"scroll_line":0xff2d7bd6,
				"toast":0xee101010,
				"toast_in":0xff1a1a1a,
			}
			self._light = {
				"bg":0xccf2f2f2,
				"frame_o":0xffd7d7d7,
				"frame_i":0xfff7f7f7,
				"title_g1":0xfff9f9f9,
				"title_g2":0xffededed,
				"title_line":0xff2d7bd6,
				"panel":0xfffbfbfb,
				"panel2":0xfff3f3f3,
				"panel3":0xffffffff,
				"text":0xff101010,
				"muted":0x90161616,
				"hint":0x80606060,
				"accent":0xff2d7bd6,
				"accent2":0xffa14cff,
				"btn_up":0xfff2f2f2,
				"btn_over":0xffe6eefc,
				"btn_down":0xffe9e9e9,
				"btn_dis":0xfff1f1f1,
				"btn_txt":0xff101010,
				"btn_txt_dis":0x80505050,
				"btn_line":0xff2d7bd6,
				"btn_line_off":0xffd0d0d0,
				"sel":0x332d7bd6,
				"match":0x262d7bd6,
				"match_cur":0x402d7bd6,
				"err":0xffd90000,
				"info":0xff1a1a1a,
				"menu_bg":0xeef7f7f7,
				"menu_in":0xfffdfdfd,
				"scroll_track":0xffe7e7e7,
				"scroll_track2":0xfff1f1f1,
				"scroll_thumb":0xffcfcfcf,
				"scroll_thumb_over":0xffb8c7e6,
				"scroll_thumb_down":0xffaebfe6,
				"scroll_line":0xff2d7bd6,
				"toast":0xeef7f7f7,
				"toast_in":0xfffdfdfd,
			}
		def Toggle(self):
			self._mode = 0 if self._mode else 1
		def Get(self):
			return self._light if self._mode else self._dark

	class _VisualBar(ui.Bar):
		def __init__(self, parent):
			ui.Bar.__init__(self)
			self.SetParent(parent)
			self.SetPosition(0, 0)
			self.Show()

	class _TextLine(ui.TextLine):
		def __init__(self, parent, txt=""):
			ui.TextLine.__init__(self)
			self.SetParent(parent)
			_safe_set_font(self, DEFAULT_FONT)
			try:
				self.SetText(_safe_str(txt))
			except:
				pass
			self.Show()

	class _HitArea(ui.Window):
		def __init__(self, parent, down_cb=None, up_cb=None, dbl_cb=None, right_cb=None, wheel_cb=None):
			ui.Window.__init__(self)
			self.SetParent(parent)
			self._down = down_cb
			self._up = up_cb
			self._dbl = dbl_cb
			self._right = right_cb
			self._wheel = wheel_cb
			self._last = 0.0
			self.Show()
		def OnMouseLeftButtonDown(self):
			try:
				if self._down:
					self._down()
			except:
				pass
			try:
				now = _now()
				if (now - self._last) <= 0.26:
					if self._dbl:
						self._dbl()
				self._last = now
			except:
				pass
			return True
		def OnMouseLeftButtonUp(self):
			try:
				if self._up:
					self._up()
			except:
				pass
			return True
		def OnMouseRightButtonDown(self):
			try:
				if self._right:
					self._right()
			except:
				pass
			return True
		def OnMouseWheel(self, nLen):
			try:
				if self._wheel:
					return self._wheel(nLen)
			except:
				pass
			return False

	class _FlatButton(ui.Window):
		def __init__(self, parent, theme_getter, label="", cb=None, align_left=False):
			ui.Window.__init__(self)
			self.SetParent(parent)
			self._tg = theme_getter
			self._cb = cb
			self._args = None
			self._hover = False
			self._down = False
			self._disabled = False
			self._align_left = True if align_left else False

			self.bg = _VisualBar(self)
			self.line = _VisualBar(self)
			_safe_not_pick(self.bg)
			_safe_not_pick(self.line)

			self.txt = _TextLine(self, label)
			if self._align_left:
				self.txt.SetHorizontalAlignLeft()
				self.txt.SetVerticalAlignCenter()
			else:
				self.txt.SetHorizontalAlignCenter()
				self.txt.SetVerticalAlignCenter()
			_safe_not_pick(self.txt)

			self.Show()
			self._Apply(True)

		def SetEvent(self, cb, *args):
			self._cb = cb
			self._args = args if args else None

		def SetText(self, t):
			try:
				self.txt.SetText(_safe_str(t))
			except:
				pass

		def SetDisabled(self, v):
			self._disabled = True if v else False
			self._hover = False
			self._down = False
			self._Apply(True)

		def SetSize(self, w, h):
			ui.Window.SetSize(self, w, h)
			self.bg.SetSize(w, h)
			self.line.SetPosition(0, h-2)
			self.line.SetSize(w, 2)
			if self._align_left:
				self.txt.SetPosition(8, h/2)
			else:
				self.txt.SetPosition(w/2, h/2)
			self._Apply(True)

		def _Fire(self):
			try:
				if not self._cb:
					return
				if self._args:
					self._cb(*self._args)
				else:
					self._cb()
			except:
				pass

		def OnMouseOverIn(self):
			if self._disabled:
				return
			self._hover = True
			self._Apply(False)

		def OnMouseOverOut(self):
			if self._disabled:
				return
			self._hover = False
			self._down = False
			self._Apply(False)

		def OnMouseLeftButtonDown(self):
			if self._disabled:
				return True
			self._down = True
			self._Apply(False)
			return True

		def OnMouseLeftButtonUp(self):
			if self._disabled:
				return True
			was = self._down
			self._down = False
			self._Apply(False)
			if was and self._hover:
				self._Fire()
			return True

		def _Apply(self, force):
			p = self._tg()
			if self._disabled:
				self.bg.SetColor(p["btn_dis"])
				self.txt.SetPackedFontColor(p["btn_txt_dis"])
				try:
					self.line.SetColor(p.get("btn_line_off", p["btn_dis"]))
				except:
					pass
				return
			if self._down:
				self.bg.SetColor(p["btn_down"])
			elif self._hover:
				self.bg.SetColor(p["btn_over"])
			else:
				self.bg.SetColor(p["btn_up"])
			self.txt.SetPackedFontColor(p["btn_txt"])
			try:
				if self._down or self._hover:
					self.line.SetColor(p.get("btn_line", p["accent"]))
				else:
					self.line.SetColor(p.get("btn_line_off", p["btn_up"]))
			except:
				pass

	class _ToggleButton(_FlatButton):
		def __init__(self, parent, theme_getter, label="", cb=None):
			self._toggled = False
			_FlatButton.__init__(self, parent, theme_getter, label, cb)
		def SetToggle(self, v):
			self._toggled = True if v else False
			self._Apply(True)
		def IsToggled(self):
			return self._toggled
		def OnMouseLeftButtonUp(self):
			if self._disabled:
				return True
			was = self._down
			self._down = False
			self._Apply(False)
			if was and self._hover:
				self._toggled = not self._toggled
				self._Apply(True)
				self._Fire()
			return True
		def _Apply(self, force):
			p = self._tg()
			if self._disabled:
				self.bg.SetColor(p["btn_dis"])
				self.txt.SetPackedFontColor(p["btn_txt_dis"])
				try:
					self.line.SetColor(p.get("btn_line_off", p["btn_dis"]))
				except:
					pass
				return
			if self._down:
				self.bg.SetColor(p["btn_down"])
			elif self._hover or self._toggled:
				self.bg.SetColor(p["btn_over"])
			else:
				self.bg.SetColor(p["btn_up"])
			self.txt.SetPackedFontColor(p["btn_txt"])
			try:
				if self._toggled or self._down or self._hover:
					self.line.SetColor(p.get("btn_line", p["accent"]))
				else:
					self.line.SetColor(p.get("btn_line_off", p["btn_up"]))
			except:
				pass

	class _Toast(ui.Window):
		def __init__(self, parent, theme_getter):
			ui.Window.__init__(self)
			self.SetParent(parent)
			_safe_float(self)
			_safe_not_pick(self)
			self._tg = theme_getter
			self._end = 0.0
			self.bg = _VisualBar(self)
			self.inn = _VisualBar(self)
			self.inn.SetPosition(1, 1)
			self.txt = _TextLine(self, "")
			self.txt.SetPosition(10, 2)
			_safe_not_pick(self.bg)
			_safe_not_pick(self.inn)
			_safe_not_pick(self.txt)
			self.Hide()
		def ShowMsg(self, s, sec):
			try:
				p = self._tg()
				self.bg.SetColor(p["toast"])
				self.inn.SetColor(p["toast_in"])
				self.txt.SetPackedFontColor(p["text"])
				self.txt.SetText(_safe_str(s))
				self._end = _now() + float(sec)
				self.Show()
				_safe_top(self)
			except:
				pass
		def Tick(self, now):
			try:
				if self.IsShow() and now >= self._end:
					self.Hide()
			except:
				pass

	class _Menu(ui.Window):
		def __init__(self, parent, theme_getter, hub, px):
			ui.Window.__init__(self)
			self.SetParent(parent)
			_safe_float(self)
			self._tg = theme_getter
			self._hub = hub
			self._hub.Add(self)
			self._px = px
			self.PAD = 7
			self.BH = 20
			self.BW = 170
			self.items = []
			self.bg = _VisualBar(self)
			self.inn = _VisualBar(self)
			self.inn.SetPosition(1, 1)
			_safe_not_pick(self.bg)
			_safe_not_pick(self.inn)
			self._want_close = False
			self._rect = None
			self.Hide()

		def Destroy(self):
			try:
				self._hub.Remove(self)
			except:
				pass

		def Clear(self):
			for b in self.items:
				try:
					b.Hide()
				except:
					pass
			self.items = []

		def Add(self, txt, cb, *args):
			def _wrap():
				try:
					if args:
						cb(*args)
					else:
						cb()
				except:
					pass
				try:
					self.Close()
				except:
					pass
			b = _FlatButton(self, self._tg, txt, _wrap, True)
			self.items.append(b)

		def _CalcWidth(self):
			try:
				maxw = 0
				for b in self.items:
					try:
						t = b.txt.GetText()
					except:
						t = ""
					w = self._px.Get(_safe_str(t)) + 26
					if w > maxw:
						maxw = w
				return _clamp(maxw, 150, 320)
			except:
				return self.BW

		def SetClampRect(self, rx, ry, rw, rh):
			self._rect = (int(rx), int(ry), int(rw), int(rh))

		def OpenAt(self, x, y):
			p = self._tg()
			self.bg.SetColor(p["menu_bg"])
			self.inn.SetColor(p["menu_in"])
			cnt = len(self.items)
			if cnt <= 0:
				return

			self.BW = int(self._CalcWidth())
			w = self.BW + self.PAD*2
			h = self.PAD*2 + cnt*self.BH + (cnt-1)*4

			if self._rect:
				rx, ry, rw, rh = self._rect
				x, y, w, h = _clamp_in_rect(x, y, w, h, rx, ry, rw, rh, 2)
			else:
				sw, sh = _screen_size()
				x, y, w, h = _clamp_in_rect(x, y, w, h, 0, 0, sw, sh, 2)

			self.SetSize(w, h)
			self.bg.SetSize(w, h)
			self.inn.SetSize(max(0, w-2), max(0, h-2))

			yy = self.PAD
			for b in self.items:
				b.SetPosition(self.PAD, yy)
				b.SetSize(self.BW, self.BH)
				yy += self.BH + 4

			self.SetPosition(int(x), int(y))
			self.Show()
			_safe_top(self)

		def Close(self):
			self.Hide()

		def IsOpen(self):
			try:
				return self.IsShow()
			except:
				return False

		def Tick(self, now):
			if not self.IsOpen():
				return
			if _is_left_down() or _is_right_down():
				cx, cy = _cursor()
				ax, ay = _abs_pos(self)
				w = self.GetWidth()
				h = self.GetHeight()
				if not (ax <= cx < ax+w and ay <= cy < ay+h):
					self._want_close = True
			else:
				if self._want_close:
					self._want_close = False
					self.Close()

	class _ScrollBar(ui.Window):
		def __init__(self, parent, theme_getter, hub):
			ui.Window.__init__(self)
			self.SetParent(parent)
			self._theme = theme_getter
			self._hub = hub
			self._hub.Add(self)

			self._on_scroll = None
			self._pos = 0.0
			self._drag = False
			self._btnDown = False
			self._drag_off_y = 0
			self._total = 1
			self._page = 1

			self.track = _VisualBar(self)
			self.track2 = _VisualBar(self)
			self.track2.SetPosition(1, 1)

			self.thumb = _VisualBar(self)
			self.thumbLine = _VisualBar(self)

			_safe_not_pick(self.track)
			_safe_not_pick(self.track2)
			_safe_not_pick(self.thumb)
			_safe_not_pick(self.thumbLine)

			self.hit = _HitArea(self, ui.__mem_func__(self._OnDown), ui.__mem_func__(self._OnUp), None, None, None)
			self.hit.SetPosition(0, 0)

			self.Show()
			self._Apply()
			self._Layout()

		def Destroy(self):
			try:
				self._hub.Remove(self)
			except:
				pass

		def SetSize(self, w, h):
			ui.Window.SetSize(self, w, h)
			try:
				self.hit.SetSize(w, h)
			except:
				pass
			self._Layout()

		def SetScrollEvent(self, cb):
			self._on_scroll = cb

		def SetRange(self, total, page):
			try:
				total = int(total)
				page = int(page)
			except:
				total = 1
				page = 1
			if total < 1:
				total = 1
			if page < 1:
				page = 1
			self._total = total
			self._page = page
			self._pos = _clamp(self._pos, 0.0, 1.0)
			self._Layout()

		def GetPos(self):
			return self._pos

		def SetPos(self, v):
			try:
				v = float(v)
			except:
				v = 0.0
			v = _clamp(v, 0.0, 1.0)
			if abs(self._pos - v) < 0.00001:
				return
			self._pos = v
			self._Layout()
			self._Emit()

		def AddLines(self, total_minus_page, delta_lines):
			try:
				total_minus_page = int(total_minus_page)
				delta_lines = int(delta_lines)
			except:
				return
			if total_minus_page <= 0:
				self.SetPos(0.0)
				return
			start = int(float(total_minus_page) * self._pos) + delta_lines
			if start < 0:
				start = 0
			if start > total_minus_page:
				start = total_minus_page
			self.SetPos(float(start) / float(total_minus_page))

		def _ThumbRect(self):
			w = self.GetWidth()
			h = self.GetHeight()
			track_h = max(4, h - 2)
			tx = 1
			tw = max(6, w - 2)

			total = float(self._total if self._total > 0 else 1)
			page = float(self._page if self._page > 0 else 1)

			ratio = _clamp(page / total, 0.03, 1.0)
			th = int(track_h * ratio)
			if th < 18:
				th = 18
			if th > track_h:
				th = track_h

			max_y = track_h - th
			off = int(max_y * self._pos) if max_y > 0 else 0
			ty = 1 + off
			return (tx, ty, tw, th, track_h)

		def _SetFromLocalY(self, local_y):
			tx, ty, tw, th, track_h = self._ThumbRect()
			max_y = track_h - th
			if max_y <= 0:
				self._pos = 0.0
				self._Layout()
				self._Emit()
				return
			yy = _clamp(int(local_y), 0, int(max_y))
			self._pos = float(yy) / float(max_y)
			self._Layout()
			self._Emit()

		def _Emit(self):
			try:
				if self._on_scroll:
					self._on_scroll()
			except:
				pass

		def _Apply(self):
			p = self._theme()
			self.track.SetColor(p["scroll_track"])
			self.track2.SetColor(p["scroll_track2"])
			if self._drag:
				self.thumb.SetColor(p["scroll_thumb_down"])
			else:
				self.thumb.SetColor(p["scroll_thumb"])
			self.thumbLine.SetColor(p["scroll_line"])

		def _Layout(self):
			w = self.GetWidth()
			h = self.GetHeight()

			self.track.SetPosition(0, 0)
			self.track.SetSize(w, h)
			self.track2.SetSize(max(0, w - 2), max(0, h - 2))

			tx, ty, tw, th, _ = self._ThumbRect()
			self.thumb.SetPosition(tx, ty)
			self.thumb.SetSize(tw, th)

			self.thumbLine.SetPosition(tx + 1, ty + th - 2)
			self.thumbLine.SetSize(max(0, tw - 2), 1)

			self._Apply()

		def _Capture(self, on):
			try:
				if on and hasattr(wndMgr, "SetMouseCaptureWindow"):
					wndMgr.SetMouseCaptureWindow(self.hit)
					return
			except:
				pass
			try:
				if (not on) and hasattr(wndMgr, "ReleaseMouseCaptureWindow"):
					wndMgr.ReleaseMouseCaptureWindow()
					return
			except:
				pass

		def _OnDown(self):
			cx, cy = _cursor()
			ax, ay = _abs_pos(self)
			local_y = int(cy - ay)

			tx, ty, tw, th, track_h = self._ThumbRect()
			self._btnDown = True
			self._drag = True

			if ty <= local_y < (ty + th):
				self._drag_off_y = local_y - ty
			else:
				self._drag_off_y = int(th / 2)
				self._SetFromLocalY(local_y - self._drag_off_y)

			self._Capture(True)
			self._Apply()
			return True

		def _OnUp(self):
			self._btnDown = False
			self._drag = False
			self._Capture(False)
			self._Apply()
			return True

		def Tick(self, now):
			if not self.IsShow() or not self._drag:
				return
			if not self._btnDown:
				self._drag = False
				self._Capture(False)
				self._Apply()
				return
			cx, cy = _cursor()
			ax, ay = _abs_pos(self)
			self._SetFromLocalY(int(cy - ay) - self._drag_off_y)

	class _RowHit(ui.Window):
		def __init__(self, parent, row, owner, down_cb, up_cb, right_cb, dbl_cb, hover_cb):
			ui.Window.__init__(self)
			self.SetParent(parent)
			self.row = row
			self.owner = owner
			self.down_cb = down_cb
			self.up_cb = up_cb
			self.right_cb = right_cb
			self.dbl_cb = dbl_cb
			self.hover_cb = hover_cb
			self._last = 0.0
			self.Show()
		def SetRow(self, r):
			self.row = r
		def _LocalCursor(self):
			cx, cy = _cursor()
			ax, ay = _abs_pos(self)
			return (cx - ax, cy - ay)
		def OnMouseOverIn(self):
			try:
				if self.hover_cb:
					self.hover_cb(self.row, True)
			except:
				pass
		def OnMouseOverOut(self):
			try:
				if self.hover_cb:
					self.hover_cb(self.row, False)
			except:
				pass
		def OnMouseLeftButtonDown(self):
			try:
				lx, ly = self._LocalCursor()
				self.down_cb(self.row, int(lx), int(ly))
			except:
				pass
			try:
				now = _now()
				if (now - self._last) <= 0.26:
					try:
						lx, ly = self._LocalCursor()
						self.dbl_cb(self.row, int(lx), int(ly))
					except:
						pass
				self._last = now
			except:
				pass
			return True
		def OnMouseLeftButtonUp(self):
			try:
				self.up_cb()
			except:
				pass
			return True
		def OnMouseRightButtonDown(self):
			try:
				lx, ly = self._LocalCursor()
				self.right_cb(self.row, int(lx), int(ly))
			except:
				pass
			return True

	class _LinePool(object):
		def __init__(self, parent, theme_getter):
			self._p = parent
			self._tg = theme_getter
			self._rows = 0
			self._bg = []
			self._match = []
			self._sel = []
			self._txt = []
			self._hit = []

		def Clear(self):
			for a in (self._bg, self._match, self._sel, self._txt, self._hit):
				for w in a:
					try:
						w.Hide()
					except:
						pass
			self._bg = []
			self._match = []
			self._sel = []
			self._txt = []
			self._hit = []
			self._rows = 0

		def Ensure(self, rows, owner, down_cb, up_cb, right_cb, dbl_cb, hover_cb):
			p = self._tg()
			while self._rows < rows:
				bg = ui.Bar(); bg.SetParent(self._p); bg.SetColor(0); bg.Show()
				ms = ui.Bar(); ms.SetParent(self._p); ms.SetColor(0); ms.Show()
				sl = ui.Bar(); sl.SetParent(self._p); sl.SetColor(0); sl.Show()
				tl = ui.TextLine(); tl.SetParent(self._p); _safe_set_font(tl, DEFAULT_FONT); tl.SetPackedFontColor(p["info"]); tl.Show()
				hit = _RowHit(self._p, self._rows, owner, down_cb, up_cb, right_cb, dbl_cb, hover_cb)
				self._bg.append(bg)
				self._match.append(ms)
				self._sel.append(sl)
				self._txt.append(tl)
				self._hit.append(hit)
				self._rows += 1

			while self._rows > rows:
				self._rows -= 1
				try: self._bg.pop().Hide()
				except: pass
				try: self._match.pop().Hide()
				except: pass
				try: self._sel.pop().Hide()
				except: pass
				try: self._txt.pop().Hide()
				except: pass
				try: self._hit.pop().Hide()
				except: pass

		def ApplyFont(self, font_name):
			for tl in self._txt:
				_safe_set_font(tl, font_name)

		def Place(self, idx, x, y, w, h, text_x):
			self._bg[idx].SetPosition(x, y)
			self._bg[idx].SetSize(w, h)
			self._match[idx].SetPosition(x, y)
			self._match[idx].SetSize(0, h)
			self._sel[idx].SetPosition(x, y)
			self._sel[idx].SetSize(0, h)
			self._txt[idx].SetPosition(text_x, y)
			self._hit[idx].SetPosition(x, y)
			self._hit[idx].SetSize(w, h)
			self._hit[idx].SetRow(idx)

		def SetVisible(self, idx, v):
			if v:
				self._bg[idx].Show()
				self._match[idx].Show()
				self._sel[idx].Show()
				self._txt[idx].Show()
				self._hit[idx].Show()
			else:
				self._bg[idx].Hide()
				self._match[idx].Hide()
				self._sel[idx].Hide()
				self._txt[idx].Hide()
				self._hit[idx].Hide()

		def SetRowText(self, idx, s, col):
			self._txt[idx].SetPackedFontColor(col)
			self._txt[idx].SetText(_safe_str(s))

		def SetRowBg(self, idx, col):
			self._bg[idx].SetColor(col)

		def SetRowMatch(self, idx, x, y, w, h, col):
			self._match[idx].SetColor(col)
			self._match[idx].SetPosition(x, y)
			self._match[idx].SetSize(w, h)

		def ClearRowMatch(self, idx):
			self._match[idx].SetColor(0)
			self._match[idx].SetSize(0, self._match[idx].GetHeight())

		def SetRowSel(self, idx, x, y, w, h, col):
			self._sel[idx].SetColor(col)
			self._sel[idx].SetPosition(x, y)
			self._sel[idx].SetSize(w, h)

		def ClearRowSel(self, idx):
			self._sel[idx].SetColor(0)
			self._sel[idx].SetSize(0, self._sel[idx].GetHeight())

	class _MiniGoto(ui.Window):
		def __init__(self, parent, theme_getter, on_go):
			ui.Window.__init__(self)
			self.SetParent(parent)
			_safe_float(self)
			self._tg = theme_getter
			self._on_go = on_go

			self.bg = _VisualBar(self)
			self.inn = _VisualBar(self)
			self.inn.SetPosition(1, 1)
			_safe_not_pick(self.bg)
			_safe_not_pick(self.inn)

			self.title = _TextLine(self, "Go line")
			self.title.SetPosition(10, 7)
			_safe_not_pick(self.title)

			self.ed = ui.EditLine()
			self.ed.SetParent(self)
			self.ed.SetSize(110, 16)
			self.ed.SetMax(10)
			self.ed.SetText("")
			try:
				self.ed.SetFontName(DEFAULT_FONT)
			except:
				pass
			self.ed.SetEscapeEvent(ui.__mem_func__(self.Hide))
			self.ed.SetReturnEvent(ui.__mem_func__(self.Go))
			self.ed.Show()

			self.btnGo = _FlatButton(self, self._tg, "Go", ui.__mem_func__(self.Go))
			self.btnX = _FlatButton(self, self._tg, "X", ui.__mem_func__(self.Hide))

			self.Hide()

		def OpenIn(self, x, y, clamp_rect):
			p = self._tg()
			self.bg.SetColor(p["menu_bg"])
			self.inn.SetColor(p["menu_in"])
			self.title.SetPackedFontColor(p["text"])

			w, h = 238, 58
			rx, ry, rw, rh = clamp_rect
			x, y, w, h = _clamp_in_rect(x, y, w, h, rx, ry, rw, rh, 2)

			self.SetSize(w, h)
			self.bg.SetSize(w, h)
			self.inn.SetSize(max(0, w-2), max(0, h-2))
			self.SetPosition(int(x), int(y))

			self.ed.SetPosition(10, 30)
			self.btnGo.SetPosition(w-112, 30)
			self.btnGo.SetSize(62, 20)
			self.btnX.SetPosition(w-43, 30)
			self.btnX.SetSize(35, 20)

			try:
				self.ed.SetFocus()
			except:
				pass

			self.Show()
			_safe_top(self)

		def Go(self):
			try:
				s = self.ed.GetText().strip()
			except:
				s = ""
			try:
				n = int(s)
			except:
				n = 0
			if n <= 0:
				return True
			try:
				self._on_go(n)
			except:
				pass
			try:
				self.Hide()
			except:
				pass
			return True

	class _ResizeGrip(_FlatButton):
		def __init__(self, parent, theme_getter, begin_cb, end_cb):
			_FlatButton.__init__(self, parent, theme_getter, "[]", None, False)
			self._begin = begin_cb
			self._end = end_cb
		def OnMouseLeftButtonDown(self):
			try:
				if self._begin:
					self._begin()
			except:
				pass
			return True
		def OnMouseLeftButtonUp(self):
			try:
				if self._end:
					self._end()
			except:
				pass
			return True

	class SyserrViewer(ui.ScriptWindow):

		TITLE_H = 34
		PAD = 8
		SCROLL_W = 14

		MIN_W = 520
		MIN_H = 300

		SEARCH_MAX_W = 360
		SEARCH_MIN_W = 170

		POLL_TIME = 0.20
		SEARCH_APPLY_DELAY = 0.12

		MAX_LINES = 25000
		TAIL_CHUNK = 65536
		TAIL_HARD_MAX = 8 * 1024 * 1024

		K_INFO = 0
		K_ERR = 1

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.AddFlag("movable")
			self.AddFlag("float")

			self._theme = _Theme()
			self._hub = _TickerHub()

			self._path = os.path.join(os.getcwd(), "syserr.txt")

			self._lines = []
			self._kinds = []
			self._cntErr = 0

			self._filtered = None

			self._stop = False

			self._searchKey = ""
			self._searchCur = ""
			self._searchSafe = ""
			self._searchNextApply = 0.0

			self._nextPoll = 0.0
			self._lastSig = (0, 0)
			self._filePos = 0
			self._tailBuf = ""

			self.isDragging = False
			self.isResizing = False
			self.dragStartX = self.dragStartY = 0
			self.winX = self.winY = 0
			self.resizeStartX = self.resizeStartY = 0
			self.startW = self.startH = 0

			self._prevKey = {}

			self.selectedView = set()
			self.selActive = False
			self.selAnchor = -1
			self.selLast = -1

			self.textSelActive = False
			self.textAnchorLine = self.textLastLine = -1
			self.textAnchorChar = self.textLastChar = 0

			self._matchCur = -1

			self.contentX = self.contentY = 0
			self.contentW = self.contentH = 0
			self.perPage = 0
			self.viewStart = 0

			self._pxCache = None
			self._lnDigits = 4

			self._menu = None
			self._goto = None
			self._toast = None

			self._lastViewStart = -1
			self._viewDirty = True

			self._wheelLines = DEFAULT_WHEEL_LINES
			self._wheelAcc = 0.0
			self._fontName = DEFAULT_FONT

			self.LINE_H = 14

			self._BuildUI()
			self.ReloadFull()
			self.Show()

		def Stop(self):
			self._stop = True
			self._ToastMsg("Stop", 0.6)
			return True

		def Destroy(self):
			try:
				if self._menu:
					self._menu.Destroy()
			except:
				pass
			try:
				if self.scroll:
					self.scroll.Destroy()
			except:
				pass
			self._lines = []
			self._kinds = []
			self._filtered = None
			self.selectedView = set()
			self._prevKey = {}
			try:
				self._pool.Clear()
			except:
				pass
			self.Hide()

		def _P(self):
			return self._theme.Get()

		def _PrefixText(self, rawIdx):
			try:
				return ("%%%dd  " % int(self._lnDigits)) % int(rawIdx + 1)
			except:
				return "%d  " % int(rawIdx + 1)

		def _BuildUI(self):
			self.SetSize(640, 360)
			self.SetCenterPosition()

			p = self._P()

			self.bg = _VisualBar(self)
			self.frameO = _VisualBar(self)
			self.frameI = _VisualBar(self)

			self.inner = ui.Window()
			self.inner.SetParent(self)
			self.inner.SetPosition(0, 0)
			self.inner.Show()

			self.bg.SetColor(p["bg"])
			self.frameO.SetColor(p["frame_o"])
			self.frameI.SetColor(p["frame_i"])
			_safe_not_pick(self.bg)
			_safe_not_pick(self.frameO)
			_safe_not_pick(self.frameI)

			self.titleG1 = _VisualBar(self.inner)
			self.titleG2 = _VisualBar(self.inner)
			self.titleLine = _VisualBar(self.inner)

			self.titleTxt = _TextLine(self.inner, "Syserr Viewer")
			self.titleTxt.SetPosition(16, 9)
			_safe_not_pick(self.titleTxt)

			self.titleG1.SetColor(p["title_g1"])
			self.titleG2.SetColor(p["title_g2"])
			self.titleLine.SetColor(p["title_line"])
			_safe_not_pick(self.titleG1)
			_safe_not_pick(self.titleG2)
			_safe_not_pick(self.titleLine)

			self.titleHit = _HitArea(self.inner, ui.__mem_func__(self._TitleDown), ui.__mem_func__(self._TitleUp))
			self.titleHit.Show()

			self.btnClose = _FlatButton(self.inner, ui.__mem_func__(self._P), "X", ui.__mem_func__(self.Close))
			self.btnMenu = _ToggleButton(self.inner, ui.__mem_func__(self._P), "Menu", ui.__mem_func__(self.ToggleMenu))
			self.btnClear = _FlatButton(self.inner, ui.__mem_func__(self._P), "Clear", ui.__mem_func__(self.ClearSyserr))

			self.search = ui.EditLine()
			self.search.SetParent(self.inner)
			self.search.SetSize(self.SEARCH_MAX_W, 15)
			self.search.SetMax(64)
			self.search.SetText("")
			try:
				self.search.SetFontName(DEFAULT_FONT)
			except:
				pass
			self.search.SetEscapeEvent(ui.__mem_func__(self._OnSearchEscape))
			self.search.SetReturnEvent(ui.__mem_func__(self._OnSearchEnter))
			self.search.Show()

			self.searchHint = _TextLine(self.inner, "Search")
			self.searchHint.SetPosition(0, 0)
			_safe_not_pick(self.searchHint)

			self.panel = _VisualBar(self.inner)
			self.panel2 = _VisualBar(self.inner)
			self.panel3 = _VisualBar(self.inner)

			self.measure = ui.TextLine()
			self.measure.SetParent(self.inner)
			self.measure.SetPosition(0, 0)
			self.measure.Hide()
			_safe_set_font(self.measure, self._fontName)

			self._pxCache = _PxCache(self.measure, 7000)

			self._pool = _LinePool(self.inner, ui.__mem_func__(self._P))

			self.scroll = _ScrollBar(self.inner, ui.__mem_func__(self._P), self._hub)
			self.scroll.SetScrollEvent(ui.__mem_func__(self.OnScroll))

			self.listHit = _HitArea(self.inner, None, None, None, ui.__mem_func__(self._OnPanelRight), ui.__mem_func__(self.OnMouseWheel))
			self.listHit.Show()

			self._toast = _Toast(self.inner, ui.__mem_func__(self._P))
			self._hub.Add(self._toast)

			self._menu = _Menu(self.inner, ui.__mem_func__(self._P), self._hub, self._pxCache)
			self._BuildMenu()

			self._goto = _MiniGoto(self.inner, ui.__mem_func__(self._P), ui.__mem_func__(self._GoLine))

			self.resizeGrip = _ResizeGrip(self.inner, ui.__mem_func__(self._P), ui.__mem_func__(self.StartResize), ui.__mem_func__(self.EndResize))
			self.resizeGrip.Show()

			self._ApplyTheme()
			self._RebuildLayout(True)

		def _UpdateSearchHint(self):
			try:
				if not self.search.IsShow():
					self.searchHint.Hide()
					return

				txt = _safe_str(self.search.GetText()).strip()
				if txt:
					self.searchHint.Hide()
				else:
					self.searchHint.Show()
			except:
				pass

		def _BuildMenu(self):
			self._menu.Clear()
			self._menu.Add("Copy", ui.__mem_func__(self.CopySelection))
			self._menu.Add("Refresh", ui.__mem_func__(self.ReloadFull))
			self._menu.Add("Stop", ui.__mem_func__(self.Stop))
			self._menu.Add("Go", ui.__mem_func__(self.ToggleGoto))
			self._menu.Add("Theme", ui.__mem_func__(self.ToggleTheme))

		def _ApplyTheme(self):
			p = self._P()
			try:
				self.titleTxt.SetPackedFontColor(p["text"])
				self.searchHint.SetPackedFontColor(p["hint"])
			except:
				pass

			self.bg.SetColor(p["bg"])
			self.frameO.SetColor(p["frame_o"])
			self.frameI.SetColor(p["frame_i"])

			self.titleG1.SetColor(p["title_g1"])
			self.titleG2.SetColor(p["title_g2"])
			self.titleLine.SetColor(p["title_line"])

			self.panel.SetColor(p["panel"])
			self.panel2.SetColor(p["panel2"])
			self.panel3.SetColor(p["panel3"])

			try:
				self.btnMenu._Apply(True)
				self.btnClose._Apply(True)
				self.btnClear._Apply(True)
				self.resizeGrip._Apply(True)
			except:
				pass

			try:
				_safe_set_font(self.measure, self._fontName)
			except:
				pass

			try:
				self._pool.ApplyFont(self._fontName)
			except:
				pass

			self._viewDirty = True
			self.UpdateView()

		def _RebuildLayout(self, force_pool=False):
			w = max(self.GetWidth(), self.MIN_W)
			h = max(self.GetHeight(), self.MIN_H)
			self.SetSize(w, h)

			self.bg.SetPosition(0, 0)
			self.bg.SetSize(w, h)

			self.frameO.SetPosition(6, 6)
			self.frameO.SetSize(w-12, h-12)

			self.frameI.SetPosition(7, 7)
			self.frameI.SetSize(w-14, h-14)

			self.inner.SetPosition(7, 7)
			self.inner.SetSize(w-14, h-14)

			iw = w - 14
			ih = h - 14

			self.titleG1.SetPosition(0, 0)
			self.titleG1.SetSize(iw, self.TITLE_H/2 + (self.TITLE_H % 2))

			self.titleG2.SetPosition(0, self.TITLE_H/2 + (self.TITLE_H % 2))
			self.titleG2.SetSize(iw, self.TITLE_H - (self.TITLE_H/2 + (self.TITLE_H % 2)))

			self.titleLine.SetPosition(0, self.TITLE_H-1)
			self.titleLine.SetSize(iw, 1)

			self.titleHit.SetPosition(0, 0)
			self.titleHit.SetSize(iw, self.TITLE_H)

			self.titleTxt.SetPosition(16, 9)

			self.btnClose.SetSize(28, 18)
			self.btnClose.SetPosition(iw-34, 7)

			right = iw - 38
			btns = [
				(self.btnMenu, 60),
				(self.btnClear, 46),
			]
			for b, bw in btns:
				right -= (bw + 6)
				b.SetPosition(right, 7)
				b.SetSize(bw, 18)
				b.Show()

			leftLimit = 150
			avail = max(0, (right - 10) - leftLimit)
			searchW = min(self.SEARCH_MAX_W, max(self.SEARCH_MIN_W, avail))

			if searchW < 90:
				self.search.Hide()
				self.searchHint.Hide()
			else:
				self.search.Show()
				self.searchHint.Show()
				self.search.SetSize(searchW, 15)
				self.search.SetPosition(leftLimit, 9)
				self.searchHint.SetPosition(leftLimit+6, 9)

			self.contentX = self.PAD
			self.contentY = self.TITLE_H + self.PAD
			self.contentW = iw - self.PAD*2 - self.SCROLL_W - 6

			bottomReserve = 24
			self.contentH = ih - self.TITLE_H - self.PAD*3 - bottomReserve

			if self.contentW < 190:
				self.contentW = 190
			if self.contentH < 150:
				self.contentH = 150

			self.panel.SetPosition(self.contentX-2, self.contentY-2)
			self.panel.SetSize(self.contentW+4, self.contentH+4)

			self.panel2.SetPosition(self.contentX-1, self.contentY-1)
			self.panel2.SetSize(self.contentW+2, self.contentH+2)

			self.panel3.SetPosition(self.contentX, self.contentY)
			self.panel3.SetSize(self.contentW, self.contentH)

			self.scroll.SetPosition(iw - self.PAD - self.SCROLL_W, self.contentY-1)
			self.scroll.SetSize(self.SCROLL_W, self.contentH+2)

			self.listHit.SetPosition(self.contentX, self.contentY)
			self.listHit.SetSize(self.contentW, self.contentH)

			self.resizeGrip.SetSize(28, 18)
			self.resizeGrip.SetPosition(iw - 34, ih - 26)

			try:
				tw = 50
				th = 20
				self._toast.SetSize(tw, th)
				self._toast.bg.SetSize(tw, th)
				self._toast.inn.SetSize(tw-2, th-2)
				self._toast.SetPosition((iw - tw) / 2, ih - 26)
			except:
				pass

			newPer = int(self.contentH / float(self.LINE_H))
			newPer = max(10, min(newPer, 320))

			needPool = force_pool or (newPer != self.perPage) or (self.perPage <= 0)

			self.perPage = newPer

			if needPool:
				self._pool.Ensure(self.perPage, self, ui.__mem_func__(self._OnLineDown), ui.__mem_func__(self._OnLineUp), ui.__mem_func__(self._OnLineRight), ui.__mem_func__(self._OnLineDbl), ui.__mem_func__(self._OnLineHover))
				self._pool.ApplyFont(self._fontName)

			for i in xrange(self.perPage):
				y = self.contentY + i*self.LINE_H
				wl = self.contentW - 2
				self._pool.Place(i, self.contentX, y, wl, self.LINE_H, self.contentX+6)

			self._viewDirty = True
			self._UpdateSearchHint()
			self.UpdateView()

		def _TitleDown(self):
			if self.isResizing or self.selActive or self.textSelActive:
				return
			cx, cy = _cursor()
			wx, wy = self.GetLocalPosition()
			iw = self.inner.GetWidth()
			if cx >= (wx + iw - 220):
				return
			self.isDragging = True
			self.dragStartX, self.dragStartY = cx, cy
			self.winX, self.winY = wx, wy

		def _TitleUp(self):
			self.isDragging = False

		def _ToastMsg(self, s, sec=0.65):
			try:
				self._toast.ShowMsg(_safe_str(s), sec)
			except:
				pass

		def _KeyEdge(self, dik):
			down = _key_down(dik)
			prev = self._prevKey.get(dik, False)
			self._prevKey[dik] = down
			return down and not prev

		def _KindOf(self, s):
			try:
				l = s.lower()
			except:
				return self.K_INFO
			if "traceback" in l or "syserr:" in l or "exception" in l:
				return self.K_ERR
			if "error" in l or "failed" in l or "assert" in l:
				return self.K_ERR
			return self.K_INFO

		def _ColorOfKind(self, k):
			p = self._P()
			if k == self.K_ERR:
				return p["err"]
			return p["info"]

		def _ResetSel(self, lineSel=True, textSel=True):
			if lineSel:
				self.selectedView = set()
				self.selActive = False
				self.selAnchor = -1
				self.selLast = -1
			if textSel:
				self.textSelActive = False
				self.textAnchorLine = self.textLastLine = -1
				self.textAnchorChar = self.textLastChar = 0

		def _NormalizeTextSelection(self):
			if self.textAnchorLine == -1 or self.textLastLine == -1:
				return None
			sL, sC = self.textAnchorLine, self.textAnchorChar
			eL, eC = self.textLastLine, self.textLastChar
			if (sL > eL) or (sL == eL and sC > eC):
				sL, eL = eL, sL
				sC, eC = eC, sC
			eC += 1
			if eC < 0:
				eC = 0
			return (sL, sC, eL, eC)

		def _GetSelectedText(self):
			ns = self._NormalizeTextSelection()
			if not ns:
				return ""
			sL, sC, eL, eC = ns
			total = self._ViewTotal()
			if total <= 0:
				return ""

			if sL < 0:
				sL = 0
			if eL >= total:
				eL = total - 1

			out = []
			for v in xrange(sL, eL+1):
				r, txt, k = self._GetLineByView(v)
				if r == -1:
					continue
				txt = _safe_str(txt)
				if sL == eL:
					out.append(txt[sC:eC])
				elif v == sL:
					out.append(txt[sC:])
				elif v == eL:
					out.append(txt[:eC])
				else:
					out.append(txt)
			try:
				return "\n".join(out)
			except:
				return ""

		def _SafeClipboard(self, text):
			try:
				text = _safe_str(text)
				if not text:
					return False
				app.SetClipboardData(text)
				self._ToastMsg("Copied", 0.55)
				return True
			except:
				return False

		def _ViewTotal(self):
			if self._filtered is None:
				return len(self._lines)
			return len(self._filtered)

		def _MapViewToRaw(self, vIdx):
			if self._filtered is None:
				return vIdx
			if 0 <= vIdx < len(self._filtered):
				return self._filtered[vIdx]
			return -1

		def _GetLineByView(self, vIdx):
			r = self._MapViewToRaw(vIdx)
			if 0 <= r < len(self._lines):
				return (r, self._lines[r], self._kinds[r])
			return (-1, "", self.K_INFO)

		def _BuildFiltered(self):
			key = self._searchKey
			if not key:
				self._filtered = None
				return
			f = []
			for i in xrange(len(self._lines)):
				try:
					s = self._lines[i]
				except:
					continue
				try:
					ok = (key in s.lower())
				except:
					ok = False
				if not ok:
					continue
				f.append(i)
			self._filtered = f

		def _UpdateCounts(self):
			cntE = 0
			for k in self._kinds:
				if k == self.K_ERR:
					cntE += 1
			self._cntErr = cntE

		def _AtBottom(self):
			total = self._ViewTotal()
			per = self.perPage
			if total <= per:
				return True
			try:
				pos = float(self.scroll.GetPos())
			except:
				pos = 0.0
			return pos >= 0.995

		def _FindMatchSpan(self, sLower, keyLower):
			if not keyLower:
				return (-1, -1)
			a = sLower.find(keyLower)
			if a == -1:
				return (-1, -1)
			return (a, a+len(keyLower))

		def _GetCharIndexFromX(self, vIdx, lx):
			r, s, k = self._GetLineByView(vIdx)
			if r == -1:
				return 0

			relX = lx
			if relX < 0:
				relX = 0

			prefix = self._PrefixText(r)
			relX -= self._pxCache.Get(prefix)
			if relX <= 0:
				return 0

			s = _safe_str(s)

			lo = 0
			hi = len(s)
			while lo < hi:
				mid = (lo + hi) / 2
				if self._pxCache.Get(s[:mid]) <= relX:
					lo = mid + 1
				else:
					hi = mid
			pos = lo - 1
			if pos < 0:
				pos = 0
			if pos > len(s):
				pos = len(s)
			return pos

		def _RecalcDigits(self):
			try:
				n = len(self._lines)
				d = len(str(n if n > 1 else 1))
				self._lnDigits = max(4, min(7, d))
			except:
				self._lnDigits = 4

		def _ReadTailLines(self):
			try:
				f = open(self._path, "rb")
			except:
				return (["syserr.txt not found."], 0)

			try:
				f.seek(0, 2)
				end = int(f.tell())
			except:
				end = 0

			if end <= 0:
				try:
					f.close()
				except:
					pass
				return (["No errors found."], end)

			max_lines = int(self.MAX_LINES)
			want_nl = max_lines + 1

			read_total = 0
			pos = end
			buf = ""
			lines = []
			while pos > 0 and len(lines) < want_nl and read_total < self.TAIL_HARD_MAX:
				ch = self.TAIL_CHUNK
				if ch > pos:
					ch = pos
				pos -= ch
				try:
					f.seek(pos)
					data = f.read(ch)
				except:
					break
				read_total += ch
				if not data:
					break
				try:
					data = data.replace("\r", "")
				except:
					pass
				buf = data + buf
				lines = buf.split("\n")

			try:
				f.close()
			except:
				pass

			if lines and lines[-1] == "":
				lines.pop()

			if len(lines) > max_lines:
				lines = lines[-max_lines:]

			if not lines:
				lines = ["No errors found."]

			return (lines, end)

		def ReloadFull(self):
			self._tailBuf = ""
			self._filePos = 0
			self._ResetSel(True, True)
			self._matchCur = -1

			lines, endPos = self._ReadTailLines()
			self._filePos = int(endPos)

			kinds = []
			for s in lines:
				if s is None:
					s = ""
				s = _safe_str(s)
				kinds.append(self._KindOf(s))

			self._lines = lines if lines else ["No errors found."]
			if len(kinds) != len(self._lines):
				kinds = []
				for s in self._lines:
					kinds.append(self._KindOf(_safe_str(s)))

			self._kinds = kinds
			self._RecalcDigits()
			self._UpdateCounts()

			try:
				st = os.stat(self._path)
				self._lastSig = (int(st.st_mtime), int(st.st_size))
			except:
				self._lastSig = (0, 0)

			try:
				t = _safe_str(self.search.GetText()).strip()
			except:
				t = ""
			self._searchKey = t.lower() if t else ""
			self._searchCur = self._searchKey
			self._searchSafe = self._searchKey

			self._pxCache.Reset()
			self._BuildFiltered()

			self.ScrollToEnd()
			self._ToastMsg("Refresh", 0.55)

			self._viewDirty = True
			self.UpdateView()
			return True

		def _AppendDelta(self):
			try:
				f = open(self._path, "rb")
				try:
					f.seek(self._filePos)
				except:
					pass
				data = f.read()
				try:
					self._filePos = f.tell()
				except:
					pass
				f.close()
			except:
				return False

			if not data:
				return False

			try:
				data = (self._tailBuf + data).replace("\r", "")
			except:
				try:
					data = _safe_str(self._tailBuf) + _safe_str(data)
					data = data.replace("\r", "")
				except:
					return False

			parts = data.split("\n")

			try:
				if data and data[-1] != "\n":
					self._tailBuf = parts.pop()
				else:
					self._tailBuf = ""
			except:
				self._tailBuf = ""

			added = 0
			for s in parts:
				if s is None:
					continue
				s = _safe_str(s)
				if not s:
					continue
				self._lines.append(s)
				k = self._KindOf(s)
				self._kinds.append(k)
				if k == self.K_ERR:
					self._cntErr += 1
				added += 1

			if added <= 0:
				return False

			if len(self._lines) > self.MAX_LINES:
				cut = len(self._lines) - self.MAX_LINES
				if cut > 0:
					self._lines = self._lines[cut:]
					self._kinds = self._kinds[cut:]
					self._UpdateCounts()

			self._RecalcDigits()
			return True

		def ClearSyserr(self):
			try:
				f = open(self._path, "wb")
				f.write("")
				f.close()
			except:
				pass

			self._lines = ["No errors found."]
			self._kinds = [self.K_INFO]
			self._cntErr = 0
			self._RecalcDigits()

			self._tailBuf = ""
			self._filePos = 0
			self._ResetSel(True, True)
			self._matchCur = -1

			try:
				self.search.SetText("")
			except:
				pass

			self._searchKey = ""
			self._searchCur = ""
			self._searchSafe = ""

			self._pxCache.Reset()
			self._BuildFiltered()

			try:
				self.scroll.SetPos(0.0)
			except:
				pass

			self.UpdateView()
			self._ToastMsg("Cleared", 0.55)
			return True

		def ScrollToEnd(self):
			total = self._ViewTotal()
			if total <= self.perPage:
				self.scroll.SetPos(0.0)
			else:
				self.scroll.SetPos(1.0)
			self._viewDirty = True
			self.UpdateView()
			return True

		def OnScroll(self):
			self._viewDirty = True
			self.UpdateView()
			return True

		def _OnSearchEscape(self):
			try:
				self.search.KillFocus()
			except:
				pass
			self._UpdateSearchHint()
			self.Close()
			return True

		def _OnSearchEnter(self):
			try:
				t = _safe_str(self.search.GetText()).strip()
			except:
				t = ""
			self._searchKey = t.lower() if t else ""
			self._searchCur = self._searchKey
			self._searchSafe = self._searchKey
			self._ApplySearchNow()
			self._UpdateSearchHint()
			return True

		def _ApplySearchNow(self):
			self._ResetSel(True, True)
			self._matchCur = -1
			self._BuildFiltered()
			self._viewDirty = True
			if self._AtBottom():
				self.ScrollToEnd()
			else:
				self.UpdateView()

		def ToggleTheme(self):
			try:
				self._theme.Toggle()
			except:
				pass
			self._ApplyTheme()
			self._ToastMsg("Theme", 0.55)
			return True

		def _MenuRect(self):
			return (self.contentX, self.contentY, self.contentW, self.contentH)

		def _CloseMenuIfClickOutside(self, cx, cy):
			if not (self._menu and self._menu.IsOpen()):
				return
			ax, ay = _abs_pos(self._menu)
			if not (ax <= cx < ax + self._menu.GetWidth() and ay <= cy < ay + self._menu.GetHeight()):
				self._menu.Close()
				self.btnMenu.SetToggle(False)

		def ToggleMenu(self):
			try:
				if self._menu and self._menu.IsOpen():
					self._menu.Close()
					self.btnMenu.SetToggle(False)
					return True
			except:
				pass
			self.OpenMenu()
			return True

		def OpenMenu(self):
			cx, cy = _cursor()
			rx, ry, rw, rh = self._MenuRect()
			lx, ly = _global_to_local(self.inner, int(cx + 6), int(cy + 6))
			self._menu.SetClampRect(rx, ry, rw, rh)
			self._menu.OpenAt(lx, ly)
			try:
				self.btnMenu.SetToggle(True)
			except:
				pass
			return True

		def ToggleGoto(self):
			try:
				if self._goto and self._goto.IsShow():
					self._goto.Hide()
					self.btnGoto.SetToggle(False)
					return True
			except:
				pass
			cx, cy = _cursor()
			rx, ry, rw, rh = self._MenuRect()
			lx, ly = _global_to_local(self.inner, int(cx - 40), int(cy - 20))
			self._goto.OpenIn(lx, ly, (rx, ry, rw, rh))
			try:
				self.btnGoto.SetToggle(True)
			except:
				pass
			return True

		def _OnPanelRight(self):
			try:
				if self._menu and self._menu.IsOpen():
					self._menu.Close()
					self.btnMenu.SetToggle(False)
					return True
			except:
				pass
			self.OpenMenu()
			return True

		def _GoLine(self, n):
			total = self._ViewTotal()
			if total <= 0:
				return

			targetRaw = n - 1
			if targetRaw < 0:
				targetRaw = 0
			if targetRaw >= len(self._lines):
				targetRaw = len(self._lines) - 1

			if self._filtered is None:
				v = targetRaw
			else:
				v = 0
				try:
					v = self._filtered.index(targetRaw)
				except:
					v = 0

			if v < 0:
				v = 0
			if v >= total:
				v = total - 1

			self.selectedView = set([v])
			self.selAnchor = self.selLast = v
			self.selActive = False
			self.textSelActive = False
			self.textAnchorLine = self.textLastLine = -1

			per = self.perPage
			if total > per:
				top = v - (per/2)
				if top < 0:
					top = 0
				if top > total - per:
					top = total - per
				self.scroll.SetPos(float(top) / float(total - per))
			else:
				self.scroll.SetPos(0.0)

			self._viewDirty = True
			self.UpdateView()

		def CopySelection(self):
			try:
				txtSel = self._GetSelectedText()
				if txtSel:
					return True if self._SafeClipboard(txtSel) else True

				if self.selectedView:
					out = []
					for v in sorted(self.selectedView):
						r, s, k = self._GetLineByView(v)
						if r != -1:
							out.append(_safe_str(s))
					if out:
						self._SafeClipboard("\n".join(out))
						return True
			except:
				pass
			return True


		def _HitTestIdxRow(self):
			cx, cy = _cursor()
			ax, ay = _abs_pos(self)
			lx, ly = cx - ax - 7, cy - ay - 7

			if lx < self.contentX or lx > (self.contentX + self.contentW):
				return (-1, -1, lx, ly)
			if ly < self.contentY or ly >= (self.contentY + self.contentH):
				return (-1, -1, lx, ly)

			row = int((ly - self.contentY) / float(self.LINE_H))
			idx = self.viewStart + row

			total = self._ViewTotal()
			if idx < 0 or idx >= total:
				return (-1, row, lx, ly)

			return (idx, row, lx, ly)


		def _OnLineHover(self, localRow, on):
			return True

		def UpdateView(self):
			total = self._ViewTotal()
			per = self.perPage

			try:
				self.scroll.SetRange(total, per)
			except:
				pass

			if total <= per:
				self.viewStart = 0
			else:
				try:
					pos = float(self.scroll.GetPos())
				except:
					pos = 0.0
				self.viewStart = int((total - per) * pos)
				if self.viewStart < 0:
					self.viewStart = 0
				elif self.viewStart > total - per:
					self.viewStart = total - per

			if not self._viewDirty and self.viewStart == self._lastViewStart:
				return

			self._viewDirty = False
			self._lastViewStart = self.viewStart

			p = self._P()
			key = self._searchKey
			hasKey = True if key else False
			textSel = self._NormalizeTextSelection()
			selCol = p["sel"]

			for i in xrange(per):
				vIdx = self.viewStart + i
				y = self.contentY + i * self.LINE_H

				self._pool.SetRowBg(i, 0)
				self._pool.ClearRowMatch(i)
				self._pool.ClearRowSel(i)

				if vIdx >= total:
					self._pool.SetVisible(i, False)
					continue

				r, txt, k = self._GetLineByView(vIdx)
				txt = _safe_str(txt)
				col = self._ColorOfKind(k)

				prefix = self._PrefixText(r if r != -1 else vIdx)
				display = prefix + txt

				fullLine = False
				partSel = None
				inTextSelRange = False

				if textSel:
					sL, sC, eL, eC = textSel
					if sL <= vIdx <= eL:
						inTextSelRange = True
						if sL == eL:
							partSel = (sC, eC)
						elif vIdx == sL:
							partSel = (sC, len(txt))
						elif vIdx == eL:
							partSel = (0, eC)
						else:
							fullLine = True

				if vIdx in self.selectedView and not partSel and not inTextSelRange:
					fullLine = True
					partSel = None

				if hasKey and not fullLine and not partSel and r != -1:
					try:
						low = txt.lower()
					except:
						low = ""
					a, b = self._FindMatchSpan(low, key)
					if a != -1:
						prefixPx = self._pxCache.Get(prefix)
						x1 = prefixPx + self._pxCache.Get(txt[:a])
						x2 = prefixPx + self._pxCache.Get(txt[:b])
						self._pool.SetRowMatch(
							i,
							self.contentX + 6 + x1,
							y,
							max(1, x2 - x1),
							self.LINE_H,
							p["match_cur"] if vIdx == self._matchCur else p["match"]
						)

				if fullLine:
					self._pool.SetRowBg(i, selCol)
				elif partSel:
					a, b = partSel
					prefixPx = self._pxCache.Get(prefix)
					x1 = prefixPx + self._pxCache.Get(txt[:a])
					x2 = prefixPx + self._pxCache.Get(txt[:b])
					self._pool.SetRowSel(
						i,
						self.contentX + 6 + x1,
						y,
						max(1, x2 - x1),
						self.LINE_H,
						selCol
					)

				self._pool.SetRowText(i, display, col)
				self._pool.SetVisible(i, True)



		def _OnLineDbl(self, localRow, lx, ly):
			vIdx = self.viewStart + localRow
			total = self._ViewTotal()
			if vIdx < 0 or vIdx >= total:
				return True
			r, s, k = self._GetLineByView(vIdx)
			if r != -1:
				self._SafeClipboard(s)
			return True

		def _OnLineRight(self, localRow, lx, ly):
			if self.isDragging or self.isResizing:
				return False
			vIdx = self.viewStart + localRow
			total = self._ViewTotal()
			if vIdx < 0 or vIdx >= total:
				return False
			ns = self._NormalizeTextSelection()
			if ns:
				sL, sC, eL, eC = ns
				if not (sL <= vIdx <= eL):
					self.textSelActive = False
					self.textAnchorLine = self.textLastLine = -1
			if not self._NormalizeTextSelection():
				if vIdx not in self.selectedView:
					self.selectedView = set([vIdx])
					self.selAnchor = self.selLast = vIdx
					self.selActive = False
					self._viewDirty = True
					self.UpdateView()
			try:
				if self._menu and self._menu.IsOpen():
					self._menu.Close()
			except:
				pass
			self.OpenMenu()
			return True

		def _OnLineDown(self, localRow, lx, ly):
			if self.isDragging or self.isResizing:
				return False
			vIdx = self.viewStart + localRow
			total = self._ViewTotal()
			if vIdx < 0 or vIdx >= total:
				return False

			shift = _shift_down()
			ctrl = _ctrl_down()
			alt = _alt_down()

			r, txt, k = self._GetLineByView(vIdx)
			prefix = self._PrefixText(r if r != -1 else vIdx)
			prefixPx = self._pxCache.Get(prefix)
			isPrefixArea = True if lx <= (prefixPx + 6) else False

			if isPrefixArea and not alt:
				self.textSelActive = False
				self.textAnchorLine = self.textLastLine = -1

				if ctrl and not shift:
					if vIdx in self.selectedView:
						try:
							self.selectedView.remove(vIdx)
						except:
							pass
					else:
						self.selectedView.add(vIdx)
					if self.selAnchor == -1:
						self.selAnchor = vIdx
					self.selLast = vIdx
					self.selActive = False
					self._viewDirty = True
					self.UpdateView()
					return True

				if shift and self.selAnchor != -1:
					a, b = self.selAnchor, vIdx
					if a > b:
						a, b = b, a
					self.selectedView = set(xrange(a, b+1))
					self.selLast = vIdx
					self.selActive = True
					self._viewDirty = True
					self.UpdateView()
					return True

				self.selectedView = set([vIdx])
				self.selAnchor = self.selLast = vIdx
				self.selActive = False
				self._viewDirty = True
				self.UpdateView()
				return True

			self.selectedView = set([vIdx])
			self.selAnchor = self.selLast = vIdx
			self.selActive = False

			txt = _safe_str(txt)
			ch = self._GetCharIndexFromX(vIdx, lx - 6)
			if ch < 0:
				ch = 0
			if ch > len(txt):
				ch = len(txt)

			self.textSelActive = True
			self.textAnchorLine = self.textLastLine = vIdx
			self.textAnchorChar = self.textLastChar = ch
			self._viewDirty = True
			self.UpdateView()
			return True

		def _OnLineUp(self):
			try:
				if self._menu and self._menu.IsOpen():
					self._menu.Close()
					self.btnMenu.SetToggle(False)
			except:
				pass

			self.selActive = False
			self.textSelActive = False
			return True


		def StartResize(self):
			self.isResizing = True
			self.resizeStartX, self.resizeStartY = _cursor()
			self.startW, self.startH = self.GetWidth(), self.GetHeight()

		def EndResize(self):
			self.isResizing = False
			self._RebuildLayout(True)
			return True

		def _AutoScroll(self, direction):
			total = self._ViewTotal()
			per = self.perPage
			if total <= per:
				return
			step_lines = 2
			total_minus = (total - per)
			if total_minus <= 0:
				return
			self.scroll.AddLines(total_minus, direction * step_lines)

		def _UpdateDragSelection(self):
			if not _is_left_down():
				self.selActive = False
				if not self.textSelActive:
					return

			if self.textSelActive:
				vIdx, row, lx, ly = self._HitTestIdxRow()
				if vIdx == -1:
					if ly < self.contentY:
						self._AutoScroll(-1)
						vIdx = self.viewStart
					elif ly > (self.contentY + self.contentH):
						self._AutoScroll(+1)
						vIdx = self.viewStart + self.perPage - 1
					else:
						return

				total = self._ViewTotal()
				if total <= 0:
					return
				if vIdx < 0:
					vIdx = 0
				if vIdx >= total:
					vIdx = total - 1

				ch = self._GetCharIndexFromX(vIdx, lx - 6)
				r, txt, k = self._GetLineByView(vIdx)
				txt = _safe_str(txt)
				if ch < 0:
					ch = 0
				if ch > len(txt):
					ch = len(txt)

				if vIdx == self.textLastLine and ch == self.textLastChar:
					return

				self.textLastLine = vIdx
				self.textLastChar = ch
				self._viewDirty = True
				self.UpdateView()
				return

			if not self.selActive:
				return

			vIdx, row, lx, ly = self._HitTestIdxRow()
			if vIdx == -1:
				if ly < self.contentY:
					self._AutoScroll(-1)
					vIdx = self.viewStart
				elif ly > (self.contentY + self.contentH):
					self._AutoScroll(+1)
					vIdx = self.viewStart + self.perPage - 1
				else:
					return

			total = self._ViewTotal()
			if total <= 0:
				return
			if vIdx < 0:
				vIdx = 0
			if vIdx >= total:
				vIdx = total - 1

			if vIdx == self.selLast:
				return

			self.selLast = vIdx
			a, b = self.selAnchor, vIdx
			if a > b:
				a, b = b, a
			self.selectedView = set(xrange(a, b+1))
			self._viewDirty = True
			self.UpdateView()

		def _JumpMatch(self, forward=True):
			key = self._searchKey
			if not key:
				self._ToastMsg("No find", 0.55)
				return True

			total = self._ViewTotal()
			if total <= 0:
				return True

			cur = self._matchCur
			step = 1 if forward else -1

			if cur != -1:
				start = cur + step
			else:
				start = self.viewStart if forward else (self.viewStart + self.perPage - 1)

			if start < 0:
				start = total - 1
			if start >= total:
				start = 0

			found = -1
			v = start
			for _ in xrange(total):
				r, s, k = self._GetLineByView(v)
				try:
					ok = (key in s.lower())
				except:
					ok = False
				if ok:
					found = v
					break
				v += step
				if v < 0:
					v = total - 1
				if v >= total:
					v = 0

			if found == -1:
				self._ToastMsg("No match", 0.55)
				return True

			self._matchCur = found
			self.selectedView = set([found])
			self.selAnchor = self.selLast = found
			self.selActive = False
			self.textSelActive = False
			self.textAnchorLine = self.textLastLine = -1

			per = self.perPage
			if total > per:
				top = found - (per/2)
				if top < 0:
					top = 0
				if top > total - per:
					top = total - per
				self.scroll.SetPos(float(top) / float(total - per))
			else:
				self.scroll.SetPos(0.0)

			self._viewDirty = True
			self.UpdateView()
			return True

		def _PollHotkeys(self):
			ctrl = _ctrl_down()
			shift = _shift_down()

			if ctrl and self._KeyEdge(app.DIK_F):
				try:
					self.search.SetFocus()
				except:
					pass
				return

			if ctrl and self._KeyEdge(app.DIK_G):
				self.ToggleGoto()
				return

			if ctrl and self._KeyEdge(app.DIK_C):
				self.CopySelection()
				return

			if ctrl and self._KeyEdge(app.DIK_A):
				total = self._ViewTotal()
				if total <= 0:
					return

				self.selectedView = set(xrange(total))
				self.selAnchor = 0
				self.selLast = total - 1
				self.selActive = False

				self.textSelActive = False
				self.textAnchorLine = self.textLastLine = -1

				self._viewDirty = True
				self.UpdateView()
				return

			if self._KeyEdge(app.DIK_ESCAPE):
				try:
					if self._menu and self._menu.IsOpen():
						self._menu.Close()
						self.btnMenu.SetToggle(False)
						return
				except:
					pass
				try:
					if self._goto and self._goto.IsShow():
						self._goto.Hide()
						self.btnGoto.SetToggle(False)
						return
				except:
					pass
				try:
					self.Close()
				except:
					pass
				return

		def Close(self):
			self.isDragging = False
			self.isResizing = False
			self.selActive = False
			self.textSelActive = False

			try:
				if self._menu:
					self._menu.Close()
				self.btnMenu.SetToggle(False)
			except:
				pass

			try:
				if self._goto:
					self._goto.Hide()
				self.btnGoto.SetToggle(False)
			except:
				pass

			try:
				self.Hide()
			except:
				pass
			return True

		def OnPressEscapeKey(self):
			self.Close()
			return True

		def OnMouseWheel(self, nLen):
			try:
				self._viewDirty = True
			except:
				pass

			total = self._ViewTotal()
			per = self.perPage
			if total <= per:
				return False

			total_minus = (total - per)
			if total_minus <= 0:
				return False

			try:
				self._wheelAcc += (-float(nLen)) * float(self._wheelLines)
			except:
				self._wheelAcc = 0.0
				return False

			step = 0
			if self._wheelAcc >= 1.0:
				step = int(self._wheelAcc)
			elif self._wheelAcc <= -1.0:
				step = -int(-self._wheelAcc)

			if step:
				self._wheelAcc -= float(step)
				try:
					self.scroll.AddLines(total_minus, step)
				except:
					pass

			try:
				self.UpdateView()
			except:
				pass
			return True

		def OnUpdate(self):
			now = _now()

			try:
				self._hub.Tick(now)
			except:
				pass

			ml = _is_left_down()
			edgeL = ml and not self._prevKey.get("ML", 0)
			self._prevKey["ML"] = ml

			try:
				if self.btnMenu.IsToggled() and not (self._menu and self._menu.IsOpen()):
					self.btnMenu.SetToggle(False)
			except:
				pass
			try:
				if self.btnGoto.IsToggled() and not (self._goto and self._goto.IsShow()):
					self.btnGoto.SetToggle(False)
			except:
				pass

			if edgeL:
				cx, cy = _cursor()
				self._CloseMenuIfClickOutside(cx, cy)
				try:
					if self._goto and self._goto.IsShow():
						ax, ay = _abs_pos(self._goto)
						if not (ax <= cx < ax + self._goto.GetWidth() and ay <= cy < ay + self._goto.GetHeight()):
							self._goto.Hide()
							self.btnGoto.SetToggle(False)
				except:
					pass

			self._PollHotkeys()
			self._UpdateDragSelection()

			if self.isDragging:
				cx, cy = _cursor()
				try:
					self.SetPosition(self.winX + (cx - self.dragStartX), self.winY + (cy - self.dragStartY))
				except:
					pass

			if self.isResizing:
				cx, cy = _cursor()
				try:
					nw = self.startW + (cx - self.resizeStartX)
					nh = self.startH + (cy - self.resizeStartY)
				except:
					nw = self.startW
					nh = self.startH

				if nw < self.MIN_W:
					nw = self.MIN_W
				if nh < self.MIN_H:
					nh = self.MIN_H

				try:
					self.SetSize(int(nw), int(nh))
				except:
					pass
				try:
					self._RebuildLayout(False)
				except:
					pass

			if self.search.IsShow():
				try:
					cur = _safe_str(self.search.GetText()).strip()
					self._UpdateSearchHint()
				except:
					cur = ""

				cur_cmp = cur.lower() if cur else ""

				if not _ctrl_down():
					if cur_cmp != self._searchSafe:
						self._searchSafe = cur_cmp
						self._searchNextApply = now + self.SEARCH_APPLY_DELAY

					if cur_cmp != self._searchCur and now >= self._searchNextApply:
						self._searchCur = cur_cmp
						self._searchKey = cur_cmp
						self._ApplySearchNow()
				else:
					if cur_cmp != self._searchSafe:
						try:
							self.search.SetText(self._searchSafe)
						except:
							pass

			if now < self._nextPoll:
				return
			self._nextPoll = now + self.POLL_TIME

			try:
				st = os.stat(self._path)
				sig = (int(st.st_mtime), int(st.st_size))
			except:
				return

			if sig == self._lastSig:
				return

			oldM, oldS = self._lastSig
			self._lastSig = sig

			if sig[1] < oldS:
				self.ReloadFull()
				return

			atBottom = self._AtBottom()

			if self._stop:
				return

			if self._AppendDelta():
				self._BuildFiltered()
				if atBottom:
					self.ScrollToEnd()
				else:
					self._viewDirty = True
					self.UpdateView()

	def OpenSyserrViewer():
		global __SYSERR_VIEWER
		try:
			if __SYSERR_VIEWER and __SYSERR_VIEWER.IsShow():
				_safe_top(__SYSERR_VIEWER)
				return __SYSERR_VIEWER
		except:
			pass
		__SYSERR_VIEWER = SyserrViewer()
		return __SYSERR_VIEWER