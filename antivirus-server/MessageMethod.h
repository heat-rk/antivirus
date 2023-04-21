#pragma once

namespace Antivirus {
	namespace MessageMethod {
		char const* const E_UNKNOWN = "";
		char const* const E_IS_PROTECTED = "is_protected";
		char const* const E_ENABLE_PROTECTION = "enable_protection";
		char const* const E_DISABLE_PROTECTION = "disable_protection";
		char const* const E_SCAN_START = "scan_start";
		char const* const E_SCAN_PAUSE = "scan_pause";
		char const* const E_SCAN_RESUME = "scan_resume";
		char const* const E_SCAN_STOP = "scan_stop";
	}
}