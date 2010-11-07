#!/usr/bin/env python

""" 

Executing shell commande depending on params

"""
import os
import time

lastRunSeg="6600.001"
sleepSec=300
fileSize=100000000
webPath="web/"

myDBdict={}

print "auto-loop regression, sleepSec=",sleepSec

#=======================================
def findDoneRuns():
    print "in findDOneRuns, last RunSeg=",lastRunSeg
    # print lastRunSeg.split(".")
    cmd="ls "+webPath+"|grep R"
    print cmd
    fin,fout=os.popen4(cmd)
    for line in fout:
        runSeg=line[1:-1]
        print runSeg, myDBdict.has_key(runSeg)
        if ( myDBdict.has_key(runSeg)):
            myDBdict[runSeg]='done'
        


#=======================================
def queryDB():
    myDBdict.clear()
    print "in querry DB, last RunSeg=",lastRunSeg
    # print lastRunSeg.split(".")
    RunSegL=lastRunSeg.split(".")
    run0=RunSegL[0]
    seg0=RunSegL[1]
    print run0,seg0
    cmd="mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e \"select   filename,run_Number,segment,start_time from analysis  where backup_status>10 AND run_number>%s "%run0+ " AND size>%d"% fileSize+" order by filename\" |grep dat"
    print cmd
    fin,fout=os.popen4(cmd)
    for line in fout:
        lineL=line.split()
        #print lineL[1],lineL[2]
        key=lineL[1]+".%03d"%int(lineL[2])
        print key
        myDBdict[key]=0
        
#======================================
#    M A I N    L O O P
#======================================
x=0
while (1):
    print "loopCounter=", x
    queryDB()
    findDoneRuns()
    kkk=sorted(myDBdict.keys())
    print kkk
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
    
    print "myDict=",myDBdict
    x+=1
    #os._exit(1)
    time.sleep(sleepSec)
    if (x>10):
        break
print "autoloop ended"
