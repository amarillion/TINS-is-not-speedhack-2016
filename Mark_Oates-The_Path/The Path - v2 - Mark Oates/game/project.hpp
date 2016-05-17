

#include "./script_helper.hpp"

#include "./scripts/start_title_screen.hpp"
#include "./scripts/start_game.hpp"


class Project : public FGUIScreen
{
public:
  WorldRenderScreen *world_render;
  WorldNavigationGUIScreen *world_navigation_gui;
  InventoryGUIScreen *inventory_gui;
  StartScreenGUIScreen *start_screen_gui;

  Project(Display *display)
    : FGUIScreen(display)
    , world_render(new WorldRenderScreen(display))
    , world_navigation_gui(new WorldNavigationGUIScreen(this, display))
    , inventory_gui(new InventoryGUIScreen(display))
    , start_screen_gui(new StartScreenGUIScreen(this, display))
  {
    // link nav render surface
    world_render->set_scene_targets_render_surface(world_navigation_gui->nav_view->render);

    ScriptHelper::initialize(world_render, world_navigation_gui, inventory_gui, start_screen_gui);

    Script::run("StartTitleScreen()");
  }
  void on_message(FGUIWidget *sender, std::string message)
  {
    std::string trigger_id = "";
    int unique_trigger_id = 0;
    if (TargetID::extract_trigger_id(message, &trigger_id))
    {
      std::cout << "Project running script \"" << trigger_id << "\"" << std::endl;
      Script::run(trigger_id);
    }
    else if (TargetID::extract_unique_trigger_id(message, &unique_trigger_id))
    {
      if (Logging::at_least(L_VERBOSE)) std::cout << "Project running script ID\"" << unique_trigger_id << "\"" << std::endl;
      Script::run_by_unique_id(unique_trigger_id);
    }
  }
};

