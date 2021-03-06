#ifndef _PARA_RESTR_FCC_HPP
#define _PARA_RESTR_FCC_HPP
// ### ParaRestrFCCoarsener.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 31/12/2004: Last Modified
//
// ###
#include <iostream>
#include "coarseners/parallel/restrictive_coarsening.hpp"
#include "hypergraph/parallel/hypergraph.hpp"

namespace parkway {
namespace parallel {

class restrictive_first_choice_coarsening : public restrictive_coarsening {
 protected:
  int vertex_visit_order_;
  int divide_by_cluster_weight_;
  int divide_by_hyperedge_length_;
  int limit_on_index_during_corasening_;

 public:
  restrictive_first_choice_coarsening(int rank, int nProcs, int nParts,
                                      int verVisOrder, int divByWt,
                                      int divByLen);
  ~restrictive_first_choice_coarsening();

  void display_options() const;
  void build_auxiliary_structures(int numPins, double aveVertDeg,
                                  double aveHedgeSize);
  void release_memory();

  hypergraph *coarsen(hypergraph &h, MPI_Comm comm);

  void permute_vertices_array(dynamic_array<int> &verts, int nLocVerts);
  void set_cluster_indices(MPI_Comm comm);
  void print_visit_order(int variable) const;
};

}  // namespace parallel
}  // namespace parkway

#endif
