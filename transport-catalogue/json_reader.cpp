#include "json_reader.h"

using namespace std;

void transport::json_reader::Json_Reader::LoadJson(std::istream& input)
{
    json::Document base= json::Load(input);
    base_requests_ = base.GetRoot().AsMap().at("base_requests"s).AsArray();
    stat_requests_ = base.GetRoot().AsMap().at("stat_requests"s).AsArray();
    render_settings_ = base.GetRoot().AsMap().at("render_settings"s).AsMap();;
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
            result.AddLength(name, name_to, length.AsInt());         
        }
    }
   // зашружаем маршруты
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
namespace detail {
   
    json::Array GetAllBuses(const std::vector<std::string>& buses) {
        json::Array result;
        for (const auto& bus : buses) {
            result.push_back(bus);
        }
        return result;
    }

    json::Dict DoStop(const json::Node& el, const transport::TransportCatalogue& catalog) {
        json::Dict node;
        const string name = el.AsMap().at("name"s).AsString();
        node["request_id"s] = el.AsMap().at("id"s).AsInt();
        const transport::info::StopInfo info = catalog.GetStop(name);
        if (info.status == "not found"s) {
            node["error_message"s] = "not found"s;
        }
        else {
            node["buses"s] = detail::GetAllBuses(info.buses);
        }
        return node;
    }

    json::Dict DoBus(const json::Node& el, const transport::TransportCatalogue& catalog) {
        json::Dict node;
        const string name = el.AsMap().at("name"s).AsString();
        const transport::info::BusInfo info = catalog.GetBus(name);
        node["request_id"s] = el.AsMap().at("id"s).AsInt();
        if (info.name.empty()) {
            node["error_message"s] = "not found"s;
        }
        else {
            node["curvature"s] = info.curvature;
            node["route_length"s] = static_cast<int>(info.length);
            node["stop_count"s] = info.stop_on_route;
            node["unique_stop_count"s] = info.unique_stop;
        }
        return node;
    }

    json::Dict DoMap(const json::Node& el, const transport::TransportCatalogue& catalog, const transport::json_reader::Json_Reader& request) {
        json::Dict node;
        node["request_id"s] = el.AsMap().at("id"s).AsInt();
        renderer::MapRenderer map(catalog,  request.GetRenderSetting());
        map.MakeMap();
        std::ostringstream oss;
        map.RenderMap(oss);
        node["map"s] = oss.str();
        return node;
    }
}
json::Array transport::json_reader::DoRequest(const transport::TransportCatalogue& catalog,const Json_Reader& request)
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
    }
    return result;
}
