var cWinPerfCounter = require('./index.js');
console.log("Addon build for node version " + cWinPerfCounter.sAddonNodeProcessorArchitecture + 
                                        " " + cWinPerfCounter.sAddonNodeVersion);

var oPerfCounter = new cWinPerfCounter("\\Processor(_Total)\\% Processor Time");
var uInterval = setInterval(function () {
  try {
    console.log(oPerfCounter.fnGetValue());
  } catch (oError) {
    console.log(oError);
  }
}, 200);
