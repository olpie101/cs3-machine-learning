#include "CContCollisionObject.h"
#include "SVector2D.h"
CContCollisionObject::CContCollisionObject() : CCollisionObject(ObjectType::Mine), targeted(false)
{
	m_vPosition = new SVector2D<double>(0,0);
}
CContCollisionObject::CContCollisionObject(ObjectType objectType, SVector2D<double> position) : CCollisionObject(objectType), targeted(false)
{
	m_vPosition = new SVector2D<double>(position.x,position.y);
}
CContCollisionObject::~CContCollisionObject()
{
	delete m_vPosition;
}

void CContCollisionObject::setPosition(SVector2D<double> position)
{
	m_vPosition->x = position.x;
	m_vPosition->y = position.y;	
}
SVector2D<double> CContCollisionObject::getPosition() const
{
	return *m_vPosition;
}

bool CContCollisionObject::isTargeted(){ return targeted; }

void CContCollisionObject::setTargeted(bool value){ targeted = value; }