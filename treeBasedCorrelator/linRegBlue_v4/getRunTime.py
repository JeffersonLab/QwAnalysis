#!/usr/bin/env python

""" 
produce run list w/ time staps from the 
"""
import os
from  JanQwOnlLib import *

#os._exit(1)

path="./webX/"
print "read from ", path
f=open(path+"12","r")
#print f
k=1
t0=0
for line in f:
    k=k+1
    #print line
    x,a,b,c,d=line.split('.')
    #print a,b
    #run=a[5:]
    run=a[9:]
    seg=int(b[:3])
    #print run, seg
    if seg<0:
        continue
    if k<0:
        continue
    RUNiSEG=run+".%03d"%seg
    unixTime=Db2UnixTime_r_s(run,seg)
    err,text,id,mmddyy,hhmm,url=HClog_r(run)
    if t0==0:
        t0=int(unixTime)
    delSec=int(unixTime)-t0    

    print k," END: R=", RUNiSEG,
    #print "unixT=",unixTime,delSec,
    #print "  HClogUrl=",text
    target=whatTarget(url)
    print "target(s)=",target
    # print "------------------"
    
    miscVal=', '.join(target)+", "+unixTime+", "+mmddyy+" "+hhmm+", "+url+", "+text.replace(',',';')
    ioPath=path+"R%s"%RUNiSEG+"/"
    print "ioPath=",ioPath
    cmd_string12 = "root -b -q prCsvRecordTwo.C'(1,"+RUNiSEG+',"%s","'%miscVal+ioPath+'","'+ioPath+"\")'"
    print "exec12:%s" % cmd_string12
    os.system(cmd_string12)
    #break

    if k>2000 or len(target)<0:
        break
    continue

  
#mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e "select   run_number, segment,size,start_time from analysis  where backup_status=11 and start_time>unix_timestamp('2010-10-28 12:00:00') and  size >1000000 order by start_time" | nl

#better

#mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e "select   run_number, filename,segment from analysis  where run_number=5999" | nl

