{
    // set up external function call signatures
    global.ext_apclient_init = external_define('gm-apclientpp.dll', 'apclient_init', dll_cdecl,
                                               ty_string, 1,
                                               ty_real);
    //  TODO: run all the other external_define() to set up global.ap_*

    // set uo constants
    global AP_RENDER_FORMAT_TEXT = 0
    global AP_RENDER_FORMAT_HTML = 1
    global AP_RENDER_FORMAT_ANSI = 2
    global.AP_STATE_DISCONNECTED = 0
    global.AP_STATE_SOCKET_CONNECTING = 1
    global.AP_STATE_SOCKET_CONNECTED = 2
    global.AP_STATE_ROOM_INFO = 3
    global.AP_STATE_SLOT_CONNECTED = 4
    global.AP_CLIENT_STATUS_UNKNOWN = 0
    global.AP_CLIENT_STATUS_READY = 10
    global.AP_CLIENT_STATUS_PLAYING = 20
    global.AP_CLIENT_STATUS_GOAL = 30
}
