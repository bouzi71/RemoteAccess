#pragma once

template <typename T>
class SmartPtr 
{
public:
	SmartPtr(T *obj)
		: m_Object(obj)
	{ }

	~SmartPtr()
	{
		delete m_Object;
	}

	operator T*() const	{ return m_Object;	}
	T* operator->() { return m_Object; }
	T& operator* () { return *m_Object; }

private:
	T* m_Object;
};