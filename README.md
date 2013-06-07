node-mp3info
============

nodejs native bindings to [mp3info](http://www.ibiblio.org/mp3info/) that compile and run under MSVC++

Installation
------------
`node-mp3info` comes bundled with its own copy of `mp3info` application (v. 8.5a), so 
there's no need to have `mp3info` installed on your system.

Unused method `curs_addparam` was completely removed and some includes were reorganized to 
provide compatibility with MSVC++ compiler.

Thanks to the following you can compile and install `node-mp3info` using npm:

```bash
npm install mp3info
```

[node-gyp environment setup](https://github.com/TooTallNate/node-gyp#installation)

Example
------------
Here's an example of how `node-mpg123n` can be used to start and stop a single song. The path
to an mp3 file is passed from command-line.

``` javascript
var mp3info = require('mp3info');

mp3info(process.argv[2], function(err, data) {
    if (err) {
    	console.log("ERROR:", err);
    	return;
    }
	console.log(data);
});
```
API
------------
### Methods
 * mp3info(path, fn);

Projects using mp3info
------------
### [Maracuyá - Virutal Jukebox](http://maracuya-jukebox.com)
 * contains precompiled version of mp3info
 * embeds nodejs
 * utilizes a simple yet powerful interface

