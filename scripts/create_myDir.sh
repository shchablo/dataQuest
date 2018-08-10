echo "#--------------------------------------------------------------------"
echo "#           Data Quest - script to create dir. "
echo "#--------------------------------------------------------------------"
user=$1
if [ -z "$1" ]
  then
    echo "Enter user name!"
    exit 1
fi

mkdir ~/CMS-RPC/dataQuest/results/$user
mkdir ~/CMS-RPC/dataQuest/cards/$user
mkdir ~/CMS-RPC/dataQuest/cards/$user/logs
echo "Created!"
source ~/CMS-RPC/dataQuest/scripts/update_cards.sh $user 
