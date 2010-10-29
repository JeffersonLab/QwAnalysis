#!/usr/bin/env python

""" 

Executing shell commande depending on params

"""
import os

f=open("4ju/runListJ","r")
print f
for line in f:
    print line
    run,seg=line.split()
    print run
    cmdSort = "(./oneRunReg.py -r %s"% run +" -s %s"%seg +" -n 100000)"
    print "exec1:%s" % cmdSort
    #  os.system(cmdSort+">& LogLoop")
    os.system(cmdSort)



    #break
#print f.readline()
#print f.readline()
