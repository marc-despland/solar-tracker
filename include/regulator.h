#ifndef _REGULATOR_H
#define _REGULATOR_H
#include "config.h"
#include "phidgetlistener.h"


class Regulator : public PhidgetListener {
public:
	static Regulator * regulator();
	static void attachPhidget();
	void scan();
	void inputEvent(int index, int state);
protected:
	static Regulator * me;
	Regulator();

};

#endif