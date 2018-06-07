param numColors;
param numNodes;
set edgeList dimen 2;

param edge {i in 0..numNodes-1, j in 0..numNodes-1} :=
	(if (i,j) in edgeList or (j,i) in edgeList then 1 else 0);
 
var x {i in 0..numNodes-1,j in 0..numColors-1} binary;

var conflict {i in 0..numNodes-1};

var conflictMtx {i in 0..numNodes-1, j in 0..numColors-1} >=0;

minimize Conflicts:
	sum {i in 0..numNodes-1 , j in 0..numColors-1}
		conflictMtx[i,j];

subject to C0 {i in 0..numNodes-1, j in 0..numColors-1}:
	conflictMtx[i,j] >= sum {k in 0..numNodes-1} edge[i,k]*x[k,j] -(1-x[i,j])*numNodes;

subject to C2 {i in 0..numNodes-1}:
	sum {j in 0..numColors-1}
		x[i,j] = 1;
