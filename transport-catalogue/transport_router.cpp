#include "transport_router.h"


router::TransportRouter::TransportRouter(const transport::TransportCatalogue& db, const json::Dict& setting):
	db_(db)
{
	routing_setting_.SetWaitingTime(setting.at("bus_wait_time"s).AsDouble()).
		SetBusSpeed(setting.at("bus_velocity"s).AsDouble());
	MakeMapRouter();
}

std::optional<json::Node> router::TransportRouter::FindRouteAsArray(std::string_view from, std::string_view to, int id_request)
{
	std::optional<graph::Router<Time>::RouteInfo> info = router_->BuildRoute(stop_id_.at(from).wait, stop_id_.at(to).wait);
	if (!info.has_value()) {
		return std::nullopt;
	}
	
	json::Builder route;
	route.StartDict().Key("items"s);
	json::Array point_of_route;
	for (auto i = 0; i < info.value().edges.size(); ++i) {
		graph::EdgeId id = info.value().edges[i];
		point_of_route.push_back(GetEdgesAsMap(id));

	}
	route.Value(std::move(point_of_route));
	route.Key("total_time"s).Value(info.value().weight).
		Key("request_id").Value(id_request).EndDict();
	return route.Build();
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
	for (const Stop& stop : db_.GetAllStops())
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

router::Time router::TransportRouter::GetTimeFromSpeed(double v) const
{
	return 60 * v / (routing_setting_.bus_velocity * 1000);
}

json::Node router::TransportRouter::GetEdgesAsMap(graph::EdgeId id)
{
	json::Builder el;
	el.StartDict();
	//	std::cout << "ID :: " << id << std::endl;
	if (wait_edge_.find(id) != wait_edge_.end()) {
		//	std::cout << "WAIT ::" << wait_edge_.at(id).name << " minut : " << wait_edge_.at(id).time << std::endl;
		el.Key("stop_name"s).Value(std::string(wait_edge_.at(id).name)).
			Key("time"s).Value(wait_edge_.at(id).time).
			Key("type"s).Value("Wait"s);

	}
	if (bus_edge_.find(id) != bus_edge_.end()) {
		//	std::cout << "BUS ::" << bus_edge_.at(id).name << " minut : " << bus_edge_.at(id).time << " Spin : " << bus_edge_.at(id).stops << std::endl;
		el.Key("bus"s).Value(std::string(bus_edge_.at(id).name)).
			Key("span_count"s).Value(bus_edge_.at(id).stops).
			Key("time"s).Value(bus_edge_.at(id).time).
			Key("type"s).Value("Bus"s);

	}
	el.EndDict();
	return el.Build();
}
