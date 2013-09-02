


typedef struct ao_data_s
{
  int samplerate;
  int channels;
  int format;
  int bps;
  int outburst; 	  
  int buffersize;         
  int pts;
} ao_data_t;


extern ao_data_t ao_data;

/* Defines that AFMT_ stuff */

/* standard, old OSS audio formats */
#ifndef AFMT_MU_LAW
#	define AFMT_MU_LAW		0x00000001
#	define AFMT_A_LAW		0x00000002
#	define AFMT_IMA_ADPCM		0x00000004
#	define AFMT_U8			0x00000008
#	define AFMT_S16_LE		0x00000010	/* Little endian signed 16*/
#	define AFMT_S16_BE		0x00000020	/* Big endian signed 16 */
#	define AFMT_S8			0x00000040
#	define AFMT_U16_LE		0x00000080	/* Little endian U16 */
#	define AFMT_U16_BE		0x00000100	/* Big endian U16 */
#endif

#ifndef AFMT_MPEG
#	define AFMT_MPEG		0x00000200	/* MPEG (2) audio */
#endif

#ifndef AFMT_AC3
#	define AFMT_AC3			0x00000400	/* Dolby Digital AC3 */
#endif

/* 32 bit formats (MSB aligned) formats */
#ifndef AFMT_S32_LE
# define AFMT_S32_LE              0x00001000
# define AFMT_S32_BE              0x00002000
#endif


/* native endian formats */
#ifndef	AFMT_S16_NE
# if WORDS_BIGENDIAN
#  define AFMT_S16_NE	AFMT_S16_BE
#  define AFMT_S32_NE	AFMT_S32_BE
# else
#  define AFMT_S16_NE	AFMT_S16_LE
#  define AFMT_S32_NE	AFMT_S32_LE
# endif
#endif

#ifndef AFMT_FLOAT
# define AFMT_FLOAT               0x00004000
#endif
int audio_out_format_bits(int format){
    switch (format)
    {
	case AFMT_S16_LE:
	case AFMT_S16_BE:
	case AFMT_U16_LE:
	case AFMT_U16_BE: 
	return 16;//16 bits

/*
  the following two formats are not available with old linux kernel
  headers (e.g. in 2.2.16)
*/
#ifdef AFMT_S32_LE
	case AFMT_S32_LE:
	return 32;
#endif
#ifdef AFMT_S32_BE
	case AFMT_S32_BE:
	return 32;
#endif
	case AFMT_FLOAT:
	return 32;
	
	case AFMT_MU_LAW:
	case AFMT_A_LAW:
	case AFMT_IMA_ADPCM:
	case AFMT_S8:
	case AFMT_U8:
	case AFMT_MPEG:
	case AFMT_AC3:
	default:
	    return 8;//default 1 byte
	
    }
    return 8;
}
