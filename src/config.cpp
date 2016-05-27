#include "config.h"
#include "log.h"


Config * Config::me=new Config();

Config::Config() {
	this->phidget=false;
	this->maestro=false;
	this->inputscan=Config::NOTDEFINED;
	this->outputscan=Config::NOTDEFINED;
	this->inputearth=Config::NOTDEFINED;
	this->outputearth=Config::NOTDEFINED;
	this->inputregulator=Config::NOTDEFINED;
	this->outputregulator=Config::NOTDEFINED;

	this->servoearth=Config::NOTDEFINED;
	this->servoreverseearth=Config::NOTDEFINED;

	this->coord_longitude=0;
	this->coord_latitude=0;

}



Config * Config::config() {
	return me;
}

void Config::loadOptions(Options * options) {

}

void Config::loadParameters(Parameters * params) {
	try {
		this->maestro=params->get("have-maestro")->asBool();
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", EMERGENCY) << "Not defined parameter have-maestro" << endl;
	}
	try {
		this->phidget=params->get("have-phidget")->asBool();
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", EMERGENCY) << "Not defined parameter have-phidget" << endl;
	}
	try {
		if (params->get("input-scan")->isAssign()) {
			this->inputscan=params->get("input-scan")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter input-scan" << endl;
	}
	try {
		if (params->get("output-scan")->isAssign()) {
			this->outputscan=params->get("output-scan")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter output-scan" << endl;
	}
	try {
		if (params->get("input-earth")->isAssign()) {
			this->inputearth=params->get("input-earth")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter input-earth" << endl;
	}
	try {
		if (params->get("output-earth")->isAssign()) {
			this->outputearth=params->get("output-earth")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter output-earth" << endl;
	}
	try {
		if (params->get("input-regulator")->isAssign()) {
			this->inputregulator=params->get("input-regulator")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter input-regulator" << endl;
	}
	try {
		if (params->get("output-regulator")->isAssign()) {
			this->outputregulator=params->get("output-regulator")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter output-earth" << endl;
	}


	try {
		if (params->get("earth-servo")->isAssign()) {
			this->servoearth=params->get("earth-servo")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter earth-servo" << endl;
	}
	try {
		if (params->get("output-regulator")->isAssign()) {
			this->servoreverseearth=params->get("reverse-earth-servo")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter reverse-earth-servo" << endl;
	}



	try {
		if (params->get("longitude")->isAssign()) {
			this->coord_longitude=params->get("longitude")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter longitude" << endl;
	}
	try {
		if (params->get("latitude")->isAssign()) {
			this->coord_latitude=params->get("latitude")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter latitude" << endl;
	}


}


bool Config::hasPhidget() {
	return me->phidget;
}

bool Config::hasMaestro(){
	return me->maestro;
}

int Config::inputIndexScan(){
	return me->inputscan;
}

int Config::inputIndexEarth(){
	return me->inputearth;
}

int Config::outputIndexEarth(){
	return me->outputearth;
}

int Config::outputIndexScan(){
	return me->outputscan;
}


int Config::inputIndexRegulator(){
	return me->outputregulator;
}

int Config::outputIndexRegulator(){
	return me->outputregulator;
}

double Config::latitude() {
	return this->coord_latitude;
}

double Config::longitude() {
	return this->coord_longitude;
}

bool Config::hasServoEarth() {
	return this->servoearth!=Config::NOTDEFINED;
}
int Config::servoEarth(){
	return this->servoearth;
}

bool Config::hasServoReverseEarth(){
	return this->servoreverseearth!=Config::NOTDEFINED;
}
int Config::servoReverseEarth(){
	return this->servoreverseearth;
}
