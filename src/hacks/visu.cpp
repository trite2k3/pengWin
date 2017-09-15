#include "visu.hpp"

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;
Visu::Visu(GameManager& csgo) : csgo(csgo),
                                settings(Settings::getInstance()){}

Visu::~Visu() {
}

void Visu::start(const string& map_name) {
  string cmd = "python visu.py " + map_name + " " + to_string(settings.radar_sleep);
  handle = popen(cmd.c_str(), "w");
  run = true;
  writeLocations = boost::thread(boost::bind(&Visu::writeFunc, this));
}

void Visu::stop(){
  run = false;
  writeLocations.join();
  fputs("quit\n", handle);
  fflush(handle);
  pclose(handle);
}

void Visu::writeFunc() {
  while (run) {
    this_thread::sleep_for(chrono::milliseconds(settings.radar_sleep));
    const char SEPERATOR[] = "|";
    vector<EntityType*> players = csgo.getPlayers();
    if (players.empty())
      continue;
    // print local player index into array[0,0]
    string first_line = to_string(csgo.getLocalPlayerIndex()) + ",0,0,0,0,0";
    fputs(first_line.c_str(), handle);
    // Format: number,hp,team,x,y,z
    int i = 0;
    for (EntityType* player : players) {
      fputs(SEPERATOR, handle);
      ostringstream os;
      os << i << ",";
      os << player->m_iHealth << ",";
      os << player->m_iTeamNum << ",";
      os << player->m_vecOrigin.x << ",";
      os << player->m_vecOrigin.y << ",";
      os << player->m_vecOrigin.z;
      fputs(os.str().c_str(), handle);
      i++;
    }
    fputs("\n", handle);
    fflush(handle);
  }
}
