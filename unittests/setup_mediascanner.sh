#!/bin/sh
# Setup the mediascanner and copy test media into place

set -e

TESTFILE="testfile"
DEST_PREFIX="/home/phablet"

status mediascanner-2.0
if [ $? -ne 0 ]; then
    echo "mediascanner-2.0 not running, starting"
    `start mediascanner-2.0`
fi

if [ ! -f "/home/phablet/Videos/$TESTFILE.mp4" ]; then
    echo "Copying video/$TESTFILE.mp4 to $DEST_PREFIX/Videos/$TESTFILE.mp4"
    cp videos/$TESTFILE.mp4 $DEST_PREFIX/Videos/$TESTFILE.mp4
fi

if [ ! -f "/home/phablet/Music/$TESTFILE.ogg" ]; then
    echo "Copying audio/$TESTFILE.ogg to $DEST_PREFIX/Music/$TESTFILE.ogg"
    cp audio/$TESTFILE.ogg $DEST_PREFIX/Music/$TESTFILE.ogg
fi

if [ ! -f "/home/phablet/Pictures/$TESTFILE.jpg" ]; then
    echo "Copying images/$TESTFILE.jpg to $DEST_PREFIX/Pictures/$TESTFILE.jpg"
    cp images/$TESTFILE.jpg $DEST_PREFIX/Pictures/$TESTFILE.jpg
fi
