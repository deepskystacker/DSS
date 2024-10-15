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
#include "ImageListModel.h"

namespace DSS
{
    //
    // Initial code based on https://meetingcpp.com/blog/items/an-introduction-into-qt-part-3.html
    //

    ImageListModel::ImageListModel(QObject* parent) : QAbstractTableModel{ parent }
    {}

    QVariant ImageListModel::data(const int row, const Column column, int role) const
    {
        QModelIndex index(createIndex(row, static_cast<int>(column)));
        return data(index, role);
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
                {
                    if (Qt::EditRole == role)
                        return 0.0;
                    else
                        return QString("N/A");
                }
                else
                {
                    if (file.m_bRegistered)
                    {
                        if (Qt::EditRole == role)
                            return file.m_fOverallQuality;
                        QString result;
                        if (file.m_bUseAsStarting)
                            result = "(*) %1";
                        else
                            result = "%1";
                        return result.arg(file.m_fOverallQuality, 0, 'f', 2);
                    }
                    else
                    {
                        if (Qt::EditRole == role)
                            return 0.0;
                        else
                            return QString("NC");
                    }
                };
                break;
            case Column::Quality:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                {
                    if (Qt::EditRole == role)
                        return 0.0;
                    return QString("N/A");
                }
                else
                {
                    if (file.m_bRegistered)
                    {
                        if (Qt::EditRole == role)
                            return file.quality;
                        return QString{ "%1" }.arg(file.quality, 0, 'f', 2);
                    }
                    else
                    {
                        if (Qt::EditRole == role)
                            return 0.0;
                        else
                            return QString("NC");
                    }
                }
                break;
            case Column::dX:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                {
                    if (Qt::EditRole == role)
                        return 0.0;
                    else
                        return QString("N/A");
                }
                else
                {
                    if (file.m_bDeltaComputed)
                    {
                        if (Qt::EditRole == role)
                            return file.m_dX;
                        return QString("%1").arg(file.m_dX, 0, 'f', 2);
                    }
                    else
                    {
                        if (Qt::EditRole == role)
                            return 0.0;
                        else
                            return QString("NC");
                    }
                };
                break;
            case Column::dY:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                {
                    if (Qt::EditRole == role)
                        return 0.0;
                    else
                        return QString("N/A");
                }
                else
                {
                    if (file.m_bDeltaComputed)
                    {
                        if (Qt::EditRole == role)
                            return file.m_dY;
                        return QString("%1").arg(file.m_dY, 0, 'f', 2);
                    }
                    else
                    {
                        if (Qt::EditRole == role)
                            return 0.0;
                        else
                            return QString("NC");
                    }
                };
                break;
            case Column::Angle:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                {
                    if (Qt::EditRole == role)
                        return 0.0;
                    else
                        return QString("N/A");
                }
                else
                {
                    if (file.m_bDeltaComputed)
                    {
                        if (Qt::EditRole == role)
                            return file.m_fAngle;
                        else
                            return QString("%1\xc2\xb0").arg(file.m_fAngle * 180.0 / M_PI, 0, 'f', 2);
                    }
                    else
                    {
                        if (Qt::EditRole == role)
                            return 0.0;
                        else
                            return QString("NC");
                    }
                };
                break;
            case Column::FileTime:
                return file.m_strDateTime;
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
                return file.m_strInfos;
                break;
            case Column::ISO:
                // ISO value, of if ISO is not available then the Gain value
                if (file.m_lISOSpeed)
                {
                    if (Qt::EditRole == role)
                        return file.m_lISOSpeed;
                    else
                        return isoToString(file.m_lISOSpeed);
                }
                else if (file.m_lGain >= 0)
                {
                    if (Qt::EditRole == role)
                        return file.m_lGain;
                    else
                        return gainToString(file.m_lGain);
                }
                else
                {
                    if (Qt::EditRole == role)
                        return QString("0");
                    else
                        return 0;
                }
                break;

            case Column::Exposure:
                if (Qt::DisplayRole == role)
                    return exposureToString(file.m_fExposure);
                else
                    return file.m_fExposure;    // For edit role, just return the number
                break;
            case Column::Aperture:
                if (Qt::DisplayRole == role)
                    return QString("%1").arg(file.m_fAperture, 0, 'f', 1);
                else
                    return file.m_fAperture;
                break;
            case Column::FWHM:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    if (Qt::EditRole == role)
                        return 0.0;
                    else return QString("N/A");
                else
                {
                    if (file.m_bRegistered)
                    {
                        if (Qt::EditRole == role)
                            return file.m_fFWHM;
                        else
                            return QString("%1").arg(file.m_fFWHM, 0, 'f', 2);
                    }
                    else
                    {
                        if (Qt::EditRole == role)
                            return 0.0;
                        else
                            return QString("NC");
                    }
                };
                break;
            case Column::Stars:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    if (Qt::EditRole == role)
                        return 0.0;
                    else return QString("N/A");
                else
                {
                    if (file.m_bRegistered)
                    {
                        if (Qt::EditRole == role)
                            return file.m_lNrStars;
                        else if (file.m_bComet)
                            return QString("%1+(C)").arg(file.m_lNrStars);
                        else
                            return QString("%1").arg(file.m_lNrStars);
                    }
                    else
                    {
                        if (Qt::EditRole == role)
                            return 0.0;
                        else
                            return QString("NC");
                    }
                }
                break;

            case Column::Background:
                if (file.m_PictureType != PICTURETYPE_LIGHTFRAME)
                    if (Qt::EditRole == role)
                        return 0.0;
                    else return QString("N/A");
                else
                {
                    if (Qt::EditRole == role)
                        return file.m_SkyBackground.m_fLight;
                    else if (file.m_SkyBackground.m_fLight)
                    {
                        return QString("%1 %").arg(file.m_SkyBackground.m_fLight * 100.0, 0, 'f', 2);
                    }
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

        if (Qt::ToolTipRole == role)
            switch (static_cast<Column>(index.column()))
            {
            case Column::Type:
                return tr("Double click to change the type");
                break;
            case Column::ISO:
            case Column::Exposure:
                return tr("Double click to edit");
                break;
            }

        if (Qt::BackgroundRole == role)
            switch (static_cast<Column>(index.column()))
            {
            case Column::Type:
                return tr("Double click to change the type");
                break;
            case Column::ISO:
            case Column::Exposure:
                return tr("Double click to edit");
                break;
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
                return tr("Path", "IDS_COLUMN_PATH");
            case Column::File:
                return tr("File", "IDS_COLUMN_FILE");
            case Column::Type:
                return tr("Type", "IDS_COLUMN_TYPE");
            case Column::Filter:
                return tr("Filter", "IDS_COLUMN_FILTER");
            case Column::Score:
                return tr("Score", "IDS_COLUMN_SCORE");
            case Column::Quality:
                return tr("Quality", "IDS_COLUMN_MEANQUALITY");
            case Column::dX:
                return tr("dX", "IDS_COLUMN_DX");
            case Column::dY:
                return tr("dY", "IDS_COLUMN_DY");
            case Column::Angle:
                return tr("Angle", "IDS_COLUMN_ANGLE");
            case Column::FileTime:
                return tr("Date/Time", "IDS_COLUMN_DATETIME");
            case Column::Size:
                return tr("Size", "IDS_COLUMN_SIZES");
            case Column::CFA:
                return tr("CFA", "IDS_COLUMN_CFA");
            case Column::Depth:
                return tr("Depth", "IDS_COLUMN_DEPTH");
            case Column::Info:
                return tr("Info", "IDS_COLUMN_INFOS");
            case Column::ISO:
                return tr("ISO/Gain", "IDS_COLUMN_ISO_GAIN");
            case Column::Exposure:
                return tr("Exposure", "IDS_COLUMN_EXPOSURE");
            case Column::Aperture:
                return tr("Aperture", "IDS_COLUMN_APERTURE");
            case Column::FWHM:
                return tr("FWHM");
            case Column::Stars:
                return tr("#Stars", "IDS_COLUMN_STARS");
            case Column::Background:
                return tr("Sky Background", "IDS_COLUMN_SKYBACKGROUND");
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
                    {
                        int idx = value.toInt();
                        switch (idx)
                        {
                        case 0:
                            file.m_strType = QCoreApplication::translate("DSS::Group", "Light", "IDS_TYPE_LIGHT");
                            file.m_PictureType = PICTURETYPE_LIGHTFRAME;
                            break;
                        case 1:
                            file.m_strType = QCoreApplication::translate("DSS::Group", "Dark", "IDS_TYPE_DARK");
                            file.m_PictureType = PICTURETYPE_DARKFRAME;
                            file.m_bUseAsStarting = false;
                            break;
                        case 2:
                            file.m_strType = QCoreApplication::translate("DSS::Group", "Flat", "IDS_TYPE_FLAT");
                            file.m_PictureType = PICTURETYPE_FLATFRAME;
                            file.m_bUseAsStarting = false;
                            break;
                        case 3:
                            file.m_strType = QCoreApplication::translate("DSS::Group", "Dark Flat", "IDS_TYPE_DARKFLAT");
                            file.m_PictureType = PICTURETYPE_DARKFLATFRAME;
                            file.m_bUseAsStarting = false;
                            break;
                        case 4:
                            file.m_strType = QCoreApplication::translate("DSS::Group", "Bias/Offset", "IDS_TYPE_OFFSET");
                            file.m_PictureType = PICTURETYPE_OFFSETFRAME;
                            file.m_bUseAsStarting = false;
                            break;
                        }
                    }
                    break;
                case Column::Filter:
                    file.m_filterName = value.toString();
                    break;
                case Column::Score:
                    file.m_fOverallQuality = value.toDouble();
                    break;
                case Column::Quality:
                    file.quality = value.toDouble();
                case Column::dX:
                    file.m_dX = value.toDouble();
                    break;
                case Column::dY:
                    file.m_dY = value.toDouble();
                    break;
                case Column::Angle:
                    file.m_fAngle = value.toDouble();
                    break;
                case Column::ISO: 
                    if (file.m_lISOSpeed)
                        file.m_lISOSpeed = value.toInt();
                    else if (file.m_lGain >= 0)
                        file.m_lGain = value.toInt();
                    break;
                case Column::Exposure:
                    file.m_fExposure = value.toDouble();
                    break;
                case Column::Aperture:
                    file.m_fAperture = value.toDouble();
                    break;
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
    void ImageListModel::addImage(const ListBitMap& image)
    {
        ZASSERT(std::find(mydata.begin(), mydata.end(), image) == mydata.end());
            
        mydata.push_back(image);
    }

    //
    // Remove rows from the model.  Before making calls to removeRows in the
    // base class to do so, Qt requires that beginRemoveRows() is called,
    // and after removal, endRemoveRows() must be called.
    //
    bool ImageListModel::removeRows(int row, int count, const QModelIndex& parent)
    {
        //
        // Remove the rows from our backing data and then call
        // base class removeRows()
        //
        auto first{ std::next(mydata.begin(), row) };
        auto last{ first + count};
        mydata.erase(first, last);
        return Inherited::removeRows(row, count, parent);
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

        if (file.m_lNrChannels == 3)
            index += 0;              // Use xxxxColour Icons
        else if (IsCYMGType(file.GetCFAType()))
            index += 4;             // Use xxxxCMYG Icons
        else if (file.GetCFAType() != CFATYPE_NONE)
            index += 8;             // Use xxxxRGB Icons
        else
            index += 12;            // Use xxxxGreyscale Icons

        //
        // Populate the static Icon array once.
        // Note: The order is important!
        //
        static std::vector<QIcon> icons{ // Atomically and thread-safe initialise the icon array.
            QIcon{ ":/stacking/LightColour.png" },
            QIcon{ ":/stacking/DarkColour.png" },
            QIcon{ ":/stacking/FlatColour.png" },
            QIcon{ ":/stacking/BiasColour.png" },
            QIcon{ ":/stacking/LightCMYG.png" },
            QIcon{ ":/stacking/DarkCMYG.png" },
            QIcon{ ":/stacking/FlatCMYG.png" },
            QIcon{ ":/stacking/BiasCMYG.png" },
            QIcon{ ":/stacking/LightRGB.png" },
            QIcon{ ":/stacking/DarkRGB.png" },
            QIcon{ ":/stacking/FlatRGB.png" },
            QIcon{ ":/stacking/BiasRGB.png" },
            QIcon{ ":/stacking/LightGreyscale.png" },
            QIcon{ ":/stacking/DarkGreyscale.png" },
            QIcon{ ":/stacking/FlatGreyscale.png" },
            QIcon{ ":/stacking/BiasGreyscale.png" }
        };
       
        if (icons[index].isNull())
        {
            qDebug("null icon");
        }
        return icons[index];
    }

    QString ImageListModel::exposureToString(double exposure) const
    {
        QString strText;

        if (exposure)
        {
            if (exposure > 86399.999) exposure = 86399.999;		// 24 hours less 1 ms
            double msecs = exposure * 1000.0;
            QTime time{ QTime(0, 0) };
            time = time.addMSecs(msecs);
            if (exposure >= 1.0)
            {
                int hours{ time.hour() };
                int mins{ time.minute() };
                QString secs{ time.toString("s.z") };


                if (hours)
                    strText = QCoreApplication::translate("StackRecap", "%1 hr %2 mn %3 s ", "IDS_EXPOSURETIME3")
                    .arg(hours)
                    .arg(mins)
                    .arg(secs);
                else if (mins)
                    strText = QCoreApplication::translate("StackRecap", "%1 mn %2 s ", "IDS_EXPOSURETIME2")
                    .arg(mins)
                    .arg(secs);
                else
                    strText = QCoreApplication::translate("StackRecap", "%1 s ", "IDS_EXPOSURETIME1")
                    .arg(secs);
            }
            else
            {
                exposure = 1.0 / exposure;
                strText = QCoreApplication::translate("StackRecap", "1/%1 s", "IDS_EXPOSUREFORMAT_INF")
                    .arg(exposure);
            };
        }
        else
            strText = "-";

        return strText;
    }

    void ImageListModel::retranslateUi()
    {
        //
        // Warning: This code is copied from Group::addFile - if one changes, the other needs
        // to change as well.
        //
        for (auto& lb : mydata)
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

            if (lb.m_lNrChannels == 3)
                lb.m_strDepth = QCoreApplication::translate("DSS::Group", "RGB %1 bit/ch", "IDS_FORMAT_RGB").arg(lb.m_lBitsPerChannel);
            else
                lb.m_strDepth = QCoreApplication::translate("DSS::Group", "Gray %1 bit", "IDS_FORMAT_GRAY").arg(lb.m_lBitsPerChannel);

            if (lb.GetCFAType() != CFATYPE_NONE)
                lb.m_strCFA = QCoreApplication::translate("DSS::Group", "Yes", "IDS_YES");
            else
                lb.m_strCFA = QCoreApplication::translate("DSS::Group", "No", "IDS_NO");
        }
        //
        // Very important to tell the table view that the model has changed.
        //
        emitChanged(0, static_cast<int>(mydata.size() - 1));
    }
}