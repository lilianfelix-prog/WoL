#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "wol.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_eth_driver.h"
#include "driver/gpio.h"

// CONFIG GPIO variables


char* TAG = "eth";
static bool eth_is_connected = false;

static esp_eth_handle_t eth_handle;
static uint8_t eth_mac[6]; 

static void eth_event_handler(void *arg, )
{
    switch(event_id){
        case ETH_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Ethernet link created");
            eth_is_connected = true;
            //retrieves the MAC @ from hardware and store it to 
            esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, eth_mac);
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Ethernet link ended");
            eth_is_connected = false;
            break;
        case ETHERNET_EVENT_START:
            ESP_LOGI(TAG, "Ethernet started");
            break;
        case ETHERNET_EVENT_STOP:
            ESP_LOGI(TAG, "Ethernet stoped");
            break;
    }
}

static void initialize_ethernet(void)
{
    //instead of polling events, the eth_event_handler is registered and the event loop task
    //will call it when it see a eth_event type event 
    esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler, NULL);
    
    //Sets the address of the PHY on the MDIO bus.
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = CONFIG_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = CONFIG_ETH_PHY_ENABLE_GPIO;

    //set the GPIO pins for the MDC and MDIO of PHY chip, how MAC configure and monitor PHY
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp32_emac_config.smi_mdc_gpio_num = CONFIG_ETH_MDC_GPIO;
    esp32_emac_config.smi_mdio_gpio_num = CONFIG_ETH_MDIO_GPIO;

}



// Configure and connect ethernet driver for a esp32 on esp-idf rtos, 
// the esp32 would act as the server listening for incoming tcp over Ethernet
// and distribute the WoL Magic Packet.
// This config code is inspired by the exemple from docs.
// /*
//  * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
//  *
//  * SPDX-License-Identifier: Apache-2.0
//  */
// //*******************installing the Ethernet driver**********************//
// /** Event handler for Ethernet events */
// static void eth_event_handler(void *arg, esp_event_base_t event_base,
//                               int32_t event_id, void *event_data)
// {
//     uint8_t mac_addr[6] = {0};
//     /* we can get the ethernet driver handle from event data */
//     esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

//     switch (event_id)
//     {
//     case ETHERNET_EVENT_CONNECTED:
//         esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
//         ESP_LOGI(TAG, "Ethernet Link Up");
//         ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
//                  mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
//         break;
//     case ETHERNET_EVENT_DISCONNECTED:
//         ESP_LOGI(TAG, "Ethernet Link Down");
//         break;
//     case ETHERNET_EVENT_START:
//         ESP_LOGI(TAG, "Ethernet Started");
//         break;
//     case ETHERNET_EVENT_STOP:
//         ESP_LOGI(TAG, "Ethernet Stopped");
//         break;
//     default:
//         break;
//     }
// }

// // create a default event loop that runs in the background
// esp_event_loop_create_default();

// // register Ethernet event handler (to deal with user-specific stuff when events like link up/down happened)
// esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL);

// esp_eth_start(eth_handle); // start Ethernet driver state machine

// //***************connect the Ethernet driver**********************//
// /** Event handler for IP_EVENT_ETH_GOT_IP */
// static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
//                                  int32_t event_id, void *event_data)
// {
//     ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
//     const esp_netif_ip_info_t *ip_info = &event->ip_info;

//     ESP_LOGI(TAG, "Ethernet Got IP Address");
//     ESP_LOGI(TAG, "~~~~~~~~~~~");
//     ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
//     ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
//     ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
//     ESP_LOGI(TAG, "~~~~~~~~~~~");
// }

// esp_netif_init()); // Initialize TCP/IP network interface (should be called only once in application)
// esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH(); // apply default network interface configuration for Ethernet
// esp_netif_t *eth_netif = esp_netif_new(&cfg);     // create network interface for Ethernet driver

// esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle));                        // attach Ethernet driver to TCP/IP stack
// esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL); // register user defined IP event handlers
// esp_eth_start(eth_handle);                                                              // start Ethernet driver state machine

int main()
{   
    //system create a central freeRTOS task, task waiting on queue for "events".
    //in this case waiting for a ethernet driver to post "event" to the central queue.
    //ex: ETHERNET_EVENT_CONNECTED event when it detects a physical link 
    esp_event_loop_create_default();
    initialize_flow_control();
    initialize_ethernet();

    char buffer[100] = {0};
    char *password = "12345";
    char *cmd_wol = "./sendwol A1:B4:C5:C1:DD:3E";
    char *msg1 = "\n Enter password: \n";
    char *msg2 = "\n correct, sending packet.. \n";
    int listen_sock, len, new_sock ;
    struct sockaddr_in servaddr, cliaddr, woladdr;
    bzero(&servaddr, sizeof(servaddr));

    listen_sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (listen_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET; 
 
    if (bind(listen_sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(listen_sock);
        exit(1);
    }
    
    // Listen for connections
    if (listen(listen_sock, 5) < 0) {
        perror("Listen failed");
        close(listen_sock);
        exit(1);
    }

   
    len = sizeof(cliaddr);

    new_sock = accept(listen_sock, (struct sockaddr*) &cliaddr, &len);
    
    send(new_sock, msg1, strlen(msg1), 0);
    
    int n = recv(new_sock, buffer, sizeof(buffer)-1, 0);
    if (n <= 0) {
        printf("No data received or connection closed\n");
        close(new_sock);
        close(listen_sock);
        exit(1);
    }
    buffer[n] = '\0';

    //remove spaces and next line
    char *newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';
    char *carriage = strchr(buffer, '\r');
    if (carriage) *carriage = '\0';

    //password handling not secure, todo
    if(strcmp(buffer, password) == 0){
            
        send(new_sock, msg2, strlen(msg2), 0);
        
        //
        int ret = system(cmd_wol);
        if (ret == -1) {
            char* err_sys = "system() failed\n";
            send(new_sock, err_sys, strlen(err_sys), 0);
        }else{
            char* sucess = "WoL packet sent successfully\n";
            send(new_sock, sucess, strlen(sucess), 0);
        }
    } else {
        char* auth_failed = "Authentication failed!\n";
        send(new_sock, auth_failed, strlen(auth_failed), 0);
    }

    close(new_sock);

    close(listen_sock);

    return 0;
}