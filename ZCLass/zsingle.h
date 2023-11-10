#pragma once
//***************************************************************************
// Project : Z Class Library
// $Workfile$
// $Revision$
// $Date$
//
// Original Author: David C. Partridge
//
// +USE+ Description: 
// Module Description.
//
// C++ header file defining template class ZSingleton which wrappers a
// Singleton object.
// 
// -USE-
// (C) Copyright 1997 David C. Partridge
//
// Language:    ANSI Standard C++
// Target:      Portable (with modifications) currently supported are:
//              Windows NT 4.x and later
//              Unix systems (untested)
//
// Modifications:
//   See history at end of file ...
//
//***************************************************************************/

template <class T>
class ZSingleton 
{
protected:
    T* m_pSingleton;
public:
	//Default constructor - create an the singleton object
	ZSingleton() : m_pSingleton(NULL)
	{
		m_pSingleton = T::GetSingleton();
	}

	//Destructor - release instance of object
	~ZSingleton(void) 
	{ 
		//Release our instance of the object
		if (m_pSingleton)
		{
			m_pSingleton->Release();
		}
	}
    
	
	//Expose methods
    T* operator->(void) 
	{
		return m_pSingleton;
	}
    
	//assignment operators
	ZSingleton& operator=(const ZSingleton<T>& otherObj)
	{
		//Singletons of same type must point to the same object - do nothing
		return *this;	
  }


};
#if (0)
// Modifications:
//
// +MH+ $Log$
//
// -MH-
#endif
