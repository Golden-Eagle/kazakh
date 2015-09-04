#include "Assets.hpp"
#include "Material.hpp"

using namespace std;
using namespace pxljm;
using namespace i3d;


Material::Material() { }


Material::~Material() { }


void Material::bind(i3d::mat4d projectionMatrix, float zfar) {
	// basic
	glUniformMatrix4fv(shader->uniformLocation("uProjectionMatrix"), 1, true, i3d::mat4f(projectionMatrix));
	glUniform1f(shader->uniformLocation("uZFar"), zfar);

	// material
	glUniform3f(shader->uniformLocation("uDiffuse"), m_diffuse.x(), m_diffuse.y(), m_diffuse.z());
	glUniform1f(shader->uniformLocation("uMetalicity"), m_metalicity);
	glUniform1f(shader->uniformLocation("uRoughness"), m_roughness);
	glUniform1f(shader->uniformLocation("uSpecular"), m_specular);

	// material maps
	GLuint textureLocation = 0;
	auto setTexture = [&](texture_ptr tex,  const string &sampler) {
		tex->bind(textureLocation);
		glUniform1i(shader->uniformLocation(sampler), textureLocation);
		textureLocation++;
	};

	if (m_useDiffuseMap && m_diffuseMap) setTexture(m_diffuseMap, "uDiffuseMap");
	if (m_normalMap) setTexture(m_normalMap, "uNormalMap");



	vector<GLuint> fragSubroutines(shader->activeSubroutines(GL_FRAGMENT_SHADER), 0);

	if (fragSubroutines.size() > 0) {
		auto addFragSubroutine = [&](const string &uniform, const string &name) {
			GLuint uidx = shader->subroutineUniformIndex(GL_FRAGMENT_SHADER, uniform);
			GLuint pidx = shader->subroutineIndex(GL_FRAGMENT_SHADER, name);
			if (uidx != GL_INVALID_INDEX && pidx != GL_INVALID_INDEX) {
				fragSubroutines[uidx] = pidx;
			}
		};

		addFragSubroutine("doRender", "material");

		// diffuse
		if (m_useDiffuseMap && m_diffuseMap) addFragSubroutine("getDiffuse", "diffuseFromTexture");
		else addFragSubroutine("getDiffuse", "diffuseFromValue");

		// metalicity
		addFragSubroutine("getMetalicity", "metalicityFromValue");
		// roughness
		addFragSubroutine("getRoughness", "roughnessFromValue");
		// specular
		addFragSubroutine("getSpecular", "specularFromValue");

		// normal
		if (m_normalMap) addFragSubroutine("getNormal", "normalFromTexture");
		else addFragSubroutine("getNormal", "normalFromValue");

		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, fragSubroutines.size(), &fragSubroutines[0]);
	}
}


// Diffuse
vec3d Material::getDiffuseValue() { return m_diffuse; }
texture_ptr Material::getDiffuseMap() { return m_diffuseMap; }
void Material::setDiffuseValue(vec3d d) { m_diffuse = d; m_useDiffuseMap = false; }
void Material::setDiffuseMap(texture_ptr d) { m_diffuseMap = d; m_useDiffuseMap = true; }
void Material::setUseDiffuseMap(bool b) { m_useDiffuseMap = b; }
bool Material::useDiffuseMap() { return m_useDiffuseMap;  }

// Attributes
float Material::getMetalicityValue() { return m_metalicity; }
void Material::setMetalicityValue(float m) { m_metalicity = m; }

float Material::getRoughnessValue() { return m_roughness;}
void Material::setRoughnessValue(float r) { m_roughness = r; }

float Material::getSpecularValue() { return m_specular; }
void Material::setSpecularValue(float s) { m_specular = s; }

// Normals
void Material::setNormalMap(texture_ptr n) { m_normalMap = n; }
texture_ptr Material::getNormalMap() { return m_normalMap; }



shader_ptr Shader::fromFile(const std::string &filename) {
	ifstream fileStream(filename);

	if (fileStream) {
		stringstream buffer;
		buffer << fileStream.rdbuf();
		return make_shared<Shader>(buffer.str());
	}
	throw std::runtime_error("fuck you");
}


Shader::Shader(const string &text) {
	m_prog = gecom::makeShaderProgram("410 core", { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, text);
}


Shader::~Shader() { }


void Shader::bind() {
	glUseProgram(m_prog);
}


GLuint Shader::uniformLocation(const string &name) {
	try {
		return m_uniformLocationCache.at(name);
	} catch (const std::out_of_range& oor) {
		GLuint location = glGetUniformLocation(m_prog, name.c_str());
		m_uniformLocationCache[name] = location;
		return location;
	}
}


GLint Shader::activeSubroutines(GLenum shadertype) {
	GLint activeCount = 0;
	glGetProgramStageiv(m_prog, shadertype, GL_ACTIVE_SUBROUTINE_UNIFORMS, &activeCount);
	return activeCount;
}


GLuint Shader::subroutineUniformIndex(GLenum shadertype, const string &name) {
	try {
		return m_subroutineUniformIndexCache.at(shadertype).at(name);
	}
	catch (const std::out_of_range& oor) {
		GLuint index = glGetSubroutineUniformLocation(m_prog, shadertype, name.c_str());
		m_subroutineUniformIndexCache[shadertype][name] = index;
		return index;
	}
}


GLuint Shader::subroutineIndex(GLenum shadertype, const string &name) {
	try {
		return m_subroutineIndexCache.at(shadertype).at(name);
	}
	catch (const std::out_of_range& oor) {
		GLuint index = glGetSubroutineIndex(m_prog, shadertype, name.c_str());
		m_subroutineIndexCache[shadertype][name] = index;
		return index;
	}
}


GLuint Shader::program() const {
	return m_prog;
}