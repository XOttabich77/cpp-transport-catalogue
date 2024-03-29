#include "json_reader.h"
#include "json_builder.h"
//#include "router.h"
#include "transport_router.h"
#include <variant>
#include <list>

using namespace std;

void transport::json_reader::Json_Reader::LoadJson(std::istream& input)
{
    json::Document base= json::Load(input);
    if(base.GetRoot().AsMap().count("base_requests"s)){
        base_requests_ = base.GetRoot().AsMap().at("base_requests"s).AsArray();
    }
    if(base.GetRoot().AsMap().count("stat_requests"s)){
        stat_requests_ = base.GetRoot().AsMap().at("stat_requests"s).AsArray();
    }
    if(base.GetRoot().AsMap().count("render_settings"s)){
        render_settings_ = base.GetRoot().AsMap().at("render_settings"s).AsMap();
    }
    if(base.GetRoot().AsMap().count("routing_settings"s)){
        routing_settings_ = base.GetRoot().AsMap().at("routing_settings"s).AsMap();
    }
    if(base.GetRoot().AsMap().count("serialization_settings"s)){
        serialization_settings_ = base.GetRoot().AsMap().at("serialization_settings"s).AsMap();
    }
}

using MapLengths = unordered_map <string, json::Dict>;

transport::TransportCatalogue transport::json_reader::LoadBaseRequest(const Json_Reader& db)
{
    const json::Array& base = db.GetBaseRequest();
    transport::TransportCatalogue result;
    MapLengths lengths;
    // создаем справочник остановок
    for (const auto &el : base) {
        if (el.AsMap().at("type"s).AsString() == "Stop"sv) {
            const string name = el.AsMap().at("name"s).AsString();
            const double latitude =  el.AsMap().at("latitude"s).AsDouble();
            const double longitude = el.AsMap().at("longitude"s).AsDouble();
            lengths[name] = el.AsMap().at("road_distances"s).AsMap();
            result.AddStop({ name, latitude, longitude });
        }
    }
    // загружаем растояния между остановками
    for (const auto & [name, to] : lengths) {
        for (auto & [name_to,length] : to) {
            result.AddLength(name, name_to, length.AsDouble());
        }
    }
    // загружаем маршруты
    for (const auto& el : base) {
        if (el.AsMap().at("type"s).AsString() == "Bus"s) {
            const string name = el.AsMap().at("name"s).AsString();
            const bool circle = el.AsMap().at("is_roundtrip"s).AsBool();
            vector <string> bus_forward;
            for (auto& stop : el.AsMap().at("stops").AsArray()) {
                bus_forward.push_back(stop.AsString());
            }
            if (!circle) {
                vector <string> back_bus = { bus_forward.begin(),prev( bus_forward.end() ) };
                bus_forward.insert( bus_forward.end(), back_bus.rbegin(), back_bus.rend() );
            }
            result.AddBus(name, bus_forward,circle);
        }
    }
    return result;
}

std::pair<router::Time, router::Speed> transport::json_reader::GetRoutSetting(const json::Dict& request)
{
    router::Time time = request.at("bus_wait_time"s).AsDouble();
    router::Speed speed = request.at("bus_velocity"s).AsDouble();
    return { time, speed };
}

namespace detail {

json::Array GetAllBuses(const std::vector<std::string>& buses) {
    json::Array result;
    for (const auto& bus : buses) {
        result.push_back(bus);
    }
    return result;
}

json::Node Error(int id) {
    json::Builder routenode;
    routenode.StartDict().
        Key("request_id"s).Value(id).
        Key("error_message"s).Value("not found"s).
        EndDict();
    return routenode.Build();
}

json::Node DoStop(const json::Node& el, const transport::TransportCatalogue& catalog) {

    json::Builder stopnode;
    const string name = el.AsMap().at("name"s).AsString();
    int id = el.AsMap().at("id"s).AsInt();
    const transport::info::StopInfo info = catalog.GetStop(name);
    if (info.status == "not found"s) {
        return Error(id);
    }
    else {
        stopnode.StartDict()
            .Key("request_id"s).Value(id);
        stopnode.Key("buses"s).Value(detail::GetAllBuses(info.buses));
    }
    stopnode.EndDict();
    return stopnode.Build();
}


json::Node DoBus(const json::Node& el, const transport::TransportCatalogue& catalog) {

    json::Builder busnode;
    const string name = el.AsMap().at("name"s).AsString();
    const transport::info::BusInfo info = catalog.GetBus(name);
    int id = el.AsMap().at("id"s).AsInt();
    if (info.name.empty()) {
        return Error(id);
    }
    else {
        busnode.StartDict()
            .Key("request_id"s).Value(id);
        busnode.Key("curvature"s).Value(info.curvature)
            .Key("route_length"s).Value(info.length)
            .Key("stop_count"s).Value(info.stop_on_route)
            .Key("unique_stop_count"s).Value(info.unique_stop);
    }
    busnode.EndDict();
    return busnode.Build();
}

json::Node DoMap(const json::Node& el, const transport::TransportCatalogue& catalog, const transport::json_reader::Json_Reader& request) {

    json::Builder mapnode;
    mapnode.StartDict()
        .Key("request_id"s).Value(el.AsMap().at("id"s).AsInt());
    renderer::MapRenderer map(catalog, request.GetRenderSetting());
    map.MakeMap();
    std::ostringstream oss;
    map.RenderMap(oss);
    mapnode.Key("map"s).Value(oss.str())
        .EndDict();
    return mapnode.Build();
}

json::Node DoRoute(const json::Node& el,
                   const transport::TransportCatalogue& catalog,
                   std::vector<graph::Edge<double>>& edges,
                   std::vector<std::vector<size_t>>& incidence_lists,
                   const transport::json_reader::Json_Reader& request) {

    auto [time, speed] = transport::json_reader::GetRoutSetting(request.GetRoutingSetting());
    static  router::TransportRouter route(catalog, edges, incidence_lists ,time, speed);
    string from = el.AsMap().at("from"s).AsString();
    string to = el.AsMap().at("to"s).AsString();
    int id = el.AsMap().at("id"s).AsInt();

    auto result(std::move(route.FindRoute(from, to)));
    if (!result.has_value()) {
        return Error(id);
    }

    json::Builder routenode;
    routenode.StartDict().Key("items"s).StartArray();
    for(const auto& el: result.value().first){
        routenode.StartDict();
        if (std::holds_alternative <router::BusEdge> (el)) {
            const router::BusEdge& bus = get< router::BusEdge>(el);
            routenode.Key("bus"s).Value(std::string(bus.name)).
                Key("span_count"s).Value(bus.stops).
                Key("time"s).Value(bus.time).
                Key("type"s).Value("Bus"s);
        }
        else if (std::holds_alternative <router::WaitEdge>(el)) {
            const router::WaitEdge& stop = get<router::WaitEdge>(el);
            routenode.Key("stop_name"s).Value(std::string(stop.name)).
                Key("time"s).Value(stop.time).
                Key("type"s).Value("Wait"s);
        }
        routenode.EndDict();
    }
    routenode.EndArray();
    routenode.Key("total_time"s).Value(result.value().second).
        Key("request_id"s).Value(id).EndDict();
    return routenode.Build();
}
}
json::Array transport::json_reader::DoRequest(const transport::TransportCatalogue& catalog,
                                              std::vector<graph::Edge<double>>& edges,
                                              std::vector<std::vector<size_t>>& incidence_lists,
                                              const Json_Reader& request)
{
    json::Array result;
    for (const auto& el : request.GetStatRequest()) {
        if (el.AsMap().at("type"s).AsString() == "Stop"s) {
            result.push_back(detail::DoStop(el,catalog));
        }
        else if (el.AsMap().at("type"s).AsString() == "Bus"s) {
            result.push_back(detail::DoBus(el,catalog) );
        }
        else if (el.AsMap().at("type"s).AsString() == "Map"s) {
            result.push_back(detail::DoMap(el, catalog, request));
        }
        else if (el.AsMap().at("type"s).AsString() == "Route"s){
            result.push_back(detail::DoRoute(el, catalog, edges, incidence_lists, request));
        }
    }
    return result;
}



