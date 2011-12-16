select
date(start_time) as date
, case
  floor( hour(start_time) / 8)
  when 0 then '0 owl'
  when 1 then '1 day'
  when 2 then '2 swing'
  end as shift
, min(run_number) as first_run
, count(run_number) as runs
, round(count(good_for_id)/count(run_number)*100,1) as "good_for (%)"
, round(count(run_quality_id)/count(run_number)*100,1) as "run_quality (%)"
from run
where start_time between date( date_sub(now(), interval 3 day) ) and now()
group by date, shift ;
