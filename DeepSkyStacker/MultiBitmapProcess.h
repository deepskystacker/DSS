#pragma once

class CBitmapPart
{
public :
	CString						m_strFile;
	int						m_lStartRow;
	int						m_lEndRow;
	int						m_lWidth;
	int						m_lNrBitmaps;

private :
	void	CopyFrom(const CBitmapPart & bp)
	{
		m_strFile	= bp.m_strFile;
		m_lStartRow = bp.m_lStartRow;
		m_lEndRow	= bp.m_lEndRow;
		m_lWidth	= bp.m_lWidth;
		m_lNrBitmaps= bp.m_lNrBitmaps;
	};

public :
	CBitmapPart(LPCTSTR szFile, int lStartRow, int lEndRow)
	{
		m_strFile	= szFile;
		m_lStartRow = lStartRow;
		m_lEndRow	= lEndRow;
        m_lWidth = 0;
        m_lNrBitmaps = 0;
	};

	CBitmapPart(const CBitmapPart & bp)
	{
		CopyFrom(bp);
	};

	const CBitmapPart & operator = (const CBitmapPart & bp)
	{
		CopyFrom(bp);
		return (*this);
	};

	virtual ~CBitmapPart()
	{
	};
};

typedef std::vector<CBitmapPart>	BITMAPPARTVECTOR;
