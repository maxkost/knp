/**
 * @file model_loader_test.cpp
 * @brief Model loader class testing.
 * @author Artiom N.
 * @date 23.10.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/framework/io/out_converters/convert_set.h>
#include <knp/framework/model_executor.h>
#include <knp/framework/network.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <generators.h>
#include <spdlog/spdlog.h>
#include <tests_common.h>

#include <filesystem>


TEST(FrameworkSuite, EmptyModelLoad)
{
    namespace kt = knp::testing;

    knp::framework::Model model(std::move(knp::framework::Network()));
    knp::framework::BackendLoader backend_loader;
    knp::framework::ModelLoader mld(backend_loader.load(knp::testing::get_backend_path()), {});
    EXPECT_NO_THROW(mld.load(model));
}


TEST(FrameworkSuite, ModelWithIncorrectInputChannel)
{
    namespace kt = knp::testing;

    auto input_gen = [](knp::core::Step step) -> knp::core::messaging::SpikeData { return {}; };

    const knp::core::UID i_channel_uid;

    knp::framework::Model model(std::move(knp::framework::Network()));

    kt::BLIFATPopulation population{kt::neuron_generator, 1};
    kt::DeltaProjection input_projection =
        kt::DeltaProjection{knp::core::UID(false), population.get_uid(), kt::synapse_generator, 1};

    const auto proj_uid = input_projection.get_uid();

    model.get_network().add_population(population);
    model.get_network().add_projection(input_projection);
    model.add_input_channel(i_channel_uid, proj_uid);

    knp::framework::BackendLoader backend_loader;
    // Incorrect channel UID.
    knp::framework::ModelLoader mld(
        backend_loader.load(knp::testing::get_backend_path()), {{knp::core::UID(), input_gen}});

    EXPECT_THROW(mld.load(model), std::logic_error);  //!OCLINT(False positive)
}
