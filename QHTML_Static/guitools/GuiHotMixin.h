/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GuiHotMixin.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef GUIHOTMIXIN_H
#define GUIHOTMIXIN_H

//	function 'blah' not inlined
#pragma warning( disable: 4710 )

namespace GS
{
	template< class T >
	class CGuiHotMixin : public T
	{
	protected:
		CGuiHotMixin( T *pParent )
			: T( pParent )
			, m_bMouseInObject( false )
			{}

		virtual void OnMouseLeave()
		{
			//TRACE( _T("OnMouseLeave\n") );
			m_bMouseInObject = false;
			ForceRedraw( *this );
		}

		virtual void OnMouseEnter()
		{
			//TRACE( _T("OnMouseEnter\n") );
			m_bMouseInObject = true;
			ForceRedraw( *this );
		}

		bool IsMouseInObject() const { return m_bMouseInObject; }

	private:
		bool m_bMouseInObject;
		
	};
}

#endif //GUIHOTMIXIN_H