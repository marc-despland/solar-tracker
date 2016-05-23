#ifndef _PHIDGET_H
#define _PHIDGET_H

using namespace std;
#include <phidget21.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include "phidgetaction.h"
#include <vector>
#include "eventmanager.h"
#include "phidgetlistener.h"
#include "inputevent.h"

class Phidget :public EventManager {
public:
	Phidget();
	~Phidget();
	static map<int , Phidget *>	interfaces;
	static Phidget * singleton;
	static bool attach();

	static string getStatus();
	static bool attached();
	static Phidget * board();
	double getVoltage();
	double getRightLux();
	double getLeftLux();
	void addInputHandler(PhidgetListener * listener);
	void setOutput(int index, bool state);

protected:

	int serial;
	const char * name;
	CPhidgetInterfaceKitHandle ifKit;


	static int CCONV attachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV detachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV errorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown);
	static int CCONV inputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State);
	static int CCONV outputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State);
	static int CCONV sensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value);
};
#endif