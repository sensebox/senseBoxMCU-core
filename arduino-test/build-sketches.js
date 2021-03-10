"use strict";

const { mkdirSync, writeFileSync } = require("fs");
const { execSync } = require("child_process");

const SketchTemplater = require("@sensebox/sketch-templater");

const { getSensorsForModel } = require("./sensorLayouts");

const sketchTemplater = new SketchTemplater("test.ingress.domain");

const genId = (i) => `59479ed5a4ad5900112d8d${i.toString(16).padStart(2, "0")}`;

const boxStub = function (model) {
  return {
    _id: "59479ed5a4ad5900112d9d00",
    model: model,
    sensors: getSensorsForModel(model).map(({ title, sensorType }, index) => ({
      title,
      sensorType,
      _id: genId(index),
    })),
    serialPort: "Serial1",
    ssid: "MY-HOME-NETWORK",
    password: "MY-SUPER-PASSWORD",
  };
};

const buildMatrix = {
  "sensebox:samd:sb": [],
};

for (const model of Object.keys(sketchTemplater._templates)) {
  if (model.includes("V2")) {
    buildMatrix["sensebox:samd:sb"].push(model);
  }
}

const mkdirp = function mkdirp(path) {
  /* eslint-disable no-empty */
  try {
    mkdirSync(path);
  } catch (e) {}
  /* eslint-enable no-empty */
};

const build = function build(board, model) {
  mkdirp(`${sketchesPath}/${model}`);
  writeFileSync(
    `${sketchesPath}/${model}/${model}.ino`,
    sketchTemplater.generateSketch(boxStub(model))
  );
  const cmdWithArgs = `arduino-cli compile --fqbn ${board} ${sketchesPath}/${model}/${model}.ino`;
  console.log(`Building model ${model} with "${cmdWithArgs}"`);
  execSync(cmdWithArgs, { stdio: [0, 1, 2] });
  console.log("###########################################################");
};

const sketchesPath = `/tmp/sketches`;
mkdirp(sketchesPath);

for (const board of Object.keys(buildMatrix)) {
  for (const model of buildMatrix[board]) {
    build(board, model);
  }
}
console.log("done");
