

class ScriptHelper
{
protected:
  static WorldRenderScreen *world_render;
  static WorldNavigationGUIScreen *world_navigation_gui;
  static InventoryGUIScreen *inventory_gui;
  static StartScreenGUIScreen *start_screen_gui;
  static bool initialized;

public:
  static void load_game_scripts();

  // helper methods
  static float normalize_rotation(float *rotation)
  {
    while (*rotation > 0.0) *rotation -= 1.0;
    while (*rotation < 0.0) *rotation += 1.0;
    return *rotation;
  }
  static void fix_least_distance_rotation_theta(float *from, float *to)
  {
    // worst. algorithm. ever.
    if (*to > *from) { if (fabs(*to-*from) > 0.5) *to = *to-1.0; }
    else { if (fabs(*from-*to) > 0.5) *to = *to+1.0; }
  }
  static void fix_least_distance_rotation(placement3d *from, placement3d *to)
  {
    normalize_rotation(&from->rotation.x);
    normalize_rotation(&from->rotation.y);
    normalize_rotation(&from->rotation.z);

    normalize_rotation(&to->rotation.x);
    normalize_rotation(&to->rotation.y);
    normalize_rotation(&to->rotation.z);

    fix_least_distance_rotation_theta(&from->rotation.x, &to->rotation.x);
    fix_least_distance_rotation_theta(&from->rotation.y, &to->rotation.y);
    fix_least_distance_rotation_theta(&from->rotation.z, &to->rotation.z);
  }
  static void move_to(placement3d *from, placement3d to, float speed=0.4)
  {
    af::motion.cmove_to(&from->position.x, to.position.x, speed);
    af::motion.cmove_to(&from->position.y, to.position.y, speed);
    af::motion.cmove_to(&from->position.z, to.position.z, speed);

    fix_least_distance_rotation(from, &to);

    af::motion.cmove_to(&from->rotation.x, to.rotation.x, speed);
    af::motion.cmove_to(&from->rotation.y, to.rotation.y, speed);
    af::motion.cmove_to(&from->rotation.z, to.rotation.z, speed);

    af::motion.cmove_to(&from->anchor.x, to.anchor.x, speed);
    af::motion.cmove_to(&from->anchor.y, to.anchor.y, speed);
    af::motion.cmove_to(&from->anchor.z, to.anchor.z, speed);
  }
  static void simple_camera_to(float x, float y, float rotation, float speed=1.0)
  {
    placement3d place = placement3d();
    place.position = vec3d(x, 2, y);
    place.rotation = vec3d(0, rotation, 0);
    place.anchor = vec3d(0, 0, 1);

    move_to(&world_render->camera->place, place, speed);
  }
  static void clear_attached_scripts()
  {
    for (auto &e : world_render->manager->elements)
    {
      Entity *entity = static_cast<Entity *>(e);
      entity->attach_script_id(0);
    }
  }
  static void delete_entity(std::string entity_id)
  {
    Entity *e = entity_by_id(entity_id);
    // god save us all...
    delete e; 
  }
  static int get_script_unique_id(std::string script_id)
  {
    Script *script = Script::find_by_id(script_id);
    if (script) return script->get_unique_id_num();

    std::cout << CONSOLE_COLOR_RED << "could not locate script by id \"" << script_id << "\"" << CONSOLE_COLOR_DEFAULT << std::endl;
    return 0;
  }
  static void attach(std::string entity_id, std::string script_id)
  {
    Entity *e = entity_by_id(entity_id);
    int script_unique_id = get_script_unique_id(script_id);
    if (e && (script_unique_id != 0))
    {
      e->attach_script_id(script_unique_id);
    }
    else
    {
      std::cout << CONSOLE_COLOR_RED << "attachment failed" << CONSOLE_COLOR_DEFAULT << std::endl;
    }
  }
  static Entity *entity_by_id(std::string entity_id)
  {
    Entity *e = static_cast<Entity *>(world_render->manager->get_element_by_id(entity_id));
    if (!e)
    {
      std::cout << CONSOLE_COLOR_RED << "could not locate entity by id \"" << entity_id << "\"" << CONSOLE_COLOR_DEFAULT << std::endl;
    }
    return e;
  }
  static void show_notification(std::string message)
  {
    inventory_gui->notification->show(message);
  }
  static void camera_to(vec3d position, vec3d rotation, vec3d anchor, float speed=0.4)
  {
    placement3d place = placement3d();
    place.position = position;
    place.rotation = rotation;
    place.anchor = anchor;

    move_to(&world_render->camera->place, place, speed);
  }
  static void set_nav_buttons
    ( std::string up_target
    , std::string down_target
    , std::string left_target
    , std::string right_target
    )
  {
    world_navigation_gui->nav_up_button->set_target_id(TargetID(up_target));
    world_navigation_gui->nav_down_button->set_target_id(TargetID(down_target));
    world_navigation_gui->nav_left_button->set_target_id(TargetID(left_target));
    world_navigation_gui->nav_right_button->set_target_id(TargetID(right_target));

    world_navigation_gui->set_usability_mode(1);
  }
  static Entity *create_entity(std::string id, vec3d position, float rotation, std::string obj_filename, std::string texture_filename)
  {
    Entity *e = new Entity(
        world_render->manager,
        id,
        af::models[obj_filename],
        af::bitmaps[texture_filename]);
    e->place.position = position;
    e->place.rotation.y = rotation;

    return e;
  }
  static void set_nav
    ( std::string up_target_num
    , std::string down_target_num
    , std::string left_target_num
    , std::string right_target_num
    )
  {
    world_navigation_gui->set_usability_mode(0);

    std::string up = (up_target_num.empty() ? "" : (tostring("goto") + up_target_num));
    world_navigation_gui->nav_up_button->set_target_id(TargetID(up));

    std::string down = (down_target_num.empty() ? "" : (tostring("goto") + down_target_num));
    world_navigation_gui->nav_down_button->set_target_id(TargetID(down));

    std::string left = (left_target_num.empty() ? "" : (tostring("goto") + left_target_num));
    world_navigation_gui->nav_left_button->set_target_id(TargetID(left));

    std::string right = (right_target_num.empty() ? "" : (tostring("goto") + right_target_num));
    world_navigation_gui->nav_right_button->set_target_id(TargetID(right));

    world_navigation_gui->set_usability_mode(1);
  }
  static void initialize(
      WorldRenderScreen *wrs,
      WorldNavigationGUIScreen *wnguis,
      InventoryGUIScreen *iguis,
      StartScreenGUIScreen *ssgui
      )
  {
    if (initialized) return;

    Script::initialize();

    world_render = wrs;
    world_navigation_gui = wnguis;
    start_screen_gui = ssgui;
    inventory_gui = iguis;

    load_game_scripts();

    initialized = true;
  }
};
WorldNavigationGUIScreen *ScriptHelper::world_navigation_gui = NULL;
InventoryGUIScreen *ScriptHelper::inventory_gui = NULL;
StartScreenGUIScreen *ScriptHelper::start_screen_gui = NULL;
WorldRenderScreen *ScriptHelper::world_render = NULL;
bool ScriptHelper::initialized = false;


