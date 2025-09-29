#include <Actors/ComponentFactory.h>

#include "ExampleComponent.h"
#include "ScreenCentered.h"

#include "TileSystem/TileInfoComponent.h"
#include "TileSystem/TileMapComponent.h"
#include "PlayerMovementComponent.h"
#include "InteractionComponent.h"

using namespace BlackBoxEngine;

static std::tuple
<
    ComponentFactory::Registrar<ExampleComponent>,
    ComponentFactory::Registrar<ScreenCentered>,
    ComponentFactory::Registrar<TileInfoComponent>,
    ComponentFactory::Registrar<TileMapComponent>,
    ComponentFactory::Registrar<PlayerMovementComponent>,
    ComponentFactory::Registrar<InteractionComponent>
 // add more components like so
 //  ,ComponentFactory::Registrar<*NAME*>
> GameComponentRegistrar;
