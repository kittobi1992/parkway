// ### driver.cpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 25/1/2005: Last Modified
//
// ###
#include "parkway.h"
#include "reader.h"
#include "utility/logging.hpp"
#include "options.hpp"

int main(int argc, char **argv) {
  /* DRIVER MAIN */
  /*
    Driver main tests the library functions

    Make the parallel files using program convert
    from the Utilities directory or otherwise (see
    manual for correct input file format).

    User is required to modify the options data_, some
    guidance can be found below - otherwise refer to the
    manual.
  */

  int options[29];
  int testType;
  int bestCut;
  int myRank;
  int numProcs;
  int numParts;

  char *outputFile;

  double constraint;

  parkway::options opts(argc, argv);

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

  MPI_Barrier(MPI_COMM_WORLD);
  parkway::utility::status::handler::set_rank(myRank);

  if (argc < 2) {
    info("USAGE:\n\n");
    info("$ mpirun [mpirun_options...] %s", argv[0]);
    info(" [parkway_options...] <hypergraph filename>\n\n");
    info("\t Unrecognized options will be ignored. \n\n");
    info("PARAMETERS: \n");
    info("\t -oFile <output_filename> \n");
    info("\t     - name of file that program information should be output to. "
                   "Default behaviour is to\n");
    info("\t       display program information to screen\n");
    info("\t -tType <test_type> \n");
    info("\t     - integer specifying the routine to be tested:\n");
    info("\t       0 -> tests "
         "ParaPartKway(char*,char*,int,double,int&,int*,MPI_Comm) - default\n");
    info("\t       1 -> tests "
         "ParaPartKway(int,int,int*,int*,int*,int*,int,double,int&,int*,int*,"
         "char*,MPI_Comm)\n\n");
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    exit(1);
  }

  /* PROCESS COMMAND LINE */
  parkway::utility::logging::set_log_file("logs/parkway_driver", myRank);

  testType = Funct::getParameterAsInteger(argc, argv, "-tType", 0);

  numParts = opts.get<int>("number-of-parts");
  constraint = opts.get<double>("balance-constraint");

  /* SETTING OPTIONS FOR TEST */

  // 0 -> all options use default, else user define
  options[0] = 1;
  // 0 -> seed chosen by sprng, else use options[1] as seed
  options[1] = 1;
  // 0 -> no disp info, 1 -> some, 2 -> lots
  options[2] = 2;
  // 0 -> do not write partition to disk, 1 -> do write
  options[3] = 1;
  // number of parallel runs
  options[4] = 1;
  // vertex to processor allocation: 0 -> as read in, 1 -> random 2 -> as
  // prescribed in partition file
  options[5] = 0;
  // hyperedge capacity_ percentile for approx para coarsening and refinement
  options[6] = 100;
  // increment in percentile options[6]
  options[7] = 1;
  // numParts*options[5] -> min number of vertices in coarse hypergraph
  options[8] = 200;
  //[9] and [10] specify reduction ratio in parallel coarsening
  options[9] = 7;
  // r = [9]/[10]
  options[10] = 4;
  // vertex visit order: 3 -> random, 1/2 inc/dec by vertex id, 4/5 inc/dec by
  // vertex wt
  options[11] = 3;
  // divide connectivity by cluster weight/hyperedge capacity_: 0-neither, 1-only
  // cluster, 2-only hedge len, 3-both
  options[12] = 3;
  // matching request resolution order: 3 -> random, 2 -> as they arrive
  options[13] = 3;
  // number serial partitioning runs
  options[14] = 1;
  // serial partitioning routine, 1-3 RB, 4 khmetis, 5 patoh, see manual
  options[15] = 1;
  // serial coarsening algorithm (only if [15] = RB, see manual)
  options[16] = 2;
  // num bisection runs in RB (only if [15] = RB, see manual)
  options[17] = 2;
  // num initial partitioning runs in RB (only if [13] = RB, see manual)
  options[18] = 1;
  // hmetis_PartKway coarsening option, vals 1-5, see manual (only if [15] = 4)
  options[19] = 2;
  // hmetis_PartKway refinement option, vals 0-3, see manual (only if [15] = 4)
  options[20] = 2;
  // patoh_partition parameter settings, vals 1-3, see manual (only if [15] = 5)
  options[21] = 2;
  // parallel uncoarsening algorithm, 1 simple, 2 only final V-Cycle, 3 all
  // V-Cycle
  options[22] = 1;
  // limit on number of V-Cycle iterations (only if [22] = 2/3)
  options[23] = 5;
  // min allowed gain for V-Cycle (percentage, see manual, only if [21] = 2/3)
  options[24] = 0;
  // percentage threshold used to reject partitions from a number of runs (see
  // manual)
  options[25] = 0;
  // reduction in [23] as partitions propagate by factor [24]/100 (see manual)
  options[26] = 0;
  // early exit criterion in parallel refinement, will exit if see
  // ([25]*num vert)/100 consecutive -ve moves
  options[27] = 100;
  // parallel refinement 0->basic, 1->use approx 2->use early exit 3->use approx
  // and early exit
  options[28] = 0;

  if (myRank == 0) {
    std::cout << "--- testing: k_way_partition(const parkway::options &, MPI_Comm)" << std::endl;
  }

  bestCut = k_way_partition(opts, MPI_COMM_WORLD);

  if (myRank == 0) {
    std::cout << "--- completed testing, best cut = " << bestCut << " ---\n";
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();

  return 0;
}
