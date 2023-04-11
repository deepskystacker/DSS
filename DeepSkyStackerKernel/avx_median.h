#pragma once

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
	T a, b;
	bool x = false, y = false;
	Medianhelper::MedianUtil(arr, 0, n - 1, quantile, a, b, x, y);

	// If n is even -> (a+b)/2 ELSE b
	return ((n & 1) == 0) ? ((a + b) / 2) : b;
}
