var mp3info = require('../lib/mp3info.js');

var len = mp3info(process.argv[2]);

console.log("Len: ", len);