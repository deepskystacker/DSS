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
// ZMngPointer Class definition
//
// This is the header for a managed pointer class where the pointer class is 
// responsible for counting the references to the pointed to class object, and
// only deletes the object when the last managed pointer object is deleted (by
// going out of scope (if on the stack) or by using delete (if on the heap).
//
// Also known as a Smart or Reference Counted Pointer
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
#ifndef ZDEFS_INCLUDED
#include <zdefs.h>
#endif

#if defined(ZCLASS_EXPLICIT_NOT_SUPPORTED)
enum ZExplicitInit
{
  ZINIT
};
#endif

template <class TheClass> 
class ZMngPointer
{
protected:
  struct PtrRc;

public:
  //
  // Use this constructor to build a managed pointer from a C++ pointer.
  //
  // An implicit conversion from a C++ pointer to a managed pointer is
  // very dangerous as objects might be deleted without you being aware
  // of it. The reason is that you might implicitly construct a managed
  // pointer from a C++ pointer by using the constructor to meet an API
  // which takes a managed pointer and you've supplied a C++ pointer.
  // the use of the explicit keyword prevents this conversion.
  //
  // After you have constructed a managed pointer from a C++ pointer, you 
  // must not use the C++ pointer again. You should only access the object
  // through the managed pointer.   Otherwise the object could be implicitly
  // destructed while you still have a C++ pointer to it.
  //
  // In particular you MUST NOT construct multiple managed pointers to 
  // an object from the same C++ pointer, as the managed pointers would keep
  // seperate reference counts and thus attempt to delete the same object
  // twice:
  //
  // MyClass * pMc = new MyClass(...);
  //
  // ZMngPointer <MyClass> p1(pMc); // OK
  // ZMngPointer <MyClass> p2(pMc); // DON'T DO THIS
  //
  // A far better/safer approach is as follows:
  //
  // ZMngPointer <MyClass> pObjectOfMyClass(new MyClass(...));
  //
#if defined(ZCLASS_EXPLICIT_NOT_SUPPORTED)
  inline ZMngPointer (TheClass * ptr, ZExplicitInit)
#else
  explicit inline ZMngPointer (TheClass * ptr)
#endif
    : pPtrRc (ptr ? new PtrRc (ptr) : 0) {}

  //
  // Default ctor
  //
  // Used like: ZMngPointer <MyClass> p3;
  //
  inline ZMngPointer ()
  : pPtrRc (0) {}

  //
  // Copy constructor
  //
  inline ZMngPointer (ZMngPointer <TheClass> const &rhs)
  // Copy the pointer to the reference count and pointer holder
  : pPtrRc (rhs.pPtrRc)
  {
    //
    // Increment the reference count if there is one.
    //
    if (pPtrRc)
      pPtrRc->referenceCount++;
  }

  //
  // Destructor
  //
  inline ~ZMngPointer ()
  {
    //
    // If we have a reference count and pointer holder then 
    // decrement the reference count.  If the reference count
    // is now zero delete the reference count and pointer holder
    // which will result in the pointed to object being deleted.
    //
    if (pPtrRc != 0 && --pPtrRc->referenceCount == 0)
      delete pPtrRc;
  }

  //
  // Assignment operator
  //
  inline ZMngPointer <TheClass>& operator= (ZMngPointer <TheClass> const& rhs)
  {
    //
    // Guard against self assignement
    //
    if (pPtrRc != rhs.pPtrRc)
    {
      //
      // If we have a reference count and pointer holder then 
      // decrement the reference count.  If the reference count
      // is now zero delete the reference count and pointer holder
      // which will result in the pointed to object being deleted.
      //
      if(pPtrRc && --pPtrRc->referenceCount == 0) 
        delete pPtrRc;
      //
      // Now that we've done our duty by decrementing the count
      // and deleting as appopriate the object we were pointing to
      // we can copy the pointer to the reference count and pointer
      // holder from the other managed pointer, and increment the
      // reference count if there is one.
      //
      pPtrRc = rhs.pPtrRc;
      if (pPtrRc)
        pPtrRc->referenceCount++;
    }
    return *this;
  }

  inline TheClass& operator * () const { return *pPtrRc->pointer;}

  inline TheClass* operator -> () const { return pPtrRc ? pPtrRc->pointer : 0;}


  //
  // This operator MF will return a C++ pointer to the object
  // managed by this class.
  //
  inline operator TheClass* () const { return pPtrRc ? pPtrRc->pointer : 0;}

  //
  // Comparison operators. Used to determine if two managed pointers
  // are pointing at the same object.
  //
  inline bool operator == (ZMngPointer <TheClass> const& rhs ) const
  { 
    //
    // If both are pointing to the same PtrRc object (or both have null pointers to
    // a PtrRc), then they must point to the same object.
    //
    return (pPtrRc == rhs.pPtrRc);
  }
  inline bool operator != (ZMngPointer <TheClass> const& rhs ) const
  { 
    return !(*this == rhs);
  }

  inline bool operator == (TheClass const* const  rhs) const
  { 
    //
    // This is here for those of you who insist on mixing the use of C++ 
    // pointers with managed pointers (except the degenerate case of the
    // NULL pointer comparison which is fine).
    //
    // If the pointer holder is null and the rhs pointer is null, we're
    // comparing two null pointers, so return true.
    //
    // If we are pointing at an object, and our pointer matches that 
    // passed in, then we are pointing at the same object as the C++
    // pointer passed in.   
    //
    return ((0 == rhs && 0 == pPtrRc) || 
            (0 != pPtrRc && pPtrRc->pointer == rhs)) ;
  }
  inline bool operator != (TheClass const* const  rhs) const
  { 
    return !(*this == rhs);
  }
  inline bool operator == (TheClass * rhs) const
  { 
    //
    // This is here for those of you who insist on mixing the use of C++ 
    // pointers with managed pointers (except the degenerate case of the
    // NULL pointer comparison which is fine).
    //
    // If the pointer holder is null and the rhs pointer is null, we're
    // comparing two null pointers, so return true.
    //
    // If we are pointing at an object, and our pointer matches that 
    // passed in, then we are pointing at the same object as the C++
    // pointer passed in.   
    //
    return ((0 == rhs && 0 == pPtrRc) || 
            (0 != pPtrRc && pPtrRc->pointer == rhs)) ;
  }
  inline bool operator != (TheClass * rhs) const
  { 
    return !(*this == rhs);
  }
protected:
  struct PtrRc
  {
    TheClass * pointer;
    unsigned long referenceCount;

    //
    // Ctor (yes for a struct)
    //
    inline PtrRc (TheClass* ptr) 
    : pointer(ptr)
    , referenceCount(1)
    {}

    //
    // Dtor
    //
    inline ~PtrRc() {delete pointer;}
  } *pPtrRc;
};

#if (0)
// Modifications:
//
// +MH+ $Log$
//
// -MH-
#endif

