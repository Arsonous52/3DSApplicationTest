#include <3ds.h>

typedef struct {
    u8* data;
    u32 size;
    u32 sampleRate;
} Sound;

ndspWaveBuf waveBuf;

Sound load_wav(const char* path) {
    Sound sound{};
    FILE* f = fopen(path, "rb");
    if (!f) return sound;

    char chunkId[4];
    u32 chunkSize;

    // Skip RIFF header
    fseek(f, 12, SEEK_SET);

    while (true) {
        if (fread(chunkId, 1, 4, f) != 4) break;
        fread(&chunkSize, 4, 1, f);

        if (memcmp(chunkId, "fmt ", 4) == 0) {
            u16 audioFormat;
            u16 numChannels;
            u32 sampleRate;
            u16 bitsPerSample;

            fread(&audioFormat, 2, 1, f);
            fread(&numChannels, 2, 1, f);
            fread(&sampleRate, 4, 1, f);
            fseek(f, 6, SEEK_CUR); // byte rate + block align
            fread(&bitsPerSample, 2, 1, f);

            if (audioFormat != 1 || bitsPerSample != 16 || numChannels != 2) {
                fclose(f);
                return sound; // unsupported format
            }
            sound.sampleRate = sampleRate;
            fseek(f, chunkSize - 16, SEEK_CUR);
        }
        else if (memcmp(chunkId, "data", 4) == 0) {
            sound.size = chunkSize;
            sound.data = (u8*)linearAlloc(sound.size);

            fread(sound.data, 1, sound.size, f);
            fclose(f);
            return sound;
        }
        else fseek(f, chunkSize, SEEK_CUR);
    }
    fclose(f);
	if (!sound.data || sound.size == 0) {
		svcBreak(USERBREAK_PANIC);
	}
    return sound;
}
void play_sound(Sound& sound) {
    memset(&waveBuf, 0, sizeof(ndspWaveBuf));

    waveBuf.data_vaddr = sound.data;
    waveBuf.nsamples = sound.size / 4; // stereo PCM16

    ndspChnReset(0);
    ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
    ndspChnSetRate(0, sound.sampleRate);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

    ndspChnWaveBufAdd(0, &waveBuf);
}