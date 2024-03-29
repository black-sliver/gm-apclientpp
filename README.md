# GM-APClientPP

Work in progress.

This is a native ("C") Game Maker compatible wrapper for the C++ Archipelago Client Lib
[apclientpp](https://github.com/black-sliver/apclientpp),
allowing to connect to an [Archipelago](https://archipelago.gg/) server with native performance and SSL support.

## Supported Game Maker

This was written for Game Maker 7 and 8, however all versions of GM that support the API described in "Using DLL" in
"Designing Games with Game Maker" of GM7/GM8 should be supported.
It only depends on `external_define`, `external_call` and `execute_string`. This is not an "extension" (.gex) file.

GMS2+ is (currently) not supported because it uses different syntax for strings.

## API Limitations

* Arrays have to be passed as JSON string from GM to the DLL.
* Arrays have to be passed as globals from the DLL to GM callback functions/scripts.
* There is a limit of how many arguments can be passed into DLL functions and as such there are some extra helper
  functions to "pre-set" arguments.
* DLL functions that have no return value will return true on success or false in case of error.

## Types

GM only has `number` and `string`, so the types listed below are defined as follows

* `int` is a `number` with an integer value in the range ± 2<sup>53</sup>-1
* `bool` is a `number` with value `0` for `false` or `not 0` (typically `1`) for `true`.
   This is different from GM's boolean handling where true / false is `>=0.5` / `<0.5`.
* `str` is a `string` in GM, and a `const char*` in C
* `json` is a `str` that is valid json. A json array is `'[...]'`, a json object is `'{...}'`.
* `int[]` is either a `json` array of numbers (GM -> DLL) or a global array variable in GM (DLL -> GM)
* `str[]` is either a `json` array of strings (GM -> DLL) or a global array variable in GM (DLL -> GM)

## API

Below are the possible calls from GM to DLL. The actual call uses `external_call`, but should be wrapped in a script
with the same name, making the calls possible as listed below. See [gml/](gml/).

We use the following notation below: `name(arg_name: arg_type, ...): return_type`

**Warning:** some of the functions will only return correct values after room info or when `is_data_package_valid()`
  returns true, typically it's safe to just wait until `ap_connected` event was called.

* `apclient_init(api_version: int): bool` initialize the lib and set API version (currently `1`),
   returns `true` on success.
* `apclient_deinit(): bool` free all resources (kind of optional, but has to be called before a second `apclient_init`)
* `apclient_connect(uuid: str, game: str, host: str): bool` start connecting to a server.
   uuid can be an empty string (for now).
* `apclient_poll(): str` call this each frame (step) to handle communication and receive events,
   returns a string that needs to be executed using `execute_string` to call your event callbacks.
* `apclient_disconnect(): bool` reset internal state and disconnect.
* `apclient_reset(): bool` reset internal state and reconnect on next `poll`.
* `apclient_get_player_alias(slot: int): str` returns player alias, player name, or `'Unknown'` for given slot.
* `apclient_get_player_game(slot: int): str` returns game name or `''` for given slot.
* `apclient_get_location_name(id: int, game: str): str` returns location name or `'Unknown'` for given id and game.
* `apclient_get_location_id(name: str): int` returns location id for given name in connected game,
   or `< -9007199254740991` if not found.
* `apclient_get_item_name(id: int, game: str): str` like location_name, but for items.
* `apclient_get_item_id(name: str): int` like location_id, but for items.
* `apclient_render_json(json: str, format: int): str` takes a json string as received by `ap_print_json` event and
   turns it into text. Pass `0` (or `global.AP_RENDER_FORMAT_TEXT`) as format to receive plain text.
* `apclient_get_state(): int` returns one of the internal state values, see constants `global.AP_STATE_*`.
* `apclient_get_seed(): str` returns seed for connected room or `''` if not connected yet
* `apclient_get_slot(): str` returns connecting player (slot) name or `""` if `connect_slot` wasn't called yet.
* `apclient_get_player_number(): int` returns player number for connected player, or -1 if not connected yet.
* `apclient_get_team_number(): int` returns team number for connected player, or -1 if not connected yet.
* `apclient_get_hint_points(): int` returns hint points for connected player.
* `apclient_get_hint_cost_points(): int` returns hint cost in points (locations checked).
* `apclient_get_hint_cost_percent(): int` returns hint cost in percent (% of locations checked).
* `apclient_is_data_package_valid(): bool` returns `true` if all strings for item and location names are available.
* `apclient_get_server_time(): number`: returns the estimated server time stamp as floating point number.
   This may be useful for things like deathlink.
* `apclient_has_password(): bool`: available in and after `ap_room_info`, `true` if `connect_slot` requires password.
* `apclient_get_checked_locations(): str`: returns a string that should be passed into `execute_string`
   to set `global.ap_checked_locations: int[]` and `global.ap_checked_locations_len: int`
* `apclient_get_missing_locations(): str`: returns a string that should be passed into `execute_string`
   to set `global.ap_missing_locations: int[]` and `global.ap_missing_locations_len: int`

The following functions set variables that would normally be passed into another function, but can't be because of GM
limitations.

* `apclient_set_items_handling(items_handling: int): bool` set the items handling that will be passed to ConnectSlot
   and ConnectUpdate.
* `apclient_set_version(ma: int, mi: int, r: int): bool` set the mod/client version that will be passed to ConnectSlot.

The following functions interact directly with the AP server and return `true` if the command was successfully queue,
or false if the command was invalid or the connection was not established yet.

* `apclient_say(message: str): bool` sends a chat message
* `apclient_connect_slot(name: str, password: str, tags: json): bool` connects to a slot.
   Wait for `connected` or `connection_refused` event to get the result.
* `apclient_connect_update_items_handling(): bool` send ConnectUpdate, only changing items_handling.
* `apclient_connect_update(tags: str[]): bool` send ConnectUpdate, changing both tags and items_handling.
* `apclient_sync(): bool` send Sync message, this should not be needed.
* `apclient_status_update(status: int): bool` sends StatusUpdate message, see `AP_CLIENT_STATUS_*` constants.
* `apclient_location_checks(locations: int[]): bool` sends LocationChecks, marking locations as checked.
* `apclient_location_scouts(locations: int[], create_as_hints: int): bool` sends LocationScouts.
   Wait for `location_info`event  to get the result.

### TODO

* A way to extract values from slot_data.

### Not implemented

The following are not implemented for `api_version = 1`.
Open for suggestions / requests.

* `apclient_get_players(): List[NetWorkPlayer]` the return type of this is not easy to implement in GM.
* `apclient_bounce(...): bool` not sure about the API and `bounced` event isn't implemented yet either.
* `apclient_get(...): bool` data storage / GetReply and Received not implemented yet
* `apclient_set_notify(...): bool` as above
* `apclient_set(...): bool` as above

## Constants

The following consts should be set up in the init:

* `global AP_RENDER_FORMAT_TEXT = 0` - get plain text from render_json
* `global AP_RENDER_FORMAT_HTML = 1` - this can't be used at the moment because it's not implemented in render_json.
* `global AP_RENDER_FORMAT_ANSI = 2` - get ansi (terminal) color codes from render_json
* `global.AP_STATE_DISCONNECTED = 0` - disconnected
* `global.AP_STATE_SOCKET_CONNECTING = 1` - connect started
* `global.AP_STATE_SOCKET_CONNECTED = 2` - network connection accepted, nothing received yet
* `global.AP_STATE_ROOM_INFO = 3` - room info was received, slot not connected yet
* `global.AP_STATE_SLOT_CONNECTED = 4` - slot successfully connected, can send/receive locations/items and chat, etc.
* `global.AP_CLIENT_STATUS_UNKNOWN = 0` - default status
* `global.AP_CLIENT_STATUS_READY = 10` - player is ready to play (setting this is optional at the moment)
* `global.AP_CLIENT_STATUS_PLAYING = 20` - player is playing their game (as above)
* `global.AP_CLIENT_STATUS_GOAL = 30` - player reached their goal.
   Use this in `apclient_status_update` on goal completion.

## Events

The script returned by `apclient_poll` expects the following functions to exist in your scripts to handle events,
so you need to define all of them:

* `ap_room_info(data: json)` called when receiving RoomInfo from server, argument is currently unused (`'{}'`).
* `ap_slot_refused(len: int)` called when `apclient_connect_slot` failed, len is the size of error messages,
   messages can be retrieved from `global.arg_errors: str[]`.
* `ap_slot_connected(slot_data: json)` called when `apclient_connect_slot` succeeded
* `ap_items_received(index: int, len: int)` called when receiving items, index is the "item index" of the first item
   that was received, len is the number of items that were received.
   Use `global.arg_ids: int[]`, `global.arg_names: str[]`, `global.arg_flags: int[]` and `global.arg_players: int[]`
   to retrieve the items.
* `ap_location_info(len: int)` called as result of a scout, len is the number of results (individual scouts).
   Use `global.arg_locations: int[]`, `global.arg_items: int[]`, `global.arg_flags: int[]`
   and `global.arg_players: int[]` to get the corresponding location ids and item ids.
* `ap_location_checked(len: int)` called when locations for the connected slot were checked, len is the count.
   Use `global.arg_ids: int[]` to retrieve the checked locations ids.
* `ap_print_json(message: json)` pass `message` back in to render_json to get a human readable text
* `ap_socket_connected()` network connection established, useful to postpone connection timeouts, `room_info` will
   automatically be received later.
* `ap_socket_disconnected()` network connection died, can't send things until reconnected.
* `ap_socket_error(error: str)` network connection failed. Useful for debugging, but client will retry internally.
   Either keep a count of errors (abort for >2 failed attempts) or use a timeout for `ap_room_info` (abort after >5sec).

### Not implemented

The following are not implemented for `api_version = 1`.

* `ap_bounced(...)` when a player sent a Bounce
* `ap_retrieved(...)` result for `apclient_get`
* `ap_set_reply(...)` when a player sent a Set that matches your SetNotify
