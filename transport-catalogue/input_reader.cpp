#include "input_reader.h"
namespace transport {
	namespace request_reader {
		using namespace std;
		
		using MapLengths = unordered_map <string, string>;

		namespace detail {
			string& Delete_Prefix(string& line, const string& del_char) {
				if (!line.empty()) {
					line = line.substr(line.find_first_not_of(del_char));
					line = line.substr(0, line.find_last_not_of(del_char) + 1);					
				}
				return line;
			}		
						
			void AddStop(TransportCatalogue& result,string & line, MapLengths & lengths) {
				auto pos = line.find(':');
				string name = line.substr(0, pos);
				line = line.substr(pos + 1);
				pos = line.find(',');
				double latitude = stod(line.substr(0, pos));
				line = line.substr(pos + 1);
				pos = line.find(',');
				double longitude;
				if (pos == line.npos) {
					longitude = stod(line); // тут осталась longitude
				}
				else {
					longitude = stod(line.substr(0, pos));
					line = line.substr(pos + 2);
					lengths[name] = line;
				}
				info::Stop stop = { name, latitude,longitude };
				result.AddStop(stop);				
			}

			void AddLength(TransportCatalogue& result, MapLengths lengths) {
				for (auto [name, line] : lengths) {
					size_t pos;
					unsigned length;

					while (true) {
						pos = line.find("m to"s);
						length = stoi(line.substr(0, pos));
						line = line.substr(pos + 5);
						pos = line.find(',');
						if (pos == line.npos) {
							result.AddLength(name, line, length);
							break;
						}
						else {
							string s = line.substr(0, pos);
							result.AddLength(name, s, length);
							line = line.substr(pos + 1);
						}
					}
				}
			}
			
			void AddBuses(TransportCatalogue& result, const vector <string> & buses) {
				for (auto bus : buses) {

					vector <string> back_bus;
					vector <string> bus_forward;

					size_t pos = bus.find(':');
					string name = bus.substr(0, pos);
					//	detail::DelPrx(name, " "s);
					bus = bus.substr(pos + 2);
					size_t end_of_str = bus.npos;

					char c = '>';
					bool circle = true;
					if (bus.find("-") != end_of_str) {
						circle = false;
						c = '-';
					}

					while (true) {
						pos = bus.find(c);
						if (pos == end_of_str) {
							string stopname = bus.substr(0, pos - 1);
							//	detail::DelPrx(stopname, " "s);
							bus_forward.push_back(stopname);
							break;
						}
						else {
							string stopname = bus.substr(0, pos - 1);
							//	detail::DelPrx(stopname, " "s);
							bus = bus.substr(pos + 2);
							bus_forward.push_back(stopname);
							if (!circle) {
								back_bus.push_back(stopname);
							}
						}
					}
					// добавить обратный маршрут если circle false
					if (!circle) {
						bus_forward.insert(bus_forward.end(), back_bus.rbegin(), back_bus.rend());
					}

					result.AddBus(name, bus_forward);
				}
			}

		}


		TransportCatalogue LoadRequest(std::istream& input)
		{
			TransportCatalogue result;
			vector <string> buses;
			MapLengths lengths;

			string quantity_req;
			getline(input, quantity_req);
			string line;
			for (int i = 0; i < stoi(quantity_req); ++i) {
				getline(input, line);
				size_t pos = line.find(' ');
				string type = line.substr(0, pos);
				line = line.substr(pos + 1);

				if (type == "Stop") {
					detail::AddStop(result, line, lengths);
					
				}

				if (type == "Bus") {
					buses.push_back(line);
				}
			}

			// заполнить растояния 
			detail::AddLength(result, lengths);

			// заполнить маршруты
			detail::AddBuses(result, buses);
			
			return result;
		}
	}
}