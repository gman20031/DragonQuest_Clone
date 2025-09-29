#pragma once

#include <vector>

#include "../Graphics/RenderingStructs.h"
#include "../Graphics/Color.h"
#include "../System/Direction.h"
#include "../Input/InputManager.h"

namespace BlackBoxEngine
{
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
        struct TextureInfo
        {
            uint8_t m_mode = kModeColored;

            const char* m_pIconFile = nullptr;
            BB_FPoint m_iconOffset{ 0,0 };
            BB_FPoint m_iconSize{ 0,0 };

            ColorValue m_highlightColor{ 0,0,0,0 };

            float m_lineWidth = 0;
            ColorValue m_underlineColor{ 0,0,0,0 };
        };
    private:
        TextureInfo m_params;
        BB_FRectangle m_iconDestination{ 0,0,0,0 };
        BB_FRectangle m_renderDestination{ 0, 0, 0, 0 };

        std::shared_ptr<BB_Texture> m_pIconTexture = nullptr;
        UserInterface* m_pAttachedInterface = nullptr;
        InterfaceNode* m_pHighlightedNode = nullptr;
    private:
        void UpdateRenderPosition(InterfaceNode* pNode);

        void RenderIcon(BB_Renderer* pRenderer) const;
        void RenderUnderline(BB_Renderer* pRenderer) const;
        void RenderColor(BB_Renderer* pRenderer) const;
    public:
        InterfaceHighlighter(UserInterface* pAttachedInterface);
        void Render(BB_Renderer* pRenderer) const;

        void SetTarget(InterfaceNode* pNode);
        void SetMode(uint8_t mode);
        void SetParmeters(TextureInfo params);
    };

    class UserInterface
    {
    public:
        struct InterfaceKeys
        {
            KeyCode m_up = KeyCode::kUp;
            KeyCode m_down = KeyCode::kDown;
            KeyCode m_right = KeyCode::kRight;
            KeyCode m_left = KeyCode::kLeft;
            KeyCode m_select = KeyCode::kReturn;
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

        void Start();
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

        virtual void StartThis() {/*EMPTY*/};
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

        void Start();
        void Update();
        void Render(BB_Renderer* pRenderer, float rootX, float rootY);

        InterfaceNode* GetAdjacentNode(Direction dir) const;
        void SetAdjacentNode(Direction dir, InterfaceNode* pNode);
        
        template<typename NodeType, typename... Args>
        NodeType* MakeChildNode(const char* pName, BB_FRectangle dimensions, Args&&... args);
        InterfaceNode* GetChildNode(const char* pName);
        InterfaceNode* GetChildNode(uint32_t nameHash);

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