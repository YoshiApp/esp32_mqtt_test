#include <Arduino.h>
#include "mqtt_client.h"
#include "WiFi.h"

extern const uint8_t mosquitto_org_pem_start[]   asm("_binary_src_mosquitto_org_pem_start");
extern const uint8_t mosquitto_org_pem_end[]   asm("_binary_src_mosquitto_org_pem_end");

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED://MQTT连上事件
            Serial.printf("MQTT_EVENT_CONNECTED\n");
            msg_id = esp_mqtt_client_subscribe(client, "/topic/iot_monitor", 1);
            // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED://MQTT断开连接事件
            // ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED://MQTT发送订阅事件
            // ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/iot_monitor", "订阅成功", 0, 0, 0);
            // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED://MQTT取消订阅事件
            // ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED://MQTT发布事件
            // ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA://MQTT接受数据事件
            // ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);   //主题
            printf("DATA=%.*s\r\n", event->data_len, event->data);      //内容
            break;
        case MQTT_EVENT_ERROR://MQTT错误事件
            Serial.printf("MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.event_handle = mqtt_event_handler;
    mqtt_cfg.uri = "mqtts://test.mosquitto.org";
    mqtt_cfg.port = 8883;
    mqtt_cfg.cert_pem = (const char *)mosquitto_org_pem_start;

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("dodo", "89326303");
    Serial.printf("WIFI_CONNECTING\n");
    while (WiFi.status() != WL_CONNECTED) //等待网络连接成功
    {

    }
    Serial.printf("WIFI_CONNECTED\n");
    mqtt_app_start();
}

void loop() {
  // put your main code here, to run repeatedly:
}