#pragma once
class Vector2
{
public:
	int x;
	int y;

	Vector2() : x(0), y(0) {}
	Vector2(int xVal, int yVal) : x(xVal), y(yVal) {}
	~Vector2() {}

	Vector2 operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}
	Vector2 operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}
	Vector2 operator-() const
	{

		return Vector2(-x, -y);
	}
	Vector2 operator*(int scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}
	Vector2 operator*(const Vector2& other) const
	{
		return Vector2(x * other.x, y * other.y);
	}
	Vector2 operator/(int scalar) const
	{
		return Vector2(x / scalar, y / scalar);
	}
	Vector2 operator/(const Vector2& other) const
	{
		return Vector2(x / other.x, y / other.y);
	}
	Vector2& operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	Vector2& operator-=(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}
	Vector2& operator*=(int scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}
	Vector2& operator*=(const Vector2& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}
	Vector2& operator/=(int scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}
	Vector2& operator/=(const Vector2& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}
private:

};
