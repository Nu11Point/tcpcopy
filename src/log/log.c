#include <xcopy.h>

int g_log_level;

static FILE* file=NULL;
static pthread_mutex_t mutex;

static char* err_levels[] = { 
	"unknown",
	"emerg",
	"alert",
	"crit",
	"error",
	"warn",
	"notice",
	"info",
	"debug"
};

void initLogInfo()
{
#if (DEBUG_TCPCOPY)
	g_log_level=LOG_DEBUG;
#else 
	g_log_level=LOG_NOTICE;
#endif
	pthread_mutex_lock (&mutex);
	file=fopen("error.log","a+");
	pthread_mutex_unlock (&mutex);
}

static struct timeval get_time()
{
	struct timeval tp;
	gettimeofday(&tp,NULL);
	return tp;
}

void log_info(int level,const char *fmt, ...)
{
	va_list         args;
	struct tm       local_time;
	time_t          t;
	char            time_str[32];
	struct tm       *p_local_time = NULL;
	char            *p_time_str = NULL;
	size_t          len = 0;
	struct timeval  usec = get_time();

	if(g_log_level >= level)
	{
		pthread_mutex_lock (&mutex);

		if (file) {
			t = time(0);
			fprintf(file,"[%s] ",err_levels[level]);
			p_local_time = localtime_r(&t,&local_time);
			if(NULL == p_local_time)
			{
				return;
			}
			p_time_str = asctime_r(p_local_time,time_str);
			if(NULL == p_time_str)
			{
				return;
			}
			len = strlen(p_time_str);
			p_time_str[len-1] = '\0';
			fprintf(file,"%s usec=%ld ",p_time_str,usec.tv_usec);
			va_start(args, fmt);
			(void)vfprintf(file, fmt, args);
			fprintf( file, "\n" );
			va_end(args);
		}
		pthread_mutex_unlock (&mutex);
	}
}

void endLogInfo()
{
	pthread_mutex_lock (&mutex);
	if(file)
	{
		(void)fclose(file);
		file = NULL;
	}	
	pthread_mutex_unlock (&mutex);
}

