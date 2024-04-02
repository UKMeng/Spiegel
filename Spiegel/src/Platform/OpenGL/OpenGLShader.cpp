#include "spgpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace spg {
	OpenGLShader::OpenGLShader(const std::string& filepath) {
		std::string shaderSource = ReadFile(filepath);
		auto shaderSources = PreProcess(shaderSource);
		Compile(shaderSources);

		// Extract shader name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		SPG_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader() 
	{
		SPG_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		SPG_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in) {
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else {
			SPG_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		SPG_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", pos);
			SPG_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			SPG_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[type == "vertex" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER] =
				source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		SPG_PROFILE_FUNCTION();

		// Get a program object.
		GLuint program = glCreateProgram();
		SPG_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;
		
		for (auto&& [type, src] : shaderSources) {
			SPG_CORE_TRACE("Shader Type: {0}", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
			SPG_CORE_TRACE("Source: {0}", src);
			GLuint shader = glCreateShader(type);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* source = (const GLchar*)src.c_str();
			glShaderSource(shader, 1, &source, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			// Check for compile time errors
			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				// Use the infoLog as you see fit.

				SPG_CORE_ERROR("{0}", infoLog.data());
				SPG_CORE_ASSERT(false, "Shader Compilation Failed");
				break;
			}
			// Attach shader to program
			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.

			for (auto id : glShaderIDs) {
				glDeleteShader(id);
			}

			// Use the infoLog as you see fit.

			SPG_CORE_ERROR("{0}", infoLog.data());
			SPG_CORE_ASSERT(false, "Shader Link Failed");
			return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs) {
			glDetachShader(program, id);
		}

		m_RendererID = program;
	}

	void OpenGLShader::Bind() const
	{
		SPG_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}
	void OpenGLShader::Unbind() const
	{
		SPG_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		SPG_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		SPG_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		SPG_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		SPG_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		SPG_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, values.x, values.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	
}