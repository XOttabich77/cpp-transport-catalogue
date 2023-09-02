#include "stat_reader.h"

namespace Transport {
	using namespace std;
		
	ostream& operator<<(ostream& os, const Info::BusInfo& info) {
		auto [name, s, u, l, c] = info;
		os << std::setprecision(6);
		os << "Bus "s << name << ": "s << s << " stops on route, "s << u << " unique stops, "s << l << " route length, "s << c << " curvature"s << endl;
		return os;
	}
	ostream& operator<<(ostream& os, const Info::StopInfo& info) {
		cout << ": "s << info.status;
		for (auto bus : info.buses) {
			cout << " "s << bus;
		}
		cout << endl;
		return os;
	}

	void Stat_reader::ReQuest(TransportCatalogue& t_cat, std::istream& input)
	{
		string quantity_req;
		getline(input, quantity_req);
		string line;
		for (int i = 0; i < stoi(quantity_req); ++i) {
			getline(input, line);
			size_t pos = line.find(' ');
			string type = line.substr(0, pos);
			line = line.substr(pos + 1);
			if (type == "Bus") {
				auto info = t_cat.GetBus(line);
				if (!info.name.empty()) {
					cout << info;
				}
				else {
					cout << "Bus "s << line << ": not found"s << endl;
				}
			}
			if (type == "Stop") {
				auto info = t_cat.GetStop(line);
				cout << "Stop " << line << info;
			}
		}

	}
}