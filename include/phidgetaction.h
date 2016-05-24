#ifndef _PHIDGETACTION_H
#define _PHIDGETACTION_H



class PhidgetAction {
public:
		virtual void inputEvent(int index, int state)=0;
};

#endif