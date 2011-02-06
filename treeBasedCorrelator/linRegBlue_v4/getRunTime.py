#!/usr/bin/env python

""" 
produce run list w/ time staps from the 
"""
import os
from  JanQwOnlLib import *

#os._exit(1)

path="./web/"
print "read from ", path
f=open(path+"lx1","r")
#print f
k=1
t0=0
for line in f:
    k=k+1
    print line

    x,a,b,c,y=line.split('.')
    print "a=",a,"b=",b
    #run=a[5:]
    run=a[-4:]
    seg=int(b[:3])
    #run="7110"
    #seg=0
    print run, seg
    #break
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
    print "------------------"
    
    miscVal=', '.join(target)+", "+unixTime+", "+mmddyy+" "+hhmm+", "+url+", "+text.replace(',',';')
    iPath=path+"R%s"%RUNiSEG+"/"
    oPath=path+"4damon/"
    print "iPath=",iPath, " oPath=",oPath
    cmd_string12 = "root -b -q prCsvRecordTwo.C'(2,"+RUNiSEG+',"%s","'%miscVal+iPath+'","'+oPath+"\")'"
    print "exec12:%s" % cmd_string12
    os.system(cmd_string12)
    break

    if k>10000:
        break
    continue

  
