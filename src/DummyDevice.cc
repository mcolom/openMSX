// $Id$

#include <cassert>
#include "DummyDevice.hh"
#include "EmuTime.hh"
#include "xmlx.hh"
#include "FileContext.hh"

namespace openmsx {

DummyDevice::DummyDevice(const XMLElement& config, const EmuTime& time) 
	: MSXDevice(config, time), MSXIODevice(config, time),
	  MSXMemDevice(config, time)
{
}

DummyDevice::~DummyDevice()
{
}

DummyDevice& DummyDevice::instance()
{
	static bool init = false;
	static XMLElement deviceElem("Dummy");
	if (!init) {
		init = true;
		deviceElem.addChild(auto_ptr<XMLElement>(new XMLElement("type", "empty")));
		deviceElem.setFileContext(auto_ptr<FileContext>(new SystemFileContext()));
	}
	static DummyDevice oneInstance(deviceElem, EmuTime::zero);
	return oneInstance;
}


// Block usage of the following methods

void DummyDevice::reset(const EmuTime& time)
{
	assert(false);
}

} // namespace openmsx
