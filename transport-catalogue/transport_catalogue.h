	#pragma once
	#include <string>
	#include<deque>
	#include<unordered_map>
	#include<string_view>
	#include<iostream>
	#include<set>
	#include<vector>
	#include<algorithm>

	#include"geo.h"

namespace Transport {
	
	namespace Info {
		struct BusInfo
		{
			std::string name;
			int stop_on_route;
			int unique_stop;
			unsigned length;
			double curvature;
		};
		struct StopInfo {
			std::string status;
			std::vector<std::string> buses;
		};
	}

	namespace Hasher {
		class StopHasher {
		public:
			size_t operator()(const std::pair<const void*, const void*> hash) const {
				size_t h1 = hasher_(hash.first);
				size_t h2 = hasher_(hash.second);
				return h1 * 37 + h2;
			}
		private:
			std::hash<const void*> hasher_;
		};
	}


	class TransportCatalogue
	{
	private:
		struct Stop
		{
			std::string name;
			double latitude;
			double longitude;

		};
		struct Bus
		{
			std::string name;
			std::vector<const Stop*> stops;
		};


	public:

		void AddStop(Stop stop);
		void AddBuss(std::string& name, const std::vector<std::string>& stops);
		void AddLength(const std::string name_from, const std::string& name_to, unsigned l);
		Stop FindStop(const std::string& name);
		Bus  FindBus(const std::string& name);
		Info::BusInfo GetBus(const std::string& name);
		Info::StopInfo GetStop(const std::string& name);
		unsigned GetLength(const Stop* from, const Stop* to);

		void ShowAll(); // для проверки

	private:

		std::deque <Stop> stops_;
		std::unordered_map <std::string_view, const Stop*> stopname_to_stop_;

		std::deque<Bus> buses_;
		std::unordered_map<std::string_view, const Bus*> busname_to_stop_;
		using KeyForMap = std::pair<const Stop*, const Stop*>;
		std::unordered_map<KeyForMap, unsigned, Hasher::StopHasher > length_;

		void PrintBus(Bus bus);
	};

}