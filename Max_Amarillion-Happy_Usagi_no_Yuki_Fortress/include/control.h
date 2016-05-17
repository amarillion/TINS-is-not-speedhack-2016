#include "component.h"
#include "ComponentBuilder.h"

class Button : public IComponent
{
private:
    int action;
protected:
    std::string text;
    ALLEGRO_BITMAP* icon;
    bool hasFocus;
    bool pressed;
    bool enabled;
public:
    int getAction() { return action; }
	virtual void handleMessage(int msg) override;
	Button (int action, std::string text, ALLEGRO_BITMAP* icon = NULL) : action(action), text(text), icon(icon), hasFocus(false), pressed(false), enabled(true) {}
	virtual ~Button() {}
    virtual bool wantsFocus () override { return true; }
    virtual void draw(const GraphicsContext &gc) override;
    virtual void handleEvent(ALLEGRO_EVENT &event) override;
    virtual void setEnabled (bool value) { enabled = value; }
    static ComponentBuilder build(int action, std::string text, ALLEGRO_BITMAP* icon = NULL);
};
