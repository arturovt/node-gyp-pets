const fs = require('fs');
const path = require('path');
const Benchmark = require('benchmark');

const { strtod, parseNumber } = require('../build/Release/node_fdp.node');

const source = fs
  .readFileSync(path.join(__dirname, '../libfdp/benchmarks/data/canada.txt'))
  .toString();

const strings = source.split('\n');
strings.pop();

const suite = new Benchmark.Suite();

suite
  .add('parseFloat', () => {
    strings.forEach(string => parseFloat(string));
  })
  .add('strtod', () => {
    strings.forEach((string) => strtod(string));
  })
  .add('parseNumber', () => {
    strings.forEach((string) => parseNumber(string));
  })
  .on('cycle', (event) => {
    console.log(String(event.target));
  })
  .on('complete', function () {
    console.log('Fastest is ' + this.filter('fastest').map('name'));
  })
  .run({ async: true });
