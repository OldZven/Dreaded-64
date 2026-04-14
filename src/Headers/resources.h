#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

class resources {
public:
	void loadAllResources();

	//---Player Animations---
	sf::Texture leftArmAnim[5];//All left arm animations
	/* Left Arm Animation
	* 0 - Loaded
	* 1 - Unloaded
	* 2 - Block
	* 3 - Idle Interact
	* 4 - Interact
	*/
	sf::Texture rightArmAnim[10];//All right arm animations
	/* Right Arm animations
	* 0 - Idle
	* 1 - Attack 1 start
	* 2 - Attack 1 Middle
	* 3 - Attack 1 End
	* 4 - Attack 2 start
	* 5 - Attack 2 Middle
	* 6 - Attack 2 End
	* 7 - Attack Power start
	* 8 - Attack Power end
	* 9 - Reload
	*/

	//---Level Textures---
	sf::Texture wallTexture[5];
	sf::Texture floorTexture[5];
	sf::Texture ceilingTexture[5];
	sf::Texture doorTexture[6];

	// Cached CPU images to avoid copyToImage() each frame
	std::vector<sf::Image> cachedFloorImages;
	std::vector<sf::Image> cachedCeilingImages;

	//---Extra Textures---
	sf::Texture arrowsTexture[2];

	//---Enemy Textures---
	sf::Texture enemyMeleeTexture[6];
	sf::Texture enemyRangedTexture[7];
	sf::Texture* enemyTempTexture[6];

	/* - Enemy Animations
	* 0 - Idle
	* 1 - Attack
	* 2 - Hit
	* 3 - Death
	* 4 - Walking 1
	* 5 - Walking 2
	* 6 - Reloading (Ranged)
	*/

	// ---HUD textures---
	sf::Texture playerTexture[7];
	/* Player animation
	* 0 - Idle
	* 1 - Looks Left
	* 2 - Looks Right
	* 3 - Eyes glow (Attack)
	* 4 - Red Blood covered (Hit Enemy)
	* 5 - Whole head flash red (Hit Player)
	* 6 - Skeleton head (Player Dead)
	*/
	sf::Texture hudTexture;
	sf::Texture menuTexture;
	sf::Texture deathScreenTexture;
	// ---HUD Font---
	sf::Font arialFont;

	//Sounds
	sf::SoundBuffer attackBuffer[5];
	sf::SoundBuffer playerBuffer[3];
	sf::SoundBuffer enemyBuffer[2];
	sf::SoundBuffer backgroundBuffer;

	//Attacking Sounds
	sf::Sound meleeStandardSound;
	sf::Sound meleeHeavySound;
	sf::Sound rangedAttackSound;
	sf::Sound reloadSound;

	//Player sounds
	sf::Sound hitPlayerSound;
	sf::Sound hitEnemySound;
	sf::Sound playerWalkSound;
	sf::Sound playerDeathSound;
	sf::Sound enemyDeathSound;
	sf::Sound interactSound;

	//Background Sounds
	sf::Sound backgroundSound;

private:

	void loadAllTextures(); //loads all textures + font
	void loadEnemyTexturesHelper(sf::Texture* texture[6], const std::string& type);
	void loadAllAudio();

	void setVolumes();
	void loopWalking();
	void loopBackground();
	bool loadTexture(sf::Texture& texture, const std::string& path);
	bool loadFont(sf::Font&, const std::string& path);
	bool loadAudio(sf::SoundBuffer& buffer, const std::string& path, sf::Sound& audio);
	std::string texturePath = "Data/Textures/";
	std::string audioPath = "Data/Audio/";

};
