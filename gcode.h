#ifndef GCODE_H_
#define GCODE_H_

#include <vector>
#include <string>

#include "Point.h"

using namespace std;

// Typedef of the function pointer
typedef double (*gcode_cmd_t)(const vector<string>&, Point&, double&, double&);

// All handled gcode commands
double g1(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double g4(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double g21(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double g28(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double g90(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double g92(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);

double m82(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double m84(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double m104(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double m106(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double m107(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double m109(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double m117(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double m140(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);
double m190(const vector<string>& line_tokens, Point& pos, double& speed, double& e_total_offset);

#endif // GCODE_H_
