#!/usr/bin/env python

""" 

Executing shell commande depending on params

"""
import os

#f=open("runListOct16-17","r")
f=open("runsNovember/runList6")
#f=open("rr6","r")
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

    #break
#print f.readline()

#mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e "select   run_number, segment,size,start_time from analysis  where backup_status=11 and start_time>unix_timestamp('2010-10-28 12:00:00') and  size >1000000 order by start_time" | nl

#better

#mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e "select   run_number, filename,segment from analysis  where run_number=5999" | nl

