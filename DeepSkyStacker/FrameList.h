#ifndef __FRAMELIST_H__
#define __FRAMELIST_H__

#include <QModelIndex>
#include "DSSProgress.h"
#include "BitmapExt.h"
#include "FrameInfo.h"
#include "DSSTools.h"
#include "MatchingStars.h"

#include "StackingTasks.h"
#include "RegisterEngine.h"

/* ------------------------------------------------------------------- */

class CMRUList
{
public :
	QString					baseKeyName;
	std::vector<QString>	m_vLists;
	int					m_lMaxLists;

private :
	void	CopyFrom(const CMRUList & ml)
	{
		m_vLists		= ml.m_vLists;
		m_lMaxLists		= ml.m_lMaxLists;
	};

public :
	CMRUList(char * baseKey = nullptr)
	{
		if (baseKey)
			baseKeyName = baseKey;
		else
			baseKeyName = "FileLists";
		m_lMaxLists = 10;
	};

	void setBasePath(const QString& baseKey)
	{
		baseKeyName = baseKey;
	};

	CMRUList(const CMRUList& ml) = delete;
	//{
	//	CopyFrom(ml);
	//};

	~CMRUList() {};

	CMRUList& operator = (const CMRUList& ml) = delete;
	//{
	//	CopyFrom(ml);
	//	return (*this);
	//};

	void	readSettings();
	void	saveSettings();

	void	Add(const QString& list);
	inline void	Add(LPCTSTR szList)
	{
		QString list = QString::fromWCharArray(szList);
	}
};

/* ------------------------------------------------------------------- */

class ListBitMap : public CFrameInfo
{
public :
	bool					m_bRemoved;
	uint16_t				m_groupId;
	bool					m_bUseAsStarting;
	QString					m_strType;
	QString					m_strPath;
	QString					m_strFile;
	bool					m_bRegistered;
	Qt::CheckState			m_bChecked;
	double					m_fOverallQuality;
	double					m_fFWHM;
	double					m_dX;
	double					m_dY;
	double					m_fAngle;
	CSkyBackground			m_SkyBackground;
	bool					m_bDeltaComputed;
	QString					m_strCFA;
	QString					m_strSizes;
	QString					m_strDepth;
	bool					m_bCompatible;
	CBilinearParameters		m_Transformation;
	VOTINGPAIRVECTOR		m_vVotedPairs;
	int					m_lNrStars;
	bool					m_bComet;


protected :
	void	CopyFrom(const ListBitMap & lb)
	{
		CFrameInfo::CopyFrom(lb);

		m_groupId			= lb.m_groupId;
		m_bRemoved			= lb.m_bRemoved;
		m_bUseAsStarting	= lb.m_bUseAsStarting;
		m_strType			= lb.m_strType;
		m_strPath			= lb.m_strPath;
		m_strFile			= lb.m_strFile;
		m_bRegistered		= lb.m_bRegistered;
		m_bChecked			= lb.m_bChecked;
		m_fOverallQuality	= lb.m_fOverallQuality;
		m_fFWHM				= lb.m_fFWHM;
		m_dX				= lb.m_dX;
		m_dY				= lb.m_dY;
		m_fAngle			= lb.m_fAngle;
		m_bDeltaComputed	= lb.m_bDeltaComputed;
		m_strCFA			= lb.m_strCFA;
		m_strSizes			= lb.m_strSizes;
		m_strDepth			= lb.m_strDepth;
		m_bCompatible		= lb.m_bCompatible;
		m_Transformation	= lb.m_Transformation;
		m_vVotedPairs		= lb.m_vVotedPairs;
		m_lNrStars			= lb.m_lNrStars;
		m_bComet			= lb.m_bComet;
		m_SkyBackground		= lb.m_SkyBackground;
	};

public :
	ListBitMap()
	{
		m_groupId			= 0;
		m_bRemoved			= false;
		m_bUseAsStarting	= false;
		m_bRegistered		= false;
		m_bChecked			= Qt::Unchecked;
		m_fOverallQuality	= 0;
		m_fFWHM				= 0;
		m_dX				= 0;
		m_dY				= 0;
		m_fAngle			= 0;
		m_bDeltaComputed	= false;
		m_bCompatible		= true;
		m_lNrStars			= 0;
		m_bComet			= 0;
	};

	ListBitMap(const ListBitMap & lb)
	{
		CopyFrom(lb);
	};

	ListBitMap & operator = (const ListBitMap & lb)
	{
		CopyFrom(lb);
		return (*this);
	};

	bool	IsUseAsStarting()
	{
		return m_bUseAsStarting;
	};

	bool	IsDeltaComputed()
	{
		return m_bDeltaComputed;
	};

	inline bool operator ==(const ListBitMap& rhs) const
	{
		return (m_strPath == rhs.m_strPath && m_strFile == rhs.m_strFile);
	}

	inline bool operator !=(const ListBitMap& rhs) const
	{
		return !(*this == rhs);
	}

	void	EraseFile()
	{
		fs::path path{ filePath };
		fs::remove(path);
		if (IsLightFrame())
		{
			path.replace_extension("Info.txt");
			fs::remove(path);
		};
	};
};

typedef std::vector<ListBitMap>		LISTBITMAPVECTOR;

/* ------------------------------------------------------------------- */

class CFrameList
{
public :
	LISTBITMAPVECTOR	m_vFiles;
	bool				m_bDirty;
	std::uint16_t		m_groupId;

public :
	CFrameList() :
		m_bDirty(false),
		m_groupId(0)
	{
	};

	virtual ~CFrameList()
	{
	};

	std::uint16_t	currentGroupId()
	{
		return m_groupId;
	}

	void	SaveListToFile(LPCTSTR szFile);
	void	LoadFilesFromList(LPCTSTR szFileList);

	bool	AddFile(LPCTSTR szFile, uint16_t groupId = 0, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, bool bCheck = false)
	{
		//AddFileToList(szFile, groupId, PictureType, bCheck);
		return true;
	};

	void	FillTasks(CAllStackingTasks & tasks);
	bool	GetReferenceFrame(CString & strReferenceFrame);
	int	GetNrUnregisteredCheckedLightFrames(int lGroupID = -1);

	bool	IsDirty(bool bReset = false)
	{
		bool			bResult = m_bDirty;

		if (bReset)
			m_bDirty = false;

		return bResult;
	};
};

#include "dssgroup.h"

namespace DSS
{
	class FrameList
	{
	public:
		std::uint16_t index;		// Initially zero - is the group we are currently working with

	private:
		std::vector<Group>	imageGroups;

	public:
		FrameList() :
			index(0)
		{
			imageGroups.emplace_back();
		};

		virtual ~FrameList()
		{
		};

		void checkAbove(double threshold);

		void checkBest(double fPercent);

		void checkAll(bool value);

		void checkAllDarks(bool check);

		void checkAllFlats(bool check);

		void checkAllOffsets(bool check);

		void checkAllLights(bool check);



		//
		// Remove everything from all groups, and clear the mapping from path to group number
		//
		void clear()
		{
			for (auto& group : imageGroups)
			{
				group.pictures.clear();
			}
			Group::clearMap();
		};

		std::uint16_t groupId() const noexcept
		{
			return index;
		};
		FrameList& setGroup(uint16_t id) noexcept
		{
			index = id;
			return *this;
		};

		QString getFirstCheckedLightFrame();

		inline QString groupName(std::uint16_t id) const noexcept
		{
			return imageGroups[id].name();
		}
		size_t checkedImageCount(const PICTURETYPE type, const int16_t id = -1) const;

		size_t unregisteredCheckedLightFrameCount(int id = -1) const;

		void fillTasks(CAllStackingTasks& tasks);

		inline bool isLightFrame(QString name) const
		{
			return imageGroups[index].pictures.isLightFrame(name);
		};

		inline bool isChecked(QString name) const
		{
			return imageGroups[index].pictures.isChecked(name);
		}

		inline bool getTransformation(QString name, CBilinearParameters& transformation, VOTINGPAIRVECTOR& vVotedPairs) const
		{
			return imageGroups[index].pictures.getTransformation(name, transformation, vVotedPairs);
		}

		FrameList& saveListToFile(fs::path file);
		FrameList& loadFilesFromList(fs::path fileList);

		inline FrameList& beginInsertRows(int count) noexcept
		{
			auto first{ imageGroups[index].pictures.rowCount() };	// Insert after end
			auto last{ first + count };
			imageGroups[index].pictures.beginInsertRows(QModelIndex(), first, last);
			return (*this);
		}

		inline FrameList& endInsertRows() noexcept
		{
			imageGroups[index].pictures.endInsertRows();
			return *this;
		}

		virtual bool addFile(fs::path file, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, bool bCheck = false, int nItem = -1)
		{
			imageGroups[index].addFile(file, PictureType, bCheck);
			return true;
		}

		void blankCheckedItemScores();

		bool areCheckedImagesCompatible();

		void updateCheckedItemScores();

		QString getReferenceFrame();

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
			return &(imageGroups[index].pictures);
		}

	};
}

/* ------------------------------------------------------------------- */

#endif // __FRAMELIST_H__