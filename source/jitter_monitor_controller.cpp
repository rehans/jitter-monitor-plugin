// Copyright(c) 2024 rehans.

#include "jitter_monitor_controller.h"
#include "jitter_monitor_cids.h"
#include "jitter_monitor_param_ids.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;

namespace Steinberg::jitter_monitoring {

//------------------------------------------------------------------------
// Controller Implementation
//------------------------------------------------------------------------
constexpr Vst::ParamID jitter_param_id = 1;
tresult PLUGIN_API Controller::initialize(FUnknown* context)
{
    // Here the Plug-in will be instantiated

    //---do not forget to call parent ------
    tresult result = EditControllerEx1::initialize(context);
    if (result != kResultOk)
    {
        return result;
    }

    if (result == kResultTrue)
    {
        //---Create Parameters------------
        parameters.addParameter(STR16("JitterCurrent"), nullptr, 1, 0,
                                Vst::ParameterInfo::kCanAutomate,
                                JitterParamIds::kJitterCurrent);
        parameters.addParameter(STR16("JitterMin"), nullptr, 1, 0,
                                Vst::ParameterInfo::kCanAutomate,
                                JitterParamIds::kJitterMin);
        parameters.addParameter(STR16("JitterMax"), nullptr, 1, 0,
                                Vst::ParameterInfo::kCanAutomate,
                                JitterParamIds::kJitterMax);
        parameters.addParameter(STR16("JitterReset"), nullptr, 1, 0,
                                Vst::ParameterInfo::kCanAutomate,
                                JitterParamIds::kJitterReset);
        parameters.addParameter(STR16("JitterMeanValue"), nullptr, 1, 0,
                                Vst::ParameterInfo::kCanAutomate,
                                JitterParamIds::kJitterMeanValue);
    }

    return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Controller::terminate()
{
    // Here the Plug-in will be de-instantiated, last possibility to remove some
    // memory!

    //---do not forget to call parent ------
    return EditControllerEx1::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API Controller::setComponentState(IBStream* state)
{
    // Here you get the state of the component (Processor part)
    if (!state)
        return kResultFalse;

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Controller::setState(IBStream* state)
{
    // Here you get the state of the controller

    return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Controller::getState(IBStream* state)
{
    // Here you are asked to deliver the state of the controller (if needed)
    // Note: the real state of your plug-in is saved in the processor

    return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API Controller::createView(FIDString name)
{
    // Here the Host wants to open your editor (if you have one)
    if (FIDStringsEqual(name, Vst::ViewType::kEditor))
    {
        // create your editor here and return a IPlugView ptr of it
        auto* view = new VSTGUI::VST3Editor(this, "view",
                                            "jitter_monitor_editor.uidesc");
        return view;
    }
    return nullptr;
}

//------------------------------------------------------------------------
} // namespace Steinberg::jitter_monitoring
