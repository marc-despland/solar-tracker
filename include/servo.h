#ifndef _SERVO_H
#define _SERVO_H

#include <stdint.h>
#include "maestro.h"
#include <map>

class Servo {
public:
		Servo(uint8_t servo, std::string name);
		void setPosition(uint16_t position);
		uint16_t getPosition();
		void setAngle(double angle);
		double getAngle();
		static std::map<int , Servo *>	servos;
		std::string getName();
		uint16_t getMin();
		uint16_t getMax();


protected:
		uint8_t num;
		uint16_t min;
		uint16_t max;
		uint16_t position;
		std::string name;
		
};

#endif
