#include <stdio.h>
#include <string.h>
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

//Message Structure Declaration
twai_message_t Button;
twai_message_t LED;

//Transmit Data Function
void transmit (uint32_t ID,char *data) {
    twai_message_t txData; // Message Structure
    txData.identifier = ID;
    txData.self = 1;
    txData.data_length_code = strlen(data);
    
    for (int i = 0; i < txData.data_length_code; i++) {
        txData.data[i] = data[i];
    }
    if (twai_transmit(&txData,pdMS_TO_TICKS(1000)) == ESP_OK) {
        ESP_LOGI(NodeName,"Message qued for transmission");
        return;
    } else {
        ESP_LOGI(NodeName,"Error: Message not qued for tranmission"); //Add TWAI error code here
        return;
    }
}

void app_main(void)
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
    char * message = "hello";
    
    transmit(4,message);
    

    twai_message_t rx_message; //Structure for incoming message

    //Receiving Data
    if (twai_receive(&rx_message,pdMS_TO_TICKS(10000))== ESP_OK) {
        printf("Messsage Recieved\n");
    } else {
        printf("Failed to recieve message\n");
        return;
    }
    
    //Reading Recieved Data
    printf("ID is %ld\n",rx_message.identifier);
    if (!(rx_message.rtr)) {
        for (int i = 0; i < rx_message.data_length_code; i++) {
            printf("%d, ", rx_message.data[i]);
        }
    }

    //Stop TWAI driver
    if (twai_stop() == ESP_OK) {
        printf("\nDriver Uninstalled\n");
    } else {
        printf("\nFailed to uninstall driver\n");
        return;
    }
}