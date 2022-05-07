#include "Schedulable.hh"
#include "Scheduler.hh"
#include <iostream>

namespace openmsx {

Schedulable::Schedulable(Scheduler& scheduler_)
	: scheduler(scheduler_)
{
}

Schedulable::~Schedulable()
{
	removeSyncPoints();
}

void Schedulable::schedulerDeleted()
{
	std::cerr << "Internal error: Schedulable \"" << typeid(*this).name()
	          << "\" failed to unregister.\n";
}

void Schedulable::setSyncPoint(EmuTime::param timestamp)
{
	scheduler.setSyncPoint(timestamp, *this);
}

bool Schedulable::removeSyncPoint()
{
	return scheduler.removeSyncPoint(*this);
}

void Schedulable::removeSyncPoints()
{
	scheduler.removeSyncPoints(*this);
}

bool Schedulable::pendingSyncPoint() const
{
	auto dummy = EmuTime::dummy();
	return scheduler.pendingSyncPoint(*this, dummy);
}

bool Schedulable::pendingSyncPoint(EmuTime& result) const
{
	return scheduler.pendingSyncPoint(*this, result);
}

EmuTime::param Schedulable::getCurrentTime() const
{
	return scheduler.getCurrentTime();
}

template<Archive Ar>
void Schedulable::serialize(Ar& ar, unsigned /*version*/)
{
	Scheduler::SyncPoints syncPoints;
	if constexpr (!Ar::IS_LOADER) {
		syncPoints = scheduler.getSyncPoints(*this);
	}
	ar.serialize("syncPoints", syncPoints);
	if constexpr (Ar::IS_LOADER) {
		removeSyncPoints();
		for (auto& s : syncPoints) {
			setSyncPoint(s.getTime());
		}
	}
}
INSTANTIATE_SERIALIZE_METHODS(Schedulable);

} // namespace openmsx
