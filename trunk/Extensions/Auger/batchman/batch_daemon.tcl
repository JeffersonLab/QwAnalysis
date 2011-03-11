#!/usr/bin/tclsh 
#
#
#        batch_daemon
#
#           by frw
#
#       Version 3.4.23
#
#  jianglai modified for G0 7-12-2003
#
#  After Paul's optimization of the batch submission script (specifying
#  the input files in the jsub script so that the date files will be copied to 
#  the local farm nodes by the auger server), I made the corresponding 
#  changes in batchman. To minimize the overhead, I just commented call the 
#  staging related calls in the main loop and process_entries(). Also 
#  all the release_files() calls (jcache -d) are commented.
#  jianglai 03-12-2004
#
#
#
#  See header of BatchMan script for change log
#
#
#
#
# Procedures in this file (don't forget about library file!)
#
#     stage_files {listofiles}
#     release_files {itask}
#     submit_job {jobfile}
#     kill_job {itask}
#     delete_file {filenm}
#     get_staging {verbose}
#     get_staged {verbose}
#     cachelink_good {filenm}
#     check_stagingprogress {i}
#     get_filelist4run {i}
#     final_runtime {j}
#     cleanup_run {i}
#     get_segment_count {statfile}
#     check_if_done {i}
#     finish_2Bstaged {}
#     calc_echodelta {count}
#     do_new {i}
#     do_unstaged {i}
#     do_staging {i}
#     do_staged {i}
#     do_submitted {i}
#     do_queued {i}
#     do_running {i}
#     do_kill {i}
#     do_aborting {i}
#     process_entries {verbose}
#     scan_entries {subset}
#     forceallentriestosubmitted {}
#     echo_help {}
#     obtain_jobid {i}
#     update_cache_links {}
#
#
#
#
# Logical Flow
#
#  init constants
#  start LOOP
#    take lock
#    read param file
#    read status file
#    determine already staged files
#    determine still staging files
#    determine running jobs
#    count & sum files used by running, queued and staging jobs
#    process running, queued and staging jobs
#    subtract file count of jobs that just got done
#    stage new jobs ONLY if file count is not exceeded
#    process other jobs
#    update status file
#    release lock
#    issue staging requests
#  LOOP unless no entries in list
#  quit
#
#
#
# TODO for each job state:
#
# do nothing for:
#   completed
#   incomplete
#   aborted
# new
#   determine files to get
# unstaged
#   submit staging request
# staging
#   check for files
# staged
#   submit job
# submitted
#   check queue status -- if NOT found mark running
# queued
#   check queue status -- if NOT found mark running
# running
#   wait for completion
# kill
#   if new -- mark aborted
#   if unstaged -- mark aborted
#   if staging -- release files, mark aborted
#   if staged -- release files, mark aborted
#   if submitted -- kill, mark aborted
# aborting
#   wait for dead



# 04-27-2003 Jianglai added global list "us_file_list" to store 
# the unstaged file segments
# 06-10-2003 Jianglai Looks like us_file_list is not so smart. If we only 
# try to stage the unstaged segments of a run, some already staged segments 
# might be sqeezed off.


# 08-16-07 Carissa/Juliette: Added some code to main section to check the total number
# of jobs being run by gzero users on the farm before performing an iteration.
# If the total number of active jobs excedes the limit (determined by the 
# number of connections the db can handle), the daemon will wait the iteration
# wait time before trying to perform the iteration again. This is to avoid 
# overloading the db when mulitple people are performing analysis passes.

# 09-14-07 Carissa: Moved the code mentioned above from main section into process_entries
# because the previous location kept the daemon from updating the status file for any runs
# during iterations where it couldn't submit new jobs. 

# 01-07-08 Carissa: maxNumJobs now 400

############################# preliminaries ############################


######## load library file #########
set ierr [source "batchman.lib"]
if {$ierr} {
	puts stderr "\n\n\n  ERROR:  could not load library file 'batchman.lib'\n\n"
	exit 77
}

set myname "batch_daemon"






##################### active interfacing with outside systems (batch, MSS) ########################


# SYSTEM INTERACTION: request data files from staging system (jcache)
proc stage_files {listofiles} {
	global  cachegroup  mychachegroup
    #puts "##### inside stage_files"
    #puts "list of files $listofiles"
	if {$cachegroup == "default"} {
		set ierr [catch {exec jcache $listofiles >& /dev/null &} answer]
	   puts "##### use default cache group, done" 
	} else {
		set ierr [catch {exec jcache -g $mychachegroup $listofiles >& /dev/null &} answer]
	    puts "##### use $mychachegroup cache group, done"   
	}
}

# SYSTEM INTERACTION: mark data files unused in staging system
proc release_files {itask} {
      global runno filecount filelist us_file_list raw_file_source cachegroup mychachegroup
      foreach ff $filelist($itask) {
        set lf "$raw_file_source/$ff"
	if {$cachegroup == "mine"} {
          set ierr [catch {exec jcache -d -g $mychachegroup $lf >& /dev/null &} answer]
	} else {
          set ierr [catch {exec jcache -d $lf >& /dev/null &} answer]
	}
      }
}

# SYSTEM INTERACTION: submit batch job
proc submit_job {jobfile} {
	set ierr [catch {exec jsub $jobfile} answer]
}

# SYSTEM INTERACTION: kill batch job
proc kill_job {itask} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	if {($jobid($itask) != {}) && ($jobid($itask) != "n/a")
	 && ($jobid($itask) != "0") && ($jobid($itask) != "*")} {
          set ierr [catch {exec jkill $jobid($itask) &} answer]
	}
}

# SYSTEM INTERACTION: delete file (used to clear old logs at NEW stage)
proc delete_file {filenm} {
      if {[file exists $filenm] == 1} {
      	set ierr [catch {exec rm -f $filenm} answer]
      }
}






############# raw data file staging related procedures #####################

# Concept: we request for files to be staged and once they are on the cache disk,
#          the batch analysis job is submitted
#          So we definitely check the chache directory for the files to be listed
#          To handle lost stage requests, we (optionally) check that files we did
#          request but have not yet shown up still have an active request in the
#          staging queue, if not, the run is marked AWOL
#          Further, we also (optionally) verify the staged files: the chache
#          directory only contains links to the actual files but sometimes these
#          links are not valid

# get list of staging tasks for current user from staging system, split by run number
proc get_staging {verbose} {
	global env
	global  raw_file_source
	global runno istatus filecount filelist us_file_list runtime entrycount
	global statestrings statebcol statebtxt stagedcount maxentries
	global starttime endtime filespresent preabortstate jobid selected
	global staging_files  bypass_staging_check
	
	# always create the variables!
	for {set i 1} {$i <= $entrycount} {incr i 1} {
	        set staging_files($i) {}
	}
	
	if {$bypass_staging_check} {return}
	
	#           1         2         3         4         5         6         7         8         9         0
	# 01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	# 300579  2   jcache    Pending frw      10  009932 ifarml2               /mss/hallc/e93026/raw/e93026_41065.log.3          
	# 300597  1   jcache    Pending frw      10  009932 ifarml2               /mss/hallc/e93026/raw/e93026_41066.log.3          
	# 300598  1   jcache    Running frw      10  009937 ifarml2    mss5       /mss/hallc/e93026/raw/e93026_41066.log.1          
	# 300598  2   jcache    Staging frw      10  009937 ifarml2    mss5       /mss/hallc/e93026/raw/e93026_41066.log.2          
	# 300598  3   jcache    Pending frw      10  009937 ifarml2    mss5       /mss/hallc/e93026/raw/e93026_41066.log.0          

	# wdconinc 2010-08-18: output format
	#           1         2         3         4         5         6         7         8         9         0
	# 01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
	# ==================================================================================================================================
	# Request Job Type      State   User     Pri Volume Client     Mover      Source
	# ==================================================================================================================================
	# 17124386 33856512 jcache    Pending mukesh   11  403841 ifarml4    meta       /mss/clas/g12/production/pass1/bos/7-4ctrk/56470.A77
	# 17124386 33856768 jcache    Pending mukesh   11  403841 ifarml4    meta       /mss/clas/g12/production/pass1/bos/7-4ctrk/56476.A33
	# 17124386 33857536 jcache    Pending mukesh   11  403848 ifarml4    meta       /mss/clas/g12/production/pass1/bos/7-4ctrk/56486.A64
	# 17124386 33860608 jcache    Pending mukesh   11  403947 ifarml4    meta       /mss/clas/g12/production/pass1/bos/7-4ctrk/56540.A07
	# 17124386 33861120 jcache    Pending mukesh   11  403949 ifarml4    meta       /mss/clas/g12/production/pass1/bos/7-4ctrk/56546.A90

	set ierr [catch {exec jtstat | grep $env(USER)} outstr]
	
	#Jianglai 05-23-2003. OK, I see the point. If someone else is also 
	#retrieving this file, also count it as a staging file
	if {$ierr != 0} {
		set ierr [catch {exec jtstat} outstr]
	}
	
	if {$ierr == 0} {
		set lines [split $outstr \n]
		set Nfound [llength $lines]
		set ifound 0
		set files_found {}
		if {$Nfound > 0} {
		    foreach oneline $lines {
		        if {[string length $oneline] > 72} {
		              set test [string trim [string range $oneline 72 end]]
		              set pos1 [string first $raw_file_source $test]
		              if {$pos1 > -1} {
				 incr pos1 [string length $raw_file_source]
		        	 incr pos1 1
		        	 set xfound [string range $test $pos1 end]
		        	 lappend files_found $xfound
		        	 incr ifound 1
		              }
		            }
        	    }
		}
		for {set i 1} {$i <= $entrycount} {incr i 1} {
		        if {$istatus($i) == "staging"} {
		            foreach ff $filelist($i) {
		        	if {[lsearch $files_found $ff] > -1} {
		        	    lappend staging_files($i) $ff
		        	}
		            }
		        }
		}     
		if {$verbose} {puts "   $ifound active stage requests"}
	} else {
		set ifound -1
		for {set i 1} {$i <= $entrycount} {incr i 1} {
			set staging_files($i) {ERR}
		}
		if {$verbose} {puts stderr "\n ERR:  Could not access staging queue ($outstr).\n"}
	}
}


# get list of staged files from directory listing; fundamental list of staged files (links!!)
proc get_staged {verbose} {
	global  raw_file_source  staged_files_dir  batchscript
	global  staged_files

	set ierr [catch {exec ls -1 $staged_files_dir} answer]
	set staged_files [split $answer "\n " ]
	
	if {$verbose} {puts "   [llength $staged_files] files staged"}
}


# update all the soft links $DATADIR against the actual existed staged 
# files on the cache disk. called iteration by iteration
proc update_cache_links {verbose} {
# Jianglai 03-23-2003 Launch pking's "update_cache_links.pl" here. Create the 
# soft links to the cached data files in the $DATADIR
    global batch_home_dir    
    set myexec "$batch_home_dir/update_cache_links.pl"
    set ierr [catch {exec $myexec > /dev/null} answer]
    if {$verbose} {puts "    finished updating datafile links"}
}


# verify that file listed in directory actually exists on disk (links!!)
# can be bypassed as this inquiry will hang HARD if file server is down!
proc cachelink_good {filenm} {
	global  staged_files_dir  bypass_exists
	
	if {$bypass_exists} {return 1}

	set ierr [catch {exec ls -L $staged_files_dir/$filenm} answer]
	return [expr ! $ierr]
}


# compare list of active staging tasks with outstanding file requests; since the run is in state STAGING,
# the files either should be staged or have an active staging task; if neither is the case, the file is AWOL
proc check_stagingprogress {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global Nfiles_staged  staged_files
	global staging_files  bypass_staging_check

	if {$staging_files($i) == "ERR"} {
		# don't check if list is bad
		set returnval 0
		set filespresent($i) "ERR"
	} else {
		set filespresent($i) ""
		set filesAWOL 0
		set cacheproblem 0
		foreach ff $filelist($i) {
			if {[lsearch $staged_files $ff] > -1} {
				if {[cachelink_good $ff]} {
					lappend filespresent($i) $ff
				} else {
					incr cacheproblem 1
				}
			} elseif {$bypass_staging_check} {
				# if it's not already staged, just assume it's staging
			} elseif {[lsearch $staging_files($i) $ff] == -1} {
			    incr filesAWOL 1
			    #Jianglai commented below, GEn specific
			    # discount files labeled as BAD in filename
			    #set fei [expr [string length $ff] - 1]
			    #set fsi [expr $fei - 2]
			    #set fts [string toupper [string range $ff $fsi $fei]]
			    #if {$fts == "BAD"} {incr filesAWOL -1}
			}
		}
		if {$cacheproblem > 0} {
			# some cache links are bad -- this is the most serious problem
			set returnval [expr -1*$cacheproblem]
		} elseif {$filesAWOL > 0} {
			# we've lost some files -- not quite as bad
			set returnval $filesAWOL
		} else {
			# no problems, list is accurate
			set returnval 0
		}
	}
	return $returnval
}




############## task entry initializations and final processing ###################


# discover list of raw data files that exist for a given run
proc get_filelist4run {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global iteration_delay  status_file  param_file
	global raw_file_source  staged_files_dir  batchscript
	global raw_file_prefix  raw_file_suffix  logfiledir
	global output_dir  tempfile_dir
	global thistime  nonewstarts
	global replay_flags  cache_flags

    #	set filenm "$raw_file_prefix$runno($i)$raw_file_suffix*"
    # Jianglai 03-23-2003. Use wild card "*runno*" for filenames on the silo
    # Just forget about the prefix and suffix
    puts "####### inside get_filelist4run"
    set filenm "*$runno($i)*"
        set thisdir [pwd]
	cd $raw_file_source
	set allfiles [glob -nocomplain $filenm]
	cd $thisdir
    
    # Jianglai 03-24-2003. We try to play a little trick. Instead of claiming 
    # all the "new" runs as new, go into the /cache dir and check 
    # whether they have been staged already. Eventually this will not 
    # matter much, since presumably each submission process will do
    # "jcache -d" after finishing the job. But it makes the code 
    # more convenient for the debugger like me.	
    
    set us_file_list($i) ""
        set filecount($i) [llength $allfiles]

	if {$filecount($i) > 0} {
	    set sorted [lsort $allfiles]
	    foreach ff $sorted {
		# Below is GEn specific, commented. Jianglai
		#	set fei [expr [string length $ff] - 1]
		#	set fsi [expr $fei - 2]
		#	set fts [string toupper [string range $ff $fsi $fei]]
		#	if {$fts == "BAD"} {incr filecount($i) -1}
		#see whether it has been staged
		set ierr [catch {exec ls $staged_files_dir/$ff >& /dev/null} answer]
		if { $ierr == 1 } {
		    lappend us_file_list($i) $ff
		}
	    }
	    set filelist($i) $sorted
	} else {
	    set filelist($i) ""
	}
	puts "filelist for run $runno($i) $filelist($i)"
	puts "us_file_list for run $runno($i) $us_file_list($i)"
    }


# for each task, the time spent in the current task is tracked;
# here we replace the dynamically updated time with the final fixed time
proc final_runtime {j} {
	global status_file current_state
	global runno istatus filecount filelist us_file_list runtime entrycount
	global starttime endtime filespresent preabortstate jobid selected

	set ierr1 [catch {exec date -d $endtime($j) +%s} end_sec]
	set ierr2 [catch {exec date -d $starttime($j) +%s} start_sec]
	if {($ierr1 == 0) && ($ierr2 == 0)} {
		set tdiff [expr ($end_sec - $start_sec)/60]
		if {$tdiff < 120} {
			set tdiff [expr int($tdiff)]
			set dt "$tdiff minutes"
		} elseif {$tdiff < 3000} {
			set tdiff [expr int($tdiff/60. + 0.5)]
			set dt "$tdiff hours"
		} else {
			set tdiff [expr int($tdiff/60./24. + 0.5)]
			set dt "$tdiff days"
		}
		set runtime($j) $dt
	} else {
		set dt [expr $ierr1 * 100 + $ierr2]
		set runtime($j) "=err_${dt}="
	}
}


# Test for the presence of the log file an the last line if it exists
proc test_logfile {runnum} {
    global logfiledir
    global interactive
    
    set logfileokay 0
    set logfile "$logfiledir/run_$runnum.log"

    if {[file exists $logfile] != 1} {
	if {$interactive} {puts " Unable to find log file $logfile; try again after 10 seconds"}
	after [expr int(10000)]
    }
    if {[file exists $logfile] == 1} {
	set ierr [catch {exec tail -1 $logfile} outstr]
	set test "I have done everything I can do..."
	set pos1 [string first $test $outstr]
	if { ($ierr == 0) && ($pos1 > -1) } {
	    set logfileokay 1
	} else {
	    set logfileokay $outstr
	}
    } else {
	set logfileokay 0
	if {$interactive} {puts " Still unable to find logfile $logfile; give up"}
    }
    return $logfileokay
}
# Test for the presence of the log file an the last line if it exists
proc gettimefromlogfile {runnum} {
    global logfiledir
    
    set logfiletime "NULL"
    set logfile "$logfiledir/run_$runnum.log"

    if {[file exists $logfile] == 1} {
	set logfiletime [clock format [file mtime $logfile] -format "%a %b %d %H:%M:%S %Z %Y"]
    }
    return $logfiletime
}


# clean up after completed/crashed/incomplete runs
proc cleanup_run {i} {
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global  restart_bad_stages  max_items_per_iteration
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount entrycount
	global thistime Nfiles_staged  release_failed_data
	
    if {[gettimefromlogfile $runno($i)] != "NULL"} {
	set endtime($i) [gettimefromlogfile $runno($i)]
    } else {
	set endtime($i) $thistime
    }
	final_runtime $i
	if {($istatus($i) == "ERR") && 
	    ([regexp "files_lost_staging" $preabortstate($i)] == 1) &&
	    ($restart_bad_stages) } {
		# don't remove AWOL if auto restart
	} elseif {$release_failed_data} {
		#release_files $i
	}
	#incr Nfiles_staged [expr -1 * $filecount($i)]
	set jobfile $tempfile_dir/jobfile.$runno($i)
	delete_file $jobfile
}







################ things to do to verify successful completion ##################


# parse analysis log file (stat report) for last file segment analyzed
proc get_segment_count {statfile} {
	set mystate "0 0"
	if {[file exists $statfile] == 1} {
	  if {[file size $statfile] > 0} {
	        set ierr [catch {exec grep File.Segment $statfile} tline]
	        if {[llength $tline] == 4} {
	          set count [lindex $tline 3]
	          set mystate "1 $count"
	        } else {
	          set mystate {-1 {$tline} 0}
	        }
	  }
	}
	return $mystate
}


# determine if an analysis job ran to completion or terminated prematurely or crashed
proc check_if_done {i} {
    #jianglai added env declaration here
    global env
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global thistime rundir
	global current_state Nfiles_staged
    global interactive

        if {$current_state($i) == "Queue State: PEND"} {
		return
	} elseif {$current_state($i) == "Queue State: HOLD_AUGER"} {
		return
	} elseif {$current_state($i) == "Queue State: HOLD_PBS"} {
		return
	} elseif {$current_state($i) == "Queue State: QUEUE"} {
		return
	} elseif {$current_state($i) == "Queue State: PSUSP"} {
		return
	} elseif {$current_state($i) == "Queue State: RUN"} {
		return
	} else {
	    # Jianglai 05-23-2003 modified the part below for Gzero
	    # launch jobstat -d and combine with the size of the output log file
	    # to figure out the recently finished 
	    # jobs. This could be done in checkrunning. i will stick with 
	    # this way to minimize the modifications. jianglai 04-29-2003
	    # Note jobstat -d only display the "recently" done job

	    #set retval [get_segment_count $output_dir/stats$runno($i).txt]
	    #set state1 [lindex $retval 0]
	    #set segno1 [lindex $retval 1]
	    #set retval [get_segment_count $output_dir/syncfilter.stats.$runno($i)]
	    #set state2 [lindex $retval 0]
	    #set segno2 [lindex $retval 1]
	    
	    set runisdone 0
	    
	    # Jianglai 05-29-2003. After the job server upgrade, added 
	    # "-w" to use wide format. Now necessary here, but just for 
	    # consistency ...

	    #Updated on 7-12-2003. After another jasmine upgrade on july 4, 
	    # jobstat format changed again. It puts one more extra empty line
	    # at the end. Why the hell are those guys paid for ... So use a 
	    # grep to remove the shitty empty line and title line. jianglai

	    set ierr [catch {exec jobstat -u $env(USER) | grep $env(USER) } outstr]
	    
	    if { $ierr !=0 } {
	      set new_N 0
	    } else {   
		set lines [split $outstr \n]
		#loop over all lines
		#do not count the first title line
		# changed. grep should have removed the title line already.
		set new_N [expr [llength $lines]]
	    }

	    if {$new_N > 0} {
		for {set j 1} {$j <= $new_N} {incr j 1} {
		    set oneline [lindex $lines [expr ($j) -1]]
		    set test [string trim [string range $oneline 0 0]]
		    #if the first word on the line is a number
		    if {[lsearch {0 1 2 3 4 5 6 7 8 9} $test] > -1} {
			set this_id    [lindex $oneline  0]
			set this_state [lindex $oneline  2]
			#below is a potential bug. Right after submitted,
			#the jobid for each one is zero. 

			if {$this_id == $jobid($i)} {
			    if {$this_state == "DONE"} {
				# sanity check. Check the last line of the log file
				set testval [test_logfile $runno($i)]
				set runisdone 1
				#  generate some default values
				set istatus($i) "ERR"
				set preabortstate($i) "$testval"
				if {$testval == 0} {
				    if {$interactive} {puts " Unable to find log file $logfile; mark this as early crash"}
				    set istatus($i) "ERR"
				    set preabortstate($i) "Early crash"
				} elseif {$testval == 1} {
				    set istatus($i) "completed"
				    set preabortstate($i) " "
				}
			    } elseif {$this_state == "EXIT"} {
				set runisdone 1
				set istatus($i) "ERR"
				set testval [test_logfile $runno($i)]
				if {$testval == 0} {
				    set preabortstate($i) "Early exit"
				} else {
				    set preabortstate($i) "$testval"
				}
			    }
			}
		    }
		}
	    }
	    if {$istatus($i)=="running" && ($current_state($i)=="Not in Queue!!!"  || [test_logfile $runno($i)] != 0)} {
		#  If this run shows up as being "not in queue" check to see if it ran to completion...
		#  Check the last line of the log file
		set testval [test_logfile $runno($i)]
		set runisdone 1
		#  generate some default values
		set istatus($i) "ERR"
		set preabortstate($i) "$testval"
		if {$testval == 0} {
		    if {$interactive} {puts " Unable to find log file $logfile; mark this as early crash"}
		    set istatus($i) "ERR"
		    set preabortstate($i) "Crash? $current_state($i)"
		} elseif {$testval == 1} {
		    set istatus($i) "completed"
		    set preabortstate($i) " "
		}
	    }
	    
	    # Below are specific for GEn. Commented. Jianglai 05-23-2003

#		if {($state1 > 0) && ($segno1 > -1)} {
#			set runisdone 1
#			if {$segno1 == [expr $filecount($i) -1]} {
#			     set istatus($i) "completed"
#			     set preabortstate($i) " "
#			} else {
#			     set istatus($i) "incomplete"
#			     set preabortstate($i) $segno1
#			}
#		} elseif {($state2 > 0) && ($segno2 > -1)} {
#			set runisdone 1
#			if {$segno2 == [expr $filecount($i) -1]} {
#			     set istatus($i) "completed"
#			     set preabortstate($i) "syncfilter"
#			} else {
#			     set istatus($i) "incomplete"
#			     set preabortstate($i) $segno2
#			}
#		} elseif {$state1 > 0} {
#			set runisdone 1
#			set istatus($i) "incomplete"
#			set preabortstate($i) $segno1
#		} elseif {$state2 > 0} {
#			set runisdone 1
#			set istatus($i) "incomplete"
#			set preabortstate($i) "$segno2-SF"
#	        } elseif {$state1 < 0} {
#		       set runisdone 1
#		       set istatus($i) "ERR"
#		       set preabortstate($i) "STATS= $state1"
#	        } elseif {$state2 < 0} {
#		       set runisdone 1
#		       set istatus($i) "ERR"
#		       set preabortstate($i) "SyncStat: $state2"
#	        }

		if {$runisdone==1} {
			if {$istatus($i) == "completed"} {
				#release_files $i
			}
			cleanup_run $i
		}
	}
}








################ other utility procedures ##############################

# sort files 2B staged by tape for efficient staging
proc finish_2Bstaged {} {
	global files2stage
    #puts "inside finish_2Bstaged ! "
    #puts "****** before sort $files2stage"
	set files2stage [lsort $files2stage]
    #puts "****** after sort $files2stage"
	set lasttape ""
	set thislist ""
    

	foreach ff $files2stage {
		if {[lindex $ff 0] == $lasttape} {
			lappend thislist [lindex $ff 2]
		} else {
			if {[llength $thislist] > 0} {stage_files $thislist}
			set lasttape [lindex $ff 0]
			set thislist [lindex $ff 2]
		}
	}
    #puts "thislist $thislist"
	if {[llength $thislist] > 0} {
	    #puts "will launch jcache!!!!!"
	    stage_files $thislist
	}
}


# calculates a reasonable increment for when to echo progress
# don't want too much or too little feedback...
proc calc_echodelta {count} {
	
	set power [expr int(log10($count))]
	set base [expr log10($count) - $power]
	if {$base < 0.2} {
		set scale 4
	} elseif {$base < 0.6} {
		set scale 2
	} else {
		set scale 1
	}
	set incr [expr int(pow(10,$power)/$scale)]
	if {int($incr) <= 0} {set incr 1}
	return $incr
}











##################### main processing routines ##########################



# STATE PROCESSING: NEW -- delete old logs and init status entry
proc do_new {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global iteration_delay  status_file  param_file
	global raw_file_source  staged_files_dir  batchscript
	global raw_file_prefix  raw_file_suffix  logfiledir
	global output_dir  tempfile_dir
	global thistime  nonewstarts
	global replay_flags  cache_flags

	get_filelist4run $i

    
    
    # Take the counter of the unstaged files instead of the filecount($i)
    # to set the staged/unstaged status
    # jianglai. revert to Frank's original scheme. Claim all the new
    # files as unstaged and launch jcache within the file limit in 
    # do_unstaged. This is certainly a safer way of handling the file
    # shipment in the long run.

#    set num_of_seg_unstagged [llength $us_file_list($i)]
#
#    if {$filecount($i) > 0} {	
#	if { $num_of_seg_unstagged > 0} {
#	    set istatus($i) "unstaged"
#	} else { #all file segs have been staged
#	    set istatus($i) "staged"
#	    set us_file_list($i) "" 
#	}
#    }  else {
#	set istatus($i) "ERR"
#	set preabortstate($i) "no raw data found!"
#    }
        get_filelist4run $i
	if {$filecount($i) > 0} {
		set istatus($i) "unstaged"
	} else {
		set istatus($i) "ERR"
		set preabortstate($i) "no raw data found!"
	}

    #jianglai's change
    #just have $filespresent filled at the end of do_new. why not?
    #check_stagingprogress $i
	
    set starttime($i) $thistime
	
	#We don't need these for Gzero. Jianglai 05-23-2003	
#	delete_file $logfiledir/$runno($i).log
#	delete_file $logfiledir/syncfilter.$runno($i)
#	delete_file $output_dir/stats$runno($i).txt
#	delete_file $output_dir/syncfilter.stats.$runno($i)
}



# STATE PROCESSING: UNSTAGED -- if we can stage more files (limit!) add this run's to list
proc do_unstaged {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global  iteration_delay  status_file  param_file  stage_limit
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global files2stage  Nfiles_staged  nonewstarts
	global thistime
	global  max_items_per_iteration  stages_this_iteration

	if {$nonewstarts} {return}

	# Jianglai changed the order of the two if below. If we
	# have exceed the file count limit, just return. Even there
	# are more runs that are already staged, don't tag those as
	# staged, before the ongoing staging process might push those
	# existing files back to silo.
	if {($stage_limit > 0) && ($Nfiles_staged > $stage_limit)} {return}
	
	# BIG EXCEPTION to limit on files in use: files happen to be there...
	if {([check_stagingprogress $i] == 0)
	 && ([llength $filelist($i)] == [llength $filespresent($i)])} {
		set istatus($i) "staged"
		set starttime($i) $thistime
		set runtime($i) 0
		incr Nfiles_staged [llength $filelist($i)]
		return
	}

	if {($max_items_per_iteration > 0) && \
	    ($stages_this_iteration >= $max_items_per_iteration)} {return}
	incr stages_this_iteration 1

	set isOK 1
	set thislist ""
	set count 0

    # Jianglai modified this to us_file_list. only do this for the unstaged 
    # files. 04-27-2003	
    # Jianglai 06-07-2003. Somehow us_file_list of the run > staging
    # limit could disappeared after the 1st group of the staging limit
    # runs are finished. So launch get_filelist4run here again.
    get_filelist4run $i

    #puts "us_file_list for run $runno($i)"
    foreach ff $us_file_list($i) { 
	#puts "$ff" 
    }
    
    # Jianglai 06-10-2003. revert back to the original scheme. submit 
    # staging requests for all the segments of the "unstaged" runs. This 
    # saves us in the long term.
    foreach ff $filelist($i) {
	        set lf "$raw_file_source/$ff"		
	#set ierr [catch {exec jls -af $lf | grep -v Available} jls]	
	#jianglai "Available" disappeared after the July 4th upgrade
	set ierr [catch {exec jls -af $lf | grep -v "tape"} jls]
		if {([lindex $jls 0] != "Nothing") && \
		    ([lindex $jls 1] == "true")} {
			incr count 1
			set tape [lindex $jls 0]
			set file [lindex $jls 3]
			set seq  [lindex $jls 2]
			if {$seq < 10} {
				set seq "00$seq"
			} elseif {$seq < 100} {
				set seq "0$seq"
			}
			set this "$tape $seq $file"
			lappend thislist $this
		} else {
			set isOK 0
		}
	    }
	if {($isOK == 1) && ($count > 0)} {
		incr Nfiles_staged $count
		foreach ff $thislist {lappend files2stage $ff}
	}
	#puts "******** files2stage $files2stage"
	set starttime($i) $thistime
	set istatus($i) "staging"
	set runtime($i) 0
}


# STATE PROCESSING: STAGING -- check progress of staging requests
# result is either still staging, staged or AWOL with some grace time
proc do_staging {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global  gracetime_AWOL  thistime

	set stageissues [check_stagingprogress $i]
	
	if {$stageissues > 0} {
	        # give it a few minutes to show up...
		if {([lindex $runtime($i) 1] != "minutes") ||
		    ([lindex $runtime($i) 0] > $gracetime_AWOL)} {
			set istatus($i) "ERR"
			set preabortstate($i) "files_lost_staging=$stageissues"
			cleanup_run $i
		}
	} elseif {$stageissues < 0} {
	        # give it a few minutes to show up...
		if {([lindex $runtime($i) 1] != "minutes") ||
		    ([lindex $runtime($i) 0] > $gracetime_AWOL)} {
			set istatus($i) "ERR"
			set preabortstate($i) "bad_cache_links=[expr -1*$stageissues]"
			cleanup_run $i
		}
	} elseif {[llength $filelist($i)] == [llength $filespresent($i)]} {
		set istatus($i) "staged"
		set starttime($i) $thistime
		set runtime($i) 0
	} else {
		# some stages still in progress, stay "staging"
	}
}


# STATE PROCESSING: STAGED -- all requiste files present, submit batch job
proc do_staged {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global iteration_delay  status_file  param_file
	global raw_file_source  staged_files_dir  batchscript
	global raw_file_prefix  raw_file_suffix  logfiledir
	global output_dir  tempfile_dir
	global thistime rundir  batch_home_dir
	global max_items_per_iteration  submits_this_iteration
	global experiment_label  nonewstarts
	global replay_flags  cache_flags

    puts " in Do Staged\n";

# jianglai 03-13-2003. commented the staging related calls below
#	set stageissues [check_stagingprogress $i]
#	if {$stageissues != 0} {
#		set istatus($i) "ERR"
#		set preabortstate($i) "files_disappeared=[expr abs($stageissues)]"
#		cleanup_run $i
#	} elseif {[llength $filelist($i)] != [llength $filespresent($i)]} {
#		set istatus($i) "ERR"
#		set preabortstate($i) "files_vanished=[expr [llength $filelist($i)] - [llength $filespresent($i)]]"
#		cleanup_run $i
#	} elseif {$nonewstarts} {
#       }
        if {$nonewstarts} {
		# don't do anything else
	} elseif {($max_items_per_iteration > 0) && \
		    ($submits_this_iteration >= $max_items_per_iteration)} {
		# don't do any more submits
	} else {
		incr submits_this_iteration 1
	    # Jianglai commented below. Apply pking's perl script instead

#		set filecommon "$raw_file_prefix$runno($i)$raw_file_suffix"
#
#		set jobfile $tempfile_dir/jobfile.$runno($i)
#		delete_file $jobfile
#		set f [open $jobfile w]

#		puts $f "# batch job submission script for run $runno($i)"
#		puts $f " "
#		puts $f "PROJECT: Gen01"
#		puts $f "JOBNAME: Gen01_$runno($i)"
#		puts $f "PROJECT: $experiment_label"
#		puts $f "JOBNAME: ${experiment_label}_$runno($i)"
#		puts $f "OS: linux"
#		puts $f "QUEUE: production"
#		puts $f "COMMAND: $batchscript"
#		puts $f "OPTIONS: $runno($i) $filecount($i) $batch_home_dir $filecommon $staged_files_dir"
# 		puts $f "COMMAND_COPY: "
# 		puts $f "TIME: "
# 		puts $f "INPUT_FILES: "
# 		puts $f "INPUT_DATA: "
# 		puts $f "MULTI_JOBS: "
# 		puts $f "OTHER_FILES: "
# 		puts $f "TOTAPE: "
# 		puts $f "TOWORK: "
# 		puts $f "OUTPUT_DATA: "
# 		puts $f "OUTPUT_TEMPLATE: "
# 		puts $f "MAIL: "
#		puts $f " "
#
#		close $f
	
#		submit_job $jobfile

	    #Jianglai's modification
	    set myexec "$batch_home_dir/qwbatchsub.pl"
	    set my_replay_flags "-O \"$replay_flags\""
	    set my_cache_flags "-C \"$cache_flags\""
	    set myerrorlog "$batch_home_dir/batch_daemon.log"
	    # remove the "&". need to finish the submission before moving on
	    puts "$myexec -r $runno($i) $my_replay_flags $my_cache_flags\n";
	    set ierr [catch {exec $myexec -r $runno($i) $my_replay_flags $my_cache_flags >> $myerrorlog} answer]
	    puts "Done with $myexec -r $runno($i) $my_replay_flags $my_cache_flags\n";
		set istatus($i) "submitted"
		set starttime($i) $thistime
	    # Jianglai 05-25-2003. get the jobid for $i
	    obtain_jobid $i
		set runtime($i) 0
	}
}

# obtains current job id from batch system for run $i. jianglai added 
# the call of this proc in do_staged so that the job_id will be shown
# right after the job is submitted
proc obtain_jobid {i} {
	# jianglai added env declaration
	global  env
	global runno istatus filecount filelist runtime entrycount
	global statestrings statebcol statebtxt stagedcount maxentries
	global starttime endtime filespresent preabortstate jobid selected
	global current_state  experiment_label  check_job_names

        set known_states "PEND RUN UNKWN"
	foreach sx $known_states {set scount($sx) 0}
	set scount(other) 0
	set scount(strange) 0

	#Updated on 5-29-2003. Oops, computer center changed the output of 
	#jobstat. What a pain ... Use -w to get the wide format instead

	#Updated on 7-12-2003. After another jasmine upgrade on july 4, 
	# jobstat format changed again. It puts one more extra empty line
	# at the end. Why the hell are those guys paid for ... So use a 
	# grep to remove the the shitty empty line and title line. jianglai

	set ierr [catch {exec jobstat -u $env(USER) | grep "${experiment_label}_$runno($i)" } outstr]

	#output is something like this:
	#821378  jianglai DONE  production farml2      farml153.jlab.org G0_15451-9671 May 29 21:38

	#can't determine the job_id now. do it in the next iteration then.
	if { $ierr != 0 } {
	    set job_id($i) 0
	    return
	}
	
	set lines [split $outstr \n]
	
	set new_N [llength $lines]
	if {$new_N == 1} {
	    set thisline [lindex $lines 0]
	    set jobid($i) [lindex $thisline 0]
	} else {
	    set jobid($i) 0
	}
}

# STATE PROCESSING: SUBMITTED -- ensure that batch job is picked up by batch system
proc do_submitted {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global thistime rundir
	global current_state  gracetime_submit

	if {$current_state($i) == "Queue State: PEND"} {
		set istatus($i) "queued"
		set starttime($i) $thistime
		#jianglai commented below. don't need them in Gzero
		#set jobfile $tempfile_dir/jobfile.$runno($i)
		#delete_file $jobfile
	} elseif {$current_state($i) == "Queue State: HOLD_AUGER"} {
		set istatus($i) "queued"
		set starttime($i) $thistime
	} elseif {$current_state($i) == "Queue State: HOLD_PBS"} {
		set istatus($i) "queued"
		set starttime($i) $thistime
	} elseif {$current_state($i) == "Queue State: QUEUE"} {
		set istatus($i) "queued"
		set starttime($i) $thistime
	} elseif {$current_state($i) == "Queue State: PSUSP"} {
		set istatus($i) "queued"
		set starttime($i) $thistime
	} elseif {$current_state($i) == "Queue State: RUN"} {
		set istatus($i) "running"
		set starttime($i) $thistime
	    #jianglai commented below. don't need them in Gzero 
	    #set jobfile $tempfile_dir/jobfile.$runno($i)
	    #delete_file $jobfile
	} else {
		check_if_done $i
		# nope not done flag it bad after 10 minutes
		if {($istatus($i) == "submitted") && ($runtime($i) != 0)} {
			if {([lindex $runtime($i) 1] != "minutes") ||
			    ([lindex $runtime($i) 0] > $gracetime_submit)} {
				set istatus($i) "ERR"
				set preabortstate($i) "job_vanished"
				cleanup_run $i
			}
		}
	}

}


# STATE PROCESSING: QUEUED -- check queue progress
# result is either still queued, actively running, completed, or crashed
proc do_queued {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global thistime rundir
	global current_state  gracetime_queue

	if {$current_state($i) == "Queue State: PEND"} {
		return
	} elseif {$current_state($i) == "Queue State: HOLD_AUGER"} {
		return
	} elseif {$current_state($i) == "Queue State: HOLD_PBS"} {
		return
	} elseif {$current_state($i) == "Queue State: QUEUE"} {
		return
	} elseif {$current_state($i) == "Queue State: PSUSP"} {
		return
	} elseif {$current_state($i) == "Queue State: RUN"} {
		set istatus($i) "running"
		set starttime($i) $thistime
	} else {
		check_if_done $i
		if {($istatus($i) == "queued") && ($runtime($i) != 0)} {
			if {([lindex $runtime($i) 1] != "minutes") ||
			    ([lindex $runtime($i) 0] > $gracetime_queue)} {
				set istatus($i) "ERR"
				set preabortstate($i) "CRASH? $current_state($i)"
				cleanup_run $i
			}
		}
	}
}


# STATE PROCESSING: RUNNING -- check analysis progress
# result is either still running, completed, incomplete, or crashed
proc do_running {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global thistime rundir
	global current_state  gracetime_queue

	if {$current_state($i) != "Queue State: RUN"} {
		check_if_done $i
		if {($istatus($i) == "running") && ($runtime($i) != 0)} {
			if {([lindex $runtime($i) 1] != "minutes") ||
			    ([lindex $runtime($i) 0] > $gracetime_queue)} {
				set istatus($i) "ERR"
				set preabortstate($i) "CRASH $current_state($i)"
				cleanup_run $i
			}
		}
	}
}


# STATE PROCESSING: KILL -- terminate job on batch farm or staging system
proc do_kill {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global  max_items_per_iteration  kills_this_iteration
	
	if {($max_items_per_iteration > 0) && \
	    ($kills_this_iteration >= $max_items_per_iteration)} {return}
	incr kills_this_iteration 1
	
	set istatus($i) "aborting"
	if {[lsearch "submitted queued running" $preabortstate($i)] > -1} {
		kill_job $i
	}
	if {[lsearch "staging staged submitted queued running" $preabortstate($i)] > -1} {
		#release_files $i
		cleanup_run $i
	}
}


# STATE PROCESSING: ABORTING -- ensure killed jobs are really killed and stay that way  ;-)
proc do_aborting {i} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir  gracetime_killed

	if {$current_state($i) == "Not in Queue!!!"} {
		set istatus($i) "aborted"
	} elseif {($runtime($i) == 0) ||
		  ([lindex $runtime($i) 1] != "minutes") ||
		  ([lindex $runtime($i) 0] > $gracetime_killed)} {
		set preabortstate($i) "cannot_kill ($current_state($i))"
	} else {
		kill_job $i
	}
}




# main task processing routine, executed once per task in status file
# jianglai 03-12-2004. This modification follows Paul's improvement of 
# the batch submission script --- now jcache and jsub are integrated on 
# the Auger server and users only need to specify the MSS file names in the 
# job file. To minimize changes I just commented the jcache related calls.

# 09-14-07 Carissa: Added code here to check that the total number of gzero 
# jobs submitted to the farm is below a certain value before submitting any
# new jobs.

proc process_entries {verbose} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount entrycount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global thistime rundir  keep_looping  auto_restart_failed
	global current_state Nfiles_staged  stage_limit
	global  max_items_per_iteration  restart_bad_stages
	global  stages_this_iteration  submits_this_iteration  kills_this_iteration  
        global maxNumJobs numJobsTot

	
	for {set i 1} {$i <= $entrycount} {incr i 1} {
		if {($istatus($i) == "staging") \
		 || ($istatus($i) == "staged") \
		 || ($istatus($i) == "submitted") \
		 || ($istatus($i) == "queued") \
		 || ($istatus($i) == "running")} {
		       #commented by jianglai 03-13-2004
	               #incr Nfiles_staged $filecount($i)
		}
	}

	#commented by jianglai 03-13-2004
	#if {$verbose} {puts "   got current file count ($Nfiles_staged of $stage_limit)"}

	set any_found 0
	set lastentryvocalized -1
	set echostep [calc_echodelta $entrycount]
	 	
	for {set i 1} {$i <= $entrycount} {incr i 1} {
	    #  Some quick hacks to reclaim the spurrious errors
	    #####  All commented out on one line.
	    #####	    if { ($istatus($i) == "ERR" && [test_logfile $runno($i)] != 0) || ($istatus($i) == "complete" && [test_logfile $runno($i)] != 1) } {set istatus($i) "running"}
	    #####


		if {[lsearch {kill restart new aborting unstaged
		              staging staged submitted queued running} $istatus($i)] > -1} {
			incr any_found 1
			if {$verbose} {
				if {($any_found == [expr int($echostep * int($any_found/$echostep))])
				    && ($any_found != $lastentryvocalized)} {
					puts "      processing item $any_found"
					set lastentryvocalized $any_found
				}
			}
		}
		
		if {$istatus($i) == "kill"}       {
			do_kill $i
		}
		if {$istatus($i) == "aborting"}       {
			do_aborting $i
		}
		# commented by jianglai 03-12-2004
		#if {$istatus($i) == "staging"}   {
		#	do_staging $i
		#}
		#if {$istatus($i) == "staged"}    {
		#	do_staged $i
		#}
		# now if new, go directly to do_stage to submit
		# the job.

		# Carissa/Juliette added Aug07: Added if/else to check total number of jobs before submitting new jobs
		if {$istatus($i) == "new"}    {		
		    if { $numJobsTot < $maxNumJobs } {
		        do_staged $i
		    } else {
		        puts " Reached max number of jobs submitted, waiting $iteration_delay mins before trying again... \n" 
		    }
		# end lines edited by Carissa	

		}		
		if {$istatus($i) == "submitted"} {
			do_submitted $i
		}
		if {$istatus($i) == "queued"}    {
			do_queued $i
		}
		if {$istatus($i) == "running"}   {
			do_running $i
		}
		# commented by jianglai 03-12-2004
		#if {$istatus($i) == "new"}       {
		#	do_new $i
		#}
		if {$istatus($i) == "unstaged"}  {
			do_unstaged $i
		}
		if {$istatus($i) == "restart"}       {
			do_kill $i
			set saverunno $runno($i)
		        initentry $i
		        set runno($i) $saverunno
		        set istatus($i) "new"
		}
	}
	
	if {$verbose} {
		puts "   $any_found active tasks processed ***"
		set outstr ""
		if {$stages_this_iteration > 0} {
			append outstr "      $stages_this_iteration stage requests"
		}
		if {$submits_this_iteration > 0} {
			append outstr "      $submits_this_iteration submits"
		}
		if {$kills_this_iteration > 0} {
			append outstr "      $kills_this_iteration kills"
		}
		if {[string length $outstr] > 0} {
		puts "$outstr"
		}
	}


	if {$restart_bad_stages} {
	  set AWOLcount 0
	  for {set i 1} {$i <= $entrycount} {incr i 1} {
		
		if {($istatus($i) == "ERR") &&
		    ([regexp "files_lost_staging" $preabortstate($i)] == 1)} {
		    
			incr any_found 1
			incr AWOLcount 1
			
			if {$verbose} {
			   if {($any_found == [expr int($echostep * int($any_found/$echostep))])} {
			      if {$any_found != $lastentryvocalized} {
		   		 puts "      processing item $any_found"
		   		 set lastentryvocalized $any_found
			      }
			   }
			}

			set saverunno $runno($i)
		        initentry $i
		        set runno($i) $saverunno
		        set istatus($i) "new"
		}
	  }	
	  if {($verbose)&&($AWOLcount>0)} {puts "   $AWOLcount AWOL restarted ***"}
	}

	
	if {$auto_restart_failed} {
	  set failcount 0
	  for {set i 1} {$i <= $entrycount} {incr i 1} {
	
		if {$verbose} {
			if {($any_found == [expr int($echostep * int((1+$any_found)/$echostep))-1])} {
				if {$any_found != $lastentryvocalized} {
					puts "     processing item [expr $any_found + 1]"
					set lastentryvocalized $any_found
				}
			}
		}
	
		if {($istatus($i) == "ERR") ||
		    ($istatus($i) == "incomplete")} {
			incr any_found 1
			incr failcount 1
			set saverunno $runno($i)
		        initentry $i
		        set runno($i) $saverunno
		        set istatus($i) "new"
		}
	  }	
	  if {($verbose)&&($failcount>0)} {puts "   $failcount failed tasks rejuvenated ***"}
	}
	
	if {$any_found == 0} {set keep_looping 0}
}


# accumulate list of runs in given state -- used in info query
proc scan_entries {subset} {
        global runno istatus filecount filelist us_file_list runtime starttime 
        global endtime filespresent preabortstate jobid  selected 
        global current_state stagedcount entrycount
        global  iteration_delay  status_file  param_file
        global  raw_file_source  staged_files_dir  batchscript
        global  raw_file_prefix  raw_file_suffix  logfiledir
        global  output_dir  tempfile_dir
        global thistime rundir  keep_looping  auto_restart_failed
        global current_state Nfiles_staged  stage_limit
        global  max_items_per_iteration  restart_bad_stages
        global  stages_this_iteration  submits_this_iteration  kills_this_iteration  
        
        set foundlist {}
	if {$subset == "any"} {
        	for {set i 1} {$i <= $entrycount} {incr i 1} {
                        lappend foundlist $runno($i)
        	}
	} else {
        	for {set i 1} {$i <= $entrycount} {incr i 1} {
                	if {$istatus($i) == $subset} {
                        	lappend foundlist $runno($i)
                	}
        	}
	}
        return $foundlist
}

# indiscriminately set all tasks' state to submitted -- used in recover mode
# this forces the script to determine best possible status of each run
proc forceallentriestosubmitted {} {
	global runno istatus filecount filelist us_file_list runtime starttime 
	global endtime filespresent preabortstate jobid  selected 
	global current_state stagedcount entrycount
	global  iteration_delay  status_file  param_file
	global  raw_file_source  staged_files_dir  batchscript
	global  raw_file_prefix  raw_file_suffix  logfiledir
	global  output_dir  tempfile_dir
	global thistime  nonewstarts

	# we indiscriminantly set all run's status to "submitted"
	# under the assumption that we next process them normally
	# this will result in all runs being identified as either
	# DONE, INCOMPLETE, QUEUED, RUNNING, or ERR

	puts -nonewline stderr "  "
        for {set i 1} {$i <= $entrycount} {incr i 1} {
		set saved $runno($i)
		initentry $i
		set runno($i) $saved
		get_filelist4run $i
		set istatus($i) "submitted"
		set starttime($i) $thistime
		set runtime($i) "=recover="
		puts -nonewline stderr "."
	}
	puts stderr " "
}

# output help text describing command line options only, not general description!
proc echo_help {} {
	puts "\n\n              ***  batch_daemon.tcl Command Line Options  ***"
	puts     "           (see header of file batchman for general description)"
	puts   "\n (no options)                          silent background operation"
	puts   "\n batch_daemon.tcl help                 produces this output"
	puts   "\n batch_daemon.tcl interactive"
	puts     " batch_daemon.tcl verbose"
	puts     "     daemon operates in foreground with step-by-stepfeedback to stdout"
	puts   "\n batch_daemon.tcl oneshot"
	puts     "     verbose operation, but only a single iteration cycle is executed"
	puts   "\n batch_daemon.tcl quietone        single, silent cycle"
	puts   "\n batch_daemon.tcl info <state>"
	puts     "     passive mode, status file is queried but not changed; list of runs with"
	puts     "     current status <state> is output to stdout (leave <state> blank for list)"
	puts   "\n batch_daemon.tcl sinfo <state>"
	puts     "     terse passive mode, suitable for piping query result to another command"
	puts   "\n batch_daemon.tcl nonewstarts"
	puts     "     verbose, only active states are processed,no new submits;"
	puts     "     brings system into stable but unfinished state "
	puts   "\n batch_daemon.tcl nonewstartsone       same as nonewstarts, one cycle only"
	puts   "\n batch_daemon.tcl recover"
	puts     "     highly specialized mode intended to help recover from serious disaster"
	puts     "     If you need to use this, then you need to study at least the relevant"
	puts     "     code and modify it.  Hopefully superflous since v3.2.26 (safeties)"
	puts   "\n"
}









#######################   MAIN part of script starts here   #########################


init_stateIDs
set rundir [pwd]
set lockfile $rundir/.batchman.LOCK

set interactive 0
set nonewstarts 0
set infochoice "none"
set recoverymode 0
if {$argc > 0} {
	set userrequest [lindex $argv 0]
	if {$userrequest == "help"} {
		echo_help
		exit 0
	} elseif {$userrequest == "oneshot"} {
		set interactive 1
		puts "\n\n  executing a single pass only...\n"
	} elseif {$userrequest == "quietone"} {
		set interactive -1
		# wdconinc 2010-08-18: no output when silent
		#puts "\n\n  executing a SILENT single pass only...\n"
	} elseif {$userrequest == "recover"} {
		set interactive 1
		set nonewstarts 1
		set recoverymode 1
		puts "\n\n  RECOVERY MODE\n   All entries will be assumed to be 'running'\n"
	} elseif {$userrequest == "nonewstartsone"} {
		set nonewstarts 1
		set interactive 1
		puts "\n\n  executing a single pass only, no new submits...\n"
	} elseif {$userrequest == "nonewstarts"} {
		set nonewstarts 1
		set interactive 2
		puts "\n\n  running in VERBOSE mode, no new submits...\n"
	} elseif {$userrequest == "interactive"} {
		set interactive 2
		puts "\n\n  running in VERBOSE mode...\n"
	} elseif {$userrequest == "verbose"} {
		set interactive 2
		puts "\n\n  running in VERBOSE mode...\n"
	} elseif {($userrequest == "info") || ($userrequest == "sinfo")} {
		if {$argc > 1} {
			set infochoice [lindex $argv 1]
			if {[lsearch "incomplete aborted kill restart new aborting unstaged \
			              staging staged submitted completed queued running ERR any" \
				      $infochoice] > -1} {
				set interactive -17
				if {$userrequest == "info"} {
					puts stdout "\n\n Extracting list of runs with current status \"$infochoice\":"
				}
			} else {
				set infochoice "none"
			}
		}
		if {$infochoice == "none"} {
			puts stderr "\n   INFO  --  USAGE:"
			puts stderr "  the \"info\" option results in the output"
			puts stderr "  of all the run numbers matching the"
			puts stderr "  indicated also status;  status of \"any\""
			puts stderr "  does just that: output all run numbers."
			puts stderr "\n  status list:"
			puts stderr "  incomplete aborted kill restart new aborting unstaged"
			puts stderr "  staging staged submitted completed queued running ERR"
			puts stderr "\n\n  ...ABORTING...  nothing to process...\n\n"
			exit 8
		}
	}
}

set keep_looping 1
while {$keep_looping == 1} {

	if {$interactive == 1} {set keep_looping 0}
	if {$interactive < 0} {
	   set interactive 0
	   set keep_looping 0
	}
	
	if {$infochoice == "none"} {
		set gotlock 0
		setlock
		while {$gotlock != 1} {
		   if {$interactive} {puts " lock taken -- waiting...  (abort right now is OK)"}
		   after 40000 
		   setlock
		}

		set ierr [catch {exec date} thistime]

		 if {$interactive} {puts "\n-----------------\n\n got lock  at $thistime"}

		set stages_this_iteration 0  
		set submits_this_iteration 0 
		set kills_this_iteration 0   
	}

	init_params $rundir
	set ierr [read_paramfile]
	
	# clear variables that track files staged or 2B staged
	set files2stage ""
	set Nfiles_staged 0

	 if {$interactive} {puts " reading status file"}

	readstatusfile
	
	 if {$interactive} {puts "   got $entrycount entries."}
	 if {$entrycount <= 0} {
	 	puts stderr "\n\n  ABORT: no ($entrycount) entries found!\n\n"
		set feedback [clearlock]
		exit 1
	 }
	 
	# Carissa/Juliette added Aug07: Get the total number of gzero jobs (all users) and set the max value
	# Current max number of connections allowed by db = 500, set this max lower so we don't 
	# get too close to the limit.

	set maxNumJobs 400

	set ierror [catch {exec jobstat -a hallc | grep "${experiment_label}_" } jobList]
	set jobListSplit [split $jobList \n]
   	set numJobsTot [llength $jobListSplit]

	if {$interactive} {puts " \n Total number of jobs (all users) is $numJobsTot, max allowed is $maxNumJobs\n"}

	# End added section

	if {$infochoice != "none"} {
		set matchlist [scan_entries $infochoice]
        	puts stdout "$matchlist"
		if {$userrequest == "info"} {
        		puts stdout "\n Found [llength $matchlist] entries of status \"$infochoice\"\n\n"
		}
	} else {
	
		if {$recoverymode} {
			 puts " Recovery: forcing all runs' state to 'running'"
			 forceallentriestosubmitted
		} else {
		    #all staging related calls commented by jianglai 03-12-2004
		    #if {$interactive} {puts " checking staging queue"}

			# get directory lisiting of staged files -- verified later
			#get_staged $interactive

			# get MSS staging summary -- bypass_staging_check option
			#get_staging $interactive

			# launch pking's update_cache_links.pl
			#update_cache_links $interactive
		    
		    if {$interactive} {puts " checking batch queue"}

			# get job server status summary
			checkrunning $interactive

			update_runtime $thistime
		}

		 if {$interactive} {puts " processing $entrycount entries"}

		    process_entries $interactive

		 if {$interactive} {puts " updating status"}

		writestatusfile $interactive
		set feedback [clearlock]

		 if {$interactive} {puts " released lock ($feedback)"}

		#finish_2Bstaged

		 if {$interactive > 1} {
		 	if {$keep_looping == 1} {
				if {$nonewstarts} {
		 			puts " *** NOTE: nonewstarts mode selected! ***"
				}
		 		puts " iteration done -- abort now if you need to."
			} else {
		 		puts "\n no active tasks remaining -- done!\n\n"
			}
		 }
	}
	
  	  if {$keep_looping == 1} {after [expr int(60000 * $iteration_delay)]}
}

exit 0
