/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	FrameData.h
Owner:	russf@gipsysoft.com

Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef FRAMEDATA_H
#define FRAMEDATA_H

class CFrameData
{
public:
	CFrameData()
		: m_nRefCount( 0 )
		, m_nLoopCount( 0 )
		{
			m_size.cx = m_size.cy = 0;
		}

	inline const GS::CDIB *GetFrameImage( size_t nFrame ) const
	{
		return m_arrFrames[ nFrame ]->GetDib();
	}

	inline GS::CDIB *GetFrameImage( size_t nFrame )
	{
		return m_arrFrames[ nFrame ]->GetDib();
	}

	void CleanupFrames()
		{
			m_size.cx = m_size.cy = 0;

			for( UINT u = 0; u < m_arrFrames.GetSize(); u++ )
			{
				delete m_arrFrames[ u ];
			}
			m_arrFrames.RemoveAll();
		}

	int GetRefCount() { return m_nRefCount; }

	void AddRef() {	m_nRefCount++; }

	void Release()
		{
			m_nRefCount--;
			if( !m_nRefCount )
				delete this;
		}

	void SetLoopCount( UINT uCount )
	{
		m_nLoopCount = uCount;
	}

	UINT GetImageLoopCount() const
	{
		return m_nLoopCount;
	}

	SIZE m_size;
	CFrameArray	m_arrFrames;
private:
	~CFrameData()
		{
			CleanupFrames();
		}

	int m_nRefCount;
	int m_nLoopCount;
};

#endif //FRAMEDATA_H