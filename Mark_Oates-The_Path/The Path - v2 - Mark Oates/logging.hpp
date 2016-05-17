

enum log_level_t
{
  L_SILENT = 0,
  L_ERRORS,
  L_NORMAL,
  L_VERBOSE,
};


#define DEFAULT_LOG_LEVEL L_NORMAL


class Logging
{
private:
  static Logging *instance;
  static Logging *get_instance()
  {
    if (!instance) instance = new Logging(DEFAULT_LOG_LEVEL);
    return instance;
  }
  Logging(log_level_t level) : level(level) {}

  log_level_t level;

public:
  static void set_level(log_level_t log_level)
  {
    Logging *instance = get_instance();
    instance->level = log_level;
  }
  static bool at_least(log_level_t log_level)
  {
    Logging *instance = get_instance();
    return instance->level >= log_level;
  }
};
Logging *Logging::instance = NULL;



