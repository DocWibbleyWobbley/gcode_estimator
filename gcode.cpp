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

void gcode_dummy(const vector<string>&, TimeCalc&, double&)
{
}

// Linear Move
void g1(const vector<string>& line_tokens, TimeCalc& calc, double& speed)
{
	double result;
	vector<string>::const_iterator it;
	pair<char, double> arg;
	Point pos = calc.get_pos();
	Point pos_old = pos;
	double distance;
	static int counter = 1;

	it = line_tokens.begin();
	for (++it; it != line_tokens.end(); ++it)
	{
		arg = extract_argument(*it);

		switch (arg.first)
		{
		// Extruder
		case 'E':
		case 'e':
			pos.e = arg.second;
			break;

		// Feedrate (Speed) in units/min
		case 'F':
		case 'f':
			speed = arg.second / 60;
			break;

		// Laser power
		case 'S':
		case 's':
			break;

		case 'X':
		case 'x':
			pos.x = arg.second;
			break;

		case 'Y':
		case 'y':
			pos.y = arg.second;
			break;

		case 'Z':
		case 'z':
			pos.z = arg.second;
			break;

		default:
			cout << "G1 - Unknown argument '" << arg.first << "'" << endl;
			break;
		}
	}

	calc.add_move(pos, speed);
}

// Dwell
void g4(const vector<string>& line_tokens, TimeCalc& calc, double& speed)
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
		case 'p':
			result += arg.second / 1000;
			break;

		// S is for seconds
		case 'S':
		case 's':
			result += arg.second;
			break;

		default:
			cout << "G4 - Unknown argument: '" << arg.first << "'" << endl;
			break;
		}
	}

	calc.add_time(result);
}

// Move to Origin (Home)
void g28(const vector<string>& line_tokens, TimeCalc& calc, double& speed)
{
	Point pos = calc.get_pos();

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	calc.set_pos(pos);
}

// Set Position
void g92(const vector<string>& line_tokens, TimeCalc& calc, double& speed)
{
	vector<string>::const_iterator it;
	pair<char, double> arg;
	Point pos = calc.get_pos();

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
		case 'e':
			pos.e = arg.second;
			break;

		case 'X':
		case 'x':
			pos.x = arg.second;
			break;

		case 'Y':
		case 'y':
			pos.y = arg.second;
			break;

		case 'Z':
		case 'z':
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
	calc.set_pos(pos);
}
