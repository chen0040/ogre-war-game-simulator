#include "stdafx.h"
#include "Terrorist.h"
#include "STDAgentManager.h"

Terrorist::Terrorist(const std::string& name, const std::string& modelName)
: STDAgent(name, modelName)
, mError(50, 50, 50)
{
	
}

Terrorist::~Terrorist()
{

}

void Terrorist::run(const Real& dt)
{
	STDAgent::run(dt);

	STDAgent* enemy=getEnemy();
	if(enemy != NULL)
	{
		this->setAnimationState("Shoot");

		const Vector3& posTarget=enemy->getPosition();
		const Vector3& pos=this->getPosition();
		Vector3 direction=posTarget-pos;
		direction.y=0;
		direction.normalise();
		Vector3 src=this->mNode->getOrientation() * this->mAlignment;
		if(1.0f + src.dotProduct(direction) < 0.0001f)
		{
			this->mNode->yaw(Degree(180));
		}
		else
		{
			this->mNode->rotate(src.getRotationTo(direction));
		}

		Vector3 err(Math::RangeRandom(-mError.x, mError.x), Math::RangeRandom(-mError.y, mError.y), Math::RangeRandom(-mError.z, mError.z));

		if(mGun != NULL)
		{
			this->mGun->shoot(this->getCenter(), enemy->getCenter()+err, dt);
		}
	}
	else
	{
		this->setAnimationState("Idle");
	}
	
}
