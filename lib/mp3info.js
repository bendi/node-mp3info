var binding = require('./bindings');

module.exports = function(path) {
	return binding.get_mp3_info(path);
}