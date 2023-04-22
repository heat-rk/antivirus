#pragma once

#define CHANNEL_TYPE_EXTERNAL 0
#define CHANNEL_TYPE_INTERNAL 1

#define PIPE_SERVICE_INPUT_PATH L"\\\\.\\pipe\\antivirus-pipe-service-input"
#define PIPE_SERVICE_OUTPUT_PATH L"\\\\.\\pipe\\antivirus-pipe-service-output"
#define PIPE_SERVICE_INPUT_INTERNAL_PATH L"\\\\.\\pipe\\antivirus-pipe-service-internal-input"
#define PIPE_SERVICE_OUTPUT_INTERNAL_PATH L"\\\\.\\pipe\\antivirus-pipe-service-internal-output"
#define PIPE_BUFFSIZE 4096

wchar_t const* const wstrOfChannelType(int type);
