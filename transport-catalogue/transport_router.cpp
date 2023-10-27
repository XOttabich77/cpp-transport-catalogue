#include "transport_router.h"


router::TransportRouter::TransportRouter(const transport::TransportCatalogue& db, const Time wait_time, const Speed speed):
	db_(db)	
{
	using namespace std::literals;
	routing_setting_.SetWaitingTime(wait_time).SetBusSpeed(speed);
	MakeMapRouter();
}
std::optional<std::pair<router::Points, router::Time>> router::TransportRouter::FindRoute(std::string_view from, std::string_view to)
{
	std::optional<graph::Router<Time>::RouteInfo> info = router_->BuildRoute(stop_id_.at(from).wait, stop_id_.at(to).wait);
	if (!info.has_value()) {
		return std::nullopt;
	}
	std::list<Edge> points_of_route;
	for (auto i = 0; i < info.value().edges.size(); ++i) {
		graph::EdgeId id = info.value().edges[i];
		points_of_route.push_back(GetEdge(id));
	}	
	return std::make_pair( points_of_route, info.value().weight );
}

void router::TransportRouter::MakeMapRouter()
{
	route_map__ = graph::DirectedWeightedGraph<Time>(db_.GetAllStops().size() * 2);
	MakeWaitEdge();
	MakeBusEdge();
	router_ = std::make_unique<graph::Router<Time>>(route_map__);
}

void router::TransportRouter::MakeWaitEdge()
{
	graph::VertexId count = 0;
	for (auto& stop : db_.GetAllStops())
	{
		graph::EdgeId edge_id = route_map__.AddEdge({ count ,count + 1,routing_setting_.bus_wait_time });
		stop_id_[stop.name].wait = count++;
		stop_id_[stop.name].go = count++;
		wait_edge_[edge_id] = { stop.name, routing_setting_.bus_wait_time };
	}
}

void router::TransportRouter::MakeBusEdge()
{
	for (const auto& bus : db_.GetAllBus()) {
		for (auto it_from = bus.stops.begin(); it_from != prev(bus.stops.end()); ++it_from) {
			Time time_to_stop = 0;
			int stop_count = 0;
			for (auto it_to = next(it_from); it_to != bus.stops.end(); ++it_to) {
				const Time l = GetTimeFromSpeed(db_.GetLength(*(it_to - 1), *it_to)); // time from next to
				time_to_stop += l;
				std::string_view name_from = (*it_from)->name;
				std::string_view name_to = (*it_to)->name;
				graph::VertexId id_from = stop_id_.at(name_from).go;
				graph::VertexId id_to = stop_id_.at(name_to).wait;
				++stop_count;
				graph::EdgeId edge_id = route_map__.AddEdge({ id_from,id_to,time_to_stop });
				bus_edge_[edge_id] = { bus.name, time_to_stop ,stop_count };
			}
		}
	}
}

router::Time router::TransportRouter::GetTimeFromSpeed(double lenght) const
{
	return 60 * lenght / (routing_setting_.bus_velocity * 1000);
}
router::Edge router::TransportRouter::GetEdge(graph::EdgeId id)
{
	router::Edge result;
	if (wait_edge_.find(id) != wait_edge_.end()) {
		result = wait_edge_[id];
	}
	else if (bus_edge_.find(id) != bus_edge_.end()) {
		result = bus_edge_[id];
	}
	return result;
}
