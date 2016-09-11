#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <wiringPi.h>
#include <sched.h>
#include <unistd.h>


#define MAXTIMINGS          100
#define DHT11               11
#define DHT22               22
#define AM2302              22

int initialized = 0;
unsigned long long last_read[32] = {};
float last_temperature[32] = {};
float last_humidity[32] = {};
int data[100];

void set_default_priority(void) {
	struct sched_param sched;
	memset(&sched, 0, sizeof(sched));
	// Go back to default scheduler with default 0 priority.
	sched.sched_priority = 0;
	sched_setscheduler(0, SCHED_OTHER, &sched);
}

inline void set_max_priority(void) {
	struct sched_param sched;
	memset(&sched, 0, sizeof(sched));
	// Use FIFO scheduler with highest priority for the lowest chance
	// of the kernel context switching.
	sched.sched_priority = sched_get_priority_max(SCHED_FIFO);
	sched_setscheduler(0, SCHED_FIFO, &sched);
}

unsigned long long getTime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  unsigned long long time = (unsigned long long)(tv.tv_sec)*1000 +
                            (unsigned long long)(tv.tv_usec)/1000;
  return time;
}

long readDHT(int type, int pin, float &temperature, float &humidity)
{
	int j = 0;
	int timeout;
	int bits[MAXTIMINGS];
  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  unsigned long long now = getTime();
  if (now - last_read[pin] < 2000) {
#ifdef VERBOSE
     printf("too early to read again pin %d: %llu\n", pin, now - last_read[pin]);
#endif
     temperature = last_temperature[pin];
     humidity = last_humidity[pin];
     return 0;
  } else {
     last_read[pin] = now + 420;
  }

	// set up as real-time scheduling as possible
	set_max_priority();

	pinMode (pin, OUTPUT);
	digitalWrite (pin, HIGH);
	usleep(10000);
	digitalWrite (pin, LOW);
	usleep(type == 11 ? 2500 : 800);
	digitalWrite (pin, HIGH);

  pinMode (pin, INPUT) ;

	for (timeout = 0; timeout < 100000 && digitalRead(pin) == 0; timeout++);
	if (timeout >= 100000) { set_default_priority(); return -3; }
	for (timeout = 0; timeout < 100000 && digitalRead(pin) == 1; timeout++);
	if (timeout >= 100000) { set_default_priority(); return -3; }

	// read data!
	for (j = 0; j < MAXTIMINGS; ++j) {
		for (timeout = 0; digitalRead(pin) == LOW && timeout <= 800; ++timeout);
		for (timeout = 0; digitalRead(pin) == HIGH && timeout <= 800; ++timeout);
		bits[j] = timeout;
		if (timeout > 800) break;
	}

	set_default_priority();

	int k = 0;
	int h = bits[0];
	for (int i = 1; i < j; i++) {
		if (h < bits[i]) h = bits[i];
	}

	#ifdef VERBOSE
	printf("j=%d, h=%d:\n", j, h);
	#endif
	for (int i = 1; i < j; i++) {
		data[k] <<= 1;
		if ((2*bits[i] - h) > 0) {
			data[k] |= 1;
			#ifdef VERBOSE
			printf("1 (%03d) ", bits[i]);
		} else {
			printf("0 (%03d) ", bits[i]);
			#endif
		}
		if (i % 8 == 0) {
			k++;
			#ifdef VERBOSE
			printf("\n");
			#endif
		}
	}

	#ifdef VERBOSE
	int crc = ((data[0] + data[1] + data[2] + data[3]) & 0xff);
	printf("\n=> %x %x %x %x (%x/%x) : %s\n",
		data[0], data[1], data[2], data[3], data[4],
		crc, (data[4] == crc) ? "OK" : "ERR");
	#endif

    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xff)))
    {
#ifdef VERBOSE
        printf("[Sensor type = %d] ", type);
#endif

        if (type == DHT11) {
#ifdef VERBOSE
          printf("Temp = %d C, Hum = %d %%\n", data[2], data[0]);
#endif
          temperature = data[2];
          humidity = data[0];
        }
        else if (type == DHT22)
        {
          float f, h;
          h = data[0] * 256 + data[1];
          h /= 10;

          f = (data[2] & 0x7F) * 256 + data[3];
          f /= 10.0;
          if (data[2] & 0x80) f *= -1;

#ifdef VERBOSE
          printf("Temp = %.1f C, Hum = %.1f %%\n", f, h);
#endif
          temperature = f;
          humidity = h;
        }
				else
				{
            return -2;
        }
    }
    else
    {
#ifdef VERBOSE
      printf("Unexpected data: j=%d: %d != %d + %d + %d + %d\n",
       j, data[4], data[0], data[1], data[2], data[3]);
#endif
      return -1;
    }

#ifdef VERBOSE
    printf("Obtained readout successfully.\n");
#endif

    // update last readout
    last_temperature[pin] = temperature;
    last_humidity[pin] = humidity;

    return 0;
}

int initialize()
{
  wiringPiSetup ();
  initialized = 1;
  memset(last_read, 0, sizeof(unsigned long long)*32);
  return 0;
}
