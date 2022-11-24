#include "SocketioClient.h"

const String getEventName(const String msg) {
	return msg.substring(2, msg.indexOf("\"",4));
}

const String getEventPayload(const String msg) {
	String result = msg.substring(msg.indexOf("\"",4)+2,msg.length()-1);
	if(result.startsWith("\"")) {
		result.remove(0,1);
	}
	if(result.endsWith("\"")) {
		result.remove(result.length()-1);
	}
	return result;
}

void SocketioClient::socketioEvent(socketIOmessageType_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case sIOtype_DISCONNECT:
      Serial.println("Disconnected!");
      break;

    case sIOtype_CONNECT:
      Serial.printf("Connected to url: %s\n", payload);

      // join default namespace (no auto join in Socket.IO V3)
      _socketIO.send(sIOtype_CONNECT, "/");
      break;

    case sIOtype_EVENT:
      String msg;
      msg = String((char*)payload);
      trigger(getEventName(msg).c_str(), getEventPayload(msg).c_str(), length);
      break;
  }
}

void SocketioClient::beginSSL(const char* host, const int port, const char* url) {
	_socketIO.beginSSL(host, port, url);
    initialize();
}

void SocketioClient::begin(const char* host, const int port, const char* url) {
	_socketIO.begin(host, port, url);
    initialize();
}

void SocketioClient::initialize() {
    _socketIO.onEvent(std::bind(&SocketioClient::socketioEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_lastPing = millis();
}

void SocketioClient::loop() {
	_socketIO.loop();
}

void SocketioClient::on(const char* event, std::function<void (const char * payload, size_t length)> func) {
	_events[event] = func;
}

void SocketioClient::emit(const char* event, const char * payload) {
	String msg = String("42[\"");
	msg += event;
	msg += "\"";
	if(payload) {
		msg += ",";
		msg += payload;
	}
	msg += "]";
	Serial.printf("[SIoC] add packet %s\n", msg.c_str());
	_packets.push_back(msg);
}

void SocketioClient::trigger(const char* event, const char * payload, size_t length) {
	auto e = _events.find(event);
	if(e != _events.end()) {
		Serial.printf("[SIoC] trigger event %s\n", event);
		e->second(payload, length);
	} else {
		Serial.printf("[SIoC] event %s not found. %d events available\n", event, _events.size());
	}
}

void SocketioClient::disconnect()
{
	trigger("disconnect", NULL, 0);
}
