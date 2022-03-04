#include <Arduino.h>

class Request {
    public:
        String method;
        String uri;
        String protocol_version;
        String user_agent;
};
