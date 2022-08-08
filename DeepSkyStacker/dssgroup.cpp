/****************************************************************************
**
** Copyright (C) 2021 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/

#define _WIN32_WINNT _WIN32_WINNT_WIN7

#include <afx.h>
#include <afxwin.h> 

#include <algorithm>
using std::min;
using std::max;
#include <vector>

#include "Ztrace.h"

#include "dsscommon.h"
#include "framelist.h"

#include "dssgroup.h"

namespace DSS
{
	void Group::AddFile(LPCTSTR szFile, uint16_t groupId, PICTURETYPE PictureType, bool bCheck)
	{
		ZFUNCTRACE_RUNTIME();
		CString				strFile = szFile;
		TCHAR				szDrive[1 + _MAX_DRIVE];
		TCHAR				szDir[1 + _MAX_DIR];
		TCHAR				szFileName[1 + _MAX_FNAME];
		TCHAR				szExt[1 + _MAX_EXT];
		LONG				lIndice = -1;


		ListBitMap			lb;

		lb.m_groupId = groupId;

		if (lb.InitFromFile(szFile, PictureType))
		{
			if (PictureType == PICTURETYPE_DARKFRAME)
			{
				lb.m_strType.LoadString(IDS_TYPE_DARK);
				lb.m_bUseAsStarting = false;
			}
			else if (PictureType == PICTURETYPE_DARKFLATFRAME)
			{
				lb.m_strType.LoadString(IDS_TYPE_DARKFLAT);
				lb.m_bUseAsStarting = false;
			}
			else if (PictureType == PICTURETYPE_FLATFRAME)
			{
				lb.m_strType.LoadString(IDS_TYPE_FLAT);
				lb.m_bUseAsStarting = false;
			}
			else if (PictureType == PICTURETYPE_OFFSETFRAME)
			{
				lb.m_strType.LoadString(IDS_TYPE_OFFSET);
				lb.m_bUseAsStarting = false;
			}
			else
			{
				lb.m_strType.LoadString(IDS_TYPE_LIGHT);
			};
			_tsplitpath(strFile, szDrive, szDir, szFileName, szExt);

			if (bCheck)
				lb.m_bChecked = Qt::Checked;

			lb.m_strPath = szDrive;
			lb.m_strPath += szDir;

			CString				strFileName;
			strFileName = szFileName;
			strFileName += szExt;

			lb.m_strFile = strFileName;

			if (lb.m_PictureType != PICTURETYPE_LIGHTFRAME)
			{
			}
			else
			{
				CLightFrameInfo			bmpInfo;

				bmpInfo.SetBitmap(szFile, false);
				if (bmpInfo.m_bInfoOk)
				{
					lb.m_bRegistered = true;
					lb.m_fOverallQuality = bmpInfo.m_fOverallQuality;
					lb.m_fFWHM = bmpInfo.m_fFWHM;
					lb.m_lNrStars = (LONG)bmpInfo.m_vStars.size();
					lb.m_bComet = bmpInfo.m_bComet;
					lb.m_SkyBackground = bmpInfo.m_SkyBackground;
					lb.m_bUseAsStarting = (PictureType == PICTURETYPE_REFLIGHTFRAME);
				}
				else
				{
				};
			};

			{
				CString				strSizes;
				CString				strDepth;

				strSizes.Format(_T("%ld x %ld"), lb.m_lWidth, lb.m_lHeight);
				lb.m_strSizes = strSizes;

				if (lb.m_lNrChannels == 3)
					strDepth.Format(IDS_FORMAT_RGB, lb.m_lBitPerChannels);
				else
					strDepth.Format(IDS_FORMAT_GRAY, lb.m_lBitPerChannels);
				lb.m_strDepth = strDepth;

				CString				strText;

				if (lb.IsMasterFrame())
				{
					if (PictureType == PICTURETYPE_DARKFRAME)
						lb.m_strType.LoadString(IDS_TYPE_MASTERDARK);
					else if (PictureType == PICTURETYPE_DARKFLATFRAME)
						lb.m_strType.LoadString(IDS_TYPE_MASTERDARKFLAT);
					else if (PictureType == PICTURETYPE_FLATFRAME)
						lb.m_strType.LoadString(IDS_TYPE_MASTERFLAT);
					else if (PictureType == PICTURETYPE_OFFSETFRAME)
						lb.m_strType.LoadString(IDS_TYPE_MASTEROFFSET);
				};

				if (lb.GetCFAType() != CFATYPE_NONE)
					strText.LoadString(IDS_YES);
				else
					strText.LoadString(IDS_NO);

				lb.m_strCFA = strText;
			};

		model.addImage(lb);

		};
	};
}