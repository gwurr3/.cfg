#include <X11/Xlib.h>
#include <X11/Xatom.h>

SV* getfocusedwindowname() {
				Display *display;
				Window focus;
				int revert;
				display = XOpenDisplay(NULL);
				XGetInputFocus(display, &focus, &revert);

				// XGetInputFocus is weird. see https://tronche.com/gui/x/xlib/input/XGetInputFocus.html
				if ( focus == PointerRoot || focus == None ) { // is there even a focused window?
		XCloseDisplay(display);
		return newSVpvf(" ");
				} else {
								char *window_name = NULL;
								XFetchName(display, focus, &window_name);

								if (window_name) {
			XCloseDisplay(display);
			return newSVpvf( " %s ", window_name);
												// If there isn't a simple WM_NAME, we have to do a whole lot of shit:
								} else {
												Atom prop_type, prop_req_type, da;
												int di;
												int status;
												unsigned char *prop_ret = NULL;
												unsigned long dl;

												prop_type = XInternAtom(display, "_NET_WM_NAME", False);
												prop_req_type = XInternAtom(display ,"UTF8_STRING",False);

												status = XGetWindowProperty(display, focus, prop_type, 0L, sizeof (Atom), 
																False, prop_req_type, &da, &di, &dl, &dl, &prop_ret);

												if (status == Success && prop_ret) {
				XCloseDisplay(display);
				return newSVpvf( " %s ", prop_ret);
												} else {
				XCloseDisplay(display);
				return newSVpvf( " NULL ");
												}
								}
				}
}




SV* get_load(){

		double loadavg[3];

if (getloadavg(loadavg, 3) == -1)
		err(1, "getloadavg");

//return newSVpvf("%.1f %.1f %.1f", loadavg[0] , loadavg[1] , loadavg[2]);
return newSVpvf("%.1f", loadavg[0]);

}





#ifdef OBSDYES
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/sensors.h>

#include <errno.h>


int64_t get_sensor_value(struct sensordev *snsrdev, char* searchname){
	// this function takes a poiner to a sensordev struct and a searchname string
	// if it finds a match for the desc of a sensor, it returns the value.
	// if not, returns 0

	struct sensor	 sensor;
	size_t		 slen = sizeof(sensor);
	int		 mib[5], numt;
	enum sensor_type type;

	mib[0] = CTL_HW;
	mib[1] = HW_SENSORS;
	mib[2] = snsrdev->num;

	for (type = 0; type < SENSOR_MAX_TYPES; type++) {
		mib[3] = type;
		for (numt = 0; numt < snsrdev->maxnumt[type]; numt++) {
			mib[4] = numt;
			if (sysctl(mib, 5, &sensor, &slen, NULL, 0) == -1) {
				if (errno != ENOENT)
					warn("sysctl");
				continue;
			}
			//printf("----- desc: %s , numt: %d , cur value: %d	 \n", sensor.desc, sensor.numt, sensor.value);
			if (strcmp(sensor.desc,searchname) == 0) {
				return sensor.value;
			}


		}
	}
	return 0;
}


struct sensordev* find_sensordev(char* searchname){
	// this function returns a pointer to a sensordev if its name matches searchname
	// if it finds nothing, returns NULL
	// ex name would be "acpibat0"

	struct sensordev* sensordev = malloc(sizeof(struct sensordev));
	size_t		 sdlen = sizeof(struct sensordev);
	int		 mib[3], dev;

	mib[0] = CTL_HW;
	mib[1] = HW_SENSORS;

	for (dev = 0; ; dev++) {
		mib[2] = dev;
		if (sysctl(mib, 3, sensordev, &sdlen, NULL, 0) == -1) {
			if (errno == ENXIO)
				continue;
			if (errno == ENOENT)
				break;
			warn("sysctl");
		}
		//printf("device: %s -- num: %d has %d sensors: \n", sensordev->xname , sensordev->num, sensordev->sensors_count);
		if (strcmp(sensordev->xname,searchname) == 0) {
			return sensordev;
		}

	}
	free(sensordev);
	return NULL;
}



int64_t sensor_by_name(char* devname, char* sensordesc){
	int64_t val = 0;
	struct sensordev* mydev = find_sensordev(devname);
	if (mydev != NULL) {
		val = get_sensor_value(mydev,sensordesc);
		free(mydev);
		return val;
	}
	return 0;
}


SV* get_batt_percent_openbsd() {
	int64_t remain = sensor_by_name("acpibat0", "remaining capacity");
	int64_t total = sensor_by_name("acpibat0", "last full capacity");
	float perc = (100.0f * remain) / total;
	return newSVpvf( "%.1f", perc);
}

SV* get_cpu_temp_thinkpad() {
				int64_t temp = sensor_by_name("itherm0", "Core 1");
				float output = temp / 10000000.0f ;
				// shift this 7 decimal places
	return newSVpvf( "%.1f", output);
}	 

#endif


#ifdef LINUXYES
#include <alsa/asoundlib.h>


SV* get_vol_alsa(SV* channelname)
{
		long min, max, outvol;
		snd_mixer_t *handle;
		snd_mixer_selem_id_t *sid;
		const char *card = "default";
		//	const char *selem_name = "PCM";

		snd_mixer_open(&handle, 0);
		snd_mixer_attach(handle, card);
		snd_mixer_selem_register(handle, NULL, NULL);
		snd_mixer_load(handle);

		snd_mixer_selem_id_alloca(&sid);
		snd_mixer_selem_id_set_index(sid, 0);
		//snd_mixer_selem_id_set_name(sid, selem_name);
		snd_mixer_selem_id_set_name(sid, SvPV(channelname, PL_na));
		snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

		if(elem != NULL) {
				snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
		snd_mixer_selem_get_playback_volume(elem, 0, &outvol);
		} else {
				outvol = 0;
				max =0;
		}

		//snd_mixer_selem_id_free(sid);
		snd_mixer_close(handle);

		float theoutput = (100.0f * outvol) / max;

		return newSVpvf("%.1f", theoutput);

}



#endif
