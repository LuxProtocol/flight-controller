#pragma once

#define MAX_FRAME_BYTES (256 * 1024)

// ---- Frame -----------------------------------------------------------------
struct Frame {
  uint8_t *msg = nullptr;      // [u32 jsonLen][json][jpeg]
  size_t   msgLen = 0;
  size_t   jsonLen = 0;
  size_t   jpegOff = 0;
  size_t   jpegLen = 0;
  uint32_t id = 0;
  ~Frame() { free(msg); }
};
using FramePtr = std::shared_ptr<Frame>;

static FramePtr latest;                         // touched only from async_tcp task
static uint32_t frameCounter = 0;
static uint32_t framesIn = 0;

static uint8_t *allocBuf(size_t n) {
  uint8_t *p = (uint8_t *)heap_caps_malloc(n, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!p) p = (uint8_t *)malloc(n);
  return p;
}
