var binding = require('./bindings');

module.exports = function(path, fn) {
	return binding.get_mp3_info(path, fn);
}