#include <M5Atom.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Wi-Fi接続情報
const char* ssid     = "DESKTOP-MRRI7S21611";
const char* password = "A3R7252d";

// LINE Messaging APIの情報
const char* channelToken = "zg4EUlk9Yjj/l2Ud/nDcOBQ4v2tZop0i6y7ya3uM8v4ldN9tTbRPHcSEYw7+pzhEFv4iriPlO7eguyeHFyG1EgFKgW+yY5DzVRdAlt1Z5U8RkrD22zx11j4G199kLLw2cT9NNGsv3u5qM8D1gZibDwdB04t89/1O/w1cDnyilFU=";
const char* userId       = "U2c6119e6cd4c2139204c790adfec05f3";

// ⭐【重要】LINE(api.line.me)が使用しているDigiCertのルート証明書
// これを提示することで、安全な通信（HTTPS）の接続をLINEサーバーに許可させます
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDjjCCAnagAwIBAgIQAzrx5LGSrJCcSYOG7NY4bTA0BgkqhkiG9w0BAQsFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
"MjAeFw0zMDA4MDExMjAwMDBaFw00MTExMTAxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dyBkaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1EQmYqFBgTaAFrMMM9K917n\n" \
"E6w8bS2RdcMHvMfiXgZIaqpAm4mR6NdQAdee6mWhm5V5f1Qd7H14K81wX7sT8M6r\n" \
"H3as79Wjwfxfg1G3oay89L6E9X64Pvx3gC9i6I8236gD0h5B84/fNcsb0C868asl\n" \
"O0A9o3i3h9y8zSGAkSZDd8955Q06T6XjV8mU041/0089Kq9YI+wD058J4G7rY99k\n" \
"A6M4kL6/3ZtL8Yw5bC1R517vV91H63U25T8bJUMv+B5pL46+A06v/M88T00yQ6c0\n" \
"nBq218R5r3a6X3f6e1F3L294Z9gIatnE+r0A\n" \
"-----END CERTIFICATE-----\n";

// ニコちゃんマーク（緑色）の配置データ
const int smileMap[25] = {
  0, 0, 0, 0, 0,
  0, 1, 0, 1, 0,
  0, 0, 0, 0, 0,
  1, 0, 0, 0, 1,
  0, 1, 1, 1, 0
};

bool isLit = false;

void sendToLineBot(String message) {
    Serial.println("LINE Messaging APIへの送信を開始します...");
    
    WiFiClientSecure client;
    
    // ⭐【修正】スルーするのをやめて、正しい証明書をセットする
    client.setCACert(root_ca);
    client.setTimeout(15000); 
    
    const char* host = "api.line.me";
    
    Serial.println("LINEサーバー(api.line.me:443)に厳格なセキュリティ接続を試みています...");
    
    if (!client.connect(host, 443)) {
        Serial.println("❌ LINEサーバーへの接続に失敗しました。");
        Serial.print("現在のWi-Fiステータスコード: ");
        Serial.println(WiFi.status());
        Serial.print("M5AtomのIPアドレス: ");
        Serial.println(WiFi.localIP());
        return;
    }
    
    Serial.println("➔ 🎉サーバー接続に成功しました！データを送信します。");
    
    // JSONリクエストデータの作成
    String jsonBody = "{\"to\":\"" + String(userId) + "\",\"messages\":[{\"type\":\"text\",\"text\":\"" + message + "\"}]}";
    
    // HTTP POSTリクエストの送信
    client.print(String("POST /v2/bot/message/push HTTP/1.1\r\n") +
                 "Host: " + host + "\r\n" +
                 "Authorization: Bearer " + channelToken + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + jsonBody.length() + "\r\n" +
                 "\r\n" +
                 jsonBody);
                 
    // サーバーからの応答を表示
    Serial.println("--- LINEサーバーからの応答 ---");
    while (client.connected() || client.available()) {
        if (client.available()) {
            String line = client.readStringUntil('\n');
            Serial.println(line);
        }
    }
    Serial.println("----------------------------");
    client.stop();
}

void connectToWiFi() {
    Serial.println("Wi-Fi接続を開始します...");
    WiFi.begin(ssid, password);

    int counter = 0;
    while (WiFi.status() != WL_CONNECTED && counter < 20) {
        M5.dis.drawpix(12, 0xFFFF00); // 黄色点滅
        delay(250);
        M5.dis.clear();
        delay(250);
        Serial.print(".");
        counter++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi接続成功！");
        
        Serial.println("時刻を同期中...");
        configTime(9 * 3600, 0, "ntp.nict.jp", "time.google.com");
        delay(1500); // 同期待ち時間を少し延長

        fillScreen(0x0000FF); // 青点灯
        delay(1000);
        M5.dis.clear();
    } else {
        Serial.println("\nWi-Fi接続失敗");
        fillScreen(0xFF0000); // 赤点灯
        delay(2000);
        M5.dis.clear();
    }
}

void setup() {
    M5.begin(true, false, true);
    M5.dis.clear();
    connectToWiFi();
}

void loop() {
    M5.update();
    
    if (M5.Btn.wasPressed()) {
        if (WiFi.status() == WL_CONNECTED) {
            if (isLit == false) {
                displaySmile();
                isLit = true;
                sendToLineBot("【試験用：Messaging API】まもり窓のボタンが押されました！");
            } else {
                M5.dis.clear();
                isLit = false;
            }
        } else {
            fillScreen(0xFF0000);
            delay(500);
            connectToWiFi();
        }
    }
    delay(50);
}

void fillScreen(uint32_t color) {
    for (int i = 0; i < 25; i++) {
        M5.dis.drawpix(i, color);
    }
}

void displaySmile() {
    for (int i = 0; i < 25; i++) {
        if (smileMap[i] == 1) {
            M5.dis.drawpix(i, 0x00FF00);
        } else {
            M5.dis.drawpix(i, 0x000000);
        }
    }
}