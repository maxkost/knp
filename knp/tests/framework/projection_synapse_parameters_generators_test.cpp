/**
 * @file projection_synapse_parameters_generators_test.cpp
 * @brief Tests for projection synapse parameters generators.
 * @date 27.08.2024
 * @license Apache 2.0
 * @author Artiom N.
 * @copyright © 2024 AO Kaspersky Lab
 */


#include <knp/core/projection.h>
#include <knp/framework/projection/synapse_parameters_generators.h>
#include <knp/synapse-traits/delta.h>

#include <tests_common.h>

#include <vector>


TEST(ProjectionSynapsesParametersGenerators, Default)
{
    const typename knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters def_syn;
    const auto new_syn =
        knp::framework::projection::parameters_generators::default_synapse_gen<knp::synapse_traits::DeltaSynapse>(0, 0);

    ASSERT_EQ(def_syn.weight_, new_syn.weight_);
    ASSERT_EQ(def_syn.delay_, new_syn.delay_);
    ASSERT_EQ(def_syn.output_type_, new_syn.output_type_);
}


TEST(ProjectionSynapsesParametersGenerators, Copy)
{
    const typename knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters def_syn;
    const typename knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters src_syn{
        123, 321, knp::synapse_traits::OutputType::INHIBITORY_CONDUCTANCE};

    const auto syn_gen =
        knp::framework::projection::parameters_generators::CopySynapseGen<knp::synapse_traits::DeltaSynapse>{src_syn};
    const auto new_syn = syn_gen(0, 0);

    ASSERT_NE(def_syn.weight_, new_syn.weight_);
    ASSERT_NE(def_syn.delay_, new_syn.delay_);
    ASSERT_NE(def_syn.output_type_, new_syn.output_type_);

    ASSERT_EQ(src_syn.weight_, new_syn.weight_);
    ASSERT_EQ(src_syn.delay_, new_syn.delay_);
    ASSERT_EQ(src_syn.output_type_, new_syn.output_type_);
}
