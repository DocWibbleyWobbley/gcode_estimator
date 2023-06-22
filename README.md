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
* Use as is to get linear calculations for time
* Copy `config_example.json` to `config.json` and edit based on your printer settings

### Usage
`./gcode_time [file] ...`

**file** - a file containing gcode

### Example output
`Estimation   - 05:20:10`

`Adding 10%   - 05:52:11`

`Removing 10% - 04:48:09`
