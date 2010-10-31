#!/usr/bin/env python

""" 

Executing shell commande depending on params

"""
import os

f=open("runListOct18","r")
#f=open("runListBoil.txt","r")
#f=open("rr3","r")
print f
for line in f:
    print line
    run,cn1,cn2,mc=line.split()
    n1=int(cn1)
    n2=int(cn2)
    print run,n1,n2,mc
    cut=""
    if mc=="c":
        cut=" -c "
    for seg in range(n1,n2+1):    
        cmdSort = "(./oneRunReg.py -r %s"% run  +" -s %03d"%seg +cut+" -n 100000)"
        print "exec1:%s" % cmdSort
        os.system(cmdSort)

    break
#print f.readline()

