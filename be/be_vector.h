/*
* Copyright (c) 2010
* efgod@126.com.
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies and
* that both that copyright notice and this permission notice appear
* in supporting documentation.  efgod@126.com. makes no
* representations about the suitability of this software for any
* purpose.  It is provided "as is" without express or implied warranty.
*/
#ifndef BE_VECTOR1_H
#define BE_VECTOR1_H

#include <cassert>
#include "be_utility.h"
#include "be_alloc.h"
#include "be_auto_ptr.h"

namespace be{

#ifdef _WIN32

#pragma warning(push) 

#pragma warning(disable:4181)

#endif

 template<class T, class ALLOC = alloc> 
 class vector
 {
 public:
  template<class NODE>
  class vector_iterator{
  public:
   typedef random_access_iterator_tag iterator_category;
   typedef ptrdiff_t   difference_type;
   typedef size_t    size_type;
   typedef NODE    value_type;
   typedef value_type&   reference;
   typedef value_type*   pointer;

   vector_iterator(NODE* nod = 0):m_cur(nod){
   }
   reference operator*()
   {
    return *m_cur;
   }
   pointer  operator->()
   {
    return m_cur;
   }

   const reference operator*()const
   {
    return *m_cur;
   }
   const pointer  operator->()const
   {
    return m_cur;
   }


   reference operator[] (difference_type offset)const
   {
    return (*(*this + offset));
   }

   vector_iterator& operator++()
   {
    ++m_cur;
    return *this;
   }
   vector_iterator operator++(int)
   {
    vector_iterator itor = *this;
    ++*this;
    return itor;
   }

   vector_iterator& operator += (size_type sz)
   {
    m_cur += sz;
    return *this;
   }

   vector_iterator operator + (size_type sz)const
   {
    vector_iterator itor = *this;
    itor += sz;
    return itor;
   }


   vector_iterator& operator--()
   {
    --m_cur;
    return *this;
   }
   vector_iterator operator--(int)
   {
    vector_iterator itor = *this;
    --*this;
    return itor;
   }

   vector_iterator& operator -= (size_type sz)
   {
    m_cur -= sz;
    return *this;
   }

   vector_iterator operator - (size_type sz)const
   {
    vector_iterator itor = *this;
    itor -= sz;
    return itor;
   }

   difference_type operator - (const vector_iterator& other)const
   {
    return m_cur - other.m_cur;
   }

   bool operator == (const vector_iterator& other)const
   {
    return m_cur == other.m_cur;
   }
   bool operator != (const vector_iterator& other)const
   {
    return m_cur != other.m_cur;
   }
   bool operator > (const vector_iterator& other)const
   {
    return m_cur > other.m_cur;
   }
   bool operator < (const vector_iterator& other)const
   {
    return m_cur < other.m_cur;
   }

   bool operator >= (const vector_iterator& other)const
   {
    return m_cur >= other.m_cur;
   }
   bool operator <= (const vector_iterator& other)const
   {
    return m_cur <= other.m_cur;
   }

  private:
   friend class vector;
   NODE* m_cur;
  };
  typedef T        node_type;
  typedef const T      const_node_type;
  typedef vector_iterator<T>    iterator;
  typedef vector_iterator<const T>  const_iterator;
  typedef T        value_type;
  typedef T&        reference;
  typedef T*        pointer;
  typedef const T&      const_reference;
  typedef const T*      const_pointer;
  typedef size_t       size_type;
  typedef allocator<node_type,ALLOC>  node_allocator;

  vector(size_type cap = 0):m_buf(cap?node_allocator::allocate_n(cap):0),
   m_size(0),m_capacity(0)
  {
   m_head = m_buf;
  }

  vector(const vector& other):m_buf(0),m_head(0),
   m_size(0),m_capacity(0)
  {
   insert(begin(), other.begin(), other.end());
  }

  vector& operator=  (const vector& other)
  {
   if(m_capacity >= other.size())
   {
    destroy(m_head, m_size);
    be_copy(m_buf, other.m_head, other.m_head + m_size);
    m_head = m_buf;
    m_size = other.m_size;
   }else{
    vector tmp(other);
    swap(tmp);
   }
   return *this;
  }

  ~vector()
  {
   destroy(m_head, m_size);
   ALLOC::deallocate(m_buf, m_capacity * sizeof(node_type));
  }

  inline size_type size()const
  {
   return m_size;
  }
  inline bool  empty()const
  {
   return !m_size;
  }
  inline reference front()
  {
   return *m_head;
  }
  inline const_reference front() const
  {
   return *m_head;
  }
  inline reference back()
  {
   return *(m_head+m_size-1);
  }
  inline const_reference back() const
  {
   return *(m_head+m_size-1);
  }

  inline iterator begin()
  {
   return iterator(m_head);
  }

  inline const_iterator begin()const
  {
   return const_iterator((const T*)m_head);
  }

  inline iterator end()
  {
   return iterator(m_head+m_size);
  }

  inline const_iterator end()const
  {
   return const_iterator((const T*)m_head);
  }

  void resize (size_type newsz)
  {
   typedef array_auto_ptr<T, allocator<T, ALLOC> > auto_ptr;
   if(newsz > m_size){
    node_type*   tmp = node_allocator::allocate_n(newsz);
    auto_ptr   ptr(tmp, newsz);
    m_capacity = newsz;
    be_copy(tmp, m_head, m_head + m_size);
    ptr.reset(m_buf, m_capacity);
    m_buf  = tmp;
    m_head  = m_buf;
    m_capacity = newsz;

   }
  }


  iterator insert(iterator pos,size_type sz, const T& x)
  {

   typedef array_auto_ptr<T, allocator<T, ALLOC> > auto_ptr;
   auto_ptr  ptr(0, 0);
   node_type*  head = m_head;
   typename iterator::difference_type distan = be_distance(m_head, pos.m_cur);
   typename iterator::difference_type hdist = 0;
   typename iterator::difference_type tdist = 0;

   assert(*(ptrdiff_t*)&distan >= 0 && *(ptrdiff_t*)&sz >= 0);
   if(m_size + sz > m_capacity){
    size_type   newsz = m_size + m_size + sz;
    node_type*   tmp  = node_allocator::allocate_n(newsz);
    ptr.reset(tmp, newsz);
    head  = tmp + m_size/ 2;
    be_copy(head, distan, m_head);
    ptr.reset(m_buf, m_capacity);
    m_capacity = newsz;
    m_buf  = tmp;
    m_head  = head;
   }
   hdist = be_distance(m_buf, m_head);
   tdist = m_capacity - (hdist + m_size);
   //判断是否是头部预留空间足够，如果头部预留空间足够，且尾部预留空间不够，则尾部不动，头部向前移动
   if(hdist >= (long)sz){
    if(tdist > (long)sz && (distan > (long)m_size - distan)){
     be_copy_backward(m_head + distan + sz, m_head + distan, m_head + m_size);
    }else{
     //头部剩余过大，移动部分到头部,缩小头部剩余空间
     if(hdist - tdist > (long)(m_size + sz) / 4){
      head = m_buf + (hdist + tdist - sz) / 2; 
      be_copy(head, m_head, m_head + distan);
      be_copy(head + distan + sz, m_head + distan, m_head + m_size);
      destroy(head + m_size + sz, m_head + m_size);
      m_head = head;

     }else{
      be_copy(m_head - sz, m_head, m_head + distan);
      m_head -= sz;
     }
    }
   }//如果头部预留空间不够，而尾部足够，则头部不动，尾部向后移动
   else{
    head = m_buf + (hdist + tdist - sz) / 2; 
    if(tdist < (long)sz){
     be_copy(head, m_head, m_head + distan);
     be_copy_backward(head + distan + sz, m_head + distan, m_head + m_size);
     m_head = head;
    }else{
     //尾部剩余过大，移动部分到尾部
     if(tdist - hdist > (long)(m_size + sz) / 4)
     {
      be_copy_backward(head + distan + sz, m_head + distan, m_head + m_size);
      be_copy_backward(head, m_head, m_head + distan);
      destroy(head, m_head);
      m_head = head;
     }else{
      be_copy_backward(m_head + distan + sz, m_head + distan, m_head + m_size); 
     }
    }
   }
   for(size_type i = 0; i < sz; ++i)
    m_head[distan + i] = x;
   m_size += sz;
   return iterator(m_head + distan);
  }

  inline iterator insert(iterator pos, const T& x)
  {
   return insert(pos, size_type(1), x);
  }

  template<class ForwardItor>
   iterator insert(iterator pos, ForwardItor first, ForwardItor last)
  {
   typedef array_auto_ptr<T, allocator<T, ALLOC> > auto_ptr;
   auto_ptr    ptr(0, 0);
   node_type*    head = m_head;
   typename iterator::difference_type distan = be_distance(m_head, pos.m_cur);
   typename iterator::difference_type sz = be_distance(first, last);
   typename iterator::difference_type hdist = 0;
   typename iterator::difference_type tdist = 0;

   assert(*(ptrdiff_t*)&distan >= 0 && *(ptrdiff_t*)&sz >= 0);
   if(m_size + sz > m_capacity){
    size_type   newsz = m_size + m_size + sz;
    node_type*   tmp  = node_allocator::allocate_n(newsz);
    ptr.reset(tmp, newsz);
    head  = tmp + m_size / 2;
    be_copy(head, distan, m_head);
    ptr.reset(m_buf, m_capacity);
    m_capacity = newsz;
    m_buf  = tmp;
    m_head  = head;
   }
   hdist = be_distance(m_buf, m_head);
   tdist = m_capacity - (hdist + m_size);
   //判断是否是头部预留空间足够，如果头部预留空间足够，且尾部预留空间不够，则尾部不动，头部向前移动
   if(hdist >= (long)sz){
    if(tdist > (long)sz && (distan > (long)m_size - distan)){
     be_copy_backward(m_head + distan + sz, m_head + distan, m_head + m_size);
    }else{
     //头部剩余过大，移动部分到头部,缩小头部剩余空间
     if(hdist - tdist > (long)(m_size + sz) / 4){
      head = m_buf + (hdist + tdist - sz) / 2; 
      be_copy(head, m_head, m_head + distan);
      be_copy(head + distan + sz, m_head + distan, m_head + m_size);
      destroy(head + m_size + sz, m_head + m_size);
      m_head = head;

     }else{
      be_copy(m_head - sz, m_head, m_head + distan);
      m_head -= sz;
     }
    }
   }//如果头部预留空间不够，而尾部足够，则头部不动，尾部向后移动
   else{
    head = m_buf + (hdist + tdist - sz) / 2; 
    if(tdist < (long)sz){
     be_copy(head, m_head, m_head + distan);
     be_copy_backward(head + distan + sz, m_head + distan, m_head + m_size);
     m_head = head;
    }else{
     //尾部剩余过大，移动部分到尾部
     if(tdist - hdist > (long)(m_size + sz) / 4)
     {
      be_copy_backward(head + distan + sz, m_head + distan, m_head + m_size);
      be_copy_backward(head, m_head, m_head + distan);
      destroy(head, m_head);
      m_head = head;
     }else{
      be_copy_backward(m_head + distan + sz, m_head + distan, m_head + m_size); 
     }
    }
   }
   be_copy(m_head + distan, first, last);
   m_size += sz;
   return iterator(m_head + distan);
  }

  iterator erase(iterator first,  iterator last)
  {
   typename iterator::difference_type distan = be_distance(m_head, first.m_cur);
   typename iterator::difference_type sz = be_distance(first,  last);

   assert(*(ptrdiff_t*)&distan >= 0 && *(ptrdiff_t*)&sz >= 0);
   if(m_size){
    //删除之后是头部剩余的元素比较多，则保持头部不变，拷贝尾部
    if((long)m_size - (distan + sz) <= (long)distan)
    {
     be_copy(m_head + distan, m_head + distan + sz, m_head + m_size);
     destroy(m_head + m_size - sz, sz);
    }else{

     be_copy_backward(m_head + sz, m_head, m_head + distan);
     destroy(m_head, sz);
     m_head += sz;
    }
   }
   m_size -= sz;
   return iterator(m_head + distan);
  }

  inline reference operator[](ptrdiff_t offset)
  {
   return m_head[offset];
  }

  inline const reference operator[](ptrdiff_t offset)const
  {
   return m_head[offset];
  }

  inline iterator erase(iterator pos) {
   return erase(pos, pos+1);
  }

  inline void  push_back(const T& t)
  {
   insert(end(), t);
  }

  inline void  push_front(const T& t)
  {
   insert(begin(), t);
  }

  inline void pop_front() { 
   if(m_size)
   {
    erase(begin()); 
   }
  }
  inline void pop_back() { 
   if(m_size)
   {
    iterator tmp = end();
    erase(--tmp);
   }
  }

  inline void  clear()
  {
   node_allocator::deallocate(m_head, m_capacity);
   m_size  = 0;
   m_capacity = 0; 
  }

  inline void  swap(vector& other)
  {
   node_type* buf = m_buf;
   node_type* nod = m_head;
   size_type sz = m_size;
   m_buf  = other.m_buf;
   m_head  = other.m_head;
   m_size  = other.m_size;
   other.m_buf = buf;
   other.m_head= nod;
   other.m_size= sz;
  }

 private:
  node_type* m_buf;
  node_type* m_head;
  size_type m_size;
  size_type m_capacity;
 };

#ifdef _WIN32
#pragma warning(pop) 
#endif

};
#endif/*BE_VECTOR1_H*/
