////////////////////////////////////////////////////////////////////////////////
// 
// RefCount.hh (completely deprecated since not thread-safe)
//
//    produced: 05/02/98 jr
// last change: 05/02/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef REFCOUNT_HH
#define REFCOUNT_HH

#include <memory>
#include <atomic>

namespace topcom {

  // reference counter must be atomic for multi-threading:
  typedef std::atomic<unsigned long> refcount_type;
  // typedef unsigned long              refcount_type;

  class RefCount {
  private:
    refcount_type _refcount;
  public:
    inline void pick_reference();
    inline void drop_reference();
    inline RefCount();
    inline RefCount(const RefCount&);
    inline ~RefCount();
    inline const refcount_type& refcount() const;
  };

  inline void RefCount::pick_reference() {
    ++_refcount;
#ifdef REFCOUNT_DEBUG
    std::cout << "pick_reference(): refcount = " << _refcount
	      << " to address " << this << std::endl;
#endif
  }
  inline void RefCount::drop_reference() {
    --_refcount;
#ifdef REFCOUNT_DEBUG
    std::cout << "drop_reference(): refcount = " << _refcount
	      << " to address " << this << std::endl;
#endif
  }
  inline RefCount::RefCount() : _refcount(0) {}
  inline RefCount::RefCount(const RefCount&) : _refcount(0) {}
  inline RefCount::~RefCount() {}
  inline const refcount_type& RefCount::refcount() const {
    return _refcount;
  }

  template <class T>
  class SmartPtr {
  private:
    class RefCountData : public RefCount {
    public:
      T                   _data;
    public:
      RefCountData() : RefCount(), _data() {}
      RefCountData(const RefCountData& rcd) : RefCount(rcd), _data(rcd._data) {}
      RefCountData(const T& obj) : RefCount(), _data(obj) {}
      ~RefCountData() {}
    };
  private:
    //   typedef std::simple_alloc<RefCountData, std::alloc> data_allocator;
    std::allocator<RefCountData> data_allocator;
  private:
    RefCountData* _bufptr;
  private:
    RefCountData* _ptr;
  public:
    // constructors:
    inline SmartPtr();
    inline SmartPtr(const SmartPtr&);
    inline SmartPtr(const T&);
    // destructor:
    inline ~SmartPtr();
    // assignment:
    inline SmartPtr& operator=(const SmartPtr&);
    // static nullpointer:
    inline static SmartPtr& smartnullptr();
    // casts:
    inline operator bool() const;
    // accessors:
    inline const refcount_type refcount() const;
    // operators:
    inline const T* operator->() const;
    inline T* operator->();
    inline const T& operator*() const;
    inline T& operator*();
    inline const bool operator==(const SmartPtr& p) const;
    inline const bool operator!=(const SmartPtr& p) const;
  };

  // constructors:
  template <class T>
  inline SmartPtr<T>::SmartPtr() : _ptr(NULL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SmartPtr<T>::SmartPtr()" << std::endl;
#endif
  }

  template <class T>
  inline SmartPtr<T>::SmartPtr(const SmartPtr& p) : _ptr(p._ptr) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SmartPtr<T>::SmartPtr(const SmartPtr&)" << std::endl;
#endif
    if (_ptr) {
      _ptr->pick_reference();
    }
  }

  template <class T>
  inline SmartPtr<T>::SmartPtr(const T& obj) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SmartPtr<T>::SmartPtr(const T&)" << std::endl;
#endif
    _ptr = data_allocator.allocate(1);
    std::allocator_traits<std::allocator<RefCountData> >::construct(data_allocator, _ptr, obj);
    _ptr->pick_reference();
  }

  // destructor:
  template <class T>
  inline SmartPtr<T>::~SmartPtr() {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SmartPtr<T>::~SmartPtr()" << std::endl;
#endif
    if (_ptr) {
      if (_ptr->refcount() == 1) {
#ifdef REFCOUNT_DEBUG
	std::cout << "DESTROY " << _ptr->_data << std::endl;
#endif
	std::allocator_traits<std::allocator<RefCountData> >::destroy(data_allocator, _ptr);
	data_allocator.deallocate(_ptr, 1);
	_ptr = NULL;
      }
      else {
	_ptr->drop_reference();
      }
    }
  }

  // assignment:
  template <class T>
  inline SmartPtr<T>& SmartPtr<T>::operator=(const SmartPtr& p) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SmartPtr<T>::operator=(const SmartPtr&)" << std::endl;
#endif
    if (_ptr) {
      if (_ptr->refcount() == 1) {
	std::allocator_traits<std::allocator<RefCountData> >::destroy(data_allocator, _ptr);
	data_allocator.deallocate(_ptr, 1);
	_ptr = NULL;
      }
      else {	
	_ptr->drop_reference();
      }
    }
    _ptr = p._ptr;
    if (_ptr) {
      _ptr->pick_reference();
    }
    return *this;
  }

  // casts:
  template <class T>
  inline SmartPtr<T>::operator bool() const {
    return (_ptr != NULL);
  }

  // accessors:
  template <class T>
  inline const refcount_type SmartPtr<T>::refcount() const {
    return _ptr->refcount();
  }

  // static nullpointer:
  template <class T>
  inline SmartPtr<T>& SmartPtr<T>::smartnullptr() {
    static SmartPtr<T> smartnullptr;
    return smartnullptr;
  }

  // operators:
  template <class T>
  inline const T* SmartPtr<T>::operator->() const {
    return &_ptr->_data;
  }

  template <class T>
  inline T* SmartPtr<T>::operator->() {
    if (_ptr) {
      if (_ptr->refcount() > 1) {
#ifdef DEBUG
	std::cout << "T* SmartPtr<T>::operator->(): copy for overwrite" << std::endl;
#endif
	_ptr->drop_reference();
	_bufptr = _ptr;
	_ptr = data_allocator.allocate(1);
	std::allocator_traits<std::allocator<RefCountData> >::construct(data_allocator, _ptr, *_bufptr);
	_ptr->pick_reference();
      }
    }
    return &_ptr->_data;
  }

  template <class T>
  inline const T& SmartPtr<T>::operator*() const {
    return std::ref(_ptr->_data);
  }

  template <class T>
  inline T& SmartPtr<T>::operator*() {
    if (_ptr) {
      if (_ptr->refcount() > 1) {
#ifdef DEBUG
	std::cout << "T& SmartPtr<T>::operator*(): copy for overwrite" << std::endl;
#endif
	_ptr->drop_reference();
	_bufptr = _ptr;
	_ptr = data_allocator.allocate(1);
	std::allocator_traits<std::allocator<RefCountData> >::construct(data_allocator, _ptr, *_bufptr);
	_ptr->pick_reference();
      }
    }
    return std::ref(_ptr->_data);
  }

  template <class T>
  inline const bool SmartPtr<T>::operator==(const SmartPtr<T>& p) const {
    return (_ptr == p._ptr);
  }

  template <class T>
  inline const bool SmartPtr<T>::operator!=(const SmartPtr<T>& p) const {
    return (!(*this == p));
  }

}; // namespace topcom

#endif

// eof RefCount.hh
