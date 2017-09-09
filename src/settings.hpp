#pragma once
#include "typedef.hpp"

#include <string>
#include <X11/Xlib.h>

using namespace std;
class Settings {
private:
  static Settings* instance;

public:
  // offsets
  addr_type glow_offset;
  addr_type attack_offset;
  addr_type local_player_offset;
  addr_type map_name_offset;
  addr_type force_jump_offset;

  // main settings
  long int main_loop_sleep;
  bool debug = false;

  // trigger settings
  long int trigger_delay;
  bool trigger_use_random = false;
  KeySym trigger_key;

  //bhop settings
  KeySym bhop_key;

  //radar settings
  bool find_map = false;

  //aimbot settings
  float aim_fov;
  long int aim_sleep;
  KeySym aim_key;
  float smoothing_factor = 0.2;

  Settings(const string&);
  static Settings getInstance();
  void load(const string&);
  void print();
};
