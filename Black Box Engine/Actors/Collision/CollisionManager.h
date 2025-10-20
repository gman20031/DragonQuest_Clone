#pragma once

#include <memory>
#include <mutex>

#include "BoundingBox.h"  
#include "AABBColliderComponent.h"
#include "../Actor.h"

#if _DEBUG
#include "../../Graphics/Color.h"
#endif

namespace BlackBoxEngine
{
    class BB_Renderer;

    class CollisionManager
    {
        friend class ColliderBorder;
        class QuadTreeNode;
#if _DEBUG
        inline static constexpr bool kDrawToScreen = true;
        inline static constexpr ColorValue kContainsColor = ColorPresets::red;
        inline static constexpr ColorValue kEmptyColor = ColorPresets::green;
        void drawNode(QuadTreeNode* pNode, BB_Renderer* pRenderer);
#endif
        inline static constexpr size_t kMaxActorsPerCell = 10;
        inline static constexpr size_t kCombineCellThreshold = kMaxActorsPerCell / 2;
        inline static constexpr size_t kMaximumDepth = 3;
        std::recursive_mutex m_collisionLocker;
        size_t m_minimumWidth;
        size_t m_minimumHeight;

        class Partition
        {
            friend class QuadTreeNode;
            std::vector<Actor*> m_pActors;
        public:
            Partition();

            void Reset();
            void AddActor(Actor* pActor);   
            void RemoveActor(Actor* pActor);
            void RunCollisionChecks() const;
            size_t Count() const { return m_pActors.size(); }
        };

        class QuadTreeNode
        {
            friend class CollisionManager;
            enum class Quadrant
            {
                kTopLeft,
                kTopRight,
                kBotLeft,
                kBotRight,
                kCount
            };
            QuadTreeNode* m_pNodes[(size_t)Quadrant::kCount] = { nullptr };
            QuadTreeNode* m_pParent = nullptr;
            Partition* m_pPartition = nullptr;
            CollisionManager* m_pManager = nullptr;
            BoundingBox m_boundingBox;

            void FreeNodes();
            void FreePartition();
            void Split();
            void Combine();
            void ExtractActors(Partition& outPartition); // recursively extracts all actors into outPartition
        public:
            QuadTreeNode(BoundingBox boundingBox, QuadTreeNode* pPrevious, CollisionManager* pManager);
            ~QuadTreeNode();

            bool IsLeaf() const { return (m_pNodes[0] == nullptr); }
            size_t ActorCount() const;
            void Clear();
            QuadTreeNode* Parent() const { return m_pParent; }

            QuadTreeNode* InsertActor(Actor* pActor);
            void RemoveActor(Actor* pActor , bool autoCombine = false);

            void RunCollisionTests();
        };
        
        QuadTreeNode* m_pRootNode;
        std::unordered_map < Actor*, QuadTreeNode*> m_actorMap;
        uint64_t m_windowChangedCallback = 0;

        void Reset() const;
        void WriteNode(std::ostream& os, QuadTreeNode* pNode, int depth); // for debugging
        bool AtMaxDepth(const QuadTreeNode* pNode) const;
        const BoundingBox& GetBorderBox() const { return m_pRootNode->m_boundingBox; }
        void SetDimensions(BB_FRectangle rect);
    public:
        CollisionManager(float x, float y, float w, float h);
        CollisionManager(BB_FRectangle bounds);
        ~CollisionManager();
        CollisionManager(const CollisionManager&) = delete;
        void operator=(const CollisionManager&) = delete;

        void RegisterActor(Actor* pActor);
        void RemoveActor(Actor* pActor);
        void NotifyActorMoved(Actor* pActor);

#ifdef _DEBUG
        void DumpToFile(const char* pFilePath);
        void DebugDraw();
#endif // _DEBUG

    };

}