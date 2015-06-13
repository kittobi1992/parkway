

#ifndef _WEBGRAPH_SEQ_CONTROLLER_HPP
#define _WEBGRAPH_SEQ_CONTROLLER_HPP

// ### WebGraphSeqController.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 07/04/2005: Last Modified
//
// ###

#include "SeqController.hpp"
#include "hypergraph/parallel/hypergraph.hpp"

namespace parallel = parkway::hypergraph::parallel;

class WebGraphSeqController : public SeqController {
protected:
public:
  WebGraphSeqController(int rank, int nProcs, int nParts, ostream &out);

  virtual ~WebGraphSeqController();
  virtual void dispSeqControllerOptions() const = 0;
  virtual void runSeqPartitioner(parallel::hypergraph &hgraph, MPI_Comm comm) = 0;
  virtual void initSeqPartitions(parallel::hypergraph &h, MPI_Comm comm);

  void initCoarsestHypergraph(parallel::hypergraph &hgraph, MPI_Comm comm);
};

#endif
