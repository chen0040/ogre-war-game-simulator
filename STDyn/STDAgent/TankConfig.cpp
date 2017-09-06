#include "TankConfig.h"
#include "../tinyxml/tinyxml.h"

TankConfig::TankConfig()
{
	mapResolution=256;
	mapTitle="Tank View";
	scale=1.5f;
	elevation=22.0f;
	mapWidth=0.20f;
	mapAlpha=1.0f;
	meshName="G6.mesh";
	explosionPSName="XS/Explosion";
	explosionDuration=1;
	backCamX=0;
	backCamY=40;
	backCamZ=160;
	cannonCamX=0;
	cannonCamY=40;
	cannonCamZ=-20;
	tankPosX=0;
	tankPosZ=0;
	missileSpeed=300;
	firingInterval=0.3f;
	tankSpeed=30;
	tankRotatingSpeed=10;
	tankCamScanningSpeed=5;
	hitRange=10;
	damageRadius=10;
	life=1000;
	missileImpact=10;
	collisionRadius=2;
	laserConnectLimit=10;
}

void TankConfig::load(const std::string& filename)
{
	TiXmlDocument doc;

	if(!doc.LoadFile(filename.c_str()))
	{
		std::ostringstream oss;
		oss << "Failed to load " << filename;
		throw Exception(42, oss.str().c_str(), "STDNaviFrame::createTank()");
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
			else if(param_name.compare("elevation")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &elevation);
			}
			else if(param_name.compare("mesh")==0)
			{
				meshName=xmlLevel1->Attribute("value");
			}
			else if(param_name.compare("speed")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &tankSpeed);
			}
			else if(param_name.compare("rotatingSpeed")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &tankRotatingSpeed);
			}
			else if(param_name.compare("camScanningSpeed")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &tankCamScanningSpeed);
			}
			else if(param_name.compare("LaserLockThreshold")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &laserConnectLimit);
			}
			else if(param_name.compare("life")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &life);
			}
			else if(param_name.compare("collisionRadius")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &collisionRadius);
			}
		}
		else if(strcmp(xmlLevel1->Value(), "map")==0)
		{
			xmlLevel1->QueryFloatAttribute("width", &mapWidth);
			xmlLevel1->QueryFloatAttribute("alpha", &mapAlpha);
			xmlLevel1->QueryIntAttribute("resolution", &mapResolution);
			mapTitle=xmlLevel1->Attribute("title");
		}
		else if(strcmp(xmlLevel1->Value(), "backCam")==0)
		{
			xmlLevel1->QueryFloatAttribute("x", &backCamX);
			xmlLevel1->QueryFloatAttribute("y", &backCamY);
			xmlLevel1->QueryFloatAttribute("z", &backCamZ);
		}
		else if(strcmp(xmlLevel1->Value(), "cannonCam")==0)
		{
			xmlLevel1->QueryFloatAttribute("x", &cannonCamX);
			xmlLevel1->QueryFloatAttribute("y", &cannonCamY);
			xmlLevel1->QueryFloatAttribute("z", &cannonCamZ);
		}
		else if(strcmp(xmlLevel1->Value(), "position")==0)
		{
			xmlLevel1->QueryFloatAttribute("x", &tankPosX);
			xmlLevel1->QueryFloatAttribute("z", &tankPosZ);
		}
		else if(strcmp(xmlLevel1->Value(), "gun")==0)
		{
			xmlLevel1->QueryFloatAttribute("missile_speed", &missileSpeed);
			xmlLevel1->QueryFloatAttribute("firing_interval", &firingInterval);
			xmlLevel1->QueryFloatAttribute("hit_range", &hitRange);
			xmlLevel1->QueryFloatAttribute("damage_radius", &damageRadius);
			xmlLevel1->QueryFloatAttribute("impact", &missileImpact);
		}
		else if(strcmp(xmlLevel1->Value(), "explosion")==0)
		{
			xmlLevel1->QueryFloatAttribute("duration", &explosionDuration);
			explosionPSName=xmlLevel1->Attribute("particle_system");
		}
	}
}