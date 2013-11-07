#include "xml.h"
#include "base64.h"

TiXmlElement* GetElementByTagName(TiXmlElement* root,const string& elementName)
{
	TiXmlElement* ret = NULL;
	
	if (root == NULL)
	{
		return NULL;
	}

	if (root != NULL && elementName == root->Value())
	{
		ret = root;
	}

	if (ret == NULL && root->FirstChildElement() != NULL)
	{
		ret = GetElementByTagName(root->FirstChildElement(),elementName);
	}

	if (ret == NULL && root->NextSiblingElement() != NULL)
	{
		ret = GetElementByTagName(root->NextSiblingElement(),elementName);
	}

	return ret;
}

string GetElementValueByTagName(TiXmlElement* root,const string& name)
{
	if (root == NULL) return string("");

	TiXmlElement* toFind = GetElementByTagName(root,name);
	if (toFind != NULL)
		return string(toFind->GetText() ? toFind->GetText() : "");
	else
		return string("");
}

string GetElementAttributeByTagName(TiXmlElement* root,const string& name,
		const string& attribute)
{
	if (root == NULL) return string("");

	string strValue("");

	TiXmlElement* toFind = GetElementByTagName(root,name);

	if (toFind != NULL)
	{
		toFind->QueryValueAttribute(attribute,&strValue);
	}

	return strValue;
}

string GetElementAttribute(TiXmlElement* element,const string& attribute)
{
	if (element == NULL) return string("");

	string strValue("");
	element->QueryValueAttribute(attribute,&strValue);
	return strValue;
}

TiXmlElement* CreateElement(const string& name,const string& value)
{
	TiXmlElement* toCreate = new TiXmlElement(name.c_str());	
	if (NULL != toCreate && value != "")
	{
		TiXmlText* Text = new TiXmlText(value.c_str());
		if (NULL != Text)
			XML_PARSE_BUILD(toCreate,Text);
	}
	
	return toCreate;
}

string Base64GetElementValueByTagName(TiXmlElement* root,const string& name)
{
	TiXmlElement* e = GetElementByTagName(root,name);
	if (e == NULL) return string("");
	
	if (GetElementAttribute(e,"encode") == "base64")
		return base64_decode(string(e->GetText() ? e->GetText() : ""));
	else
		return string(e->GetText() ? e->GetText() : "");
}

TiXmlElement* Base64CreateElement(const string& name,const string& value)
{
	TiXmlElement* e = NULL;

	if (value != "")
	{
		e = CreateElement(name,base64_encode((const unsigned char*)value.c_str(),value.size()));
		if (e != NULL) e->SetAttribute("encode","base64");
	}
	else 
	{
		e = CreateElement(name,value);
	}

	return e;
}
