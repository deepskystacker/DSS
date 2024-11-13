// DeepSkyStackerCL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#if defined(Q_OS_WIN) && !defined(NDEBUG)
//
// Visual Leak Detector
#include <vld.h>
#endif

#include <QtLogging>
#include <QImageReader>
#include "DeepSkyStackerCL.h"
#include "progressconsole.h"
#include "FrameList.h"
#include "StackingEngine.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "tracecontrol.h"
#include "ztrace.h"

//
// Set up tracing and manage trace file deletion
//
DSS::TraceControl traceControl{ std::source_location::current().file_name() };

namespace
{
	QtMessageHandler originalHandler;
	void qtMessageLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg)
	{
		QByteArray localMsg = msg.toLocal8Bit();
		const char* file = context.file ? context.file : "";
		char* name{ static_cast<char*>(_alloca(1 + strlen(file))) };
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

DeepSkyStackerCommandLine::DeepSkyStackerCommandLine(int& argc, char** argv) :
	QCoreApplication(argc, argv),
	m_consoleOut{ stdout }
{
	ZFUNCTRACE_RUNTIME();
	DSSBase::setInstance(this);
}

bool DeepSkyStackerCommandLine::Run()
{
	ConsoleOut() << "DeepSkyStacker " << VERSION_DEEPSKYSTACKER << " Command Line" << Qt::endl << Qt::endl;
	if (!DecodeCommandLine())
	{
		OutputCommandLineHelp();
		return false;
	}

	StackingTask* task = new StackingTask(this, Process, GetStackingParams(), ConsoleOut());
	connect(task, &StackingTask::finished, this, &DeepSkyStackerCommandLine::quit);
	QTimer::singleShot(0, task, SLOT(run()));
	exec();

	return true;
}

void DeepSkyStackerCommandLine::reportError(const QString& message, [[maybe_unused]] const QString& type, [[maybe_unused]] Severity severity, [[maybe_unused]] Method method, bool terminate)
{
	if (terminate) traceControl.setDeleteOnExit(false);
	std::cerr << message.toUtf8().constData() << std::endl;
	if (terminate) QCoreApplication::exit(1);
}

void DeepSkyStackerCommandLine::Process(StackingParams& stackingParams, QTextStream& consoleOut)
{
	DSS::ProgressConsole progress(stackingParams.GetTerminalMode());
	DSS::FrameList frameList;
	bool bContinue = true;
	bool bUseFits = stackingParams.IsOptionSet(StackingParams::eStackingOption::FITS_OUTPUT);

	if (stackingParams.IsOptionSet(StackingParams::eStackingOption::REGISTER) && stackingParams.IsOptionSet(StackingParams::eStackingOption::STACKING))
		consoleOut << "Registering and stacking from file list: ";
	else if (stackingParams.IsOptionSet(StackingParams::eStackingOption::REGISTER))
		consoleOut << "Registering from file list: ";
	else
		consoleOut << "Stacking from file list: ";
	consoleOut << stackingParams.GetFileList() << Qt::endl;

	if (stackingParams.IsOptionSet(StackingParams::eStackingOption::REGISTER))
		consoleOut << "Register again already registered light frames: " << (stackingParams.IsOptionSet(StackingParams::eStackingOption::FORCE_REGISTER) ? "yes" : "no") << Qt::endl;
	consoleOut << Qt::endl;

	frameList.loadFilesFromList(QDir(stackingParams.GetFileList()).filesystemAbsolutePath());

	CAllStackingTasks tasks;
	frameList.fillTasks(tasks);
	tasks.ResolveTasks();

	// Open list file
	if (stackingParams.IsOptionSet(StackingParams::eStackingOption::REGISTER) || !frameList.countUnregisteredCheckedLightFrames())
	{
		// Register checked light frames
		CRegisterEngine	RegisterEngine;
		RegisterEngine.OverrideIntermediateFileFormat(bUseFits ? IFF_FITS : IFF_TIFF);
		bContinue = RegisterEngine.RegisterLightFrames(tasks, frameList.getReferenceFrame(), stackingParams.IsOptionSet(StackingParams::eStackingOption::FORCE_REGISTER), &progress);
	}
	if (stackingParams.IsOptionSet(StackingParams::eStackingOption::STACKING) && bContinue)
	{
		// Stack register light frames
		CStackingEngine StackingEngine;
		std::shared_ptr<CMemoryBitmap> pBitmap;

		StackingEngine.SetSaveIntermediate(stackingParams.IsOptionSet(StackingParams::eStackingOption::SAVE_INTERMEDIATE));
		StackingEngine.SetSaveCalibrated(stackingParams.IsOptionSet(StackingParams::eStackingOption::SAVE_CALIBRATED));
		StackingEngine.OverrideIntermediateFileFormat(bUseFits ? IFF_FITS : IFF_TIFF);
		bContinue = StackingEngine.StackLightFrames(tasks, &progress, pBitmap);
		if (bContinue)
		{
			fs::path outputPath(stackingParams.GetOutputFilename().toStdU16String());
			if (outputPath.empty())
				StackingEngine.GetDefaultOutputFileName(outputPath, stackingParams.GetFileList().toStdU16String().c_str(), !bUseFits);

			stackingParams.SetOutputFile(QString::fromStdU16String(outputPath.generic_u16string().c_str()));
			StackingEngine.WriteDescription(tasks, outputPath);
			SaveBitmap(stackingParams, pBitmap);
		}
	}
	consoleOut << Qt::endl;
}

bool DeepSkyStackerCommandLine::DecodeCommandLine()
{
	bool bResult = false;
	std::int32_t i = 0;
	const QStringList vCommandLine = arguments();

	// At least 2 arguments are needed (registering and/or stacking + filename)
	// The first is always the program name, so skip that and start at position 1.
	bResult = (vCommandLine.size() >= 2);
	for (i = 1; i < vCommandLine.size() && bResult; i++)
	{
		if (!vCommandLine[i].compare("/s", Qt::CaseInsensitive))
		{
			SetOption(StackingParams::eStackingOption::STACKING);
		}
		else if (!vCommandLine[i].compare("/SR", Qt::CaseInsensitive))
		{
			SetOption(StackingParams::eStackingOption::STACKING);
			SetOption(StackingParams::eStackingOption::SAVE_INTERMEDIATE);
		}
		else if (!vCommandLine[i].compare("/SC", Qt::CaseInsensitive))
		{
			SetOption(StackingParams::eStackingOption::STACKING);
			SetOption(StackingParams::eStackingOption::SAVE_CALIBRATED);
		}
		else if (!vCommandLine[i].compare("/FITS", Qt::CaseInsensitive))
		{
			SetOption(StackingParams::eStackingOption::FITS_OUTPUT);
		}
		else if (!vCommandLine[i].compare("/r", Qt::CaseInsensitive))
		{
			SetOption(StackingParams::eStackingOption::REGISTER);
			if (!vCommandLine[i].compare("/R", Qt::CaseSensitive))
				SetOption(StackingParams::eStackingOption::FORCE_REGISTER);
		}
		else if (!vCommandLine[i].left(3).compare("/O:", Qt::CaseInsensitive))
		{
			// Check output file name
			const QString outputFile(vCommandLine[i].right(vCommandLine[i].length() - 3));
			GetStackingParams().SetOutputFile(outputFile);
			if (!checkFileCanBeWrittenTo(outputFile))
			{
				ConsoleOut() << "Cannot write to " << outputFile << " (not a valid filename)" << Qt::endl;
				bResult = false;
			}
		}
		else if (!vCommandLine[i].left(3).compare("/OF", Qt::CaseInsensitive))
		{
			QString strFormat(vCommandLine[i].right(vCommandLine[i].length() - 3));
			if (strFormat == "16" || strFormat == "16i")
				GetStackingParams().SetTiffFormat(TF_16BITRGB);
			else if (strFormat == "32" || strFormat == "32i")
				GetStackingParams().SetTiffFormat(TF_32BITRGB);
			else if (strFormat == "32r")
				GetStackingParams().SetTiffFormat(TF_32BITRGBFLOAT);
			else
			{
				ConsoleOut() << "Unrecognized or unsupported bit format " << strFormat << Qt::endl;
				bResult = false;
			}
		}
		else if (!vCommandLine[i].left(3).compare("/OC", Qt::CaseInsensitive))
		{
			QString strCompression(vCommandLine[i].right(vCommandLine[i].length() - 3));
			if (strCompression == "0")
				GetStackingParams().SetTiffCompression(TC_NONE);
			else if (strCompression == "1")
				GetStackingParams().SetTiffCompression(TC_LZW);
			else if (strCompression == "2")
				GetStackingParams().SetTiffCompression(TC_DEFLATE);
			else
			{
				ConsoleOut() << "Unrecognized or unsupported compression format " << strCompression << Qt::endl;
				bResult = false;
			}
		}
 		else if (!vCommandLine[i].left(3).compare("/OS", Qt::CaseInsensitive))
 		{
 			QString strOutputMode(vCommandLine[i].right(vCommandLine[i].length() - 3));
 			if (strOutputMode == "0")
 				GetStackingParams().SetOutputStyle(TERMINAL_OUTPUT_MODE::BASIC);
 			else if (strOutputMode == "1")
 				GetStackingParams().SetOutputStyle(TERMINAL_OUTPUT_MODE::COLOURED);
 			else
 			{
 				ConsoleOut() << "Unrecognized or unsupported output format " << strOutputMode << Qt::endl;
 				bResult = false;
 			}
 		}
		else
		{
			QString fileList(vCommandLine[i]);
			if (!fileList.isEmpty() && fileExists(fileList))
			{
				GetStackingParams().SetFileList(fileList);
			}
			else
			{
				bResult = false;
				ConsoleOut() << "File list file does not exist [" << fileList << "]" << Qt::endl;
			}
		}
	}

	if (!GetStackingParams().IsOptionSet(StackingParams::eStackingOption::STACKING) && 
		!GetStackingParams().IsOptionSet(StackingParams::eStackingOption::REGISTER)
		)
		bResult = false;

	return bResult;
}

void DeepSkyStackerCommandLine::OutputCommandLineHelp()
{

	ConsoleOut() << Qt::endl;
	ConsoleOut() << Qt::endl;
	ConsoleOut() << "Syntax is DeepSkyStackerCL [/r|R] [/s] [/O:<>] [/OFxx] [/OCx] [/FITS] <ListFileName>" << Qt::endl;
	ConsoleOut() << Qt::endl;
	ConsoleOut() << " /r	        - Register frames (only the ones not already registered)" << Qt::endl;
	ConsoleOut() << " /R            - Register frames (even the ones already registered)" << Qt::endl;
	ConsoleOut() << " /S            - Stack frames" << Qt::endl;
	ConsoleOut() << " /SR           - Save each registered and calibrated light frame in" << Qt::endl;
	ConsoleOut() << "                 output file format (implies /S)" << Qt::endl;
	ConsoleOut() << " /SC           - Save each calibrated light frame in" << Qt::endl;
	ConsoleOut() << "                 output file format (implies /S)" << Qt::endl;
	ConsoleOut() << " /O:<filename> - Output file name (full path)" << Qt::endl;
	ConsoleOut() << "                 Default is Autosave.tif in the folder of the first light frame" << Qt::endl;
	ConsoleOut() << "                 (implies /S or /SC)" << Qt::endl;
	ConsoleOut() << " /OFxxx        - Output format (16 or 32 bits depth)" << Qt::endl;
	ConsoleOut() << "                 16 or 16i: 16 bits integer" << Qt::endl;
	ConsoleOut() << "                 32 or 32i: 32 bits integer" << Qt::endl;
	ConsoleOut() << "                 32r: 32 bits rational (default)" << Qt::endl;
	ConsoleOut() << " /OCx          - Output file compression" << Qt::endl;
	ConsoleOut() << "                 0: no compression (default)" << Qt::endl;
	ConsoleOut() << "                 1: LZW compression" << Qt::endl;
	ConsoleOut() << "                 2: ZIP (Deflate) compression" << Qt::endl;
	ConsoleOut() << " /OSx          - Output style (if terminal supports it)" << Qt::endl;
	ConsoleOut() << "                 0: simple (default)" << Qt::endl;
	ConsoleOut() << "                 1: colored" << Qt::endl;
	ConsoleOut() << " /FITS         - Override format of output files to be FITS (default is TIFF)" << Qt::endl;
	ConsoleOut() << "<ListFileName> - Name of a file list saved by DeepSkyStacker" << Qt::endl;
	ConsoleOut() << Qt::endl;
	ConsoleOut() << "Examples:" << Qt::endl;
	ConsoleOut() << "DeepSkyStackerCL /r /OS1 c:\\MyLists\\SampleList.txt" << Qt::endl;
	ConsoleOut() << "  will register all the checked light frames of the list" << Qt::endl;
	ConsoleOut() << "  and the output on the commandline will be colored" << Qt::endl;
	ConsoleOut() << Qt::endl;
	ConsoleOut() << "DeepSkyStackerCL /r /s /OF16i c:\\MyLists\\SampleList.txt" << Qt::endl;
	ConsoleOut() << "  will register then stack all the checked light frames of the list" << Qt::endl;
	ConsoleOut() << "  and save the result in a 16 bits integer TIFF file named" << Qt::endl;
	ConsoleOut() << "  autosave.tif in the folder of the first light frame" << Qt::endl;
	ConsoleOut() << Qt::endl;
}
/* ------------------------------------------------------------------- */

bool DeepSkyStackerCommandLine::fileExists(const QString& path)
{
	QFileInfo check_file(path);

	// check if file exists and if yes: Is it really a file and no directory?
	if (check_file.exists() && check_file.isFile())
		return true;
	return false;
}

bool DeepSkyStackerCommandLine::checkFileCanBeWrittenTo(const QString& path)
{
	QFile file(path);
	bool bAlreadyExsits = file.exists();
	if (!bAlreadyExsits)
		return true;

	if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
		return false;

	file.close();
	return true;
}

void DeepSkyStackerCommandLine::SaveBitmap(StackingParams& stackingParams, const std::shared_ptr<CMemoryBitmap> pBitmap)
{
	if (!(pBitmap && !stackingParams.GetOutputFilename().isEmpty()))
		return;

	DSS::ProgressConsole progress(stackingParams.GetTerminalMode());

	const QString strText(QCoreApplication::translate("DeepSkyStackerCL", "Saving Final image in %1", "IDS_SAVINGFINAL").arg(stackingParams.GetOutputFilename()));
	progress.Start1(strText, 0);
	progress.Start2(strText, 0);

	bool bMonochrome = pBitmap->IsMonochrome();
	if (stackingParams.IsOptionSet(StackingParams::eStackingOption::FITS_OUTPUT))
	{
		FITSFORMAT fitsformat = FF_UNKNOWN;
		switch (stackingParams.GetTiffFormat())
		{
		case TF_16BITRGB:
			fitsformat = bMonochrome ? FF_16BITGRAY : FF_16BITRGB;
			break;
		case TF_32BITRGB:
			fitsformat = bMonochrome ? FF_32BITGRAY : FF_32BITRGB;
			break;
		case TF_32BITRGBFLOAT:
			fitsformat = bMonochrome ? FF_32BITGRAYFLOAT : FF_32BITRGBFLOAT;
			break;
		}
		WriteFITS(stackingParams.GetOutputFilename().toStdU16String(), pBitmap.get(), &progress, fitsformat, nullptr);
	}
	else
	{
		TIFFFORMAT tiffFormat = stackingParams.GetTiffFormat();
		if (bMonochrome)
		{
			switch (tiffFormat)
			{
			case TF_16BITRGB:
				tiffFormat = TF_16BITGRAY;
				break;
			case TF_32BITRGB:
				tiffFormat = TF_32BITGRAY;
				break;
			case TF_32BITRGBFLOAT:
			default:
				tiffFormat = TF_32BITGRAYFLOAT;
				break;
			}
		}
		WriteTIFF(stackingParams.GetOutputFilename().toStdU16String(), pBitmap.get(), &progress, tiffFormat, stackingParams.GetTiffCompression(), nullptr);
	}
}

/* ------------------------------------------------------------------- */

int main(int argc, char* argv[])
{
	ZFUNCTRACE_RUNTIME();

#if defined(Q_OS_WIN)
	// Set console code page to UTF-8 so console knows how to interpret string data
	SetConsoleOutputCP(CP_UTF8);
#endif

	//
	// Log Qt messages to the trace file as well as to the debugger.
	//
	originalHandler = qInstallMessageHandler(qtMessageLogger);

	//
	// Silence the windows heap checker as we use Visual Leak Detector
	//
#if defined(Q_OS_WIN)
	_CrtSetDbgFlag(0);
#endif

	Exiv2::XmpParser::initialize();
	::atexit(Exiv2::XmpParser::terminate);

	//
	// Increase maximum size of QImage from the default of 128MB to 1GB
	//
	constexpr int oneGB{ 1024 * 1024 * 1024 };
	QImageReader::setAllocationLimit(oneGB);

	DeepSkyStackerCommandLine process(argc, argv);

	process.Run();

	return 0;
}