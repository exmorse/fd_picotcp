extern char vde_switch_name[256];

extern unsigned char macaddr[6];
extern int is_mac_changed;
extern char device_name[8];
extern struct pico_device *pico_dev;

extern char ipv4_mask_string[16];
extern char ipv6_mask_string[100];

extern void handle_wakeup(uint16_t ev, struct pico_socket* socket);

extern fd_elem* fd_table[FD_TABLE_SIZE];

