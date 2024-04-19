#define apclient_init
{
    return external_call(ext_apclient_init, argument0)
}
#define apclient_deinit
{
    return external_call(ext_apclient_deinit)
}
#define apclient_connect
{
    return external_call(ext_apclient_connect, argument0, argument1, argument2)
}
#define apclient_poll
{
    return external_call(ext_apclient_poll)
}
#define apclient_disconnect
{
    return external_call(ext_apclient_disconnect)
}
#define apclient_reset
{
    return external_call(ext_apclient_reset)
}
#define apclient_get_player_alias
{
    return external_call(ext_apclient_get_player_alias, argument0)
}
#define apclient_get_player_game
{
    return external_call(ext_apclient_get_player_game, argument0)
}
#define apclient_get_location_name
{
    return external_call(ext_apclient_get_location_name, argument0, argument1)
}
#define apclient_get_location_id
{
    return external_call(ext_apclient_get_location_id, argument0)
}
#define apclient_get_item_name
{
    return external_call(ext_apclient_get_item_name, argument0, argument1)
}
#define apclient_get_item_id
{
    return external_call(ext_apclient_get_item_id, argument0)
}
#define apclient_render_json
{
    return external_call(ext_apclient_render_json, argument0, argument1)
}
#define apclient_get_state
{
    return external_call(ext_apclient_get_state)
}
#define apclient_get_seed
{
    return external_call(ext_apclient_get_seed)
}
#define apclient_get_slot
{
    return external_call(ext_apclient_get_slot)
}
#define apclient_get_player_number
{
    return external_call(ext_apclient_get_player_number)
}
#define apclient_get_team_number
{
    return external_call(ext_apclient_get_team_number)
}
#define apclient_get_hint_points
{
    return external_call(ext_apclient_get_hint_points)
}
#define apclient_get_hint_cost_points
{
    return external_call(ext_apclient_get_hint_cost_points)
}
#define apclient_get_hint_cost_percent
{
    return external_call(ext_apclient_get_hint_cost_percent)
}
#define apclient_is_data_package_valid
{
    return external_call(ext_apclient_is_data_package_valid)
}
#define apclient_get_server_time
{
    return external_call(ext_apclient_get_server_time)
}
#define apclient_has_password
{
    return external_call(ext_apclient_has_password)
}
#define apclient_get_checked_locations
{
    return external_call(ext_apclient_get_checked_locations)
}
#define apclient_get_missing_locations
{
    return external_call(ext_apclient_get_missing_locations)
}
#define apclient_set_items_handling
{
    return external_call(ext_apclient_set_items_handling, argument0)
}
#define apclient_set_version
{
    return external_call(ext_apclient_set_version, argument0, argument1, argument2)
}
#define apclient_say
{
    return external_call(ext_apclient_say, argument0)
}
#define apclient_connect_slot
{
    return external_call(ext_apclient_connect_slot, argument0, argument1, argument2)
}
#define apclient_connect_update_items_handling
{
    return external_call(ext_apclient_connect_update_items_handling)
}
#define apclient_connect_update
{
    return external_call(ext_apclient_connect_update, argument0)
}
#define apclient_sync
{
    return external_call(ext_apclient_sync)
}
#define apclient_status_update
{
    return external_call(ext_apclient_status_update, argument0)
}
#define apclient_location_checks
{
    return external_call(ext_apclient_location_checks, argument0)
}
#define apclient_location_scouts
{
    return external_call(ext_apclient_location_scouts, argument0, argument1)
}
#define apclient_bounce
{
    return external_call(ext_apclient_bounce, argument0)
}
#define apclient_set_bounce_targets
{
    return external_call(ext_apclient_set_bounce_targets, argument0, argument1, argument2)
}
#define apclient_death_link
{
    return external_call(ext_apclient_death_link, argument0)
}
#define apclient_json_proxy
{
    return external_call(ext_apclient_json_proxy, argument0, argument1)
}
#define apclient_json_exists
{
    return external_call(ext_apclient_json_exists, argument0, argument1)
}
#define apclient_json_typeof
{
    return external_call(ext_apclient_json_typeof, argument0)
}
#define apclient_json_size
{
    return external_call(ext_apclient_json_size, argument0)
}
#define apclient_json_get_string
{
    return external_call(ext_apclient_json_get_string, argument0)
}
#define apclient_json_string_at
{
    return external_call(ext_apclient_json_string_at, argument0, argument1)
}
#define apclient_json_get_number
{
    return external_call(ext_apclient_json_get_number, argument0)
}
#define apclient_json_number_at
{
    return external_call(ext_apclient_json_number_at, argument0, argument1)
}
#define apclient_json_dump
{
    return external_call(ext_apclient_json_dump, argument0)
}
