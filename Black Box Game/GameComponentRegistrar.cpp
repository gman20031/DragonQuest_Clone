#include <Actors/ComponentFactory.h>

#include "ExampleComponent.h"
#include "ScreenCentered.h"

#include "TileSystem/TileInfoComponent.h"
#include "TileSystem/TileMapComponent.h"
#include "Encounters/EncounterComponent.h"
#include "Interactions/InteractionComponent.h"
#include "Interactions/StairComponent.h"
#include "Interactions/TalkComponent.h"
#include "Interactions/TakeComponent.h"
#include "PlayerMovementComponent.h"
#include "PlayerStatsComponent.h"
#include "InventoryComponent.h"
#include "CheatComponent.h"

using namespace BlackBoxEngine;

static std::tuple
<
    ComponentFactory::Registrar<ExampleComponent>,
    ComponentFactory::Registrar<ScreenCentered>,
    ComponentFactory::Registrar<TileInfoComponent>,
    ComponentFactory::Registrar<TileMapComponent>,
    ComponentFactory::Registrar<PlayerMovementComponent>,
    ComponentFactory::Registrar<InteractionComponent>,
    ComponentFactory::Registrar<InnTalkComponent>,
    ComponentFactory::Registrar<CastleTalkComponent>,
    ComponentFactory::Registrar<TakeComponent>,
    ComponentFactory::Registrar<EncounterComponent>,
    ComponentFactory::Registrar<BaseStairComponent>,
    ComponentFactory::Registrar<PlayerStatsComponent>,
    ComponentFactory::Registrar<InventoryComponent>,
    ComponentFactory::Registrar<PlayerCheatComponent>
> GameComponentRegistrar;
