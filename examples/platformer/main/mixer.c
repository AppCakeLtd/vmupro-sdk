#include "mixer.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "vmupro_sdk.h"

static const char *TAG = "Mixer";
static bool initialised = false;

enum
{
    MAX_FILENAME_LEN = 255
};

enum
{
    BUFFER_LEN_BYTES = 4096
};

// typedef enum {
//     MS_IDLE,
//     MS_PLAYING,
// } MixerState;

typedef struct
{
    bool playing;
    int sizeInBytes;
    int bytesSent;
    char fileName[MAX_FILENAME_LEN];    
    uint8_t buffer[BUFFER_LEN_BYTES];
} MixerChannel;

enum
{
    NUM_CHANNELS = 2
};
static MixerChannel *channels[NUM_CHANNELS];

void ResetChannel(MixerChannel *chan)
{
    // memset(chan, 0, sizeof(MixerChannel));
    chan->sizeInBytes = 0;
    chan->bytesSent = 0;
    memset(chan->fileName, 0, MAX_FILENAME_LEN);
    memset(chan->buffer, 0, BUFFER_LEN_BYTES);
}

void LoadChannel(int idx, const char *path)
{

    MixerChannel *chan = channels[idx];

    if (chan->playing)
    {
        vmupro_log(VMUPRO_LOG_INFO, TAG, "Channel %d already playing", idx);
        ResetChannel(chan);
    }

    int nameLen = strlen(path);
    if (nameLen > MAX_FILENAME_LEN - 1)
    {
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "Chan %d, file path %s is too long", idx, path);
        return;
    }
    memcpy(chan->fileName, path, nameLen);

    int fileSize = vmupro_get_file_size(path);

    if (fileSize <= 0)
    {
        vmupro_log(VMUPRO_LOG_INFO, TAG, "Chan %d Failed to read file at path %s", idx, path);
        return;
    }

    // assume mono
    chan->sizeInBytes = fileSize;
    chan->playing = true;
}

void InitMixer()
{
    if (initialised)
    {
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "Already initialised");
        return;
    }
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Init");

    for (int i = 0; i < NUM_CHANNELS; i++)
    {

        channels[i] = (MixerChannel *)malloc(sizeof(MixerChannel));
        ResetChannel(channels[i]);
    }


    vmupro_audio_start_listen_mode();

    initialised = true;
}

void DeInitMixer()
{

    if (!initialised)
    {
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "Not inited, can't deinit");
        return;
    }

    vmupro_audio_exit_listen_mode();
    initialised = false;
}

void TestPlayClip()
{

    // vmupro_log(VMUPRO_LOG_INFO, TAG, "test play clip");

    LoadChannel( 0, "/sdcard/blue_danube_raw_mono.raw" );

}

void ChannelFinished(int idx){

    MixerChannel * chan = channels[idx];
    chan->playing = false;

}

void UpdateChannel(int idx){

    MixerChannel * chan = channels[idx];

    if (!chan->playing) return;

    uint32_t samplesFiled;
    uint32_t samplesAvail;
    int bufferPercent = vmupro_get_ringbuffer_fill_state(&samplesFiled, &samplesAvail);
    
    if ( bufferPercent > 60){
        return;
    }
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Buffer percent %d\n", bufferPercent);

    memset(chan->buffer, 0, BUFFER_LEN_BYTES);
    int bytesLeftInFile = (chan->sizeInBytes - chan->bytesSent);
    int bytesLeftInRingBuffer = (samplesAvail - samplesFiled) * 2;
    
    int readSizeBytes = BUFFER_LEN_BYTES;
    if( readSizeBytes > bytesLeftInFile ){
        readSizeBytes = bytesLeftInFile;
        vmupro_log(VMUPRO_LOG_INFO, TAG, "Chan %d, File %s approaching end", idx, chan->fileName);
    }
    if ( readSizeBytes > bytesLeftInRingBuffer){
        vmupro_log(VMUPRO_LOG_INFO, TAG, "Chan %d, File %s read size > avail ring buffer bytes: %d vs %d", idx, chan->fileName, readSizeBytes, bytesLeftInRingBuffer);
        readSizeBytes = bytesLeftInRingBuffer;
    }

    bool success = vmupro_read_file_bytes( chan->fileName, chan->buffer, chan->bytesSent, readSizeBytes);
    if ( !success ){
        vmupro_log(VMUPRO_LOG_INFO, TAG, "Chan %d File %s failed to read %d bytes", idx, chan->fileName, readSizeBytes);
        ChannelFinished(idx);
        return;
    }

    vmupro_audio_add_stream_samples((int16_t*)&chan->buffer, readSizeBytes /2, VMUPRO_AUDIO_MONO, true);
    chan->bytesSent += readSizeBytes;

}

void TestUpdate()
{

    for( int i = 0; i < NUM_CHANNELS; i++){
        UpdateChannel(i);
    }

}