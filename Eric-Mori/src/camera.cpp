#include <inc/camera.hpp>

Camera::Camera(void) {

  map_width = 0;
  map_height = 0;

  display_width = 0;
  display_height = 0;

  pan_x = 32.0;
  pan_y = 32.0;

  target_x = 0.0;
  target_y = 0.0;

  pan_speed = 2.0;
}

float Camera::getX(void) {

  return pan_x;
}

float Camera::getY(void) {

  return pan_y;
}

void Camera::pan(void) {

  pan_x = target_x - (display_width / 2) + 8;
  pan_y = target_y - (display_height / 2) + 8;

  if (pan_x < 16) {

    pan_x = 16;
  }

  if (pan_y < 16) {

    pan_y = 16;
  }

  // @TODO: Set global tile dimensions variable or constant.

  if (pan_x > (map_width * 16) - display_width - 16) {

    pan_x = (map_width * 16) - display_width - 16;
  }

  if (pan_y > (map_height * 16) - display_height - 16) {

    pan_y = (map_height * 16) - display_height - 16;
  }
}

void Camera::setTargetX(const float x) {

  target_x = x;
}

void Camera::setTargetY(const float y) {

  target_y = y;
}

void Camera::setMapWidth(const int width) {

  map_width = width;
}

void Camera::setMapHeight(const int height) {

  map_height = height;
}

void Camera::setDisplayWidth(const int width) {

  display_width = width;
}

void Camera::setDisplayHeight(const int height) {

  display_height = height;
}
