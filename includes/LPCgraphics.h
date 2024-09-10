
#include "LPCgraphics.hpp"

namespace LPC {
	namespace priv {
		struct LPCgraphicsstartup_t {
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR gdiplusToken;
			LPCgraphicsstartup_t() {
				Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			}
			~LPCgraphicsstartup_t() {
				Gdiplus::GdiplusShutdown(gdiplusToken);
			}
		} LPCgraphicsstartup_t_instance;
    }
}
