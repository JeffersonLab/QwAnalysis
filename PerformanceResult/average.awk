# This small script is used to calculate a running mean
# , a running sample deviation, and a running standard deviation
# in xterm.

# How to use.....
# $4 is what interests me.
# For example, 
# ps aux | grep jhlee | awk '{print $2} '
# 20488
# 20490
# 20528
# 20544
# 20674
# 20805
# 20806
# 20807
# 23841

# then $4 is replaced by $2, then
# ps aux | grep jhlee | awk -f average.awk
# Total count        : 52
# mean               : 10966.7
# Sample Deviation   : 8159.65
# Standard Deviation : 8080.81

# Tuesday, August 17 11:17:02 EDT 2010, jhlee
#awk ' {print "  CPU " $2 " "$3 " " $4 " " $5 " " $6 " s per event)" }'

BEGIN {
    count = 0
    mean0 = 0
    sd0 = 0
    sample_deviation = 0
    standard_deviation =0 

}
{ 
    x = $4
    count += 1
#    totalsize += x
    indexI1 = x
    mean1 = mean0 +1/count * ( x - mean0 )
    sd1   = sd0 + ( x - mean0 )*( x - mean1 )
    mean0 = mean1
    sd0   = sd1
    if( count != 0 && sd0 != 0) {
	sample_deviation = sqrt(1/(count-1)*sd0)
	standard_deviation = sqrt(1/count*sd0)
    }
}

END {
    print "Total count        : " count
 #   print "average size = " totalsize/count
    print  "mean : +- [ sample deviation, standard deviation] "
    printf "mean : %8.2lf +- [ %8.2lf, %8.2lf ] s\n", mean0, sample_deviation, standard_deviation
}
    
