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
	class FrameList final
	{
	public:
		int index{ 0 };		// Initially zero - is the group we are currently working with

	private:
		std::vector<Group> imageGroups;

	public:
		FrameList()
		{
			imageGroups.emplace_back();
		};
		FrameList(const FrameList&) = delete;
		FrameList(FrameList&&) = delete;
		FrameList& operator=(const FrameList&) = delete;
		FrameList& operator=(FrameList&&) = delete;
		~FrameList() = default;

		auto groups_cbegin() const { return imageGroups.cbegin(); }
		auto groups_cend() const { return imageGroups.cend(); }

		void changePictureType(int nItem, PICTURETYPE PictureType);

		void checkAbove(double threshold);

		void checkBest(double fPercent);

		void checkAll(bool value);

		void checkAllDarks(bool check);

		void checkAllFlats(bool check);

		void checkAllOffsets(bool check);

		void checkAllLights(bool check);

		void checkImage(const QString& image, bool check);

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

		inline void setGroup(int id) noexcept
		{
			index = id;
		}

		inline int lastGroupId() const noexcept
		{
			return static_cast<int>(imageGroups.size()) - 1;
		}

		inline void addGroup()
		{
			imageGroups.emplace_back();
		}

		inline size_t groupSize(const int id) const
		{
			ZASSERTSTATE(id < imageGroups.size());
			ZASSERTSTATE(id >= 0);
			return imageGroups[id].size();
		}

		inline size_t groupCount() const
		{
			return imageGroups.size();
		}

		QString getFirstCheckedLightFrame();

		inline QString groupName(const int id) const
		{
			return imageGroups[id].name();
		}

		size_t checkedImageCount(const PICTURETYPE type, const int id = -1) const;

		size_t countUnregisteredCheckedLightFrames(const int id = -1) const;

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

		void updateOffset(fs::path file, double xOffset, double yOffset, double angle, const CBilinearParameters& bilinearParameters, const VOTINGPAIRVECTOR& vVotedPairs);

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
		void setGroupName(int id, const QString& name);

		//
		// retranslate group names unless changed
		//
		void retranslateUi();

	};
};

