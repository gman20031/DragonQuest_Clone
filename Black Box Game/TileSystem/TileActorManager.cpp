#include "TileActorManager.h"

#include <Resources/XML/BlackBoxXMLParser.h>
#include <Resources/ResourceManager.h>
#include <BlackBoxManager.h>
#include <System/Log.h>

#include "TileInfoComponent.h"

using namespace BlackBoxEngine;

void TileActorManager::CheckForTileComponent(const ActorPtr& pActor)
{
    auto* pTileComponent = pActor->GetComponent<TileInfoComponent>();
    if (!pTileComponent)
        BB_LOG(LogType::kWarning, "Loaded tile actor does not contain TileInfoComponent, is this a tile? id:", (char)pActor->GetId() );
}

TileActorManager::TileActorManager()
{
    XMLElementParser elementParser = ResourceManager::GetRawXMLDATA(kTileLookupPath);

    const char* pFilePath = nullptr;
    elementParser.GetChildElement("tile");
    while (elementParser)
    {
        elementParser.GetText(&pFilePath);
        if (!pFilePath)
        {
            BB_LOG(LogType::kFailure, "Error loadiing in Tile Actors");
            break;
        }

        const char* pName = elementParser.GetFirstAttribute();
        m_highestId = static_cast<Actor::Id>(pName[0]);
        const auto& pActor = LoadActor(pFilePath);
        CheckForTileComponent(pActor);

        elementParser = elementParser.GetSibling();
    }
}

std::shared_ptr<BB_Texture> TileActorManager::GetTileTexture(BlackBoxEngine::Actor::Id id)
{
    const auto& actor = GetActor(id);
    auto* pComponent = actor->GetComponent<TileInfoComponent>();
    if (!pComponent)
    {
        BB_LOG(LogType::kError, "Tile actor did not have tileInfo Component, returning nullptr texture, id:", id);
        return nullptr;
    }
    return pComponent->GetTexture();
}
