#include "memory_access.hpp"
#include "typedef.hpp"
#include "manager.hpp"
#include "visu.hpp"
#include "trigger.hpp"
#include "aimer.hpp"
#include "settings.hpp"
#include "bunnyhop.hpp"
#include "hotkey.hpp"

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
  bool use_bhop = false;
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
    else if (!strcmp(argv[i], "-bhop")) {
      use_bhop = true;
      cout << "Enabled: Bunnyhop" << endl;
    }
  }

  Settings settings("settings.cfg");
  debug = debug || settings.debug;
  settings.debug = debug;
  if (debug) {
    settings.print();
  }
  MemoryAccess mem(&settings);
  GameManager csgo = GameManager(mem);


  Trigger trigger(csgo);
  Aimer aimer(csgo);
  BunnyHopper bhopper(csgo);
  Visu visu;
  HotkeyManager hotkeyMan(csgo);

  while (csgo.gameRunning()) {
    if (debug) cout << "Waiting until connected..." << endl;
    while (!csgo.isOnServer()) {
      if (!csgo.gameRunning())
        break;
      this_thread::sleep_for(chrono::milliseconds(3000));
    }
    if (!csgo.gameRunning())
      break;

    if (debug) cout << "Connected to a server..." << endl;

    if (use_radar) {
      string map_name = "";
      if (settings.find_map) {
        if (debug) cout << "Scanning for map..." << endl;
        while (map_name == "") {
          map_name = csgo.getMapName();
          this_thread::sleep_for(chrono::milliseconds(1000));
        }
      } else {
        cout << "Map detection deactivated. Please choose map:" << endl;
        cin >> map_name;
      }
      cout << "Found Map: " << map_name << endl;
      visu.start(map_name);
    }

    if (use_bhop) {
      boost::function<void()> bhopFunc = boost::bind(&BunnyHopper::jumpCheck, &bhopper);
      hotkeyMan.bind(settings.bhop_key, bhopFunc);
    }
    if (use_trigger){
      boost::function<void()> triggerFunc = boost::bind(&Trigger::triggerCheck, &trigger);
      hotkeyMan.bind(settings.trigger_key, triggerFunc);
    }
    if (use_aimbot) {
      boost::function<void()> aimFunc = boost::bind(&Aimer::aimCheck, &aimer);
      hotkeyMan.bind(settings.aim_key, aimFunc);
    }
    hotkeyMan.startListen();

    // main loop
    while (csgo.isOnServer()) {
      csgo.grabPlayers();
      if (debug) {
        // csgo.printPlayers();
      }
      if (use_radar)
        csgo.printPlayerLocationsToFile("/tmp/locs.csv");
      this_thread::sleep_for(chrono::milliseconds(settings.main_loop_sleep));
    }
    if (use_radar) visu.stop();
    if (debug) cout << "Not on a server. Entering sleep mode..." << endl;
  }

  if (debug) cout << "Game closed. Terminating..." << endl;
  return 0;
}
