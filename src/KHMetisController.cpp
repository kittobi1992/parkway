
#ifndef _KHMETIS_CONTROLLER_CPP
#define _KHMETIS_CONTROLLER_CPP

// ### KHMetisController.cpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 30/11/2004: Last Modified
//
// ###

#include "configurtion.h"
#ifdef LINK_HMETIS
#include "KHMetisController.hpp"

KHMetisController::KHMetisController(parkway::serial::greedy_k_way_refiner *k,
                                     int rank, int nProcs,
                                     int nParts, const int *options)
    : parkway::serial::controller(rank, nProcs, nParts) {
  int i;

  kWayRefiner = k;
  lenOfOptions = 9;
  maxPartWt = 0;
  avePartWt = 0;

  khMetisOptions.resize(9);

  for (i = 0; i < lenOfOptions; ++i)
    khMetisOptions[i] = options[i];
}

KHMetisController::~KHMetisController() {
  kWayRefiner->destroy_data_structures();
}

void KHMetisController::display_options() const { }

void KHMetisController::run(parkway::parallel::hypergraph &hgraph,
                            MPI_Comm comm) {
  initialize_coarsest_hypergraph(hgraph, comm);

#ifdef DEBUG_CONTROLLER
  assert(h);
#endif

  hgraph.set_number_of_partitions(1);

  khMetisOptions[1] = number_of_runs_ / number_of_processors_ + 1;

  int numVertices = hgraph.number_of_vertices();
  int numHedges = hgraph.number_of_hyperedges();
  int totWt = hgraph.vertex_weight();

  int numHedgesCut;
  int ubFactor = static_cast<int>(floor(k_way_constraint_ * 100));

  int *vWeights = hgraph.vertex_weights().data();
  int *hOffsets = hgraph.hyperedge_offsets().data();
  int *pinList = hgraph.pin_list().data();
  int *hEdgeWts = hgraph.hyperedge_weights().data();
  int *pArray = hgraph.partition_vector().data();

  khMetisOptions[7] = RANDOM(1, 10000000);

  HMETIS_PartKway(numVertices, numHedges, vWeights, hOffsets, pinList, hEdgeWts,
                  number_of_parts_, ubFactor, khMetisOptions.data(), pArray,
                  &numHedgesCut);

  avePartWt = static_cast<double>(totWt) / number_of_parts_;
  maxPartWt = static_cast<int>(floor(avePartWt + avePartWt * k_way_constraint_));

  hgraph.calculate_cut_size(number_of_parts_, 0, comm);

  // kWayRefiner->set_maximum_part_weight(maxPartWt);
  // kWayRefiner->set_average_part_weight(avePartWt);
  // kWayRefiner->rebalance(hgraph);

#ifdef DEBUG_CONTROLLER
  MPI_Barrier(comm);
  for (int i = 0; i < number_of_processors_; ++i) {
    if (rank_ == i)
      h->checkPartitions(number_of_parts_, maxPartWt);

    MPI_Barrier(comm);
  }
#endif

  // ###
  // project partitions
  // ###

  initialize_serial_partitions(hgraph, comm);

#ifdef DEBUG_CONTROLLER
  hgraph.checkPartitions(number_of_parts_, maxPartWt, comm);
#endif
}

#endif
#endif
