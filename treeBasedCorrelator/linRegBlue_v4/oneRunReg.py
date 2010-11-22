#!/usr/bin/env python

""" 
 ./linRegBlue 5844 0 10000 blueReg.conf blueR5844.000.slope.root
 
"""
import os
from optparse import OptionParser
import datetime
print "START JAN"

wrkDir="./out/"
safeDir="./web/"
destWeb="/group/qweak/www/html/onlineRegression/autoA/"
#destWeb="/group/qweak/www/html/onlineRegression/B/"
hclogWeb="https://hallcweb.jlab.org/hclog/1011_archive/"
motherCsv="mother_online_summary.csv"


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

parser.add_option("-v","--verbose", 
                  dest="VERBOSE", 
                  action = "store_true", 
                  help="print all variables, matrices, and results", 
                  default = True)

parser.add_option("-c","--cuts", 
                  dest="CUTS", 
                  action = "store_true", 
                  help="use custom cuts",
                  default = False)

parser.add_option("-m","--mock_regress", 
                  dest="MOCK_REGRESS", 
                  action = "store_true", 
                  help="skip regression and rename back existing output directory, post on the web", 
                  default = False)

#... END
(options,args) = parser.parse_args()

#################################
# Performance tuning parameters #
#################################

VERBOSE=options.VERBOSE
RUN_NO=options.RUN_NO
RUN_SEG=options.RUN_SEG
TOT_EVE=options.TOT_EVE
MOCK_REGRESS = options.MOCK_REGRESS
CUTS=options.CUTS
RUNiSEG="%d." %RUN_NO + "%03d" % RUN_SEG


if VERBOSE:
    print ">>> Executing with parameters:"
    print ">>>  MOCK_REGRESS = %s " % MOCK_REGRESS
    print ">>>  RUN.SEG = %s" % RUNiSEG 
    print ">>>  TOT_EVE = %d" % TOT_EVE
    print ">>>  custom CUTS = %s" % CUTS

################################
#  exporting HTML line
################################

def  exportHtml(inpFile,outFile,pdfURL) :
    print inpFile,"--> export HTML -->", outFile
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
    f.write("<td  rowspan=2 > %d " %totEve +"<br> %d\n" %seenEve)
    # .... get regRMS
    cmd= "grep \#regresRMS "+inpFile+" | cut -f2 -d,"
    fin,fout=os.popen4(cmd)
    regRmsStr=fout.read()
    f.write("<th> regres "+ regRmsStr+"\n")
    #.....  get Sorting date'
    now=datetime.datetime.now()
    #str(now)
    #print now
    f.write("<td  rowspan=2 >"+now.strftime("%Y-%m-%d %H:%M:%S\n"))

    # .... get HClog URL  
    cmd= "wget --no-check-certificate "+hclogWeb+"logdir_auto.html -O - | grep \"Run %d"%RUN_NO+"\" | tail -n  1 >" +wrkDir+"logHC"
    os.system(cmd)
    f1=open(wrkDir+"logHC","r")
      
    myURL1=f1.readlines()[-1]
    f1.close()
    #print "aa1 ",myURL1
    # in rare case there are multiple URLs in the same line
    myURL2=myURL1.split("<LI>")[-1]
    print "aa2 ",myURL2
    
    myURL=myURL2.replace('HREF="','HREF="'+hclogWeb)
    #print "bb",myURL
    f.write("<td  rowspan=2 > "+ myURL+"\n")     

    # .... get nonregr RMS
    cmd= "grep \#inputRMS "+inpFile+" | cut -f2 -d,"
    fin,fout=os.popen4(cmd)
    nonrRmsStr=fout.read()
    f.write("<tr align=right> <th> nonreg "+ nonrRmsStr+"\n")

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
    
    cmd_stringAb = 'echo "<tr> <td>R%s'%RUNiSEG+' <td><td colspan=6 > aborted linear  regression <td  colspan=4 > sorted: '+now.strftime("%Y-%m-%d %H:%M:%S")+'">'+safeDir+"R%s.html"%RUNiSEG
    print "execAb:%s" % cmd_stringAb
    os.system(cmd_stringAb)

    os._exit(1)

#############  M A I N  ###############
if VERBOSE:
    print ">>> Executing code ..."
#i=int(xxx)


#os._exit(1)
os.system("rm -rf outOld")
os.system("mv out outOld")
outDirName="R%s/"%RUNiSEG
retCode=0

if MOCK_REGRESS:
    os.system("cp -rp web/%s out" % outDirName )
else:
    retCode=doubleRegress()

if (retCode ):
    abortMe(retCode)
   

cmd_string4 = "root -b -q plCor.C'(0,\"R%s\")'" % RUNiSEG 
print "exec4:%s" % cmd_string4
os.system(cmd_string4+">& out/log4")

pdfName="regR%s.pdf"%RUNiSEG

cmd_string5 = "cat out/R%s_page*ps "% RUNiSEG +" | ps2pdf - out/"+pdfName
print "exec5:%s" % cmd_string5
os.system(cmd_string5+">& out/log5")


cmd_string6 = "rm -f out/R%s_page*ps "% RUNiSEG 
print "exec6:%s" % cmd_string6
os.system(cmd_string6+">& out/log6")
 
pdfURL="<A HREF=\""+outDirName+pdfName+"\"> R%s"%RUNiSEG+"</A>" + "<br> <A HREF=\""+outDirName+"logS2\"> log </A>"
print pdfURL

exportHtml("out/logS2", safeDir+"R%s"%RUNiSEG+".html",pdfURL)

cmd_string7 = "mv out "+safeDir+outDirName 
print "exec7:%s" % cmd_string7
os.system(cmd_string7)

cmd_string8 = "ls -r "+safeDir+"R*html "
print "exec8:%s" % cmd_string8
fin,fout=os.popen4(cmd_string8)
line=fout.read()
#print "cc1 ",line
line2=line.replace("\n"," ");
#print "cc1 ",line2

cmd_string9 = "rm -f "+safeDir+"index.html; cat "+safeDir+"top.html "+line2+safeDir+"bottom.html > "+safeDir+"index.html "
#print "exec9:%s" % cmd_string9
os.system(cmd_string9)

#..... generate mothr CVS table
print "rr ", RUNiSEG
run=RUNiSEG[:4]
seg=int(RUNiSEG[5:])
print run, seg
cmd="mysql --host=cdaql6.jlab.org --user=tracker data_tracker_qweak -e \"select   filename,start_time from total  where run_number=%s"%run+" and segment=%d"%seg+"\" |grep dat"
print "exec DB time query:%s" % cmd
fin,fout=os.popen4(cmd)
string=fout.read()
#print string
unixTime=string.split()[1]
print "R"+run+".%03d"%seg+ " "+ unixTime

ioPath=safeDir+"R%s"%RUNiSEG+"/"
print "ioPath=",ioPath
cmd_string12 = "root -b -q prCsvRecord.C'("+RUNiSEG+',%s ,"'%unixTime+ioPath+'","'+ioPath+"\")'"
print "exec12:%s" % cmd_string12
os.system(cmd_string12)

#...  merrge all CSV in to one big table
cmd_string13 = "rm -f "+safeDir+motherCsv+"; cat "+safeDir+"R*/*csv  > "+safeDir+motherCsv
print "exec13:%s" % cmd_string13
os.system(cmd_string13)




#...... copy all to final web page
cmd_string100 = "scp -rp -i ~/balewski/keys/id_rsa-ifarml4  "+safeDir+"/index.html  "+safeDir+outDirName+" "+safeDir+motherCsv+" balewski@ifarml4:"+destWeb

print "exec100:%s" % cmd_string100
os.system(cmd_string100)
os.system("date")

os._exit(1)


# define command line to be executed

# transfer host memory to device 

#print  "scp -rp outR%d balewski@deltag5.lns.mit.edu:0x" % RUN_NO
#os._exit(1)
#cmd_string9 = "scp -rp -i ~/balewski/keys/id_dsa-janDeltag5 index.html "+outDirName+" balewski@deltag5.lns.mit.edu:Sites/QweakA/"
