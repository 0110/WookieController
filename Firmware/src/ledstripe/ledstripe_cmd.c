/*
 *  ledstripe_cmd.c
 *
 *  Created by ollo on 14.11.2014.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include "ledstripe_cmd.h"
#include "ledstripe.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "shell.h"

void cmd_ledstripe_modify(BaseSequentialStream *chp, int argc, char *argv[]) {
#define LEDSTRIPE_USAGE_HELP "Possible commands are:\r\nshow\tprint content\r\nwrite (offset) (value)\r\nfill (start offset) (end) (value)\r\nfill (start offset) (hex to send, without spaces!)\r\n"

	if (argc < 1) {
		chprintf(chp, "Usage <command> (parameter)\r\n");
		chprintf(chp, LEDSTRIPE_USAGE_HELP);
		return;
	}

	if (argc >= 1) {
		if (strcmp(argv[0], "write") == 0) {
			if (argc < 3) {
				chprintf(chp, "Usage: led write (offset, starting with '1') (value)\r\n");
			} else {
				int offset = atoi(argv[1]);
				int value = atoi(argv[2]);

				/*Offset adaption: */
				offset--;

				ledstripe_fb[offset] = value;
				chprintf(chp, "Set LEDSTRIPE at %d with 0x%2X (%d)\r\n", offset,
						value, value);
			}
		} else if (strcmp(argv[0], "fill") == 0) {
			if (argc < 3) {
				chprintf(chp,
						"Usage: led fill (start offset) (end) (value)\r\n");
				chprintf(chp,
						"Usage: led fill (start offset) (hex values, without spaces)\r\n");
				return;
			} else if (argc < 4) {
				char tmpHex[3];
				uint8_t* tmpLEDSTRIPE;
				int i = 0;
				int offset = atoi(argv[1]);
				int strLength = strlen(argv[2]);
				int length = strLength / 2; /* HEX values -> therefore divided by two) */
				long value;
				char* end;
				if (length <= 0 || (offset + length) >= (LEDSTRIPE_MAXIMUM * LEDSTRIPE_COLORS_PER_LED)
						|| (strLength % 2 != 0)) {
					chprintf(chp,
							"Could not extract HEX value, maximum of %d (got %d, %s, must be even)\r\n",
							LEDSTRIPE_MAXIMUM, (offset + length),
							(strLength % 2) ? "odd" : "even");
					chprintf(chp,
							"Usage: ledstripe fill (start offset) (hex values, without spaces)\r\n");
					return;
				}

				chprintf(chp, "Updating %d bytes beginning at %d\r\n", length,
						offset);
				/* allocate working buffer */
				tmpLEDSTRIPE = chHeapAlloc(NULL, length);
				if (tmpLEDSTRIPE == 0) {
					chprintf(chp, "Could allocate memory\r\n");
					return;
				}

				/* valid data to write on the buffer */
				for (i = 0; i < length; i++) {
					memcpy(tmpHex, (argv[2] + (i * 2)), 2);
					tmpHex[2] = 0; /* mark the last as end */

					value = strtol(tmpHex, &end, 16);
					tmpLEDSTRIPE[i] = (uint8_t) value;
					if (*end) {
						chprintf(chp, "Could not extract '%s'\r\n", tmpHex);
						chHeapFree(tmpLEDSTRIPE);
						return;
					}
				}

				/* Update the LEDSTRIPE buffer */
				memcpy(&(ledstripe_fb[offset]), tmpLEDSTRIPE, length);

				/* clean the memory again */
				chHeapFree(tmpLEDSTRIPE);
			} else {
				int offset = atoi(argv[1]);
				int end = atoi(argv[2]);
				int value = atoi(argv[3]);
				int length;

				/* swap, if the user cannot determine the lower number */
				if (end < offset) {
					int tmp = offset;
					offset = end;
					end = tmp;
				}

				length = end - offset;
				for (; offset < end; offset++) {
					ledstripe_fb[offset] = value;
				}
				chprintf(chp, "Filled LEDSTRIPE with %2X (%d times)\r\n", value,
						length);
			}
		} else if (strcmp(argv[0], "show") == 0) {
			int i, width, height = 0;

			// Get the LED strip from the physical dimension
			width = LEDSTRIPE_MAXIMUM;
			height = 1;

			if (width == 0 && height == 0) { /* Display the complete LEDSTRIPE universe */
				for (i = 0; i < LEDSTRIPE_MAXIMUM; i++) {
					chprintf(chp, "%.2X%", ledstripe_fb[i]);
				}
			} else { /* We have valid information! Let's display the wall on the shell */
				chprintf(chp, "LEDSTRIPE is filled with %d x %d pixel\r\n",
						width, height);
				for (i = 0; i < width * height; i++) {
					chprintf(chp, "%.2X%.2X%.2X|",
							ledstripe_fb[i * LEDSTRIPE_COLORS_PER_LED + 0],
							ledstripe_fb[i * LEDSTRIPE_COLORS_PER_LED + 1],
							ledstripe_fb[i * LEDSTRIPE_COLORS_PER_LED + 2]);

					/* generate a new line after each row */
					if ((i + 1) % width == 0) {
						chprintf(chp, "\r\n");
					}
				}
				chprintf(chp, "\r\n");
			}
		} else if (strcmp(argv[0], "demo") == 0) {
			int i;
			int offset = 2;
			// Set the color to manipulate
			if (argc >= 2) {
				offset = atoi(argv[1]);
			} else {
				chprintf(chp, "Options are:\r\n0: red\r\n1: green\r\n2: blue\r\n");
			}

			for(i=0; i < (LEDSTRIPE_MAXIMUM * LEDSTRIPE_COLORS_PER_LED); i++)
			{
				if (i % 3 == offset)
				{
					ledstripe_fb[i] = (i * 10) % 255;
				}
			}
		} else if (strcmp(argv[0], "clear") == 0) {
			int i;

			for(i=0; i < (LEDSTRIPE_MAXIMUM * LEDSTRIPE_COLORS_PER_LED); i++)
			{
				ledstripe_fb[i] = 0;
			}
		} else if (strcmp(argv[0], "help") == 0) {
			chprintf(chp, "Possible commands are:\r\n"
			LEDSTRIPE_USAGE_HELP);
		}
	}

}
