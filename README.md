Bounce Quest Game
Introduction
Bounce Quest is an engaging and dynamic game designed for Arduino with a TFT display. The game involves controlling a paddle to keep a ball in play, breaking blocks, and navigating through various challenges.

Features
Dynamic Paddle Control: Move the paddle left and right to keep the ball in play.
Block Breaking Gameplay: Break rows of blocks by hitting them with the ball.
Scoring System: Score points by breaking blocks, with a display of the current score and high score.
Multiple Balls and Special Blocks: Includes functionality for ball splitting and special block effects, like paddle growth.
Menu System: Navigate through the main menu, start a new game, or view the high score.
Hardware Requirements
Arduino compatible board
TFT display module (with rm67162 driver)
Two buttons for control (select and confirm)
Setup
Hardware Setup: Connect the TFT display to your Arduino board and set up the select and confirm buttons.
Library Installation: Include the TFT_eSPI and rm67162 libraries in your Arduino IDE.
Upload the Code: Compile and upload the provided sketch to your Arduino board.
Controls
Select Button: Move the paddle left or navigate through the menu.
Confirm Button: Move the paddle right or make selections in the menu.
Code Structure
Initialization: Set up the display, buttons, and game variables in setup().
Main Loop: Handle the game state, menu navigation, and game updates in loop().
Game Mechanics: Functions for handling ball movement, block collisions, scoring, and game over scenarios.
Customization
You can customize various aspects of the game, such as paddle size, ball speed, block arrangement, and more by tweaking the defined variables and logic in the code.
