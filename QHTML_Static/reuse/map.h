/*----------------------------------------------------------------------
Copyright (c) 1998 Lee Alexander
Please see the file "licence.txt" for licencing details.
File:		Map.h
Owner:	leea@pobox.com
Purpose:Interface for hash map
----------------------------------------------------------------------*/

#ifndef MAP_CONTAINER_H
#define MAP_CONTAINER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace Container
{
	template< class Key, class Value> class CMapIter;

	template<class Key, class Value> class MapBucketEntry
	{
	public:
		Value m_Value;
		Key		m_Key;

		MapBucketEntry *m_pNext;
		MapBucketEntry(): m_pNext( NULL ){}
		const MapBucketEntry &operator = ( const MapBucketEntry &rhs ){ m_Value = rhs.m_Value; m_Key = rhs.m_Key; m_pNext = rhs.m_pNext; return this; }
		MapBucketEntry( const MapBucketEntry &rhs ){ operator =( rhs ); }
	};

	template<class Key, class Value>
	class CMap
	{
		public:
		CMap( size_t uBucketArrayLength = 97 );
		CMap( const CMap<Key, Value> & );

		void SetBucketSize( size_t uSize );
		size_t GetBucketSize() const;

		virtual ~CMap();

		CMap<Key, Value> &operator = ( const CMap<Key, Value> & );

		//
		// Operations
		Value *Lookup( const Key &key );
		bool Lookup( const Key &key, Value &value );
		const Value *Lookup( const Key &key ) const;

		void SetAt(const Key &key, Value value );
		Value &CreateAt( const Key &key );
		void RemoveAll();
		void RemoveAt( const Key &key );

		size_t GetSize() const { return m_uCount; };

		//
		//	**Implementation**, do use anything under here. the only reason why some of this is public is that there seems
		//	to be a bug in the MSVC 6.0 compiler that even though the CMapIter is a friend it still cannot access CMaps
		//	privates.
		private:
		size_t	m_uBucketArrayLength;
		MapBucketEntry<Key, Value>	**m_pEntrys;

		private:
		// richg - 19990224 - VC5 chokes if the declarator 'class' is specified here.
		// it produces a 'redefinition error'. Everything works fine if the
		// 'class' declarator is omitted, though.
		// friend class CMapIter< class Key, class Value>;
		friend class CMapIter< Key, Value>;

		MapBucketEntry<Key, Value> *Find( size_t uHash, const Key &key ) const;


		size_t				m_uCount;

		private:
		};

	#include "map.inl"


	////////////////////////////////////
	// Helper functions


	//
	//	Hash value for simple types

	template<class Key>
	inline size_t HashIt( const Key &key )
	{
		//
		// Default implementation. If you get a compile error here then you need to create a 'HashIt' function
		return ( (size_t)(void*)(unsigned short)key) >> 4;
	}
	//
	// Default implementation for comparison
	template<class Key>
	inline bool ElementsTheSame( const Key &lhs, const Key &rhs )
	{
		return lhs == rhs;
	}


	//
	//	Hash value for a LPCTSTR
	inline size_t HashIt( LPCTSTR pcszText )
	{
		size_t uHash = 0;
		while( *pcszText )
		{
			uHash = uHash << 1 ^ toupper( *pcszText++ );
		}

		return uHash;
	}
	//
	//	Overriden comparison for LPCTSTR
	inline bool ElementsTheSame( LPCTSTR pcszLHS, LPCTSTR pcszRHS )
	{
		return _tcsicmp( pcszLHS, pcszRHS ) == 0;
	}


	//
	//	Hash value for a LPTSTR
	inline size_t HashIt( LPTSTR pszText )
	{
		return HashIt((LPCTSTR)pszText);
	}

	inline bool ElementsTheSame( LPTSTR pcszLHS, LPTSTR pcszRHS )
	{
		return _tcsicmp( pcszLHS, pcszRHS ) == 0;
	}

}
#endif //#ifdef MAP_CONTAINER_H