#include "Object.h"


Object::Object()
{

}

Object::Object(ID3D11Device* _pd3dDevice, char* filePath, Vector3 position)
{
	XMStoreFloat4x4(&_modelMatrix, XMMatrixIdentity());

	_meshData = OBJLoader::Load(filePath, _pd3dDevice);

	_particleModel = new ParticleModel(position, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));

	_quaternion = new Quaternion();
	CalculateTransformMatrix(_transformMatrix, _particleModel->GetPosition(), *_quaternion);
}

Object::~Object()
{

}

ParticleModel* Object::GetParticleModel() const
{
	return _particleModel;
}

Quaternion* Object::GetQuaternion()
{
	return _quaternion;
}


MeshData* Object::ReturnMeshDataPointer()
{
	return (&_meshData);
}

XMFLOAT4X4 Object::ReturnModelMatrix()
{
	return _modelMatrix;
}

XMFLOAT4X4* Object::ReturnModelMatrixPointer()
{
	return &_modelMatrix;
}

Vector3* Object::ReturnPositionPointer()
{
	return &_particleModel->GetPosition();
}

XMMATRIX* Object::ReturnTransformMatrix()
{
	return &_transformMatrix;
}

XMVECTOR* Object::ReturnVectorQuaternion()
{
	XMVECTOR vectorQuaternion = XMVectorSet(_quaternion->i, _quaternion->j , _quaternion->k, _quaternion->r);

	return &vectorQuaternion;
}

void Object::UpdateTransformMatrix()
{
	CalculateTransformMatrix(_transformMatrix, _particleModel->GetPosition(), *_quaternion);
}


void Object::UpdatePosition(float x, float y, float z)
{
	_particleModel->SetPosition(Vector3(_particleModel->GetPosition().x + x, _particleModel->GetPosition().y + y, _particleModel->GetPosition().z + z));
}

void Object::UpdateModelMatrix()
{
	CalculateTransformMatrix(_transformMatrix, _particleModel->GetPosition(), *_quaternion);
}
