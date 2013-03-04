#!/bin/bash


####### Process Command line options
for i in $*
do
  case $i in
    --user=*)
      BATCH_USER=`echo $i | sed 's/--user=//'`
      ;;
    *)
      echo "Error: option $i is unkown";
      exit -1;
      ;;
  esac
done

if [ -z "${BATCH_USER}" ];
then
  echo "Error: --user must be specified!!!";
  exit -1;
fi


db='/w/hallc/compton/compton_runs.db';

#RESULT=`jobstat -u ${BATCH_USER}  | grep "^[0-9]*"`
RESULT=`jobstat -u ${BATCH_USER} | grep -v "JOB_ID" | grep "^[0-9]*"| awk '{print $1,$3}'`

`sqlite3 $db "UPDATE pass_details SET rootfile_status='Checking' where (rootfile_status='Active' OR rootfile_status='Submitted') AND (request_id IS NOT NULL)"`;
#for ROW in $RESULT; do
#while read -r request status
printf %s "$RESULT" | while IFS=\  read -r request status
do
  echo "Checking Request_id : $request Status:$status";
  if [ $status == 'R' ];
  then
    `sqlite3 $db "UPDATE pass_details SET rootfile_status='Active' where request_id=${request}"`;
  fi

  if [ $status == 'Q' ] || [ $status == 'H' ] || [ $status == 'A' ] || [ $status == 'E' ];
  then
    `sqlite3 $db "UPDATE pass_details SET rootfile_status='Submitted' where request_id=${request}"`;
  fi
done

## Now clean up the runs that have already finished
`sqlite3 $db "UPDATE pass_details SET rootfile_status='InSilo' where rootfile_status='Checking'"`
