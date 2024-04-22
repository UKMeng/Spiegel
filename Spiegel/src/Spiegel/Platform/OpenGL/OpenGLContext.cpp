#include "spgpch.h"
#include "OpenGLContext.h"

namespace spg {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle) 
	{
		SPG_CORE_ASSERT(windowHandle, "WindowHandle is nullptr!")
	}

	void OpenGLContext::Init()
	{
		SPG_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SPG_CORE_ASSERT(status, "Failed to initialize GLAD");

		// TODO: Show this on stats panel
		SPG_CORE_INFO("OpenGL Info:");
		SPG_CORE_INFO(" Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		SPG_CORE_INFO(" Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		SPG_CORE_INFO(" Version: {0}", (const char*)glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		SPG_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle); // ������ɫ���壨����һ��������GLFW����ÿһ��������ɫ�Ĵ󻺳壩��������һ�����б��������ƣ����ҽ�����Ϊ�����ʾ����Ļ�ϡ�
	}
}