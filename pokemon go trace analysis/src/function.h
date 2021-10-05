#ifndef FUNCTION_H
#define FUNCTION_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

using namespace std;

void StateOfPacket(string readline, bool &is_other, bool &is_uplink_PG,
                   bool &is_downlink_PG);

#endif