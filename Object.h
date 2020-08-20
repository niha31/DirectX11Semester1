#pragma once

#include "OBJLoader.h"
#include <d3d11_1.h>
#include "Vector3.h"
#include "ParticleModel.h"
#include "Quaternions.h"
using namespace DirectX;

class Object
{
private:
	MeshData _meshData;
	XMFLOAT4X4 _modelMatrix;
	XMMATRIX _transformMatrix;
	ParticleModel* _particleModel;
	Quaternion* _quaternion;

public:
	Object();
	Object(ID3D11Device* _pd3dDevice, char* filePath, Vector3 position);
	~Object();

	MeshData* ReturnMeshDataPointer();
	XMFLOAT4X4 ReturnModelMatrix();
	Vector3* ReturnPositionPointer();
	ParticleModel* GetParticleModel() const;
	Quaternion* GetQuaternion();
	XMVECTOR* ReturnVectorQuaternion();
	XMMATRIX* ReturnTransformMatrix();
	XMFLOAT4X4* ReturnModelMatrixPointer();

	void UpdateTransformMatrix();
	void UpdatePosition(float x, float y, float z);
	void UpdateModelMatrix();
};

