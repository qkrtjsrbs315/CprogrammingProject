#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>
#include <thread>
#include <chrono>

using namespace cv;

bool isImageShowTimeChecked();
double kToC(double k);

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    char *data = (char *)userp;
    memcpy(data, contents, realsize);
    return realsize;
}
//kelvin to Celcious
double kToC(double k) {
    return (k - 273.15);
}

char *getCityFromIpInfo()
{
    CURL *curl;
    CURLcode res;
    char ipUrl[] = "http://ipinfo.io/json";
    char ipResponseBuffer[4096];

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

    json_error_t error;
    json_t *root = json_loads(ipResponseBuffer, JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK, &error);

    if (!root)
    {
        fprintf(stderr, "JSON parsing failed: %s\n Weather response buffer: %s\n", error.text, ipResponseBuffer);
        return NULL;
    }

    const char *city = json_string_value(json_object_get(root, "city"));
    printf("City: %s\n", city);

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return strdup(city);
}

void ImageShow(const char weather[], double *temperature)
{
    char path[256];
    sprintf(path, "./img/%s.png", weather);
    Mat img = imread(path);

    if (img.empty())
    {
        printf("Error: Could not open or read the image file\n");
        return;
    }

    // 기온 정보 표시
    char temperatureString[20];
    snprintf(temperatureString, sizeof(temperatureString), "Temperature: %.2f C", *temperature);
    putText(img, temperatureString, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

    while (true)
    {
        imshow("Weather", img);
        waitKey(200);
        if (isImageShowTimeChecked())
            break;
    }

    return;
}

bool isHourChecked()
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return (timeinfo->tm_sec == 0);
}

//for infinity loop escape in ImageShow
bool isImageShowTimeChecked()
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return (timeinfo->tm_sec == 57);
}

char *getWeatherInfo(const char *city, double *temperature)
{
    CURL *curl;
    CURLcode res;
    char apiKey[] = "62ab1a82ccdc8f500c06ce702ca2f432";
    char weatherUrl[256];
    json_error_t error;

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
        json_t *root = json_loads(response_buffer, JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK, &error);
        if (root)
        {
            json_t *mainObject = json_object_get(root, "main");
            if (mainObject)
            {
                json_t *tempValue = json_object_get(mainObject, "temp");
                if (json_is_real(tempValue))
                {
                    *temperature = json_real_value(tempValue);
                    *temperature = kToC(*temperature);
                    printf("Temperature: %.2f Celsius\n", *temperature);
                }
            }

            json_t *weather = json_object_get(root, "weather");
            if (json_array_size(weather) > 0)
            {
                json_t *firstWeather = json_array_get(weather, 0);
                json_t *mainValue = json_object_get(firstWeather, "main");

                if (json_is_string(mainValue))
                {
                    const char *mainString = json_string_value(mainValue);
                    printf("Now weather: %s\n", mainString);

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
            fprintf(stderr, "JSON parsing failed: %s\n Weather response buffer: %s\n", error.text, response_buffer);
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return NULL;
}

int main(void)
{
    bool initialExecution = true;
    namedWindow("Weather", WINDOW_AUTOSIZE);

    while (true)
    {
        if (initialExecution || isHourChecked())
        {
            char *city = getCityFromIpInfo();

            if (city)
            {
                printf("Now City: %s\n", city);

                double temperature;
                char *weather = getWeatherInfo(city, &temperature);
                printf("Main function weather value: %s\n", weather);

                if (strstr(weather, "Clear") != NULL)
                    ImageShow("Clear", &temperature);
                else if (strstr(weather, "Clouds") != NULL)
                    ImageShow("Clouds", &temperature);
                else if (strstr(weather, "Rain") != NULL)
                    ImageShow("Rain", &temperature);
                else if (strstr(weather, "Snow") != NULL)
                    ImageShow("Snow", &temperature);
                else
                    printf("Unexpected weather condition\n");

                free(weather);
                free(city);
            }
            else
            {
                printf("Can't fetch the City Info.\n");
            }

            initialExecution = false;
        }
    }

    return 0;
}

