On a new installation set wait_timeout in /etc/my.cnf to some large number e.g.:
wait_timeout = 31536000

View varibales with mysqladmin variables | grep timeout (e.g.)

N.B. when checking the timeout value from the command line interface the value
shown for wait_timeout is actually the same as that of interactive_timeout since 
the mysql command shell is an interactive shell. IT WILL NOT LOOK CHANGED FROM
THE COMMAND LINE - so use mysqladmin variables ...

