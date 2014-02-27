#ifndef GLF_WINDOW_INCLUDED
#define GLF_WINDOW_INCLUDED

//#pragma warning(disable : once)

// OpenGL
#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <GL/glew.h>
#	include <GL/wglew.h>
#	include <cl/cl.h>
#	include <cl/cl_ext.h>
#	include <cl/cl_gl.h>
#	include <cl/cl_gl_ext.h>
//#	include <GL/glext.h>
#elif defined(linux) || defined(__linux)
#	include <GL/glew.h>
#	define GL_GLEXT_PROTOTYPES 1
#	include <GL/gl.h>
#	include <GL/glext.h>
#elif defined(__APPLE__)
#	include <OpenGL/gl.h>
#else
#	error "Unsupported platform"
#endif

// GLF libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/half_float.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <gli/gli.hpp>
#include <gli/gtx/loader.hpp>

// STL
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

namespace glf
{
#ifdef WIN32
	static std::string const DATA_DIRECTORY("data/");
#else
        static std::string const DATA_DIRECTORY("../mass_spring/data/");
#endif

	enum mouse_button
	{
		MOUSE_BUTTON_NONE = 0,
		MOUSE_BUTTON_LEFT = (1 << 0),
		MOUSE_BUTTON_RIGHT = (1 << 1),
		MOUSE_BUTTON_MIDDLE = (1 << 2)
	};

	struct window
	{
		window(glm::ivec2 const & Size) :
			Size(Size),
			MouseOrigin(Size >> 1),
			MouseCurrent(Size >> 1),
			TranlationOrigin(0, 4),
			TranlationCurrent(0, 4),
			RotationOrigin(0), 
			RotationCurrent(0),
			MouseButtonFlags(0)
		{
			memset(KeyPressed, 0, sizeof(KeyPressed));	
		}

		glm::ivec2 Size;
		glm::vec2 MouseOrigin;
		glm::vec2 MouseCurrent;
		glm::vec2 TranlationOrigin;
		glm::vec2 TranlationCurrent;
		glm::vec2 RotationOrigin;
		glm::vec2 RotationCurrent;
		int MouseButtonFlags;
		std::size_t KeyPressed[256];
	};

	std::string loadFile(std::string const & Filename);
	bool checkError(const char* Title);
	bool checkProgram(GLuint ProgramName);
	bool checkShader(GLuint ShaderName, char const* Source);
	bool validateProgram(GLuint ProgramName);

	int version(int Major, int Minor);
	int run();

	namespace semantic
	{
		namespace uniform
		{
			enum type
			{
				MATERIAL  = 0,
				TRANSFORM0 = 1,
				TRANSFORM1 = 2
			};
		};

		namespace image
		{
			enum type
			{
				DIFFUSE = 0,
				PICKING = 1
			};
		}//namesapce image

		namespace attr
		{
			enum type
			{
				POSITION = 0,
				COLOR	 = 3,
				TEXCOORD = 4
			};
		}//namespace attr

		namespace vert
		{
			enum type
			{
				POSITION = 0,
				COLOR	 = 3,
				TEXCOORD = 4,
				INSTANCE = 7
			};
		}//namespace vert

		namespace frag
		{
			enum type
			{
				COLOR	= 0,
				RED		= 0,
				GREEN	= 1,
				BLUE	= 2,
				ALPHA	= 0
			};
		}//namespace frag

		namespace renderbuffer
		{
			enum type
			{
				DEPTH,
				COLOR0
			};
		}//namespace renderbuffer

	}//namespace semantic

	struct vertex_v2fv2f
	{
		vertex_v2fv2f
		(
			glm::vec2 const & Position,
			glm::vec2 const & Texcoord
		) :
			Position(Position),
			Texcoord(Texcoord)
		{}

		glm::vec2 Position;
		glm::vec2 Texcoord;
	};

	struct vertex_v3fv2f
	{
		vertex_v3fv2f
		(
			glm::vec3 const & Position,
			glm::vec2 const & Texcoord
		) :
			Position(Position),
			Texcoord(Texcoord)
		{}

		glm::vec3 Position;
		glm::vec2 Texcoord;
	};

	struct vertex_v3fv3f
	{
		vertex_v3fv3f
		(
			glm::vec3 const & Position,
			glm::vec3 const & Texcoord
		) :
			Position(Position),
			Texcoord(Texcoord)
		{}

		glm::vec3 Position;
		glm::vec3 Texcoord;
	};

	struct vertex_v4fv2f
	{
		vertex_v4fv2f
		(
			glm::vec4 const & Position,
			glm::vec2 const & Texcoord
		) :
			Position(Position),
			Texcoord(Texcoord)
		{}

		glm::vec4 Position;
		glm::vec2 Texcoord;
	};

	struct vertex_v2fc4f
	{
		vertex_v2fc4f
		(
			glm::vec2 const & Position,
			glm::vec4 const & Color
		) :
			Position(Position),
			Color(Color)
		{}

		glm::vec2 Position;
		glm::vec4 Color;
	};

	struct vertex_v4fc4f
	{
		vertex_v4fc4f
		(
			glm::vec4 const & Position,
			glm::vec4 const & Color
		) :
			Position(Position),
			Color(Color)
		{}

		glm::vec4 Position;
		glm::vec4 Color;
	};

	struct vertex_v2fc4ub
	{
		vertex_v2fc4ub
		(
			glm::vec2 const & Position,
			glm::u8vec4 const & Color
		) :
			Position(Position),
			Color(Color)
		{}

		glm::vec2 Position;
		glm::u8vec4 Color;
	};

	struct vertex_v2fv2fv4ub
	{
		vertex_v2fv2fv4ub
		(
			glm::vec2 const & Position,
			glm::vec2 const & Texcoord,
			glm::u8vec4 const & Color
		) :
			Position(Position),
			Texcoord(Texcoord),
			Color(Color)
		{}

		glm::vec2 Position;
		glm::vec2 Texcoord;
		glm::u8vec4 Color;
	};

	struct vertexattrib
	{
		vertexattrib() :
			Enabled(GL_FALSE),
			//Binding(0),
			Size(4),
			Stride(0),
			Type(GL_FLOAT),
			Normalized(GL_FALSE),
			Integer(GL_FALSE),
			Long(GL_FALSE),
			Divisor(0),
			Pointer(NULL)
		{}

		vertexattrib
		(
			GLint Enabled,
			//GLint Binding,
			GLint Size,
			GLint Stride,
			GLint Type,
			GLint Normalized,
			GLint Integer,
			GLint Long,
			GLint Divisor,
			GLvoid* Pointer
		) :
			Enabled(Enabled),
			//Binding(Binding),
			Size(Size),
			Stride(Stride),
			Type(Type),
			Normalized(Normalized),
			Integer(Integer),
			Long(Long),
			Divisor(Divisor),
			Pointer(Pointer)
		{}

		GLint Enabled;
		//GLint Binding;
		GLint Size;
		GLint Stride;
		GLint Type;
		GLint Normalized;
		GLint Integer;
		GLint Long;
		GLint Divisor;
		GLvoid* Pointer;
	};

	bool operator== (vertexattrib const & A, vertexattrib const & B)
	{
		return A.Enabled == B.Enabled && 
			A.Size == B.Size && 
			A.Stride == B.Stride && 
			A.Type == B.Type && 
			A.Normalized == B.Normalized && 
			A.Integer == B.Integer && 
			A.Long == B.Long;
	}

	bool operator!= (vertexattrib const & A, vertexattrib const & B)
	{
		return !(A == B);
	}

}//namespace glf

namespace 
{
	extern glf::window Window;
}//namespace 

#define GLF_BUFFER_OFFSET(i) ((char *)NULL + (i))
#ifndef GL_EXTERNAL_VIRTUAL_MEMORY_BUFFER_AMD
#define GL_EXTERNAL_VIRTUAL_MEMORY_BUFFER_AMD 0x9160
#endif

#ifndef WGL_CONTEXT_CORE_PROFILE_BIT_ARB
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#endif

#ifndef WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif

#ifndef WGL_CONTEXT_ES2_PROFILE_BIT_EXT
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#endif

#ifndef GLX_CONTEXT_CORE_PROFILE_BIT_ARB
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#endif

#ifndef GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
#define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif

#ifndef WGLX_CONTEXT_ES2_PROFILE_BIT_EXT
#define WGLX_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#endif

#ifndef GLUT_ES_PROFILE
#define GLUT_ES_PROFILE 1
#endif

#include "glf.inl"

#endif//GLF_WINDOW_INCLUDED