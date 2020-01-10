#!/bin/sh

FILEROOT=/proc/sys/fs
CLASSNAME=Linux_FileSystemParameter

OUTFILE=$CLASSNAME".instance"
rm -f $OUTFILE
if [[ $1 = "-rm" ]]; then exit 0; fi

# Find all the appropriate /proc files
FILES=$( find $FILEROOT -type f );
for FILE in $FILES; do
   # SettingID
   echo $FILE

   # if "file is Readable" then "Value, change tab separators to spaces"
   if [[ `stat -c "%A" $FILE` = ?r* ]]; then cat $FILE | sed -e 's/[	]/ /g'; else echo; fi

   # Edittable
   if [[ `stat -c "%A" $FILE` = ??w* ]]; then echo TRUE; else echo FALSE; fi
 
   echo 
done > $OUTFILE 
