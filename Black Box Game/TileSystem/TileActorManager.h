#pragma once

#include <unordered_map>
#include <Actors/ActorManager.h>
#include <Graphics/Texture.h>

class TileActorManager : public BlackBoxEngine::ActorManager
{
public:
    using ActorPtr = std::unique_ptr<BlackBoxEngine::Actor>;
    inline static const char* kTileLookupPath = "../Assets/Tiles/TileLookup.xml";
private:
    void CheckForTileComponent(const ActorPtr& pActor);

public:
    TileActorManager();

    std::shared_ptr<BlackBoxEngine::BB_Texture> GetTileTexture(BlackBoxEngine::Actor::Id id);

};
