#ifndef _TRACKER_H
#define _TRACKER_H
#include "config.h"


#include "config.h"
#include "searchalgorithm.h"


class TrackerSearchResult : public SearchResult{
public:
	double angleleft=-1;
	double angleright=-1;
	double maxleft=-1;
	double maxright=-1;
	double maxlux=-1;
	double anglelux=-1;
};

class Tracker : public SearchAlgorithm {
public:
	Tracker();
	SearchResult * search();

protected:

};

#endif