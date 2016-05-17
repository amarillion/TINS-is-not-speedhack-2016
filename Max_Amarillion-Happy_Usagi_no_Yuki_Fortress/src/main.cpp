#include <assert.h>
#include "color.h"
#include "mainloop.h"
#include "engine.h"

const int GAME_WIDTH=800;
const int GAME_HEIGHT=600;

int main(int argc, const char *const *argv)
{
	auto mainloop = MainLoop();
	auto engine = Engine::newInstance();

	mainloop
		.setEngine(engine)
		.setAppName("Tins2016")
		.setTitle("Happy Usagi no Yuki Fortress")
		.setConfigFilename("tins16.cfg")
		.setDimension(GAME_WIDTH, GAME_HEIGHT);

	if (!mainloop.init(argc, argv) && !engine->init())
	{
		mainloop.run();
		engine->done();
	}

	return 0;
}
