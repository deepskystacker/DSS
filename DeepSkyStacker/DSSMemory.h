#pragma once
#ifndef __DSSMEMORY_H__
#define __DSSMEMORY_H__

#ifdef _DEBUG

namespace Gdiplus
{
	namespace DllExports
	{
#include <GdiplusMem.h>
};

#ifndef _GDIPLUSBASE_H
#define _GDIPLUSBASE_H
	class GdiplusBase
	{
	public:
		void(operator delete)(void* in_pVoid)
		{
			DllExports::GdipFree(in_pVoid);
		}

		void* (operator new)(size_t in_size)
		{
			return DllExports::GdipAlloc(in_size);
		}

		void(operator delete[])(void* in_pVoid)
		{
			DllExports::GdipFree(in_pVoid);
		}

		void* (operator new[])(size_t in_size)
		{
			return DllExports::GdipAlloc(in_size);
		}

		void * (operator new)(size_t nSize, [[maybe_unused]] LPCSTR lpszFileName, [[maybe_unused]] int nLine)
		{
			return DllExports::GdipAlloc(nSize);
		}

		void operator delete(void* p, [[maybe_unused]] LPCSTR lpszFileName, [[maybe_unused]] int nLine)
		{
			DllExports::GdipFree(p);
		}

		};
#endif // #ifndef _GDIPLUSBASE_H
		}
#endif // #ifdef _DEBUG

#endif // __DSSMEMORY_H__
