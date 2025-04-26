#include "pch.h"
#include "ztrace.h"
#include "tracecontrol.h"
#include "dssbase.h"

extern std::unique_ptr<std::uint8_t[]> backPocket;
extern DSS::TraceControl traceControl;
//extern char const* global_program_name;

namespace
{
	void writeOutput(const char* text)
	{
		fputs(text, stderr);
		ZTRACE_RUNTIME(text);
	}
}

#if defined(Q_OS_WIN)

#include "StackWalker.h"

namespace {
	class DSSStackWalker : public StackWalker
	{
		bool suppressOutputFlag{ false };
	public:
		DSSStackWalker() : StackWalker{ StackWalker::RetrieveVerbose | StackWalker::SymBuildPath }
		{}
		void suppressOutput(const bool suppress) {
			this->suppressOutputFlag = suppress;
		}
	protected:
		virtual void OnOutput(LPCSTR text) override
		{
			if (suppressOutputFlag)
				return;
			fprintf(stderr, text);
			ZTRACE_RUNTIME(text);
			//		StackWalker::OnOutput(text); // Just OutputDebugString()
		}

		virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD lastErrorCode, DWORD64 address) override
		{
			if (strcmp(szFuncName, "SymGetLineFromAddr64") == 0)
				return;
			StackWalker::OnDbgHelpErr(szFuncName, lastErrorCode, address);
		}
	};

	DSSStackWalker sw;

	thread_local char printBuffer[192];

	const auto printException = [](const char* text, const std::uint32_t exceptionCode, const size_t threadId, const void* rip, const auto* excInfo) -> const char*
	{
		auto printExc = [text, exceptionCode, threadId, rip, excInfo](const char* excName) -> const char*
		{
			constexpr size_t maxSz = sizeof(printBuffer);
			const char* info = (exceptionCode == STATUS_ACCESS_VIOLATION || exceptionCode == STATUS_IN_PAGE_ERROR) ? ((excInfo[0] & 0x01) == 0 ? "READ" : "WRITE") : ("");
			snprintf(printBuffer, maxSz, "%s Thread %" PRIx64 ", ExCode 0x%08x, RIP %p: %s %s\n", text, threadId, exceptionCode, rip, excName, info);
			return printBuffer;
		};
		switch (exceptionCode)
		{
		case STATUS_ACCESS_VIOLATION: return printExc("ACCESS_VIOLATION");
		case STATUS_ARRAY_BOUNDS_EXCEEDED: return printExc("ARRAY_BOUNDS_EXCEEDED");
		case STATUS_DATATYPE_MISALIGNMENT: return printExc("DATATYPE_MISALIGNMENT");
		case STATUS_FLOAT_INVALID_OPERATION: return printExc("FLT_INVALID_OPERATION");
		case STATUS_FLOAT_STACK_CHECK: return printExc("FLT_STACK_CHECK");
		case STATUS_ILLEGAL_INSTRUCTION: return printExc("ILLEGAL_INSTRUCTION");
		case STATUS_IN_PAGE_ERROR: return printExc("IN_PAGE_ERROR");
		case STATUS_INTEGER_DIVIDE_BY_ZERO: return printExc("INT_DIVIDE_BY_ZERO");
		case STATUS_NONCONTINUABLE_EXCEPTION: return printExc("NONCONTINUABLE_EXCEPTION");
		case STATUS_PRIVILEGED_INSTRUCTION: return printExc("PRIV_INSTRUCTION");
		case STATUS_STACK_OVERFLOW: return printExc("STACK_OVERFLOW");
		case STATUS_INVALID_HANDLE: return printExc("INVALID_HANDLE");
		case STATUS_NO_MEMORY: return printExc("NO_MEMORY");
		case STATUS_CONTROL_STACK_VIOLATION: return printExc("CONTROL_STACK_VIOLATION");
		case STATUS_HEAP_CORRUPTION: return printExc("HEAP_CORRUPTION");
		case STATUS_STACK_BUFFER_OVERRUN: return printExc("STACK_BUFFER_OVERRUN");
//		case STATUS_ASSERTION_FAILURE: return printExc("ASSERTION_FAILURE");
		default: return nullptr;
		}
		// return nullptr;
	};

	std::atomic<std::uint32_t> barrier{ 0 };
	std::thread::id currentThreadId{};

	void traceTheStack(const bool suppressOutput, auto pContext)
	{
		sw.suppressOutput(suppressOutput);
		if constexpr (std::is_same_v<decltype(pContext), nullptr_t>)
			sw.ShowCallstack();
		else
			sw.ShowCallstack(GetCurrentThread(), pContext);
		sw.suppressOutput(false);
	}

	long WINAPI DssCriticalExceptionHandler(EXCEPTION_POINTERS* pExc)
	{
		constexpr auto returnCode = EXCEPTION_CONTINUE_SEARCH; // should show the error pop-up message box
		const EXCEPTION_RECORD* exc = pExc->ExceptionRecord;
		const std::uint32_t excCode = exc->ExceptionCode;
		const void* rip = exc->ExceptionAddress;
		const auto* excInfo = exc->ExceptionInformation;
		const auto thisThreadId = std::this_thread::get_id();
		const size_t myThreadId = std::hash<std::thread::id>{}(thisThreadId);

		if (const char* str = printException("ExH -", excCode, myThreadId, rip, excInfo); str != nullptr)
			writeOutput(str);
		else // don't care about the exception
			return returnCode;

		if (barrier.exchange(1) == 0) // atomic::exchange() returns the value before the call, so we are the first one.
		{
			traceControl.setDeleteOnExit(false); // Here we know there's a serious situation, so we prevent deletion of the trace file.
			currentThreadId = thisThreadId;
			backPocket.reset(nullptr);
			fprintf(stderr, "Thread %" PRIx64 " beginning StackWalk\n", myThreadId);
			if (excCode != EXCEPTION_STACK_OVERFLOW)
				traceTheStack(false, pExc->ContextRecord); // false = do NOT suppress output.
			fprintf(stderr, "Thread %" PRIx64 " finished StackWalk\n", myThreadId);
			fflush(stderr);
			barrier = 0; // Reset to initial value, so that all other waiting threads can resume.
			barrier.notify_all(); // Notify all waiting threads that we finished the stack walk.
			std::terminate();
		}
		else // Another stack walk is done.
		{
			if (currentThreadId == thisThreadId) // Exception while tracing the stack -> there's nothing we can do.
			{
				if (const char* str = printException("Second exception!", excCode, myThreadId, rip, excInfo); str != nullptr)
					writeOutput(str);
				return returnCode;
			}
			barrier.wait(1);
		}

		return returnCode;
	}


} // namespace

void setDssExceptionHandling()
{
	// Add our own vectored exception handler to the front of the handler chain, so it gets called early (ideally first).
	AddVectoredExceptionHandler(1, DssCriticalExceptionHandler);
	traceTheStack(true, nullptr); // Stack tracing to initialise StackWalker and pre-load all libraries, true = output suppressed.
}

#elif defined(Q_OS_LINUX)
#include <csignal>
#include <execinfo.h>
#include <link.h>

namespace
{

	// converts a function's address in memory to its VMA address in the executable file. VMA is what addr2line expects
	std::uintptr_t convertToVMA(void* addr)
	{
		Dl_info info;
		struct link_map* link_map;
		dladdr1((void*)addr, &info, (void**)&link_map, RTLD_DL_LINKMAP);
		return reinterpret_cast<std::uintptr_t>(addr) - link_map->l_addr;
	}


	/* Resolve symbol name and source location given the path to the executable
	   and an address */
	int addr2line(char const* const program_name, std::uintptr_t addr)
	{
		char addr2line_cmd[512] = { '\0' };

		/* have addr2line map the address to the relevant line in the code */
		snprintf(addr2line_cmd, sizeof(addr2line_cmd), "addr2line -f -C -p -e %.256s %p", program_name, (void*)addr);

		/* This will print a nicely formatted string specifying the
		   function and source line of the address */
		FILE* in;
		char buff[512];
		// is this the check for command execution exited with not 0?
		if (!(in = popen(addr2line_cmd, "r"))) {
			// I want to return the exit code and error message too if any
			return 1;
		}
		// this part echoes the output of the command that's executed
		while (fgets(buff, sizeof(buff), in) != NULL)
		{
			::writeOutput(buff);
		}

		int status = pclose(in);
		return WEXITSTATUS(status);
	}

	constexpr size_t MAX_STACK_FRAMES{ 64 };
	static void* stack_trace[MAX_STACK_FRAMES];
	std::atomic<std::uint32_t> barrier{ 0 };

	//
	// This function is NOT THREAD SAFE!
	// Make sure to use barriers or similar before calling it.
	// After the call terminate the process.
	//
	void posix_print_stack_trace()
	{
		char buffer[1024] = { '\0' }; // buffer for error message

		const int trace_size = backtrace(stack_trace, MAX_STACK_FRAMES);

		//
		// Refer to https://stackoverflow.com/questions/56046062/linux-addr2line-command-returns-0
		// for a discussion of why we need to convert the address in the stack trace to call
		// addr2line 
		//

		// Skip the first two stack frames (as they are this function and
		//   our handler), and also skip the last frame as it's (always?) junk.

		// Skip the first two frames on Linux as they don't appear be useful.
		constexpr int skip_frames{ 2 };

		for (int i = skip_frames; i < (trace_size - 1); ++i) // To see the entire entire stack trace, change to: for (i = 0 ...
		{
			Dl_info info;
			if (dladdr(stack_trace[i], &info))
			{
				std::uintptr_t VMA_addr = convertToVMA(stack_trace[i]);
				//
				// Decrement the PC so we point to actual source line in error, not the one
				// following it.
				//
				VMA_addr = VMA_addr - 1;
				//if (addr2line(global_program_name, VMA_addr) != 0)
				if (addr2line(info.dli_fname, VMA_addr) != 0)
				{
					static char* *const messages = backtrace_symbols(stack_trace, trace_size);
					if (messages != nullptr)
					{
						snprintf(buffer, sizeof(buffer) / sizeof(char), "  error determining line # for: %s\n", messages[i]);
						::writeOutput(buffer);
					}
				}
			}
		}
		// No need to free the message buffer, because the process will be terminated now.
//		free(messages);
	}

	void signalHandler(int signal)
	{
		// If we reach this point, we know that something disastrous happened. The process was sent one of the fatal signals (Seg. violation, FP exception, ...).
		// We run the signal handler and then terminate the process.

		// The signal handler can only be executed by a single thread. All other threads must wait.
		if (barrier.exchange(1) == 0) // atomic::exchange() returns the value before the call, so we are the first one.
		{
			backPocket.reset(nullptr); // Release back pocket storage
			traceControl.setDeleteOnExit(false);

			char name[8] = { '\0' };
			switch (signal)
			{
			case SIGINT:
				strcpy(name, "SIGINT");
				break;
			case SIGILL:
				strcpy(name, "SIGILL");
				break;
			case SIGFPE:
				strcpy(name, "SIGFPE");
				break;
			case SIGSEGV:
				strcpy(name, "SIGSEGV");
				break;
			case SIGTERM:
				strcpy(name, "SIGTERM");
				break;
			default:
				snprintf(name, sizeof(name) / sizeof(char), "%d", signal);
			}

			ZTRACE_RUNTIME("In signalHandler(%s)", name);

			posix_print_stack_trace();

			barrier = 0; // Reset to initial value, so that all other waiting threads can resume.
			barrier.notify_all(); // Notify all waiting threads that we finished the stack walk.

			std::exit(1);
		}
		else
		{
			barrier.wait(1);
		}
	}
} // namespace


void setDssExceptionHandling()
{
	std::signal(SIGINT, signalHandler);
	std::signal(SIGILL, signalHandler);
	std::signal(SIGFPE, signalHandler);
	std::signal(SIGSEGV, signalHandler);
	std::signal(SIGTERM, signalHandler);
}
#endif
