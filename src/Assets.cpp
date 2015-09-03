#include "Assets.hpp"

using namespace std;
using json = nlohmann::json;

using namespace i3d;

namespace pxljm {

	namespace {
		map<string, material_ptr> materialMap;
		map<string, shader_ptr> shaderMap;
		map<string, mesh_ptr> meshMap;
		map<string, texture_ptr> textureMap;
	}

	namespace assets {
		void init(const std::string &configPath) {
			cout << "readfing file" << endl;
			ifstream configFile(configPath);
			if (configFile) {
				json config;
				config << configFile;
				//TODO fail case

				cout << " >> shaders" << endl;
				for (auto it = config["shaders"].begin(); it != config["shaders"].end(); ++it) {
					cout << " " << it.key() << ":" << it.value() << endl;
					string tag = it.key(), filePath = it.value();
					shaderMap[tag] = Shader::fromFile(filePath);
				}

				cout << " >> images" << endl;
				for (auto it = config["images"].begin(); it != config["images"].end(); ++it) {
					cout << " " << it.key() << ":" << it.value() << endl;
					string tag = it.key(), filePath = it.value();
					textureMap[tag] = Texture::fromFile(filePath);
				}


				cout << " >> materials" << endl;
				for (auto it = config["materials"].begin(); it != config["materials"].end(); ++it) {
					cout << " " << it.key() << ":" << it.value() << endl;
					string tag = it.key();
					json properties = it.value();
					material_ptr m = make_shared<Material>();

					for(auto p = properties.begin(); p != properties.end(); ++p) {
						string propTag = p.key();
						if (propTag == "shader") {
							string shaderTag = p.value();
							m->shader = getShader(shaderTag);
						}
						else if (propTag == "diffuse") {
							vector<double> color = p.value();
							m->setDiffuseValue(vec3d(color[0], color[1], color[2]));
						}
						else if (propTag == "diffuse-map") {
							string textureTag = p.value();
							m->setDiffuseMap(getTexture(textureTag));
						}
						else if (propTag == "rough") {
							m->setRoughnessValue(p.value());
						}
						else if (propTag == "metal") {
							m->setMetalicityValue(p.value());
						}
						else if (propTag == "spec") {
							m->setSpecularValue(p.value());
						}
						else if (propTag == "normal-map") {
							string textureTag = p.value();
							m->setNormalMap(getTexture(textureTag));
						}
					}

					materialMap[tag] = m;
				}


				cout << " >> meshes" << endl;
				for (auto it = config["meshes"].begin(); it != config["meshes"].end(); ++it) {
					cout << " " << it.key() << ":" << it.value() << endl;
					string tag = it.key(), filePath = it.value();
					meshMap[tag] = make_shared<Mesh>(filePath);
				}

			} else {
				cerr << "Could not find Asset Configuration file. Aborting..." << endl;
				abort();
			}
			cout << "finsihed readfing file" << endl;

		}


		material_ptr getMaterial(const std::string &tag) {
			return materialMap.at(tag);
		}


		shader_ptr getShader(const std::string &tag) {
			return shaderMap.at(tag);
		}


		mesh_ptr getMesh(const std::string &tag) {
			for(auto meshPair : meshMap) {
				gecom::Log::info() << "meshPair: " << meshPair.first;
			}
			return meshMap.at(tag);
		}

		texture_ptr getTexture(const std::string &tag) {
			return textureMap.at(tag);
		}
	}
}