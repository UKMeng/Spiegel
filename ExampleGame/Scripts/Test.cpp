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

		if (spg::Input::IsKeyPressed(spg::Key::A))
			translation.x -= speed * ts;
		if (spg::Input::IsKeyPressed(spg::Key::D))
			translation.x += speed * ts;
		if (spg::Input::IsKeyPressed(spg::Key::W))
			translation.y += speed * ts;
		if (spg::Input::IsKeyPressed(spg::Key::S))
			translation.y -= speed * ts;
	}
};

extern "C" __declspec(dllexport) spg::ScriptableEntity * CreateCameraController()
{
	return new CameraController();
}