echo "#--------------------------------------------------------------------"
echo "#           Data Quest - script to update basic cards."
echo "#--------------------------------------------------------------------"
user=$1
if [ -z "$1" ]
  then
    echo "Enter user name!"
    exit 1
fi
#TB0818
idDefault=12N_tqQnElbUzDAhfi6Z_AnHYr-Gx6GTs4yGvAXBoeu0
idDefaultAnalysis=0
idPublic=1LbKJG5HxfUXZZJOgeAvqTbJA_SHBn5niAEQjYENTk9w
idPublicAnalysis=0
idPublicRATE=25618932
idPublicBinToROOT=1299430126
idDEEP=1rgFeg3_J7ladhw4-CrV4Mlsi6MPsqnv-928B7Nf4DwA
idDEEPAnalysis=0
wget --content-disposition -P ~/CMS-RPC/dataQuest/cards/$user "https://docs.google.com/spreadsheets/d/$idDefault/export?format=csv&gid=$idDefaultAnalysis"
wget --content-disposition -P ~/CMS-RPC/dataQuest/cards/$user "https://docs.google.com/spreadsheets/d/$idPublic/export?format=csv&gid=$idPublicAnalysis"
wget --content-disposition -P ~/CMS-RPC/dataQuest/cards/$user "https://docs.google.com/spreadsheets/d/$idPublic/export?format=csv&gid=$idPublicRATE"
wget --content-disposition -P ~/CMS-RPC/dataQuest/cards/$user "https://docs.google.com/spreadsheets/d/$idPublic/export?format=csv&gid=$idPublicBinToROOT"

if [ "$1" == "kshchablo" ]
  then
    wget --content-disposition -P ~/CMS-RPC/dataQuest/cards/$user "https://docs.google.com/spreadsheets/d/$idDEEP/export?format=csv&gid=$idDEEPAnalysis"
    mv ~/CMS-RPC/dataQuest/cards/$user/LyPeti-COAX_RETURN_TB0818.csvLyPeti\ -\ COAX_RETURN_TB0818.csv ~/CMS-RPC/dataQuest/cards/$user/deep.csv
fi
#-------------------------------------------------------------------------------
mv ~/CMS-RPC/dataQuest/cards/$user/TESTBEAM_GIF_0818_public-COAX_RETURN_TB0818.csvTESTBEAM_GIF++_0818_public\ -\ COAX_RETURN_TB0818.csv ~/CMS-RPC/dataQuest/cards/$user/default.csv
mv ~/CMS-RPC/dataQuest/cards/$user/TESTBEAM_GIF_0818_public-rate_COAX_RETURN_TB0818.csvTESTBEAM_GIF++_0818_public\ -\ rate_COAX_RETURN_TB0818.csv ~/CMS-RPC/dataQuest/cards/$user/rate.csv
mv ~/CMS-RPC/dataQuest/cards/$user/TESTBEAM_GIF_0818_public-binToROOT.csvTESTBEAM_GIF++_0818_public\ -\ binToROOT.csv ~/CMS-RPC/dataQuest/cards/$user/binToROOT.csv
mv ~/CMS-RPC/dataQuest/cards/$user/TESTBEAM_GIF_default-COAX_RETURN_TB042018.csvTESTBEAM_GIF++_default\ -\ COAX_RETURN_TB042018.csv ~/CMS-RPC/dataQuest/cards/$user/backUP.csv

