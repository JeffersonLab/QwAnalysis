#!/bin/bash

# Path to the database ROOT files
db_rootfiles_path="/group/qweak/www/html/private/db_rootfiles"
data_quality_path="/group/qweak/www/html/private/data_quality"
#db_rootfiles_path=~/db_rootfiles
#data_quality_path=.
data_quality_path="output"

# Plots to be generated
plots="mdallpmtavg bcmdd bcm"


# Move to output directory
mkdir -p ${data_quality_path}
pushd ${data_quality_path}

# Loop over the two runing periods
for run in 1 2 ; do

    # Loop over targets
    for target in HYDROGEN-CELL ; do

        # Loop over wiens in those runs
        if [ ${run} == "1" ] ; then wiens="1 2 3 4 5" ; fi
        if [ ${run} == "2" ] ; then wiens="6 7 8 9 10" ; fi
        for wien in $wiens ; do

            echo Run $run -- Target $target -- Wien $wien

            mkdir -p ${data_quality_path}/run${run}/${target}/
            pushd ${data_quality_path}/run${run}/${target}/

            # Check whether the database ROOT file exists
            file="${db_rootfiles_path}/run${run}/parity/${target}_on_tree.root"
            echo ${file}
            if [ -e ${file} ] ; then

                # Loop over plots
                for plot in $plots ; do

                    $QWANALYSIS/Extensions/QwDBLib/${plots} "${file}" $wien
                    #~/QwAnalysis/Extensions/QwDBLib/${plots} "${file}" $wien

                done # end of loop over plots

            else 
                echo "Did not find any rootfiles. This makes me a sad panda."

            fi # end of if file exists

            popd

        done # end of loop over wiens

    done # end of loop over targets

done # end of loop over runs

popd # back to starting directory
