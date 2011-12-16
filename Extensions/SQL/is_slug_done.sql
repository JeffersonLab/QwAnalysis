/*

	This script identifies all the runlets in a slug and
	determines which have not yet been analyzed.
	To use from the mysql prompt:

	mysql> set @slug := 50 ; # optional, default stored in file
	mysql> source sql/is_slug_done.sql ;

	To e.g. see which runlets still need to be analyzed:

	mysql> select * from slug_runlets where analysis_id is null ;

 */

set @slug := if(@slug, @slug, 159) ;

drop temporary table if exists slug_runlets ; 
create temporary table slug_runlets as 
select 
	runlet_id
,	analysis_id
,	run.run_number
,	segment_number
,	runlet.good_for_id as good_for
,	run_quality_id as run_quality
from 
	runlet join run using (run_id) 
	left join analysis using (runlet_id)
where 
	segment_number is not null 
	and slug = @slug 
; 

select
	@slug
,	min(run_number)		as first_run
,	max(run_number)		as last_run
,	count(*)		as segments
, 	count(analysis_id)	as analyzed
,	count(*) - count(analysis_id) as unanalyzed
from 
	slug_runlets natural left join analysis 
	natural join runlet
;

