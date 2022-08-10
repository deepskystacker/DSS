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
#include <vld.h>

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//
// Want to support windows 7 and up
//
#define _WIN32_WINNT _WIN32_WINNT_WIN7

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcview.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlbase.h>

#include <stdlib.h>

#include <windows.h>
#include <commctrl.h>

#include <algorithm>
using std::min;
using std::max;

#include <iostream>

#include "DSSCommon.h"
#include "DSSMemory.h"
#include "Ztrace.h"

#include <Qt>

#include "ImageListModel.h"
#include "FrameList.h"

namespace DSS
{
    constexpr auto M_PI = 3.14159265358979323846;
    //
    // Initial code based on https://meetingcpp.com/blog/items/an-introduction-into-qt-part-3.html
    //
	QVariant ImageListModel::data(const QModelIndex& index, int role) const

    {
        if (!index.isValid())
            return QVariant();

        if (index.row() >= mydata.size() || index.row() < 0)
            return QVariant();

        int row = index.row();

        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            switch (Column(index.column()))
            {
            case Column::PathCol:
                return QString::fromWCharArray(mydata[row].m_strPath.GetString());
                break;
            case Column::FileCol:
                return QString::fromWCharArray(mydata[row].m_strFile.GetString());
                break;
            case Column::TypeCol:
                return QString::fromWCharArray(mydata[row].m_strType.GetString());
                break;
            case Column::FilterCol:
                return QString::fromWCharArray(mydata[row].m_filterName.GetString());
                break;
            case Column::ScoreCol:
                if (mydata[row].m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (mydata[row].m_bRegistered)
                    {
                        QString result;
                        if (mydata[row].m_bUseAsStarting)
                            result = "(*) %1";
                        else
                            result = "%1";
                        return result.arg(mydata[row].m_fOverallQuality, 0, 'f', 2);
                    }
                    else
                        return QString("NC");
                };
                break;
            case Column::dXCol:
                if (mydata[row].m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (mydata[row].m_bDeltaComputed)
                        return QString("%1").arg(mydata[row].m_dX, 0, 'f', 2);
                    else
                        return QString("NC");
                };
                break;
            case Column::dYCol:
                if (mydata[row].m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (mydata[row].m_bDeltaComputed)
                        return QString("%1").arg(mydata[row].m_dY, 0, 'f', 2);
                    else
                        return QString("NC");
                };
                break;
            case Column::AngleCol:
                if (mydata[row].m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (mydata[row].m_bDeltaComputed)
                        return QString("%1 °").arg(mydata[row].m_fAngle * 180.0 / M_PI, 0, 'f', 2);
                    else
                        return QString("NC");
                };
                break;
            case Column::DateTimeCol:
                return QString::fromWCharArray(mydata[row].m_strDateTime.GetString());
                break;
            case Column::SizeCol:
                return QString::fromWCharArray(mydata[row].m_strSizes.GetString());
                break;
            case Column::CFACol:
                return mydata[row].GetCFAType() != CFATYPE_NONE) ? tr("Yes", "IDS_YES") : tr("No", "IDS_NO"));
            case Column::DepthCol:
                return QString::fromWCharArray(mydata[row].m_strDepth.GetString());
                break;
            case Column::InfoCol:
                return QString::fromWCharArray(mydata[row].m_strInfos.GetString());
                break;

            case Column::ISOCol:
            case Column::ExposureCol:
            case Column::ApertureCol:
            case Column::FWHMCol:
            case Column::StarsCol:
            case Column::BackgroundCol:
                return "TestData";
                //return mydata[index.row()].getFirstname();
            }
        }
        return QVariant();

        if (role == Qt::DecorationRole)
        {
            if (0 == index.column())
                return rowIcon;
            else return QVariant();
        }

        if (role == Qt::CheckStateRole)
        {
            if (0 == index.column())
                return checkState;
            else return QVariant();

        }
            
    }

    QVariant ImageListModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Horizontal)
        {
            switch (Column(section))
            {
            case Column::PathCol:
                return tr("Path");
            case Column::FileCol:
                return tr("File");
            case Column::TypeCol:
                return tr("Type");
            case Column::FilterCol:
                return tr("Filter");
            case Column::ScoreCol:
                return tr("Score");
            case Column::dXCol:
                return tr("dX");
            case Column::dYCol:
                return tr("dY");
            case Column::AngleCol:
                return tr("Angle");
            case Column::DateTimeCol:
                return tr("Date/Time");
            case Column::SizeCol:
                return tr("Size");
            case Column::CFACol:
                return tr("CFA");
            case Column::DepthCol:
                return tr("Depth");
            case Column::InfoCol:
                return tr("Info");
            case Column::ISOCol:
                return tr("ISO/Gain");
            case Column::ExposureCol:
                return tr("Exposure");
            case Column::ApertureCol:
                return tr("Aperture");
            case Column::FWHMCol:
                return tr("FWHM");
            case Column::StarsCol:
                return tr("#Stars");
            case Column::BackgroundCol:
                return tr("Sky Background");
            }
        }
        return QVariant();
    }

    bool ImageListModel::setData(const int row, const Column column, const QVariant& value, int role)
    {
        QModelIndex index(createIndex(row, (int)column));
        return setData(index, value, role);
    }

    bool ImageListModel::setData(const QModelIndex& index, const QVariant& value, int role)
    {
        if (index.isValid() && !(index.row() >= mydata.size() || index.row() < 0))
        {
            if (role == Qt::DisplayRole || role == Qt::EditRole)
            {
                int row = index.row();

                switch (Column(index.column()))
                {
                case Column::PathCol:
                case Column::FileCol:
                case Column::TypeCol:
                    mydata[row].setType(value.toString());
                    break;
                case Column::FilterCol:
                case Column::ScoreCol:
                    mydata[row].m_fOverallQuality = value.toDouble();
                    break;
                case Column::dXCol:
                case Column::dYCol:
                case Column::AngleCol:
                case Column::DateTimeCol:
                case Column::SizeCol:
                case Column::CFACol:
                case Column::DepthCol:
                case Column::InfoCol:
                case Column::ISOCol:
                    mydata[row].setISO(value.toString());
                    break;
                case Column::ExposureCol:
                    mydata[row].setExposure(value.toString());
                    break;
                case Column::ApertureCol:
                case Column::FWHMCol:
                case Column::StarsCol:
                }
                emit dataChanged(index, index);
                return true;
            }
        }
        return false;
    }

    bool ImageListModel::setSkyBackground(const int row, const CSkyBackground& bg, int role)
    {
        QModelIndex index(createIndex(row, (int)Column::BackgroundCol));
        if (index.isValid() && !(index.row() >= mydata.size() || index.row() < 0))
        {
            if (role == Qt::DisplayRole || role == Qt::EditRole)
            {
                mydata[row].m_SkyBackground = bg;
                emit dataChanged(index, index);
                return true;
            }
        }
        return false;
    }

    //
    // Add an image (row) to the table, Before calling addImage, Qt requires that 
    // beginInsertRows() be called, and once batch of images is added, then
    // endInsertRows() must be called.
    //
    void ImageListModel::addImage(ListBitMap image)
    {
        if (std::find(mydata.begin(), mydata.end(), image) != mydata.end())
            return;
        mydata.push_back(std::move(image));
    }

    //
    // Remove a row from the model.  Before making calls to do so, Qt requires
    // that beginRemoveRows() is called, and on completion of current batch of
    // deletions, then endRemoveRows() must be called.
    //
    void ImageListModel::removeImage(int row)
    {
        mydata.erase(std::next(mydata.begin(), row));
    }
}