/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GuiBaseWindowObject.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef GUIBASEWINDOWOBJECT_H
#define GUIBASEWINDOWOBJECT_H

#ifndef GUIBASEOBJECT_H
	#include <guitools/GuiBaseObject.h>
#endif	//	GUIBASEOBJECT_H

#ifndef MAP_CONTAINER_H
	#include <reuse/map.h>
#endif	//	MAP_CONTAINER_H

namespace GS
{
	#define WINDOW_DATA	0
	enum {knDeadWindow = 1};


	template <class base>
	LRESULT CALLBACK TemplateWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		LPARAM lparam = GetWindowLongPtr( hwnd, WINDOW_DATA );
		base *pWnd = reinterpret_cast<base*>( lparam );

		switch( message )
		{
		case WM_NCCREATE:
			if( pWnd == NULL )
			{
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>( lParam );
				pWnd = new base( hwnd, lpcs->style );
#if defined(_WIN64)
				::SetWindowLongPtr( hwnd, WINDOW_DATA, reinterpret_cast<LONG_PTR>( pWnd ) );
#else
				::SetWindowLong(hwnd, WINDOW_DATA, reinterpret_cast<LONG>(pWnd));
#endif
			}
		}

		if( lparam == GS::knDeadWindow || !pWnd )
		{
			return ::DefWindowProc( hwnd, message, wParam, lParam );
		}

		ASSERT( pWnd );
		LRESULT lr = pWnd->OnWindowsMessage( message, wParam, lParam );
		switch( message )
		{
		case WM_NCDESTROY:
#if defined(_WIN64)
			::SetWindowLongPtr(hwnd, WINDOW_DATA, reinterpret_cast<LONG_PTR>(GS::knDeadWindow));
#else
			::SetWindowLong(hwnd, WINDOW_DATA, reinterpret_cast<LONG>(GS::knDeadWindow));
#endif
			delete pWnd;
			break;
		}
		return lr;
	}

	//
	//
	//	Declare your intention to use this as a window
	#define DECLARE_WINDOW_TYPE	\
		static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );	\
		static bool Create( WinHelper::CRect &rc, HWND hwndParent, DWORD dwStyle, HMENU hmenu, HINSTANCE hInstance )	\


	//
	//	Define the object as a window
	#define DEFINE_WINDOW_TYPE( T, pcszClass )	\
		LRESULT CALLBACK T::WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )\
		{\
			return GS::TemplateWndProc<T>( hwnd, message, wParam, lParam );\
		}\
		\
		bool T::Create( WinHelper::CRect &rc, HWND hwndParent, DWORD dwStyle, HMENU hmenu, HINSTANCE hInstance )	\
		{	\
			if( hInstance )	\
			{	\
				WNDCLASSEX wcex = {0};	\
				wcex.cbSize = sizeof( WNDCLASSEX );	\
				wcex.style					= CS_DBLCLKS | CS_PARENTDC;	\
				wcex.lpfnWndProc		= (WNDPROC)T::WndProc;	\
				wcex.cbClsExtra			= 0;	\
				wcex.cbWndExtra			= sizeof( T * );	\
				wcex.hInstance			= hInstance;	\
				wcex.hbrBackground	= NULL;	\
				wcex.lpszMenuName		= NULL;	\
				wcex.lpszClassName	= pcszClass;	\
				if( !RegisterClassEx( &wcex ) )	\
				{	\
					TRACE( _T("Failed to register class: %s\n"), pcszClass );\
					return false;	\
				}	\
			}\
			if( ::CreateWindow( pcszClass, NULL, dwStyle, rc.left, rc.top, rc.Width(), rc.Height(), hwndParent, hmenu, hInstance, NULL ) ) \
			{	\
				return true;\
			}\
			return false;	\
		}


	class CGuiBaseWindowObject : public CGuiBaseObject
	{
	public:
		CGuiBaseWindowObject( CGuiBaseObject *pgboParent, HWND hwnd );
		virtual ~CGuiBaseWindowObject();

		virtual void OnCloseWindow();
		virtual bool OnCreateWindow();

		UINT StartTimer( CGuiBaseObject *pSect, UINT nInterval );
		void StopTimer( UINT uTimerID );
		
		void ForceRedraw( const WinHelper::CRect &rc );

		HWND GetSafeHwnd() const { return m_hwnd; }

		void DestroyWindow()
			{ ::DestroyWindow( m_hwnd ); m_hwnd = NULL; }

		virtual LRESULT OnWindowsMessage( UINT message, WPARAM wParam, LPARAM lParam );
	protected:
		virtual void OnTimer( UINT uTimerID );

		void CancelMouseDown();
		void CancelHighlight();
		HWND m_hwnd;

		//
		//	Timer stuff
		Container::CMap<int, CGuiBaseObject *> m_mapTimerEvents;
		UINT m_uNextTimerID;


		//
		//	Our mouse enter/leave timer. This MUST MUST MUST come after the
		//	map for timer events otherwise the map is destroyed first and then
		//	out timer attempts to remove itself...which results in an ASSERT party
		//	that is hard to get out of.
		CTimer m_timerEnterLeave;

		CGuiBaseObject *m_pgboMouseDowned;
		CGuiBaseObject *m_pgboHighlighted;
	};
};

#endif //GUIBASEWINDOWOBJECT_H