const fs = require('fs');
const path = require('path');
const assert = require('assert');
const Benchmark = require('benchmark');

const {
  strtod,
  parseNumber,
  googleStringToDouble,
  stdlibAtof,
} = require('../build/Release/node_fdp.node');

const source = fs
  .readFileSync(path.join(__dirname, '../libfdp/benchmarks/data/canada.txt'))
  .toString();

const strings = source.split('\n');
strings.pop();

function validate() {
  // Ensure that all parsed numbers are equal.
  strings.forEach(string => {
    const native = parseFloat(string);
    assert.equal(native, strtod(string));
    assert.equal(native, parseNumber(string));
    assert.equal(native, googleStringToDouble(string));
    assert.equal(native, stdlibAtof(string));
  });
}

function runSuite() {
  const suite = new Benchmark.Suite();

  suite
    .add('parseFloat', () => {
      strings.forEach(string => parseFloat(string));
    })
    .add('fast_double_parser::parse_float_strtod', () => {
      strings.forEach(string => strtod(string));
    })
    .add('fast_double_parser::parse_number', () => {
      strings.forEach(string => parseNumber(string));
    })
    .add('double_conversion::StringToDoubleConverter', () => {
      strings.forEach(string => googleStringToDouble(string));
    })
    .add('stdlib.atof', () => {
      strings.forEach(string => stdlibAtof(string));
    })
    .on('cycle', event => {
      console.log(String(event.target));
    })
    .on('complete', function () {
      console.log('Fastest is ' + this.filter('fastest').map('name'));
    })
    .run({ async: true });
}

validate();
runSuite();
