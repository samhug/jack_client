#ifndef JACKCLIENT_H
#define JACKCLIENT_H

#include <string>

#include <jack/jack.h>
#include <jack/midiport.h>

using namespace std;

class JackClient
{
public:
    enum jack_state_t {not_active, active, closed};

private:
    jack_client_t  *client;
    jack_options_t  client_options;
    jack_status_t   client_status;
    jack_state_t    client_state;

public:
    JackClient(string name);
    virtual ~JackClient();

    static int jack_process_callback(jack_nframes_t nframes, void* arg);

    void start();
    void stop();
    void close();

    string get_name();
    jack_nframes_t get_sample_rate();
    jack_nframes_t get_buffer_size();

private:
    int __process_callback(jack_nframes_t nframes);

};


#endif // JACKCLIENT_H

