#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include <cstdlib>
#include <cmath>
#include <algorithm>

bool shinyLevel = false;

class $modify(MyGameObject, GameObject) {
    void setObjectColor(const ccColor3B& color) {
		if (shinyLevel == true){
			GameObject::setObjectColor(color);
		}else{
			GameObject::setObjectColor(color);
		}
    }

    void setGlowColor(const ccColor3B& color) {
		if (shinyLevel == true){
			GameObject::setGlowColor(color);
		}else{
			GameObject::setGlowColor(color);
		}
    }
};

class $modify(MyPlayLayer, PlayLayer){
	bool init(GJGameLevel* level, bool dontCreateObjects) {

		if (!PlayLayer::init(level, dontCreateObjects)) return false;

		ShinyRandomizer();

		return true;
	}

	void ShinyRandomizer(){
		int randomNum = rand() % 2;

		if (randomNum == 1){
			log::info("number:{}", randomNum);
			shinyLevel = true;
		}else{
			shinyLevel = false;
		}
	}
	
	void resetLevel() {

		ShinyRandomizer();
		PlayLayer::resetLevel();
	}
};