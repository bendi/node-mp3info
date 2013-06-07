var mp3info = require('../lib/mp3info.js');

mp3info(process.argv[2], function(err, data) {
	if (err) {
		console.log("ERROR:", err);
		return;
	}
	console.log(data);
});
