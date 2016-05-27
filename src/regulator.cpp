#include "regulator.h"

#include "phidget.h"
#include "earth.h"
#include <unistd.h>
#include "log.h"




Regulator::Regulator() :SearchAlgorithm() {
}

SearchResult * Regulator::search() {
	RegulatorSearchResult * result=new RegulatorSearchResult();
	bool reguled=false;
	uint16_t start=this->earth->getMin();
	uint16_t end=this->earth->getMax();
	uint16_t step=(end-start)/100;
	result->position=this->earth->getPosition();
	while (!reguled) {
		result->left=this->captor->getLeftLux();
		result->right=this->captor->getRightLux();
		reguled=(result->left>=result->right*0.95) && (result->left<=result->right*1.05);
		if (!reguled && (result->left>result->right*1.05)) {
			result->position+=step;
			if (result->position>end) {
				reguled=true; 
			} else {
				this->earth->setPosition(result->position);
				usleep(100000);
			}
		}
		if (!reguled && (result->left<result->right*0.95)) {
			result->position-=step;
			if (result->position>end) {
				reguled=true; 
			} else {
				this->earth->setPosition(result->position);
				usleep(100000);
			}
		}
		Log::logger->log("REGULATOR",NOTICE) << "Reguled Left="<<left<<" - Right="<<right  << endl;
	}
	return result;
}