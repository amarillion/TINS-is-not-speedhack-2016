

class PickupCardKeyDecrypter : public Script, public ScriptHelper
{
public:
  PickupCardKeyDecrypter() : Script("PickupCardKeyDecrypter") {}
  void activate() override
  {
    inventory_gui->add_item(InventoryItem::CARD_KEY_DECRYPTER);
    delete_entity("CardKeyDecrypter");
    show_notification("picked up Card Key Decrypter");
  }
};



class PickupEncryptedCardKey : public Script, public ScriptHelper
{
public:
  PickupEncryptedCardKey() : Script("PickupEncryptedCardKey") {}
  void activate() override
  {
    inventory_gui->add_item(InventoryItem::ENCRYPTED_CARD_KEY);
    delete_entity("EncryptedCardKey");
    show_notification("picked up Encrypted Card Key");
  }
};



class OpenMainDoor : public Script, public ScriptHelper
{
public:
  OpenMainDoor() : Script("OpenMainDoor") {}
  void activate() override
  {
    if (inventory_gui->has_item(InventoryItem::Type::DECRYPTED_CARD_KEY))
    {
      show_notification("You have escaped the moon base.");
    }
    else
    {
      show_notification("You need a valid key");
    }
  }
};


