const assert = require('assert');
const { clhash } = require('../');
const { key, hashed } = clhash.sync('my long string');
console.log({ key, hashed });
assert(typeof key === 'bigint');
assert(typeof hashed === 'bigint');

new Promise((resolve, reject) => {
  clhash('my long string', (error, { key, hashed }) => {
    if (error) {
      reject(error);
    } else {
      resolve({ key, hashed });
    }
  });
}).then(({ key, hashed }) => {
  console.log({ key, hashed });
  assert(typeof key === 'bigint');
  assert(typeof hashed === 'bigint');
});
