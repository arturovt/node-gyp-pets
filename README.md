# node-clhash

Node.js library that provides bindings for the CLHash C library (https://github.com/lemire/clhash).

To install the package run the following command:

```sh
$ npm install node-clhash
# Or if you use yarn
$ yarn add node-clhash
```

The `get_random_key_for_clhash` uses `posix_memalign` internally and needs to be the key pointer to be aligned. The library does not expose binding for the `get_random_key_for_clhash` and doesn't allow the user to generate its key. Instead, it generates the key and then the hashed value and returns both values to the user:

```js
const { clhash } = require('node-clhash');
const { key, hashed } = clhash('my long string');
console.log({ key, hashed }); // { key: 5067639175440424385n, hashed: 8451564168848164053n }
```

The `key` and `hashed` values are `BigInt`. The `clhash` returns `uint64_t`, which is `unsigned long long`. The maximum `uint64_t` value is `2 ^ 64 - 1` and the maximum JavaScript integer value is `2 ^ 53 - 1` (`Number.MAX_SAFE_INTEGER`).
