#include "misc/memory_access.hpp"
#include "misc/typedef.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <unistd.h>

using namespace std;


void write_offsets(vector<string> names, vector<string> offsets, const string& file_name) {
  if(names.size() != offsets.size()){
    cout << "offset and name length not matching" << endl;
  }
  ofstream file(file_name);
  file << "## offsets" << endl;
  for (size_t i = 0; i < names.size(); i++){
    file << names[i] << "=" << offsets[i] << endl;
  }
  file.close();
}

void print_offsets(vector<string> names, vector<string> offsets) {
  cout << "--------- Found Offsets ---------" << endl;
  for (size_t i = 0; i < names.size(); i++){
    cout << names[i] << "=" << offsets[i] << endl;
  }
}

void write_settings(const string& file_name) {
  ofstream file(file_name, ios_base::app);
  file << endl;
  file << "## settings" << endl;
  file << "# general settings" << endl;
  file << "main_loop_sleep=10" << endl;
  file << "debug=false" << endl;
  file << "panic_key=F9" << endl;
  file << "# file for reading mouse clicks" << endl;
  file << "# usually /dev/input/event*" << endl;
  file << "mouse_file=/dev/input/event2" << endl;
  file << "# trigger settings" << endl;
  file << "trigger_delay=10" << endl;
  file << "trigger_use_random=false" << endl;
  file << "# find your key with \"$ dumpkeys -l\"" << endl;
  file << "trigger_key=Caps_Lock" << endl;
  file << "# bhop settings" << endl;
  file << "bhop_key=space" << endl;
  file << "# aimbot settings" << endl;
  file << "aim_key=mouse1" << endl;
  file << "aim_sleep=20" << endl;
  file << "aim_fov=1.9" << endl;
  file << "aim_autoshoot=false" << endl;
  file << "#bone_ids=3,4,5,6,7,8,10,11,12,13,14,39,40,41,42,70,71,72,77,78,79" << endl;
  file << "bone_ids=3,4,5,6,7,8" << endl;
  file << "# enable / disable smoothing on first shot you may want to disable this for perfect flicks" << endl;
  file << "aim_smooth_first_shot=true" << endl;
  file << "smoothing_factor=0.25" << endl;
  file << "smoke_check=true" << endl;
  file << "aim_teammates=false" << endl;
  file << "# your ingame sensitivity (used for aim calculations)" << endl;
  file << "sensitivity=1.3" << endl;
  file << "# radar settings" << endl;
  file << "find_map=true" << endl;
  file << "radar_sleep=100" << endl;
  file << "radar_generic=false" << endl;
  file.close();
}

int main(int argc, char** argv) {
  if (getuid() != 0){
    cout << "Not root" << endl;
    return 0;
  }
  MemoryAccess mem(nullptr);
  mem.getPid();
  Addr_Range clientRange = mem.getClientRange();
  Addr_Range engineRange = mem.getEngineRange();
  cout << hex << "Client size: " << clientRange.second - clientRange.first << endl;
  cout << "Engine size: " << engineRange.second - engineRange.first << endl;
  const char glowPointerCall_data[] = "\xE8\x00\x00\x00\x00\x48\x8b\x3d\x00\x00\x00\x00\xBE\x01\x00\x00\x00\xC7";
  const char glowPointerCall_pattern[] = "x????xxx????xxxxxx";
  const char local_player_addr_data[] = "\x48\x89\xe5\x74\x0e\x48\x8d\x05\x00\x00\x00\x00";
  const char local_player_addr_pattern[] = "xxxxxxxx????";
  const char atk_mov_data[] = "\x89\xd8\x83\xc8\x01\xf6\xc2\x03\x0f\x45\xd8\x44\x89\x00\x83\xe0\x01\xf7\xd8\x83\e8\x03";
  const char atk_mov_pattern[] = "xxxxxxxxxxxxx?xxxxxxxx"; //
  const char map_name_data[] = "\x48\x89\xC7\x44\x89\x9D\xB4\xFE\xFF\xFF\x48\x8D\x35\x44\x98\xB8\x00";
  const char map_name_pattern[] = "xxxxxxxxxxxxx????";
  // const char force_jump_data[] = "\x44\x89\xe8\xc1\xe0\x1d\xc1\xf8\x1f\x83\xe8\x03\x45\x84\xe4\x74\x08\x21\xd0";
  // const char force_jump_pattern[] = "xxxxxxxxxxxxxxxx?xx";
  const char split_screen_data[] = "\x55\x89\xFE\x48\x8D\x3D\x00\x00\x00\x00\x48\x89\xE5\x5D\xE9\xAD\xFF\xFF\xFF";
  const char split_screen_pattern[] = "xxxxxx????xxxxxxxxx";
  const char isConnectedMove_data[] = "\x48\x8b\x05\x00\x00\x00\x00\xC6\x05\x00\x00\x00\x00\x00\x48\x8b\x10";
  const char isConnectedMove_pattern[] = "xxx????x????xxxx";

  vector<string> offsets;
  vector<string> offset_names;
  char offset_buf[64];

  cout << "-- Glow Pointer --" << endl;
  addr_type glowPointerCall = mem.find_pattern(glowPointerCall_data, glowPointerCall_pattern, clientRange);
  addr_type glowFunctionCall = mem.getCallAddress((void*) glowPointerCall);
  // unsigned int glowOffset;
	// mem.read((void*) (glowFunctionCall + 0x10), &glowOffset, sizeof(int));
  // addr_type glowManAddr = glowFunctionCall + 0x10 + glowOffset + 0x4;
  addr_type glowManAddr = mem.getAbsoluteAddress((void*) (glowFunctionCall + 0x9), 3, 7);
  addr_type glow_offset = glowManAddr - clientRange.first;
  // addr_type glow_offset = glowManAddr;
  sprintf(offset_buf, "0x%lx", glow_offset);
  offset_names.push_back("glow_offset");
  offsets.push_back(string(offset_buf));


  cout << "-- Local Player --" << endl;
  addr_type localPlayerFunction = mem.find_pattern(local_player_addr_data, local_player_addr_pattern, clientRange);
  addr_type local_player_addr = mem.getCallAddress((void*) (localPlayerFunction + 0x7));
  addr_type local_player_offset = local_player_addr - clientRange.first;
  sprintf(offset_buf, "0x%lx", local_player_offset);
  offset_names.push_back("local_player_offset");
  offsets.push_back(string(offset_buf));


  cout << "-- Attack --" << endl;
  addr_type attack_addr = mem.find_pattern(atk_mov_data, atk_mov_pattern, clientRange);
  addr_type atk_offset = attack_addr - clientRange.first;
  sprintf(offset_buf, "0x%lx", atk_offset);
  offset_names.push_back("attack_offset");
  offsets.push_back(string(offset_buf));

  cout << "-- Map Name --" << endl;
  addr_type map_name_call = mem.find_pattern(map_name_data, map_name_pattern, engineRange);
  addr_type map_name_addr = mem.getCallAddress((void*) (map_name_call + 0xC));
  // example: "maps/de_dust2.bsp"
  addr_type map_name_offset = map_name_addr - engineRange.first + 0x5; // add 5 because of "maps/"
  sprintf(offset_buf, "0x%lx", map_name_offset);
  offset_names.push_back("map_name_offset");
  offsets.push_back(string(offset_buf));

  // cout << "-- Force Jump --" << endl;
  // addr_type force_jump_call = mem.find_pattern(force_jump_data, force_jump_pattern, clientRange);
  // addr_type force_jump_addr = mem.getCallAddress((void*) (force_jump_call + 0x1A));
  // addr_type force_jump_offset = force_jump_addr - clientRange.first;
  // sprintf(offset_buf, "0x%lx", force_jump_offset);
  // offset_names.push_back("force_jump_offset");
  // offsets.push_back(string(offset_buf));


  cout << "-- is Connected --" << endl;
  addr_type isConnected_call = mem.find_pattern(isConnectedMove_data, isConnectedMove_pattern, engineRange);
  addr_type isConnected_addr = mem.getCallAddress((void*) (isConnected_call + 0x8)) + 1;
  addr_type isConnected_offset = isConnected_addr - engineRange.first;
  sprintf(offset_buf, "0x%lx",isConnected_offset);
  offset_names.push_back("isConnected_offset");
  offsets.push_back(string(offset_buf));


  // cout << "Test1: " << test1 << endl;
  // cout << "Test2: " << test2 << endl;
  // cout << viewAngels << endl;
  write_offsets(offset_names, offsets, "settings.cfg");
  print_offsets(offset_names, offsets);
  write_settings("settings.cfg");
} //
