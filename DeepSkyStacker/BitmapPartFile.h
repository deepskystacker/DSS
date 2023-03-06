#pragma once

class CBitmapPartFile
{
public:
	QString m_strFile;
	int m_lStartRow;
	int m_lEndRow;
	int m_lWidth;
	int m_lNrBitmaps;

private:
	void	CopyFrom(const CBitmapPartFile& bp)
	{
		m_strFile = bp.m_strFile;
		m_lStartRow = bp.m_lStartRow;
		m_lEndRow = bp.m_lEndRow;
		m_lWidth = bp.m_lWidth;
		m_lNrBitmaps = bp.m_lNrBitmaps;
	};

public:
	CBitmapPartFile(const QString& szFile, int lStartRow, int lEndRow)
	{
		m_strFile = szFile;
		m_lStartRow = lStartRow;
		m_lEndRow = lEndRow;
		m_lWidth = 0;
		m_lNrBitmaps = 0;
	};

	CBitmapPartFile(const CBitmapPartFile& bp)
	{
		CopyFrom(bp);
	};

	const CBitmapPartFile& operator= (const CBitmapPartFile& bp)
	{
		CopyFrom(bp);
		return *this;
	};

	virtual ~CBitmapPartFile()
	{}
};
typedef std::vector<CBitmapPartFile> BITMAPPARTFILEVECTOR;
