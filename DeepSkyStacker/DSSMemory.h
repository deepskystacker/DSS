#pragma once
#ifndef __DSSMEMORY_H__
#define __DSSMEMORY_H__

#pragma push_macro("new")
#if defined(new)
#undef new
#endif
#pragma push_macro("malloc")
#if defined(malloc)
#undef malloc
#endif
#pragma push_macro("free")
#if defined(free)
#undef free
#endif
#pragma push_macro("calloc")
#if defined(calloc)
#undef calloc
#endif
#pragma push_macro("realloc")
#if defined(realloc)
#undef realloc
#endif
#pragma push_macro("_recalloc")
#if defined(_recalloc)
#undef _recalloc
#endif

void *    dss_malloc(size_t sz __DSS_LEAK_DBG_PARAM_LIST__);
void *    dss_calloc(size_t nelem, size_t elsize __DSS_LEAK_DBG_PARAM_LIST__);
void *    dss_realloc(void * ptr, size_t sz __DSS_LEAK_DBG_PARAM_LIST__);
void *    dss_recalloc(void * ptr, size_t nelem, size_t elsize __DSS_LEAK_DBG_PARAM_LIST__);
void     dss_free(void * ptr);

#define malloc(sz)                        dss_malloc(sz, _NORMAL_BLOCK, __FILE__, __LINE__)
#define free(ptr)                        dss_free(ptr)
#define calloc(nelem, elsize)            dss_calloc(nelem, elsize, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(ptr, sz)                dss_realloc(ptr, sz, _NORMAL_BLOCK, __FILE__, __LINE__)
#define _recalloc(ptr, nelem, elsize)    dss_recalloc(ptr, nelem, elsize, _NORMAL_BLOCK, __FILE__, __LINE__)

inline void * operator new (size_t size, int block_type, LPCTSTR szFile, int nLineNo)
{
	return dss_malloc(size, block_type, szFile, nLineNo);
}
inline void * operator new[](size_t size, int block_type, LPCTSTR szFile, int nLineNo)
{ return dss_malloc(size, block_type, szFile, nLineNo); }

inline void * operator new (size_t size)
{
	return dss_malloc(size, _CLIENT_BLOCK, NULL, 0);
}
inline void * operator new[](size_t size)
{ return dss_malloc(size, _CLIENT_BLOCK, NULL, 0); }

inline void operator delete (void * ptr)
{
	dss_free(ptr);
}
inline void operator delete[](void * ptr)
{	dss_free(ptr); }

//Don't forget to pop at the end of the file
#pragma pop_macro("_recalloc")
#pragma pop_macro("realloc")
#pragma pop_macro("calloc")
#pragma pop_macro("free")
#pragma pop_macro("malloc")
#pragma pop_macro("new") 


// Implement the methods

void *    dss_malloc(size_t sz __DSS_LEAK_DBG_PARAM_LIST__)
{
	return ::malloc(sz); 
}
void *    dss_calloc(size_t nelem, size_t elsize __DSS_LEAK_DBG_PARAM_LIST__)
{
	return ::calloc(nelem, elsize);
}

void *    dss_realloc(void * ptr, size_t sz __DSS_LEAK_DBG_PARAM_LIST__)
{
	return ::realloc(ptr, sz);
}

void *    dss_recalloc(void * ptr, size_t nelem, size_t elsize __DSS_LEAK_DBG_PARAM_LIST__)
{
	return ::_recalloc(ptr, nelem, elize);
}

void     dss_free(void * ptr)
{
	return ::free(ptr);
}

#endif // __DSSMEMORY_H__
