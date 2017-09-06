#ifndef _H_AGENT_TERRORIST_H
#define _H_AGENT_TERRORIST_H

#include "STDAgent.h"

class Terrorist : public STDAgent
{
public:
	Terrorist(const std::string& name, const std::string& modelName);
	virtual ~Terrorist();

public:
	virtual void run(const Real& dt);

public:
	void setError(const Vector3& err) { mError=err; }
	const Vector3& getError() const { return mError; }

public:
	Vector3 mError;
};
#endif
