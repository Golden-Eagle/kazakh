#pragma once

#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <map>

#include <gecom/Uncopyable.hpp>
#include <gecom/GL.hpp>
#include <gecom/Initial3D.hpp>

#include "SimpleShader.hpp"
#include "Texture.hpp"


namespace pxljm {

	class Material;
	class Shader;

	using material_ptr = std::shared_ptr<Material>;
	using shader_ptr = std::shared_ptr<Shader>;

	class Material : gecom::Uncopyable, public std::enable_shared_from_this<Material> {
	private:
		bool m_useDiffuseMap = false;
		i3d::vec3d m_diffuse = i3d::vec3d(0.5, 0.5, 0.5);
		texture_ptr m_diffuseMap;

		float m_metalicity = 0;
		float m_roughness = 0;
		float m_specular = 0;

		texture_ptr m_normalMap;


	public:
		Material();
		~Material();

		void bind(i3d::mat4d, float);

		// Diffuse
		i3d::vec3d getDiffuseValue();
		texture_ptr getDiffuseMap();
		void setDiffuseValue(i3d::vec3d);
		void setDiffuseMap(texture_ptr);
		void setUseDiffuseMap(bool);
		bool useDiffuseMap();

		// Attributes
		float getMetalicityValue();
		void setMetalicityValue(float);

		float getRoughnessValue();
		void setRoughnessValue(float);

		float getSpecularValue();
		void setSpecularValue(float);


		// Normals
		void setNormalMap(texture_ptr);
		texture_ptr getNormalMap();





		shader_ptr shader;
	};

	class Shader : gecom::Uncopyable, public std::enable_shared_from_this<Shader> {
	private:
		std::map<std::string, GLuint> m_uniformLocationCache;
		std::map<GLenum, std::map<std::string, GLuint>> m_subroutineUniformIndexCache;
		std::map<GLenum, std::map<std::string, GLuint>> m_subroutineIndexCache;
		GLuint m_prog = 0;

	public:
		static shader_ptr fromFile(const std::string &);
		Shader(const std::string &);
		~Shader();
		void bind();

		GLuint uniformLocation(const std::string &);
		GLint  activeSubroutines(GLenum shadertype);
		GLuint subroutineUniformIndex(GLenum, const std::string &);
		GLuint subroutineIndex(GLenum, const std::string &);
		GLuint program() const;
	};
}