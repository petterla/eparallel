#ifndef EP_AUTO_PTR_H
#define EP_AUTO_PTR_H

namespace ep{


	template<class	T>
	class	auto_ptr{
	public:
		auto_ptr(T* ptr = 0):m_ptr(ptr)
		{

		}
		auto_ptr(auto_ptr& other):m_ptr(other.m_ptr){
			other.m_ptr = 0;
		}
		auto_ptr& operator = (auto_ptr& other)
		{
			delete m_ptr;
			m_ptr = other.m_ptr;
			other.m_ptr = 0;
			return	*this;
		}
		~auto_ptr()
		{
			delete m_ptr;
		}

		T* reset(T* pt)
		{
			T* t  = m_ptr;
			m_ptr = pt;
			return	t;
		}

		T* get()
		{
			return	m_ptr;
		}

	private:
		T* m_ptr;
	};

	template<class T>
	class	array_auto_ptr{
	public:
		array_auto_ptr(T* ptr = 0, size_t sz = 0):m_ptr(ptr),m_sz(sz)
		{

		}
		array_auto_ptr(array_auto_ptr& other):m_ptr(other.m_ptr),m_sz(other.m_sz){
			other.m_ptr = 0;
			other.m_sz  = 0;
		}
		array_auto_ptr& operator = (array_auto_ptr& other)
		{
			delete [] m_ptr;
			m_ptr = other.m_ptr;
			m_sz  = other.m_sz;
			other.m_ptr = 0;
			other.m_sz  = 0;
			return	*this;
		}
		~array_auto_ptr()
		{
			delete [] m_ptr;
		}

		T* reset(T* pt, size_t sz)
		{
			T* t  = m_ptr;
			m_ptr = pt;
			m_sz  = sz;
			return	t;
		}

		T* get()
		{
			return	m_ptr;
		}

	private:
		T*     m_ptr;
		size_t m_sz;
	};


};

#endif/*EP_AUTO_PTR_H*/
