#!/bin/bash

set -e

# extract models version
version=$(jq -r '.peerDependencies."@sensebox/opensensemap-api-models"' package.json)

# fetch tarball
filename=$(npm pack "@sensebox/opensensemap-api-models@$version")

rm -rf arduino-test/sensorLayouts

# unarchive
tar xvzf "$filename" --strip-components=3 -C arduino-test/ --wildcards '*/src/box/sensorLayouts/*'

rm -rf "$filename"

echo -e "\ninstalled sensorLayouts from @sensebox/opensensemap-api-models@$version into arduino-test/sensorLayouts"
