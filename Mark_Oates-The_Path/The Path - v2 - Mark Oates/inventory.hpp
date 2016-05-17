

class InventoryItem
{
public:
  enum Type
  {
    EMPTY = 0,
    ENCRYPTED_CARD_KEY,
    CARD_KEY_DECRYPTER,
    DECRYPTED_CARD_KEY,
  };

  Type type;

  InventoryItem(InventoryItem::Type type)
    : type(type)
  {}

  bool is_empty()
  {
    return type == EMPTY;
  }

  std::string get_name()
  {
    switch(type)
    {
      case ENCRYPTED_CARD_KEY:
        return "Encrypted Card Key";
        break;
      case CARD_KEY_DECRYPTER:
        return "Card Key Decrypter";
        break;
      case DECRYPTED_CARD_KEY:
        return "Decrypted Card Key";
        break;
      default:
        return "";
        break;
    }
  }
  std::string get_description()
  {
    switch(type)
    {
      case ENCRYPTED_CARD_KEY:
        return "";
        break;
      case CARD_KEY_DECRYPTER:
        return "";
        break;
      case DECRYPTED_CARD_KEY:
        return "";
        break;
      default:
        return "";
        break;
    }
    return std::string("");
  }
  ALLEGRO_BITMAP *get_image()
  {
    switch(type)
    {
      case ENCRYPTED_CARD_KEY:
        return af::bitmaps["card_key.png"];
        break;
      case CARD_KEY_DECRYPTER:
        return af::bitmaps["card_key_decryptor.png"];
        break;
      case DECRYPTED_CARD_KEY:
        return af::bitmaps["card_key.png"];
        break;
      default:
        return NULL;
        break;
    }
    return NULL;
  }
};


class InventoryGUIInventoryToggleButton : public FGUIWidget
{
public:
  InventoryItem shown_item;

  InventoryGUIInventoryToggleButton(FGUIWidget *parent)
    : FGUIWidget(parent, new FGUISurfaceAreaBox(SCREEN_W-80, 60, 120, 80))
    , shown_item(InventoryItem::Type::EMPTY)
  {}
  void on_click()
  {
    if (Logging::at_least(L_VERBOSE)) std::cout << "InventoryGUIInventoryToggleButton" << std::endl;
    send_message_to_parent("toggle_visibility_mode()");
  }
  void set_shown_item(InventoryItem item)
  {
    shown_item = item;
  }
  void on_draw()
  {
    Style::draw_button(Style::NORMAL, place, shown_item.is_empty() ? "0" : "", shown_item.get_image());
  }
  void show(float speed=0.5)
  {
    if (Logging::at_least(L_VERBOSE)) std::cout << "InventoryGUIInventoryToggleButton.show()" << std::endl;
    af::motion.cmove_to(&place.position.x, SCREEN_W-80, speed);
  }
  void hide(float speed=0.5)
  {
    if (Logging::at_least(L_VERBOSE)) std::cout << "InventoryGUIInventoryToggleButton.hide()" << std::endl;
    af::motion.cmove_to(&place.position.x, SCREEN_W+60, speed);
  }
};


class InventoryGUIItemButton : public FGUIWidget
{
private:
  class GUICombineButton : public FGUIWidget
  {
  public:
    GUICombineButton(FGUIWidget *parent)
      : FGUIWidget(parent, new FGUISurfaceAreaBox(-30, parent->place.size.y/2, 40, 50))
    {}
    void on_click() override
    {
      if (Logging::at_least(L_VERBOSE)) std::cout << "GUICombineButton.on_click()" << std::endl;
      send_message_to_parent("attempt_to_combine()");
    }
    void on_draw() override
    {
      Style::draw_button(Style::NORMAL, place, "<");
    }
  };

public:
  InventoryItem item;
  int show_x_pos;
  bool selected;

  GUICombineButton *combine_button;

  InventoryGUIItemButton(FGUIWidget *parent, float x, float y)
    : FGUIWidget(parent, new FGUISurfaceAreaBox(x, y, 80, 80))
    , item(InventoryItem::Type::EMPTY)
    , show_x_pos(x)
    , selected(false)
    , combine_button(NULL)
  {
    combine_button = new GUICombineButton(this);
  }
  void set_item(InventoryItem item)
  {
    this->item = item;
  }
  void clear_item()
  {
    this->item = InventoryItem(InventoryItem::Type::EMPTY);
  }
  bool is_empty()
  {
    return (item.type == InventoryItem::Type::EMPTY);
  }
  void on_click()
  {
    if (Logging::at_least(L_VERBOSE)) std::cout << "InventoryGUIItemButton" << std::endl;
    send_message_to_parent("feature_my_item()");
  }
  void select()
  {
    selected = true;
  }
  void deselect()
  {
    selected = false;
  }
  void on_message(FGUIWidget *sender, std::string message)
  {
    // message will always be attempt_to_combine()
    if (sender == combine_button) send_message_to_parent("attempt_to_combine()");
  }
  void on_draw()
  {
    Style::draw_button(selected ? Style::SELECTED : Style::NORMAL, place, item.is_empty() ? "-" : "", item.get_image());
  }
  void show(float speed=0.5)
  {
    if (Logging::at_least(L_VERBOSE)) std::cout << "InventoryGUIItemButton.show()" << std::endl;
    af::motion.cmove_to(&place.position.x, show_x_pos, speed);
  }
  void hide(float speed=0.5)
  {
    if (Logging::at_least(L_VERBOSE)) std::cout << "InventoryGUIItemButton.hide()" << std::endl;
    af::motion.cmove_to(&place.position.x, show_x_pos+200, speed);
  }
};


class InventoryGUICurrentItemShowcase : public FGUIWidget
{
public:
  InventoryItem item;
  FGUIImage *featured_image;
  FGUIText *title_text;
  FGUITextBox *description_textbox;

  InventoryGUICurrentItemShowcase(FGUIWidget *parent)
    : FGUIWidget(parent, new FGUISurfaceAreaBox(SCREEN_W-500, SCREEN_H/2, 600, 400))
    , item(InventoryItem::Type::EMPTY)
    , title_text(NULL)
    , description_textbox(NULL)
    , featured_image(NULL)
  {
    featured_image = new FGUIImage(this, place.size.x/2, place.size.y/2, item.get_image());
    title_text = new FGUIText(this, 20, 20, item.get_name());
    title_text->place.align.y = 0;
    title_text->set_font(af::fonts["space age.otf 34"]);
    description_textbox = new FGUITextBox(this, place.size.x/2, place.size.y*0.75, place.size.x*0.8, place.size.y*0.25, item.get_description()) ;
    description_textbox->set_text_color(color::black);
    description_textbox->place.align.x = 0.5;
    description_textbox->place.align.y = 0;
  }
  void on_draw() override
  {
    Style::draw_button(Style::NORMAL, place, item.is_empty() ? "- EMPTY -" : "");
  }
  void set_item(InventoryItem item)
  {
    this->item = item;
    featured_image->set_bitmap(item.get_image());
    title_text->set_text(item.get_name());
    description_textbox->set_text(item.get_description());
  }
  void show(float speed=0.5)
  {
    if (Logging::at_least(L_VERBOSE)) std::cout << "InventoryGUICurrentItemShowcase.show()" << std::endl;
    af::motion.cmove_to(&place.position.x, SCREEN_W-500, speed);
  }
  void hide(float speed=0.5)
  {
    if (Logging::at_least(L_VERBOSE)) std::cout << "InventoryGUICurrentItemShowcase.hide()" << std::endl;
    af::motion.cmove_to(&place.position.x, SCREEN_W+500, speed);
  }
};


class InventoryGUINotification : public FGUIWidget
{
public:
  std::string notification_text;
  float visibility_timer;

  InventoryGUINotification(FGUIWidget *parent)
    : FGUIWidget(parent, new FGUISurfaceAreaBox(SCREEN_W/2, SCREEN_H/4*3, 900, 100))
    , visibility_timer(-1)
  {
  }
  void on_timer() override
  {
    visibility_timer -= 1.0/60.0;
    if (visibility_timer < 0) visibility_timer = -1.0;
  }
  void mouse_down_func() override
  {
    FGUIWidget::mouse_down_func();
    visibility_timer = -1.0;
  }
  void on_draw() override
  {
    if (visibility_timer < 0) return;
    Style::draw_button(Style::NORMAL, 0, 0, place.size.x, place.size.y, notification_text);
    //Style::draw_text_box(0, 0, place.size.x, place.size.y, notification_text);
  }
  void show(std::string text)
  {
    notification_text = text;
    visibility_timer = 5.0;
  }
};


class InventoryGUIBehindBlocker : public FGUIWidget
{
public:
  InventoryGUIBehindBlocker(FGUIWidget *parent)
    : FGUIWidget(parent, new FGUISurfaceAreaBox(SCREEN_W/2, SCREEN_H/2, SCREEN_W, SCREEN_H))
  {}
  void on_click() override
  {
    send_message_to_parent("set_visibility_mode(1)");
  }
  void on_draw() override
  {
    al_draw_filled_rectangle(0, 0, place.size.x, place.size.y, color::color(color::black, 0.3));
  }
  void show()
  {
    place.scale.x = 1.0;
    place.scale.y = 1.0;
  }
  void hide()
  {
    place.scale.x = 0;
    place.scale.y = 0;
  }
};


class InventoryGUIScreen : public FGUIScreen
{
public:
  const int NUM_INVENTORY_ITEM_BUTTONS;

  // MODE 1 Widget
  InventoryGUIInventoryToggleButton *toggle_button;

  // MODE 2 Widgets
  InventoryGUICurrentItemShowcase *current_item_showcase;
  InventoryGUIBehindBlocker *behind_blocker;
  std::vector<InventoryGUIItemButton *> item_buttons;

  InventoryGUINotification *notification;

  int current_mode;

  InventoryGUIScreen(Display *display)
    : FGUIScreen(display)
    , NUM_INVENTORY_ITEM_BUTTONS(2)
    , toggle_button(NULL)
    , current_item_showcase(NULL)
    , behind_blocker(NULL)
    , notification(NULL)
    , item_buttons()
    , current_mode(1)
  {
    // create our Inventory GUI Widgets
    behind_blocker = new InventoryGUIBehindBlocker(this);
    toggle_button = new InventoryGUIInventoryToggleButton(this);
    current_item_showcase = new InventoryGUICurrentItemShowcase(this);
    notification = new InventoryGUINotification(this);
    for (unsigned i=0; i<NUM_INVENTORY_ITEM_BUTTONS; i++)
    {
      InventoryGUIItemButton *button = new InventoryGUIItemButton(this, SCREEN_W-100, 200+90*i);
      item_buttons.push_back(button);
    }

    // set our current GUI mode
    set_visibility_mode(current_mode);
  }

  void on_message(FGUIWidget *sender, std::string message) override
  {
    if (message == "") return;
    else if (message == "toggle_visibility_mode()")
    {
      if (current_mode == 1) set_visibility_mode(2);
      else if (current_mode == 2) set_visibility_mode(1);
    }
    else if (message == "set_visibility_mode(0)") set_visibility_mode(0);
    else if (message == "set_visibility_mode(1)") set_visibility_mode(1);
    else if (message == "set_visibility_mode(2)") set_visibility_mode(2);
    else if (message == "feature_my_item()")
    {
      // deselect all the other buttons
      for (auto &item : item_buttons)
        if (item != sender) static_cast<InventoryGUIItemButton *>(item)->deselect();

      // set the item in the showcase
      InventoryGUIItemButton *button = static_cast<InventoryGUIItemButton *>(sender);
      current_item_showcase->set_item(button->item);
      button->select();

      // set the item in the inventory toggle button
      toggle_button->set_shown_item(button->item);
    }
    else if (message == "attempt_to_combine()")
    {
      InventoryGUIItemButton *combining_button = static_cast<InventoryGUIItemButton *>(sender);
      InventoryItem combined_item = attempt_combination(current_item_showcase->item, combining_button->item);
      if (!combined_item.is_empty())
      {
        // combination was successful
        std::cout << "Combination was successful" << std::endl;

        // clear the item on both selected button
        InventoryGUIItemButton *selected_item_button = get_selected_item_button();
        selected_item_button->clear_item();
        selected_item_button->deselect();

        // set the new combined_item item on the combining_button
        combining_button->set_item(combined_item);
        combining_button->select();
        combining_button->send_message_to_parent("feature_my_item()");

        notification->show("You decrypted the Card Key");
      }
      else
      {
        // combination failed
        std::cout << "Combination failed" << std::endl;
      }
    }
  }

  bool symmetric_yes(InventoryItem itemA, InventoryItem itemB, InventoryItem::Type type1, InventoryItem::Type type2)
    // ^ nice 3AM name for a function
  {
    if (itemA.type == type1 && itemB.type == type2) return true;
    if (itemB.type == type1 && itemA.type == type2) return true;
    return false;
  }

  InventoryItem attempt_combination(InventoryItem itemA, InventoryItem itemB)
  {
    if (symmetric_yes(itemA, itemB, InventoryItem::Type::ENCRYPTED_CARD_KEY, InventoryItem::Type::CARD_KEY_DECRYPTER))
    {
      return InventoryItem(InventoryItem::Type::DECRYPTED_CARD_KEY);
    }

    // TODO: process possible combinations here
    // return the item that is created when combining two other items
    // a failed combination will return an item that is EMPTY
    return InventoryItem(InventoryItem::Type::EMPTY);
  }

  bool has_item(InventoryItem::Type item_type)
  {
    for (auto &button : item_buttons)
      if (button->item.type == item_type) return true;
    return false;
  }

  InventoryGUIItemButton *get_selected_item_button()
  {
    for (auto &button : item_buttons) if (button->selected) return button;
    return NULL;
  }

  InventoryGUIItemButton *find_first_empty_inventory_button()
  {
    for (auto &button : item_buttons) if (button->is_empty()) return button;
    return NULL;
  }

  bool add_item(InventoryItem item)
  {
    InventoryGUIItemButton *empty_button = find_first_empty_inventory_button();
    if (!empty_button) return false;
    empty_button->set_item(item);
    return true;
  }

  void set_visibility_mode(int mode)
  {
    std::cout << " === setting GUI mode " << mode << " ===" << std::endl;
    switch(mode)
    {
      case 0:
        behind_blocker->hide();
        toggle_button->hide();
        current_item_showcase->hide();
        for (auto &button : item_buttons) button->hide();
        current_mode = 0;
        break;
      case 1:
        behind_blocker->hide();
        toggle_button->show();
        current_item_showcase->hide();
        for (auto &button : item_buttons) button->hide();
        current_mode = 1;
        break;
      case 2:
        behind_blocker->show();
        toggle_button->show();
        current_item_showcase->show();
        for (auto &button : item_buttons) button->show();
        current_mode = 2;
        break;
      default:
        // Undefined Mode
        std::cout << "undefined InventoryGUIScreen mode " << mode << std::endl;
        break;
    }
  }
};



