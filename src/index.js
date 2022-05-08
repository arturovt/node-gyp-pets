const path = require('path');
const clhash = require('node-gyp-build')(path.join(__dirname, '..'));
module.exports = clhash;
