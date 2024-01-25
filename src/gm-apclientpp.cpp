/* gm-apclientpp.cpp
 * Game Maker wrapper for apclientpp
 * https://github.com/black-sliver/gm-apclientpp
 */

#include "../include/gm-apclientpp.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>

#include <apclient.hpp>
#include <nlohmann/json.hpp>


#ifndef GM_APCLIENTPP_EXPORTS
#   ifndef GM_DLL_EXPORT // compiling cpp without explicit export
#       error "Define GM_APCLIENTPP_EXPORTS when building DLL or GM_DLL_EXPORT= when building static."
#   endif
#endif


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
static std::thread worker;
static std::mutex mut; // protect data access from two threads simultaneously
static std::mutex connect_mutex; // protect connect/disconnect because worker is global


static struct Cleanup {
    virtual ~Cleanup() {
        // automatically call deinit on exit
        // this is required to stop the worker thread, if running
        apclient_deinit();
    }
} cleanup;


static void from_json(const json& j, std::list<APClient::TextNode>& nodes) {
    for (const auto& v: j) {
        nodes.push_back(APClient::TextNode::from_json(v));
    }
}

static void queue_script(const std::string& commands)
{
    queue.push("{\r\n" + commands + "\r\n}");
}

static void replace_all(std::string& s, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = s.find(from, start_pos)) != std::string::npos) {
        s.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

static void escape_inplace(std::string& s)
{
    // # -> \#
    replace_all(s, "#", "\\#");
    // \n -> #
    replace_all(s, "\n", "#");
    // \r -> nothing
    replace_all(s, "\r", "");
    // ' -> ' + "'" + '
    replace_all(s, "'", "'+\"'\"+'");
}

static std::string escape_string(const std::string& orig)
{
   std::string s = orig;
   escape_inplace(s);
   return s;
}

static void show_error(const std::string& error)
{
    queue_script("show_message('Error: " + escape_string(error) + "');");
}


static void apclient_impl_reset()
{
    queue = std::queue<std::string>();
    if (apclient)
        apclient->reset();
}

static void apclient_impl_disconnect()
{
    apclient_impl_reset();
    apclient.reset(nullptr);
}

static void apclient_impl_auto_poll()
{
    while (true) {
        {
            const std::lock_guard<std::mutex> lock(mut);
            if (!apclient)
                break;
            apclient->poll();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    const std::lock_guard<std::mutex> lock(mut);
}

static void apclient_atexit()
{
    apclient_deinit();
}


double apclient_init(double api_version)
{
    const std::lock_guard<std::mutex> lock(mut);
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
    const std::lock_guard<std::mutex> connect_lock(connect_mutex);
    {
        const std::lock_guard<std::mutex> lock(mut);
        apclient_impl_disconnect();
    }
    {
        if (worker.joinable())
            worker.join();
    }
    {
        const std::lock_guard<std::mutex> lock(mut);
        api = 0;
        return GM_TRUE;
    }
}

double apclient_connect(const char* uuid, const char* game, const char* host)
{
    const std::lock_guard<std::mutex> connect_lock(connect_mutex);
    {
        // stop previous client, if any
        const std::lock_guard<std::mutex> lock(mut);
        apclient_impl_disconnect();
    }
    {
        // wait for worker thread to stop
        if (worker.joinable())
            worker.join();
    }
    {
        // start new client
        const std::lock_guard<std::mutex> lock(mut);
        apclient.reset(new APClient(uuid, game, host));

        apclient->set_room_info_handler([]() {
            queue_script(
                "j = '{}';\r\n" // currently we don't pass anything from room info
                "ap_room_info(j);"
            );
        });

        apclient->set_slot_refused_handler([](const std::list<std::string>& reasons) {
            std::string s;
            int i = 0;
            for (const auto& reason: reasons)
                s += "global.arg_errors[" + std::to_string(i++) + "]='" + escape_string(reason) + "';\r\n";
            s += "ap_slot_refused(" + std::to_string(i) + ");";
            queue_script(s);
        });

        apclient->set_slot_connected_handler([](const json& slot_data) {
            queue_script(
                "j = '" + escape_string(slot_data.dump()) + "';\r\n"
                "ap_slot_connected(j);"
            );
        });

        apclient->set_items_received_handler([](const std::list<APClient::NetworkItem>& items) {
            std::string game = apclient->get_player_game(apclient->get_player_number());
            std::string s;
            int i = 0;
            for (const auto& item: items) {
                std::string item_name = apclient->get_item_name(item.item, game);
                s +=
                    "global.arg_items[" + std::to_string(i) + "]=" + std::to_string(item.item) + ";\r\n"
                    "global.arg_names[" + std::to_string(i) + "]='" + escape_string(item_name) + "';\r\n"
                    "global.arg_flags[" + std::to_string(i) + "]=" + std::to_string(item.flags) + ";\r\n"
                    "global.arg_players[" + std::to_string(i) + "]=" + std::to_string(item.player) + ";\r\n";
                i++;
            };
            s += "ap_location_info(" + std::to_string(i) + ");";
        });

        apclient->set_location_info_handler([](const std::list<APClient::NetworkItem>& items) {
            std::string s;
            int i = 0;
            for (const auto& item: items) {
                s +=
                    "global.arg_items[" + std::to_string(i) + "]=" + std::to_string(item.item) + ";\r\n"
                    "global.arg_flags[" + std::to_string(i) + "]=" + std::to_string(item.flags) + ";\r\n"
                    "global.arg_players[" + std::to_string(i) + "]=" + std::to_string(item.player) + ";\r\n"
                    "global.arg_locations[" + std::to_string(i) + "]=" + std::to_string(item.location) + ";\r\n";
                i++;
            };
            s += "ap_location_info(" + std::to_string(i) + ");";
        });

        apclient->set_location_checked_handler([](const std::list<int64_t>& locations) {
            std::string s;
            int i = 0;
            for (const auto& location: locations)
                s += "global.arg_ids[" + std::to_string(i++) + "]=" + std::to_string(location) + ";\r\n";
            s += "ap_location_checked(" + std::to_string(i) + ");";
            queue_script(s);
        });

        apclient->set_print_json_handler([](const json& command) {
            queue_script(
                "j = '" + escape_string(command.dump()) + "';\r\n"
                "ap_print_json(j);"
            );
        });

        apclient->set_socket_connected_handler([]() {
            queue_script("ap_socket_connected();");
        });

        apclient->set_socket_disconnected_handler([]() {
            queue_script("ap_socket_disconnected();");
        });

        apclient->set_socket_error_handler([](const std::string& msg) {
            queue_script("ap_socket_error('" + escape_string(msg) + "');");
        });

        // start worker
        worker = std::thread(apclient_impl_auto_poll);
    }

    return GM_TRUE;
}

double apclient_disconnect()
{
    const std::lock_guard<std::mutex> connect_lock(connect_mutex);
    {
        const std::lock_guard<std::mutex> lock(mut);
        apclient_impl_disconnect();
    }
    {
        if (worker.joinable())
            worker.join();
    }
    return GM_TRUE;
}

double apclient_reset()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return GM_FALSE;
    apclient_impl_reset();
    return GM_TRUE;
}

const char* apclient_poll()
{
    const std::lock_guard<std::mutex> lock(mut);
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
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return "";
    result = apclient->get_player_alias((int)slot);
    return result.c_str();
}

const char* apclient_get_player_game(double slot)
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return "";
    result = apclient->get_player_game((int)slot);
    return result.c_str();
}

const char* apclient_get_location_name(double id, const char* game)
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return "";
    result = apclient->get_location_name((uint64_t)id, game);
    return result.c_str();
}

double apclient_get_location_id(const char* name)
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return (double)APClient::INVALID_NAME_ID;
    return apclient->get_location_id(name);
}

const char* apclient_get_item_name(double id, const char* game)
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return "";
    result = apclient->get_item_name((uint64_t)id, game);
    return result.c_str();
}

double apclient_get_item_id(const char* name)
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return (double)APClient::INVALID_NAME_ID;
    return apclient->get_item_id(name);
}

const char* apclient_render_json(const char* json_str, double format)
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return "";

    std::list<APClient::TextNode> msg;
    try {
        from_json(json::parse(json_str), msg);
    } catch (std::exception ex) {
        show_error(ex.what());
        return "";
    }

    int int_format = (int)format;
    result = apclient->render_json(msg, (APClient::RenderFormat)int_format);
    return result.c_str();
}

double apclient_get_state()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return 0.;
    return (double)apclient->get_state();
}

const char* apclient_get_seed()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return "";
    result = apclient->get_seed();
    return result.c_str();
}

const char* apclient_get_slot()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return "";
    result = apclient->get_slot();
    return result.c_str();
}

double apclient_get_player_number()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return -1.;
    return (double)apclient->get_player_number();
}

double apclient_get_team_number()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return -1.;
    return (double)apclient->get_team_number();
}

double apclient_get_hint_points()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return 0.;
    return (double)apclient->get_hint_points();
}

double apclient_get_hint_cost_points()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return 0.;
    return (double)apclient->get_hint_cost_points();
}

double apclient_get_hint_cost_percent()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return 0.;
    return (double)apclient->get_hint_cost_percent();
}

double apclient_is_data_package_valid()
{
    const std::lock_guard<std::mutex> lock(mut);
    return GM_BOOL(apclient && apclient->is_data_package_valid());
}

double apclient_get_server_time()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return 0.;
    return apclient->get_server_time();
}

double apclient_has_password()
{
    const std::lock_guard<std::mutex> lock(mut);
    return GM_BOOL(apclient && apclient->has_password());
}

const char* apclient_get_checked_locations()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return "{}";
    script =
        "{\r\n";
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
    const std::lock_guard<std::mutex> lock(mut);
    if (!apclient)
        return "{}";
    script =
        "{\r\n";
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
    const std::lock_guard<std::mutex> lock(mut);
    items_handling_changed = items_handling != (int)value;
    items_handling = (int)value;
    return GM_TRUE;
}

double apclient_set_version(double major, double minor, double revision)
{
    const std::lock_guard<std::mutex> lock(mut);
    client_version = {(int)major, (int)minor, (int)revision};
    return GM_TRUE;
}

double apclient_say(const char* message)
{
    const std::lock_guard<std::mutex> lock(mut);
    return GM_BOOL(apclient && apclient->Say(message));
}

double apclient_connect_slot(const char* name, const char* password, const char* tags_str)
{
    const std::lock_guard<std::mutex> lock(mut);
    json tags_j;
    try {
        tags_j = json::parse(tags_str);
    } catch (std::exception ex) {
        show_error(ex.what());
        return GM_FALSE;
    }

    if (apclient && apclient->ConnectSlot(name, password, items_handling, tags_j, client_version)) {
        items_handling_changed = false;
        return GM_TRUE;
    }
    return GM_FALSE;
}

double apclient_connect_update_items_handling()
{
    const std::lock_guard<std::mutex> lock(mut);
    if (apclient && apclient->ConnectUpdate(true, items_handling, false, {})) {
        items_handling_changed = false;
        return GM_TRUE;
    }
    return GM_FALSE;
}

double apclient_connect_update(const char* tags_str)
{
    const std::lock_guard<std::mutex> lock(mut);
    json tags_j;
    try {
        tags_j = json::parse(tags_str);
    } catch (std::exception ex) {
        show_error(ex.what());
        return GM_FALSE;
    }

    if (apclient && apclient->ConnectUpdate(items_handling_changed, items_handling, true, tags_j)) {
        items_handling_changed = false;
        return GM_TRUE;
    }
    return GM_FALSE;
}

double apclient_sync()
{
    const std::lock_guard<std::mutex> lock(mut);
    return GM_BOOL(apclient && apclient->Sync());
}

double apclient_status_update(double status)
{
    const std::lock_guard<std::mutex> lock(mut);
    int int_status = (int)status;
    return GM_BOOL(apclient && apclient->StatusUpdate((APClient::ClientStatus)int_status));
}

double apclient_location_checks(const char* locations_str)
{
    const std::lock_guard<std::mutex> lock(mut);
    json locations_j;
    try {
        locations_j = json::parse(locations_str);
    } catch (std::exception ex) {
        show_error(ex.what());
        return GM_FALSE;
    }

    return GM_BOOL(apclient && apclient->LocationChecks(locations_j));
}

double apclient_location_scouts(const char* locations_str, double create_as_hint)
{
    const std::lock_guard<std::mutex> lock(mut);
    json locations_j;
    try {
        locations_j = json::parse(locations_str);
    } catch (std::exception ex) {
        show_error(ex.what());
        return GM_FALSE;
    }

    return GM_BOOL(apclient && apclient->LocationScouts(locations_j, (int)create_as_hint));
}
