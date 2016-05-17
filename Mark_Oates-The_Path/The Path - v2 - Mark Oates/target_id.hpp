

ALLEGRO_COLOR encode_id(int id)
{
	ALLEGRO_COLOR color;
	unsigned char r = id / 256;
	unsigned char g = id % 256;
	return al_map_rgba(r, g, 0, 255);
}


int decode_id(ALLEGRO_COLOR color)
{
	unsigned char r, g, b, a;
	al_unmap_rgba(color, &r, &g, &b, &a);
	return r * 256 + g;
}


class TargetID
{
public:
  std::string id;
  TargetID(std::string id)
    : id(id)
  {}
  bool is_empty()
  {
    return id == "";
  }
  std::string get_trigger_message() { return tostring("trigger_target_id ") + id; }
  static bool extract_trigger_id(std::string message, std::string *extracted_id)
  {
    if (strncmp(message.c_str(), "trigger_target_id ", 18) == 0)
    {
      // we have a valid trigger message
      *extracted_id = message.substr(18);
      return true;
    }
    return false;
  }
  static std::string compose_unique_trigger_id_message(int unique_id)
  {
    return tostring("trigger_unique_target_id ") + tostring(unique_id);
  }
  static bool extract_unique_trigger_id(std::string message, int *extracted_unique_id)
  {
    if (strncmp(message.c_str(), "trigger_unique_target_id ", 25) == 0)
    {
      // we have a valid trigger message
      *extracted_unique_id = atoi(message.substr(25).c_str());
      return true;
    }
    return false;
  }
};


