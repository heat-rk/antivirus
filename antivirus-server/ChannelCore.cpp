#include "ChannelCore.h"

wchar_t const* const wstrOfChannelType(int type) {
    switch (type) {
    case CHANNEL_TYPE_EXTERNAL:
        return L"External";
    case CHANNEL_TYPE_INTERNAL:
        return L"Internal";
    default:
        return L"Unknown";
    }
}