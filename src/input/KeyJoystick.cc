// $Id$

#include "KeyJoystick.hh"
#include "UserInputEventDistributor.hh"
#include "KeyCodeSetting.hh"
#include "SettingsConfig.hh"
#include "CliComm.hh"
#include "InputEvents.hh"
#include <cassert>
#include <iostream>

using std::string;

namespace openmsx {

KeyJoystick::KeyJoystick(const string& name_)
	: name(name_)
{
	UserInputEventDistributor::instance().registerEventListener(
		UserInputEventDistributor::MSX, *this);

	status = JOY_UP | JOY_DOWN | JOY_LEFT | JOY_RIGHT |
	         JOY_BUTTONA | JOY_BUTTONB;

	up.reset   (new KeyCodeSetting(name + ".up",
		"key for direction up",    Keys::K_UP));
	down.reset (new KeyCodeSetting(name + ".down",
		"key for direction down",  Keys::K_DOWN));
	left.reset (new KeyCodeSetting(name + ".left",
		"key for direction left",  Keys::K_LEFT));
	right.reset(new KeyCodeSetting(name + ".right",
		"key for direction right", Keys::K_RIGHT));
	trigA.reset(new KeyCodeSetting(name + ".triga",
		"key for trigger A",       Keys::K_SPACE));
	trigB.reset(new KeyCodeSetting(name + ".trigb",
		"key for trigger B",       Keys::K_M));
}

KeyJoystick::~KeyJoystick()
{
	UserInputEventDistributor::instance().unregisterEventListener(
		UserInputEventDistributor::MSX, *this);
}


// Pluggable
const string& KeyJoystick::getName() const
{
	return name;
}

const string& KeyJoystick::getDescription() const
{
	static const string desc(
		"Key-Joystick, use your keyboard to emulate an MSX joystick. "
		"See manual for information on how to configure this.");
	return desc;
}

void KeyJoystick::allUp()
{
	status = JOY_UP | JOY_DOWN | JOY_LEFT | JOY_RIGHT |
	         JOY_BUTTONA | JOY_BUTTONB;
}

void KeyJoystick::plugHelper(Connector* /*connector*/, const EmuTime& /*time*/)
{
}

void KeyJoystick::unplugHelper(const EmuTime& /*time*/)
{
}


// KeyJoystickDevice
byte KeyJoystick::read(const EmuTime& /*time*/)
{
	return status;
}

void KeyJoystick::write(byte /*value*/, const EmuTime& /*time*/)
{
	//do nothing
}


// EventListener
bool KeyJoystick::signalEvent(const UserInputEvent& event)
{
	switch (event.getType()) {
	case OPENMSX_CONSOLE_ON_EVENT:
		allUp();
		break;
	default: // must be keyEvent
		assert(dynamic_cast<const KeyEvent*>(&event));
		Keys::KeyCode key = (Keys::KeyCode)((int)((KeyEvent&)event).getKeyCode() &
		                                    (int)Keys::K_MASK);
		switch (event.getType()) {
		case OPENMSX_KEY_DOWN_EVENT:
			if      (key == up->getValue())    status &= ~JOY_UP;
			else if (key == down->getValue())  status &= ~JOY_DOWN;
			else if (key == left->getValue())  status &= ~JOY_LEFT;
			else if (key == right->getValue()) status &= ~JOY_RIGHT;
			else if (key == trigA->getValue()) status &= ~JOY_BUTTONA;
			else if (key == trigB->getValue()) status &= ~JOY_BUTTONB;
			break;
		case OPENMSX_KEY_UP_EVENT:
			if      (key == up->getValue())    status |= JOY_UP;
			else if (key == down->getValue())  status |= JOY_DOWN;
			else if (key == left->getValue())  status |= JOY_LEFT;
			else if (key == right->getValue()) status |= JOY_RIGHT;
			else if (key == trigA->getValue()) status |= JOY_BUTTONA;
			else if (key == trigB->getValue()) status |= JOY_BUTTONB;
			break;
		default:
			assert(false);
		}
	}
	return true;
}

} // namespace openmsx
