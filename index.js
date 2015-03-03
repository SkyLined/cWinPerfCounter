// Attempt to load each of the available .node files for the current processor
// architecture until one succeeds, or throw an exception if none do.
var asVersions = require("fs").readdirSync(__dirname + "\\bin\\" + process.arch);
var sModule = __dirname.match(/^.*[\/\\](.*?)$/)[1];
if (!asVersions.some(function (sVersion) {
  try {
    module.exports = require("./bin/" + process.arch + "/" + sVersion + "/" + sModule);
    return true;
  } catch (e) {
    return false;
  }
})) {
  throw new Error("Your processor architecture and node version are not supported");
};

