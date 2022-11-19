////////////////////////////////////////////////////////////////////////////////
// 
// IndexTableArray.hh 
//
//    produced: 29/05/98 jr
// last change: 29/05/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef INDEXTABLEARRAY_HH
#define INDEXTABLEARRAY_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <mutex>

#include "CommandlineOptions.hh"
#include "LabelSet.hh"

#include "PlainHashIndexTable.hh"

namespace topcom {

  typedef PlainHashIndexTable<LabelSet> IndexTable;

  // #include "PlainArray.hh"
  // typedef PlainArray<IndexTable> _IndexTableArray;
#include <vector>
  typedef std::vector<IndexTable> _index_table_array_type;


  typedef LabelSet Simplex;

  class IndexTableArray : public _index_table_array_type {
  protected:
    // to guard the static table, we use a mutex:
    mutable std::mutex _index_table_mutex;
    bool _preprocessed;
    bool _inside_multithreading;
  public:
    inline IndexTableArray() :
      _preprocessed(false),
      _inside_multithreading(false) {}
    inline IndexTableArray(const IndexTableArray& ita) :
      _index_table_array_type(ita),
      _preprocessed(ita._preprocessed),
      _inside_multithreading(ita._inside_multithreading) {}
    inline const size_type resize(const size_type new_size) {
#ifdef DEBUG
      {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "resize of index table requested "
		  << "(old size = " << size()
		  << ", new size = " << new_size
		  << ") ..." << std::endl;
      }
#endif
      if (_preprocessed) {
	if (new_size > size()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "IndexTableArray has *** not *** been preprocessed properly:" << std::endl;
	  std::cerr << "effective resize of index table requested "
		    << "(old size = " << size()
		    << ", new size = " << new_size
		    << ") - exiting" << std::endl;	
	  exit(1);
	}
	else {
#ifdef DEBUG
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "no resize necessary ..." << std::endl;
#endif
	}
      }
      else {
	if (new_size > size()) {
	  if (_inside_multithreading) {
#ifdef DEBUG
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "locked resize ..." << std::endl;
#endif
	    std::lock_guard<std::mutex> index_table_guard(_index_table_mutex);
	    _index_table_array_type::resize(new_size);
	  }
	  else {
#ifdef DEBUG
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "unlocked resize ..." << std::endl;
#endif
	    _index_table_array_type::resize(new_size);
	  }
	  for (size_type i = 0; i < size(); ++i) {
	    (*this)[i]._inside_multithreading = _inside_multithreading;
	  }
	}
	else {
#ifdef DEBUG
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "no resize necessary ..." << std::endl;
#endif
	}
      }
#ifdef DEBUG
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done" << std::endl;
#endif
      return _index_table_array_type::size();
    }
    inline size_type cardsize(const size_type card) const {
      return (*this)[card].size();
    }
    inline IndexTable::const_iterator begin(const size_type card) const {
      return (*this)[card].begin();
    }
    inline IndexTable::const_iterator end(const size_type card) const {
      return (*this)[card].end();
    }
    inline IndexTable::const_iterator find(const size_type card, const LabelSet& is) const {
      return (*this)[card].find(is);
    }
    inline const bool preprocessed() {
      return _preprocessed;
    }
    inline const bool preprocessed(const size_type card) {
      return (*this)[card].preprocessed();
    }
    inline void set_preprocessed(const bool preprocessed) {
      _preprocessed = preprocessed;
      for (size_type i = 0; i < _index_table_array_type::size(); ++i) {
	set_preprocessed(i, preprocessed);
      }
    }
    inline void set_preprocessed(const size_type card, const bool preprocessed) {
      (*this)[card].set_preprocessed(preprocessed);
    }
    inline void start_multithreading(const size_type card) {
      (*this)[card].start_multithreading();
    }
    inline void stop_multithreading(const size_type card) {
      (*this)[card].stop_multithreading();
    }
    inline void start_multithreading() {
      _inside_multithreading = true;
      for (size_type i = 0; i < size(); ++i) {
	start_multithreading(i);
      }
    }
    inline void stop_multithreading() {
      for (size_type i = 0; i < size(); ++i) {
	stop_multithreading(i);
      }
      _inside_multithreading = false;
    }
    inline const LabelSet& get_obj(const size_type card, const size_type index) const {

      // we lock this on one lower level:
      return (*this)[card].get_obj(index);
    }
    inline size_type get_index(const size_type card, const LabelSet& is) {
      if (!_preprocessed && (card + 1 > this->size())) {

	// resize is thread-safe because it locks internally:
	resize(card + 1);
      }
#ifdef DEBUG
      std::cerr << "inserting into " << (*this)[card] << " with card " << card << " ..." << std::endl;
#endif
      
      // we lock this on one lower level:
      return (*this)[card].get_index(is);
    }
    inline size_type get_index(const LabelSet& is) {
      return get_index(is.card(), is);
    }
  };

}; // namespace topcom

#endif

// eof IndexTableArray.hh
