#include <iostream>
#include <unistd.h>

#include "simple_client.h"

using namespace std;

int main(int argc, char *argv[])
{
    cout << "JackClient example simple_client" << endl;

    SimpleClient client;

    client.add_audio_in_port("input");
    client.add_audio_out_port("output");

    client.start();

    sleep(-1);

    client.close();

    return 0;
}
