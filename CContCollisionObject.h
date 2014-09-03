#ifndef CContCOLLISIONOBJECT_H
#define CContCOLLISIONOBJECT_H
#include "SVector2D.h"
#include "CCollisionObject.h"
class CContCollisionObject: public CCollisionObject
{
public:
	CContCollisionObject();
	CContCollisionObject(ObjectType objectType, SVector2D<double> position);
	virtual ~CContCollisionObject();

	void setPosition(SVector2D<double> position);
	SVector2D<double> getPosition() const;
	bool isTargeted();
	void setTargeted(bool value);
	
private:
	SVector2D<double> * m_vPosition;
	bool targeted = false;
};

#endif