#!/bin/sh
#
# This script checks for the presence of an autoresponse message, and if
# present executes the autoresponder.
#
# To use this autoresponder, copy this file to
# vmailmgr/vdeliver-postdeliver if it does not already exist.  If it
# exists, copy this file somewhere accessable and add a call to it to
# vmailmgr/vdeliver-postdeliver.
#
# The autoresponder used here can be found at:
#	http://www.netmeridian.com/e-huss/autorespond.tar.gz
#

# amount of time to consider a message (in seconds)
SENDER_TIME=10000
# maximum number of messages to allow within SENDER_TIME seconds
SENDER_NUM=5

if test -s $MAILDIR/autoresponse/message.txt
then
  autorespond $SENDER_TIME $SENDER_NUM \
  	$MAILDIR/autoresponse/message.txt $MAILDIR/autoresponse
fi
