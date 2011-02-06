#!/usr/bin/env python

""" 

Executing shell commande depending on params

"""
import os
import time

lastRunSeg="9243.000"
sleepSec=300
fileSize=900000000
webPath="web/"

myDBdict={}

print "auto-loop regression, sleepSec=",sleepSec

#=======================================
def findDoneRuns():
    print "in findDoneRuns, last RunSeg=",lastRunSeg
    # print lastRunSeg.split(".")
    cmd="ls "+webPath+"|grep R*html"
    print cmd
    fin,fout=os.popen4(cmd)
    for line in fout:
        runSeg=line[1:-6]
        print 'done already:', runSeg, myDBdict.has_key(runSeg)
        if ( myDBdict.has_key(runSeg)):
            myDBdict[runSeg]='done'
        


#=======================================
def queryDB():
    print "in querry DB, last RunSeg=",lastRunSeg
    # print lastRunSeg.split(".")
    RunSegL=lastRunSeg.split(".")
    run0=RunSegL[0]
    seg0=RunSegL[1]
    print run0,seg0
    cmd="mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e \"select   filename,run_Number,segment,start_time from analysis  where backup_status>10 AND run_number>%s "%run0+ " AND size>%d"% fileSize+" order by filename\" |grep dat |grep pari"
    print cmd
    fin,fout=os.popen4(cmd)
    for line in fout:
        lineL=line.split()
        #print lineL[1],lineL[2]
        key=lineL[1]+".%03d"%int(lineL[2])
        #print key
        myDBdict[key]=0
        
#======================================
#    M A I N    L O O P
#======================================
x=0
while (1):
    print "loopCounter=", x
    myDBdict.clear()
    queryDB()
    findDoneRuns()
    kkk=sorted(myDBdict.keys())
    print "loopX=",x," myDict=",myDBdict
    #print kkk
    for key in kkk :
        if myDBdict[key]=='done':
            continue
        print key
        RunSegL=key.split(".")
        run0=RunSegL[0]
        seg0=RunSegL[1]
        print run0,seg0
        cmdSort = "(./oneRunReg.py -r %s"% run0  +" -s %s"%seg0 +" -n 100000)"
        print "exec1:%s" % cmdSort
        os.system(cmdSort)
        #os._exit(1)
    
    os.system("date");
    print "loopX=",x," sleep for ",sleepSec,"sec ***********************************"

    x+=1
    #os._exit(1)
    time.sleep(sleepSec)
    #if (x>10000):
    #    break
print "autoloop ended"

#OLD test: mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e "select filename,run_Number,segment,start_time from analysis where backup_status > 10 AND run_number > 7600  order by filename"

# mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e "select filename,backup_status,start_time from analysis where backup_status > 10 AND run_number > 7600  and filename like 'parity%' order by filename"
