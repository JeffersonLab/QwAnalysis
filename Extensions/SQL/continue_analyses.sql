/*
	Identify runs with late segments missing from the database,
	generate cut-and-pasteable commands to resume them.

	Call like this:
	$ cat - continue_analyses.sql | ./qweakdb
	set @slug := 155; -- change the slug number here, or whatever
	^D

	Copy & paste outputted commands into an ifarm console
*/

set @slug := if(@slug, @slug, 156) ;
source is_slug_done.sql ;
select concat(
'/group/qweak/QwAnalysis/Linux_CentOS5.3-x86_64/QwAnalysis_2.00/Extensions/Auger/batchman/official_run2pass1'
,       ' --runs ', run_number
,       ' --segments ', min(segment_number),':',max(segment_number)
	) as segments 
from slug_runlets
where analysis_id is null
group by run_number ;
