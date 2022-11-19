////////////////////////////////////////////////////////////////////////////////
// 
// RegularityCheck.cc
//
//    produced: 2001/10/28 jr
// last change: 2001/10/28 jr
//
////////////////////////////////////////////////////////////////////////////////

#include <set>
#include <unordered_map>

#include "RegularityCheck.hh"

namespace topcom {

#ifdef REGULARITY_CACHE
  ConstraintCache  RegularityCheck::_cache;
#endif

  std::mutex RegularityCheck::_reg_mutex;
  size_type  RegularityCheck::cnt_calls = 0;


  // constructors:

  RegularityCheck::RegularityCheck(const PointConfiguration& points,
				   const Chirotope& chiro,
				   const Incidences& incidences, 
				   const SimplicialComplex& t) :
    _heights(points.no()),
    _coeffs(),
    _pointsptr(&points),
    _chiroptr (&chiro),
    _triangptr(&t),
    _incidencesptr(&incidences) {

    static const Vector zerovec(_chiroptr->no());
  
    // // keep a vector of matrices:
    // std::vector<StairCaseMatrix> matrices(_pointsptr->rank());

    parameter_type no   = _chiroptr->no();
    parameter_type rank = _chiroptr->rank();
    
    // we build the coefficient matrix from the chirotope and the triangulation t:
    SimplicialComplex intfacets;
    std::unordered_set<Simplex, Hash<Simplex> > unionsimps;

    // we ignore duplicates (disabled):
    size_type cnt_duplicates(0UL);

    // for all simplices in the triangulation t collect interior facets:
    for (SimplicialComplex::const_iterator iter = t.begin();
	 iter != t.end();
	 ++iter) {
      intfacets += _incidencesptr->intfacets(*iter);
    }
    size_type intfacets_cnt = intfacets.card();
  
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << intfacets_cnt << " interior facets in triangulation " << t << std::endl;
    }

    // prepare a matrix with a column for each interior facet
    // (some may be irrelevant, but at this point we do not care):
    _coeffs.resize(intfacets_cnt, zerovec);

    parameter_type col_cnt = 0;
  
    // for all interior facets ...
    for (SimplicialComplex::const_iterator iter = intfacets.begin();
	 iter != intfacets.end();
	 ++iter) {
      const Simplex intfacet(*iter);
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "processing interior facet " << intfacet << " ..." << std::endl;
      }

      // find the two simplices in t (or fewer, if t is partial) containing intfacet:
      const SimplicialComplex simppair(t * _incidencesptr->intcofacets(intfacet));
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "simplices in partial triangulation containing it are " << simppair << std::endl;
      }
      if (simppair.card() < 2) {
	// intfacet is not interior in t, thus no constraint:
	continue;
      }
      SimplicialComplex::const_iterator pairiter(simppair.begin());
      const Simplex simp1(*pairiter);
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "simp1 = " << simp1 << std::endl;
      }
      ++pairiter;
      const Simplex simp2(*pairiter);
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "simp2 = " << simp2 << std::endl;
      }
      const Simplex unionsimp(simp1 + simp2);
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "simp1 union simp2 = " << unionsimp << std::endl;
      }
      if (unionsimps.find(unionsimp) != unionsimps.end()) {
	// constraints on identical rank + 1 subsets will be identical:
	continue;
      }
      unionsimps.insert(unionsimp);
#ifdef REGULARITY_CACHE
      const ConstraintCacheEntry constraint_config(unionsimp, intfacet);
#endif

      // grab reference to the right column in resulting matrix:
      Vector& new_col = _coeffs[col_cnt];
      ++col_cnt;

#ifdef REGULARITY_CACHE
      // check for cached values:
      {
	std::lock_guard<std::mutex> lock(_reg_mutex);     
	ConstraintCache::const_iterator find_iter = _cache.find(constraint_config);
	if (find_iter != _cache.end()) {
	  new_col = find_iter->second;
	  continue;
	}
      }
#endif
    
      // compute the index not in simp1:
      const Simplex simpdiff = simp2 - intfacet;
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "vertex not in interior facet: " << simpdiff << std::endl;
      }
      if (simpdiff.empty()) {
	continue;
      }
      const parameter_type new_index = *simpdiff.begin();

      // simp1 is the calibration simplex --
      // if point new_index receives infinite height, then new_index is folded above aff(simp1):
      int calibrate = (*_chiroptr)(simp1);

      // determinants can be taken from the chirotope, since
      // RegularityCheck is only called when there are points available:
      new_col[new_index] = calibrate * _chiroptr->det(simp1);
      calibrate = -calibrate;

      // build a permutation out of simp1, i.e., at first, new_index is missing:
      Permutation simp1_perm(no, rank, simp1);

      // the remaining coefficients now receive alternating signs,
      // when the points i missing in new_index union simp1 minus i are traversed in increasing order;
      // the first missing element is the min of simp1:
      Simplex::const_iterator simp1_iter = simp1.begin();


      // count iterations:
      int cnt = 0;

      // overwrite first element of simp1_perm with new_index,
      // then the min of simp1 is missing:
      simp1_perm[cnt] = new_index;

      // iterate over further missing elements:
      while (true) {

	// fetch determinant from chirotope structure:
	new_col[*simp1_iter] = calibrate * _chiroptr->det(simp1_perm);
	calibrate = -calibrate;

	// check if we are done already:
	if (++cnt == rank) {
	  break;
	}
      
	// exchange the permutation element at cnt with the currently missing element:
	simp1_perm[cnt] = *simp1_iter;

	// increment iterator:
	++simp1_iter;
      }
#ifdef REGULARITY_CACHE
      {
	// cache the computational result:
	std::lock_guard<std::mutex> lock(_reg_mutex);     
	_cache[constraint_config] = new_col;
      }
#endif
    
      //////////////////////////////////////////////////////////////////////////////
      // obsolete section (matrix computations can now be avoided altogether):
      //////////////////////////////////////////////////////////////////////////////
      // // build the matrix of all points corresponding to the folding constraint
      // // for the internal ridge:
      // StairCaseMatrix basis_matrix;
      // const Matrix raw_basis_matrix(*_pointsptr, simp1);
      // basis_matrix.augment(raw_basis_matrix);
      // // for (basis_type::const_iterator basis_iter = simp1.begin();
      // // 	 basis_iter !=  simp1.end();
      // // 	 ++basis_iter) {
      // //   basis_matrix.augment((*_pointsptr)[*basis_iter]);
      // // }
      // const parameter_type new_index = *((simp2 - intfacet).begin());

      // // the orientation of simp1 is the calibration according to a point with
      // // infinite height:
      // Field det_basis = basis_matrix.det();
      // int calibrate(sign(det_basis));
      // new_col[new_index] = calibrate * det_basis;
    
      // calibrate = -calibrate;
    
      // // build the matrices with the point not in simp1 and the facets of simp1
      // // matrix[i] is built from (new_index union simp1 minus i):
      // int cnt(0);
      // for (int i = 0; i < matrices.size(); ++i) {
      //   matrices[i] = StairCaseMatrix((*_pointsptr)[new_index]);
      // }
      // for (Simplex::const_iterator simp_iter = simp1.begin();
      // 	 simp_iter != simp1.end();
      // 	 ++simp_iter) {
      //   for (int i = 0; i < cnt; ++i) {
      // 	matrices[i].augment((*_pointsptr)[*simp_iter]);
      //   }
      //   if (cnt + 1 < matrices.size()) {
      // 	matrices[cnt + 1].augment((*_pointsptr)[*simp_iter]);
      // 	for (int i = cnt + 2; i < matrices.size(); ++i) {
      // 	  matrices[i] = matrices[cnt + 1];
      // 	}
      //   }
      //   ++cnt;
      // }
      //
      // // collect the results:
      // cnt = 0;
      // for (Simplex::const_iterator simp_iter = simp1.begin();
      // 	 simp_iter != simp1.end();
      // 	 ++simp_iter) {    
      //   new_col.at(*simp_iter) = calibrate * matrices.at(cnt).det();
      //   calibrate = -calibrate;
      //   ++cnt;
      // }
      //////////////////////////////////////////////////////////////////////////////
      // end obsolete section
      //////////////////////////////////////////////////////////////////////////////
      
      //////////////////////////////////////////////////////////////////////////////
      // another obsolete section
      //////////////////////////////////////////////////////////////////////////////
      // // extract the matrix of simp1 and consider it to the right of column new_index:
      // Matrix after_gap_matrix(*_pointsptr, simp1);

      // // compute det with the help of a StairCaseMatrix:
      // StairCaseMatrix comp_matrix;
      // comp_matrix.augment(after_gap_matrix);
      // Field det_simp1 = comp_matrix.det();
      // int calibrate = sign(det_simp1);

      // // calibrate the sign of simp1 to "plus":
      // new_col[new_index] = calibrate * det_simp1;
      // calibrate = -calibrate;

      // // all other matrice start with column "new_index":
      // StairCaseMatrix before_gap_matrix((*_pointsptr)[new_index]);

      // // the matrices to the right of the missing column start with the calibration matrix:
      // IntegerSet ignored_cols;
      // parameter_type cnt = 0;
      // for (Simplex::const_iterator simp_iter = simp1.begin();
      // 	 simp_iter != simp1.end();
      // 	 ++simp_iter) {

      //   // ignore the next column from the matrix after the missing column:
      //   ignored_cols += cnt;
      //   ++cnt;
      
      //   // reuse the eliminations performed on the part left to the missing column:
      //   comp_matrix = before_gap_matrix;
      //   comp_matrix.augment(after_gap_matrix, ignored_cols);

      //   // complete and eliminate by the matrix right of the missing column and store its determinant:
      //   new_col[*simp_iter] = calibrate * comp_matrix.det();

      //   if (cnt == rank) {
      // 	break;
      //   }
      //   calibrate = -calibrate;
      
      //   // add the missing column to generate the matrix left of the next missing column and eliminate:
      //   before_gap_matrix.augment((*_pointsptr)[*simp_iter]);
      // }
      //////////////////////////////////////////////////////////////////////////////
      // end obsolete section
      //////////////////////////////////////////////////////////////////////////////
        
      if (CommandlineOptions::debug()) {

	// check the result:
	Matrix simp1_matrix(*_pointsptr, simp1);
	Field det_simp1 = simp1_matrix.det();

	Vector check_col(_chiroptr->no());
	int calibrate = sign(det_simp1);
	const parameter_type new_index = *(simp2 - intfacet).begin();
	check_col[new_index] = new_col[new_index];
	calibrate = -calibrate;
      
	basis_type basis(simp1);
	for (Simplex::const_iterator simp_iter = simp1.begin();
	     simp_iter != simp1.end();
	     ++simp_iter) {
	  const parameter_type idx(*simp_iter);
	  basis -= idx;
	  StairCaseMatrix basis_matrix;
	  basis_matrix.push_back((*_pointsptr)[new_index]);	  
	  for (basis_type::const_iterator basis_iter = basis.begin();
	       basis_iter !=  basis.end();
	       ++basis_iter) {
	    basis_matrix.augment((*_pointsptr)[*basis_iter]);
	  }
	  check_col[idx] = calibrate * basis_matrix.det();
	  calibrate = -calibrate;
	  basis += idx;
	}
      
	if (new_col != check_col) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "RegularityCheck::RegularityCheck(...):"
		    << " constraint coefficient vector inconsistent with check:" << std::endl;
	  std::cerr << "simp1    : " << simp1 << std::endl;
	  std::cerr << "new_index: " << new_index << std::endl;
	  std::cerr << "fast     : " << new_col << std::endl;
	  std::cerr << "old      : " << check_col << std::endl;
	  std::cerr << "exiting" << std::endl;
	  exit(1);
	}
      }

      // redundancy (which can only occur in the case of coplanar points) need not be removed here;
      // redundancy removal is faster in the LP solver;
      // thus, simply push back the vector:
      // _coeffs.push_back(new_col);
    
      //////////////////////////////////////////////////////////////////////////////
      // another obsolete section
      //////////////////////////////////////////////////////////////////////////////
      // // finally, push_back the new column if it is new;
      // // it corresponds to 
      // // local regularity at an internal facet
      // // (we take a column for efficiency reasons,
      // // in the LP solver it will be a row, of course):
      // if (CommandlineOptions::debug()) {
      // std::lock_guard<std::mutex> lock(IO_sync::mutex);
      //   std::cerr << "checking novelty of new_col from interior facet "
      // 		<< intfacet << ": " << new_col << std::endl;
      // }
      // if (!new_col.is_zero() && (old_columns.find(new_col) == old_columns.end())) {
      //   _coeffs.push_back(new_col);
      //   old_columns.insert(new_col);
      // }
      // else {
      //   ++cnt_duplicates;
      // }
      //////////////////////////////////////////////////////////////////////////////
      // end obsolete section
      //////////////////////////////////////////////////////////////////////////////
    }

    // remove trailing zero-columns that were not needed:
    _coeffs.resize(col_cnt);

    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << _coeffs.size() << " columns found" << std::endl;
      // std::cerr << cnt_duplicates << " duplicate columns found and ignored" << std::endl;
      std::cerr << "the coefficient matrix for regularity check:" << std::endl;
      if (_coeffs.coldim() == 0) {
	std::cerr << "no constraints." << std::endl;
      }
      else {
	_coeffs.transpose().pretty_print(std::cout);
      }
    }
  }

}; // namespace topcom

// eof RegularityCheck.cc
