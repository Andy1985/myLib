#ifndef MY_XML_H
#define MY_XML_H

#include <string>
using std::string;

#include "tinyxml.h"

#define XML_PARSE_INIT() TiXmlDocument* myDoc = new TiXmlDocument();if (NULL == myDoc) return false;
#define XML_PARSE_ERROR(x) { if (NULL == x) { delete myDoc; return false; } }
#define XML_PARSE_EXIT() { delete myDoc; return true; }
#define XML_PARSE_DECL() TiXmlDeclaration *decl = new TiXmlDeclaration ("1.0","UTF-8","no");
#define XML_PARSE_ADD(x,y) if (false == x->LinkEndChild(y)) { return false; }
#define XML_PARSE_BUILD(x,y) x->LinkEndChild(y);

TiXmlElement* GetElementByTagName(TiXmlElement* root,const string& elementName);
string GetElementValueByTagName(TiXmlElement* root,const string& name);
string GetElementAttributeByTagName(TiXmlElement* root,const string& name,const string& attribute);
string GetElementAttribute(TiXmlElement* element,const string& attribute);
TiXmlElement* CreateElement(const string& name,const string& value);
string Base64GetElementValueByTagName(TiXmlElement* root,const string& name);
TiXmlElement* Base64CreateElement(const string& name,const string& value);
#endif /*MY_XML_H*/
