#include "StdAfx.h"
#include "FratelloSecurity.h"

#ifdef ENABLE_FRATELLO_SECURITY

#include "Locale.h"
#include "PythonApplication.h"
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"
#include "PythonPlayerEventHandler.h"
#include "../EterBase/Timer.h"
#include "../ScriptLib/PythonLauncher.h"

#include <TlHelp32.h>
#include <Psapi.h>
#include <intrin.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <deque>
#include <limits>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace
{
	constexpr uint64_t kFastScanIntervalMs = 2500;
	constexpr uint64_t kMediumScanIntervalMs = 7500;
	constexpr uint64_t kFullScanIntervalMs = 20000;
	constexpr uint64_t kFastScanEarlyIntervalMs = 650;
	constexpr uint64_t kMediumScanEarlyIntervalMs = 1800;
	constexpr uint64_t kFullScanEarlyIntervalMs = 4500;
	constexpr uint64_t kEarlyProtectionWindowMs = 90000;
	constexpr uint64_t kAdaptiveProtectionWindowMs = 45000;
	constexpr uint64_t kThreadScanIntervalMs = 5000;
	constexpr uint64_t kPopupDisplayMs = 4500;
	constexpr size_t kLogCapacity = 256;
	constexpr size_t kCriticalRegionFastSize = 48;
	constexpr size_t kCriticalRegionFullSize = 96;
	constexpr size_t kMemoryMediumRegionBudget = 420;
	constexpr int kSuspiciousScoreDelta = 8;
	constexpr int kHighScoreDelta = 25;
	constexpr int kCriticalScoreDelta = 100;
	constexpr int kHighScoreCloseThreshold = 80;
	constexpr int kSuspiciousCloseThreshold = 130;
	constexpr int kUnknownModuleConfirmHits = 2;
	constexpr int kProxyModuleConfirmHits = 2;
	constexpr int kSuspiciousThreadConfirmHits = 2;
	constexpr int kTimingAnomalyConfirmHits = 4;
	constexpr int kTimingCriticalConfirmHits = 3;
	constexpr uint64_t kTimingWarmupMs = 12000;
	constexpr uint64_t kTimingUiGraceMs = 4000;
	constexpr double kTimingSoftRatioLow = 0.78;
	constexpr double kTimingSoftRatioHigh = 1.28;
	constexpr double kTimingHardRatioLow = 0.55;
	constexpr double kTimingHardRatioHigh = 1.75;
	constexpr double kTimingFrameHardRatio = 1.70;
	constexpr int kBotAnomalyConfirmHits = 6;
	constexpr int kTimingHistoryWindow = 32;
	constexpr int kBotIntervalWindow = 48;
	constexpr bool kEnableInternalSelfTests = false;
	constexpr bool kInternalSelfTestCriticalClose = false;
	constexpr bool kInternalSelfTestTimingWarp = false;

	struct CScopedHandle
	{
		HANDLE handle;

		explicit CScopedHandle(HANDLE h = INVALID_HANDLE_VALUE)
			: handle(h)
		{
		}

		~CScopedHandle()
		{
			if (handle != INVALID_HANDLE_VALUE && handle != nullptr)
				CloseHandle(handle);
		}

		CScopedHandle(const CScopedHandle&) = delete;
		CScopedHandle& operator=(const CScopedHandle&) = delete;

		CScopedHandle(CScopedHandle&& rhs) noexcept
			: handle(rhs.handle)
		{
			rhs.handle = INVALID_HANDLE_VALUE;
		}

		CScopedHandle& operator=(CScopedHandle&& rhs) noexcept
		{
			if (this != &rhs)
			{
				if (handle != INVALID_HANDLE_VALUE && handle != nullptr)
					CloseHandle(handle);
				handle = rhs.handle;
				rhs.handle = INVALID_HANDLE_VALUE;
			}
			return *this;
		}

		bool IsValid() const
		{
			return handle != INVALID_HANDLE_VALUE && handle != nullptr;
		}
	};

	inline uint64_t GetMsTick()
	{
		return static_cast<uint64_t>(GetTickCount64());
	}

	inline uint64_t FileTimeToMs(const FILETIME& ft)
	{
		ULARGE_INTEGER value{};
		value.LowPart = ft.dwLowDateTime;
		value.HighPart = ft.dwHighDateTime;
		return value.QuadPart / 10000ull;
	}

	inline uint64_t GetSystemTimeMs()
	{
		FILETIME ft{};
		GetSystemTimeAsFileTime(&ft);
		return FileTimeToMs(ft);
	}

	inline uint64_t GetPreciseSystemTimeMs()
	{
		using GetSystemTimePreciseAsFileTimeFn = VOID(WINAPI*)(LPFILETIME);
		static const GetSystemTimePreciseAsFileTimeFn fn =
			reinterpret_cast<GetSystemTimePreciseAsFileTimeFn>(
				GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetSystemTimePreciseAsFileTime"));

		FILETIME ft{};
		if (fn)
			fn(&ft);
		else
			GetSystemTimeAsFileTime(&ft);
		return FileTimeToMs(ft);
	}

	inline uint64_t GetUnbiasedInterruptMs()
	{
		using QueryUnbiasedInterruptTimeFn = BOOL(WINAPI*)(PULONGLONG);
		static const QueryUnbiasedInterruptTimeFn fn =
			reinterpret_cast<QueryUnbiasedInterruptTimeFn>(
				GetProcAddress(GetModuleHandleA("kernel32.dll"), "QueryUnbiasedInterruptTime"));

		ULONGLONG time100ns = 0;
		if (fn && fn(&time100ns))
			return static_cast<uint64_t>(time100ns / 10000ull);
		return 0;
	}

	inline uint64_t HashMemoryFNV1a64(const uint8_t* data, size_t length)
	{
		constexpr uint64_t kOffset = 14695981039346656037ull;
		constexpr uint64_t kPrime = 1099511628211ull;

		uint64_t hash = kOffset;
		for (size_t i = 0; i < length; ++i)
		{
			hash ^= static_cast<uint64_t>(data[i]);
			hash *= kPrime;
		}
		return hash;
	}

	inline uint64_t RandomU64()
	{
		static std::atomic<uint64_t> counter{ 0x9E3779B97F4A7C15ull };
		LARGE_INTEGER qpc{};
		QueryPerformanceCounter(&qpc);

		uint64_t x = static_cast<uint64_t>(qpc.QuadPart);
		x ^= (static_cast<uint64_t>(GetCurrentProcessId()) << 16ull);
		x ^= (static_cast<uint64_t>(GetCurrentThreadId()) << 32ull);
		x ^= static_cast<uint64_t>(__rdtsc());
		x ^= counter.fetch_add(0x9E3779B97F4A7C15ull, std::memory_order_relaxed);

		x ^= (x >> 12);
		x ^= (x << 25);
		x ^= (x >> 27);
		return x * 0x2545F4914F6CDD1Dull;
	}

	inline std::string ToLowerAscii(std::string text)
	{
		for (char& ch : text)
			ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
		return text;
	}

	inline void ReplaceBackslashes(std::string& path)
	{
		for (char& ch : path)
		{
			if (ch == '\\')
				ch = '/';
		}
	}

	inline std::string NormalizePath(const char* rawPath)
	{
		if (!rawPath || !rawPath[0])
			return std::string();

		char fullPath[MAX_PATH * 2] = {};
		DWORD resolvedLen = GetFullPathNameA(rawPath, static_cast<DWORD>(sizeof(fullPath)), fullPath, nullptr);
		std::string result;
		if (resolvedLen > 0 && resolvedLen < sizeof(fullPath))
			result.assign(fullPath);
		else
			result.assign(rawPath);

		result = ToLowerAscii(result);
		ReplaceBackslashes(result);
		return result;
	}

	inline std::string DirectoryFromPath(const std::string& normalizedPath)
	{
		if (normalizedPath.empty())
			return std::string();

		const size_t pos = normalizedPath.find_last_of('/');
		if (pos == std::string::npos)
			return std::string();
		return normalizedPath.substr(0, pos + 1);
	}

	inline bool StartsWith(const std::string& value, const std::string& prefix)
	{
		return value.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), value.begin());
	}

	inline std::string BaseNameFromPath(const std::string& normalizedPath)
	{
		if (normalizedPath.empty())
			return std::string();

		const size_t pos = normalizedPath.find_last_of('/');
		if (pos == std::string::npos)
			return normalizedPath;

		if (pos + 1 >= normalizedPath.size())
			return std::string();
		return normalizedPath.substr(pos + 1);
	}

	inline bool IsExecutableProtection(DWORD protection)
	{
		const DWORD mask = protection & 0xFFu;
		switch (mask)
		{
		case PAGE_EXECUTE:
		case PAGE_EXECUTE_READ:
		case PAGE_EXECUTE_READWRITE:
		case PAGE_EXECUTE_WRITECOPY:
			return true;
		default:
			return false;
		}
	}

	inline bool IsWritableProtection(DWORD protection)
	{
		const DWORD mask = protection & 0xFFu;
		switch (mask)
		{
		case PAGE_READWRITE:
		case PAGE_WRITECOPY:
		case PAGE_EXECUTE_READWRITE:
		case PAGE_EXECUTE_WRITECOPY:
			return true;
		default:
			return false;
		}
	}

	inline bool IsRwxProtection(DWORD protection)
	{
		return IsExecutableProtection(protection) && IsWritableProtection(protection);
	}

	template <typename TMemberFn>
	const void* MemberFunctionAddress(TMemberFn memberFn)
	{
		if (sizeof(memberFn) != sizeof(void*))
			return nullptr;

		void* address = nullptr;
		std::memcpy(&address, &memberFn, sizeof(address));
		return address;
	}

	inline bool ReadFileHash(const std::string& normalizedPath, uint64_t& outHash, uint64_t& outSize)
	{
		outHash = 0;
		outSize = 0;

		CScopedHandle file(CreateFileA(normalizedPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
		if (!file.IsValid())
			return false;

		std::array<uint8_t, 64 * 1024> buffer{};
		uint64_t hash = 14695981039346656037ull;
		constexpr uint64_t kPrime = 1099511628211ull;

		while (true)
		{
			DWORD bytesRead = 0;
			if (!ReadFile(file.handle, buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, nullptr))
				return false;
			if (bytesRead == 0)
				break;

			outSize += bytesRead;
			for (DWORD i = 0; i < bytesRead; ++i)
			{
				hash ^= buffer[i];
				hash *= kPrime;
			}
		}

		outHash = hash;
		return true;
	}

	const char* ViolationTypeName(CFratelloSecurity::EViolationType type)
	{
		using EViolationType = CFratelloSecurity::EViolationType;
		switch (type)
		{
		case EViolationType::ModuleInjected: return "ModuleInjected";
		case EViolationType::UnknownModuleLoaded: return "UnknownModuleLoaded";
		case EViolationType::ManualMapSuspected: return "ManualMapSuspected";
		case EViolationType::ExecutablePrivateMemory: return "ExecutablePrivateMemory";
		case EViolationType::RwxMemoryDetected: return "RwxMemoryDetected";
		case EViolationType::SuspiciousThread: return "SuspiciousThread";
		case EViolationType::DebuggerDetected: return "DebuggerDetected";
		case EViolationType::CriticalRegionModified: return "CriticalRegionModified";
		case EViolationType::InlineHookDetected: return "InlineHookDetected";
		case EViolationType::ImportHookDetected: return "ImportHookDetected";
		case EViolationType::PythonTamperDetected: return "PythonTamperDetected";
		case EViolationType::TimingManipulationDetected: return "TimingManipulationDetected";
		case EViolationType::BotBehaviorDetected: return "BotBehaviorDetected";
		case EViolationType::HoneypotModified: return "HoneypotModified";
		case EViolationType::FileIntegrityFailed: return "FileIntegrityFailed";
		case EViolationType::SectionIntegrityFailed: return "SectionIntegrityFailed";
		case EViolationType::FratelloSelfTamperDetected: return "FratelloSelfTamperDetected";
		default: return "Unknown";
		}
	}

	inline bool ValidateImageHeader(uintptr_t baseAddress)
	{
		if (!baseAddress)
			return false;

		const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(baseAddress);
		if (!dos || dos->e_magic != IMAGE_DOS_SIGNATURE)
			return false;

		const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(baseAddress + static_cast<uintptr_t>(dos->e_lfanew));
		if (!nt || nt->Signature != IMAGE_NT_SIGNATURE)
			return false;

		return true;
	}

	struct SHideProcessWindowContext
	{
		DWORD processId = 0;
		HWND excludeWindow = nullptr;
	};

	BOOL CALLBACK HideProcessWindowsEnumProc(HWND hwnd, LPARAM lParam)
	{
		auto* ctx = reinterpret_cast<SHideProcessWindowContext*>(lParam);
		if (!ctx || !hwnd)
			return TRUE;

		DWORD windowPid = 0;
		GetWindowThreadProcessId(hwnd, &windowPid);
		if (windowPid != ctx->processId)
			return TRUE;

		if (hwnd == ctx->excludeWindow)
			return TRUE;

		if (!IsWindowVisible(hwnd))
			return TRUE;

		ShowWindow(hwnd, SW_HIDE);
		return TRUE;
	}

	struct SSecurityPopupModel
	{
		const char* title = nullptr;
		const char* line1 = nullptr;
		const char* line2 = nullptr;
		const char* line3 = nullptr;
		const char* action = nullptr;
		DWORD timeoutMs = 0;
	};

	struct SSecurityPopupState
	{
		SSecurityPopupModel model;
		uint64_t shownAt = 0;
		HFONT hTitleFont = nullptr;
		HFONT hBodyFont = nullptr;
		HFONT hActionFont = nullptr;
	};

	void FillSolidRect(HDC hdc, const RECT& rc, COLORREF color)
	{
		HBRUSH brush = CreateSolidBrush(color);
		FillRect(hdc, &rc, brush);
		DeleteObject(brush);
	}

	void FillVerticalGradient(HDC hdc, const RECT& rc, COLORREF top, COLORREF bottom)
	{
		const int height = rc.bottom > rc.top ? static_cast<int>(rc.bottom - rc.top) : 1;
		for (int y = 0; y < height; ++y)
		{
			const double t = static_cast<double>(y) / static_cast<double>(height);
			const BYTE r = static_cast<BYTE>(GetRValue(top) + (GetRValue(bottom) - GetRValue(top)) * t);
			const BYTE g = static_cast<BYTE>(GetGValue(top) + (GetGValue(bottom) - GetGValue(top)) * t);
			const BYTE b = static_cast<BYTE>(GetBValue(top) + (GetBValue(bottom) - GetBValue(top)) * t);
			HPEN pen = CreatePen(PS_SOLID, 1, RGB(r, g, b));
			HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
			MoveToEx(hdc, rc.left, rc.top + y, nullptr);
			LineTo(hdc, rc.right, rc.top + y);
			SelectObject(hdc, oldPen);
			DeleteObject(pen);
		}
	}

	void StrokeRoundRect(HDC hdc, const RECT& rc, int radius, COLORREF color, int width)
	{
		HPEN pen = CreatePen(PS_SOLID, width, color);
		HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)));
		HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
		RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, radius, radius);
		SelectObject(hdc, oldPen);
		SelectObject(hdc, oldBrush);
		DeleteObject(pen);
	}

	void DrawShieldMark(HDC hdc, const RECT& rc, COLORREF accent, COLORREF darkAccent)
	{
		POINT shield[6] =
		{
			{ (rc.left + rc.right) / 2, rc.top },
			{ rc.right, rc.top + 14 },
			{ rc.right - 6, rc.top + ((rc.bottom - rc.top) * 3) / 5 },
			{ (rc.left + rc.right) / 2, rc.bottom },
			{ rc.left + 6, rc.top + ((rc.bottom - rc.top) * 3) / 5 },
			{ rc.left, rc.top + 14 }
		};

		HBRUSH fill = CreateSolidBrush(RGB(42, 9, 15));
		HPEN glowPen = CreatePen(PS_SOLID, 5, RGB(86, 18, 27));
		HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, glowPen));
		HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, fill));
		Polygon(hdc, shield, 6);
		SelectObject(hdc, oldPen);
		SelectObject(hdc, oldBrush);
		DeleteObject(glowPen);
		DeleteObject(fill);

		HPEN accentPen = CreatePen(PS_SOLID, 2, accent);
		HBRUSH innerFill = CreateSolidBrush(darkAccent);
		oldPen = static_cast<HPEN>(SelectObject(hdc, accentPen));
		oldBrush = static_cast<HBRUSH>(SelectObject(hdc, innerFill));
		Polygon(hdc, shield, 6);
		SelectObject(hdc, oldPen);
		SelectObject(hdc, oldBrush);
		DeleteObject(accentPen);
		DeleteObject(innerFill);

		HPEN linePen = CreatePen(PS_SOLID, 3, RGB(255, 70, 82));
		oldPen = static_cast<HPEN>(SelectObject(hdc, linePen));
		MoveToEx(hdc, (rc.left + rc.right) / 2, rc.top + 16, nullptr);
		LineTo(hdc, (rc.left + rc.right) / 2, rc.bottom - 18);
		MoveToEx(hdc, rc.left + 21, rc.top + 36, nullptr);
		LineTo(hdc, (rc.left + rc.right) / 2, rc.top + 52);
		LineTo(hdc, rc.right - 21, rc.top + 36);
		SelectObject(hdc, oldPen);
		DeleteObject(linePen);
	}

	LRESULT CALLBACK SecurityShieldPopupProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_NCCREATE)
		{
			const CREATESTRUCTA* createStruct = reinterpret_cast<const CREATESTRUCTA*>(lParam);
			SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
			return TRUE;
		}

		auto* state = reinterpret_cast<SSecurityPopupState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));
		switch (message)
		{
		case WM_CREATE:
		{
			if (!state)
				return -1;

			LOGFONTA titleFont{};
			titleFont.lfHeight = -38;
			titleFont.lfWeight = FW_HEAVY;
			titleFont.lfQuality = CLEARTYPE_QUALITY;
			titleFont.lfCharSet = DEFAULT_CHARSET;
			strncpy_s(titleFont.lfFaceName, "Bahnschrift", _TRUNCATE);
			state->hTitleFont = CreateFontIndirectA(&titleFont);

			LOGFONTA bodyFont{};
			bodyFont.lfHeight = -20;
			bodyFont.lfWeight = FW_MEDIUM;
			bodyFont.lfQuality = CLEARTYPE_QUALITY;
			bodyFont.lfCharSet = DEFAULT_CHARSET;
			strncpy_s(bodyFont.lfFaceName, "Segoe UI", _TRUNCATE);
			state->hBodyFont = CreateFontIndirectA(&bodyFont);

			LOGFONTA actionFont{};
			actionFont.lfHeight = -17;
			actionFont.lfWeight = FW_SEMIBOLD;
			actionFont.lfQuality = CLEARTYPE_QUALITY;
			actionFont.lfCharSet = DEFAULT_CHARSET;
			strncpy_s(actionFont.lfFaceName, "Bahnschrift", _TRUNCATE);
			state->hActionFont = CreateFontIndirectA(&actionFont);

			state->shownAt = GetMsTick();
			SetTimer(hwnd, 1, 40, nullptr);
			return 0;
		}
		case WM_TIMER:
		{
			if (!state)
				return 0;

			if (GetMsTick() - state->shownAt >= state->model.timeoutMs)
			{
				KillTimer(hwnd, 1);
				DestroyWindow(hwnd);
				return 0;
			}

			InvalidateRect(hwnd, nullptr, FALSE);
			return 0;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_KEYDOWN:
			DestroyWindow(hwnd);
			return 0;
		case WM_ERASEBKGND:
			return 1;
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rc{};
			GetClientRect(hwnd, &rc);

			const COLORREF bgTop = RGB(5, 6, 10);
			const COLORREF bgBottom = RGB(18, 4, 8);
			const COLORREF panelTop = RGB(20, 21, 27);
			const COLORREF panelBottom = RGB(10, 12, 18);
			const COLORREF red = RGB(236, 32, 54);
			const COLORREF redDeep = RGB(82, 8, 18);
			const COLORREF redSoft = RGB(148, 20, 34);
			const COLORREF textMain = RGB(248, 250, 252);
			const COLORREF textSub = RGB(190, 198, 210);
			const COLORREF textMuted = RGB(115, 126, 145);

			FillVerticalGradient(hdc, rc, bgTop, bgBottom);

			RECT outer = rc;
			InflateRect(&outer, -14, -14);
			StrokeRoundRect(hdc, outer, 28, RGB(68, 11, 19), 2);

			RECT panel = outer;
			InflateRect(&panel, -18, -18);
			FillVerticalGradient(hdc, panel, panelTop, panelBottom);
			StrokeRoundRect(hdc, panel, 22, RGB(122, 18, 31), 2);

			RECT header = { panel.left, panel.top, panel.right, panel.top + 86 };
			FillVerticalGradient(hdc, header, RGB(38, 10, 16), RGB(18, 18, 24));

			HPEN redPen = CreatePen(PS_SOLID, 2, red);
			HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, redPen));
			MoveToEx(hdc, panel.left + 28, header.bottom - 2, nullptr);
			LineTo(hdc, panel.right - 28, header.bottom - 2);
			SelectObject(hdc, oldPen);
			DeleteObject(redPen);

			RECT shieldRect = { panel.left + 34, panel.top + 28, panel.left + 104, panel.top + 106 };
			DrawShieldMark(hdc, shieldRect, red, redDeep);

			RECT pill = { panel.right - 190, panel.top + 32, panel.right - 34, panel.top + 58 };
			FillSolidRect(hdc, pill, RGB(38, 9, 16));
			StrokeRoundRect(hdc, pill, 16, redSoft, 1);

			SetBkMode(hdc, TRANSPARENT);
			HFONT oldFont = nullptr;
			if (state && state->hActionFont)
				oldFont = static_cast<HFONT>(SelectObject(hdc, state->hActionFont));
			SetTextColor(hdc, RGB(255, 96, 108));
			DrawTextA(hdc, "CRITICAL SECURITY EVENT", -1, &pill, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

			RECT titleRect = { panel.left + 126, panel.top + 30, panel.right - 210, panel.top + 78 };
			if (state && state->hTitleFont)
				SelectObject(hdc, state->hTitleFont);
			SetTextColor(hdc, textMain);
			DrawTextA(hdc, state && state->model.title ? state->model.title : "Security Shield", -1, &titleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

			RECT bodyPanel = { panel.left + 34, panel.top + 124, panel.right - 34, panel.bottom - 74 };
			FillSolidRect(hdc, bodyPanel, RGB(13, 15, 22));
			StrokeRoundRect(hdc, bodyPanel, 18, RGB(50, 55, 68), 1);

			RECT bodyRect = { bodyPanel.left + 28, bodyPanel.top + 26, bodyPanel.right - 28, bodyPanel.bottom - 26 };
			if (state && state->hBodyFont)
				SelectObject(hdc, state->hBodyFont);
			SetTextColor(hdc, textSub);

			std::string bodyText;
			bodyText += state && state->model.line1 ? state->model.line1 : "Unauthorized modification detected.";
			bodyText += "\n";
			bodyText += state && state->model.line2 ? state->model.line2 : "The client will now close to protect the game environment.";
			if (state && state->model.line3 && state->model.line3[0])
			{
				bodyText += "\n";
				bodyText += state->model.line3;
			}
			DrawTextA(hdc, bodyText.c_str(), -1, &bodyRect, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);

			RECT footerRect = { panel.left + 34, panel.bottom - 50, panel.right - 34, panel.bottom - 24 };
			if (state && state->hActionFont)
				SelectObject(hdc, state->hActionFont);
			SetTextColor(hdc, textMuted);
			DrawTextA(hdc, state && state->model.action ? state->model.action : "Secured session termination in progress", -1, &footerRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

			HPEN scanPen = CreatePen(PS_SOLID, 1, RGB(70, 16, 23));
			oldPen = static_cast<HPEN>(SelectObject(hdc, scanPen));
			for (int y = panel.top + 10; y < panel.bottom; y += 18)
			{
				MoveToEx(hdc, panel.left + 12, y, nullptr);
				LineTo(hdc, panel.right - 12, y);
			}
			SelectObject(hdc, oldPen);
			DeleteObject(scanPen);

			if (oldFont)
				SelectObject(hdc, oldFont);

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_DESTROY:
		{
			if (state)
			{
				if (state->hTitleFont)
				{
					DeleteObject(state->hTitleFont);
					state->hTitleFont = nullptr;
				}
				if (state->hBodyFont)
				{
					DeleteObject(state->hBodyFont);
					state->hBodyFont = nullptr;
				}
				if (state->hActionFont)
				{
					DeleteObject(state->hActionFont);
					state->hActionFont = nullptr;
				}
			}
			PostQuitMessage(0);
			return 0;
		}
		default:
			break;
		}

		return DefWindowProcA(hwnd, message, wParam, lParam);
	}

	bool ShowShieldPopupWindow(const SSecurityPopupModel& model)
	{
		const char* kWindowClass = "SecurityShieldPopupClass2026";
		HINSTANCE instance = GetModuleHandleA(nullptr);

		WNDCLASSEXA wndClass{};
		wndClass.cbSize = sizeof(wndClass);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = &SecurityShieldPopupProc;
		wndClass.hInstance = instance;
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.lpszClassName = kWindowClass;
		RegisterClassExA(&wndClass);

		const int width = 760;
		const int height = 360;
		const int screenW = GetSystemMetrics(SM_CXSCREEN);
		const int screenH = GetSystemMetrics(SM_CYSCREEN);
		const int x = ((screenW - width) > 0) ? ((screenW - width) / 2) : 0;
		const int y = ((screenH - height) > 0) ? ((screenH - height) / 2) : 0;

		SSecurityPopupState state{};
		state.model = model;
		state.model.timeoutMs = (state.model.timeoutMs == 0) ? static_cast<DWORD>(kPopupDisplayMs) : state.model.timeoutMs;

		HWND hwnd = CreateWindowExA(
			WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
			kWindowClass,
			model.title ? model.title : "Security Shield",
			WS_POPUP,
			x, y, width, height,
			nullptr,
			nullptr,
			instance,
			&state);

		if (!hwnd)
			return false;

		HRGN windowRgn = CreateRoundRectRgn(0, 0, width, height, 30, 30);
		if (windowRgn)
		{
			if (!SetWindowRgn(hwnd, windowRgn, TRUE))
				DeleteObject(windowRgn);
		}

		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
		SetForegroundWindow(hwnd);

		MSG message{};
		while (GetMessageA(&message, nullptr, 0, 0) > 0)
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}

		return true;
	}

	DWORD WINAPI SecurityPopupThreadProc(LPVOID parameter)
	{
		SSecurityPopupModel* model = static_cast<SSecurityPopupModel*>(parameter);
		if (!model)
			return 0;

		const SSecurityPopupModel localModel = *model;
		delete model;
		ShowShieldPopupWindow(localModel);
		return 0;
	}

	bool ShowShieldPopupWindowThreaded(const SSecurityPopupModel& model)
	{
		SSecurityPopupModel* threadModel = nullptr;
		try
		{
			threadModel = new SSecurityPopupModel(model);
		}
		catch (...)
		{
			return false;
		}

		HANDLE threadHandle = CreateThread(nullptr, 0, &SecurityPopupThreadProc, threadModel, 0, nullptr);
		if (!threadHandle)
		{
			delete threadModel;
			return false;
		}

		const DWORD waitMs = (model.timeoutMs == 0 ? static_cast<DWORD>(kPopupDisplayMs) : model.timeoutMs) + 1800u;
		WaitForSingleObject(threadHandle, waitMs);
		CloseHandle(threadHandle);
		return true;
	}
}

struct CFratelloSecurity::SImpl
{
	struct SModuleInfo
	{
		uintptr_t baseAddress = 0;
		uint32_t imageSize = 0;
		std::string normalizedPath;
		bool trustedPath = false;
		bool validImage = false;
	};

	struct SCriticalRegion
	{
		std::string name;
		const uint8_t* address = nullptr;
		size_t size = 0;
		uint64_t hash = 0;
		bool fastScan = true;
	};

	struct SLogEntry
	{
		uint64_t timestampMs = 0;
		EViolationSeverity severity = EViolationSeverity::Info;
		EViolationType type = EViolationType::ModuleInjected;
		std::string message;
	};

	struct SSectionSnapshot
	{
		const uint8_t* address = nullptr;
		size_t size = 0;
		uint64_t hash = 0;
	};

	struct SFileSnapshot
	{
		std::string normalizedPath;
		uint64_t hash = 0;
		uint64_t size = 0;
		bool valid = false;
	};

	struct SIatSnapshot
	{
		uintptr_t thunkAddress = 0;
		uintptr_t functionAddress = 0;
		std::string importModule;
	};

	struct SApiSnapshot
	{
		std::string name;
		const uint8_t* address = nullptr;
		size_t size = 0;
		uint64_t hash = 0;
	};

	struct SCanaryState
	{
		uint64_t sessionKey = 0;
		std::array<uint64_t, 32> slots{};
		std::array<uint64_t, 32> shadows{};
		std::array<uint64_t, 8> groupChecksums{};
		uint64_t checksum = 0;
	};

	struct STimingState
	{
		LARGE_INTEGER frequency{};
		LARGE_INTEGER lastQpc{};
		uint64_t lastTick = 0;
		uint64_t lastTimeGetTime = 0;
		uint64_t lastSystemTime = 0;
		uint64_t lastPreciseSystemTime = 0;
		uint64_t lastUnbiasedInterrupt = 0;
		uint64_t lastClientTimer = 0;
		uint64_t lastFrameTimer = 0;
		uint64_t initializedAtMs = 0;
		uint64_t lastCriticalAtMs = 0;
		uint64_t lastUiGraceAtMs = 0;
		int anomalyCount = 0;
		int criticalCount = 0;
		int stableCount = 0;
		double suspicionScore = 0.0;
		std::deque<double> ratioHistory;
		std::deque<double> frameRatioHistory;
	};

	struct SBotState
	{
		bool lastLeftDown = false;
		uint64_t lastLeftDownTs = 0;
		int anomalyCount = 0;
		std::deque<uint64_t> intervalsMs;
		std::array<bool, 8> actionDown{};
		std::array<uint64_t, 8> actionLastTs{};
		std::deque<uint64_t> actionIntervalsMs;
	};

	struct SThreadRecord
	{
		DWORD threadId = 0;
		uintptr_t startAddress = 0;
	};

	std::mutex mutex;
	std::atomic<bool> healthy{ true };
	bool preStartInitialized = false;
	bool postStartInitialized = false;
	bool shutdown = false;
	bool closingRequested = false;
	bool popupShown = false;
	bool selfTestViolationFired = false;
	bool selfTestDiagnosticsRan = false;

	uint64_t lastFastScanAtMs = 0;
	uint64_t lastMediumScanAtMs = 0;
	uint64_t lastFullScanAtMs = 0;
	uint64_t lastThreadScanAtMs = 0;
	uint64_t adaptiveScanUntilMs = 0;
	uint64_t startupAtMs = 0;
	uint64_t frameCounter = 0;
	uintptr_t incrementalMemoryCursor = 0;
	int riskScore = 0;

	DWORD processId = 0;
	DWORD mainThreadId = 0;

	std::string processPath;
	std::string processDirectory;
	std::string systemDirectory;
	std::string syswow64Directory;
	std::string windowsDirectory;
	std::string winSxSDirectory;
	std::string currentDirectory;

	std::unordered_map<std::string, SModuleInfo> moduleBaseline;
	std::unordered_map<std::string, int> unknownModuleHits;
	std::unordered_map<std::string, int> proxyModuleHits;
	std::unordered_map<std::string, int> sectionAnomalyHits;
	std::unordered_set<DWORD> knownThreads;
	std::unordered_map<DWORD, int> suspiciousThreadHits;
	std::unordered_map<uintptr_t, int> privateExecHits;
	std::unordered_set<uintptr_t> executableMemoryBaseline;
	std::unordered_map<std::string, int> launchSurfaceHits;
	std::map<EViolationType, int> violationCounters;
	std::deque<SLogEntry> logs;

	std::vector<SCriticalRegion> criticalRegions;
	std::vector<SIatSnapshot> importSnapshots;
	std::vector<SApiSnapshot> timerApiSnapshots;
	SSectionSnapshot textSnapshot;
	SFileSnapshot executableSnapshot;
	SCanaryState canaryState;
	STimingState timingState;
	SBotState botState;

	void Log(EViolationSeverity severity, EViolationType type, const char* fmt, ...)
	{
		char buffer[512] = {};
		va_list args;
		va_start(args, fmt);
		_vsnprintf(buffer, sizeof(buffer) - 1, fmt, args);
		va_end(args);

		SLogEntry entry;
		entry.timestampMs = GetMsTick();
		entry.severity = severity;
		entry.type = type;
		entry.message = buffer;
		logs.emplace_back(std::move(entry));
		if (logs.size() > kLogCapacity)
			logs.pop_front();

		switch (severity)
		{
		case EViolationSeverity::Info:
			Tracenf("FratelloSecurity [INFO] %s", buffer);
			break;
		case EViolationSeverity::Suspicious:
			Tracenf("FratelloSecurity [SUSPICIOUS] %s", buffer);
			break;
		case EViolationSeverity::High:
			TraceError("FratelloSecurity [HIGH] %s", buffer);
			break;
		case EViolationSeverity::Critical:
			TraceError("FratelloSecurity [CRITICAL] %s", buffer);
			break;
		}
	}

	bool IsTrustedPath(const std::string& normalizedPath) const
	{
		if (normalizedPath.empty())
			return false;

		if (StartsWith(normalizedPath, processDirectory))
			return true;

		if (!systemDirectory.empty() && StartsWith(normalizedPath, systemDirectory))
			return true;

		if (!syswow64Directory.empty() && StartsWith(normalizedPath, syswow64Directory))
			return true;

		if (!winSxSDirectory.empty() && StartsWith(normalizedPath, winSxSDirectory))
			return true;

		return false;
	}

	std::string ModuleIdentity(const SModuleInfo& module) const
	{
		if (!module.normalizedPath.empty())
			return module.normalizedPath;

		char temp[64] = {};
		snprintf(temp, sizeof(temp), "0x%p", reinterpret_cast<void*>(module.baseAddress));
		return std::string(temp);
	}

	bool IsLikelyProxyTargetName(const std::string& moduleBaseName) const
	{
		if (moduleBaseName.empty())
			return false;

		static const char* const proxyNames[] =
		{
			"version.dll",
			"winmm.dll",
			"dinput8.dll",
			"dsound.dll",
			"d3d9.dll",
			"xinput1_3.dll",
			"xinput9_1_0.dll",
			"dbghelp.dll"
		};

		for (const char* name : proxyNames)
		{
			if (moduleBaseName == name)
				return true;
		}

		return false;
	}

	bool IsProxyHijackCandidate(const SModuleInfo& module) const
	{
		if (module.normalizedPath.empty() || processDirectory.empty() || systemDirectory.empty())
			return false;

		const std::string moduleName = BaseNameFromPath(module.normalizedPath);
		if (!IsLikelyProxyTargetName(moduleName))
			return false;

		if (!StartsWith(module.normalizedPath, processDirectory))
			return false;

		// Main executable itself may share the process directory and is not a proxy DLL.
		if (module.normalizedPath == processPath)
			return false;

		return true;
	}

	void ApplyProcessMitigationHardening()
	{
		PROCESS_MITIGATION_DEP_POLICY depPolicy{};
		depPolicy.Enable = 1;
		depPolicy.Permanent = 1;
		if (!SetProcessMitigationPolicy(ProcessDEPPolicy, &depPolicy, sizeof(depPolicy)))
		{
			Log(EViolationSeverity::Info, EViolationType::FratelloSelfTamperDetected,
				"DEP mitigation policy unavailable (%lu)", static_cast<unsigned long>(GetLastError()));
		}

		PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY extensionPolicy{};
		extensionPolicy.DisableExtensionPoints = 1;
		if (!SetProcessMitigationPolicy(ProcessExtensionPointDisablePolicy, &extensionPolicy, sizeof(extensionPolicy)))
		{
			Log(EViolationSeverity::Info, EViolationType::FratelloSelfTamperDetected,
				"Extension-point mitigation unavailable (%lu)", static_cast<unsigned long>(GetLastError()));
		}

		PROCESS_MITIGATION_IMAGE_LOAD_POLICY imagePolicy{};
		imagePolicy.NoRemoteImages = 1;
		imagePolicy.NoLowMandatoryLabelImages = 1;
		imagePolicy.PreferSystem32Images = 1;
		if (!SetProcessMitigationPolicy(ProcessImageLoadPolicy, &imagePolicy, sizeof(imagePolicy)))
		{
			Log(EViolationSeverity::Info, EViolationType::FratelloSelfTamperDetected,
				"Image-load mitigation unavailable (%lu)", static_cast<unsigned long>(GetLastError()));
		}
	}

	bool IsInEarlyProtectionWindow(uint64_t nowMs) const
	{
		if (startupAtMs == 0 || nowMs < startupAtMs)
			return false;
		return (nowMs - startupAtMs) <= kEarlyProtectionWindowMs;
	}

	bool CollectEnvironmentPaths()
	{
		char modulePath[MAX_PATH * 2] = {};
		if (!GetModuleFileNameA(nullptr, modulePath, static_cast<DWORD>(sizeof(modulePath))))
			return false;

		processPath = NormalizePath(modulePath);
		processDirectory = DirectoryFromPath(processPath);

		char sysPath[MAX_PATH] = {};
		if (GetSystemDirectoryA(sysPath, static_cast<UINT>(sizeof(sysPath))))
		{
			systemDirectory = NormalizePath(sysPath);
			if (!systemDirectory.empty() && systemDirectory.back() != '/')
				systemDirectory.push_back('/');
		}

		char winPath[MAX_PATH] = {};
		if (GetWindowsDirectoryA(winPath, static_cast<UINT>(sizeof(winPath))))
		{
			windowsDirectory = NormalizePath(winPath);
			if (!windowsDirectory.empty() && windowsDirectory.back() != '/')
				windowsDirectory.push_back('/');

			syswow64Directory = windowsDirectory + "syswow64/";
			winSxSDirectory = windowsDirectory + "winsxs/";
		}

		char cwd[MAX_PATH * 2] = {};
		if (GetCurrentDirectoryA(static_cast<DWORD>(sizeof(cwd)), cwd) > 0)
		{
			currentDirectory = NormalizePath(cwd);
			if (!currentDirectory.empty() && currentDirectory.back() != '/')
				currentDirectory.push_back('/');
		}

		return !processPath.empty() && !processDirectory.empty();
	}

	bool EnumerateModules(std::vector<SModuleInfo>& outModules)
	{
		outModules.clear();

		CScopedHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId));
		if (!snapshot.IsValid())
			return false;

		MODULEENTRY32 moduleEntry{};
		moduleEntry.dwSize = sizeof(moduleEntry);
		if (!Module32First(snapshot.handle, &moduleEntry))
			return false;

		do
		{
			SModuleInfo info;
			info.baseAddress = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
			info.imageSize = moduleEntry.modBaseSize;
			info.normalizedPath = NormalizePath(moduleEntry.szExePath);
			info.trustedPath = IsTrustedPath(info.normalizedPath);
			info.validImage = ValidateImageHeader(info.baseAddress);
			outModules.emplace_back(std::move(info));
		}
		while (Module32Next(snapshot.handle, &moduleEntry));

		return true;
	}

	const SModuleInfo* FindModuleForAddress(const std::vector<SModuleInfo>& modules, uintptr_t address) const
	{
		for (const SModuleInfo& module : modules)
		{
			const uintptr_t begin = module.baseAddress;
			const uintptr_t end = module.baseAddress + module.imageSize;
			if (address >= begin && address < end)
				return &module;
		}
		return nullptr;
	}

	bool SnapshotThreads(std::vector<SThreadRecord>& outThreads)
	{
		outThreads.clear();

		CScopedHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0));
		if (!snapshot.IsValid())
			return false;

		THREADENTRY32 entry{};
		entry.dwSize = sizeof(entry);
		if (!Thread32First(snapshot.handle, &entry))
			return false;

		using NtQueryInformationThreadFn = LONG(NTAPI*)(HANDLE, LONG, PVOID, ULONG, PULONG);
		static const NtQueryInformationThreadFn ntQueryInformationThread =
			reinterpret_cast<NtQueryInformationThreadFn>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread"));

		do
		{
			if (entry.th32OwnerProcessID != processId)
				continue;

			SThreadRecord record;
			record.threadId = entry.th32ThreadID;
			record.startAddress = 0;

			if (ntQueryInformationThread)
			{
				CScopedHandle threadHandle(OpenThread(THREAD_QUERY_INFORMATION, FALSE, entry.th32ThreadID));
				if (!threadHandle.IsValid())
					threadHandle = CScopedHandle(OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ThreadID));

				if (threadHandle.IsValid())
				{
					PVOID startAddr = nullptr;
					LONG ntStatus = ntQueryInformationThread(threadHandle.handle, 9, &startAddr, sizeof(startAddr), nullptr);
					if (ntStatus >= 0)
						record.startAddress = reinterpret_cast<uintptr_t>(startAddr);
				}
			}

			outThreads.emplace_back(record);
		}
		while (Thread32Next(snapshot.handle, &entry));

		return true;
	}

	void BuildModuleBaseline()
	{
		std::vector<SModuleInfo> modules;
		if (!EnumerateModules(modules))
			return;

		moduleBaseline.clear();
		for (const SModuleInfo& module : modules)
			moduleBaseline.emplace(ModuleIdentity(module), module);
	}

	void BuildThreadBaseline()
	{
		std::vector<SThreadRecord> threads;
		if (!SnapshotThreads(threads))
			return;

		knownThreads.clear();
		for (const SThreadRecord& thread : threads)
			knownThreads.insert(thread.threadId);
	}

	void BuildExecutableMemoryBaseline(const std::vector<SModuleInfo>& modules)
	{
		uintptr_t address = 0;
		while (true)
		{
			MEMORY_BASIC_INFORMATION mbi{};
			const SIZE_T queried = VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi));
			if (queried != sizeof(mbi))
				break;

			const uintptr_t regionBase = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
			const uintptr_t nextAddress = regionBase + mbi.RegionSize;
			if (nextAddress <= regionBase)
				break;

			const DWORD protection = mbi.Protect & (~PAGE_GUARD);
			if (mbi.State == MEM_COMMIT && IsExecutableProtection(protection))
			{
				const bool privateExec = (mbi.Type == MEM_PRIVATE);
				const bool rwx = IsRwxProtection(protection);
				const SModuleInfo* owner = FindModuleForAddress(modules, regionBase);
				if (privateExec || rwx || owner == nullptr)
					executableMemoryBaseline.insert(regionBase);
			}

			address = nextAddress;
		}
	}

	void InitializeCanary()
	{
		canaryState.sessionKey = RandomU64() ^ reinterpret_cast<uintptr_t>(this) ^ GetMsTick();
		for (size_t i = 0; i < canaryState.slots.size(); ++i)
		{
			const uint64_t v = RandomU64() ^ _rotl64(canaryState.sessionKey, static_cast<int>(i & 31)) ^
				(0x9E3779B97F4A7C15ull * (i + 1));
			canaryState.slots[i] = v;
			canaryState.shadows[i] = _rotr64(v ^ canaryState.sessionKey, static_cast<int>((i * 7) & 31));
		}

		for (size_t group = 0; group < canaryState.groupChecksums.size(); ++group)
		{
			const size_t begin = group * 4;
			canaryState.groupChecksums[group] = HashMemoryFNV1a64(
				reinterpret_cast<const uint8_t*>(&canaryState.slots[begin]), 4 * sizeof(uint64_t));
		}
		canaryState.checksum = HashMemoryFNV1a64(reinterpret_cast<const uint8_t*>(canaryState.slots.data()),
			canaryState.slots.size() * sizeof(uint64_t));
	}

	bool VerifyCanary()
	{
		const uint64_t currentChecksum = HashMemoryFNV1a64(reinterpret_cast<const uint8_t*>(canaryState.slots.data()),
			canaryState.slots.size() * sizeof(uint64_t));
		if (currentChecksum != canaryState.checksum)
			return false;

		for (size_t i = 0; i < canaryState.slots.size(); ++i)
		{
			const uint64_t expected = _rotr64(canaryState.slots[i] ^ canaryState.sessionKey, static_cast<int>((i * 7) & 31));
			if (expected != canaryState.shadows[i])
				return false;
		}

		for (size_t group = 0; group < canaryState.groupChecksums.size(); ++group)
		{
			const size_t begin = group * 4;
			const uint64_t groupChecksum = HashMemoryFNV1a64(
				reinterpret_cast<const uint8_t*>(&canaryState.slots[begin]), 4 * sizeof(uint64_t));
			if (groupChecksum != canaryState.groupChecksums[group])
				return false;
		}

		return true;
	}

	void RegisterCriticalRegion(const char* name, const void* address, size_t size, bool fastScan)
	{
		if (!name || !address || size == 0)
			return;

		const uint8_t* regionBegin = reinterpret_cast<const uint8_t*>(address);
		SCriticalRegion region;
		region.name = name;
		region.address = regionBegin;
		region.size = size;
		region.fastScan = fastScan;
		region.hash = HashMemoryFNV1a64(regionBegin, size);
		criticalRegions.emplace_back(std::move(region));
	}

	template <typename TMemberFn>
	void RegisterCriticalMember(const char* name, TMemberFn memberFn, size_t size, bool fastScan)
	{
		const void* address = MemberFunctionAddress(memberFn);
		if (!address)
		{
			Log(EViolationSeverity::Info, EViolationType::FratelloSelfTamperDetected,
				"Skip critical region '%s': member address not representable", name);
			return;
		}

		RegisterCriticalRegion(name, address, size, fastScan);
	}

	void RegisterOwnCriticalRegions()
	{
		RegisterCriticalMember("CFratelloSecurity::RunFastScan", &CFratelloSecurity::RunFastScan, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CFratelloSecurity::RunFullScan", &CFratelloSecurity::RunFullScan, kCriticalRegionFullSize, false);
		RegisterCriticalMember("CFratelloSecurity::Update", &CFratelloSecurity::Update, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CFratelloSecurity::TriggerViolation", &CFratelloSecurity::TriggerViolation, kCriticalRegionFastSize, true);
		RegisterCriticalRegion("ELTimer_GetMSec", reinterpret_cast<const void*>(&ELTimer_GetMSec), kCriticalRegionFastSize, true);
		RegisterCriticalRegion("ELTimer_SetFrameMSec", reinterpret_cast<const void*>(&ELTimer_SetFrameMSec), kCriticalRegionFastSize, true);
		RegisterCriticalRegion("ELTimer_GetFrameMSec", reinterpret_cast<const void*>(&ELTimer_GetFrameMSec), kCriticalRegionFastSize, true);
		RegisterCriticalRegion("ELTimer_GetServerMSec", reinterpret_cast<const void*>(&ELTimer_GetServerMSec), kCriticalRegionFastSize, true);
		RegisterCriticalMember("CTimer::Advance", &CTimer::Advance, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CTimer::Adjust", &CTimer::Adjust, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CTimer::GetCurrentSecond", &CTimer::GetCurrentSecond, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CTimer::GetCurrentMillisecond", &CTimer::GetCurrentMillisecond, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CTimer::GetElapsedMilliecond", &CTimer::GetElapsedMilliecond, kCriticalRegionFastSize, true);
	}

	void RegisterApplicationCriticalRegions()
	{
		RegisterCriticalMember("CPythonApplication::Loop", &CPythonApplication::Loop, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonApplication::Process", &CPythonApplication::Process, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonApplication::UpdateGame", &CPythonApplication::UpdateGame, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonApplication::RenderGame", &CPythonApplication::RenderGame, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonApplication::Create", &CPythonApplication::Create, kCriticalRegionFastSize, false);

		RegisterCriticalMember("CPythonLauncher::Create", &CPythonLauncher::Create, kCriticalRegionFullSize, false);
		RegisterCriticalMember("CPythonLauncher::RunLine", &CPythonLauncher::RunLine, kCriticalRegionFullSize, true);
		RegisterCriticalMember("CPythonLauncher::RunFile", &CPythonLauncher::RunFile, kCriticalRegionFullSize, true);
		RegisterCriticalMember("CPythonLauncher::RunMemoryTextFile", &CPythonLauncher::RunMemoryTextFile, kCriticalRegionFullSize, false);

		RegisterCriticalMember("CPythonNetworkStream::Process", &CPythonNetworkStream::Process, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonNetworkStream::ConnectLoginServer", &CPythonNetworkStream::ConnectLoginServer, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::ConnectGameServer", &CPythonNetworkStream::ConnectGameServer, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::SetLoginPhase", &CPythonNetworkStream::SetLoginPhase, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::SetSelectPhase", &CPythonNetworkStream::SetSelectPhase, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::SetLoadingPhase", &CPythonNetworkStream::SetLoadingPhase, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::SetGamePhase", &CPythonNetworkStream::SetGamePhase, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::SendSpecial", &CPythonNetworkStream::SendSpecial, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonNetworkStream::SendAttackPacket", &CPythonNetworkStream::SendAttackPacket, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonNetworkStream::SendCharacterStatePacket", &CPythonNetworkStream::SendCharacterStatePacket, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonNetworkStream::SendUseSkillPacket", &CPythonNetworkStream::SendUseSkillPacket, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonNetworkStream::SendItemPickUpPacket", &CPythonNetworkStream::SendItemPickUpPacket, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::SendFlyTargetingPacket", &CPythonNetworkStream::SendFlyTargetingPacket, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::SendShootPacket", &CPythonNetworkStream::SendShootPacket, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::ServerCommand", &CPythonNetworkStream::ServerCommand, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonNetworkStream::ClientCommand", &CPythonNetworkStream::ClientCommand, kCriticalRegionFastSize, false);

		RegisterCriticalMember("CPythonPlayer::Update", &CPythonPlayer::Update, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonPlayer::NEW_Attack", &CPythonPlayer::NEW_Attack, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonPlayer::NEW_MoveToDirection", &CPythonPlayer::NEW_MoveToDirection, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonPlayer::NEW_MoveToMouseScreenDirection", &CPythonPlayer::NEW_MoveToMouseScreenDirection, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonPlayer::NEW_SetMouseState", &CPythonPlayer::NEW_SetMouseState, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonPlayer::SetAttackKeyState", &CPythonPlayer::SetAttackKeyState, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonPlayer::ClickSkillSlot", &CPythonPlayer::ClickSkillSlot, kCriticalRegionFastSize, false);
		RegisterCriticalMember("CPythonPlayer::SendClickItemPacket", &CPythonPlayer::SendClickItemPacket, kCriticalRegionFastSize, false);

		RegisterCriticalMember("CPythonPlayerEventHandler::OnMove", &CPythonPlayerEventHandler::OnMove, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonPlayerEventHandler::OnAttack", &CPythonPlayerEventHandler::OnAttack, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonPlayerEventHandler::OnUseSkill", &CPythonPlayerEventHandler::OnUseSkill, kCriticalRegionFastSize, true);
		RegisterCriticalMember("CPythonPlayerEventHandler::OnHit", &CPythonPlayerEventHandler::OnHit, kCriticalRegionFastSize, true);
	}

	void SnapshotCriticalRegions()
	{
		for (SCriticalRegion& region : criticalRegions)
		{
			if (region.address && region.size)
				region.hash = HashMemoryFNV1a64(region.address, region.size);
		}
	}

	bool DecodeHookTarget(const uint8_t* address, uintptr_t& outTarget) const
	{
		outTarget = 0;
		if (!address)
			return false;

		const uint8_t op = address[0];
		if (op == 0xE9 || op == 0xE8)
		{
			const int32_t rel = *reinterpret_cast<const int32_t*>(address + 1);
			outTarget = reinterpret_cast<uintptr_t>(address + 5 + rel);
			return true;
		}

		// JMP [addr]
		if (op == 0xFF && address[1] == 0x25)
		{
			const uintptr_t ptr = *reinterpret_cast<const uintptr_t*>(address + 2);
			if (!ptr)
				return false;
			outTarget = *reinterpret_cast<const uintptr_t*>(ptr);
			return outTarget != 0;
		}

		return false;
	}

	bool VerifyCriticalRegions(bool fastOnly, const std::vector<SModuleInfo>& modules)
	{
		bool ok = true;
		for (const SCriticalRegion& region : criticalRegions)
		{
			if (fastOnly && !region.fastScan)
				continue;
			if (!region.address || region.size == 0)
				continue;

			const uint64_t currentHash = HashMemoryFNV1a64(region.address, region.size);
			if (currentHash == region.hash)
				continue;

			uintptr_t target = 0;
			if (DecodeHookTarget(region.address, target))
			{
				const SModuleInfo* owner = FindModuleForAddress(modules, reinterpret_cast<uintptr_t>(region.address));
				const SModuleInfo* targetModule = FindModuleForAddress(modules, target);

				const bool ownerTrusted = owner && owner->trustedPath;
				const bool targetTrusted = targetModule && targetModule->trustedPath;
				if (ownerTrusted && !targetTrusted)
				{
					Log(EViolationSeverity::Critical, EViolationType::InlineHookDetected,
						"Inline hook suspected at %s", region.name.c_str());
					return false;
				}
			}

			Log(EViolationSeverity::Critical, EViolationType::CriticalRegionModified,
				"Critical region modified: %s", region.name.c_str());
			ok = false;
		}
		return ok;
	}

	bool SnapshotMainTextSection()
	{
		const uintptr_t mainBase = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
		if (!mainBase || !ValidateImageHeader(mainBase))
			return false;

		const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(mainBase);
		const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(mainBase + static_cast<uintptr_t>(dos->e_lfanew));
		const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
		for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section)
		{
			char sectionName[9] = {};
			std::memcpy(sectionName, section->Name, 8);
			const std::string normalizedName = ToLowerAscii(sectionName);
			if (normalizedName.find(".text") == 0)
			{
				const uintptr_t sectionAddress = mainBase + section->VirtualAddress;
				const size_t virtualSize = static_cast<size_t>(section->Misc.VirtualSize);
				const size_t rawSize = static_cast<size_t>(section->SizeOfRawData);
				const size_t sectionSize = virtualSize > rawSize ? virtualSize : rawSize;
				if (!sectionAddress || sectionSize == 0)
					return false;

				textSnapshot.address = reinterpret_cast<const uint8_t*>(sectionAddress);
				textSnapshot.size = sectionSize;
				textSnapshot.hash = HashMemoryFNV1a64(textSnapshot.address, textSnapshot.size);
				return true;
			}
		}

		return false;
	}

	bool VerifyMainTextSection()
	{
		if (!textSnapshot.address || textSnapshot.size == 0)
			return true;

		const uint64_t currentHash = HashMemoryFNV1a64(textSnapshot.address, textSnapshot.size);
		return currentHash == textSnapshot.hash;
	}

	void SnapshotExecutableFile()
	{
		executableSnapshot.normalizedPath = processPath;
		executableSnapshot.valid = ReadFileHash(executableSnapshot.normalizedPath, executableSnapshot.hash, executableSnapshot.size);
	}

	bool VerifyExecutableFile()
	{
		if (!executableSnapshot.valid)
			return true;

		uint64_t currentHash = 0;
		uint64_t currentSize = 0;
		if (!ReadFileHash(executableSnapshot.normalizedPath, currentHash, currentSize))
			return false;

		return currentHash == executableSnapshot.hash && currentSize == executableSnapshot.size;
	}

	void AddTimerApiSnapshot(const char* moduleName, const char* procName)
	{
		HMODULE module = GetModuleHandleA(moduleName);
		if (!module)
			return;

		const auto* address = reinterpret_cast<const uint8_t*>(GetProcAddress(module, procName));
		if (!address)
			return;

		SApiSnapshot snapshot;
		snapshot.name = std::string(moduleName) + "!" + procName;
		snapshot.address = address;
		snapshot.size = 24;
		snapshot.hash = HashMemoryFNV1a64(snapshot.address, snapshot.size);
		timerApiSnapshots.emplace_back(std::move(snapshot));
	}

	void SnapshotTimerApiIntegrity()
	{
		timerApiSnapshots.clear();
		AddTimerApiSnapshot("winmm.dll", "timeGetTime");
		AddTimerApiSnapshot("kernel32.dll", "QueryPerformanceCounter");
		AddTimerApiSnapshot("kernel32.dll", "GetTickCount");
		AddTimerApiSnapshot("kernel32.dll", "GetTickCount64");
		AddTimerApiSnapshot("kernel32.dll", "GetSystemTimeAsFileTime");
		AddTimerApiSnapshot("kernel32.dll", "GetSystemTimePreciseAsFileTime");
		AddTimerApiSnapshot("kernel32.dll", "QueryUnbiasedInterruptTime");
		AddTimerApiSnapshot("ntdll.dll", "NtQuerySystemTime");
	}

	bool VerifyTimerApiIntegrity(const std::vector<SModuleInfo>& modules)
	{
		for (const SApiSnapshot& snapshot : timerApiSnapshots)
		{
			if (!snapshot.address || snapshot.size == 0)
				continue;

			const uint64_t currentHash = HashMemoryFNV1a64(snapshot.address, snapshot.size);
			if (currentHash == snapshot.hash)
				continue;

			uintptr_t target = 0;
			if (DecodeHookTarget(snapshot.address, target))
			{
				const SModuleInfo* targetModule = FindModuleForAddress(modules, target);
				if (!targetModule || !targetModule->trustedPath)
				{
					Log(EViolationSeverity::Critical, EViolationType::TimingManipulationDetected,
						"Timer API redirected outside trusted modules: %s", snapshot.name.c_str());
					return false;
				}
			}

			Log(EViolationSeverity::Critical, EViolationType::TimingManipulationDetected,
				"Timer API code changed after baseline: %s", snapshot.name.c_str());
			return false;
		}

		return true;
	}

	void SnapshotImportAddressTable()
	{
		importSnapshots.clear();

		const uintptr_t mainBase = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
		if (!mainBase || !ValidateImageHeader(mainBase))
			return;

		const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(mainBase);
		const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(mainBase + static_cast<uintptr_t>(dos->e_lfanew));
		const IMAGE_DATA_DIRECTORY& importDirectory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if (importDirectory.VirtualAddress == 0 || importDirectory.Size == 0)
			return;

		const auto* importDesc = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(mainBase + importDirectory.VirtualAddress);
		for (; importDesc->Name != 0; ++importDesc)
		{
			const char* importName = reinterpret_cast<const char*>(mainBase + importDesc->Name);
			uintptr_t thunkRva = importDesc->FirstThunk;
			if (!thunkRva)
				continue;

			auto* iatThunk = reinterpret_cast<IMAGE_THUNK_DATA32*>(mainBase + thunkRva);
			for (; iatThunk->u1.Function != 0; ++iatThunk)
			{
				SIatSnapshot snapshot;
				snapshot.thunkAddress = reinterpret_cast<uintptr_t>(&iatThunk->u1.Function);
				snapshot.functionAddress = static_cast<uintptr_t>(iatThunk->u1.Function);
				if (importName)
					snapshot.importModule = ToLowerAscii(importName);
				importSnapshots.emplace_back(std::move(snapshot));
			}
		}
	}

	bool VerifyImportAddressTableSnapshot(const std::vector<SModuleInfo>& modules)
	{
		if (importSnapshots.empty())
			return true;

		for (const SIatSnapshot& snapshot : importSnapshots)
		{
			if (!snapshot.thunkAddress || !snapshot.functionAddress)
				continue;

			const uintptr_t currentAddress = *reinterpret_cast<const uintptr_t*>(snapshot.thunkAddress);
			if (currentAddress == snapshot.functionAddress)
				continue;

			const SModuleInfo* owner = FindModuleForAddress(modules, currentAddress);
			if (!owner || !owner->trustedPath || !owner->validImage)
			{
				Log(EViolationSeverity::Critical, EViolationType::ImportHookDetected,
					"IAT entry redirected outside trusted modules: %s", snapshot.importModule.c_str());
				return false;
			}

			Log(EViolationSeverity::High, EViolationType::ImportHookDetected,
				"IAT entry changed after baseline: %s", snapshot.importModule.c_str());
			return false;
		}

		return true;
	}

	bool ValidateImportAddressTable(const std::vector<SModuleInfo>& modules)
	{
		const uintptr_t mainBase = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
		if (!mainBase || !ValidateImageHeader(mainBase))
			return true;

		const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(mainBase);
		const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(mainBase + static_cast<uintptr_t>(dos->e_lfanew));
		const IMAGE_DATA_DIRECTORY& importDirectory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if (importDirectory.VirtualAddress == 0 || importDirectory.Size == 0)
			return true;

		const auto* importDesc = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(mainBase + importDirectory.VirtualAddress);
		for (; importDesc->Name != 0; ++importDesc)
		{
			uintptr_t thunkRva = importDesc->FirstThunk;
			if (!thunkRva)
				continue;

			auto* iatThunk = reinterpret_cast<IMAGE_THUNK_DATA32*>(mainBase + thunkRva);
			for (; iatThunk->u1.Function != 0; ++iatThunk)
			{
				const uintptr_t procAddress = static_cast<uintptr_t>(iatThunk->u1.Function);
				if (!procAddress)
					continue;

				const SModuleInfo* module = FindModuleForAddress(modules, procAddress);
				if (!module)
					return false;

				if (!module->trustedPath || !module->validImage)
					return false;
			}
		}

		return true;
	}

	void RaiseAdaptiveScrutiny()
	{
		const uint64_t until = GetMsTick() + kAdaptiveProtectionWindowMs;
		if (until > adaptiveScanUntilMs)
			adaptiveScanUntilMs = until;
	}

	bool IsAdaptiveScrutinyActive(uint64_t nowMs) const
	{
		return adaptiveScanUntilMs != 0 && nowMs < adaptiveScanUntilMs;
	}

	void ApplyRiskAndMaybeClose(EViolationType type, EViolationSeverity severity)
	{
		++violationCounters[type];

		switch (severity)
		{
		case EViolationSeverity::Info:
			break;
		case EViolationSeverity::Suspicious:
			riskScore += kSuspiciousScoreDelta;
			RaiseAdaptiveScrutiny();
			break;
		case EViolationSeverity::High:
			riskScore += kHighScoreDelta;
			RaiseAdaptiveScrutiny();
			break;
		case EViolationSeverity::Critical:
			riskScore += kCriticalScoreDelta;
			healthy.store(false, std::memory_order_release);
			return;
		}

		if (severity == EViolationSeverity::High && (violationCounters[type] >= 2 || riskScore >= kHighScoreCloseThreshold))
		{
			Log(EViolationSeverity::High, type, "Confirmed high-risk signal; closing protected session: %s score=%d hits=%d",
				ViolationTypeName(type), riskScore, violationCounters[type]);
			healthy.store(false, std::memory_order_release);
			return;
		}

		if (severity == EViolationSeverity::Suspicious && riskScore >= kSuspiciousCloseThreshold)
		{
			Log(EViolationSeverity::High, type, "Accumulated suspicious risk exceeded threshold; closing protected session: %s score=%d hits=%d",
				ViolationTypeName(type), riskScore, violationCounters[type]);
			healthy.store(false, std::memory_order_release);
		}
	}

	void CheckDebuggerGuard()
	{
		bool debuggerDetected = false;

		if (IsDebuggerPresent())
			debuggerDetected = true;

		BOOL remoteDebugger = FALSE;
		CheckRemoteDebuggerPresent(GetCurrentProcess(), &remoteDebugger);
		if (remoteDebugger)
			debuggerDetected = true;

		using NtQueryInformationProcessFn = LONG(NTAPI*)(HANDLE, LONG, PVOID, ULONG, PULONG);
		static const NtQueryInformationProcessFn ntQueryInformationProcess =
			reinterpret_cast<NtQueryInformationProcessFn>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess"));

		if (ntQueryInformationProcess)
		{
			ULONG_PTR debugPort = 0;
			if (ntQueryInformationProcess(GetCurrentProcess(), 7, &debugPort, sizeof(debugPort), nullptr) >= 0 && debugPort != 0)
				debuggerDetected = true;

			ULONG debugFlags = 0;
			if (ntQueryInformationProcess(GetCurrentProcess(), 31, &debugFlags, sizeof(debugFlags), nullptr) >= 0 && debugFlags == 0)
				debuggerDetected = true;

			HANDLE debugObject = nullptr;
			if (ntQueryInformationProcess(GetCurrentProcess(), 30, &debugObject, sizeof(debugObject), nullptr) >= 0 && debugObject != nullptr)
				debuggerDetected = true;
		}

		if (debuggerDetected)
		{
			Log(EViolationSeverity::Critical, EViolationType::DebuggerDetected, "Debugger attach detected");
			ApplyRiskAndMaybeClose(EViolationType::DebuggerDetected, EViolationSeverity::Critical);
		}
	}

	void CheckProxyModuleGuard(const std::vector<SModuleInfo>& modules)
	{
		for (const SModuleInfo& module : modules)
		{
			if (!IsProxyHijackCandidate(module))
				continue;

			const std::string identity = ModuleIdentity(module);
			const int hits = ++proxyModuleHits[identity];

			Log(EViolationSeverity::Suspicious, EViolationType::ModuleInjected,
				"Potential proxy module observed (%d): %s", hits, identity.c_str());
			ApplyRiskAndMaybeClose(EViolationType::ModuleInjected, EViolationSeverity::Suspicious);

			if (hits >= kProxyModuleConfirmHits)
			{
				Log(EViolationSeverity::Critical, EViolationType::ModuleInjected,
					"Proxy module hijack confirmed: %s", identity.c_str());
				ApplyRiskAndMaybeClose(EViolationType::ModuleInjected, EViolationSeverity::Critical);
			}
		}
	}

	void CheckLaunchSurfaceGuard()
	{
		if (processDirectory.empty() || currentDirectory.empty())
			return;

		if (StartsWith(currentDirectory, processDirectory))
			return;

		const int hits = ++launchSurfaceHits[currentDirectory];
		Log(EViolationSeverity::Suspicious, EViolationType::ModuleInjected,
			"Client started with unexpected working directory (%d)", hits);
		ApplyRiskAndMaybeClose(EViolationType::ModuleInjected, hits >= 2 ? EViolationSeverity::High : EViolationSeverity::Suspicious);
	}

	void CheckModuleSectionGuard(const SModuleInfo& module, bool inBaseline)
	{
		if (!module.validImage || module.baseAddress == 0)
			return;

		const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module.baseAddress);
		const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(module.baseAddress + static_cast<uintptr_t>(dos->e_lfanew));
		const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
		for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section)
		{
			const bool executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
			const bool writable = (section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0;
			if (!executable || !writable)
				continue;

			if (inBaseline)
				return;

			if (module.trustedPath && !StartsWith(module.normalizedPath, processDirectory))
				return;

			const std::string identity = ModuleIdentity(module);
			const int hits = ++sectionAnomalyHits[identity];
			Log(EViolationSeverity::Suspicious, EViolationType::SectionIntegrityFailed,
				"Writable executable PE section observed (%d): %s", hits, BaseNameFromPath(identity).c_str());

			if (!module.trustedPath && hits >= 2)
				ApplyRiskAndMaybeClose(EViolationType::SectionIntegrityFailed, EViolationSeverity::High);
			else
				ApplyRiskAndMaybeClose(EViolationType::SectionIntegrityFailed, EViolationSeverity::Suspicious);
			return;
		}
	}

	void CheckModuleGuard(const std::vector<SModuleInfo>& modules)
	{
		CheckLaunchSurfaceGuard();
		CheckProxyModuleGuard(modules);

		for (const SModuleInfo& module : modules)
		{
			const std::string identity = ModuleIdentity(module);
			const bool inBaseline = (moduleBaseline.find(identity) != moduleBaseline.end());

			if (!module.validImage)
			{
				Log(EViolationSeverity::High, EViolationType::ManualMapSuspected,
					"Module has invalid PE header: %s", identity.c_str());
				ApplyRiskAndMaybeClose(EViolationType::ManualMapSuspected, EViolationSeverity::High);
				continue;
			}

			CheckModuleSectionGuard(module, inBaseline);

			if (!inBaseline)
			{
				if (!module.trustedPath)
				{
					const int hits = ++unknownModuleHits[identity];
					Log(EViolationSeverity::Suspicious, EViolationType::UnknownModuleLoaded,
						"Unknown module observed (%d): %s", hits, identity.c_str());
					if (hits >= kUnknownModuleConfirmHits)
					{
						Log(EViolationSeverity::High, EViolationType::UnknownModuleLoaded,
							"Unknown untrusted module confirmed: %s", identity.c_str());
						ApplyRiskAndMaybeClose(EViolationType::UnknownModuleLoaded, EViolationSeverity::High);
					}
					else
					{
						ApplyRiskAndMaybeClose(EViolationType::UnknownModuleLoaded, EViolationSeverity::Suspicious);
					}
				}
				else
				{
					moduleBaseline.emplace(identity, module);
					Log(EViolationSeverity::Info, EViolationType::ModuleInjected,
						"Trusted runtime module added to baseline: %s", identity.c_str());
				}
			}
		}
	}

	void CheckMemoryGuard(const std::vector<SModuleInfo>& modules, bool fullScan)
	{
		uintptr_t address = fullScan ? 0 : incrementalMemoryCursor;
		size_t scannedRegions = 0;

		while (true)
		{
			MEMORY_BASIC_INFORMATION mbi{};
			const SIZE_T queried = VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi));
			if (queried != sizeof(mbi))
			{
				incrementalMemoryCursor = 0;
				break;
			}

			const uintptr_t regionBase = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
			const uintptr_t nextAddress = regionBase + mbi.RegionSize;
			if (nextAddress <= regionBase)
			{
				incrementalMemoryCursor = 0;
				break;
			}

			const DWORD protection = mbi.Protect & (~PAGE_GUARD);
			if (mbi.State == MEM_COMMIT && IsExecutableProtection(protection))
			{
				const bool privateExec = (mbi.Type == MEM_PRIVATE);
				const bool rwx = IsRwxProtection(protection);
				const SModuleInfo* owner = FindModuleForAddress(modules, regionBase);

				if (owner && !owner->trustedPath)
				{
					Log(EViolationSeverity::High, EViolationType::ModuleInjected,
						"Executable page belongs to untrusted module at 0x%p", reinterpret_cast<void*>(regionBase));
					ApplyRiskAndMaybeClose(EViolationType::ModuleInjected, EViolationSeverity::High);
				}

				if (executableMemoryBaseline.find(regionBase) != executableMemoryBaseline.end())
				{
					address = nextAddress;
					++scannedRegions;
					if (!fullScan && scannedRegions >= kMemoryMediumRegionBudget)
					{
						incrementalMemoryCursor = address;
						break;
					}
					continue;
				}

				if ((privateExec || rwx) && owner == nullptr && IsInEarlyProtectionWindow(GetMsTick()))
				{
					executableMemoryBaseline.insert(regionBase);
					address = nextAddress;
					++scannedRegions;
					if (!fullScan && scannedRegions >= kMemoryMediumRegionBudget)
					{
						incrementalMemoryCursor = address;
						break;
					}
					continue;
				}

				if (privateExec && owner == nullptr)
				{
					const int hits = ++privateExecHits[regionBase];
					if (hits >= 3)
					{
						Log(EViolationSeverity::High, EViolationType::ExecutablePrivateMemory,
							"Executable private memory confirmed at 0x%p", reinterpret_cast<void*>(regionBase));
						ApplyRiskAndMaybeClose(EViolationType::ExecutablePrivateMemory, EViolationSeverity::High);
					}
					else
					{
						Log(EViolationSeverity::Suspicious, EViolationType::ExecutablePrivateMemory,
							"Executable private memory observed (%d) at 0x%p", hits, reinterpret_cast<void*>(regionBase));
						ApplyRiskAndMaybeClose(EViolationType::ExecutablePrivateMemory, EViolationSeverity::Suspicious);
					}
				}

				if (rwx && (privateExec || owner == nullptr || !owner->trustedPath))
				{
					Log(EViolationSeverity::High, EViolationType::RwxMemoryDetected,
						"RWX executable region detected at 0x%p", reinterpret_cast<void*>(regionBase));
					ApplyRiskAndMaybeClose(EViolationType::RwxMemoryDetected, EViolationSeverity::High);
				}
			}

			address = nextAddress;
			++scannedRegions;

			if (!fullScan && scannedRegions >= kMemoryMediumRegionBudget)
			{
				incrementalMemoryCursor = address;
				break;
			}
		}
	}

	void CheckThreadGuard(const std::vector<SModuleInfo>& modules)
	{
		std::vector<SThreadRecord> threads;
		if (!SnapshotThreads(threads))
			return;

		for (const SThreadRecord& thread : threads)
		{
			if (thread.threadId == mainThreadId)
			{
				knownThreads.insert(thread.threadId);
				continue;
			}

			if (thread.startAddress == 0)
			{
				knownThreads.insert(thread.threadId);
				continue;
			}

			const SModuleInfo* owner = FindModuleForAddress(modules, thread.startAddress);
			bool suspicious = (owner == nullptr) || !owner->trustedPath;

			if (!suspicious && knownThreads.find(thread.threadId) == knownThreads.end())
			{
				knownThreads.insert(thread.threadId);
				continue;
			}

			if (!suspicious)
				continue;

			int hits = ++suspiciousThreadHits[thread.threadId];
			Log(EViolationSeverity::Suspicious, EViolationType::SuspiciousThread,
				"Suspicious thread start detected (%d): tid=%lu start=0x%p",
				hits, static_cast<unsigned long>(thread.threadId), reinterpret_cast<void*>(thread.startAddress));

			if (hits >= kSuspiciousThreadConfirmHits)
			{
				Log(EViolationSeverity::High, EViolationType::SuspiciousThread,
					"Suspicious thread confirmed: tid=%lu", static_cast<unsigned long>(thread.threadId));
				ApplyRiskAndMaybeClose(EViolationType::SuspiciousThread, EViolationSeverity::High);
			}
			else
			{
				ApplyRiskAndMaybeClose(EViolationType::SuspiciousThread, EViolationSeverity::Suspicious);
			}
		}
	}

	void CheckTimingGuard()
	{
		if (timingState.frequency.QuadPart == 0)
			QueryPerformanceFrequency(&timingState.frequency);

		LARGE_INTEGER nowQpc{};
		QueryPerformanceCounter(&nowQpc);

		const uint64_t nowTick = GetMsTick();
		uint64_t nowTimeGetTime = static_cast<uint64_t>(timeGetTime());
		const uint64_t nowSystem = GetSystemTimeMs();
		const uint64_t nowPreciseSystem = GetPreciseSystemTimeMs();
		const uint64_t nowUnbiased = GetUnbiasedInterruptMs();
		uint64_t nowClientTimer = static_cast<uint64_t>(ELTimer_GetMSec());
		uint64_t nowFrameTimer = static_cast<uint64_t>(ELTimer_GetFrameMSec());

		if (timingState.initializedAtMs == 0)
		{
			timingState.initializedAtMs = nowTick;
			timingState.lastQpc = nowQpc;
			timingState.lastTick = nowTick;
			timingState.lastTimeGetTime = nowTimeGetTime;
			timingState.lastSystemTime = nowSystem;
			timingState.lastPreciseSystemTime = nowPreciseSystem;
			timingState.lastUnbiasedInterrupt = nowUnbiased;
			timingState.lastClientTimer = nowClientTimer;
			timingState.lastFrameTimer = nowFrameTimer;
			return;
		}

		const uint64_t realNow = nowUnbiased != 0 ? nowUnbiased : (nowPreciseSystem != 0 ? nowPreciseSystem : nowSystem);
		const uint64_t realLast = timingState.lastUnbiasedInterrupt != 0 ? timingState.lastUnbiasedInterrupt :
			(timingState.lastPreciseSystemTime != 0 ? timingState.lastPreciseSystemTime : timingState.lastSystemTime);
		if (realNow <= realLast)
			return;

		const uint64_t realDeltaMs = realNow - realLast;
		if (realDeltaMs < 250)
			return;

		const bool warmup = (nowTick >= timingState.initializedAtMs) &&
			((nowTick - timingState.initializedAtMs) < kTimingWarmupMs);
		if (kEnableInternalSelfTests && kInternalSelfTestTimingWarp && !warmup)
		{
			const uint64_t warpedDelta = realDeltaMs * 3ull;
			nowTimeGetTime = timingState.lastTimeGetTime + warpedDelta;
			nowClientTimer = timingState.lastClientTimer + warpedDelta;
			nowFrameTimer = timingState.lastFrameTimer + warpedDelta;
		}
		if (realDeltaMs > 6000)
		{
			timingState.lastQpc = nowQpc;
			timingState.lastTick = nowTick;
			timingState.lastTimeGetTime = nowTimeGetTime;
			timingState.lastSystemTime = nowSystem;
			timingState.lastPreciseSystemTime = nowPreciseSystem;
			timingState.lastUnbiasedInterrupt = nowUnbiased;
			timingState.lastClientTimer = nowClientTimer;
			timingState.lastFrameTimer = nowFrameTimer;
			timingState.anomalyCount = (std::max)(0, timingState.anomalyCount - 1);
			timingState.criticalCount = (std::max)(0, timingState.criticalCount - 1);
			return;
		}

		const double realDelta = static_cast<double>(realDeltaMs);
		const LONGLONG qpcFrequency = timingState.frequency.QuadPart > 0 ? timingState.frequency.QuadPart : 1;
		const double qpcDelta = static_cast<double>(nowQpc.QuadPart - timingState.lastQpc.QuadPart) * 1000.0 /
			static_cast<double>(qpcFrequency);
		const double tickDelta = nowTick >= timingState.lastTick ? static_cast<double>(nowTick - timingState.lastTick) : -1.0;
		const double timeGetTimeDelta = nowTimeGetTime >= timingState.lastTimeGetTime ?
			static_cast<double>(nowTimeGetTime - timingState.lastTimeGetTime) : -1.0;
		const double systemDelta = nowSystem >= timingState.lastSystemTime ?
			static_cast<double>(nowSystem - timingState.lastSystemTime) : -1.0;
		const double preciseDelta = nowPreciseSystem >= timingState.lastPreciseSystemTime ?
			static_cast<double>(nowPreciseSystem - timingState.lastPreciseSystemTime) : -1.0;
		const double clientDelta = nowClientTimer >= timingState.lastClientTimer ?
			static_cast<double>(nowClientTimer - timingState.lastClientTimer) : -1.0;
		const double frameDelta = nowFrameTimer >= timingState.lastFrameTimer ?
			static_cast<double>(nowFrameTimer - timingState.lastFrameTimer) : -1.0;
		const double clientRatio = clientDelta > 0.0 ? clientDelta / realDelta : 1.0;

		std::array<double, 6> ratios{};
		size_t ratioCount = 0;
		auto addRatio = [&](double delta)
		{
			if (delta > 0.0)
				ratios[ratioCount++] = delta / realDelta;
		};

		addRatio(qpcDelta);
		addRatio(tickDelta);
		addRatio(timeGetTimeDelta);
		addRatio(systemDelta);
		addRatio(preciseDelta);
		addRatio(clientDelta);

		double minRatio = (std::numeric_limits<double>::max)();
		double maxRatio = 0.0;
		bool hardAnomaly = false;
		bool softAnomaly = false;

		for (size_t i = 0; i < ratioCount; ++i)
		{
			const double ratio = ratios[i];
			minRatio = (std::min)(minRatio, ratio);
			maxRatio = (std::max)(maxRatio, ratio);

			if (ratio < kTimingHardRatioLow || ratio > kTimingHardRatioHigh)
				hardAnomaly = true;
			else if (ratio < kTimingSoftRatioLow || ratio > kTimingSoftRatioHigh)
				softAnomaly = true;
		}

		if (ratioCount >= 2 && minRatio > 0.0 && maxRatio / minRatio > 1.55)
			hardAnomaly = true;
		else if (ratioCount >= 2 && minRatio > 0.0 && maxRatio / minRatio > 1.25)
			softAnomaly = true;

		if (frameDelta > 0.0)
		{
			const double frameRatio = frameDelta / realDelta;
			timingState.frameRatioHistory.emplace_back(frameRatio);
			if (timingState.frameRatioHistory.size() > static_cast<size_t>(kTimingHistoryWindow))
				timingState.frameRatioHistory.pop_front();

			if (frameRatio > kTimingFrameHardRatio || frameRatio < 0.45)
			{
				if (hardAnomaly || softAnomaly)
					hardAnomaly = true;
			}
			else if (frameRatio > kTimingSoftRatioHigh || frameRatio < kTimingSoftRatioLow)
			{
				if (hardAnomaly || softAnomaly)
					softAnomaly = true;
			}
		}

		if (hardAnomaly)
		{
			timingState.criticalCount += warmup ? 1 : 2;
			timingState.anomalyCount += 2;
			timingState.suspicionScore += 35.0;
		}
		else if (softAnomaly)
		{
			++timingState.anomalyCount;
			timingState.suspicionScore += 12.0;
			if (timingState.criticalCount > 0)
				--timingState.criticalCount;
		}
		else
		{
			++timingState.stableCount;
			if (timingState.anomalyCount > 0)
				--timingState.anomalyCount;
			if (timingState.criticalCount > 0)
				--timingState.criticalCount;
			timingState.suspicionScore = (std::max)(0.0, timingState.suspicionScore - 4.0);
		}

		timingState.ratioHistory.emplace_back(clientRatio);
		if (timingState.ratioHistory.size() > static_cast<size_t>(kTimingHistoryWindow))
			timingState.ratioHistory.pop_front();

		if (!warmup && (timingState.criticalCount >= kTimingCriticalConfirmHits || timingState.suspicionScore >= 90.0))
		{
			if (nowTick - timingState.lastCriticalAtMs > 1000)
			{
				timingState.lastCriticalAtMs = nowTick;
				Log(EViolationSeverity::Critical, EViolationType::TimingManipulationDetected,
					"Speed manipulation confirmed (real=%0.1fms qpc=%0.2f tick=%0.2f time=%0.2f client=%0.2f frame=%0.2f spread=%0.2f)",
					realDelta, qpcDelta / realDelta, tickDelta / realDelta, timeGetTimeDelta / realDelta,
					clientRatio, frameDelta > 0.0 ? frameDelta / realDelta : 1.0,
					(minRatio > 0.0 && maxRatio > 0.0) ? maxRatio / minRatio : 1.0);
			}
			ApplyRiskAndMaybeClose(EViolationType::TimingManipulationDetected, EViolationSeverity::Critical);
		}
		else if (!warmup && timingState.anomalyCount >= kTimingAnomalyConfirmHits)
		{
			Log(EViolationSeverity::High, EViolationType::TimingManipulationDetected,
				"Timing drift confirmed (client=%0.2f spread=%0.2f score=%0.1f)",
				clientRatio, (minRatio > 0.0 && maxRatio > 0.0) ? maxRatio / minRatio : 1.0,
				timingState.suspicionScore);
			ApplyRiskAndMaybeClose(EViolationType::TimingManipulationDetected, EViolationSeverity::High);
			timingState.anomalyCount = (std::max)(0, timingState.anomalyCount - 2);
		}

		timingState.lastQpc = nowQpc;
		timingState.lastTick = nowTick;
		timingState.lastTimeGetTime = nowTimeGetTime;
		timingState.lastSystemTime = nowSystem;
		timingState.lastPreciseSystemTime = nowPreciseSystem;
		timingState.lastUnbiasedInterrupt = nowUnbiased;
		timingState.lastClientTimer = nowClientTimer;
		timingState.lastFrameTimer = nowFrameTimer;
	}

	void CheckBotBehaviorGuard()
	{
		const uint64_t now = GetMsTick();
		const bool leftDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

		if (leftDown && !botState.lastLeftDown)
		{
			if (botState.lastLeftDownTs != 0 && now > botState.lastLeftDownTs)
			{
				const uint64_t delta = now - botState.lastLeftDownTs;
				botState.intervalsMs.emplace_back(delta);
				if (botState.intervalsMs.size() > static_cast<size_t>(kBotIntervalWindow))
					botState.intervalsMs.pop_front();
			}
			botState.lastLeftDownTs = now;
		}
		botState.lastLeftDown = leftDown;

		const int actionKeys[8] = { VK_SPACE, '1', '2', '3', '4', VK_F1, VK_F2, VK_F3 };
		for (size_t i = 0; i < botState.actionDown.size(); ++i)
		{
			const bool down = (GetAsyncKeyState(actionKeys[i]) & 0x8000) != 0;
			if (down && !botState.actionDown[i])
			{
				if (botState.actionLastTs[i] != 0 && now > botState.actionLastTs[i])
				{
					const uint64_t delta = now - botState.actionLastTs[i];
					if (delta > 20 && delta < 3000)
					{
						botState.actionIntervalsMs.emplace_back(delta);
						if (botState.actionIntervalsMs.size() > static_cast<size_t>(kBotIntervalWindow))
							botState.actionIntervalsMs.pop_front();
					}
				}
				botState.actionLastTs[i] = now;
			}
			botState.actionDown[i] = down;
		}

		if (botState.intervalsMs.size() < 18)
		{
			if (botState.actionIntervalsMs.size() < 24)
				return;
		}

		double sum = 0.0;
		for (uint64_t v : botState.intervalsMs)
			sum += static_cast<double>(v);

		const double mean = botState.intervalsMs.empty() ? 0.0 : sum / static_cast<double>(botState.intervalsMs.size());
		double varianceSum = 0.0;
		for (uint64_t v : botState.intervalsMs)
		{
			const double d = static_cast<double>(v) - mean;
			varianceSum += d * d;
		}
		const double variance = botState.intervalsMs.empty() ? 0.0 : varianceSum / static_cast<double>(botState.intervalsMs.size());

		double actionSum = 0.0;
		for (uint64_t v : botState.actionIntervalsMs)
			actionSum += static_cast<double>(v);

		const double actionMean = botState.actionIntervalsMs.empty() ? 0.0 : actionSum / static_cast<double>(botState.actionIntervalsMs.size());
		double actionVarianceSum = 0.0;
		for (uint64_t v : botState.actionIntervalsMs)
		{
			const double d = static_cast<double>(v) - actionMean;
			actionVarianceSum += d * d;
		}
		const double actionVariance = botState.actionIntervalsMs.empty() ? 0.0 :
			actionVarianceSum / static_cast<double>(botState.actionIntervalsMs.size());

		// Extremely uniform click intervals are strong automation signals.
		const bool clickRhythm = botState.intervalsMs.size() >= 18 && mean > 35.0 && mean < 420.0 && variance < 5.0;
		const bool actionRhythm = botState.actionIntervalsMs.size() >= 24 && actionMean > 40.0 && actionMean < 700.0 && actionVariance < 6.0;
		if (clickRhythm || actionRhythm)
			++botState.anomalyCount;
		else if (botState.anomalyCount > 0)
			--botState.anomalyCount;

		if (botState.anomalyCount >= kBotAnomalyConfirmHits)
		{
			Log(EViolationSeverity::High, EViolationType::BotBehaviorDetected,
				"Bot-like input rhythm confirmed (click=%0.2f/%0.2f action=%0.2f/%0.2f)",
				mean, variance, actionMean, actionVariance);
			ApplyRiskAndMaybeClose(EViolationType::BotBehaviorDetected, EViolationSeverity::High);
			botState.anomalyCount = 0;
		}
	}

	void RunImportGuard(const std::vector<SModuleInfo>& modules)
	{
		if (!VerifyImportAddressTableSnapshot(modules))
		{
			ApplyRiskAndMaybeClose(EViolationType::ImportHookDetected, EViolationSeverity::Critical);
			return;
		}

		if (!ValidateImportAddressTable(modules))
		{
			Log(EViolationSeverity::High, EViolationType::ImportHookDetected, "IAT integrity verification failed");
			ApplyRiskAndMaybeClose(EViolationType::ImportHookDetected, EViolationSeverity::High);
		}
	}

	void RunPythonGuard(const std::vector<SModuleInfo>& modules)
	{
		for (const SModuleInfo& module : modules)
		{
			if (module.normalizedPath.find("python") == std::string::npos)
				continue;

			if (!module.trustedPath || !module.validImage)
			{
				Log(EViolationSeverity::High, EViolationType::PythonTamperDetected,
					"Untrusted Python related module detected: %s", module.normalizedPath.c_str());
				ApplyRiskAndMaybeClose(EViolationType::PythonTamperDetected, EViolationSeverity::High);
			}
		}
	}

	void RunSectionIntegrityGuard()
	{
		if (!VerifyMainTextSection())
		{
			Log(EViolationSeverity::Critical, EViolationType::SectionIntegrityFailed, "Main .text section hash mismatch");
			ApplyRiskAndMaybeClose(EViolationType::SectionIntegrityFailed, EViolationSeverity::Critical);
		}
	}

	void RunFileIntegrityGuard()
	{
		if (!VerifyExecutableFile())
		{
			Log(EViolationSeverity::High, EViolationType::FileIntegrityFailed, "Executable file integrity mismatch");
			ApplyRiskAndMaybeClose(EViolationType::FileIntegrityFailed, EViolationSeverity::High);
		}
	}

	void RunSelfProtectionGuard(const std::vector<SModuleInfo>& modules, bool fastOnly)
	{
		if (!VerifyCanary())
		{
			Log(EViolationSeverity::Critical, EViolationType::HoneypotModified, "Canary state modified");
			ApplyRiskAndMaybeClose(EViolationType::HoneypotModified, EViolationSeverity::Critical);
			return;
		}

		if (!VerifyCriticalRegions(fastOnly, modules))
		{
			ApplyRiskAndMaybeClose(EViolationType::FratelloSelfTamperDetected, EViolationSeverity::Critical);
		}
	}

	void RunInternalDiagnosticsSelfTest()
	{
		if (selfTestDiagnosticsRan || frameCounter < 20)
			return;

		selfTestDiagnosticsRan = true;

		std::array<uint8_t, 64> region{};
		for (size_t i = 0; i < region.size(); ++i)
			region[i] = static_cast<uint8_t>((i * 31u) ^ 0xA7u);

		const uint64_t cleanHash = HashMemoryFNV1a64(region.data(), region.size());
		region[17] ^= 0x5Au;
		const uint64_t tamperedHash = HashMemoryFNV1a64(region.data(), region.size());
		Log(EViolationSeverity::Info, EViolationType::CriticalRegionModified,
			"Internal self-test critical-region hash=%s", cleanHash != tamperedHash ? "ok" : "failed");

		std::array<uint8_t, 8> jmp{};
		jmp[0] = 0xE9;
		*reinterpret_cast<int32_t*>(&jmp[1]) = 0;
		uintptr_t hookTarget = 0;
		const bool hookDecodeOk = DecodeHookTarget(jmp.data(), hookTarget) &&
			hookTarget == reinterpret_cast<uintptr_t>(jmp.data() + 5);
		Log(EViolationSeverity::Info, EViolationType::InlineHookDetected,
			"Internal self-test inline-hook decode=%s", hookDecodeOk ? "ok" : "failed");

		const double simulatedQpcDelta = 1.000;
		const double simulatedTickDelta = 1.004;
		const double simulatedRatio = simulatedTickDelta / simulatedQpcDelta;
		Log(EViolationSeverity::Info, EViolationType::TimingManipulationDetected,
			"Internal self-test normal timing ratio=%0.3f", simulatedRatio);

		double sum = 0.0;
		std::array<double, 24> machineIntervals{};
		for (double& interval : machineIntervals)
		{
			interval = 133.0;
			sum += interval;
		}
		const double mean = sum / static_cast<double>(machineIntervals.size());
		double variance = 0.0;
		for (double interval : machineIntervals)
		{
			const double d = interval - mean;
			variance += d * d;
		}
		variance /= static_cast<double>(machineIntervals.size());
		Log(EViolationSeverity::Info, EViolationType::BotBehaviorDetected,
			"Internal self-test bot rhythm variance=%0.3f", variance);
	}

	void RunInternalSelfTests()
	{
		if (!kEnableInternalSelfTests)
			return;

		RunInternalDiagnosticsSelfTest();

		if ((frameCounter % 600) == 0)
			Log(EViolationSeverity::Info, EViolationType::FratelloSelfTamperDetected, "Internal self-test heartbeat");

		if (kInternalSelfTestCriticalClose && !selfTestViolationFired && startupAtMs != 0 && (GetMsTick() - startupAtMs) > 7000)
		{
			selfTestViolationFired = true;
			canaryState.slots[3] ^= 0xA5A55A5A10010001ull;
			Log(EViolationSeverity::Critical, EViolationType::FratelloSelfTamperDetected,
				"Developer self-test: forced canary violation");
			RunSelfProtectionGuard(std::vector<SModuleInfo>(), true);
		}
	}
};

#endif // ENABLE_FRATELLO_SECURITY

CFratelloSecurity::CFratelloSecurity()
	: m_pImpl(nullptr)
{
#ifdef ENABLE_FRATELLO_SECURITY
	m_pImpl = new SImpl();
#endif
}

CFratelloSecurity::~CFratelloSecurity()
{
#ifdef ENABLE_FRATELLO_SECURITY
	delete m_pImpl;
	m_pImpl = nullptr;
#endif
}

CFratelloSecurity& CFratelloSecurity::Instance()
{
	static CFratelloSecurity s_instance;
	return s_instance;
}

bool CFratelloSecurity::InitializePreStart()
{
#ifdef ENABLE_FRATELLO_SECURITY
	if (!m_pImpl)
		return false;

	std::lock_guard<std::mutex> lock(m_pImpl->mutex);
	if (m_pImpl->preStartInitialized)
		return m_pImpl->healthy.load(std::memory_order_acquire);

	m_pImpl->processId = GetCurrentProcessId();
	m_pImpl->mainThreadId = GetCurrentThreadId();
	if (!m_pImpl->CollectEnvironmentPaths())
	{
		m_pImpl->Log(EViolationSeverity::Critical, EViolationType::FratelloSelfTamperDetected,
			"Unable to resolve process/environment paths");
		m_pImpl->healthy.store(false, std::memory_order_release);
		return false;
	}

	m_pImpl->selfTestViolationFired = false;
	m_pImpl->selfTestDiagnosticsRan = false;
	m_pImpl->executableMemoryBaseline.clear();

	m_pImpl->ApplyProcessMitigationHardening();
	m_pImpl->InitializeCanary();
	m_pImpl->RegisterOwnCriticalRegions();
	m_pImpl->BuildModuleBaseline();
	m_pImpl->BuildThreadBaseline();
	m_pImpl->SnapshotMainTextSection();
	m_pImpl->SnapshotExecutableFile();
	m_pImpl->SnapshotImportAddressTable();
	m_pImpl->SnapshotTimerApiIntegrity();
	m_pImpl->SnapshotCriticalRegions();

	const uint64_t now = GetMsTick();
	m_pImpl->startupAtMs = now;
	m_pImpl->lastFastScanAtMs = now;
	m_pImpl->lastMediumScanAtMs = now;
	m_pImpl->lastFullScanAtMs = m_pImpl->lastFastScanAtMs;
	m_pImpl->lastThreadScanAtMs = m_pImpl->lastFastScanAtMs;
	m_pImpl->preStartInitialized = true;
	m_pImpl->shutdown = false;
	m_pImpl->closingRequested = false;

	std::vector<SImpl::SModuleInfo> modules;
	if (m_pImpl->EnumerateModules(modules))
	{
		m_pImpl->BuildExecutableMemoryBaseline(modules);
		m_pImpl->CheckDebuggerGuard();
		m_pImpl->CheckModuleGuard(modules);
		if (!m_pImpl->VerifyTimerApiIntegrity(modules))
			m_pImpl->ApplyRiskAndMaybeClose(EViolationType::TimingManipulationDetected, EViolationSeverity::Critical);
		m_pImpl->CheckMemoryGuard(modules, true);
		m_pImpl->RunImportGuard(modules);
		m_pImpl->RunPythonGuard(modules);
		m_pImpl->RunSelfProtectionGuard(modules, true);
	}
	else
	{
		m_pImpl->Log(EViolationSeverity::High, EViolationType::ModuleInjected,
			"PreStart module enumeration failed");
		m_pImpl->ApplyRiskAndMaybeClose(EViolationType::ModuleInjected, EViolationSeverity::High);
	}

	m_pImpl->Log(EViolationSeverity::Info, EViolationType::ModuleInjected, "PreStart initialized");
	return m_pImpl->healthy.load(std::memory_order_acquire);
#else
	return true;
#endif
}

bool CFratelloSecurity::InitializePostStart()
{
#ifdef ENABLE_FRATELLO_SECURITY
	if (!m_pImpl)
		return false;

	std::lock_guard<std::mutex> lock(m_pImpl->mutex);
	if (!m_pImpl->preStartInitialized)
		return false;
	if (m_pImpl->postStartInitialized)
		return m_pImpl->healthy.load(std::memory_order_acquire);

	m_pImpl->RegisterApplicationCriticalRegions();
	m_pImpl->SnapshotCriticalRegions();

	std::vector<SImpl::SModuleInfo> modules;
	if (m_pImpl->EnumerateModules(modules))
	{
		for (const SImpl::SModuleInfo& module : modules)
			m_pImpl->moduleBaseline.emplace(m_pImpl->ModuleIdentity(module), module);

		m_pImpl->BuildExecutableMemoryBaseline(modules);
		m_pImpl->CheckModuleGuard(modules);
		m_pImpl->RunImportGuard(modules);
		m_pImpl->RunPythonGuard(modules);
		m_pImpl->RunSelfProtectionGuard(modules, true);
	}

	m_pImpl->BuildThreadBaseline();
	m_pImpl->postStartInitialized = true;
	m_pImpl->Log(EViolationSeverity::Info, EViolationType::ModuleInjected, "PostStart initialized");
	return m_pImpl->healthy.load(std::memory_order_acquire);
#else
	return true;
#endif
}

void CFratelloSecurity::Update()
{
#ifdef ENABLE_FRATELLO_SECURITY
	if (!m_pImpl)
		return;

	bool canRun = false;
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		if (!m_pImpl->preStartInitialized || m_pImpl->shutdown)
			return;

		++m_pImpl->frameCounter;
		m_pImpl->RunInternalSelfTests();
		m_pImpl->CheckTimingGuard();
		canRun = true;
	}

	if (canRun)
		RunBackgroundScan();

	if (!IsHealthy())
		CloseClientSecurely(true);
#endif
}

void CFratelloSecurity::Shutdown()
{
#ifdef ENABLE_FRATELLO_SECURITY
	if (!m_pImpl)
		return;

	std::lock_guard<std::mutex> lock(m_pImpl->mutex);
	if (m_pImpl->shutdown)
		return;

	m_pImpl->shutdown = true;
	m_pImpl->Log(EViolationSeverity::Info, EViolationType::ModuleInjected, "Shutdown completed");
#endif
}

bool CFratelloSecurity::IsHealthy() const
{
#ifdef ENABLE_FRATELLO_SECURITY
	return m_pImpl && m_pImpl->healthy.load(std::memory_order_acquire);
#else
	return true;
#endif
}

void CFratelloSecurity::RunFastScan()
{
#ifdef ENABLE_FRATELLO_SECURITY
	if (!m_pImpl)
		return;

	std::lock_guard<std::mutex> lock(m_pImpl->mutex);
	if (!m_pImpl->preStartInitialized || m_pImpl->shutdown)
		return;

	std::vector<SImpl::SModuleInfo> modules;
	if (!m_pImpl->EnumerateModules(modules))
	{
		m_pImpl->Log(EViolationSeverity::Suspicious, EViolationType::ModuleInjected, "FastScan: module enumeration failed");
		return;
	}

	m_pImpl->CheckDebuggerGuard();
	m_pImpl->CheckModuleGuard(modules);
	if (!m_pImpl->VerifyTimerApiIntegrity(modules))
	{
		m_pImpl->ApplyRiskAndMaybeClose(EViolationType::TimingManipulationDetected, EViolationSeverity::Critical);
		return;
	}
	m_pImpl->RunSelfProtectionGuard(modules, true);
	m_pImpl->CheckTimingGuard();
	m_pImpl->CheckBotBehaviorGuard();
	m_pImpl->RunPythonGuard(modules);
#endif
}

void CFratelloSecurity::RunFullScan()
{
#ifdef ENABLE_FRATELLO_SECURITY
	if (!m_pImpl)
		return;

	std::lock_guard<std::mutex> lock(m_pImpl->mutex);
	if (!m_pImpl->preStartInitialized || m_pImpl->shutdown)
		return;

	std::vector<SImpl::SModuleInfo> modules;
	if (!m_pImpl->EnumerateModules(modules))
	{
		m_pImpl->Log(EViolationSeverity::High, EViolationType::ModuleInjected, "FullScan: module enumeration failed");
		m_pImpl->ApplyRiskAndMaybeClose(EViolationType::ModuleInjected, EViolationSeverity::High);
		return;
	}

	m_pImpl->CheckModuleGuard(modules);
	m_pImpl->CheckMemoryGuard(modules, true);
	if (!m_pImpl->VerifyTimerApiIntegrity(modules))
	{
		m_pImpl->ApplyRiskAndMaybeClose(EViolationType::TimingManipulationDetected, EViolationSeverity::Critical);
		return;
	}
	m_pImpl->RunImportGuard(modules);
	m_pImpl->RunPythonGuard(modules);
	m_pImpl->RunSectionIntegrityGuard();
	m_pImpl->RunFileIntegrityGuard();
	m_pImpl->RunSelfProtectionGuard(modules, false);

	const uint64_t now = GetMsTick();
	if (now - m_pImpl->lastThreadScanAtMs >= kThreadScanIntervalMs)
	{
		m_pImpl->CheckThreadGuard(modules);
		m_pImpl->lastThreadScanAtMs = now;
	}
#endif
}

void CFratelloSecurity::RunBackgroundScan()
{
#ifdef ENABLE_FRATELLO_SECURITY
	if (!m_pImpl)
		return;

	const uint64_t now = GetMsTick();
	uint64_t fastInterval = kFastScanIntervalMs;
	uint64_t mediumInterval = kMediumScanIntervalMs;
	uint64_t fullInterval = kFullScanIntervalMs;

	bool runFast = false;
	bool runMedium = false;
	bool runFull = false;
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		if (!m_pImpl->preStartInitialized || m_pImpl->shutdown)
			return;

		if (m_pImpl->IsInEarlyProtectionWindow(now) || m_pImpl->IsAdaptiveScrutinyActive(now))
		{
			fastInterval = kFastScanEarlyIntervalMs;
			mediumInterval = kMediumScanEarlyIntervalMs;
			fullInterval = kFullScanEarlyIntervalMs;
		}

		if (now - m_pImpl->lastFastScanAtMs >= fastInterval)
		{
			m_pImpl->lastFastScanAtMs = now;
			runFast = true;
		}

		if (now - m_pImpl->lastMediumScanAtMs >= mediumInterval)
		{
			m_pImpl->lastMediumScanAtMs = now;
			runMedium = true;
		}

		if (now - m_pImpl->lastFullScanAtMs >= fullInterval)
		{
			m_pImpl->lastFullScanAtMs = now;
			runFull = true;
		}
	}

	if (runFast)
		RunFastScan();

	if (runMedium)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		if (m_pImpl->preStartInitialized && !m_pImpl->shutdown)
		{
			std::vector<SImpl::SModuleInfo> modules;
			if (m_pImpl->EnumerateModules(modules))
			{
				m_pImpl->CheckModuleGuard(modules);
				m_pImpl->CheckMemoryGuard(modules, false);
				if (!m_pImpl->VerifyTimerApiIntegrity(modules))
				{
					m_pImpl->ApplyRiskAndMaybeClose(EViolationType::TimingManipulationDetected, EViolationSeverity::Critical);
					return;
				}
				m_pImpl->RunPythonGuard(modules);
				m_pImpl->RunSelfProtectionGuard(modules, true);
				m_pImpl->CheckTimingGuard();

				if (now - m_pImpl->lastThreadScanAtMs >= kThreadScanIntervalMs)
				{
					m_pImpl->CheckThreadGuard(modules);
					m_pImpl->lastThreadScanAtMs = now;
				}
			}
			else
			{
				m_pImpl->Log(EViolationSeverity::Suspicious, EViolationType::ModuleInjected, "MediumScan: module enumeration failed");
			}
		}
	}

	if (runFull)
	{
		RunFullScan();
	}
#endif
}

void CFratelloSecurity::TriggerViolation(EViolationType type, EViolationSeverity severity, const char* details)
{
#ifdef ENABLE_FRATELLO_SECURITY
	if (!m_pImpl)
		return;

	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		if (details && details[0])
			m_pImpl->Log(severity, type, "%s", details);
		else
			m_pImpl->Log(severity, type, "Violation signaled");
		m_pImpl->ApplyRiskAndMaybeClose(type, severity);
	}

	if (!IsHealthy())
		CloseClientSecurely(true);
#else
	(void)type;
	(void)severity;
	(void)details;
#endif
}

void CFratelloSecurity::ShowSecurityPopup(bool preferGerman)
{
#ifdef ENABLE_FRATELLO_SECURITY
	(void)preferGerman;
	if (!m_pImpl)
		return;

	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		if (m_pImpl->popupShown)
			return;
		m_pImpl->popupShown = true;
	}

	SSecurityPopupModel popup{};
	popup.title = "Security Shield";
	popup.line1 = "Unauthorized modification detected.";
	popup.line2 = "The client will now close to protect the game environment.";
	popup.line3 = "";
	popup.action = "Secured session termination in progress";

	if (!ShowShieldPopupWindowThreaded(popup))
	{
		const char* fallbackEnglish =
			"Security Shield\n"
			"Unauthorized modification detected.\n"
			"The client will now close to protect the game environment.";
		MessageBoxA(nullptr, fallbackEnglish, "Security Shield", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SYSTEMMODAL);
	}
#else
	(void)preferGerman;
#endif
}

void CFratelloSecurity::CloseClientSecurely(bool immediate)
{
#ifdef ENABLE_FRATELLO_SECURITY
	if (!m_pImpl)
		return;

	bool shouldClose = false;
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		if (m_pImpl->closingRequested)
			return;
		m_pImpl->closingRequested = true;
		shouldClose = true;
	}

	if (!shouldClose)
		return;

	const bool preferGerman = LocaleService_IsEUROPE();

	SHideProcessWindowContext hideContext{};
	hideContext.processId = GetCurrentProcessId();
	hideContext.excludeWindow = nullptr;
	EnumWindows(&HideProcessWindowsEnumProc, reinterpret_cast<LPARAM>(&hideContext));

	ShowSecurityPopup(preferGerman);

	PostQuitMessage(0);
	if (m_pImpl->mainThreadId != 0)
		PostThreadMessage(m_pImpl->mainThreadId, WM_QUIT, 0, 0);

	if (immediate)
	{
		// Critical tamper can leave Python/D3D state unsafe for normal teardown.
		// End only this client process after the user-facing shield was shown.
		Sleep(150);
		ExitProcess(0);
	}
#else
	(void)immediate;
#endif
}
