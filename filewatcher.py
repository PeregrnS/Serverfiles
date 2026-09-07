# -*- coding: utf-8 -*-

"""
filewatcher.py
Version: 1.1 
Author: fratello_exe
buyer discord: erikcikadir
"""

import app
if app.ENABLE_FILEWATCHER:

	import os
	import sys
	import imp
	import time
	import traceback
	
	try:
		import ui
		import chat
		import wndMgr
	except:
		ui = None
		chat = None
		wndMgr = None
		app = None
	
	try:
		import dbg
	except:
		dbg = None
	
	try:
		import constInfo
	except:
		constInfo = None
	
	ROOT_PATH = r"main\data\root"
	UISCRIPT_PATH = r"main\data\root\uiscript"
	
	WATCH_INTERVAL = 0.10
	DEBOUNCE_SEC = 0.14
	
	TITLEBAR_H = 22
	DOUBLE_CLICK_SEC = 0.55
	DOUBLE_CLICK_DIST = 6
	
	AUTO_TARGET_ON_MOUSE = True
	AUTO_TOGGLE_ON_DOUBLE_CLICK = True
	EXCLUSIVE_SESSION = True
	
	CHAT_PREFIX = "[Filewatcher:] "
	CHAT_THROTTLE_SEC = 0.12
	CHAT_FAIL_LINES = 3
	
	MAX_READ_BYTES = 8 * 1024 * 1024
	
	_last_chat_t = 0.0
	_last_chat_msg = None
	
	_HUB = None
	_MGR = None
	
	_ORIG_LOAD = None
	_ORIG_MOUSE_DOWN = None
	_ORIG_DESTROY = None
	_CONSTINFO_ORIG_SET = None
	
	_ORIG_MOUSE_DOWN_TITLE = None
	_ORIG_MOUSE_DOWN_BWTB = None
	
	_GUARD_T = 0.0
	_GUARD_X = 0
	_GUARD_Y = 0
	_GUARD_TID = 0
	
	def _now():
		try:
			return app.GetTime()
		except:
			return time.time()
	
	def _chat(msg):
		global _last_chat_t, _last_chat_msg
		if not chat:
			return
		try:
			t = _now()
			if msg == _last_chat_msg and (t - _last_chat_t) < CHAT_THROTTLE_SEC:
				return
			_last_chat_t = t
			_last_chat_msg = msg
		except:
			pass
		try:
			chat.AppendChat(chat.CHAT_TYPE_INFO, msg)
		except:
			try:
				chat.AppendChat(0, msg)
			except:
				pass
	
	def _trace(msg):
		try:
			if dbg and hasattr(dbg, "TraceError"):
				dbg.TraceError(msg)
		except:
			pass
	
	def _to_str(x):
		try:
			if isinstance(x, unicode):
				return x.encode("utf8")
		except:
			pass
		try:
			return str(x)
		except:
			return None
	
	def _norm(p):
		try:
			return os.path.normpath(p)
		except:
			return p
	
	def _realpath(p):
		try:
			return os.path.realpath(p)
		except:
			try:
				return os.path.abspath(p)
			except:
				return p
	
	def _exists(p):
		try:
			return os.path.exists(p)
		except:
			return False
	
	def _isfile(p):
		try:
			return os.path.isfile(p)
		except:
			return False
	
	def _isdir(p):
		try:
			return os.path.isdir(p)
		except:
			return False
	
	def _splitext(p):
		try:
			return os.path.splitext(p)
		except:
			return (p, "")
	
	def _join(a, b):
		try:
			return os.path.join(a, b)
		except:
			return (a.rstrip("/\\") + "/" + b.lstrip("/\\"))
	
	def _pyc_to_py(p):
		try:
			base, ext = _splitext(p)
			if ext.lower() in (".pyc", ".pyo"):
				return base + ".py"
		except:
			pass
		return p
	
	def _strip_bom(raw):
		try:
			if raw and raw.startswith("\xef\xbb\xbf"):
				return raw[3:]
		except:
			pass
		return raw
	
	def _read_file_bytes(path):
		f = None
		try:
			f = open(path, "rb")
			raw = f.read(MAX_READ_BYTES + 1)
			if len(raw) > MAX_READ_BYTES:
				raw = raw[:MAX_READ_BYTES]
			return raw
		except:
			return None
		finally:
			try:
				if f:
					f.close()
			except:
				pass
	
	def _stat_sig(p):
		try:
			st = os.stat(p)
			try:
				ct = float(getattr(st, "st_ctime", 0.0))
			except:
				ct = 0.0
			return (float(st.st_mtime), int(st.st_size), ct)
		except:
			return (0.0, 0, 0.0)
	
	def _get_root():
		global ROOT_PATH
		try:
			r = _to_str(ROOT_PATH)
		except:
			r = None
		if r:
			r = _norm(_realpath(r))
			if _isdir(r):
				return r
		try:
			h = os.path.dirname(_realpath(__file__))
			if h and _isdir(h):
				return _norm(_realpath(h))
		except:
			pass
		try:
			cwd = os.getcwd()
			if cwd and _isdir(cwd):
				return _norm(_realpath(cwd))
		except:
			pass
		return ""
	
	def _get_uiscript():
		global UISCRIPT_PATH
		try:
			u = _to_str(UISCRIPT_PATH)
		except:
			u = None
		if u:
			u = _norm(_realpath(u))
			if _isdir(u):
				return u
		return ""
	
	def _ensure_syspath():
		r = _get_root()
		try:
			if r and r not in sys.path:
				sys.path.insert(0, r)
		except:
			pass
	
	def _pretty_path(p):
		try:
			if not p:
				return ""
			p = _norm(_realpath(p))
			r = _get_root()
			if r:
				try:
					rr = _norm(_realpath(r))
					if not rr.endswith(os.sep):
						rr += os.sep
					if p.startswith(rr):
						return os.path.relpath(p, r).replace("\\", "/")
				except:
					pass
			return os.path.basename(p)
		except:
			return os.path.basename(p or "")
	
	def _resolve_uipath(filename):
		if not filename:
			return None
		fn = _to_str(filename)
		if not fn:
			return None
		try:
			fn = fn.replace("/", os.sep).replace("\\", os.sep)
		except:
			pass
		fn = _norm(fn)
	
		try:
			if os.path.isabs(fn):
				ab = _norm(_realpath(fn))
				if _exists(ab) and _isfile(ab):
					return ab
		except:
			pass
	
		try:
			parts = [x for x in fn.split(os.sep) if x]
			while parts and parts[0].lower() in ("uiscript", "uiscripts"):
				parts = parts[1:]
			rest = os.sep.join(parts) if parts else fn
		except:
			rest = fn
	
		u = _get_uiscript()
		if u:
			cand = _norm(_realpath(_join(u, rest)))
			if _exists(cand) and _isfile(cand):
				return cand
		return None
	
	def _mouse_pos():
		try:
			if wndMgr and hasattr(wndMgr, "GetMousePosition"):
				return wndMgr.GetMousePosition()
		except:
			pass
		try:
			if app and hasattr(app, "GetCursorPosition"):
				return app.GetCursorPosition()
		except:
			pass
		return (0, 0)
	
	def _fw_guard(win):
		global _GUARD_T, _GUARD_X, _GUARD_Y, _GUARD_TID
		try:
			mx, my = _mouse_pos()
		except:
			mx, my = (0, 0)
		try:
			now = _now()
		except:
			now = time.time()
		try:
			tid = id(_find_owner_window(win) or win)
		except:
			tid = id(win)
		try:
			if tid == _GUARD_TID and (now - _GUARD_T) < 0.03 and abs(mx - _GUARD_X) <= 1 and abs(my - _GUARD_Y) <= 1:
				return False
		except:
			pass
		_GUARD_T = now
		_GUARD_X = mx
		_GUARD_Y = my
		_GUARD_TID = tid
		return True
	
	def _win_global_pos(win):
		try:
			if hasattr(win, "GetGlobalPosition"):
				return win.GetGlobalPosition()
		except:
			pass
		return (0, 0)
	
	def _win_size(win):
		w = 0
		h = 0
		try:
			if hasattr(win, "GetWidth"):
				w = win.GetWidth()
			if hasattr(win, "GetHeight"):
				h = win.GetHeight()
		except:
			pass
		return (w, h)
	
	def _get_parent(win):
		if not win:
			return None
		try:
			if hasattr(win, "GetParent"):
				return win.GetParent()
		except:
			pass
		try:
			return getattr(win, "parent", None) or getattr(win, "_parent", None) or getattr(win, "m_parent", None)
		except:
			return None
	
	def _looks_like_scriptwindow(win):
		if not win:
			return False
		try:
			return hasattr(win, "GetChild") and hasattr(win, "Show") and hasattr(win, "Hide")
		except:
			return False
	
	def _find_owner_window(win):
		cur = win
		last_any = None
		last_script = None
		for _ in xrange(96):
			if not cur:
				break
			last_any = cur
			if _looks_like_scriptwindow(cur):
				last_script = cur
			p = _get_parent(cur)
			if not p:
				break
			cur = p
		return last_script or last_any or win
	
	def _geom_capture(win):
		if not win:
			return None
		x = 0
		y = 0
		w = 0
		h = 0
		try:
			if hasattr(win, "GetLocalPosition"):
				x, y = win.GetLocalPosition()
			elif hasattr(win, "GetPosition"):
				x, y = win.GetPosition()
			elif hasattr(win, "GetGlobalPosition"):
				gx, gy = win.GetGlobalPosition()
				p = _get_parent(win)
				if p and hasattr(p, "GetGlobalPosition"):
					px, py = p.GetGlobalPosition()
					x, y = (gx - px, gy - py)
				else:
					x, y = (gx, gy)
		except:
			x, y = (0, 0)
		try:
			if hasattr(win, "GetWidth"):
				w = win.GetWidth()
			if hasattr(win, "GetHeight"):
				h = win.GetHeight()
		except:
			w, h = (0, 0)
		return (int(x), int(y), int(w), int(h))
	
	def _geom_apply(win, geom):
		if not win or not geom:
			return
		try:
			x, y, w, h = geom
			if hasattr(win, "SetSize") and w > 0 and h > 0:
				win.SetSize(w, h)
			if hasattr(win, "SetPosition"):
				win.SetPosition(x, y)
		except:
			pass
	
	def _safe_del_module(modname):
		try:
			if modname in sys.modules:
				del sys.modules[modname]
		except:
			pass
	
	def _inject_core_deps(d):
		try:
			if ui is not None:
				d["ui"] = ui
		except:
			pass
		try:
			if chat is not None:
				d["chat"] = chat
		except:
			pass
		try:
			if wndMgr is not None:
				d["wndMgr"] = wndMgr
		except:
			pass
		try:
			if app is not None:
				d["app"] = app
		except:
			pass
		try:
			if dbg is not None:
				d["dbg"] = dbg
		except:
			pass
		try:
			if constInfo is not None:
				d["constInfo"] = constInfo
		except:
			pass
	
	def _make_module(modname, filepath):
		mod = imp.new_module(modname)
		sys.modules[modname] = mod
		d = mod.__dict__
		try:
			d.clear()
		except:
			pass
		d["__name__"] = modname
		d["__file__"] = filepath
		d["__builtins__"] = __builtins__
		_inject_core_deps(d)
		return mod
	
	def _load_source(modname, filepath):
		filepath = _norm(_realpath(_pyc_to_py(filepath)))
		if not filepath or not _exists(filepath) or not _isfile(filepath):
			return None, "file missing"
		_safe_del_module(modname)
		raw = _read_file_bytes(filepath)
		if raw is None:
			return None, "read failed"
		raw = _strip_bom(raw)
		mod = _make_module(modname, filepath)
		d = mod.__dict__
		try:
			code_obj = compile(raw, filepath, "exec")
		except:
			return None, traceback.format_exc()
		try:
			exec code_obj in d
		except:
			return None, traceback.format_exc()
		_inject_core_deps(d)
		return mod, None
	
	def _load_uiscript_windowdict(p):
		raw = _read_file_bytes(p)
		if raw is None:
			return None, "read"
		raw = _strip_bom(raw)
		try:
			d = {"__builtins__": __builtins__, "__file__": p, "__name__": "__uiscript__"}
			exec compile(raw, p, "exec") in d
			w = d.get("window", None)
			if type(w) != dict:
				return None, "no_window"
			return w, None
		except:
			return None, traceback.format_exc()
	
	def _resolve_from_window(win):
		if not win:
			return (None, None)
		try:
			mod = sys.modules.get(win.__class__.__module__)
			if mod:
				p = getattr(mod, "__file__", None)
				if p:
					p = _norm(_realpath(_pyc_to_py(p)))
					if _exists(p):
						return p, win.__class__.__name__
		except:
			pass
		try:
			cls = win.__class__.__name__
			n = cls.lower().replace("window", "")
			if n.startswith("ui"):
				name = n + ".py"
			else:
				name = "ui" + n + ".py"
			r = _get_root()
			if r:
				p = _norm(_realpath(_join(r, name)))
				if _exists(p):
					return p, cls
		except:
			pass
		return (None, None)
	
	def _module_name_for_class(classname):
		return "_fw_live_" + str(classname)
	
	def _fw_patch_uidel():
		try:
			import ui as _ui
		except:
			return
	
		if not _ui or not hasattr(_ui, "ScriptWindow"):
			return
	
		def _wrap_del(cls):
			orig = getattr(cls, "__del__", None)
			if not orig:
				return
	
			def _FW_SafeDel(self):
				try:
					if _ui is None or not hasattr(_ui, "ScriptWindow"):
						return
					orig(self)
				except:
					pass
	
			cls.__del__ = _FW_SafeDel
	
		try:
			_wrap_del(_ui.ScriptWindow)
		except:
			pass
	
		try:
			for cls in _ui.ScriptWindow.__subclasses__():
				try:
					_wrap_del(cls)
				except:
					pass
		except:
			pass
	
	class _WatchHub(object):
		def __init__(self):
			self._path_sig = {}
			self._path_sessions = {}
			self._session_paths = {}
			self._session_cb = {}
			self._last_poll = 0.0
	
		def register(self, session, paths, cb):
			if not session:
				return
			clean = set()
			for p in (paths or []):
				try:
					p = _norm(_realpath(_pyc_to_py(p)))
				except:
					continue
				if not p or not _exists(p) or not _isfile(p):
					continue
				clean.add(p)
	
			self._session_cb[session] = cb
			old = self._session_paths.get(session, set())
	
			added = clean.difference(old)
			removed = old.difference(clean)
	
			for p in added:
				sset = self._path_sessions.get(p)
				if not sset:
					self._path_sessions[p] = set([session])
					self._path_sig[p] = _stat_sig(p)
				else:
					sset.add(session)
	
			for p in removed:
				sset = self._path_sessions.get(p)
				if sset:
					try:
						sset.remove(session)
					except:
						pass
					if not sset:
						try:
							del self._path_sessions[p]
						except:
							pass
						try:
							del self._path_sig[p]
						except:
							pass
	
			self._session_paths[session] = clean
	
		def unregister(self, session):
			if not session:
				return
			old = self._session_paths.get(session, set())
			for p in old:
				sset = self._path_sessions.get(p)
				if sset:
					try:
						sset.remove(session)
					except:
						pass
					if not sset:
						try:
							del self._path_sessions[p]
						except:
							pass
						try:
							del self._path_sig[p]
						except:
							pass
			try:
				del self._session_paths[session]
			except:
				pass
			try:
				del self._session_cb[session]
			except:
				pass
	
		def poll(self):
			try:
				now = _now()
			except:
				now = time.time()
			if (now - self._last_poll) < WATCH_INTERVAL:
				return
			self._last_poll = now
	
			paths = self._path_sig.keys()
			if not paths:
				return
	
			by_session = {}
			for p in paths:
				try:
					nsig = _stat_sig(p)
				except:
					continue
				osig = self._path_sig.get(p)
				if osig is None or nsig == osig:
					continue
				self._path_sig[p] = nsig
				sessions = list(self._path_sessions.get(p, []))
				for s in sessions:
					try:
						by_session.setdefault(s, []).append(p)
					except:
						pass
	
			if not by_session:
				return
	
			for s, changed in by_session.iteritems():
				cb = self._session_cb.get(s)
				if not cb:
					continue
				try:
					cb(changed)
				except:
					pass
	
	
	def _hub():
		global _HUB
		if _HUB is None:
			_HUB = _WatchHub()
		return _HUB
	
	
	class LiveSession(object):
		def __init__(self, original_window, py_filepath, classname, manager):
			self.mgr = manager
			self.original = original_window
			self.py_filepath = _norm(_realpath(py_filepath))
			self.classname = classname
	
			self.window = None
			self._swapped = False
			self._reloading = False
			self._geom = None
	
			self._last_change_at = 0.0
			self._last_trigger = ""
	
			self._instance_key = None
			try:
				self._instance_key = getattr(self.original, "_fw_instance_key", None)
			except:
				self._instance_key = None
	
			self._modname = None
			self._update_watchlist(self.original)
	
		def _on_files_changed(self, changed):
			try:
				self._last_change_at = _now()
			except:
				self._last_change_at = 0.0
			try:
				self._last_trigger = _pretty_path((changed or [""])[0])
			except:
				self._last_trigger = _pretty_path(self.py_filepath)
			self.mgr._mark_pending(self, changed)
	
		def _build_watch_paths(self, win):
			paths = [self.py_filepath]
			ui_paths = []
	
			try:
				p = getattr(win, "_fw_uipaths", None)
				if p:
					for x in p:
						rp = _resolve_uipath(x)
						if rp:
							ui_paths.append(rp)
			except:
				pass
	
			try:
				p = getattr(win, "_fw_last_uipath", None)
				rp = _resolve_uipath(p)
				if rp:
					ui_paths.append(rp)
			except:
				pass
	
			uniq = []
			seen = {}
			for p in (paths + ui_paths):
				try:
					p = _norm(_realpath(_pyc_to_py(p)))
				except:
					continue
				if not p or not _exists(p) or not _isfile(p):
					continue
				if p in seen:
					continue
				seen[p] = 1
				uniq.append(p)
			return uniq
	
		def _update_watchlist(self, win):
			try:
				paths = self._build_watch_paths(win)
			except:
				paths = [self.py_filepath]
			try:
				_hub().register(self, paths, self._on_files_changed)
			except:
				pass
	
		def _swap_in(self):
			if self._swapped:
				return
			self._swapped = True
			try:
				if self.original:
					self.original.Hide()
					try:
						self.original.AddFlag("not_pick")
					except:
						pass
			except:
				pass
	
		def reload(self, reason=""):
			if self._reloading:
				return False
	
			try:
				if reason == "changed" and self._last_change_at and (_now() - self._last_change_at) < DEBOUNCE_SEC:
					self.mgr._mark_pending(self, self.mgr._pending_changed.get(self, None))
					return False
			except:
				pass
	
			self._reloading = True
			try:
				src_geom = None
				if self.window:
					src_geom = _geom_capture(self.window)
				elif self.original:
					src_geom = _geom_capture(self.original)
				self._geom = src_geom or self._geom
	
				old_win = self.window
				old_mod = self._modname
	
				modname = _module_name_for_class(self.classname) + "_" + str(int(_now() * 1000.0))
				mod, err = _load_source(modname, self.py_filepath)
				_fw_patch_uidel()
				if not mod:
					self.mgr._on_reload_failed(self, err or "reload failed", short="compile")
					return False
	
				try:
					cls = getattr(mod, self.classname)
				except:
					self.mgr._on_reload_failed(self, "class not found: %s" % self.classname, short="class")
					_safe_del_module(modname)
					return False
	
				try:
					new_win = cls()
				except:
					self.mgr._on_reload_failed(self, traceback.format_exc(), short="ctor")
					_safe_del_module(modname)
					return False
	
				if self._geom:
					_geom_apply(new_win, self._geom)
	
				try:
					if self._instance_key and constInfo and hasattr(constInfo, "SetInstance"):
						constInfo.SetInstance(self._instance_key, new_win)
				except:
					pass
	
				try:
					for _fn in ("Open", "OnOpen", "RefreshStatus", "RefreshCharacter", "Refresh", "UpdateWindow", "UpdateStatus"):
						f = getattr(new_win, _fn, None)
						if not f or not callable(f):
							continue
						try:
							f()
							break
						except TypeError:
							continue
						except:
							break
				except:
					pass
	
				try:
					new_win.Show()
				except:
					pass
	
	
				self.window = new_win
				self._modname = modname
	
				try:
					self._instance_key = getattr(new_win, "_fw_instance_key", None) or self._instance_key
				except:
					pass
	
				self._update_watchlist(new_win)
	
				if not self._swapped:
					self._swap_in()
	
				if old_win:
					try:
						setattr(old_win, "_fw_internal_destroy", 1)
					except:
						pass
					try:
						old_win.Hide()
					except:
						pass
					try:
						old_win.Destroy()
					except:
						pass
	
				if old_mod and old_mod != self._modname:
					_safe_del_module(old_mod)
	
				self.mgr._on_reload_ok(self, reason=reason, trigger=(self._last_trigger or _pretty_path(self.py_filepath)), silent=(reason == "start"))
				return True
			finally:
				self._reloading = False
	
		def stop(self, show_original=True):
			try:
				_hub().unregister(self)
			except:
				pass
	
			geom = None
			try:
				geom = _geom_capture(self.window) or self._geom
			except:
				geom = self._geom
	
			try:
				if self.window:
					self.window.Hide()
			except:
				pass
			try:
				if self.window:
					try:
						setattr(self.window, "_fw_internal_destroy", 1)
					except:
						pass
					self.window.Destroy()
			except:
				pass
			self.window = None
	
			try:
				if self._modname:
					_safe_del_module(self._modname)
			except:
				pass
			self._modname = None
	
			if show_original and self.original:
				try:
					if geom:
						_geom_apply(self.original, geom)
				except:
					pass
				try:
					self.original.DelFlag("not_pick")
				except:
					pass
				try:
					self.original.Show()
				except:
					pass
				try:
					if self._instance_key and constInfo and hasattr(constInfo, "SetInstance"):
						constInfo.SetInstance(self._instance_key, self.original)
				except:
					pass
	
	
	class _FWManager(object):
		def __init__(self):
			self._sessions = []
			self._session_by_original = {}
			self._session_by_live = {}
			self._active = None
			self._pending = {}
			self._pending_changed = {}
			self._click_state = {}
	
		def set_active(self, win):
			self._active = win
	
		def get_active(self):
			return self._active
	
		def _mark_pending(self, session, changed):
			try:
				self._pending[session] = 1
			except:
				pass
			try:
				if changed:
					self._pending_changed[session] = list(changed)
				else:
					self._pending_changed[session] = None
			except:
				pass
	
		def _find_session_for_window(self, win):
			if not win:
				return None
			try:
				s = self._session_by_original.get(id(win))
				if s:
					return s
				s = self._session_by_live.get(id(win))
				if s:
					return s
			except:
				pass
			return None
	
		def _stop_all_sessions(self, silent=True):
			old = self._sessions[:]
			for s in old:
				try:
					self.stop_session(s, silent=silent)
				except:
					pass
	
		def _pick_toggle_target(self, win):
			try:
				root = _find_owner_window(win)
				if root:
					return root
			except:
				pass
			return self.get_active() or win
	
		def toggle_for_window(self, win):
			win = self._pick_toggle_target(win)
			if not win:
				_chat(CHAT_PREFIX + "no target")
				return
	
			s = self._find_session_for_window(win)
			if s:
				self.stop_session(s, silent=False)
				return
	
			py_path, cls = _resolve_from_window(win)
			if not py_path or not cls:
				_chat(CHAT_PREFIX + "no .py")
				return
	
			if EXCLUSIVE_SESSION:
				self._stop_all_sessions(silent=True)
	
			self.start_session(win, py_path, cls)
	
		def start_session(self, original_win, py_path, cls):
			try:
				old = self._session_by_original.get(id(original_win))
				if old:
					self.stop_session(old, silent=True)
			except:
				pass
	
			try:
				s = LiveSession(original_win, py_path, cls, self)
				self._sessions.append(s)
				self._session_by_original[id(original_win)] = s
	
				if not s.reload(reason="start"):
					self.stop_session(s, silent=True)
					return
	
				try:
					if s.window:
						self._session_by_live[id(s.window)] = s
				except:
					pass
	
				_chat(CHAT_PREFIX + "on: " + _pretty_path(s.py_filepath))
			except:
				self._on_reload_failed(_DummySession(py_path), traceback.format_exc(), short="start")
	
		def stop_session(self, s, silent=False, show_original=True):
			live_win = None
			try:
				live_win = getattr(s, "window", None)
			except:
				live_win = None
	
			try:
				s.stop(show_original=show_original)
			except:
				pass
	
			try:
				if s in self._sessions:
					self._sessions.remove(s)
			except:
				pass
			try:
				if s.original and id(s.original) in self._session_by_original:
					del self._session_by_original[id(s.original)]
			except:
				pass
			try:
				if live_win and id(live_win) in self._session_by_live:
					del self._session_by_live[id(live_win)]
			except:
				pass
			try:
				if s in self._pending:
					del self._pending[s]
			except:
				pass
			try:
				if s in self._pending_changed:
					del self._pending_changed[s]
			except:
				pass
	
			if not silent:
				_chat(CHAT_PREFIX + "off: " + _pretty_path(getattr(s, "py_filepath", "")))
	
		def _on_reload_ok(self, session, reason="", trigger="", silent=False):
			if silent:
				return
			try:
				_chat(CHAT_PREFIX + "reload: " + (trigger or _pretty_path(session.py_filepath)))
			except:
				pass
	
		def _on_reload_failed(self, session, err, short=""):
			try:
				_chat(CHAT_PREFIX + "fail: " + _pretty_path(getattr(session, "py_filepath", "")) + (" [" + short + "]" if short else ""))
			except:
				pass
			try:
				_trace(err or traceback.format_exc())
			except:
				pass
			if CHAT_FAIL_LINES <= 0:
				return
			try:
				tb = err or traceback.format_exc()
				lines = (tb or "").splitlines()
				for ln in lines[-CHAT_FAIL_LINES:]:
					_chat(CHAT_PREFIX + ln)
			except:
				pass
	
		def tick(self):
			sessions = self._sessions[:]
			pending = self._pending.keys()
			self._pending = {}
	
			new_live = {}
			for s in sessions:
				try:
					if s.window:
						new_live[id(s.window)] = s
				except:
					pass
			self._session_by_live = new_live
	
			for s in pending:
				try:
					s.reload(reason="changed")
				except:
					pass
	
		def on_mouse_down(self, win):
			root = None
			try:
				root = _find_owner_window(win)
			except:
				root = win
	
			if AUTO_TARGET_ON_MOUSE:
				self.set_active(root or win)
	
			if AUTO_TOGGLE_ON_DOUBLE_CLICK:
				self._double_click_eval(win, root or win)
	
		def on_destroy(self, win):
			s = self._find_session_for_window(win)
			if not s:
				return
			if win is getattr(s, "original", None):
				self.stop_session(s, silent=True, show_original=False)
			else:
				self.stop_session(s, silent=True, show_original=True)
	
		def _double_click_eval(self, clicked_win, toggle_target):
			try:
				mx, my = _mouse_pos()
	
				root = toggle_target
				gx, gy = _win_global_pos(root)
				w, h = _win_size(root)
	
				lx = mx - gx
				ly = my - gy
	
				if lx < 0 or ly < 0:
					return
				if w > 0 and lx > w:
					return
				if h > 0 and ly > h:
					return
	
				try:
					is_title = False
					n = root.__class__.__name__.lower()
					if "titlebar" in n or isinstance(root, ui.TitleBar):
						is_title = True
				except:
					is_title = False
	
				if is_title and ly > TITLEBAR_H:
					return
	
				key = id(root)
				now = _now()
				t, pmx, pmy, cnt = self._click_state.get(key, (0.0, mx, my, 0))
	
				if (now - t) <= DOUBLE_CLICK_SEC and abs(mx - pmx) <= DOUBLE_CLICK_DIST and abs(my - pmy) <= DOUBLE_CLICK_DIST:
					cnt += 1
				else:
					cnt = 1
	
				self._click_state[key] = (now, mx, my, cnt)
	
				if cnt >= 2:
					self._click_state[key] = (0.0, mx, my, 0)
					self.toggle_for_window(root)
			except:
				pass
	
	
	class _DummySession(object):
		def __init__(self, filepath):
			self.py_filepath = filepath
	
	
	def _mgr():
		global _MGR
		if _MGR is None:
			_MGR = _FWManager()
		return _MGR
	
	
	def configure(root_path=None, uiscript_path=None):
		global ROOT_PATH, UISCRIPT_PATH
		if root_path is not None:
			ROOT_PATH = root_path
		if uiscript_path is not None:
			UISCRIPT_PATH = uiscript_path
		_ensure_syspath()
	
	
	def set_active_window(win):
		_mgr().set_active(win)
	
	def get_active_window():
		return _mgr().get_active()
	
	def toggle_for_window(win):
		_mgr().toggle_for_window(win)
	
	def toggle_from_active():
		_mgr().toggle_for_window(_mgr().get_active())
	
	def tick():
		try:
			_hub().poll()
		except:
			pass
		try:
			_mgr().tick()
		except:
			pass
	
	def auto_update():
		tick()
	
	
	def _patch_constinfo():
		global _CONSTINFO_ORIG_SET
		if not constInfo or not hasattr(constInfo, "SetInstance") or _CONSTINFO_ORIG_SET:
			return
		_CONSTINFO_ORIG_SET = constInfo.SetInstance
	
		def _SafeSetInstance(key, obj):
			try:
				if obj is not None:
					try:
						setattr(obj, "_fw_instance_key", key)
					except:
						pass
				return _CONSTINFO_ORIG_SET(key, obj)
			except KeyError:
				return
			except:
				try:
					return _CONSTINFO_ORIG_SET(key, obj)
				except:
					return
	
		try:
			constInfo.SetInstance = _SafeSetInstance
		except:
			pass
	
	def install():
		global _ORIG_LOAD, _ORIG_MOUSE_DOWN, _ORIG_DESTROY
		global _ORIG_MOUSE_DOWN_TITLE, _ORIG_MOUSE_DOWN_BWTB
	
		if not ui:
			return False
	
		_ensure_syspath()
		m = _mgr()
	
		_patch_constinfo()
		_fw_patch_uidel()
	
		try:
			if hasattr(ui, "PythonScriptLoader") and hasattr(ui.PythonScriptLoader, "LoadScriptFile"):
				if _ORIG_LOAD is None:
					_ORIG_LOAD = ui.PythonScriptLoader.LoadScriptFile
	
					def _FW_LoadScriptFile(self, wnd, filename):
						resolved = None
						try:
							resolved = _resolve_uipath(filename)
						except:
							pass
	
						if wnd:
							try:
								if not hasattr(wnd, "_fw_uipaths"):
									wnd._fw_uipaths = []
								path = resolved if resolved else filename
								wnd._fw_uipaths.append(path)
								wnd._fw_last_uipath = path
							except:
								pass
	
						if resolved:
							try:
								if hasattr(self, "LoadScript"):
									w, _ = _load_uiscript_windowdict(resolved)
									if w:
										return self.LoadScript(wnd, w)
							except:
								pass
							try:
								return _ORIG_LOAD(self, wnd, resolved)
							except:
								pass
	
						return _ORIG_LOAD(self, wnd, filename)
	
					ui.PythonScriptLoader.LoadScriptFile = _FW_LoadScriptFile
		except:
			pass
	
		try:
			if hasattr(ui, "TitleBar") and hasattr(ui.TitleBar, "OnMouseLeftButtonDown"):
				if _ORIG_MOUSE_DOWN_TITLE is None:
					_ORIG_MOUSE_DOWN_TITLE = ui.TitleBar.OnMouseLeftButtonDown
	
					def _FW_TitleBar_MouseDown(self, *args):
						try:
							if m and _fw_guard(self):
								m.on_mouse_down(self)
						except:
							pass
						try:
							return _ORIG_MOUSE_DOWN_TITLE(self, *args)
						except:
							try:
								return _ORIG_MOUSE_DOWN_TITLE(self)
							except:
								return
	
					ui.TitleBar.OnMouseLeftButtonDown = _FW_TitleBar_MouseDown
		except:
			pass
	
		try:
			if hasattr(ui, "BoardWithTitleBar") and hasattr(ui.BoardWithTitleBar, "OnMouseLeftButtonDown"):
				if _ORIG_MOUSE_DOWN_BWTB is None:
					_ORIG_MOUSE_DOWN_BWTB = ui.BoardWithTitleBar.OnMouseLeftButtonDown
	
					def _FW_BWTB_MouseDown(self, *args):
						try:
							if m and _fw_guard(self):
								m.on_mouse_down(self)
						except:
							pass
						try:
							return _ORIG_MOUSE_DOWN_BWTB(self, *args)
						except:
							try:
								return _ORIG_MOUSE_DOWN_BWTB(self)
							except:
								return
	
					ui.BoardWithTitleBar.OnMouseLeftButtonDown = _FW_BWTB_MouseDown
		except:
			pass
	
		try:
			if hasattr(ui, "ScriptWindow"):
	
				if _ORIG_MOUSE_DOWN is None and hasattr(ui.ScriptWindow, "OnMouseLeftButtonDown"):
					_ORIG_MOUSE_DOWN = ui.ScriptWindow.OnMouseLeftButtonDown
	
					def _FW_MouseDown(self, *args):
						try:
							if m and _fw_guard(self):
								m.on_mouse_down(self)
						except:
							pass
						try:
							return _ORIG_MOUSE_DOWN(self, *args)
						except:
							try:
								return _ORIG_MOUSE_DOWN(self)
							except:
								return
	
					ui.ScriptWindow.OnMouseLeftButtonDown = _FW_MouseDown
	
				if _ORIG_DESTROY is None and hasattr(ui.ScriptWindow, "Destroy"):
					_ORIG_DESTROY = ui.ScriptWindow.Destroy
	
					def _FW_Destroy(self):
						try:
							if getattr(self, "_fw_internal_destroy", False):
								return _ORIG_DESTROY(self)
						except:
							pass
	
						try:
							if m:
								m.on_destroy(self)
						except:
							pass
	
						return _ORIG_DESTROY(self)
	
					ui.ScriptWindow.Destroy = _FW_Destroy
		except:
			pass
	
		return True
	
	def bootstrap():
		return install()