#ifndef GCODE_H_
#define GCODE_H_

#include <vector>
#include <string>

#include "Point.h"
#include "TimeCalc.h"

using namespace std;


// Typedef of the function pointer
typedef void (*gcode_cmd_t)(const vector<string>&, TimeCalc&, double&);

// Always returns 0 sec
void gcode_dummy(const vector<string>&, TimeCalc&, double&);

// All handled gcode commands
void g1(const vector<string>& line_tokens, TimeCalc& calc, double& speed);
void g4(const vector<string>& line_tokens, TimeCalc& calc, double& speed);
void g28(const vector<string>& line_tokens, TimeCalc& calc, double& speed);
void g92(const vector<string>& line_tokens, TimeCalc& calc, double& speed);

#endif // GCODE_H_
