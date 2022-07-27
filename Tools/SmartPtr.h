#ifndef	_SMARTPTR_H_
#define	_SMARTPTR_H_

template <class T>
class CSmartPtr
{
public:
	T* m_p;

public:
	CSmartPtr() : m_p{ nullptr }
	{}

	CSmartPtr(T* p) : m_p{ p }
	{
		if (p != nullptr)
			p->AddRef();
	}

	CSmartPtr(const CSmartPtr<T>& p) : m_p{ p.m_p }
	{
		if (p.m_p != nullptr)
			p.m_p->AddRef();
	}

	void Attach(T* p)
	{
		if (p != nullptr)
			p->AddRef();
		if (m_p != nullptr)
			m_p->Release();
		m_p = p;
	}

	void Create()
	{
		if (m_p != nullptr)
			m_p->Release();
		m_p = nullptr;
		Attach(new T);
	}

	bool CopyTo(T** pp)
	{
		bool bResult = false;
		if (pp != nullptr)
		{
			*pp = m_p;
			if (m_p != nullptr)
			{
				m_p->AddRef();
				bResult = true;
			}
		}

		return bResult;
	}

	void Release()
	{
		T* pTemp = m_p;
		if (pTemp != nullptr)
		{
			m_p = nullptr;
			pTemp->Release();
		}
	}

	virtual ~CSmartPtr()
	{
		Release();
	}

	T& operator*() const
	{
		return *m_p;
	}

	T** operator&()
	{
		if (m_p != nullptr)
		{
			// To avoid memory leak
			m_p->Release();
			m_p = nullptr;
		}
		return &m_p;
	}

	T* operator->()
	{
		return m_p;
	}

	operator T*() const
	{
		return m_p;
	}

	T* operator=(T* lp)
	{
		if (lp != m_p)
			Attach(lp);

		return *this;
	}

	T* operator=(const CSmartPtr<T>& p)
	{
		if (p.m_p != m_p)
			Attach(p.m_p);

		return *this;
	}

	bool operator!() const
	{
		return m_p == nullptr;
	}

	bool operator<(const CSmartPtr<T>& p) const
	{
		return static_cast<std::uintptr_t>(m_p) < static_cast<std::uintptr_t>(p.m_p);
	}

	bool operator<(const T* p) const
	{
		return static_cast<std::uintptr_t>(m_p) < static_cast<std::uintptr_t>(p);
	}

	bool operator!=(const T* p) const
	{
		return m_p != p;
	}

	bool operator==(const T* p) const
	{
		return m_p == p;
	}
};

#endif		//	_SMARTPTR_H_
