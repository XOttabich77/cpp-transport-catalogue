#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <unordered_map>

#include "transport_catalogue.h"
namespace transport {
	namespace request_reader {
		TransportCatalogue LoadRequest(std::istream& input);
	}
}

