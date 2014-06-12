/*
 * xblanker.c:
 *
 * Copyright (c) 2009 James McKenzie <20@madingley.org>,
 * All rights reserved.
 *
 */

/*
 * Copyright (c) 2014 Citrix Systems, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


static char rcsid[] =
"$Id: xblanker.c,v 1.2 2009/07/31 12:59:40 jamesmck Exp $";

/*
 * $Log: xblanker.c,v $
 * Revision 1.2  2009/07/31 12:59:40  jamesmck
 * *** empty log message ***
 *
 * Revision 1.1  2009/07/31 11:58:28  jamesmck
 * *** empty log message ***
 *
 */



#include "project.h"

struct xs_handle *xsh;

#define XS_ENTER_PATH "switcher/enter"
#define XS_LEAVE_PATH "switcher/leave"
#define XS_RESOLUTION "attr/desktopDimensions"

#define ENTER 1
#define LEAVE 0

int xenbus_read(int enter)
{
    unsigned int len=0;
    char cbuf[128];
    char *buf= xs_read(xsh, XBT_NULL, enter ? XS_ENTER_PATH : XS_LEAVE_PATH, &len);

    if (!buf) return -1;
    if (!len) return -1;

    if (len>=sizeof(cbuf))
        len=sizeof(cbuf)-1;

    memcpy(cbuf,buf,len);
    cbuf[len]=0;

    return atoi(cbuf);
}

void
blank (void)
{
}

void
unblank (void)
{
}

void xenbus_write(int enter, int v)
{
    unsigned int len;
    char buf[128];

    len=sprintf(buf,"%d",v);
    xs_write(xsh,XBT_NULL, enter ? XS_ENTER_PATH : XS_LEAVE_PATH,buf,len);
}

/* This function updates the screen resolution in xenstore */
void update_resolution(int width, int height)
{
    unsigned int len;
    char buf[128];

    len = sprintf(buf, "%d %d", width, height);
    xs_write(xsh, XBT_NULL, XS_RESOLUTION, buf, len);
}

/* First time it's called, this function initializes stuffs and
   updates the resolution to the current one.
   On subsequent calls, it just checks if anything changed and updates
   the resolution accordingly.
   TODO: handle multiple displays and fill activeAdapter nodes
   accordingly. */
void check_new_resolution(void)
{
  static Display *display = NULL;

  if (display == NULL)
    {
      int screen;
      Window window;
      XWindowAttributes xwAttr;

      /* FIXME: we want a list of displays, associated with adapters,
	 to be able to update xenstore correctly... */
      while (!(display = XOpenDisplay(":0")))
	sleep(1);
      if (display == 0)
	{
	  fprintf(stderr, "Failed to open display\n");
	  return;
	}
      screen = DefaultScreen(display);
      window = DefaultRootWindow(display);
      if (window < 0)
	{
	  fprintf(stderr, "Failed to get root window\n");
	  return;
	}

      XGetWindowAttributes(display, window, &xwAttr);
      update_resolution(xwAttr.width, xwAttr.height);

      XSelectInput (display, window, ExposureMask | StructureNotifyMask);
      XMapWindow (display, window);

      return;
    }

  while (XPending(display))
    {
      XEvent e;

      XNextEvent(display, &e);
      if (e.type == ConfigureNotify)
	{
	  XConfigureEvent xce;

	  xce = e.xconfigure;
	  update_resolution(xce.width, xce.height);
	}
    }
}

    int
main (int argc, char *argv[])
{
    struct timeval tv = { 0 };
    fd_set rfds;
    int fd = 0;

    int enter_state=0;
    int leave_state=0;

    struct pci_id_match m = { 0 };
    struct pci_device_iterator *iter;
    struct pci_device *device;
    int n = 0;
    char tmp[256] = { 0 };
    char *vendor_name;

    xsh=xs_domain_open();
    if (!xsh) return 1;

    m.vendor_id = PCI_MATCH_ANY;
    m.device_id = PCI_MATCH_ANY;
    m.subvendor_id = PCI_MATCH_ANY;
    m.subdevice_id = PCI_MATCH_ANY;
    m.device_class = 0x30000;
    m.device_class_mask = 0x00ff0000;

    pci_system_init ();

    iter = pci_id_match_iterator_create(&m);
    while ((device = pci_device_next(iter)))
      {
	pci_device_probe(device);
	if (pci_device_has_kernel_driver(device))
	  {
	    sprintf(tmp, "display/activeAdapter/%d", n);
	    vendor_name = pci_device_get_vendor_name(device);
	    xs_write(xsh, XBT_NULL, tmp, vendor_name, strlen(vendor_name));
	    n++;
	  }
      }
    sprintf(tmp, "%d", n);
    xs_write(xsh, XBT_NULL, "display/activeAdapter", tmp, strlen(tmp));
    pci_iterator_destroy(iter);

    check_new_resolution();

    FD_ZERO (&rfds);
    for (;;)
    {
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int	k=xenbus_read(ENTER);
        switch(k) {
            case 0:
            case 1:
	    case 2:
                enter_state = k;
        }
        k=xenbus_read(LEAVE);
        switch (k) {
            case 0:
            case 1:
	    case 2:
                leave_state = k;
        }


        if (leave_state == 1)
        {
            fprintf(stderr, "blank\n");
            blank();
        }
        if (enter_state == 1)
        {
            fprintf(stderr, "unblank\n");
            unblank();
        }

        if (leave_state)
            xenbus_write(LEAVE, 2);
        if (enter_state)
            xenbus_write(ENTER, 2);

        while (tv.tv_sec || tv.tv_usec)
        {
            if (select (fd + 1, &rfds, NULL, NULL, &tv) == 1)
            {
            }
        }

	check_new_resolution();
    }
    return 0;
}
