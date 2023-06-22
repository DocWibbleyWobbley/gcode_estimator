all:
	g++ -std=c++11 gcode.cpp main.cpp Point.cpp TimeCalc.cpp -o gcode_time
