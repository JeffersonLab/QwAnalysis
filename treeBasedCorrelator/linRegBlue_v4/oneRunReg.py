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
#.... OLD
parser.add_option("-b","--block-size", 
                  dest="BLOCK_SIZE", 
                  type="choice", 
                  choices = ["8","12","16"], 
                  help="size of thread block (8, 12, or 16)", 
                  default = 16)


#... END
(options,args) = parser.parse_args()

#################################
# Performance tuning parameters #
#################################

BLOCK_SIZE=int(options.BLOCK_SIZE)# value can be 8, 12, or 16
VERBOSE=options.VERBOSE
RUN_NO=options.RUN_NO
TOT_EVE=options.TOT_EVE

if VERBOSE:
    print ">>> Executing with parameters:"
    print ">>> BLOCK_SIZE = %s" % BLOCK_SIZE
    print ">>>  RUN_NO = %d" % RUN_NO 
    print ">>>  TOT_EVE = %d" % TOT_EVE

################################
#  exporting HTML line
################################

def  exportHtml(inpFile,outFile,pdfURL) :
    print inpFile,"--> export HTML -->", outFile
    f=open(outFile,'a')
    #print f
    f.write("<!--   new run  R%d ------------>\n" %RUN_NO)
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

#############  M A I N  ###############
if VERBOSE:
    print ">>> Executing code ..."
#i=int(xxx)


#os._exit(1)
os.system("rm -rf out")
os.mkdir("out")

cmdSort = "(./linRegBlue %d.000" % RUN_NO + " %d" %  TOT_EVE + " )"
print "exec1:%s" % cmdSort
os.system(cmdSort+">& out/logS1")


cmd_string2 = "mv out/R%d_alphasNew.root" % RUN_NO + " out/R%d_alphas.root" % RUN_NO
print "exec2:%s" % cmd_string2
os.system(cmd_string2+">& out/log2")

print "exec3:%s" % cmdSort
os.system(cmdSort+">& out/logS2")


cmd_string4 = "root -b -q plCor.C'(0,\"R%d.000\")'" % RUN_NO 
print "exec4:%s" % cmd_string4
os.system(cmd_string4+">& out/log4")

pdfName="regR%d.pdf"%RUN_NO
outDirName="R%d/"%RUN_NO

cmd_string5 = "cat out/R%d.000_page*ps "% RUN_NO +" | ps2pdf - out/"+pdfName
print "exec5:%s" % cmd_string5
os.system(cmd_string5+">& out/log5")


cmd_string6 = "rm -f out/R%d.000_page*ps "% RUN_NO 
print "exec6:%s" % cmd_string6
os.system(cmd_string6+">& out/log6")

pdfURL="<A HREF=\""+outDirName+pdfName+"\"> R%d"%RUN_NO+"</A>" + "<br> <A HREF=\""+outDirName+"logS2\"> log </A>"
print pdfURL
#exportHtml("out/logS2", "out/line.html")
exportHtml("out/logS2", "index.html",pdfURL)

cmd_string7 = "mv out "+outDirName 
print "exec7:%s" % cmd_string7
os.system(cmd_string7)

#os._exit(1)
cmd_string9 = "scp -rp -i ~/balewski/keys/id_dsa-janDeltag5 index.html "+outDirName+" balewski@deltag5.lns.mit.edu:Sites/QweakA/"
print "exec9:%s" % cmd_string9
os.system(cmd_string9)

os._exit(1)


# define command line to be executed

# transfer host memory to device 

print  "scp -rp outR%d balewski@deltag5.lns.mit.edu:0x" % RUN_NO
