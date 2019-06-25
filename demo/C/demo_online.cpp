

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <signal.h>
#include "Zouwu.h"


#define true    1
#define false   0
#define ERROR   1


//set alsa
const char alsa_device[] = "default";
snd_pcm_hw_params_t* hw_params;
//snd_pcm_info_t* snd_info;
snd_pcm_t* alsa_inst;
// creat Zouwu handle
void *g_hZouwu;

unsigned int SampleRate  = 16000;
unsigned int NumChannels = 1;
//int bitsample = 16;

volatile sig_atomic_t g_Exit_flag = 0;



int init_alsa()
{
	int err = 0;

	if ((err = snd_pcm_open(&alsa_inst, alsa_device, SND_PCM_STREAM_CAPTURE, 0)) < 0)
	{
        fprintf(stderr, "open %s device ERROR:  %d,%s\n",alsa_device,err,snd_strerror(err));
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
	{
        fprintf(stderr, "allocate hardware parameter ERROR:  %d,%s\n",err,snd_strerror(err));
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_any(alsa_inst, hw_params)) < 0)
	{
        fprintf(stderr, "initialize hardware ERROR:  %d,%s\n",err,snd_strerror(err));
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_set_access(alsa_inst, hw_params,SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
        fprintf(stderr, "set access ERROR:  %d,%s\n",err,snd_strerror(err));
		return ERROR;
	}

	if ((err = snd_pcm_hw_params_set_format(alsa_inst, hw_params,SND_PCM_FORMAT_S16_LE)) < 0)
	{
        fprintf(stderr, "set format ERROR:  %d,%s\n",err,snd_strerror(err));
		return ERROR;
	}
    //set sample rate
	if ((err = snd_pcm_hw_params_set_rate_near(alsa_inst, hw_params,&SampleRate, 0)) < 0)
	{
        fprintf(stderr, "set sample rate ERROR:  %d,%s\n",err,snd_strerror(err));
		return ERROR;
	}
    //set NumChannels
	if ((err = snd_pcm_hw_params_set_channels(alsa_inst, hw_params, NumChannels))< 0)
	{
        fprintf(stderr, "set channel ERROR:  %d,%s\n",err,snd_strerror(err));
		return ERROR;
	}

	if ((err = snd_pcm_hw_params(alsa_inst, hw_params)) < 0)
	{
        fprintf(stderr, "set params ERROR:  %d,%s\n",err,snd_strerror(err));
		return ERROR;
	}

	if ((err = snd_pcm_prepare(alsa_inst)) < 0)
	{		
        fprintf(stderr, "audio interface ERROR:  %d,%s\n",err,snd_strerror(err));
		return ERROR;
	}

	if ((err = snd_pcm_start(alsa_inst)) < 0)
	{
        fprintf(stderr, "start soundcard ERROR:  %d,%s\n",err,snd_strerror(err));
		return ERROR;
	}

	snd_pcm_hw_params_free(hw_params);

	return 0;
}


int Listen()
{

	int err = 0;
	const int buffer_size = 512;
	short data[buffer_size];
	int result = 0;
    printf("Hotword Listening ...\n");
	while (!g_Exit_flag)
	{
        err = snd_pcm_readi(alsa_inst, data, buffer_size) != buffer_size;

        if (err < 0) {	
            fprintf(stderr, "cannot read from audio interface ERROR:  %d,%s\n",err,snd_strerror(err));
            return ERROR;
        }
		 else {
            ZouwuProc(g_hZouwu, data, &result);
			if (result) {
				 printf("Hotword %d detected!\n", result);
			}
		}
	}
	return 0;
}

void Exitsignal(int signum) {
    g_Exit_flag = 1;
    printf("Hotword Exit ...\n");

}

int main(int argc, char *argv[])
{	
    //catch Ctrl+C signal
	signal(SIGINT, Exitsignal);
    int ret = 0;
    //initialize soundcard
	ret = init_alsa();
    if(ret)exit(0);
    //initialize ZouWu 
    ZouwuInit(&g_hZouwu);
    //load model 
    const char model[] = "../../resources/model/1.mdl";
    ZouwuLoadModel(g_hZouwu, model);
    //set sensitivity
    float sensitivity = 0.30f;
    int32_t id = 1;
    ZouwuSetParam(g_hZouwu, "sensitivity", &sensitivity,id);
   
    Listen();
    
    ZouwuFinal(&g_hZouwu);
    printf("ZouwuFinal done!\n");
    snd_pcm_close(alsa_inst);
    printf("close soundcard!\n");

	return 0;
}