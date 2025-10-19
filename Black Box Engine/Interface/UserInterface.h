#pragma once

#include <vector>

#include "../Graphics/RenderingStructs.h"
#include "../Graphics/Color.h"
#include "../System/Direction.h"
#include "../Input/InputManager.h"

namespace BlackBoxEngine
{
    class Sprite;
    class InterfaceNode;
    class BB_Renderer;
    class BB_Texture;

    class InterfaceHighlighter
    {
    public:
        inline static constexpr uint8_t kModeIcon      = 0b0001;
        inline static constexpr uint8_t kModeColored   = 0b0010;
        inline static constexpr uint8_t kModeUnderline = 0b0100;
        inline static constexpr uint8_t kModeAll       = 0b1111;
        inline static constexpr uint8_t kModeNone      = 0b0000;
        struct Parameters
        {
            uint8_t mode = kModeColored;

            const char* pSpriteFile = nullptr;
            BB_FPoint iconOffset{ 0,0 };
            BB_FPoint iconSize{ 0,0 };

            ColorValue highlighterColor{ 0,0,0,0 };

            float lineWidth = 0;
            ColorValue underlineColor{ 0,0,0,0 };
        };
    private:
        Parameters m_params;
        BB_FRectangle m_iconDestination{ 0,0,0,0 };
        BB_FRectangle m_renderDestination{ 0, 0, 0, 0 };

        Sprite* m_pSprite = nullptr;
        UserInterface* m_pAttachedInterface = nullptr;
        InterfaceNode* m_pHighlightedNode = nullptr;
    private:
        void UpdateRenderPosition(InterfaceNode* pNode);

        void RenderUnderline(BB_Renderer* pRenderer) const;
        void RenderColor(BB_Renderer* pRenderer) const;
    public:
        InterfaceHighlighter(UserInterface* pAttachedInterface);
        ~InterfaceHighlighter();
        const InterfaceHighlighter& operator=( InterfaceHighlighter&& ) = delete;
        const InterfaceHighlighter& operator=( const InterfaceHighlighter& ) = delete;
        InterfaceHighlighter( InterfaceHighlighter&& ) = delete;
        InterfaceHighlighter( const InterfaceHighlighter& ) = delete;

        void Render(BB_Renderer* pRenderer) const;
        void Start();
        void Stop();

        void SetTarget(InterfaceNode* pNode);
        void SetMode(uint8_t mode);
        void SetParameters( const Parameters& params);
        Sprite* GetSprite() { return m_pSprite; }
    };

    class UserInterface
    {
    public:
        struct InterfaceKeys
        {
            KeyCode up = KeyCode::kUp;
            KeyCode down = KeyCode::kDown;
            KeyCode right = KeyCode::kRight;
            KeyCode left = KeyCode::kLeft;
            KeyCode select = KeyCode::kReturn;
        };

    private:
        InterfaceHighlighter m_highlighter;
        InterfaceKeys m_keycodes;

        InputManager::InputTarget* m_pInputTarget = nullptr;
        InterfaceNode* m_pRootNode = nullptr;
        InterfaceNode* m_pCursorPosition = nullptr;
        int32_t m_managerId = -1;

        void MoveCursor(Direction dir);
        void SelectTargetedNode();
        void DeSelectTargetNode();
        void SetupKeysForInputTarget();
        void Start();
        void Stop();
    public:
        UserInterface();
        ~UserInterface();
        UserInterface(const UserInterface&) = delete;
        UserInterface(UserInterface&&) = default;
        UserInterface& operator=(const UserInterface&) = delete;
        UserInterface& operator=(UserInterface&&) = default;

        InputManager::InputTarget* GetInputTarget();
        InterfaceHighlighter* GetHighlight() { return &m_highlighter; };

        void SetInterfaceKeys(const InterfaceKeys& keys);
        void SetInterfaceKeys(KeyCode up, KeyCode down, KeyCode right, KeyCode left, KeyCode select);
    
        template<typename NodeType, typename... Args>
        NodeType* AddNode(const char* pName, BB_FRectangle dimensions , Args&&... args);
        InterfaceNode* FindNode(const char* pName);
        InterfaceNode* GetRoot() { return m_pRootNode; }

        void SetCursorTarget(InterfaceNode* pNode);
        void AddToScreen();
        void RemoveFromScreen();

        void Update();
        void Render(BB_Renderer* pRenderer);
    };

    class InterfaceNode
    {
    private:
        InterfaceNode* m_adjacentNodes[4] = {};
    
    protected:
        InterfaceNode* m_pParent = nullptr;
        std::vector<InterfaceNode*> m_childNodes;

        uint32_t m_nameHash;
        BB_FRectangle m_nodeRenderRect;

        // Called when the interface is added to screen
        virtual void StartThis() {/*EMPTY*/ };
        // Called when the interface is removed from screen
        virtual void StopThis() {/*EMPTY*/};
        virtual void UpdateThis() {/*EMPTY*/};
        virtual void RenderThis([[maybe_unused]]BB_Renderer* pRenderer, [[maybe_unused]] float rootX, [[maybe_unused]] float rootY) {/*EMPTY*/};
    public:
        InterfaceNode(InterfaceNode* pParent, const char* pName);
        InterfaceNode(InterfaceNode* pParent, const char* pName, BB_FRectangle dimensions);
        virtual ~InterfaceNode();
        InterfaceNode(const InterfaceNode&) = delete;
        InterfaceNode(InterfaceNode&&) = default;
        InterfaceNode& operator=(const InterfaceNode&) = delete;
        InterfaceNode& operator=(InterfaceNode&&) = default;

        BB_FRectangle GetRenderBox() const { return m_nodeRenderRect; }
        void SetDimensions(const BB_FRectangle& dimensions) { m_nodeRenderRect = dimensions; };
        void SetOffset(float x, float y);
        void SetSize(float w, float h);

        // Called when the interface is added to screen
        void Start();
        // Called when the interface is removed from screen
        void Stop();
        void Update();
        void Render(BB_Renderer* pRenderer, float rootX, float rootY);

        InterfaceNode* GetAdjacentNode(Direction dir) const;
        void SetAdjacentNode(Direction dir, InterfaceNode* pNode);
        
        template<typename NodeType, typename... Args>
        NodeType* MakeChildNode(const char* pName, BB_FRectangle dimensions, Args&&... args);
        InterfaceNode* GetChildNode(const char* pName);
        InterfaceNode* GetChildNode(uint32_t nameHash);
        void RemoveAllChildNodes();
        void RemoveChildNode( InterfaceNode* pChildNode );
        void RemoveChildNode( const char* pName );
        void RemoveChildNode( uint32_t nameHash );

        BB_FPoint GetScreenPosition();

        virtual void OnTargeted() {};
        virtual void OnTargetedStop() { };
        virtual void OnInteracted() { };
        virtual void OnInteractStop() {};
    };

    //class DynamicInterfaceGroup : public InterfaceNode
    //{
    //public:
    //    float m_yPad = 0;
    //    float m_xPad = 0;
    //    Direction m_grownDirection = Direction::kDown;
    //    BB_AnchorPoint m_anchorPoint = BB_AnchorPoint::kCenterTrue;
    //public:

    //    void AddNewNode()
    //};

    template<typename NodeType, typename ...Args>
    inline NodeType* UserInterface::AddNode(const char* pName, BB_FRectangle dimensions, Args&& ...args)
    {
        if(m_pRootNode)
            return m_pRootNode->MakeChildNode<NodeType>(pName, dimensions, std::forward<Args>(args)...);
        return nullptr;
    }

    template<typename NodeType, typename ...Args>
    inline NodeType* InterfaceNode::MakeChildNode(const char* pName, BB_FRectangle dimensions, Args&& ...args)
    {
        NodeType* pNode = new NodeType(this, pName, dimensions, std::forward<Args>(args)...);
        m_childNodes.emplace_back(pNode);
        return pNode;
    }

};