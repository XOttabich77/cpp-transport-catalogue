#pragma once
#include <unordered_map>
#include <string_view>
#include <string>
#include <optional>
#include <list>
#include <variant>
#include <memory>
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"


namespace router {

using Time = double;
using Speed = double;

struct RoutingSetting {
    Time bus_wait_time;
    Speed bus_velocity;
    RoutingSetting& SetWaitingTime(Time v) {
        this->bus_wait_time = v;
        return *this;
    }
    RoutingSetting& SetBusSpeed(double v) {
        this->bus_velocity = v;
        return *this;
    }
};
struct Vertex {
    graph::VertexId wait;
    graph::VertexId go;
};
struct BusEdge {
    std::string_view name;
    Time time;
    int stops;
};
struct WaitEdge {
    std::string_view name;
    Time time;
};
using Edge = std::variant< BusEdge, WaitEdge>;
using Points = std::list<Edge>;

class TransportRouter {
public:
    explicit TransportRouter(const transport::TransportCatalogue& db, const Time wait_time, const Speed speed);
    explicit TransportRouter(const transport::TransportCatalogue& db,
                             std::vector<graph::Edge<Time>>& edges,
                             std::vector<std::vector<graph::EdgeId>>& il,
                             const Time wait_time, const Speed speed);
    std::optional <std::pair<Points, Time>> FindRoute(std::string_view from, std::string_view to);
    graph::DirectedWeightedGraph<Time> GetGraph() const { return route_map__;}
    graph::Router<Time>* GetRouter() const { return router_.get(); }


private:
    RoutingSetting routing_setting_;
    const transport::TransportCatalogue& db_;
    graph::DirectedWeightedGraph<Time> route_map__; //+
    std::unique_ptr<graph::Router<Time>> router_;	//+
    std::unordered_map<graph::EdgeId, BusEdge> bus_edge_;
    std::unordered_map<graph::EdgeId, WaitEdge> wait_edge_;
    std::unordered_map<std::string_view, Vertex> stop_id_;
    graph::EdgeId edge_id_ = 0;

    void MakeMapRouter(bool flag = true);
    void MakeWaitEdge(bool flag = true);
    void MakeBusEdge(bool flag = true);
    Time GetTimeFromSpeed(double v) const;
    Edge GetEdge(graph::EdgeId id);
};
}
