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
#include "stdafx.h"
#include "group.h"
#include "ImageListModel.h"
#include "ztrace.h"
#include "FrameInfo.h"
#include "RegisterEngine.h"

namespace DSS
{
	Group::Group() :
		pictures{ std::make_unique<ImageListModel>() },
		Index{ nextIndex++ },		// First group is Main Group with Index of 0
		Dirty{ false },
		nameChanged{ false }
	{
		if (0 == Index)
		{
			Name = QCoreApplication::translate("DSS::StackingDlg", "Main Group", "IDS_MAINGROUP");
		}
		else
		{
			Name = QCoreApplication::translate("DSS::StackingDlg", "Group %1", "IDS_GROUPIDMASK").arg(Index);
		}
	}

	Group::Group(Group&& rhs) noexcept :
		pictures{ std::move(rhs.pictures) },
		Index{ std::exchange(rhs.Index, 0) },
		Name{ std::move(rhs.Name) },
		Dirty{ std::exchange(rhs.Dirty, false) },
		nameChanged{ std::exchange(rhs.nameChanged, false) }
	{}

	Group& Group::operator=(Group&& rhs) noexcept
	{
		if (this != &rhs)
		{
			pictures = std::move(rhs.pictures);
			Index = std::exchange(rhs.Index, 0);
			Name = std::move(rhs.Name);
			Dirty = std::exchange(rhs.Dirty, false);
			nameChanged = std::exchange(rhs.nameChanged, false);
		}
		return *this;
	}

	void Group::addImage(const ListBitMap& image)
	{
		pictures->addImage(image);
		Dirty = true;
	}
	
	size_t Group::size() const noexcept
	{
		return pictures->rowCount();
	}

	void Group::addFile(fs::path file, PICTURETYPE PictureType, bool bCheck, int)
	{
		ZFUNCTRACE_RUNTIME();

		ListBitMap			lb;

		lb.m_groupId = static_cast<decltype(ListBitMap::m_groupId)>(Index);

		//
		// Warning: If this code is changed, matching changes must also be made to ImageListModel::retranslateUi()
		//
		if (lb.InitFromFile(file, PictureType))
		{
			if (lb.IsMasterFrame())
			{
				if (lb.m_PictureType == PICTURETYPE_DARKFRAME)
					lb.m_strType = QCoreApplication::translate("DSS::Group", "Master Dark", "IDS_TYPE_MASTERDARK");
				else if (lb.m_PictureType == PICTURETYPE_DARKFLATFRAME)
					lb.m_strType = QCoreApplication::translate("DSS::Group", "Master Dark Flat", "IDS_TYPE_MASTERDARKFLAT");
				else if (lb.m_PictureType == PICTURETYPE_FLATFRAME)
					lb.m_strType = QCoreApplication::translate("DSS::Group", "Master Flat", "IDS_TYPE_MASTERFLAT");
				else if (lb.m_PictureType == PICTURETYPE_OFFSETFRAME)
					lb.m_strType = QCoreApplication::translate("DSS::Group", "Master Offset", "IDS_TYPE_MASTEROFFSET");
			}
			else
			{
				if (lb.m_PictureType == PICTURETYPE_DARKFRAME)
					lb.m_strType = QCoreApplication::translate("DSS::Group", "Dark", "IDS_TYPE_DARK");
				else if (lb.m_PictureType == PICTURETYPE_DARKFLATFRAME)
					lb.m_strType = QCoreApplication::translate("DSS::Group", "Dark Flat", "IDS_TYPE_DARKFLAT");
				else if (lb.m_PictureType == PICTURETYPE_FLATFRAME)
					lb.m_strType = QCoreApplication::translate("DSS::Group", "Flat", "IDS_TYPE_FLAT");
				else if (lb.m_PictureType == PICTURETYPE_OFFSETFRAME)
					lb.m_strType = QCoreApplication::translate("DSS::Group", "Bias/Offset", "IDS_TYPE_OFFSET");
				else
					lb.m_strType = QCoreApplication::translate("DSS::Group", "Light", "IDS_TYPE_LIGHT");
			}


			if (bCheck)
				lb.m_bChecked = Qt::Checked;
			else
				lb.m_bChecked = Qt::Unchecked;

			if (file.has_parent_path())
				lb.m_strPath = QString::fromStdU16String(file.parent_path().generic_u16string());
			else
				lb.m_strPath = QString::fromStdU16String(file.root_path().generic_u16string());

			lb.m_strFile = QString::fromStdU16String(file.filename().generic_u16string());

			if (lb.m_PictureType == PICTURETYPE_LIGHTFRAME)
			{
				CLightFrameInfo			bmpInfo;

				bmpInfo.SetBitmap(file.c_str());
				if (bmpInfo.m_bInfoOk)
				{
					lb.m_bRegistered = true;
					lb.m_fOverallQuality = bmpInfo.m_fOverallQuality;
					lb.quality = bmpInfo.quality;
					lb.m_fFWHM = bmpInfo.m_fFWHM;
					lb.m_lNrStars = static_cast<decltype(lb.m_lNrStars)>(bmpInfo.m_vStars.size());
					lb.m_bComet = bmpInfo.m_bComet;
					lb.m_SkyBackground = bmpInfo.m_SkyBackground;
					lb.m_bUseAsStarting = (PictureType == PICTURETYPE_REFLIGHTFRAME);
				}
			}

			lb.m_strSizes = QString("%1 x %2").arg(lb.m_lWidth).arg(lb.m_lHeight);

			if (lb.m_lNrChannels == 3)
				lb.m_strDepth = QCoreApplication::translate("DSS::Group", "RGB %1 bit/ch", "IDS_FORMAT_RGB").arg(lb.m_lBitsPerChannel);
			else
				lb.m_strDepth = QCoreApplication::translate("DSS::Group", "Gray %1 bit", "IDS_FORMAT_GRAY").arg(lb.m_lBitsPerChannel);

			if (lb.GetCFAType() != CFATYPE_NONE)
				lb.m_strCFA = QCoreApplication::translate("DSS::Group", "Yes", "IDS_YES");
			else
				lb.m_strCFA = QCoreApplication::translate("DSS::Group", "No", "IDS_NO");

		}

		pathToGroup.emplace(file, Index);

		pictures->addImage(lb);
	}
}
