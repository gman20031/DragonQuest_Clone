#include "InventoryComponent.h"



void InventoryComponent::Load(const BlackBoxEngine::XMLElementParser parser)
{
	parser.GetChildVariable("HasScroll", &m_hasTablet);
	parser.GetChildVariable("HasTorch", &m_hasTorch);
	parser.GetChildVariable("HasClub", &m_hasClub);
	parser.GetChildVariable("HasClothes", &m_hasLeatherClothes);
}

void InventoryComponent::Save(BlackBoxEngine::XMLElementParser parser)
{
	parser.NewChildVariable("HasScroll", m_hasTablet);
	parser.NewChildVariable("HasTorch", m_hasTorch);
	parser.NewChildVariable("HasClub", m_hasClub);
	parser.NewChildVariable("HasClothes", m_hasLeatherClothes);
}
