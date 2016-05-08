#include "maestro.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "log.h"

const uint16_t Maestro::vendorId = 0x1ffb;
const size_t Maestro::nProductIds = 4;
const uint16_t Maestro::productIds[] = { 0x0089, 0x008a, 0x008b, 0x008c };

#define MAX_SERVOS  24

Maestro * Maestro::controller=NULL;
bool Maestro::detected=false;

bool Maestro::init() {
  Maestro::detected=true;
  try {
    Maestro::controller=new Maestro();
  }catch(NoMaestroFoundError &e) {
    Maestro::detected=false;
  }
  return Maestro::detected;
}

Maestro::Maestro() {
  libusb_init(&ctx);

  libusb_device** list;
  int count = libusb_get_device_list(ctx, &list);

  // scan usb devices looking for a Micro Maestro
  bool found = false;
  for (int i = 0; i < count; i++) {
    libusb_device* device = list[i];

    // check if this is a known device
    struct libusb_device_descriptor desc;
    libusb_get_device_descriptor(device, &desc);
    if (!isMaestroDevice(desc)) { continue; }
    productId = desc.idProduct;
    Log::logger->log("MAESTRO",DEBUG) << "productId " << productId<< endl;
    switch (productId) {
      case 0x0089:
        servoCount=6;
        break;
       case 0x008a:
        servoCount=12;
        break;
      case 0x008b:
        servoCount=18;
        break;
      case 0x008c:
        servoCount=24;
        break;
   }

    // attempt to open device
    int ret = libusb_open(device, &deviceHandle);
    throwIfError<MaestroInitializationError>(ret);

    // found a matching device that succesfully opened
    found = true;
    break;
  }

  libusb_free_device_list(list, 1);

  if (!found) {
    // there is no open device handle, so no need to close it
    libusb_exit(ctx);

    // fail fast; more clear than trying to open devices in the background
    throw NoMaestroFoundError();
  }
}

Maestro::~Maestro() {
  // libusb cleanup
  libusb_close(deviceHandle);
  libusb_exit(ctx);
}

bool Maestro::isMaestroDevice(libusb_device_descriptor& desc) {
  if (desc.idVendor != vendorId) { return false; }
  for (uint8_t i = 0; i < nProductIds; i++) {
    if (desc.idProduct == productIds[i]) {
      // vendor and product both matched
      return true;
    }
  }
  // no product id match
  return false;
}

int Maestro::controlTransfer(uint8_t type, uint8_t request, uint16_t value, uint16_t index) {
  int ret = libusb_control_transfer(
    deviceHandle, type, request,
    value, index,
    (unsigned char*) 0, 0, 5000);
  return throwIfError<MaestroControlError>(ret);
}

int Maestro::controlTransferData(uint8_t type, uint8_t request, uint16_t value, uint16_t index, void * data, ushort length) {
  int ret = libusb_control_transfer(
    deviceHandle, type, request,
    value, index,
    (unsigned char*) data, length, 5000);
  return throwIfError<MaestroControlError>(ret);
}


template <class T>
int Maestro::throwIfError(int code) {
  if (code < 0) {
    // code is an error, throw
    T* err = new T(errorDescription(code));
    throw err;
  }
  return code;
}

std::string Maestro::errorDescription(int error) {
  switch (error) {
    case -1: return "I/O error.";
    case -2: return "Invalid parameter.";
    case -3: return "Access denied.";
    case -4: return "Device does not exist.";
    case -5: return "No such entity.";
    case -6: return "Busy.";
    case -7: return "Timeout.";
    case -8: return "Overflow.";
    case -9: return "Pipe error.";
    case -10: return "System call was interrupted.";
    case -11: return "Out of memory.";
    case -12: return "Unsupported/unimplented operation.";
    case -99: return "Other error";
    default: return "Unknown error code.";
  };
}


void Maestro::getVariablesMicroMaestro(MaestroVariables variables, short * stack[], ushort * callStack[], ServoStatus * servos[]) {
  size_t datalength= sizeof(MicroMaestroVariables) + servoCount * sizeof(ServoStatus);
  char * data= new char[datalength];
  controlTransferData(0xC0, REQUEST_GET_VARIABLES, 0, 0, data, datalength);

  Log::logger->log("MAESTRO",DEBUG) << "Sizeof MicroMaestroVariables " << sizeof(MicroMaestroVariables)<< endl;
  Log::logger->log("MAESTRO",DEBUG) << "Sizeof ServoStatus " << sizeof(ServoStatus)<< endl;
  Log::logger->log("MAESTRO",DEBUG) << "Sizeof uint8_t " << sizeof(uint8_t)<< endl;
  Log::logger->log("MAESTRO",DEBUG) << "Sizeof uint16_t " << sizeof(uint16_t)<< endl;
  Log::logger->log("MAESTRO",DEBUG) << "Sizeof int16_t " << sizeof(int16_t)<< endl;
  Log::logger->log("MAESTRO",DEBUG) << "Datalength " << datalength<< endl;
   
  MicroMaestroVariables tmp = *(MicroMaestroVariables *) data;
  variables.stackPointer = tmp.stackPointer;
  variables.callStackPointer = tmp.callStackPointer;
  variables.errors = tmp.errors;
  variables.programCounter = tmp.programCounter;
  variables.scriptDone = tmp.scriptDone;
  variables.performanceFlags = 0;

  *servos = new ServoStatus[servoCount];
  //printf("Reading variables %lu\n", servoCount);
  for (uint8_t i = 0; i < servoCount; i++){
    //printf("\tServos %d \n", i);
    Log::logger->log("MAESTRO",DEBUG) << "servo " << i<< endl;
    ServoStatus * mde=(ServoStatus *)(data + sizeof(MicroMaestroVariables) + sizeof(ServoStatus) * i);
    Log::logger->log("MAESTRO",DEBUG) << "\tposition " << mde->position<< endl;

    (*servos)[i] = *(ServoStatus*)(data + + sizeof(MicroMaestroVariables) + sizeof(ServoStatus) * i);
  }

  *stack = new short[variables.stackPointer];
  for(size_t i = 0; i < variables.stackPointer; i++) {
    *stack[i] = *(tmp.stack+i);
  }

  *callStack = new ushort[variables.callStackPointer];
  for (size_t i = 0; i < variables.callStackPointer; i++) {
    *callStack[i] = *(tmp.callStack + i);
  }
}


void Maestro::getVariablesServos(ServoStatus * servos[]){
  if (servoCount==6) {
    MaestroVariables variables;
    short * stack;
    ushort * callStack;
    getVariablesMicroMaestro(variables, &stack, &callStack, servos);
  } else {
    //Not implemented
    //getVariablesMiniMaestro(out callStack);
  }
}



void Maestro::setTarget(uint8_t servo, uint16_t value) {
	if (Maestro::controller!=NULL) {
  		Maestro::controller->controlTransfer(0x40, REQUEST_SET_TARGET, value, servo);
  	}
}

void Maestro::setSpeed(uint8_t servo, uint16_t value) {
	if (Maestro::controller!=NULL) {
  		Maestro::controller->controlTransfer(0x40, REQUEST_SET_SERVO_VARIABLE, value, servo);
  	}
}

void Maestro::setAcceleration(uint8_t servo, uint8_t value) {
	if (Maestro::controller!=NULL) {
  		Maestro::controller->controlTransfer(0x40, REQUEST_SET_SERVO_VARIABLE, value, servo | 0x80);
  	}
}

size_t Maestro::getServosCount() {
  return servoCount;
}

std::string Maestro::getStatus() {
	std::stringstream tmp;
	if (Maestro::detected) {
		ServoStatus * servos;
		Maestro::controller->getVariablesServos(&servos);
  
		tmp << "\t\"maestro\" : {" <<std::endl;
		tmp << "\t\t\"servos\" : [";
		for (size_t i=0; i<Maestro::controller->getServosCount(); i++) {
			tmp << "{" <<std::endl;
			tmp << "\t\t\t\"code\": " << i << ","<<std::endl;
			tmp << "\t\t\t\"position\": " << servos[i].position << ","<<std::endl;
			tmp << "\t\t\t\"target\": " << servos[i].target << ","<<std::endl;
			tmp << "\t\t\t\"speed\": " << servos[i].speed << ","  <<std::endl;
			tmp << "\t\t\t\"acceleration\": " << (uint16_t) servos[i].acceleration << "," <<std::endl;
			tmp << "\t\t\t\"min\": " << (uint16_t) Maestro::getServoMinPosition(i) << "," <<std::endl;
			tmp << "\t\t\t\"neutral\": " << (uint16_t) Maestro::getServoNeutralPosition(i) << "," <<std::endl;
			tmp << "\t\t\t\"max\": " << (uint16_t) Maestro::getServoMaxPosition(i) <<std::endl;
			if (i<Maestro::controller->getServosCount()-1) {
				tmp <<  "\t\t}, ";
			} else {
				tmp << "\t\t}]" << std::endl;
			}
		}
		tmp << "\t}" <<std::endl;
  	} else {
  		tmp << "\t\"maestro\" : {}"<<std::endl;
	}
	return tmp.str();
}


uint16_t Maestro::getServoMinPosition(uint8_t servo) {
 	uint8_t value=0;
 	if (Maestro::controller!=NULL) {
 		char * data= new char[1];
		Maestro::controller->controlTransferData((uint8_t) 0xC0, (uint8_t) REQUEST_GET_PARAMETER, (uint16_t) 0, (uint16_t) (32+9*servo), data, (ushort) 1);
   		value=*(uint8_t *) data;
   	}
	return (uint16_t) value*64;
}
uint16_t Maestro::getServoMaxPosition(uint8_t servo) {
 	uint8_t value=0;
 	if (Maestro::controller!=NULL) {
 		char * data= new char[1];
		Maestro::controller->controlTransferData((uint8_t) 0xC0, (uint8_t) REQUEST_GET_PARAMETER, (uint16_t) 0, (uint16_t) (33+9*servo), data, (ushort) 1);
   		value=*(uint8_t *) data;
   	}
	return (uint16_t) value*64;
}
uint16_t Maestro::getServoNeutralPosition(uint8_t servo) {
 	uint16_t value=0;
 	if (Maestro::controller!=NULL) {
 		char * data= new char[2];
		Maestro::controller->controlTransferData((uint8_t) 0xC0, (uint8_t) REQUEST_GET_PARAMETER, (uint16_t) 0, (uint16_t) (34+9*servo), data, (ushort) 2);
   		value=*(uint16_t *) data;
   	}
	return value;
}


void Maestro::setServoMinPosition(uint8_t servo, uint16_t min) {
 	if (Maestro::controller!=NULL) {
 		uint8_t value=(min/64);
 		uint16_t index= (uint16_t) ((1 << 8) + (32+9*servo));
 		Maestro::controller->controlTransfer(0x40, REQUEST_SET_PARAMETER, value, index);
	}
}

void Maestro::setServoMaxPosition(uint8_t servo, uint16_t max) {
 	if (Maestro::controller!=NULL) {
 		uint8_t value=(max/64);
 		uint16_t index= (uint16_t) ((1 << 8) + (33+9*servo));
 		Maestro::controller->controlTransfer(0x40, REQUEST_SET_PARAMETER, value, index);
	}
}
void Maestro::setServoNeutralPosition(uint8_t servo, uint16_t value) {
 	if (Maestro::controller!=NULL) {
 		uint16_t index= (uint16_t) ((2 << 8) + (34+9*servo));
 		Maestro::controller->controlTransfer(0x40, REQUEST_SET_PARAMETER, value, index);
	}
}
/*
        private unsafe ushort getRawParameter(uscParameter parameter)
        {
            Range range = Usc.getRange(parameter);
            ushort value = 0;
            byte[] array = new byte[range.bytes];
            try
            {
                controlTransfer(0xC0, (byte)uscRequest.REQUEST_GET_PARAMETER, 0, (ushort)parameter, array);
            }
            catch (Exception e)
            {
                throw new Exception("There was an error getting parameter " + parameter.ToString() + " from the device.", e);
            }
            if (range.bytes == 1)
            {
                // read a single byte
                fixed (byte* pointer = array)
                {
                    value = *(byte*)pointer;
                }
            }
            else
            {
                // read two bytes
                fixed (byte* pointer = array)
                {
                    value = *(ushort*)pointer;
                }
            }
            return value;
        }
        protected static Range getRange(uscParameter parameterId)
        {
            if (parameterId == uscParameter.PARAMETER_INITIALIZED)
                return Range.u8;

            switch (parameterId)
            {
                case uscParameter.PARAMETER_SERVOS_AVAILABLE:
                    return Range.u8;
                case uscParameter.PARAMETER_SERVO_PERIOD:
                    return Range.u8;
                case uscParameter.PARAMETER_MINI_MAESTRO_SERVO_PERIOD_L:
                    return Range.u8;
                case uscParameter.PARAMETER_MINI_MAESTRO_SERVO_PERIOD_HU:
                    return Range.u16;
                case uscParameter.PARAMETER_SERVO_MULTIPLIER:
                    return Range.u8;
                case uscParameter.PARAMETER_CHANNEL_MODES_0_3:
                case uscParameter.PARAMETER_CHANNEL_MODES_4_7:
                case uscParameter.PARAMETER_CHANNEL_MODES_8_11:
                case uscParameter.PARAMETER_CHANNEL_MODES_12_15:
                case uscParameter.PARAMETER_CHANNEL_MODES_16_19:
                case uscParameter.PARAMETER_CHANNEL_MODES_20_23:
                case uscParameter.PARAMETER_ENABLE_PULLUPS:
                    return Range.u8;
                case uscParameter.PARAMETER_SERIAL_MODE:
                    return new Range(1, 0, 3);
                case uscParameter.PARAMETER_SERIAL_BAUD_DETECT_TYPE:
                    return new Range(1, 0, 1);
                case uscParameter.PARAMETER_SERIAL_NEVER_SUSPEND:
                    return Range.boolean;
                case uscParameter.PARAMETER_SERIAL_TIMEOUT:
                    return Range.u16;
                case uscParameter.PARAMETER_SERIAL_ENABLE_CRC:
                    return Range.boolean;
                case uscParameter.PARAMETER_SERIAL_DEVICE_NUMBER:
                    return Range.u7;
                case uscParameter.PARAMETER_SERIAL_FIXED_BAUD_RATE:
                    return Range.u16;
                case uscParameter.PARAMETER_SERIAL_MINI_SSC_OFFSET:
                    return new Range(1, 0, 254);
                case uscParameter.PARAMETER_SCRIPT_CRC:
                    return Range.u16;
                case uscParameter.PARAMETER_SCRIPT_DONE:
                    return Range.boolean;
            }

            // must be one of the servo parameters
            switch ((((byte)parameterId - (byte)uscParameter.PARAMETER_SERVO0_HOME) % 9) +
                    (byte)uscParameter.PARAMETER_SERVO0_HOME)
            {
                case (byte)uscParameter.PARAMETER_SERVO0_HOME:
                case (byte)uscParameter.PARAMETER_SERVO0_NEUTRAL:
                    return new Range(2, 0, 32440); // 32640 - 200
                case (byte)uscParameter.PARAMETER_SERVO0_RANGE:
                    return new Range(1, 1, 50); // the upper limit could be adjusted
                case (byte)uscParameter.PARAMETER_SERVO0_SPEED:
                case (byte)uscParameter.PARAMETER_SERVO0_MAX:
                case (byte)uscParameter.PARAMETER_SERVO0_MIN:
                case (byte)uscParameter.PARAMETER_SERVO0_ACCELERATION:
                    return Range.u8;
            }

            throw new ArgumentException("Invalid parameterId " + parameterId.ToString() + ", can not determine the range of this parameter.");
        }

        protected struct Range
        {
            public Byte bytes;
            public Int32 minimumValue;
            public Int32 maximumValue;

            internal Range(Byte bytes, Int32 minimumValue, Int32 maximumValue)
            {
                this.bytes = bytes;
                this.minimumValue = minimumValue;
                this.maximumValue = maximumValue;
            }

            public Boolean signed
            {
                get
                {
                    return minimumValue < 0;
                }
            }

            internal static Range u32 = new Range(4, 0, 0x7FFFFFFF);
            internal static Range u16 = new Range(2, 0, 0xFFFF);
            internal static Range u12 = new Range(2, 0, 0x0FFF);
            internal static Range u10 = new Range(2, 0, 0x03FF);
            internal static Range u8 = new Range(1, 0, 0xFF);
            internal static Range u7 = new Range(1, 0, 0x7F);
            internal static Range boolean = new Range(1, 0, 1);
        }*/