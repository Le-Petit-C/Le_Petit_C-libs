
namespace LPC {
	namespace priv {
		const MSG* pmsg;
	}
}

inline const MSG* LPC::GetMSG() {
	return priv::pmsg;
}

namespace LPC {
	namespace priv {
		struct LPCgamewndinit_t {
			LPCgamewndinit_t();
			~LPCgamewndinit_t();
		} LPCgamewndinit_t_instance;
	}
}

LPC::priv::LPCgamewndinit_t::LPCgamewndinit_t(){
#ifndef LPCgamewndtitle
	gamewndstartup(event);
#else
	gamewndstartup(event, LPCgamewndtitle);
#endif
}

LPC::priv::LPCgamewndinit_t::~LPCgamewndinit_t() {
	gamewndshutdown();
}

int _cdecl main(int argc, char** argv) {
	init();
	MSG msg{};
	LPC::priv::pmsg = &msg;
	BOOL ret;
	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (ret > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			LPC::dprintf("Unexpected error happend, GetMessage returned %d.", ret);
		}
	}
}
