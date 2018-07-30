echo "#--------------------------------------------------------------------"
echo "#  Data Quest - script to update public cards. GIF 0418 (sdhcal11)   "
echo "#--------------------------------------------------------------------"
user=$1
if [ -z "$1" ]
  then
    echo "Enter user name!"
    exit 1
fi
#COSMIC_JUL18
idCOSMJUL=1LbKJG5HxfUXZZJOgeAvqTbJA_SHBn5niAEQjYENTk9w
idCOSMJULAnalysis=0
idCOSMJULbinToROOT=1299430126
wget --content-disposition -P ~/CMS-RPC/dataQuest/cards/$user "https://docs.google.com/spreadsheets/d/$idCOSMJUL/export?format=csv&gid=$idCOSMJULAnalysis"
wget --content-disposition -P ~/CMS-RPC/dataQuest/cards/$user "https://docs.google.com/spreadsheets/d/$idCOSMJUL/export?format=csv&gid=$idCOSMJULbinToROOT"
#-------------------------------------------------------------------------------
mv ~/CMS-RPC/dataQuest/cards/$user/COSMIC_JULY18-COAX_RETURN_TB042018.csvCOSMIC_JULY18\ -\ COAX_RETURN_TB042018.csv ~/CMS-RPC/dataQuest/cards/$user/default.csv
mv ~/CMS-RPC/dataQuest/cards/$user/COSMIC_JULY18-binToROOT.csvCOSMIC_JULY18\ -\ binToROOT.csv ~/CMS-RPC/dataQuest/cards/$user/binToROOT.csv

