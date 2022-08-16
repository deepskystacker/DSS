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
#include <QModelIndex>
#include <QVariant>

class ListBitMap;

namespace DSS
{
    class ImageListModel :
        public QAbstractTableModel
    {
        Q_OBJECT

        typedef QAbstractTableModel
            Inherited;

        std::vector<ListBitMap> mydata;
    public:
        enum class Column
        {
            PathCol = 0, FileCol, TypeCol, FilterCol, ScoreCol,
            dXCol, dYCol, AngleCol, DateTimeCol, SizeCol, CFACol, DepthCol,
            InfoCol, ISOCol, ExposureCol, ApertureCol, FWHMCol, StarsCol,
            BackgroundCol, MAX_COLS
        };


        using Inherited::beginInsertRows;
        using Inherited::endInsertRows;

        typedef std::vector<ListBitMap>::const_iterator const_iterator;
        typedef std::vector<ListBitMap>::iterator iterator;

        explicit ImageListModel(QObject* parent = 0) : QAbstractTableModel(parent)
        {
        };

        int rowCount(const QModelIndex& parent) const
        {
            // Return number of images we know about
            return static_cast<int>(mydata.size());
        };

        int columnCount(const QModelIndex& parent) const
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
            return QString::fromWCharArray(mydata[row].m_strFileName.GetString());
        }

        //
        // Check if a loaded image is a light frame
        //
        bool isLightFrame(QString name)
        {
            for (auto const& data : mydata)
            {
                if (data.m_strFileName == name.toStdWString().c_str())
                {
                    if (data.IsLightFrame()) return true;
                    else return false;
                 }
            }

            return false;
        }

        //
        // Check if a loaded image is a light frame
        //
        bool isChecked(QString name)
        {
            for (auto const& data : mydata)
            {
                if (data.m_strFileName == name.toStdWString().c_str())
                {
                    if (data.IsLightFrame()) return true;
                    else return false;
                }
            }

            return false;
        }

        //
        // Get the transformation if any
        //
        bool getTransformation(QString name, CBilinearParameters& transformation, VOTINGPAIRVECTOR& vVotedPairs)
        {
             for (auto const& data : mydata)
            {
                if (data.IsLightFrame() && data.m_strFileName == name.toStdWString().c_str())
                {
                    transformation = data.m_Transformation;
                    vVotedPairs = data.m_vVotedPairs;
                    return true;
                };
            };

            return false;
        }
    };
};

