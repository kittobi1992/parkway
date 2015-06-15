#ifndef _VCYCLEFINAL_CONTROLLER_HPP
#define _VCYCLEFINAL_CONTROLLER_HPP

// ### ParaVCycleFinalController.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 4/1/2005: Last Modified
//
// ###

#include "parallel_v_cycle_controller.hpp"

class parallel_v_cycle_final_controller : public parallel_v_cycle_controller {
 public:
  parallel_v_cycle_final_controller(parallel_restrictive_coarsening &rc, parallel_coarsener &c,
                            parallel::refiner &r, parkway::serial::controller &ref, int rank,
                            int nP, int percentile, int inc, int approxRef,
                            int limit, double limitAsPercent, std::ostream &out);
  ~parallel_v_cycle_final_controller();

  void run(MPI_Comm comm);
  void print_type() const;
};

#endif
