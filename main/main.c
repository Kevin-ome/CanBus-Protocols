#include <stdio.h>
#include "driver/gpio.h" //GPIO Drivers
#include "driver/twai.h" //TWAI Drivers
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//Node configuration
#define TxPin GPIO_NUM_17
#define RxPin GPIO_NUM_18 
#define NodeMode TWAI_MODE_NO_ACK
static const char *NodeName = "Node1";


void app_main()
{
    //Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TxPin,RxPin,NodeMode); 
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    //Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(NodeName, "Driver installed");
    } else {
        ESP_LOGI(NodeName,"Failed to install driver");
        return;
    }

    //Start TWAI driver
    if (twai_start() == ESP_OK) {
        ESP_LOGI(NodeName,"Driver started\n");
    } else {
        ESP_LOGI(NodeName,"Failed to start driver\n");
        return;
    }
    
    twai_message_t zero; //message containing zero
    zero.identifier =  0x7A;
    zero.data_length_code = 4;
    for (int i = 0; i < 4; i++) {
        zero.data[i] = 0;
    }

    //Tranmitting Data
    if (twai_transmit(&zero,pdMS_TO_TICKS(1000)) == ESP_OK) {
        printf("Message queued for transmission\n");
    } else {
        printf("Failed to queue message for transmission\n");
    }

    twai_message_t input; //Structure for incoming message

    //Receiving Data
    if (twai_receive(&input,pdMS_TO_TICKS(10000))== ESP_OK) {
        printf("Messsage Recieved\n");
    } else {
        printf("Failed to recieve message\n");
        return;
    }
    
    //Reading Recieved Data
    printf("ID is %ld\n",input.identifier);
    if (!(input.rtr)) {
        for (int i = 0; i < input.data_length_code; i++) {
            printf("Data byte %d = %d\n", i, input.data[i]);
        }
    }

    //Stop TWAI driver
    if (twai_stop() == ESP_OK) {
        printf("Driver Uninstalled");
    } else {
        printf("Failed to uninstall driver\n");
        return;
    }
}