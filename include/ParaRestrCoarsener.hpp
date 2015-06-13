
#ifndef _PARA_RESTR_COARSENER_HPP
#define _PARA_RESTR_COARSENER_HPP

// ### ParaRestrCoarsener.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 4/1/2005: Last Modified
//
// ###

#include <iostream>
#include "data_structures/dynamic_array.hpp"
#include "hypergraph/parallel/hypergraph.hpp"
#include "hypergraph/parallel/loader.hpp"

namespace parallel = parkway::hypergraph::parallel;
namespace ds = parkway::data_structures;

class ParaRestrCoarsener : public parallel::loader {
 protected:
  /* coarsening auxiliary variables */

  int totalHypergraphWt;
  int maxVertexWt;
  int minNodes;
  int stopCoarsening;
  int clusterIndex;
  int totalClusters;
  int myMinCluIndex;

  double reductionRatio;
  double balConstraint;

  /* partition data_ structures */

  int numPartitions;
  int *partitionVector;
  int *partitionVectorOffsets;
  int *partitionCuts;

  ds::dynamic_array<int> *clusterWeights;
  ds::dynamic_array<int> *pVector;

 public:
  ParaRestrCoarsener(int _rank, int _numProcs, int _numParts, std::ostream &out);

  virtual ~ParaRestrCoarsener();
  virtual parallel::hypergraph *coarsen(parallel::hypergraph &h, MPI_Comm comm) = 0;
  virtual void setClusterIndices(MPI_Comm comm) = 0;
  virtual void release_memory() = 0;
  virtual void dispCoarseningOptions() const = 0;
  virtual void buildAuxiliaryStructs(int numTotPins, double aveVertDeg,
                                     double aveHedgeSize) = 0;

  void load(const parallel::hypergraph &h, MPI_Comm comm);

  parallel::hypergraph *contractHyperedges(parallel::hypergraph &h, MPI_Comm comm);

  inline void setReductionRatio(double ratio) {
    reductionRatio = ratio;
  }
  inline void setBalConstraint(double constraint) {
    balConstraint = constraint;
  }
  inline void setMinNodes(int min) { minNodes = min; }
  inline void setMaxVertexWt(int m) { maxVertexWt = m; }
  inline void setTotGraphWt(int tot) { totalHypergraphWt = tot; }
};

#endif
