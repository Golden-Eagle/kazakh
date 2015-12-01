#pragma once

#include <unordered_set>

#include "ComponentSystem.hpp"
#include "Render.hpp"
#include "SceneRender.hpp"

#include <gecom/Window.hpp>


namespace pxljm {

	// class Projection {
	// public:
	// 	Projection() : m_projectionTransform(initial3d::mat4d(1)) {}

	// 	void setPerspectiveProjection(double fovy, double aspect, double zNear, double zFar) {
	// 		double f = initial3d::math::cot(fovy / 2);

	// 		m_projectionTransform = initial3d::mat4d(0);
	// 		m_projectionTransform(0, 0) = f / aspect;
	// 		m_projectionTransform(1, 1) = f;
	// 		m_projectionTransform(2, 2) = (zFar + zNear) / (zNear - zFar);
	// 		m_projectionTransform(2, 3) = (2 * zFar * zNear) / (zNear - zFar);
	// 		m_projectionTransform(3, 2) = -1;

	// 		m_zfar = zFar;
	// 	}

	// 	void setOrthographicProjection(double left, double right, double bottom, double top, double nearVal, double farVal) {
	// 		m_projectionTransform = initial3d::mat4d(0);
	// 		m_projectionTransform(0, 0) = 2 / (right - left);
	// 		m_projectionTransform(0, 3) = (right + left) / (right - left);
	// 		m_projectionTransform(1, 1) = 2 / (top - bottom);
	// 		m_projectionTransform(1, 3) = (top + bottom) / (top - bottom);
	// 		m_projectionTransform(2, 2) = -2 / (farVal - nearVal);
	// 		m_projectionTransform(2, 3) = (farVal + nearVal) / (farVal - nearVal);
	// 		m_projectionTransform(3, 3) = 1;

	// 		m_zfar = farVal;
	// 	}

	// 	initial3d::mat4d getProjectionTransform() {
	// 		return m_projectionTransform;
	// 	}

	// 	double getFarPlane() {
	// 		return m_zfar;
	// 	}

	// private:
	// 	double m_zfar;
	// 	initial3d::mat4d m_projectionTransform;
	// };



	class Camera : public virtual EntityComponent {
	private:
		SceneRenderStrategy<DefaultSceneRenderer> m_renderStrategy;

	public:
		//TODO UNHACK
		gecom::size2i size;

		void registerWith(Scene &s);
		void deregisterWith(Scene &s);

		virtual void update(int, int) = 0; //update with width and height

		virtual RenderStrategy* getRenderStrategy();

		virtual i3d::mat4d getViewMatrix();
		virtual i3d::mat4d getProjectionMatrix() = 0;
		virtual double getZnear() = 0;
		virtual void setZnear(double) = 0;
		virtual double getZfar() = 0;
		virtual void setZfar(double) = 0;
	};


	class PerspectiveCamera : public Camera {
	private:
		float m_fov = 1.0; //radians
		float m_zNear = 0.1;
		float m_zFar = 10000;

		i3d::mat4d m_proj;

	public:
		PerspectiveCamera() { }

		virtual void update(int, int);

		virtual i3d::mat4d getProjectionMatrix();

		virtual double getZnear();
		virtual void setZnear(double);

		virtual double getZfar();
		virtual void setZfar(double);

		virtual double getFOV();
		virtual void setFOV(double);
	};


	class StaticDefaultCamera : public PerspectiveCamera {
	public:
		StaticDefaultCamera() { }

		virtual i3d::mat4d getViewMatrix(){
			return i3d::mat4d();
			//return i3d::mat4d::translate(0, 0, 5);

		}
	};


	class CameraSystem : public ComponentSystem	{
	private:
		std::unordered_set<Camera *> m_cameras;
		Camera * m_primary = nullptr;
		StaticDefaultCamera m_defaultCamera;

	public:
		CameraSystem() { }

		void update(int, int);

		void registerCamera(Camera *);
		void deregisterCamera(Camera *);

		Camera * getPrimaryCamera();
		void setCamera(Camera *);
	};
}
