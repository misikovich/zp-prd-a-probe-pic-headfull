idf_component_register(
    SRCS "main.c" "sound.c" "bt_spp.c" "bridge.c" "frame_filter.c"
         "${CMAKE_CURRENT_LIST_DIR}/../../../protocol/wprotocol.c"
    INCLUDE_DIRS "." "${CMAKE_CURRENT_LIST_DIR}/../../../protocol"
    PRIV_REQUIRES nvs_flash bt esp_driver_uart esp_driver_ledc esp_driver_gpio esp_timer
)
