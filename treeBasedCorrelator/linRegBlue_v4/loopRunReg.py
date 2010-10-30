#!/usr/bin/env python

""" 

Executing shell commande depending on params

"""
import os

f=open("runListBoil.txt","r")
#f=open("aa1","r")
print f
for line in f:
    print line
    run,seg=line.split()
    print run
    cut=""
    if seg=="c":
        cut=" -c "
    cmdSort = "(./oneRunReg.py -r %s"% run  +" -s 000 "+cut+" -n 100000)"
    print "exec1:%s" % cmdSort
    os.system(cmdSort)

    #break
#print f.readline()

