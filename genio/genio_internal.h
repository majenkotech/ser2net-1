/*
 *  ser2net - A program for allowing telnet connection to serial ports
 *  Copyright (C) 2001  Corey Minyard <minyard@acm.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SER2NET_GENIO_INTERNAL_H
#define SER2NET_GENIO_INTERNAL_H

#include <stddef.h>
#include "genio.h"

enum genio_type {
    GENIO_TYPE_TCP,
    GENIO_TYPE_UDP,
    GENIO_TYPE_STDIO
};

struct genio_functions {
    int (*write)(struct genio *net, unsigned int *count,
		 const void *buf, unsigned int buflen);

    int (*raddr_to_str)(struct genio *net, int *pos,
			char *buf, unsigned int buflen);

    socklen_t (*get_raddr)(struct genio *net,
			   struct sockaddr *addr, socklen_t addrlen);

    int (*open)(struct genio *net);

    int (*close)(struct genio *net, void (*close_done)(struct genio *net));

    void (*free)(struct genio *net);

    void (*set_read_callback_enable)(struct genio *net, bool enabled);

    void (*set_write_callback_enable)(struct genio *net, bool enabled);
};

/*
 * This structure represents a network connection, return from the
 * acceptor callback in genio_acceptor.
 */
struct genio {
    void *user_data;
    const struct genio_callbacks *cbs;

    const struct genio_functions *funcs;

    enum genio_type type;
};

struct genio_acceptor_functions {
    int (*startup)(struct genio_acceptor *acceptor);

    int (*shutdown)(struct genio_acceptor *acceptor,
		    void (*shutdown_done)(struct genio_acceptor *acceptor));

    void (*set_accept_callback_enable)(struct genio_acceptor *acceptor,
				       bool enabled);

    void (*free)(struct genio_acceptor *acceptor);
};

/*
 * This function handles accepts on network I/O code and calls back the
 * user for the new connection.
 */
struct genio_acceptor {
    void *user_data;
    const struct genio_acceptor_callbacks *cbs;

    const struct genio_acceptor_functions *funcs;

    enum genio_type type;
};

#define container_of(ptr, type, member) \
    ((type *)(((char *) ptr) - offsetof(type, member)))

struct opensocks
{
    int fd;
    int family;
};

/*
 * Open a set of sockets given the addrinfo list, one per address.
 * Return the actual number of sockets opened in nr_fds.  Set the
 * I/O handler to readhndlr, with the given data.
 *
 * Note that if the function is unable to open an address, it just
 * goes on.  It returns NULL if it is unable to open any addresses.
 * Also, open IPV6 addresses first.  This way, addresses in shared
 * namespaces (like IPV4 and IPV6 on INADDR6_ANY) will work properly
 */
struct opensocks *open_socket(struct selector_s *sel,
			      struct addrinfo *ai,
			      void (*readhndlr)(int, void *),
			      void (*writehndlr)(int, void *), void *data,
			      unsigned int *nr_fds,
			      void (*fd_handler_cleared)(int, void *));

void check_ipv6_only(int family, struct sockaddr *addr, int fd);

#endif /* SER2NET_GENIO_INTERNAL_H */