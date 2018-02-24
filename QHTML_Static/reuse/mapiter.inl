/*----------------------------------------------------------------------
Copyright (c) 1998 Lee Alexander
Please see the file "licence.txt" for licencing details.
File:		Map.h
Owner:	leea@pobox.com
Purpose:Iterator implemenatation for map
----------------------------------------------------------------------*/

template<class Key, class Value>
CMapIter<Key, Value>::CMapIter( const CMap<Key, Value> &map )
	:	m_Map( map )
	,	m_pBucket( NULL )
	,	m_iEntry( 0 )
{
	Reset();
}

//
//	Operations
template<class Key, class Value>
void CMapIter<Key, Value>::Reset()
{
	m_pBucket = NULL;

	//
	// Set pointers to first entry that contains some data
	for( m_iEntry = 0; m_iEntry < m_Map.m_uBucketArrayLength; m_iEntry++ )
	{
		MapBucketEntry<Key, Value> *pEntry = m_Map.m_pEntrys[m_iEntry];
		if( pEntry )
		{
			m_pBucket = pEntry;
			m_pNext = m_pBucket -> m_pNext;
			return;
		}
	}
}

//
//	Navigation
template<class Key, class Value>
void CMapIter<Key, Value>::Next()
{
	if( m_pNext )
	{
		m_pBucket = m_pNext;
		m_pNext = m_pNext -> m_pNext;
		return;
	}

	//
	// Get next array bucket
	while( ++m_iEntry < m_Map.m_uBucketArrayLength )
	{
		MapBucketEntry<Key, Value> *pEntry = m_Map.m_pEntrys[m_iEntry];
		if( pEntry )
		{
			m_pBucket = pEntry;
			m_pNext = m_pBucket -> m_pNext;
			return;
		}
	}

	m_pBucket = NULL;
}

//
//	State
template<class Key, class Value>
bool CMapIter<Key, Value>::EOL() const
{
	return m_pBucket == NULL;
}

//
// Content
template<class Key, class Value>
Value &CMapIter<Key, Value>::GetValue() const
{
	ASSERT( m_pBucket );
	return m_pBucket -> m_Value;
}

template<class Key, class Value>
const Key &CMapIter<Key, Value>::GetKey() const
{
	ASSERT( m_pBucket );
	return m_pBucket -> m_Key;
}
