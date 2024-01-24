#define ap_init
///ap_init()

global.AP_RENDER_FORMAT_TEXT = 0 // get plain text from render_json
global.AP_RENDER_FORMAT_HTML = 1 // this can't be used at the moment because it's not implemented in render_json.
global.AP_RENDER_FORMAT_ANSI = 2 // get ansi (terminal) color codes from render_json
global.AP_STATE_DISCONNECTED = 0 // disconnected
global.AP_STATE_SOCKET_CONNECTING = 1 // connect started
global.AP_STATE_SOCKET_CONNECTED = 2 // network connection accepted, nothing received yet
global.AP_STATE_ROOM_INFO = 3 // room info was received, slot not connected yet
global.AP_STATE_SLOT_CONNECTED = 4 // slot successfully connected, can send/receive locations/items and chat, etc.
global.AP_CLIENT_STATUS_UNKNOWN = 0 // default status
global.AP_CLIENT_STATUS_READY = 10 // player is ready to play (setting this is optional at the moment)
global.AP_CLIENT_STATUS_PLAYING = 20 // player is playing their game (as above)
global.AP_CLIENT_STATUS_GOAL = 30 // player reached their goal. Use this in apclient_status_update on goal completion.

globalvar ext_apclient_init, ext_apclient_deinit, ext_apclient_connect, ext_apclient_poll, ext_apclient_disconnect, ext_apclient_reset, ext_apclient_get_player_alias,
    ext_apclient_get_player_game, ext_apclient_get_location_name, ext_apclient_get_location_id, ext_apclient_get_item_name, ext_apclient_get_item_id,
    ext_apclient_render_json, ext_apclient_get_state, ext_apclient_get_seed, ext_apclient_get_slot, ext_apclient_get_player_number, ext_apclient_get_team_number,
    ext_apclient_get_hint_points, ext_apclient_get_hint_cost_points, ext_apclient_get_hint_cost_percent, ext_apclient_is_data_package_valid, ext_apclient_get_server_time,
    ext_apclient_has_password, ext_apclient_get_checked_locations, ext_apclient_get_missing_locations, ext_apclient_set_items_handling, ext_apclient_set_version,
    ext_apclient_say, ext_apclient_connect_slot, ext_apclient_connect_update_items_handling, ext_apclient_connect_update, ext_apclient_sync, ext_apclient_status_update,
    ext_apclient_location_checks, ext_apclient_location_scouts;

// ext_FN_NAME = external_define("gm-apclientpp.dll", "FN_NAME", dll_cdecl, RESULT_TYPE, ARG_NUM, [ARG_1_TYPE], ...);
ext_apclient_init = external_define("gm-apclientpp.dll", "apclient_init", dll_cdecl, ty_real, 1, ty_real);
ext_apclient_deinit = external_define("gm-apclientpp.dll", "apclient_deinit", dll_cdecl, ty_real, 0);
ext_apclient_connect = external_define("gm-apclientpp.dll", "apclient_connect", dll_cdecl, ty_real, 3, ty_string, ty_string, ty_string);
ext_apclient_poll = external_define("gm-apclientpp.dll", "apclient_poll", dll_cdecl, ty_string, 0);
ext_apclient_disconnect = external_define("gm-apclientpp.dll", "apclient_disconnect", dll_cdecl, ty_real, 0);
ext_apclient_reset = external_define("gm-apclientpp.dll", "apclient_reset", dll_cdecl, ty_real, 0);
ext_apclient_get_player_alias = external_define("gm-apclientpp.dll", "apclient_get_player_alias", dll_cdecl, ty_string, 1, ty_real);
ext_apclient_get_player_game = external_define("gm-apclientpp.dll", "apclient_get_player_game", dll_cdecl, ty_string, 1, ty_real);
ext_apclient_get_location_name = external_define("gm-apclientpp.dll", "apclient_get_location_name", dll_cdecl, ty_string, 2, ty_real, ty_string);
ext_apclient_get_location_id = external_define("gm-apclientpp.dll", "apclient_get_location_id", dll_cdecl, ty_real, 1, ty_string);
ext_apclient_get_item_name = external_define("gm-apclientpp.dll", "apclient_get_item_name", dll_cdecl, ty_string, 2, ty_real, ty_string);
ext_apclient_get_item_id = external_define("gm-apclientpp.dll", "apclient_get_item_id", dll_cdecl, ty_real, 1, ty_string);
ext_apclient_render_json = external_define("gm-apclientpp.dll", "apclient_render_json", dll_cdecl, ty_string, 2, ty_string, ty_real);
ext_apclient_get_state = external_define("gm-apclientpp.dll", "apclient_get_state", dll_cdecl, ty_real, 0);
ext_apclient_get_seed = external_define("gm-apclientpp.dll", "apclient_get_seed", dll_cdecl, ty_string, 0);
ext_apclient_get_slot = external_define("gm-apclientpp.dll", "apclient_get_slot", dll_cdecl, ty_string, 0);
ext_apclient_get_player_number = external_define("gm-apclientpp.dll", "apclient_get_player_number", dll_cdecl, ty_real, 0);
ext_apclient_get_team_number = external_define("gm-apclientpp.dll", "apclient_get_team_number", dll_cdecl, ty_real, 0);
ext_apclient_get_hint_points = external_define("gm-apclientpp.dll", "apclient_get_hint_points", dll_cdecl, ty_real, 0);
ext_apclient_get_hint_cost_points = external_define("gm-apclientpp.dll", "apclient_get_hint_cost_points", dll_cdecl, ty_real, 0);
ext_apclient_get_hint_cost_percent = external_define("gm-apclientpp.dll", "apclient_get_hint_cost_percent", dll_cdecl, ty_real, 0);
ext_apclient_is_data_package_valid = external_define("gm-apclientpp.dll", "apclient_is_data_package_valid", dll_cdecl, ty_real, 0);
ext_apclient_get_server_time = external_define("gm-apclientpp.dll", "apclient_get_server_time", dll_cdecl, ty_real, 0);
ext_apclient_has_password = external_define("gm-apclientpp.dll", "apclient_has_password", dll_cdecl, ty_real, 0);
ext_apclient_get_checked_locations = external_define("gm-apclientpp.dll", "apclient_get_checked_locations", dll_cdecl, ty_string, 0);
ext_apclient_get_missing_locations = external_define("gm-apclientpp.dll", "apclient_get_missing_locations", dll_cdecl, ty_string, 0);
ext_apclient_set_items_handling = external_define("gm-apclientpp.dll", "apclient_set_items_handling", dll_cdecl, ty_real, 1, ty_real);
ext_apclient_set_version = external_define("gm-apclientpp.dll", "apclient_set_version", dll_cdecl, ty_real, 3, ty_real, ty_real, ty_real);
ext_apclient_say = external_define("gm-apclientpp.dll", "apclient_say", dll_cdecl, ty_real, 1, ty_string);
ext_apclient_connect_slot = external_define("gm-apclientpp.dll", "apclient_connect_slot", dll_cdecl, ty_real, 3, ty_string, ty_string, ty_string);
ext_apclient_connect_update_items_handling = external_define("gm-apclientpp.dll", "apclient_connect_update_items_handling", dll_cdecl, ty_real, 0);
ext_apclient_connect_update = external_define("gm-apclientpp.dll", "apclient_connect_update", dll_cdecl, ty_real, 0);
ext_apclient_sync = external_define("gm-apclientpp.dll", "apclient_sync", dll_cdecl, ty_real, 0);
ext_apclient_status_update = external_define("gm-apclientpp.dll", "apclient_status_update", dll_cdecl, ty_real, 1, ty_real);
ext_apclient_location_checks = external_define("gm-apclientpp.dll", "apclient_location_checks", dll_cdecl, ty_real, 1, ty_string);
ext_apclient_location_scouts = external_define("gm-apclientpp.dll", "apclient_location_scouts", dll_cdecl, ty_real, 2, ty_string, ty_real);

