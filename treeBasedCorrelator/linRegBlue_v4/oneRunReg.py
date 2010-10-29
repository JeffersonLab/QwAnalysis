#!/usr/bin/env python

""" 

Executing shell commande depending on params

"""
import os
from optparse import OptionParser
import datetime
print "START JAN"


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
                  default = 100000)

parser.add_option("-v","--verbose", 
                  dest="VERBOSE", 
                  action = "store_true", 
                  help="print all variables, matrices, and results", 
                  default = True)

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
RUN_SEG="%d." %RUN_NO + "%03d" % RUN_SEG


if VERBOSE:
    print ">>> Executing with parameters:"
    print ">>>  MOCK_REGRESS = %s " % MOCK_REGRESS
    print ">>>  RUN.SEG = %s" % RUN_SEG 
    print ">>>  TOT_EVE = %d" % TOT_EVE

################################
#  exporting HTML line
################################

def  exportHtml(inpFile,outFile,pdfURL) :
    print inpFile,"--> export HTML -->", outFile
    f=open(outFile,'a')
    #print f
    f.write("<!--   new run  R%s ------------>\n" %RUN_SEG)
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
    # .... get regAvr
    cmd= "grep \#regresAVR "+inpFile+" | cut -f2 -d,"
    fin,fout=os.popen4(cmd)
    regAvrStr=fout.read()
    f.write("<td> "+ regAvrStr+"\n")
    # .... get nonregr RMS
    cmd= "grep \#inputRMS "+inpFile+" | cut -f2 -d,"
    fin,fout=os.popen4(cmd)
    nonrRmsStr=fout.read()
    f.write("<tr align=right> <th> nonreg "+ nonrRmsStr+"\n")
    # .... get nonregrAvr
    cmd= "grep \#inputAVR "+inpFile+" | cut -f2 -d,"
    fin,fout=os.popen4(cmd)
    nonrAvrStr=fout.read()
    f.write("<td> "+ nonrAvrStr+"\n")     
    f.close()

################################
#  perform regression twice
################################

def  doubleRegress():
    print "do double regression"
    
    os.mkdir("out")

    cmdSort = "(./linRegBlue %s" % RUN_SEG + " %d" %  TOT_EVE + " )"
    print "exec1:%s" % cmdSort
    os.system(cmdSort+">& out/logS1")

    # verify jobs did not aborted
    cmd= "grep \#success out/logS1 "
    fin,fout=os.popen4(cmd)
    line=fout.read()
    print line.find("suc")
    if ( line.find("#success") <0 ):
        return 1
           
    cmd_string2 = "mv out/R%d_alphasNew.root" % RUN_NO + " out/R%d_alphas.root" % RUN_NO
    print "exec2:%s" % cmd_string2
    os.system(cmd_string2+">& out/log2")
    
    print "exec3:%s" % cmdSort
    os.system(cmdSort+">& out/logS2")
    # verify jobs did not aborted
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
    print "JB ABORT PYTHON , code=%d "%code + " run=%s"%RUN_SEG
    os._exit(1)

#############  M A I N  ###############
if VERBOSE:
    print ">>> Executing code ..."
#i=int(xxx)


#os._exit(1)
os.system("rm -rf out")
outDirName="R%s/"%RUN_SEG
retCode=0

if MOCK_REGRESS:
    os.system("cp -rp web/%s out" % outDirName )
else:
    retCode=doubleRegress()

if (retCode ):
    abortMe(retCode)
   

cmd_string4 = "root -b -q plCor.C'(0,\"R%s\")'" % RUN_SEG 
print "exec4:%s" % cmd_string4
os.system(cmd_string4+">& out/log4")

pdfName="regR%s.pdf"%RUN_SEG

cmd_string5 = "cat out/R%s_page*ps "% RUN_SEG +" | ps2pdf - out/"+pdfName
print "exec5:%s" % cmd_string5
os.system(cmd_string5+">& out/log5")


cmd_string6 = "rm -f out/R%s_page*ps "% RUN_SEG 
print "exec6:%s" % cmd_string6
os.system(cmd_string6+">& out/log6")
 
pdfURL="<A HREF=\""+outDirName+pdfName+"\"> R%s"%RUN_SEG+"</A>" + "<br> <A HREF=\""+outDirName+"logS2\"> log </A>"
print pdfURL

exportHtml("out/logS2", "web/index.html",pdfURL)

cmd_string7 = "mv out web/"+outDirName 
print "exec7:%s" % cmd_string7
os.system(cmd_string7)

#os._exit(1)
#cmd_string9 = "scp -rp -i ~/balewski/keys/id_dsa-janDeltag5 index.html "+outDirName+" balewski@deltag5.lns.mit.edu:Sites/QweakA/"

cmd_string9 = "scp -rp -i ~/balewski/keys/id_rsa-ifarml4  web/index.html web/"+outDirName+" balewski@ifarml4:/group/qweak/www/html/onlineRegression/boilA/"

print "exec9:%s" % cmd_string9
os.system(cmd_string9)

os._exit(1)


# define command line to be executed

# transfer host memory to device 

print  "scp -rp outR%d balewski@deltag5.lns.mit.edu:0x" % RUN_NO
