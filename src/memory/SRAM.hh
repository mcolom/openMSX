// $Id$

#ifndef __SRAM_HH__
#define __SRAM_HH__

#include "Ram.hh"

namespace openmsx {

class XMLElement;

class SRAM : public Ram
{
public:
	SRAM(const string& name, int size,
	     const XMLElement& config, const char* header = NULL);
	SRAM(const string& name, const string& description, int size,
	     const XMLElement& config, const char* header = NULL);
	virtual ~SRAM();

private:
	void init();
	
	const XMLElement& config;
	const char* header;
};

} // namespace openmsx

#endif
