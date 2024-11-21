#pragma once

namespace
{
	QtMessageHandler originalHandler;
	void qtMessageLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg)
	{
		static thread_local size_t bufferSize = 2048;
		static thread_local std::unique_ptr<char[]> charBuffer = std::make_unique<char[]>(bufferSize); // 2kB char buffer allocated (thread safe) on first usage of this logger.

		QByteArray localMsg = msg.toLocal8Bit();
		const char* file = context.file != nullptr ? context.file : "";
		if (const size_t len = strlen(file); len >= bufferSize)
		{
			bufferSize = std::max(len + 1, 2 * bufferSize); // Double size of buffer, but at least as large as len.
			charBuffer = std::make_unique<char[]>(bufferSize); // Free the current buffer and allocate the new (larger) one.
		}
		char* const name = charBuffer.get();

		strcpy(name, file);
		if (0 != strlen(name))
		{
			fs::path path{ name };
			strcpy(name, path.filename().string().c_str());
		}

		switch (type) {
		case QtDebugMsg:
			ZTRACE_RUNTIME("Qt Debug: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		case QtInfoMsg:
			ZTRACE_RUNTIME("Qt Info: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		case QtWarningMsg:
			ZTRACE_RUNTIME("Qt Warn: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		case QtCriticalMsg:
			ZTRACE_RUNTIME("Qt Critical: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		case QtFatalMsg:
			ZTRACE_RUNTIME("Qt Fatal: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		}
		originalHandler(type, context, msg);
	}
}
