#ifndef	_SMARTPTR_H_
#define	_SMARTPTR_H_

class CRefCount
{
protected :
	LONG		m_lRefCount;

public :
	CRefCount()
	{
		m_lRefCount = 0;
	};

	virtual ~CRefCount()
	{
	};

	void	AddRef()
	{
		#if defined(_WINDOWS_)
		InterlockedIncrement(&m_lRefCount);
		#else
		m_lRefCount++;
		#endif
	};

	void	Release()
	{
		#if defined(_WINDOWS_)
		if (!InterlockedDecrement(&m_lRefCount))
			delete this;
		#else
		m_lRefCount--;
		if (!m_lRefCount)
			delete this;
		#endif
	};
};

template <class T>
class CSmartPtr
{
public :
	T *			m_p;

public :
	CSmartPtr()
	{
		m_p = NULL;
	};

	CSmartPtr(T * p)
	{
		if (p)
			p->AddRef();
		m_p = p;
	};

	CSmartPtr(const CSmartPtr<T> & p)
	{
		if (p.m_p)
			p.m_p->AddRef();
		m_p = p.m_p;
	};

	void Attach(T * p)
	{
		if (p)
			p->AddRef();

		if (m_p)
			m_p->Release();
		m_p = p;
	};

	void	Create()
	{
		if (m_p)
			m_p->Release();
		m_p = NULL;
		Attach(new T);
	};

	BOOL	CopyTo(T ** pp)
	{
		BOOL		bResult = FALSE;
		if (pp)
		{
			*pp = m_p;
			if (m_p)
			{
				m_p->AddRef();
				bResult = TRUE;
			};
		};

		return bResult;
	};

	void	Release()
	{
		T *		pTemp = m_p;
		if (pTemp)
		{
			m_p = NULL;
			pTemp->Release();
		};
	};

	virtual ~CSmartPtr()
	{
		Release();
	};

	T & operator * () const
	{
		return *m_p;
	};

	T ** operator & ()
	{
		if (m_p)
		{
			// To avoid memory leak
			m_p->Release();
			m_p = NULL;
		};
		return &m_p;
	};

	T * operator ->()
	{
		return m_p;
	};

	operator T*() const 
	{
		return m_p;
	}

	T * operator = (T * lp)
	{
		if (lp != m_p)
			Attach(lp);

		return (*this);
	};

	T * operator = (const CSmartPtr<T> & p)
	{
		if (p.m_p != m_p)
			Attach(p.m_p);

		return (*this);
	};

	bool operator !()
	{
		if (m_p)
			return false;
		else
			return true;
	};

	bool operator < (const CSmartPtr<T> & p) const
	{
		return (LONG)m_p < (LONG)p.m_p;
	};
	bool operator < (const T * p) const
	{
		return (LONG)m_p < (LONG)p;
	};
	bool operator != (const T * p) const
	{
		return m_p != p;
	};
	bool operator == (const T * p) const
	{
		return m_p == p;
	};
};

#endif		//	_SMARTPTR_H_
