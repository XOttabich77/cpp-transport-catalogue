﻿#include <iostream>

#include <sstream>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;



int main()
{
    std::istringstream input{
        "10\n"
        "Stop Tolstopaltsevo: 55.611087, 37.208291\n" //290
        "Stop Marushkino: 55.595884,    37.209755\n"
        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
        "Stop Rasskazovka: 55.632761, 37.333324\n"
        "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700\n"
        "Stop Biryusinka: 55.581065, 37.648390\n"
        "Stop Universam: 55.587655, 37.645687\n"
        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"        
    }; 
    Transport::TransportCatalogue Cat = Transport::Request_reader::LoadRequest(cin);
  //  TransportCatalogue Cat = request_reader::LoadRequest(input);
  //  Cat.ShowAll();
  //  Cat.GetStop("Universam");
   // Cat.GetBus("750");
   // Cat.GetBus("256");
   // Cat.GetBus("255");
 /*   std::istringstream output{
        "3\n"
        "Bus 256\n"
        "Bus 750\n"
        "Bus 751\n"
    };*/
    Transport::Stat_reader::ReQuest(Cat, cin);
    
}
