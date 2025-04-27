//#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"

#include "bsp/esp-bsp.h"
#include "es8311.h"
#include "audio.h"

#include "driver/i2s_std.h"
//#include "esp_codec_dev.h"

//#include "display/my_disp.h"
//#include "button/button.h"




static const char *TAG = "audio";
static i2s_chan_handle_t i2s_tx_chan;
static i2s_chan_handle_t i2s_rx_chan;

#define BUFFER_SIZE     (256)
#define SAMPLE_RATE     (8000)




// Pointer to a file that is going to be played 
//const char music_nyan[] = "/spiffs/nyan_16bit_mono_8khz.wav";
//const char music_explosion[] = "/spiffs/explosion_16bit_mono_8khz.wav";
//const char *play_filename = music_nyan;


esp_err_t ret;
es8311_handle_t es8311_dev;
volatile bool g_stop_playback = false;

typedef struct __attribute__((packed))
{
    uint8_t ignore_0[22];
    uint16_t num_channels;
    uint32_t sample_rate;
    uint8_t ignore_1[6];
    uint16_t bits_per_sample;
    uint8_t ignore_2[4];
    uint32_t data_size;
    uint8_t data[];
} dumb_wav_header_t;

void audio_init(void)
{
    i2s_std_config_t i2s_std_config = BSP_I2S_DUPLEX_MONO_CFG(SAMPLE_RATE);
    uint16_t mclk_mult = i2s_std_config.clk_cfg.mclk_multiple;

    ESP_LOGI(TAG, "I2S_sample rate: %li", i2s_std_config.clk_cfg.sample_rate_hz);
    ESP_LOGI(TAG, "I2S_MCLK_multiplier: %li", i2s_std_config.clk_cfg.mclk_multiple);
    ESP_LOGI(TAG, "I2S_MCLK_freq: %li", SAMPLE_RATE * mclk_mult);

    es8311_dev = es8311_create(BSP_I2C_NUM, ES8311_ADDRRES_0);

    // Configure I2S peripheral and Power Amplifier 
    bsp_audio_init(&i2s_std_config, &i2s_tx_chan, &i2s_rx_chan);
    bsp_audio_poweramp_enable(true);

    // Create and configure ES8311 I2C driver 

    const es8311_clock_config_t clk_cfg = {
        .mclk_from_mclk_pin = true,
        .mclk_inverted = false,
        .sclk_inverted = false,
        .mclk_frequency = mclk_mult * SAMPLE_RATE,
        .sample_frequency = SAMPLE_RATE
    };

    /*ret = es8311_clock_config(es8311_dev, &clk_cfg, ES8311_RESOLUTION_16);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ES8311 clock config: %s", esp_err_to_name(ret));
        return;
    }*/


    ret = es8311_init(es8311_dev, &clk_cfg, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ES8311: %s", esp_err_to_name(ret));
        return;
    }
    
    // Set default volume
    ret = es8311_voice_volume_set(es8311_dev, DEFAULT_VOLUME, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set default volume: %s", esp_err_to_name(ret));
    }
}
void audio_play(const char *play_filename)
{
    g_stop_playback = false;  // Reset flag on new playback
    int16_t *wav_bytes = malloc(BUFFER_SIZE);
    assert(wav_bytes != NULL);

    // Open WAV file 
    ESP_LOGI(TAG, "Playing file %s", play_filename);
    FILE *play_file = fopen(play_filename, "rb");
    if (play_file == NULL) {
        ESP_LOGW(TAG, "%s file does not exist!", play_filename);
    }

    // Read WAV header file 
    dumb_wav_header_t wav_header;
    if (fread((void *)&wav_header, 1, sizeof(wav_header), play_file) != sizeof(wav_header)) {
        ESP_LOGW(TAG, "Error in reading file");
    }
    /*
    ESP_LOGI(TAG, "Number of channels: %d", wav_header.num_channels);
    ESP_LOGI(TAG, "Bits per sample: %d", wav_header.bits_per_sample);
    ESP_LOGI(TAG, "Sample rate: %d", wav_header.sample_rate);
    ESP_LOGI(TAG, "Data size: %d", wav_header.data_size);
    */

    uint32_t bytes_send_to_i2s = 0;
    while (bytes_send_to_i2s < wav_header.data_size && !g_stop_playback) {
        // Get data from SPIFFS
        size_t bytes_read_from_spiffs = fread(wav_bytes, 1, BUFFER_SIZE, play_file);

        // Send it to I2S 
        size_t i2s_bytes_written;
        ESP_ERROR_CHECK(i2s_channel_write(i2s_tx_chan, wav_bytes, bytes_read_from_spiffs, &i2s_bytes_written, pdMS_TO_TICKS(500)));
        bytes_send_to_i2s += i2s_bytes_written;
    }

    fclose(play_file);
    free(wav_bytes);
    ESP_LOGI(TAG, "Finnished playing file");
    if (g_stop_playback)
    {
        g_stop_playback = false; // Reset flag if playback was stopped
        ESP_LOGI(TAG, "Playback stopped by user");
        vTaskDelete(NULL); // Delete task if playback was stopped
    }
    //g_stop_playback = false; // Reset flag after playback
}

// Set volume to 0-100%
void audio_set_volume(void * volume)
{
    esp_err_t ret = es8311_voice_volume_set(es8311_dev, (int)(intptr_t)volume, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set volume: %s", esp_err_to_name(ret));
    }
    vTaskDelete(NULL); // Delete task after setting volume
}


void audio_loop_task(void * FILE_param)
{
    FileID file_id = (FileID)FILE_param; // Cast back to FileID
    while (1) {
        audio_play(file_paths[file_id]);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void audio_task(void * FILE_param)
{
    g_stop_playback = true; // Stop any previous playback
    vTaskDelay(pdMS_TO_TICKS(10)); // Wait for the previous playback to stop
    while (g_stop_playback) {
        vTaskDelay(pdMS_TO_TICKS(1)); // Wait for the previous playback to stop
    }
    FileID file_id = (FileID)FILE_param; // Cast back to FileID
    audio_play(file_paths[file_id]);
    vTaskDelete(NULL); // Delete task after playing the file
}


void audio_play_explosion() {

    //es8311_voice_volume_set(es8311_dev, 100, NULL);
    FileID file_id = (FileID)FILE_EXPLOSION; // Cast back to FileID
    audio_play(file_paths[file_id]);

    vTaskDelete(NULL); // Delete task after playing the file
}