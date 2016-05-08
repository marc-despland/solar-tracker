#ifndef _PHIDGET_H
#define _PHIDGET_H

using namespace std;
#include <phidget21.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

class Phidget {
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

protected:

	int serial;
	const char * name;
	CPhidgetInterfaceKitHandle ifKit;
	void setOutput(int index, bool state);

	static int CCONV attachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV detachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV errorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown);
	static int CCONV inputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State);
	static int CCONV outputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State);
	static int CCONV sensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value);
};
#endif