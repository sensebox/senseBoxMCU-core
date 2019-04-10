#include "WebUtil.h"

const char *CRLF = "\r\n";
const char *SP = " ";
const String no_err = String("");
// '/' was excluded from the reserved symbols to be able to check the path as a whole.
const char reserved[9] = {';', '?', ':', '@', '&', '=', '+', '$', ','};
const String methods[8] = {String("GET"), String("HEAD"),
    String("POST"), String("PUT"), String("DELETE"), String("CONNECT"),
    String("OPTIONS"), String("TRACE")};
const String supportedMethods[1] = {String("GET")};
const String supportedVersions[3] = {String("HTTP/1.0"), String("HTTP/1.1"), String("HTTP/2.0")};

String parseRequestField(String line, Request &request) {
    // A request field consists of a key and a value, which are
    // separated by a colon
    if (line.indexOf(":") == -1) {
        return String("Invalid format for request field.");
    }
    // Get the key and the value.
    // It is expected that the key contains no colon.
    String key = line.substring(0, line.indexOf(":"));
    key.trim();
    String value = line.substring(line.indexOf(":") + 1);
    value.trim();
    // We currently ignore all fields but the user agent
    // If you want to save other fields at them here in the same manner.
    if (key == String("User-Agent")) {
        request.user_agent = value;
    }
    return no_err;
}

String parseMethod(const String methodString, String &target) {
    bool found = false;
    for (int i = 0; i < 8; i++) {
        if (methods[i] == methodString) {
            found = true;
            break;
        }
    }
    if (!found) {
        return String("Unknown request method.");
    }
    found = false;
    for (int i = 0; i < 8; i++) {
        if (methods[i] == methodString) {
            target = methodString;
            return no_err;
        }
    }
    if (!found) {
        return String("Unsupported request method.");
    }
}

String parseUri(const String uri, String &target) {
    if (uri.length() == 0) {
        target = String("/");
    } else {
        target = uri;
    }
    if (target[0] != '/') {
        return String("Only supporting absolute paths as URIs. (") + target + ")";
    }
    for (int i = 0; i < 9; i++) {
        if (target.indexOf(reserved[i]) != -1) {
            return String("URI contains reserved symbol. (") + reserved[i] + ")";
        }
    }
    target = uri;
    return no_err;
}

String parseProtocolVersion(const String str, String &version) {
    for (int i = 0; i < 3; i++) {
        if (str == supportedVersions[i]) {
            version = str;
            return no_err;
        }
    }
    return String("Unknown HTTP Version detected. (") + str + ")";
}

String parseRequestLine(String requestLine, Request &request) {
    int idx = requestLine.indexOf(SP);
    if (idx == -1) {
        return String("Request line is malformed. (no spaces)");
    }
    String methodString = requestLine.substring(0, idx);
    String error = parseMethod(methodString, request.method);
    if (error.length() > 0) { return error; }
    int scd_idx = requestLine.indexOf(SP, idx+1);
    if (scd_idx == -1) {
        return String("Request line is malformed. (missing space)");
    }
    String uri = requestLine.substring(idx+1, scd_idx);
    error = parseUri(uri, request.uri);
    if (error.length() > 0) { return error; }
    if (scd_idx+1 >= requestLine.length()) { 
        return String("Missing version string.");
    }
    String version = requestLine.substring(scd_idx+1);
    error = parseProtocolVersion(version, request.protocol_version);
    if (error.length() > 0) { return error; }
    return no_err;
}

String parseRequest(String requestString, Request &request) {
    // Search for first line break and try to parse line as request line.
    int line_idx = 0;
    int pointer = requestString.indexOf(CRLF);
    if (pointer == -1) {
        return String("Request doesn't contain a single line ending.");
    }
    String requestLine = requestString.substring(0, pointer);
    // If there was an error when parsing the request line return that error.
    String error = parseRequestLine(requestLine, request);
    if (error.length() > 0) { return error; }
    // Otherwise parse all preceding lines until one line is empty.
    // A line is empty if the position in memory of two succeeding linebreaks is 2.
    int nextPointer = requestString.indexOf(CRLF, pointer + 2);
    while (pointer != nextPointer - 2) {
        // If no line break was found, the request did not end with a empty line, as it should.
        if (nextPointer == -1) {
            return String("Did not receive blank line at end of request.");
        }
        // Parse all lines as request fields.
        String line = requestString.substring(pointer, nextPointer);
        error = parseRequestField(line, request);
        if (error.length() > 0) { return error; }
        // search for next line break
        pointer = nextPointer;
        nextPointer = requestString.indexOf(CRLF, pointer + 2);
    }
    return no_err;
}

int parseRequestSafe(String requestString, Request &request) {
    // Try to parse request.
    String error = parseRequest(requestString, request);
    // If it fails print the error and return false as error code
    if (error.length() > 0) {
        Serial.println(String("An exception occurred when parsing the request: ") + error);
        return 0;
    }
    // Otherwise return 1
    return 1;
}

String buildSuccessfulResponse(const Request &request, const String &html) {
    String responseHeader = String("HTTP/1.1 200 OK") + CRLF;
    responseHeader = responseHeader + "Content-Type: text/html" + CRLF;
    responseHeader = responseHeader + "Connection: close" + CRLF + CRLF;
    return responseHeader + html;
}

String buildNotFoundResponse(const Request &request) {
    String responseHeader = String("HTTP/1.1 404 Not Found") + CRLF;
    responseHeader = responseHeader + "Content-Type: text/html" + CRLF;
    responseHeader = responseHeader + "Connection: close" + CRLF + CRLF;
    return responseHeader + String("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
            "<html>"
            "<head>"
            "   <title>404 Not Found</title>"
            "</head>"
            "<body>"
            "   <h1>Not Found</h1>"
            "   <p>The requested URL was not found on this server.</p>"
            "</body>"
            "</html>");
}

String buildHTML(const String &header, const String &body) {
    return String("<!DOCTYPE HTML>") + CRLF
        + "<html>" + CRLF
        + "<head>" + CRLF
        + header + CRLF
        + "</head>" + CRLF
        + "<body>" + CRLF
        + body + CRLF
        + "</body>" + CRLF
        + "</html>";
}

String buildTag(const String &tag, const String &content) {
    return String("<") + tag + ">" + content + "</" + tag + ">";
}

String listenClient(WiFiClient &client) {
    boolean currentLineIsBlank = true;
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    return request;
}
