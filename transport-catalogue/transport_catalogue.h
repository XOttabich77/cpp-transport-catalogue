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

namespace transport {
	
	namespace info {
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
		struct Stop
		{
			std::string name;
			Coordinates coordinate;
		};
	}

	namespace hasher {
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
		
		struct Bus
		{
			std::string name;
			std::vector<const info::Stop*> stops;
		};


	public:

		void AddStop(const info::Stop& stop);
		void AddBus(const std::string& name, const std::vector<std::string>& stops);
		void AddLength(const std::string& name_from, const std::string& name_to, unsigned length);
		// из ревью - "вообще лучше использовать int, расстояние может быть и отрицательным"
		// из задания - Все Di — "целые __________положительные______ числа, каждое из которых не превышает 1 000 000"  

		info::Stop* FindStop(const std::string_view name);
		Bus*  FindBus(const std::string_view name);
		info::BusInfo GetBus(const std::string& name);
		info::StopInfo GetStop(const std::string& name);
		

	private:

		std::deque <info::Stop> stops_;
		std::unordered_map <std::string_view, info::Stop*> stopname_to_stop_;

		std::deque<Bus> buses_;
		std::unordered_map<std::string_view, Bus*> busname_to_stop_;
		using KeyForMap = std::pair<const info::Stop*, const info::Stop*>;
		std::unordered_map<KeyForMap, unsigned, hasher::StopHasher > length_;

		unsigned GetLength(const info::Stop* from, const info::Stop* to);
		
	};

}