#include <Actors/ComponentFactory.h>

#include "ExampleComponent.h"

using namespace BlackBoxEngine;

static std::tuple
<
    ComponentFactory::Registrar<ExampleComponent>
 // add more components like so
 //  ,ComponentFactory::Registrar<*NAME*>
> GameComponentRegistrar;
