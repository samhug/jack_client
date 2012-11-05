#include <iostream>
#include <stdexcept>
#include <cassert>

#include "jack_client.h"

//using namespace std;

/**
 * Initializes the JACK client and registers callbacks
 *
 * @param[in]   name    The name to register the JACK client under
 */
JackClient::JackClient(string name)
{
    client_options = JackNullOption;
    client_state = not_active;

    // Initialize the JACK client
    if ((client = jack_client_open(name.c_str(), client_options, &client_status)) == NULL) {
        throw runtime_error("Unable to initialize the JACK client. Is the server running?");
    }
    if (client_status & JackServerStarted) {
        cerr << "JACK server started" << endl;
    }
    if (client_status & JackNameNotUnique) {
        cerr << "NOTE: Unique name `" << get_name() << "` assigned by JACK" << endl;
    }


    // Register the callback function with JACK
    if (jack_set_process_callback(client, JackClient::jack_process_callback, this) != 0) {
        throw runtime_error("Unable to register process callback");
    }
}

/**
 * Closes the JACK client if needed
 */
JackClient::~JackClient()
{
    // If the JACK client hasn't been closed yet then close it
    if (client_state != closed) {
        close();
    }
}

/**
 * This callback function is called directly by JACK. It calls the
 * __process_callback function transfering context back to the JackClient
 * object.
 *
 * @param[in]   nframes The number of frames to process
 * @param[in]   arg     Pointer to callback meta data
 * @return 0 on success
 */
int JackClient::jack_process_callback(jack_nframes_t nframes, void *arg)
{
    JackClient *client = static_cast<JackClient*>(arg);

    // Do callback handling in the context of the client object
    return client->__process_callback(nframes);
}

int JackClient::__process_callback(jack_nframes_t nframes)
{
    map<string,jack_port_t*>::iterator it;

    vector<void*> input_buffers;
    vector<void*> output_buffers;

    for ( it=input_ports.begin(); it != input_ports.end(); it++ )
        input_buffers.push_back(jack_port_get_buffer((*it).second, nframes));

    for ( it=output_ports.begin(); it != output_ports.end(); it++ )
        output_buffers.push_back(jack_port_get_buffer((*it).second, nframes));

    return process_callback(nframes, input_buffers, output_buffers);
}

/**
 * Activates the JACK client
 */
void JackClient::start()
{
    assert(client_state == not_active);

    if (jack_activate(client) != 0) {
        throw runtime_error("Unable to activate JACK client");
    }
 
    client_state = active;
}

/**
 * Deactivates the JACK client
 */
void JackClient::stop()
{
    assert(client_state == active);

    if (jack_deactivate(client) != 0) {
        throw runtime_error("Unable to deactivate JACK client");
    }

    client_state = not_active;
}

/**
 * Closes the JACK client
 */
void JackClient::close()
{
    assert(client_state != closed);

    // If the client is running stop it
    if (client_state == active) {
        stop();
    }

    // Unregister all of the ports
    map<string,jack_port_t*>::iterator it;
    for ( it=input_ports.begin() ; it != input_ports.end(); it++ )
        remove_in_port((*it).first);

    for ( it=output_ports.begin() ; it != output_ports.end(); it++ )
        remove_out_port((*it).first);

    // Close the JACK client
    if (jack_client_close(client) != 0) {
        throw runtime_error("Unable to close JACK client");
    }

    client_state = closed;
}


void JackClient::add_audio_in_port(string name)
{
    add_in_port(name, JACK_DEFAULT_AUDIO_TYPE);
}
void JackClient::add_audio_out_port(string name)
{
    add_out_port(name, JACK_DEFAULT_AUDIO_TYPE);
}
void JackClient::add_midi_in_port(string name)
{
    add_in_port(name, JACK_DEFAULT_MIDI_TYPE);
}
void JackClient::add_midi_out_port(string name)
{
    add_out_port(name, JACK_DEFAULT_MIDI_TYPE);
}

void JackClient::add_in_port(string name, const char* type)
{
    // Make sure an input port dosn't already exist with that name
    if (input_ports.count(name) > 0) {
        throw runtime_error("An input port already exists with that name");
    }

    jack_port_t* in_port = add_port(name.c_str(), type, JackPortIsInput);
    input_ports[name] = in_port;
}
void JackClient::add_out_port(string name, const char* type)
{
    // Make sure an output port dosn't already exist with that name
    if (output_ports.count(name) > 0) {
        throw runtime_error("An output port already exists with that name");
    }

    jack_port_t* out_port = add_port(name.c_str(), type, JackPortIsOutput);
    output_ports[name] = out_port;
}

jack_port_t* JackClient::add_port(string name, const char* type, unsigned long flags)
{
    if (client_state == active) {
        throw runtime_error("You cannot add ports while the client is active.");
    }
    if (client_state == closed) {
        throw runtime_error("You cannot add ports while the client is closed.");
    }

    jack_port_t* port;
    if((port = jack_port_register(client, name.c_str(), type, flags, 0)) == NULL) {
        string err_message = "Unable to register port: ";
        err_message.append(name);
        throw runtime_error(err_message);
    }

    return port;
}


void JackClient::remove_in_port(string name)
{
    input_ports.erase(name);
}

void JackClient::remove_out_port(string name)
{
    output_ports.erase(name);
}

string JackClient::get_name()
{
    string name(jack_get_client_name(client));
    return name;
}

JackClient::jack_state_t JackClient::get_state()
{
    return client_state;
}


jack_nframes_t JackClient::get_sample_rate()
{
    return jack_get_sample_rate(client);
}

jack_nframes_t JackClient::get_buffer_size()
{
    return jack_get_buffer_size(client);
}
