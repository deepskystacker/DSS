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

#include "FrameList.h"
#include "ImageListModel.h"

namespace DSS
{
    constexpr auto M_PI = 3.14159265358979323846;
    //
    // Initial code based on https://meetingcpp.com/blog/items/an-introduction-into-qt-part-3.html
    //

    ImageListModel::ImageListModel(QObject* parent) :
    QAbstractTableModel(parent)
    {
        //
        // Populate the Icon array if it's not already been done
        //
        if (0 == ImageListModel::icons.size())
        {
            std::lock_guard lock(ImageListModel::mutex);
            if (0 == ImageListModel::icons.size())          // check for race condtion
            {
                ImageListModel::icons.emplace_back(":/stacking/LightColour.png");
                ImageListModel::icons.emplace_back(":/stacking/DarkColour.png");
                ImageListModel::icons.emplace_back(":/stacking/FlatColour.png");
                ImageListModel::icons.emplace_back(":/stacking/BiasColour.png");
                ImageListModel::icons.emplace_back(":/stacking/LightCMYG.png");
                ImageListModel::icons.emplace_back(":/stacking/DarkCMYG.png");
                ImageListModel::icons.emplace_back(":/stacking/FlatCMYG.png");
                ImageListModel::icons.emplace_back(":/stacking/BiasCMYG.png");
                ImageListModel::icons.emplace_back(":/stacking/LightRGB.png");
                ImageListModel::icons.emplace_back(":/stacking/DarkRGB.png");
                ImageListModel::icons.emplace_back(":/stacking/FlatRGB.png");
                ImageListModel::icons.emplace_back(":/stacking/BiasRGB.png");
                ImageListModel::icons.emplace_back(":/stacking/LightGreyscale.png");
                ImageListModel::icons.emplace_back(":/stacking/DarkGreyscale.png");
                ImageListModel::icons.emplace_back(":/stacking/FlatGreyscale.png");
                ImageListModel::icons.emplace_back(":/stacking/BiasGreyscale.png");
            }
        }
    }

	QVariant ImageListModel::data(const QModelIndex& index, int role) const

    {
        if (!index.isValid())
            return QVariant();

        if (index.row() >= mydata.size() || index.row() < 0)
            return QVariant();

        int row = index.row();

        const auto& file = mydata[row];
        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            switch (Column(index.column()))
            {
            case Column::Path:
                return file.m_strPath;
                break;
            case Column::File:
                return file.m_strFile;
                break;
            case Column::Type:
                return file.m_strType;
                break;
            case Column::Filter:
                return file.m_filterName;
                break;
            case Column::Score:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (file.m_bRegistered)
                    {
                        QString result;
                        if (file.m_bUseAsStarting)
                            result = "(*) %1";
                        else
                            result = "%1";
                        return result.arg(file.m_fOverallQuality, 0, 'f', 2);
                    }
                    else
                        return QString("NC");
                };
                break;
            case Column::dX:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (file.m_bDeltaComputed)
                        return QString("%1").arg(file.m_dX, 0, 'f', 2);
                    else
                        return QString("NC");
                };
                break;
            case Column::dY:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (file.m_bDeltaComputed)
                        return QString("%1").arg(file.m_dY, 0, 'f', 2);
                    else
                        return QString("NC");
                };
                break;
            case Column::Angle:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (file.m_bDeltaComputed)
                        return QString("%1 °").arg(file.m_fAngle * 180.0 / M_PI, 0, 'f', 2);
                    else
                        return QString("NC");
                };
                break;
            case Column::FileTime:
                return QString::fromWCharArray(file.m_strDateTime.GetString());
                break;
            case Column::Size:
                return file.m_strSizes;
                break;
            case Column::CFA:
                return file.GetCFAType() != CFATYPE_NONE ? tr("Yes", "IDS_YES") : tr("No", "IDS_NO");
            case Column::Depth:
                return file.m_strDepth;
                break;
            case Column::Info:
                return QString::fromWCharArray(file.m_strInfos.GetString());
                break;
            case Column::ISO:
                // ISO value, of if ISO is not available then the Gain value
                if (file.m_lISOSpeed)
                    return isoToString(file.m_lISOSpeed);
                else if (file.m_lGain >= 0)
                    return gainToString(file.m_lGain);
                else
                    return QString("0");
                break;
            case Column::Exposure:
                return exposureToString(file.m_fExposure);
                break;
            case Column::Aperture:
                return QString("%1").arg(file.m_fAperture, 0, 'f', 1);
                break;
            case Column::FWHM:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (file.m_bRegistered)
                        return QString("%1").arg(file.m_fFWHM, 0, 'f', 2);
                    else
                        return QString("NC");
                };
                break;
            case Column::Stars:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (file.m_bRegistered)
                    {
                        if (file.m_bComet)
                            return QString("%1+(C)").arg(file.m_lNrStars);
                        else
                            return QString("%1").arg(file.m_lNrStars);
                    }
                    else
                        return QString("NC");
                }
                break;

            case Column::Background:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    return QString("N/A");
                else
                {
                    if (file.m_SkyBackground.m_fLight)
                        return QString("%1 %").arg(file.m_SkyBackground.m_fLight * 100.0, 0, 'f', 2);
                    else
                        return QString("NC");
                }
                break;

            default:
                return QVariant();
            }
        }

        if (role == Qt::DecorationRole)
        {
            if (0 == index.column())
                return rowIcon(file);
            else return QVariant();
        }

        if (role == Qt::CheckStateRole)
        {
            if (0 == index.column())
                return file.m_bChecked;
            else return QVariant();

        }

        return QVariant();
            
    }

    QVariant ImageListModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Horizontal)
        {
            switch (Column(section))
            {
            case Column::Path:
                return tr("Path");
            case Column::File:
                return tr("File");
            case Column::Type:
                return tr("Type");
            case Column::Filter:
                return tr("Filter");
            case Column::Score:
                return tr("Score");
            case Column::dX:
                return tr("dX");
            case Column::dY:
                return tr("dY");
            case Column::Angle:
                return tr("Angle");
            case Column::FileTime:
                return tr("Date/Time");
            case Column::Size:
                return tr("Size");
            case Column::CFA:
                return tr("CFA");
            case Column::Depth:
                return tr("Depth");
            case Column::Info:
                return tr("Info");
            case Column::ISO:
                return tr("ISO/Gain");
            case Column::Exposure:
                return tr("Exposure");
            case Column::Aperture:
                return tr("Aperture");
            case Column::FWHM:
                return tr("FWHM");
            case Column::Stars:
                return tr("#Stars");
            case Column::Background:
                return tr("Sky Background");
            }
        }
        return QVariant();
    }

    bool ImageListModel::setData(const int row, const Column column, const QVariant& value, int role)
    {
        QModelIndex index(createIndex(row, static_cast<int>(column)));
        return setData(index, value, role);
    }

    bool ImageListModel::setData(const QModelIndex& index, const QVariant& value, int role)
    {
        bool changed{ true };
        if (index.isValid() && !(index.row() >= mydata.size() || index.row() < 0))
        {
            int row = index.row();
            auto& file = mydata[row];

            if (role == Qt::DisplayRole || role == Qt::EditRole)
            {
                switch (static_cast<Column>(index.column()))
                {
                case Column::Path:
                    file.m_strPath = value.toString();
                    break;
                case Column::File:
                    file.m_strFile = value.toString();
                    break;
                case Column::Type:
                    file.m_strType = value.toString();
                    break;
                case Column::Filter:
                    file.m_filterName = value.toString();
                    break;
                case Column::Score:
                    file.m_fOverallQuality = value.toDouble();
                    break;
                case Column::dX:
                    file.m_dX = value.toDouble();
                    break;
                case Column::dY:
                    file.m_dY = value.toDouble();
                    break;
                case Column::Angle:
                    file.m_fAngle = value.toDouble();
                    break;
                // case Column::ISO: 
                // case Column::Exposure:
                // case Column::Aperture:
                case Column::FWHM:
                    file.m_fFWHM = value.toDouble();
                    break;
                case Column::Stars:
                    file.m_lNrStars = value.toUInt();
                    break;
                default:
                    changed = false;
                }
                if (changed) emit dataChanged(index, index);
                return true;
            }
            else if (Qt::CheckStateRole == role && 0 == index.column())
            {
                file.m_bChecked = static_cast<Qt::CheckState>(value.toInt());
                emit dataChanged(index, index);
                return true;
            }
                
        }
        return false;
    }

    bool ImageListModel::setSkyBackground(const int row, const CSkyBackground& bg, int role)
    {
        QModelIndex index(createIndex(row, (int)Column::Background));
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
        mydata.emplace_back(std::move(image));
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

    //
    // Work out which Icon to display based on the frame type
    //
    QVariant ImageListModel::rowIcon(const ListBitMap& file) const
    {
        QVariant result;
        PICTURETYPE type{ file.m_PictureType };
        int16_t index{ 0 };

        switch (type)
        {
        case PICTURETYPE_LIGHTFRAME:
        case PICTURETYPE_REFLIGHTFRAME:
            index = 0;
            break;
        case PICTURETYPE_DARKFRAME:
        case PICTURETYPE_DARKFLATFRAME:
            index = 1;
            break;
        case PICTURETYPE_FLATFRAME:
            index = 2;
            break;
        case PICTURETYPE_OFFSETFRAME:
            index = 3;
            break;
        default:
            return QVariant();      // we're outta here
        }

        // if (file.m_lNrChannels == 3)
        // index +=0;               // Use xxxxColour Icons
        if (IsCYMGType(file.GetCFAType()))
            index += 4;             // Use xxxxCMYG Icons
        else if (file.GetCFAType() != CFATYPE_NONE)
            index += 8;             // Use xxxxRGB Icons
        else
            index += 12;            // Use xxxxGreyscale Icons
        
        if (ImageListModel::icons[index].isNull())
        {
            qDebug("null icon");
        }
        return ImageListModel::icons[index];
    }
}