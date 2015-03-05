cWinPerfCounter
===============

Access Windows performance counters through Node.js.

Getting Started
---------------
1. Install cWinPerfCounter via NPM.
  
  `npm install cwinperfcounter`
  
  Optionally: rename `cwinperfcounter` to `cWinPerfCounter`: npm is unable to
  handle the complexity of uppercase characters in a module name. Node.js on
  Windows does not have this problem, so renaming the folder is not required
  for you to use the module.
  
2. Require cWinPerfCounter in your project.
  
  `var cWinPerfCounter=require("cWinPerfCounter");`

3. Instantiate cWinPerfCounter for the performance counter you want to read.
  
  `var oPerfCounter = new cWinPerfCounter("\\Processor(_Total)\\% Processor Time");`
  
4. Read values for the requested performance counter.
  
  ```  
  setInterval(function() {
    try {
      console.log(oPerfCounter.fnGetValue());
    } catch (oError) {
      console.log(oError);
    }
  }, 1000);
  ```
  
  This particular performance counter will return the average total CPU usage
  on your machine since the last time it was read. This means it will not
  return a value the first time you call it, but only after you have called it
  twice. The try ... catch will handle the error on the first read.

Notes
-----
### cWinPerfCounter cross-platform support

cWinPerfCounter comes with pre-build binaries for all supported processor
architectures and Node.js versions (ia32 and x64, Node.js v0.9.0 - v0.12.0).
These files are stored in separate sub-folders of the `bin` folder. The
`index.js` attempts to load each `.node` file for the user's processor
architecture until one works. This means it may attempt to load various versions
that fail before it will eventually load the right version and return.

Future Node.js versions may be supported, if they can load the addon for the
latest version included in this package. If the requirements for compiled addons
change in a future version, an additional build of cWinPerfCounter.node will be
required.

### Building cWinPerfCounter
The `build.cmd` script will read a list of combinations of processor
architecture and Node.js version from "build-targets.txt". It will build a
cWinPerfCounter.node file for each of them and store it in its own sub-folder
under the "bin" folder.
You can also build a `.node` file for a single processor architecture and
Node.js version by providing them as arguments to `build.cmd`, eg `build.cmd
x64 0.10.10`.

### How to find out what performance counters are available

You can either search the web, or you can run "perfmon.exe" on your machine and
follow these steps:
* open `Performance` -> `Monitoring Tools` -> `Performance Monitor`,
* right-click on the graph and select "Add Counters...",
* select and add the counters you are interested in, then click "Ok".
* right-click the graph again and select "Properties...",
* in the "Data" tab, you will see a list of the names of the performance
  counters you have selected. You should be able to use these names with
  cWinPerfCounter.

--------------------------------------------------------------------------------

### License
This code is licensed under [CC0 v1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/).
