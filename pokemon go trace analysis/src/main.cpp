#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

#include "function.h"

using namespace std;

void help_message() {
  cout << "how to compile: make" << endl;
  cout << "how to run: "
          "./bin/trace_analysis input/<file_name>.txt "
          "output/trace_analysis_<file_name>.csv "
       << endl;
  cout << "how to clean: make clean" << endl;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    help_message();
    return 0;
  }

  // ? append EOF to the end of the file
  // ? I still cannot figure out how std::ios::eof work
  // ? Mine works differently from
  // ?"https://stackoverflow.com/questions/21647/reading-from-text-file-until-eof-repeats-last-line"
  fstream fapp(argv[1], ios::app);
  fapp << "\nEOF";
  fapp.close();

  // open the file for input/output
  fstream fin(argv[1], ios::in);
  fstream fout(argv[2], ios::out);
  fout << "time"
       << ","
       << "# of other packets"
       << ","
       << "# of uplink PG packets"
       << ","
       << "# of downlink PG packets"
       << ","
       << "# of total PG packets"
       << ","
       << "uplink PG BW"
       << ","
       << "downlink PG BW"
       << ","
       << "total PG BW"
       << "\n";

  // var in the output file
  // I think there's no need to separate uplink/downlink of other packets and
  // count their BW
  int time = 0;
  int num_other_packet = 0;
  int num_uplink_PG_packet = 0;
  int num_downlink_PG_pcaket = 0;
  int PG_uplink_BW = 0;
  int PG_downlink_BW = 0;
  int aggregate_num_other_packet = 0;
  int aggregate_num_uplink_PG_packet = 0;
  int aggregate_num_downlink_PG_packet = 0;
  int aggregate_PG_uplink_BW = 0;
  int aggregate_PG_downlink_BW = 0;

  // 3 different states of the packets
  bool is_other = false;
  bool is_uplink_PG = false;
  bool is_downlink_PG = false;

  // tools to find the actual arrival time of the packets
  int packet_time;
  int previous_packet_time = -1;
  string packet_time_str;

  string readline; // read one line for each while loop
  while (getline(fin, readline)) {
    // end of file situation
    if (readline == "EOF" || readline == "") {
      fout << time << "," << num_other_packet << "," << num_uplink_PG_packet
           << "," << num_downlink_PG_pcaket << ","
           << num_uplink_PG_packet + num_downlink_PG_pcaket << ","
           << PG_uplink_BW << "," << PG_downlink_BW << ","
           << PG_uplink_BW + PG_downlink_BW << "\n";
      fout << "aggregate"
           << "," << aggregate_num_other_packet << ","
           << aggregate_num_uplink_PG_packet << ","
           << aggregate_num_downlink_PG_packet << ","
           << aggregate_num_uplink_PG_packet + aggregate_num_downlink_PG_packet
           << "," << aggregate_PG_uplink_BW << "," << aggregate_PG_downlink_BW
           << "," << aggregate_PG_uplink_BW + aggregate_PG_downlink_BW << "\n";
      fout << "average" // remember to turn int to float (or double)
           << "," << (float)aggregate_num_other_packet / (float)time << ","
           << (float)aggregate_num_uplink_PG_packet / (float)time << ","
           << (float)aggregate_num_downlink_PG_packet / (float)time << ","
           << (float)(aggregate_num_uplink_PG_packet +
                      aggregate_num_downlink_PG_packet) /
                  (float)time
           << "," << (float)aggregate_PG_uplink_BW / (float)time << ","
           << (float)aggregate_PG_downlink_BW / (float)time << ","
           << (float)(aggregate_PG_uplink_BW + aggregate_PG_downlink_BW) /
                  (float)time
           << "\n";
      break;
    }

    // tools to obtain the length (size/bytes) of the packet
    size_t pos = 0;
    string space_delimiter = " ";

    // find the state of the packet
    StateOfPacket(readline, is_other, is_uplink_PG, is_downlink_PG);

    // find the arrived time
    // general case: there won't be no packet for more than 1 min
    packet_time_str =
        readline.substr(13, 2); // only slice out second part of the time
    packet_time = stoi(packet_time_str);

    // ** the 1st packet, time has not been set
    if (previous_packet_time == -1) {
      previous_packet_time = packet_time;

      // deal with num and BW of the packet according to 3 different states
      if (is_other == true) {
        num_other_packet++;
        aggregate_num_other_packet++;
      }
      if (is_uplink_PG == true) {
        num_uplink_PG_packet++;
        aggregate_num_uplink_PG_packet++;

        // if this packet has some bytes
        if (readline.find("length") != std::string::npos) {
          // delete all the words except the last one, which is the length of
          // the packet
          while ((pos = readline.find(space_delimiter)) != std::string::npos) {
            readline.erase(0, pos + space_delimiter.length());
          }
          PG_uplink_BW += stoi(readline);
          aggregate_PG_uplink_BW += stoi(readline);
        }
      }
      if (is_downlink_PG == true) {
        num_downlink_PG_pcaket++;
        aggregate_num_downlink_PG_packet++;

        // if this packet has some bytes
        if (readline.find("length") != std::string::npos) {
          // delete all the words except the last one, which is the length of
          // the packet
          while ((pos = readline.find(space_delimiter)) != std::string::npos) {
            readline.erase(0, pos + space_delimiter.length());
          }
          PG_downlink_BW += stoi(readline);
          aggregate_PG_downlink_BW += stoi(readline);
        }
      }
      continue;
    }

    // * for the rest of the packets (other than the 1st one)
    else {
      // * time has elapsed but within 1 sec
      if (previous_packet_time == packet_time) {
        // deal with num and BW of the packet according to 3 different states
        if (is_other == true) {
          num_other_packet++;
          aggregate_num_other_packet++;
        }
        if (is_uplink_PG == true) {
          num_uplink_PG_packet++;
          aggregate_num_uplink_PG_packet++;

          // if this packet has some bytes
          if (readline.find("length") != std::string::npos) {
            // delete all the words except the last one, which is the length of
            // the packet
            while ((pos = readline.find(space_delimiter)) !=
                   std::string::npos) {
              readline.erase(0, pos + space_delimiter.length());
            }
            PG_uplink_BW += stoi(readline);
            aggregate_PG_uplink_BW += stoi(readline);
          }
        }
        if (is_downlink_PG == true) {
          num_downlink_PG_pcaket++;
          aggregate_num_downlink_PG_packet++;

          // if this packet has some bytes
          if (readline.find("length") != std::string::npos) {
            // delete all the words except the last one, which is the length of
            // the packet
            while ((pos = readline.find(space_delimiter)) !=
                   std::string::npos) {
              readline.erase(0, pos + space_delimiter.length());
            }
            PG_downlink_BW += stoi(readline);
            aggregate_PG_downlink_BW += stoi(readline);
          }
        }
        continue;
      }

      // * time has elapsed for more than 1 sec
      if (previous_packet_time < packet_time) {
        // print the previous result first
        fout << time << "," << num_other_packet << "," << num_uplink_PG_packet
             << "," << num_downlink_PG_pcaket << ","
             << num_uplink_PG_packet + num_downlink_PG_pcaket << ","
             << PG_uplink_BW << "," << PG_downlink_BW << ","
             << PG_uplink_BW + PG_downlink_BW << "\n";

        // renew the time
        time += packet_time - previous_packet_time;
        previous_packet_time = packet_time;

        // reset some var
        num_other_packet = 0;
        num_uplink_PG_packet = 0;
        num_downlink_PG_pcaket = 0;
        PG_uplink_BW = 0;
        PG_downlink_BW = 0;

        // deal with num and BW of the packet according to 3 different states
        if (is_other == true) {
          num_other_packet++;
          aggregate_num_other_packet++;
        }
        if (is_uplink_PG == true) {
          num_uplink_PG_packet++;
          aggregate_num_uplink_PG_packet++;

          // if this packet has some bytes
          if (readline.find("length") != std::string::npos) {
            // delete all the words except the last one, which is the length of
            // the packet
            while ((pos = readline.find(space_delimiter)) !=
                   std::string::npos) {
              readline.erase(0, pos + space_delimiter.length());
            }
            PG_uplink_BW += stoi(readline);
            aggregate_num_uplink_PG_packet += stoi(readline);
          }
        }
        if (is_downlink_PG == true) {
          num_downlink_PG_pcaket++;
          aggregate_num_downlink_PG_packet++;

          // if this packet has some bytes
          if (readline.find("length") != std::string::npos) {
            // delete all the words except the last one, which is the length of
            // the packet
            while ((pos = readline.find(space_delimiter)) !=
                   std::string::npos) {
              readline.erase(0, pos + space_delimiter.length());
            }
            PG_downlink_BW += stoi(readline);
            aggregate_PG_downlink_BW += stoi(readline);
          }
        }
        continue;
      }

      // * time has elapsed for more than 1 sec, and cross the 60 sec
      if (previous_packet_time > packet_time) {
        // print the previous result first
        fout << time << "," << num_other_packet << "," << num_uplink_PG_packet
             << "," << num_downlink_PG_pcaket << ","
             << num_uplink_PG_packet + num_downlink_PG_pcaket << ","
             << PG_uplink_BW << "," << PG_downlink_BW << ","
             << PG_uplink_BW + PG_downlink_BW << "\n";

        // renew the time
        time += packet_time + (60 - previous_packet_time);
        previous_packet_time = packet_time;

        // reset some var
        num_other_packet = 0;
        num_uplink_PG_packet = 0;
        num_downlink_PG_pcaket = 0;
        PG_uplink_BW = 0;
        PG_downlink_BW = 0;

        // deal with num and BW of the packet according to 3 different states
        if (is_other == true) {
          num_other_packet++;
          aggregate_num_other_packet++;
        }
        if (is_uplink_PG == true) {
          num_uplink_PG_packet++;
          aggregate_num_uplink_PG_packet++;

          // if this packet has some bytes
          if (readline.find("length") != std::string::npos) {
            // delete all the words except the last one, which is the length of
            // the packet
            while ((pos = readline.find(space_delimiter)) !=
                   std::string::npos) {
              readline.erase(0, pos + space_delimiter.length());
            }
            PG_uplink_BW += stoi(readline);
            aggregate_num_uplink_PG_packet += stoi(readline);
          }
        }
        if (is_downlink_PG == true) {
          num_downlink_PG_pcaket++;
          aggregate_num_downlink_PG_packet++;

          // if this packet has some bytes
          if (readline.find("length") != std::string::npos) {
            // delete all the words except the last one, which is the length of
            // the packet
            while ((pos = readline.find(space_delimiter)) !=
                   std::string::npos) {
              readline.erase(0, pos + space_delimiter.length());
            }
            PG_downlink_BW += stoi(readline);
            aggregate_PG_downlink_BW += stoi(readline);
          }
        }
      }
      continue;
    }
  }
  fin.close();
  fout.close();
  return 0;
}