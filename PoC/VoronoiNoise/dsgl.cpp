#include "dsgl.hpp"

namespace DSGL {
	// ----- Exception ----- //
	
	Exception::Exception(int code, const char* msg) : Exception(code, msg, NULL) {}
	
	Exception::Exception(int code, const char * msg, const char * ressource) {
		this->code = code;
		this->msg = std::string(msg);
		#ifdef DSGL_DEBUG
			if (ressource != NULL) {
				this->ressource = std::string(ressource);
				std::cerr << "DSGL: " << this->ressource << "\n";
			}
			std::cerr << this->msg << "\n";
		#endif
	}
	
	// ----- Context ----- //
	
	Context::Context(const char * name, int width, int height, int glMajorVersion, int glMinorVersion) {
		this->name = std::string(name);
		this->width = width;
		this->height = height;
		this->glMajorVersion = glMajorVersion;
		this->glMinorVersion = glMinorVersion;
		this->window = 0;
	}
	
	Context::Context(int width, int height, int glMajorVersion, int glMinorVersion) : Context("", width, height, glMajorVersion, glMinorVersion) {}
	
	Context::~Context() {
		#ifdef DSGL_GLFW
			glfwTerminate();
		#endif
	}
	
	void Context::InitSimpleWindow() {
		#if defined(DSGL_GLFW)
			if (!glfwInit()) {
				throw Exception(DSGL_GLFW_INIT_FAILED, DSGL_MSG_GLFW_INIT_FAILED);
			}

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->glMajorVersion);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->glMinorVersion);

			this->window = glfwCreateWindow(this->width, this->height, this->name.c_str(), NULL, NULL);

			if (!window)  {
				throw Exception(DSGL_WINDOW_POINTER_NULL, DSGL_MSG_WINDOW_POINTER_NULL);
			}

			glfwMakeContextCurrent(window);
		#endif
		#if defined(DSGL_GL3W)
		  if(gl3wInit() != 0) {
		    throw Exception(DSGL_GL3W_INIT_FAILED, DSGL_MSG_GL3W_INIT_FAILED);
		  }
		#endif
	}
	
	// FrameBufferObject //

	FrameBufferObject::FrameBufferObject(GLuint width, GLuint height) : FrameBufferObject(width, height, DSGL_FBO_DEPTH, GL_RGB) {}

	FrameBufferObject::FrameBufferObject(GLuint width, GLuint height, bool withDepthBuffer, GLenum bufferType) {
	  	// Create FBO //
	  	this->width = width;
		this->height = height;

		glGenFramebuffers(1, &this->ID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->ID);
		if (!glIsBuffer(this->ID)) {
			throw Exception(DSGL_CANNOT_CREATE_FBO, DSGL_MSG_CANNOT_CREATE_FBO);
		}
		// Create destination texture //
		glGenTextures(1, &this->textureID);
		glBindTexture(GL_TEXTURE_2D, this->textureID);
		if(!glIsTexture(this->textureID)) {
			throw Exception(DSGL_CANNOT_CREATE_TEXTURE, DSGL_MSG_CANNOT_CREATE_TEXTURE);
		}
		glTexImage2D(GL_TEXTURE_2D, 0,bufferType, width, height, 0, bufferType, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
		// Optional depth buffer //
		if (withDepthBuffer) {
			glGenRenderbuffers(1, &this->depthBufferID);
			glBindRenderbuffer(GL_RENDERBUFFER, this->depthBufferID);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthBufferID);
		}

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->textureID, 0);
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw Exception(DSGL_CANNOT_CREATE_FBO, DSGL_MSG_CANNOT_CREATE_FBO);
		}

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBufferObject::DrawBind() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->ID);
		if (!glIsBuffer(this->ID)) {
			throw Exception(DSGL_FBO_DOESNT_EXIST, DSGL_MSG_CANNOT_CREATE_FBO);
		}
		glViewport(0,0,this->width,this->height);
	}
	void FrameBufferObject::Bind() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->textureID);
		glBindImageTexture (0, this->textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	void FrameBufferObject::DrawUnbind() {
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBufferObject::Unbind() {
    		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// ----- VertexBufferObject ----- //
	
        VertexBufferObject::VertexBufferObject(GLsizeiptr size, const GLvoid * data) : VertexBufferObject(size, data, GL_STATIC_DRAW) {}
	
	VertexBufferObject::VertexBufferObject(GLsizeiptr size, const GLvoid * data, GLenum usage) {
		glGenBuffers(1, &this->ID);
		glBindBuffer(GL_ARRAY_BUFFER, this->ID);
			if(glIsBuffer(this->ID)) {
				glBufferData(GL_ARRAY_BUFFER, size, data, usage);
			}
			else {
				throw Exception(DSGL_CANNOT_CREATE_VBO, DSGL_MSG_CANNOT_CREATE_VBO);
			}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	VertexBufferObject::~VertexBufferObject() {
		glDeleteBuffers(1, &this->ID);
	}
	
	void VertexBufferObject::Bind() {
		if(this->ID != 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
			if(!glIsBuffer(this->ID)) {
				throw Exception(DSGL_VBO_DOESNT_EXIST, DSGL_MSG_VBO_DOESNT_EXIST);
			}
		}
		else {
			throw Exception(DSGL_VBO_IS_NULL, DSGL_MSG_VBO_IS_NULL);
		}
	}

	// ----- TextureBuffer ----- //

	TextureBuffer::TextureBuffer() {
		glGenTextures(1, &this->textureID);
		glBindTexture(GL_TEXTURE_BUFFER, this->textureID);
		if(!glIsTexture(this->textureID)) {
			throw Exception(DSGL_CANNOT_CREATE_TEXTURE, DSGL_MSG_CANNOT_CREATE_TEXTURE);
		}
		glBindTexture(GL_TEXTURE_BUFFER, 0);

		glGenBuffers(1, &this->bufferID);
  		glBindBuffer(GL_TEXTURE_BUFFER, this->bufferID);
		if(!glIsBuffer(this->bufferID)) {
			throw Exception(DSGL_CANNOT_CREATE_TEXTURE_BUFFER, DSGL_MSG_CANNOT_CREATE_TEXTURE_BUFFER);
		}
  		glBindBuffer(GL_TEXTURE_BUFFER, 0);
	}

	TextureBuffer::TextureBuffer(GLsizeiptr size, const GLvoid * data, GLenum usage) : TextureBuffer(size, data, usage, GL_RGBA32F) {}

	TextureBuffer::TextureBuffer(GLsizeiptr size, const GLvoid * data, GLenum usage, GLenum gpuSideFormat) : TextureBuffer() {
		this->size = size;
		this->usage = usage;
		this->gpuSideFormat = gpuSideFormat;
		glBindBuffer(GL_TEXTURE_BUFFER, this->bufferID);
  		glBufferData(GL_TEXTURE_BUFFER, size, data, usage);
		glBindBuffer(GL_TEXTURE_BUFFER,0);
	}

	TextureBuffer::~TextureBuffer() {
		glDeleteTextures(1, &this->textureID);
		glDeleteBuffers(1, &this->bufferID);
	}

	 void TextureBuffer::Bind() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, this->textureID);
		glBindBuffer(GL_TEXTURE_BUFFER, this->bufferID);
		glTexBuffer(GL_TEXTURE_BUFFER, this->gpuSideFormat, this->bufferID);
	}

	void TextureBuffer::Bind(GLuint unit) {
		this->Bind();
		glBindImageTexture(unit, this->textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, gpuSideFormat);
	}
	
	void TextureBuffer::Unbind() {
    		glBindTexture(GL_TEXTURE_BUFFER, 0);
		glBindBuffer(GL_TEXTURE_BUFFER,0);
	}
	

	// ---- Textures ----- //

	Texture::Texture(GLuint target) {
		this->target = target;
		glGenTextures(1, &this->ID);
		glBindTexture(target, this->ID);
		if(!glIsTexture(this->ID)) {
			throw Exception(DSGL_CANNOT_CREATE_TEXTURE, DSGL_MSG_CANNOT_CREATE_TEXTURE);
		}
		glBindTexture(target, 0);
	}

	Texture::Texture(GLenum target, GLuint width, GLuint height, GLvoid * rawData) : Texture(target, width, height, rawData, GL_RGBA, GL_FLOAT, GL_RGBA32F) {}
	
	Texture::Texture(GLenum target, GLuint width, GLuint height, GLvoid * rawData, GLenum cpuSideFormat, GLenum cpuSideType) : Texture(target, width, height, rawData, cpuSideFormat, cpuSideType, GL_RGBA32F) {
	}

	Texture::Texture(GLenum target, GLuint width, GLuint height, GLvoid * rawData, GLenum cpuSideFormat, GLenum cpuSideType, GLint gpuSideFormat) : Texture(target) {
	  	this->width = width;
		this->height = height;
		this->rawData = rawData;
		this->cpuSideFormat = cpuSideFormat;
		this->cpuSideType = cpuSideType;
		this->gpuSideFormat = gpuSideFormat;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(target, this->ID); {
			glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glGenerateMipmap(target);
			if( width % 4 != 0) {
			  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // In case of NPOT width texture, prevent from misalignment shit.
			}
			if(target == GL_TEXTURE_2D) {
				glTexImage2D(target, 0, gpuSideFormat, this->width, this->height, 0, cpuSideFormat, cpuSideType, this->rawData);
			}
			else if(target == GL_TEXTURE_1D) {
				glTexImage1D(target, 0, gpuSideFormat, this->width, 0, cpuSideFormat, cpuSideType, this->rawData);
			}
		}
		glBindTexture(target, 0);
	}

	Texture::~Texture() {
		glDeleteTextures(1, &this->ID);
	}

	void Texture::Bind() {
		this->Bind(0);
	}

	void Texture::Bind(GLuint unit) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(this->target, this->ID);
		glBindImageTexture(unit, this->ID, 0, GL_FALSE, 0, GL_WRITE_ONLY, gpuSideFormat);
	}

	void Texture::Unbind() {
		glBindImageTexture (0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, gpuSideFormat);
    		glBindTexture(this->target, 0);
	}

	// ---- VertexArrayObject ----- //
	
	VertexArrayObject::VertexArrayObject() {
		glGenVertexArrays(1, &this->ID);
		this->Bind();
		glBindVertexArray(0);
	}

	VertexArrayObject::VertexArrayObject(GLuint IBO, GLuint VBO) {
		glGenVertexArrays(1, &this->ID);
		this->Bind(); {	
			this->SetVertex(VBO);
			this->SetElements(IBO);
		}
		glBindVertexArray(0);
	}
	
	VertexArrayObject::VertexArrayObject(GLuint IBO, GLuint VBO, GLuint instances) {
		glGenVertexArrays(1, &this->ID);
		this->Bind(); {	
			this->SetVertex(VBO);
			this->SetElements(IBO);
			this->SetInstances(instances);
		}
		glBindVertexArray(0);
		
	}
	
	void VertexArrayObject::Bind() {
		glBindVertexArray(this->ID);
		if(!glIsVertexArray(this->ID)) {
			throw Exception(DSGL_VAO_DOESNT_EXIST, "DSGL: VAO doesn't exist.");			
		}
	}
	
	void VertexArrayObject::SetVertex(GLuint vertex) {
		this->Bind(); {
			if (vertex == 0) {
				throw Exception(DSGL_VBO_IS_NULL, DSGL_MSG_VBO_IS_NULL);
			}
			glBindBuffer(GL_ARRAY_BUFFER, vertex);
			if(!glIsBuffer(vertex)) {
				throw Exception(DSGL_VBO_DOESNT_EXIST, "DSGL: Vertex buffer doesn't exist.");
			}
			this->VBO = vertex;
		}
		glBindVertexArray(0);
	}

	

	void VertexArrayObject::SetElements(GLuint elements) {
		this->Bind(); {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);
			if(elements != 0 && !glIsBuffer(elements)) {
				throw Exception(DSGL_IBO_DOESNT_EXIST, "DSGL: Elements buffer doesn't exist.");
			}
			this->IBO = elements;
		}
		glBindVertexArray(0);
	}
	
	void VertexArrayObject::SetInstances(GLuint instances) {
		this->Bind(); {
			if (instances == 0) {
				throw Exception(DSGL_INSTANCES_IS_NULL, "DSGL: instance buffer name is null.");
			}
			glBindBuffer(GL_ARRAY_BUFFER, instances);
			if(!glIsBuffer(instances)) {
				throw Exception(DSGL_INSTANCES_DOESNT_EXIST, "DSGL: instances buffer doesn't exist.");
			}
			this->instances = instances;
		}
		glBindVertexArray(0);
	}
	
	void VertexArrayObject::AttribPointer(GLuint buffer, GLuint index,GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) {
		if(!glIsVertexArray(this->ID)) {
			throw Exception(DSGL_VAO_DOESNT_EXIST, "DSGL: VAO doesn't exist.");			
		}
		glBindVertexArray(this->ID); {
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			if(!glIsBuffer(this->VBO)) {
				throw Exception(DSGL_VBO_DOESNT_EXIST, "DSGL: Vertex buffer doesn't exist.");
			}
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, size, type, normalized, stride, pointer);
		}
		glBindVertexArray(0);
	}

	void VertexArrayObject::AttribPointer(GLuint index,GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) {
		this->AttribPointer(this->VBO, index, size, type, normalized, stride, pointer);
	}
	
	void VertexArrayObject::InstancesAttribPointer(GLuint index,GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer, GLuint divisor) {
		if(!glIsVertexArray(this->ID)) {
			throw Exception(DSGL_VAO_DOESNT_EXIST, "DSGL: VAO doesn't exist.");			
		}
		glBindVertexArray(this->ID); {
			glBindBuffer(GL_ARRAY_BUFFER, this->instances);
			if(!glIsBuffer(this->instances)) {
				throw Exception(DSGL_INSTANCES_DOESNT_EXIST, "DSGL: instances buffer doesn't exist.");
			}
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, size, type, normalized, stride, pointer);
			glVertexAttribDivisor(index, divisor);
		}
		glBindVertexArray(0);
	}
	
	VertexArrayObject::~VertexArrayObject() {
		glDeleteVertexArrays(1,&this->ID);
	}
	
	// ---- Elements ----- //
	
	Elements::Elements(GLsizeiptr size, const GLvoid * data) : Elements(size, data, GL_STATIC_DRAW){}
	
	Elements::Elements(GLsizeiptr size, const GLvoid * data, GLenum usage) {
		glGenBuffers(1, &this->ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID); {
			if(glIsBuffer(this->ID)) {
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
			}
			else {
				throw Exception(DSGL_CANNOT_CREATE_IBO, "DSGL: Elements buffer creation failed.");
			}
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	Elements::~Elements() {
		glDeleteBuffers(1, &this->ID);
	}
	
	void Elements::Bind() {
		if (this->ID != 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
			if(!glIsBuffer(this->ID)) {
				throw Exception(DSGL_IBO_DOESNT_EXIST, "DSGL: Elements buffer creation failed.");
			}
		}
		else {
			throw Exception(DSGL_IBO_IS_NULL, "DSGL: IBO is null.");
		}
	}
	
	// ---- Shader ----- //
		
	Shader::Shader(const char * inputShader, GLuint shaderType, int option) {
		if (inputShader == NULL) {
			this->ID = 0;
			return;
		}
				
		this->Result = GL_FALSE;
		
		// Create shader //

		this->ID = glCreateShader(shaderType);
		if (this->ID == 0) {
			if ((option & 1) == 0) {
				throw Exception(DSGL_CANNOT_CREATE_SHADER, DSGL_MSG_CANNOT_CREATE_SHADER, inputShader);
			}
			else {
				throw Exception(DSGL_CANNOT_CREATE_SHADER, DSGL_MSG_CANNOT_CREATE_SHADER);
			}
		}	

		// Copy from existing string //
		if ((option & 1) == 1)  {
			this->shaderSource = std::string(inputShader);
		}

		// Read from file and load into memory //
		else {
			ReadFromFile(inputShader);
		}

		// Read from memory and compile //
		const char * shaderSource_ptr = this->shaderSource.c_str();
		glShaderSource(this->ID, 1, &shaderSource_ptr, NULL);
		glCompileShader(this->ID);
		
		glGetShaderiv(this->ID, GL_COMPILE_STATUS, &this->Result);

		if (!this->Result) {
			this->shaderErrorMessages = new char[DSGL_SHADER_ERROR_LENGTH]() ;
			glGetShaderInfoLog(this->ID, DSGL_SHADER_ERROR_LENGTH, NULL, this->shaderErrorMessages);
			std::cout <<  this->shaderErrorMessages << "\n" ;
			delete[] this->shaderErrorMessages; this->shaderErrorMessages = NULL;
			if ((option & 1) == 0) {
				throw Exception(DSGL_ERROR_AT_SHDR_COMPILE_TIME, DSGL_MSG_ERROR_AT_SHDR_COMPILE_TIME, inputShader);
			}
			else {
				throw Exception(DSGL_ERROR_AT_SHDR_COMPILE_TIME, DSGL_MSG_ERROR_AT_SHDR_COMPILE_TIME);
			}
		}
	}
	
	Shader::Shader(const char * inputShader, GLuint shaderType) : Shader(inputShader, shaderType, DSGL_READ_FROM_FILE) {}
	
	void Shader::ReadFromFile(const char * shaderFilename) {
		FILE * shader = fopen (shaderFilename, "r");
		char * stringBuffer = NULL;
		if (shader == 0) {
			throw Exception(DSGL_CANNOT_READ_SHADER_SOURCE, "DSGL: Cannot read shader source.", shaderFilename);
		}
		this->shaderSourceSize = this->GetFileSize(shaderFilename)+1;
		stringBuffer = new char[this->shaderSourceSize];
		for (int i=0; i < this->shaderSourceSize; i++) {
			stringBuffer[i] = (unsigned char ) fgetc(shader);
			if (stringBuffer[i] == EOF) {
				stringBuffer[i] = '\0';
				break;
			}
		}
		
		this->shaderSource.assign(stringBuffer, this->shaderSourceSize);
		
		fclose(shader);
		delete[] stringBuffer;
	}

	int Shader::GetFileSize(const char * inputFilePath) {
		// http://www.cplusplus.com/doc/tutorial/files/
		std::streampos begin, end;
		std::ifstream inputFile(inputFilePath, std::ios::binary);
		begin = inputFile.tellg();
		inputFile.seekg(0, std::ios::end);
		end = inputFile.tellg();
		inputFile.close();
		return int(end - begin);
	}

	
	Shader::~Shader() {
		glDeleteShader(this->ID);
	}

	// ComputeShader //

	ComputeProgram::ComputeProgram(const char * inputShader, int option) {
		this->compute = std::make_shared<Shader>(inputShader, GL_COMPUTE_SHADER, option);
		GLint InfoLogLength = 0;
		
		this->ID = glCreateProgram();
		glAttachShader(this->ID, this->compute->ID);
  		glLinkProgram(this->ID);
		glGetProgramiv(this->ID, GL_LINK_STATUS, &this->Result);

		if (!Result) {
			glGetProgramiv(this->ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			this->programErrorMessages = new char[DSGL_SHADER_ERROR_LENGTH]() ;
			glGetProgramInfoLog(this->ID, GL_INFO_LOG_LENGTH, NULL, &this->programErrorMessages[0]);
			std::cout << this->programErrorMessages << "\n";
			delete[] this->programErrorMessages;
			
			if (glIsProgram(this->ID)) {
				glDeleteProgram(this->ID);
			}			
			
			throw DSGL_ERROR_AT_SHDR_COMPILE_TIME;
		}
	}

	ComputeProgram::ComputeProgram(const char * inputShader) : ComputeProgram(inputShader, DSGL_READ_FROM_FILE) {}

	ComputeProgram::~ComputeProgram() {
		if (glIsShader(this->compute->ID)) {
			glDetachShader(this->ID, this->compute->ID);
		}
		if (glIsProgram(this->ID)) {
			glDeleteProgram(this->ID);
		}
	}

	void ComputeProgram::Use() {
		this->Use(1,1,1);
	}

	void ComputeProgram::Use(GLuint x, GLuint y, GLuint z) {
		if(glIsProgram(this->ID)) {
			glUseProgram(this->ID);
			glDispatchCompute(x,y,z);
		}
		else {
			throw Exception(DSGL_ID_DOESNT_NAME_A_PROGRAM, DSGL_MSG_ID_DOESNT_NAME_A_PROGRAM);
		}
	}

	// ----- PipelineProgram ----- //

	PipelineProgram::PipelineProgram(const char * inputVertexShader, const char * inputFragmentShader) : PipelineProgram(inputVertexShader, NULL, NULL, NULL, inputFragmentShader) {}
	
	PipelineProgram::PipelineProgram(
		const char * inputVertexShader,
		const char * inputTesselationControlShader,
		const char * inputTesselationEvaluationShader,
		const char * inputGeometryShader,
		const char * inputFragmentShader) {
		
		GLint InfoLogLength = 0;
		
		// Create program //
		this->ID = glCreateProgram();
		if (!glIsProgram(this->ID)) {
			throw Exception(DSGL_CANNOT_CREATE_PROGRAM, "DSGL: Cannot create program");
		}	

		// Create shaders //
		this->vertex = std::make_shared<Shader>(inputVertexShader, GL_VERTEX_SHADER, DSGL_READ_FROM_FILE);
		this->tesselationControl = std::make_shared<Shader>(inputTesselationControlShader, GL_TESS_CONTROL_SHADER, DSGL_READ_FROM_FILE);
		this->tesselationEvaluation = std::make_shared<Shader>(inputTesselationEvaluationShader, GL_TESS_EVALUATION_SHADER, DSGL_READ_FROM_FILE);
		this->geometry = std::make_shared<Shader>(inputGeometryShader, GL_GEOMETRY_SHADER, DSGL_READ_FROM_FILE);
		this->fragment = std::make_shared<Shader>(inputFragmentShader, GL_FRAGMENT_SHADER, DSGL_READ_FROM_FILE);

		// Link and compile //

		if (glIsShader(this->vertex->ID)) {
			glAttachShader(this->ID, this->vertex->ID);
		}

		if (glIsShader(this->tesselationControl->ID)) {
			glAttachShader(this->ID, this->tesselationControl->ID);
		}
		
		if (glIsShader(this->tesselationEvaluation->ID)) {
			glAttachShader(this->ID, this->tesselationEvaluation->ID);
		}
		
		if (glIsShader(this->geometry->ID)) {
			glAttachShader(this->ID, this->geometry->ID);
		}
		
		if (glIsShader(this->fragment->ID)) {
			glAttachShader(this->ID, this->fragment->ID);
		}
		
		glLinkProgram(this->ID);
		
		// Clean shaders //
		Clean(DSGL_CLEAN_SHADERS_ONLY);

		glGetProgramiv(this->ID, GL_LINK_STATUS, &this->Result);

		if (!Result) {
			glGetProgramiv(this->ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			this->programErrorMessages = new char[DSGL_SHADER_ERROR_LENGTH]() ;
			glGetProgramInfoLog(this->ID, GL_INFO_LOG_LENGTH, NULL, &this->programErrorMessages[0]);
			std::cout << this->programErrorMessages << "\n";
			delete[] this->programErrorMessages;
			Clean(DSGL_CLEAN_ALL);
			throw DSGL_ERROR_AT_SHDR_COMPILE_TIME;
		}
	}	
	
	PipelineProgram::~PipelineProgram() {
		Clean(DSGL_CLEAN_ALL);
	}
	
	void PipelineProgram::Use() {
		if(glIsProgram(this->ID)) {
			glUseProgram(this->ID);
		}
		else {
			throw Exception(DSGL_ID_DOESNT_NAME_A_PROGRAM, DSGL_MSG_ID_DOESNT_NAME_A_PROGRAM);
		}
	}
	
	void PipelineProgram::Clean(bool shadersOnly) {
		if (glIsShader(this->vertex->ID)) {
			glDetachShader(this->ID, this->vertex->ID);
			glDeleteShader(this->vertex->ID);
		}

		if (glIsShader(this->tesselationControl->ID)) {
			glDetachShader(this->ID, this->tesselationControl->ID);
			glDeleteShader(this->tesselationControl->ID);
		}
		
		if (glIsShader(this->tesselationEvaluation->ID)) {
			glDetachShader(this->ID, this->tesselationEvaluation->ID);
			glDeleteShader(this->tesselationEvaluation->ID);
		}
		
		if (glIsShader(this->geometry->ID)) {
			glDetachShader(this->ID, this->geometry->ID);
			glDeleteShader(this->geometry->ID);
		}
		
		if (glIsShader(this->fragment->ID)) {
			glDetachShader(this->ID, this->fragment->ID);
			glDeleteShader(this->fragment->ID);
		}
		
		if (!shadersOnly) {
			if (glIsProgram(this->ID)) {
				glDeleteProgram(this->ID);
			}			
		}
	}

	// Shader Program //
	void ShaderProgram::Uniformui(const char * uniformName, GLuint v0) {
		GLint loc = glGetUniformLocation(this->ID, uniformName);
		if (loc != -1) {
			glUniform1ui(loc, v0);
		}
		else {
		  throw Exception(DSGL_UNIFORM_LOCATION_DOESNT_EXISTS, DSGL_MSG_UNIFORM_LOCATION_DOESNT_EXISTS, uniformName);
		}
	}

	void ShaderProgram::Uniformf(const char * uniformName, GLfloat v0) {
		GLint loc = glGetUniformLocation(this->ID, uniformName);
		if (loc != -1) {
			glUniform1f(loc, v0);
		}
		else {
		  throw Exception(DSGL_UNIFORM_LOCATION_DOESNT_EXISTS, DSGL_MSG_UNIFORM_LOCATION_DOESNT_EXISTS, uniformName);
		}
	}

	void ShaderProgram::Uniformf(const char * uniformName, GLfloat v0, GLfloat v1) {
		GLint loc = glGetUniformLocation(this->ID, uniformName);
		if (loc != -1) {
			glUniform2f(loc, v0, v1);
		}
		else {
		  throw Exception(DSGL_UNIFORM_LOCATION_DOESNT_EXISTS, DSGL_MSG_UNIFORM_LOCATION_DOESNT_EXISTS, uniformName);
		}
	}

	void ShaderProgram::Uniformf(const char * uniformName, GLfloat v0, GLfloat v1, GLfloat v2) {
		GLint loc = glGetUniformLocation(this->ID, uniformName);
		if (loc != -1) {
			glUniform3f(loc, v0, v1, v2);
		}
		else {
		  throw Exception(DSGL_UNIFORM_LOCATION_DOESNT_EXISTS, DSGL_MSG_UNIFORM_LOCATION_DOESNT_EXISTS, uniformName);
		}
	}

	void ShaderProgram::Uniform4fv(const char * uniformName, int size, GLfloat * array) {
		GLint loc = glGetUniformLocation(this->ID, uniformName);
		if (loc != -1) {
			glUniform4fv(loc, size, array);
		}
		else {
		  throw Exception(DSGL_UNIFORM_LOCATION_DOESNT_EXISTS, DSGL_MSG_UNIFORM_LOCATION_DOESNT_EXISTS, uniformName);
		}
	}

	void ShaderProgram::Uniform3fv(const char * uniformName, int size, GLfloat * array) {
		GLint loc = glGetUniformLocation(this->ID, uniformName);
		if (loc != -1) {
			glUniform3fv(loc, size, array);
		}
		else {
		  throw Exception(DSGL_UNIFORM_LOCATION_DOESNT_EXISTS, DSGL_MSG_UNIFORM_LOCATION_DOESNT_EXISTS, uniformName);
		}
	}

	// ----- Miscellaneous functions ----- //
	
	void PrintNicelyWorkGroupsCapabilities() {
		int workgroup_count[3];
		int workgroup_size[3];
		int workgroup_invocations;

		for (int i=0; i < 3; i++) {	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, i, &workgroup_count[i]); }

		printf ("DSGL: MAXIMUM WORK GROUP COUNT:\n\tx:%u\n\ty:%u\n\tz:%u\n",
		workgroup_size[0], workgroup_size[1], workgroup_size[2]);
		
		for (int i=0; i < 3; i++) { glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, &workgroup_size[i]); }

		printf ("DSGL: MAXIMUM WORK GROUP SIZE:\n\tx:%u\n\ty:%u\n\tz:%u\n",
		workgroup_size[0], workgroup_size[1], workgroup_size[2]);

		glGetIntegerv (GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workgroup_invocations);
		printf ("DSGL: MAXIMUM WORK GROUP INVOCATIONS:\n\t%u\n", workgroup_invocations);
	}
}
