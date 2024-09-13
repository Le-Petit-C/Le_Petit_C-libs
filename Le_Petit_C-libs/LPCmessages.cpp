
#include "pch.h"

namespace {
	struct eventinfo {
		UINT mapid = 0;
		LPC::EVENTPROC proc = nullptr;
		char* description = nullptr;
		eventinfo() {}
		void swap(eventinfo& info) noexcept {
			std::swap(description, info.description);
			std::swap(mapid, info.mapid);
		}
		eventinfo(eventinfo&& info) noexcept{
			swap(info);
		}
	};
}

static std::mutex msgmtx;
static std::vector<eventinfo> msgvec;

UINT LPC::registerEvent(const char* description, UINT mapid, EVENTPROC proc) {
	eventinfo *info;
	UINT index;
	std::lock_guard<std::mutex> guard(msgmtx);
	index = (UINT)msgvec.size();
	msgvec.push_back(eventinfo());
	info = &msgvec.back();
	info->mapid = mapid;
	info->proc = proc;
	size_t size = strlen(description) + 1;
	info->description = (char*)malloc(size);
	memcpy_s(info->description, size, description, size);
	return index + WM_USER;
}

UINT LPC::getEventMapId(UINT msg) {
	msg -= WM_USER;
	if (msg >= msgvec.size()) return 0;
	return msgvec[msg].mapid;
}

const char* LPC::getEventMapDescription(UINT msg) {
	msg -= WM_USER;
	if (msg >= msgvec.size()) return nullptr;
	return msgvec[msg].description;
}

LRESULT LPC::LPCDispatchMessage(const MSG* lpMsg) {
	UINT i = lpMsg->message - WM_USER;
	if (i < msgvec.size()) {
		LPC::EVENTPROC proc = msgvec[i].proc;
		if(proc != nullptr)
			proc(lpMsg->message, lpMsg->wParam, lpMsg->lParam);
		return 0;
	}
	return DispatchMessage(lpMsg);
}
