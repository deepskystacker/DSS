#include "stdafx.h"

#if defined(_WINDOWS)

#include <errhandlingapi.h>
#include <ZExcept.h>
#include "StackWalker.h"
#include "DeepSkyStacker.h"


extern std::unique_ptr<std::uint8_t[]> backPocket;

namespace {

	class DSSStackWalker : public StackWalker
	{
	public:
		DSSStackWalker() : StackWalker() {}
	protected:
		virtual void OnOutput(LPCSTR text) override
		{
			fprintf(stderr, text);
			ZTRACE_RUNTIME(text);
			StackWalker::OnOutput(text);
		}
	};

	DSSStackWalker sw;

	void writeOutput(const char* text)
	{
		fputs(text, stderr);
		ZTRACE_RUNTIME(text);
	}

	long WINAPI DssVectoredExceptionHandler(EXCEPTION_POINTERS* pointers)
	{
		const EXCEPTION_RECORD* exc = pointers->ExceptionRecord;

		auto traceAndterminate = [](const char* text, const bool stacktrace) -> long
		{
			backPocket.reset();
			writeOutput(text);
			if (stacktrace)
				sw.ShowCallstack();
			fflush(stderr);
			DeepSkyStacker::instance()->close();
			return EXCEPTION_CONTINUE_SEARCH;
		};

		// These are the exceptions we handle. Each of them is so disastrous that the app cannot continue.
		//   In those cases (except STACK_OVERFLOW), we try to print the stack trace.
		// All other exceptions are ignored by our handler.
		switch (exc->ExceptionCode)
		{
		case EXCEPTION_ACCESS_VIOLATION: return traceAndterminate("Error: EXCEPTION_ACCESS_VIOLATION\n", true);
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return traceAndterminate("Error: EXCEPTION_ARRAY_BOUNDS_EXCEEDED\n", true);
		case EXCEPTION_DATATYPE_MISALIGNMENT: return traceAndterminate("Error: EXCEPTION_DATATYPE_MISALIGNMENT\n", true);
		case EXCEPTION_FLT_INVALID_OPERATION: return traceAndterminate("Error: EXCEPTION_FLT_INVALID_OPERATION\n", true);
		case EXCEPTION_FLT_STACK_CHECK: return traceAndterminate("Error: EXCEPTION_FLT_STACK_CHECK\n", true);
		case EXCEPTION_ILLEGAL_INSTRUCTION: return traceAndterminate("Error: EXCEPTION_ILLEGAL_INSTRUCTION\n", true);
		case EXCEPTION_IN_PAGE_ERROR: return traceAndterminate("Error: EXCEPTION_IN_PAGE_ERROR\n", true);
		case EXCEPTION_INT_DIVIDE_BY_ZERO: return traceAndterminate("Error: EXCEPTION_INT_DIVIDE_BY_ZERO\n", true);
		case EXCEPTION_NONCONTINUABLE_EXCEPTION: return traceAndterminate("Error: EXCEPTION_NONCONTINUABLE_EXCEPTION\n", true);
		case EXCEPTION_PRIV_INSTRUCTION: return traceAndterminate("Error: EXCEPTION_PRIV_INSTRUCTION\n", true);
		case EXCEPTION_STACK_OVERFLOW: return traceAndterminate("Error: EXCEPTION_STACK_OVERFLOW\n", false);
		default: break;
		}

		// Ignore the exception and let the normal exception handling continue.
		return EXCEPTION_CONTINUE_SEARCH;
	}
} // namespace

void setDssExceptionHandling()
{
	// Add our own vectored exception handler to the front of the handler chain, so it gets called early (ideally first).
	AddVectoredExceptionHandler(1, DssVectoredExceptionHandler);
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
