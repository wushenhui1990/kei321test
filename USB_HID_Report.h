#ifndef __USB_HID_REPORT_H__
#define __USB_HID_REPORT_H__

typedef struct report_buff_st
{
	unsigned char*	rp_buff;
	unsigned int	rp_len;
}rp_buff_st;


void fill_report_packet( unsigned char report_id ,rp_buff_st*rp_buff)  reentrant;
void send_packet (unsigned char ReportID);

#endif