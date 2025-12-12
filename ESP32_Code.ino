#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "wifi name";
const char* password = "wifi password";

// Servo setup
Servo myServo;
int servoPin = 25;

// Web server on port 80
WebServer server(80);

// HTML page stored in program memory
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Servo Control</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
        }

        .switch-container {
            position: relative;
            width: 320px;
            height: 520px;
            background: #2a2a2a;
            border-radius: 12px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.5),
                        inset 0 1px 0 rgba(255, 255, 255, 0.1);
            padding: 40px;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }

        .switch-plate {
            position: relative;
            width: 180px;
            height: 340px;
            background: #1a1a1a;
            border-radius: 8px;
            box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.8);
            overflow: hidden;
        }

        .switch-zone {
            position: absolute;
            width: 100%;
            height: 50%;
            cursor: pointer;
        }

        .switch-zone.top {
            top: 0;
        }

        .switch-zone.bottom {
            bottom: 0;
        }

        .switch-button {
            position: absolute;
            left: 50%;
            transform: translateX(-50%);
            width: 160px;
            height: 160px;
            background: #333;
            border-radius: 6px;
            transition: top 0.4s cubic-bezier(0.4, 0.0, 0.2, 1);
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.6);
            top: 170px;
            pointer-events: none;
        }

        .switch-button.on {
            top: 10px;
        }

        .led-indicator {
            position: absolute;
            right: 10px;
            top: 50%;
            transform: translateY(-50%);
            width: 4px;
            height: 140px;
            background: #ff6b00;
            border-radius: 2px;
            opacity: 1;
            transition: background 0.3s ease, box-shadow 0.3s ease;
            box-shadow: 0 0 10px #ff6b00, 0 0 20px #ff6b00;
        }

        .switch-button.on .led-indicator {
            background: #00d4ff;
            box-shadow: 0 0 10px #00d4ff, 0 0 20px #00d4ff;
        }
    </style>
</head>
<body>
    <div class="switch-container">
        <div class="switch-plate">
            <div class="switch-zone top" id="topZone"></div>
            <div class="switch-zone bottom" id="bottomZone"></div>
            <div class="switch-button" id="switchButton">
                <div class="led-indicator"></div>
            </div>
        </div>
    </div>

    <script>
        const topZone = document.getElementById('topZone');
        const bottomZone = document.getElementById('bottomZone');
        const switchButton = document.getElementById('switchButton');
        
        let isOn = false;

        topZone.addEventListener('click', () => {
            if (!isOn) {
                isOn = true;
                switchButton.classList.add('on');
                sendServoCommand(180);
            }
        });

        bottomZone.addEventListener('click', () => {
            if (isOn) {
                isOn = false;
                switchButton.classList.remove('on');
                sendServoCommand(0);
            }
        });

        function sendServoCommand(position) {
            fetch('/servo', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ position: position })
            })
            .then(response => response.json())
            .then(data => {
                console.log('Servo moved to:', position);
            })
            .catch(error => {
                console.error('Error:', error);
            });
        }

        window.addEventListener('load', () => {
            sendServoCommand(0);
        });
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleServo() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    
    // Parse JSON manually (simple approach)
    int posIndex = body.indexOf("\"position\":");
    if (posIndex != -1) {
      int position = body.substring(posIndex + 11).toInt();
      
      // Send response FIRST to prevent timeout
      server.send(200, "application/json", "{\"status\":\"success\",\"position\":" + String(position) + "}");
      
      // THEN move servo
      int pulseWidth = map(position, 0, 180, 500, 2500);
      myServo.writeMicroseconds(pulseWidth);
      
      Serial.print("Servo moved to: ");
      Serial.println(position);
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No body\"}");
  }
}

void setup() {
  
  // Attach servo to GPIO 25
  myServo.attach(servoPin);
  
  // Move to starting position (0 degrees)
  int pulseWidth = map(0, 0, 180, 500, 2500);
  myServo.writeMicroseconds(pulseWidth);
  delay(1000);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/servo", HTTP_POST, handleServo);
  
  // Start server
  server.begin();
}

void loop() {
  server.handleClient();
}