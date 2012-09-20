#!/bin/sh

echo "*** WARNING *** - this script will delete all data currently in databases!!!"
echo "Are you SURE you want to continue?"
echo
echo "Enter 'y' to continue" 
read RESPONSE
if [ "$RESPONSE" != "y" ] 
then 
    echo "NOT installing schema"
else 
    echo "Enter mysql root passwd"
    read MYSQL_PASS
    mysql -uroot -p $MYSQL_PASS -e "source ./grants.sql"
    mysql -uroot -p $MYSQL_PASS -e "source ./schema.sql"
fi

