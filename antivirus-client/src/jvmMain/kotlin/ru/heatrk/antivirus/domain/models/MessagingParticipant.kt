package ru.heatrk.antivirus.domain.models

enum class MessagingParticipant(val id: String) {
    UNKNOWN(""),

    // BEGIN OF SERVER PARTICIPANTS BLOCK

    SERVER_STATUS_NOTIFIER("server_status_notifier"),

    // END OF SERVER PARTICIPANTS BLOCK



    // BEGIN OF CLIENT PARTICIPANTS BLOCK

    CLIENT_ERROR_HANDLER("client_error_handler"),
    CLIENT_STATUS_RECEIVER("client_status_receiver"),

    // END OF CLIENT PARTICIPANTS BLOCK
}