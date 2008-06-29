// $Id$

#ifndef WD2793BASEDFDC_HH
#define WD2793BASEDFDC_HH

#include "MSXFDC.hh"
#include <memory>

namespace openmsx {

class DriveMultiplexer;
class WD2793;

class WD2793BasedFDC : public MSXFDC
{
public:
	virtual void reset(const EmuTime& time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

protected:
	WD2793BasedFDC(MSXMotherBoard& motherBoard, const XMLElement& config);
	virtual ~WD2793BasedFDC();

	const std::auto_ptr<DriveMultiplexer> multiplexer;
	const std::auto_ptr<WD2793> controller;
};

REGISTER_BASE_NAME_HELPER(WD2793BasedFDC, "WD2793BasedFDC");

} // namespace openmsx

#endif
