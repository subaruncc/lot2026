#include <M5Atom.h>

// ニコちゃんマーク（緑色）の配置データ
// 0: 消灯, 1: 点灯
const int smileMap[25] = {
  0, 0, 0, 0, 0,
  0, 1, 0, 1, 0,
  0, 0, 0, 0, 0,
  1, 0, 0, 0, 1,
  0, 1, 1, 1, 0
};

// 現在LEDが点灯しているかどうかの状態を管理する変数（false = 消灯, true = 点灯）
bool isLit = false;

void setup() {
    // M5Atomの初期化（Serial有効、I2C無効、Display有効）
    M5.begin(true, false, true);
    
    // 起動時は画面を真っ暗（クリア）にする
    M5.dis.clear();
}

void loop() {
    // ボタンの状態を更新する
    M5.update();
    
    // M5Atomの本体ボタンが「押された瞬間」を検知
    if (M5.Btn.wasPressed()) {
        
        if (isLit == false) {
            // 現在消灯しているなら、ニコちゃんマークを点灯
            displaySmile();
            isLit = true; // 状態を「点灯中」に変更
            Serial.println("ニコちゃんマークを表示しました！");
        } else {
            // 現在点灯しているなら、画面を消灯
            M5.dis.clear();
            isLit = false; // 状態を「消灯中」に変更
            Serial.println("画面をクリアしました。");
        }
    }
    
    // 次の判定までわずかに待つ（ボタンのチャタリング防止）
    delay(50);
}

// 5x5のLEDマトリックスにニコちゃんマークを描画する関数
void displaySmile() {
    for (int i = 0; i < 25; i++) {
        if (smileMap[i] == 1) {
            // 【修正】Pを小文字の p に変更（drawpix）
            M5.dis.drawpix(i, 0x00FF00); // 緑色
        } else {
            // 【修正】Pを小文字の p に変更（drawpix）
            M5.dis.drawpix(i, 0x000000); // 消灯
        }
    }
}