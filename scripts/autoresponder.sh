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
#	http://em.ca/~bruceg/qmail-autoresponder/
#

if test -s $MAILDIR/autoresponse/message.txt
then
  qmail-autoresponder $MAILDIR/autoresponse/message.txt $MAILDIR/autoresponse
fi
