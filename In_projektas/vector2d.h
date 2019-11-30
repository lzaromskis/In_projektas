/*
* Lygiagretaus programavimo
* inžinerinis projektas
* Lukas Žaromskis IFF-7/5
*
* vector.h
*/
#pragma once

namespace lukzar
{
	class Vector2D
	{
	private:
		double _x;
		double _y;

	public:
		Vector2D();
		Vector2D(double x, double y);
		Vector2D(const Vector2D &v);
		~Vector2D();
		double x() const { return this->_x; }
		double y() const { return this->_y; }
		Vector2D Normalize() const;
		double Length() const;
		Vector2D Add(const Vector2D &other) const;
		Vector2D Multiply(const double& coef) const;	
	};
}