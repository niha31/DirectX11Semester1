#include "Vector3.h"

Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(float x1, float y1, float z1)
{
	x = x1;
	y = y1;
	z = z1;
}

Vector3::Vector3(const Vector3& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

Vector3 Vector3::operator+(const Vector3& vec)
{
	//Returns a new vector summing the values for each component with the 
	//corresponding component in the added vector

	return Vector3(x + vec.x, y + vec.y, z + vec.z);
}

Vector3& Vector3::operator+=(const Vector3& vec)
{
	//Returns ‘this’ pointer (i.e. self-reference summing the values for 
	//each component with the corresponding component in the added vector

	x += vec.x;
	y += vec.y;
	z += vec.z;

	return *this;
}

Vector3 Vector3::operator-(const Vector3& vec)
{
	return Vector3(x - vec.x, y - vec.y, z - vec.z);
}

Vector3& Vector3::operator-=(const Vector3& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;

	return *this;
}

Vector3 Vector3::operator*(float value)
{
	return Vector3(x * value, y * value, z * value);
}

Vector3& Vector3::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;

	return *this;
}

Vector3 Vector3::operator/(float value)
{
	assert(value != 0); //prevent devide by 0

	return Vector3(x / value, y / value, z / value);
}

Vector3& Vector3::operator/=(float value)
{
	assert(value != 0);

	x /= value;
	y /= value;
	z /= value;

	return *this;
}

Vector3& Vector3::operator=(const Vector3& vec)
{
	return Vector3(x = vec.x, y = vec.y, z = vec.z);
}

float Vector3::dot_product(const Vector3& vec)
{
	return (x * vec.x + y * vec.y + z * vec.z);
}

Vector3 Vector3::cross_product(const Vector3& vec)
{
	float ni = y * vec.z - z * vec.y;
	float nj = z * vec.x - x * vec.z;
	float nk = x * vec.y - y * vec.x;

	return Vector3(ni, nj, nk);
}

float Vector3::magnitude()
{
	return sqrt((x * x) + (y * y) + (z * z));
}

float Vector3::square()
{
	return x * x + y * y + z * z;
}

Vector3 Vector3::normalization()
{
	float mag = magnitude();
	return Vector3(x / mag, y / mag, z / mag);

}

float Vector3::distance(const Vector3& vec)
{
	return sqrt(pow(vec.x - this->x, 2) + pow(vec.y - this->y, 2) + pow(vec.y - this->y, 2));
}

float Vector3::show_X()
{
	return x;
}
float Vector3::show_Y()
{
	return y;
}
float Vector3::show_Z()
{
	return z;
}
void Vector3::disp()
{
	cout << x << " " << y << " " << z << endl;
}
