#pragma once

#include <atomic>
#include <memory>
#include <stdexcept>
#include <vector>

#include "dsp.h"
#include "model_config.h"
#include "slimmable.h"

namespace nam
{
namespace container
{

struct Submodel
{
  double max_value;
  std::unique_ptr<DSP> model;
};

/// \brief A container model that holds multiple submodels at different sizes
///
/// SetSlimmableSize selects the active submodel based on the max_value thresholds.
/// Each submodel covers values up to (but not including) its max_value.
/// The last submodel is the fallback for values at or above the last threshold.
class ContainerModel : public DSP, public SlimmableModel
{
public:
  /// \brief Constructor
  /// \param submodels Vector of submodels sorted by max_value ascending
  /// \param expected_sample_rate Expected sample rate in Hz
  ContainerModel(std::vector<Submodel> submodels, const double expected_sample_rate);

  void process(NAM_SAMPLE** input, NAM_SAMPLE** output, const int num_frames) override;
  void prewarm() override;
  void Reset(const double sampleRate, const int maxBufferSize) override;
  void SetPrewarmOnReset(const bool prewarmOnReset) override;
  void SetSlimmableSize(const double val) override;
  int GetPrewarmSamples() override;

private:
  std::vector<Submodel> _submodels;
  std::atomic<size_t> _active_index{0};
  size_t _previous_index = 0;
  // Number of samples over which a submodel switch is crossfaded, and how many remain in the current fade.
  int _crossfade_length = 0;
  int _crossfade_remaining = 0;
  // Pre-allocated scratch for the fading-out submodel's output during a crossfade.
  std::vector<NAM_SAMPLE> _crossfade_output;

  DSP& _active_model() { return *_submodels[_active_index.load(std::memory_order_acquire)].model; }
};

// Config / registration

struct ContainerConfig : public ModelConfig
{
  nlohmann::json raw_config;
  double sample_rate;

  std::unique_ptr<DSP> create(std::vector<float> weights, double sampleRate) override;
};

std::unique_ptr<ModelConfig> create_config(const nlohmann::json& config, double sampleRate);

} // namespace container
} // namespace nam
