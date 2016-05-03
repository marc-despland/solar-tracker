#include "phidget.h"
#include "log.h"
#include <sstream>
#include <cmath>
#include <ctgmath>

map<int, Phidget *>	Phidget::interfaces;

Phidget * Phidget::singleton=NULL;

Phidget * Phidget::board() {
	return Phidget::singleton;
};

bool Phidget::attach() {
	Phidget::singleton=new Phidget();
	return Phidget::singleton->attached();
}

Phidget::Phidget() {
	int result;
	const char *err;
	Log::logger->log("PHIDGET",DEBUG) << "Constructor call" << endl;
	CPhidgetInterfaceKit_create(&(this->ifKit));

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)this->ifKit, Phidget::attachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)this->ifKit, Phidget::detachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)this->ifKit, Phidget::errorHandler, NULL);

	//Registers a callback that will run if an input changes.
	//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetInterfaceKit_set_OnInputChange_Handler(this->ifKit, Phidget::inputChangeHandler, NULL);

	//Registers a callback that will run if the sensor value changes by more than the OnSensorChange trig-ger.
	//Requires the handle for the IntefaceKit, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetInterfaceKit_set_OnSensorChange_Handler(this->ifKit, Phidget::sensorChangeHandler, NULL);

	//Registers a callback that will run if an output changes.
	//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetInterfaceKit_set_OnOutputChange_Handler(this->ifKit, Phidget::outputChangeHandler, NULL);

	//open the interfacekit for device connections
	CPhidget_open((CPhidgetHandle)this->ifKit, -1);

	Log::logger->log("PHIDGET",DEBUG) << "Waiting to detect the Phidget board" << endl;
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		Log::logger->log("PHIDGET",ERROR) << "An error occurs on detection of the Phidget board : " << err << endl;
		this->serial=-1;
	} else {
		CPhidget_getSerialNumber((CPhidgetHandle)this->ifKit, &(this->serial));
		CPhidget_getDeviceName((CPhidgetHandle)this->ifKit, &(this->name));
		Phidget::interfaces[this->serial]=this;
		Log::logger->log("PHIDGET",NOTICE) << "Phidget board " << this->name <<"(" << this->serial<<") attached" << endl;
	}
};

Phidget::~Phidget() {
	Log::logger->log("PHIDGET",DEBUG) << "Destructor call" << endl;
};

int CCONV Phidget::attachHandler(CPhidgetHandle IFK, void *userptr) {
	Log::logger->log("PHIDGET",DEBUG) << "AttachHandler" << endl;
	int serialNo;
	const char *name;

	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);
	Log::logger->log("PHIDGET",DEBUG) << "\tName\t:"<< name << endl;
	Log::logger->log("PHIDGET",DEBUG) << "\tSerial\t:"<< serialNo << endl;
	return 0;
};

int CCONV Phidget::detachHandler(CPhidgetHandle IFK, void *userptr){
	Log::logger->log("PHIDGET",DEBUG) << "DetachHandler" << endl;
	int serialNo;
	const char *name;

	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);
	Log::logger->log("PHIDGET",DEBUG) << "\tName\t:"<< name << endl;
	Log::logger->log("PHIDGET",DEBUG) << "\tSerial\t:"<< serialNo << endl;
	if (Phidget::interfaces.count(serialNo)==1) {
		Phidget::interfaces.erase(serialNo);
	}
	return 0;
};

int CCONV Phidget::errorHandler(CPhidgetHandle IFK, void *userptr, int errorCode, const char *unknown) {
	Log::logger->log("PHIDGET",ERROR) << "ErrorHandler code:" <<errorCode << " - " << unknown<< endl;
	return 0;
};

int CCONV Phidget::inputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int index, int state){
	Log::logger->log("PHIDGET",DEBUG) << "InputChangeHandler index:"<<index<< " state:"<< state << endl;
	if ((index==0) || (index==1)) {
		if (Phidget::attached()) {
			Phidget::board()->setOutput(index, state);
		}
	}
	return 0;
};

int CCONV Phidget::outputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int index, int state){
	Log::logger->log("PHIDGET",DEBUG) << "OutputChangeHandler index:"<<index<< " state:"<< state << endl;
	return 0;
};

int CCONV Phidget::sensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int index, int value){
	Log::logger->log("PHIDGET",DEBUG) << "SensorChangeHandler index:"<<index<< " state:"<< value << endl;
	return 0;
};


string Phidget::getStatus() {
	int numInputs, numOutputs, numSensors;
	stringstream tmp;
	if (this->serial >0) {
		int version;
		CPhidget_getDeviceVersion((CPhidgetHandle)this->ifKit, &version);
		tmp << "\t\"phidget\" : {" <<endl;
		tmp << "\t\t\"name\": \"" << this->name<<"\","<<endl;
		tmp << "\t\t\"serial\": \"" << this->serial<<"\","<<endl;
		tmp << "\t\t\"version\": \"" << version<<"\","<<endl;
		tmp << "\t\t\"voltage\": \"" << this->getVoltage()<<"\","<<endl;
		tmp << "\t\t\"left lux\": \"" << this->getLeftLux()<<"\","<<endl;
		tmp << "\t\t\"right lux\": \"" << this->getRightLux()<<"\","<<endl;
		tmp << "\t\t\"inputs\": [ ";
		CPhidgetInterfaceKit_getInputCount(this->ifKit, &numInputs);
		for (int i=0; i<numInputs; i++) {
			int value;
			CPhidgetInterfaceKit_getInputState(this->ifKit, i, &value);
			if (i<numInputs-1) {
				tmp << value << ", ";
			} else {
				tmp << value << "]," << endl;
			}
		}
		tmp << "\t\t\"outputs\": [ ";
		CPhidgetInterfaceKit_getOutputCount(this->ifKit, &numOutputs);
		for (int i=0; i<numOutputs; i++) {
			int value;
			CPhidgetInterfaceKit_getOutputState(this->ifKit, i, &value);
			if (i<numOutputs-1) {
				tmp << value << ", ";
			} else {
				tmp << value << "]," << endl;
			}
		}
		tmp << "\t\t\"sensors\": [ ";
		CPhidgetInterfaceKit_getSensorCount(this->ifKit, &numSensors);
		for (int i=0; i<numSensors; i++) {
			int value;
			CPhidgetInterfaceKit_getSensorValue(this->ifKit, i, &value);
			if (i<numSensors-1) {
				tmp << value << ", ";
			} else {
				tmp << value << "]" << endl;
			}
		}
		tmp << "\t}";

	} else {
		tmp << "\t\"phidget\" : {}";
	}
	return tmp.str();
};

void Phidget::setOutput(int index, bool state) {
	if (this->serial>0) {
		int numOutputs;
		CPhidgetInterfaceKit_getOutputCount(this->ifKit, &numOutputs);
		if (index<numOutputs) {
			if (state) {
				CPhidgetInterfaceKit_setOutputState(this->ifKit, index, PTRUE);
			} else {
				CPhidgetInterfaceKit_setOutputState(this->ifKit, index, PFALSE);
			}
		}
	}
}

bool Phidget::attached() {
	if (Phidget::singleton==NULL) return false;
	return (Phidget::singleton->serial>0);
}

double Phidget::getVoltage() {
	if (this->serial>0) {
		int value;
		CPhidgetInterfaceKit_getSensorValue(this->ifKit, 5, &value);
		return ((((double) value/200)-2.5)/0.0681);
	} else {
		return -1;
	}
}

double Phidget::getRightLux() {
	if (this->serial>0) {
		int value;
		CPhidgetInterfaceKit_getSensorValue(this->ifKit, 6, &value);
		return (exp((0.02293*value)+0.07776));
	} else {
		return -1;
	}
}

double Phidget::getLeftLux() {
	if (this->serial>0) {
		int value;
		CPhidgetInterfaceKit_getSensorValue(this->ifKit, 7, &value);
		return (exp((0.02167*value)-1.0321));
	} else {
		return -1;
	}
}