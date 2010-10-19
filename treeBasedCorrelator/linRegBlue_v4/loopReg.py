#!/usr/bin/env python

""" 

Executing shell commande depending on params

"""
import os
from optparse import OptionParser
print "START JAN"
# -- command line options
usage = "usage: %prog [options]"
parser = OptionParser(usage)

parser.add_option("-r","--run-number", 
                  dest="RUN_NO", 
                  type="int", 
                  help="Qweak run number ", 
                  default = 5830)

parser.add_option("-n","--tot_events", 
                  dest="TOT_EVE", 
                  type="int", 
                  help="number of events to process ", 
                  default = 10001)

parser.add_option("-v","--verbose", 
                  dest="VERBOSE", 
                  action = "store_true", 
                  help="print all variables, matrices, and results", 
                  default = True)
#.... OLD
parser.add_option("-b","--block-size", 
                  dest="BLOCK_SIZE", 
                  type="choice", 
                  choices = ["8","12","16"], 
                  help="size of thread block (8, 12, or 16)", 
                  default = 16)

parser.add_option("-s","--spill", 
                  dest="SPILL", 
                  action="store_true", 
                  help="toggle spill", 
                  default = False)

#... END
(options,args) = parser.parse_args()

#################################
# Performance tuning parameters #
#################################

BLOCK_SIZE=int(options.BLOCK_SIZE)# value can be 8, 12, or 16
SPILL=options.SPILL # True = On, False = Off
VERBOSE=options.VERBOSE
RUN_NO=options.RUN_NO
TOT_EVE=options.TOT_EVE

if VERBOSE:
    print ">>> Executing with parameters:"
    print ">>> BLOCK_SIZE = %s" % BLOCK_SIZE
    print ">>> SPILL = %s" % SPILL
    print ">>>  RUN_NO = %d" % RUN_NO 
    print ">>>  TOT_EVE = %d" % TOT_EVE
 
if SPILL:
    SPILL="#define SPILL"
else:
    SPILL="#undef SPILL"

# define command line to be executed

# transfer host memory to device 
if VERBOSE:
    print ">>> Executing code ..."


os.system("rm -rf out")
os.mkdir("out")

cmd_string = "(./linRegBlue %d.000" % RUN_NO + " %d" %  TOT_EVE + " )"
print "exec1:%s" % cmd_string
os.system(cmd_string+">& out/log1")

cmd_string2 = "mv out/R%d_alphasNew.root" % RUN_NO + " out/R%d_alphas.root" % RUN_NO
print "exec2:%s" % cmd_string2
os.system(cmd_string2+">& out/log2")

print "exec3:%s" % cmd_string
os.system(cmd_string+">& out/log3")


cmd_string4 = "root -b -q plCor.C'(0,\"R%d.000\")'" % RUN_NO 
print "exec4:%s" % cmd_string4
os.system(cmd_string4+">& out/log4")

cmd_string5 = "cat out/R%d.000_page*ps "% RUN_NO +" | ps2pdf - out/allR%d.000.pdf " % RUN_NO
print "exec5:%s" % cmd_string5
os.system(cmd_string5+">& out/log5")

cmd_string6 = "rm -f out/R%d.000_page*ps "% RUN_NO 
print "exec6:%s" % cmd_string6
os.system(cmd_string6+">& out/log6")

cmd_string7 = "mv out outR%d"% RUN_NO 
print "exec7:%s" % cmd_string7
os.system(cmd_string7)


print  "scp -rp outR%d balewski@deltag5.lns.mit.edu:0x" % RUN_NO
