/* gm-apclientpp.h
 * Game Maker wrapper for apclientpp
 * https://github.com/black-sliver/gm-apclientpp
 */

/* switch between dll export and dll import */
#ifndef GM_DLL_EXPORT
#   ifdef _WIN32
#       ifdef GM_APCLIENTPP_EXPORTS
#           define GM_DLL_EXPORT __declspec(dllexport)
#       else
#           define GM_DLL_EXPORT __declspec(dllimport)
#       endif
#   else
#       define GM_DLL_EXPORT __attribute__ ((visibility ("default")))
#   endif
#   define GM_DLL_EXPORT_DEFINED
#endif

/* default to C calling convention */
#ifndef GM_DLL_CALL
#   ifdef _WIN32
#       define GM_DLL_CALL __cdecl
#   else
#       define GM_DLL_CALL
#   endif
#   define GM_DLL_CALL_DEFINED
#endif

/* exported functions */
#ifdef __cplusplus
extern "C" {
#endif
    GM_DLL_EXPORT double GM_DLL_CALL apclient_init(double api_version);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_deinit(); 
    GM_DLL_EXPORT double GM_DLL_CALL apclient_connect(const char* uuid, const char* game, const char* host);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_disconnect();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_reset();
    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_poll();

    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_get_player_alias(double slot);
    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_get_player_game(double slot);
    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_get_location_name(double id, const char* game);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_get_location_id(const char* name);
    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_get_item_name(double id, const char* game);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_get_item_id(const char* name);

    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_render_json(const char* json_str, double format);

    GM_DLL_EXPORT double GM_DLL_CALL apclient_get_state();
    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_get_seed();
    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_get_slot();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_get_player_number();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_get_hint_points();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_get_hint_cost_points();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_get_hint_cost_percent();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_is_data_package_valid();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_get_server_time();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_has_password();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_get_server_time();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_has_password();

    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_get_checked_locations();
    GM_DLL_EXPORT const char* GM_DLL_CALL apclient_get_missing_locations();

    GM_DLL_EXPORT double GM_DLL_CALL apclient_set_items_handling(double items_handling);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_set_version(double major, double minor, double revision);

    GM_DLL_EXPORT double GM_DLL_CALL apclient_say(const char* message);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_connect_slot(const char* name, const char* password, const char* tags);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_connect_update_items_handling();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_connect_update(const char* tags);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_sync();
    GM_DLL_EXPORT double GM_DLL_CALL apclient_status_update(double status);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_location_checks(const char* locations_json);
    GM_DLL_EXPORT double GM_DLL_CALL apclient_location_scouts(const char* locations_json, double create_as_hint);
#ifdef __cplusplus
}
#endif

/* cleanup macros */
#ifdef GM_DLL_EXPORT_DEFINED
#   undef GM_DLL_EXPORT
#endif
#ifdef GM_DLL_CALL_DEFINED
#   undef GM_DLL_CALL
#endif
