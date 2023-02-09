// DeepSkyStackerCL.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>
#include "QtProgressConsole.h"
#include "FrameList.h"
#include "StackingEngine.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "SetUILanguage.h"

class StackingParams
{
public:
	StackingParams() :
		registering{ false },
		stacking{ false },
		g_bForceRegister{ false },
		tiffFormat{ TF_32BITRGBFLOAT },
		tiffCompression{ TC_NONE },
		terminalOutputMode{ TERMINAL_OUTPUT_MODE::FORMATTED },
		saveIntermediate{ false },
		saveCalibrated{ false },
		fitsOutput{ false },
		consoleOut{ stdout }
	{
	}
	bool registering;
	bool stacking;
	QString g_strListFile;
	QString g_strOutputFile;
	bool g_bForceRegister;
	TIFFFORMAT tiffFormat;
	TIFFCOMPRESSION tiffCompression;
	TERMINAL_OUTPUT_MODE terminalOutputMode;
	bool saveIntermediate;
	bool saveCalibrated;
	bool fitsOutput;
	QTextStream consoleOut;
};
StackingParams params;

/* ------------------------------------------------------------------- */

bool fileExists(const QString& path)
{
	QFileInfo check_file(path);

	// check if file exists and if yes: Is it really a file and no directory?
	if (check_file.exists() && check_file.isFile())
		return true;
	return false;
}
bool checkFileCanBeWrittenTo(const QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly))
		return false;

	file.close();
	file.remove();
	return true;
}

bool DecodeCommandLine(int argc, _TCHAR* argv[])
{
	bool bResult = false;
	LONG i;
	std::vector<QString> vCommandLine;

	for (i = 1; i < argc; i++)
		vCommandLine.emplace_back(QString::fromWCharArray(argv[i]));

	// At least 2 arguments (registering and/or stacking + filename)
	bResult = (vCommandLine.size() >= 2);
	for (i = 0; i < vCommandLine.size() && bResult; i++)
	{
		if (!vCommandLine[i].compare("/s", Qt::CaseInsensitive))
		{
			params.stacking = true;
		}
		else if (!vCommandLine[i].compare("/SR", Qt::CaseInsensitive))
		{
			params.stacking = true;
			params.saveIntermediate = true;
		}
		else if (!vCommandLine[i].compare("/SC", Qt::CaseInsensitive))
		{
			params.stacking = true;
			params.saveCalibrated = true;
		}
		else if (!vCommandLine[i].compare("/FITS", Qt::CaseInsensitive))
		{
			params.fitsOutput = true;
		}
		else if (!vCommandLine[i].compare("/r", Qt::CaseInsensitive))
		{
			params.registering = true;
			if (!vCommandLine[i].compare("/R", Qt::CaseSensitive))
				params.g_bForceRegister = true;
		}
		else if (!vCommandLine[i].left(3).compare("/O:", Qt::CaseInsensitive))
		{
			// Check output file name
			params.g_strOutputFile = vCommandLine[i].right(vCommandLine[i].length() - 3);
			if (!checkFileCanBeWrittenTo(params.g_strOutputFile))
			{
				params.consoleOut << "Cannot write to " << params.g_strOutputFile << " (not a valid filename)" << Qt::endl;
				bResult = false;
			}
		}
		else if (!vCommandLine[i].left(3).compare("/OF", Qt::CaseInsensitive))
		{
			QString strFormat(vCommandLine[i].right(vCommandLine[i].length() - 3));
			if (strFormat == "16" || strFormat == "16i")
				params.tiffFormat = TF_16BITRGB;
			else if (strFormat == "32" || strFormat == "32i")
				params.tiffFormat = TF_32BITRGB;
			else if (strFormat == "32r")
				params.tiffFormat = TF_32BITRGBFLOAT;
			else
			{
				params.consoleOut << "Unrecognized or unsupported bit format " << strFormat << Qt::endl;
				bResult = false;
			}
		}
		else if (!vCommandLine[i].left(3).compare("/OC", Qt::CaseInsensitive))
		{
			QString strCompression(vCommandLine[i].right(vCommandLine[i].length() - 3));
			if (strCompression == "0")
				params.tiffCompression = TC_NONE;
			else if (strCompression == "1")
				params.tiffCompression = TC_LZW;
			else if (strCompression == "2")
				params.tiffCompression = TC_DEFLATE;
			else
			{
				params.consoleOut << "Unrecognized or unsupported compression format " << strCompression << Qt::endl;
				bResult = false;
			}
		}
		else if (!vCommandLine[i].left(3).compare("/OS", Qt::CaseInsensitive))
		{
			QString strOutputMode(vCommandLine[i].right(vCommandLine[i].length() - 3));
			if (strOutputMode == "0")
				params.terminalOutputMode = TERMINAL_OUTPUT_MODE::BASIC;
			else if (strOutputMode == "1")
				params.terminalOutputMode = TERMINAL_OUTPUT_MODE::COLOURED;
			else if (strOutputMode == "2")
				params.terminalOutputMode = TERMINAL_OUTPUT_MODE::FORMATTED;
			else
			{
				params.consoleOut << "Unrecognized or unsupported output format " << strOutputMode << Qt::endl;
				bResult = false;
			}
		}
		else
		{
			if (fileExists(vCommandLine[i]))
				params.g_strListFile = vCommandLine[i];
			else
				bResult = false;
		}
	}

	if (!params.stacking && !params.registering)
		bResult = false;

	if (params.g_strListFile.isEmpty())
		bResult = false;

	return bResult;
}

/* ------------------------------------------------------------------- */

void SaveBitmap(const std::shared_ptr<CMemoryBitmap> pBitmap)
{
	if (pBitmap && !params.g_strOutputFile.isEmpty())
	{
		bool bMonochrome;
		DSS::ProgressConsole progress(params.terminalOutputMode);

		const QString strText(QCoreApplication::translate("DeepSkyStackerCL", "Saving Final image in %1", "IDS_SAVINGFINAL").arg(params.g_strOutputFile));
		progress.Start1(strText, 0);
		progress.Start2(strText, 0);

		bMonochrome = pBitmap->IsMonochrome();

		if (params.fitsOutput)
		{
			FITSFORMAT fitsformat = FF_UNKNOWN;
			switch (params.tiffFormat)
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
			WriteFITS(params.g_strOutputFile.toStdWString().c_str(), pBitmap.get(), &progress, fitsformat, nullptr);
		}
		else
		{
			if (bMonochrome)
			{
				switch (params.tiffFormat)
				{
				case TF_16BITRGB:
					params.tiffFormat = TF_16BITGRAY;
					break;
				case TF_32BITRGB:
					params.tiffFormat = TF_32BITGRAY;
					break;
				case TF_32BITRGBFLOAT:
					params.tiffFormat = TF_32BITGRAYFLOAT;
					break;
				}
			}
			WriteTIFF(params.g_strOutputFile.toStdWString().c_str(), pBitmap.get(), &progress, params.tiffFormat, params.tiffCompression, nullptr);
		}
	}
}

/* ------------------------------------------------------------------- */

int _tmain(int argc, _TCHAR* argv[])
{
	SetUILanguage();

#ifndef NOGDIPLUS
	GdiplusStartupInput		gdiplusStartupInput;
	GdiplusStartupOutput	gdiSO;
	ULONG_PTR				gdiplusToken;
	ULONG_PTR				gdiHookToken;

	// Initialize GDI+.
	gdiplusStartupInput.SuppressBackgroundThread = true;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);
#endif

	std::vector<QString>	vCommandLine;

	params.consoleOut << "DeepSkyStacker " << VERSION_DEEPSKYSTACKER << " Command Line" << Qt::endl << Qt::endl;

	// Decode command line
	if (!DecodeCommandLine(argc, argv))
	{
		params.consoleOut << "Syntax is DeepSkyStackerCL [/r|R] [/s] [/O:<>] [/OFxx] [/OCx] [/FITS] <ListFileName>" << Qt::endl;
		params.consoleOut << Qt::endl;
		params.consoleOut << " /r	         - Register frames (only the ones not already registered)" << Qt::endl;
		params.consoleOut << " /R            - Register frames (even the ones already registered)" << Qt::endl;
		params.consoleOut << " /S            - Stack frames" << Qt::endl;
		params.consoleOut << " /SR           - Save each registered and calibrated light frame in" << Qt::endl;
		params.consoleOut << "                 a TIFF files (implies /S)" << Qt::endl;
		params.consoleOut << " /SC           - Save each calibrated light frame in" << Qt::endl;
		params.consoleOut << "                 a TIFF files (implies /S)" << Qt::endl;
		params.consoleOut << " /O:<filename> - Output file name (full path)" << Qt::endl;
		params.consoleOut << "                 Default is Autosave.tif in the folder of the first light frame" << Qt::endl;
		params.consoleOut << "                 (implies /S or /SC)" << Qt::endl;
		params.consoleOut << " /OFxxx        - Output format (16 or 32 bits depth)" << Qt::endl;
		params.consoleOut << "                 16 or 16i: 16 bits integer" << Qt::endl;
		params.consoleOut << "                 32 or 32i: 32 bits integer" << Qt::endl;
		params.consoleOut << "                 32r: 32 bits rational (default)" << Qt::endl;
		params.consoleOut << " /OCx          - Output file compression" << Qt::endl;
		params.consoleOut << "                 0: no compression (default)" << Qt::endl;
		params.consoleOut << "                 1: LZW compression" << Qt::endl;
		params.consoleOut << "                 2: ZIP (Deflate) compression" << Qt::endl;
		params.consoleOut << " /OSx          - Output style" << Qt::endl;
		params.consoleOut << "                 0: simple" << Qt::endl;
		params.consoleOut << "                 1: colored" << Qt::endl;
		params.consoleOut << "                 2: compact (default)" << Qt::endl;
		params.consoleOut << " /FITS         - Output file format is FITS (default is TIFF)" << Qt::endl;
		params.consoleOut << "<ListFileName> - Name of a file list saved by DeepSkyStacker" << Qt::endl;
		params.consoleOut << Qt::endl;
		params.consoleOut << "Examples:" << Qt::endl;
		params.consoleOut << "DeepSkyStackerCL /r /OS1 c:\\MyLists\\SampleList.txt" << Qt::endl;
		params.consoleOut << "  will register all the checked light frames of the list" << Qt::endl;
		params.consoleOut << "  and the output on the commandline will be colored" << Qt::endl;
		params.consoleOut << Qt::endl;
		params.consoleOut << "DeepSkyStackerCL /r /s /OF16i c:\\MyLists\\SampleList.txt" << Qt::endl;
		params.consoleOut << "  will register then stack all the checked light frames of the list" << Qt::endl;
		params.consoleOut << "  and save the result in a 16 bits integer TIFF file named" << Qt::endl;
		params.consoleOut << "  autosave.tif in the folder of the first light frame" << Qt::endl;
		params.consoleOut << Qt::endl;
	}
	else
	{
		DSS::ProgressConsole progress(params.terminalOutputMode);
		DSS::FrameList frameList;
		bool bContinue = true;

		if (params.registering && params.stacking)
			params.consoleOut << "Registering and stacking: " << params.g_strListFile << " list" << Qt::endl;
		else if (params.registering)
			params.consoleOut << "Registering: " << params.g_strListFile << " list" << Qt::endl;
		else
			params.consoleOut << "Stacking: " << params.g_strListFile << " list" << Qt::endl;

		if (params.registering)
			params.consoleOut << "Register again already registered light frames: " << (params.g_bForceRegister ? "yes" : "no") << Qt::endl;
		params.consoleOut << Qt::endl;

		frameList.loadFilesFromList(QDir(params.g_strListFile).filesystemAbsolutePath());

		CAllStackingTasks tasks;
		frameList.fillTasks(tasks);
		tasks.ResolveTasks();

		// Open list file
		if (params.registering || !frameList.countUnregisteredCheckedLightFrames())
		{
			// Register checked light frames
			CRegisterEngine	RegisterEngine;
			bContinue = RegisterEngine.RegisterLightFrames(tasks, params.g_bForceRegister, &progress);
		}
		if (params.stacking && bContinue)
		{
			// Stack register light frames
			CStackingEngine StackingEngine;
			std::shared_ptr<CMemoryBitmap> pBitmap;

			StackingEngine.SetSaveIntermediate(params.saveIntermediate);
			StackingEngine.SetSaveCalibrated(params.saveCalibrated);
			bContinue = StackingEngine.StackLightFrames(tasks, &progress, pBitmap);
			if (bContinue)
			{
				CString cstrOutputPath(params.g_strOutputFile.toStdWString().c_str());
				if (StackingEngine.GetDefaultOutputFileName(cstrOutputPath, params.g_strListFile.toStdWString().c_str(), !params.fitsOutput))
				{
					params.g_strOutputFile = QString::fromStdWString(cstrOutputPath.GetString());
					StackingEngine.WriteDescription(tasks, params.g_strOutputFile.toStdWString().c_str());
					SaveBitmap(pBitmap);
				}
			}
		}
	}
	params.consoleOut << Qt::endl;

#ifndef NOGDIPLUS
	// Shutdown GDI+
	gdiSO.NotificationUnhook(gdiHookToken);
	GdiplusShutdown(gdiplusToken);
#endif

	return 0;
}