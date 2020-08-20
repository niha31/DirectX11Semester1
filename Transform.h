#pragma once

#include <d3d11_1.h>
#include <directxmath.h>
#include "Vector3.h"
#include <vector>

using namespace DirectX;

class Transform
{
public:
	Transform();
	Transform(XMVECTOR position, Vector3 rotation, Vector3 scale);
	~Transform();

	void SetScale(Vector3 scale) { _scale = scale; }
	void SetScale(float x, float y, float z) { _scale.x = x; _scale.y = y; _scale.z = z; }

	Vector3 GetScale() const { return _scale; }

	void SetRotation(Vector3 rotation) { _rotation = rotation; }
	void SetRotation(float x, float y, float z) { _rotation.x = x; _rotation.y = y; _rotation.z = z; }

	Vector3 GetRotation() const { return _rotation; }

	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4(&_world); }
	void SetWorldMatrix(XMMATRIX scale, XMMATRIX rotation, XMMATRIX translation);
	void SetWorldMatrix(XMMATRIX object, XMMATRIX parent);

	//Setters and Getters for position/rotation/scale
	void SetPosition(XMVECTOR pos) { _position = pos; }
	void SetPosition(float x, float y, float z) { XMVectorSetX(_position, x); XMVectorSetY(_position, y); XMVectorSetZ(_position, z); } //_position.x = x; _position.y = y; _position.z = z; }
	XMVECTOR GetPosition() const { return _position; }

private:
	XMVECTOR _position;
	Vector3 _rotation;
	Vector3 _scale;
	XMFLOAT4X4 _world;

};

