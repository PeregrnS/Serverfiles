#pragma once

#include <cstddef>
#include <cstdint>

class CFratelloSecurity
{
public:
	enum class EViolationSeverity : uint8_t
	{
		Info = 0,
		Suspicious = 1,
		High = 2,
		Critical = 3
	};

	enum class EViolationType : uint8_t
	{
		ModuleInjected = 0,
		UnknownModuleLoaded,
		ManualMapSuspected,
		ExecutablePrivateMemory,
		RwxMemoryDetected,
		SuspiciousThread,
		DebuggerDetected,
		CriticalRegionModified,
		InlineHookDetected,
		ImportHookDetected,
		PythonTamperDetected,
		TimingManipulationDetected,
		BotBehaviorDetected,
		HoneypotModified,
		FileIntegrityFailed,
		SectionIntegrityFailed,
		FratelloSelfTamperDetected
	};

public:
	static CFratelloSecurity& Instance();

	bool InitializePreStart();
	bool InitializePostStart();
	void Update();
	void Shutdown();
	bool IsHealthy() const;

	void RunFastScan();
	void RunFullScan();
	void RunBackgroundScan();

	void TriggerViolation(EViolationType type, EViolationSeverity severity, const char* details);
	void ShowSecurityPopup(bool preferGerman);
	void CloseClientSecurely(bool immediate);

private:
	CFratelloSecurity();
	~CFratelloSecurity();
	CFratelloSecurity(const CFratelloSecurity&) = delete;
	CFratelloSecurity& operator=(const CFratelloSecurity&) = delete;

private:
	struct SImpl;
	SImpl* m_pImpl;
};
