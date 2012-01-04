/* 

   This mysql script searches for runlets which have more than results
   from more than one analysis stored in a data table.  To check a
   different data table, edit the assignment to @datatable in the
   first SELECT statement, then call (from the mysql prompt):

	source find_dupes.sql

   You may only search data tables containing the column analysis_id.

 */

set @datatable := if(@datatable, @datatable, "md_data");
select @datatable ;

-- The first temporary table, multianalyzed, contains all the runs
-- that have been analyzed more than once.
DROP TEMPORARY TABLE IF EXISTS multianalyzed;
CREATE TEMPORARY TABLE multianalyzed AS
        SELECT 
                count(analysis_id) as analyses
        ,       analysis.runlet_id as runlet_id
        FROM analysis 
        GROUP BY analysis.runlet_id
        HAVING analyses > 1
;

-- The second temporary table, possibly_ambiguous, is used to count
-- the number of analyses which are reported in @datatable.
-- 
-- Logically this query can be made without also creating
-- multianalyzed; however when SELECTing from (analyze JOIN md_data
-- USING analysis_id) there are 30 times as many rows and it's the
-- difference between deciseconds and seconds or execution time.
DROP TEMPORARY TABLE IF EXISTS possibly_ambiguous;
set @ambiguous := CONCAT("
CREATE TEMPORARY TABLE possibly_ambiguous AS
        SELECT 
	analysis.runlet_id as runlet_id
        , analysis_id
        , time
        FROM multianalyzed 
                JOIN analysis   USING (runlet_id) 
                JOIN "
                  , @datatable , 
                                " USING (analysis_id)
        GROUP BY analysis_id
        ORDER BY analysis.runlet_id
") ;
prepare list_ambiguous from @ambiguous; 
execute list_ambiguous;

-- The third temporary table, ambiguous, contains only the runlets
-- where more than one analysis result appears in the @datatable.
DROP TEMPORARY TABLE IF EXISTS ambiguous ;
CREATE TEMPORARY TABLE ambiguous AS
        SELECT 
                runlet_id
		, run_number
		, segment_number as seg
                , group_concat(analysis_id) AS all_ids
                , count(analysis_id) as analyses  
        FROM possibly_ambiguous 
	NATURAL JOIN runlet
        GROUP BY runlet_id
        HAVING analyses > 1 ;

-- Sample query: count and list the first few ambiguous runs
select @limit := 10;
SELECT @ex1 := "SELECT * from ambiguous LIMIT ?" as ex1 ;
prepare ex1 from @ex1;
execute ex1 using @limit;

-- Sample query: display the analysis times for the first few
-- duplicated analyses
select @ex2 := CONCAT("
SELECT runlet_id, analysis_id, time
FROM ambiguous JOIN analysis USING (runlet_id)
ORDER BY time DESC
LIMIT ?
") as ex2 \G
prepare ex2 from @ex2; 
execute ex2 using @limit;

-- Sample query: count the number of runlets with multiple analyses
-- stored, and the total number of analyses that need to be evaluated
-- (for @datatable).
select @ex3 := "SELECT count(*), sum(analyses) from ambiguous" as ex3;
prepare ex3 from @ex3;
execute ex3; 
