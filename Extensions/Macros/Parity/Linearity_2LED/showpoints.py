#!/usr/bin/env pylab
import ROOT
from matplotlib.pyplot import *

class pmtdata:
    def __init__(self, tree, cut):
        n = tree.Draw("pmt_ac : time : std", cut, "goff")
        pmtdata         = tree.GetV1()
        time            = tree.GetV2()
        std             = tree.GetV3()
        self.pmtdata    = [ pmtdata[i]  for i in xrange(n) ]
        self.time       = [ time[i]     for i in xrange(n) ]
        self.std        = [ std[i]      for i in xrange(n) ]
        self.draw()

    def draw(self, *args, **kwargs):
        if not len(args): args = ('.',)
        p = plot(self.time, self.pmtdata, *args, **kwargs)
        return p

f = ROOT.TFile("run18995.Mps_Tree_serial.root")
mps = f.Mps_Tree_serial
alldata = pmtdata(mps, "cleandata && time < 53.25" )
close()
alldata.draw(".-")

ac_on  = pmtdata(mps,
                 "cleandata && time < 53.25 && std < 0.04 && pmt_ac > 0.25" )
ac_off = pmtdata(mps,
                 "cleandata && time < 53.25 && std < 0.04 && pmt_ac <= 0.25" )

gca().get_children()[3].set_markersize(12)
gca().get_children()[4].set_markersize(12)
v = (53.20, 53.23, 0.15, 0.35)
axis(v)
legend( ('all points', 'ac_on', 'ac_off', ))
xlabel('time (seconds)')
ylabel('pmt_ac (volts)')
title("qwk_md7neg (pmt_ac), from run 18995")
gcf().savefig("ac_cuts.png")
