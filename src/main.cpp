#include "memory_access.hpp"
#include "typedef.hpp"
#include "manager.hpp"
#include "visu.hpp"
#include "trigger.hpp"
#include "aimer.hpp"
#include "settings.hpp"

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv) {
  if (getuid() != 0){
    cout << "Not root" << endl;
    return 0;
  }
  bool use_radar = false;
  bool use_trigger = false;
  bool use_aimbot = false;
  bool debug = false;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-radar")) {
      cout << "Enabled: Radar" << endl;
      use_radar = true;
    }
    else if (!strcmp(argv[i], "-trigger")) {
      cout << "Enabled: Trigger" << endl;
      use_trigger = true;
    }
    else if (!strcmp(argv[i], "-debug")) {
      debug = true;
      cout << "Enabled: Debugging" << endl;
    }
    else if (!strcmp(argv[i], "-aimbot")) {
      use_aimbot = true;
      cout << "Enabled: Aimbot" << endl;
    }
  }

  Settings settings("settings.cfg");
  if (debug) {
    cout << "Settings loaded:" << endl;
    cout << hex << "glow offset: " << settings.glow_offset << endl;
    cout << "attack offset: " << settings.attack_offset << endl;
    cout << "local player offset: " << settings.local_player_offset << endl;
  }
  MemoryAccess mem(&settings);
  GameManager csgo = GameManager(mem);

  if (use_radar) {
    string map_name = "";
    if (debug) cout << "Scanning for map..." << endl;
    while (map_name == "") {
      map_name = mem.getMapName();
      this_thread::sleep_for(chrono::milliseconds(300));
    }
    cout << "Found Map: " << map_name << endl;
    Visu visu(map_name);
    visu.start();
  }

  Trigger trigger(csgo);
  Aimer aimer(csgo);
  while (true) {
    csgo.grabPlayers();
    if (use_trigger)
      trigger.triggerCheck();
    if (debug) {
      csgo.printPlayers();
      // csgo.printEntities();
    }
    if (use_radar)
      csgo.printPlayerLocationsToFile("/tmp/locs.csv");
    this_thread::sleep_for(chrono::milliseconds(settings.main_loop_sleep));
    if(use_aimbot) {
      vector<EntityType*> players = csgo.getPlayers();
      aimer.xSetAim(players[1]);
    }
  }
  return 0;
}
