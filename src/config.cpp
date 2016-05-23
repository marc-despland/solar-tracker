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
			this->inputscan=params->get("input-earth")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter input-earth" << endl;
	}
	try {
		if (params->get("output-earth")->isAssign()) {
			this->outputscan=params->get("output-earth")->asInt();
		}
	} catch (UnknownParameterNameException &e) {
		Log::logger->log("CONFIG", DEBUG) << "Not defined parameter output-earth" << endl;
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
	return me->outputearth;
}

int Config::outputIndexEarth(){
	return me->inputearth;
}

int Config::outputIndexScan(){
	return me->outputscan;
}

