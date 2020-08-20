#include "ParticleModel.h"
ParticleModel::ParticleModel(Vector3 position, Vector3 initialVelocity, Vector3 initialAcceleration)
{
	_velocity = initialVelocity;
	_acceleration = initialAcceleration;
	_position = position;
	_drag = 0.0025f;
	_thrust = Vector3(0.0f, 0.0f, 0.0f);
	
}

ParticleModel::~ParticleModel()
{

}

void ParticleModel::Update(float t)
{
	// Move gameobject
	//move cube
	if (GetKeyState('W') & 0x8000)
	{
		AddThrust(Vector3(0.01f, 0.0f, 0.0f));

	}
	if (GetKeyState('S') & 0x8000)
	{
		AddThrust(Vector3(-0.01f, 0.0f, 0.0f));

	}
	if (GetKeyState('A') & 0x8000)
	{
		AddThrust(Vector3(0.0f, 0.0f, 0.01f));
	}
	if (GetKeyState('D') & 0x8000)
	{
		AddThrust(Vector3(0.0f, 0.0f, -0.01f));
	}


	if (GetKeyState('Q') & 0x8000)
	{
		//SetPosOnPlane();
		//AddWeight();
		AddThrust(Vector3(0.0f, 0.01f, 0.0f));
		//AddUpThrust();
		//UpdateNetForce(t);
		//AddUpThrust();
		/*UpdateAcceleration();
		UpdateVelocity(t);
		UpdatePosition(t);*/
	}
	 
	
	//AddThrust(Vector3(0.0f, 0.01f, 0.0f));

	DynamicAcceleration(t);
}

void ParticleModel::DynamicAcceleration(const float t)
{
	Buoyancy();
	UpdateNetForce(t);
	UpdateAcceleration();
	UpdateVelocity(t);
	UpdatePosition(t);
}

void ParticleModel::UpdateNetForce(const float t)
{
	_netForce = Vector3(0.0f, 0.0f, 0.0f);
	_netForce = _netForce + _thrust;

	_netForce.y -= _gravity;
	_netForce.y += _buoyancy;

	_thrust = Vector3(0.0f, 0.0f, 0.0f);
}

void ParticleModel::UpdateAcceleration()
{
	_acceleration = _netForce / _mass;
}

void ParticleModel::UpdateVelocity(const float t)
{
	_velocity += _acceleration;

	_velocity *= (1.0f - (_drag * t));
}

void ParticleModel::UpdatePosition(const float t)
{
	Vector3 newPos = _position + _velocity;
	
	if (newPos.y <= 0)
	{
		newPos.y = 0.0f;
	}
	
	_position = newPos;
}

void ParticleModel::SetPosOnPlane()
{
	Vector3 temp = _position;
	_position = Vector3(temp.x, 0.0f, temp.z);
}

void ParticleModel::AddThrust(Vector3 force)
{
	_thrust += force;
}

void ParticleModel::AddWeight()
{
	_weight = _mass * _gravity;
}

void ParticleModel::AddUpThrust()
{
	if (_position.y == 0)
	{
		_upThrust = _weight;
	}
	else
	{
		_upThrust = -_weight;
	}

	_netForce.y = _netForce.y * _upThrust;
}

void ParticleModel::Buoyancy()
{
	if (_position.y <= 0)
	{
		AddUpThrust();
		_buoyancy = (0.1f * 0.035f);
	}
	else if (_position.y >= 0.2)
	{
		_buoyancy = (-0.1f * 0.035f);
	}
	
}