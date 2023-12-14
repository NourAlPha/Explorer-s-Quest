# Explorer's Quest
## Description 
Explorer's Quest is a 3D running game that consists of 2 different levels developed using Opengl/glut C++.

## Main menu
Select one of the options using arrow keys.

## Game Levels 
### Level 1
The player is dropped into an island in which he is required to collect gemstones. Each gemstone color will grant you a key of the same color only when all the gemstones are collected of that type, which then is inserted in one of the pillars. When you insert all the keys into the correct pillars, the portal is opened that teleports that player into level 2 once he pass through it.
- **Collectables** : Gemstones and Keys
- **Obstacles** : Trees

![Screenshot (159)](https://github.com/NourAlPha/Explorer-s-Quest/assets/49641430/b65b12aa-7271-4436-a6bf-ba1ea79c486d)


### Level 2
The player is teleported into the underworld where he needs to avoid falling into the lava and he needs also to collect two diamonds that opens a portal to finally escape the game world and win the game. Collecting the first diamond opens a portal to cave with falling pillars, which has the final diamond which also opens a portal that takes the player back to the previous location.
- **Collectables** : Coins and Diamonds
- **Obstacles** : Falling pillars that the player should avoid

![Screenshot (161)](https://github.com/NourAlPha/Explorer-s-Quest/assets/49641430/cc76e6ea-9020-48a3-a249-eeb240dc6cf6)


## Video


https://github.com/NourAlPha/Explorer-s-Quest/assets/49641430/d2c91489-8074-4ba3-a91e-d002ae39738b




## Game Camera
The game camera is freely controlled using the mouse. There are 2 game modes for the camera: First Person Camera & Third Person Camera.
### First Person Camera 
Camera eye is the head of the player and center is a point in front of the player 
### Third Person Camera
Camera eye is behind the player while camera center is the player itself

## Game Controls
### Mouse 
- `Free movement` &rarr;Control the xz-plane camera 
- `Left Click` &rarr;Control the y-axis camera
### Key board
- `Press W` &rarr; Player move forward
- `Press A` &rarr; Player move left
- `Press S` &rarr; Player move backward
- `Press D` &rarr; Player move right
- `Press Space` &rarr; Player jumping
- `Press Tab` &rarr; View score
- `Press 1` &rarr; First person camera
- `Press 2` &rarr; Third person camera


## Win/Lose Actions
### Level 1
- **Win Action** : No win Action
- **Lose Action** : Fall off the island
### Level 2
- **Win Action** : Successfully pass through the final protal 
- **Lose Action** : Fall into the lava or collide with the falling pillars

## Challenges 
- Rendering difficult models and textures for the animation
- Free camera rotation
- Collision with falling pillars

## Authors 
-   [Nour Eldien Ayman](https://github.com/NourAlPha)
-   [Mohamed Ahmed](https://github.com/MohammmedAhmed8)
-   [Yosef Samy Koka](https://github.com/koka-afk)
-   [Omar Osama](https://github.com/omarosama2114)
