#!/bin/sh
test -s $MAILDIR/autoresponse/message.txt && \
  autorespond 10000 5 $MAILDIR/autoresponse/message.txt $MAILDIR/autoresponse
