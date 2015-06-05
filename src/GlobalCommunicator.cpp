
#ifndef _GLOBAL_COMMUNICATOR_CPP
#define _GLOBAL_COMMUNICATOR_CPP

// ### GlobalCommunicator.cpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 4/1/2005: Last Modified
//
// ###

#include "GlobalCommunicator.hpp"

GlobalCommunicator::GlobalCommunicator(int rank, int nProcs) {
  register int i;

  myRank = rank;
  numProcs = nProcs;

  dataOutSets.setLength(numProcs);
  sendLens.setLength(numProcs);
  recvLens.setLength(numProcs);
  sendDispls.setLength(numProcs);
  recvDispls.setLength(numProcs);

  for (i = 0; i < numProcs; ++i)
    dataOutSets[i] = new FastDynaArray<int>(1024);
}

GlobalCommunicator::~GlobalCommunicator() {
  register int i;

  for (i = 0; i < numProcs; ++i)
    DynaMem<FastDynaArray<int> >::deletePtr(dataOutSets[i]);
}

void GlobalCommunicator::freeMemory() {
  register int i;

  for (i = 0; i < numProcs; ++i)
    dataOutSets[i]->setLength(0);

  sendArray.setLength(0);
  receiveArray.setLength(0);
}

void GlobalCommunicator::sendFromDataOutArrays(MPI_Comm comm) {
  int *array;
  int arrayLen;

  register int ij;
  register int i;
  register int j;

  j = 0;
  for (i = 0; i < numProcs; ++i) {
    sendDispls[i] = j;
#ifdef DEBUG_BASICS
    assert(sendLens[i] >= 0);
#endif
    j += sendLens[i];
  }

  sendArray.setLength(j);

  ij = 0;
  for (i = 0; i < numProcs; ++i) {
    array = dataOutSets[i]->getArray();
    arrayLen = sendLens[i];

    for (j = 0; j < arrayLen; ++j) {
      sendArray[ij++] = array[j];
    }
  }

  MPI_Alltoall(sendLens.getArray(), 1, MPI_INT, recvLens.getArray(), 1, MPI_INT,
               comm);

  j = 0;
  for (i = 0; i < numProcs; ++i) {
    recvDispls[i] = j;
    j += recvLens[i];
  }

  receiveArray.setLength(j);

  MPI_Alltoallv(sendArray.getArray(), sendLens.getArray(),
                sendDispls.getArray(), MPI_INT, receiveArray.getArray(),
                recvLens.getArray(), recvDispls.getArray(), MPI_INT, comm);
}

#endif