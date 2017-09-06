#include "URAVConfig.h"
#include "../tinyxml/tinyxml.h"

URAVConfig::URAVConfig()
{
	mapResolution=256;
	mapTitle="UAV View";
	scale=0.3f;
	mapWidth=0.20f;
	mapAlpha=1.0f;
	meshName="razor.mesh";
	frontalCamX=0;
	frontalCamY=-20;
	frontalCamZ=30;
	backCamX=0;
	backCamY=40;
	backCamZ=160;
	posX=0;
	posY=120;
	posZ=0;
	speed=30;
	rotatingSpeed=10;
	camScanningSpeed=5;
	hoveringSpeed=10;
	life=1000;
	collisionRadius=2;
	explosionPSName="XS/Explosion";
	explosionDuration=1;
}

void URAVConfig::load(const std::string& filename)
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
			else if(param_name.compare("speed")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &speed);
			}
			else if(param_name.compare("rotatingSpeed")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &rotatingSpeed);
			}
			else if(param_name.compare("camScanningSpeed")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &camScanningSpeed);
			}
			else if(param_name.compare("hoveringSpeed")==0)
			{
				xmlLevel1->QueryFloatAttribute("value", &hoveringSpeed);
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
		else if(strcmp(xmlLevel1->Value(), "frontalCam")==0)
		{
			xmlLevel1->QueryFloatAttribute("x", &frontalCamX);
			xmlLevel1->QueryFloatAttribute("y", &frontalCamY);
			xmlLevel1->QueryFloatAttribute("z", &frontalCamZ);
		}
		else if(strcmp(xmlLevel1->Value(), "backCam")==0)
		{
			xmlLevel1->QueryFloatAttribute("x", &backCamX);
			xmlLevel1->QueryFloatAttribute("y", &backCamY);
			xmlLevel1->QueryFloatAttribute("z", &backCamZ);
		}
		else if(strcmp(xmlLevel1->Value(), "position")==0)
		{
			xmlLevel1->QueryFloatAttribute("x", &posX);
			xmlLevel1->QueryFloatAttribute("z", &posZ);
			xmlLevel1->QueryFloatAttribute("y", &posY);
		}
		else if(strcmp(xmlLevel1->Value(), "explosion")==0)
		{
			xmlLevel1->QueryFloatAttribute("duration", &explosionDuration);
			explosionPSName=xmlLevel1->Attribute("particle_system");
		}
	}
}