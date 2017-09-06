#include "TerroristConfig.h"
#include "../tinyxml/tinyxml.h"

TerroristConfig::TerroristConfig()
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
	}
}