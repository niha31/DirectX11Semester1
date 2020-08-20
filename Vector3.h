#pragma once
#include<iostream>
#include<math.h>
#include<assert.h>
using namespace std;

class Vector3
{
public:

	float x, y, z;

	Vector3();
	Vector3(float x1, float y1, float z1 = 0);
	Vector3(const Vector3& vec);


	Vector3 operator+(const Vector3& vec);   //addition
	Vector3& operator+=(const Vector3& vec);//assign new result to vector
	Vector3 operator-(const Vector3& vec); //substraction
	Vector3& operator-=(const Vector3& vec);//assign new result to vector
	Vector3 operator*(float value);    //multiplication
	Vector3& operator*=(float value);  //assign new result to vector.
	Vector3 operator/(float value);    //division
	Vector3& operator/=(float value);  //assign new result to vector
	Vector3& operator=(const Vector3& vec);

	float dot_product(const Vector3& vec); //scalar dot_product
	Vector3 cross_product(const Vector3& vec); //cross_product
	Vector3 normalization();   //normalized vector

	float square(); //gives square of the vector
	float distance(const Vector3& vec); //distance between two vectors
	float magnitude();  //magnitude of the vector


	float show_X(); //return x
	float show_Y(); //return y
	float show_Z(); //return z
	void disp();    //display value of vectors
};



