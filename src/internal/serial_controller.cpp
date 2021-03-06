// ### SeqController.cpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 30/11/2004: Last Modified
//
// ###
#include "internal/serial_controller.hpp"
#include "utility/logging.hpp"

namespace parkway {
namespace serial {


controller::controller(int rank, int nProcs, int nParts) {
  rank_ = rank;
  number_of_processors_ = nProcs;
  number_of_parts_ = nParts;
  number_of_runs_ = 0;
  accept_proportion_ = 0;
  maximum_vertex_weight_ = 0;
  accept_proportion_ = 0;

  hypergraph_ = nullptr;

  partition_vector_.resize(0);
  partition_vector_cuts_.resize(0);
  partition_vector_offsets_.resize(0);
}

controller::~controller() {
}

void controller::initialize_coarsest_hypergraph(parallel::hypergraph &hgraph,
                                                MPI_Comm comm) {
  int i;
  int j;
  int ij;

  int recvArrayLen;
  int index;
  int numLocalVertices = hgraph.number_of_vertices();
  int numLocalHedges = hgraph.number_of_hyperedges();
  int numLocalPins = hgraph.number_of_pins();
  int localVertexWt = hgraph.vertex_weight();

  auto localVertWeight = hgraph.vertex_weights();
  auto localHedgeOffsets = hgraph.hyperedge_offsets();
  auto localHedgeWeights = hgraph.hyperedge_weights();
  auto localPins = hgraph.pin_list();

  int numVertices = hgraph.total_number_of_vertices();
  int numHedges;
  int numPins;
  int totVertexWt;

  dynamic_array<int> vWeights;
  dynamic_array<int> hEdgeWeights;
  dynamic_array<int> hEdgeOffsets;
  dynamic_array<int> pinList;
  dynamic_array<int> recvDispls(number_of_processors_);
  dynamic_array<int> recvLens(number_of_processors_);
  dynamic_array<int> recvArray;

  MPI_Allgather(&numLocalVertices, 1, MPI_INT, recvLens.data(), 1, MPI_INT,
                comm);

  ij = 0;
  for (i = 0; i < number_of_processors_; ++i) {
    recvDispls[i] = ij;
    ij += recvLens[i];
  }

#ifdef DEBUG_CONTROLLER
  assert(numVertices == ij);
#endif

  vWeights.resize(numVertices);

  MPI_Allreduce(&localVertexWt, &totVertexWt, 1, MPI_INT, MPI_SUM, comm);
  MPI_Allgatherv(localVertWeight.data(), numLocalVertices, MPI_INT,
                 vWeights.data(), recvLens.data(),
                 recvDispls.data(), MPI_INT, comm);
  MPI_Allgather(&numLocalHedges, 1, MPI_INT, recvLens.data(), 1, MPI_INT,
                comm);

  ij = 0;
  for (i = 0; i < number_of_processors_; ++i) {
    recvDispls[i] = ij;
    ij += recvLens[i];
  }

  numHedges = ij;
  hEdgeWeights.resize(numHedges);

  MPI_Allgatherv(localHedgeWeights.data(), numLocalHedges, MPI_INT,
                 hEdgeWeights.data(), recvLens.data(),
                 recvDispls.data(), MPI_INT, comm);

  ij = 0;
  for (i = 0; i < number_of_processors_; ++i) {
    recvDispls[i] = ij;
    ++recvLens[i];
    ij += recvLens[i];
  }

  recvArrayLen = ij;
  recvArray.resize(recvArrayLen);
  MPI_Allgatherv(localHedgeOffsets.data(), numLocalHedges + 1, MPI_INT,
                 recvArray.data(), recvLens.data(),
                 recvDispls.data(), MPI_INT, comm);
  hEdgeOffsets.resize(numHedges + 1);

  j = 1;
  index = 0;
  hEdgeOffsets[0] = 0;

  for (i = 1; i < recvArrayLen; ++i) {
    if (recvArray[i] != 0) {
      ij = recvArray[i] - recvArray[i - 1];
      hEdgeOffsets[j] = hEdgeOffsets[j - 1] + ij;
      ++j;
    } else {
      recvLens[index++] = recvArray[i - 1];
    }
  }

#ifdef DEBUG_CONTROLLER
  assert(index == numProcs - 1);
#endif

  recvLens[index] = recvArray[recvArrayLen - 1];

  ij = 0;
  for (i = 0; i < number_of_processors_; ++i) {
    recvDispls[i] = ij;
    ij += recvLens[i];
  }

  numPins = ij;
  pinList.resize(numPins);
  MPI_Allgatherv(localPins.data(), numLocalPins, MPI_INT, pinList.data(),
                 recvLens.data(), recvDispls.data(), MPI_INT, comm);

  hypergraph_ = new serial::hypergraph(vWeights, numVertices);

  hypergraph_->set_number_of_hyperedges(numHedges);
  hypergraph_->set_number_of_pins(numPins);
  hypergraph_->set_total_weight(totVertexWt);
  hypergraph_->set_hyperedge_weights(hEdgeWeights);
  hypergraph_->set_hyperedge_offsets(hEdgeOffsets);
  hypergraph_->set_pin_list(pinList);
  hypergraph_->buildVtoHedges();
  hypergraph_->print_characteristics();
}

void controller::initialize_serial_partitions(
    parallel::hypergraph &hgraph, MPI_Comm comm) {
  int i;
  int j;
  int ij;

  int keepMyPartition;
  int proc;
  int numKept;
  int myBestCut = hypergraph_->cut(0);
  int ijk;
  int startOffset;
  int endOffset;
  int totToSend;

  dynamic_array<int> hPartitionVector;
  dynamic_array<int> hPartVectorOffsets;
  dynamic_array<int> hPartCuts;

  int numTotVertices = hypergraph_->number_of_vertices();
  auto pVector = hypergraph_->partition_vector();
  auto pCuts = hypergraph_->partition_cuts();

  dynamic_array<int> numVperProc(number_of_processors_);
  dynamic_array<int> procDispls(number_of_processors_);

  dynamic_array<int> sendLens(number_of_processors_);
  dynamic_array<int> sendDispls(number_of_processors_);
  dynamic_array<int> recvLens(number_of_processors_);
  dynamic_array<int> recvDispls(number_of_processors_);
  dynamic_array<int> sendArray;

  dynamic_array<int> procCuts(number_of_processors_);
  dynamic_array<int> procs(number_of_processors_);
  dynamic_array<int> keepPartitions(number_of_processors_);

  // ###
  // First root processor determines
  // which partitions to keep
  // ###

  MPI_Gather(&myBestCut, 1, MPI_INT, procCuts.data(), 1, MPI_INT, ROOT_PROC,
             comm);

  if (rank_ == ROOT_PROC) {
    numKept = 0;

    for (i = 0; i < number_of_processors_; ++i)
      procs[i] = i;

    procs.random_permutation();

    for (i = 0; i < number_of_processors_; ++i) {
      proc = procs[i];
      ij = 0;

      for (j = i + 1; j < number_of_processors_; ++j) {
        if (procCuts[proc] == procCuts[procs[j]]) {
          ij = 1;
          break;
        }
      }

      if (ij == 0) {
        keepPartitions[proc] = 1;
        ++numKept;
      } else
        keepPartitions[proc] = 0;
    }
  }

  MPI_Scatter(keepPartitions.data(), 1, MPI_INT, &keepMyPartition, 1,
              MPI_INT, ROOT_PROC, comm);
  MPI_Bcast(&numKept, 1, MPI_INT, ROOT_PROC, comm);

  hgraph.set_number_of_partitions(numKept);

  hPartitionVector = hgraph.partition_vector();
  hPartVectorOffsets = hgraph.partition_offsets();
  hPartCuts = hgraph.partition_cuts();

  // ###
  // communicate partition vector values
  // ###

  j = number_of_processors_ - 1;
  ij = numTotVertices / number_of_processors_;

  for (i = 0; i < j; ++i)
    numVperProc[i] = ij;

  numVperProc[i] = ij + numTotVertices % number_of_processors_;

  j = 0;
  ij = 0;

  for (i = 0; i < number_of_processors_; ++i) {
    sendDispls[i] = j;
    procDispls[i] = ij;
    sendLens[i] = numVperProc[i] * keepMyPartition;
    j += sendLens[i];
    ij += numVperProc[i];
  }

  sendArray.resize(j);
  totToSend = j;

  ij = 0;

  for (ijk = 0; ijk < number_of_processors_; ++ijk) {
    for (j = 0; j < keepMyPartition; ++j) {
      startOffset = procDispls[ijk];
      endOffset = startOffset + numVperProc[ijk];

      for (i = startOffset; i < endOffset; ++i) {
        sendArray[ij++] = pVector[i];
      }
    }
  }
#ifdef DEBUG_CONTROLLER
  assert(ij == totToSend);
#endif

  MPI_Alltoall(sendLens.data(), 1, MPI_INT, recvLens.data(), 1, MPI_INT,
               comm);

  ij = 0;

  for (i = 0; i < number_of_processors_; ++i) {
    recvDispls[i] = ij;
    ij += recvLens[i];
  }

#ifdef DEBUG_CONTROLLER
  assert(ij == hPartVectorOffsets[numKept]);
#endif

  MPI_Alltoallv(sendArray.data(), sendLens.data(),
                sendDispls.data(), MPI_INT, hPartitionVector.data(),
                recvLens.data(), recvDispls.data(), MPI_INT, comm);

  // ###
  // communicate partition cuts
  // ###

  MPI_Allgather(&keepMyPartition, 1, MPI_INT, recvLens.data(), 1, MPI_INT,
                comm);

  ij = 0;

  for (i = 0; i < number_of_processors_; ++i) {
    recvDispls[i] = ij;
    ij += recvLens[i];
  }

  MPI_Allgatherv(pCuts.data(), keepMyPartition, MPI_INT, hPartCuts.data(),
                 recvLens.data(), recvDispls.data(), MPI_INT, comm);

  for (i = 0; i < numKept; ++i) {
    progress("%i ", hPartCuts[i]);
  }
  progress("\n");
}

int controller::choose_best_partition() const {
#ifdef DEBUG_CONTROLLER
  assert(numSeqRuns > 0);
  assert(partitionCuts.getLength() > 0);
#endif

  int i = 1;
  int j = 0;

  for (; i < number_of_runs_; ++i)
    if (partition_vector_cuts_[i] < partition_vector_cuts_[j])
      j = i;

  return j;
}

int controller::accept_cut() const {
  int i;
  int bestCut;

#ifdef DEBUG_CONTROLLER
  assert(h);
  assert(numSeqRuns > 0);
  assert(partitionVector.getLength() == h->getNumVertices() * numSeqRuns);
  assert(partitionCuts.getLength() == numSeqRuns);
  assert(partitionVectorOffsets.getLength() == numSeqRuns + 1);
  for (i = 0; i < numSeqRuns; ++i)
    assert(partitionCuts[i] > 0);
#endif

  bestCut = partition_vector_cuts_[0];

  for (i = 1; i < number_of_runs_; ++i) {
    if (partition_vector_cuts_[i] < bestCut) {
      bestCut = partition_vector_cuts_[i];
    }
  }

  return (static_cast<int>(
      floor(static_cast<double>(bestCut) + bestCut * accept_proportion_)));
}

}  // namespace serial
}  // namespace parkway
