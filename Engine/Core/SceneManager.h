#pragma once

#include "Headers\Essential.h"

namespace D2DEngine
{
	struct SceneTable
	{
		static bool Compare(const SceneTable& p1, const SceneTable& p2)
		{
			if (p1.id < p2.id)
				return true;
			else
				return false;
		}
		int id;
		class Scene* (*SceneFactoryFucntion)();
	};

	class SceneManager
	{
		friend class Engine;
	public:
		static inline SceneManager& GetInstance() { return SceneManager::s_instance; }
		inline int GetCurrentSceneId() const { return m_currentSceneId; }
		inline void SetFirstScene(int sceneId) { m_nextSceneId = sceneId; m_sceneChangeFlag = true; }
		inline void LoadScene(int sceneId) { m_nextSceneId = sceneId; m_sceneChangeFlag = true; }
		void RegisterSceneTable(SceneTable table[], unsigned int count);
		inline class Scene& GetCurrentScene() const { return *m_pCurrentScene; }
	private:
		SceneManager();
		~SceneManager();
		bool CreateNextScene();
	private:
		static SceneManager s_instance;
		std::vector<SceneTable> m_sceneTable;
		class Scene* m_pCurrentScene;
		int m_currentSceneId;
		int m_nextSceneId;
		bool m_sceneChangeFlag;
	};
}
