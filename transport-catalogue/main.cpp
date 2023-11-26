#include <fstream>
#include <iostream>
#include <string_view>

#include <sstream>
#include "json_reader.h"
#include "request_handler.h"
#include "serialization.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
	// make base here
	transport::json_reader::Json_Reader test;
        test.LoadJson(std::cin);
        transport::TransportCatalogue catalog = transport::json_reader::LoadBaseRequest(test);        
        auto [time,speed] = transport::json_reader::GetRoutSetting(test.GetRoutingSetting());
        router::TransportRouter route(catalog, time, speed );
        transport::Serialization out(catalog, route.GetGraph(), test);
        out.Do();        

    } else if (mode == "process_requests"sv) {
        // process requests here
        transport::json_reader::Json_Reader test;        
        test.LoadJson(std::cin);   
        transport::TransportCatalogue catalog;
        std::vector<graph::Edge<double>> edges;
        std::vector<std::vector<size_t>> incidence_lists;
        transport::DeSerialization(catalog, edges, incidence_lists, test);
        json::Print(json::Document(transport::json_reader::DoRequest(catalog,edges, incidence_lists, test)),std::cout);
    } else {
        PrintUsage();
        return 1;
    }
}
