
#  ifndef _PARA_COARSENER_HPP
#  define _PARA_COARSENER_HPP


// ### ParaCoarsener.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
// 
// HISTORY: 
//
// 4/1/2005: Last Modified
//
// ###


#  include "ParaHypergraphLoader.hpp"
#  include "HashTables.hpp"


using namespace std;


class ParaCoarsener  
  : public ParaHypergraphLoader 
{

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
  
  FastDynaArray<int> clusterWeights;

public:

  ParaCoarsener(int _rank, int _numProcs, int _numParts, ostream &out);

  virtual ~ParaCoarsener();
  virtual ParaHypergraph *coarsen(ParaHypergraph &h, MPI_Comm comm)=0;
  virtual void setClusterIndices(MPI_Comm comm)=0;
  virtual void releaseMemory()=0;
  virtual void dispCoarseningOptions() const=0;
  virtual void buildAuxiliaryStructs(int numTotPins, double aveVertDeg, double aveHedgeSize)=0;
  
  void loadHyperGraph(const ParaHypergraph &h, MPI_Comm comm);

  ParaHypergraph *contractHyperedges(ParaHypergraph &h, MPI_Comm comm);

  inline void setReductionRatio(register double ratio) { reductionRatio = ratio; }
  inline void setBalConstraint(register double constraint) { balConstraint = constraint; }
  inline void setMinNodes(register int min) { minNodes = min; }
  inline void setMaxVertexWt(register int m) { maxVertexWt = m; }
  inline void setTotGraphWt(register int t) { totalHypergraphWt = t; }

};



#  endif
