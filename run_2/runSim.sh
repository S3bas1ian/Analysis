#/bin/bash

#run anywhere
#designed for use with fivi simulation

#five arguments are required
#arg1: number of events to simulate in each block
#arg2: first block to simulate
#arg3: number of blocks to simulate
#arg4: number of threads to use
#arg5: (absolute) path to file containing necessary parameters for simulation

#total number of simulated events is given by arg1 * (arg3 - arg2 + 1)

#only run if all required args are given
if [ $# -ne 5 ]; then
    echo "usage: ./runSim.sh [block size] [first block] [block count] [thread count] [settings file]"
    echo $#
else
    #save input args with more descriptive names
    blksize=$1
    firstblk=$2
    blkcount=$3
    threadcount=$4
    simdata=$5
    filemax=$threadcount
    #account for zero-indexing
    ((blkcount--))
    ((filemax--))
    
    time=`date +%s`
    
    #find path to executable
    line=`cat $simdata | grep ^EXE_PATH`
    
    tokens=( $line )

    exepath=${tokens[1]}
    
    #move to output directory
    line=`cat $simdata | grep ^OUTPUT_PATH`
    
    tokens=( $line )

    outpath=${tokens[1]}
    
    cd $outpath
    
    mkdir -p data
    
    #do the following for each block
    for i in `seq $firstblk $blkcount`; do
        #calculate starting point for each block
        ((firstev=i*blksize))
        
        #run geant
        $exepath "$simdata" -b "$i" -z "$blksize" -t "$threadcount"
        
        mkdir "$firstev"
        
        mv output* "$firstev"/
        
        mv "$firstev"/ data/
        
    done
    
fi

