/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_Types.h
Owner:	russf@gipsysoft.com
Purpose:	All 'types' used in QHTM

	Motivation:
	To make porting easier. If the basic classes can be swapped out then
	replacing them without something more 'local' should be easier and should
	require less code mods.

	Also, moving windows dependent types from the core code and only having
	it within certain allowed areas (drawing and window management) will make
	porting to other OS's much simpler.

	Whether we get to the point of porting or not is yet to be discuvered!

	Much work to be done. The ultimate test is removing windows.h etc. from
	the standard header and only including it in the key modules.

----------------------------------------------------------------------*/
#ifndef QHTM_TYPES_H
#define QHTM_TYPES_H

#ifndef MAP_CONTAINER_H
	#include <reuse/Map.h>
#endif	//	MAP_CONTAINER_H

#ifndef ARRAY_CONTAINER_H
	#include <reuse/Array.h>
#endif	//	ARRAY_CONTAINER_H

#define MapClass				Container::CMap
#define MapIterClass		Container::CMapIter
#define ArrayClass			Container::CArray
#define StackClass			Container::CStack

#define StringClass			CSimpleString
#define ArrayOfInt			Container::CArray< int >
#define ArrayOfSizet		Container::CArray< size_t >
#define ArrayOfBool			Container::CArray< bool >
#define ArrayOfChar			Container::CArray< TCHAR >

//
//	Simple sting uses "ArrayOfChar"
#ifndef SIMPLESTRING_H
	#include "SimpleString.h"
#endif	//	SIMPLESTRING_H

#endif //QHTM_TYPES_H