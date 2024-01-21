#include <string>
#include <queue>
#include <utility>
#include <memory>
#include <cstdint>

#include <apclient.hpp>
#include <nlohmann/json.hpp>


#ifndef DLL_EXPORT
#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#define DLL_CALL __cdecl
#else
#define DLL_EXPORT __attribute__ ((visibility ("default")))
#define DLL_CALL
#endif
#endif


extern "C" {
    DLL_EXPORT double DLL_CALL apclient_init(double api_version);
    DLL_EXPORT double DLL_CALL apclient_deinit(); 
    DLL_EXPORT double DLL_CALL apclient_connect(const char* uuid, const char* game, const char* host);
    DLL_EXPORT double DLL_CALL apclient_disconnect();
    DLL_EXPORT double DLL_CALL apclient_reset();
    DLL_EXPORT const char* DLL_CALL apclient_poll();

    DLL_EXPORT const char* DLL_CALL apclient_get_player_alias(double slot);
    DLL_EXPORT const char* DLL_CALL apclient_get_player_game(double slot);
    DLL_EXPORT const char* DLL_CALL apclient_get_location_name(double id, const char* game);
    DLL_EXPORT double DLL_CALL apclient_get_location_id(const char* name);
    DLL_EXPORT const char* DLL_CALL apclient_get_item_name(double id, const char* game);
    DLL_EXPORT double DLL_CALL apclient_get_item_id(const char* name);

    DLL_EXPORT const char* DLL_CALL apclient_render_json(const char* json_str, double format);

    DLL_EXPORT double DLL_CALL apclient_get_state();
    DLL_EXPORT const char* DLL_CALL apclient_get_seed();
    DLL_EXPORT const char* DLL_CALL apclient_get_slot();
    DLL_EXPORT double DLL_CALL apclient_get_player_number();
    DLL_EXPORT double DLL_CALL apclient_get_hint_points();
    DLL_EXPORT double DLL_CALL apclient_get_hint_cost_points();
    DLL_EXPORT double DLL_CALL apclient_get_hint_cost_percent();
    DLL_EXPORT double DLL_CALL apclient_is_data_package_valid();
    DLL_EXPORT double DLL_CALL apclient_get_server_time();
    DLL_EXPORT double DLL_CALL apclient_has_password();
    DLL_EXPORT double DLL_CALL apclient_get_server_time();
    DLL_EXPORT double DLL_CALL apclient_has_password();

    DLL_EXPORT const char* DLL_CALL apclient_get_checked_locations();
    DLL_EXPORT const char* DLL_CALL apclient_get_missing_locations();

    DLL_EXPORT double DLL_CALL apclient_set_items_handling(double items_handling);
    DLL_EXPORT double DLL_CALL apclient_set_version(double major, double minor, double revision);

    DLL_EXPORT double DLL_CALL apclient_say(const char* message);
    DLL_EXPORT double DLL_CALL apclient_connect_slot(const char* name, const char* password, const char* tags);
    DLL_EXPORT double DLL_CALL apclient_connect_update_items_handling();
    DLL_EXPORT double DLL_CALL apclient_connect_update(const char* tags);
    DLL_EXPORT double DLL_CALL apclient_sync();
    DLL_EXPORT double DLL_CALL apclient_status_update(double status);
    DLL_EXPORT double DLL_CALL apclient_location_checks(const char* locations_json);
    DLL_EXPORT double DLL_CALL apclient_location_scouts(const char* locations_json, double create_as_hint);
}


#define GM_FALSE (0.)
#define GM_TRUE (1.)
#define GM_BOOL(cond) ((cond) ? GM_TRUE : GM_FALSE)

using json = nlohmann::json;


static int api = 0;
static std::unique_ptr<APClient> apclient;
static std::string result; // buffer for string results of simple functions
static std::queue<std::string> queue; // queue of events to run on poll
static std::string script; // buffer for script result
static APClient::Version client_version;
static int items_handling = 0;
static bool items_handling_changed = false;


static void from_json(const json& j, std::list<APClient::TextNode>& nodes) {
    for (const auto& v: j) {
        nodes.push_back(APClient::TextNode::from_json(v));
    }
}


double apclient_init(double api_version)
{
    if (api != 0) // already initialized
        return GM_FALSE;
    if (api_version != 1) // unsupported api version
        return GM_FALSE;
    api = api_version;
    client_version = {0, 4, 3};
    items_handling = 0;
    return GM_TRUE;
}

double apclient_deinit()
{
    apclient_disconnect();
    api = 0;
    return GM_TRUE;
}

double apclient_connect(const char* uuid, const char* game, const char* host)
{
    apclient.reset(new APClient(uuid, game, host));
    return GM_TRUE;
}

double apclient_disconnect()
{
    apclient_reset();
    apclient.reset(nullptr);
    return GM_TRUE;
}

double apclient_reset()
{
    queue = std::queue<std::string>();
    if (!apclient)
        return GM_FALSE;
    apclient->reset();
    return GM_TRUE;
}

const char* apclient_poll()
{
    if (!apclient)
        return "{}";
    apclient->poll();
    if (queue.empty()) {
        script = "{}";
    } else {
        script = std::move(queue.front());
        queue.pop();
    }
    return script.c_str();
}

const char* apclient_get_player_alias(double slot)
{
    if (!apclient)
        return "";
    result = apclient->get_player_alias((int)slot);
    return result.c_str();
}

const char* apclient_get_player_game(double slot)
{
    if (!apclient)
        return "";
    result = apclient->get_player_game((int)slot);
    return result.c_str();
}

const char* apclient_get_location_name(double id, const char* game)
{
    if (!apclient)
        return "";
    result = apclient->get_location_name((uint64_t)id, game);
    return result.c_str();
}

double apclient_get_location_id(const char* name)
{
    if (!apclient)
        return (double)APClient::INVALID_NAME_ID;
    return apclient->get_location_id(name);
}

const char* apclient_get_item_name(double id, const char* game)
{
    if (!apclient)
        return "";
    result = apclient->get_item_name((uint64_t)id, game);
    return result.c_str();
}

double apclient_get_item_id(const char* name)
{
    if (!apclient)
        return (double)APClient::INVALID_NAME_ID;
    return apclient->get_item_id(name);
}

const char* apclient_render_json(const char* json_str, double format)
{
    if (!apclient)
        return "";

    std::list<APClient::TextNode> msg;
    try {
        from_json(json::parse(json_str), msg);
    } catch (std::exception ex) {
        return "";
    }

    result = apclient->render_json(msg, (APClient::RenderFormat)format);
    return result.c_str();
}

double apclient_get_state()
{
    if (!apclient)
        return 0.;
    return (double)apclient->get_state();
}

const char* apclient_get_seed()
{
    if (!apclient)
        return "";
    result = apclient->get_seed();
    return result.c_str();
}

const char* apclient_get_slot()
{
    if (!apclient)
        return "";
    result = apclient->get_slot();
    return result.c_str();
}

double apclient_get_player_number()
{
    if (!apclient)
        return -1.;
    return (double)apclient->get_player_number();
}

double apclient_get_hint_points()
{
    if (!apclient)
        return 0.;
    return (double)apclient->get_hint_points();
}

double apclient_get_hint_cost_points()
{
    if (!apclient)
        return 0.;
    return (double)apclient->get_hint_cost_points();
}

double apclient_get_hint_cost_percent()
{
    if (!apclient)
        return 0.;
    return (double)apclient->get_hint_cost_percent();
}

double apclient_is_data_package_valid()
{
    return GM_BOOL(apclient && apclient->is_data_package_valid());
}

double apclient_get_server_time()
{
    if (!apclient)
        return 0.;
    return apclient->get_server_time();
}

double apclient_has_password()
{
    return GM_BOOL(apclient && apclient->has_password());
}

const char* apclient_get_checked_locations()
{
    if (!apclient)
        return "{}";
    script =
        "{\r\n"
        "    global.ap_checked_locations = [];\r\n";
    int i = 0;
    for (const auto& location: apclient->get_checked_locations())
        script += "    global.ap_checked_locations[" + std::to_string(i++) + "]=" + std::to_string(location) + ";\r\n";
    script += 
        "    global.ap_checked_locations_len=" + std::to_string(i) + ";\r\n"
        "}";
    return script.c_str();
}

const char* apclient_get_missing_locations()
{
    if (!apclient)
        return "{}";
    script =
        "{\r\n"
        "    global.ap_missing_locations = [];\r\n";
    int i = 0;
    for (const auto& location: apclient->get_missing_locations())
        script += "    global.ap_missing_locations[" + std::to_string(i++) + "]=" + std::to_string(location) + ";\r\n";
    script += 
        "    global.ap_missing_locations_len=" + std::to_string(i) + ";\r\n"
        "}";
    return script.c_str();
}

double apclient_set_items_handling(double value)
{
    items_handling_changed = items_handling != (int)value;
    items_handling = (int)value;
    return GM_TRUE;
}

double apclient_set_version(double major, double minor, double revision)
{
    client_version = {(int)major, (int)minor, (int)revision};
    return GM_TRUE;
}

double apclient_say(const char* message)
{
    return GM_BOOL(apclient && apclient->Say(message));
}

double apclient_connect_slot(const char* name, const char* password, const char* tags_str)
{
    json tags_j = json::parse(tags_str);
    if (apclient && apclient->ConnectSlot(name, password, items_handling, tags_j, client_version)) {
        items_handling_changed = false;
        return GM_TRUE;
    }
    return GM_FALSE;
}

double apclient_connect_update_items_handling()
{
    if (apclient && apclient->ConnectUpdate(true, items_handling, false, {})) {
        items_handling_changed = false;
        return GM_TRUE;
    }
    return GM_FALSE;
}

double apclient_connect_update(const char* tags_str)
{
    json tags_j = json::parse(tags_str);
    if (apclient && apclient->ConnectUpdate(items_handling_changed, items_handling, true, tags_j)) {
        items_handling_changed = false;
        return GM_TRUE;
    }
    return GM_FALSE;
}

double apclient_sync()
{
    return GM_BOOL(apclient && apclient->Sync());
}

double apclient_status_update(double status)
{
    return GM_BOOL(apclient && apclient->StatusUpdate((APClient::ClientStatus)status));
}

double apclient_location_checks(const char* locations_str)
{
    json locations_j = json::parse(locations_str);
    return GM_BOOL(apclient && apclient->LocationChecks(locations_j));
}

double apclient_location_scouts(const char* locations_str, double create_as_hint)
{
    json locations_j = json::parse(locations_str);
    return GM_BOOL(apclient && apclient->LocationScouts(locations_j, (int)create_as_hint));
}
