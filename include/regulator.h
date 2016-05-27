#ifndef _REGULATOR_H
#define _REGULATOR_H
#include "config.h"
#include "searchalgorithm.h"


class RegulatorSearchResult : public SearchResult{
public:
	double left=0;
	double right=0;
	uint16_t position=0;
};


class Regulator: public SearchAlgorithm{
public:
	Regulator();
	SearchResult * search();

protected:
};

#endif