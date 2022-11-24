#ifndef __SOCKETIOCLIENT_H__
#define __SOCKETIOCLIENT_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include <vector>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#define SOCKETIOCLIENT_DEBUG(...) Serial.printf(__VA_ARGS__);
//#define SOCKETIOCLIENT_DEBUG(...)

#define PING_INTERVAL 10000 //TODO: use socket.io server response

//#define SOCKETIOCLIENT_USE_SSL
#ifdef SOCKETIOCLIENT_USE_SSL
	#define DEFAULT_PORT 443
#else
	#define DEFAULT_PORT 80
#endif
#define DEFAULT_URL "/socket.io/?EIO=4"
#define DEFAULT_FINGERPRINT ""


class SocketioClient {
private:
	std::vector<String> _packets;
	SocketIOclient _socketIO;
	int _lastPing;
	std::map<String, std::function<void (const char * payload, size_t length)>> _events;

	void trigger(const char* event, const char * payload, size_t length);
	void socketioEvent(socketIOmessageType_t type, uint8_t* payload, size_t length);
    void initialize();
public:
    void beginSSL(const char* host, const int port = DEFAULT_PORT, const char* url = DEFAULT_URL);
	void begin(const char* host, const int port = DEFAULT_PORT, const char* url = DEFAULT_URL);
	void loop();
	void on(const char* event, std::function<void (const char * payload, size_t length)>);
	void emit(const char* event, const char * payload = NULL);
	void disconnect();
};

#endif
