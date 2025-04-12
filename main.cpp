Below is the C++ code to consume the provided APIs using the `libcurl` library:

```cpp
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>

// Function to create a chat session
std::string createChatSession(const std::string& apiKey, const std::string& externalUserId) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string url = "https://api.on-demand.io/chat/v1/sessions";
    std::string response;
    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(headers, ("apikey: " + apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    Json::Value body;
    body["pluginIds"] = Json::Value(Json::arrayValue); // Empty array
    body["externalUserId"] = externalUserId;

    Json::StreamWriterBuilder writer;
    std::string requestBody = Json::writeString(writer, body);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL error: " + std::string(curl_easy_strerror(res)));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    Json::CharReaderBuilder reader;
    Json::Value jsonResponse;
    std::string errs;
    std::istringstream stream(response);
    if (!Json::parseFromStream(reader, stream, &jsonResponse, &errs)) {
        throw std::runtime_error("Failed to parse JSON response: " + errs);
    }

    if (jsonResponse["data"].isMember("id")) {
        return jsonResponse["data"]["id"].asString();
    } else {
        throw std::runtime_error("Session ID not found in response");
    }
}

// Function to submit a query
std::string submitQuery(const std::string& apiKey, const std::string& sessionId, const std::string& query) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string url = "https://api.on-demand.io/chat/v1/sessions/" + sessionId + "/query";
    std::string response;
    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(headers, ("apikey: " + apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    Json::Value body;
    body["endpointId"] = "predefined-openai-gpt4o";
    body["query"] = query;
    body["pluginIds"] = Json::Value(Json::arrayValue);
    body["pluginIds"].append("plugin-1712327325");
    body["pluginIds"].append("plugin-1713962163");
    body["pluginIds"].append("plugin-1722260873");
    body["pluginIds"].append("plugin-1713954536");
    body["pluginIds"].append("plugin-1713958591");
    body["pluginIds"].append("plugin-1713958830");
    body["pluginIds"].append("plugin-1713961903");
    body["pluginIds"].append("plugin-1713967141");
    body["responseMode"] = "sync";
    body["reasoningMode"] = "medium";

    Json::StreamWriterBuilder writer;
    std::string requestBody = Json::writeString(writer, body);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL error: " + std::string(curl_easy_strerror(res)));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

int main() {
    try {
        std::string apiKey = "<replace_api_key>";
        std::string externalUserId = "<replace_external_user_id>";
        std::string query = "Put your query here";

        // Step 1: Create Chat Session
        std::string sessionId = createChatSession(apiKey, externalUserId);
        std::cout << "Session ID: " << sessionId << std::endl;

        // Step 2: Submit Query
        std::string response = submitQuery(apiKey, sessionId, query);
        std::cout << "Query Response: " << response << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
```

### **Dependencies**:
1. **libcurl**: Install `libcurl` for HTTP requests.
2. **jsoncpp**: Install `jsoncpp` for JSON parsing and serialization.

### **Build Instructions**:
Compile the code with the following flags:
```bash
g++ -o chat_api_consumer chat_api_consumer.cpp -lcurl -ljsoncpp
```

Replace `<replace_api_key>` and `<replace_external_user_id>` with actual values before running the program.