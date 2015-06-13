
#ifndef _PARA_REFINER_HPP
#define _PARA_REFINER_HPP

// ### ParaRefiner.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 4/1/2005: Last Modified
//
// ###

#include "hypergraph/parallel/hypergraph.hpp"
#include "hypergraph/parallel/loader.hpp"
#include "data_structures/dynamic_array.hpp"
#include "data_structures/map_to_pos_int.hpp"

namespace parallel = parkway::hypergraph::parallel;
namespace ds = parkway::data_structures;

class ParaRefiner : public parallel::loader {
protected:
  int maxPartWt;
  int numPartitions;

  int *partitionVector;
  int *partitionVectorOffsets;
  int *partitionCuts;

  int *currPVector;
  int currPnumber;

  double balConstraint;
  double avePartWt;

  ds::dynamic_array<int> partWeights;

  /* newly added structures */

  int numNonLocVerts;
  int numNonLocVertsHedges;
  int *currNonLocPVector;

  ds::dynamic_array<int> nonLocVerts;
  ds::dynamic_array<int> partIndices;
  ds::dynamic_array<int> indexIntoPartIndices;

  ds::dynamic_array<int> nonLocVToHedges;
  ds::dynamic_array<int> nonLocOffsets;

  ds::map_to_pos_int toNonLocVerts;

public:
  ParaRefiner(int rank, int nProcs, int nParts, std::ostream &out);

  virtual ~ParaRefiner();
  virtual void dispRefinementOptions() const = 0;
  virtual void release_memory() = 0;
  virtual void initDataStructs(const parallel::hypergraph &h, MPI_Comm comm) = 0;
  virtual void resetDataStructs() = 0;
  virtual void setPartitioningStructs(int pNumber, MPI_Comm comm) = 0;
  virtual void refine(parallel::hypergraph &h, MPI_Comm comm) = 0;
  virtual int computeCutsize(MPI_Comm comm) = 0;

  void load(const parallel::hypergraph &h, MPI_Comm comm);
  void initPartitionStructs(const parallel::hypergraph &h, MPI_Comm comm);

  inline void setBalConstraint(double b) { balConstraint = b; }
  inline int getMaxPartWt() const { return maxPartWt; }
};

#endif
