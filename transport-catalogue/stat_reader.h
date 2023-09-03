#pragma once
#include<iostream>
#include <iomanip>

#include"transport_catalogue.h"
namespace transport {
	namespace stat_reader {
		void ReQuest(TransportCatalogue& Tcat, std::istream& input);
	}
}
