//
// Created by an_vartenkov on 27.03.24.
//
#include "type_id_defines.h"

#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

namespace knp::framework
{
template <>
int get_synapse_type_id<synapse_traits::DeltaSynapse>()
{
    return 1000;
}

template <>
std::string get_synapse_type_name<synapse_traits::DeltaSynapse>()
{
    return "knp:BasicDeltaSynapse";
}
}  // namespace knp::framework
