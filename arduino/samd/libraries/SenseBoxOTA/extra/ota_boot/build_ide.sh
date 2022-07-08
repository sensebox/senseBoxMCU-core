#!/bin/sh -x
set -e

ARDUINO=arduino
DIR=$(dirname "$0")
SKETCH_NAME="ota_boot.ino"
SKETCH="$DIR/SKETCH_NAME"
BUILD_PATH="$DIR/build"
OUTPUT_PATH="$DIR/../../src/boot"

if [[ "$OSTYPE" == "darwin"* ]]; then
	ARDUINO="/Applications/Arduino.app/Contents/MacOS/Arduino"
fi

buildSDUBootSketch() {
	BOARD=$1
	DESTINATION=$2

	$ARDUINO --verify --board $BOARD --preserve-temp-files --pref build.path="$BUILD_PATH" $SKETCH
	cat "$BUILD_PATH/$SKETCH_NAME.bin" | xxd -i > $DESTINATION
	rm -rf "$BUILD_PATH"
}

mkdir -p "$OUTPUT_PATH"
buildSDUBootSketch "sensebox:samd:sb:power=off" "$OUTPUT_PATH/sensebox_mcu.h"
