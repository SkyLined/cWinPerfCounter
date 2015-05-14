// Attempt to load each of the available .node files for the current processor
// architecture until one succeeds, or throw an exception if none do.
var sModule = __dirname.match(/^.*[\/\\](.*?)$/)[1];
module.exports = require("./build/Release/" + sModule + ".node")[sModule];