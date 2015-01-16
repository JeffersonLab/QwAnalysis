#!/bin/bash
stem=$1
BMOD_OUT_DIR="/net/data1/paschkedata1/bmod_out${stem}"
mkdir $BMOD_OUT_DIR 
mkdir ${BMOD_OUT_DIR}/plots 
mkdir ${BMOD_OUT_DIR}/regression
mkdir ${BMOD_OUT_DIR}/slopelists
mkdir ${BMOD_OUT_DIR}/diagnostics
mkdir ${BMOD_OUT_DIR}/slopes 
mkdir ${BMOD_OUT_DIR}/macrocycle_slopes 
mkdir ${BMOD_OUT_DIR}/coil_sensitivities
mkdir ${BMOD_OUT_DIR}/rootfiles

cp $BMOD_SRC/config/setup_mpsonly.config $BMOD_SRC/config/setup_mpsonly${stem}.config 
