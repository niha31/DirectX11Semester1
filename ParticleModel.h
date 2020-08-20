#pragma once
#include <d3d11_1.h>
#include <directxmath.h>
#include <vector>
#include "Vector3.h"

using namespace DirectX;

class ParticleModel
{
private:
	Vector3 _velocity;
	Vector3 _acceleration;
	Vector3 _position;
	float _mass = 50.0f;
	float _gravity = 0.001f;


	//forces applied on object
	Vector3 _netForce;
	Vector3 _thrust;
	float _weight;
	float _upThrust;
	float _buoyancy;

	float _drag;

public:
	ParticleModel(Vector3 position, Vector3 initialVelocity, Vector3 initialAcceleration);
	~ParticleModel();

	void Update(float t);

	void SetVelocity(Vector3 velocity) { _velocity = velocity; }
	Vector3 GetVelocity() const { return _velocity; }

	void SetAcceleration(Vector3 acceleration) { _acceleration = acceleration; }
	Vector3 GetAcceleration() const { return _acceleration; }

	void SetPosition(Vector3 position) { _position = position; }
	Vector3 GetPosition() const { return _position; }

	void UpdateNetForce(const float t);
	void UpdateAcceleration();
	void UpdateVelocity(const float t);
	void DynamicAcceleration(const float t);
	void UpdatePosition(const float t);
	void SetPosOnPlane();

	void AddThrust(Vector3 force);
	void AddWeight();
	void AddUpThrust();

	void Buoyancy();


};

