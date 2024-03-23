#include "spgpch.h"
#include "OpenGLContext.h"

namespace spg {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle) {
		SPG_CORE_ASSERT(windowHandle, "WindowHandle is nullptr!")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SPG_CORE_ASSERT(status, "Failed to initialize GLAD");

		SPG_CORE_INFO("OpenGL Info:");
		SPG_CORE_INFO(" Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		SPG_CORE_INFO(" Vendor: {0}", (const char*)glGetString(GL_RENDERER));
		SPG_CORE_INFO(" Version: {0}", (const char*)glGetString(GL_VERSION));
	}
	void OpenGLContext::SwapBuffers()
	{
		glBegin(GL_TRIANGLES);
		glEnd();
		glfwSwapBuffers(m_WindowHandle); // 交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。
	}
}