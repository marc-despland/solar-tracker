#include "maestro.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

const uint16_t Maestro::vendorId = 0x1ffb;
const size_t Maestro::nProductIds = 4;
const uint16_t Maestro::productIds[] = { 0x0089, 0x008a, 0x008b, 0x008c };

#define MAX_SERVOS  24

Maestro * Maestro::controller=new Maestro();

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
  size_t ret;
  ret=controlTransferData(0xC0, REQUEST_GET_VARIABLES, 0, 0, data, datalength);

  MicroMaestroVariables tmp = *(MicroMaestroVariables*)data;
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
    (*servos)[i] = *(ServoStatus*)(data + sizeof(MicroMaestroVariables) + sizeof(ServoStatus) * i);
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
  controlTransfer(0x40, REQUEST_SET_TARGET, value, servo);
}

void Maestro::setSpeed(uint8_t servo, uint16_t value) {
  controlTransfer(0x40, REQUEST_SET_SERVO_VARIABLE, value, servo);
}

void Maestro::setAcceleration(uint8_t servo, uint16_t value) {
  // setting high bit on servo number indicates acceleration
  controlTransfer(0x40, REQUEST_SET_SERVO_VARIABLE, value, servo | 0x80);
}

size_t Maestro::getServosCount() {
  return servoCount;
}

std::string Maestro::getStatus() {
  ServoStatus * servos;
  Maestro::controller->getVariablesServos(&servos);
  std::stringstream tmp;
  tmp << "\t\"maestro\" : {" <<std::endl;
  tmp << "\t\t\"position\" : [";
  for (int i=0; i<Maestro::controller->getServosCount(); i++) {
      if (i<Maestro::controller->getServosCount()-1) {
        tmp << servos[i].position << ", ";
      } else {
        tmp << servos[i].position << "]" << std::endl;
      }
    }
  tmp << "\t\"}" <<std::endl;
  return tmp.str();
}