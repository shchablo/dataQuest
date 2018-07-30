#! /bin/sh
echo "#--------------------------------------------------------------------"
echo "#           Data Quest - script for update basic cards."
echo "#--------------------------------------------------------------------"
counter=0
until [ $counter -gt 1000 ]
do
  echo $counter
~/projects/dataQuest/bin/dq --type=card ../cards/basic/LyPeti_COAX_RETURN_TB042018.csv  --filter\&\deadTime_window=$counter  --comment=$counter 
sleep 10s
  ((counter=counter + 25))
done

 
