#include "serialization.h"
#include "json_builder.h"
#include "graph.h"


namespace transport {

json::Dict MakeRoutingSettings(const travel_base::DB& db){
    using namespace std::literals;
    json::Builder rs;
    rs.StartDict();
    rs.Key("bus_wait_time"s).Value(db.routing_settings().bus_wait_time()).
       Key("bus_velocity"s).Value(db.routing_settings().bus_velocity()).
    EndDict();
    return rs.Build().AsMap();
}

void MakeColor(json::Builder& rs,const travel_base::Color& color){
    if(color.type_case() == travel_base::Color::TypeCase::kColorName){
        rs.Value(color.color_name());
    }else if(color.type_case() == travel_base::Color::TypeCase::kRgb){
        rs.StartArray().Value(static_cast<int>(color.rgb().red())).
            Value(static_cast<int>(color.rgb().green())).
            Value(static_cast<int>(color.rgb().blue())).
            EndArray();

    }else if(color.type_case() == travel_base::Color::TypeCase::kRgba){
        rs.StartArray().Value(static_cast<int>(color.rgba().rgb().red())).
            Value(static_cast<int>(color.rgba().rgb().green())).
            Value(static_cast<int>(color.rgba().rgb().blue())).
            Value(color.rgba().opacity()).
            EndArray();
    }
}

json::Dict MakeRenderSetting(const travel_base::DB& db){
    using namespace std::literals;
    json::Builder rs;
    rs.StartDict();
    rs.Key("width"s).Value(db.render_setting().width()).
        Key("height"s).Value(db.render_setting().height()).
        Key("padding"s).Value(db.render_setting().padding()).
        Key("stop_radius"s).Value(db.render_setting().stop_radius()).
        Key("line_width"s).Value(db.render_setting().line_width()).
        Key("bus_label_font_size"s).Value(static_cast<int>(db.render_setting().bus_label_font_size()));
    rs.Key("bus_label_offset"s).StartArray().
        Value(db.render_setting().bus_label_offset().x()).Value(db.render_setting().bus_label_offset().y()).
        EndArray();
    rs.Key("stop_label_font_size"s).Value(static_cast<int>(db.render_setting().stop_label_font_size()));
    rs.Key("stop_label_offset"s).StartArray().
        Value(db.render_setting().stop_label_offset().x()).Value(db.render_setting().stop_label_offset().y()).
        EndArray();
    rs.Key("underlayer_color");
    MakeColor(rs,db.render_setting().underlayer_color());
    rs.Key("underlayer_width").Value(db.render_setting().underlayer_width());
    rs.Key("color_palette");
    int size_color = db.render_setting().color_palette_size();
    rs.StartArray();
    for (int i = 0; i < size_color; ++i) {
        MakeColor(rs,db.render_setting().color_palette(i));
    }
    rs.EndArray();
    rs.EndDict();
    return rs.Build().AsMap();
}

std::vector<graph::Edge<double>> MakeEdge(const travel_base::DB& db){
    std::vector<graph::Edge<double>> rs;
    size_t size = db.graph().edges_size();
    for(size_t i = 0; i< size; ++i){
        rs.push_back({db.graph().edges(i).from(),
                      db.graph().edges(i).to(),
                      db.graph().edges(i).weight()});
    }
    return rs;
}
std::vector<std::vector<size_t>> MakeIncidenceLists(const travel_base::DB& db){
    std::vector<std::vector<size_t>> rs;
    size_t size_il = db.graph().ilist_size();
    rs.resize(size_il);
    for(size_t i = 0; i < size_il; ++i) {
        size_t size_i = db.graph().ilist(i).incidence_size();
        for (size_t k = 0; k < size_i; ++k) {
            rs[i].push_back(db.graph().ilist(i).incidence(k));
        }
    }
    return rs;
}

void DeSerialization(TransportCatalogue& db,
                     std::vector<graph::Edge<double>>& edges,
                     std::vector<std::vector<size_t>>& incidence_lists,
                     transport::json_reader::Json_Reader& setting)
{
    std::string file_name = setting.GetSerializationSetting().at("file"s).AsString();
    std::ifstream input(file_name,ios::binary);
    travel_base::DB protobuf_db;
    protobuf_db.ParseFromIstream(&input);
    std::unordered_map<size_t,info::Stop*> ptr_stops;

    int size_stops = protobuf_db.stops_size();
    for (int i = 0; i < size_stops; ++i) {
        info::Stop stop;
        stop.name = protobuf_db.stops(i).name();
        stop.coordinate.lat = protobuf_db.stops(i).coordinate().lat();
        stop.coordinate.lng = protobuf_db.stops(i).coordinate().lng();
        db.AddStop(stop);
        ptr_stops[protobuf_db.stops(i).id()] = db.FindStop(stop.name);
    }

    int size_buses = protobuf_db.buses_size();
    for (int i = 0; i < size_buses; ++i) {
        std::string name = protobuf_db.buses(i).name();
        bool circle = protobuf_db.buses(i).circle();
        int size_stops = protobuf_db.buses(i).id_stop_size();
        std::vector<std::string> stops(size_stops);
        for (int k = 0; k < size_stops; ++k) {
            size_t id = protobuf_db.buses(i).id_stop(k);
            std::string stop_name = ptr_stops.at(id)->name;
            stops[k]=stop_name;
        }
        db.AddBus(name, stops, circle);        
    }

    int size_length = protobuf_db.lengths_size();
    for (int i= 0; i< size_length; ++i){
        std::string name_from = ptr_stops.at(protobuf_db.lengths(i).stop_from_id())->name;
        std::string name_to = ptr_stops.at(protobuf_db.lengths(i).stop_to_id())->name;
        double length = protobuf_db.lengths(i).length();
        db.AddLength(name_from, name_to, length);
    }
    edges = MakeEdge(protobuf_db);
    incidence_lists = MakeIncidenceLists(protobuf_db);
    setting.SetRenderSetting(MakeRenderSetting(protobuf_db));
    setting.SetRoutingSettings(MakeRoutingSettings(protobuf_db));
}



Serialization::Serialization(const TransportCatalogue& db,const Graph& graph, const Setting& setting):
    db_(db),
    graph_(graph),
    serialization_settings_(setting.GetSerializationSetting()),
    routing_settings_(setting.GetRoutingSetting()),
    render_settings_(setting.GetRenderSetting())
    {
    }
 void Serialization::Do(){
    SetTarget();
    AddStops();
    AddBuses();
    AddLengths();
    AddRenderSetting();
    AddRoutingSettings();
    AddGraph();
    protobuf_db_.SerializeToOstream(&output_);
    }

 void Serialization::SetTarget(){
    std::string file_name = serialization_settings_.at("file"s).AsString();
    output_.open(file_name,ios::binary);
 }

 void Serialization::AddEdgesGraph(travel_base::Edge* edge, size_t from, size_t to, double weight){
    edge->set_from(from);
    edge->set_to(to);
    edge->set_weight(weight);
 }

 void Serialization::AddGraph(){
    travel_base::Graph graph;
    size_t size = graph_.GetEdgeCount();
    for (int i = 0; i < size; ++i){
        const size_t from = graph_.GetEdge(i).from;
        const size_t to = graph_.GetEdge(i).to;
        const double weight = graph_.GetEdge(i).weight;
        AddEdgesGraph(protobuf_db_.mutable_graph()->add_edges(), from,to,weight);
    }
    travel_base::IncidenceLists* il;
    size = graph_.GetVertexCount();
    for(int i = 0; i < size; ++i){
        il = protobuf_db_.mutable_graph()->add_ilist();
        for(const auto el : graph_.GetIncidentEdges(i)){
            il->add_incidence(el);
        }
    }
 }

 void Serialization::AddRoutingSettings(){
    auto [time,speed] = transport::json_reader::GetRoutSetting(routing_settings_);
    protobuf_db_.mutable_routing_settings()->set_bus_wait_time(time);
    protobuf_db_.mutable_routing_settings()->set_bus_velocity(speed);
 }

 renderer::RenderSetting Serialization::SetRenderSetting(){
    return renderer::SetRenderSetting(render_settings_);
 }

 void Serialization::AddRenderSetting(){
    renderer::RenderSetting setting = SetRenderSetting();
    travel_base::RenderSetting r_setting;
    r_setting.set_width(setting.width);
    r_setting.set_height(setting.height);
    r_setting.set_padding(setting.padding);
    r_setting.set_line_width(setting.line_width);
    r_setting.set_stop_radius(setting.stop_radius); //5
    r_setting.set_bus_label_font_size(setting.bus_label_font_size);
    r_setting.mutable_bus_label_offset()->set_x(setting.bus_label_offset.x); // 7
    r_setting.mutable_bus_label_offset()->set_y(setting.bus_label_offset.y);
    r_setting.set_stop_label_font_size(setting.stop_label_font_size);
    r_setting.mutable_stop_label_offset()->set_x(setting.stop_label_offset.x); // 9
    r_setting.mutable_stop_label_offset()->set_y(setting.stop_label_offset.y);
    AddColor(r_setting.mutable_underlayer_color(),setting.underlayer_color);
    r_setting.set_underlayer_width(setting.underlayer_width);
    for(int i = 0; i < setting.color_palette.size(); ++i){
        AddColor(r_setting.add_color_palette(),setting.color_palette[i]);
    }
    *protobuf_db_.mutable_render_setting() = std::move(r_setting);
 }

 void Serialization::AddColor(travel_base::Color* color,svg::Color set_color){
    if(std::holds_alternative<std::string>(set_color)){
        color->set_color_name(std::get<std::string>(set_color));
    } else if(std::holds_alternative<svg::Rgb>(set_color)){
        svg::Rgb rgb = std::get<svg::Rgb>(set_color);
        color->mutable_rgb()->set_red(rgb.red);
        color->mutable_rgb()->set_blue(rgb.blue);
        color->mutable_rgb()->set_green(rgb.green);
    } else if(std::holds_alternative<svg::Rgba>(set_color)){
        svg::Rgba rgba = std::get<svg::Rgba>(set_color);
        color->mutable_rgba()->mutable_rgb()->set_red(rgba.red);
        color->mutable_rgba()->mutable_rgb()->set_blue(rgba.blue);
        color->mutable_rgba()->mutable_rgb()->set_green(rgba.green);
        color->mutable_rgba()->set_opacity(rgba.opacity);
    }
 }

 void Serialization::AddLengths(){
    for(const auto el:db_.GetLengths()){
        const auto [from,to] = el.first;
        const double l = el.second;
        AddEdge(protobuf_db_.add_lengths(),from,to,l);
    }
 }
 void Serialization::AddEdge(travel_base::Length* edge,const info::Stop* from,const info::Stop* to, const double l){
    edge->set_stop_from_id(reinterpret_cast<size_t>(from));
    edge->set_stop_to_id(reinterpret_cast<size_t>(to));
    edge->set_length(l);
 }

 void Serialization::AddBuses(){
    for(const auto& bus:db_.GetAllBus()){
        AddBus(protobuf_db_.add_buses(),bus);
    }
 }
 void Serialization::AddBus(travel_base::Bus* pbbus, const info::Bus& bus ){
    pbbus->set_name(bus.name);
    for(const auto stop: bus.stops){
        pbbus->add_id_stop(reinterpret_cast<size_t>(stop));
    }
    pbbus->set_circle(bus.circle);
 }

 void  Serialization::AddStops(){
    for(const auto& stop:db_.GetAllStops()){
        AddStop(protobuf_db_.add_stops(),stop);
    }
 }
 void  Serialization::AddStop(travel_base::Stop* pbstop,const info::Stop& stop){
    pbstop->set_name(stop.name);
    {
        travel_base::Coordinate coord;
        coord.set_lat(stop.coordinate.lat);
        coord.set_lng(stop.coordinate.lng);
        *pbstop->mutable_coordinate()= std::move(coord);
    }
    pbstop->set_id(reinterpret_cast<size_t>(db_.FindStop(stop.name)));
 }

 } //end namespace transpor
