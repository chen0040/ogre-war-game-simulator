#include "stdafx.h"
#include "TerroristConfig.h"
#include "../tinyxml/tinyxml.h"
#include "../STDAgent/STDAgentManager.h"
#include <sstream>

TerroristConfig::TerroristConfig()
: mTerroristCreationCounter(0)
{
	scale=0.2f;
	life=1000;
	meshName="robot.mesh";
	elevation=10;
	depth=5;
	width=10;
	explosionPSName="XS/Explosion";
	explosionDuration=1;
}

TerroristConfig::~TerroristConfig()
{
	std::map<std::string, Terrorist*>::iterator biter=mTerrorists.begin();
	std::map<std::string, Terrorist*>::iterator itEnd=mTerrorists.end();
	Terrorist* agent;
	for(; biter != itEnd; ++biter)
	{
		agent=biter->second;
		STDAgentManager::getSingletonPtr()->removeAgent(agent->getName());
	}
	mTerrorists.clear();
}

std::pair<Terrorist*, Terrorist*> TerroristConfig::getBestTerroristPair()
{
	std::map<std::string, Terrorist*>::iterator biter=mTerrorists.begin();
	std::map<std::string, Terrorist*>::iterator itEnd=mTerrorists.end();
	Terrorist* agent;
	Terrorist* bagent=NULL;
	Terrorist* bagent2=NULL;
	float bestScore=0;
	for(; biter != itEnd; ++biter)
	{
		agent=biter->second;
		float score=agent->getScore();
		if(score > bestScore)
		{
			bestScore=score;
			bagent2=bagent;
			bagent=agent;
		}
	}
	return std::make_pair<Terrorist*, Terrorist*>(bagent, bagent2);
}

Terrorist* TerroristConfig::getBestTerrorist()
{
	std::map<std::string, Terrorist*>::iterator biter=mTerrorists.begin();
	std::map<std::string, Terrorist*>::iterator itEnd=mTerrorists.end();
	Terrorist* agent;
	Terrorist* bagent=NULL;
	float bestScore=0;
	for(; biter != itEnd; ++biter)
	{
		agent=biter->second;
		float score=agent->getScore();
		if(score > bestScore)
		{
			bestScore=score;
			bagent=agent;
		}
	}
	return bagent;
}

void TerroristConfig::save(const std::string& filename)
{
	TiXmlDocument doc;
	if(!doc.LoadFile(filename.c_str()))
	{
		throw Exception(42, "Failed to load URAV.xml", "STDNaviFrame::createURAV()");
	}

	TiXmlElement* xmlRoot=doc.RootElement();
	for(TiXmlElement* xmlLevel1 = xmlRoot->FirstChildElement(); xmlLevel1 != NULL; xmlLevel1 = xmlLevel1->NextSiblingElement())
	{
		if(strcmp(xmlLevel1->Value(), "locations")==0)
		{
			xmlLevel1->Clear();
			std::vector<Vector3>::const_iterator piter=mLocations.begin();
			std::vector<Vector3>::const_iterator piter_end=mLocations.end();

			for(; piter != piter_end; ++piter)
			{
				TiXmlElement* xmlLevel2=new TiXmlElement("location");
				xmlLevel2->SetDoubleAttribute("x", (*piter).x);
				xmlLevel2->SetDoubleAttribute("y", (*piter).y);
				xmlLevel2->SetDoubleAttribute("z", (*piter).z);
				xmlLevel1->LinkEndChild(xmlLevel2);
			}
		}
	}

	doc.SaveFile(filename.c_str());
}

void TerroristConfig::load(const std::string &filename)
{
	TiXmlDocument doc;

	if(!doc.LoadFile(filename.c_str()))
	{
		throw Exception(42, "Failed to load URAV.xml", "STDNaviFrame::createURAV()");
	}

	TiXmlElement* xmlRoot=doc.RootElement();
	for(TiXmlElement* xmlLevel1 = xmlRoot->FirstChildElement(); xmlLevel1 != NULL; xmlLevel1 = xmlLevel1->NextSiblingElement())
	{
		if(strcmp(xmlLevel1->Value(), "param")==0)
		{
			std::string param_name=xmlLevel1->Attribute("name");
			if(param_name.compare("scale")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &scale);
			}
			else if(param_name.compare("mesh")==0)
			{
				meshName=xmlLevel1->Attribute("value");
			}
			else if(param_name.compare("life")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &life);
			}
			else if(param_name.compare("elevation")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &elevation);
			}
			else if(param_name.compare("animation")==0)
			{
				animation=xmlLevel1->Attribute("value");
			}
			else if(param_name.compare("depth")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &depth);
			}
			else if(param_name.compare("width")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &width);
			}
		}
		else if(strcmp(xmlLevel1->Value(), "explosion")==0)
		{
			xmlLevel1->QueryFloatAttribute("duration", &explosionDuration);
			explosionPSName=xmlLevel1->Attribute("particle_system");
		}
		else if(strcmp(xmlLevel1->Value(), "locations")==0)
		{
			float x, y, z;
			for(TiXmlElement* xmlLevel2=xmlLevel1->FirstChildElement(); xmlLevel2 != NULL; xmlLevel2 = xmlLevel2->NextSiblingElement())
			{
				if(strcmp(xmlLevel2->Value(), "location")==0)
				{
					xmlLevel2->QueryFloatAttribute("x", &x);
					xmlLevel2->QueryFloatAttribute("y", &y);
					xmlLevel2->QueryFloatAttribute("z", &z);
					mLocations.push_back(Vector3(x, y, z));
				}
			}
		}
	}
}

void TerroristConfig::updateTerrorists(const Ogre::Real &dt)
{
	std::map<std::string, Terrorist*>::iterator titer=mTerrorists.begin();
	while(titer != mTerrorists.end())
	{
		titer->second->run(dt);
		if(titer->second->getLife() <= 0)
		{
			STDAgentManager::getSingletonPtr()->removeAgent(titer->second->getName());
			titer=mTerrorists.erase(titer);
		}
		else
		{
			++titer;
		}
	}
}

void TerroristConfig::createTerrorists()
{
	std::vector<Vector3>::const_iterator liter=mLocations.begin();
	std::vector<Vector3>::const_iterator liter_end=mLocations.end();
	for(; liter != liter_end; ++liter)
	{
		createTerrorist(*liter);
	}
}

Terrorist* TerroristConfig::createTerrorist()
{
	std::set<STDAgent*> agnts;
	STDAgentManager* sam=STDAgentManager::getSingletonPtr();
	std::vector<Vector3>::const_iterator liter=mLocations.begin();
	std::vector<Vector3>::const_iterator liter_end=mLocations.end();
	for(; liter != liter_end; ++liter)
	{
		Vector3 pos=*liter;
		sam->getAgentsWithinRadius(pos, 10, agnts);
		if(agnts.empty())
		{
			return createTerrorist(pos);
		}
	}
	return NULL;
}

Terrorist* TerroristConfig::createTerrorist(const Vector3& pos)
{
	size_t terrorId=(++mTerroristCreationCounter);
	std::ostringstream oss;
	oss << "Terrorist" << terrorId;
	Terrorist* terrorist=STDAgentManager::getSingletonPtr()->createTerrorist(oss.str(), meshName);
	terrorist->setScale(scale, scale, scale);
	terrorist->getEntity()->setRenderQueueGroup(RENDER_QUEUE_1);
	
	terrorist->setLife(life);

	terrorist->setPosition(pos);
	this->mLocations.push_back(pos);

	if(animation.compare("") != 0)
	{
		terrorist->setAnimationState(animation);
	}

	terrorist->setExplosionDuration(explosionDuration);
	terrorist->setExplosionParticleSystem(explosionPSName);
	Gun* gun=terrorist->createGun();
	gun->setMaxBulletCount(1000);
	gun->setHitRange(50);
	mTerrorists[oss.str()]=terrorist;

	Terrorist* bagent=this->getBestTerrorist();
	if(bagent != NULL)
	{
		terrorist->setError(bagent->getError() + Vector3(Math::RangeRandom(0, 20), Math::RangeRandom(0, 20), Math::RangeRandom(0, 20)));
	}

	std::vector<STDAgent*>::iterator eiter=mEnemies.begin();
	std::vector<STDAgent*>::iterator eiter_end=mEnemies.end();
	for(; eiter != eiter_end; ++eiter)
	{
		terrorist->addEnemy(*eiter);
	}

	return terrorist;
}

Terrorist* TerroristConfig::createTerrorist(const Vector3& rayBegin, const Vector3& rayEnd)
{
	Vector3 rayDirection=rayEnd - rayBegin;
	Real length=rayDirection.normalise();
	Vector3 pos=rayBegin+ rayDirection * (length+ depth);
	pos.y-=elevation;
	return this->createTerrorist(pos);
}

Terrorist* TerroristConfig::getNearbyTerrorist(const Vector3& pt, const Real& radius)
{
	Terrorist* targetedTerrorist=NULL;
	Real sqR=radius * radius;
	std::map<std::string, Terrorist*>::iterator itTerror=mTerrorists.begin();
	std::map<std::string, Terrorist*>::iterator itTerrorEnd=mTerrorists.end();
	for(; itTerror !=itTerrorEnd ; ++itTerror)
	{
		Vector3 disp=itTerror->second->getCenter() - pt;
		if(disp.squaredLength() <= sqR)
		{
			targetedTerrorist=itTerror->second;
			break;
		}
	}

	return targetedTerrorist;
}

Terrorist* TerroristConfig::getNearbyTerrorist(const Vector3& pt)
{
	return getNearbyTerrorist(pt, this->width);
}