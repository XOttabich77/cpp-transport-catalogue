#include "transport_catalogue.h"


namespace transport {
	using namespace std;
	void TransportCatalogue::AddStop(const info::Stop& stop)
	{
		stops_.push_back(move(stop));
		stopname_to_stop_[stops_.back().name] = &stops_.back();
	}

	void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stops, bool circle)
	{
		Bus bus;
		bus.name = move(name);
		bus.circle = circle;
		for (const auto& stop : stops) {
			bus.stops.push_back(stopname_to_stop_.at(stop));
		}
		buses_.push_back(move(bus));
		busname_to_stop_[buses_.back().name] = &buses_.back();
	}

	void TransportCatalogue::AddLength(const std::string& name_from, const std::string& name_to, size_t length)
	{
		auto key = make_pair(stopname_to_stop_.at(name_from), stopname_to_stop_.at(name_to));
		length_[key] = length;
	}

	info::Stop* TransportCatalogue::FindStop(const std::string_view name) const  
	{
		if (stopname_to_stop_.count(name)) {
			return  (stopname_to_stop_.at(name));
		}
		return nullptr;
	}

	TransportCatalogue::Bus* TransportCatalogue::FindBus(const std::string_view name) const
	{
		if (busname_to_stop_.count(name)) {
			return busname_to_stop_.at(name);
		}
		return nullptr;
	}

	info::BusInfo TransportCatalogue::GetBus(const std::string& name) const
	{
		const Bus* bus = FindBus(name);
		
		if (bus!=nullptr) {	
			int quantity_stop = static_cast<int>(bus->stops.size());
			int unuque_stop = static_cast<int>(set<const info::Stop*>(bus->stops.begin(), bus->stops.end()).size());
			double glenght = 0;
			size_t lenght = 0;
			for (int i = 0; i < quantity_stop - 1; ++i) {
				glenght += ComputeDistance( bus->stops[i]->coordinate , bus->stops[i + 1]->coordinate);
				lenght += GetLength(bus->stops[i], bus->stops[i + 1]);
			}
			return { bus->name, quantity_stop, unuque_stop, lenght, lenght / glenght };
		}
		return {};
	}

	info::StopInfo TransportCatalogue::GetStop(const std::string& name) const
	{
		info::StopInfo stop_info;
		
		info::Stop* stop = FindStop(name);
		if (stop==nullptr) {
			stop_info.status = "not found";
			return stop_info;
		}
		auto stop_adress = stopname_to_stop_.at(stop->name);
		for (auto& bus : buses_) {
			if (count(bus.stops.begin(), bus.stops.end(), stop_adress)) {
				stop_info.buses.push_back(bus.name);
			}
		}
		if (stop_info.buses.empty()) {
			stop_info.status = "no buses";
		}
		else {
			stop_info.status = "buses";
			sort(stop_info.buses.begin(), stop_info.buses.end());
		}
		return stop_info;

	}

	bool TransportCatalogue::IsBusCircle(const std::string& name) const
	{
		const Bus* bus = FindBus(name);
		return bus->circle;
	}

	size_t TransportCatalogue::GetLength(const info::Stop* from, const info::Stop* to) const
	{
		if (length_.count({ from,to })) {
			return	length_.at({ from,to });
		}
		return length_.at({ to,from });
	}
	

}