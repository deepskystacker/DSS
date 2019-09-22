#if !defined(_REGISTRY_H__)
#define _REGISTRY_H__

/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1997 by Joerg Koenig
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    J.Koenig@adg.de                 (company site)
//    Joerg.Koenig@rhein-neckar.de    (private site)
/////////////////////////////////////////////////////////////////////////////

// last revised $Date: 24.09.98 21:12 $ $Revision: 5 $

// -----------------------------------------------------------------------------
// Description:
//	This headerfile describes the following classes:
//		CRegistry	registry wrapper (non-abstract base class)
//		CRegVal		registry information holder
//
//	Inheritance is neccessary only if you want to use the "RegistryTreeWalk()"
//	function declared in CRegistry. Then you have to derive your own class and to
//	override the "OnValueHit()" and/or the "OnKeyHit()" virtual member functions
//	(see below).
// -----------------------------------------------------------------------------

#ifdef _EXPLICIT
	#pragma message (__FILE__ ": Redefining macro _EXPLICIT")
	#undef _EXPLICIT
#endif

#if _MSC_VER >= 1100
	#define _EXPLICIT	explicit
#else
	#define _EXPLICIT
#endif

#ifndef STRICT
	#pragma message (__FILE__ ": You should #define STRICT to keep CRegistry behave correct in all circumstances.")
#endif

#include <winreg.h>

class CRegVal ;

class CRegistry {
	HKEY		m_hCurrentKey ;

	public:

#ifndef _REG_NO_TREEWALK
		enum {
			// the following constants are for the RegistryTreeWalk()
			// member function:
			RTW_NotifyValue		= 1,
			RTW_NotifyKey		= 2,
			RTW_NotifyAll		= 3		// same as "RTW_NotifyValue | RTW_NotifyKey"
		};
#endif	// _REG_NO_TREEWALK

	public:
		_EXPLICIT CRegistry( HKEY OpenKey = HKEY_CURRENT_USER ) ;
		CRegistry( const CRegistry & ) ;
		virtual ~CRegistry() ;

        CRegistry& operator=(CRegistry const& other) = delete;

	public:		// attributes
		// GetKey() returns the current HKEY
		HKEY		GetKey() const ;

		// Change the current key of the registry and return
		// the old one.
		HKEY		SetKey( HKEY ) ;

	public:		// operations
		// "Connect()" connects the registry object with either
		// the local or a remote computer. A connection to the
		// local computer will be forced by setting the <Remote>
		// parameter to NULL.
		// if the function fails (i.e. returns FALSE), then
		// the previously connected HKEY will be restored, so
		// there is no need to reset it (via the SetKey() member
		// function).
		// The <Remote> string shall be in UNC notation (such as
		// "\\computer" - note that you have to write each backslash
		// twice in C/C++ code) !
		//
		// NOTE: You cannot create subkeys in the root of a
		//		 predefined key if you've called "Connect()".
		//		 Thus the following code will *not* work:
		//			reg.Connect( TEXT("\\\\picard") ) ;
		//			reg.SaveKey( TEXT("MyOwnKeyInTheRoot"), TEXT("AValue"), regval ) ;
		//		 if "MyOwnKeyInTheRoot" does not already exist.
		//
		// NOTE: that you can actually connect to either
		//		 HKEY_LOCAL_MACHINE or HKEY_USERS only. This is a
		//		 restriction by Windows ...
		BOOL		Connect(
						LPCTSTR Remote,
						HKEY = HKEY_LOCAL_MACHINE
					) ;

		// Load the value <ValName> of a given subkey <SubKey>
		// of the current key in the registry. The value's data will
		// be stored in <Data>. Returns TRUE on success, FALSE on
		// failure (i.e. <SubKey> or <ValName> (or both) not found,
		// or not enough permissions)
		BOOL		LoadKey(		// load data of any type
						LPCTSTR SubKey,
						LPCTSTR ValName,
						CRegVal & Data
					) const;
		BOOL		LoadKey(		// load a number
						LPCTSTR SubKey,
						LPCTSTR ValName,
						DWORD & Number
					) const;
		BOOL		LoadKey(		// load binary data
						LPCTSTR SubKey,
						LPCTSTR ValName,
						BYTE * pBuffer,
						DWORD dwSize
					) const;

		BOOL		LoadKey(		// load a string
						LPCTSTR SubKey,
						LPCTSTR ValName,
						CString & String
					) const;
#ifdef _AFXDLL
		BOOL		LoadKey(		// load an array of strings
						LPCTSTR SubKey,
						LPCTSTR ValName,
						CStringArray & arrString
					) const;
#endif

		// save <Data> as <ValName> in the given <SubKey> of the
		// current key in the registry. If <ValName> does not exist,
		// it will be created, elsewise its value will be changed.
		// if <SubKey> does not exist, then it will be created too
		// (with all sub-subkeys in it).
		// You have to separate subkeys like a DOS-Path
		// (example: "ADG\\Pegasus\\General\\Protocol" is a valid subkey)
		// Returns TRUE on success or FALSE on failure (i.e. not enough
		// permissions)
		BOOL		SaveKey(		// store data of any type
						LPCTSTR SubKey,
						LPCTSTR ValName,
						const CRegVal & Data
					) const;
		BOOL		SaveKey(		// store a number
						LPCTSTR SubKey,
						LPCTSTR ValName,
						const DWORD & Number
					) const;
		BOOL		SaveKey(		// store binary data
						LPCTSTR SubKey,
						LPCTSTR ValName,
						const LPBYTE pBuffer,
						const DWORD dwSize
					) const;
		BOOL		SaveKey(		// store a string
						LPCTSTR SubKey,
						LPCTSTR ValName,
						LPCTSTR String
					) const;

#ifdef _AFXDLL
		BOOL		SaveKey(	// store an array of strings
						LPCTSTR SubKey,
						LPCTSTR ValName,
						const CStringArray & arrString
					) const;
#endif

		// "DeleteKey()" deletes the given Key in the registry. If
		// <ValName> is equal to NULL, then the complete key will
		// be removed (with all of its subkeys).
		// Otherwise only the given <ValName> of that
		// <SubKey> is affected.
		BOOL		DeleteKey(
						LPCTSTR SubKey,
						LPCTSTR ValName = 0
					) const ;

		// The "HasEntries()" member function checks wether the
		// given <SubKey> of the current key has other subkeys
		// and/or values. If so - it returns TRUE, otherwise FALSE.
		// If you define a buffer for either <SubKeyCount> or
		// <ValueCount> (or both), the number of hits of that type of
		// entry will be stored in it.
		BOOL		HasEntries(
						LPCTSTR SubKey,
						DWORD * SubKeyCount = 0,
						DWORD * ValueCount = 0
					) const ;

#ifndef _REG_NO_TREEWALK
		// The "RegistryTreeWalk()" function walks through the registry
		// beginning at the current key and the given <SubKey>. if
		// <SubKey> is NULL, then the walk begins at root level.
		// For every value the function "OnValueHit()" will be called.
		// Note that OnValueHit() and OnKeyHit() are dummies by default
		// (don't made it pure, because usage of the RegistryTreeWalk()
		// function may be very seldom...).
		// You have to derive your own class and to override
		// these handler function(s), if you want to use the
		// "RegistryTreeWalk()" function!
		// NOTE: If you specify a <SubKey> other than NULL, the "OnKeyHit()"
		// notification-handler for this initial key will NOT be called !
		// "RegistryTreeWalk()" returns TRUE on success or FALSE on failure
		// (i.e. failure on accessing the registry (watch your DEBUG-Window
		// in debugging mode) or one of the notification-handlers
		// (OnValueHit(), OnKeyHit()) forces the walk to abort)
		BOOL		RegistryTreeWalk(
						LPCTSTR SubKey = 0,
						UINT Flags = CRegistry::RTW_NotifyAll
					);

	public:		// overridables
		// The "OnValueHit()" function will be called from within the
		// "RegistryTreeWalk()" function, whenever a valid value is
		// hit. You should return TRUE, if you want to continue the
		// walk or FALSE to abort. This function will only be called
		// if you specified either "RTW_NotifyValue" or "RTW_NotifyAll"
		// in the flags to "RegistryTreeWalk()".
		// The <Depth> parameter indicates the depth of the walk. It
		// will be zero on root-level (or the given <SubKey> to
		// "RegistryTreeWalk()", see above), one on the subkey, two on
		// the sub-subkey and so on.
		// NOTE that the default implementation of this function returns
		// FALSE always.
		virtual BOOL	OnValueHit(
							LPCTSTR Key,
							LPCTSTR ValueName,
							const CRegVal & Value,
							USHORT Depth
						);

		// "OnKeyHit()" will be called from the "RegistryTreeWalk()" function
		// only if you have specified either "RTW_NotifyKey" or "RTW_NotifyAll"
		// in the flags to "RegistryTreeWalk()".
		// For the <Depth> parameter see "OnValueHit()" above.
		// NOTE that the default-implementation returns always FALSE !
		virtual BOOL	OnKeyHit( LPCTSTR Key, USHORT Depth ) ;
#endif	// _REG_NO_TREEWALK

	public:		// static members ...
		// same as above, but at static scope for lower-level usage.
		static BOOL		LoadKey(
							LPCTSTR SubKey,
							LPCTSTR ValName,
							CRegVal & Data,
							HKEY Key
						) ;

		// same as above, but at static scope for lower-level usage.
		static BOOL		SaveKey(
							LPCTSTR SubKey,
							LPCTSTR ValName,
							const CRegVal & Data,
							HKEY Key
						) ;

		// same as above, but at static scope for lower-level usage.
		static BOOL		DeleteKey(
							LPCTSTR SubKey,
							LPCTSTR ValName,
							HKEY Key
						) ;

		// same as above, but at static scope for lower-level usage.
		static BOOL		HasEntries(
							LPCTSTR SubKey,
							DWORD * SubKeyCount,
							DWORD * ValueCount,
							HKEY Key
						) ;

	private:
#ifndef _REG_NO_TREEWALK
		// The "RegTreeWalk()" will be called from "RegistryTreeWalk()" or from
		// itself recursivly.
		BOOL			RegTreeWalk( LPCTSTR SubKey, HKEY hSubKey, UINT Flags, USHORT Depth ) ;
#endif	// _REG_NO_TREEWALK
};

class CRegVal {
	friend class CRegistry ;

	HANDLE		m_hHeap ;
	LPBYTE		m_pbyteData ;
	DWORD		m_dwDataSize ;
	DWORD		m_dwType ;

	public:
		CRegVal() ;
		_EXPLICIT CRegVal( DWORD ValueToSet );
		_EXPLICIT CRegVal( LPCTSTR ValueToSet );
		CRegVal( const LPBYTE pBuffer, DWORD Size);

#ifdef _AFXDLL
		_EXPLICIT CRegVal( const CStringArray & ValueToSet ) ;
#endif

		~CRegVal() ;

	public:			// functions for the world
		DWORD		GetType() const;
		DWORD		GetSize() const;
		void		SetType( DWORD );

		// direct access to the buffer:
		// NOTE that this buffer is valid as long as this object lives and
		// doesnt change its content !
		// It is much safer to call one of the "GetValue()" methods below !
		LPBYTE			GetRawData() { return m_pbyteData; }
		const LPBYTE	GetRawData() const { return m_pbyteData; }

		// the following GetValue()-functions return FALSE, if the type
		// of the registry value doesn't match. In such a case
		// the data (the parameter refers to) stays undefined.

		// this function can be used with the following types:
		//		REG_SZ				zero-sized unicode string
		//		REG_EXPAND_SZ		same as REG_SZ (with environ. references)
		//		REG_LINK			symbolic link (unicode)
		//		REG_MULTI_SZ		multiple unicode strings (each separated
		//							by '\0' and a double '\0' at the end)
		// This function is somewhat dangerous, because <strp> will point
		// directly to the data-storage of this class, so if you delete
		// the CRegVal object, the pointer is not longer valid !
		// See the following two functions for alternatives.
		BOOL		GetValue( LPCTSTR & strp ) const ;

		// Use this function for a safer get of a string (except an array)
		// thus:
		//		REG_SZ
		//		REG_EXPAND_SZ
		//		REG_LINK
		BOOL		GetValue( CString & str ) const;

		// Get an array of strings
		// thus:
		//		REG_MULTI_SZ
#ifdef _AFXDLL
		BOOL		GetValue( CStringArray & arr ) const;
#endif

		// this function can be used with the following types:
		//		REG_BINARY			free form binary
		//		REG_RESOURCE_LIST	resource list in the resource map
		//		REG_FULL_RESOURCE_DESCRIPTOR	resource list in the
		//										hardware description
		// if 'size' is not exactly of the same value as the data's size, then
		// this function returns FALSE too.
		BOOL		GetValue( LPBYTE & buffer, DWORD size ) const ;

		// this function can be used with the following types:
		//		REG_DWORD					32-bit number
		//		REG_DWORD_LITTLE_ENDIAN		same as REG_DWORD
		//		REG_DWORD_BIG_ENDIAN		32-bit number
		BOOL		GetValue( DWORD & ) const ;

		// use this function to set a numeric value
		void		SetValue( DWORD Value ) ;

		// use the following function to set up a string value. if that string
		// contains references to the environment (such as "%PATH%") then you
		// should set <Expand> to TRUE.
		void		SetValue( LPCTSTR Value, BOOL Expand = FALSE ) ;

		// use the following function to set up a multiple string (i.e. string
		// array)
		void		SetValue( LPCTSTR Array [], const DWORD Size ) ;

#ifdef _AFXDLL
		void		SetValue( const CStringArray & );
#endif

		// the following function allows you to set up binary data of any
		// type and size.
		void		SetValue( const LPBYTE Data, const DWORD Size ) ;

		CRegVal &	operator= ( DWORD dwVal ) {
						SetValue( dwVal );
						return * this ;
					}

		CRegVal &	operator= ( LPCTSTR pszVal ) {
						SetValue( pszVal );
						return * this ;
					}
#ifdef _AFXDLL
		CRegVal &	operator= ( const CStringArray & arr ) {
						SetValue( arr );
						return * this ;
					}
#endif

	private:		// functions for "friend"s only
		BYTE *		AllocateDataBuffer( DWORD ) ;
		void		FreeDataBuffer() ;
} ;

#endif // !defined(_REGISTRY_H__)
