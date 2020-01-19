#!/bin/sh

# Test classes
CLASSNAMES='Linux_KernelParameter Linux_ABIParameter Linux_FileSystemParameter Linux_VirtualMemoryParameter Linux_NetworkCoreParameter Linux_NetworkIPv4Parameter Linux_NetworkUnixParameter'

for CLASSNAME in $CLASSNAMES; do
   echo "Running tests for $CLASSNAME..."
   ./run.sh $CLASSNAME
done

