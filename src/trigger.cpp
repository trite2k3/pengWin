#include "trigger.hpp"
#include "memory_access.hpp"
#include "manager.hpp"
#include "clicker.hpp"
#include "typedef.hpp"
#include "settings.hpp"

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <X11/keysymdef.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <boost/thread.hpp>

using namespace std;

Trigger::Trigger(GameManager& csgo) : csgo(csgo),
                                      mem(csgo.getMemoryAccess()),
                                      clicker(Clicker(csgo.getMemoryAccess())),
                                      settings(Settings::getInstance()){}

Trigger::~Trigger() {}

void Trigger::triggerCheck() {
  Team ownTeam = csgo.getTeam();
  unsigned int crosshairTarget = csgo.getCrosshairTarget();
  // if (settings.debug) cout << "crosshairTarget: " << crosshairTarget << endl;
  if (!crosshairTarget)
    return;

  normal_distribution<double> distrib((double) settings.trigger_delay, (double) settings.trigger_delay / 2);
  std::default_random_engine gen;
  vector<EntityType*>& players = csgo.getPlayers();
  if (players.size() < 2)
    return;
  for (EntityType* player : players) {
    if (player->m_iEntityId == crosshairTarget && player->m_iTeamNum != ownTeam){
      if (settings.trigger_use_random) {
        long int wait_time = (long) distrib(gen);
        while (wait_time < 0)
          wait_time = (long) distrib(gen);
        this_thread::sleep_for(chrono::milliseconds(wait_time));
      }
      else {
        this_thread::sleep_for(chrono::milliseconds(settings.trigger_delay));
      }
      // clicker.memClick();
      if (settings.debug) cout << "fired shot" << endl;
      clicker.xClick();
    }
  }
    this_thread::sleep_for(chrono::milliseconds(1));
  if (settings.debug) cout << "triggerCheck ended..." << endl;
}
