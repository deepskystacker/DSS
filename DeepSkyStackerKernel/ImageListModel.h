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
#include "FrameInfo.h"
namespace DSS
{
    class ImageListModel :
        public QAbstractTableModel
    {
        Q_OBJECT

        typedef QAbstractTableModel
            Inherited;

        friend class FrameList;
        friend class StackingDlg;

        static inline std::mutex mutex {};
        static inline std::vector<QIcon> icons {};

        //
        // Change to use a deque instead of a vector as it offers O(n) insertion 
        // and deletion of elements unlike vector.  Cost is a somewhat higher 
        // cost in terms of storage.
        //
        std::deque<ListBitMap> mydata;

    public:
        using Inherited::beginInsertRows;
        using Inherited::endInsertRows;

        typedef std::deque<ListBitMap>::const_iterator const_iterator;
        typedef std::deque<ListBitMap>::iterator iterator;

        explicit ImageListModel(QObject* parent = nullptr);
 
        inline [[nodiscard]] int rowCount([[maybe_unused]] const QModelIndex& parent = QModelIndex()) const override
        {
            // Return number of images we know about
            return static_cast<int>(mydata.size());
        };

        inline [[nodiscard]] int columnCount([[maybe_unused]] const QModelIndex& parent = QModelIndex()) const override
        {
            // Pretty simple really
            return static_cast<int>(Column::MAX_COLS);
        };


        inline [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override
        {
            auto flags = Inherited::flags(index);
            switch (Column(index.column()))
            {
            case Column::Path:
                flags |= (Qt::ItemIsUserCheckable);
                break;
            case Column::Type:
            case Column::ISO:
            case Column::Exposure:
                flags |= (Qt::ItemIsEditable);
                break;
            }
            return flags;
        }

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        QVariant data(const int row, const Column column, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
        bool setData(const int row, const Column column, const QVariant& value, int role = Qt::EditRole);
        bool setSkyBackground(const int row, const CSkyBackground& bg, int role = Qt::EditRole);

        // Before adding a row to the model, Qt requires that
        // beginInsertRows() be called, and once batch of images is added, then
        // endInsertRows() must be called.
        void addImage(const ListBitMap& image);

        bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
        const_iterator cbegin()const { return mydata.begin(); }
        const_iterator cend()const { return mydata.end(); }
        iterator begin() { return mydata.begin(); }
        iterator end() { return mydata.end(); }

        //
        // return the file held in the base FrameInfo object
        //
        fs::path selectedFile(int row) const
        {
            return mydata[row].filePath;
        }

        //
        // Check if a loaded image is a light frame
        //
        bool isLightFrame(const fs::path& name) const
        {
            for (auto const& data : mydata)
            {
                if (data.filePath == name)
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
        bool isChecked(const fs::path& name) const
        {
            for (auto const& data : mydata)
            {
                if (data.filePath == name)
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

        void emitChanged(int startRow, int endRow, int startColumn = 0, int endColumn = static_cast<int>(Column::MAX_COLS)-1)
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
            if (!mydata.empty())
            {
                beginRemoveRows(QModelIndex(), 0, static_cast<int>(mydata.size()) - 1);
                mydata.clear();
                endRemoveRows();
            }
        }

        private:

        QString exposureToString(double exposure) const;

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

protected:
        void retranslateUi();
    };
};

