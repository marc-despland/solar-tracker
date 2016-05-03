#ifndef _SERVO_H
#define _SERVO_H

#include <stdint.h>
#include "maestro.h"
class Servo {

public:
		Servo();
		Servo(uint8_t servo);
		void setPosition(uint16_t position);
		uint16_t getPosition();
		void setAngle(double angle);
		uint16_t getAngle();

protected:
		uint8_t num;
		uint16_t min;
		uint16_t max;
		uint16_t position;
};

#endif
