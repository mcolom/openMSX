// $Id$

#ifndef __ROMASCII16KB_HH__
#define __ROMASCII16KB_HH__

#include "Rom16kBBlocks.hh"

namespace openmsx {

class RomAscii16kB : public Rom16kBBlocks
{
public:
	RomAscii16kB(const XMLElement& config, const EmuTime& time,
	             auto_ptr<Rom> rom);
	virtual ~RomAscii16kB();
	
	virtual void reset(const EmuTime& time);
	virtual void writeMem(word address, byte value, const EmuTime& time);
	virtual byte* getWriteCacheLine(word address) const;
};

} // namespace openmsx

#endif
