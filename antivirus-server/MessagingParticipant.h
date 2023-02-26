#pragma once

namespace Antivirus {
	namespace MessagingParticipant {
		char const* const E_UNKNOWN = "";

        // BEGIN OF SERVER PARTICIPANTS BLOCK

        char const* const E_SERVER_STATUS_NOTIFIER = "server_status_notifier";

        // END OF SERVER PARTICIPANTS BLOCK



        // BEGIN OF CLIENT PARTICIPANTS BLOCK

        char const* const E_CLIENT_ERROR_HANDLER = "client_error_handler";
        char const* const E_CLIENT_STATUS_RECEIVER = "client_status_receiver";

        // END OF CLIENT PARTICIPANTS BLOCK
	}
}