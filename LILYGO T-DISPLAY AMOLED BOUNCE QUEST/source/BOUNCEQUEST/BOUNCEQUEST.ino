#include "rm67162.h"
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();  
TFT_eSprite sprite = TFT_eSprite(&tft);  

#define selectButton 0  // 定义选择按钮
#define confirmButton 21  // 定义确认按钮

const int initialPaddleWidth = 180;
const int paddleHeight = 5; 
int paddleX, paddleWidth; 
const int paddleY = 530; 
const int ballRadius = 3; 小球半径
int score = 0; //游戏内得分
int highScore = 0; //最高得分
bool gameActive = false; //标志
bool inMainMenu = true; //标志
bool inHighScoreMenu = false;
int mainMenuSelection = 0;  // 用于主菜单选项的高亮
bool gameJustEnded = false;  // 新增的标志

const int blockRows = 70;  // 方块行数
const int blockCols = 48; //方块列数
const int blockWidth = 5; //每一个方块的宽度
const int blockHeight = 5; //每一个方块的列数
bool blocks[blockRows][blockCols];
int blockStartY = 30;  // 设置方块的起始Y坐标，用于确保在得分下方

struct Ball {
  int x, y;
  int speedX, speedY;
  int splitCounter;  // 分裂计数器
};


const int maxBalls = 150;  // 允许的小球最大数量
Ball balls[maxBalls];     // 小球数组
int ballCount = 1;        // 初始化小球数量

bool isSplitBlock;  // 裂变方块的存在标志
int splitBlockRow, splitBlockCol;  // 裂变方块的位置

bool isPaddleGrowBlock;  // 球拍增长方块的存在标志
int paddleGrowBlockRow, paddleGrowBlockCol;  // 球拍增长方块的位置
unsigned long paddleGrowStartTime;  // 球拍增长开始的时间
const int paddleGrowDuration = 10000;  // 球拍增长持续时间，单位毫秒
const int extraPaddleLength = 10;  // 球拍增加的长度

void setup() {
  pinMode(selectButton, INPUT_PULLUP);
  pinMode(confirmButton, INPUT_PULLUP);

  tft.init();
  tft.setRotation(0);
  sprite.createSprite(240, 536);
  sprite.setSwapBytes(true);

  rm67162_init();
  lcd_brightness(255);

  // 初始化状态并显示主菜单
  inMainMenu = true;
  gameActive = false;
  inHighScoreMenu = false;
  showMainMenu();
}

void loop() {
  if (gameJustEnded) {
    showMainMenu();
    return;  // 确保不再进入其他游戏逻辑
  }

  if (inMainMenu) {
    handleMainMenu();
    if (inMainMenu) {
      showMainMenu();
    }
  } else if (gameActive) {
    updateGame();
    drawGame();
  } else if (inHighScoreMenu) {
    showHighScore();
  }
}



void handleMainMenu() {
  if (digitalRead(selectButton) == LOW) {
    mainMenuSelection = (mainMenuSelection + 1) % 2;  // 在两个选项间切换
    showMainMenu();
    delay(200);  // 防抖
  }

  if (digitalRead(confirmButton) == LOW) {
    delay(200); // 防抖
    if (mainMenuSelection == 1) {
      inMainMenu = false;
      inHighScoreMenu = true;
      showHighScore();
    } else {
      inMainMenu = false;
      gameActive = true;
      startGame();  // 开始新游戏
    }
  }
}

void showMainMenu() {
  gameJustEnded = false;  // 重置游戏刚结束的标志
  sprite.fillSprite(TFT_DARKGREEN);  // 将背景设置为墨绿色

  // 绘制主菜单文本
  sprite.setTextColor(TFT_WHITE, TFT_TRANSPARENT); // 设置文字颜色和透明背景
  

  sprite.fillSprite(TFT_TRANSPARENT);

  // 添加 "tiltle" 标签
  sprite.setTextColor(TFT_WHITE, TFT_TRANSPARENT);  // 白色文字，透明背景
  sprite.setCursor(15, 100);  // 调整位置以居中
  sprite.setTextSize(3);
  sprite.print("Bounce Quest");
  
  // 设置文字颜色和背景色
  sprite.setTextColor(TFT_WHITE, TFT_TRANSPARENT);  // 白色文字，透明背景

  // "Start Game"选项
  sprite.setTextSize(2);
  sprite.setCursor(62, 220);  // 调整位置以居中
  if (mainMenuSelection == 0) {
    sprite.setTextColor(TFT_YELLOW, TFT_TRANSPARENT);  // 高亮显示，透明背景
  }
  sprite.print("Start Game");

  sprite.setTextColor(TFT_WHITE, TFT_TRANSPARENT);  // 重置为白色文字，透明背景

  // "High Score"选项
  sprite.setTextSize(2);
  sprite.setCursor(62, 250);  // 调整位置以居中
  if (mainMenuSelection == 1) {
    sprite.setTextColor(TFT_YELLOW, TFT_TRANSPARENT);  // 高亮显示，透明背景
  }
  sprite.print("High Score");

  // 添加 "By Peter" 标签
  sprite.setTextColor(TFT_WHITE, TFT_TRANSPARENT);  // 白色文字，透明背景
  sprite.setCursor(95, 320);  // 调整位置以居中
  sprite.setTextSize(1);
  sprite.print("By Peter");

  // 添加 "select" 标签
  sprite.setTextColor(TFT_WHITE, TFT_TRANSPARENT);  // 白色文字，透明背景
  sprite.setCursor(10, 520);  // 调整位置以居中
  sprite.setTextSize(1.5);
  sprite.print("SELECT");

  // 添加 "confirm" 标签
  sprite.setTextColor(TFT_WHITE, TFT_TRANSPARENT);  // 白色文字，透明背景
  sprite.setCursor(192, 520);  // 调整位置以居中
  sprite.setTextSize(1.5);
  sprite.print("CONFIRM");

  lcd_PushColors(0, 0, 240, 536, (uint16_t*)sprite.getPointer());
}


void showHighScore() {
  sprite.fillSprite(TFT_BLACK);
  
  // 设置更大的字体大小显示 "High Score"
  sprite.setCursor(50, 268);  // 显示坐标
  sprite.setTextColor(TFT_WHITE);
  sprite.setTextSize(2);  // 字体大小
  sprite.print("High Score: ");
  sprite.println(highScore);

  lcd_PushColors(0, 0, 240, 536, (uint16_t*)sprite.getPointer());

  if (digitalRead(selectButton) == LOW) {
    delay(200);  // 防抖延时
    inHighScoreMenu = false;
    inMainMenu = true;
    showMainMenu();
  }
}

void startGame() {
  gameActive = true;
  resetGame();
}

void resetGame() {
  paddleX = (240 - initialPaddleWidth) / 2;
  paddleWidth = initialPaddleWidth;
  score = 0;

  ballCount = 1;  // 重置小球数量为1
  for (int i = 0; i < maxBalls; i++) {
    balls[i].x = random(ballRadius, 240 - ballRadius);
    balls[i].y = paddleY - ballRadius - 10;
    balls[i].speedX = 3;
    balls[i].speedY = -3;
    balls[i].splitCounter = 0;  // 初始化分裂计数器
  }

  // 重新生成方块
  for (int i = 0; i < blockRows; i++) {
    for (int j = 0; j < blockCols; j++) {
      blocks[i][j] = random(0, 2) == 0;
    }
  }

  // 重新设置特殊方块
  splitBlockRow = random(0, blockRows);
  splitBlockCol = random(0, blockCols);
  isSplitBlock = true;

  paddleGrowBlockRow = random(0, blockRows);
  paddleGrowBlockCol = random(0, blockCols);
  isPaddleGrowBlock = true;
}




void updateGame() {
  // 球拍移动逻辑
  bool paddleMovedRight = false;
  bool paddleMovedLeft = false;

  if (digitalRead(confirmButton) == LOW) {
    paddleX += 3;
    paddleMovedRight = true;
    if (paddleX > 240 - paddleWidth) paddleX = 240 - paddleWidth;
  }
  if (digitalRead(selectButton) == LOW) {
    paddleX -= 3;
    paddleMovedLeft = true;
    if (paddleX < 0) paddleX = 0;
  }

  // 小球移动和碰撞逻辑
  bool anyBallLeft = false;
  for (int i = 0; i < ballCount; i++) {
    balls[i].x += balls[i].speedX;
    balls[i].y += balls[i].speedY;

    // 小球与屏幕边界的碰撞
    if (balls[i].x <= ballRadius || balls[i].x >= 240 - ballRadius) balls[i].speedX = -balls[i].speedX;
    if (balls[i].y <= ballRadius) balls[i].speedY = -balls[i].speedY;

    // 小球与球拍的碰撞
    if (balls[i].y + ballRadius >= paddleY && balls[i].y - ballRadius <= paddleY + paddleHeight) {
      if (balls[i].x + ballRadius >= paddleX && balls[i].x - ballRadius <= paddleX + paddleWidth) {
        balls[i].speedY = -abs(balls[i].speedY);  // Y方向速度反向

        // 根据球拍移动调整小球水平方向速度
        if (paddleMovedRight) {
          balls[i].speedX += (balls[i].speedX < 0) ? -1 : 1;
        } else if (paddleMovedLeft) {
          balls[i].speedX -= (balls[i].speedX > 0) ? -1 : 1;
        }

        // 移除原本的得分逻辑
      }
    }

    // 限制小球的最大水平速度
    balls[i].speedX = max(min(balls[i].speedX, 5), -5);

    // 检查小球是否仍在屏幕上
    if (balls[i].y < 536 - ballRadius) {
      anyBallLeft = true;
    } else {
      // 小球触及屏幕底部时移除
      removeBall(i);
      i--; // 索引调整
    }
  }

  // 检查方块碰撞
  checkBlockCollision();

  // 所有小球均未在屏幕上
  if (!anyBallLeft) {
    gameActive = false;
    showGameOver();
  }

  // 检查球拍增长是否结束
  if (millis() - paddleGrowStartTime > paddleGrowDuration && paddleWidth > initialPaddleWidth) {
    paddleX += extraPaddleLength / 2; // 调整球拍中心
    paddleWidth -= extraPaddleLength;
  }

  // 检查并重新生成方块
  checkAndRegenerateBlocks();
}





void splitBall(int index) {
  if (ballCount < maxBalls) {
    balls[ballCount] = balls[index];  // 复制小球的位置和速度
    balls[ballCount].speedX = -balls[index].speedX;  // 改变新小球的水平方向
    balls[ballCount].splitCounter = 0;  // 初始化新小球的分裂计数器
    ballCount++;
  }
}




void checkBlockCollision() {
  for (int i = 0; i < blockRows; i++) {
    for (int j = 0; j < blockCols; j++) {
      if (blocks[i][j]) {
        int blockX = j * blockWidth;
        int blockY = i * blockHeight + blockStartY;
        for (int k = 0; k < ballCount; k++) {
          if (balls[k].x + ballRadius > blockX && balls[k].x - ballRadius < blockX + blockWidth &&
              balls[k].y + ballRadius > blockY && balls[k].y - ballRadius < blockY + blockHeight) {
            blocks[i][j] = false;
            // 根据小球接触方块的位置反转速度
            if (balls[k].x < blockX || balls[k].x > blockX + blockWidth) {
              balls[k].speedX = -balls[k].speedX;
            }
            if (balls[k].y < blockY || balls[k].y > blockY + blockHeight) {
              balls[k].speedY = -balls[k].speedY;
            }
            score++;

            if (balls[k].y < paddleY - ballRadius) {  // 小球未触碰球拍
              balls[k].splitCounter++;
              for (int k = 0; k < ballCount; k++) {
                if (balls[k].splitCounter >= 12) { // 检查分裂条件
                  splitBall(k);  // 分裂小球
                  balls[k].splitCounter = 0;  // 重置分裂计数器
                }
              }
            }

            // 检查是否击中了特殊方块
            if (isSplitBlock && i == splitBlockRow && j == splitBlockCol) {
              splitBalls();  // 裂变小球
              isSplitBlock = false;
            }

            if (isPaddleGrowBlock && i == paddleGrowBlockRow && j == paddleGrowBlockCol) {
              paddleX -= extraPaddleLength / 2; // 将球拍中心保持不变
              paddleWidth += extraPaddleLength;
              paddleGrowStartTime = millis();
              isPaddleGrowBlock = false;
            }
          }
        }
      }
    }
  }
}

void splitBalls() {
  if (ballCount < maxBalls - 1) {
    int newCount = ballCount * 2;  // 小球数量加倍
    for (int i = ballCount; i < newCount && i < maxBalls; i++) {
      balls[i] = balls[i - ballCount];  // 复制小球的位置和速度
      balls[i].speedX = -balls[i].speedX;  // 改变新小球的水平方向
      ballCount++;
    }
  }
}

void drawGame() {
  if (gameJustEnded) return;  // 如果游戏刚结束，不再绘制游戏画面
  sprite.fillSprite(TFT_BLACK);

  // 在屏幕顶部以大号字体显示得分
  sprite.setCursor(10, 10);
  sprite.setTextColor(TFT_YELLOW);
  sprite.setTextSize(2);
  sprite.print("Score: ");
  sprite.println(score);

  // 视觉样式绘制球拍
  sprite.fillRect(paddleX, paddleY, paddleWidth, paddleHeight, TFT_BLUE);

  // 绘制所有小球
  for (int i = 0; i < ballCount; i++) {
    sprite.fillCircle(balls[i].x, balls[i].y, ballRadius, TFT_YELLOW);
  }

  // 使用不同颜色绘制所有可见的方块，包括特殊方块
  for (int i = 0; i < blockRows; i++) {
    for (int j = 0; j < blockCols; j++) {
      if (blocks[i][j]) {
        int x = j * blockWidth;
        int y = i * blockHeight + blockStartY;
        uint16_t color = TFT_WHITE; // 默认方块颜色
        if (isSplitBlock && i == splitBlockRow && j == splitBlockCol) {
          color = TFT_RED;  // 裂变方块用红色
        } else if (isPaddleGrowBlock && i == paddleGrowBlockRow && j == paddleGrowBlockCol) {
          color = TFT_GREEN;  // 球拍增长方块用绿色
        }
        sprite.fillRect(x, y, blockWidth, blockHeight, color);
      }
    }
  }

  // 将画好的内容推送到屏幕上
  lcd_PushColors(0, 0, 240, 536, (uint16_t*)sprite.getPointer());
}


void showGameOver() {
  gameJustEnded = true;  // 设置游戏刚刚结束的标志
  sprite.fillSprite(TFT_BLACK);
  sprite.setCursor(10, 268);
  sprite.setTextColor(TFT_WHITE);
  sprite.setTextSize(1);
  sprite.setTextSize(2);
  sprite.print("Game Over! Score: ");
  sprite.setCursor(10, 288);
  sprite.setTextSize(2);
  sprite.println(score);
  lcd_PushColors(0, 0, 240, 536, (uint16_t*)sprite.getPointer());

  if (score > highScore) {
    highScore = score;
  }

  delay(2000);  // 添加一个延时

  // 重置游戏状态和得分
  score = 0;
  gameActive = false;
  inMainMenu = true; // 设置为返回主菜单
}



void checkAndRegenerateBlocks() {
  bool allCleared = true;
  for (int i = 0; i < blockRows; i++) {
    for (int j = 0; j < blockCols; j++) {
      if (blocks[i][j]) {
        allCleared = false;
        break;
      }
    }
    if (!allCleared) break;
  }

  if (allCleared) {
    // 重新生成方块
    for (int i = 0; i < blockRows; i++) {
      for (int j = 0; j < blockCols; j++) {
        blocks[i][j] = random(0, 2) == 0;
      }
    }

    // 重置小球数量和分裂计数器
    ballCount = 1;
    balls[0].x = random(ballRadius, 240 - ballRadius);
    balls[0].y = paddleY - ballRadius - 10;
    balls[0].speedX = 3;
    balls[0].speedY = -3;
    balls[0].splitCounter = 0;

    // 缩短球拍长度，最小值20
    paddleWidth = max(paddleWidth - 10, 20);
  }
}




void removeBall(int index) {
  for (int i = index; i < ballCount - 1; i++) {
    balls[i] = balls[i + 1];
  }
  ballCount--;
}