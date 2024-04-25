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
#include "DSSCommon.h"
#include "ImageListModel.h"

class ListBitMap;

namespace DSS
{
	class Group final
	{
	private:
		friend class FrameList;
		using IndexType = unsigned int;

		//
		// Initial group id is zero
		//
		inline static constinit IndexType nextIndex{ 0 };
		//
		// Map of files to group id - used to check which group refers to a file (maybe none)
		//
		inline static std::map<fs::path, IndexType> pathToGroup{};

	public:

		//
		// Qt Table Model class derived from QAbstractTableModel
		//
		std::unique_ptr<ImageListModel> pictures;

		Group();
		//
		// Don't intend this to be copied or assigned.
		//
		Group(const Group&) = delete;
		Group& operator=(const Group&) = delete;
		Group(Group&& rhs) noexcept;
		Group& operator=(Group&& rhs) noexcept;

		//
		// Accessors
		//
		size_t size() const noexcept;

		inline QString name() const noexcept
		{
			return Name;
		}
		inline Group& setName(QString const& name) noexcept
		{
			Name = name;
			nameChanged = true;
			return *this;
		}
		inline bool dirty() const noexcept
		{
			return Dirty;
		}
		inline Group& setDirty(bool value = true) noexcept
		{
			Dirty = value;
			return *this;
		}

		IndexType index() const noexcept { return Index; };

		//
		// Will call addImage() internally
		//
 		void addFile(fs::path file, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, bool bCheck = false, int nItem = -1);

		//
		// Add an image (row) to the table
		//
		void addImage(const ListBitMap& image);

		static int whichGroupContains(const fs::path& path)
		{
			if (auto iter = pathToGroup.find(path); iter != pathToGroup.end())
				return static_cast<int>(iter->second);
			else
				return -1; // no group
		}

		static int32_t fileCount()
		{
			return static_cast<int32_t>(pathToGroup.size());
		}

		//
		// Reset to initial conditions - one group that is empty
		//
		static void reset()
		{
			nextIndex = 1;
			pathToGroup.clear();
		}

		static void removeFromMap(fs::path file)
		{
			pathToGroup.erase(pathToGroup.find(file));
		}

	private:
		IndexType Index;		// This group's number
		//
		// Every group has a name - initially "Main Group" or Group n"
		//
		QString Name;
		bool Dirty;
		bool nameChanged;		// set if user has renamed the group
	};
}
