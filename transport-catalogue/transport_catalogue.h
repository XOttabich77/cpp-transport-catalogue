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
	#include "domain.h"

namespace transport {

	class TransportCatalogue
	{
	private:
		
		struct Bus
		{
			std::string name;
			std::vector<const info::Stop*> stops;
			bool circle;
		};


	public:

		void AddStop(const info::Stop& stop);
		void AddBus(const std::string& name, const std::vector<std::string>& stops, bool circle);
		void AddLength(const std::string& name_from, const std::string& name_to, size_t length);
		
		info::Stop* FindStop(const std::string_view name) const;
		Bus*  FindBus(const std::string_view name) const;
		info::BusInfo GetBus(const std::string& name) const;
		info::StopInfo GetStop(const std::string& name) const ;

		const std::deque<Bus>& GetAllBus() const {
			return buses_;
		}
		bool IsBusCircle(const std::string& name) const;
		

	private:

		std::deque <info::Stop> stops_;
		std::unordered_map <std::string_view, info::Stop*> stopname_to_stop_;

		std::deque<Bus> buses_;
		std::unordered_map<std::string_view, Bus*> busname_to_stop_;
		using KeyForMap = std::pair<const info::Stop*, const info::Stop*>;
		std::unordered_map<KeyForMap, size_t, hasher::StopHasher > length_;

		size_t GetLength(const info::Stop* from, const info::Stop* to) const;
		
	};

}