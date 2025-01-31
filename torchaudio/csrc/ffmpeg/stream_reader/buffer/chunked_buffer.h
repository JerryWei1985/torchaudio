#pragma once
#include <torchaudio/csrc/ffmpeg/ffmpeg.h>
#include <torchaudio/csrc/ffmpeg/stream_reader/buffer.h>

namespace torchaudio {
namespace ffmpeg {

//////////////////////////////////////////////////////////////////////////////
// Chunked Buffer Implementation
//////////////////////////////////////////////////////////////////////////////
// Common to both audio and video
class ChunkedBuffer : public Buffer {
 protected:
  ChunkedBuffer(int frames_per_chunk, int num_chunks);

  // Each AVFrame is converted to a Tensor and stored here.
  std::deque<torch::Tensor> chunks;

  // The number of frames to return as a chunk
  // If <0, then user wants to receive all the frames
  const int64_t frames_per_chunk;
  // The numbe of chunks to retain
  const int64_t num_chunks;
  // The number of currently stored chunks
  // For video, one Tensor corresponds to one frame, but for audio,
  // one Tensor contains multiple samples, so we track here.
  int64_t num_buffered_frames = 0;

 public:
  bool is_ready() const override;
  void flush() override;
};

class ChunkedAudioBuffer : public ChunkedBuffer {
  void push_tensor(torch::Tensor frame);

 public:
  ChunkedAudioBuffer(int frames_per_chunk, int num_chunks);

  void push_frame(AVFrame* frame) override;
  c10::optional<torch::Tensor> pop_chunk() override;
};

class ChunkedVideoBuffer : public ChunkedBuffer {
  const torch::Device device;
  void push_tensor(const torch::Tensor& frame);

 public:
  ChunkedVideoBuffer(
      int frames_per_chunk,
      int num_chunks,
      const torch::Device& device);

  void push_frame(AVFrame* frame) override;
  c10::optional<torch::Tensor> pop_chunk() override;
};

} // namespace ffmpeg
} // namespace torchaudio
