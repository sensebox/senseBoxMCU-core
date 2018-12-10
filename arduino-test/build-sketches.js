'use strict';

const fs = require('fs');
const child_process = require('child_process');

const SketchTemplater = require('@sensebox/sketch-templater');

const sketchTemplater = new SketchTemplater('test.ingress.domain');

const boxStub = function (model) {
  return {
    _id: '59479ed5a4ad5900112d8dec',
    model: model,
    sensors: [
      {
        title: 'Temperatur',
        _id: '59479ed5a4ad5900112d8ded',
        sensorType: 'HDC1080'
      },
      {
        title: 'rel. Luftfeuchte',
        _id: '59479ed5a4ad5900112d8dee',
        sensorType: 'HDC1080'
      },
      {
        title: 'Luftdruck',
        _id: '59479ed5a4ad5900112d8def',
        sensorType: 'BMP280'
      },
      {
        title: 'Beleuchtungsstärke',
        _id: '59479ed5a4ad5900112d8df0',
        sensorType: 'TSL45315'
      },
      {
        title: 'UV-Intensität',
        _id: '59479ed5a4ad5900112d8df1',
        sensorType: 'VEML6070'
      },
      {
        title: 'PM10',
        _id: '59479ed5a4ad5900112d8df2',
        sensorType: 'SDS011'
      },
      {
        title: 'PM2.5',
        _id: '59479ed5a4ad5900112d8df3',
        sensorType: 'SDS011'
      }
    ],
    serialPort: 'Serial1',
    ssid: 'MY-HOME-NETWORK',
    password: 'MY-SUPER-PASSWORD'
  };
};

const buildMatrix = {
  'sensebox:samd:sb': []
};

for (const model of Object.keys(sketchTemplater._templates)) {
  if (model.includes('V2')) {
    buildMatrix['sensebox:samd:sb'].push(model);
  }
}

const mkdirp = function mkdirp(path) {
  /* eslint-disable no-empty */
  try {
    fs.mkdirSync(path);
  } catch (e) { }
  /* eslint-enable no-empty */
};

const build = function build(board, model) {
  mkdirp(`${sketchesPath}/${model}`);
  fs.writeFileSync(
    `${sketchesPath}/${model}/${model}.ino`,
    sketchTemplater.generateSketch(boxStub(model))
  );
  console.log(
    `Building model ${model} with "arduino --verbose-build --verify --board ${board} ${sketchesPath}/${model}/${model}.ino"`
  );
  child_process.execSync(
    `arduino --verbose-build --verify --board ${board} ${sketchesPath}/${model}/${model}.ino`,
    { stdio: [0, 1, 2] }
  );
  console.log('###########################################################');
};

const sketchesPath = `${__dirname}/sketches`;
mkdirp(sketchesPath);

for (const board of Object.keys(buildMatrix)) {
  for (const model of buildMatrix[board]) {
    build(board, model);
  }
}
console.log('done');
