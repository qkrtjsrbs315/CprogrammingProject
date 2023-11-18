#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>


// 콜백 함수 정의
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    char* data = (char*)userp;

    // 받은 데이터를 버퍼에 복사
    memcpy(data, contents, realsize);

    return realsize;
}

int main(void) {
    CURL* curl;
    CURLcode res;

    // libcurl 초기화
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // curl 핸들 생성
    curl = curl_easy_init();
    if (curl) {
        // API 엔드포인트 및 파라미터 설정 (OpenWeatherMap 사용 예제)
        char city[50];
        printf("도시 이름을 입력하세요: ");
        scanf("%s", city);

        char url[256];
        char apiKey[] = "62ab1a82ccdc8f500c06ce702ca2f432";  // 여기에 실제 API 키를 넣으세요

        // URL 동적 생성
        sprintf(url, "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", city, apiKey);

        // API 응답을 저장할 버퍼 생성
        char response_buffer[4096];

        // curl 옵션 설정
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

        // HTTP GET 요청 수행
        res = curl_easy_perform(curl);

        // 결과 확인
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
            printf("API 응답:\n%s\n", response_buffer);

        // curl 핸들 정리
        curl_easy_cleanup(curl);
    }

    // libcurl 정리
    curl_global_cleanup();

    return 0;
}