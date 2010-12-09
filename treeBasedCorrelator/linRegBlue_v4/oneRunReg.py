#!/usr/bin/env python

""" 
 ./linRegBlue 5844 0 10000 blueReg.conf blueR5844.000.slope.root

 scp -rp -i ~/balewski/keys/id_rsa-ifarml4 xxx balewski@ifarml4:/group/qweak/www/html/onlineRegression/A
"""
import os
from optparse import OptionParser
import datetime
from JanQwOnlLib import *

print "START ONE run segment"
wrkDir="./out/"
safeDir="./web/"
destWeb="/group/qweak/www/html/onlineRegression/autoA/"
motherCsvMD="mother_online_MD.csv"
motherCsvLumi="mother_online_LUMI.csv"

# -- command line options
usage = "usage: %prog [options]"
parser = OptionParser(usage)

parser.add_option("-r","--run-number", 
                  dest="RUN_NO", 
                  type="int", 
                  help="Qweak run number ", 
                  default = 5830)

parser.add_option("-s","--run-segment", 
                  dest="RUN_SEG", 
                  type="int", 
                  help="Qweak run segment ", 
                  default = 001)

parser.add_option("-n","--tot_events", 
                  dest="TOT_EVE", 
                  type="int", 
                  help="number of events to process ", 
                  default = 10000)

parser.add_option("-c","--cuts", 
                  dest="CUTS", 
                  action = "store_true", 
                  help="use custom cuts",
                  default = False)

#... END
(options,args) = parser.parse_args()

#################################
# Performance tuning parameters #
#################################

RUN_NO=options.RUN_NO
RUN_SEG=options.RUN_SEG
TOT_EVE=options.TOT_EVE
CUTS=options.CUTS
RUNiSEG="%d." %RUN_NO + "%03d" % RUN_SEG

print ">>> Executing with parameters:"
print ">>>  RUN.SEG = %s" % RUNiSEG 
print ">>>  TOT_EVE = %d" % TOT_EVE
print ">>>  custom CUTS = %s" % CUTS

################################
#  exporting HTML line
################################

def  exportHtml(inpFile,outFile,pdfURL) :
    print inpFile,"--> export HTML -->", outFile

    # first gather information from web pages
    
    # .... get HClog URL
    err,text,id,mmddyy,hhmm,urlHClog=HClog_r(str(RUN_NO))
    unixTime=Db2UnixTime_r_s(str(RUN_NO),str(RUN_SEG))
    target=whatTarget(urlHClog)

    print "target(s)=",target
    ioPath=wrkDir
    miscVal=', '.join(target)+", "+unixTime+", "+mmddyy+" "+hhmm+", "+urlHClog+", "+text.replace(',',';')
    print "ioPath=",ioPath

    #..... generate CVS record for MD
    cmd_string12 = "root -b -q prCsvRecordTwo.C'(0,"+RUNiSEG+',"%s","'%miscVal+ioPath+'","'+ioPath+"\")'"
    print "exec12:%s" % cmd_string12
    fin,fout=os.popen4(cmd_string12)
    fullLog=fout.readlines()
    print fullLog

    print "..... generate DB record" 
    cmd_string12a = "root -b -q prCsvRecordTwo.C'(2,"+RUNiSEG+',"%s","'%miscVal+ioPath+'","'+ioPath+"\")'"
    print "exec12a:%s" % cmd_string12a
    fin,fout=os.popen4(cmd_string12a)
    fullLog=fout.readlines()
    print fullLog


    #..... generate CVS record for LUMIs
    cmd_string12b = "root -b -q prCsvRecordTwo.C'(1,"+RUNiSEG+',"%s","'%miscVal+ioPath+'","'+ioPath+"\")'"
    print "exec12b:%s" % cmd_string12b
    fin,fout=os.popen4(cmd_string12b)
    fullLog=fout.readlines()
    print fullLog

    fullLogL=' '.join(fullLog).split("#regresRMS")
    regRmsStr=fullLogL[1]
    print '1$$$$$$$$$$$$',regRmsStr

    fullLogL=fullLogL[2].split("#bcmRMS")
    bcmRmsStr=fullLogL[1]
    print '2$$$$$$$$$$$$',bcmRmsStr

    nonrRmsStr=fullLogL[2].split("#nonregresRMS")[1]
    print '3$$$$$$$$$$$$',nonrRmsStr

     
    f=open(outFile,'w')
    #print f
    f.write("<!--   new run  R%s ------------>\n" %RUNiSEG)
    f.write("<tr> <td  rowspan=2> "+ pdfURL)

    #...  get # of events
    cmd= "grep \#totEve "+inpFile+" | cut -f2 -d\ "
    fin,fout=os.popen4(cmd)
    totEve=int(fout.read())
    cmd= "grep \#seenEve "+inpFile+" | cut -f2 -d\ "
    fin,fout=os.popen4(cmd)
    seenEve=int(fout.read())
    f.write("<td  rowspan=2 > %d "%seenEve  +"<br><font size=\"-1\">  %d\n" %totEve)
    f.write("<td  rowspan=2 > "+ target[0]+"<br> <font size=\"-1\"> "+target[1]+","+target[2]+"\n")
    f.write(regRmsStr+"\n")
    f.write(bcmRmsStr+"\n")
    #.....  get Sorting date'
    now=datetime.datetime.now()
    #str(now)
    #print now
    f.write("<td  rowspan=2 >"+now.strftime("%Y-%m-%d %H:%M:%S\n"))
    f.write("<td  rowspan=2 >"+mmddyy+" "+hhmm+"\n")
    f.write('<td  rowspan=2 ><font size="-1"> <a href="'+urlHClog+'"> '+  text+'</a>\n')     
    f.write(nonrRmsStr+"\n")
#mmddyy,hhmm,url
    f.close()

################################
#  perform regression twice
################################

def  doubleRegress():
    print "do double regression"
    os.system("date");
    os.mkdir("out")

    cutsStr=" blueReg.conf"
    if CUTS:
        cutsStr=" blueRegR%d"%RUN_NO +".conf"
    cmdSort = "(./linRegBlue %d" % RUN_NO + " %03d " % RUN_SEG + " %d" %  TOT_EVE + cutsStr+ " )"
    print "exec1:%s" % cmdSort
    os.system(cmdSort+">& out/logS1")

    print " verify sorting job did not aborted"
    os.system("date");
    cmd= "grep \#success out/logS1 "
    fin,fout=os.popen4(cmd)
    line=fout.read()
    print line.find("suc")
    if ( line.find("#success") <0 ):
        return 1
           
    cmd_string2 = "mv out/blueR"+ RUNiSEG + "new.slope.root out/blueR"+ RUNiSEG + ".slope.root"
    print "exec2:%s" % cmd_string2
    os.system(cmd_string2+">& out/log2")

    cmdSort2 = "(./linRegBlue %d" % RUN_NO + " %03d " % RUN_SEG + " %d" %  TOT_EVE + cutsStr+ " blueR"+ RUNiSEG + ".slope.root )"
    print "exec3:%s" % cmdSort2
    os.system(cmdSort2+">& out/logS2")
    # verify job did not aborted
    cmd= "grep \#success out/logS2 "
    fin,fout=os.popen4(cmd)
    line=fout.read()
    print line.find("suc")
    if ( line.find("#success") <0 ):
        return 2
           
    return 0
    
################################
#  ABORT  script
################################

def  abortMe(code):
    print "JB ABORT PYTHON , code=%d "%code + " run=%s"%RUNiSEG
    #.....  get Sorting date'
    now=datetime.datetime.now()
    
    cmd_stringAb = 'echo "<tr> <td>R%s'%RUNiSEG+' <td><td colspan=5 > aborted linear  regression <td  colspan=8 > sorted: '+now.strftime("%Y-%m-%d %H:%M:%S")+'">'+safeDir+"R%s.html"%RUNiSEG
    print "execAb:%s" % cmd_stringAb
    os.system(cmd_stringAb)

    os._exit(1)

#############  M A I N  ###############
print ">>> Executing code ..."

# define all final pathes , names ......
outDirName="R%s/"%RUNiSEG
pdfName="regR%s.pdf"%RUNiSEG
pdfURL="<A HREF=\""+outDirName+pdfName+"\"> R%s"%RUNiSEG+"</A>" + "<br> <A HREF=\""+outDirName+"logS2\"> regr.log </A>"
finHtml=safeDir+"R%s"%RUNiSEG+".html"

#print pdfURL
#os._exit(1)
retCode=0

if 1:
    os.system("rm -rf outOld")
    os.system("mv out outOld")
    retCode=doubleRegress()
#os._exit(1)

if (retCode ):
    abortMe(retCode)
   

cmd_string4 = "root -b -q plCor.C'(0,\"R%s\")'" % RUNiSEG 
print "exec4:%s" % cmd_string4
os.system(cmd_string4+">& out/log4")
#os._exit(1)


cmd_string5 = "cat out/R%s_page*ps "% RUNiSEG +" | ps2pdf - out/"+pdfName
print "exec5:%s" % cmd_string5
os.system(cmd_string5+">& out/log5")


cmd_string6 = "rm -f out/R%s_page*ps "% RUNiSEG 
print "exec6:%s" % cmd_string6
os.system(cmd_string6+">& out/log6")
 

exportHtml("out/logS2",finHtml ,pdfURL)

cmd_string7 = "mv out "+safeDir+outDirName 
print "exec7:%s" % cmd_string7
os.system(cmd_string7)

#..... order all HTML in reverse order
cmd_string8 = "ls -r "+safeDir+"R*html "
print "exec8:%s" % cmd_string8
fin,fout=os.popen4(cmd_string8)
line=fout.read()
#print "cc1 ",line
line2=line.replace("\n"," ");
#print "cc1 ",line2

cmd_string9 = "rm -f "+safeDir+"index.html; cat "+safeDir+"top.html "+line2+" "+safeDir+"bottom.html > "+safeDir+"index.html "
#print "exec9:%s" % cmd_string9
os.system(cmd_string9)
#os._exit(1)


#...  merrge all MD CSV in to one big table
cmd_string13 = "rm -f "+safeDir+motherCsvMD+"; cat "+safeDir+"R*/*MD.csv  > "+safeDir+motherCsvMD
print "exec13:%s" % cmd_string13
os.system(cmd_string13)

#...  merrge all LUMI CSV in to one big table
cmd_string14 = "rm -f "+safeDir+motherCsvLumi+"; cat "+safeDir+"R*/*LUMI.csv  > "+safeDir+motherCsvLumi
print "exec14:%s" % cmd_string14
os.system(cmd_string14)


#...... copy all to final web page
cmd_string100 = "scp -rp -i ~/balewski/keys/id_rsa-ifarml4  "+safeDir+"/index.html  "+safeDir+outDirName+" "+safeDir+motherCsvMD+" "+safeDir+motherCsvLumi+" balewski@ifarml4:"+destWeb

print "exec100:%s" % cmd_string100
os.system(cmd_string100)
os.system("date")
os._exit(1)#tmp

#..... regenarate plots, after CSV table is rebuild
cmd_string110 = "(./doAutoPlots.sh)"
print "exec110:%s" % cmd_string110
os.system(cmd_string110)
          
os._exit(1)


# define command line to be executed

# transfer host memory to device 

#print  "scp -rp outR%d balewski@deltag5.lns.mit.edu:0x" % RUN_NO
#os._exit(1)
#cmd_string9 = "scp -rp -i ~/balewski/keys/id_dsa-janDeltag5 index.html "+outDirName+" balewski@deltag5.lns.mit.edu:Sites/QweakA/"
