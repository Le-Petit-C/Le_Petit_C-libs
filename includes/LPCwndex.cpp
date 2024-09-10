
#include "LPCwndex.hpp"

namespace LPC {
	wndbuffer* defaultbuffer;
}

LPC::setDefault::setDefault(window& wnd) : lastbuffer(defaultbuffer), shoulddeletebuffer(true) {
	defaultbuffer = new wndbuffer(wnd);
}

LPC::setDefault::setDefault(window&& wnd) : lastbuffer(defaultbuffer), shoulddeletebuffer(true), shoulddeletewindow(true) {
	defaultbuffer = new wndbuffer(new window(std::move(wnd)));
}

LPC::setDefault::setDefault(wndbuffer& buf) : lastbuffer(defaultbuffer) {
	defaultbuffer = &buf;
}

LPC::setDefault::~setDefault() {
	window* pwnd = defaultbuffer->getwnd();
	if (shoulddeletebuffer) delete defaultbuffer;
	if (shoulddeletewindow) delete pwnd;
	defaultbuffer = lastbuffer;
}