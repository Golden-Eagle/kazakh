#include "CameraSystem.hpp"
#include "Scene.hpp"


using namespace std;
using namespace pxljm;
using namespace i3d;


void Camera::registerWith(Scene &s) { s.cameraSystem().registerCamera(this); }


void Camera::deregisterWith(Scene &s) { s.cameraSystem().deregisterCamera(this); }


RenderStrategy* Camera::getRenderStrategy() { return &m_renderStrategy; }


i3d::mat4d Camera::getViewMatrix(){
	return !entity()->root()->matrix();
}


void PerspectiveCamera::update(int w, int h) {
	double aspect = double(w) / h;
	double f = initial3d::math::cot(m_fov / 2);

	m_proj = initial3d::mat4d(0);
	m_proj(0, 0) = f / aspect;
	m_proj(1, 1) = f;
	m_proj(2, 2) = (m_zFar + m_zNear) / (m_zNear - m_zFar);
	m_proj(2, 3) = (2 * m_zFar * m_zNear) / (m_zNear - m_zFar);
	m_proj(3, 2) = -1;

	//TODO UNHACK
	size = gecom::size2i(w, h);
}

i3d::mat4d PerspectiveCamera::getProjectionMatrix() { return m_proj; }

double PerspectiveCamera::getZnear() { return m_zNear; }
void PerspectiveCamera::setZnear(double f) { m_zNear = f; }

double PerspectiveCamera::getZfar() { return m_zFar; }
void PerspectiveCamera::setZfar(double f) { m_zFar = f; }

double PerspectiveCamera::getFOV() { return m_fov; }
void PerspectiveCamera::setFOV(double f) { m_fov = f; }



void CameraSystem::update(int w, int h) {
	getPrimaryCamera()->update(w, h);
}


void CameraSystem::registerCamera(Camera *c) {
	m_cameras.insert(c);
}


void CameraSystem::deregisterCamera(Camera *c) {
	if (m_primary == c) m_primary = nullptr;
	m_cameras.erase(c);
}


Camera * CameraSystem::getPrimaryCamera() {
	if (m_primary) return m_primary;
	if (!m_cameras.empty()) {
		auto it = m_cameras.begin();
		m_primary = *it;
		return m_primary;
	}
	return &m_defaultCamera;
}


void CameraSystem::setCamera(Camera *c) {
	m_primary = c;
}