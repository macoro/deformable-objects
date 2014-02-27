#include <glf/glf.hpp>
#include <camera.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <tetramesh.h>

#define kEpsilon    1.0e-6f
#define kPI         3.1415926535897932384626433832795f
#define kHalfPI     1.5707963267948966192313216916398f
#define kTwoPI      2.0f*kPI

namespace
{
    std::string const SHADER_VERT_SOURCE_MS_RENDER(glf::DATA_DIRECTORY + "msRender.vsh");
    std::string const SHADER_FRAG_SOURCE_MS_RENDER(glf::DATA_DIRECTORY + "msRender.fsh");
    std::string const SHADER_GEOM_SOURCE_MS_RENDER(glf::DATA_DIRECTORY + "msRender.gsh");
    std::string const SHADER_VERT_SOURCE_MS_UPDATEPOSITION(glf::DATA_DIRECTORY + "msUpdatePosition.vsh");
    std::string const SHADER_VERT_SOURCE_MS_LINEARFORCE(glf::DATA_DIRECTORY + "msLinearForce.vsh");
    std::string const SHADER_GEOM_SOURCE_MS_LINEARFORCE(glf::DATA_DIRECTORY + "msLinearForce.gsh");
    std::string const SHADER_VERT_SOURCE_MS_VOLUMEFORCE(glf::DATA_DIRECTORY + "msVolumeForce.vsh");
    std::string const SHADER_GEOM_SOURCE_MS_VOLUMEFORCE(glf::DATA_DIRECTORY + "msVolumeForce.gsh");
	std::string const SHADER_VERT_SOURCE_FLOOR(glf::DATA_DIRECTORY + "floor.vsh");
	std::string const SHADER_FRAG_SOURCE_FLOOR(glf::DATA_DIRECTORY + "floor.fsh");
	std::string const TEXTURE_FLOOR(glf::DATA_DIRECTORY + "OldFloor.dds");
	int const SAMPLE_SIZE_WIDTH(1024);
	int const SAMPLE_SIZE_HEIGHT(800);
	int const SAMPLE_MAJOR_VERSION(4);
    int const SAMPLE_MINOR_VERSION(3);

	glf::window Window(glm::ivec2(SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT));

	namespace program
	{
		enum type
		{
            MS_RENDER,
			MS_UPDATEPOSITION,
            MS_LINEARFORCE,
            MS_VOLUMEFORCE,
			FLOOR,
            MAX
		};
	}//namespace program
	
	namespace pipeline
	{
		enum type
		{
            MS_RENDER,
            MS_UPDATEPOSITION,
            MS_LINEARFORCE,
            MS_VOLUMEFORCE,
			FLOOR,
			MAX
		};
	}//namespace pipeline
	
	namespace texture
	{
		enum type
		{
			FLOOR,
			FORCE,
			POSITION,
			PREVPOSITION,
			VELOCITY,
			MAX
		};
	}//namespace texture
	
	namespace image
	{
		enum type
		{
			FORCE,
			POSITION,
			PREVPOSITION,
			VELOCITY,
			MAX
		};
	}//namespace image
	
	namespace buffer
	{
		enum type
		{
			VERTEX,
			INV_MASS,
			TETRA_INDEX,
            EDGE_INDEX,
            //FORCE,
			MAX
		};
	}//namespace buffer

	namespace attr_vertex
	{
		enum type 
		{
			VERTEX,
            INV_MASS,
			MAX 
		};
	}
	
    GLuint vpMatrix(0);
	GLuint VertexArrayName(0);
	GLuint TextureName[texture::MAX];
	GLuint SamplerName(0);
	GLuint BufferName[buffer::MAX];
	GLuint ProgramName[program::MAX];
	GLuint PipelineName[pipeline::MAX];	
}//namespace

//MVP
glm::mat4 Projection = glm::perspective(60.0f, float(Window.Size.x) / Window.Size.y, 0.1f, 1000.0f);
Camera camera(glm::vec3(0.0f, 30.0f,50.0f), glm::vec3(0.0f,-3.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f), Projection);
//uniform variables MS_RENDER
GLuint uM, uVP, uNM, uMV, uLightInt, uLightPos;
GLuint uKd, uKa, uKs, uShin, urowSize;

glm::vec4 lpos = glm::vec4(0.0f,0.0f,50.0f,1.0f);
glm::vec4 lint = glm::vec4(1.0f);
float h = 0.0075f;
glm::vec4 k_DVd = glm::vec4(100.0f, 20.0f, 0.01f, 0.01f);
glm::vec4 Kd    = glm::vec4(0.1f, 0.1f, 0.5f,1.0f);
glm::vec4 Ks    = glm::vec4(0.3f, 0.3f, 1.0f, 1.0f);
glm::vec4 Ka    = glm::vec4(0.1f, 0.1f, 0.3f,1.0f);
float shin = 100.0f;
//uniform variables MS_UPDATEPOSITION
GLuint upM, uph, uprowSize, upfrowSize;
//uniform variable MS_LINEARFORCE
GLuint ulfM, ulfrowSize,ulfFrowSize,ulfK, ulfDamp;
//uniform variable MS_VOLUMEFORCE
GLuint uvfM, uvfrowSize, uvfFrowSize, uvfK, uvfDamp;

//TetraMesh
std::string ELEFILE(glf::DATA_DIRECTORY + "model/bunny2.ele");
std::string NODEFILE(glf::DATA_DIRECTORY + "model/bunny2.node");

TetraMesh tetraMesh(NODEFILE.c_str(), ELEFILE.c_str(), 5.0f);
int rowSize = tetraMesh.m_rowSize;
int frowSize = tetraMesh.m_frowSize;

void myKeyboard(unsigned char key, int x, int y)
{
  switch(key)
  {
    // controls for camera
    // add up/down and left/right controls
    case 'L':      camera.slide(.2, 0, 0); break;// slide camera right
    case 'L' + 32: camera.slide(-0.2, 0, 0); break; // slide camera left

    case 'U':      camera.slide(0, -0.2, 0); break;// slide camera down
    case 'U' + 32: camera.slide(0, 0.2, 0); break; // slide camera up

    case 'F':    camera.slide(0,0, 0.2); break; // slide camera forward
    case 'F' + 32: camera.slide(0,0,-0.2); break; //slide camera back
    // add pitch controls
    case 'P':      camera.pitch(-1.0); break;
    case 'P' + 32: camera.pitch( 1.0); break;
    // add yaw controls
    case 'Y':      camera.yaw(-1.0); break;
    case 'Y' + 32: camera.yaw( 1.0); break;
    // add roll controls
    case 'R':      camera.roll(1.0); break;
    case 'R' + 32: camera.roll(-1.0); break;
  }
    glutPostRedisplay(); // draws it again
}

void printMat4(glm::mat4 &m){
    for(int i = 0; i<4; i++)
    std::cout << m[0][i]<<" "<<m[1][i]<<" "<<
                 m[2][i]<<" "<<m[3][i]<< std::endl;
}


bool initProgram()
{
	bool Validated(true);
	
	glGenProgramPipelines(pipeline::MAX, PipelineName);
    glBindProgramPipeline(PipelineName[pipeline::MS_RENDER]);
    glBindProgramPipeline(PipelineName[pipeline::MS_UPDATEPOSITION]);
    glBindProgramPipeline(PipelineName[pipeline::MS_LINEARFORCE]);
    glBindProgramPipeline(PipelineName[pipeline::MS_VOLUMEFORCE]);
	glBindProgramPipeline(PipelineName[pipeline::FLOOR]);
	glBindProgramPipeline(0);

    if(Validated)
    {
        GLuint VertShaderName = glf::createShader(GL_VERTEX_SHADER, SHADER_VERT_SOURCE_MS_RENDER);
        GLuint GeomShaderName = glf::createShader(GL_GEOMETRY_SHADER, SHADER_GEOM_SOURCE_MS_RENDER);
        GLuint FragShaderName = glf::createShader(GL_FRAGMENT_SHADER, SHADER_FRAG_SOURCE_MS_RENDER);

        ProgramName[program::MS_RENDER] = glCreateProgram();
        glProgramParameteri(ProgramName[program::MS_RENDER], GL_PROGRAM_SEPARABLE, GL_TRUE);
        glAttachShader(ProgramName[program::MS_RENDER], VertShaderName);
        glAttachShader(ProgramName[program::MS_RENDER], GeomShaderName);
        glAttachShader(ProgramName[program::MS_RENDER], FragShaderName);
        glDeleteShader(VertShaderName);
        glDeleteShader(GeomShaderName);
        glDeleteShader(FragShaderName);
        glLinkProgram(ProgramName[program::MS_RENDER]);
        Validated = Validated && glf::checkProgram(ProgramName[program::MS_RENDER]);
    }

    if(Validated)
    {
        glUseProgramStages(PipelineName[pipeline::MS_RENDER], GL_VERTEX_SHADER_BIT | GL_GEOMETRY_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::MS_RENDER]);
        uM = glGetUniformLocation( ProgramName[program::MS_RENDER], "Model");
        uVP = glGetUniformLocation( ProgramName[program::MS_RENDER], "ViewProjection");
        uMV = glGetUniformLocation( ProgramName[program::MS_RENDER], "ModelViewMatrix");
        uNM = glGetUniformLocation( ProgramName[program::MS_RENDER], "NormalMatrix");
        uLightPos = glGetUniformLocation( ProgramName[program::MS_RENDER], "LightPosition");
        uLightInt = glGetUniformLocation( ProgramName[program::MS_RENDER], "LightIntensity");
        uKs = glGetUniformLocation( ProgramName[program::MS_RENDER], "Ks");
        uKa = glGetUniformLocation( ProgramName[program::MS_RENDER], "Ka");
        uKd = glGetUniformLocation( ProgramName[program::MS_RENDER], "Kd");
        uShin = glGetUniformLocation( ProgramName[program::MS_RENDER], "Shininess");
        urowSize = glGetUniformLocation( ProgramName[program::MS_RENDER], "row_size");

        Validated = Validated && glf::checkError("initProgram - stage");
    }

    if(Validated)
	{
        GLuint VertShaderName = glf::createShader(GL_VERTEX_SHADER, SHADER_VERT_SOURCE_MS_UPDATEPOSITION);

        ProgramName[program::MS_UPDATEPOSITION] = glCreateProgram();
		glProgramParameteri(ProgramName[program::MS_UPDATEPOSITION], GL_PROGRAM_SEPARABLE, GL_TRUE);
        glAttachShader(ProgramName[program::MS_UPDATEPOSITION], VertShaderName);
        glDeleteShader(VertShaderName);
		glLinkProgram(ProgramName[program::MS_UPDATEPOSITION]);
		Validated = Validated && glf::checkProgram(ProgramName[program::MS_UPDATEPOSITION]);
	}

	if(Validated)
	{
        glUseProgramStages(PipelineName[pipeline::MS_UPDATEPOSITION], GL_VERTEX_SHADER_BIT, ProgramName[program::MS_UPDATEPOSITION]);
        uph = glGetUniformLocation( ProgramName[program::MS_UPDATEPOSITION], "step");
        upM = glGetUniformLocation( ProgramName[program::MS_UPDATEPOSITION], "Model");
        uprowSize = glGetUniformLocation( ProgramName[program::MS_UPDATEPOSITION], "row_size");
        upfrowSize = glGetUniformLocation( ProgramName[program::MS_UPDATEPOSITION], "frow_size");
        Validated = Validated && glf::checkError("initProgram - stage");
    }

    if(Validated)
    {
        GLuint VertShaderName = glf::createShader(GL_VERTEX_SHADER, SHADER_VERT_SOURCE_MS_LINEARFORCE);
        GLuint GeomShaderName = glf::createShader(GL_GEOMETRY_SHADER, SHADER_GEOM_SOURCE_MS_LINEARFORCE);

        ProgramName[program::MS_LINEARFORCE] = glCreateProgram();
        glProgramParameteri(ProgramName[program::MS_LINEARFORCE], GL_PROGRAM_SEPARABLE, GL_TRUE);
        glAttachShader(ProgramName[program::MS_LINEARFORCE], VertShaderName);
        glAttachShader(ProgramName[program::MS_LINEARFORCE], GeomShaderName);
        glDeleteShader(VertShaderName);
        glDeleteShader(GeomShaderName);
        glLinkProgram(ProgramName[program::MS_LINEARFORCE]);
        Validated = Validated && glf::checkProgram(ProgramName[program::MS_LINEARFORCE]);
    }

    if(Validated)
    {
        glUseProgramStages(PipelineName[pipeline::MS_LINEARFORCE], GL_VERTEX_SHADER_BIT | GL_GEOMETRY_SHADER_BIT, ProgramName[program::MS_LINEARFORCE]);
        ulfK = glGetUniformLocation( ProgramName[program::MS_LINEARFORCE], "K");
        ulfDamp = glGetUniformLocation( ProgramName[program::MS_LINEARFORCE], "damp");
        ulfM = glGetUniformLocation( ProgramName[program::MS_LINEARFORCE], "Model");
        ulfrowSize = glGetUniformLocation( ProgramName[program::MS_LINEARFORCE], "row_size");
        ulfFrowSize = glGetUniformLocation( ProgramName[program::MS_LINEARFORCE], "frow_size");
        Validated = Validated && glf::checkError("initProgram - stage");
    }

    if(Validated)
    {
        GLuint VertShaderName = glf::createShader(GL_VERTEX_SHADER, SHADER_VERT_SOURCE_MS_VOLUMEFORCE);
        GLuint GeomShaderName = glf::createShader(GL_GEOMETRY_SHADER, SHADER_GEOM_SOURCE_MS_VOLUMEFORCE);

        ProgramName[program::MS_VOLUMEFORCE] = glCreateProgram();
        glProgramParameteri(ProgramName[program::MS_VOLUMEFORCE], GL_PROGRAM_SEPARABLE, GL_TRUE);
        glAttachShader(ProgramName[program::MS_VOLUMEFORCE], VertShaderName);
        glAttachShader(ProgramName[program::MS_VOLUMEFORCE], GeomShaderName);
        glDeleteShader(VertShaderName);
        glDeleteShader(GeomShaderName);
        glLinkProgram(ProgramName[program::MS_VOLUMEFORCE]);
        Validated = Validated && glf::checkProgram(ProgramName[program::MS_VOLUMEFORCE]);
    }

    if(Validated)
    {
        glUseProgramStages(PipelineName[pipeline::MS_VOLUMEFORCE], GL_VERTEX_SHADER_BIT | GL_GEOMETRY_SHADER_BIT, ProgramName[program::MS_VOLUMEFORCE]);
        uvfK = glGetUniformLocation( ProgramName[program::MS_VOLUMEFORCE], "K");
        uvfDamp = glGetUniformLocation( ProgramName[program::MS_VOLUMEFORCE], "damp");
        uvfM = glGetUniformLocation( ProgramName[program::MS_VOLUMEFORCE], "Model");
        uvfrowSize = glGetUniformLocation( ProgramName[program::MS_VOLUMEFORCE], "row_size");
        uvfFrowSize = glGetUniformLocation( ProgramName[program::MS_VOLUMEFORCE], "frow_size");
        Validated = Validated && glf::checkError("initProgram - stage");
    }

	if(Validated)
	{
		GLuint VertShaderName = glf::createShader(GL_VERTEX_SHADER, SHADER_VERT_SOURCE_FLOOR);
		GLuint FragShaderName = glf::createShader(GL_FRAGMENT_SHADER, SHADER_FRAG_SOURCE_FLOOR);

		ProgramName[program::FLOOR] = glCreateProgram();
		glProgramParameteri(ProgramName[program::FLOOR], GL_PROGRAM_SEPARABLE, GL_TRUE);
		glAttachShader(ProgramName[program::FLOOR], VertShaderName);
		glAttachShader(ProgramName[program::FLOOR], FragShaderName);
		glDeleteShader(VertShaderName);
		glDeleteShader(FragShaderName);
		glLinkProgram(ProgramName[program::FLOOR]);
		Validated = Validated && glf::checkProgram(ProgramName[program::FLOOR]);
	}

    if(Validated)
	{
		glUseProgramStages(PipelineName[pipeline::FLOOR], GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName[program::FLOOR]);
        vpMatrix = glGetUniformLocation( ProgramName[program::FLOOR], "MVP");
		Validated = Validated && glf::checkError("initProgram - stage");
	}

	return Validated;
}

bool initBuffer()
{
    bool Validated(true);

	glGenBuffers(buffer::MAX, BufferName);

    glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*tetraMesh.nnode*3, tetraMesh.m_verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::INV_MASS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*tetraMesh.nnode, tetraMesh.m_imass, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::TETRA_INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*4*tetraMesh.ntetra,tetraMesh.m_indexs, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::EDGE_INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*tetraMesh.nedge,tetraMesh.m_edges, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

//    glBindBuffer(GL_TEXTURE_BUFFER, BufferName[buffer::FORCE]);
//    glBufferData(GL_TEXTURE_BUFFER, sizeof(GLfloat)*tetraMesh.nnode*3, tetraMesh.m_force, GL_DYNAMIC_COPY);
//    glBindBuffer(GL_TEXTURE_BUFFER,0);

	return Validated;
}

bool initSampler()
{
	bool Validated(true);

	glGenSamplers(1, &SamplerName);
	glSamplerParameteri(SamplerName, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(SamplerName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return Validated;
}

bool initTextures()
{
    bool Validated(true);

    glGenTextures(texture::MAX, TextureName);

    //the floor
    gli::texture2D Texture = gli::load(TEXTURE_FLOOR);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName[texture::FLOOR]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexStorage2D(GL_TEXTURE_2D, GLint(Texture.levels()), GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 
		GLsizei(Texture[0].dimensions().x), GLsizei(Texture[0].dimensions().y));

	for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
	{
		glCompressedTexSubImage2D(
			GL_TEXTURE_2D,
			GLint(Level),
			0, 0,
			GLsizei(Texture[Level].dimensions().x), 
			GLsizei(Texture[Level].dimensions().y), 
			GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 
			GLsizei(Texture[Level].capacity()), 
			Texture[Level].data());
	}
	
    // Setup texture for force
//    glBindTexture(GL_TEXTURE_BUFFER,TextureName[texture::FORCE]);
//    //Attach the buffer object to the texture and specify format
//    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, BufferName[buffer::FORCE]);
    glBindTexture(GL_TEXTURE_2D,TextureName[texture::FORCE]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, frowSize, frowSize);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frowSize, frowSize, GL_RED, GL_FLOAT, tetraMesh.m_force);
    //position texture
        glBindTexture(GL_TEXTURE_2D,TextureName[texture::POSITION]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, rowSize, rowSize);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rowSize, rowSize, GL_RGBA, GL_FLOAT, tetraMesh.m_position);
   //previous position texture
        glBindTexture(GL_TEXTURE_2D, TextureName[texture::PREVPOSITION]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, rowSize, rowSize);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rowSize, rowSize, GL_RGBA, GL_FLOAT, tetraMesh.m_prevpos);
    //velocity texture or old position
        glBindTexture(GL_TEXTURE_2D,TextureName[texture::VELOCITY]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, rowSize, rowSize);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rowSize, rowSize, GL_RGBA, GL_FLOAT, tetraMesh.m_velocity);

	return Validated;
}

bool initVertexArray()
{
	bool Validated(true);

    glGenVertexArrays(1, &VertexArrayName);
    glBindVertexArray(VertexArrayName);
        glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
        glVertexAttribPointer(attr_vertex::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::INV_MASS]);
        glVertexAttribPointer(attr_vertex::INV_MASS, 1, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glEnableVertexAttribArray(attr_vertex::VERTEX);
        glEnableVertexAttribArray(attr_vertex::INV_MASS);
    glBindVertexArray(0);

	return Validated;
}

bool initDebugOutput()
{
	bool Validated(true);

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageCallbackARB(&glf::debugOutput, NULL);

	return Validated;
}

bool begin()
{
	bool Validated(true);
	Validated = Validated && glf::checkGLVersion(SAMPLE_MAJOR_VERSION, SAMPLE_MINOR_VERSION);
    if(Validated && glf::checkExtension("GL_ARB_debug_output"))
        Validated = initDebugOutput();
    if(Validated)
        Validated = initProgram();
    if(Validated)
        Validated = initBuffer();
    if(Validated)
        Validated = initVertexArray();
    if(Validated)
        Validated = initTextures();
    if(Validated)
        Validated = initSampler();
    glutKeyboardFunc(myKeyboard);
    return Validated;
}

bool end()
{
	bool Validated(true);

	glDeleteBuffers(buffer::MAX, BufferName);
    glDeleteTextures(texture::MAX, TextureName);
    glDeleteProgram(ProgramName[program::MS_RENDER]);
	glDeleteProgram(ProgramName[program::FLOOR]);
	glDeleteVertexArrays(1, &VertexArrayName);
	glDeleteSamplers(1, &SamplerName);
	glDeleteProgramPipelines(pipeline::MAX, PipelineName);

	return Validated;
}

void display()
{

    glViewportIndexedf(0, 0, 0, float(Window.Size.x), float(Window.Size.y));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.75f, 0.75f, 1.0f, 1.0f)[0]);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);

    glm::mat4 VP = camera.getprojection()*camera.ViewMatrix();
    //render the floor
    glProgramUniformMatrix4fv(ProgramName[program::FLOOR], vpMatrix, 1, GL_FALSE, glm::value_ptr(VP));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureName[texture::FLOOR]);
    glBindSampler(0, SamplerName);
    glBindVertexArray(VertexArrayName);
    glBindProgramPipeline(PipelineName[pipeline::FLOOR]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    //mass_spring simulation
    //bind shared images
    glBindImageTexture(image::POSITION, TextureName[texture::POSITION], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(image::PREVPOSITION, TextureName[texture::PREVPOSITION], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(image::VELOCITY, TextureName[texture::VELOCITY], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(image::FORCE, TextureName[texture::FORCE], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

    //calculate linear preservation forces
    glProgramUniformMatrix4fv(ProgramName[program::MS_LINEARFORCE], ulfM, 1, GL_FALSE, glm::value_ptr(tetraMesh.m_transform) );
    glProgramUniform1i(ProgramName[program::MS_LINEARFORCE], ulfrowSize, rowSize);
    glProgramUniform1i(ProgramName[program::MS_LINEARFORCE], ulfFrowSize, frowSize);
    glProgramUniform1f(ProgramName[program::MS_LINEARFORCE], ulfK, k_DVd.x);
    glProgramUniform1f(ProgramName[program::MS_LINEARFORCE], ulfDamp, k_DVd.z);

    glBindVertexArray(VertexArrayName);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,BufferName[buffer::EDGE_INDEX]);
    glEnable(GL_RASTERIZER_DISCARD);
    glBindProgramPipeline(PipelineName[pipeline::MS_LINEARFORCE]);
    glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glDrawElements( GL_LINES, 2*tetraMesh.nedge, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //calculate volume preservation forces
    glProgramUniformMatrix4fv(ProgramName[program::MS_VOLUMEFORCE], uvfM, 1, GL_FALSE, glm::value_ptr(tetraMesh.m_transform) );
    glProgramUniform1i(ProgramName[program::MS_VOLUMEFORCE], uvfrowSize, rowSize);
    glProgramUniform1i(ProgramName[program::MS_VOLUMEFORCE], uvfFrowSize, frowSize);
    glProgramUniform1f(ProgramName[program::MS_VOLUMEFORCE], uvfK, k_DVd.y);
    glProgramUniform1f(ProgramName[program::MS_VOLUMEFORCE], uvfDamp, k_DVd.w);

    glBindVertexArray(VertexArrayName);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,BufferName[buffer::TETRA_INDEX]);
    glEnable(GL_RASTERIZER_DISCARD);
    glBindProgramPipeline(PipelineName[pipeline::MS_VOLUMEFORCE]);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glDrawElements( GL_LINES_ADJACENCY, 4*tetraMesh.ntetra, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //update position
    glProgramUniformMatrix4fv(ProgramName[program::MS_UPDATEPOSITION], upM, 1, GL_FALSE, glm::value_ptr(tetraMesh.m_transform) );
    glProgramUniform1i(ProgramName[program::MS_UPDATEPOSITION], uprowSize, rowSize);
    glProgramUniform1i(ProgramName[program::MS_UPDATEPOSITION], upfrowSize, frowSize);
    glProgramUniform1f(ProgramName[program::MS_UPDATEPOSITION], uph, h);

    glBindVertexArray(VertexArrayName);
    glEnable(GL_RASTERIZER_DISCARD);
    glBindProgramPipeline(PipelineName[pipeline::MS_UPDATEPOSITION]);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glDrawArrays(GL_POINTS, 0, tetraMesh.nnode);

    //render
    glProgramUniformMatrix4fv(ProgramName[program::MS_RENDER], uM, 1, GL_FALSE, glm::value_ptr(tetraMesh.m_transform) );
    glProgramUniformMatrix4fv(ProgramName[program::MS_RENDER], uVP, 1, GL_FALSE, glm::value_ptr(VP) );
    glProgramUniformMatrix4fv(ProgramName[program::MS_RENDER], uMV, 1, GL_FALSE, glm::value_ptr(camera.ViewMatrix()) );
    glProgramUniformMatrix3fv(ProgramName[program::MS_RENDER], uNM, 1, GL_FALSE, glm::value_ptr(glm::mat3(camera.ViewMatrix())) );
    glProgramUniform4fv(ProgramName[program::MS_RENDER], uLightPos, 1, glm::value_ptr(lpos) );
    glProgramUniform4fv(ProgramName[program::MS_RENDER], uLightInt, 1, glm::value_ptr(lint) );
    glProgramUniform4fv(ProgramName[program::MS_RENDER], uKd, 1, glm::value_ptr(Kd) );
    glProgramUniform4fv(ProgramName[program::MS_RENDER], uKs, 1, glm::value_ptr(Ks) );
    glProgramUniform4fv(ProgramName[program::MS_RENDER], uKa, 1, glm::value_ptr(Ka) );
    glProgramUniform1f(ProgramName[program::MS_RENDER], uShin, shin );
    glProgramUniform1i(ProgramName[program::MS_RENDER], urowSize, rowSize);

    glBindVertexArray(VertexArrayName);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,BufferName[buffer::TETRA_INDEX]);
    glDisable(GL_RASTERIZER_DISCARD);
    glBindProgramPipeline(PipelineName[pipeline::MS_RENDER]);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glDrawElements( GL_LINES_ADJACENCY, 4*tetraMesh.ntetra, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glf::swapBuffers();
    glutPostRedisplay();
    glDisable(GL_MULTISAMPLE);
}

int main(int argc, char* argv[])
{
    tetraMesh.translate(glm::vec3(0.0f, 10.0f, 0.0f));
    //tetraMesh.rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    std::cout<<"image width:"<<rowSize<<" force image width:"<<3*tetraMesh.nnode<<std::endl;
	return glf::run(
		argc, argv,
		glm::ivec2(::SAMPLE_SIZE_WIDTH, ::SAMPLE_SIZE_HEIGHT), 
		WGL_CONTEXT_CORE_PROFILE_BIT_ARB, ::SAMPLE_MAJOR_VERSION, 
		::SAMPLE_MINOR_VERSION);
}
