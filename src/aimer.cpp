#include "typedef.hpp"
#include "aimer.hpp"
#include "manager.hpp"
#include "clicker.hpp"


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

using namespace std;
Aimer::Aimer(GameManager& csgo) : csgo(csgo),
                                  mem(csgo.getMemoryAccess()),
                                  clicker(Clicker(csgo.getMemoryAccess())){

  display= XOpenDisplay(NULL);
  root_window = XRootWindow(display, 0);
}

void Aimer::setAim(EntityType* enemy) {
}

// needs m_rawinput 0
void Aimer::xSetAim(EntityType* enemy) {
  // EntityType* local_player = csgo.getLocalPlayer();
  EntityType* local_player = csgo.getPlayers()[0];
  Vector player_pos = local_player->m_vecOrigin;
  Vector enemy_pos = enemy->m_vecOrigin;
  Vector dist = getDist(&player_pos, &enemy_pos);
  printf("dist: %f, %f, %f\n", dist.x, dist.y, dist.z);
  normalize_vector(&dist);

  QAngle currAngle = local_player->m_angNetworkAngles;
  float radians = degree_to_radian(currAngle.y);
  float y = cos(radians);
  Vector current_view = {0, y, sqrt(1 - y*y)};
  printf("view: %f, %f, %f\n", current_view.x, current_view.y, current_view.z);
  float missing_angle = acos(scalar_prod(&dist, &current_view));
  cout << "missing angle: " << radian_to_degree(missing_angle) << endl;
}

void Aimer::moveAim(int dx, int dy) {
  int fd;
  struct uinput_user_dev uidev;

  fd = open("/dev/input/uinput", O_WRONLY | O_NONBLOCK);
  if(fd < 0)
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        errexit("error: ioctl(fd, UI_SET_EVBIT, EV_KEY)");
    if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
        errexit("error: ioctl(fd, UI_SET_KEYBIT, BTN_LEFT)");

    if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
        errexit("error: ioctl(fd, UI_SET_EVBIT, EV_REL)");
    if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
        errexit("error: ioctl(fd, UI_SET_RELBIT, REL_X)");
    if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
        errexit("error: ioctl(fd, UI_SET_RELBIT, REL_Y)");

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    if(write(fd, &uidev, sizeof(uidev)) < 0)
        errexit("error: write &uidev");

    if(ioctl(fd, UI_DEV_CREATE) < 0)
        errexit("error: ioctl UI_DEV_CREATE");

    sleep(1);

    struct input_event ev;
    int i, dx, dy;
    for (i = 0; i < 20; i++)
    {
        switch(i % 4) {
        case 0:
            dx = 50;
            dy = -50;
            break;
        case 1:
            dx = 50;
            dy = 50;
            break;
        case 2:
            dx = -50;
            dy = 50;
            break;
        case 3:
            dx = -50;
            dy = -50;
            break;
        }

        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_REL;
        ev.code = REL_X;
        ev.value = dx;
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            errexit("error: write");

        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_REL;
        ev.code = REL_Y;
        ev.value = dy;
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            errexit("error: write");

        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_SYN;
        if(write(fd, &ev, sizeof(struct input_event)) < 0)
            errexit("error: write");

        usleep(800000);
    }

    sleep(2);

    if(ioctl(fd, UI_DEV_DESTROY) < 0)
        errexit("error: ioctl UI_DEV_DESTROY");

    close(fd);

}
