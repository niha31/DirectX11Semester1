#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(XMVECTOR position, XMVECTOR lookat, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_position = position;
	
	_worldUp = XMVectorSet(0.0f, 1.0f, 0.0, 0.0f);
	SetLookAt(lookat);

	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;

	// Initialize the projection matrix
	Reshape(windowWidth, windowHeight, nearDepth, farDepth);

	Update();

	
}

Camera::~Camera()
{

}

void Camera::Update()
{
	//// Initialize the view matrix
	_view = XMMatrixLookToLH(_position, _front, _worldUp);
}

void Camera::SetPosition(XMVECTOR position)
{
	_position = position;
	Update();
}
XMFLOAT3 Camera::ReturnPosition()
{
	XMFLOAT3 returnFloat3;
	XMStoreFloat3(&returnFloat3, _position);
	return returnFloat3;
}

void Camera::SetFront(XMFLOAT3 front)
{
	_front = XMVectorSet(front.x, front.y, front.z, 1.0f);
}

XMFLOAT3 Camera::ReturnLookAt()
{
	XMFLOAT3 returnFloat3;
	XMStoreFloat3(&returnFloat3, _front);
	return returnFloat3;
}

void Camera::SetLookAt(XMVECTOR lookat)
{
	_front = XMVector3Normalize(lookat - _position);

	_right = XMVector3Normalize(XMVector3Cross(_front, _worldUp));
	_up = XMVector3Normalize(XMVector3Cross(_right, _front));
}

void Camera::SetUp(XMVECTOR worldUp)
{
	_worldUp = worldUp;
}

XMFLOAT3 Camera::ReturnUp()
{
	XMFLOAT3 returnFloat3;
	XMStoreFloat3(&returnFloat3, _worldUp);
	return returnFloat3;
}

XMMATRIX Camera::ReturnView()
{
	return _view;
}

XMMATRIX Camera::ReturnProjection()
{
	return _projection;
}

XMMATRIX Camera::ReturnViewProjection()
{
	return XMMatrixMultiply(_projection, _view);
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / (FLOAT)windowHeight, nearDepth, farDepth);
}

void Camera::MoveFowards()
{
	_position += _front * 0.05f;
}

void Camera::MoveBackwards()
{
	_position -= _front * 0.05f;
}

void Camera::MoveLeft()
{
	_position -= _right * 0.05f;
}

void Camera::MoveRight()
{
	_position += _right * 0.05f;
}