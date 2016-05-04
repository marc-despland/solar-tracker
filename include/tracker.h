#ifndef _TRACKER_H
#define _TRACKER_H


class Tracker {
public:
	Tracker();
	void scan();
	double angleleft;
	double angleright;
	double maxleft;
	double maxright;
	double maxlux;
	double anglelux;

};

#endif