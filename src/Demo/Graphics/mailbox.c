/*
 * Access system mailboxes
 */
#include "mailbox.h"

/* Mailbox memory addresses */
static volatile unsigned int *MAILBOX0READ = (unsigned int *) (0x2000b880);
static volatile unsigned int *MAILBOX0STATUS = (unsigned int *) (0x2000b898);
static volatile unsigned int *MAILBOX0WRITE = (unsigned int *) (0x2000b8a0);

/* Bit 31 set in status register if the write mailbox is full */
#define MAILBOX_FULL 0x80000000

/* Bit 30 set in status register if the read mailbox is empty */
#define MAILBOX_EMPTY 0x40000000

unsigned int readmailbox(unsigned int channel)
{
  unsigned int val;

  if(channel > 15)
    return 0xFFFFFFFF;

  while(*MAILBOX0STATUS & MAILBOX_EMPTY)
    {
      /* wait for mailbox to be full */
    }

  val = *MAILBOX0READ;

  if((val & 15) == channel)
    return (val & 0xFFFFFFF0);
  else
    return 0xFFFFFFFF;
}

void writemailbox(unsigned int channel, unsigned int data)
{
  if(channel > 15)
    return;

  if(data & 0x000F)
    return;

  /* Wait for mailbox to be not full */
  while (*MAILBOX0STATUS & MAILBOX_FULL)
    {
      /* wait for mailbox to be empty */
    }

  *MAILBOX0WRITE = (data | channel);
}
