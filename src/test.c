#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

void test_get_request() {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/example");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "GET request failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
}

void test_post_request() {
    CURL *curl;
    CURLcode res;

    const char *json_data = "{\"name\":\"Test Name\",\"description\":\"Test Description\"}";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/example");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, (struct curl_slist *)curl_slist_append(NULL, "Content-Type: application/json"));

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "POST request failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
}

void test_put_request() {
    CURL *curl;
    CURLcode res;

    const char *json_data = "{\"name\":\"Updated Name\",\"description\":\"Updated Description\"}";
    const char *url = "http://localhost:8080/example?id=1";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, (struct curl_slist *)curl_slist_append(NULL, "Content-Type: application/json"));

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "PUT request failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
}

void test_delete_request() {
    CURL *curl;
    CURLcode res;

    const char *url = "http://localhost:8080/example?id=1";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "DELETE request failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
}

int main() {
    printf("Testing POST request...\n");
    test_post_request();
    printf("\n");
    printf("Testing GET request...\n");
    test_get_request();
    printf("\n"); 
    printf("Testing PUT request...\n"); // TODO: falta implementar las ids dinamicas cuando se cree en el post para el put y delete
    test_put_request();
    printf("\n");
    printf("Testing DELETE request...\n");
    test_delete_request();
    printf("\n");
    return 0;
}
