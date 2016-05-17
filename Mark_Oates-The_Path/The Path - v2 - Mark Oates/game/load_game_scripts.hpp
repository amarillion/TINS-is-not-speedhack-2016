

#include "./scripts/gotos.hpp"
#include "./scripts/pickups.hpp"

void ScriptHelper::load_game_scripts()
{
  // load the individual scripts
  new StartTitleScreen();
  new StartGame();

  new goto1();
  new goto2();
  new goto3();
  new goto4();
  new goto5();
  new goto8();
  new goto18();
  new goto23();


  new PickupCardKeyDecrypter();
  new PickupEncryptedCardKey();

  
  new OpenMainDoor();
}


