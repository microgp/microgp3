#!/bin/bash

FILE=$1
TMP=tmpfile.$$

echo Updating $FILE

./UpdateTeam.pl <$FILE >$TMP
mv $TMP $FILE
