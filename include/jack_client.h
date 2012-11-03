#ifndef JACKCLIENT_H
#define JACKCLIENT_H

#include <string>
#include <vector>

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

    vector<jack_port_t*> input_ports;
    vector<jack_port_t*> output_ports;

public:
    JackClient(string name);
    virtual ~JackClient();

    static int jack_process_callback(jack_nframes_t nframes, void* arg);

    void start();
    void stop();
    void close();

    size_t add_audio_in_port(string name);
    size_t add_audio_out_port(string name);
    size_t add_midi_in_port(string name);
    size_t add_midi_out_port(string name);

    string get_name();
    jack_state_t get_state();

    jack_nframes_t get_sample_rate();
    jack_nframes_t get_buffer_size();

private:
    int __process_callback(jack_nframes_t nframes);

    jack_port_t* add_port(string name, const char* type, unsigned long flags);

    size_t add_in_port(string name, const char* type);
    size_t add_out_port(string name, const char* type);

protected:
    virtual int process_callback(jack_nframes_t nframes, vector<void*> input_buffers, vector<void*> output_buffers) = 0;

};


#endif // JACKCLIENT_H

