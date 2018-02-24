/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	MessageReflector.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef MESSAGEREFLECTOR_H
#define MESSAGEREFLECTOR_H

class CMessageReflector
{
public:

	virtual LRESULT OnWindowMessage( UINT uMessage, WPARAM wParam, LPARAM lParam ) = 0;

	static CMessageReflector* IsMessageReflector( HWND hwnd );

	void SetMessageReflector( HWND hwnd );
	void RemoveMessageReflector( HWND hwnd );
};

#endif //MESSAGEREFLECTOR_H