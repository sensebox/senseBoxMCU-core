#!/bin/sh -x
set -e

ARDUINO=arduino-cli
DIR=$(dirname "$0")
SKETCH_NAME="ota_boot.ino"
BUILD_PATH="$DIR/build"
OUTPUT_PATH="$DIR/../../src/boot"

buildBootSketch() {
	BOARD=$1
	DESTINATION=$2
	"$ARDUINO" compile --fqbn="$BOARD" --build-path="$BUILD_PATH" --no-color "$DIR/$SKETCH_NAME" > $OUTPUT_PATH/buildinfo.txt
	xxd -i < "$BUILD_PATH/$SKETCH_NAME.bin" > $DESTINATION
	rm -rf "$BUILD_PATH"
}

mkdir -p "$OUTPUT_PATH"
buildBootSketch "sensebox:samd:sb:power=off" "$OUTPUT_PATH/sensebox_mcu.h"
