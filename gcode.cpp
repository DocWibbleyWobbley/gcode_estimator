#include "gcode.h"
#include <utility>
#include <iostream>

#include <exception>

#define DEBUG 0

pair<char, double> extract_argument(const string& str)
{
	pair<char, double> result;

	result.first = str[0];
	try
	{
		result.second = stod(str.substr(1));
	}
	catch (invalid_argument& e)
	{
		cout << "Cannot parse double '" << str << "'" << endl;
	}
	catch (exception& e)
	{
		cout << "Exception - " << e.what() << endl;
	}

	// Ensure capital letters
	if ((result.first >= 'a') && (result.first <= 'z'))
	{
		result.first -= 'a' - 'A';
	}
	else if (!((result.first >= 'A') && (result.first <= 'Z')))
	{
		cout << "Argument not a letter '" << result.first << "'" << endl;
	}

	return result;
}

// Linear Move
double g1(const vector<string>& line_tokens, Point& pos, double& speed)
{
	double result;
	vector<string>::const_iterator it;
	pair<char, double> arg;
	Point end;
	double distance;

	end = pos;
	it = line_tokens.begin();
	for (++it; it != line_tokens.end(); ++it)
	{
		arg = extract_argument(*it);

		switch (arg.first)
		{
		// Extruder
		case 'E':
			end.e = arg.second;
			break;

		// Feedrate (Speed) in units/min
		case 'F':
			speed = arg.second / 60;
			break;

		// Laser power
		case 'S':
			break;

		case 'X':
			end.x = arg.second;
			break;

		case 'Y':
			end.y = arg.second;
			break;

		case 'Z':
			end.z = arg.second;
			break;

		default:
			cout << "G1 - Unknown argument '" << arg.first << "'" << endl;
			break;
		}
	}

#if DEBUG
	for (vector<string>::const_iterator it = line_tokens.begin(); it != line_tokens.end(); ++it)
	{
		cout << *it << " ";
	}
	cout << endl;

	cout << "Old pos - ";
	pos.print();
	cout << endl;
	cout << "New pos - ";
	end.print();
	cout << endl;
#endif // DEBUG

	result = 0;
	// Perform time calc
	distance = pos.get_dist(end);
#if DEBUG
	cout << "Distance - " << distance << endl;
	cout << "Speed - " << speed << endl;
#endif
	if (0 != speed)
	{
		result = distance / speed;
	}

	// Our new position
	pos = end;

#if DEBUG
	cout << "Time - " << result << endl << endl;
#endif // DEBUG

	return result;
}

// Dwell
double g4(const vector<string>& line_tokens, Point& pos, double& speed)
{
	double result;
	vector<string>::const_iterator it;
	pair<char, double> arg;

	result = 0;
	it = line_tokens.begin();
	for (++it; it != line_tokens.end(); ++it)
	{
		arg = extract_argument(*it);

		switch (arg.first)
		{
		// P is for miliseconds
		case 'P':
			result += arg.second / 1000;
			break;

		// S is for seconds
		case 'S':
			result += arg.second;
			break;

		default:
			cout << "G4 - Unknown argument: '" << arg.first << "'" << endl;
			break;
		}
	}

#if DEBUG
	for (vector<string>::const_iterator it = line_tokens.begin(); it != line_tokens.end(); ++it)
	{
		cout << *it << " ";
	}
	cout << endl;

	cout << "Time - " << result << endl << endl;
#endif // DEBUG

	return result;
}

// Set Units to Milimeters
double g21(const vector<string>& line_tokens, Point& pos, double& speed)
{
	return 0;
}

// Move to Origin (Home)
double g28(const vector<string>& line_tokens, Point& pos, double& speed)
{
	// Used only at beginning or end so not relevant
	return 0;
}

// Set to Absolute Positioning
double g90(const vector<string>& line_tokens, Point& pos, double& speed)
{
	return 0;
}

// Set Position
double g92(const vector<string>& line_tokens, Point& pos, double& speed)
{
	vector<string>::const_iterator it;
	pair<char, double> arg;

#if DEBUG
	for (vector<string>::const_iterator it = line_tokens.begin(); it != line_tokens.end(); ++it)
	{
		cout << *it << " ";
	}
	cout << endl;

	cout << "Old pos - ";
	pos.print();
	cout << endl;
#endif

	it = line_tokens.begin();
	for (++it; it != line_tokens.end(); ++it)
	{
		arg = extract_argument(*it);

		switch (arg.first)
		{
		case 'E':
			pos.e = arg.second;
			break;

		case 'X':
			pos.x = arg.second;
			break;

		case 'Y':
			pos.y = arg.second;
			break;

		case 'Z':
			pos.z = arg.second;
			break;

		default:
			cout << "G92 - Unknown argument '" << arg.first << "'" << endl;
			break;
		}
	}

#if DEBUG
	cout << "New pos - ";
	pos.print();
	cout << endl << endl;
#endif // DEBUG

	// There's no actual movement
	return 0;
}

// Set extruder to absolute mode
double m82(const vector<string>& line_tokens, Point& pos, double& speed)
{
	return 0;
}

// Stop idle hold
double m84(const vector<string>& line_tokens, Point& pos, double& speed)
{
	return 0;
}

// Set Extruder Temperature
double m104(const vector<string>& line_tokens, Point& pos, double& speed)
{
	return 0;
}

// Fan On
double m106(const vector<string>& line_tokens, Point& pos, double& speed)
{
	return 0;
}

// Fan Off
double m107(const vector<string>& line_tokens, Point& pos, double& speed)
{
	return 0;
}

// Set Extruder Temperature and Wait
double m109(const vector<string>& line_tokens, Point& pos, double& speed)
{
	// Assume everything is preheated
	return 0;
}

// Display Message
double m117(const vector<string>& line_tokens, Point& pos, double& speed)
{
	return 0;
}

// Set Bed Temperature (Fast)
double m140(const vector<string>& line_tokens, Point& pos, double& speed)
{
	return 0;
}

// Wait for bed temperature to reach target temp
double m190(const vector<string>& line_tokens, Point& pos, double& speed)
{
	// Assume everything is preheated
	return 0;
}
