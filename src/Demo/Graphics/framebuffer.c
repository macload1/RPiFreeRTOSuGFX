#include "framebuffer.h"
#include "mailbox.h"


FrameBufferDescription FrameBufferInfo __attribute__((aligned (16))) =
  { 480, 272, 480, 272, 0, 16, 0, 0, 0, 0 };
//FrameBufferDescription FrameBufferInfo __attribute__((aligned (32))) =
//  { 480, 272, 480, 272, 0, 32, 0, 0, 0, 0 };


/* Initialise the framebuffer */
unsigned int fb_init(unsigned int width, unsigned int height, unsigned int bitDepth)
{
  if(width > 4096)
    return 0;
  if(height > 4096)
    return 0;
  if(bitDepth > 32)
    return 0;

  FrameBufferInfo.width = width;
  FrameBufferInfo.height = height;
  FrameBufferInfo.vWidth = width;
  FrameBufferInfo.vHeight = height;
  FrameBufferInfo.bitDepth = bitDepth;
  
  unsigned int var;
  unsigned int physical_mb;

  physical_mb = 0x40000000 + (unsigned int) &FrameBufferInfo;

  writemailbox(1, physical_mb);

  var = readmailbox(1);

  if(var == 0)
    return 1;
  else
    return 0;
}

