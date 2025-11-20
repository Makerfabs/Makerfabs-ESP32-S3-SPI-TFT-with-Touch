#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/ringbuf.h>
#include <freertos/event_groups.h>
#include <math.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string.h>
#include <dirent.h>
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "../../boards/matouch-3-5-8080-s3/board.h"

static const char *TAG = "RELEASE";



#define MAX_IMAGES 10
static lv_obj_t *img_obj;
static char image_paths[MAX_IMAGES][512];
static int image_count = 0;
static int img_index = 0;

// Hide the old image after animation and delete the timer
static void gesture_event_cb(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if(dir != LV_DIR_LEFT && dir != LV_DIR_RIGHT) return; // only handle left/right

    if (image_count <= 0) return;

    int new_index = img_index + (dir == LV_DIR_LEFT ? 1 : -1);
    if(new_index < 0) new_index = image_count - 1;
    if(new_index >= image_count) new_index = 0;

    img_index = new_index;
    
    // Load image from SD card
    lvgl_port_lock(0);
    lv_img_set_src(img_obj, image_paths[img_index]);
    lvgl_port_unlock();
}

static void lv_demo_purecolor_png_switch()
{
    static const char* manual_image_paths[] = {
        "A:/5.jpg",
        "A:/6.jpg", 
        "A:/7.jpg",
        "A:/8.jpg"
    };
    image_count = sizeof(manual_image_paths) / sizeof(manual_image_paths[0]);

    for(int i = 0; i < image_count && i < MAX_IMAGES; i++) {
        strncpy(image_paths[i], manual_image_paths[i], sizeof(image_paths[i]) - 1);
        image_paths[i][sizeof(image_paths[i]) - 1] = '\0';
        ESP_LOGI(TAG, "Added image path: %s", image_paths[i]);
    }
    
    ESP_LOGI(TAG, "Total images added: %d", image_count);
    if (image_count > 0) {
        // create a single image object and switch src immediately on gesture
        img_obj = lv_img_create(lv_scr_act());
        lv_img_set_src(img_obj, image_paths[0]);

        // register gesture handler on the screen
        lv_obj_add_event(lv_scr_act(), gesture_event_cb, LV_EVENT_GESTURE, NULL);
    } else {
        ESP_LOGW(TAG, "No PNG images found on SD card");
        // Display a message
        lvgl_port_lock(0);
        lv_obj_t* label = lv_label_create(lv_scr_act());
        lv_label_set_text(label, "No PNG images found on SD card");
        lv_obj_center(label);
        lvgl_port_unlock();
    }
}

static void release_task(void *arg)
{
    // imu_switch_jpg_task();
    lv_demo_purecolor_png_switch();
    vTaskDelete(NULL);
}


void release_demo(void)
{
    xTaskCreate(release_task,
                "release_task",
                1024 * 10,
                NULL,
                5,
                NULL);
}