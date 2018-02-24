// ==========================================================================
//								Class Specification : 
//							COXListBoxEx & COXFontListBox
// ==========================================================================

// Header file : OXListBoxEx.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////


/*

	OVERVIEW	-	COXListBoxEx

COXListBoxEx is CListBox derived class that provides a lot of additional 
functionality that might be at different circumstances. New features mostly
have to do with the way list box items are displayed. Using this class
you can set images to be displayed altogether with text. You can easily
assign any font, text and background color, indention from the left side 
for any item in a list box. Also you can assign a tooltip to any item so 
whenever mouse is moved in the item rectangle the corresponding tooltip 
will be displayed. You can specify background color and tooltip for the 
list box control as well.

COXListBoxEx class introduces new functionality that might be espesially
useful in combo box control (to use COXListBoxEx derived class as combo box 
dropdown list box). This functionality allows you to assign some items as
Most Recent Used (MRU) items. MRU items always reside at the top of the list 
and separated from the rest of the items by separator (line is drawn between
last MRU and first normal items). COXListBoxEx allows you to specify the 
maximum number of MRU items. And also it supports the functionality of 
saving and restoring MRU items to/from the registry.


	Usage

In order to use COXListBoxEx object in your application you have to create it
using standard CListBox::Create function or subclass the existing control
(e.g. using DDX/DDV technology). When creating control explicitely or defining 
it in dialog template you have to make sure that the following requirements 
are met:

  LBS_OWNERDRAWVARIABLE must be specified
  if control is supposed to display text then LBS_HASSTRINGS style must be specified

After control was successfully created or subclassed you might want to populate it 
with items. The process of adding new items is exactly the same as for the standard
CListBox control. We extended standard declaration of some functions in order to 
accomodate new functionality. Use following function in order to add or insert
new item:

	int AddString(LPCTSTR lpszItem, int nMask, OXLISTBOXITEM* pLBI);
	int AddString(LPCTSTR lpszItem, int nMask=0, CFont* pFont=NULL, 
		COLORREF clrText=::GetSysColor(COLOR_WINDOWTEXT),
		COLORREF clrBackground=::GetSysColor(COLOR_WINDOW),
		CString sTooltipText=_T(""), int nImageIndex=-1, int nIndent=0, 
		COLORREF clrTextHighlight=::GetSysColor(COLOR_HIGHLIGHTTEXT),
		COLORREF clrBackgroundHighlight=::GetSysColor(COLOR_HIGHLIGHT));
	int InsertString(int nIndex, LPCTSTR lpszItem, int nMask, OXLISTBOXITEM* pLBI);
	int InsertString(int nIndex, LPCTSTR lpszItem, int nMask=0, 
		CFont* pFont=NULL, COLORREF clrText=::GetSysColor(COLOR_WINDOWTEXT),
		COLORREF clrBackground=::GetSysColor(COLOR_WINDOW),
		CString sTooltipText=_T(""), int nImageIndex=-1, int nIndent=0, 
		COLORREF clrTextHighlight=::GetSysColor(COLOR_HIGHLIGHTTEXT),
		COLORREF clrBackgroundHighlight=::GetSysColor(COLOR_HIGHLIGHT));

As you can see you can specify all new item settings at the moment this item is added.


You can change any item property at runtime using the following functions:

	BOOL SetItemInfo(int nIndex, int nMask, OXLISTBOXITEM* pLBI);
	virtual BOOL SetItemInfo(int nIndex, int nMask=0, CFont* pFont=NULL, 
		COLORREF clrText=::GetSysColor(COLOR_WINDOWTEXT),
		COLORREF clrBackground=::GetSysColor(COLOR_WINDOW),
		CString sTooltipText=_T(""), int nImageIndex=-1, int nIndent=0, 
		COLORREF clrTextHighlight=::GetSysColor(COLOR_HIGHLIGHTTEXT),
		COLORREF clrBackgroundHighlight=::GetSysColor(COLOR_HIGHLIGHT));
	BOOL SetItemFont(int nIndex, CFont* pFont);
	BOOL SetItemTextColor(int nIndex, COLORREF clrText);
	BOOL SetItemTextColorHighlight(int nIndex, COLORREF clrTextHighlight);
	BOOL SetItemBkColor(int nIndex, COLORREF clrBackground);
	BOOL SetItemBkColorHighlight(int nIndex, COLORREF clrBackgroundHighlight);
	BOOL SetItemImageIndex(int nIndex, int nImageIndex);
	BOOL SetItemIndent(int nIndex, int nIndent);
	BOOL SetItemTooltipText(int nIndex, CString sTooltipText);



You can retrieve any item property at runtime using the following functions:

	OXLISTBOXITEM* GetItemInfo(int nIndex) const;
	CFont* GetItemFont(int nIndex) const;
	COLORREF GetItemTextColor(int nIndex) const;
	COLORREF GetItemBkColor(int nIndex) const;
	int GetItemImageIndex(int nIndex) const;
	int GetItemIndent(int nIndex) const;
	CString GetItemTooltipText(int nIndex) const;


You can specify a background color of the control using:
	void SetBkColor(COLORREF clrBackground);

And tooltip for the control can be set using:
	BOOL SetItemTooltipText(int nIndex, CString sTooltipText);


We already mentioned MRU items paradigm that is supported by 
COXListBoxEx class. Here is the list of functions that can be used in 
order to specify maximum number of MRU items, to move/remove items 
to/from the MRU section and to save and restore the state of MRU items:

	int MRUAdd(int nItemIndex) ;
	int MRUMove(int nMRUIndex, int nMRUIndexNew); 
	virtual int MRUInsert(int nItemIndex, int nMRUIndex, BOOL bForceToInsert=TRUE);
	virtual int MRUDelete(int nMRUIndex);
	void SetSaveRestoreMRUState(BOOL bSaveRestore);
	BOOL GetSaveRestoreMRUState() const;



Even greater level of customization can be achieved through deriving your own
class from COXListBoxEx. This class has a big set of protected virtual functions 
that are reponsible for implementing class features. By overriding those functions 
you can provide your own logic in the control. Refer to class reference for 
details.



The sample that demonstrates COXListBoxEx class is called ExtendedListBox and 
can be found in the .\Samples\gui\ExtendedListBox subdirectory of your 
Ultimate Toolbox directory. 



Dependency:
	#include "OXListBoxEx.h"


Source code files:
	"OXListBoxEx.cpp"		

Resource file:
	#include "OXListBoxEx.rc"

*/



#if !defined(_OXLISTBOXEX_H__)
#define _OXLISTBOXEX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "OXDllExt.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif //__AFXTEMPL_H__

#ifdef SelectFont
#undef SelectFont
#endif

#define OXLBI_FONT					0x00000001
#define OXLBI_TEXTCOLOR				0x00000002
#define OXLBI_BKCOLOR				0x00000004
#define OXLBI_TOOLTIPTEXT			0x00000008
#define OXLBI_IMAGEINDEX			0x00000010
#define OXLBI_INDENT				0x00000020
#define OXLBI_TEXTCOLORHIGHLIGHT	0x00000040
#define OXLBI_BKCOLORHIGHLIGHT		0x00000080
#define OXLBI_ALL					OXLBI_FONT|OXLBI_TEXTCOLOR|OXLBI_BKCOLOR|OXLBI_TOOLTIPTEXT|OXLBI_IMAGEINDEX|OXLBI_INDENT|OXLBI_TEXTCOLORHIGHLIGHT|OXLBI_BKCOLORHIGHLIGHT


#define OXLB_IMAGEOFFSET_X			2
#define OXLB_IMAGEOFFSET_Y			2
#define OXLB_TEXTOFFSET_X			2
#define OXLB_TEXTOFFSET_Y			2

#define OXLB_SEPARATOR_HEIGHT		4

#define ID_OXLB_TOOLTIP				0xfff0

typedef struct _tagOXLISTBOXITEM
{
	CFont* m_pFont;
	COLORREF m_clrText;
	COLORREF m_clrTextHighlight;
	COLORREF m_clrBackground;
	COLORREF m_clrBackgroundHighlight;
	CString m_sTooltipText;
	int m_nImageIndex;
	int m_nIndent;

	CRect m_rectItem;
	CRect m_rectImage;
	CRect m_rectText;

	int m_nMRUIndex;


public:
	_tagOXLISTBOXITEM(CFont* pFont=NULL, 
		COLORREF clrText=GetSysColor(COLOR_WINDOWTEXT),
		COLORREF clrBackground=GetSysColor(COLOR_WINDOW),
		CString sTooltipText=_T(""), int nImageIndex=-1, int nIndent=0,
		COLORREF clrTextHighlight=GetSysColor(COLOR_HIGHLIGHTTEXT),
		COLORREF clrBackgroundHighlight=GetSysColor(COLOR_HIGHLIGHT)) :	
			m_pFont(pFont),
			m_clrText(clrText),
			m_clrTextHighlight(clrTextHighlight),
			m_clrBackground(clrBackground),
			m_clrBackgroundHighlight(clrBackgroundHighlight),
			m_sTooltipText(sTooltipText),
			m_nImageIndex(nImageIndex),
			m_nIndent(nIndent),
			m_rectItem(0,0,0,0),
			m_rectImage(0,0,0,0),
			m_rectText(0,0,0,0),
			m_nMRUIndex(-1)
	{
	};


	_tagOXLISTBOXITEM(const _tagOXLISTBOXITEM& lbi) :
		m_pFont(lbi.m_pFont),
		m_clrText(lbi.m_clrText),
		m_clrTextHighlight(lbi.m_clrTextHighlight),
		m_clrBackground(lbi.m_clrBackground),
		m_clrBackgroundHighlight(lbi.m_clrBackgroundHighlight),
		m_sTooltipText(lbi.m_sTooltipText),
		m_nImageIndex(lbi.m_nImageIndex),
		m_nIndent(lbi.m_nIndent),
		m_rectItem(lbi.m_rectItem),
		m_rectImage(lbi.m_rectImage),
		m_rectText(lbi.m_rectText),
		m_nMRUIndex(lbi.m_nMRUIndex)
	{
	};


	_tagOXLISTBOXITEM& operator=(const _tagOXLISTBOXITEM& lbi)
	{
		if(this==&lbi)
			return *this;

		m_pFont=lbi.m_pFont;
		m_clrText=lbi.m_clrText;
		m_clrTextHighlight=lbi.m_clrTextHighlight;
		m_clrBackground=lbi.m_clrBackground;
		m_clrBackgroundHighlight=lbi.m_clrBackgroundHighlight;
		m_sTooltipText=lbi.m_sTooltipText;
		m_nImageIndex=lbi.m_nImageIndex;
		m_nIndent=lbi.m_nIndent;

		m_rectItem=lbi.m_rectItem;
		m_rectImage=lbi.m_rectImage;
		m_rectText=lbi.m_rectText;

		m_nMRUIndex=lbi.m_nMRUIndex;

		return *this;
	}

} OXLISTBOXITEM;

/////////////////////////////////////////////////////////////////////////////
// COXListBoxEx window

class COXListBoxEx : public CListBox
{
	DECLARE_DYNAMIC(COXListBoxEx);
// Construction
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect:	Constructs the object
	COXListBoxEx();

// Attributes
public:

protected:
	// internal map for storing all information about items
	CMap<int,int,OXLISTBOXITEM*,OXLISTBOXITEM*> m_mapItems;

	// internal tooltip control, displays tooltips for the control and any item
	CToolTipCtrl m_tooltip;

	// an image list associated with list box. Every item in the list box might
	// have an image assigned to it as index in this image list
	CImageList* m_pImageList;

	// the maximum allowed number before separator (in MRU (most recent used) section)
	int m_nMaxItemsBeforeSeparator;
	// the index of the last item before separator
	int m_nLastItemBeforeSeparator;

	// internal variables used to redraw the control in the most efficient way
	//
	// index of the current widest item
	int m_nWidestItemIndex;
	// index of the last top item
	int m_nLastTopIndex;

	// background color
	COLORREF m_clrBackground;
	
	// flag that specifies the way a selected item is being drawn. If it is set to 
	// TRUE then only the area that covered by item's text will be highlighted when 
	// an item is selected
	BOOL m_bHighlightOnlyText;

	// control's tooltip text
	CString m_sTooltipText;

	// flag that specifies that MRU (most recent used) items should be saved/loaded
	// to/from registry
	BOOL m_bSaveRestoreMRUState;

	// set to TRUE if WindowsNT running
	// some draw routines behave differently under Windows95 and WindowsNT
	static BOOL m_bWindowsNTRunning;

// Operations
public:
	// --- In  :
	// --- Out : 
	// --- Returns: Control's tooltip text
	// --- Effect:	Retrieves tooltip text assigned to the control
	virtual CString GetTooltipText() const { return m_sTooltipText; }

	// --- In  :	sTooltipText	-	new tooltip text
	// --- Out : 
	// --- Returns: 
	// --- Effect:	Sets control's tooltip text
	inline void SetTooltipText(CString sTooltipText) { m_sTooltipText=sTooltipText; }


	// --- In  :
	// --- Out : 
	// --- Returns: Control's background color
	// --- Effect:	Retrieves the background color of the control
	virtual COLORREF GetBkColor() const { return m_clrBackground; }

	// --- In  :	clrBackground	-	new background color
	// --- Out : 
	// --- Returns: 
	// --- Effect:	Sets control's background color
	inline void SetBkColor(COLORREF clrBackground) 
	{ 
		m_clrBackground=clrBackground; 
		if(::IsWindow(GetSafeHwnd()))
			RedrawWindow();
	}


	// --- In  :
	// --- Out : 
	// --- Returns: Flag that specifies the way a selected item is being highlighted.
	//				TRUE means that only the area covered by text will be highlighted,
	//				otherwise the whole item will be highlighted (default 
	//				functionality)
	// --- Effect:	Retrieves the flag that specifies the way a selected item is 
	//				highlighted
	virtual BOOL GetHighlightOnlyText() const { return m_bHighlightOnlyText; }

	// --- In  :	bHighlightOnlyText	-	flag that specifies the way a selected 
	//										item is being highlighted. TRUE means 
	//										that only the area covered by text will 
	//										be highlighted,	otherwise the whole item 
	//										will be highlighted (default functionality)
	// --- Out : 
	// --- Returns: 
	// --- Effect:	Sets the flag that specifies the way a selected item is 
	//				highlighted
	inline void SetHighlightOnlyText(BOOL bHighlightOnlyText) 
	{ 
		m_bHighlightOnlyText=bHighlightOnlyText; 
		UpdateTooltipTools();
		if(::IsWindow(GetSafeHwnd()))
			RedrawWindow();
	}

	// --- In  :
	// --- Out : 
	// --- Returns: Pointer to the internal tooltip control that is used to display
	//				tooltips for the control and items
	// --- Effect:	Retrieves a pointer to the internal tooltip control
	virtual CToolTipCtrl* GetToolTipCtrl() { return &m_tooltip; }


	// --- In  :
	// --- Out : 
	// --- Returns: Pointer to the current image list object associated with the 
	//				control. Images from this image list can be assigned to any item
	//				in the control using SetItemImageIndex() function.
	// --- Effect:	Retrieves a pointer to the associated image list object
	virtual CImageList* GetImageList() const { return m_pImageList; }

	// --- In  :	pImageList	-	pointer to new image list object that will be
	//								associated with the control. Images from this 
	//								image list can be assigned to any item in the 
	//								control using SetItemImageIndex() function. Note
	//								that we save only the pointer to specified image
	//								list, which means a programmer have to guarantee 
	//								that the lifetime of this image list object more
	//								or equal to the control's object lifetime.
	// --- Out : 
	// --- Returns: 
	// --- Effect:	Associates image list object with the control
	inline void SetImageList(CImageList* pImageList) 
	{ 
		m_pImageList=pImageList; 
		if(::IsWindow(GetSafeHwnd()))
			RecalcItemsExtent();
	}


	// --- In  :
	// --- Out : 
	// --- Returns: The maximum number of items that can be moved to the MRU (most 
	//				recent used) section. MRU section is located at the top of the 
	//				control and visually separated from the rest items by a separator. 
	//				If -1 is returned then any number of items can be moved to 
	//				MRU section.
	// --- Effect:	Retrieves the maximum number of items that can be moved to the 
	//				MRU section
	inline int GetMaxItemsBeforeSeparator() const 
	{ 
		return m_nMaxItemsBeforeSeparator; 
	}

	// --- In  :	nMaxItemsBeforeSeparator	-	The maximum number of items that 
	//												can be moved to the MRU (most 
	//												recent used) section. MRU section 
	//												is located at the top of the 
	//												control and visually separated 
	//												from the rest items by a separator. 
	//												If -1 is specified then any number 
	//												of items can be moved to 
	//												MRU section
	// --- Out : 
	// --- Returns: 
	// --- Effect:	Sets the maximum number of items that can be moved to the 
	//				MRU section. When new item is being added to the MRU section and 
	//				the maximum number of items has been already reached then the last 
	//				item before separator will be removed from MRU section
	void SetMaxItemsBeforeSeparator(int nMaxItemsBeforeSeparator);


	// --- In  :
	// --- Out : 
	// --- Returns: The index of the last item in the MRU (most recent used) section. 
	// --- Effect:	Retrieves the index of the last item in the MRU section
	inline int GetLastItemBeforeSeparator() const 
	{ 
		return m_nLastItemBeforeSeparator; 
	}


	// --- In  :	nIndex	-	index of the item which extended info will be 
	//							retrieved
	// --- Out : 
	// --- Returns: A pointer to OXLISTBOXITEM structure that provides an extended
	//				information for list box item. OXLISTBOXITEM is declared as 
	//				follows:
	//
	//				typedef struct _tagOXLISTBOXITEM
	//				{
	//					CFont* m_pFont;
	//					COLORREF m_clrText;
	//					COLORREF m_clrTextHighlight;
	//					COLORREF m_clrBackground;
	//					COLORREF m_clrBackgroundHighlight;
	//					CString m_sTooltipText;
	//					int m_nImageIndex;
	//					int m_nIndent;
	//
	//					CRect m_rectItem;
	//					CRect m_rectImage;
	//					CRect m_rectText;
	//
	//					int m_nMRUIndex;
	//				} OXLISTBOXITEM;
	//
	//				where:
	//
	//				m_pFont					-	pointer	to CFont object used to draw 
	//											the item's text			
	//				m_clrText				-	item's text color
	//				m_clrTextHighlight		-	item's text color in selected state
	//				m_clrBackground			-	item's background color
	//				m_clrBackgroundHighlight-	item's background color in selected 
	//											state
	//				m_sTooltipText			-	item's tooltip text
	//				m_nIndent				-	item's indent from the left side
	//				m_rectItem				-	item's rectangle
	//				m_rectImage				-	item's image rectangle
	//				m_rectText				-	item's text rectangle
	//				m_nMRUIndex				-	index of the item in MRU (most recent 
	//											used) section, if -1 then item is not 
	//											in MRU section
	//
	// --- Effect:	Retrieves extended info about the specified list box item
	OXLISTBOXITEM* GetItemInfo(int nIndex) const;

	// --- In  :	nIndex		-	index of the item which extended info will be set
	//				nMask		-	any combination of the following constants:
	//
	//						OXLBI_FONT				-	font info will be set
	//						OXLBI_TEXTCOLOR			-	text color will be set
	//						OXLBI_TEXTCOLORHIGHLIGHT-	text color in selected state 
	//													will be set
	//						OXLBI_BKCOLOR			-	background color in selected 
	//													state will be set
	//						OXLBI_BKCOLORHIGHLIGHT	-	background color will be set
	//						OXLBI_TOOLTIPTEXT		-	tooltip text will be set
	//						OXLBI_IMAGEINDEX		-	image index will be set
	//						OXLBI_INDENT			-	indent will be set
	//								
	//								You can the following constant in order to set all
	//								extended info: OXLBI_ALL
	//
	//				pLBI		-	a pointer to OXLISTBOXITEM structure that provides 
	//								an extended	information for list box item. Refer
	//								to the documentation on GetItemInfo() function
	//								for details on the OXLISTBOXITEM structure
	//				pFont		-	a pointer to CFont object to be used to draw the 
	//								specified item
	//				clrText		-	color to be used to draw the specified item text
	//				clrTextHighlight-	color to be used to draw the specified item 
	//									text in selected state
	//				clrBackground-	color to be used to draw the specified item 
	//								background
	//				clrBackgroundHighlight-	color to be used to draw the specified 
	//								item background in selected state
	//				sTooltipText-	tooltip text to be displayed when mouse is moved
	//								in the item's rectangle
	//				nImageIndex	-	index of the image in the associated image list. 
	//								If -1 is specified then there is no image to be 
	//								displayed for the item
	//				nIndent		-	the number of pixels from the left side of the 
	//								control's client area where the item is drawn
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Sets extended info for the specified list box item
	BOOL SetItemInfo(int nIndex, int nMask, OXLISTBOXITEM* pLBI);
	virtual BOOL SetItemInfo(int nIndex, int nMask=0, CFont* pFont=NULL, 
		COLORREF clrText=::GetSysColor(COLOR_WINDOWTEXT),
		COLORREF clrBackground=::GetSysColor(COLOR_WINDOW),
		CString sTooltipText=_T(""), int nImageIndex=-1, int nIndent=0,
		COLORREF clrTextHighlight=::GetSysColor(COLOR_HIGHLIGHTTEXT),
		COLORREF clrBackgroundHighlight=::GetSysColor(COLOR_HIGHLIGHT));


	// --- In  :	nIndex	-	index of the item for which font info will be 
	//							retrieved
	// --- Out : 
	// --- Returns: A pointer to CFont object that is used to draw the specified item
	// --- Effect:	Retrieves a pointer to the CFont object that is used to draw the 
	//				specified item
	CFont* GetItemFont(int nIndex) const;

	// --- In  :	nIndex	-	index of the item for which font info will be 
	//							retrieved
	//				pFont	-	pointer to the CFont object to be used to 
	//							draw the specified item
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Sets the CFont object to be used to draw the specified item
	BOOL SetItemFont(int nIndex, CFont* pFont);


	// --- In  :	nIndex	-	index of the item for which text color info will be 
	//							retrieved
	// --- Out : 
	// --- Returns: Color that is used to draw the specified item text
	// --- Effect:	Retrieves the color that is used to draw the specified item text
	COLORREF GetItemTextColor(int nIndex) const;

	// --- In  :	nIndex	-	index of the item for which text color info will be 
	//							retrieved
	//				clrText	-	color to be used to draw the specified item text
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Sets the color to be used to draw the specified item text
	BOOL SetItemTextColor(int nIndex, COLORREF clrText);


	// --- In  :	nIndex	-	index of the item for which text color in selected 
	//							state info will be retrieved
	// --- Out : 
	// --- Returns: Color that is used to draw the specified item text in selected 
	//				state
	// --- Effect:	Retrieves the color that is used to draw the specified item text 
	//				in selected state
	COLORREF GetItemTextColorHighlight(int nIndex) const;

	// --- In  :	nIndex				-	index of the item for which text color 
	//										in selected state info will be retrieved
	//				clrTextHighlight	-	color to be used to draw the specified 
	//										item text in selected state
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Sets the color to be used to draw the specified item text 
	//				in selected state
	BOOL SetItemTextColorHighlight(int nIndex, COLORREF clrTextHighlight);

	
	// --- In  :	nIndex	-	index of the item for which background color info 
	//							will be retrieved
	// --- Out : 
	// --- Returns: Color that is used to draw the specified item background
	// --- Effect:	Retrieves the color that is used to draw the specified item 
	//				background
	COLORREF GetItemBkColor(int nIndex) const;

	// --- In  :	nIndex			-	index of the item for which background color 
	//									info will be retrieved
	//				clrBackground	-	color to be used to draw the specified item 
	//									background
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Sets the color to be used to draw the specified item background
	BOOL SetItemBkColor(int nIndex, COLORREF clrBackground);

	
	// --- In  :	nIndex	-	index of the item for which background color 
	//							in selected state info will be retrieved
	// --- Out : 
	// --- Returns: Color that is used to draw the specified item background
	//				in selected state
	// --- Effect:	Retrieves the color that is used to draw the specified item 
	//				background in selected state
	COLORREF GetItemBkColorHighlight(int nIndex) const;

	// --- In  :	nIndex					-	index of the item for which background 
	//											color in selected state info will be 
	//											retrieved
	//				clrBackgroundHighlight	-	color to be used to draw the 
	//											specified item background in selected 
	//											state
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Sets the color to be used to draw the specified item background
	//				in selected state
	BOOL SetItemBkColorHighlight(int nIndex, COLORREF clrBackgroundHighlight);

	
	// --- In  :	nIndex	-	index of the item for which image info 
	//							will be retrieved
	// --- Out : 
	// --- Returns: Index of the image in the associated image list. If -1 is
	//				returned then there is no image to be displayed for the item
	// --- Effect:	Retrieves the index of the image in the associated image list.
	int GetItemImageIndex(int nIndex) const;

	// --- In  :	nIndex		-	index of the item for which font info will be 
	//								retrieved
	//				nImageIndex	-	index of the image in the associated image list. 
	//								If -1 is returned then there is no image to be 
	//								displayed for the item
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Assigns index of the image in the associated image list to the 
	//				specified item
	BOOL SetItemImageIndex(int nIndex, int nImageIndex);


	// --- In  :	nIndex	-	index of the item for which indent info 
	//							will be retrieved
	// --- Out : 
	// --- Returns: The number of pixels from the left side of the control's client
	//				area where the item is drawn
	// --- Effect:	Retrieves the item's indent in pixels from the left side of the
	//				control's client area
	int GetItemIndent(int nIndex) const;

	// --- In  :	nIndex	-	index of the item for which font info will be 
	//							retrieved
	//				nIndent	-	the number of pixels from the left side of the 
	//							control's client area where the item is drawn
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Sets the indent from the left side for the specified item
	BOOL SetItemIndent(int nIndex, int nIndent);


	// --- In  :	nIndex	-	index of the item for which tooltip text info 
	//							will be retrieved
	// --- Out : 
	// --- Returns: The tooltip text that will be displayed when mouse is moved
	//				in the item's rectangle
	// --- Effect:	Retrieves the item's tooltip text
	CString GetItemTooltipText(int nIndex) const;

	// --- In  :	nIndex			-	index of the item for which font info will be 
	//									retrieved
	//				sTooltipText	-	the tooltip text to be displayed when mouse 
	//									is moved in the item's rectangle
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Sets the tooltip text for the specified item
	BOOL SetItemTooltipText(int nIndex, CString sTooltipText);


	// --- In  :	lpszItem	-	item's text
	//				nMask		-	any combination of the following constants:
	//
	//								OXLBI_FONT			-	font info will be set
	//								OXLBI_TEXTCOLOR		-	text color will be set
	//								OXLBI_BKCOLOR		-	background color will be set
	//								OXLBI_TOOLTIPTEXT	-	tooltip text will be set
	//								OXLBI_IMAGEINDEX	-	image index will be set
	//								OXLBI_INDENT		-	indent will be set
	//								
	//								You can the following constant in order to set all
	//								extended info: OXLBI_ALL
	//
	//				pLBI		-	a pointer to OXLISTBOXITEM structure that provides 
	//								an extended	information for list box item. Refer
	//								to the documentation on GetItemInfo() function
	//								for details on the OXLISTBOXITEM structure
	//				pFont		-	a pointer to CFont object to be used to draw the 
	//								specified item
	//				clrText		-	color to be used to draw the specified item text
	//				clrTextHighlight-	color to be used to draw the specified item 
	//									text in selected state
	//				clrBackground-	color to be used to draw the specified item 
	//								background
	//				clrBackgroundHighlight-	color to be used to draw the specified 
	//								item background in selected state
	//				sTooltipText-	tooltip text to be displayed when mouse is moved
	//								in the item's rectangle
	//				nImageIndex	-	index of the image in the associated image list. 
	//								If -1 is specified then there is no image to be 
	//								displayed for the item
	//				nIndent		-	the number of pixels from the left side of the 
	//								control's client area where the item is drawn
	// --- Out : 
	// --- Returns: The zero-based index to the string in the list box. The return 
	//				value is LB_ERR if an error occurs; the return value is 
	//				LB_ERRSPACE if insufficient space is available to store the 
	//				new string
	// --- Effect:	Adds a string to a list box
	int AddString(LPCTSTR lpszItem, int nMask, OXLISTBOXITEM* pLBI);
	int AddString(LPCTSTR lpszItem, int nMask=0, CFont* pFont=NULL, 
		COLORREF clrText=::GetSysColor(COLOR_WINDOWTEXT),
		COLORREF clrBackground=::GetSysColor(COLOR_WINDOW),
		CString sTooltipText=_T(""), int nImageIndex=-1, int nIndent=0,
		COLORREF clrTextHighlight=::GetSysColor(COLOR_HIGHLIGHTTEXT),
		COLORREF clrBackgroundHighlight=::GetSysColor(COLOR_HIGHLIGHT));


	// --- In  :	nIndex		-	zero-based index of the position to insert the 
	//								string. If this parameter is -1, the string is 
	//								added to the end of the list
	//				lpszItem	-	item's text
	//				nMask		-	any combination of the following constants:
	//
	//								OXLBI_FONT			-	font info will be set
	//								OXLBI_TEXTCOLOR		-	text color will be set
	//								OXLBI_BKCOLOR		-	background color will be set
	//								OXLBI_TOOLTIPTEXT	-	tooltip text will be set
	//								OXLBI_IMAGEINDEX	-	image index will be set
	//								OXLBI_INDENT		-	indent will be set
	//								
	//								You can the following constant in order to set all
	//								extended info: OXLBI_ALL
	//
	//				pLBI		-	a pointer to OXLISTBOXITEM structure that provides 
	//								an extended	information for list box item. Refer
	//								to the documentation on GetItemInfo() function
	//								for details on the OXLISTBOXITEM structure
	//				pFont		-	a pointer to CFont object to be used to draw the 
	//								specified item
	//				clrText		-	color to be used to draw the specified item text
	//				clrTextHighlight-	color to be used to draw the specified item 
	//									text in selected state
	//				clrBackground-	color to be used to draw the specified item 
	//								background
	//				clrBackgroundHighlight-	color to be used to draw the specified 
	//								item background in selected state
	//				sTooltipText-	tooltip text to be displayed when mouse is moved
	//								in the item's rectangle
	//				nImageIndex	-	index of the image in the associated image list. 
	//								If -1 is specified then there is no image to be 
	//								displayed for the item
	//				nIndent		-	the number of pixels from the left side of the 
	//								control's client area where the item is drawn
	// --- Out : 
	// --- Returns: The zero-based index to the string in the list box. The return 
	//				value is LB_ERR if an error occurs; the return value is 
	//				LB_ERRSPACE if insufficient space is available to store the 
	//				new string
	// --- Effect:	Adds a string to a list box
	int InsertString(int nIndex, LPCTSTR lpszItem, int nMask, OXLISTBOXITEM* pLBI);
	int InsertString(int nIndex, LPCTSTR lpszItem, int nMask=0, 
		CFont* pFont=NULL, COLORREF clrText=::GetSysColor(COLOR_WINDOWTEXT),
		COLORREF clrBackground=::GetSysColor(COLOR_WINDOW),
		CString sTooltipText=_T(""), int nImageIndex=-1, int nIndent=0,
		COLORREF clrTextHighlight=::GetSysColor(COLOR_HIGHLIGHTTEXT),
		COLORREF clrBackgroundHighlight=::GetSysColor(COLOR_HIGHLIGHT));


	// --- In  :	nIndex		-	zero-based index of the item to be deleted
	// --- Out : 
	// --- Returns: A count of the strings remaining in the list. The return 
	//				value is LB_ERR if nIndex specifies an index greater than 
	//				the number of items in the list
	// --- Effect:	Deletes an item in a list box
	int DeleteString(UINT nIndex);


	// --- In  :	nItemIndex	-	index of the item which will be moved to 
	//								MRU (most recent used) section
	// --- Out : 
	// --- Returns: The new zero-based index to the string in the list box. The return 
	//				value is LB_ERR if an error occurs or maximum number of items
	//				in MRU section has already been reached
	// --- Effect:	Moves specified item to the bottom of MRU section
	inline int MRUAdd(int nItemIndex) 
	{ 
		return MRUInsert(nItemIndex,GetLastItemBeforeSeparator()+1,FALSE); 
	}

	// --- In  :	nItemIndex	-	index of the MRU item (most recent used) which 
	//								will be moved to new position in MRU section
	//				nMRUIndexNew-	new index of the position in MRU section where 
	//								specified item must be moved to
	// --- Out : 
	// --- Returns: The new zero-based index to the string in the list box. The return 
	//				value is LB_ERR if an error occurs
	// --- Effect:	Moves specified MRU item to new position in MRU section
	inline int MRUMove(int nMRUIndex, int nMRUIndexNew) 
	{
		return MRUInsert(nMRUIndex,nMRUIndexNew,FALSE);
	}

	// --- In  :	nItemIndex		-	index of the item which will be moved to new 
	//									position in MRU (most recent used) section
	//				nMRUIndex		-	the position in MRU section where specified 
	//									item must be moved to
	//				bForceToInsert	-	flag that handle the situation when the 
	//									maximum number of MRU items has been reached.
	//									In this case if this class is set to TRUE the 
	//									specified item will be moved to the specified
	//									MRU position and the last MRU item will be 
	//									moved out of the MRU section, otherwise the
	//									function will fail and return value will be 
	//									set to LB_ERR
	// --- Out : 
	// --- Returns: The new zero-based index to the string in the list box. The return 
	//				value is LB_ERR if an error occurs
	// --- Effect:	Moves specified item to new position in MRU section
	virtual int MRUInsert(int nItemIndex, int nMRUIndex, BOOL bForceToInsert=TRUE);

	// --- In  :	nMRUIndex		-	the position in MRU section from which an
	//									item must be removed
	// --- Out : 
	// --- Returns: The new zero-based index to the string in the list box. The return 
	//				value is LB_ERR if an error occurs
	// --- Effect:	Removes specified item from MRU section. Item is not being removed
	//				from the list box but added to the bottom of the list
	virtual int MRUDelete(int nMRUIndex);


	// --- In  :	bSaveRestore	-	if TRUE then the order of items in MRU 
	//									section will be saved in the registry
	//									before the control is destroyed. When
	//									control is being populated a programmer
	//									can check for this flag and restore the 
	//									saved MRU state by calling RestoreMRUState()
	//									function. Note that RestoreMRUState() is
	//									protected function, which means that kind of 
	//									functionality can be provided only in a 
	//									derived class. 
	// --- Out : 
	// --- Returns: 
	// --- Effect:	Sets the flag that specifies whether MRU state will be saved
	//				in the registry or not
	inline void SetSaveRestoreMRUState(BOOL bSaveRestore) 
	{ 
		m_bSaveRestoreMRUState=bSaveRestore; 
	}
	
	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if the order of items in MRU section will be saved in the 
	//				registry before the control is destroyed. When control is being 
	//				populated a programmer can check for this flag and restore the 	
	//				saved MRU state by calling RestoreMRUState() function. Note that 
	//				RestoreMRUState() is protected function, which means that kinf of 
	//				functionality can be provided only in a derived class
	// --- Effect:	Retrieves the flag that specifies whether MRU state will be saved
	//				in the registry or not
	inline BOOL GetSaveRestoreMRUState() const { return m_bSaveRestoreMRUState; }

	
	// --- In  :	
	// --- Out : 
	// --- Returns: 
	// --- Effect:	Recalculates the coordinates of the items displayed 
	//				in the control. Called every time an item's state
	//				that influence the size of the item has been changed
	void RecalcLayout();

	
	// --- In  :	
	// --- Out : 
	// --- Returns: 
	// --- Effect:	Removes all items from a list box
	void ResetContent();
	


protected:
	// recalculates the positioning of the specified item or all items at once
	void RecalcItemsExtent(int nItemIndex=-1);
	
	// calculates rectangles for text and image
	//

	// --- In  :	nIndex			-	zero-based index of the item 
	// --- Out :	rectItem		-	item's rectangle
	//				rectItemVisible	-	visible portion of item's rectangle
	//				rectImage		-	rectangle where image is drawn
	//				rectText		-	rectangle where text is drawn
	// --- Returns: 
	// --- Effect:	Calculates the rectangles  where item should draw its parts.
	//				Advanced overridable, can be overridden in a derived class
	//				in order to position items in different manner
	virtual void CalcItemRects(int nIndex, CRect& rectItem,
		CRect& rectItemVisible, CRect& rectImage, CRect& rectText);

	// --- In  :	nIndex			-	zero-based index of the item 
	// --- Out :	nWidthImage		-	width of associated image
	//				nHeightImage	-	height of associated image
	// --- Returns: 
	// --- Effect:	Calculates the size of the image associated with the specified 
	//				item. Advanced overridable, can be overridden in a derived class
	//				in order to support images of different size associated with
	//				different items. In COXListBoxEx the image size is the same 
	//				for all items
	virtual void CalcImageSize(int nIndex, int& nWidthImage, int& nHeightImage);

	// --- In  :	nIndex			-	zero-based index of the item 
	// --- Out :	nWidthText		-	width of text
	//				nHeightText		-	height of text
	// --- Returns: 
	// --- Effect:	Calculates the size of the text drawn using item's font. Advanced 
	//				overridable, can be overridden in a derived class in order to 
	//				support non standard text drawing routine
	virtual void CalcTextSize(int nIndex, int& nWidthText, int& nHeightText);

	//
	//////////////////////////////////////////////


	// --- In  :	lpszItem	-	item's text
	//				nMask		-	any combination of the following constants:
	//
	//								OXLBI_FONT			-	font info will be set
	//								OXLBI_TEXTCOLOR		-	text color will be set
	//								OXLBI_BKCOLOR		-	background color will be set
	//								OXLBI_TOOLTIPTEXT	-	tooltip text will be set
	//								OXLBI_IMAGEINDEX	-	image index will be set
	//								OXLBI_INDENT		-	indent will be set
	//								
	//								You can the following constant in order to set all
	//								extended info: OXLBI_ALL
	//
	//				pLBI		-	a pointer to OXLISTBOXITEM structure that provides 
	//								an extended	information for list box item. Refer
	//								to the documentation on GetItemInfo() function
	//								for details on the OXLISTBOXITEM structure
	// --- Out : 
	// --- Returns: The zero-based index to the string in the list box. The return 
	//				value is LB_ERR if an error occurs; the return value is 
	//				LB_ERRSPACE if insufficient space is available to store the 
	//				new string
	// --- Effect:	Adds a string to the list box as a result of removing it from 
	//				MRU (most recent used) section. The default implementation 
	//				just add the item to the bottom of the list. Advanced 
	//				overridable, can be overridden in a derived class in order to 
	//				support non standard sorting 
	virtual int AddFormerMRUString(LPCTSTR lpszItem, int nMask, OXLISTBOXITEM* pLBI) 
	{
		return AddString(lpszItem,nMask,pLBI);
	}


	// --- In  :	lpszSectionName	-	name of the hive in the registry where
	//									MRU (most recent used) items state 
	//									will be saved
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Saves the MRU items state in registry. Advanced 
	//				overridable, can be overridden in a derived class in order to 
	//				support saving additional info about MRU items. This function
	//				uses the key automatically assigned to your application when 
	//				you call CWinApp::SetRegistryKey() function. You may call it
	//				in your CWinApp derived class InitInstance() function. 
	//				If SetRegistryKey() is not called then this function will fail
	virtual BOOL SaveMRUState(LPCTSTR lpszSectionName=_T("OXLISTBOXEX_MRU"));

	// --- In  :	lpszSectionName	-	name of the hive in the registry where
	//									MRU (most recent used) items state 
	//									were saved
	// --- Out : 
	// --- Returns: TRUE if succeed, otherwise FALSE
	// --- Effect:	Restores the MRU items state from registry. Advanced 
	//				overridable, can be overridden in a derived class in order to 
	//				support restoring additional info about MRU items. This function
	//				uses the key automatically assigned to your application when 
	//				you call CWinApp::SetRegistryKey() function. You may call it
	//				in your CWinApp derived class InitInstance() function. 
	//				If SetRegistryKey() is not called then this function will fail
	virtual BOOL RestoreMRUState(LPCTSTR lpszSectionName=_T("OXLISTBOXEX_MRU"));


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXListBoxEx)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect:	Destructs the object
	virtual ~COXListBoxEx();

	// helper function to define whether WindowsNT >=4.0 is running
	static BOOL IsWindowsNTRunning();

protected:
	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if control was successfully initialized
	// --- Effect:	Initializes the internal members of the control. Advanced 
	//				overridable, can be overridden in a derived class in order to 
	//				initialize an additional data
	virtual BOOL InitializeListBox();
	

	// --- In  :	pDC			-	device context to draw in
	//				rectSeparator-	rectangle within which the separator 
	//								must be drawn
	//				pLBI		-	a pointer to OXLISTBOXITEM structure that provides 
	//								an extended	information for list box item. Refer
	//								to the documentation on GetItemInfo() function
	//								for details on the OXLISTBOXITEM structure
	// --- Out :	
	// --- Returns:	
	// --- Effect:	Draws a separator between MRU (most recent used) section and the
	//				rest of the items. Advanced overridable, can be overridden in a 
	//				derived class in order to draw a separator in a different way
	virtual void DrawSeparator(CDC* pDC, CRect rectSeparator, OXLISTBOXITEM* pLBI);

	// --- In  :	pDC			-	device context to draw in
	//				rectImage	-	rectangle within which the image must be drawn
	//				nStyle		-	flag specifying the drawing style. Refer to 
	//								the documentation on CImageList::Draw() function
	//								for details on values this argument can take
	//				pLBI		-	a pointer to OXLISTBOXITEM structure that provides 
	//								an extended	information for list box item. Refer
	//								to the documentation on GetItemInfo() function
	//								for details on the OXLISTBOXITEM structure
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Draws an image associated with the item. Advanced overridable, 
	//				can be overridden in a derived class in order to draw 
	//				associated image in a specific way
	virtual void DrawImage(CDC* pDC, CRect& rectImage, UINT nStyle, 
		OXLISTBOXITEM* pLBI);

	// --- In  :	pDC			-	device context to draw in
	//				lpszText	-	text to draw
	//				rectText	-	rectangle within which the text must be drawn
	//				clrText		-	text color
	//				pFont		-	font to draw the text
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Draws the specified text. Advanced overridable, 
	//				can be overridden in a derived class in order to draw 
	//				the specified text in a specific way
	virtual void DrawText(CDC* pDC, LPCTSTR lpszText, CRect& rectText,
		COLORREF clrText, CFont* pFont);

	// updates tooltip rectangles when the size of item(s) change
	void UpdateTooltipTools();

	// Generated message map functions
protected:
	//{{AFX_MSG(COXListBoxEx)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


/*

	OVERVIEW	-	COXFontListBox

COXFontListBox is COXListBoxEx derived class that is designed to automatically
populate the control with installed fonts and display them in alphabetical order. 
Every item in the list box corresponds to some font and the name of the font is 
appeared as item text. All items are drawn using associated fonts that they 
represent. Also image is displayed that specifies whether the font associated 
with an item is TrueType or not. 


	Usage

In order to use COXFontListBox object in your application you have to create it
using standard CListBox::Create function or subclass the existing control
(e.g. using DDX/DDV technology). When creating control explicitely or defining 
it in dialog template you have to make sure that the following requirements 
are met:

  LBS_OWNERDRAWVARIABLE must be specified
  LBS_HASSTRINGS style must be specified
  LBS_SORT style must NOT be specified

After control was successfully created or subclassed it will be automatically 
populated with all fonts defined in screen device context (default device context).
You can specify your own device context (e.g. printer device context) using:

	void SetDC(HDC dc);

While a control is being populated with enumerated fonts the following protected 
virtual function will be called for every font before it is added to the list:

	virtual BOOL FilterFont(OXLBFONTINFO* pLBFI);

Default implementation of this function returns always TRUE. in your own 
implementation you might override it in order to filter the fonts displayed in 
the list box. 

As we mentioned before every item in the control is being drawn using the 
associated font this item represents. We use unified font height for all items 
in the control. You can set/retrieve the font height value using these functions:

	void SetFontHeight(int nHeight);
	virtual int GetFontHeight();


COXFontListBox is COXListBoxEx derived so all COXListBoxEx functionality is 
available for you in this class as well. Refer to COXListBoxEx class overview
and class reference for details on it.

The last thing we would like to note is that altogether with standard way of 
setting current selected item using CListBox::SetCurSel() function COXFontListBox
allows you to use CFont, LOGFONT or name of the font as arguments for the 
following functions:

	int SelectFont(CFont* pFont);
	int SelectFont(LOGFONT* pLF);
	int SelectFont(LPCTSTR lpszFontName);


There is no that really much that you would like to do with this control other
than using the standard functionality. This class is used internally by our
COXFontComboBox class but you migth find it useful to use in your applications 
where you need to provide some customization functionality which is based on 
different available fonts 



The sample that demonstrates COXFontListBox class is called ExtendedListBox and 
can be found in the .\Samples\gui\ExtendedListBox subdirectory of your 
Ultimate Toolbox directory. 



Dependency:
	#include "OXListBoxEx.h"


Source code files:
	"OXListBoxEx.cpp"		

Resource file:
	#include "OXListBoxEx.rc"



*/




#define OXLBF_DEFAULTFONTHEIGHT					20
#define OXLBF_DEFAULTMAXNUMBEFORESEPARATOR		6

typedef struct _tagOXLBFONTINFO
{
	// full font info
	LOGFONT m_lf;
	// font type:	TRUETYPE_FONTTYPE
	//				DEVICE_FONTTYPE
	//				RASTER_FONTTYPE
	DWORD m_dwType;
	// flag that set to TRUE if the font displays only symbols 
	BOOL m_bIsSymbol;

public:
	_tagOXLBFONTINFO operator=(const _tagOXLBFONTINFO& lbfi)
	{
		if(this==&lbfi)
			return *this;

		m_lf=lbfi.m_lf;
		m_dwType=lbfi.m_dwType;
		m_bIsSymbol=lbfi.m_bIsSymbol;

		return *this;
	}

} OXLBFONTINFO;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXLISTBOXEX_H__)
