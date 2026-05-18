#ifndef __EAB_SOUND_IMPL
#define __EAB_SOUND_IMPL

typedef struct {} sound_Sound;
sound_Sound sound_init(char *path);
void        sound_set_pitch(sound_Sound s, float pitch);
void        sound_play(sound_Sound sound);
void        sound_free(sound_Sound sound);

#endif
