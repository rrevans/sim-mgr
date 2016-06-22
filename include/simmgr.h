/*
 * simmagr.h
 *
 * Simulation Manager
 *
 * Copyright (C) 2016 Terence Kelleher. All rights reserved.
 *
 */

#ifndef _SIMMGR_H
#define _SIMMGR_H

#include <semaphore.h>
#include <time.h>

// Defines
//
#define SIMMGR_VERSION		1
#define STR_SIZE			64
#define SIMMGR_SHM_NAME		"simmgr"
#define LOG_TO_FILE			0

// When initSHM is called, only the simmgr daemon should set OPEN_WITH_CREATE
// All other open with _OPEN_ACCESS
#define	OPEN_WITH_CREATE	1
#define OPEN_ACCESS			0


// Data Structures
//
struct cardiac
{
	char rhythm[STR_SIZE];
	char vpc[STR_SIZE];
	int vpc_freq;		// 0-100% - Frequencey of VPC insertions (when vpc is not set to "none")
	char vfib_amplitude[STR_SIZE];	// low, med, high
	int pea;			// Pulseless Electrical Activity
	int rate;			// Heart Rate in Beats per Minute
	int nibp_rate;		// Non-Invasive Rate - Only reports when cuff is on
	int transfer_time;	// Trend length for change in rate;
	char pwave[STR_SIZE];
	int pr_interval;	// PR interval in msec
	int qrs_interval;		// QRS in msec
	int bps_sys;	// Systolic
	int bps_dia;	// Diastolic
	int pulse_strength;	// 0 - None, 3 - strong
	unsigned int pulseCount;
	char heart_sound[STR_SIZE];
	int heart_sound_volume;
	int heart_sound_mute;
	int ecg_indicator;
};

struct scenario
{
	char active[STR_SIZE];		// Name of active scenario
	char start[STR_SIZE];		// Date/Time scenario started
	char runtime[STR_SIZE];
	char state[STR_SIZE];
	char scene_name[STR_SIZE];	// Currently running scene
	int scene_id;				// Currently running scene
	int record;					// Set in initiator section to start/stop video recording
};

struct respiration
{
	// Sounds for Inhalation, Exhalation and Background
	char left_lung_sound[STR_SIZE];		// Base Sound 
	char left_sound_in[STR_SIZE];
	char left_sound_out[STR_SIZE];
	char left_sound_back[STR_SIZE];
	int left_lung_sound_volume;
	int left_lung_sound_mute;
	
	char right_lung_sound[STR_SIZE];		// Base Sound
	char right_sound_in[STR_SIZE];
	char right_sound_out[STR_SIZE];
	char right_sound_back[STR_SIZE];
	int right_lung_sound_volume;
	int right_lung_sound_mute;
	
	//char left_lung_sound[STR_SIZE];
	//char right_lung_sound[STR_SIZE];
	
	// Duration of in/ex
	int inhalation_duration;	// in msec
	int exhalation_duration;	// in msec
	
	int spo2;					// 0-100%
	int rate;					// Breaths per minute
	int etco2;					// End Tidal CO2
	int transfer_time;			// Trend length for change in rate;
	int etco2_indicator;
	int spo2_indicator;
	int chest_movement;
	
	unsigned int breathCount;
};

struct auscultation
{
	int side;	// 0 - None, 1 - L:eft, 2 - Right
	int row;	// Row 0 is closest to spine
	int col;	// Col 0 is closets to head
};
struct pulse
{
	int position;	// Position code of active pulse check
};
struct cpr
{
	int last;			// msec time of last compression
	int	compression;	// 0 to 100%
	int release;		// 0 to 100%
	int duration;
};
struct defibrillation
{
	int last;			// msec time of last shock
	int energy;			// Energy in Joules of last shock
};

struct hdr
{
	int	version;
	int size;
};
struct server
{
	char name[STR_SIZE]; 		// SimCtl Hostname
	char ip_addr[STR_SIZE];		// Public Network IP Address
	char server_time[STR_SIZE];	// Linux date/timestamp
	int msec_time;				// msec timer.
};
struct general
{
	int temperature;			// degrees * 10, (eg 96.8 is 968)
	int transfer_time;			// Trend length
};
struct media
{
	char filename[STR_SIZE];
	int play;
};
struct vocals
{
	char filename[STR_SIZE];
	int repeat;
	int volume;
	int play;
	int mute;
};
struct logfile
{
	sem_t	sema;	// Mutex lock - Used to allow multiple writers
	int		active;
	int		lines_written;
	char	filename[STR_SIZE];
};
struct status
{
	// Status of controlled parameters
	struct cardiac			cardiac;
	struct scenario 		scenario;
	struct respiration		respiration;
	struct general			general;
	struct vocals			vocals;
	struct media			media;
	
	// Status of sensed actions
	struct auscultation		auscultation;
	struct pulse			pulse;
	struct cpr				cpr;
	struct defibrillation	defibrillation;
	
};

// The instructor structure is commands from the Instructor Interface
struct instructor
{
	sem_t	sema;	// Mutex lock
	struct cardiac		cardiac;
	struct scenario 	scenario;
	struct respiration	respiration;
	struct general		general;
	struct vocals		vocals;
	struct media		media;
	
};
	
// Data Structure of Shared memory file
struct simmgr_shm
{
	// Static data describing the data simmgr_shm structure for verification.
	struct hdr hdr;
	
	// Dynamic data relating to the Server System
	struct server server;
	
	// SimMgr Status - Written by the SimMgr only. Read from all others
	struct status status;
	
	// Commands from Instructor Interface. Written by the II Ajax calls. Cleared by SimMgr when processed.
	struct instructor instructor;

	// Log file status
	struct logfile logfile;
};

// For generic trend processor
struct trend
{
	double start;
	double end;
	double current;
	double changePerSecond;
	time_t nextTime;
	time_t endTime;
	int seconds;
};

// Prototypes
// 
int	initSHM(int create );
void log_message(const char *filename, const char* message);
void daemonize(void );
int kbhit(void);
int checkExit(void );
char *nth_occurrence(char *haystack, char *needle, int nth);
char *do_command_read(const char *cmd_str, char *buffer, int max_len );
void get_date(char *buffer );
char *getETH0_IP();
char *itoa(int val, char *buf, int radix );
void signal_fault_handler(int sig);
void cleanString(char *strIn );

// Defines and protos for sim-log
#define SIMLOG_MODE_READ	0
#define SIMLOG_MODE_WRITE	1
#define SIMLOG_MODE_CREATE	2

int simlog_create(void );			// Create new file
int simlog_open(int rw );			// Open for Read or Write - Takes Mutex if writing
int simlog_write(char *msg );		// Write line
int simlog_read(char *rbuf );		// Read next line
int simlog_read_line(char *rbuf, int lineno );		// Read line from line number
void simlog_close();				// Closes file and release Mutex if held
void simlog_end();
void simlog_entry(char *msg );

// Shared Parse functions
int cardiac_parse(const char *elem, const char *value, struct cardiac *card );
int respiration_parse(const char *elem,  const char *value, struct respiration *resp );
int general_parse(const char *elem,  const char *value, struct general *gen );
int vocals_parse(const char *elem,  const char *value, struct vocals *voc );
int media_parse(const char *elem,  const char *value, struct media *med );
void initializeParameterStruct(struct instructor *initParams );
void processInit(struct instructor *initParams  );
int getValueFromName(char *param_class, char *param_element );
// Global Data
//
#ifndef SIMUTIL
extern int simmgrSHM;					// The File for shared memory
extern struct simmgr_shm *simmgr_shm;	// Data structure of the shared memory
#endif

// Used for deamonize only
extern char *program_invocation_name;
extern char *program_invocation_short_name;

#endif // _SIMMGR_H