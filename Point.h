#ifndef GCODE_POINT_HPP_
#define GCODE_POINT_HPP_

class Point
{
public:
	Point(double x = 0, double y = 0, double z = 0, double e = 0);

	double x;
	double y;
	double z;
	double e;

	double get_dist(const Point& other) const;
	void print();
};

#endif // GCODE_POINT_HPP_
