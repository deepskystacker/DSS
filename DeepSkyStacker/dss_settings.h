#pragma once

class CDSSSetting
{
public:
	CString				m_strName;
	CBezierAdjust		m_BezierAdjust;
	CRGBHistogramAdjust	m_HistoAdjust;

private:
	void	CopyFrom(const CDSSSetting& cds)
	{
		m_strName = cds.m_strName;
		m_BezierAdjust = cds.m_BezierAdjust;
		m_HistoAdjust = cds.m_HistoAdjust;
	};

public:
	CDSSSetting() {};
	virtual ~CDSSSetting() {};

	CDSSSetting(const CDSSSetting& cds)
	{
		CopyFrom(cds);
	};

	CDSSSetting& operator = (const CDSSSetting& cds)
	{
		CopyFrom(cds);
		return (*this);
	};

	bool operator < (const CDSSSetting& cds) const
	{
		int			nCompare;
		nCompare = m_strName.CompareNoCase(cds.m_strName);

		if (nCompare < 0)
			return true;
		else
			return false;
	};

	bool	Load(FILE* hFile)
	{
		LONG		lNameSize;
		TCHAR		szName[2000];

		fread(&lNameSize, sizeof(lNameSize), 1, hFile);
		fread(szName, sizeof(TCHAR), lNameSize, hFile);
		m_strName = szName;
		return m_BezierAdjust.Load(hFile) && m_HistoAdjust.Load(hFile);
	};

	bool	Save(FILE* hFile)
	{
		LONG		lNameSize = m_strName.GetLength() + 1;
		fwrite(&lNameSize, sizeof(lNameSize), 1, hFile);
		fwrite((LPCTSTR)m_strName, sizeof(TCHAR), lNameSize, hFile);

		return m_BezierAdjust.Save(hFile) && m_HistoAdjust.Save(hFile);
	};
};

typedef std::list<CDSSSetting>			DSSSETTINGLIST;
typedef	DSSSETTINGLIST::iterator		DSSSETTINGITERATOR;

class CDSSSettings
{
private:
	std::list<CDSSSetting>	m_lSettings;
	bool					m_bLoaded;

public:
	CDSSSettings()
	{
		m_bLoaded = false;
	};
	virtual ~CDSSSettings() {};

	bool	IsLoaded()
	{
		return m_bLoaded;
	};
	bool	Load(LPCTSTR szFile = nullptr);
	bool	Save(LPCTSTR szFile = nullptr);

	int	Count()
	{
		return static_cast<int>(m_lSettings.size());
	};

	bool	GetItem(int lIndice, CDSSSetting& cds)
	{
		bool			bResult = false;

		if (lIndice < static_cast<int>(m_lSettings.size()))
		{
			DSSSETTINGITERATOR	it;

			it = m_lSettings.begin();
			while (lIndice)
			{
				it++;
				lIndice--;
			};

			cds = (*it);
			bResult = true;
		};

		return bResult;
	};

	bool	Add(const CDSSSetting& cds)
	{
		m_lSettings.push_back(cds);
		return true;
	};

	bool	Remove(int lIndice)
	{
		bool			bResult = false;

		if (lIndice < static_cast<int>(m_lSettings.size()))
		{
			DSSSETTINGITERATOR	it;

			it = m_lSettings.begin();
			while (lIndice)
			{
				it++;
				lIndice--;
			};

			m_lSettings.erase(it);
			bResult = true;
		};

		return bResult;
	};
};

