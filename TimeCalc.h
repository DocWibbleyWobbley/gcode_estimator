#ifndef TIMECALC_H_
#define TIMECALC_H_

#include <algorithm>
#include <vector>

#include "Point.h"
#define EXTRUDER_COUNT 1
#define AXIS_COUNT (3 + EXTRUDER_COUNT)

class TimeCalc
{
public:
	struct PrinterMove
	{
		PrinterMove();

		double dist;
		double delta[AXIS_COUNT];
		double unit[AXIS_COUNT];
		double vel[AXIS_COUNT];
		double accel[AXIS_COUNT];
		double vel_start[AXIS_COUNT];
		double vel_end[AXIS_COUNT];
	};

private:
	double m_extruder_length[EXTRUDER_COUNT];
	double m_time;
	double m_pos[AXIS_COUNT];

	double m_accel_max[AXIS_COUNT];
	double m_vel_max[AXIS_COUNT];
	double m_jerk_max[AXIS_COUNT];

	double m_print_accel_max;
	double m_travel_accel_max;
	double m_retract_accel_max;

	std::vector<PrinterMove> m_moves;

public:
	TimeCalc();

	void reset();

	void add_move(Point new_pos, double speed);
	void set_pos(Point new_pos);
	void flush();
	void add_time(double time);
	double get_time() const;
	double get_extruder_length(int idx) const;
	Point get_pos() const;

	void set_max_acceleration(int axis, double value);
	void set_max_velocity(int axis, double value);
	void set_max_jerk(int axis, double value);

	void set_print_accel(double value);
	void set_travel_accel(double value);
	void set_retract_accel(double value);
};

#endif // TIMECALC_H_
