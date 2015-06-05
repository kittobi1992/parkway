
#ifndef _FM_REFINER_HPP
#define _FM_REFINER_HPP

// ### FMRefiner.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 30/11/2004: Last Modified
//
// NOTES:
//
// 21/11/2004:
//
// changed so that does not always expect a balanced bisection
// furthermore, should the initial bisection be unbalanced, it
// does not expect to be able to rebalance it.
//
// ###

#include "Bit.hpp"
#include "Refiner.hpp"
#include "BucketNode.hpp"

using namespace std;

class FMRefiner : public Refiner {

protected:
  // ###
  // auxiliary members
  // ###

  int bucketArraysLen;
  int maxPossGain;
  int insertMethod;
  int eeThreshold;
  int maxNonPosMoves;

  // ###
  // bucket arrays
  // ###

  NodePtrArray buckets;
  FastDynaArray<NodeArray *> bucketArrays;

  FastDynaArray<int> maxGains;
  FastDynaArray<int> maxGainEntries;
  FastDynaArray<int> numBucketsInArray;
  FastDynaArray<int> vInPart;
  FastDynaArray<int> vertexGains;
  FastDynaArray<int> moveList;

  BitField locked;

public:
  FMRefiner(int max, int insMethod, int ee, int dL);
  ~FMRefiner();

  void dispRefinerOptions(ostream &out) const;
  void printQdis(ostream &out) const;

  void buildBuckets();
  void restoreBuckets();
  void destroyBuckets();
  void initPartitionStruct();

  void prepVertexGains();
  void initGains1to0();
  void initGains0to1();

  void removeBucketsFrom1();
  void removeBucketsFrom0();
  void removeUnlockedFromBucketArrays();
  void moveToBucketArray(int vPart, int vGain, int v);
  void removeFromBucketArray(int v, int vPart, int vGain);

  void adjustMaxGainPtr(register int dP);
  void updateGains(register int v);
  void updateGains1_0(register int v);
  void updateGains0_1(register int v);
  void unmakeMove(register int v);

  void refine(Hypergraph &h);

  int doFidMatPass();
  int doRebalancingPass(int largePart);
  int chooseMaxGainVertex();
  int chooseLegalMove(int sP);

  inline void setEEThreshold() {
    maxNonPosMoves = static_cast<int>(
        floor((static_cast<double>(numVertices) / 100) * eeThreshold));
  }

  inline void removeEEThreshold() { maxNonPosMoves = numVertices; }

  inline int checkerHigh(int array) {
    register int i;
    for (i = (maxPossGain << 1); i > 0; --i) {
      if ((*bucketArrays[array])[i].next)
        return i;
    }
    return i;
  }

  inline void sanityCheck() {
    for (register int i = 0; i < 2; ++i) {
      assert(maxGains[i] == maxGainEntries[i] - maxPossGain);
      assert(maxGainEntries[i] == checkerHigh(i));
    }
  }
};

#endif
