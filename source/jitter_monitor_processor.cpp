// Copyright(c) 2024 rehans.

#include "jitter_monitor_processor.h"
#include "jitter_monitor_cids.h"
#include "jitter_monitor_param_ids.h"
#include <chrono>

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"

using namespace Steinberg;

namespace Steinberg::jitter_monitoring {

//------------------------------------------------------------------------
namespace {

//------------------------------------------------------------------------
auto update_jitter(const Vst::ProcessData& data,
                   jittering::State& jitter) -> jittering::State
{
    return jittering::update(data.processContext->sampleRate, data.numSamples,
                             jitter);
}

//------------------------------------------------------------------------
auto update_parameters(Vst::ProcessData& data, jittering::State& jitter) -> void
{
    if (!data.outputParameterChanges)
        return;

    int32 index            = 0;
    auto param_value_queue = data.outputParameterChanges->addParameterData(
        JitterParamIds::kJitterCurrent, index);
    if (param_value_queue)
    {
        param_value_queue->addPoint(0, jitter.current, index);
    }

    param_value_queue = data.outputParameterChanges->addParameterData(
        JitterParamIds::kJitterMin, index);
    if (param_value_queue)
    {
        param_value_queue->addPoint(0, jitter.min, index);
    }

    param_value_queue = data.outputParameterChanges->addParameterData(
        JitterParamIds::kJitterMax, index);
    if (param_value_queue)
    {
        param_value_queue->addPoint(0, jitter.max, index);
    }

    param_value_queue = data.outputParameterChanges->addParameterData(
        JitterParamIds::kJitterMeanValue, index);
    if (param_value_queue)
    {
        param_value_queue->addPoint(0, jitter.average, index);
    }
}

//------------------------------------------------------------------------
} // namespace

//------------------------------------------------------------------------
// Processor
//------------------------------------------------------------------------
Processor::Processor()
{
    //--- set the wanted controller for our processor
    setControllerClass(jitter_monitoring::kControllerUID);
}

//------------------------------------------------------------------------
Processor::~Processor() {}

//------------------------------------------------------------------------
tresult PLUGIN_API Processor::initialize(FUnknown* context)
{
    // Here the Plug-in will be instantiated

    //---always initialize the parent-------
    tresult result = AudioEffect::initialize(context);
    // if everything Ok, continue
    if (result != kResultOk)
    {
        return result;
    }

    //--- create Audio IO ------
    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

    /* If you don't need an event bus, you can remove the next line */
    addEventInput(STR16("Event In"), 1);

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Processor::terminate()
{
    // Here the Plug-in will be de-instantiated, last possibility to remove some
    // memory!

    //---do not forget to call parent ------
    return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API Processor::setActive(TBool state)
{
    //--- called when the Plug-in is enable/disable (On/Off) -----
    return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
constexpr Vst::ParamID jitter_param_id = 1;
tresult PLUGIN_API Processor::process(Vst::ProcessData& data)
{
    if (data.inputParameterChanges)
    {
        int32 numParamsChanged =
            data.inputParameterChanges->getParameterCount();
        for (int32 index = 0; index < numParamsChanged; index++)
        {
            Vst::IParamValueQueue* paramQueue =
                data.inputParameterChanges->getParameterData(index);
            if (paramQueue)
            {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount();
                switch (paramQueue->getParameterId())
                {
                    case JitterParamIds::kJitterReset: {
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset,
                                                 value) == kResultTrue)
                        {
                            if (value == 0.)
                            {
                                jitter = jittering::reset(jitter);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    if (data.processContext)
    {
        if (n == 0)
        {
            jitter.system_time_stamp = jittering::get_current_system_time();
        }
        else
        {
            jitter = update_jitter(data, jitter);
            update_parameters(data, jitter);
        }

        n++;
    }

    //--- Here you have to implement your processing

    if (data.numSamples > 0)
    {
        //--- ------------------------------------------
        // here as example a default implementation where we try to copy the
        // inputs to the outputs: if less input than outputs then clear outputs
        //--- ------------------------------------------

        int32 minBus = std::min(data.numInputs, data.numOutputs);
        for (int32 i = 0; i < minBus; i++)
        {
            int32 minChan = std::min(data.inputs[i].numChannels,
                                     data.outputs[i].numChannels);
            for (int32 c = 0; c < minChan; c++)
            {
                // do not need to be copied if the buffers are the same
                if (data.outputs[i].channelBuffers32[c] !=
                    data.inputs[i].channelBuffers32[c])
                {
                    memcpy(data.outputs[i].channelBuffers32[c],
                           data.inputs[i].channelBuffers32[c],
                           data.numSamples * sizeof(Vst::Sample32));
                }
            }
            data.outputs[i].silenceFlags = data.inputs[i].silenceFlags;

            // clear the remaining output buffers
            for (int32 c = minChan; c < data.outputs[i].numChannels; c++)
            {
                // clear output buffers
                memset(data.outputs[i].channelBuffers32[c], 0,
                       data.numSamples * sizeof(Vst::Sample32));

                // inform the host that this channel is silent
                data.outputs[i].silenceFlags |= ((uint64)1 << c);
            }
        }
        // clear the remaining output buffers
        for (int32 i = minBus; i < data.numOutputs; i++)
        {
            // clear output buffers
            for (int32 c = 0; c < data.outputs[i].numChannels; c++)
            {
                memset(data.outputs[i].channelBuffers32[c], 0,
                       data.numSamples * sizeof(Vst::Sample32));
            }
            // inform the host that this bus is silent
            data.outputs[i].silenceFlags =
                ((uint64)1 << data.outputs[i].numChannels) - 1;
        }
    }

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Processor::setupProcessing(Vst::ProcessSetup& newSetup)
{
    //--- called before any processing ----
    return AudioEffect::setupProcessing(newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API Processor::canProcessSampleSize(int32 symbolicSampleSize)
{
    // by default kSample32 is supported
    if (symbolicSampleSize == Vst::kSample32)
        return kResultTrue;

    // disable the following comment if your processing support kSample64
    /* if (symbolicSampleSize == Vst::kSample64)
        return kResultTrue; */

    return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Processor::setState(IBStream* state)
{
    // called when we load a preset, the model has to be reloaded
    IBStreamer streamer(state, kLittleEndian);

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Processor::getState(IBStream* state)
{
    // here we need to save the model
    IBStreamer streamer(state, kLittleEndian);

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Processor::setProcessing(TBool state)
{
    jitter = jittering::reset(jitter);
    n      = 0;

    return AudioEffect::setProcessing(state);
}

//------------------------------------------------------------------------
} // namespace Steinberg::jitter_monitoring