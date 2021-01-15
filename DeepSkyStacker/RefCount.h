#pragma once

#include <atomic>

class CRefCount
{
protected:
	std::atomic<long> m_lRefCount;

public:
	CRefCount() :
		m_lRefCount{ 0 }
	{
	};

	virtual ~CRefCount()
	{
	};

	void	AddRef()
	{
		m_lRefCount++;
	};

	void	Release()
	{
		if (m_lRefCount.fetch_sub(1) == 1) // If it was previously one (is now zero), delete the object.
			delete this;
	};
};
