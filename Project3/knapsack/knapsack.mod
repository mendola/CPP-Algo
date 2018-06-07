# Parameters from the input file
param n;
param costBound;
param value {i in 0..n-1};
param cost {i in 0..n-1};

# Variables to be solved for
var x {i in 0..n-1} binary;

# ILP formulation
maximize Value:
sum {i in 0..n-1} x[i]*value[i];

subject to Cost:
	sum {j in 0..n-1} x[j]*cost[j] <= costBound;
