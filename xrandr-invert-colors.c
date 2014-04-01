/*
   This is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Redshift is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Redshift.  If not, see <http://www.gnu.org/licenses/>.

   Copyright (c) 2013  Zoltan Padrah <zoltan_padrah@users.sf.net>
 */

#include "gamma_randr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _(x) x

int invert_colors(randr_state_t *state);
int parse_arguments(int argc, const char *argv[], randr_state_t *state);

int main(int argc, const char *argv[]){
	randr_state_t state;

	if(randr_init(&state) < 0){
		printf("error while init\n");
		return 1;
	}
	if(randr_start(&state) < 0){
		printf("error while start\n");
		return 1;
	}

	if(parse_arguments(argc, argv, &state) < 0) {
		printf("error while parsing arguments\n");
		return 2;
	}

	if(invert_colors(&state) < 0){
		printf("error while inverting\n");
		// no return!
	}

#if TESTING_RESTORE_AFTER_SET
	sleep(3);

	randr_restore(&state);
#endif

	randr_free(&state);

	return 0;
}

void display_usage(const char *program_name);
void display_version(const char *program_name);

int parse_arguments(int argc, const char *argv[], randr_state_t *state) {
	/* first argument is the program name, skip it */
	int current_arg;
	for (current_arg = 1; current_arg < argc; current_arg++) {
		if (strcmp(argv[current_arg], "-s") == 0) {
			/* next argument should be the screen number */
			if (argc - current_arg > 1) {
				current_arg++;
				int screen_nr;
				char extra_junk[4];
				if(sscanf(argv[current_arg], "%d%c", &screen_nr, extra_junk) != 1) {
					printf("Screen number expected after '-s', instead of '%s'. Aborting.\n", 
						argv[current_arg]);
					return -1;
				}
				printf("screen_nr = %d\n", screen_nr);
				state->crtc_num = screen_nr;
			} else {
				printf("Screen number expected after '-s' option. Aborting.\n");
				return -3;
			}
		} else if (strcmp(argv[current_arg], "-h") == 0) {
			display_usage(argv[0]);
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[current_arg], "--help") == 0) {
			display_usage(argv[0]);
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[current_arg], "--version") == 0) {
			display_version(argv[0]);
			exit(EXIT_SUCCESS);
		} else {
			printf("Unknown argument: '%s'. Aborting.\n", argv[current_arg]);
			return -4;
		}
	}
	return 0;
}

void display_usage(const char * program_name) {
	printf("Usage: %s [-s screen_number]\n", program_name);
	printf("       %s -h\n", program_name);
	printf("       %s --help\n", program_name);
	printf("       %s --version\n", program_name);
	printf("Simple utility that inverts colors on all screens, using XrandR.\n");
	printf("When no parameter is given, it inverts colors on all screens; "
		"if the screen_number parameter is given, then it inverts colors on "
		"that particular XRandR screen. See 'xrandr -q' for the list of screens.\n");
}

void display_version(const char * program_name) {
	printf("%s: version 0.01\n", program_name);
}

int invert_colors_for_crtc(randr_state_t *state, int crtc_num);

int invert_colors(randr_state_t *state){
	int r;

	/* If no CRTC number has been specified,
	   set temperature on all CRTCs. */
	if (state->crtc_num < 0) {
		for (unsigned int i = 0; i < state->crtc_count; i++) {
			r = invert_colors_for_crtc(state, i);
			if (r < 0) return -1;
		}
	} else {
		return invert_colors_for_crtc(state, state->crtc_num);
	}

	return 0;
}

void copy_inverted_ramps(const randr_crtc_state_t *crtc_status,
						 uint16_t *r, uint16_t *g, uint16_t *b);


int invert_colors_for_crtc(randr_state_t *state, int crtc_num){

		xcb_generic_error_t *error;

	if (crtc_num >= (int)state->crtc_count || crtc_num < 0) {
		fprintf(stderr, _("CRTC %d does not exist. "),
			state->crtc_num);
		if (state->crtc_count > 1) {
			fprintf(stderr, _("Valid CRTCs are [0-%d].\n"),
				state->crtc_count-1);
		} else {
			fprintf(stderr, _("Only CRTC 0 exists.\n"));
		}

		return -1;
	}

	xcb_randr_crtc_t crtc = state->crtcs[crtc_num].crtc;
	unsigned int ramp_size = state->crtcs[crtc_num].ramp_size;

	/* Create new gamma ramps */
	uint16_t *gamma_ramps = malloc(3*ramp_size*sizeof(uint16_t));
	if (gamma_ramps == NULL) {
		perror("malloc");
		return -1;
	}

	uint16_t *gamma_r = &gamma_ramps[0*ramp_size];
	uint16_t *gamma_g = &gamma_ramps[1*ramp_size];
	uint16_t *gamma_b = &gamma_ramps[2*ramp_size];

	copy_inverted_ramps(&(state->crtcs[crtc_num]), gamma_r, gamma_g, gamma_b);

	/* Set new gamma ramps */
	xcb_void_cookie_t gamma_set_cookie =
		xcb_randr_set_crtc_gamma_checked(state->conn, crtc,
						 ramp_size, gamma_r,
						 gamma_g, gamma_b);
	error = xcb_request_check(state->conn, gamma_set_cookie);

	free(gamma_ramps);

	if (error) {
		fprintf(stderr, _("`%s' returned error %d\n"),
			"RANDR Set CRTC Gamma", error->error_code);
		return -1;
	}
	return 0;
}


void copy_inverted_ramps(const randr_crtc_state_t *crtc_status,
						 uint16_t *r, uint16_t *g, uint16_t *b){
	unsigned int ramp_size = crtc_status->ramp_size;
	unsigned i;
	for(i = 0; i < ramp_size; i++){
		r[ramp_size - 1 - i] = crtc_status->saved_ramps[i + 0 * ramp_size];
	}
	for(i = 0; i < ramp_size; i++){
		g[ramp_size - 1 - i] = crtc_status->saved_ramps[i + 1 * ramp_size];
	}
	for(i = 0; i < ramp_size; i++){
		b[ramp_size - 1 - i] = crtc_status->saved_ramps[i + 2 * ramp_size];
	}
}
