#pragma once

#include <QModelIndex>
#include <QString>
#include "DSSProgress.h"
#include "BitmapExt.h"
#include "FrameInfo.h"
#include "DSSTools.h"
#include "MatchingStars.h"

#include "StackingTasks.h"
#include "RegisterEngine.h"
#include "group.h"

namespace DSS
{
	class FrameList
	{
	public:
		std::uint16_t index;		// Initially zero - is the group we are currently working with

	typedef std::vector<Group>::const_iterator const_group_iterator;


	private:
		std::vector<Group>	imageGroups;
		std::uint16_t lastGroup;

	public:
		FrameList() :
			index{ 0 },
			lastGroup{ 0 }
		{
			imageGroups.emplace_back();
		};

		virtual ~FrameList()
		{
		};

		const_group_iterator groups_cbegin() const { return imageGroups.begin(); }
		const_group_iterator groups_cend() const { return imageGroups.end(); }

		void changePictureType(int nItem, PICTURETYPE PictureType);

		void checkAbove(double threshold);

		void checkBest(double fPercent);

		void checkAll(bool value);

		void checkAllDarks(bool check);

		void checkAllFlats(bool check);

		void checkAllOffsets(bool check);

		void checkAllLights(bool check);

		void checkImage(const QString& image, bool check);

		ListBitMap* getListBitMap(int row)
		{
			//
			// return address of the relevant ListBitMap in the current
			// group
			//
			return &imageGroups[index].pictures->mydata[row];
		}

		//
		// Remove everything from all groups, and clear the mapping from path to group number
		//
		void clear()
		{
			for (auto& group : imageGroups)
			{
				group.pictures->clear();
			}
			imageGroups.resize(1);
			Group::reset();
		}

		std::uint16_t groupId() const noexcept
		{
			return index;
		};
		FrameList& setGroup(uint16_t id) noexcept
		{
			index = id;
			return *this;
		};

		inline std::uint16_t lastGroupId() const noexcept
		{
			return static_cast<uint16_t>(imageGroups.size() - 1);
		}

		inline std::uint16_t addGroup()
		{
			imageGroups.emplace_back();
			return static_cast<uint16_t>(imageGroups.size() - 1);
		}

		inline size_t groupSize(uint16_t id) const
		{
			ZASSERTSTATE(id < imageGroups.size());
			return imageGroups[id].size();
		}

		inline size_t groupCount() const
		{
			return imageGroups.size();
		}

		QString getFirstCheckedLightFrame();

		inline QString groupName(std::uint16_t id) const noexcept
		{
			return imageGroups[id].name();
		}
		size_t checkedImageCount(const PICTURETYPE type, const int16_t id = -1) const;

		size_t countUnregisteredCheckedLightFrames(int id = -1) const;

		void fillTasks(CAllStackingTasks& tasks);

		inline bool isLightFrame(QString name) const
		{
			return imageGroups[index].pictures->isLightFrame(name);
		};

		inline bool isChecked(QString name) const
		{
			return imageGroups[index].pictures->isChecked(name);
		}

		inline bool getTransformation(QString name, CBilinearParameters& transformation, VOTINGPAIRVECTOR& vVotedPairs) const
		{
			return imageGroups[index].pictures->getTransformation(name, transformation, vVotedPairs);
		}

		FrameList& saveListToFile(fs::path file);
		FrameList& loadFilesFromList(fs::path fileList);

		inline FrameList& beginInsertRows(int count)
		{
			auto first{ imageGroups[index].pictures->rowCount() };	// Insert after end
			auto last{ first + count - 1 };
			imageGroups[index].pictures->beginInsertRows(QModelIndex(), first, last);
			return (*this);
		}

		inline FrameList& endInsertRows()
		{
			imageGroups[index].pictures->endInsertRows();
			return *this;
		}

		virtual bool addFile(fs::path file, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, bool bCheck = false, [[maybe_unused]]int nItem = -1)
		{
			imageGroups[index].addFile(file, PictureType, bCheck);
			return true;
		}

		void blankCheckedItemScores();

		bool areCheckedImagesCompatible(QString& reason);

		void updateCheckedItemScores();

		void updateItemScores(const QString& fileName);

		QString getReferenceFrame();
		bool getReferenceFrame(CString& string);

		void clearOffsets();

		void clearOffset(fs::path file);

		void updateOffset(fs::path file, double xOffset, double yOffset, double angle, const CBilinearParameters& bilinearParameters, const VOTINGPAIRVECTOR vVotedPairs);

		inline bool dirty() const
		{
			//
			// If any group is dirty we are dirty
			//
			for (auto const& g : imageGroups)
			{
				if (g.dirty()) return true;
			}
			return false;
		};

		inline FrameList& setDirty(bool value = false) noexcept
		{
			for (auto& g : imageGroups)
			{
				g.setDirty(value);
			};
			return *this;
		};

		inline ImageListModel* currentTableModel()
		{
			return imageGroups[index].pictures.get();
		}

		inline void removeFromMap(fs::path file)
		{
			Group::removeFromMap(file);
		};

		// Change the name of the specified group
		void setGroupName(std::uint16_t id, const QString& name);

		//
		// retranslate group names unless changed
		//
		void retranslateUi();

	};
};

