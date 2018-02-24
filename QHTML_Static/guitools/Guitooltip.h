/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Guitooltip.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef GUITOOLTIP_H
#define GUITOOLTIP_H

#if !defined (_WIN32_WCE)

	namespace GS
	{

		class CGuiTooltip
		{
		public:
			inline HWND GetSafeHwnd() { return m_hwnd; }

			bool AddTool( HWND hwnd, RECT &rc, LPCTSTR pcszText, UINT uID );
			bool AddTool( HWND hwnd, LPCTSTR pcszText );
			void UpdateTipText( HWND hwnd, LPCTSTR pcszText );

			void RemoveAll();

			void UpdateToolRect( HWND hwnd, UINT uID, const RECT &rc );

			bool Create( HWND hwndParent );
			void Activate( BOOL bActive = TRUE );

			CGuiTooltip();
			virtual ~CGuiTooltip();

		private:
			HWND m_hwnd;
		};
	};
#endif	//	!defined (_WIN32_WCE)

#endif //GUITOOLTIP_H