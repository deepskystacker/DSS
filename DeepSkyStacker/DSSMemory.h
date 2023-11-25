#pragma once

#ifdef _DEBUG

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

		void* (operator new)(size_t nSize, LPCSTR, int)
		{
			return DllExports::GdipAlloc(nSize);
		}

		void operator delete(void* p, LPCSTR, int)
		{
			DllExports::GdipFree(p);
		}

		};
#endif // #ifndef _GDIPLUSBASE_H
#endif // #ifdef _DEBUG
