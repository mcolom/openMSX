// $Id$

#ifndef __YM2413CORE_HH__
#define __YM2413CORE_HH__

#include "openmsx.hh"

namespace openmsx {

class EmuTime;

class YM2413Core
{
	public:
		virtual void reset(const EmuTime &time) = 0;
		virtual void writeReg(byte reg, byte value, const EmuTime &time) = 0;
};

} // namespace openmsx

#endif
