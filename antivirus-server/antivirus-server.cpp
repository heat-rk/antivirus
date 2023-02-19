#include "channel.h"
#include "scanner.h"
#include <stdio.h>

using namespace Antivirus;

void handleClientMessage(Channel* channel, byte* message) {
    printf("%s", message);
    channel->write((unsigned char*) "Hello, Kotlin!\n");

}

int main() {
    Channel* channel = (Channel*) malloc(sizeof(Channel));
    Scanner* scanner = (Scanner*) malloc(sizeof(Scanner));
    
    channel->connect(handleClientMessage);
    scanner->start();

    free(channel);
    free(scanner);

    return 0;
}
