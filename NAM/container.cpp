#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <sstream>

#include "container.h"
#include "get_dsp.h"
#include "model_config.h"

namespace nam
{
namespace container
{

// =============================================================================
// ContainerModel
// =============================================================================

ContainerModel::ContainerModel(std::vector<Submodel> submodels, const double expected_sample_rate)
: DSP(1, 1, expected_sample_rate)
, _submodels(std::move(submodels))
{
  if (_submodels.empty())
    throw std::runtime_error("ContainerModel: no submodels provided");

  // Validate ordering and that final max_value covers 1.0
  for (size_t i = 1; i < _submodels.size(); ++i)
  {
    if (_submodels[i].max_value <= _submodels[i - 1].max_value)
      throw std::runtime_error("ContainerModel: submodels must be sorted by ascending max_value");
  }
  if (_submodels.back().max_value < 1.0)
    throw std::runtime_error("ContainerModel: last submodel max_value must be >= 1.0");

  // Validate all submodels have the same expected sample rate
  for (const auto& sm : _submodels)
  {
    double sr = sm.model->GetExpectedSampleRate();
    if (sr != expected_sample_rate && sr != NAM_UNKNOWN_EXPECTED_SAMPLE_RATE
        && expected_sample_rate != NAM_UNKNOWN_EXPECTED_SAMPLE_RATE)
    {
      std::stringstream ss;
      ss << "ContainerModel: submodel sample rate mismatch (expected " << expected_sample_rate << ", got " << sr << ")";
      throw std::runtime_error(ss.str());
    }
  }

  // Default to full size (last submodel)
  _active_index = _submodels.size() - 1;
}

void ContainerModel::process(NAM_SAMPLE** input, NAM_SAMPLE** output, const int num_frames)
{
  if (_crossfade_remaining <= 0)
  {
    _active_model().process(input, output, num_frames);
    return;
  }

  // Outgoing model: process in place on a copy of the input, since NAM process() is called in place.
  for (int i = 0; i < num_frames; ++i)
    _crossfade_output[i] = input[0][i];
  NAM_SAMPLE* previousChannels[1] = {_crossfade_output.data()};
  _submodels[_previous_index].model->process(previousChannels, previousChannels, num_frames);

  // Incoming submodel: process in place over the real buffer.
  _active_model().process(input, output, num_frames);

  for (int i = 0; i < num_frames && _crossfade_remaining > 0; ++i)
  {
    const double t = (double)(_crossfade_length - _crossfade_remaining) / (double)_crossfade_length;
    // The incoming model can emit NaN while it warms; fall back to the outgoing model for those samples.
    const NAM_SAMPLE incoming = std::isfinite(output[0][i]) ? output[0][i] : _crossfade_output[i];
    output[0][i] = _crossfade_output[i] * (1.0 - t) + incoming * t;
    --_crossfade_remaining;
  }
}

void ContainerModel::prewarm()
{
  for (auto& sm : _submodels)
    sm.model->prewarm();
}

void ContainerModel::Reset(const double sampleRate, const int maxBufferSize)
{
  DSP::Reset(sampleRate, maxBufferSize);
  for (auto& sm : _submodels)
    sm.model->Reset(sampleRate, maxBufferSize);

  // 30 ms crossfade, scratch buffers sized to the largest block we can be asked to process.
  _crossfade_length = std::max(1, (int)(0.030 * sampleRate));
  _crossfade_remaining = 0;
  _previous_index = _active_index;
  _crossfade_output.assign(maxBufferSize, (NAM_SAMPLE)0.0);
}

void ContainerModel::SetSlimmableSize(const double val)
{
  size_t new_index = _submodels.size() - 1;
  for (size_t i = 0; i < _submodels.size(); ++i)
  {
    if (val < _submodels[i].max_value)
    {
      new_index = i;
      break;
    }
  }

  // Skip the reset when the submodel is unchanged so dragging within a range does not glitch the audio.
  if (new_index == _active_index)
    return;

  // No reset here: Reset() re-runs prewarm() and spikes the audio thread, and the submodels are already warmed at load.
  // Just switch and let the crossfade cover the swap.
  _previous_index = _active_index;
  _active_index = new_index;
  _crossfade_remaining = _crossfade_length;
}

// =============================================================================
// Config / factory
// =============================================================================

std::unique_ptr<DSP> ContainerConfig::create(std::vector<float> weights, double sampleRate)
{
  (void)weights; // Container has no top-level weights

  auto submodels_json = raw_config["submodels"];
  if (!submodels_json.is_array() || submodels_json.empty())
    throw std::runtime_error("SlimmableContainer: 'submodels' must be a non-empty array");

  std::vector<Submodel> submodels;
  submodels.reserve(submodels_json.size());

  for (const auto& entry : submodels_json)
  {
    double max_val = entry.at("max_value").get<double>();
    const auto& model_json = entry.at("model");

    // Each submodel is a full NAM model spec (has architecture, config, weights, etc.)
    auto dsp = get_dsp(model_json);

    submodels.push_back({max_val, std::move(dsp)});
  }

  return std::make_unique<ContainerModel>(std::move(submodels), sampleRate);
}

std::unique_ptr<ModelConfig> create_config(const nlohmann::json& config, double sampleRate)
{
  auto c = std::make_unique<ContainerConfig>();
  c->raw_config = config;
  c->sample_rate = sampleRate;
  return c;
}

// Auto-register
static ConfigParserHelper _register_SlimmableContainer("SlimmableContainer", create_config);

} // namespace container
} // namespace nam
