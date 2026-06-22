#include <M5Atom.h>
#include <WiFi.h>

const char* ssid = "001NCC-WiFi";
const char* password = "001NCC0707"; // 👈 末尾に「;」を追加しました

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

void connectToWiFi() {
  Serial.println("WiFI接続を開始します...");
  WiFi.begin(ssid, password);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter < 40) {
    M5.dis.drawpix(12, 0xFFFF00); // 真ん中を黄色に
        delay(250);
        M5.dis.clear();
        delay(250);
        Serial.print(".");
        counter++;
  }

  if (WiFi.status() == WL_CONNECTED) {
        // 接続成功：全体を一度「青色」に光らせる
        Serial.println("\nWi-Fi接続成功！");
        Serial.print("IPアドレス: ");
        Serial.println(WiFi.localIP());
        
        fillScreen(0x0000FF); // 青
        delay(1000);
        M5.dis.clear();
    } else {
        // 接続失敗：全体を一度「赤色」に光らせる
        Serial.println("\nWi-Fi接続に失敗しました。SSIDやパスワードを確認してください。");
        fillScreen(0xFF0000); // 赤
        delay(2000);
        M5.dis.clear();
    }
}

void setup() {
    // M5Atomの初期化（Serial有効、I2C無効、Display有効）
    M5.begin(true, false, true);
    
    // 起動時は画面を真っ暗（クリア）にする
    M5.dis.clear();

    connectToWiFi();
}

void loop() {
    M5.update();
    
    if (M5.Btn.wasPressed()) {
        // Wi-Fiが繋がっているか確認
        if (WiFi.status() == WL_CONNECTED) {
            if (isLit == false) {
                displaySmile();
                isLit = true;
                Serial.println("ボタンが押されました：ニコちゃんマーク表示中");
            } else {
                M5.dis.clear();
                isLit = false;
                Serial.println("ボタンが押されました：消灯");
            }
        } else {
            // もしボタンを押した時にWi-Fiが切れていたら赤く光って再接続を試みる
            Serial.println("Wi-Fiが切断されています。再接続します。");
            fillScreen(0xFF0000);
            delay(500);
            connectToWiFi();
        }
    }
    
    delay(50);
}

// 画面全体を指定した色で塗りつぶす関数
void fillScreen(uint32_t color) {
    for (int i = 0; i < 25; i++) {
        M5.dis.drawpix(i, color);
    }
}

// ニコちゃんマークを表示する関数
void displaySmile() {
    for (int i = 0; i < 25; i++) {
        if (smileMap[i] == 1) {
            M5.dis.drawpix(i, 0x00FF00); // 緑
        } else {
            M5.dis.drawpix(i, 0x000000);
        }
    }
}