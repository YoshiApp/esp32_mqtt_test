#include <Arduino.h>
#include "mqtt_client.h"
#include "WiFi.h"

extern const uint8_t mosquitto_org_pem_start[]   asm("_binary_src_mosquitto_org_pem_start");
extern const uint8_t mosquitto_org_pem_end[]   asm("_binary_src_mosquitto_org_pem_end");

const char *topic_rx = "/units/1111/rx";
const char *topic_tx = "/units/1111/tx";

esp_mqtt_client_handle_t client;

void publishMotorSetting() {
    printf("Publish Motor Setting\n");
    char data[] = {0x02, 0x04, 0x01, 0x01, 0x23, 0x00, 0x10, 0x22, 0x33};
    esp_mqtt_client_publish(client, topic_tx, data, 9, 1, 0);
}

void publishMotorStatus() {
    printf("Publish Motor Status\n");
    char data[] = {0x02, 0x05, 0x00};
    esp_mqtt_client_publish(client, topic_tx, data, 3, 1, 0);
}

void parseMqttData(char *data, int len) {
    if (data[0] == 0x02 && data[1] == 0x01) { // Motor Start
        printf("Received Motor Start\n");
    } else if (data[0] == 0x02 && data[1] == 0x02) { // Motor Stop
        printf("Received Motor Stop\n");
    } else if (data[0] == 0x02 && data[1] == 0x03) { // Motor Param Query
        printf("Received Query Motor %02x\n", data[2]);
        if (data[2] == 0x00) {
            publishMotorSetting();
        } else {
            publishMotorStatus();
        }
    }
}

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED://MQTT连上事件
            Serial.printf("MQTT_EVENT_CONNECTED\n");
            msg_id = esp_mqtt_client_subscribe(client, topic_rx, 1);
            // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED://MQTT断开连接事件
            Serial.printf("MQTT_EVENT_DISCONNECTED\n");
            // ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED://MQTT发送订阅事件
            Serial.printf("MQTT_EVENT_SUBSCRIBED, msg_id=%d\n", event->msg_id);
            // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED://MQTT取消订阅事件
            // ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED://MQTT发布事件
            Serial.printf("MQTT_EVENT_PUBLISHED, msg_id=%d\n", event->msg_id);
            break;
        case MQTT_EVENT_DATA://MQTT接受数据事件
            Serial.printf("MQTT_EVENT_DATA\n");

            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);   //主题
            // printf("DATA=%.*s\r\n", event->data_len, ss);      //内容
            for (int i = 0; i < event->data_len; i++)
                printf("%02x ", event->data[i]);
            printf("\n");
            
            parseMqttData(event->data, event->data_len);
            break;
        case MQTT_EVENT_ERROR://MQTT错误事件
            Serial.printf("MQTT_EVENT_ERROR\n");
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

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("JuHome", "Cc326303");
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