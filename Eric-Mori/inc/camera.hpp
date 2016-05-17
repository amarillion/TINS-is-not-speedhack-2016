#ifndef TINS_INC_CAMERA_HPP
#define TINS_INC_CAMERA_HPP

#include <iostream>

using std::cout;

class Camera {

  public:

    Camera(void);

    float getX(void);
    float getY(void);

    void pan(void);

    void setTargetX(const float);
    void setTargetY(const float);

    void setMapWidth(const int);
    void setMapHeight(const int);

    void setDisplayWidth(const int);
    void setDisplayHeight(const int);

  private:

    int map_width;
    int map_height;

    int display_width;
    int display_height;

    float pan_x;
    float pan_y;

    float target_x;
    float target_y;

    float pan_speed;
};

#endif
