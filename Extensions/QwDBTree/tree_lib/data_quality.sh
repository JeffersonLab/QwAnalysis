#!/bin/bash

# Path to the database ROOT files
db_rootfiles_path="/group/qweak/www/html/private/db_rootfiles"
data_quality_path="/group/qweak/www/html/private/data_quality"

# Plots to be generated
plots="data_quality"


# Move to output directory
mkdir -p ${data_quality_path}
pushd ${data_quality_path}

# Loop over two runs
for run in 1 2 ; do
  if [ ${run} == "1" ] ; then wiens="1 2 3 4 5" ; fi
  if [ ${run} == "2" ] ; then wiens="6 7 8 9 10" ; fi

  # Loop over wiens in those runs
  for wien in $wiens ; do

    echo Run $run -- Wien $wien

    mkdir -p ${data_quality_path}/run${run}/wien${wien}
    pushd ${data_quality_path}/run${run}/wien${wien}

    # Check whether the database ROOT file exists
    file="${db_rootfiles_path}/run${run}/wien${wien}/parity/HYDROGEN-CELL_on_tree.root"
    if [ -f "${file}" ] ; then

      # Loop over plots
      for plot in $plots ; do

        $QWANALYSIS/Extensions/QwDBTree/tree_lib/${plots} "${file}"

      done # end of loop over plots

    fi # end of if file exists

    popd

  done # end of loop over wiens

done # end of loop over runs

popd # back to starting directory
