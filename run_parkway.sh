PARKWAY="$HOME/hypergraph_partitioner/parkway/bin/parkway_driver"
PARKWAY_CONVERTER="$HOME/mt-kahypar/build/tools/MetisToParkway"
HYPERGRAPH=$1
NUM_PROCESSORS=$2
NUM_BLOCKS=$3
EPSILON=$4
BIN_HYPERGRAPH=$1.bin

$PARKWAY_CONVERTER -h $HYPERGRAPH -p $NUM_PROCESSORS
mpirun -N $NUM_PROCESSORS $PARKWAY -p $NUM_BLOCKS -c $EPSILON --display-info --display-progress --hypergraph=$BIN_HYPERGRAPH