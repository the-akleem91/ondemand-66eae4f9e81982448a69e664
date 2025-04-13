Below is the C++ code to consume the provided APIs using the `libcurl` library:

```cpp
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>

// Helper function to handle response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to create a chat session
std::string createChatSession(const std::string& apiKey, const std::string& externalUserId) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://api.on-demand.io/chat/v1/sessions";
        std::string jsonBody = "{\"pluginIds\":[],\"externalUserId\":\"" + externalUserId + "\"}";

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("apikey: " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return readBuffer;
}

// Function to submit a query
std::string submitQuery(const std::string& apiKey, const std::string& sessionId, const std::string& query) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://api.on-demand.io/chat/v1/sessions/" + sessionId + "/query";
        std::string jsonBody = "{\"endpointId\":\"predefined-openai-gpt4o\",\"query\":\"" + query +
                               "\",\"pluginIds\":[\"plugin-1712327325\",\"plugin-1713962163\",\"plugin-1722260873\","
                               "\"plugin-1713954536\",\"plugin-1713958591\",\"plugin-1713958830\",\"plugin-1713961903\","
                               "\"plugin-1713967141\"],\"responseMode\":\"sync\",\"reasoningMode\":\"medium\"}";

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("apikey: " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return readBuffer;
}

int main() {
    std::string apiKey = "<replace_api_key>";
    std::string externalUserId = "<replace_external_user_id>";
    std::string query = "Put your query here";

    // Step 1: Create Chat Session
    std::string sessionResponse = createChatSession(apiKey, externalUserId);

    // Parse the session ID from the response
    Json::Value jsonData;
    Json::Reader jsonReader;
    if (!jsonReader.parse(sessionResponse, jsonData)) {
        std::cerr << "Failed to parse JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
        return 1;
    }

    std::string sessionId = jsonData["data"]["id"].asString();
    if (sessionId.empty()) {
        std::cerr << "Failed to extract session ID from response." << std::endl;
        return 1;
    }

    std::cout << "Session ID: " << sessionId << std::endl;

    // Step 2: Submit Query
    std::string queryResponse = submitQuery(apiKey, sessionId, query);

    std::cout << "Query Response: " << queryResponse << std::endl;

    return 0;
}
```

### **Dependencies**:
1. Install `libcurl` for HTTP requests.
2. Install `jsoncpp` for JSON parsing.

### **Build Instructions**:
Compile the code with the following flags:
```bash
g++ -o chat_api_consumer chat_api_consumer.cpp -lcurl -ljsoncpp
```

Replace `<replace_api_key>` and `<replace_external_user_id>` with actual values before running the program.