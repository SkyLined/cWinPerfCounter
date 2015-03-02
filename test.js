var cWinPerfCounter = require('./index.js');

var oPerfCounter = new cWinPerfCounter("\\Processor(_Total)\\% Processor Time");
var uInterval = setInterval(function () {
  try {
    console.log(oPerfCounter.fnGetValue());
  } catch (oError) {
    console.log(oError);
  }
}, 200);
