#include "function.h"

void StateOfPacket(string readline, bool &is_other, bool &is_uplink_PG,
                   bool &is_downlink_PG) {
  if (readline.find("bc.googleusercontent.com.https") == std::string::npos) {
    is_other = true;
    is_uplink_PG = false;
    is_downlink_PG = false;
    return;
  }
  if (readline.find("bc.googleusercontent.com.https:") != std::string::npos) {
    is_other = false;
    is_uplink_PG = true;
    is_downlink_PG = false;
    return;
  }
  if (readline.find("bc.googleusercontent.com.https >") != std::string::npos) {
    is_other = false;
    is_uplink_PG = false;
    is_downlink_PG = true;
    return;
  }
}