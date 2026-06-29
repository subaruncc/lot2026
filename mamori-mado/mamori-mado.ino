#include <M5Atom.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Wi-Fi接続情報
const char* ssid     = "DESKTOP-MRRI7S21611";
const char* password = "A3R7252d";

// 【要設定】先ほどコピーしたDiscordのWebhook URLをここに貼り付けてください
const char* discordWebhookUrl = "https://discord.com/api/webhooks/1521015525822693449/v28misL9DKmPerF_V1vTD7D43DWDshmYjhlSJ_yWW1i5gqq8KEqFBLjcFYsG7JhOo4T0";

// ★【実験用設定】テストのために、最初は「10秒」にしています。
// 本番（30分）にする場合は、下の 10000 を 1800000 に書き換えてください。
// ※ 30分 ＝ 30 × 60秒 × 1000ミリ秒 ＝ 1,800,000 ミリ秒
const unsigned long NO_TRACK_PERIOD = 1000000; 

// ニコちゃんマーク（緑色）の配置データ
const int smileMap[25] = {
  0, 0, 0, 0, 0,
  0, 1, 0, 1, 0,
  0, 0, 0, 0, 0,
  1, 0, 0, 0, 1,
  0, 1, 1, 1, 0
};

bool isLit = false;

// 最後にボタンが押された時刻を保存する変数
unsigned long lastPressedTime = 0; 
// 警告通知をすでに送ったかどうかを管理するフラグ
bool alertSent = false; 

// Discord送信関数
void sendToDiscord(String message) {
    Serial.println("Discordへの送信を開始します...");
    WiFiClientSecure client;
    client.setInsecure();
    
    String urlStr = String(discordWebhookUrl);
    urlStr.replace("https://", "");
    int splitIndex = urlStr.indexOf("/");
    String host = urlStr.substring(0, splitIndex);
    String urlPath = urlStr.substring(splitIndex);
    
    if (!client.connect(host.c_str(), 443)) {
        Serial.println("❌ Discordサーバーへの接続に失敗しました。");
        return;
    }
    
    String jsonBody = "{\"content\":\"" + message + "\"}";
    client.print(String("POST ") + urlPath + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + jsonBody.length() + "\r\n" +
                 "Connection: close\r\n\r\n" +
                 jsonBody);
                 
    while (client.connected() || client.available()) {
        if (client.available()) { client.readStringUntil('\n'); }
    }
    Serial.println("Discordへの送信処理を完了しました。");
    client.stop();
}

void connectToWiFi() {
    Serial.println("Wi-Fi接続を開始します...");
    WiFi.begin(ssid, password);
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED && counter < 20) {
        M5.dis.drawpix(12, 0xFFFF00); delay(250); M5.dis.clear(); delay(250);
        counter++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi接続成功！");
        fillScreen(0x0000FF); delay(1000); M5.dis.clear();
        
        // 起動した（Wi-Fiに繋がった）時間を初期値としてセット
        lastPressedTime = millis(); 
    } else {
        fillScreen(0xFF0000); delay(2000); M5.dis.clear();
    }
}

void setup() {
    M5.begin(true, false, true);
    M5.dis.clear();
    connectToWiFi();
}

void loop() {
    M5.update();
    
    // 現在の起動からの経過時間を取得（ミリ秒単位）
    unsigned long currentTime = millis();
    
    // 【判定】もし最後に押されてから指定時間（10秒/30分）が経過し、まだ通知をしていなければ
    if (currentTime - lastPressedTime > NO_TRACK_PERIOD && !alertSent) {
        // 画面を黄色にしてピンチをお知らせ
        fillScreen(0xFF5500); 
        
        // Discordに警告を飛ばす
        sendToDiscord("⚠️【まもり窓・警告】30分以上ドアの動きが検知できません。確認してください。");
        
        // 何回も連続で通知がいかないように、フラグをtrueにする
        alertSent = true; 
    }
    
    // ボタンが押されたとき（ドアが動いたとき）
    if (M5.Btn.wasPressed()) {
        if (WiFi.status() == WL_CONNECTED) {
            // タイマーと通知フラグを完全にリセットする
            lastPressedTime = millis(); 
            alertSent = false; 
            
            if (isLit == false) {
                displaySmile();
                isLit = true;
                sendToDiscord("🔔【まもり窓】動作を検知しました。（タイマーをリセットしました）");
            } else {
                M5.dis.clear();
                isLit = false;
            }
        } else {
            fillScreen(0xFF0000); delay(500); connectToWiFi();
        }
    }
    delay(50);
}

void fillScreen(uint32_t color) {
    for (int i = 0; i < 25; i++) { M5.dis.drawpix(i, color); }
}

void displaySmile() {
    for (int i = 0; i < 25; i++) {
        if (smileMap[i] == 1) { M5.dis.drawpix(i, 0x00FF00); }
        else { M5.dis.drawpix(i, 0x000000); }
    }
}