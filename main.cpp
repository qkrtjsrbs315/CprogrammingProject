#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
// 콜백 함수 정의
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    char *data = (char *)userp;
    memcpy(data, contents, realsize);
    return realsize;
}

// ipinfo.io에서 현재 IP의 도시를 가져오는 함수
char *getCityFromIpInfo()
{
    CURL *curl;
    CURLcode res;
    char ipUrl[] = "http://ipinfo.io/json";
    char ipResponseBuffer[4096];

    // libcurl 초기화
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (!curl)
    {
        fprintf(stderr, "Curl initialization failed.\n");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, ipUrl);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, ipResponseBuffer);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        fprintf(stderr, "ip curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return NULL;
    }

    // Extract city from the IP response
    json_error_t error;
    json_t *root = json_loads(ipResponseBuffer, 0, &error);
    if (!root)
    {
        fprintf(stderr, "JSON parsing failed: %s\n, ip_response_buffer: %s\n", error.text,ipResponseBuffer);
        return NULL;
    }

    // Extract the "city" value
    const char *city = json_string_value(json_object_get(root, "city"));

    // Print the extracted "city" value
    printf("City: %s\n", city);

    // curl 핸들 정리
    curl_easy_cleanup(curl);
    // libcurl 정리
    curl_global_cleanup();

    return strdup(city);
}

// OpenWeatherMap API에서 날씨 정보를 가져오는 함수
char *getWeatherInfo(const char *city)
{
    CURL *curl;
    CURLcode res;
    char apiKey[] = "62ab1a82ccdc8f500c06ce702ca2f432";
    char weatherUrl[256];
    json_error_t error;
    // libcurl 초기화
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (!curl)
    {
        fprintf(stderr, "Curl initialization failed.\n");
        return NULL;
    }

    snprintf(weatherUrl, sizeof(weatherUrl), "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", city, apiKey);

    char response_buffer[4096];

    curl_easy_setopt(curl, CURLOPT_URL, weatherUrl);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        fprintf(stderr, "weather curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    else
    {
        // JSON 파싱 및 "main" 값 추출
        json_t *root = json_loads(response_buffer, 0, &error);
        if (root)
        {
            json_t *weather = json_object_get(root, "weather");
            if (json_array_size(weather) > 0)
            {
                json_t *firstWeather = json_array_get(weather, 0);
                json_t *mainValue = json_object_get(firstWeather, "main");

                if (json_is_string(mainValue))
                {
                    const char *mainString = json_string_value(mainValue);
                    printf("now weather: %s\n", mainString);
                    
  		    char *result = strdup(mainString);
                    json_decref(root);
                    curl_easy_cleanup(curl);
                    curl_global_cleanup();
    
                    return result;
                 }
            }

             json_decref(root);
        }
        else
        {
            fprintf(stderr, "JSON parsing failed: %s\n Weather response buffer: %s\n", error.text,response_buffer);
        }
    }

    // curl 핸들 정리
    curl_easy_cleanup(curl);
    // libcurl 정리
    curl_global_cleanup();

    return NULL;

}
void ImageShow(const char weather[]){
   char path[256];
   sprintf(path,"./img/%s.png",weather);
   Mat img = imread(path);
   if (img.empty()){
      printf("Error: Could not open or read the image file\n");
      return;
   }
   namedWindow("Weather",WINDOW_AUTOSIZE);
   imshow("Weather",img);
   waitKey(0);




}

int main(void)
{
    char *city = getCityFromIpInfo();

    if (city)
    {
        printf("now City: %s\n", city);
        char *weather = getWeatherInfo(city);
        printf("main function weather value : %s\n",weather);
        ImageShow(weather);  


        // 사용이 끝난 도시 정보 메모리 해제
        free(weather);
        free(city);
    }
    else
    {
        printf("Can't fetch the City Info.\n");
    }

    return 0;
}
