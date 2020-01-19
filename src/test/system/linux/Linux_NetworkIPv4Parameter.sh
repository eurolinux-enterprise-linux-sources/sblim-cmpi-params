#!/bin/sh

FILEROOT=/proc/sys/net/ipv4
CLASSNAME=Linux_NetworkIPv4Parameter

OUTFILE=$CLASSNAME".instance"
rm -f $OUTFILE
if [[ $1 = "-rm" ]]; then exit 0; fi

# Find all the appropriate /proc files
FILES=$( find $FILEROOT -type f );
for FILE in $FILES; do
   # SettingID
   echo $FILE

   # Value, change tab separators to spaces
   VALUE=`more $FILE | sed -e 's/[	]/ /g'`
   if [[ -n $VALUE ]]; then echo $VALUE; else echo; fi

   # Edittable
   if [[ `stat -c "%A" $FILE` = ??w* ]]; then echo TRUE; else echo FALSE; fi
 
   echo 
done > $OUTFILE 
