#include "PrinterPortSimpl.hh"
#include "DeviceConfig.hh"
#include "XMLElement.hh"
#include "serialize.hh"
#include <memory>

namespace openmsx {

constexpr static_string_view DESCRIPTION =
	"Play samples via your printer port.";

PrinterPortSimpl::PrinterPortSimpl(const HardwareConfig& hwConf_)
	: hwConf(hwConf_)
{
}

bool PrinterPortSimpl::getStatus(EmuTime::param /*time*/)
{
	return true; // TODO check
}

void PrinterPortSimpl::setStrobe(bool /*strobe*/, EmuTime::param /*time*/)
{
	// ignore strobe // TODO check
}

void PrinterPortSimpl::writeData(byte data, EmuTime::param time)
{
	dac->writeDAC(data, time);
}

void PrinterPortSimpl::createDAC()
{
	static XMLElement* xml = [] {
		auto& doc = XMLDocument::getStaticDocument();
		auto* result = doc.allocateElement("simpl");
		result->setFirstChild(doc.allocateElement("sound"))
		      ->setFirstChild(doc.allocateElement("volume", "12000"));
		return result;
	}();
	dac.emplace("simpl", DESCRIPTION, DeviceConfig(hwConf, *xml));
}

void PrinterPortSimpl::plugHelper(Connector& /*connector*/, EmuTime::param /*time*/)
{
	createDAC();
}

void PrinterPortSimpl::unplugHelper(EmuTime::param /*time*/)
{
	dac.reset();
}

std::string_view PrinterPortSimpl::getName() const
{
	return "simpl";
}

std::string_view PrinterPortSimpl::getDescription() const
{
	return DESCRIPTION;
}

template<Archive Ar>
void PrinterPortSimpl::serialize(Ar& ar, unsigned /*version*/)
{
	if (isPluggedIn()) {
		if constexpr (Ar::IS_LOADER) {
			createDAC();
		}
		ar.serialize("dac", *dac);
	}
}
INSTANTIATE_SERIALIZE_METHODS(PrinterPortSimpl);
REGISTER_POLYMORPHIC_INITIALIZER(Pluggable, PrinterPortSimpl, "PrinterPortSimpl");

} // namespace openmsx
