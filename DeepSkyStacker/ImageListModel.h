#pragma once
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

#include <vector>
#include <QObject>
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>
#include <QVariant>
#include "FrameList.h"


namespace DSS
{
    class ImageListModel :
        public QAbstractTableModel
    {
        Q_OBJECT

        typedef QAbstractTableModel
            Inherited;

        friend class FrameList;

        std::vector<ListBitMap> mydata;

        static inline const std::vector<QIcon> icons{
            QIcon("qrc:///stacking/DarkColour-16.png")
            //     etc...
        };

    public:
        enum class Column
        {
            Path = 0, File, Type, Filter, Score,
            dX, dY, Angle, FileTime, Size, CFA, Depth,
            Info, ISO, Exposure, Aperture, FWHM, Stars,
            Background, MAX_COLS
        };


        using Inherited::beginInsertRows;
        using Inherited::endInsertRows;

        typedef std::vector<ListBitMap>::const_iterator const_iterator;
        typedef std::vector<ListBitMap>::iterator iterator;

        explicit ImageListModel(QObject* parent = nullptr) : 
            QAbstractTableModel(parent)
        {
        };

        int rowCount(const QModelIndex& parent = QModelIndex()) const override
        {
            // Return number of images we know about
            return static_cast<int>(mydata.size());
        };

        int columnCount(const QModelIndex& parent = QModelIndex()) const override
        {
            // Pretty simple really
            return static_cast<int>(Column::MAX_COLS);
        };

        virtual QVariant data(const QModelIndex& index, int role) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
        bool setData(const int row, const Column column, const QVariant& value, int role = Qt::EditRole);
        bool setSkyBackground(const int row, const CSkyBackground& bg, int role = Qt::EditRole);

        // Before adding a row to the model, Qt requires that
        // beginInsertRows() be called, and once batch of images is added, then
        // endInsertRows() must be called.
        void addImage(ListBitMap image);

        void removeImage(int row);
        // bool setData(const QModelIndex& index, const QVariant& value, int role);
        // Qt::ItemFlags flags(const QModelIndex& index) const;
        const_iterator cbegin()const { return mydata.begin(); }
        const_iterator cend()const { return mydata.end(); }
        iterator begin() { return mydata.begin(); }
        iterator end() { return mydata.end(); }

        //
        // return the filename held in the base FrameInfo object
        //
        QString selectedFileName(int row) const
        {
            return QString::fromStdU16String(mydata[row].filePath.generic_u16string());
        }

        //
        // Check if a loaded image is a light frame
        //
        bool isLightFrame(QString name) const
        {
            for (auto const& data : mydata)
            {
                if (data.filePath == fs::path(name.toStdString()))
                {
                    if (data.IsLightFrame()) return true;
                    else return false;
                 }
            }

            return false;
        }

        //
        // Check if a loaded image has been checked
        //
        bool isChecked(QString name) const
        {
            for (auto const& data : mydata)
            {
                if (data.filePath == fs::path(name.toStdU16String()))
                {
                    if (Qt::Checked == data.m_bChecked) return true;
                    else return false;
                }
            }

            return false;
        }

        //
        // Get the transformation if any
        //
        bool getTransformation(QString name, CBilinearParameters& transformation, VOTINGPAIRVECTOR& vVotedPairs) const
        {
            for (auto const& data : mydata)
            {
                if (data.IsLightFrame() && data.filePath == fs::path(name.toStdU16String()))
                {
                    transformation = data.m_Transformation;
                    vVotedPairs = data.m_vVotedPairs;
                    return true;
                };
            };

            return false;
        }

        void emitChanged(int startRow, int endRow, int startColumn = 0, int endColumn = static_cast<int>(Column::MAX_COLS))
        {
            QModelIndex start{ createIndex(startRow, startColumn) };
            QModelIndex end{ createIndex(endRow, endColumn) };
            emit dataChanged(start, end);
        }

        //
        // Delete everything
        //
        inline void clear()
        {
            removeRows(0, static_cast<int>(mydata.size()));
            mydata.clear();
        }

        private:

        inline QString exposureToString(double fExposure) const
        {
            QString strText;

            if (fExposure)
            {
                qint64			exposure;

                if (fExposure >= 1)
                {
                    exposure = fExposure;
                    qint64			remainingTime = exposure;
                    qint64			hours, mins, secs;

                    hours = remainingTime / 3600;
                    remainingTime -= hours * 3600;
                    mins = remainingTime / 60;
                    remainingTime -= mins * 60;
                    secs = remainingTime;

                    if (hours)
                        strText = QString(QCoreApplication::translate("StackRecap", "%1 hr %2 mn %3 s ", "IDS_EXPOSURETIME3"))
                        .arg(hours)
                        .arg(mins)
                        .arg(secs);
                    else if (mins)
                        strText = QString(QCoreApplication::translate("StackRecap", "%1 mn %2 s ", "IDS_EXPOSURETIME2"))
                        .arg(mins)
                        .arg(secs);
                    else
                        strText = QString(QCoreApplication::translate("StackRecap", "%1 s ", "IDS_EXPOSURETIME1"))
                        .arg(secs);
                }
                else
                {
                    exposure = 1.0 / fExposure + 0.5;
                    strText = QString(QCoreApplication::translate("StackRecap", "1/%1 s", "IDS_EXPOSUREFORMAT_INF"))
                        .arg(exposure);
                };
            }
            else
                strText = "-";

            return strText;
        }

        inline QString isoToString(int lISOSpeed) const
        {
            if (lISOSpeed)
                return QString::number(lISOSpeed);
            else
                return QString("-");
        }

        inline QString gainToString(int lGain) const
        {
            if (lGain >= 0)
                return QString::number(lGain);
            else
                return QString("-");
        }

        QVariant rowIcon(const ListBitMap& file) const;
    };
};

