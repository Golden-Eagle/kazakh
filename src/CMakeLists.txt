

# list headers here
set(headers

	"Pxljm.hpp"

	"json.hpp"

	"Game.hpp"
	"Renderer.hpp"
	"ComponentTest.hpp"
	"Collider.hpp"
	"SimpleShader.hpp"
	
	# Assets
	"Assets.hpp"
	"Material.hpp"
	"Mesh.hpp"
	"Texture.hpp"

	# Entity
	"ComponentSystem.hpp"
	"CameraSystem.hpp"
	"Entity.hpp"
	"DrawSystem.hpp"
	"LightSystem.hpp"
	"PhysicsSystem.hpp"
	"UpdateSystem.hpp"
	# "SoundSystem.hpp"

	"Scene.hpp"

	"UIRenderSystem.hpp"

	"State.hpp"
	"PlayState.hpp"
	# "Joystick.hpp"
)


# list sources here
set(sources
	"main.cpp"
	"Renderer.cpp"
	
	"Assets.cpp"
	"Material.cpp"
	"Mesh.cpp"
	"Texture.cpp"

	"ComponentSystem.cpp"
	"Entity.cpp"
	"CameraSystem.cpp"
	"DrawSystem.cpp"
	"LightSystem.cpp"
	"PhysicsSystem.cpp"
	"UpdateSystem.cpp"
	# "SoundSystem.cpp"
	
	"Scene.cpp"

	"UIRenderSystem.cpp"
)


add_executable(pxljm ${headers} ${sources})

target_link_libraries(pxljm PRIVATE gecom)

target_link_libraries(pxljm PRIVATE BulletCollision BulletDynamics BulletSoftBody)

target_link_libraries(pxljm PRIVATE stb imgui)
# target_link_libraries(pxljm PRIVATE fmod_lowlevel fmod_studio)