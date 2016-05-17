

class StartGame: public Script, public ScriptHelper
{
public:
  StartGame() : Script("StartGame()") {}
  void activate() override
  {
    world_navigation_gui->set_usability_mode(1);
    inventory_gui->set_visibility_mode(1);
    start_screen_gui->hide(0);

    // clean up all the previous game's assets here
    // (hahahaha!)

    // load game assets
    Entity *e = new Entity(
        world_render->manager,
        "Station",
        af::models["space_station-04.obj"],
        af::bitmaps["floor_texture.jpeg"]);
        //af::bitmaps["uv.png"]);

    // create the card key
    create_entity("EncryptedCardKey", vec3d(5, 0, 4), 0.4, "card_key-02.obj", "sci_fi_panel2.jpg");

    // create the card key decrypter
    create_entity("CardKeyDecrypter", vec3d(-3, 0, 4+24), 0.3, "card_key_decrypter-02.obj", "tech_device_texture.jpg");

    // create the posters
    create_entity("SnowPoster", vec3d(19, 2, 0), -0.25, "poster3.obj", "snow_poster.jpg");
    create_entity("PoetryPoster", vec3d(0, 2, 31), 0.5, "poster3.obj", "brilliant_poster.jpg");

    // create the door frame
    create_entity("MainDoorFrame", vec3d(-12, 0, 0), 0.25, "main_door_frame-01.obj", "tech_device_texture.jpg");
    create_entity("MainDoorTerminal", vec3d(-12, 0, 0), 0.25, "main_door_terminal-01.obj", "terminal_gui.jpg");
    create_entity("MainDoorDoor", vec3d(-12, 0, 0), 0.25, "main_door_door-01.obj", "sci_fi_panel2.jpg");

    // goto the starting point
    Script::run("goto1");
  }
};


