#!/bin/sh
nsec=0
echo aa $nsec 
rm -f  plots/wrk/*
date
root.exe  -b <<EOF
.x plAuto.C(0,2,$nsec)
EOF

date
echo converting ps to png 
convert  -rotate 90  -density 174  plots/wrk/QwOnlReg001.ps  plots/wrk/QwOnlReg001.png
convert  -rotate 90  -density 174  plots/wrk/QwOnlReg002.ps  plots/wrk/QwOnlReg002.png
convert  -rotate 90  -density 174  plots/wrk/QwOnlReg003.ps  plots/wrk/QwOnlReg003.png

cat plots/wrk/QwOnlReg*.ps |ps2pdf - plots/wrk/*.pdf
scp -rp -i ~/balewski/keys/id_rsa-ifarml4 plots/wrk/*.pdf balewski@ifarml4:/group/qweak/www/html/onlineRegression/autoA/history
mv plots/wrk/*.pdf plots/wrk/QwOnlRegSumAll.pdf
scp -rp -i ~/balewski/keys/id_rsa-ifarml4  plots/index.html plots/wrk/QwOnlReg*.png plots/wrk/QwOnlRegSumAll.pdf balewski@ifarml4:/group/qweak/www/html/onlineRegression/autoA/plots
exit
