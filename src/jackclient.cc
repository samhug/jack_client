#include <iostream>
#include <stdexcept>

#include "jackclient.h"

//using namespace std;

/**
 * Initializes the JACK client and registers callbacks
 *
 * @param[in]   name    The name to register the JACK client under
 */
JackClient::JackClient(string name)
{
    client_options = JackNullOption;

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
    client->__process_callback(nframes);
}

int JackClient::__process_callback(jack_nframes_t nframes)
{

}

/**
 * Activates the JACK client
 */
void JackClient::start()
{
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
    if (jack_client_close(client) != 0) {
        throw runtime_error("Unable to close JACK client");
    }

    client_state = closed;
}

string JackClient::get_name()
{
    string name(jack_get_client_name(client));
    return name;
}

jack_nframes_t JackClient::get_sample_rate()
{
    return jack_get_sample_rate(client);
}

jack_nframes_t JackClient::get_buffer_size()
{
    return jack_get_buffer_size(client);
}
