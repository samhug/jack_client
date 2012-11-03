#ifndef SIMPLECLIENT_H
#define SIMPLECLIENT_H

#include "jack_client.h"

class SimpleClient : public JackClient
{
public:
    SimpleClient();

private:
    int process_callback(jack_nframes_t nframes, vector<void*> input_buffers, vector<void*> output_buffers);
};

#endif // SIMPLECLIENT_H
