#include <string.h>

#include "simple_client.h"

SimpleClient::SimpleClient() : JackClient("simple_client")
{

}

int SimpleClient::process_callback(jack_nframes_t nframes, vector<void*> input_buffers, vector<void*> output_buffers)
{
    memcpy(output_buffers[0], input_buffers[0], nframes*sizeof(jack_default_audio_sample_t));
    return 0;
}
