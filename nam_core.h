// GENOME aggregator header for NAM core (sdatkinson/NeuralAmpModelerCore).
// This file is GENOME-specific; it does not exist upstream.
//
// Includes the NAM public headers and exposes the upstream `nam::` namespace
// under the legacy alias `namcore::` used throughout GENOME source.
//
// Assumes Eigen and nlohmann/json are reachable via the project's existing
// header search paths (modules/eigen and the bundled
// Dependencies/nlohmann/json.hpp shipped with this repo).

#pragma once

#include "NAM/version.h"
#include "NAM/dsp.h"
#include "NAM/get_dsp.h"
#include "NAM/activations.h"
#include "NAM/conv1d.h"
#include "NAM/linear.h"
#include "NAM/convnet.h"
#include "NAM/lstm.h"
#include "NAM/util.h"
#include "NAM/container.h"
#include "NAM/model_config.h"
#include "NAM/registry.h"
#include "NAM/ring_buffer.h"
#include "NAM/slimmable.h"
#include "NAM/film.h"
#include "NAM/gating_activations.h"
#include "NAM/wavenet/model.h"
#include "NAM/wavenet/slimmable.h"
#include "NAM/wavenet/a2_fast.h"

/**
 * nam_core
 * Copyright Orosys 2026
 *
 * Author: Umut M. Dabager (umut@arteradsp.com)
 * Creation date: 2026-04-27
 */

namespace namcore = nam;
