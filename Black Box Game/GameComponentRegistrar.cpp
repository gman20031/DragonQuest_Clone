#include <Actors/ComponentFactory.h>

#include "ExampleComponent.h"
#include "ScreenCentered.h"

using namespace BlackBoxEngine;

static std::tuple
<
    ComponentFactory::Registrar<ExampleComponent>,
    ComponentFactory::Registrar<ScreenCentered>
 // add more components like so
 //  ,ComponentFactory::Registrar<*NAME*>
> GameComponentRegistrar;
