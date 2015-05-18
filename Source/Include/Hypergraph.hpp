
#  ifndef _HYPERGRAPH_HPP
#  define _HYPERGRAPH_HPP


// ### Hypergraph.hpp ###
//
// Copyright (C) 2004,  Aleksandar Trifunovic, Imperial College London
// 
// HISTORY: 
// 
// 30/11/2004: Last Modified
//
// ###


#  include <unistd.h>
//#  include <fstream>
#  include <cstdio>
#  include <cassert>

#  include "Macros.h"
#  include "Funct.hpp"
#  include "Dyna.hpp"


using namespace std;


class Hypergraph 
{

protected:

  int totWeight;
  int numVertices;
  int numHedges;
  int numPins;
  int numPartitions;
  
  FastDynaArray<int> vWeight;
  FastDynaArray<int> hEdgeWeight;
  FastDynaArray<int> matchVector;

  FastDynaArray<int> partitionCuts;
  FastDynaArray<int> partitionVector;
  FastDynaArray<int> partitionVectorOffsets;
  
  FastDynaArray<int> pinList;
  FastDynaArray<int> hEdgeOffsets;

  FastDynaArray<int> vToHedges;
  FastDynaArray<int> vOffsets;
  
public:
  
  Hypergraph(int *vWts, int numV);
  Hypergraph(int *vWts, int *pVector, int numV, int cut);
  ~Hypergraph();
  
  void hypergraphFromFile(const char* filename);
  void buildVtoHedges();

  void resetMatchVector();
  void resetPartitionVectors();
  void resetVertexMaps();

  void projectPartitions(const Hypergraph &coarseGraph);
  void removeBadPartitions(double fractionOK);  
  void setNumPartitions(int nPartitions);
  void copyOutPartition(register int *pVector, int numV, int pNo) const;
  void copyInPartition(const register int *pVector, int numV, int pNo, int cut);
  void printCharacteristics(ostream &o);
  void printPercentiles(ostream &o);

  int keepBestPartition();

  inline int getNumVertices() const { return numVertices; }
  inline int getNumHedges() const { return numHedges; }
  inline int getNumPins() const { return numPins; }
  inline int getTotWeight() const { return totWeight; }
  inline int getNumPartitions() const { return numPartitions; }
  inline int getCut(register int pNo) const { return partitionCuts[pNo]; }

  inline int *getVerWeightsArray() const { return vWeight.getArray(); }
  inline int *getHedgeWeightsArray() const { return hEdgeWeight.getArray(); }
  inline int *getMatchVectorArray() const { return matchVector.getArray(); }
  inline int *getPinListArray() const { return pinList.getArray(); }
  inline int *getHedgeOffsetArray() const { return hEdgeOffsets.getArray(); }
  inline int *getVtoHedgesArray() const { return vToHedges.getArray(); }
  inline int *getVerOffsetsArray() const { return vOffsets.getArray(); }

  inline int *getPartVectorArray() const { return partitionVector.getArray(); }
  inline int *getPartOffsetArray() const { return partitionVectorOffsets.getArray(); }
  inline int *getPartCutArray() const { return partitionCuts.getArray(); }
  inline int *getPartitionVector(register int pNo) const
  {
    return (&partitionVector[partitionVectorOffsets[pNo]]);
  }
  
  inline void setNumHedges(register int newNum) { numHedges = newNum; }
  inline void setNumPins(register int newNum) { numPins = newNum; }
  inline void setNumVertices(register int newNum) { numVertices = newNum; }
  inline void setTotWeight(register int newWt) { totWeight = newWt; }
  inline void setWtsArray(register int *array, register int len) { vWeight.setArray(array,len); }
  inline void setHedgeWtArray(register int *array, register int len) { hEdgeWeight.setArray(array,len); }
  inline void setPinListArray(register int *array, register int len) { pinList.setArray(array,len); }
  inline void setHedgeOffsetArray(register int *array, register int len) { hEdgeOffsets.setArray(array,len); }
  inline void setVtoHedgesArray(register int *array, register int len) { vToHedges.setArray(array,len); }
  inline void setVoffsetsArray(register int *array, register int len) { vOffsets.setArray(array,len); }

  inline void setPartitionCutsArray(register int *a, register int len) { partitionCuts.setArray(a,len); }
  inline void setPartitionVectorArray(register int *a, register int len) { partitionVector.setArray(a,len); }   

  int getExposedHedgeWt() const;
  int calcCutsize(int nP, int partitionNo) const;
  int getSOED(int nP, int partitionNo) const;

  void initCutsizes(int numParts);
  void checkPartitions(int nP, int maxWt) const;
  void checkPartition(int partitionNum, int numParts, int maxPartWt) const;
  
  void convertToDIMACSGraphFile(const char *fName) const;

};


#  endif









