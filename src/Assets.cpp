#include "Assets.hpp"

using namespace std;
using json = nlohmann::json;

namespace gecom {

	namespace {
		map<string, material_ptr> materialMap;
		map<string, shader_ptr> shaderMap;
		map<string, mesh_ptr> meshMap;
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
					shaderMap[tag] = make_shared<Shader>(filePath);
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
					}

					materialMap[tag] = m;
				}


				cout << " >> meshes" << endl;
				for (auto it = config["meshes"].begin(); it != config["meshes"].end(); ++it) {
					cout << " " << it.key() << ":" << it.value() << endl;
					string tag = it.key(), filePath = it.value();
					meshMap[tag] = make_shared<Mesh>(filePath);
				}

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
			return meshMap.at(tag);
		}
	}
}