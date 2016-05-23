#ifndef _CONFIG_H
#define _CONFIG_H

#include "options.h"
#include "parameters.h"


class Config {

public:
	static const int NOTDEFINED=-1;
	static Config * config();
	void loadOptions(Options * options);
	void loadParameters(Parameters * params);

	bool hasPhidget();
	bool hasMaestro();
	int inputIndexScan();
	int inputIndexEarth();
	int outputIndexEarth();
	int outputIndexScan();

protected:
	Config();
	static Config * me;
	bool phidget;
	bool maestro;
	int inputscan;
	int outputscan;
	int inputearth;
	int outputearth;
};

#endif