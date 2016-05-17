

class StartTitleScreen: public Script, public ScriptHelper
{
public:
  StartTitleScreen() : Script("StartTitleScreen()") {}
  void activate() override
  {
    simple_camera_to(0, 0, 0.2, 0);

    world_navigation_gui->set_usability_mode(0, 0);
    inventory_gui->set_visibility_mode(0);
    start_screen_gui->hide(0);
    start_screen_gui->show(0);
  }
};


