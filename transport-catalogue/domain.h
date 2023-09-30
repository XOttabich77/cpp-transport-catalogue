#pragma once
#include <string>
//#include<deque>
//#include<unordered_map>
//#include<string_view>
//#include<iostream>
//#include<set>
#include<vector>
//#include<algorithm>

#include"geo.h"

namespace transport {

	namespace info {
		struct BusInfo
		{
			std::string name;
			int stop_on_route;
			int unique_stop;			
			double length;
			double curvature;
		};
		struct StopInfo {
			std::string status;
			std::vector<std::string> buses;
		};
		struct Stop
		{
			std::string name;
			geo::Coordinates coordinate;
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
}
