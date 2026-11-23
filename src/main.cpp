#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace std;

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include <cstdlib>

bool shinyLevel = false;
float hueShift = 50;

ccColor3B shiftHue(ccColor3B color){
	float r = color.r / 255.f;
	float g = color.g / 255.f;
	float b = color.b / 255.f; 
	
	float cmax = max(r, max(g, b));
	float cmin = min(r, min(g, b));
	float diff = cmax - cmin;
	float h = 0.f; 
	float s = 0.f;
	float v = cmax;

	//hue calculation
	if (cmax == cmin){
		h = 0.f;
	}else if (cmax == r){
		h = fmod(60.f * ((g - b) / diff) + 360.f, 360.f);
	}else if (cmax == g){
		h = fmod(60.f * ((b - r) / diff) + 120.f, 360.f);
	}else if (cmax == b){
		h = fmod(60.f * ((r - g) / diff) + 240.f, 360.f);
	}

	if (cmax != 0.f){
		s = diff / cmax;
	}

	//changing the hue
	h = fmod(h + hueShift + 360.f, 360.f);
	
	//converting back to rgb
	float c = s * v;
	float x = c * (1.f - abs(fmod(h / 60.f, 2.f) - 1.f));
	float m = v - c;

	if (h < 60.f){
		r = c, g = x, b = 0.f;
	}else if (h < 120.f){
		r = x, g = c, b = 0.f;
	}else if (h < 180.f){
		r = 0.f, g = c, b = x;
	}else if (h < 240.f){
		r = 0.f, g = x, b = c;
	}else if (h < 300.f){
		r = x, g = 0.f, b = c;
	}else {
		r = c, g = 0.f, b = x;
	}

	return{
		static_cast<GLubyte>((r + m) * 255.f),
		static_cast<GLubyte>((g + m) * 255.f),
		static_cast<GLubyte>((b + m) * 255.f)
	};
}

class $modify(MyGameObject, GameObject) {
    void setObjectColor(const ccColor3B& color) {
		if (shinyLevel == true){
			GameObject::setObjectColor(shiftHue(color));
		}else{
			GameObject::setObjectColor(color);
		}
    }

    void setGlowColor(const ccColor3B& color) {
		if (shinyLevel == true){
			GameObject::setGlowColor(shiftHue(color));
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
		PlayLayer::resetLevel();
		ShinyRandomizer();
	}
};