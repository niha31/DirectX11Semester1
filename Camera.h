#pragma once
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

using namespace DirectX;

class Camera
{
private:
	// Private attributes to store the camera position and view 
	// volume

	XMVECTOR _position;
	XMVECTOR _front;
	XMVECTOR _right;
	XMVECTOR _up;
	XMVECTOR _worldUp;
	

	XMMATRIX _view;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	CONST FLOAT _yaw = -90.0f;
	CONST FLOAT _pitch = 0.0f;

	// attributes to hold the view and projection matrices which will be passed to the shader

	XMMATRIX  _projection;

public:

	//Constructor and destructor for the camera
	Camera();
	Camera(XMVECTOR position, XMVECTOR lookat, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	~Camera();

	// update function to make the current view and projection matrices
	void Update();

	// you will need a set of functions to set and return the position, lookat and up attributes
	void SetPosition(XMVECTOR position);
	XMFLOAT3 ReturnPosition();

	void SetFront(XMFLOAT3 front);
	XMFLOAT3 ReturnLookAt();
	void SetLookAt(XMVECTOR lookat);

	void SetUp(XMVECTOR worldUp);
	XMFLOAT3 ReturnUp();
		
	// You will need functions to get the View, Projection and combined ViewProjection matrices
	XMMATRIX ReturnView();
	XMMATRIX ReturnProjection();
	XMMATRIX ReturnViewProjection();

	// A function to reshape the camera volume if the window is resized.
	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	void MoveFowards();
	void MoveBackwards();
	void MoveLeft();
	void MoveRight();
};

