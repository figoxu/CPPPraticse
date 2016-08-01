#ifndef _SUBJECT_H_
#define _SUBJECT_H_

class Observer;
class Subject{
public:
	virtual ~Subject() = 0;
	virtual void registerObserver(Observer *) = 0;
	virtual void removeObserver(Observer*) = 0;
	virtual void notifyObserver() const = 0;
};

Subject::~Subject()
{
}

#endif
