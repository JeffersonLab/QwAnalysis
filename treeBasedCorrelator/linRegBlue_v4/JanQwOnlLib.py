#!/usr/bin/env python
import os

"""
Author: Jan Balewski, MIT
Library of utility functions for Qweak online event processing, 
"""

################################
#  
################################

def  Db2UnixTime_r_s(run,seg):
    #print "Db2RunTime2: run,seg=",run,seg
    cmd="mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e \"select   filename,start_time from total  where run_number=%s"%run+" and segment=%s"%seg+"\" |grep dat"
    #print "exec1:%s" % cmd
    fin,fout=os.popen4(cmd)
    string=fout.read()
    #print string
    unixTime=string.split()[1]
    #print run+".%03d"%seg+ " "+ unixTime
    return unixTime


################################
#  
################################

def  HClog_r(run):
    #print "HClog_r: run=",run
    badAnswer=1,"HClogURL_NotFound2_R"+run,0,0,0,0
    if int(run) < 7717 :
        yymm="1011"
    elif int(run) < 8540 :
        yymm="1012"
    elif int(run) < 9595 :
        yymm="1101"
    elif int(run) < 10270 :
        yymm="1102"
    else:
        yymm="1103"
    hclogWeb="https://hallcweb.jlab.org/hclog/"+yymm+"_archive/"

    # .... get HClog URL  
    cmd= "wget --no-check-certificate "+hclogWeb+"logdir_auto.html -O - | grep \"Run "+run+"\" | tail -n  1 "
    print "cmd_wget=",cmd
    fin,fout=os.popen4(cmd)
    myURL1=fout.readlines()[-1]
    if len(myURL1)<2 :
        return badAnswer,"aa"
    #print "aa1 ",myURL1,len(myURL1)
    # in rare case there are multiple URLs in the same line
    myURL2=myURL1.split("<LI>")[-1]
    #print "aa2 ",myURL2
    xxL=myURL2.split('"')
    url=hclogWeb+xxL[1]
    body1=xxL[2][1:].split("</a>")[0]
    body=body1.split("</A>")[0]
    bodyL=body.split()
    id=bodyL[0]
    mmddyy=bodyL[1]
    hhmm=bodyL[2]
    #print myURL1.find("cdaq")>=0
    #print "bodyL=",bodyL
    if bodyL[3]!="cdaq" :
        return badAnswer
    text=body.split(":")[2]
    #myURL=myURL2.replace('HREF="','HREF="'+hclogWeb)
    #print "aa3",myURL
   
    return 0,text,id,mmddyy,hhmm,url

################################
#  
################################

def  whatTarget(url):
    #print "inp url=",url
        # .... get HClog URL  
    cmd= 'wget --no-check-certificate '+url+' -O -' 
    #print "cmd_wget=",cmd
    fin,fout=os.popen4(cmd)
    linesA=' '.join(fout.readlines())
    if len(linesA)<2 :
        return "noTargetInfo"
    lines=linesA
    line1=lines.split("Qweak Target")[1]
    #print 'line1=',line1
    target=line1.split("<td>")[1][:-5]
    target=target.replace('Aluminum','Alu')
    target=target.replace('HYDROGEN-CELL','LH2-Cell')
    target=target.replace('HOME=OutOfBeam','HOME=Out')

    lines=linesA
    line1=lines.split("Raster (width) X mm")[1]
    xRast=line1.split("<td>")[1][:-5]

    lines=linesA
    line1=lines.split("Raster (height) Y mm")[1]
    yRast=line1.split("<td>")[1][:-5]

    lines=linesA
    line1=lines.split("Half Wave Plate")[1]
    hwp='hwp'+line1.split("<td>")[1][:-5]

    #print 'line2=',xRast
    return target,xRast+'x'+yRast,hwp

################################
#  
################################

def  whatTargetXXX(text):
    #print "inp text=",text
    myTarg=set()
    targDict={'LH2':['Boil','LH2'],
              'Al':['% al', '% Al','US AL','Al 4%','% US Al'],
              'test':['DAQ test','pedestal','commissioning'],
              'none':['Target out','no target'],
              'C12':['% C12']
              }
    
    for key in targDict.keys():
        #print "k=",key
        for phraze in targDict[key]:
            #print phraze, phraze in text
            if phraze in text:
                myTarg.add(key)



    return myTarg   
            
