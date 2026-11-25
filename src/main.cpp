#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace std;

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/GJGroundLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

bool shinyLevel = false;
float hueShift = 200;
CCTexture2D* shinyEndScreenTexture1 = nullptr;
CCTexture2D* shinyEndScreenTexture2 = nullptr;

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

class $modify(GameObject) {
	void setObjectColor(ccColor3B const& color) {
		if (shinyLevel == true){
			GameObject::setObjectColor(shiftHue(color));
			return;
		}
		GameObject::setObjectColor(color);
	}
    void setChildColor(const ccColor3B& color) {
		if (shinyLevel == true){
			GameObject::setChildColor(shiftHue(color));
			return;
		}
		GameObject::setChildColor(color);
    }
};

class $modify(CCSprite){

	bool changeGrounds(auto groundLayer, const ccColor3B& color){
		if (!groundLayer) return false;

        auto ground1 = groundLayer->m_ground1Sprite;
        auto ground2 = groundLayer->m_ground2Sprite;
		auto line = groundLayer->m_lineSprite;

		if (line == this){
			CCSprite::setColor(shiftHue(color));
			return true;
		}
        if (ground1 && ground1->getChildren()) {
            for (int i = 0; i < ground1->getChildren()->count(); i++) {
                if (ground1->getChildren()->objectAtIndex(i) == this) {
                    CCSprite::setColor(shiftHue(color));
                    return true;
                }
            }
        }
        if (ground2 && ground2->getChildren()) {
            for (int i = 0; i < ground2->getChildren()->count(); i++) {
                if (ground2->getChildren()->objectAtIndex(i) == this) {
                    CCSprite::setColor(shiftHue(color));
                    return true;
                }
            }
        }
		return false;
	}

	void setColor(const ccColor3B& color){
		auto baseLayer = GJBaseGameLayer::get();

		if (!shinyLevel || !baseLayer){
			CCSprite::setColor(color);
			return;
		}

		if (baseLayer->m_background == this){
			CCSprite::setColor(shiftHue(color));
			return;
		}

		if (changeGrounds(baseLayer->m_groundLayer, color)) return;
		if (changeGrounds(baseLayer->m_groundLayer2, color)) return;

		auto middleGround = baseLayer->m_middleground;
		
		if (middleGround){
			auto mg1 = middleGround->m_ground1Sprite;
			auto mg2 = middleGround->m_ground2Sprite;

			if (mg1 == this || mg2 == this){
				CCSprite::setColor(shiftHue(color));
				return;
			}
		}

		CCSprite::setColor(color);
	}
};

class $modify(PlayLayer){
	bool init(GJGameLevel* level, bool dontCreateObjects) {

		if (!PlayLayer::init(level, dontCreateObjects)) return false;

		ShinyRandomizer();

		return true;
	}

	void ShinyRandomizer(){
		int randomNum = rand() % 3;

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
	void onQuit() {
		PlayLayer::onQuit();
		shinyLevel = false;
	}
};

class $modify(EndLevelLayer) {

	void setSpriteTexture(const char* id){
		auto sprite = static_cast<CCSprite*>(this->getChildByIDRecursive(id));
		if (sprite) sprite->setTexture(shinyEndScreenTexture1);
	}

	void setButtonTexture(const char* id, CCTexture2D* texture){
		auto button = static_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive(id));
		if (button){
			auto sprite = static_cast<CCSprite*>(button->getNormalImage());
			if (sprite) sprite->setTexture(texture);
		}		
	}

    void customSetup() {
        EndLevelLayer::customSetup();

        if (!shinyLevel || !shinyEndScreenTexture1 || !shinyEndScreenTexture2) return;

		setSpriteTexture("left-border");
		setSpriteTexture("right-border");
		setSpriteTexture("top-border");
		setSpriteTexture("bottom-border");
		setSpriteTexture("level-complete-text");

		setButtonTexture("retry-button", shinyEndScreenTexture1);
		setButtonTexture("exit-button", shinyEndScreenTexture1);
		setButtonTexture("edit-button", shinyEndScreenTexture1);
		setButtonTexture("leaderboard-button", shinyEndScreenTexture1);
		setButtonTexture("death-tracker-button", shinyEndScreenTexture2);
    }
};

$on_mod(Loaded){
	shinyEndScreenTexture1 = CCTextureCache::sharedTextureCache()->addImage("shiny_GJ_GameSheet03.png"_spr, false);
	shinyEndScreenTexture2 = CCTextureCache::sharedTextureCache()->addImage("shiny_GJ_GameSheet04.png"_spr, false);
}