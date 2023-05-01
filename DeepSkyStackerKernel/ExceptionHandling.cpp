#include "stdafx.h"


#if defined(_WINDOWS)

#include "StackWalker.h"
#include "Ztrace.h"
#include "tracecontrol.h"

extern std::unique_ptr<std::uint8_t[]> backPocket;
extern DSS::TraceControl traceControl;

namespace {

	class DSSStackWalker : public StackWalker
	{
	public:
		DSSStackWalker() : StackWalker{}
		{}
	protected:
		virtual void OnOutput(LPCSTR text) override
		{
			fprintf(stderr, text);
			ZTRACE_RUNTIME(text);
			//		StackWalker::OnOutput(text); // Just OutputDebugString()
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

	void writeOutput(const char* text)
	{
		fputs(text, stderr);
		ZTRACE_RUNTIME(text);
	}

	void traceTheStack()
	{
		sw.ShowCallstack();
	}

	long WINAPI DssCriticalExceptionHandler(EXCEPTION_POINTERS* pExc)
	{
		traceControl.setDeleteOnExit(false);
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

		if (barrier.fetch_add(1) == 0) // We are the first one
		{
			currentThreadId = thisThreadId;
			backPocket.reset();
			fprintf(stderr, "Thread %" PRIx64 " beginning StackWalk\n", myThreadId);
			if (excCode != EXCEPTION_STACK_OVERFLOW)
				traceTheStack();
			fprintf(stderr, "Thread %" PRIx64 " finished StackWalk\n", myThreadId);
			fflush(stderr);
			std::terminate();
		}
		else // another stack walk is done
		{
			if (currentThreadId == thisThreadId) // Exception while tracing the stack -> there's nothing we can do.
			{
				if (const char* str = printException("Second exception!", excCode, myThreadId, rip, excInfo); str != nullptr)
					writeOutput(str);
				return returnCode;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		}

		return returnCode;
	}

} // namespace

void setDssExceptionHandling()
{
	// Add our own vectored exception handler to the front of the handler chain, so it gets called early (ideally first).
	AddVectoredExceptionHandler(1, DssCriticalExceptionHandler);
}

#else

/* Resolve symbol name and source location given the path to the executable
   and an address */
int addr2line(char const* const program_name, void const* const addr)
{
	char addr2line_cmd[512]{ 0 };

	/* have addr2line map the address to the relevant line in the code */
#ifdef __APPLE__
  /* apple does things differently... */
	sprintf(addr2line_cmd, "atos -o %.256s %p", program_name, addr);
#else
	sprintf(addr2line_cmd, "addr2line -f -p -e %.256s %p", program_name, addr);
#endif

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
		writeOutput(buff);
	}
	return WEXITSTATUS(pclose(in));
}

constexpr size_t MAX_STACK_FRAMES{ 64 };
static void* stack_traces[MAX_STACK_FRAMES];
void posix_print_stack_trace()
{
	int i, trace_size = 0;
	char** messages = (char**)NULL;
	char buffer[1024]{};	// buffer for error message


	trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
	messages = backtrace_symbols(stack_traces, trace_size);

	/* skip the first couple stack frames (as they are this function and
	   our handler) and also skip the last frame as it's (always?) junk. */
	   // for (i = 3; i < (trace_size - 1); ++i)
	   // we'll use this for now so you can see what's going on
	for (i = 0; i < trace_size; ++i)
	{
		if (addr2line(global_program_name, stack_traces[i]) != 0)
		{
			snprintf(buffer, sizeof(buffer) / sizeof(char),
				"  error determining line # for: %s\n", messages[i]);
			writeOutput(buffer);
		}

	}
	if (messages) { free(messages); }
}

void signalHandler(int signal)
{
	if (backPocket)
	{
		free(backPocket);
		backPocket = nullptr;
	}

	char name[8]{};
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
	DeepSkyStacker::instance()->close();
}

void setDssExceptionHandling()
{
	std::signal(SIGINT, signalHandler);
	std::signal(SIGILL, signalHandler);
	std::signal(SIGFPE, signalHandler);
	std::signal(SIGSEGV, signalHandler);
	std::signal(SIGTERM, signalHandler);
}

#endif
