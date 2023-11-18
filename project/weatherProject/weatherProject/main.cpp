#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>


// �ݹ� �Լ� ����
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    char* data = (char*)userp;

    // ���� �����͸� ���ۿ� ����
    memcpy(data, contents, realsize);

    return realsize;
}

int main(void) {
    CURL* curl;
    CURLcode res;

    // libcurl �ʱ�ȭ
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // curl �ڵ� ����
    curl = curl_easy_init();
    if (curl) {
        // API ��������Ʈ �� �Ķ���� ���� (OpenWeatherMap ��� ����)
        char city[50];
        printf("���� �̸��� �Է��ϼ���: ");
        scanf("%s", city);

        char url[256];
        char apiKey[] = "62ab1a82ccdc8f500c06ce702ca2f432";  // ���⿡ ���� API Ű�� ��������

        // URL ���� ����
        sprintf(url, "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", city, apiKey);

        // API ������ ������ ���� ����
        char response_buffer[4096];

        // curl �ɼ� ����
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

        // HTTP GET ��û ����
        res = curl_easy_perform(curl);

        // ��� Ȯ��
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
            printf("API ����:\n%s\n", response_buffer);

        // curl �ڵ� ����
        curl_easy_cleanup(curl);
    }

    // libcurl ����
    curl_global_cleanup();

    return 0;
}