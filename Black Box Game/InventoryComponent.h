#pragma once
#include <Actors/Component.h>

class BlackBoxGame;
class InventoryComponent : public BlackBoxEngine::Component
{
    GENERATE_ID("InventoryComponent");

    bool m_hasTablet = false;
    bool m_hasTorch = true;
   //bool m_hasClub = true;
   //bool m_hasClothes = true;

public:
    InventoryComponent(BlackBoxEngine::Actor* pOwner) : Component(pOwner) {}
    virtual ~InventoryComponent(){ }

  
    void Start() override {}
    void Load([[maybe_unused]] const BlackBoxEngine::XMLElementParser parser) override;
    void Save([[maybe_unused]] BlackBoxEngine::XMLElementParser parser) override;

    void SetHasTablet(bool value) { m_hasTablet = value; }
    bool GetHasTablet() { return m_hasTablet; }

    void SetHasTorch(bool value) { m_hasTorch = value; }
    bool GetHasTorch() { return m_hasTorch; }

};

