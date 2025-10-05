#include <Actors/ComponentFactory.h>

#include "ExampleComponent.h"
#include "ScreenCentered.h"

#include "TileSystem/TileInfoComponent.h"
#include "TileSystem/TileMapComponent.h"
#include "PlayerMovementComponent.h"
#include "InteractionComponent.h"
#include "StairComponent.h"
#include "TalkComponent.h"
#include "TakeComponent.h"

using namespace BlackBoxEngine;

static std::tuple
<
    ComponentFactory::Registrar<ExampleComponent>,
    ComponentFactory::Registrar<ScreenCentered>,
    ComponentFactory::Registrar<TileInfoComponent>,
    ComponentFactory::Registrar<TileMapComponent>,
    ComponentFactory::Registrar<PlayerMovementComponent>,
    ComponentFactory::Registrar<InteractionComponent>,
    ComponentFactory::Registrar<CaveEntranceComponent>,
    ComponentFactory::Registrar<StairDownComponent>,
    ComponentFactory::Registrar<StairUpLevel1Component>,
    ComponentFactory::Registrar<StairUpLevel2Component>,
    ComponentFactory::Registrar<TalkComponent>,
    ComponentFactory::Registrar<TakeComponent>

> GameComponentRegistrar;
