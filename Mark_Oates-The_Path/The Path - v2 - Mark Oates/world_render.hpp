


class Entity : public ElementID
{
private:
  static Shader *flat_color_shader;
  static bool shaders_initialized;

  void initialize_shaders()
  {
    if (!shaders_initialized)
    {
      flat_color_shader = new Shader("data/shaders/flat_color_vertex.glsl",
        "data/shaders/flat_color_fragment.glsl");
      shaders_initialized = true;
    }
  }

public:
  placement3d place;
  placement3d velocity;
  ModelNew *model;
  ALLEGRO_BITMAP *texture;
  int attached_script_id;

  Entity(ElementIDManager *manager, std::string id, ModelNew *model=NULL, ALLEGRO_BITMAP *texture=NULL)
    : ElementID(manager)
    , place()
    , velocity()
    , model(model)
    , texture(texture)
    , attached_script_id(0)
  {
    initialize_shaders();

    set_id(id);
    velocity.align = vec3d(0, 0, 0);
    velocity.scale = vec3d(0, 0, 0);
  }

  void attach_script_id(int script_unique_id)
  {
    attached_script_id = script_unique_id;
  }

  int get_attached_script_id()
  {
    return attached_script_id;
  }

  bool is_script_attached()
  {
    return attached_script_id != 0;
  }

  void draw()
  {
    if (!model) return;
    if (texture) model->set_texture(texture);

    Shader::stop();

    place.start_transform();
    model->draw();
    place.restore_transform();
  }

  void draw_flat_color(ALLEGRO_COLOR color)
  {
    if (!model) return;

    flat_color_shader->use();
    if (is_script_attached()) Shader::set_vec4("tint", color.r, color.g, color.b, color.a);
    else Shader::set_vec4("tint", 0, 0, 0, 0);

    place.start_transform();
    model->draw();
    place.restore_transform();
  }
};
Shader *Entity::flat_color_shader = NULL;
bool Entity::shaders_initialized = false;




class WorldRenderScreen : public Screen
{
private:
  void primary_timer_func() override
  {
    update_scene();
    if (scene_targets_render_surface_ref) draw_scene_targets(scene_targets_render_surface_ref);
    draw_scene();
  }

  void set_projection(ALLEGRO_BITMAP *bitmap, ALLEGRO_TRANSFORM *t)
  {
    float aspect_ratio = (float)al_get_bitmap_height(bitmap) / al_get_bitmap_width(bitmap);
    al_perspective_transform(t, -1, aspect_ratio, 1, 1, -aspect_ratio, 100);
    al_use_projection_transform(t);
  }

public:
  ElementIDManager *manager;
  Entity *camera;

  ALLEGRO_BITMAP *scene_targets_render_surface_ref;

  WorldRenderScreen(Display *display)
    : Screen(display)
    , manager(new ElementIDManager())
    , camera(NULL)
    , scene_targets_render_surface_ref(NULL)
  {
    // create the camera
    camera = new Entity(manager, "Camera", NULL, NULL);
  }

  void set_scene_targets_render_surface(ALLEGRO_BITMAP *surface)
  {
    scene_targets_render_surface_ref = surface;
  }

  void update_scene()
  {
    for (auto &elem : manager->elements)
    {
      Entity *entity = static_cast<Entity *>(elem);
      entity->place += entity->velocity;
    }
  }

  void prep_render(ALLEGRO_BITMAP *surface, placement3d &camera_view)
  {
    // setup the render settings
    al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
    al_set_render_state(ALLEGRO_WRITE_MASK, ALLEGRO_MASK_DEPTH | ALLEGRO_MASK_RGBA);
    al_clear_depth_buffer(1);

    ALLEGRO_TRANSFORM t;
    camera_view.build_reverse_transform(&t);
    set_projection(surface, &t);
  }

  void draw_scene()
  {
    al_clear_to_color(color::black);
    prep_render(backbuffer_sub_bitmap, camera->place);

    // draw our entities
    for (auto &elem : manager->elements)
    {
      Entity *entity = static_cast<Entity *>(elem);
      entity->draw();
    }
  }

  void draw_scene_targets(ALLEGRO_BITMAP *surface)
  {
    ALLEGRO_STATE state;
    al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
    al_set_target_bitmap(surface);

    prep_render(surface, camera->place);
    al_clear_to_color(color::transparent);

    // draw our entities
    for (auto &elem : manager->elements)
    {
      Entity *entity = static_cast<Entity *>(elem);
      entity->draw_flat_color(encode_id(entity->get_attached_script_id()));
    }

    al_restore_state(&state);
  }
};



