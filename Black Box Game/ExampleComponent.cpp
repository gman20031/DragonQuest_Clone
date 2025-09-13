#include "ExampleComponent.h"

#include <System/Log.h>

using namespace BlackBoxEngine;

void ExampleComponent::Start()
{
    // when this is created or the game is started.
    BB_LOG(LogType::kMessage, "Example Component Started");
}

void ExampleComponent::Update()
{
    // every tick

}

void ExampleComponent::Render()
{
    // to add to rendering

}

void ExampleComponent::OnCollide([[maybe_unused]] Actor* pOtherActor)
{
    // if this actor has a collider, and walks into another actor with a collider
    BB_LOG(LogType::kMessage, "Example Component Collided");

}

void ExampleComponent::Save( XMLElementParser parser)
{
    // how this actor should be saved as XML
    BB_LOG(LogType::kMessage, "Example Component Saved");

    parser.NewChildVariable("m_bar", m_bar);
    parser.NewChildVariable("m_baz", m_baz);
    parser.NewChildVariable("m_foo", m_foo); // std::strings dont *need* to be converted to c_strings first

}

void ExampleComponent::Load(const XMLElementParser parser)
{
    // how this actor should be loaded as XML
    BB_LOG(LogType::kMessage, "Example Component Loaded");

    parser.GetChildVariable("m_bar", &m_bar); // loaded by passing in the pointer to the member variable
    parser.GetChildVariable("m_baz", &m_baz); // even strings
    parser.GetChildVariable("m_foo", &m_foo); // and std::strings

}
