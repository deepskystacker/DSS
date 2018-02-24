/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	cursor.h
Owner:	russf@gipsysoft.com
Purpose:	Cursor abstraction.
----------------------------------------------------------------------*/
#ifndef CURSOR_H
#define CURSOR_H


class CCursor
{
public:
	CCursor();
	virtual ~CCursor();

	enum CursorShape { knMinimumCursor, knArrow, knHand, knMaximumCursor };
	void Load( CursorShape c );

	void Set();


private:
	CursorShape m_cursor;
};


#endif //CURSOR_H