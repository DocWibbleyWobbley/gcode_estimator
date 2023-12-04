#ifndef GCODE_H_
#define GCODE_H_

#include <vector>
#include <string>

#include "Point.h"
#include "TimeCalc.h"

using namespace std;


// Typedef of the function pointer
typedef void (*gcode_cmd_t)(const vector<string>&, TimeCalc&, double&, bool& is_absolute);

// Always returns 0 sec
void gcode_dummy(const vector<string>&, TimeCalc&, double&, bool&);

// All handled gcode commands
void g1(const vector<string>& line_tokens, TimeCalc& calc, double& speed, bool& is_absolute);
void g4(const vector<string>& line_tokens, TimeCalc& calc, double& speed, bool& is_absolute);
void g28(const vector<string>& line_tokens, TimeCalc& calc, double& speed, bool& is_absolute);
void g90(const vector<string>& line_tokens, TimeCalc& calc, double& speed, bool& is_absolute);
void g91(const vector<string>& line_tokens, TimeCalc& calc, double& speed, bool& is_absolute);
void g92(const vector<string>& line_tokens, TimeCalc& calc, double& speed, bool& is_absolute);

#endif // GCODE_H_
