#!/bin/sh

FILEROOT=/proc/sys/net/core
CLASSNAME=Linux_NetworkCoreParameter

OUTFILE=$CLASSNAME".instance"
rm -f $OUTFILE
if [[ $1 = "-rm" ]]; then exit 0; fi

# Find all the appropriate /proc files
FILES=$( find $FILEROOT -type f );
for FILE in $FILES; do
   # SettingID
   echo $FILE

   # Value, change tab separators to spaces
   cat $FILE | sed -e 's/[	]/ /g'

   # Edittable
   if [[ `stat -c "%A" $FILE` = ??w* ]]; then echo TRUE; else echo FALSE; fi
 
   echo 
done > $OUTFILE 
