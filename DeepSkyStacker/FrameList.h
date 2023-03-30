#pragma once
#include "group.h"
#include "MatchingStars.h"

class CAllStackingTasks;
class CBilinearParameters;
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
		// Return address of the relevant ListBitMap in the current group
		//
		ListBitMap* getListBitMap(const int row);

		//
		// Remove everything from all groups, and clear the mapping from path to group number
		//
		void clear();

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

		size_t groupSize(const int id) const;

		inline size_t groupCount() const
		{
			return imageGroups.size();
		}

		QString getFirstCheckedLightFrame() const;

		inline QString groupName(const int id) const
		{
			return imageGroups[id].name();
		}

		size_t checkedImageCount(const PICTURETYPE type, const int id = -1) const;

		size_t countUnregisteredCheckedLightFrames(const int id = -1) const;

		void fillTasks(CAllStackingTasks& tasks);

		bool isLightFrame(const QString name) const;

		bool isChecked(const QString name) const;

		bool getTransformation(const QString name, CBilinearParameters& transformation, VOTINGPAIRVECTOR& vVotedPairs) const;

		FrameList& saveListToFile(fs::path file);
		FrameList& loadFilesFromList(fs::path fileList);

		FrameList& beginInsertRows(const int count);
		FrameList& endInsertRows();

		bool addFile(fs::path file, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, bool bCheck = false, int nItem = -1);

		void blankCheckedItemScores() const;

		bool areCheckedImagesCompatible(QString& reason) const;

		void updateCheckedItemScores();

		void updateItemScores(const QString& fileName);

		QString getReferenceFrame() const;
		bool getReferenceFrame(CString& string) const;

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
		}

		inline FrameList& setDirty(const bool value = false) noexcept
		{
			for (auto& g : imageGroups)
			{
				g.setDirty(value);
			}
			return *this;
		}

		inline ImageListModel* currentTableModel()
		{
			return imageGroups[index].pictures.get();
		}

		inline void removeFromMap(fs::path file)
		{
			Group::removeFromMap(file);
		}

		// Change the name of the specified group
		void setGroupName(int id, const QString& name);

		//
		// retranslate group names unless changed
		//
		void retranslateUi();

	};
}
