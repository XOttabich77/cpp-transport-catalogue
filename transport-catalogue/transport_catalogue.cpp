#include "transport_catalogue.h"


namespace Transport {
	using namespace std;
	void TransportCatalogue::AddStop(Stop stop)
	{
		stops_.push_back(move(stop));
		stopname_to_stop_[stops_.back().name] = &stops_.back();
	}

	void TransportCatalogue::AddBuss(std::string& name, const std::vector<std::string>& stops)
	{
		Bus bus;
		bus.name = move(name);
		for (const auto& stop : stops) {
			bus.stops.push_back(stopname_to_stop_.at(stop));
		}
		buses_.push_back(move(bus));
		busname_to_stop_[buses_.back().name] = &buses_.back();
	}

	void TransportCatalogue::AddLength(const std::string name_from, const std::string& name_to, unsigned l)
	{
		auto key = make_pair(stopname_to_stop_.at(name_from), stopname_to_stop_.at(name_to));
		length_[key] = l;
	}

	TransportCatalogue::Stop TransportCatalogue::FindStop(const std::string& name)
	{
		if (stopname_to_stop_.count(name)) {
			return *stopname_to_stop_.at(name);
		}
		return Stop();
	}

	TransportCatalogue::Bus TransportCatalogue::FindBus(const std::string& name)
	{
		if (busname_to_stop_.count(name)) {
			return *busname_to_stop_.at(name);
		}
		return Bus();
	}

	Info::BusInfo TransportCatalogue::GetBus(const std::string& name)
	{
		Bus bus = FindBus(name);
		if (!bus.name.empty()) {
			//	PrintBus(bus);
			int quantity_stop = static_cast<int>(bus.stops.size());
			int unuque_stop = static_cast<int>(set<const Stop*>(bus.stops.begin(), bus.stops.end()).size());
			double glenght = 0;
			unsigned lenght = 0;
			for (int i = 0; i < quantity_stop - 1; ++i) {
				glenght += ComputeDistance({ bus.stops[i]->latitude,bus.stops[i]->longitude }, { bus.stops[i + 1]->latitude,bus.stops[i + 1]->longitude });
				lenght += GetLength(bus.stops[i], bus.stops[i + 1]);
			}
			return { bus.name, quantity_stop, unuque_stop, lenght, lenght / glenght };
		}
		return {};
	}

	Info::StopInfo TransportCatalogue::GetStop(const std::string& name)
	{
		Info::StopInfo stop_info;

		Stop stop = FindStop(name);
		if (stop.name.empty()) {
			stop_info.status = "not found";
			return stop_info;
		}
		auto stop_adress = stopname_to_stop_.at(stop.name);
		for (auto bus : buses_) {
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

	unsigned TransportCatalogue::GetLength(const Stop* from, const Stop* to)
	{
		if (length_.count({ from,to })) {
			return	length_.at({ from,to });
		}
		return length_.at({ to,from });
	}

	
	void TransportCatalogue::PrintBus(TransportCatalogue::Bus bus) {
		cout << "\n" << bus.name << " " << busname_to_stop_.at(bus.name) << endl;
		for (auto stop : bus.stops) {
			cout << stop->name << " " << stop << endl;
		}
	}

	void TransportCatalogue::ShowAll()
	{
		cout << "STOPS:\n";
		for (auto stop : stops_) {
			cout << stop.name << " " << stopname_to_stop_.at(stop.name) << endl;
		}

		cout << "\nBUS:\n";
		for (auto bus : buses_) {
			PrintBus(bus);
	
		}
		cout << "\nLENGHT\n";
		for (auto l : length_) {
			cout << l.second << endl;
		}
	}

}