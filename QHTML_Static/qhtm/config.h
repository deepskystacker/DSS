/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	config.h
Owner:	russf@gipsysoft.com
Purpose:	QHTM configuration

		Use the #defines below to add or remove QHTM functionality.

----------------------------------------------------------------------*/
#ifndef CONFIG_H
#define CONFIG_H


//
//	If you don't want cooltips support
#ifndef UNDER_CE
	#define QHTM_COOLTIPS
#endif	//	UNDER_CE

//
//	Comment out to totally remove imaging from QHTM
#define QHTM_BUILD_INTERNAL_IMAGING

//
//	Comment out to prevent QHTM from drawing focus for links
//	as well as keyboard navigation
#define QHTM_ALLOW_FOCUS


//
//	QHTM can use the <control ...> tag to create and display windows controls.
//	Uncomment it to try it. This is experimental and has some BUGS
#ifndef UNDER_CE
	#define	QHTM_WINDOWS_CONTROL
#endif	//	UNDER_CE


//
//	Printing 
#ifndef UNDER_CE
	#define QHTM_ALLOW_PRINT
#endif	//	UNDER_CE

//
//	Rendering HTML onto any device context
#ifndef UNDER_CE
	#define QHTM_ALLOW_RENDER
#endif	//	UNDER_CE


//
//	QHTM_MessageBox of course
#ifndef UNDER_CE
	#define QHTM_ALLOW_HTML_MESSAGEBOX
#endif	//	UNDER_CE

//
//	HTML on a button
#ifndef UNDER_CE
	#define QHTM_ALLOW_HTML_BUTTON
#endif	//	UNDER_CE

//
//	Allow HTML listboxes
#ifndef UNDER_CE
	#define QHTM_ALLOW_HTML_LISTBOX
#endif	//	UNDER_CE

//
//	Image object callback
#define QHTM_ALLOW_IMAGECALLBACK

//
//	resource callbacks...
#define QHTM_ALLOW_RESOURCECALLBACK

//
//	Allow the QHTM_ADD_HTML functionality
#define QHTM_ALLOW_ADD_HTML

//
//	QHTM tips for links etc.
#ifndef UNDER_CE
	#define QHTM_ALLOW_TOOLTIPS
#endif	//	UNDER_CE

#ifndef UNDER_CE
	#define QHTM_ALLOW_FORMS
#endif	//	UNDER_CE

//
//	Define this to stop linking the multiple monitor support
//	This is useful if you are statically linking against MFC
//#define _NO_MULTIMON_SUPPORT


#endif //CONFIG_H