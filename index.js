var path = require("path");
var sModuleName = path.basename(__dirname);
var sArchitecture = process.arch;
var sVersion = process.versions.node.match(/^(\d+\.\d+)\.\d+$/)[1];
module.exports = require("./addon/" + sArchitecture + "/" + sVersion + "/" + sModuleName);
