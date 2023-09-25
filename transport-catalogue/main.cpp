#include <iostream>
#include <sstream>
#include "json_reader.h"
#include "svg.h"
#include "map_renderer.h"

using namespace std;

int main()
{     
 
    transport::json_reader::Json_Reader test;
    test.LoadJson(cin);

    transport::TransportCatalogue catalog = transport::json_reader::LoadBaseRequest(test);
    json::Print(json::Document(transport::json_reader::DoRequest(catalog, test)),cout);

}

