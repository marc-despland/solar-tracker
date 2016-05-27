#include "tracker.h"
#include <unistd.h>
#include "log.h"

Tracker::Tracker() :SearchAlgorithm(){
}

SearchResult * Tracker::search(){
	TrackerSearchResult * result=new TrackerSearchResult();
	uint16_t start=earth->getMin();
	uint16_t end=earth->getMax();
	uint16_t step=(end-start)/100;
	this->earth->setPosition(start);
	usleep(1000000);
	for (uint16_t i=start; i<end; i+=step) {
		this->earth->setPosition(i);
		usleep(200000);
		double left=this->captor->getLeftLux();
		double right=this->captor->getRightLux();
		if (result->maxleft<left) {
			result->maxleft=left;
			result->angleleft=this->earth->getAngle();
		}
		if (result->maxright<right) {
			result->maxright=right;
			result->angleright=this->earth->getAngle();
		}
		if (result->maxlux<(left+right)) {
			result->maxlux=(left+right);
			result->anglelux=this->earth->getAngle();
		}
		Log::logger->log("TRACKER",NOTICE) << "Max lux = " << result->maxlux << " Current = "<< left+right << endl;
		Log::logger->log("TRACKER",NOTICE) << "Max Angle = " << result->anglelux << " Current = "<< this->earth->getAngle() << endl;
	}
	this->earth->setAngle(result->anglelux);
	return((SearchResult * )result);

}
