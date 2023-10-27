#pragma once
#include <unordered_map>
#include <string_view>
#include <string>
#include <optional>
#include <list>
#include <variant>

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
		TransportRouter(const transport::TransportCatalogue& db, const Time wait_time, const Speed speed);
		std::optional <std::pair<Points, Time>> FindRoute(std::string_view from, std::string_view to);

	private:
		RoutingSetting routing_setting_;
		const transport::TransportCatalogue& db_;	
		graph::DirectedWeightedGraph<Time> route_map__;
		std::unique_ptr<graph::Router<Time>> router_;				
		std::unordered_map<graph::EdgeId, BusEdge> bus_edge_;		
		std::unordered_map<graph::EdgeId, WaitEdge> wait_edge_;	
		std::unordered_map<std::string_view, Vertex> stop_id_;

		void MakeMapRouter();
		void MakeWaitEdge();
		void MakeBusEdge();
		Time GetTimeFromSpeed(double v) const;
		Edge GetEdge(graph::EdgeId id);
	};
}
