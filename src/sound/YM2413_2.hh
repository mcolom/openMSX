// $Id$

#ifndef YM2413_2_HH
#define YM2413_2_HH

#include "YM2413Interface.hh"
#include "serialize_meta.hh"
#include <string>
#include <memory>

namespace openmsx {

class MSXMotherBoard;
class XMLElement;
class Global; // Defined in .cc:

class YM2413_2 : public YM2413Interface
{
public:
	YM2413_2(MSXMotherBoard& motherBoard, const std::string& name,
	         const XMLElement& config, const EmuTime& time);
	virtual ~YM2413_2();

	virtual void reset(const EmuTime& time);
	virtual void writeReg(byte r, byte v, const EmuTime& time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	const std::auto_ptr<Global> global;
};
REGISTER_POLYMORPHIC_INITIALIZER(YM2413Interface, YM2413_2, "YM2413-Jarek-Burczynski");

} // namespace openmsx

#endif
