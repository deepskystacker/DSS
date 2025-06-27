#include "pch.h"
#include "avx_includes.h"
#include "avx_simd_check.h"
#include "Multitask.h"

#if defined (Q_OS_LINUX)
#include <cpuid.h>
#endif

#if defined (Q_OS_MACOS)
#include <sys/sysctl.h>
#endif


bool AvxSimdCheck::checkAvx2CpuSupport()
{
#if defined (Q_OS_MACOS)  && defined(Q_PROCESSOR_ARM) // OSX builds will be ARM, so will support all AVX versions (incl. AVX2) through emulation with Simde (cf. avx_includes.h).
	return true;
#else
#if defined(Q_OS_WIN) // MSVC verions
	SYSTEM_INFO info;
	GetNativeSystemInfo(&info);
	if (info.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64) // AVX instructions can only be supported on x64 CPUs. 
		return false;

	int cpuid[4] = { -1 };

	__cpuidex(cpuid, 1, 0);
	const bool FMAsupported = ((cpuid[2] & (1 << 12)) != 0);
	const bool POPCNTsupported = ((cpuid[2] & (1 << 23)) != 0);
	const bool XSAVEsupported = ((cpuid[2] & (1 << 26)) != 0);
	const bool OSXSAVEsupported = ((cpuid[2] & (1 << 27)) != 0);

	__cpuidex(cpuid, 7, 0);
	const bool AVX2supported = ((cpuid[1] & (1 << 5)) != 0);
	//const bool BMI1supported = ((cpuid[1] & (1 << 3) != 0); 
	//const bool BMI2supported = ((cpuid[1] & (1 << 8)) != 0); 

	const bool RequiredCpuFlags = FMAsupported && POPCNTsupported && XSAVEsupported && OSXSAVEsupported && AVX2supported;

	// OS supports AVX (YMM registers) - Note: XGETBV may only be executed on CPUs with XSAVE flag set. 
	const bool AVXenabledOS = RequiredCpuFlags ? ((_xgetbv(0) & 6) == 6) : false; // 6 = SSE (0x2) + YMM (0x4) 

	// Additionally set flush to zero and denormals to zero - Note: (S)GETCSR are SSE instructions, so supported by all x64 CPUs. 
	_mm_setcsr(_mm_getcsr() | _MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);

	return (RequiredCpuFlags && AVXenabledOS);

#else // GCC/Linux version (applies to Linux and X86_64 OSX builds)
	bool result = 
		__builtin_cpu_supports("avx2") && 
		__builtin_cpu_supports("fma") &&
		__builtin_cpu_supports("popcnt") &&
		__builtin_cpu_supports("xsave") &&
		__builtin_cpu_supports("osxsave");

	// Additionally set flush to zero and denormals to zero - Note: (S)GETCSR are SSE instructions, so supported by all x64 CPUs. 
	_mm_setcsr(_mm_getcsr() | _MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);

	return result;

#endif
#endif
};

bool AvxSimdCheck::checkSimdAvailability()
{
	static const bool simdAvailable = checkAvx2CpuSupport();
	// If user has disabled SIMD vectorisation (settings dialog) -> return false;
	return Multitask::GetUseSimd() && simdAvailable;
}

#if defined (Q_OS_MACOS)

void AvxSimdCheck::reportCpuType()
{
	char buffer[128] = { '\0' };
	size_t bufferSize = sizeof(buffer);
	std::stringstream outputStrm;

	// Get CPU brand string
	outputStrm << "CPU Type: ";
	if (sysctlbyname("machdep.cpu.brand_string", &buffer, &bufferSize, NULL, 0) == 0)
		outputStrm << buffer << std::endl;
	else
		outputStrm << "Failed to get processor details." << std::endl;

	// Get number of CPU cores
	int coreCount = 0;
	size_t coreSize = sizeof(coreCount);
	if (sysctlbyname("hw.physicalcpu", &coreCount, &coreSize, NULL, 0) == 0)
		outputStrm << "Physical Cores: " << coreCount << std::endl;

	if (sysctlbyname("hw.logicalcpu", &coreCount, &coreSize, NULL, 0) == 0)
		outputStrm << "Logical Cores: " << coreCount << std::endl;

	std::cerr << outputStrm.str();
	ZTRACE_RUNTIME(outputStrm.str());
}

#else

void AvxSimdCheck::reportCpuType()
{
#if defined(Q_OS_WIN) 
	char architecture[8]{ '\0' };
	SYSTEM_INFO info;

	const auto getArchitectureString = [&architecture](const auto architectureId) -> void
	{
		constexpr auto maxSize = sizeof(architecture);
		switch (architectureId)
		{
		case PROCESSOR_ARCHITECTURE_INTEL:
			strcpy_s(architecture, maxSize, "x86");
			break;
		case PROCESSOR_ARCHITECTURE_ARM:
			strcpy_s(architecture, maxSize, "ARM");
			break;
		case PROCESSOR_ARCHITECTURE_IA64:
			strcpy_s(architecture, maxSize, "IA64");
			break;
		case PROCESSOR_ARCHITECTURE_AMD64:
			strcpy_s(architecture, maxSize, "x64");
			break;
		case PROCESSOR_ARCHITECTURE_ARM64:
			strcpy_s(architecture, maxSize, "ARM64");
			break;
		default:
			strcpy_s(architecture, maxSize, "Unknown");
		}
	};

	GetNativeSystemInfo(&info);
	const auto nativeArchitecture = info.wProcessorArchitecture;
	getArchitectureString(nativeArchitecture);

	ZTRACE_RUNTIME("Native processor architecture: %s", architecture);
	std::cerr << "Native processor architecture: " << architecture << std::endl;

	GetSystemInfo(&info);
	if (info.wProcessorArchitecture != nativeArchitecture)
	{
		getArchitectureString(info.wProcessorArchitecture);
		ZTRACE_RUNTIME("Emulated processor architecture: %s", architecture);
		std::cerr << "Emulated processor architecture: " << architecture << std::endl;
	}
#endif 
	int cpuid[4] = { -1 };
#if defined(Q_OS_WIN) 
	__cpuid(cpuid, 0x80000000);
#elif defined(Q_OS_LINUX) 
	__cpuid(0x80000000, cpuid[0], cpuid[1], cpuid[2], cpuid[3]);
#else 
#error "System not supported!" 
#endif 
	const int nExtIds = cpuid[0];
	char brand[64] = { '\0' };
	if (nExtIds >= 0x80000004)
	{
#if defined(Q_OS_WIN) 
		__cpuidex(cpuid, 0x80000002, 0);
		memcpy(brand, cpuid, sizeof(cpuid));
		__cpuidex(cpuid, 0x80000003, 0);
		memcpy(brand + 16, cpuid, sizeof(cpuid));
		__cpuidex(cpuid, 0x80000004, 0);
		memcpy(brand + 32, cpuid, sizeof(cpuid));
#else
		cpuid[3] = cpuid[2] = cpuid[1] = cpuid[0] = 0;
		__cpuid(0x80000002, cpuid[0], cpuid[1], cpuid[2], cpuid[3]);
		memcpy(brand, cpuid, sizeof(cpuid));
		cpuid[3] = cpuid[2] = cpuid[1] = cpuid[0] = 0;
		__cpuid(0x80000003, cpuid[0], cpuid[1], cpuid[2], cpuid[3]);
		memcpy(brand + 16, cpuid, sizeof(cpuid));
		cpuid[3] = cpuid[2] = cpuid[1] = cpuid[0] = 0;
		__cpuid(0x80000004, cpuid[0], cpuid[1], cpuid[2], cpuid[3]);
		memcpy(brand + 32, cpuid, sizeof(cpuid));
#endif
	}
	else
		memcpy(brand, "CPU brand not detected", 22);

	// 
	// Also report this on stderr so if we get a SIGILL the information  
	// will be there along with the exception traceback.  
	// 
	std::cerr << "CPU Type: " << brand << std::endl;
	ZTRACE_RUNTIME("CPU type: %s", brand);
}

#endif
