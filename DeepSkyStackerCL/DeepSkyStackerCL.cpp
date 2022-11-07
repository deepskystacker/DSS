// DeepSkyStackerCL.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>

#include <gdiplus.h>
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")

static	BOOL				g_bRegistering = FALSE;
static	BOOL				g_bStacking = FALSE;
static	std::wstring		g_strListFile;
static  CString				g_strOutputFile;
static  BOOL				g_bForceRegister = FALSE;
static  TIFFFORMAT			g_TIFFFormat = TF_32BITRGBFLOAT;
static	TIFFCOMPRESSION		g_TIFFCompression = TC_NONE;
static  BOOL				g_bSaveIntermediate = FALSE;
static  BOOL				g_bSaveCalibrated = FALSE;
static  BOOL				g_bFITSOutput = FALSE;

#include "QtProgressConsole.h"
#include "FrameList.h"
#include "StackingEngine.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "SetUILanguage.h"

/* ------------------------------------------------------------------- */

BOOL	DecodeCommandLine(int argc, _TCHAR* argv[])
{
	BOOL					bResult = FALSE;
	LONG					i;
	std::vector<CString>	vCommandLine;

	for (i = 1;i<argc;i++)
	{
		vCommandLine.emplace_back(argv[i]);
	};

	// At least 2 arguments (registering and/or stacking + filename)
	bResult = (vCommandLine.size() >= 2);
	for (i = 0;i<vCommandLine.size() && bResult;i++)
	{
		if (!vCommandLine[i].CompareNoCase(_T("/s")))
		{
			g_bStacking = TRUE;
		}
		else if (!vCommandLine[i].CompareNoCase(_T("/SR")))
		{
			g_bStacking = TRUE;
			g_bSaveIntermediate = TRUE;
		}
		else if (!vCommandLine[i].CompareNoCase(_T("/SC")))
		{
			g_bStacking = TRUE;
			g_bSaveCalibrated = TRUE;
		}
		else if (!vCommandLine[i].CompareNoCase(_T("/FITS")))
		{
			g_bFITSOutput = TRUE;
		}
		else if (!vCommandLine[i].CompareNoCase(_T("/r")))
		{
			g_bRegistering = TRUE;
			if (!vCommandLine[i].Compare(_T("/R")))
				g_bForceRegister = TRUE;
		}
		else if (!vCommandLine[i].Left(3).CompareNoCase(_T("/O:")))
		{
			// Check output file name
			g_strOutputFile = vCommandLine[i].Right(vCommandLine[i].GetLength()-3);
			FILE *			hFile;

			hFile = _tfopen(g_strOutputFile, _T("wb"));
			if (hFile)
			{
				fclose(hFile);
				DeleteFile(g_strOutputFile);
			}
			else
			{
				_tprintf(_T("Cannot write to %s (not a valid filename)\n"), (LPCTSTR)g_strOutputFile);
				bResult = FALSE;
			};
		}
		else if (!vCommandLine[i].Left(3).CompareNoCase(_T("/OF")))
		{
			CString			strFormat;

			strFormat = vCommandLine[i].Right(vCommandLine[i].GetLength()-3);
			if ((strFormat == _T("16")) || (strFormat == _T("16i")))
			{
				g_TIFFFormat = TF_16BITRGB;
			}
			else if ((strFormat == _T("32")) || (strFormat == _T("32i")))
			{
				g_TIFFFormat = TF_32BITRGB;
			}
			else if (strFormat == _T("32r"))
			{
				g_TIFFFormat = TF_32BITRGBFLOAT;
			}
			else
			{
				_tprintf(_T("Unrecognized or unsupported bit format %s\n"), (LPCTSTR)strFormat);
				bResult = FALSE;
			};
		}
		else if (!vCommandLine[i].Left(3).CompareNoCase(_T("/OC")))
		{
			CString			strCompression;

			strCompression = vCommandLine[i].Right(vCommandLine[i].GetLength()-3);
			if (strCompression == _T("0"))
			{
				g_TIFFCompression = TC_NONE;
			}
			else if (strCompression == _T("1"))
			{
				g_TIFFCompression = TC_LZW;
			}
			else if (strCompression == _T("2"))
			{
				g_TIFFCompression = TC_DEFLATE;
			}
			else
			{
				_tprintf(_T("Unrecognized or unsupported compression format %s\n"), (LPCTSTR)strCompression);
				bResult = FALSE;
			};
		}
		else
		{
			// Check that it is a file
			FILE *			hFile;

			hFile = _tfopen(vCommandLine[i], _T("rt"));
			if (hFile)
			{
				g_strListFile = vCommandLine[i];
				fclose(hFile);
			}
			else
				bResult = FALSE;
		};
	};

	if (!g_bStacking && !g_bRegistering)
		bResult = FALSE;
	if (g_strListFile.empty())
		bResult = FALSE;

	return bResult;
};

/* ------------------------------------------------------------------- */

void ComputeStacks(DSS::FrameList & frameList, CAllStackingTasks & tasks)
{
	size_t	comets = 0;
	uint16_t group = 0;
	bool				bReferenceFrameHasComet = false;
	double				fMaxScore = -1.0;

	tasks.Clear();
	// Iterate over all groups.
	for (auto groupIter = frameList.groups_cbegin(); groupIter != frameList.groups_cend(); ++group)
	{
		// and then over each image in the group
		for (auto it = groupIter->pictures->cbegin();
			it != groupIter->pictures->cend(); ++it)
		{
			if (it->m_bChecked == Qt::Checked)
			{
				if (it->m_fOverallQuality > fMaxScore)
				{
					fMaxScore = it->m_fOverallQuality;
					bReferenceFrameHasComet = it->m_bComet;
				}
				tasks.AddFileToTask(*it, group);
				if (it->m_bComet)
					comets++;

				if ((PICTURETYPE_LIGHTFRAME == it->m_PictureType) &&
					!g_strOutputFile.GetLength())
				{
					CString			strPath = it->filePath.generic_wstring().c_str();
					TCHAR			szDrive[_MAX_DRIVE];
					TCHAR			szDir[_MAX_DIR];

					_tsplitpath(strPath, szDrive, szDir, nullptr, nullptr);
					strPath = szDrive;
					strPath += szDir;
					if (g_bFITSOutput)
						strPath += "AutoSave.fts";
					else
						strPath += "AutoSave.tif";

					g_strOutputFile = strPath;
				}
			}
			++group;
		}
	}

	if (comets > 1 && bReferenceFrameHasComet)
		tasks.SetCometAvailable(TRUE);
	tasks.ResolveTasks();
};

/* ------------------------------------------------------------------- */

void SaveBitmap(std::shared_ptr<CMemoryBitmap> pBitmap)
{
	if (pBitmap && g_strOutputFile.GetLength())
	{
		BOOL					bMonochrome;
		DSS::QtProgressConsole		progress;

		bMonochrome = pBitmap->IsMonochrome();

		if (g_bFITSOutput)
		{
			FITSFORMAT			fitsformat = FF_UNKNOWN;

			switch (g_TIFFFormat)
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
			};

			WriteFITS(g_strOutputFile, pBitmap.get(), &progress, fitsformat, nullptr);
		}
		else
		{
			if (bMonochrome)
			{
				switch (g_TIFFFormat)
				{
				case TF_16BITRGB :
					g_TIFFFormat = TF_16BITGRAY;
					break;
				case TF_32BITRGB :
					g_TIFFFormat = TF_32BITGRAY;
					break;
				case TF_32BITRGBFLOAT :
					g_TIFFFormat = TF_32BITGRAYFLOAT;
					break;
				};
			};

			WriteTIFF(g_strOutputFile, pBitmap.get(), &progress, g_TIFFFormat, g_TIFFCompression, nullptr);
		};
	};
};

/* ------------------------------------------------------------------- */

int _tmain(int argc, _TCHAR* argv[])
{
	OleInitialize(nullptr);

	SetUILanguage();

	#ifndef NOGDIPLUS
	GdiplusStartupInput		gdiplusStartupInput;
	GdiplusStartupOutput	gdiSO;
	ULONG_PTR				gdiplusToken;
	ULONG_PTR				gdiHookToken;

	// Initialize GDI+.
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);
	#endif

	std::vector<CString>	vCommandLine;

	_tprintf(_T("DeepSkyStacker %s Command Line\n\n"), _T(VERSION_DEEPSKYSTACKER));

	// Decode command line
	if (!DecodeCommandLine(argc, argv))
	{
		_tprintf(_T("Syntax is DeepSkyStackerCL [/r|R] [/s] [/O:<>] [/OFxx] [/OCx] [/FITS] <ListFileName>\n"));
		_tprintf(_T(" /r	     - Register frames (only the ones not already registered)\n"));
		_tprintf(_T(" /R      - Register frames (even the ones already registered)\n"));
		_tprintf(_T(" /S      - Stack frames\n"));
		_tprintf(_T(" /SR     - Save each registered and calibrated light frame in\n"));
		_tprintf(_T("           a TIFF files (implies /S)\n"));
		_tprintf(_T(" /SC     - Save each calibrated light frame in\n"));
		_tprintf(_T("           a TIFF files (implies /S)\n"));
		_tprintf(_T(" /O:<outputfilename> - Output file name (full path)\n"));
		_tprintf(_T("           Default is Autosave.tif in the folder of the first light frame\n"));
		_tprintf(_T("           (implies /S or /SC)\n"));
		_tprintf(_T(" /OFxxx  - Output format (16 or 32 bits depth)\n"));
		_tprintf(_T("           16 or 16i: 16 bits integer\n"));
		_tprintf(_T("           32 or 32i: 32 bits integer\n"));
		_tprintf(_T("           32r: 32 bits rational (default)\n"));
		_tprintf(_T(" /OCx    - Output file compression\n"));
		_tprintf(_T("           0: no compression (default)\n"));
		_tprintf(_T("           1: LZW compression\n"));
		_tprintf(_T("           2: ZIP (Deflate) compression\n"));
		_tprintf(_T(" /FITS     Output file format is FITS (default is TIFF)\n"));
		_tprintf(_T("<ListFileName> is the name of a file list saved by DeepSkyStacker\n\n"));
		_tprintf(_T("Exemples:\n"));
		_tprintf(_T("DeepSkyStackerCL /r c:\\MyLists\\SampleList.txt\n"));
		_tprintf(_T("  will register all the checked light frames of the list\n\n"));
		_tprintf(_T("DeepSkyStackerCL /r /s /OF16i c:\\MyLists\\SampleList.txt\n"));
		_tprintf(_T("  will register then stack all the checked light frames of the list\n"));
		_tprintf(_T("  and save the result in a 16 bits integer TIFF file named\n"));
		_tprintf(_T("  autosave.tif in the folder of the first light frame\n\n"));
	}
	else
	{
		DSS::QtProgressConsole		progress;
		DSS::FrameList			frameList;
		BOOL					bContinue = TRUE;

		if (g_bRegistering && g_bStacking)
			_tprintf(_T("Registering and stacking %s list\n"), g_strListFile.c_str());
		else if (g_bRegistering)
			_tprintf(_T("Registering %s list\n"), g_strListFile.c_str());
		else
			_tprintf(_T("Stacking %s list\n"), g_strListFile.c_str());

		if (g_bRegistering)
		{
			_tprintf(_T("Register again already registered light frames: "));
			if (g_bForceRegister)
				_tprintf(_T(" yes\n"));
			else
				_tprintf(_T(" no\n"));
		};

		frameList.loadFilesFromList(g_strListFile);

		CAllStackingTasks		tasks;

		frameList.fillTasks(tasks);
		tasks.ResolveTasks();

		// Open list file
		if (g_bRegistering || !frameList.countUnregisteredCheckedLightFrames())
		{
			// Register checked light frames
			CRegisterEngine	RegisterEngine;

			bContinue = RegisterEngine.RegisterLightFrames(tasks, g_bForceRegister, &progress);
		};
		if (g_bStacking && bContinue)
		{
			// Stack register light frames
			CStackingEngine				StackingEngine;
			std::shared_ptr<CMemoryBitmap>	pBitmap;

			StackingEngine.SetSaveIntermediate(g_bSaveIntermediate);
			StackingEngine.SetSaveCalibrated(g_bSaveCalibrated);
			bContinue = StackingEngine.StackLightFrames(tasks, &progress, pBitmap);
			if (bContinue)
			{
				if (StackingEngine.GetDefaultOutputFileName(g_strOutputFile, g_strListFile.c_str(), !g_bFITSOutput))
				{
					StackingEngine.WriteDescription(tasks, g_strOutputFile);
					SaveBitmap(pBitmap);
				};
			};
		};
	};

	#ifndef NOGDIPLUS
	// Shutdown GDI+
	gdiSO.NotificationUnhook(gdiHookToken);
	GdiplusShutdown(gdiplusToken);
	#endif

	OleUninitialize();

	return 0;
}

