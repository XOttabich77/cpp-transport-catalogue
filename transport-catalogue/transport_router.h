#pragma once
#include <unordered_map>
#include <string_view>
#include <string>
//#include <iostream>
#include <optional>

#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"
#include "json_builder.h"
#include "json.h"
#include "domain.h"

namespace router {
	using namespace std::string_literals;
	using namespace transport::info;
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
	
	class TransportRouter {
		
	public:
		TransportRouter(const transport::TransportCatalogue& db, const json::Dict& setting);
		std::optional<json::Node> FindRouteAsArray(std::string_view from, std::string_view to, int id_request);		
	
	private:
		RoutingSetting routing_setting_;
		const transport::TransportCatalogue& db_;	
		graph::DirectedWeightedGraph<Time> route_map__;
		std::unique_ptr<graph::Router<Time>> router_;
		
		struct BusEdge {
			std::string_view name;
			Time time;
			int stops;
		};
		std::unordered_map<graph::EdgeId, BusEdge> bus_edge_;
		
		struct WaitEdge {
			std::string_view name;
			Time time;
		};
		std::unordered_map<graph::EdgeId, WaitEdge> wait_edge_;	

		std::unordered_map<std::string_view, Vertex> stop_id_;

		void MakeMapRouter();
		void MakeWaitEdge();
		void MakeBusEdge();
		Time GetTimeFromSpeed(double v) const;
		json::Node GetEdgesAsMap(graph::EdgeId id);



	};
}
