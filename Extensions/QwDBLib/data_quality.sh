#!/bin/bash

# Path to the database ROOT files
db_rootfiles_path="/group/qweak/www/html/private/db_rootfiles"
data_quality_path="/group/qweak/www/html/private/data_quality"

# Plots to be generated
plots="parity/on/mdallpmtavg parity/on/uslumi parity/offoff/bcmdd parity/offoff/bcm"

# Targets to be generated
targets="HYDROGEN-CELL DS-4%-Aluminum US-2%-Aluminum"

# Runs to loop over
runs="run1 run2"

# Setup LD library path
basefile=`readlink -e $0`
basedir=`dirname $basefile`
export LD_LIBRARY_PATH=$basedir:${LD_LIBRARY_PATH}

# Move to output directory
mkdir -p "${data_quality_path}"
pushd "${data_quality_path}"

# Loop over the two running periods
for run in $runs ; do
    mkdir -p "${data_quality_path}/${run}"

    # Loop over targets
    for target in $targets ; do
        mkdir -p "${data_quality_path}/${run}/${target}"

        # Loop over wiens in those runs
        wiens="1 2 3 4 5 6 7 8 9 10"
        if [ ${run} == "run1" ] ; then wiens="1 2 3 4 5" ; fi
        if [ ${run} == "run2" ] ; then wiens="6 7 8 9 10" ; fi
        for wien in $wiens ; do

            echo Run $run -- Target $target -- Wien $wien

            mkdir -p "${data_quality_path}/${run}/${target}/wien${wien}"
            pushd "${data_quality_path}/${run}/${target}/wien${wien}"

            # Copy html template
            sed -e "s|%wien%|$wien|g" -e "s|%target%|$target|g" -e "s|%run%|$run|g" \
                $basedir/templates/wien.html \
                >  "${data_quality_path}/${run}/${target}/wien${wien}/index.html"

            # Loop over plots
            for plot in $plots ; do

                mkdir -p $plot
                pushd $plot

		type=`dirname $plot`
		heltype=`dirname $type`
                regtype=`basename $type`
                macro=`basename $plot`

                # Check whether the database ROOT file exists
                for file in ${db_rootfiles_path}/${run}/${heltype}/${target}_${regtype}_tree.root ; do
                    if [ -e "${file}" ] ; then

                        echo "Analyzing ${file} with  $QWANALYSIS/Extensions/QwDBLib/${macro}"
                        $QWANALYSIS/Extensions/QwDBLib/${macro} "${file}" "${wien}"

                    else
                        echo "Did not find any rootfiles in ${db_rootfiles_path}/${run}/${heltype}/${target}_${regtype}_tree.root. This makes me a sad panda."

                    fi # end of if file exists
                done

                popd

            done # end of loop over plots

            popd

        done # end of loop over wiens

    done # end of loop over targets

done # end of loop over runs

popd # back to starting directory
