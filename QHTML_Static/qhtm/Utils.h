/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	Utils.h
Owner:	russf@gipsysoft.com
Purpose:	Some utility code for the entire project.
----------------------------------------------------------------------*/
#ifndef UTILS_H
#define UTILS_H

//
//	Register/unregister a section for a timer event.
//	CSectionABC::OnTimer( int ) will be called every time the event fires
void GetMousePoint( class WinHelper::CPoint &pt );

#endif //UTILS_H