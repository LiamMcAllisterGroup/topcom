//////////////////////////////////////////////////////////////////////////
// SymmetricFlipGraph.hh
// produced: 29/06/98 jr
// last change: 29/06/98 jr
/////////////////////////////////////////////////////////////////////////
#ifndef SYMMETRICFLIPGRAPH_HH
#define SYMMETRICFLIPGRAPH_HH

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <assert.h>

#include "ContainerIO.hh"
#include "LabelSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "Incidences.hh"
#include "Volumes.hh"
#include "Flip.hh"
#include "MarkedFlips.hh"
#include "TriangNode.hh"
#include "TriangFlips.hh"
#include "Symmetry.hh"
#include "RegularityCheck.hh"

#ifndef STL_CONTAINERS
#include "HashKey.hh"
#include "HashSet.hh"
namespace topcom {
  typedef HashSet<SimplicialComplex>                triang_container_type;
  typedef HashSet<TriangNode>                       orbit_type;
  typedef HashMap<TriangNode, TriangFlips>          tnode_check_type; // Node->Flips
  typedef HashMap<TriangNode, TriangFlips>          tnode_container_type; // Node->Flips
  typedef HashSet<Vector>                           gkz_container_type;
  typedef HashMap<TriangNode, symmetryptr_datapair> stabilizer_container_type;
};
#else
#include <unordered_set>
#include <unordered_map>
namespace topcom {
  typedef std::unordered_set<SimplicialComplex, Hash<SimplicialComplex> >         triang_container_type;
  typedef std::unordered_set<TriangNode, Hash<TriangNode> >                       orbit_type;
  typedef std::map<TriangNode, TriangFlips>                                       tnode_check_type;
  typedef std::unordered_map<TriangNode, TriangFlips, Hash<TriangNode> >          tnode_container_type; // Node->Flips
  typedef std::unordered_set<Vector, Hash<Vector> >                               gkz_container_type;
  typedef std::unordered_map<TriangNode, symmetryptr_datapair, Hash<TriangNode> > stabilizer_container_type;
};
#endif // of #ifndef STL_CONTAINERS

namespace topcom {

  // the following container holds for each symmetry its representation on elements
  // together with its representation on rank-subsets:
  typedef std::vector<std::pair<Symmetry, Symmetry> >                         simpidx_symmetries_type;
  typedef std::vector<simpidx_symmetries_type>                                worker_simpidx_symmetries_type;

  class __sbfs_cout_triang_base {
  public:
    inline virtual void operator()(const size_type no,
				   const TriangNode& tn) const {}
  };

  class __sbfs_cout_triang : public __sbfs_cout_triang_base {
  public:
    inline virtual void operator()(const size_type no,
				   const TriangNode& tn) const {
      std::cout << "T[" << tn.ID() << "] := ";
      std::cout << (SimplicialComplex)tn << ";" << std::endl;
    }
  };

  class __sbfs_output_pred_base {
  public:
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      return true;
    }
    inline virtual bool is_invariant() const {
      return true;
    }
  };

  class __sbfs_output_checkno : public __sbfs_output_pred_base {
  public:
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      return (tn.card() == CommandlineOptions::no_of_simplices());
    }
    inline virtual bool is_invariant() const {
      return true;
    }
  };

  class __sbfs_output_checkmaxno : public __sbfs_output_pred_base {
  public:
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      return (tn.card() <= CommandlineOptions::max_no_of_simplices());
    }
    inline virtual bool is_invariant() const {
      return true;
    }
  };

  class __sbfs_output_checknonregularity : public __sbfs_output_pred_base {
  public:
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      assert (points.no() == chiro.no());
      return !RegularityCheck(points, chiro, incidences, tn).is_regular();
    }
    inline virtual bool is_invariant() const {
      return CommandlineOptions::symmetries_are_affine();
    }
  };

  class __sbfs_output_checkunimodularity : public __sbfs_output_pred_base {
  private:
    Field _points_vol;
    Field _min_vol;
  public:
    inline __sbfs_output_checkunimodularity(const PointConfiguration& points,
					    const Chirotope&          chiro) :
      _points_vol(points.volume()),
      _min_vol(std::numeric_limits<int>::max()) {
      Permutation perm(points.no(), points.rank());
      do {
	Simplex simp(perm);
	if (chiro(simp) != FieldConstants::ZERO) {
	  const Field simp_vol(points.volume(simp));
	  if (simp_vol < _min_vol) {
	    _min_vol = simp_vol;
	  }
	}
      } while (perm.lexnext());
    }
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      return (tn.card()  * _min_vol == _points_vol);
    }
    inline virtual bool is_invariant() const {
      return CommandlineOptions::symmetries_are_isometric();
    }
  };

  class __sbfs_search_pred_base {
  public:
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      return true;
    }
    inline virtual bool is_invariant() const {
      return true;
    }
  };

  class __sbfs_search_checkreg : public __sbfs_search_pred_base {
  public:
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      assert (points.no() == chiro.no());
      Vector heights(points.no());
      RegularityCheck regchecker(points, chiro, incidences, tn);
      bool regular = regchecker.is_regular();
      if (regular) {
	if (CommandlineOptions::output_heights()) {
	  std::cout << "h[" << (SimplicialComplex)tn << "] := " << regchecker.heights() << ";\n";
	}
	return true;
      }
      else {
	return false;
      }
    }
    inline virtual bool is_invariant() const {
      return CommandlineOptions::symmetries_are_affine();
    }
  };

  class __sbfs_search_seedsupportonly : public __sbfs_search_pred_base {
  private:
    LabelSet _seed_support;
  public:
    inline __sbfs_search_seedsupportonly(const LabelSet& seed_support) : 
      _seed_support(seed_support) {}
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      return (tn.support() == _seed_support);
    }
    inline virtual bool is_invariant() const {
      return false;
    }
  };

  class __sbfs_search_seedsupportandregular : public __sbfs_search_pred_base {
  private:
    LabelSet _seed_support;
  public:
    inline __sbfs_search_seedsupportandregular(const LabelSet& seed_support) : 
      _seed_support(seed_support) {}
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      assert (points.no() == chiro.no());
      Vector heights(points.no());
      RegularityCheck regchecker(points, chiro, incidences, tn);
      bool regular = regchecker.is_regular();
      if (regular) {
	if (CommandlineOptions::output_heights()) {
	  std::cout << "h[" << (SimplicialComplex)tn << "] := " << regchecker.heights() << ";\n";
	}
	return (tn.support() == _seed_support);
      }
      else {
	return false;
      }
    }
    inline virtual bool is_invariant() const {
      return false;
    }
  };

  class __sbfs_search_sometimes : public __sbfs_search_pred_base {
  public:
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {

      static long count(0);
    
      if (CommandlineOptions::debug()) {
	std::cerr << "checking regularity? ";
      }
      if (count++ == CommandlineOptions::sometimes_frequency()) {
	count = 0;
	if (CommandlineOptions::debug()) {
	  std::cerr << "yes." << std::endl;
	}
	if (CommandlineOptions::debug()) {
	  std::cerr << "checking regularity ..." << std::endl;
	}
	const bool found_regular = RegularityCheck(points, chiro, incidences, tn).is_regular();
	if (found_regular) {
	  std::cerr << "regular triangulation found in component:" << std::endl;
	  std::cerr << "triangulation: " << std::endl;
	  std::cerr << tn << std::endl;
	  exit(1);
	}
	else if (CommandlineOptions::debug()) {
	  std::cerr << "not regular --- done." << std::endl;
	}
	return true;
      }
      else {
	if (CommandlineOptions::debug()) {
	  std::cerr << " not yet." << std::endl;
	}
	return true;
      }
    }
    inline virtual bool is_invariant() const {
      return CommandlineOptions::symmetries_are_affine();
    }
  };

  class __sbfs_search_min_no_of_verts : public __sbfs_search_pred_base {
  private: 
    mutable size_type _min_no_of_verts;
  public:

    // constructor:
    inline __sbfs_search_min_no_of_verts(const size_type min_no_of_verts) :
      _min_no_of_verts(min_no_of_verts) {}

    // function:
    inline virtual bool operator()(const PointConfiguration& points,
				   const Chirotope&          chiro,
				   const Incidences&         incidences,
				   const TriangNode&         tn) const {
      const size_type tn_support_card(tn.support().card());
      if (tn_support_card < _min_no_of_verts) {
	_min_no_of_verts = tn_support_card;
	if (CommandlineOptions::verbose()) {
	  std::cerr << "found triangulation with one vertex removed:" << std::endl;
	  std::cerr << tn << std::endl;
	  std::cerr << "support:" << std::endl;
	  std::cerr << tn.support() << std::endl;
	  std::cerr << "number of vertices:" << std::endl;
	  std::cerr << tn_support_card << std::endl;
	}
	return true;
      }
      else {
	if (tn_support_card > _min_no_of_verts) {
	  return false;
	}
	else {
	  return true;
	}
      }
    }
    inline virtual bool is_invariant() const {
      return true;
    }
  };


  class SymmetricFlipGraph {
  private:
    typedef __sbfs_cout_triang_base             cout_triang_base_type;
    typedef __sbfs_cout_triang                  cout_triang_type;
    typedef __sbfs_output_pred_base             output_pred_base_type;
    typedef __sbfs_output_checkno               output_checkno_type;
    typedef __sbfs_output_checkmaxno            output_checkmaxno_type;
    typedef __sbfs_output_checknonregularity    output_checknonregularity_type;
    typedef __sbfs_output_checkunimodularity    output_checkunimodularity_type;
    typedef __sbfs_search_pred_base             search_pred_base_type;
    typedef __sbfs_search_checkreg              search_checkreg_type;
    typedef __sbfs_search_sometimes             search_checksometimes_type;
    typedef __sbfs_search_seedsupportonly       search_seedsupportonly_type;
    typedef __sbfs_search_seedsupportandregular search_seedsupportandregular_type;
    typedef __sbfs_search_min_no_of_verts       search_min_no_of_verts_type;
  public:
    class SymmetryWorker {
    private:
      const int                        _workerID;
      SymmetricFlipGraph*              _callerptr;
      simpidx_symmetries_type*         _worker_symmetriesptr;  // need updated simpidx symmetries
      TriangNode                       _tn;
      TriangNode                       _equivalent_tnode;
      Vector                           _gkz;
      Vector                           _equivalent_gkz;
      enum State {
	idle,
	hired_for_find_old_symmetry_class,
	hired_for_build_orbit_with_searchpred,
	done,
	stopped
      };
      mutable State                    _state;
    public:
      mutable std::condition_variable  worker_condition;
    private:
      SymmetryWorker() = delete;
    public:
      SymmetryWorker(SymmetryWorker&& sw);
      SymmetryWorker(const int workerID,
		     SymmetricFlipGraph&,
		     simpidx_symmetries_type&);
      void find_old_symmetry_class();
      void build_orbit_with_searchpred() const;

      // interface with caller:
      void pass_work_for_find_old_symmetry_class(const TriangNode&, const Vector&);
      void pass_work_for_build_orbit_with_searchpred(const TriangNode&);
      void stop_worker();
    private:
      bool _wake_up() const;
    public:
      void operator()();
    };
    friend class SymmetricFlipGraph::SymmetryWorker;
#ifdef CHECK_NEW
  private:
    tnode_container_type            _all_triangs;
#endif
  private:
    const parameter_type            _no;
    const parameter_type            _rank;
    const PointConfiguration*       _pointsptr;
    const Chirotope*                _chiroptr;
    const Incidences*               _inctableptr;
    const Volumes*                  _voltableptr;
    const SymmetryGroup             _symmetries;
    SimplicialComplex               _seed;
    symmetryptr_datapair            _node_symmetryptrs;
    simpidx_symmetries_type         _simpidx_symmetries;
    tnode_container_type            _previous_triangs;
    tnode_container_type            _new_triangs;
    Vector                          _current_gkz;
    Vector                          _new_gkz;
    Vector                          _equivalent_gkz;
    gkz_container_type              _known_gkzs;
    stabilizer_container_type       _stabilizers;
    tnode_container_type::iterator  _find_iter;
    size_type                       _totalcount;
    size_type                       _symcount;
    size_type                       _reportcount;
    size_type                       _flipcount;
  private:
    TriangNode                      _representative;
    Symmetry                        _transformation;
    size_type                       _orbitsize;
    orbit_type                      _orbit;
    bool                            _rep_has_search_pred;
  private:
    cout_triang_base_type*          _cout_triang_ptr;
    output_pred_base_type*          _output_pred_ptr;
    search_pred_base_type*          _search_pred_ptr;
    bool                            _print_triangs;
    bool                            _only_fine_triangs;
    bool                            _only_unimodular_triangs;
  private:
    size_type                       _processed_count;
    int                             _dump_no;
    std::fstream                    _dump_str;
  private:
    // multi-threading:
    int                             _no_of_threads;
    std::vector<std::thread>        _threads;
    std::mutex                      _old_symmetry_class_pushresults_mutex;
    std::mutex                      _searchpred_pushresults_mutex;
    int                             _no_of_busy_threads;
    int                             _no_of_waiting_threads;
    mutable std::mutex              _main_mutex;
    mutable std::condition_variable _main_condition;

    int                             _location_of_old_symmetry_class;
    std::vector<SymmetryWorker>     _symmetry_workers;
    worker_simpidx_symmetries_type  _worker_symmetries;
    std::vector<orbit_type>         _worker_orbits;

  private:
    SymmetricFlipGraph();
    SymmetricFlipGraph(const SymmetricFlipGraph&);
    SymmetricFlipGraph& operator=(const SymmetricFlipGraph&);
  public:
    // constructor:
    inline SymmetricFlipGraph(const parameter_type, 
			      const parameter_type,
			      const PointConfiguration&,
			      const Chirotope&,
			      const SymmetryGroup&,
			      const SimplicialComplex&,
			      const symmetryptr_datapair&,
			      const Volumes*,
			      const bool = false,
			      const bool = false);
    // destructor:
    inline ~SymmetricFlipGraph();
    // results:
    inline const size_type totalcount() const { return _totalcount; }
    inline const size_type symcount()   const { return _symcount; }
    // internal methods:
    void _mark_equivalent_flips(const TriangNode&, 
				const tnode_container_type::iterator,
				const FlipRep&);
    void _mark_equivalent_flips(const TriangNode&,
				TriangFlips&, 
				const FlipRep&);
  
    TriangNode           _map                         (const Vector&)                      const;
    TriangNode           _map                         (const std::pair<Symmetry, Symmetry>&,
						       const TriangNode&)                  const;
    void                 _map_into                    (std::pair<Symmetry, Symmetry>&,
						       const TriangNode&,
						       TriangNode&)                        const;
    TriangNode           _map_via_indexset            (const Symmetry&,
						       const TriangNode&)                  const;
    void                 _map_into_via_indexset       (const Symmetry&,
						       const TriangNode&,
						       TriangNode&)                        const;
    symmetryptr_datapair _stabilizer_ptrs             (const TriangNode&,
						       const Vector&)                      const;
    symmetryptr_datapair _stabilizer_ptrs_via_indexset(const TriangNode&,
						       const Vector&)                      const;
    Vector               _gkz                         (const TriangNode&)                  const;
    
    bool _threads_done() const;
    int  _old_symmetry_class(const TriangNode&, const Vector&);
    void _process_newtriang(const TriangNode&, 
			    const TriangFlips&, 
			    const TriangNode&, 
			    const FlipRep&);
    void _process_flips(const TriangNode&, const TriangFlips&);
    void _bfs_step();
    void _bfs();
    void _init_symmetry_workers();
    void _init();
    // stream output/input:
    std::istream& read(std::istream&);
    inline std::ostream& write(std::ostream&) const;
    friend inline std::istream& operator>>(std::istream& ist, SymmetricFlipGraph& sbfs);
    friend inline std::ostream& operator<<(std::ostream& ost, const SymmetricFlipGraph& sbfs);
  };

  inline SymmetricFlipGraph::SymmetricFlipGraph(const parameter_type        no, 
						const parameter_type        rank,
						const PointConfiguration&   points,
						const Chirotope&            chiro,
						const SymmetryGroup&        symmetries,
						const SimplicialComplex&    seed,
						const symmetryptr_datapair& seed_symmetryptrs,
						const Volumes*              voltableptr,
						const bool                  print_triangs,
						const bool                  only_fine_triangs) :
#ifdef CHECK_NEW
    _all_triangs(),
#endif
    _no(no), _rank(rank), _pointsptr(&points), _chiroptr(&chiro), _inctableptr(0),
    _voltableptr(voltableptr),
    _symmetries(symmetries), _simpidx_symmetries(),
    _seed(seed), _node_symmetryptrs(seed_symmetryptrs),
    _previous_triangs(), _new_triangs(),
    _current_gkz(no), _new_gkz(no), _equivalent_gkz(no), 
    _known_gkzs(),
    _stabilizers(),
    _find_iter(_previous_triangs.begin()),
    _totalcount(0), _symcount(0), _flipcount(0), _reportcount(CommandlineOptions::report_frequency()),
    _representative(_symcount, no, rank, seed), _transformation(_no),
    _orbitsize(0), _orbit(),
    _rep_has_search_pred(false),
    _print_triangs(print_triangs),
    _only_fine_triangs(only_fine_triangs),
    _processed_count(0L),
    _dump_no(0),
    _location_of_old_symmetry_class(0),
    _no_of_threads(std::min<size_type>(symmetries.size(), CommandlineOptions::no_of_threads())),
    _old_symmetry_class_pushresults_mutex(),
    _searchpred_pushresults_mutex(),
    _worker_orbits(std::min<size_type>(symmetries.size(), CommandlineOptions::no_of_threads())),
    _worker_symmetries(std::min<size_type>(symmetries.size(), CommandlineOptions::no_of_threads())),
    _no_of_busy_threads(0),
    _no_of_waiting_threads(0) {
    _init();
  }

  inline SymmetricFlipGraph::~SymmetricFlipGraph() {
    if (CommandlineOptions::parallel_symmetries()) {
      for (int i = 0; i < _no_of_threads; ++i) {
	_symmetry_workers[i].stop_worker();
	_symmetry_workers[i].worker_condition.notify_one();
      }
      for (int i = 0; i < _no_of_threads; ++i) {
	if (_threads[i].joinable()) {
	  _threads[i].join();
	}
      }
    }
    delete _cout_triang_ptr;
    delete _output_pred_ptr;
    delete _search_pred_ptr;
    if (_inctableptr) {
      delete _inctableptr;
    }
  }

  // stream output/input:

  inline std::ostream& SymmetricFlipGraph::write(std::ostream& ost) const {
    ost << "TOPCOM SymmetricFlipGraph dump start:" << std::endl;
    ost << "_dump_no             " << _dump_no << std::endl;
    ost << "_no                  " << _no << std::endl;
    ost << "_rank                " << _rank << std::endl;
    if (_pointsptr) {
      ost << "_points              " << *_pointsptr << std::endl;
    }
    if (_chiroptr) {
      ost << "_chiro               " << *_chiroptr << std::endl;
    }
    ost << "_symmetries          " << _symmetries << std::endl;
    ost << "_print_triangs       " << _print_triangs << std::endl;
    ost << "_only_fine_triangs   " << _only_fine_triangs << std::endl;
    ost << "_previous_triangs    " << _previous_triangs << std::endl;
    ost << "_new_triangs         " << _new_triangs << std::endl;
    ost << "_totalcount          " << _totalcount << std::endl;
    ost << "_symcount            " << _symcount << std::endl;
    ost << "_reportcount         " << _reportcount << std::endl;
    ost << "SymmetricFlipGraph dump end." << std::endl;
    return ost;
  }

  inline std::istream& operator>>(std::istream& ist, SymmetricFlipGraph& sbfs) {
    return sbfs.read(ist);
  }

  inline std::ostream& operator<<(std::ostream& ost, const SymmetricFlipGraph& sbfs) {
    return sbfs.write(ost);
  }

}; // namespace topcom

#endif

// eof SymmetricFlipGraph.hh
