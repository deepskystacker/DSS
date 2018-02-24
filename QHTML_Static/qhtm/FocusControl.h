/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	FocusControl.h
Owner:	russf@gipsysoft.com
Purpose:

	Abstract class that takes care of setting, getting and determining who has the focus.

----------------------------------------------------------------------*/
#ifndef FOCUSCONTROL_H
#define FOCUSCONTROL_H

class CFocusControlABC
{
public:
	//
	//	Set the focus to this item
	virtual void SetFocus( bool bHasFocus ) = 0;

	//
	//	Perform whatever activation is needed (e.g. launch a URL)
	virtual void Activate() = 0;

	//
	//	Update the display of the item
	virtual void Update() = 0;

	//
	//	Is this item focused
	virtual bool IsFocused() const = 0;

	//
	//	Get the focus items bounding rectangle
	virtual void GetObjectRect( WinHelper::CRect &rcBounds ) const = 0;

	//
	//	Return true if the object has the same name as the one passed in.
	virtual bool IsSameName( LPCTSTR pcszName ) const { UNREF( pcszName ); return false; }

	//
	//	Get the name of this object
	virtual LPCTSTR GetName() const { return NULL; }

	//
	//	Return true if this control is selected
	virtual bool IsSelected() const { return true; }

	void SetHTMLID( const StringClass &strHTMLID )
		{ m_strHTMLID = strHTMLID; }

	bool IsSameID( const StringClass &strHTMLID ) const
		{ return strHTMLID.GetLength() && m_strHTMLID.GetLength() && _tcsicmp( strHTMLID, m_strHTMLID ) == 0; }

private:
	StringClass m_strHTMLID;
};


#endif //FOCUSCONTROL_H