#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>

#include "Point.h"
#include "gcode.h"

using namespace std;

void usage(char *name)
{
	cout << "Usage: " << name << " [file]" << endl;
}

bool is_gcode(const string& line)
{
	bool result = true;

	if (line.empty())
	{
		result = false;
	}

	if (0 == line.find(';'))
	{
		result = false;
	}

	return result;
}

string& remove_comments(string& str)
{
	size_t pos;

	pos = str.find(';');
	if (string::npos != pos)
	{
		str.erase(pos);
	}

	return str;
}

vector<string> tokenize_line(string line)
{
	vector<string> result;
	size_t pos = 0;

	while (!line.empty())
	{
		pos = line.find(' ');
		if (string::npos == pos)
		{
			pos = line.size();
		}

		// Create a token
		result.push_back(line.substr(0, pos));
		// Erase the token
		line.erase(0, pos);
		// Erase the space
		line.erase(0, 1);
	}

	return result;
}

void init_map(map<string, gcode_cmd_t>& gcode_map)
{
	// Case insensitive
	gcode_map["G1"] = g1;
	gcode_map["g1"] = g1;

	gcode_map["G4"] = g4;
	gcode_map["g4"] = g4;

	gcode_map["G21"] = g21;
	gcode_map["g21"] = g21;

	gcode_map["G28"] = g28;
	gcode_map["g28"] = g28;

	gcode_map["G90"] = g90;
	gcode_map["g90"] = g90;

	gcode_map["G92"] = g92;
	gcode_map["g92"] = g92;

	gcode_map["M82"] = m82;
	gcode_map["m82"] = m82;

	gcode_map["M84"] = m84;
	gcode_map["m84"] = m84;

	gcode_map["M104"] = m104;
	gcode_map["m104"] = m104;

	gcode_map["M106"] = m106;
	gcode_map["m106"] = m106;

	gcode_map["M107"] = m107;
	gcode_map["m107"] = m107;

	gcode_map["M109"] = m109;
	gcode_map["m109"] = m109;

	gcode_map["M117"] = m117;
	gcode_map["m117"] = m117;

	gcode_map["M140"] = m140;
	gcode_map["m140"] = m140;

	gcode_map["M190"] = m190;
	gcode_map["m190"] = m190;
}

void print_time(double sec)
{
	int hours;
	int min;

	hours = sec / 3600;
	sec -= hours * 3600;
	min = sec / 60;
	sec -= min * 60;

	printf("%02d:%02d:%02.0f", hours, min, sec);
}

int main(int argc, char **argv)
{
	ifstream file;
	string line;
	vector<string> line_tokens;

	map<string, gcode_cmd_t> gcode_map;

	Point pos;
	double speed;
	double total_time;
	double e_total;
	int hours;
	int minutes;

	if (argc < 2)
	{
		usage(argv[0]);
		return -1;
	}

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	pos.e = 0;
	speed = 0;
	total_time = 0;
	e_total = 0;

	init_map(gcode_map);

	file.open(argv[1]);
	while (!file.eof())
	{
		getline(file, line);
		if (!is_gcode(remove_comments(line)))
		{
			continue;
		}
		line_tokens = tokenize_line(line);

		if (0 != gcode_map.count(line_tokens[0]))
		{
			total_time += gcode_map[line_tokens[0]](line_tokens, pos, speed, e_total);
		}
		else
		{
			cout << "Unknown Gcode '" << line_tokens[0] << "'" << endl;
		}
	}
	file.close();

	e_total += pos.e;

	cout << "Filament - " << e_total << "mm" << endl;

	cout << "Estimation - ";
	print_time(total_time);
	cout << endl;

	cout << "Adding 10% - ";
	print_time(total_time * 1.1);
	cout << endl;

	return 0;
}
