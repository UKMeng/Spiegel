#include "Spiegel.h"
#include <windows.h>

bool IsKeyDown(int virtualKeyCode)
{
	return GetAsyncKeyState(virtualKeyCode) < 0;
}



// Native Script Example
class CameraController : public spg::ScriptableEntity
{
public:
	/*void OnCreate()
	{

	}

	void OnDestroy()
	{
	}*/

	void OnUpdate(spg::Timestep ts)
	{
		auto& translation = GetComponent<spg::TransformComponent>().Translation;
		float speed = 5.0f;

		if (IsKeyDown('A'))
			translation.x -= speed * ts;
		if (IsKeyDown('D'))
			translation.x += speed * ts;
		if (IsKeyDown('W'))
			translation.y += speed * ts;
		if (IsKeyDown('S'))
			translation.y -= speed * ts;
	}
};

extern "C" __declspec(dllexport) spg::ScriptableEntity * CreateCameraController()
{
	return new CameraController();
}