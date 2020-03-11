/* Non-Volatile Storage (NVS) Read and Write a Value */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "nvs_flash.h"

void app_main() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    
    // Open
    nvs_handle my_handle;
    nvs_open("STORAGE", NVS_READWRITE, &my_handle);

    int32_t getVar = 666;
    int32_t setVar = 111;
    // Read
    err = nvs_get_i32(my_handle, "KeyValue", &getVar);
    switch (err) {
        case ESP_OK:
            printf("The read value is %d [getVar]\n", getVar);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            // Initialaizing value
            setVar = 123;
            // Write value
            err = nvs_set_i32(my_handle, "KeyValue", setVar);
            printf((err != ESP_OK) ? "Failed!\n" : "The written value is %d\n",setVar);
            // Commit written value.
            nvs_commit(my_handle);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
    }

    // Close
    nvs_close(my_handle);
}
