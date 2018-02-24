/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	SmoothAnimationCalc.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef SMOOTHANIMATIONCALC_H
#define SMOOTHANIMATIONCALC_H

namespace GS
{
	class CSmoothAnimationCalc
	{
	public:
		CSmoothAnimationCalc( UINT uDivisor )
			: m_uDivisor( uDivisor )
			, m_uFrom( 0 )
			, m_uTo( 0 )
		{
			ASSERT( uDivisor );
		}

		void SetSizes( UINT uFrom, UINT uTo )
		{
			m_uFrom = uFrom;
			m_uTo = uTo;
		}

		UINT GetSize() const { return m_uFrom; }
		bool IsFinished() const { return m_uFrom == m_uTo; }
		void Increment()
		{
			if( m_uFrom < m_uTo )
			{
				if( m_uTo - m_uFrom <= m_uDivisor )
				{
					m_uFrom = m_uTo;
				}
				else
				{
					m_uFrom += ( m_uTo - m_uFrom ) / m_uDivisor;
				}
			}
			else
			{
				if( m_uFrom - m_uTo <= m_uDivisor )
				{
					m_uFrom = m_uTo;
				}
				else
				{
					m_uFrom -= ( m_uFrom - m_uTo ) / m_uDivisor;
				}
			}
		}

	private:
		UINT m_uDivisor;
		UINT m_uFrom;
		UINT m_uTo;
	};
};

#endif //SMOOTHANIMATIONCALC_H