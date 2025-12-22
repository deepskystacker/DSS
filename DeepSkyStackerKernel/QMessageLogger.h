#pragma once

namespace
{
	QtMessageHandler originalHandler;
	void qtMessageLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg)
	{
		constexpr size_t bufferSize = 4096;
		char name[bufferSize] = { '\0' };

		const QByteArray localMsg = msg.toLocal8Bit();
		const char* file = context.file != nullptr ? context.file : "";
		const size_t fnLength = strlen(file);

		// If the length of the file path is greater than our buffer size:
		// then copy only the last characters of the path that fit into the buffer.
		std::strncpy(name, fnLength >= bufferSize ? (file + fnLength - bufferSize + 1) : file, bufferSize - 1);
		if (0 != strlen(name))
		{
			fs::path path{ name };
			std::strncpy(name, path.filename().string().c_str(), bufferSize - 1); // It's only the filename (without path), this will for sure fit into the buffer.
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
		default: break;
		}
		originalHandler(type, context, msg);
	}
}
