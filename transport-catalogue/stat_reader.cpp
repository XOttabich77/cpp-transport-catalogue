#include "stat_reader.h"

namespace transport {
	using namespace std;
	
	namespace detail {
		ostream& operator<<(ostream& os, const info::BusInfo& info) {			
			os << std::setprecision(6);
			os << "Bus "s << info.name << ": "s << info.stop_on_route << " stops on route, "s << info.unique_stop << " unique stops, "s << info.length << " route length, "s << info.curvature << " curvature"s << endl;
			return os;
		}
		ostream& operator<<(ostream& os, const info::StopInfo& info) {
			os << ": "s << info.status;
			for (auto bus : info.buses) {
				os << " "s << bus;
			}
			os << endl;
			return os;
		}
		ostream& Print_Bus(ostream& os, const info::BusInfo & info, const string & line) {
			if (!info.name.empty()) {
				os << info;
			}
			else {
				os << "Bus "s << line << ": not found"s << endl;
			}
			return os;
		}
		ostream& Print_Stop(ostream& os, const info::StopInfo& info,const string & line) {
			os << "Stop " << line << info;
			return os;
		}
	}

	void stat_reader::ReQuest(TransportCatalogue& t_cat, std::istream& input)
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
				detail::Print_Bus(cout,t_cat.GetBus(line), line);
			}
			if (type == "Stop") {
				detail::Print_Stop(cout,t_cat.GetStop(line),line);
				
			}
		}

	}
}