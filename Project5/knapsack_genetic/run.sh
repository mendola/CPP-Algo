#!/bin/bash

# Allows pathing to be independant of cwd script is invoked from
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
IPATH="$SCRIPTPATH/input"
OPATH="$SCRIPTPATH/output"

for input in $(ls $IPATH/*.input); do
	BASENAME=$(basename -- "$input")
	FILENAME="${BASENAME%.*}"
        printf "Running $FILENAME\n"
<<<<<<< Updated upstream
	$SCRIPTPATH/tsack $IPATH/$FILENAME.input > $OPATH/$FILENAME.output
=======
	$SCRIPTPATH/p1a $IPATH/$FILENAME.input > $OPATH/$FILENAME.output
>>>>>>> Stashed changes
done
