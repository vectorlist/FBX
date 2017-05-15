
#pragma once

#include <matrix4x4.h>
#include <vec2f.h>

#define SPEED_ROTAION	0.25f
#define SPEED_MOVEMENT	6.0f

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 3.0f;
const float SENSITIVTY = 0.25f;
const float ZOOM = 45.0f;
class Camera
{
public:
	vec3f position;
	vec3f front;
	vec3f up;
	vec3f right;
	vec3f worldUp;

	float yaw;
	float pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	float delta = 0.0f;

	Matrix4x4 projection;

	float fovy;
	float aspect;
	float nearPlane;
	float farPlane;

	Camera(vec3f position = vec3f(0.0f, 0.0f, 0.0f),
		vec3f up = vec3f(0.0f, 1.0f, 0.0f),
		float yaw = YAW, float pitch = PITCH)
		: front(vec3f(0.0f, 0.0f, -1.0f)),
		MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->position = position;
		this->worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
		front(vec3f(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->position = vec3f(posX, posY, posZ);
		this->worldUp = vec3f(upX, upY, upZ);
		this->yaw = yaw;
		this->pitch = pitch;
		this->updateCameraVectors();
	}
	void setPerspective(float fovy, float aspect, float znear, float zfar)
	{
		this->fovy = fovy;
		this->aspect = aspect;
		this->nearPlane = znear;
		this->farPlane = zfar;
		projection = vml::perspective(fovy, aspect, znear, zfar);
	}

	void setLookAt(const vec3f &pos, const vec3f &center, const vec3f &up)
	{
		position = pos;
		worldUp = up;
		vec3f dir = (center - position);
		this->front = dir.normalized();
		this->right = vec3f::cross(this->front, this->worldUp).normalized();
		this->up = vec3f::cross(this->right, this->front).normalized();
		pitch = atanf(front.y);
		yaw = atan2f(front.x, front.z);
		//updateCameraVectors();
	}
	bool motion = false;
	const Matrix4x4& proj() const
	{
		return projection;
	}

	const Matrix4x4 view() const
	{
		return vml::lookAt(position, position + front, up);
	}

	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->yaw += xoffset;
		this->pitch += yoffset;

		if (constrainPitch)
		{
			if (this->pitch > 89.0f)
				this->pitch = 89.0f;
			if (this->pitch < -89.0f)
				this->pitch = -89.0f;
		}
		this->updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}

	struct 
	{
		bool forward = false;
		bool backward = false;
		bool left = false;
		bool right = false;
		bool up = false;
	}keys;

	void moveProcess(float delta)
	{
		float velocity = this->MovementSpeed * delta;
		if (keys.forward)
			position += front * velocity;
		if (keys.backward)
			position -= front * velocity;
		if (keys.left)
			position -= right * velocity;
		if (keys.right)
			position += right * velocity;
		if (keys.up)
			position += up * velocity;
	}

	void ProcessKeyboard(Camera_Movement direction)
	{
		float velocity = this->MovementSpeed * delta;
		if (direction == FORWARD)
			position += front * velocity;
		if (direction == BACKWARD)
			position -= front * velocity;
		if (direction == LEFT)
			position -= right * velocity;
		if (direction == RIGHT)
			position += right * velocity;
		if(direction == UP)
			position += worldUp * velocity;
	}

	vec2f mousePos;

private:
	void updateCameraVectors()
	{
		vec3f front;
		front.x = cos(radians(yaw)) * cos(radians(pitch));
		front.y = sin(radians(pitch));
		front.z = sin(radians(yaw)) * cos(radians(pitch));
		this->front = front.normalized();
		this->right = vec3f::cross(this->front, this->worldUp).normalized();
		this->up = vec3f::cross(this->right, this->front).normalized();
	}
};