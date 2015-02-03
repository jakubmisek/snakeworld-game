#ifndef _POSITION_H_
#define _POSITION_H_

class CVector
{
public:
	CVector(double x, double y);
	CVector(CVector&vec);
	
	double x,y;

	void	Add( CVector&vec );
	void	Sub( CVector&vec );

	double	Distance( CVector&vec );
	double	Length();

	void	Normalize();

	CVector operator * (double scalar);
	CVector operator + (CVector&vec);
	CVector operator - (CVector&vec);

	CVector Rotate( CVector&cossin );
};

#endif