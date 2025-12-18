#pragma once
/****************************************************************************
**
** Copyright (C) 2024, 2025 Martin Toeltsch
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/

namespace Medianhelper
{

	template <class T>
	inline void swap(T* a, T* b)
	{
		T temp = *a;
		*a = *b;
		*b = temp;
	}

	// Returns the index i of the pivot element, so that (arr[n] < pivot) for (n < i).
	template <class T>
	inline int partition(T arr[], const int l, const int r)
	{
		const T pivot = arr[r];
		int i = l, j = l;
		while (j < r)
		{
			if (arr[j++] < pivot)
				i++;
			else
				break;
		}
		while (j < r)
		{
			if (arr[j] < pivot)
			{
				swap(&arr[i], &arr[j]);
				i++;
			}
			j++;
		}
		swap(&arr[i], &arr[r]);
		return i;
	}

	template <class T>
	inline void MedianUtil(T arr[], const int l, const int r, const int quantile, T& a, T& b, bool& x, bool& y)
	{
		if (l <= r)
		{
			int partitionIndex = partition(arr, l, r); // Left of partitionIndex are only smaller elements.

			// If partion index = quantile, then we found the median of odd number element in arr[]
			if (partitionIndex == quantile)
			{
				b = arr[partitionIndex];
				y = true;
				if (x)
					return;
			}
			// If index = quantile - 1, then we get a & b as middle element of arr[]
			else if (partitionIndex == quantile - 1)
			{
				a = arr[partitionIndex];
				x = true;
				if (y)
					return;
			}

			// If partitionIndex >= quantile then find the index in first half of arr[]
			if (partitionIndex >= quantile)
				return MedianUtil(arr, l, partitionIndex - 1, quantile, a, b, x, y);
			// If partitionIndex <= quantile then find the index in second half of arr[]
			else
				return MedianUtil(arr, partitionIndex + 1, r, quantile, a, b, x, y);
		}
	}

};

template <class T>
inline T qMedian(T arr[], const int n, const int quantile)
{
	T a{ 0 }, b{ 0 };
	bool x = false, y = false;
	Medianhelper::MedianUtil(arr, 0, n - 1, quantile, a, b, x, y);

	// If n is even -> (a+b)/2 ELSE b
	return ((n & 1) == 0) ? ((a + b) / 2) : b;
}
