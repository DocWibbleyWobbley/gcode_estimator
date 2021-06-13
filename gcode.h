#ifndef GCODE_H_
#define GCODE_H_

#include <vector>
#include <string>

#include "Point.h"

using namespace std;

// Typedef of the function pointer
typedef double (*gcode_cmd_t)(const vector<string>&, Point&, double&);

// All handled gcode commands
double g1(const vector<string>& line_tokens, Point& pos, double& speed);
double g4(const vector<string>& line_tokens, Point& pos, double& speed);
double g21(const vector<string>& line_tokens, Point& pos, double& speed);
double g28(const vector<string>& line_tokens, Point& pos, double& speed);
double g90(const vector<string>& line_tokens, Point& pos, double& speed);
double g92(const vector<string>& line_tokens, Point& pos, double& speed);

double m82(const vector<string>& line_tokens, Point& pos, double& speed);
double m84(const vector<string>& line_tokens, Point& pos, double& speed);
double m104(const vector<string>& line_tokens, Point& pos, double& speed);
double m106(const vector<string>& line_tokens, Point& pos, double& speed);
double m107(const vector<string>& line_tokens, Point& pos, double& speed);
double m109(const vector<string>& line_tokens, Point& pos, double& speed);
double m117(const vector<string>& line_tokens, Point& pos, double& speed);
double m140(const vector<string>& line_tokens, Point& pos, double& speed);
double m190(const vector<string>& line_tokens, Point& pos, double& speed);

#endif // GCODE_H_
