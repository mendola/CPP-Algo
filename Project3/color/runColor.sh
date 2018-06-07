#!/bin/bash

IPATH="data"
OPATH="results"

for input in $(ls $IPATH/*.dat);
do
	echo "Input is: $input"
	BASENAME=$(basename --suffix=".dat" "$input")
	FILENAME="($BASENAME%.*)"
	printf "Running $BASENAME\n" #$FILENAME\n"
	
	TXT="option solver cplex; \
option cplex_options 'time=600';
model color.mod;
data data/color/$BASENAME.dat;
solve;
display numNodes;
display numColors;
display x;
display Conflicts;"
	RUNFILE=$BASENAME.run
	SAVEFILE=$OPATH/$BASENAME.output
	echo "RUNFILE is: $RUNFILE"
	echo "$TXT" > $RUNFILE
	echo "$SAVEFILE"
	touch $SAVEFILE
	ampl $RUNFILE > $SAVEFILE
	rm $RUNFILE
done
