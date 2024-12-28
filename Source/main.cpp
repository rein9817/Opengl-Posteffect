#include "../Externals/Include/Common.h"

#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3
#define MENU_EFFECT_NONE 4
#define MENU_EFFECT_ABSTRACTION 5
#define MENU_EFFECT_PIXELIZATION 6 
#define MENU_EFFECT_SINE_WAVE 7
#define MENU_EFFECT_BLOOM 8
#define MENU_EFFECT_WATERCOLOR 9
#define MENU_EFFECT_MAGNIFIER 10
#define MENU_EFFECT_NORMAL 11

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;
float barPosition = SCR_WIDTH / 2.0f;
bool isDragging = false;
float cameraDistance = 166.0f * 1.5f;
float cameraRotationX = 0.0f;
float cameraRotationY = 45.0f;
float mouseX = 0.0f;
float mouseY = 0.0f;


GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;
int currentEffect = MENU_EFFECT_NONE;

glm::mat4 model = glm::mat4(1.0f);
GLuint quadVAO = 0;
GLuint quadVBO;


using namespace glm;
using namespace std;
GLuint TextureFromFile(const char* path, const std::string& directory);
void renderQuad();


struct DebugInfo {
	glm::vec3 modelMin;
	glm::vec3 modelMax;
	float modelScale = 0.05f;
	float cameraDistance = 200.0f;
	float cameraAngleX = 30.0f;
	float cameraAngleY = 45.0f;
} debugInfo;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	GLuint id;
	string type;
	string path;
};

struct Camera
{
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	float yaw;
	float pitch;

	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f))
	{
		this->front = glm::vec3(0.0f, 0.0f, -1.0f);
		this->up = glm::vec3(0.0f, 1.0f, 0.0f);
		this->yaw = -90.0f;
		this->pitch = 0.0f;
		this->position = pos;
		this->updateCameraVectors();
	}

	void updateCameraVectors()
	{
		glm::vec3 newFront;
		newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		newFront.y = sin(glm::radians(pitch));
		newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(newFront);
		right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
		up = glm::normalize(glm::cross(right, front));
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}
} camera;

bool firstMouse = true;
float lastX = 0.0f;
float lastY = 0.0f;
bool mousePressed = false;

const float moveSpeed = 0.1f;



class Shader {
public:
	GLuint ID;

	Shader(const char* vertexPath, const char* fragmentPath) {
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// Compile shaders
		GLuint vertex, fragment;
		int success;
		char infoLog[512];

		// Vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// Fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// Shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use() {
		glUseProgram(ID);
	}

	void setMat4(const std::string& name, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setInt(const string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec3(const std::string& name, const glm::vec3& value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec4(const std::string& name, const glm::vec4& value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
};


class Mesh
{
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex>& inVertices, vector<GLuint>& inIndices, vector<Texture>& inTextures) {
		this->vertices = inVertices;
		this->indices = inIndices;
		this->textures = inTextures;

		setupMesh();
	}


	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			shader.setInt("texture_diffuse1", i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	GLuint VAO, VBO, EBO;
	void setupMesh() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}
};


class Model {
public:
	Model(const std::string& path) {
		loadModel(path);
	}

	void Draw(Shader shader) {
		for (auto& mesh : meshes) {
			mesh.Draw(shader);
		}
	}



private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	void loadModel(const std::string& path) {

		// get current working directory
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			std::cout << "Current working directory: " << cwd << std::endl;
		}

		// set current working directory
		directory = ".";

		const aiScene* scene = aiImportFile(path.c_str(),
			aiProcess_Triangulate |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_OptimizeMeshes);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "Assimp error: " << std::endl;
			return;
		}
		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0]) {
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			aiString texturePath;
			// if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
			//	std::cout << "Found diffuse texture: " << texturePath.C_Str() << std::endl;
			// }
			//else {
				//std::cout << "No diffuse texture found for this material" << std::endl;
			//}

			vector<Texture> diffuseMaps = loadMaterialTextures(material,
				aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);

			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++) {
				if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}

			if (!skip) {
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();

				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}

		return textures;
	}
};


GLuint TextureFromFile(const char* path, const std::string& directory) {

	std::string texturePath = path;
	std::string baseName = "lost_empire";

	std::string assetPath = "../Assets/";  // 從VC14回到上層再進入Assets
	std::string rgbPath = assetPath + baseName + "-RGB.png";
	std::string rgbaPath = assetPath + baseName + "-RGBA.png";
	std::string alphaPath = assetPath + baseName + "-Alpha.png";

	std::cout << "Attempting to load texture from: " << rgbPath << std::endl;

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, nrComponents;
	unsigned char* data = nullptr;
	bool useRGBA = false;


	data = stbi_load(rgbPath.c_str(), &width, &height, &nrComponents, 0);

	if (!data) {
		data = stbi_load(rgbaPath.c_str(), &width, &height, &nrComponents, 0);
		useRGBA = true;
	}

	if (data) {
		GLenum format = useRGBA ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenerateMipmap(GL_TEXTURE_2D);


		stbi_image_free(data);

		if (!useRGBA) {
			data = stbi_load(alphaPath.c_str(), &width, &height, &nrComponents, 0);
			if (data) {
				stbi_image_free(data);
			}
		}
	}

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << "OpenGL error during texture loading: 0x"
			<< std::hex << err << std::dec << std::endl;
	}

	return textureID;
}

void checkGLError(const char* location) {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL error at " << location << ": 0x" << std::hex << err << std::endl;
	}
}

Model* myModel = nullptr;

char** loadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char** srcp = new char* [1];
	srcp[0] = src;
	return srcp;
}

void freeShaderSource(char** srcp)
{
	delete[] srcp[0];
	delete[] srcp;
}

class PostEffect {
public:
	GLuint fbo;
	GLuint texColorBuffer;
	// shader
	Shader* pixelizationShader;
	Shader* sineWaveShader;
	Shader* watercolorShader;
	Shader* bloomShader;  
	Shader* imageAbstractionShader;
	Shader* magnifierShader;
	Shader* blurShader;

	float pixelSize = 10.0f;
	float time = 0.0f;

	float gamma = 2.2f;
	

	float bloomThreshold = 0.5f;    
	float bloomIntensity = 2.0f;   
	float softThreshold = 0.3f;     
	float exposure = 1.5f;          

	GLuint blurTexture;
	GLuint noiseTexture;
	GLuint watercolorFBO;

	PostEffect() {

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glGenTextures(1, &texColorBuffer);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);


		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer is not complete!" << std::endl;
		else
			std::cout << "Framebuffer is complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glGenFramebuffers(1, &watercolorFBO);
        glGenTextures(1, &blurTexture);
        glGenTextures(1, &noiseTexture);

		// initalize shader
		try {
			pixelizationShader = new Shader("../Assets/post.vs.glsl", "../Assets/pixelization.fs.glsl");
			magnifierShader = new Shader("../Assets/post.vs.glsl", "../Assets/magnifier.fs.glsl");
			sineWaveShader = new Shader("../Assets/post.vs.glsl", "../Assets/sinewave.fs.glsl");
			bloomShader = new Shader("../Assets/post.vs.glsl", "../Assets/bloom.fs.glsl");
			imageAbstractionShader = new Shader("../Assets/post.vs.glsl", "../Assets/abstraction.fs.glsl");
			blurShader = new Shader("../Assets/post.vs.glsl", "../Assets/blur.fs.glsl");
			watercolorShader = new Shader("../Assets/post.vs.glsl", "../Assets/watercolor.fs.glsl");
		}
		catch (const std::exception& e) {
			std::cerr << "Failed to load shaders: " << e.what() << std::endl;
		}
	}

	GLuint getColorTexture() { return texColorBuffer; }
	GLuint getFBO() { return fbo; }

	void process() {
		if (currentEffect == MENU_EFFECT_PIXELIZATION) {
			pixelizationShader->use();
			pixelizationShader->setInt("screenTexture", 0);
			pixelizationShader->setFloat("pixelSize", pixelSize);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			renderQuad();
		}
		else if (currentEffect == MENU_EFFECT_SINE_WAVE)
		{
			sineWaveShader->use();
			sineWaveShader->setInt("screenTexture", 0);
			sineWaveShader->setFloat("time", time);
			sineWaveShader->setFloat("time", time);
			sineWaveShader->setFloat("power1", 0.05f); //wave amplitude
			sineWaveShader->setFloat("power2", 10.0f);  // wave frequency

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			renderQuad();
		}
		else {
			std::cout << "No effect selected" << std::endl;
		}
	}

	void processBloom() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		bloomShader->use();
		bloomShader->setInt("screenTexture", 0);
		bloomShader->setFloat("threshold", bloomThreshold);
		bloomShader->setFloat("softThreshold", softThreshold);
		bloomShader->setFloat("bloomIntensity", bloomIntensity);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		renderQuad();
	}

	void processWatercolor() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		watercolorShader->use();
		watercolorShader->setInt("screenTexture", 0);
		watercolorShader->setFloat("distortionStrength", 0.02f);
		watercolorShader->setFloat("quantizationLevels", 8.0f);
		watercolorShader->setFloat("noiseScale", 5.0f);
		watercolorShader->setFloat("edgeStrength", 0.3f);
		watercolorShader->setFloat("noiseStrength", 0.1f);
		watercolorShader->setFloat("time", time);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		renderQuad();
	}

	void processImageAbstraction() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		imageAbstractionShader->use();
		imageAbstractionShader->setInt("screenTexture", 0);
		imageAbstractionShader->setVec2("screenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
		imageAbstractionShader->setFloat("blurRadius1", 2.0f);
		imageAbstractionShader->setFloat("blurRadius2", 4.0f);
		imageAbstractionShader->setFloat("edgeThreshold", 0.1f);
		imageAbstractionShader->setFloat("edgeStrength", 0.7f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		renderQuad();
	}

	void processMagnifier()
	{
		magnifierShader->use();
		magnifierShader->setInt("screenTexture", 0);
		magnifierShader->setFloat("magnifierRadius", 0.15f);
		magnifierShader->setFloat("magnifierBorder", 0.003f);   
		magnifierShader->setFloat("zoomFactor", 2.0f);        
		magnifierShader->setVec2("mouse", glm::vec2(mouseX, mouseY));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		renderQuad();

	}

	void updateTime(float dt) { time += dt * 0.5f; }
private:

	void setupFBO(GLuint fbo, GLuint texture) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	}
};

void My_Init() {
	glClearColor(0.529f, 0.808f, 0.922f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// 面剔除
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Alpha 混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	if (!std::ifstream("lost_empire.obj").good()) {
		std::cout << "Warning: lost_empire.obj not found!" << std::endl;
	}
	if (!std::ifstream("lost_empire-RGB.png").good()) {
		std::cout << "Warning: lost_empire-RGB.png not found!" << std::endl;
	}
	if (!std::ifstream("lost_empire-RGBA.png").good()) {
		std::cout << "Warning: lost_empire-RGBA.png not found!" << std::endl;
	}

	try {
		myModel = new Model("lost_empire.obj");
		if (myModel) {
			debugInfo.cameraDistance = 50.0f;  
			debugInfo.modelScale = 0.1f;   
			debugInfo.cameraAngleX = 30.0f;   
			debugInfo.cameraAngleY = 45.0f;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error initializing model: " << e.what() << std::endl;
	}
}


// modify to show the before and after images
void My_Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	static float lastTime = 0.0f;
	static PostEffect* imageAbstraction = nullptr;
	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	if ((currentEffect == MENU_EFFECT_PIXELIZATION ||
		currentEffect == MENU_EFFECT_SINE_WAVE ||
		currentEffect == MENU_EFFECT_BLOOM ||
		currentEffect == MENU_EFFECT_WATERCOLOR ||
		currentEffect == MENU_EFFECT_ABSTRACTION ||
		currentEffect == MENU_EFFECT_MAGNIFIER)) {

		if (!imageAbstraction) imageAbstraction = new PostEffect();
		glBindFramebuffer(GL_FRAMEBUFFER, imageAbstraction->getFBO());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	if (imageAbstraction && currentEffect == MENU_EFFECT_SINE_WAVE) {
		imageAbstraction->updateTime(deltaTime);
	}


	// 渲染場景
	if (myModel) {
		static Shader* modelShader = nullptr;
		if (!modelShader) {
			modelShader = new Shader("../Assets/vertex.vs.glsl", "../Assets/fragment.fs.glsl");
		}

		modelShader->use();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, debugInfo.cameraDistance * 10.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.1f));

		modelShader->setMat4("projection", projection);
		modelShader->setMat4("view", view);
		modelShader->setMat4("model", model);
		modelShader->setBool("showNormal", currentEffect == MENU_EFFECT_NORMAL);
		myModel->Draw(*modelShader);
	}

	if (currentEffect == MENU_EFFECT_PIXELIZATION ||
		currentEffect == MENU_EFFECT_SINE_WAVE) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		imageAbstraction->process();
	}
	else if (currentEffect == MENU_EFFECT_BLOOM) {
		imageAbstraction->processBloom();
	}
	else if (currentEffect == MENU_EFFECT_WATERCOLOR) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		imageAbstraction->processWatercolor();
	}
	else if (currentEffect == MENU_EFFECT_ABSTRACTION) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		imageAbstraction->processImageAbstraction();
	}
	else if (currentEffect == MENU_EFFECT_MAGNIFIER) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		imageAbstraction->processMagnifier();
	}

	glutSwapBuffers();
}




void My_MouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0) 
		cameraDistance *= 0.9f;
	else
		cameraDistance *= 1.1f;

	glutPostRedisplay();
}

void My_SpecialKeys(int key, int x, int y)
{
	const float rotationSpeed = 5.0f;

	switch (key)
	{
	case GLUT_KEY_LEFT:
		cameraRotationY -= rotationSpeed;
		break;
	case GLUT_KEY_RIGHT:
		cameraRotationY += rotationSpeed;
		break;
	case GLUT_KEY_UP:
		cameraRotationX -= rotationSpeed;
		break;
	case GLUT_KEY_DOWN:
		cameraRotationX += rotationSpeed;
		break;
	}

	glutPostRedisplay();
}


void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(timer_speed, My_Timer, val);
}

void My_Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		mousePressed = true;
		lastX = x;
		lastY = y;
	}
	else if (state == GLUT_UP)
	{
		mousePressed = false;
	}


}


void My_Motion(int x, int y)
{
	if (mousePressed)
	{

		if (firstMouse)
		{
			lastX = x;
			lastY = y;
			firstMouse = false;
		}

		float xoffset = x - lastX;
		float yoffset = lastY - y;
		lastX = x;
		lastY = y;

		float sensitivity = 0.2f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera.yaw += xoffset;
		camera.pitch += yoffset;

		if (camera.pitch > 89.0f) camera.pitch = 89.0f;
		if (camera.pitch < -89.0f) camera.pitch = -89.0f;

		camera.updateCameraVectors();
	}

	mouseX = x / (float)SCR_WIDTH;
	mouseY = 1.0f - y / (float)SCR_HEIGHT;

	glutPostRedisplay();
}

void My_Keyboard(unsigned char key, int x, int y)
{

	float moveSpeed = 0.2f;
	switch (key)
	{
	case 'w':
		camera.position += moveSpeed * camera.front;
		break;
	case 's':
		camera.position -= moveSpeed * camera.front;
		break;
	case 'a':
		camera.position -= moveSpeed * camera.right;
		break;
	case 'd':
		camera.position += moveSpeed * camera.right;
		break;
	case 'z':
		camera.position.y += moveSpeed;
		break;
	case 'x':
		camera.position.y -= moveSpeed;
		break;
	}

	glutPostRedisplay();
}

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void My_Menu(int id)
{
	switch (id)
	{
	case MENU_TIMER_START:
		if (!timer_enabled)
		{
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	case MENU_EXIT:
		exit(0);
		break;
	case MENU_EFFECT_NONE:
		currentEffect = MENU_EFFECT_NONE;
		break;
	case MENU_EFFECT_PIXELIZATION:
		currentEffect = MENU_EFFECT_PIXELIZATION;
		break;
	case MENU_EFFECT_SINE_WAVE:
		currentEffect = MENU_EFFECT_SINE_WAVE;
		break;
	case MENU_EFFECT_BLOOM:
		currentEffect = MENU_EFFECT_BLOOM;
		break;
	case MENU_EFFECT_WATERCOLOR:
		currentEffect = MENU_EFFECT_WATERCOLOR;
		break;
	case MENU_EFFECT_ABSTRACTION:
		std::cout << "Setting effect to IMAGE ABSTRACTION" << std::endl;
		currentEffect = MENU_EFFECT_ABSTRACTION;
		break;
	case MENU_EFFECT_MAGNIFIER:
		currentEffect = MENU_EFFECT_MAGNIFIER;
		break;
	case MENU_EFFECT_NORMAL:
		currentEffect = MENU_EFFECT_NORMAL;
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
#ifdef __APPLE__
	// Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("AS2_Framework"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif
	dumpInfo();
	My_Init();

	// Create a menu and bind it to mouse right button.
	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);
	int menu_effect = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddSubMenu("Effects", menu_effect);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_effect);
	glutAddMenuEntry("None", MENU_EFFECT_NONE);
	glutAddMenuEntry("Normal", MENU_EFFECT_NORMAL);
	glutAddMenuEntry("Image Abstraction", MENU_EFFECT_ABSTRACTION);
	glutAddMenuEntry("Pixelization", MENU_EFFECT_PIXELIZATION);
	glutAddMenuEntry("Sine Wave", MENU_EFFECT_SINE_WAVE);
	glutAddMenuEntry("Bloom", MENU_EFFECT_BLOOM);
	glutAddMenuEntry("Watercolor", MENU_EFFECT_WATERCOLOR);
	glutAddMenuEntry("Magnifier", MENU_EFFECT_MAGNIFIER);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutMotionFunc(My_Motion);
	glutMouseWheelFunc(My_MouseWheel);
	glutTimerFunc(timer_speed, My_Timer, 0);

	// Enter main event loop.
	glutMainLoop();

	return 0;
}