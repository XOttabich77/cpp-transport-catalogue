#include <iostream>

#include <sstream>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;



int main()
{     
    transport::TransportCatalogue catalog = transport::request_reader::LoadRequest(cin);
    transport::stat_reader::ReQuest(catalog, cin);
    
}

