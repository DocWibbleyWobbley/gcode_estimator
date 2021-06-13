#include "Point.h"
#include <math.h>
#include <stdio.h>

Point::Point(double x, double y, double z, double e)
: x(x), y(y), z(z), e(e)
{
}

double Point::get_dist(const Point& other) const
{
	double result;
	double dx;
	double dy;
	double dz;
	double de;

	dx = other.x - x;
	dy = other.y - y;
	dz = other.z - z;
	de = abs(other.e - e);

	result = sqrt(dx * dx + dy * dy + dz * dz);

	if (0 == result)
	{
		result = de;
	}

	return result;
}

void Point::print()
{
	printf("X - %10.5f, Y - %10.5f, Z - %10.5f, E - %10.5f", x, y, z, e);
}
