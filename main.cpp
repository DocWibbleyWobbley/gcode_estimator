#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>

#include "Point.h"
#include "gcode.h"
#include "json.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define FOLDER_DELIM '\\'
#else
#define FOLDER_DELIM '/'
#endif

using namespace std;

void usage(char *name)
{
	cout << "Usage:" << endl;
	cout << name << " [file] ..." << endl;
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

	gcode_map["G21"] = gcode_dummy;
	gcode_map["g21"] = gcode_dummy;

	gcode_map["G28"] = g28;
	gcode_map["g28"] = g28;

	gcode_map["G90"] = g90;
	gcode_map["g90"] = g90;

	gcode_map["G91"] = g91;
	gcode_map["g91"] = g91;

	gcode_map["G92"] = g92;
	gcode_map["g92"] = g92;

	gcode_map["M82"] = gcode_dummy;
	gcode_map["m82"] = gcode_dummy;

	gcode_map["M84"] = gcode_dummy;
	gcode_map["m84"] = gcode_dummy;

	gcode_map["M104"] = gcode_dummy;
	gcode_map["m104"] = gcode_dummy;

	gcode_map["M106"] = gcode_dummy;
	gcode_map["m106"] = gcode_dummy;

	gcode_map["M107"] = gcode_dummy;
	gcode_map["m107"] = gcode_dummy;

	gcode_map["M109"] = gcode_dummy;
	gcode_map["m109"] = gcode_dummy;

	gcode_map["M117"] = gcode_dummy;
	gcode_map["m117"] = gcode_dummy;

	gcode_map["M140"] = gcode_dummy;
	gcode_map["m140"] = gcode_dummy;

	gcode_map["M190"] = gcode_dummy;
	gcode_map["m190"] = gcode_dummy;
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

void init_calc(string arg0, TimeCalc& calc)
{
	ifstream file;
	size_t pos;
	nlohmann::json conf;
	vector<string> conf_keys_types = {"acceleration", "velocity", "jerk"};
	vector<string> conf_keys_axes = {"x", "y", "z"};

	pos = arg0.rfind(FOLDER_DELIM);
	arg0.erase(pos + 1);
	arg0 += "config.json";
	file.open(arg0);

	if (!file.is_open())
	{
		return;
	}

	try
	{
		file >> conf;
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
		return;
	}
	file.close();

	for (int j = 0; j < conf_keys_types.size(); ++j)
	{
		string& key = conf_keys_types[j];

		if (conf.contains(key) && conf["key"].is_object())
		{
			// Set 3D Axes
			for (int i = 0; i < conf_keys_axes.size(); ++i)
			{
				if (conf[key].contains(conf_keys_axes[i]) && conf[key][conf_keys_axes[i]].is_number())
				{
					switch (j)
					{
					case 0:
						calc.set_max_acceleration(i, conf[key][conf_keys_axes[i]]);
						break;

					case 1:
						calc.set_max_velocity(i, conf[key][conf_keys_axes[i]]);
						break;

					case 2:
						calc.set_max_jerk(i, conf[key][conf_keys_axes[i]]);
						break;
					}
				}
			}

			// Set Extruders
			if (conf[key].contains("e") && conf[key]["e"].is_array())
			{
				for (int i = 0; i < conf[key]["e"].size(); ++i)
				{
					if (conf[key]["e"][i].is_number())
					{
						switch (j)
						{
						case 0:
							calc.set_max_acceleration(3 + i, conf[key]["e"][i]);
							break;

						case 1:
							calc.set_max_velocity(3 + i, conf[key]["e"][i]);
							break;

						case 2:
							calc.set_max_jerk(3 + i, conf[key]["e"][i]);
							break;
						}
					}
				}
			}
		}

		// Set Print Acceleration
		if (conf.contains("acceleration") &&
			conf["acceleration"].contains("print") &&
			conf["acceleration"]["print"].is_number())
		{
			calc.set_print_accel(conf["acceleration"]["print"]);
		}

		// Set Travel Acceleration
		if (conf.contains("acceleration") &&
			conf["acceleration"].contains("travel") &&
			conf["acceleration"]["travel"].is_number())
		{
			calc.set_travel_accel(conf["acceleration"]["travel"]);
		}

		// Set Retract Acceleration
		if (conf.contains("acceleration") &&
			conf["acceleration"].contains("retract") &&
			conf["acceleration"]["retract"].is_number())
		{
			calc.set_retract_accel(conf["acceleration"]["retract"]);
		}
	}
}

int main(int argc, char **argv)
{
	ifstream file;
	string line;
	vector<string> filenames(&(argv[1]), &(argv[argc]));
	vector<string> line_tokens;

	map<string, gcode_cmd_t> gcode_map;

	TimeCalc calc;
	double speed;
	bool is_absolute = true;
	double total_time;
	int hours;
	int minutes;

	if (argc < 2)
	{
		usage(argv[0]);
		return -1;
	}

	init_map(gcode_map);
	init_calc(argv[0], calc);

	for (string& fname : filenames)
	{
		calc.reset();
		speed = 0;

		file.open(fname);
		while (file.good())
		{
			getline(file, line);
			if (!is_gcode(remove_comments(line)))
			{
				continue;
			}
			line_tokens = tokenize_line(line);

			if (0 != gcode_map.count(line_tokens[0]))
			{
				gcode_map[line_tokens[0]](line_tokens, calc, speed, is_absolute);
			}
			else
			{
				cout << "Unknown Gcode '" << line_tokens[0] << "'" << endl;
			}
		}
		file.close();

		if (filenames.size() > 1)
		{
			cout << "--- " << fname << " ---" << endl;
		}

		calc.flush();
		total_time = calc.get_time();
		cout << "Filament - " << calc.get_extruder_length(0) << "mm" << endl;

		cout << "Estimation - ";
		print_time(total_time);
		cout << endl;

		cout << "+10%       - ";
		print_time(total_time * 1.1);
		cout << endl;

		cout << "-12.5%     - ";
		print_time(total_time * 0.875);
		cout << endl;
	}
	return 0;
}
