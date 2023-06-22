// 1. Dx Dy Dz De; e_total_offset += De;
// 2. m = sqrt(Dx * Dx + Dy * Dy + Dz * Dz)
// 3. Dx /= m; Dy /= m; Dz /= m; De /= m;
// 4. V = min(speed, Vmax_x / Dx, Vmax_y / Dy, Vmax_z / Dz, Vmax_e / De);
// 5. A = min(Amax_x / Dx, Amax_y / Dy, Amax_z / Dz, Amax_e / De);
// 6. Vx = V * Dx; Vy = V * Dy; Vz = V * Dz; Ve = V * De;
// 6.1 Vend_prev_x = Vx; ... -> Calculate prev move and wait for current Vend
// 7. Ax = A * Dx; Ay = A * Dy; Az = A * Dz; Ae = A * De;
// 8. time = max(calc_time_complex(x), ...(y), ...(z), ...(e))

#include "TimeCalc.h"

#define DEBUG_CALC 0

#include <limits>
#include "math.h"

#include <iostream>

using namespace std;

static double vector_min(const vector<double>& vec)
{
	double min;
	int idx;

	if (vec.empty())
	{
		return 0.0;
	}

	idx = 0;
	min = abs(vec[0]);

	for (int i = 1; i < vec.size(); ++i)
	{
		if (abs(vec[i]) < min)
		{
			min = abs(vec[i]);
			idx = i;
		}
	}

	return vec[idx];
}

static double vector_max(const vector<double>& vec)
{
	double max;
	int idx;

	if (vec.empty())
	{
		return 0.0;
	}

	idx = 0;
	max = abs(vec[0]);

	for (int i = 1; i < vec.size(); ++i)
	{
		if (abs(vec[i]) > max)
		{
			idx = i;
			max = abs(vec[i]);
		}
	}

	return vec[idx];
}

static double calc_dist(double vel, double accel, double t)
{
	return vel * t + 0.5 * accel * t * t;
}

static double calc_time_to_accel(double accel, double vel_start, double vel_end)
{
	return abs((vel_start - vel_end) / accel);
}

// Triangle Velocity
static double calc_time_very_complex(double dist, double vel_start, double vel_end, double accel)
{
	double a;
	double b;
	double c;
	double d;
	double val[2];
	double res = 0;

	if (accel == 0)
	{
		return 0;
	}

	a = accel;
	b = 2 * vel_end;
	c = (vel_end * vel_end - vel_start * vel_start)/(2 * accel) - dist;
	d = b * b - 4 * a * c;

	d = sqrt(d);

	val[0] = (-b + d)/(2*a);
	val[1] = (-b - d)/(2*a);

	if (val[0] >= 0)
	{
		res = val[0];
	}

	if (val[1] >= 0)
	{
		res = val[1];
	}

	return (2 * res + (vel_end - vel_start) / accel);
}

static double calc_time_complex(double dist, double accel, double max_vel_change, double vel_start, double vel_end, double vel_max_target)
{
	double result = 0.0;
	double accel_time;
	double accel_dist;
	double deccel_time;
	double deccel_dist;

	if (dist == 0)
	{
		return 0;
	}

	if (accel == 0)
	{
		// Linear calculation - It's something
		return dist/vel_max_target;
	}

	// "Instant" velocity change on <= "Jerk"
	if (vel_max_target != vel_start)
	{
		if (abs(vel_max_target - vel_start) <= max_vel_change)
		{
			vel_start = vel_max_target;
		}
	}

	// "Instant" velocity change on <= "Jerk"
	if (vel_max_target != vel_end)
	{
		if (abs(vel_max_target - vel_end) <= max_vel_change)
		{
			vel_end = vel_max_target;
		}
	}

	accel_time = calc_time_to_accel(accel, vel_start, vel_max_target);
	accel_dist = calc_dist(vel_start, accel, accel_time);

	deccel_time = calc_time_to_accel(accel, vel_max_target, vel_end);
	deccel_dist = calc_dist(vel_max_target, -accel, deccel_time);

	// Triangle Velocity
	if (abs(accel_dist + deccel_dist) > abs(dist))
	{
		result = calc_time_very_complex(dist, vel_start, vel_end, accel);
		// Something's fucky
		if (0 == result)
		{
			// Linear calculation - It's something
			return abs(dist/vel_max_target);
		}
	}
	// Trapezoid Velocity
	else
	{
		dist -= (accel_dist + deccel_dist);
		result += (accel_time + deccel_time);
		result += abs(dist / vel_max_target);
	}

	return result;
}


TimeCalc::PrinterMove::PrinterMove()
: dist(0.0)
{
	fill_n(delta, AXIS_COUNT, 0.0);
	fill_n(unit, AXIS_COUNT, 0.0);
	fill_n(vel, AXIS_COUNT, 0.0);
	fill_n(accel, AXIS_COUNT, 0.0);
	fill_n(vel_start, AXIS_COUNT, 0.0);
	fill_n(vel_end, AXIS_COUNT, 0.0);
}

TimeCalc::TimeCalc()
: m_time(0.0)
{
	reset();
}

void TimeCalc::reset()
{
	m_time = 0.0;

	fill_n(m_extruder_length, EXTRUDER_COUNT, 0.0);
	fill_n(m_pos, AXIS_COUNT, 0.0);
	fill_n(m_accel_max, AXIS_COUNT, 0.0);
	fill_n(m_vel_max, AXIS_COUNT, 0.0);
	fill_n(m_jerk_max, AXIS_COUNT, 0.0);

	m_moves.clear();
}

void TimeCalc::add_move(Point new_pos, double speed)
{
	PrinterMove move_new;
	vector<double> temp;
	Point pos_temp;
	double pos_new[AXIS_COUNT];
	double vel;
	double accel;
	double prev_move_idx;
	static int counter = 0;

	pos_new[0] = new_pos.x;
	pos_new[1] = new_pos.y;
	pos_new[2] = new_pos.z;
	pos_new[3] = new_pos.e;

	pos_temp = Point(m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

	move_new.dist = new_pos.get_dist(pos_temp);

#if DEBUG_CALC
	cout << "---" << endl;
	cout << "Dist - " << move_new.dist << endl;
#endif

	if (0.0 == move_new.dist)
	{
		return;
	}

	// Calculate Delta And Unit Vector
	for (int i = 0 ; i < AXIS_COUNT; ++i)
	{
		move_new.delta[i] = pos_new[i] - m_pos[i];
		move_new.unit[i] = move_new.delta[i] / move_new.dist;
#if DEBUG_CALC
		cout << "Unit [" << i << "] - " << move_new.unit[i] << endl;
#endif
	}

	// Calculate velocity
	temp.clear();
	temp.push_back(speed);
	for (int i = 0 ; i < AXIS_COUNT; ++i)
	{
		if (0.0 != move_new.unit[i] && 0.0 != m_vel_max[i])
		{
			temp.push_back(m_vel_max[i] / move_new.unit[i]);
		}
	}
	vel = abs(vector_min(temp));

#if DEBUG_CALC
	cout << "Vel - " << vel << endl;
#endif

	// Calculate acceleration
	temp.clear();
	for (int i = 0 ; i < AXIS_COUNT; ++i)
	{
		if (0.0 != move_new.unit[i] && 0.0 != m_accel_max[i])
		{
			temp.push_back(m_accel_max[i] / move_new.unit[i]);
		}
	}
	accel = abs(vector_min(temp));

#if DEBUG_CALC
	cout << "Accel - " << accel << endl;
#endif

	// Calculate Component Velocity and Acceleration
	for (int i = 0 ; i < AXIS_COUNT; ++i)
	{
		move_new.vel[i] = vel * move_new.unit[i];
		move_new.accel[i] = accel * move_new.unit[i];
	}

	// Calculate veclocities for/from previous move
	if (!m_moves.empty())
	{
		prev_move_idx = m_moves.size() - 1;

		for (int i = 0; i < AXIS_COUNT; ++i)
		{
			// If we're changing direction
			if (m_moves[prev_move_idx].vel[i] * move_new.vel[i] < 0)
			{
				m_moves[prev_move_idx].vel_end[i] = 0.0;
			}
			else
			{
				// If we're accelerating
				if (abs(m_moves[prev_move_idx].vel[i]) < abs(move_new.vel[i]))
				{
					m_moves[prev_move_idx].vel_end[i] = m_moves[prev_move_idx].vel[i];
				}
				else
				{
					m_moves[prev_move_idx].vel_end[i] = move_new.vel[i];
				}
			}
		}

		// Calc end velocity for prev move
		temp.clear();
		for (int i = 0; i < AXIS_COUNT; ++i)
		{
			if (0.0 != m_moves[prev_move_idx].unit[i])
			{
				temp.push_back(m_moves[prev_move_idx].vel_end[i] / m_moves[prev_move_idx].unit[i]);
			}
		}
		vel = vector_min(temp);

		for (int i = 0; i < AXIS_COUNT; ++i)
		{
			m_moves[prev_move_idx].vel_end[i] = vel * m_moves[prev_move_idx].unit[i];
			move_new.vel_start[i] = m_moves[prev_move_idx].vel_end[i];
#if DEBUG_CALC
			cout << "Vel Start [" << i << "] - " << m_moves[prev_move_idx].vel_end[i] << endl;
#endif
		}

		// Do a move calculation
		flush();
	}

	m_moves.push_back(move_new);
	for (int i = 0; i < AXIS_COUNT; ++i)
	{
		m_pos[i] = pos_new[i];
	}
}

void TimeCalc::set_pos(Point new_pos)
{
	flush();
	if (new_pos.e != m_pos[3])
	{
		m_extruder_length[0] += m_pos[3];
		m_extruder_length[0] -= new_pos.e;
	}

	m_pos[0] = new_pos.x;
	m_pos[1] = new_pos.y;
	m_pos[2] = new_pos.z;
	m_pos[3] = new_pos.e;
}

void TimeCalc::flush()
{
	vector<double> times;
	double max;

	for (PrinterMove& move : m_moves)
	{
		// Find the slowest axis and add it to the total time
		times.clear();
		for (int i = 0; i < AXIS_COUNT; ++i)
		{
			times.push_back(calc_time_complex(move.delta[i], move.accel[i],
				m_jerk_max[i], move.vel_start[i], move.vel_end[i], move.vel[i]));
		}
		max = vector_max(times);

		if (max < 0)
		{
			printf("Dist - %10.5f - %10.5f\n", move.dist, max);

			for (int i = 0; i < AXIS_COUNT; ++i)
			{
				printf("Delta [%d] - %10.5f\n", i, move.delta[i]);
			}

			for (int i = 0; i < AXIS_COUNT; ++i)
			{
				printf("Unit [%d]  - %10.5f\n", i, move.unit[i]);
			}

			for (int i = 0; i < AXIS_COUNT; ++i)
			{
				printf("V0 [%d]    - %10.5f\n", i, move.vel_start[i]);
			}

			for (int i = 0; i < AXIS_COUNT; ++i)
			{
				printf("Vel [%d]   - %10.5f\n", i, move.vel[i]);
			}

			for (int i = 0; i < AXIS_COUNT; ++i)
			{
				printf("V2 [%d]    - %10.5f\n", i, move.vel_end[i]);
			}

			for (int i = 0; i < AXIS_COUNT; ++i)
			{
				printf("Accel [%d] - %10.5f\n", i, move.accel[i]);
			}
		}
		m_time += max;
	}
	m_moves.clear();
}

void TimeCalc::add_time(double time)
{
	m_time += time;
}

double TimeCalc::get_time() const
{
	return m_time;
}

double TimeCalc::get_extruder_length(int idx) const
{
	if (idx >= EXTRUDER_COUNT)
	{
		return 0.0;
	}

	return m_extruder_length[idx] + m_pos[3 + idx];
}

Point TimeCalc::get_pos() const
{
	return Point(m_pos[0], m_pos[1], m_pos[2], m_pos[3]);
}

void TimeCalc::set_max_acceleration(int axis, double value)
{
	if ((axis > 0) && (axis < AXIS_COUNT))
	{
		m_accel_max[axis] = value;
	}
}

void TimeCalc::set_max_velocity(int axis, double value)
{
	if ((axis > 0) && (axis < AXIS_COUNT))
	{
		m_vel_max[axis] = value;
	}
}

void TimeCalc::set_max_jerk(int axis, double value)
{
	if ((axis > 0) && (axis < AXIS_COUNT))
	{
		m_jerk_max[axis] = value;
	}
}
