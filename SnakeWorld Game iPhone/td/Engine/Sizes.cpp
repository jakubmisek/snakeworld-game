#include "stdafx.h"
#include "Sizes.h"

TAPoint::TAPoint()
{
	//x.setContainer(this);
	//x.setter(&Point::setX);
	//x.getter(&Point::getX);
	//y.setContainer(this);
	//y.setter(&Point::setY);
	//y.getter(&Point::getY);
	//z.setContainer(this);
	//z.setter(&Point::setZ);
	//z.getter(&Point::getZ);
}

TASize::TASize()
{
	//w.setContainer(this);
	//w.setter(&Size::setW);
	//w.getter(&Size::getW);
	//h.setContainer(this);
	//h.setter(&Size::setH);
	//h.getter(&Size::getH);
}


TAColor::TAColor()
{
	r.setValue(1.0f);
	g.setValue(1.0f);
	b.setValue(1.0f);
	a.setValue(1.0f);
}

void TAColor::SetColor( float rval, float gval, float bval, float aval )
{
	r.setValue(rval);
	g.setValue(gval);
	b.setValue(bval);
	a.setValue(aval);
}