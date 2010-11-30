#!/usr/bin/env python

""" 
produce run list w/ time staps from the 
"""
import os

path="../stableOct/sort7100-7199-hw_sum/"
path="./web/"
print "read from ", path
f=open(path+"1","r")
print f
for line in f:
    #print line
    x,a,b,c,d=line.split('.')
    #print a,b
    #run=a[5:]
    run=a[9:]
    seg=int(b[:3])
    #print run, seg
    cmd="mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e \"select   filename,start_time from total  where run_number=%s"%run+" and segment=%d"%seg+"\" |grep dat"
    #print "exec1:%s" % cmd
    fin,fout=os.popen4(cmd)
    string=fout.read()
    #print string
    unixTime=string.split()[1]
    print run+".%03d"%seg+ " "+ unixTime
   #    continue;

    
    RUNiSEG=run+".%03d"%seg
    ioPath=path+"R%s"%RUNiSEG+"/"
    print "ioPath=",ioPath
    cmd_string12 = "root -b -q prCsvRecord.C'("+RUNiSEG+',%s,"'%unixTime+ioPath+'","'+ioPath+"\")'"
    print "exec12:%s" % cmd_string12
    os.system(cmd_string12)
    #break

  
#mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e "select   run_number, segment,size,start_time from analysis  where backup_status=11 and start_time>unix_timestamp('2010-10-28 12:00:00') and  size >1000000 order by start_time" | nl

#better

#mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e "select   run_number, filename,segment from analysis  where run_number=5999" | nl

