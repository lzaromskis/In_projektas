/*
* Lygiagretaus programavimo
* inžinerinis projektas
* Lukas Žaromskis IFF-7/5
*
* vector.cpp
*/

#include "vector2d.h"
#include <math.h>

namespace lukzar
{
	Vector2D::Vector2D()
	{
		this->_x = 0.0;
		this->_y = 0.0;
	}

	Vector2D::Vector2D(double x, double y)
	{
		this->_x = x;
		this->_y = y;
	}

	Vector2D::Vector2D(const Vector2D &v)
	{
		this->_x = v._x;
		this->_y = v._y;
	}

	Vector2D::~Vector2D()
	{

	}

	Vector2D Vector2D::Normalize() const
	{
		double length = this->Length();
		if (length > 0.0)
		{
			double mult = 1.0 / length;
			return Vector2D(this->_x * mult, this->_y * mult);
		}
		return Vector2D(this->_x, this->_y);
	}

	double Vector2D::Length() const
	{
		return sqrt(pow(this->_x, 2.0) + pow(this->_y, 2.0));
	}

	 Vector2D Vector2D::Add(const Vector2D& other) const
	 {
		 return Vector2D(this->_x + other._x, this->_y + other._y);
	 }

	 Vector2D Vector2D::Multiply(const double& coef) const
	 {
		 return Vector2D(this->_x * coef, this->_y * coef);
	 }
}