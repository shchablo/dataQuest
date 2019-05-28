#!/bin/bash
# UPDATE_CARDS
echo "#--------------------------------------------------------------------"
echo "#           Data Quest - script to update basic cards."
echo "#--------------------------------------------------------------------"

# Checks if it's bash
if [ ! -f $0 ]; then
    echo "Error: Please run bash <script.sh>"
    exit 2
fi
SCRIPT_PATH=`dirname $0`
cd $SCRIPT_PATH
ABS_SCRIPT_PATH=$PWD
cd -
SECOND_LINE=`tail -n +2 $0 | head -n 1`
# Checks if it's our file
if [ "$SECOND_LINE" != "# UPDATE_CARDS" ]; then
  echo "It's not your script path"
  exit 2
fi

# Checks if param for username exist 
if [ -z "$1" ]; then
  echo "Error: Enter floder name!"
  DIR="basic"
else
  DIR=$1
fi

# Checks if it inside dataQuest folder
IS_DATAQUEST_FOLDER="0"
ABS_DATAQUEST_PATH=""
TMP_PATH=""
FOLDERS=$(echo $ABS_SCRIPT_PATH | tr "/" "\n")
for FOLDER in $FOLDERS
do
  if [ "$FOLDER" == "dataQuest_iRPCCluster" ]; then
    IS_DATAQUEST_FOLDER="1"
    echo $FOLDER
    ABS_DATAQUEST_PATH="$TMP_PATH/$FOLDER"
  else
    TMP_PATH="$TMP_PATH/$FOLDER"
  fi
done
if [ "$IS_DATAQUEST_FOLDER" == "0" ]; then
  echo "dataQuest folder isn't included in the path"
  exit 3
fi
echo $ABS_DATAQUEST_PATH
#echo $SECOND_LINE

#plotTable
idROOT=1tq2qRgTdqL0rLwkNj-jYyk4RRIhiKUnT_ot8TxaiiVg
idRootTGraphErrors=0
idRootTH1=725890743
rm $ABS_DATAQUEST_PATH/cards/$DIR/*
wget --content-disposition -P $ABS_DATAQUEST_PATH/cards/$DIR "https://docs.google.com/spreadsheets/d/$idROOT/export?format=csv&gid=$idRootTGraphErrors" 
wget --content-disposition -P $ABS_DATAQUEST_PATH/cards/$DIR "https://docs.google.com/spreadsheets/d/$idROOT/export?format=csv&gid=$idRootTH1"
#-------------------------------------------------------------------------------

#LyPeti
idLyPeti=19RpZyD-mJxsMG7MuZ8OfVroIGv-mwm2zQ3pzzspvEbI
idLyPetiAnalysis=0
wget --content-disposition -P $ABS_DATAQUEST_PATH/cards/$DIR "https://docs.google.com/spreadsheets/d/$idLyPeti/export?format=csv&gid=$idLyPetiAnalysis"
#-------------------------------------------------------------------------------

#Delet all files 
for i in $ABS_DATAQUEST_PATH/cards/$DIR/*
do
    k=`echo ${i}|sed s/' - '/'_'/g`
      mv "${i}" ${k}
    done
