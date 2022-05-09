const assert = require('assert');
const { clhash } = require('../');
const { key, hashed } = clhash('my dog');
console.log({ key, hashed });
assert(typeof key === 'bigint');
assert(typeof hashed === 'bigint');
