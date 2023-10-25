#include <iostream>
#include <sstream>
#include "json_reader.h"
#include "svg.h"
#include "map_renderer.h"
#include "request_handler.h"



#include <chrono>
#include <thread>

using namespace std;

int main()
{     
 
    transport::json_reader::Json_Reader test;
    test.LoadJson(cin);

    transport::TransportCatalogue catalog = transport::json_reader::LoadBaseRequest(test);
    json::Print(json::Document(transport::json_reader::DoRequest(catalog, test)),cout);
//    json::Print(json::Document(transport::DoRequest(catalog, test)), cout);
   
 //   using namespace std::this_thread; // sleep_for, sleep_until
 //   using namespace std::chrono; // nanoseconds, system_clock, seconds

 //   sleep_for(seconds(30));
   

}

