#pragma once

namespace Antivirus {
	namespace MessageMethod {
		char const* const E_UNKNOWN = "";
        char const* const E_GET_STATUS = "get_status";
		char const* const E_SCAN_LAST = "scan_last";
		char const* const E_SCAN_START = "scan_start";
		char const* const E_SCAN_PAUSE = "scan_pause";
		char const* const E_SCAN_STOP = "scan_stop";
	}
}