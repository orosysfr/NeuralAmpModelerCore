// GENOME single-translation-unit aggregator for NAM core v0.5.1.
// This file is GENOME-specific; it does not exist upstream.
//
// Including all NAM .cpp files into one TU keeps the .jucer project from
// having to enumerate each source individually and avoids ABI surprises from
// inline functions appearing in multiple TUs.

#if defined(_MSC_VER)
  #pragma warning(push)
  #pragma warning(disable : 4100 4244 4267 4305 4456 4458)
#endif

#include "NAM/activations.cpp"
#include "NAM/container.cpp"
#include "NAM/conv1d.cpp"
#include "NAM/convnet.cpp"
#include "NAM/dsp.cpp"
#include "NAM/get_dsp.cpp"
#include "NAM/lstm.cpp"
#include "NAM/ring_buffer.cpp"
#include "NAM/util.cpp"
#include "NAM/wavenet/model.cpp"
#include "NAM/wavenet/slimmable.cpp"
#if defined(NAM_ENABLE_A2_FAST)
  #include "NAM/wavenet/a2_fast.cpp"
#endif

/**
 * Copyright Orosys 2026
 */

#if defined(_MSC_VER)
  #pragma warning(pop)
#endif
