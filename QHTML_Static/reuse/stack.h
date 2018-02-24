/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Stack.h
Owner:	russf@gipsysoft.com
Purpose:	Array based stack class.
----------------------------------------------------------------------*/
#ifndef STACK_H
#define STACK_H

#ifndef ARRAY_CONTAINER_H
	#include <Array.h>
#endif //#ifndef ARRAY_CONTAINER_H


namespace Container
{
	template<class T> 
	class CStack: private CArray<T>
	{
		public:
		using CArray<T>::GetSize;
		using CArray<T>::Add;
		inline void Push( const T item )
		//
		//	Add an item to the stack
		{
			Add( item );
		}

		inline T Pop()
		//
		//	Get the top of the stack and remove it
		{
			ASSERT( GetSize() );
			T t = GetAt( GetSize() - 1 );

			RemoveAt( GetSize() - 1 );

			return t;
		}

		inline T Top() const
		//
		//	Get the top of the stack. But do not pop it.
		{
			ASSERT( GetSize() );
			return GetAt( GetSize() - 1 );
		}

		inline T& Top()
		//
		// Return reference to top of stack
		{
			ASSERT( GetSize() );
			return GetAt( GetSize() - 1 );
		}
	};
}

#endif //STACK_H
