#!/usr/bin/env pylab
## probably some numpy/matplotlib import statements should go here ...

import os
from numpy import *
from matplotlib.pyplot import *

dirname = os.environ['QWANALYSIS'] + '/Extensions/Macros/Parity/Linearity_2LED/'
filename = 'run18995.qwk_md7pos.txt'

class results:
    def __init__(self, f = filename, d = dirname, label='') :
        self.dirname = d
        self.filename = f
        self.label = label
        \
            self.ac_baseline  , self.ac_amplitude  , \
            self.dc_baseline  , self.dc_amplitude  , \
            self.ac_baseline_e, self.ac_amplitude_e, \
            self.dc_baseline_e, self.dc_amplitude_e, \
        = loadtxt( dirname + f, unpack=True )

    def makeplot(self):

        subplot(3,1,2)
        plot( self.dc_baseline, self.ac_amplitude, '.-' )
        ylabel( 'ac_amplitude (V)' )

        subplot(3,1,3)
        plot( self.dc_baseline, self.dc_amplitude, '.-' )
        ylabel( 'dc_amplitude (V)' )
        xlabel( 'dc_baseline (V)' )

        subplot(3,1,1)
        plot( self.dc_baseline, self.ac_baseline, '.-', label=self.label )
        # errorbar( dc_baseline, ac_baseline, ac_baseline_e, dc_baseline_e,
        # '.', capsize=0 )
        ylabel( 'ac_baseline (V)' )
        legend( loc='best' )

class properties: "Details for the run associated with one PMT"
class all_runs:
    def __init__(self):
        ## reverse the hash: use pmt names as keys
        import runlist
        self.runlist = { runlist.runlist[key]: key for key in runlist.runlist }

        self.pmt = {}
        for pmt in sort( self.runlist.keys() ):
            run_number = self.runlist[ pmt ]

            self.pmt[ pmt ] = properties()
            self.pmt[ pmt ].run_number = run_number
            self.pmt[ pmt ].result = results(
                'run{}.{}.dat'.format(run_number, pmt),
                label = '{}, run {}'.format(pmt, run_number),
                )

    def makeplot(self):
        fig = figure( figsize=(8.5,11) )

        for pmt in sort( self.pmt.keys() ):
            self.pmt[pmt].result.makeplot()
