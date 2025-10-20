#pragma once

#include <System/MessageManager.h>

static constexpr BlackBoxEngine::MessageIdType kMessageUIOpen =   BlackBoxEngine::StringHash( "UIOpened" );
static constexpr BlackBoxEngine::MessageIdType kMessageUIClosed = BlackBoxEngine::StringHash( "UIClosed" );
static constexpr BlackBoxEngine::MessageIdType kLevelChanging =   BlackBoxEngine::StringHash( "LevelChanging" );
static constexpr BlackBoxEngine::MessageIdType kLevelChangEnd =   BlackBoxEngine::StringHash( "LevelChangeEnded" );



