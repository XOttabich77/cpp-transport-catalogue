#pragma once
#include <map>
#include <fstream>
#include <string.h>
#include <string_view>

#include "transport_catalogue.h"
#include "json.h"
#include "transport_catalogue.pb.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "svg.h"
//#include "transport_router.h"
using namespace std;

namespace transport {

void DeSerialization(TransportCatalogue& db,
                     std::vector<graph::Edge<double>>& edges,
                     std::vector<std::vector<size_t>>& incidence_lists,
                     transport::json_reader::Json_Reader& setting);


class Serialization{
    using Graph = graph::DirectedWeightedGraph<double>;
    using Setting = transport::json_reader::Json_Reader;
public:
    explicit Serialization(const TransportCatalogue& db,const Graph& graph ,const Setting & setting);
    void StartSerilization();

private:
    const TransportCatalogue db_;
    const Graph graph_;
    const json::Dict serialization_settings_;
    const json::Dict routing_settings_;
    const json::Dict render_settings_;
    std::ofstream output_;
    travel_base::DB protobuf_db_;

    void SetTarget();
    void AddEdgesGraph(travel_base::Edge* edge, size_t from, size_t to, double weight);
    void AddGraph();
    void AddRoutingSettings();
    void AddRenderSetting();
    void AddColor(travel_base::Color* color,svg::Color set_color);
    void AddLengths();
    void AddEdge(travel_base::Length* edge,const info::Stop* from,const info::Stop* to, const double l);
    void AddBuses();
    void AddBus(travel_base::Bus* pbbus, const info::Bus& bus );
    void AddStops();
    void AddStop(travel_base::Stop* pbstop,const info::Stop& stop);
    renderer::RenderSetting SetRenderSetting();


};

} //end namespace transport
