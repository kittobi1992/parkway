
#ifndef _KHMETIS_CONTROLLER_HPP
#define _KHMETIS_CONTROLLER_HPP

// ### KHMetisController.hpp ###
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

// ## hMeTiS library function declaration ##
extern "C" {
void HMETIS_PartKway(int nv, int nh, int *vwt, int *hptr, int *hv, int *hwt,
                     int p, int ub, int *opt, int *pvector, int *cut);
}

#include "internal/serial_controller.hpp"
#include "refiners/serial/greedy_k_way_refiner.hpp"

using namespace std;

class KHMetisController : public parkway::serial::controller {
protected:
  int lenOfOptions;
  int maxPartWt;

  double avePartWt;

  dynamic_array<int> khMetisOptions;

  parkway::serial::greedy_k_way_refiner *kWayRefiner;

public:
  KHMetisController(parkway::serial::greedy_k_way_refiner  *k,
                    int rank, int nProcs, int nParts,
                    const int *options);
  ~KHMetisController();

  void display_options() const;
  void run(parkway::parallel::hypergraph &h, MPI_Comm comm);
};

#endif
#endif
