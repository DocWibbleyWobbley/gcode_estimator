# Gcode Estimator
C/C++ Gcode Estimator

# Credits
JSON Library Used - https://github.com/nlohmann/json
v3.11.2

### Prerequisites
`g++ make`

### Building
1. Go to the directory
2. Execute `make`

### Settings
* Use without printer config to use linear moves. You should look at +10% Estimation
* Alternatively copy `config_example.json` to `config.json` and edit based on your printer settings. You should look at -15% Estimation

### Usage
`./gcode_time [file] ...`

**file** - a file containing gcode

### Example output
`Estimation - 05:20:10`

`+10%       - 05:52:11`

`-15%       - 04:32:09`
