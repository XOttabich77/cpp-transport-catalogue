#pragma once
#include<iostream>
#include <iomanip>

#include"transport_catalogue.h"
namespace Transport {
	namespace Stat_reader {
		void ReQuest(TransportCatalogue& Tcat, std::istream& input);
	}
}
