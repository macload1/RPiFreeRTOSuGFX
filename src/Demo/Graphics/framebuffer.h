#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

typedef struct FrameBufferDescription {
  unsigned int width;
  unsigned int height;
  unsigned int vWidth;
  unsigned int vHeight;
  unsigned int pitch;
  unsigned int bitDepth;
  unsigned int x;
  unsigned int y;
  void* pointer;
  unsigned int size;
 } FrameBufferDescription;

extern FrameBufferDescription FrameBufferInfo;

unsigned int fb_init(unsigned int width, unsigned int height, unsigned int bitDepth);

#endif	/* FRAMEBUFFER_H */
