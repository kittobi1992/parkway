
#ifndef _REFINER_HPP
#define _REFINER_HPP

// ### Refiner.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 30/11/2004: Last Modified
//
// ###

#include <ostream>
#include "Macros.h"
#include "data_structures/dynamic_array.hpp"
#include "hypergraph/serial/hypergraph.hpp"
#include "hypergraph/serial/loader.hpp"

namespace serial = parkway::hypergraph::serial;
using parkway::data_structures::dynamic_array;

class Refiner : public serial::loader {
 protected:
  int maxPartWt;
  int numParts;
  int *partitionVector;

  double acceptProp;
  double avePartWt;

  dynamic_array<int> partWeights;

 public:
  Refiner(int dispL);

  virtual ~Refiner();
  virtual void refine(serial::hypergraph &h) = 0;
  virtual void dispRefinerOptions(std::ostream &out) const = 0;

  inline int getMaxPartWt() const { return maxPartWt; }

  inline void setMaxPartWt(int max) { maxPartWt = max; }
  inline void setAvePartWt(double ave) { avePartWt = ave; }
  inline void setPartitionVector(int nP) {
    partitionVector = &partitionVectors[partitionOffsets[nP]];
  }

  int calcCutsize() const;
};

#endif
