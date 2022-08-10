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

#include <memory>

#include <QCoreApplication>
#include <QString>
#include "ImageListModel.h" 

namespace DSS
{
	class Group
	{
	private:
		//
		// Initial group id is zero
		//
		inline static std::uint16_t nextIndex{ 0 };

	public:

		//
		// Qt Table Model class derived from QAbstractTableModel
		//
		ImageListModel	model;

		explicit Group::Group() :
			Index(nextIndex++) 			// First group is Main Group with Index of 0
		{
			if (0 == Index)
			{
				Name = QCoreApplication::translate("StackingDlg", "Main Group");
			}
			else
			{
				Name = QCoreApplication::translate("StackingDlg", "Group %1").arg(Index);
			}
		}

		//
		// This is a very simple class so we can let the compiler synthesise the copy ctor 
		// and operator =
		//

		//
		// Accessors
		//
		QString name() const noexcept { return Name; };
		Group& setName(QString const& name) noexcept { Name = name; return *this; };

		uint index() const noexcept { return Index; };

		//
		// Will call addImage() internally
		//
 		void AddFile(LPCTSTR szFile, uint16_t groupId = 0, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, bool bCheck = false);

		//
		// Add an image (row) to the table
		//
		void addImage(ListBitMap image)
		{
			model.addImage(image);
		}

	protected:
		uint16_t Index;		// This group's number
		//
		// Every group has a name - initially "Main Group" or Group n"
		//
		QString Name;


	};
}
