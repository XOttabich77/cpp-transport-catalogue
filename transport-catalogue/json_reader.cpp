#include "json_reader.h"
#include "json_builder.h"

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
    // ������� ���������� ���������          
    for (const auto &el : base) {
       if (el.AsMap().at("type"s).AsString() == "Stop"sv) {
            const string name = el.AsMap().at("name"s).AsString();
            const double latitude =  el.AsMap().at("latitude"s).AsDouble();
            const double longitude = el.AsMap().at("longitude"s).AsDouble();
            lengths[name] = el.AsMap().at("road_distances"s).AsMap();
            result.AddStop({ name, latitude, longitude });
        }
    }    
   // ��������� ��������� ����� �����������
    for (const auto & [name, to] : lengths) {
        for (auto & [name_to,length] : to) {
            result.AddLength(name, name_to, length.AsDouble());         
        }
    }
   // ��������� ��������
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

    json::Node DoStop(const json::Node& el, const transport::TransportCatalogue& catalog) {
       
        json::Builder stopnode;
        const string name = el.AsMap().at("name"s).AsString();
        stopnode.StartDict()
            .Key("request_id"s).Value(el.AsMap().at("id"s).AsInt());
        const transport::info::StopInfo info = catalog.GetStop(name);
        if (info.status == "not found"s) {
            stopnode.Key("error_message"s).Value("not found"s);
        }
        else {
            stopnode.Key("buses"s).Value(detail::GetAllBuses(info.buses));
        }
        stopnode.EndDict();
        return stopnode.Build();
    }

   
    json::Node DoBus(const json::Node& el, const transport::TransportCatalogue& catalog) {
      
        json::Builder busnode;
        const string name = el.AsMap().at("name"s).AsString();
        const transport::info::BusInfo info = catalog.GetBus(name);
        busnode.StartDict()
            .Key("request_id"s).Value(el.AsMap().at("id"s).AsInt());
        if (info.name.empty()) {
            busnode.Key("error_message"s).Value("not found"s);
        }
        else {
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
