#include <M5Atom.h>
#include <FastLED.h>

const int smileMap[25] = {
  0,0,0,0,0,
  0,1,0,1,0,
  0,0,0,0,0,
  1,0,0,0,1,
  0,1,1,1,0,
};
// 現在LEDが点灯しているかどうかの状態を管理
bool isLit = false

void setup() {
  // M5Atomの初期化（Serial有効、I2C無効、Display有効）
  M5.begin(true, false, true)

  // 起動時は画面を真っ暗にする
  M5.disBuff.clear();
}

void loop() {
  M5.update();

  if (M5.Btn.wasPressed()) {
    
    if (isLit == false) {
      displaySmile();
      isLit = true;
      Serial.println("ニコちゃんマークを表示しました！");
    } else {
      M5.disBuff.clear();
      isLit = false;
      Serial.println("画面をクリアしました。");
    }
  }

  delay(50);

}

void displaySmile() {
  for (int i = 0; i < 25; i++) {
    if (smileMap[i] == 1) {
      m5.disBuff.setpix(i, 0x00FF00);
    } else {
      M5.desBuff.setpix(i, 0x00000);
    }
  }
}
