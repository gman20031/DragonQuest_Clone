#include "CollisionManager.h"

#include <cassert>
#include <fstream>

#include "../../BlackBoxManager.h"
#include "../../System/Log.h"
#include "../Actor.h"
#include "BoundingBox.h"

namespace BlackBoxEngine
{

    static const BoundingBox GetBoundingBox(Actor* pActor)
    {
        assert(pActor);
        auto* pComponent = pActor->GetComponent<AABBColliderComponent>();
        assert(pComponent);
        return pComponent->GetCollider();
    }

    ////////////////////////////////////////////////////////////////////
    /// Parition
    ////////////////////////////////////////////////////////////////////

    CollisionManager::Partition::Partition()
    {
        m_pActors.reserve(kMaxActorsPerCell + 1);
    }

    void CollisionManager::Partition::Reset()
    {
        m_pActors.clear();
    }

    void CollisionManager::Partition::AddActor(Actor* pActor)
    {
        assert(pActor);
        m_pActors.emplace_back(pActor);
    }

    void CollisionManager::Partition::RemoveActor(Actor* pActor)
    {
        for (auto it = m_pActors.begin(); it != m_pActors.end(); ++it)
        {
            if (**it == *pActor)
            {
                *it = m_pActors.back();
                m_pActors.pop_back();
                return;
            }
        }
    }

    void CollisionManager::Partition::RunCollisionChecks() const
    {
        auto* pMessageSystem = BlackBoxManager::Get()->m_pMessagingManager;
        const size_t size = m_pActors.size();
        for (size_t outerIndex = 0; outerIndex < size; ++outerIndex)
        {
            for (size_t innerIndex = outerIndex + 1; innerIndex < size; ++innerIndex)
            {
                // don't collide with ourself
                if (innerIndex == outerIndex)
                    continue;
                const auto& innerBounding = GetBoundingBox(m_pActors[innerIndex]);
                const auto& outerBounding = GetBoundingBox(m_pActors[outerIndex]);
                if ( outerBounding.Intersects(innerBounding) )
                {
                    pMessageSystem->EnqueueMessage(EngineMessages::kCollided, m_pActors[innerIndex], m_pActors[outerIndex]);
                    m_pActors[innerIndex]->Collided(m_pActors[outerIndex]);
                    m_pActors[outerIndex]->Collided(m_pActors[innerIndex]);
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////
    /// Quad Tree Node
    ////////////////////////////////////////////////////////////////////

    void CollisionManager::QuadTreeNode::FreeNodes()
    {
        for (QuadTreeNode* pNode : m_pNodes)
        {
            if (pNode)
            {
                delete pNode;
                pNode = nullptr;
            }
        }
    }

    void CollisionManager::QuadTreeNode::FreePartition()
    {
        if (m_pPartition)
        {
            delete m_pPartition;
            m_pPartition = nullptr;
        }
    }

    void CollisionManager::QuadTreeNode::Split()
    {
        assert(IsLeaf() && m_pPartition && m_pManager);
        using enum Quadrant;

        // create the bounding box for the subNodes
        BoundingBox outBox = m_boundingBox;
        outBox.m_rect.w /= 2;
        outBox.m_rect.h /= 2;

        const float halfWidth = m_boundingBox.m_rect.w / 2;
        const float halfHeight = m_boundingBox.m_rect.h / 2;

        // construct subNodes
        m_pNodes[(size_t)kTopLeft]  = new QuadTreeNode(outBox, this , m_pManager);
        outBox.m_rect.x += halfWidth;
        m_pNodes[(size_t)kTopRight] = new QuadTreeNode(outBox, this , m_pManager);
        outBox.m_rect.y += halfHeight;
        m_pNodes[(size_t)kBotRight] = new QuadTreeNode(outBox, this , m_pManager);
        outBox.m_rect.x -= halfWidth;
        m_pNodes[(size_t)kBotLeft]  = new QuadTreeNode(outBox, this , m_pManager);
        
        // move the pointers to their correct subNodes
        for (Actor* pActor : m_pPartition->m_pActors)
            InsertActor(pActor);
        FreePartition();
    }

    void CollisionManager::QuadTreeNode::Combine()
    {
        assert(!IsLeaf());
        if (!m_pPartition)
            m_pPartition = new Partition;
        ExtractActors(*m_pPartition);
        FreeNodes();
    }

    void CollisionManager::QuadTreeNode::ExtractActors(Partition& outPartition)
    {
        if (!IsLeaf())
        {
            for (QuadTreeNode* pNode : m_pNodes)
                pNode->ExtractActors(outPartition);
        }
        else if (m_pPartition)
#if __cplusplus > 202002L   // for c++23
            outPartition.m_pActors.append_range(m_partition->m_pActors);
#else
            outPartition.m_pActors.insert(outPartition.m_pActors.begin(), m_pPartition->m_pActors.begin(), m_pPartition->m_pActors.end());//cpp 20
#endif
    }

    CollisionManager::QuadTreeNode::QuadTreeNode(BoundingBox boundingBox, QuadTreeNode* pPrevious, CollisionManager* pManager)
         : m_boundingBox(boundingBox)
         , m_pParent(pPrevious)
    {
        m_pManager = pManager;
    }

    CollisionManager::QuadTreeNode::~QuadTreeNode()
    {
        FreeNodes();
        FreePartition();
    }

    size_t CollisionManager::QuadTreeNode::ActorCount() const
    {
        size_t count = 0;
        if (m_pPartition)
            count = m_pPartition->Count();
        else if (!IsLeaf())
        {
            for (auto& pNode : m_pNodes)
            {
                count += pNode->ActorCount();
            }
        }
        return count;
    }

    void CollisionManager::QuadTreeNode::Clear()
    {
        if (!m_pPartition)
            return;

        if (IsLeaf())
            m_pPartition->Reset();
        else
        {
            for (auto& pNode : m_pNodes)
            {
                pNode->Clear();
            }
        }
    }

    // TODO: SIMPLIFY . Too much nesting, too many conditionals.
    CollisionManager::QuadTreeNode* CollisionManager::QuadTreeNode::InsertActor(Actor* pActor)
    {
        const BoundingBox& actorBoundingBox = GetBoundingBox(pActor);
        //assert(m_boundingBox.Intersects(box));
        if (!m_boundingBox.Intersects(actorBoundingBox))
        {
            BB_LOG(LogType::kError, "Attempted to insert an actor into a cell that is not intersecting the actor");
            return nullptr;
        }

        QuadTreeNode* pOut = nullptr;
        if (IsLeaf())
        {   
            if (!m_pPartition)
                m_pPartition = new Partition;

            if (m_pPartition->Count() >= kMaxActorsPerCell && !m_pManager->AtMaxDepth(this) )
            {
                Split();
                pOut = InsertActor(pActor);
            }
            else
            {
                m_pPartition->AddActor(pActor);
                pOut = this;
            }
        }
        else 
        {
            for (auto& pNode : m_pNodes)
            {
                // can be added to 1 or more nodes.
                if (pNode->m_boundingBox.Intersects(actorBoundingBox))
                {
                    auto* pInserted = pNode->InsertActor(pActor);

                    if (pOut)
                        pOut = this;
                    else
                        pOut = pInserted;
                }
            }
        }
        m_pManager->m_actorMap[pActor] = pOut;
        return pOut;
    }

    void CollisionManager::QuadTreeNode::RemoveActor(Actor* pActor , bool autoCombine)
    {
        if (IsLeaf())
        {
            if(m_pPartition)
                m_pPartition->RemoveActor(pActor);
            return;
        }
        for( auto& pNode : m_pNodes)
        {
            pNode->RemoveActor(pActor);
        }
        if (autoCombine && ActorCount() < kCombineCellThreshold)
            Combine();
    }

    void CollisionManager::QuadTreeNode::RunCollisionTests()
    {
        if (!IsLeaf())
        {
            for (auto& pNode : m_pNodes)
            {
                pNode->RunCollisionTests();
            }
            return;
        }
        if (m_pPartition)
        {
            m_pPartition->RunCollisionChecks();
            return;
        }
    }

    ////////////////////////////////////////////////////////////////////
    /// Collision Manager
    ////////////////////////////////////////////////////////////////////

    /////////////// private


    void CollisionManager::Reset() const
    {
        m_pRootNode->Clear();
    }

    void CollisionManager::WriteNode(std::ostream& os, QuadTreeNode* pNode, int depth)
    {
        using enum QuadTreeNode::Quadrant;
        auto addPadding = [depth, &os]()
            {
                for (size_t i = 0; i < depth; ++i)
                    os << "  ";
            };

        if (pNode->IsLeaf())
        {
            const auto& rect = pNode->m_boundingBox.m_rect;
            addPadding();
            os << "Bounds(" << rect.x << ',' << rect.y << ',' << rect.w << ',' << rect.h << ") | Count " << pNode->ActorCount();
        }
        else
        {
            ++depth;
            addPadding();
            os << "<Top Left>\n";
            WriteNode(os, pNode->m_pNodes[(size_t)kTopLeft], depth);
            addPadding();
            os << "<Top Right>\n";
            WriteNode(os, pNode->m_pNodes[(size_t)kTopRight], depth);
            addPadding();
            os << "<Bot Right>\n";
            WriteNode(os, pNode->m_pNodes[(size_t)kBotRight], depth);
            addPadding();
            os << "<Bot Left>\n";
            WriteNode(os, pNode->m_pNodes[(size_t)kBotLeft], depth);
        }
        os << '\n';
    }

    bool CollisionManager::AtMaxDepth(const QuadTreeNode* pNode) const
    {
        return (
            pNode->m_boundingBox.m_rect.w < m_minimumWidth &&
            pNode->m_boundingBox.m_rect.h < m_minimumHeight
        );
    }

    void CollisionManager::SetDimensions(BB_FRectangle bounds)
    {
        m_minimumHeight = (size_t)bounds.h;
        m_minimumWidth = (size_t)bounds.w;
        for (size_t i = 0; i < kMaximumDepth; ++i)
        {
            m_minimumHeight /= 2;
            m_minimumWidth /= 2;
        }
    }

    /////////////// public

    CollisionManager::CollisionManager(BB_FRectangle bounds)
        : m_pRootNode( new QuadTreeNode(bounds, nullptr, this) )
    {
        //auto resetCollisionBounds = [this](Message& message) -> void
        //    {
        //        BB_IntRectangle dims = BlackBoxManager::Get()->GetWindow()->GetRenderBox();
        //    };
        //m_windowChangedCallback = BlackBoxManager::Get()->m_pMessagingManager->RegisterListenerString
        //("WindowSizeChanged", resetCollisionBounds);

        SetDimensions(bounds);
    }

    CollisionManager::CollisionManager(float x, float y, float w, float h)
        : CollisionManager( BB_FRectangle{x,y,w,h} )
    {

    }

    CollisionManager::~CollisionManager()
    {
        assert(m_pRootNode);
        delete m_pRootNode;
        BlackBoxManager::Get()->m_pMessagingManager->RemoveListener(m_windowChangedCallback);
    }

    void CollisionManager::RegisterActor(Actor * pActor)
    {
        assert(pActor);
        std::unique_lock lock( m_collisionLocker );
        QuadTreeNode* pNode = m_pRootNode->InsertActor(pActor);
     
        if ( !pNode )
            BB_LOG(LogType::kError, "Failed to register actor into collision manager");
        
        m_actorMap[pActor] = pNode;
    }

    // TODO : MAKE THIS A SIMPILER/SMALLER FUNCTION
    void CollisionManager::NotifyActorMoved(Actor* pActor)
    {
        auto it = m_actorMap.find(pActor);
        if (it == m_actorMap.end())
        {
            BB_LOG(LogType::kError, "Notified actor moved that wasnt registered to collision manager");
            return;
        }
        std::unique_lock lock( m_collisionLocker );
        QuadTreeNode* pCurrentOwningNode = it->second;
        QuadTreeNode* pNewOwningNode = pCurrentOwningNode;
        const auto& actorBounding = GetBoundingBox(pActor);

        // if the actor walked off the screen and is 100% out of the bounds of this collision manager
        if (!pCurrentOwningNode)
        {
            if ( m_pRootNode->m_boundingBox.Intersects( actorBounding ))
            {
                pNewOwningNode = m_pRootNode->InsertActor(pActor);
                pNewOwningNode->RunCollisionTests();
            }
            return;
        }

        // remove the actor from all nodes that would contain this actor
        pCurrentOwningNode->RemoveActor(pActor, true);

        // step back through the tree to find the highest node that fully contains this actor or are our of bounds.
        // this includes the original node, 
        while (pNewOwningNode && ! pNewOwningNode->m_boundingBox.Contains(actorBounding))
            pNewOwningNode = pNewOwningNode->Parent();

        // if the actor walked off the map
        if (!pNewOwningNode)
        {
            if (m_pRootNode->m_boundingBox.Intersects(actorBounding)) // still some part on the map.
                pNewOwningNode = m_pRootNode;
            else
            {
                m_actorMap[pActor] = pNewOwningNode;
                return; // no need to check collision;
            }
        }

        // Now we know exactly where the actor has moved too, move the actor to the correct spot.
        pNewOwningNode = pNewOwningNode->InsertActor(pActor);
        pNewOwningNode->RunCollisionTests();
    }

    void CollisionManager::RemoveActor(Actor* pActor)
    {
        std::unique_lock lock( m_collisionLocker );
        auto it = m_actorMap.find(pActor);
        if (it == m_actorMap.end())
        {
            BB_LOG(LogType::kError, "Attempted to remove actor that was not added");
            return;
        }
        it->second->RemoveActor(it->first, true);
        m_actorMap.erase(it);
    }

#ifdef _DEBUG
    void CollisionManager::DumpToFile(const char* pFilePath)
    {
        std::fstream file;
        file.open(pFilePath, std::ios::out);
        assert(file.is_open());
        file << "<Root>\n";
        WriteNode(file , m_pRootNode, 1);
        file.close();
    }

    void CollisionManager::drawNode(QuadTreeNode* pNode,BB_Renderer* pRenderer)
    {
        if (pNode->IsLeaf())
        {
            if (pNode->ActorCount() > 0)
                pRenderer->SetDrawColor(kContainsColor);
            else
                pRenderer->SetDrawColor(kEmptyColor);

            pRenderer->DrawRectGame(pNode->m_boundingBox.m_rect);
        }
        else
        {
            for (auto* pSubNode : pNode->m_pNodes)
                drawNode(pSubNode, pRenderer);
        }

    }

    void CollisionManager::DebugDraw()
    {
        if constexpr (!kDrawToScreen)
            return;
        auto* pRenderer = BlackBoxManager::Get()->GetWindow()->GetRenderer();
        drawNode(m_pRootNode, pRenderer);
    }

#endif _DEBUG
}

