#pragma once

#include <unordered_set>
#include <chrono>
#include <memory>
#include <queue>

#include <gecom/Window.hpp>

#include "ComponentSystem.hpp"

namespace pxljm {

	class Updatable;
	class InputUpdatable;

	// 
	// Component System for Update and InputUpdate Components
	// 
	class UpdateSystem : public ComponentSystem {
	public:
		using clock_t = std::chrono::steady_clock;

		UpdateSystem();

		void registerUpdatable(Updatable *);
		void deregisterUpdatable(Updatable *);
		void update(clock_t::time_point now, clock_t::duration maxlead, clock_t::duration timebudget);

		void registerInputUpdatable(InputUpdatable *);
		void deregisterInputUpdatable(InputUpdatable *);
		void inputUpdate();

		const std::shared_ptr<gecom::WindowEventProxy> & eventProxy() {
			return m_wep;
		}

	private:
		std::unordered_set<Updatable *> m_updatables;
		std::unordered_set<InputUpdatable *> m_inputUpdatables;
		std::shared_ptr<gecom::WindowEventProxy> m_wep = std::make_shared<gecom::WindowEventProxy>();

		struct update_data {
			Updatable *up = nullptr;
			clock_t::time_point prev, next;
		};

		struct update_data_compare {
			bool operator()(const update_data &a, const update_data &b) {
				return a.next > b.next;
			}
		};

		std::priority_queue<update_data, std::vector<update_data>, update_data_compare> m_update_queue;
	};


	//
	// Update component
	//
	class Updatable : public virtual EntityComponent {
	public:
		using clock_t = UpdateSystem::clock_t;

		virtual void registerWith(Scene &) override;
		virtual void deregisterWith(Scene &) override;

		virtual void update(clock_t::time_point now, clock_t::time_point prev) = 0;
		virtual std::chrono::duration<double> updateInterval();

		static double timeDelta(clock_t::time_point a, clock_t::time_point b) {
			using namespace std::chrono;
			return duration_cast<duration<double>>(a - b).count();
		}
	};


	//
	// Input Update component
	//
	class InputUpdatable : public virtual EntityComponent {
	public:
		virtual void registerWith(Scene &) override;
		virtual void deregisterWith(Scene &) override;

		virtual void inputUpdate(gecom::WindowEventProxy &) = 0;
	};
}
