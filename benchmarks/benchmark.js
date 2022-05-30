const fs = require('fs');
const path = require('path');
const assert = require('assert');
const { performance } = require('perf_hooks');

const { strtod, parseNumber } = require('../build/Release/node_fdp.node');

const source = fs
  .readFileSync(path.join(__dirname, '../libfdp/benchmarks/data/canada.txt'))
  .toString();

const strings = source.split('\n');
strings.pop();

let t0, t1;

const parseFloatResults = [];
t0 = performance.now();
for (const string of strings) {
  parseFloatResults.push(parseFloat(string));
}
t1 = performance.now();

console.log(`parseFloat: ${t1 - t0}ms.`);

const strtodResults = [];
t0 = performance.now();
for (const string of strings) {
  strtodResults.push(strtod(string));
}
t1 = performance.now();

console.log(`strtod: ${t1 - t0}ms.`);

const parseNumberResults = [];
t0 = performance.now();
for (const string of strings) {
  parseNumberResults.push(parseNumber(string));
}
t1 = performance.now();

console.log(`parseNumber: ${t1 - t0}ms.`);

assert(parseFloatResults.length === strtodResults.length);
assert(parseFloatResults.length === parseNumberResults.length);
for (let index = 0, length = parseFloatResults.length; index < length; index++) {
  const parseFloatResult = parseFloatResults[index];
  assert(parseFloatResult === strtodResults[index]);
  assert(parseFloatResult === parseNumberResults[index]);
}
